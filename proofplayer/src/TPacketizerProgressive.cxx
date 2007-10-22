// @(#)root/proofplayer:$Id$
// Author: Zev Benjamin  13/09/2005

/*************************************************************************
 * Copyright (C) 1995-2005, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPacketizerProgressive                                               //
//                                                                      //
// This class is one of PROOF packetizers.                              //
// Packetizer generates packets to be processed on PROOF worker servers.//
// A packet is an event range (begin entry and number of entries) or    //
// object range (first object and number of objects) in a TTree         //
// (entries) or a directory (objects) in a file.                        //
// Packets are generated taking into account the performance of the     //
// remote machine, the time it took to process a previous packet on     //
// the remote machine, the locality of the database files, etc.         //
//                                                                      //
// The TPacketizerProgressive does not pre-open the files to calculate  //
// the total number of events. It just walks sequentially through the   //
// list of files.                                                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TMessage.h"
#include "TObjString.h"
#include "TParameter.h"
#include "TPerfStats.h"
#include "TPacketizerProgressive.h"
#include "TProofDebug.h"
#include "TProofServ.h"
#include "TSocket.h"
#include "TSystem.h"
#include "Riostream.h"
#include "TClass.h"

#include "TDSet.h"
#include "TError.h"
#include "TList.h"
#include "TMap.h"
#include "TMath.h"
#include "TNtupleD.h"
#include "TSlave.h"
#include "TTimer.h"
#include "TUrl.h"

//______________________________________________________________________________
TPacketizerProgressive::TFileNode::TFileNode(const char *name)
   : fNodeName(name), fFiles(new TList), fUnAllocFileNext(0),fActFiles(new TList),
     fActFileNext(0), fMySlaveCnt(0), fSlaveCnt(0)
{
   // Constructor

   fFiles->SetOwner();
   fActFiles->SetOwner(kFALSE);
}

//______________________________________________________________________________
TPacketizerProgressive::TFileNode::~TFileNode()
{
   // Destructor

   SafeDelete(fFiles);
   SafeDelete(fActFiles);
}

//______________________________________________________________________________
void TPacketizerProgressive::TFileNode::Add(TDSetElement *elem)
{
   // Add data set element

   TFileStat *f = new TFileStat(this,elem);
   fFiles->Add(f);
   if (fUnAllocFileNext == 0) fUnAllocFileNext = fFiles->First();
}

//______________________________________________________________________________
void TPacketizerProgressive::TFileNode::DecSlaveCnt(const char *wrk)
{
   // Decrement worker counter

   if (fNodeName != wrk)
      fSlaveCnt--;

   R__ASSERT(fSlaveCnt >= 0);
}

//______________________________________________________________________________
TPacketizerProgressive::TFileStat *TPacketizerProgressive::TFileNode::GetNextUnAlloc()
{
   // Get next unallocated file

   TObject *next = fUnAllocFileNext;

   if (next != 0) {
      // make file active
      fActFiles->Add(next);
      if (fActFileNext == 0) fActFileNext = fActFiles->First();

      // move cursor
      fUnAllocFileNext = fFiles->After(fUnAllocFileNext);
   }

   return (TFileStat *) next;
}

//______________________________________________________________________________
TPacketizerProgressive::TFileStat *TPacketizerProgressive::TFileNode::GetNextActive()
{
   // Get next active

   TObject *next = fActFileNext;

   if (fActFileNext != 0) {
      fActFileNext = fActFiles->After(fActFileNext);
      if (fActFileNext == 0) fActFileNext = fActFiles->First();
   }

   return (TFileStat *) next;
}

//______________________________________________________________________________
void TPacketizerProgressive::TFileNode::RemoveActive(TFileStat *file)
{
   // Remove active

   if (fActFileNext == file) fActFileNext = fActFiles->After(file);
   fActFiles->Remove(file);
   if (fActFileNext == 0) fActFileNext = fActFiles->First();
}

//______________________________________________________________________________
Int_t TPacketizerProgressive::TFileNode::Compare(const TObject *other) const
{
   // Must return -1 if this is smaller than obj, 0 if objects are equal
   // and 1 if this is larger than obj.

   const TFileNode *obj = dynamic_cast<const TFileNode*>(other);
   R__ASSERT(obj != 0);

   Int_t myVal = GetSlaveCnt();
   Int_t otherVal = obj->GetSlaveCnt();
   if (myVal < otherVal) {
      return -1;
   } else if (myVal > otherVal) {
      return 1;
   } else {
      return 0;
   }
}

//______________________________________________________________________________
void TPacketizerProgressive::TFileNode::Print(Option_t *) const
{
   // Print file node status

   cout << "OBJ: " << IsA()->GetName() << "\t" << fNodeName
        << "\tMySlaveCount " << fMySlaveCnt
        << "\tSlaveCount " << fSlaveCnt << endl;
}

//______________________________________________________________________________
void TPacketizerProgressive::TFileNode::Reset()
{
   // Reset file node

   fUnAllocFileNext = fFiles->First();
   fActFiles->Clear();
   fActFileNext = 0;
   fSlaveCnt = 0;
   fMySlaveCnt = 0;
}

//______________________________________________________________________________
Int_t TPacketizerProgressive::TFileNode::GetNumberOfActiveFiles() const
{
   // Number of active files

   return (fActFiles ? fActFiles->GetSize() : 0);
}

//______________________________________________________________________________
Bool_t TPacketizerProgressive::TFileNode::HasActiveFiles()
{
   // TRUE if at least one file is active

   if (fActFiles && fActFiles->GetSize() > 0)
      return kTRUE;
   return kFALSE;
}

//______________________________________________________________________________
TPacketizerProgressive::TFileStat::TFileStat(TFileNode *node, TDSetElement *elem)
   : fIsDone(kFALSE), fNode(node), fElement(elem), fNextEntry(elem->GetFirst())
{
   // Constructor
}


//______________________________________________________________________________
TPacketizerProgressive::TSlaveStat::TSlaveStat(TSlave *slave)
   : fSlave(slave), fFileNode(0), fCurFile(0), fCurElem(0), fProcessed(0)
{
   // Constructor
}

//______________________________________________________________________________
const char *TPacketizerProgressive::TSlaveStat::GetName() const
{
   // Worker name

   return (fSlave ? fSlave->GetName() : "");
}

ClassImp(TPacketizerProgressive)

//______________________________________________________________________________
TPacketizerProgressive::TPacketizerProgressive(TDSet* dset, TList* slaves,
                                               Long64_t first, Long64_t num,
                                               TList* input)
   : TVirtualPacketizer(input), fDset(dset), fSlaves(slaves), fSlavesRemaining(0),
     fFirstEvent(first), fTotalEvents(num), fEntriesSeen(0), fFilesOpened(0),
     fEstTotalEntries(0)
{
   // Constructor

   PDB(kPacketizer,1) Info("TPacketizerProgressive",
                           "enter (first %lld, num %lld)", first, num);
   if (fTotalEvents != -1) {
      // -1 events indicate to process the entire TDSet
      Error("TPacketizerProgressive",
            "this packetizer does not handle TDSet regions");
   }

   fSlavesRemaining = new TList;
   fSlaveStats = new TMap;
   fUnAllocSlaves = new TList;
   fUnAllocNonSlaves = new TList;
   fActiveSlaves = new TList;
   fActiveNonSlaves = new TList;
   fLastEntrySizes = new TList;
   fNewFileSlaves = new THashTable;

   if (fSlaves)
      fSlavesRemaining->AddAll(fSlaves);

   fValid = kTRUE;

   Init();
}

//______________________________________________________________________________
TPacketizerProgressive::~TPacketizerProgressive()
{
   // Destructor

   fSlaveStats->DeleteValues();

   delete fSlavesRemaining;
   delete fSlaveStats;
   delete fUnAllocSlaves;
   delete fUnAllocNonSlaves;
   delete fActiveSlaves;
   delete fActiveNonSlaves;
   delete fLastEntrySizes;
   delete fNewFileSlaves;
}

//______________________________________________________________________________
void TPacketizerProgressive::Init()
{
   // Build slave -> slave stats map, available slave host list,
   // and hash table of slave names.

   TMap host_map; // host_maps slave hostnames to their TFileNode objects
   TIter i(fSlaves);
   i.Reset();
   TSlave* slave;
   // go through the list of slaves and fill fSlaveStats with TSlaveStats
   // objects which also include appropriate TFileNode objects.
   // TFileNode objects are also stored in host_map.
   while ((slave = (TSlave*) i.Next())) {
      PDB(kPacketizer, 3)
         Info("Init", "adding info for slave %s", slave->GetName());
      TSlaveStat* ss = new TSlaveStat(slave);
      fSlaveStats->Add(slave, ss);

      // ensure that slaves on the same host have the same
      // TFileNode object
      TObjString host(slave->GetName());
      TFileNode* fn = (TFileNode*) host_map.GetValue(&host);
      if (! fn) {
         fn = new TFileNode(slave->GetName());
         host_map.Add(new TObjString(slave->GetName()), fn);
      }
      fn->IncMySlaveCnt();
      ss->SetFileNode(fn);
   }

   // Resolve end-point urls to optmize distribution
   // dset->Lookup(); // moved to TProofPlayerRemote::Process

   // put TDSetElements in the appropriate TFileStat object
   THashTable slaves_added; // map of slaves that have already
                            // been added to the available list
   TMap nonslaves_added; //map of added non-slave nodes that host needed files
   fDset->Reset();
   TDSetElement* e;
   while ((e = (TDSetElement*) fDset->Next())) {
      TUrl url = e->GetFileName();
      TObjString host = url.GetHost();
      PDB(kPacketizer, 3) Info("Init", "found TDSetElement on host %s",
                               host.GetString().Data());
      TFileNode* fn = (TFileNode*) host_map.GetValue(&host);
      if (! fn) {
         if (! (fn = (TFileNode*) nonslaves_added.GetValue(&host))) {
            PDB(kPacketizer, 3) Info("Init", "adding info for non-slave %s",
                                     host.GetString().Data());
            // the element is on a non-slave host: make a new
            // TFileNode and add it to the list of unallocated non-slaves
            fn = new TFileNode(host.GetString().Data());
            fUnAllocNonSlaves->Add(fn);
            nonslaves_added.Add(&host, fn);
         }
      } else {
         if (! slaves_added.FindObject(fn)) {
            fUnAllocSlaves->Add(fn);
            slaves_added.Add(fn);
         }
      }
      fn->Add(e);
   }

   host_map.DeleteKeys();

   PDB(kPacketizer, 2) Info("Init", "finished init");
}

//______________________________________________________________________________
Long64_t TPacketizerProgressive::GetEntriesProcessed(TSlave* s) const
{
   // Get entries to be processed

   TSlaveStat* stat = (TSlaveStat*) fSlaveStats->GetValue(s);
   return stat->GetEntriesProcessed();
}

//______________________________________________________________________________
TDSetElement *TPacketizerProgressive::BuildPacket(TSlaveStat* stat,
                                                  Long64_t size)
{
   // Build a packet
   // size - target size of the packet
   // BUT the packet size may rise to (size * 2 - 1) !!

   TFileStat* fs = stat->GetCurrentFile();
   if (! fs) {
      Error("BuildPacket", "no TFileStat assigned");
      return 0;
   }

   Long64_t elem_entries = stat->GetCurrentElement()->GetNum();

   Long64_t num = size;
   Long64_t entries_remaining = elem_entries - fs->GetNextEntry();

   // process everything that's left if there are fewer than num entries
   // or if there are fewer than num*2
   if (elem_entries != -1 && (num > entries_remaining ||
                              (entries_remaining < (num * 2)))) {
      num = entries_remaining;
   }

   PDB(kPacketizer,3)
      Info("BuildPacket",
           "packet of size %lld requested (assigning %lld)."
           "  pos: %lld, num entries: %lld",
            size, num, fs->GetNextEntry(), elem_entries);

   TDSetElement* base = stat->GetCurrentElement();
   TDSetElement* packet = CreateNewPacket(base, fs->GetNextEntry(), num);
   fs->MoveNextEntry(num);
   stat->IncEntriesProcessed(num);
   fProcessed += num;
   // cleanup if this file is done
   if ( fs->GetNextEntry() == elem_entries) {
      fs->SetDone();
      fs->GetNode()->RemoveActive(fs);

      if (! stat->GetFileNode()->HasActiveFiles()) {
         fActiveSlaves->Remove(stat->GetFileNode());
         fActiveNonSlaves->Remove(stat->GetFileNode());
      }
   }

   return packet;
}

//______________________________________________________________________________
void TPacketizerProgressive::RecalculatePacketSize(Long64_t newCount)
{
   // Shrink fLastEntrySizes to (kEntryListSize - 1).

   if (fLastEntrySizes->GetSize() >= kEntryListSize) {
      while (fLastEntrySizes->GetSize() >= (kEntryListSize - 1)) {
         TParameter<Long64_t>* first = (TParameter<Long64_t>*) fLastEntrySizes->First();
         fLastEntrySizes->Remove(first);
         delete first;
      }
   }

   // add the new count on to the end
   fLastEntrySizes->AddLast(new TParameter<Long64_t>("", newCount));

   // calculate the mean
   Long64_t total = 0, mean = 0, elems_remaining;
   elems_remaining = fDset->GetListOfElements()->GetSize() - fFilesOpened;
   PDB(kPacketizer, 4)
      Info("RecalculatePacketSize",
           "files opened: %lld, fdset size: %d, elems remaining: %lld",
           fFilesOpened, fDset->GetListOfElements()->GetSize(), elems_remaining);
   TParameter<Long64_t>* count;
   TIter i(fLastEntrySizes);
   while ((count = (TParameter<Long64_t>*) i.Next())) {
      total += count->GetVal();
   }
   mean = total / fLastEntrySizes->GetSize();
   fEstTotalEntries = fEntriesSeen + mean * elems_remaining;
   Int_t num_slaves = fSlavesRemaining->GetSize();

   fPacketSize = fEstTotalEntries / (num_slaves * 20);
   if (fPacketSize == 0) {
      fPacketSize = 1;
   }

   PDB(kPacketizer, 3)
      Info("RecalculatePacketSize",
           "estimated number of entries: %lld, new packet size: %lld",
           fEstTotalEntries, fPacketSize);
}

//______________________________________________________________________________
TPacketizerProgressive::TFileStat *TPacketizerProgressive::GetNextActive(TSlaveStat* stat)
{
   // Get next active file

   fActiveSlaves->Sort();
   fActiveNonSlaves->Sort();
   TFileStat* file = 0;

   // check if the current host has any more TFileNodes
   if (stat->GetFileNode() && stat->GetFileNode()->HasActiveFiles()) {
      PDB(kPacketizer, 3)
         Info("GetNextActive",
              "Assigning slave %s TDSetElement from current data source",
              stat->GetName());

      file = stat->GetFileNode()->GetNextActive();
      return file;
   }

   // check if any non slaves have active TFileNodes
   if (fActiveNonSlaves->GetSize() &&
       ((TFileNode*) fActiveNonSlaves->First())->GetSlaveCnt() < kNonSlaveHostConnLim) {
      PDB(kPacketizer, 3)
         Info("GetNextActive",
              "Assigning slave %s TDSetElement from non-slave data source",
              stat->GetName());

      file = ((TFileNode*) fActiveNonSlaves->First())->GetNextActive();
      return file;
   }

   // check if any slaves have active TFileNodes
   if (fActiveSlaves->GetSize() &&
       ((TFileNode*) fActiveSlaves->First())->GetSlaveCnt() < kSlaveHostConnLim) {
      PDB(kPacketizer, 3)
         Info("GetNextActive",
              "Assigning slave %s TDSetElement from peer data source",
              stat->GetName());

      file = ((TFileNode*) fActiveSlaves->First())->GetNextActive();
      return file;
   }

   return 0;
}

//______________________________________________________________________________
TPacketizerProgressive::TFileStat *TPacketizerProgressive::GetNextUnAlloc(TSlaveStat* stat)
{
   // Get next unallocated node

   fUnAllocSlaves->Sort();
   fUnAllocNonSlaves->Sort();
   TFileStat* file = 0;
   TFileNode* fn = 0;

   // check if the current host has any more TFileNodes
   if (stat->GetFileNode() && stat->GetFileNode()->HasUnAllocFiles()) {
      PDB(kPacketizer, 3)
         Info("GetNextUnAlloc",
              "Assigning slave %s TDSetElement from current data source",
              stat->GetName());

      fn = stat->GetFileNode();
      file = fn->GetNextUnAlloc();

      // add this slave to the list of actives if it isn't already there
      if (fUnAllocNonSlaves->FindObject(fn) &&
          ! fActiveNonSlaves->FindObject(fn)) {
         fActiveNonSlaves->Add(fn);
      }
      if (fUnAllocSlaves->FindObject(fn) &&
          ! fActiveSlaves->FindObject(fn)) {
         fActiveSlaves->Add(fn);
      }

      // remove this slave from the list of unalloc'd if it doesn't have any
      // more active files
      if (!fn->HasUnAllocFiles()) {
         if (fUnAllocNonSlaves->FindObject(fn)) {
            fUnAllocNonSlaves->Remove(fn);
         }
         if (fUnAllocSlaves->FindObject(fn)) {
            fUnAllocSlaves->Remove(fn);
         }
      }
      return file;
   }

   // check if any non slaves have unallocated TFileNodes
   if (fUnAllocNonSlaves->GetSize() &&
       ((TFileNode*)fUnAllocNonSlaves->First())->GetSlaveCnt() < kNonSlaveHostConnLim) {
      PDB(kPacketizer, 3)
         Info("GetNextUnAlloc",
              "Assigning slave %s TDSetElement from non-slave data source",
              stat->GetName());

      fn = (TFileNode*) fUnAllocNonSlaves->First();
      file = fn->GetNextUnAlloc();
      if (! fn->HasUnAllocFiles()) {
         fUnAllocNonSlaves->Remove(fn);
      }
      if (! fActiveNonSlaves->FindObject(fn)) {
         fActiveNonSlaves->Add(fn);
      }
      return file;
   }

   // check if any slaves have unallocated TFileNodes
   if (fUnAllocSlaves->GetSize() &&
       ((TFileNode*) fUnAllocSlaves->First())->GetSlaveCnt() < kSlaveHostConnLim) {
      PDB(kPacketizer, 3)
         Info("GetNextUnAlloc",
              "Assigning slave %s TDSetElement from peer data source",
              stat->GetName());

      fn = (TFileNode*) fUnAllocSlaves->First();
      file = fn->GetNextUnAlloc();
      if (! fn->HasUnAllocFiles()) {
         fUnAllocSlaves->Remove(fn);
      }
      if (! fActiveNonSlaves->FindObject(fn)) {
         fActiveSlaves->Add(fn);
      }
      return file;
   }

   return 0;
}

//______________________________________________________________________________
TDSetElement *TPacketizerProgressive::GetNextPacket(TSlave *s, TMessage *r)
{
   // Get next packet

   PDB(kPacketizer, 3) Info("GetNextPacket", "enter with slave %s", s->GetName());

   TSlaveStat* stat = (TSlaveStat*) fSlaveStats->GetValue(s);

   PDB(kPacketizer, 4)
      Info("GetNextPacket", "current file node (%s) has %d connections",
           stat->GetFileNode()->GetName(), stat->GetFileNode()->GetSlaveCnt());
   PDB(kPacketizer, 4)
      Info("GetNextPacket", "unalloc'd slaves: %d, unalloc'd non-slaves: %d,"
           " active slaves: %d, active non-slaves: %d",
           fUnAllocSlaves->GetSize(), fUnAllocNonSlaves->GetSize(),
           fActiveSlaves->GetSize(), fActiveNonSlaves->GetSize());

   Double_t latency, proctime, proccpu;
   Long64_t bytesread, numentries;
   // send back performance statistics
   if (stat->GetCurrentElement() && ! stat->GetCurrentFile()->IsDone()) {
      (*r) >> latency >> proctime >> proccpu >> bytesread >> numentries;
      if (gPerfStats != 0) {
         gPerfStats->PacketEvent(s->GetOrdinal(), s->GetName(),
                                 stat->GetCurrentElement()->GetFileName(),
                                 numentries, latency, proctime, proccpu, bytesread);
      }
   }

   // cleanup and send perf stats info if the slave just finished a file
   if (stat->GetCurrentFile() && stat->GetCurrentFile()->IsDone()) {
      if (gPerfStats != 0) {
         TFileStat* file = stat->GetCurrentFile();
         gPerfStats->FileEvent(s->GetOrdinal(), s->GetName(),
                               file->GetNode()->GetName(),
                               file->GetElement()->GetFileName(), kFALSE);
      }
      stat->SetCurrentElement(0);
      stat->SetCurrentFile(0);
   }

   // this is the second call to GetNextPacket: record number of entries
   if (fNewFileSlaves->FindObject(stat)) {
      RecalculatePacketSize(numentries);
      PDB(kPacketizer, 3) Info("GetNextPacket",
         "Newly opened file has %lld entries; updated packet size to %lld",
         numentries, fPacketSize);
      fEntriesSeen += numentries;
      fFilesOpened++;
      stat->GetCurrentElement()->SetNum(numentries);
      fNewFileSlaves->Remove(stat);
   }

   // check if this slave is already working on a TDSetElement
   if (stat->GetCurrentFile() && ! stat->GetCurrentFile()->IsDone()) {
      if (stat->GetCurrentElement()->GetNum() == -1) {
         // we grabbed an element that hasn't been fully opened yet
         PDB(kPacketizer, 3) Info("GetNextPacket",
            "working on a packet that isn't fully opened, waiting");
         return (TDSetElement*) -1;
      }
      return BuildPacket(stat, fPacketSize);
   }

   // the slave has finished its TDSetElement

   Int_t foundUnallocatedFile = -1; // important for the packet size
   stat->GetFileNode()->DecSlaveCnt(s->GetName());
   TFileStat* fs = 0;
   // try to find an unallocated file first
   if ((fs = GetNextUnAlloc(stat))) {
      PDB(kPacketizer, 3)
         Info("AssignElement", "giving slave %s unallocated file", stat->GetName());
      stat->SetFileNode(fs->GetNode());
      stat->GetFileNode()->IncSlaveCnt(s->GetName());
      stat->SetCurrentFile(fs);
      stat->SetCurrentElement(fs->GetElement());
      fNewFileSlaves->Add(stat);

      foundUnallocatedFile = 1;

   // next try to get an active file
   } else if ((fs = GetNextActive(stat))) {
      PDB(kPacketizer, 3) Info("AssignElement", "giving slave %s active file",
                               stat->GetName());
      stat->SetFileNode(fs->GetNode());
      stat->GetFileNode()->IncSlaveCnt(s->GetName());
      stat->SetCurrentFile(fs);
      stat->SetCurrentElement(fs->GetElement());

      if (stat->GetCurrentElement()->GetNum() == -1) {
         // we grabbed an element that hasn't been fully opened yet
         PDB(kPacketizer, 3)
            Info("AssignElement",
                 "grabbed a packet that isn't fully opened, waiting");
         return (TDSetElement*) -1;
      }
      foundUnallocatedFile = 0;
   }

   if (foundUnallocatedFile == -1) { // -1 means no more packets
      // no more packets
      PDB(kPacketizer, 3) Info("GetNextPacket", "no more packets available");
      fSlavesRemaining->Remove(s);
      return 0;
   } else {
      if (gPerfStats != 0) {
         TFileStat* file = stat->GetCurrentFile();
         gPerfStats->FileEvent(s->GetOrdinal(), s->GetName(),
                               file->GetNode()->GetName(),
                               file->GetElement()->GetFileName(), kTRUE);
      }
      if (foundUnallocatedFile == 1)
         return BuildPacket(stat, 1);
      else
         return BuildPacket(stat, fPacketSize);
   }
}

//______________________________________________________________________________
Bool_t TPacketizerProgressive::HandleTimer(TTimer *)
{
   // Send progress message to client.

   PDB(kPacketizer, 4)
      Info("HandlerTimer", "estimated total entries: %lld, entries processed: %lld",
                            fEstTotalEntries, fProcessed);

   if (fProgress == 0) return kFALSE; // timer stopped already

   // wait until we get an estimate
   if (fEstTotalEntries <= 0) return kFALSE;

   TMessage m(kPROOF_PROGRESS);

   if (gProofServ->GetProtocol() > 11) {

      // Prepare progress info
      TTime tnow = gSystem->Now();
      Float_t now = (Float_t) (Long_t(tnow) - fStartTime) / (Double_t)1000.;
      Double_t evts = (Double_t) fProcessed;
      Double_t mbs = (fBytesRead > 0) ? fBytesRead / TMath::Power(2.,20.) : 0.; //�--> MB

      // Times and counters
      Float_t evtrti = -1., mbrti = -1.;
      if (TestBit(TVirtualPacketizer::kIsInitializing)) {
         // Initialization
         fInitTime = now;
      } else {
         // Fill the reference as first
         if (fCircProg->GetEntries() <= 0) {
            fCircProg->Fill((Double_t)0., 0., 0.);
            // Best estimation of the init time
            fInitTime = (now + fInitTime) / 2.;
         }
         // Time between updates
         fTimeUpdt = now - fProcTime;
         // Update proc time
         fProcTime = now - fInitTime;
         // Good entry
         fCircProg->Fill((Double_t)fProcTime, evts, mbs);
         // Instantaneous rates (at least 5 reports)
         if (fCircProg->GetEntries() > 4) {
            Double_t *ar = fCircProg->GetArgs();
            fCircProg->GetEntry(0);
            Double_t dt = (Double_t)fProcTime - ar[0];
            evtrti = (dt > 0) ? (Float_t) (evts - ar[1]) / dt : -1. ;
            mbrti = (dt > 0) ? (Float_t) (mbs - ar[2]) / dt : -1. ;
            if (gPerfStats != 0)
               gPerfStats->RateEvent((Double_t)fProcTime, dt,
                                     (Long64_t) (evts - ar[1]),
                                     (Long64_t) ((mbs - ar[2])*TMath::Power(2.,20.)));
         }
      }

      // Fill the message now
      m << fEstTotalEntries << fProcessed << fBytesRead << fInitTime << fProcTime
        << evtrti << mbrti;

   } else {
      // Old format
      m << fEstTotalEntries << fProcessed;
   }

   // send message to client;
   gProofServ->GetSocket()->Send(m);

   return kFALSE; // ignored?
}
