//*CMZ :  2.23/12 30/01/2000  09.01.36  by  Rene Brun
//*CMZ :  2.23/11 04/01/2000  16.49.39  by  Rene Brun
//*CMZ :  2.23/08 31/10/99  11.42.28  by  Rene Brun
//*CMZ :  2.23/03 16/09/99  11.27.55  by  Rene Brun
//*CMZ :  2.22/01 26/04/99  12.07.23  by  Rene Brun
//*CMZ :  2.21/08 15/03/99  11.39.55  by  Rene Brun
//*CMZ :  2.21/06 15/02/99  09.08.07  by  Rene Brun
//*-- Author :    Rene Brun   19/01/97

////////////////////////////////////////////////////////////////////////
//
//             A simple example with a ROOT tree
//             =================================
//
//  This program creates :
//    - a ROOT file
//    - a tree
//  Additional arguments can be passed to the program to control the flow
//  of execution. (see comments describing the arguments in the code).
//      Event  nevent comp split fill
//  All arguments are optional: Default is
//      Event  400      1    1     1
//
//  In this example, the tree consists of one single "super branch"
//  The statement ***tree->Branch("event", event, 64000,split);*** below
//  will parse the structure described in Event.h and will make
//  a new branch for each data member of the class if split is set to 1.
//    - 5 branches corresponding to the basic types fNtrack,fNseg,fNvertex
//           ,fFlag and fTemperature.
//    - 3 branches corresponding to the members of the subobject EventHeader.
//    - one branch for each data member of the class Track of TClonesArray.
//    - one branch for the object fH (histogram of class TH1F).
//
//  if split = 0 only one single branch is created and the complete event
//  is serialized in one single buffer.
//  if comp = 0 no compression at all.
//  if comp = 1 event is compressed.
//  if comp = 2 same as 1. In addition branches with floats in the TClonesArray
//                         are also compressed.
//  The 4th argument fill can be set to 0 if one wants to time
//     the percentage of time spent in creating the event structure and
//     not write the event in the file.
//  In this example, one loops over nevent events.
//  The branch "event" is created at the first event.
//  The branch address is set for all other events.
//  For each event, the event header is filled and ntrack tracks
//  are generated and added to the TClonesArray list.
//  For each event the event histogram is saved as well as the list
//  of all tracks.
//
//  The number of events can be given as the first argument to the program.
//  By default 400 events are generated.
//  The compression option can be activated/deactivated via the second argument.
//
//   ---Running/Linking instructions----
//  This program consists of the following files and procedures.
//    - Event.h event class description
//    - Event.C event class implementation
//    - MainEvent.C the main program to demo this class might be used (this file)
//    - EventCint.C  the CINT dictionary for the event and Track classes
//        this file is automatically generated by rootcint (see Makefile),
//        when the class definition in Event.h is modified.
//
//   ---Analyzing the Event.root file with the interactive root
//        example of a simple session
//   Root > TFile f("Event.root")
//   Root > T.Draw("fNtrack")   //histogram the number of tracks per event
//   Root > T.Draw("fPx")       //histogram fPx for all tracks in all events
//   Root > T.Draw("fXfirst:fYfirst","fNtrack>600")
//                              //scatter-plot for x versus y of first point of each track
//   Root > T.Draw("fH.GetRMS()")  //histogram of the RMS of the event histogram
//
//   Look also in the same directory at the following macros:
//     - eventa.C  an example how to read the tree
//     - eventb.C  how to read events conditionally
//
////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "TROOT.h"
#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"
#include "TStopwatch.h"

#include "Event.h"


//______________________________________________________________________________
int main(int argc, char **argv)
{
   TROOT simple("simple","Example of creation of a tree");

   Int_t nevent = 400;     // by default create 400 events
   Int_t comp   = 1;       // by default file is compressed
   Int_t split  = 1;       // by default, split Event in sub branches
   Int_t write  = 1;       // by default the tree is filled
   Int_t hfill  = 0;       // by default histograms are not filled
   Int_t read   = 0;
   Int_t arg4   = 1;
   Int_t arg5   = 600;     //default number of tracks per event

   if (argc > 1)  nevent = atoi(argv[1]);
   if (argc > 2)  comp   = atoi(argv[2]);
   if (argc > 3)  split  = atoi(argv[3]);
   if (argc > 4)  arg4   = atoi(argv[4]);
   if (argc > 5)  arg5   = atoi(argv[5]);
   if (arg4 ==  0) { write = 0; hfill = 0; read = 1;}
   if (arg4 ==  1) { write = 1; hfill = 0;}
   if (arg4 ==  2) { write = 0; hfill = 0;}
   if (arg4 == 10) { write = 0; hfill = 1;}
   if (arg4 == 11) { write = 1; hfill = 1;}
   if (arg4 == 20) { write = 0; read  = 1;}  //read sequential
   if (arg4 == 25) { write = 0; read  = 2;}  //read random


   TFile *hfile;
   TTree *tree;
   Event *event = 0;

   // Fill event, header and tracks with some random numbers
   //   Create a timer object to benchmark this loop
   TStopwatch timer;
   timer.Start();
   Int_t nb = 0;
   Int_t ev;
   Int_t bufsize;
   Double_t told = 0;
   Double_t tnew = 0;
   Int_t printev = 100;
   if (arg5 < 100) printev = 1000;
   if (arg5 < 10)  printev = 10000;

//         Read case
   if (read) {
      hfile = new TFile("Event.root");
      tree = (TTree*)hfile->Get("T");
      TBranch *branch = tree->GetBranch("event");
      branch->SetAddress(&event);
      Int_t nentries = (Int_t)tree->GetEntries();
      nevent = TMath::Max(nevent,nentries);
      if (read == 1) {  //read sequential
         for (ev = 0; ev < nevent; ev++) {
            if (ev%printev == 0) {
               tnew = timer.RealTime();
               printf("event:%d, rtime=%f s\n",ev,tnew-told);
               told=tnew;
               timer.Continue();
            }
            nb += tree->GetEntry(ev);        //read complete event in memory
         }
      } else {    //read random
         Int_t evrandom;
         for (ev = 0; ev < nevent; ev++) {
            if (ev%printev == 0) cout<<"event="<<ev<<endl;
            evrandom = Int_t(nevent*gRandom->Rndm(1));
            nb += tree->GetEntry(evrandom);  //read complete event in memory
         }
      }
   } else {
//         Write case
      // Create a new ROOT binary machine independent file.
      // Note that this file may contain any kind of ROOT objects, histograms,
      // pictures, graphics objects, detector geometries, tracks, events, etc..
      // This file is now becoming the current directory.
      hfile = new TFile("Event.root","RECREATE","TTree benchmark ROOT file");
      hfile->SetCompressionLevel(comp);

     // Create histogram to show write_time in function of time
     Float_t curtime = -0.5;
     Int_t ntime = nevent/printev;
     TH1F *htime = new TH1F("htime","Real-Time to write versus time",ntime,0,ntime);
     HistogramManager *hm = 0;
     if (hfill) {
        TDirectory *hdir = new TDirectory("histograms", "all histograms");
        hm = new HistogramManager(hdir);
     }

     // Create a ROOT Tree and one superbranch
      TTree *tree = new TTree("T","An example of a ROOT tree");
      tree->SetAutoSave(1000000000);  // autosave when 1 Gbyte written
      bufsize = 256000;
      if (split)  bufsize /= 4;
      event = new Event();
      TBranch *branch = tree->Branch("event", "Event", &event, bufsize,split);
      branch->SetAutoDelete(kFALSE);
      char etype[20];
      
      for (ev = 0; ev < nevent; ev++) {
         if (ev%printev == 0) {
            tnew = timer.RealTime();
            printf("event:%d, rtime=%f s\n",ev,tnew-told);
            htime->Fill(curtime,tnew-told);
            curtime += 1;
            told=tnew;
            timer.Continue();
         }

         Float_t sigmat, sigmas;
         gRandom->Rannor(sigmat,sigmas);
         Int_t ntrack   = Int_t(arg5 +arg5*sigmat/120.);
         Float_t random = gRandom->Rndm(1);

         sprintf(etype,"type%d",ev%5);
         event->SetType(etype);
         event->SetHeader(ev, 200, 960312, random);
         event->SetNseg(Int_t(10*ntrack+20*sigmas));
         event->SetNvertex(1);
         event->SetFlag(UInt_t(random+0.5));
         event->SetTemperature(random+20.);

         for(UChar_t m = 0; m < 10; m++) {
            event->SetMeasure( m, gRandom->Gaus(m,m+1) ); 
         }
         for(UChar_t i0 = 0; i0 < 4; i0++) {
            for(UChar_t i1 = 0; i1 < 4; i1++) {
               event->SetMatrix(i0,i1,gRandom->Gaus(i0*i1,1));
            }
         }

         //  Create and Fill the Track objects
         for (Int_t t = 0; t < ntrack; t++) event->AddTrack(random);

         if (write) nb += tree->Fill();  //fill the tree

         if (hm) hm->Hfill(event);      //fill histograms

         event->Clear();
      }
      if (write) {
         hfile->Write();
         tree->Print();
      }
   }

   //  Stop timer and print results
   timer.Stop();
   Float_t mbytes = 0.000001*nb;
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();


   printf("\n%d events and %d bytes processed.\n",nevent,nb);
   printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
   if (read) {
      printf("You read %f Mbytes/Realtime seconds\n",mbytes/rtime);
      printf("You read %f Mbytes/Cputime seconds\n",mbytes/ctime);
   } else {
      printf("compression level=%d, split=%d, arg4=%d\n",comp,split,arg4);
      printf("You write %f Mbytes/Realtime seconds\n",mbytes/rtime);
      printf("You write %f Mbytes/Cputime seconds\n",mbytes/ctime);
      //printf("file compression factor = %f\n",hfile.GetCompressionFactor());
   }
   hfile->Close();
   return 0;
}
