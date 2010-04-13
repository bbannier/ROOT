// @(#)root/geom:$Id$
// Author: Andrei Gheata   01/11/01
// CheckGeometry(), CheckOverlaps() by Mihaela Gheata

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//______________________________________________________________________________
//   TGeoChecker - Geometry checking package.
//===============
//
// TGeoChecker class provides several geometry checking methods. There are two
// types of tests that can be performed. One is based on random sampling or
// ray-tracing and provides a visual check on how navigation methods work for
// a given geometry. The second actually checks the validity of the geometry
// definition in terms of overlapping/extruding objects. Both types of checks
// can be done for a given branch (starting with a given volume) as well as for 
// the geometry as a whole.
//
// 1. TGeoChecker::CheckPoint(Double_t x, Double_t y, Double_t z)
//
// This method can be called direcly from the TGeoManager class and print a
// report on how a given point is classified by the modeller: which is the
// full path to the deepest node containing it, and the (under)estimation 
// of the distance to the closest boundary (safety).
//
// 2. TGeoChecker::RandomPoints(Int_t npoints)
//
// Can be called from TGeoVolume class. It first draws the volume and its
// content with the current visualization settings. Then randomly samples points
// in its bounding box, plotting in the geometry display only the points 
// classified as belonging to visible volumes. 
//
// 3. TGeoChecker::RandomRays(Int_t nrays, Double_t startx, starty, startz)
//
// Can be called and acts in the same way as the previous, but instead of points,
// rays having random isotropic directions are generated from the given point.
// A raytracing algorithm propagates all rays untill they exit geometry, plotting
// all segments crossing visible nodes in the same color as these.
//
// 4. TGeoChecker::Test(Int_t npoints)
//
// Implementation of TGeoManager::Test(). Computes the time for the modeller
// to find out "Where am I?" for a given number of random points.
//
// 5. TGeoChecker::LegoPlot(ntheta, themin, themax, nphi, phimin, phimax,...)
// 
// Implementation of TGeoVolume::LegoPlot(). Draws a spherical radiation length 
// lego plot for a given volume, in a given theta/phi range.
//
// 6. TGeoChecker::Weigth(Double_t precision)
//
// Implementation of TGeoVolume::Weigth(). Estimates the total weigth of a given 
// volume by matrial sampling. Accepts as input the desired precision.
//
// Overlap checker
//-----------------
//
//  -> add it from the doc.
/*
<img src="gif/t_checker.jpg">
*/
//End_Html

#include "TVirtualPad.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TH2.h"
#include "TRandom3.h"
#include "TPolyMarker3D.h"
#include "TPolyLine3D.h"
#include "TStopwatch.h"

#include "TGeoVoxelFinder.h"
#include "TGeoBBox.h"
#include "TGeoPcon.h"
#include "TGeoManager.h"
#include "TGeoOverlap.h"
#include "TGeoPainter.h"
#include "TGeoChecker.h"

#include "TBuffer3D.h"
#include "TBuffer3DTypes.h"
#include "TMath.h"

#include <stdlib.h>

// statics and globals

ClassImp(TGeoChecker)

//______________________________________________________________________________
TGeoChecker::TGeoChecker()
            :TObject(),
             fGeoManager(NULL),
             fVsafe(NULL),
             fBuff1(NULL),
             fBuff2(NULL),
             fFullCheck(kFALSE),
             fVal1(NULL),
             fVal2(NULL),
             fFlags(NULL),
             fTimer(NULL),
             fSelectedNode(NULL),
             fNchecks(0),
             fNmeshPoints(1000)
{
// Default constructor
}

//______________________________________________________________________________
TGeoChecker::TGeoChecker(TGeoManager *geom)
            :TObject(),
             fGeoManager(geom),
             fVsafe(NULL),
             fBuff1(NULL),
             fBuff2(NULL),
             fFullCheck(kFALSE),
             fVal1(NULL),
             fVal2(NULL),
             fFlags(NULL),
             fTimer(NULL),
             fSelectedNode(NULL),
             fNchecks(0),
             fNmeshPoints(1000)
{
// Constructor for a given geometry
   fBuff1 = new TBuffer3D(TBuffer3DTypes::kGeneric,500,3*500,0,0,0,0);
   fBuff2 = new TBuffer3D(TBuffer3DTypes::kGeneric,500,3*500,0,0,0,0);   
}

//______________________________________________________________________________
TGeoChecker::~TGeoChecker()
{
// Destructor
   if (fBuff1) delete fBuff1;
   if (fBuff2) delete fBuff2;
   if (fTimer) delete fTimer;
}

//______________________________________________________________________________
void TGeoChecker::OpProgress(const char *opname, Long64_t current, Long64_t size, TStopwatch *watch, Bool_t last, Bool_t refresh)
{
// Print current operation progress.
   static Long64_t icount = 0;
   static TString oname;
   static TString nname;
   static Long64_t ocurrent = 0;
   static Long64_t osize = 0;
   static Int_t oseconds = 0;
   static TStopwatch *owatch = 0;
   static Bool_t oneoftwo = kFALSE;
   static Int_t nrefresh = 0;
   const char symbol[4] = {'=','\\','|','/'}; 
   char progress[11] = "          ";
   Int_t ichar = icount%4;
   
   if (!refresh) {
      nrefresh = 0;
      if (!size) return;
      owatch = watch;
      oname = opname;
      ocurrent = TMath::Abs(current);
      osize = TMath::Abs(size);
      if (ocurrent > osize) ocurrent=osize;
   } else {
      nrefresh++;
      if (!osize) return;
   }     
   icount++;
   Double_t time = 0.;
   Int_t hours = 0;
   Int_t minutes = 0;
   Int_t seconds = 0;
   if (owatch && !last) {
      owatch->Stop();
      time = owatch->RealTime();
      hours = (Int_t)(time/3600.);
      time -= 3600*hours;
      minutes = (Int_t)(time/60.);
      time -= 60*minutes;
      seconds = (Int_t)time;
      if (refresh)  {
         if (oseconds==seconds) {
            owatch->Continue();
            return;
         }
         oneoftwo = !oneoftwo;   
      }
      oseconds = seconds;   
   }
   if (refresh && oneoftwo) {
      nname = oname;
      if (fNchecks <= 0) fNchecks = nrefresh+1;
      Int_t pctdone = (Int_t)(100.*nrefresh/fNchecks);
      oname = Form("     == %d%% ==", pctdone);
   }         
   Double_t percent = 100.0*ocurrent/osize;
   Int_t nchar = Int_t(percent/10);
   if (nchar>10) nchar=10;
   Int_t i;
   for (i=0; i<nchar; i++)  progress[i] = '=';
   progress[nchar] = symbol[ichar];
   for (i=nchar+1; i<10; i++) progress[i] = ' ';
   progress[10] = '\0';
   oname += "                    ";
   oname.Remove(20);
   if(size<10000) fprintf(stderr, "%s [%10s] %4lld ", oname.Data(), progress, ocurrent);
   else if(size<100000) fprintf(stderr, "%s [%10s] %5lld ",oname.Data(), progress, ocurrent);
   else fprintf(stderr, "%s [%10s] %7lld ",oname.Data(), progress, ocurrent);
   if (time>0.) fprintf(stderr, "[%6.2f %%]   TIME %.2d:%.2d:%.2d             \r", percent, hours, minutes, seconds);
   else fprintf(stderr, "[%6.2f %%]\r", percent);
   if (refresh && oneoftwo) oname = nname;
   if (owatch) owatch->Continue();
   if (last) {
      icount = 0;
      owatch = 0;
      ocurrent = 0;
      osize = 0;
      oseconds = 0;
      oneoftwo = kFALSE;
      nrefresh = 0;
      fprintf(stderr, "\n");
   }   
}   

//_____________________________________________________________________________
void TGeoChecker::CheckBoundaryErrors(Int_t ntracks, Double_t radius)
{
// Check pushes and pulls needed to cross the next boundary with respect to the
// position given by FindNextBoundary. If radius is not mentioned the full bounding
// box will be sampled.
   gRandom = new TRandom3();
   TGeoVolume *tvol = fGeoManager->GetTopVolume();
   Info("CheckBoundaryErrors", "Top volume is %s",tvol->GetName());  
   const TGeoShape *shape = tvol->GetShape();
   TGeoBBox *box = (TGeoBBox *)shape;
   Double_t dl[3];
   Double_t ori[3];
   Double_t xyz[3];
   Double_t nxyz[3];
   Double_t dir[3];
   Double_t relp;
   Char_t path[1024];
   Char_t cdir[10];

   // Tree part
   TFile *f=new TFile("geobugs.root","recreate");
   TTree *bug=new TTree("bug","Geometrical problems");
   bug->Branch("pos",xyz,"xyz[3]/D");
   bug->Branch("dir",dir,"dir[3]/D");
   bug->Branch("push",&relp,"push/D");
   bug->Branch("path",&path,"path/C");
   bug->Branch("cdir",&cdir,"cdir/C");
  
   dl[0] = box->GetDX();
   dl[1] = box->GetDY();
   dl[2] = box->GetDZ();
   ori[0] = (box->GetOrigin())[0];
   ori[1] = (box->GetOrigin())[1];
   ori[2] = (box->GetOrigin())[2];
   if (radius>0)
      dl[0] = dl[1] = dl[2] = radius;
  
   TH1::AddDirectory(kFALSE);
   TH1F *hnew = new TH1F("hnew","Precision pushing",30,-20.,10.);
   TH1F *hold = new TH1F("hold","Precision pulling", 30,-20.,10.);
   TH2F *hplotS = new TH2F("hplotS","Problematic points",100,-dl[0],dl[0],100,-dl[1],dl[1]);
   gStyle->SetOptStat(111111);

   TGeoNode *node = 0;
   Long_t igen=0;
   Long_t itry=0;
   Long_t n100 = ntracks/100;
   Double_t rad = TMath::Sqrt(dl[0]*dl[0]+dl[1]*dl[1]);
   printf("Random box : %f, %f, %f, %f, %f, %f\n", ori[0], ori[1], ori[2], dl[0], dl[1], dl[2]);
   printf("Start... %i points\n", ntracks);
   if (!fTimer) fTimer = new TStopwatch();
   fTimer->Reset();
   fTimer->Start();
   while (igen<ntracks) {
      Double_t phi1  = TMath::TwoPi()*gRandom->Rndm();
      Double_t r     = rad*gRandom->Rndm();
      xyz[0] = ori[0] + r*TMath::Cos(phi1);
      xyz[1] = ori[1] + r*TMath::Sin(phi1);
      Double_t z = (1.-2.*gRandom->Rndm());
      xyz[2] = ori[2]+dl[2]*z*TMath::Abs(z);
      ++itry;
      fGeoManager->SetCurrentPoint(xyz);
      node = fGeoManager->FindNode();
      if (!node || node==fGeoManager->GetTopNode()) continue;
      ++igen;
      if (n100 && !(igen%n100)) 
         OpProgress("Sampling progress:",igen, ntracks, fTimer);
      Double_t cost = 1.-2.*gRandom->Rndm();
      Double_t sint = TMath::Sqrt((1.+cost)*(1.-cost));
      Double_t phi  = TMath::TwoPi()*gRandom->Rndm();
      dir[0] = sint * TMath::Cos(phi);
      dir[1] = sint * TMath::Sin(phi);
      dir[2] = cost;
      fGeoManager->SetCurrentDirection(dir);
      fGeoManager->FindNextBoundary();
      Double_t step = fGeoManager->GetStep();

      relp = 1.e-21;
      for(Int_t i=0; i<30; ++i) {
         relp *=10.;
         for(Int_t j=0; j<3; ++j) nxyz[j]=xyz[j]+step*(1.+relp)*dir[j];
         if(!fGeoManager->IsSameLocation(nxyz[0],nxyz[1],nxyz[2])) {
            hnew->Fill(i-20.);
            if(i>15) {
               const Double_t* norm = fGeoManager->FindNormal();
               strcpy(path,fGeoManager->GetPath());
               Double_t dotp = norm[0]*dir[0]+norm[1]*dir[1]+norm[2]*dir[2];
               printf("Forward error i=%d p=%5.4f %5.4f %5.4f s=%5.4f dot=%5.4f path=%s\n",
                       i,xyz[0],xyz[1],xyz[2],step,dotp,path);
               hplotS->Fill(xyz[0],xyz[1],(Double_t)i);
               strcpy(cdir,"Forward");
               bug->Fill();
            }
	         break;
         }
      }
      
      relp = -1.e-21;
      for(Int_t i=0; i<30; ++i) {
         relp *=10.;
         for(Int_t j=0; j<3; ++j) nxyz[j]=xyz[j]+step*(1.+relp)*dir[j];
         if(fGeoManager->IsSameLocation(nxyz[0],nxyz[1],nxyz[2])) {
            hold->Fill(i-20.);
            if(i>15) {
               const Double_t* norm = fGeoManager->FindNormal();
               strcpy(path,fGeoManager->GetPath());
               Double_t dotp = norm[0]*dir[0]+norm[1]*dir[1]+norm[2]*dir[2];
               printf("Backward error i=%d p=%5.4f %5.4f %5.4f s=%5.4f dot=%5.4f path=%s\n",
                       i,xyz[0],xyz[1],xyz[2],step,dotp,path);
               strcpy(cdir,"Backward");
               bug->Fill();
            }
            break;
         }
      }
   }
   fTimer->Stop();

   printf("CPU time/point = %5.2emus: Real time/point = %5.2emus\n",
	       1000000.*fTimer->CpuTime()/itry,1000000.*fTimer->RealTime()/itry);
   bug->Write();
   delete bug;
   bug=0;
   delete f;

   CheckBoundaryReference();

   printf("Effic = %3.1f%%\n",(100.*igen)/itry);
   TCanvas *c1 = new TCanvas("c1","Results",600,800);
   c1->Divide(1,2);
   c1->cd(1);
   gPad->SetLogy();
   hold->Draw();
   c1->cd(2);
   gPad->SetLogy();
   hnew->Draw();
   /*TCanvas *c3 = */new TCanvas("c3","Plot",600,600);
   hplotS->Draw("cont0");
}   

//_____________________________________________________________________________
void TGeoChecker::CheckBoundaryReference(Int_t icheck)
{
// Check the boundary errors reference file created by CheckBoundaryErrors method.
// The shape for which the crossing failed is drawn with the starting point in red
// and the extrapolated point to boundary (+/- failing push/pull) in yellow.
   Double_t xyz[3];
   Double_t nxyz[3];
   Double_t dir[3];
   Double_t lnext[3];
   Double_t push;
   Char_t path[1024];
   Char_t cdir[10];
   // Tree part
   TFile *f=new TFile("geobugs.root","read");
   TTree *bug=(TTree*)f->Get("bug");
   bug->SetBranchAddress("pos",xyz);
   bug->SetBranchAddress("dir",dir);
   bug->SetBranchAddress("push",&push);
   bug->SetBranchAddress("path",&path);
   bug->SetBranchAddress("cdir",&cdir);

   Int_t nentries = (Int_t)bug->GetEntries();
   printf("nentries %d\n",nentries);
   if (icheck<0) {
      for (Int_t i=0;i<nentries;i++) {
         bug->GetEntry(i);
         printf("%-9s error push=%g p=%5.4f %5.4f %5.4f s=%5.4f dot=%5.4f path=%s\n",
	             cdir,push,xyz[0],xyz[1],xyz[2],1.,1.,path);
      }
   } else {
      if (icheck>=nentries) return;
      Int_t idebug = TGeoManager::GetVerboseLevel();
      TGeoManager::SetVerboseLevel(5);
      bug->GetEntry(icheck);
      printf("%-9s error push=%g p=%5.4f %5.4f %5.4f s=%5.4f dot=%5.4f path=%s\n",
	          cdir,push,xyz[0],xyz[1],xyz[2],1.,1.,path);
      fGeoManager->SetCurrentPoint(xyz);
      fGeoManager->SetCurrentDirection(dir);
      fGeoManager->FindNode();
      TGeoNode *next = fGeoManager->FindNextBoundary();
      Double_t step = fGeoManager->GetStep();
      for (Int_t j=0; j<3; j++) nxyz[j]=xyz[j]+step*(1.+0.1*push)*dir[j];
      Bool_t change = !fGeoManager->IsSameLocation(nxyz[0],nxyz[1],nxyz[2]);
      printf("step=%g in: %s\n", step, fGeoManager->GetPath());
      printf("  -> next = %s push=%g  change=%d\n", next->GetName(),push, (UInt_t)change);
      next->GetVolume()->InspectShape();
      next->GetVolume()->DrawOnly();
      if (next != fGeoManager->GetCurrentNode()) {
         Int_t index1 = fGeoManager->GetCurrentVolume()->GetIndex(next);
         if (index1>=0) fGeoManager->CdDown(index1);
      }
      TPolyMarker3D *pm = new TPolyMarker3D();
      fGeoManager->MasterToLocal(xyz, lnext);
      pm->SetNextPoint(lnext[0], lnext[1], lnext[2]);
      pm->SetMarkerStyle(2);
      pm->SetMarkerSize(0.2);
      pm->SetMarkerColor(kRed);
      pm->Draw("SAME");
      TPolyMarker3D *pm1 = new TPolyMarker3D();
      for (Int_t j=0; j<3; j++) nxyz[j]=xyz[j]+step*dir[j];
      fGeoManager->MasterToLocal(nxyz, lnext);
      pm1->SetNextPoint(lnext[0], lnext[1], lnext[2]);
      pm1->SetMarkerStyle(2);
      pm1->SetMarkerSize(0.2);
      pm1->SetMarkerColor(kYellow);
      pm1->Draw("SAME");
      TGeoManager::SetVerboseLevel(idebug);
   }   
   delete bug;
   delete f;
}   

//______________________________________________________________________________
void TGeoChecker::CheckGeometryFull(Bool_t checkoverlaps, Bool_t checkcrossings, Int_t ntracks, const Double_t *vertex)
{
// Geometry checking. Opional overlap checkings (by sampling and by mesh). Optional
// boundary crossing check + timing per volume.
//
// STAGE 1: extensive overlap checking by sampling per volume. Stdout need to be 
//  checked by user to get report, then TGeoVolume::CheckOverlaps(0.01, "s") can 
//  be called for the suspicious volumes.
// STAGE2 : normal overlap checking using the shapes mesh - fills the list of 
//  overlaps.
// STAGE3 : shooting NRAYS rays from VERTEX and counting the total number of 
//  crossings per volume (rays propagated from boundary to boundary until 
//  geometry exit). Timing computed and results stored in a histo.
// STAGE4 : shooting 1 mil. random rays inside EACH volume and calling 
//  FindNextBoundary() + Safety() for each call. The timing is normalized by the 
//  number of crossings computed at stage 2 and presented as percentage. 
//  One can get a picture on which are the most "burned" volumes during 
//  transportation from geometry point of view. Another plot of the timing per 
//  volume vs. number of daughters is produced. 
// All histos are saved in the file statistics.root
   Int_t nuid = fGeoManager->GetListOfUVolumes()->GetEntries();
   if (!fTimer) fTimer = new TStopwatch();
   Int_t i;
   Double_t value;
   fFlags = new Bool_t[nuid];
   memset(fFlags, 0, nuid*sizeof(Bool_t));
   TGeoVolume *vol;
   TCanvas *c = new TCanvas("overlaps", "Overlaps by sampling", 800,800);
   new TRandom3();

// STAGE 1: Overlap checking by sampling per volume
   if (checkoverlaps) {
      printf("====================================================================\n");
      printf("STAGE 1: Overlap checking by sampling within 10 microns\n");
      printf("====================================================================\n");
      fGeoManager->CheckOverlaps(0.001, "s"); 

   // STAGE 2: Global overlap/extrusion checking
      printf("====================================================================\n");
      printf("STAGE 2: Global overlap/extrusion checking within 10 microns\n");
      printf("====================================================================\n");
      fGeoManager->CheckOverlaps(0.001);
   }   
   
   if (!checkcrossings) {
      delete [] fFlags;
      fFlags = 0;
      delete c;
      return;
   }   
   
   fVal1 = new Double_t[nuid];
   fVal2 = new Double_t[nuid];
   memset(fVal1, 0, nuid*sizeof(Double_t));
   memset(fVal2, 0, nuid*sizeof(Double_t));
   // STAGE 3: How many crossings per volume in a realistic case ?
   // Ignore volumes with no daughters

   // Generate rays from vertex in phi=[0,2*pi] theta=[0,pi]
//   Int_t ntracks = 1000000;
   printf("====================================================================\n");
   printf("STAGE 3: Propagating %i tracks starting from vertex\n and conting number of boundary crossings...\n", ntracks);
   printf("====================================================================\n");
   Int_t nbound = 0;
   Double_t theta, phi;
   Double_t point[3], dir[3];
   memset(point, 0, 3*sizeof(Double_t));
   if (vertex) memcpy(point, vertex, 3*sizeof(Double_t));
   
   fTimer->Reset();
   fTimer->Start();
   for (i=0; i<ntracks; i++) {
      phi = 2.*TMath::Pi()*gRandom->Rndm();
      theta= TMath::ACos(1.-2.*gRandom->Rndm());
      dir[0]=TMath::Sin(theta)*TMath::Cos(phi);
      dir[1]=TMath::Sin(theta)*TMath::Sin(phi);
      dir[2]=TMath::Cos(theta);
      if ((i%100)==0) OpProgress("Transporting tracks",i, ntracks, fTimer); 
//      if ((i%1000)==0) printf("... remaining tracks %i\n", ntracks-i);
      nbound += PropagateInGeom(point,dir);
   }
   fTimer->Stop();
   Double_t time1 = fTimer->CpuTime() *1.E6;
   Double_t time2 = time1/ntracks;
   Double_t time3 = time1/nbound;
   OpProgress("Transporting tracks",ntracks, ntracks, fTimer, kTRUE); 
   printf("Time for crossing %i boundaries: %g [ms]\n", nbound, time1);
   printf("Time per track for full geometry traversal: %g [ms], per crossing: %g [ms]\n", time2, time3);

// STAGE 4: How much time per volume:
   
   printf("====================================================================\n");
   printf("STAGE 4: How much navigation time per volume per next+safety call\n");
   printf("====================================================================\n");
   TGeoIterator next(fGeoManager->GetTopVolume());
   TGeoNode*current;
   TString path;
   vol = fGeoManager->GetTopVolume();
   memset(fFlags, 0, nuid*sizeof(Bool_t));
   TStopwatch timer;
   timer.Start();
   i = 0;
   char volname[30];
   sprintf(volname, "Tracking %s", vol->GetName());
   volname[15] = '\0';
   OpProgress(volname,i++, nuid, &timer); 
   Score(vol, 1, TimingPerVolume(vol)); 
   while ((current=next())) {
      vol = current->GetVolume();
      Int_t uid = vol->GetNumber();
      if (fFlags[uid]) continue;
      fFlags[uid] = kTRUE;
      next.GetPath(path);
      fGeoManager->cd(path.Data());
      sprintf(volname, "Tracking %s", vol->GetName());
      volname[15] = '\0';
      OpProgress(volname,i++, nuid, &timer); 
      Score(vol,1,TimingPerVolume(vol));
   }   
   OpProgress("STAGE 4 completed",i, nuid, &timer, kTRUE); 
   // Draw some histos
   Double_t time_tot_pertrack = 0.;
   TCanvas *c1 = new TCanvas("c2","ncrossings",10,10,900,500);
   c1->SetGrid();
   c1->SetTopMargin(0.15);
   TFile *f = new TFile("statistics.root", "RECREATE");
   TH1F *h = new TH1F("h","number of boundary crossings per volume",3,0,3);
   h->SetStats(0);
   h->SetFillColor(38);
   h->SetBit(TH1::kCanRebin);
   
   memset(fFlags, 0, nuid*sizeof(Bool_t));
   for (i=0; i<nuid; i++) {
      vol = fGeoManager->GetVolume(i);
      if (!vol->GetNdaughters()) continue;
      time_tot_pertrack += fVal1[i]*fVal2[i];
      h->Fill(vol->GetName(), (Int_t)fVal1[i]);
   }
   time_tot_pertrack /= ntracks;
   h->LabelsDeflate();
   h->LabelsOption(">","X");
   h->Draw();   


   TCanvas *c2 = new TCanvas("c3","time spent per volume in navigation",10,10,900,500);
   c2->SetGrid();
   c2->SetTopMargin(0.15);
   TH2F *h2 = new TH2F("h2", "time per FNB call vs. ndaughters", 100, 0,100,100,0,15);
   h2->SetStats(0);
   h2->SetMarkerStyle(2);
   TH1F *h1 = new TH1F("h1","percent of time spent per volume",3,0,3);
   h1->SetStats(0);
   h1->SetFillColor(38);
   h1->SetBit(TH1::kCanRebin);
   for (i=0; i<nuid; i++) {
      vol = fGeoManager->GetVolume(i);
      if (!vol->GetNdaughters()) continue;
      value = fVal1[i]*fVal2[i]/ntracks/time_tot_pertrack;
      h1->Fill(vol->GetName(), value);
      h2->Fill(vol->GetNdaughters(), fVal2[i]);
   }     
   h1->LabelsDeflate();
   h1->LabelsOption(">","X");
   h1->Draw();
   TCanvas *c3 = new TCanvas("c4","timing vs. ndaughters",10,10,900,500);
   c3->SetGrid();
   c3->SetTopMargin(0.15);
   h2->Draw();   
   f->Write();
   delete [] fFlags;
   fFlags = 0;
   delete [] fVal1;
   fVal1 = 0;
   delete [] fVal2;
   fVal2 = 0;
   delete fTimer;
   fTimer = 0;
   delete c;
}

//______________________________________________________________________________
Int_t TGeoChecker::PropagateInGeom(Double_t *start, Double_t *dir)
{
// Propagate from START along DIR from boundary to boundary until exiting 
// geometry. Fill array of hits.
   fGeoManager->InitTrack(start, dir);
   TGeoNode *current = 0;
   Int_t nzero = 0;
   Int_t nhits = 0;
   while (!fGeoManager->IsOutside()) {
      current = fGeoManager->FindNextBoundaryAndStep(TGeoShape::Big(), kFALSE);
      if (!current || fGeoManager->IsOutside()) return nhits;
      Double_t step = fGeoManager->GetStep();
      if (step<2.*TGeoShape::Tolerance()) {
         nzero++;
         continue;
      } 
      else nzero = 0;
      if (nzero>3) {
      // Problems in trying to cross a boundary
         printf("Error in trying to cross boundary of %s\n", current->GetName());
         return nhits;
      }
      // Generate the hit
      nhits++;
      TGeoVolume *vol = current->GetVolume();
      Score(vol,0,1.);
      Int_t iup = 1;
      TGeoNode *mother = fGeoManager->GetMother(iup++);
      while (mother && mother->GetVolume()->IsAssembly()) {
         Score(mother->GetVolume(), 0, 1.);
         mother = fGeoManager->GetMother(iup++);
      }   
   }
   return nhits;
}      

//______________________________________________________________________________
void TGeoChecker::Score(TGeoVolume *vol, Int_t ifield, Double_t value)
{
// Score a hit for VOL
   Int_t uid = vol->GetNumber();
   switch (ifield) {
      case 0:
         fVal1[uid] += value;
         break;
      case 1:
         fVal2[uid] += value;
   }
}   

//______________________________________________________________________________
void TGeoChecker::SetNmeshPoints(Int_t npoints) {
// Set number of points to be generated on the shape outline when checking for overlaps.
   fNmeshPoints = npoints;
   if (npoints<1000) {
      Error("SetNmeshPoints", "Cannot allow less than 1000 points for checking - set to 1000");
      fNmeshPoints = 1000;
   }
}   

//______________________________________________________________________________
Double_t TGeoChecker::TimingPerVolume(TGeoVolume *vol)
{
// Compute timing per "FindNextBoundary" + "Safety" call. Volume must be
// in the current path.
   fTimer->Reset();
   const TGeoShape *shape = vol->GetShape();
   TGeoBBox *box = (TGeoBBox *)shape;
   Double_t dx = box->GetDX();
   Double_t dy = box->GetDY();
   Double_t dz = box->GetDZ();
   Double_t ox = (box->GetOrigin())[0];
   Double_t oy = (box->GetOrigin())[1];
   Double_t oz = (box->GetOrigin())[2];
   Double_t point[3], dir[3], lpt[3], ldir[3];
   Double_t pstep = 0.;
   pstep = TMath::Max(pstep,dz);
   Double_t theta, phi;
   Int_t idaughter;
   fTimer->Start();
   Double_t dist;
   Bool_t inside;
   for (Int_t i=0; i<1000000; i++) {
      lpt[0] = ox-dx+2*dx*gRandom->Rndm();
      lpt[1] = oy-dy+2*dy*gRandom->Rndm();
      lpt[2] = oz-dz+2*dz*gRandom->Rndm();
      fGeoManager->GetCurrentMatrix()->LocalToMaster(lpt,point);
      fGeoManager->SetCurrentPoint(point[0],point[1],point[2]);
      phi = 2*TMath::Pi()*gRandom->Rndm();
      theta= TMath::ACos(1.-2.*gRandom->Rndm());
      ldir[0]=TMath::Sin(theta)*TMath::Cos(phi);
      ldir[1]=TMath::Sin(theta)*TMath::Sin(phi);
      ldir[2]=TMath::Cos(theta);
      fGeoManager->GetCurrentMatrix()->LocalToMasterVect(ldir,dir);
      fGeoManager->SetCurrentDirection(dir);
      fGeoManager->SetStep(pstep);
      fGeoManager->ResetState();
      inside = kTRUE;
      dist = TGeoShape::Big();
      if (!vol->IsAssembly()) {
         inside = vol->Contains(lpt);
         if (!inside) {
            dist = vol->GetShape()->DistFromOutside(lpt,ldir,3,pstep); 
//            if (dist>=pstep) continue;
         } else {   
            vol->GetShape()->DistFromInside(lpt,ldir,3,pstep);
         }   
            
         if (!vol->GetNdaughters()) vol->GetShape()->Safety(lpt, inside);
      }   
      if (vol->GetNdaughters()) {
         fGeoManager->Safety();
         fGeoManager->FindNextDaughterBoundary(point,dir,idaughter,kFALSE);
      }   
   }
   fTimer->Stop();
   Double_t time_per_track = fTimer->CpuTime();
   Int_t uid = vol->GetNumber();
   Int_t ncrossings = (Int_t)fVal1[uid];
   if (!vol->GetNdaughters())
      printf("Time for volume %s (shape=%s): %g [ms] ndaughters=%d ncross=%d\n", vol->GetName(), vol->GetShape()->GetName(), time_per_track, vol->GetNdaughters(), ncrossings);
   else
      printf("Time for volume %s (assemb=%d): %g [ms] ndaughters=%d ncross=%d\n", vol->GetName(), vol->IsAssembly(), time_per_track, vol->GetNdaughters(), ncrossings);
   return time_per_track;
}

//______________________________________________________________________________
void TGeoChecker::CheckGeometry(Int_t nrays, Double_t startx, Double_t starty, Double_t startz) const
{
// Shoot nrays with random directions from starting point (startx, starty, startz)
// in the reference frame of this volume. Track each ray until exiting geometry, then
// shoot backwards from exiting point and compare boundary crossing points.
   Int_t i, j;
   Double_t start[3], end[3];
   Double_t dir[3];
   Double_t dummy[3];
   Double_t eps = 0.;
   Double_t *array1 = new Double_t[3*1000];
   Double_t *array2 = new Double_t[3*1000];
   TObjArray *pma = new TObjArray();
   TPolyMarker3D *pm;
   pm = new TPolyMarker3D();
   pm->SetMarkerColor(2); // error > eps
   pm->SetMarkerStyle(8);
   pm->SetMarkerSize(0.4);
   pma->AddAt(pm, 0);
   pm = new TPolyMarker3D();
   pm->SetMarkerColor(4); // point not found
   pm->SetMarkerStyle(8);
   pm->SetMarkerSize(0.4);
   pma->AddAt(pm, 1);
   pm = new TPolyMarker3D();
   pm->SetMarkerColor(6); // extra point back
   pm->SetMarkerStyle(8);
   pm->SetMarkerSize(0.4);
   pma->AddAt(pm, 2);
   Int_t nelem1, nelem2;
   Int_t dim1=1000, dim2=1000;
   if ((startx==0) && (starty==0) && (startz==0)) eps=1E-3;
   start[0] = startx+eps;
   start[1] = starty+eps;
   start[2] = startz+eps;
   Int_t n10=nrays/10;
   Double_t theta,phi;
   Double_t dw, dwmin, dx, dy, dz;
   Int_t ist1, ist2, ifound;
   for (i=0; i<nrays; i++) {
      if (n10) {
         if ((i%n10) == 0) printf("%i percent\n", Int_t(100*i/nrays));
      }
      phi = 2*TMath::Pi()*gRandom->Rndm();
      theta= TMath::ACos(1.-2.*gRandom->Rndm());
      dir[0]=TMath::Sin(theta)*TMath::Cos(phi);
      dir[1]=TMath::Sin(theta)*TMath::Sin(phi);
      dir[2]=TMath::Cos(theta);
      // shoot direct ray
      nelem1=nelem2=0;
//      printf("DIRECT %i\n", i);
      ShootRay(&start[0], dir[0], dir[1], dir[2], array1, nelem1, dim1);
      if (!nelem1) continue;
//      for (j=0; j<nelem1; j++) printf("%i : %f %f %f\n", j, array1[3*j], array1[3*j+1], array1[3*j+2]);
      memcpy(&end[0], &array1[3*(nelem1-1)], 3*sizeof(Double_t));
      // shoot ray backwards
//      printf("BACK %i\n", i);
      ShootRay(&end[0], -dir[0], -dir[1], -dir[2], array2, nelem2, dim2, &start[0]);
      if (!nelem2) {
         printf("#### NOTHING BACK ###########################\n");
         for (j=0; j<nelem1; j++) {
            pm = (TPolyMarker3D*)pma->At(0);
            pm->SetNextPoint(array1[3*j], array1[3*j+1], array1[3*j+2]);
         }
         continue;
      }             
//      printf("BACKWARDS\n");
      Int_t k=nelem2>>1;
      for (j=0; j<k; j++) {
         memcpy(&dummy[0], &array2[3*j], 3*sizeof(Double_t));
         memcpy(&array2[3*j], &array2[3*(nelem2-1-j)], 3*sizeof(Double_t));
         memcpy(&array2[3*(nelem2-1-j)], &dummy[0], 3*sizeof(Double_t));
      }
//      for (j=0; j<nelem2; j++) printf("%i : %f ,%f ,%f   \n", j, array2[3*j], array2[3*j+1], array2[3*j+2]);         
      if (nelem1!=nelem2) printf("### DIFFERENT SIZES : nelem1=%i nelem2=%i ##########\n", nelem1, nelem2);
      ist1 = ist2 = 0;
      // check first match
      
      dx = array1[3*ist1]-array2[3*ist2];
      dy = array1[3*ist1+1]-array2[3*ist2+1];
      dz = array1[3*ist1+2]-array2[3*ist2+2];
      dw = dx*dir[0]+dy*dir[1]+dz*dir[2];
      fGeoManager->SetCurrentPoint(&array1[3*ist1]);
      fGeoManager->FindNode();
//      printf("%i : %s (%g, %g, %g)\n", ist1, fGeoManager->GetPath(), 
//             array1[3*ist1], array1[3*ist1+1], array1[3*ist1+2]);
      if (TMath::Abs(dw)<1E-4) {
//         printf("   matching %i (%g, %g, %g)\n", ist2, array2[3*ist2], array2[3*ist2+1], array2[3*ist2+2]);
         ist2++;
      } else {
         printf("### NOT MATCHING %i f:(%f, %f, %f) b:(%f %f %f) DCLOSE=%f\n", ist2, array1[3*ist1], array1[3*ist1+1], array1[3*ist1+2], array2[3*ist2], array2[3*ist2+1], array2[3*ist2+2],dw);
         pm = (TPolyMarker3D*)pma->At(0);
         pm->SetNextPoint(array2[3*ist2], array2[3*ist2+1], array2[3*ist2+2]);
         if (dw<0) {
            // first boundary missed on way back
         } else {
            // first boundary different on way back
            ist2++;
         }
      }      
      
      while ((ist1<nelem1-1) && (ist2<nelem2)) {
         fGeoManager->SetCurrentPoint(&array1[3*ist1+3]);
         fGeoManager->FindNode();
//         printf("%i : %s (%g, %g, %g)\n", ist1+1, fGeoManager->GetPath(), 
//                array1[3*ist1+3], array1[3*ist1+4], array1[3*ist1+5]);
         
         dx = array1[3*ist1+3]-array1[3*ist1];
         dy = array1[3*ist1+4]-array1[3*ist1+1];
         dz = array1[3*ist1+5]-array1[3*ist1+2];
         // distance to next point
         dwmin = dx+dir[0]+dy*dir[1]+dz*dir[2];
         while (ist2<nelem2) {
            ifound = 0;
            dx = array2[3*ist2]-array1[3*ist1];
            dy = array2[3*ist2+1]-array1[3*ist1+1];
            dz = array2[3*ist2+2]-array1[3*ist1+2];
            dw = dx+dir[0]+dy*dir[1]+dz*dir[2];
            if (TMath::Abs(dw-dwmin)<1E-4) {
               ist1++;
               ist2++;
               break;
            }   
            if (dw<dwmin) {
            // point found on way back. Check if close enough to ist1+1
               ifound++;
               dw = dwmin-dw;
               if (dw<1E-4) {
               // point is matching ist1+1
//                  printf("   matching %i (%g, %g, %g) DCLOSE=%g\n", ist2, array2[3*ist2], array2[3*ist2+1], array2[3*ist2+2], dw);
                  ist2++;
                  ist1++;
                  break;
               } else {
               // extra boundary found on way back   
                  fGeoManager->SetCurrentPoint(&array2[3*ist2]);
                  fGeoManager->FindNode();
                  pm = (TPolyMarker3D*)pma->At(2);
                  pm->SetNextPoint(array2[3*ist2], array2[3*ist2+1], array2[3*ist2+2]);
                  printf("### EXTRA BOUNDARY %i :  %s found at DCLOSE=%f\n", ist2, fGeoManager->GetPath(), dw);
                  ist2++;
                  continue;
               }
            } else {
               if (!ifound) {
                  // point ist1+1 not found on way back
                  fGeoManager->SetCurrentPoint(&array1[3*ist1+3]);
                  fGeoManager->FindNode();
                  pm = (TPolyMarker3D*)pma->At(1);
                  pm->SetNextPoint(array2[3*ist1+3], array2[3*ist1+4], array2[3*ist1+5]);
                  printf("### BOUNDARY MISSED BACK #########################\n");
                  ist1++;
                  break;
               } else {
                  ist1++;
                  break;
               }
            }
         }               
      }                          
   }   
   pm = (TPolyMarker3D*)pma->At(0);
   pm->Draw("SAME");
   pm = (TPolyMarker3D*)pma->At(1);
   pm->Draw("SAME");
   pm = (TPolyMarker3D*)pma->At(2);
   pm->Draw("SAME");
   if (gPad) {
      gPad->Modified();
      gPad->Update();
   }      
   delete [] array1;
   delete [] array2;
}

//______________________________________________________________________________
void TGeoChecker::CleanPoints(Double_t *points, Int_t &numPoints) const
{
// Clean-up the mesh of pcon/pgon from useless points
   Int_t ipoint = 0;
   Int_t j, k=0;
   Double_t rsq;
   for (Int_t i=0; i<numPoints; i++) {
      j = 3*i;
      rsq = points[j]*points[j]+points[j+1]*points[j+1];
      if (rsq < 1.e-10) continue;
      points[k] = points[j];
      points[k+1] = points[j+1];
      points[k+2] = points[j+2];
      ipoint++;
      k = 3*ipoint;
   }
   numPoints = ipoint;
}      

//______________________________________________________________________________
TGeoOverlap *TGeoChecker::MakeCheckOverlap(const char *name, TGeoVolume *vol1, TGeoVolume *vol2, TGeoMatrix *mat1, TGeoMatrix *mat2, Bool_t isovlp, Double_t ovlp)
{
// Check if the 2 non-assembly volume candidates overlap/extrude. Returns overlap object.
   TGeoOverlap *nodeovlp = 0;
   Int_t numPoints1 = fBuff1->NbPnts();
   Int_t numSegs1   = fBuff1->NbSegs();
   Int_t numPols1   = fBuff1->NbPols();
   Int_t numPoints2 = fBuff2->NbPnts();
   Int_t numSegs2   = fBuff2->NbSegs();
   Int_t numPols2   = fBuff2->NbPols();
   Int_t ip;
   Bool_t extrude, isextrusion, isoverlapping;
   Double_t *points1 = fBuff1->fPnts; 
   Double_t *points2 = fBuff2->fPnts;
   Double_t local[3], local1[3];
   Double_t point[3];
   Double_t safety = TGeoShape::Big();
   if (vol1->IsAssembly() || vol2->IsAssembly()) return nodeovlp;
   TGeoShape *shape1 = vol1->GetShape();
   TGeoShape *shape2 = vol2->GetShape();
   OpProgress("refresh", 0,0,NULL,kFALSE,kTRUE);   
   shape1->GetMeshNumbers(numPoints1, numSegs1, numPols1);
   if (!shape1->IsComposite() && 
       fBuff1->fID != (TObject*)shape1) {
      // Fill first buffer.
      fBuff1->SetRawSizes(TMath::Max(numPoints1,fNmeshPoints), 3*TMath::Max(numPoints1,fNmeshPoints), 0, 0, 0, 0);
      points1 = fBuff1->fPnts;
      if (shape1->GetPointsOnSegments(fNmeshPoints, points1)) {
         numPoints1 = fNmeshPoints;
      } else {   
         shape1->SetPoints(points1);
      }   
//      if (shape1->InheritsFrom(TGeoPcon::Class())) {
//         CleanPoints(points1, numPoints1);
//         fBuff1->SetRawSizes(numPoints1, 3*numPoints1,0, 0, 0, 0);
//      }   
      fBuff1->fID = shape1;
   }   
   shape2->GetMeshNumbers(numPoints2, numSegs2, numPols2);
   if (!shape2->IsComposite() && 
       fBuff2->fID != (TObject*)shape2) {
      // Fill second buffer.
      fBuff2->SetRawSizes(TMath::Max(numPoints2,fNmeshPoints), 3*TMath::Max(numPoints2,fNmeshPoints), 0, 0, 0,0);
      points2 = fBuff2->fPnts;
      if (shape2->GetPointsOnSegments(fNmeshPoints, points2)) {
         numPoints2 = fNmeshPoints;
      } else {   
         shape2->SetPoints(points2);
      }   
//      if (shape2->InheritsFrom(TGeoPcon::Class())) {
//         CleanPoints(points2, numPoints2);
//         fBuff1->SetRawSizes(numPoints2, 3*numPoints2,0,0,0,0);
//      }   
      fBuff2->fID = shape2;
   }   
      
   if (!isovlp) {
   // Extrusion case. Test vol2 extrude vol1.
      isextrusion=kFALSE;
      // loop all points of the daughter
      if (!shape2->IsComposite()) {
         for (ip=0; ip<numPoints2; ip++) {
            memcpy(local, &points2[3*ip], 3*sizeof(Double_t));
            if (local[0]<1e-10 && local[1]<1e-10) continue;
            mat2->LocalToMaster(local, point);
            mat1->MasterToLocal(point, local);
            extrude = !shape1->Contains(local);
            if (extrude) {
               safety = shape1->Safety(local, kFALSE);
               if (safety<ovlp) extrude=kFALSE;
            }    
            if (extrude) {
               if (!isextrusion) {
                  isextrusion = kTRUE;
                  nodeovlp = new TGeoOverlap(name, vol1, vol2, mat1,mat2,kFALSE,safety);
                  nodeovlp->SetNextPoint(point[0],point[1],point[2]);
                  fGeoManager->AddOverlap(nodeovlp);
               } else {
                  if (safety>nodeovlp->GetOverlap()) nodeovlp->SetOverlap(safety);
                  nodeovlp->SetNextPoint(point[0],point[1],point[2]);
               }   
            }
         }
      }                
      // loop all points of the mother
      if (!shape1->IsComposite()) {
         for (ip=0; ip<numPoints1; ip++) {
            memcpy(local, &points1[3*ip], 3*sizeof(Double_t));
            if (local[0]<1e-10 && local[1]<1e-10) continue;
            mat1->LocalToMaster(local, point);
            mat2->MasterToLocal(point, local1);
            extrude = shape2->Contains(local1);
            if (extrude) {
               // skip points on mother mesh that have no neghbourhood ouside mother
               safety = shape1->Safety(local,kTRUE);
               if (safety>1E-6) {
                  extrude = kFALSE;
               } else {   
                  safety = shape2->Safety(local1,kTRUE);
                  if (safety<ovlp) extrude=kFALSE;
               }   
            }   
            if (extrude) {
               if (!isextrusion) {
                  isextrusion = kTRUE;
                  nodeovlp = new TGeoOverlap(name, vol1,vol2,mat1,mat2,kFALSE,safety);
                  nodeovlp->SetNextPoint(point[0],point[1],point[2]);
                  fGeoManager->AddOverlap(nodeovlp);
               } else {
                  if (safety>nodeovlp->GetOverlap()) nodeovlp->SetOverlap(safety);
                  nodeovlp->SetNextPoint(point[0],point[1],point[2]);
               }   
            }
         }
      }
      return nodeovlp;
   }            
   // Check overlap
   Bool_t overlap;
   overlap = kFALSE;
   isoverlapping = kFALSE;
   if (!shape1->IsComposite()) {
      // loop all points of first candidate
      for (ip=0; ip<numPoints1; ip++) {
         memcpy(local, &points1[3*ip], 3*sizeof(Double_t));
         if (local[0]<1e-10 && local[1]<1e-10) continue;
         mat1->LocalToMaster(local, point);
         mat2->MasterToLocal(point, local); // now point in local reference of second
         overlap = shape2->Contains(local);
         if (overlap) {
            safety = shape2->Safety(local, kTRUE);
            if (safety<ovlp) overlap=kFALSE;
         }    
         if (overlap) {
            if (!isoverlapping) {
               isoverlapping = kTRUE;
               nodeovlp = new TGeoOverlap(name,vol1,vol2,mat1,mat2,kTRUE,safety);
               nodeovlp->SetNextPoint(point[0],point[1],point[2]);
               fGeoManager->AddOverlap(nodeovlp);
            } else {
               if (safety>nodeovlp->GetOverlap()) nodeovlp->SetOverlap(safety);
               nodeovlp->SetNextPoint(point[0],point[1],point[2]);
            }     
         }
      }
   }   
   // loop all points of second candidate
   if (!shape2->IsComposite()) {
      for (ip=0; ip<numPoints2; ip++) {
         memcpy(local, &points2[3*ip], 3*sizeof(Double_t));
         if (local[0]<1e-10 && local[1]<1e-10) continue;
         mat2->LocalToMaster(local, point);
         mat1->MasterToLocal(point, local); // now point in local reference of first
         overlap = shape1->Contains(local);
         if (overlap) {
            safety = shape1->Safety(local, kTRUE);
            if (safety<ovlp) overlap=kFALSE;
         }    
         if (overlap) {
            if (!isoverlapping) {
               isoverlapping = kTRUE;
               nodeovlp = new TGeoOverlap(name,vol1,vol2,mat1,mat2,kTRUE,safety);
               nodeovlp->SetNextPoint(point[0],point[1],point[2]);
               fGeoManager->AddOverlap(nodeovlp);
            } else {
               if (safety>nodeovlp->GetOverlap()) nodeovlp->SetOverlap(safety);
               nodeovlp->SetNextPoint(point[0],point[1],point[2]);
            }     
         }
      }
   } 
   return nodeovlp;  
}

//______________________________________________________________________________
void TGeoChecker::CheckOverlapsBySampling(TGeoVolume *vol, Double_t /* ovlp */, Int_t npoints) const
{
// Check illegal overlaps for volume VOL within a limit OVLP by sampling npoints
// inside the volume shape.
   Int_t nd = vol->GetNdaughters();
   if (nd<2) return;
   TGeoVoxelFinder *voxels = vol->GetVoxels();
   if (!voxels) return;
   if (voxels->NeedRebuild()) {
      voxels->Voxelize();
      vol->FindOverlaps();
   }   
   TGeoBBox *box = (TGeoBBox*)vol->GetShape();
   TGeoShape *shape;
   TGeoNode *node;
   Double_t dx = box->GetDX();
   Double_t dy = box->GetDY();
   Double_t dz = box->GetDZ();
   Double_t pt[3];
   Double_t local[3];
   Int_t *check_list = 0;
   Int_t ncheck = 0;
   const Double_t *orig = box->GetOrigin();
   Int_t ipoint = 0;
   Int_t itry = 0;
   Int_t iovlp = 0;
   Int_t id=0, id0=0, id1=0;
   Bool_t in, incrt;
   Double_t safe;
   TString name1 = "";
   TString name2 = "";
   TGeoOverlap **flags = 0;
   TGeoNode *node1, *node2;
   Int_t novlps = 0;
   TGeoHMatrix mat1, mat2;
   if (!gRandom) new TRandom3();
   while (ipoint < npoints) {
   // Shoot randomly in the bounding box.
      pt[0] = orig[0] - dx + 2.*dx*gRandom->Rndm();
      pt[1] = orig[1] - dy + 2.*dy*gRandom->Rndm();
      pt[2] = orig[2] - dz + 2.*dz*gRandom->Rndm();
      if (!vol->Contains(pt)) {
         itry++;
         if (itry>10000 && !ipoint) {
            Error("CheckOverlapsBySampling", "No point inside volume!!! - aborting");
            break;
         }   
         continue;
      }   
      // Check if the point is inside one or more daughters
      in = kFALSE;
      ipoint++;
      check_list = voxels->GetCheckList(pt, ncheck);
      if (!check_list || ncheck<2) continue;
      for (id=0; id<ncheck; id++) {
         id0 = check_list[id];
         node  = vol->GetNode(id0);
         // Ignore MANY's
         if (node->IsOverlapping()) continue;
         node->GetMatrix()->MasterToLocal(pt,local);
         shape = node->GetVolume()->GetShape();
         incrt = shape->Contains(local);
         if (!incrt) continue;
         if (!in) {
            in = kTRUE;
            id1 = id0;
            continue;
         }
         // The point is inside 2 or more daughters, check safety
         safe = shape->Safety(local, kTRUE);
//         if (safe < ovlp) continue;
         // We really have found an overlap -> store the point in a container
         iovlp++;
         if (!novlps) {
            flags = new TGeoOverlap*[nd*nd];
            memset(flags, 0, nd*nd*sizeof(TGeoOverlap*));
         }
         TGeoOverlap *nodeovlp = flags[nd*id1+id0];
         if (!nodeovlp) {
            novlps++;
            node1 = vol->GetNode(id1);
            name1 = node1->GetName();
            mat1 = node1->GetMatrix();
            Int_t cindex = node1->GetVolume()->GetCurrentNodeIndex();
            while (cindex >= 0) {
               node1 = node1->GetVolume()->GetNode(cindex);
               name1 += Form("/%s", node1->GetName());
               mat1.Multiply(node1->GetMatrix());
               cindex = node1->GetVolume()->GetCurrentNodeIndex();
            }   
            node2 = vol->GetNode(id0);
            name2 = node2->GetName();
            mat2 = node2->GetMatrix();
            cindex = node2->GetVolume()->GetCurrentNodeIndex();
            while (cindex >= 0) {
               node2 = node2->GetVolume()->GetNode(cindex);
               name2 += Form("/%s", node2->GetName());
               mat2.Multiply(node2->GetMatrix());
               cindex = node2->GetVolume()->GetCurrentNodeIndex();
            }   
            nodeovlp = new TGeoOverlap(Form("Volume %s: node %s overlapping %s", 
               vol->GetName(), name1.Data(), name2.Data()), node1->GetVolume(),node2->GetVolume(),
               &mat1,&mat2, kTRUE, safe);
            flags[nd*id1+id0] = nodeovlp;
            fGeoManager->AddOverlap(nodeovlp);
         } 
         // Max 100 points per marker
         if (nodeovlp->GetPolyMarker()->GetN()<100) nodeovlp->SetNextPoint(pt[0],pt[1],pt[2]);
         if (nodeovlp->GetOverlap()<safe) nodeovlp->SetOverlap(safe);
      }
   }

   if (flags) delete [] flags;
   if (!novlps) return;
   Double_t capacity = vol->GetShape()->Capacity();
   capacity *= Double_t(iovlp)/Double_t(npoints);
   Double_t err = 1./TMath::Sqrt(Double_t(iovlp));
   Info("CheckOverlapsBySampling", "#Found %d overlaps adding-up to %g +/- %g [cm3] for daughters of %s",
         novlps, capacity, err*capacity, vol->GetName());
}   

//______________________________________________________________________________
Int_t TGeoChecker::NChecksPerVolume(TGeoVolume *vol)
{
// Compute number of overlaps combinations to check per volume
   if (vol->GetFinder()) return 0;
   UInt_t nd = vol->GetNdaughters();
   if (!nd) return 0;
   Bool_t is_assembly = vol->IsAssembly();
   TGeoIterator next1(vol);
   TGeoIterator next2(vol);
   Int_t nchecks = 0;
   TGeoNode *node;
   UInt_t id;
   if (!is_assembly) {      
      while ((node=next1())) {
         if (node->IsOverlapping()) {
            next1.Skip();
            continue;
         }   
         if (!node->GetVolume()->IsAssembly()) {
            nchecks++;
            next1.Skip();
         }
      }            
   }
   // now check if the daughters overlap with each other
   if (nd<2) return nchecks;
   TGeoVoxelFinder *vox = vol->GetVoxels();
   if (!vox) return nchecks;


   TGeoNode *node1, *node01, *node02;
   Int_t novlp;
   Int_t *ovlps;
   Int_t ko;
   UInt_t io;
   for (id=0; id<nd; id++) {  
      node01 = vol->GetNode(id);
      if (node01->IsOverlapping()) continue;
      vox->FindOverlaps(id);
      ovlps = node01->GetOverlaps(novlp);
      if (!ovlps) continue;
      for (ko=0; ko<novlp; ko++) { // loop all possible overlapping candidates
         io = ovlps[ko];           // (node1, shaped, matrix1, points, fBuff1)
         if (io<=id) continue;
         node02 = vol->GetNode(io);
         if (node02->IsOverlapping()) continue;        
        
         // We have to check node against node1, but they may be assemblies
         if (node01->GetVolume()->IsAssembly()) {
            next1.Reset(node01->GetVolume());
            while ((node=next1())) {
               if (!node->GetVolume()->IsAssembly()) {
                  if (node02->GetVolume()->IsAssembly()) {
                     next2.Reset(node02->GetVolume());
                     while ((node1=next2())) {
                        if (!node1->GetVolume()->IsAssembly()) {
                           nchecks++;
                           next2.Skip();
                        }
                     }
                  } else {
                     nchecks++;
                  }
                  next1.Skip();
               }
            }
         } else {
            // node not assembly         
            if (node02->GetVolume()->IsAssembly()) { 
               next2.Reset(node02->GetVolume());
               while ((node1=next2())) {
                  if (!node1->GetVolume()->IsAssembly()) {
                     nchecks++;
                     next2.Skip();
                  }
               }
            } else {
               // node1 also not assembly
               nchecks++;
            }
         }                         
      }                
      node01->SetOverlaps(0,0);
   }   
   return nchecks;
}      

//______________________________________________________________________________
void TGeoChecker::CheckOverlaps(const TGeoVolume *vol, Double_t ovlp, Option_t *option)
{
// Check illegal overlaps for volume VOL within a limit OVLP.
   if (vol->GetFinder()) return;
   UInt_t nd = vol->GetNdaughters();
   if (!nd) return;
   TGeoShape::SetTransform(gGeoIdentity);
   fNchecks = NChecksPerVolume((TGeoVolume*)vol);
   Bool_t sampling = kFALSE;
   TString opt(option);
   opt.ToLower();
   if (opt.Contains("s")) sampling = kTRUE;
   if (opt.Contains("f")) fFullCheck = kTRUE;
   else                   fFullCheck = kFALSE;
   if (sampling) {
      opt = opt.Strip(TString::kLeading, 's');
      Int_t npoints = atoi(opt.Data());
      if (!npoints) npoints = 1000000;
      CheckOverlapsBySampling((TGeoVolume*)vol, ovlp, npoints);
      return;
   }   
   Bool_t is_assembly = vol->IsAssembly();
   TGeoIterator next1((TGeoVolume*)vol);
   TGeoIterator next2((TGeoVolume*)vol);
   TString path;
   // first, test if daughters extrude their container
   TGeoNode * node, *nodecheck;
   TGeoChecker *checker = (TGeoChecker*)this;

   TGeoOverlap *nodeovlp = 0;
   UInt_t id;
   Int_t level;
// Check extrusion only for daughters of a non-assembly volume
   if (!is_assembly) {      
      while ((node=next1())) {
         if (node->IsOverlapping()) {
            next1.Skip();
            continue;
         }   
         if (!node->GetVolume()->IsAssembly()) {
            if (fSelectedNode) {
            // We have to check only overlaps of the selected node (or real daughters if an assembly)
               if ((fSelectedNode != node) && (!fSelectedNode->GetVolume()->IsAssembly())) {
                  next1.Skip();
                  continue;
               }
               if (node != fSelectedNode) {   
                  level = next1.GetLevel();
                  while ((nodecheck=next1.GetNode(level--))) {
                     if (nodecheck == fSelectedNode) break;
                  }
                  if (!nodecheck) {   
                     next1.Skip();
                     continue;
                  }   
               }   
            }
            next1.GetPath(path);
            nodeovlp = checker->MakeCheckOverlap(Form("%s extruded by: %s", vol->GetName(),path.Data()),
                                 (TGeoVolume*)vol,node->GetVolume(),gGeoIdentity,(TGeoMatrix*)next1.GetCurrentMatrix(),kFALSE,ovlp);
            next1.Skip();
         }
      }            
   }

   // now check if the daughters overlap with each other
   if (nd<2) return;
   TGeoVoxelFinder *vox = vol->GetVoxels();
   if (!vox) {
      Warning("CheckOverlaps", "Volume %s with %i daughters but not voxelized", vol->GetName(),nd);
      return;
   } 
   if (vox->NeedRebuild()) {
      vox->Voxelize();
      vol->FindOverlaps();
   }     
   TGeoNode *node1, *node01, *node02;
   TGeoHMatrix hmat1, hmat2;
   TString path1;
   Int_t novlp;
   Int_t *ovlps;
   Int_t ko;
   UInt_t io;
   for (id=0; id<nd; id++) {  
      node01 = vol->GetNode(id);
      if (node01->IsOverlapping()) continue;
      vox->FindOverlaps(id);
      ovlps = node01->GetOverlaps(novlp);
      if (!ovlps) continue;
      next1.SetTopName(node01->GetName());
      path = node01->GetName();
      for (ko=0; ko<novlp; ko++) { // loop all possible overlapping candidates
         io = ovlps[ko];           // (node1, shaped, matrix1, points, fBuff1)
         if (io<=id) continue;
         node02 = vol->GetNode(io);
         if (node02->IsOverlapping()) continue;        
         // Try to fasten-up things...
//         if (!TGeoBBox::AreOverlapping((TGeoBBox*)node01->GetVolume()->GetShape(), node01->GetMatrix(),
//                                       (TGeoBBox*)node02->GetVolume()->GetShape(), node02->GetMatrix())) continue;
         next2.SetTopName(node02->GetName());
         path1 = node02->GetName();
        
         // We have to check node against node1, but they may be assemblies
         if (node01->GetVolume()->IsAssembly()) {
            next1.Reset(node01->GetVolume());
            while ((node=next1())) {
               if (!node->GetVolume()->IsAssembly()) {
                  next1.GetPath(path);
                  hmat1 = node01->GetMatrix();
                  hmat1 *= *next1.GetCurrentMatrix();
                  if (node02->GetVolume()->IsAssembly()) {
                     next2.Reset(node02->GetVolume());
                     while ((node1=next2())) {
                        if (!node1->GetVolume()->IsAssembly()) {
                           if (fSelectedNode) {
                           // We have to check only overlaps of the selected node (or real daughters if an assembly)
                              if ((fSelectedNode != node) && (fSelectedNode != node1) && (!fSelectedNode->GetVolume()->IsAssembly())) {
                                 next2.Skip();
                                 continue;
                              }   
                              if ((fSelectedNode != node1) && (fSelectedNode != node)) {
                                 level = next2.GetLevel();
                                 while ((nodecheck=next2.GetNode(level--))) {
                                    if (nodecheck == fSelectedNode) break;
                                 }
                                 if (node02 == fSelectedNode) nodecheck = node02;
                                 if (!nodecheck) {   
                                    level = next1.GetLevel();
                                    while ((nodecheck=next1.GetNode(level--))) {
                                       if (nodecheck == fSelectedNode) break;
                                    }   
                                 }
                                 if (node01 == fSelectedNode) nodecheck = node01;
                                 if (!nodecheck) {   
                                    next2.Skip();
                                    continue;
                                 }   
                              }   
                           }
                           next2.GetPath(path1);
                           hmat2 = node02->GetMatrix();
                           hmat2 *= *next2.GetCurrentMatrix();
                           nodeovlp = checker->MakeCheckOverlap(Form("%s/%s overlapping %s/%s", vol->GetName(),path.Data(),vol->GetName(),path1.Data()),
                                              node->GetVolume(),node1->GetVolume(),&hmat1,&hmat2,kTRUE,ovlp);  
                           next2.Skip();
                        }
                     }
                  } else {
                     if (fSelectedNode) {
                     // We have to check only overlaps of the selected node (or real daughters if an assembly)
                        if ((fSelectedNode != node) && (fSelectedNode != node02) && (!fSelectedNode->GetVolume()->IsAssembly())) {
                           next1.Skip();
                           continue;
                        }   
                        if ((fSelectedNode != node) && (fSelectedNode != node02)) {
                           level = next1.GetLevel();
                           while ((nodecheck=next1.GetNode(level--))) {
                              if (nodecheck == fSelectedNode) break;
                           }
                           if (node01 == fSelectedNode) nodecheck = node01;
                           if (!nodecheck) {   
                              next1.Skip();
                              continue;
                           }   
                        }   
                     }
                     nodeovlp = checker->MakeCheckOverlap(Form("%s/%s overlapping %s/%s", vol->GetName(),path.Data(),vol->GetName(),path1.Data()),
                                        node->GetVolume(),node02->GetVolume(),&hmat1,node02->GetMatrix(),kTRUE,ovlp);  
                  }
                  next1.Skip();
               }
            }
         } else {
            // node not assembly         
            if (node02->GetVolume()->IsAssembly()) { 
               next2.Reset(node02->GetVolume());
               while ((node1=next2())) {
                  if (!node1->GetVolume()->IsAssembly()) {
                     if (fSelectedNode) {
                     // We have to check only overlaps of the selected node (or real daughters if an assembly)
                        if ((fSelectedNode != node1) && (fSelectedNode != node01) && (!fSelectedNode->GetVolume()->IsAssembly())) {
                           next2.Skip();
                           continue;
                        }   
                        if ((fSelectedNode != node1) && (fSelectedNode != node01)) {
                           level = next2.GetLevel();
                           while ((nodecheck=next2.GetNode(level--))) {
                              if (nodecheck == fSelectedNode) break;
                           }
                           if (node02 == fSelectedNode) nodecheck = node02;
                           if (!nodecheck) {   
                              next2.Skip();
                              continue;
                           }   
                        }   
                     }
                     next2.GetPath(path1);
                     hmat2 = node02->GetMatrix();
                     hmat2 *= *next2.GetCurrentMatrix();
                     nodeovlp = checker->MakeCheckOverlap(Form("%s/%s overlapping %s/%s", vol->GetName(),path.Data(),vol->GetName(),path1.Data()),
                                        node01->GetVolume(),node1->GetVolume(),node01->GetMatrix(),&hmat2,kTRUE,ovlp);  
                     next2.Skip();
                  }
               }
            } else {
               // node1 also not assembly
               if (fSelectedNode && (fSelectedNode != node01) && (fSelectedNode != node02)) continue;
               nodeovlp = checker->MakeCheckOverlap(Form("%s/%s overlapping %s/%s", vol->GetName(),path.Data(),vol->GetName(),path1.Data()),
                                  node01->GetVolume(),node02->GetVolume(),node01->GetMatrix(),node02->GetMatrix(),kTRUE,ovlp);  
            }
         }                         
      }                
      node01->SetOverlaps(0,0);
   }
}

//______________________________________________________________________________
void TGeoChecker::PrintOverlaps() const
{
// Print the current list of overlaps held by the manager class.
   TIter next(fGeoManager->GetListOfOverlaps());
   TGeoOverlap *ov;
   printf("=== Overlaps for %s ===\n", fGeoManager->GetName());
   while ((ov=(TGeoOverlap*)next())) ov->PrintInfo();
}

//______________________________________________________________________________
void TGeoChecker::CheckPoint(Double_t x, Double_t y, Double_t z, Option_t *)
{
//--- Draw point (x,y,z) over the picture of the daughers of the volume containing this point.
//   Generates a report regarding the path to the node containing this point and the distance to
//   the closest boundary.

   Double_t point[3];
   Double_t local[3];
   point[0] = x;
   point[1] = y;
   point[2] = z;
   TGeoVolume *vol = fGeoManager->GetTopVolume();
   if (fVsafe) {
      TGeoNode *old = fVsafe->GetNode("SAFETY_1");
      if (old) fVsafe->GetNodes()->RemoveAt(vol->GetNdaughters()-1);
   }   
//   if (vol != fGeoManager->GetMasterVolume()) fGeoManager->RestoreMasterVolume();
   TGeoNode *node = fGeoManager->FindNode(point[0], point[1], point[2]);
   fGeoManager->MasterToLocal(point, local);
   // get current node
   printf("===  Check current point : (%g, %g, %g) ===\n", point[0], point[1], point[2]);
   printf("  - path : %s\n", fGeoManager->GetPath());
   // get corresponding volume
   if (node) vol = node->GetVolume();
   // compute safety distance (distance to boundary ignored)
   Double_t close = fGeoManager->Safety();
   printf("Safety radius : %f\n", close);
   if (close>1E-4) {
      TGeoVolume *sph = fGeoManager->MakeSphere("SAFETY", vol->GetMedium(), 0, close, 0,180,0,360);
      sph->SetLineColor(2);
      sph->SetLineStyle(3);
      vol->AddNode(sph,1,new TGeoTranslation(local[0], local[1], local[2]));
      fVsafe = vol;
   }
   TPolyMarker3D *pm = new TPolyMarker3D();
   pm->SetMarkerColor(2);
   pm->SetMarkerStyle(8);
   pm->SetMarkerSize(0.5);
   pm->SetNextPoint(local[0], local[1], local[2]);
   if (vol->GetNdaughters()<2) fGeoManager->SetTopVisible();
   else fGeoManager->SetTopVisible(kFALSE);
   fGeoManager->SetVisLevel(1);
   if (!vol->IsVisible()) vol->SetVisibility(kTRUE);
   vol->Draw();
   pm->Draw("SAME");
   gPad->Modified();
   gPad->Update();
}  

//______________________________________________________________________________
TH2F *TGeoChecker::LegoPlot(Int_t ntheta, Double_t themin, Double_t themax,
                            Int_t nphi,   Double_t phimin, Double_t phimax,
                            Double_t /*rmin*/, Double_t /*rmax*/, Option_t *option)
{
// Generate a lego plot fot the top volume, according to option.
   TH2F *hist = new TH2F("lego", option, nphi, phimin, phimax, ntheta, themin, themax);
   
   Double_t degrad = TMath::Pi()/180.;
   Double_t theta, phi, step, matprop, x;
   Double_t start[3];
   Double_t dir[3];
   TGeoNode *startnode, *endnode;
   Int_t i;  // loop index for phi
   Int_t j;  // loop index for theta
   Int_t ntot = ntheta * nphi;
   Int_t n10 = ntot/10;
   Int_t igen = 0, iloop=0;
   printf("=== Lego plot sph. => nrays=%i\n", ntot);
   for (i=1; i<=nphi; i++) {
      for (j=1; j<=ntheta; j++) {
         igen++;
         if (n10) {
            if ((igen%n10) == 0) printf("%i percent\n", Int_t(100*igen/ntot));
         }  
         x = 0;
         theta = hist->GetYaxis()->GetBinCenter(j);
         phi   = hist->GetXaxis()->GetBinCenter(i)+1E-3;
         start[0] = start[1] = start[2] = 1E-3;
         dir[0]=TMath::Sin(theta*degrad)*TMath::Cos(phi*degrad);
         dir[1]=TMath::Sin(theta*degrad)*TMath::Sin(phi*degrad);
         dir[2]=TMath::Cos(theta*degrad);
         fGeoManager->InitTrack(&start[0], &dir[0]);
         startnode = fGeoManager->GetCurrentNode();
         if (fGeoManager->IsOutside()) startnode=0;
         if (startnode) {
            matprop = startnode->GetVolume()->GetMaterial()->GetRadLen();
         } else {
            matprop = 0.;
         }      
         fGeoManager->FindNextBoundary();
//         fGeoManager->IsStepEntering();
         // find where we end-up
         endnode = fGeoManager->Step();
         step = fGeoManager->GetStep();
         while (step<1E10) {
            // now see if we can make an other step
            iloop=0;
            while (!fGeoManager->IsEntering()) {
               iloop++;
               fGeoManager->SetStep(1E-3);
               step += 1E-3;
               endnode = fGeoManager->Step();
            }
            if (iloop>1000) printf("%i steps\n", iloop);   
            if (matprop>0) {
               x += step/matprop;
            }   
            if (endnode==0 && step>1E10) break;
            // generate an extra step to cross boundary
            startnode = endnode;    
            if (startnode) {
               matprop = startnode->GetVolume()->GetMaterial()->GetRadLen();
            } else {
               matprop = 0.;
            }      
            
            fGeoManager->FindNextBoundary();
            endnode = fGeoManager->Step();
            step = fGeoManager->GetStep();
         }
         hist->Fill(phi, theta, x); 
      }
   }
   return hist;           
}

//______________________________________________________________________________
void TGeoChecker::RandomPoints(TGeoVolume *vol, Int_t npoints, Option_t *option)
{
// Draw random points in the bounding box of a volume.
   if (!vol) return;
   gRandom = new TRandom3();
   vol->VisibleDaughters(kTRUE);
   vol->Draw();
   TString opt = option;
   opt.ToLower();
   TObjArray *pm = new TObjArray(128);
   TPolyMarker3D *marker = 0;
   const TGeoShape *shape = vol->GetShape();
   TGeoBBox *box = (TGeoBBox *)shape;
   Double_t dx = box->GetDX();
   Double_t dy = box->GetDY();
   Double_t dz = box->GetDZ();
   Double_t ox = (box->GetOrigin())[0];
   Double_t oy = (box->GetOrigin())[1];
   Double_t oz = (box->GetOrigin())[2];
   Double_t *xyz = new Double_t[3];
   printf("Random box : %f, %f, %f\n", dx, dy, dz);
   TGeoNode *node = 0;
   printf("Start... %i points\n", npoints);
   Int_t i=0;
   Int_t igen=0;
   Int_t ic = 0;
   Int_t n10 = npoints/10;
   Double_t ratio=0;
   while (igen<npoints) {
      xyz[0] = ox-dx+2*dx*gRandom->Rndm();
      xyz[1] = oy-dy+2*dy*gRandom->Rndm();
      xyz[2] = oz-dz+2*dz*gRandom->Rndm();
      fGeoManager->SetCurrentPoint(xyz);
      igen++;
      if (n10) {
         if ((igen%n10) == 0) printf("%i percent\n", Int_t(100*igen/npoints));
      }  
      node = fGeoManager->FindNode();
      if (!node) continue;
      if (!node->IsOnScreen()) continue;
      // draw only points in overlapping/non-overlapping volumes
      if (opt.Contains("many") && !node->IsOverlapping()) continue;
      if (opt.Contains("only") && node->IsOverlapping()) continue;
      ic = node->GetColour();
      if ((ic<0) || (ic>=128)) ic = 1;
      marker = (TPolyMarker3D*)pm->At(ic);
      if (!marker) {
         marker = new TPolyMarker3D();
         marker->SetMarkerColor(ic);
//         marker->SetMarkerStyle(8);
//         marker->SetMarkerSize(0.4);
         pm->AddAt(marker, ic);
      }
      marker->SetNextPoint(xyz[0], xyz[1], xyz[2]);
      i++;
   }
   printf("Number of visible points : %i\n", i);
   ratio = (Double_t)i/(Double_t)igen;
   printf("efficiency : %g\n", ratio);
   for (Int_t m=0; m<128; m++) {
      marker = (TPolyMarker3D*)pm->At(m);
      if (marker) marker->Draw("SAME");
   }
   fGeoManager->GetTopVolume()->VisibleDaughters(kFALSE);
   printf("---Daughters of %s made invisible.\n", fGeoManager->GetTopVolume()->GetName());
   printf("---Make them visible with : gGeoManager->GetTopVolume()->VisibleDaughters();\n");
   delete pm;
   delete [] xyz;
}   

//______________________________________________________________________________
void TGeoChecker::RandomRays(Int_t nrays, Double_t startx, Double_t starty, Double_t startz)
{
// Randomly shoot nrays from point (startx,starty,startz) and plot intersections 
// with surfaces for current top node.
   TObjArray *pm = new TObjArray(128);
   TPolyLine3D *line = 0;
   gRandom = new TRandom3();
   TGeoVolume *vol=fGeoManager->GetTopVolume();
   vol->VisibleDaughters(kTRUE);

   Double_t start[3];
   Double_t dir[3];
   Int_t istep= 0;
   const Double_t *point = fGeoManager->GetCurrentPoint();
   vol->Draw();
   printf("Start... %i rays\n", nrays);
   TGeoNode *startnode, *endnode;
   Bool_t vis1,vis2;
   Int_t i=0;
   Int_t ipoint;
   Int_t itot=0;
   Int_t n10=nrays/10;
   Double_t theta,phi, step;
   while (itot<nrays) {
      itot++;
      ipoint = 0;
      if (n10) {
         if ((itot%n10) == 0) printf("%i percent\n", Int_t(100*itot/nrays));
      }
      start[0] = startx;
      start[1] = starty;
      start[2] = startz;
      phi = 2*TMath::Pi()*gRandom->Rndm();
      theta= TMath::ACos(1.-2.*gRandom->Rndm());
      dir[0]=TMath::Sin(theta)*TMath::Cos(phi);
      dir[1]=TMath::Sin(theta)*TMath::Sin(phi);
      dir[2]=TMath::Cos(theta);
      startnode = fGeoManager->InitTrack(start[0],start[1],start[2], dir[0],dir[1],dir[2]);
      line = 0;
      if (fGeoManager->IsOutside()) startnode=0;
      vis1 = (startnode)?(startnode->IsOnScreen()):kFALSE;
      if (vis1) {
         line = new TPolyLine3D(2);
         line->SetLineColor(startnode->GetVolume()->GetLineColor());
         line->SetPoint(ipoint++, startx, starty, startz);
         i++;
         pm->Add(line);
      }
      // find where we end-up
      fGeoManager->FindNextBoundary();
      step = fGeoManager->GetStep();
      endnode = fGeoManager->Step();
      vis2 = (endnode)?(endnode->IsOnScreen()):kFALSE;
      while (step<1E10) {
         istep = 0;
         while (!fGeoManager->IsEntering()) {
            istep++;
            if (istep>1E4) break;
            fGeoManager->SetStep(1E-3);
            endnode = fGeoManager->Step();
            step += 1E-3;
         }      
         if (istep>1E4) break;
//         if (istep) printf("ADDED : %f (%i steps)\n", istep*1E-3, istep);
         vis2 = (endnode)?(endnode->IsOnScreen()):kFALSE;
         if (ipoint>0) {
         // old visible node had an entry point -> finish segment
            line->SetPoint(ipoint, point[0], point[1], point[2]);
            ipoint = 0;
            line   = 0;
         }
         if (vis2) {
            // create new segment
            line = new TPolyLine3D(2);   
            line->SetLineColor(endnode->GetVolume()->GetLineColor());
            line->SetPoint(ipoint++, point[0], point[1], point[2]);
            i++;
            pm->Add(line);
         } 
         // now see if we can make an other step
         if (endnode==0 && step>1E10) break;
         // generate an extra step to cross boundary
         startnode = endnode;    
         fGeoManager->FindNextBoundary();
         step = fGeoManager->GetStep();
         endnode = fGeoManager->Step();
      }      
   }   
   // draw all segments
   for (Int_t m=0; m<pm->GetEntriesFast(); m++) {
      line = (TPolyLine3D*)pm->At(m);
      if (line) line->Draw("SAME");
   }
   printf("number of segments : %i\n", i);
   fGeoManager->GetTopVolume()->VisibleDaughters(kFALSE);
   printf("---Daughters of %s made invisible.\n", fGeoManager->GetTopVolume()->GetName());
   printf("---Make them visible with : gGeoManager->GetTopVolume()->VisibleDaughters();\n");
   delete pm;
}

//______________________________________________________________________________
TGeoNode *TGeoChecker::SamplePoints(Int_t npoints, Double_t &dist, Double_t epsil,
                                    const char* g3path)
{
// shoot npoints randomly in a box of 1E-5 arround current point.
// return minimum distance to points outside
   // make sure that path to current node is updated
   // get the response of tgeo
   TGeoNode *node = fGeoManager->FindNode();
   TGeoNode *nodegeo = 0;
   TGeoNode *nodeg3 = 0;
   TGeoNode *solg3 = 0;
   if (!node) {dist=-1; return 0;}
   gRandom = new TRandom3();
   Bool_t hasg3 = kFALSE;
   if (strlen(g3path)) hasg3 = kTRUE;
   char geopath[200];
   sprintf(geopath, "%s\n", fGeoManager->GetPath());
   dist = 1E10;
   TString common = "";
   // cd to common path
   Double_t point[3];
   Double_t closest[3];
   TGeoNode *node1 = 0;
   TGeoNode *node_close = 0;
   dist = 1E10;
   Double_t dist1 = 0;
   // initialize size of random box to epsil
   Double_t eps[3];
   eps[0] = epsil; eps[1]=epsil; eps[2]=epsil;
   const Double_t *pointg = fGeoManager->GetCurrentPoint();
   if (hasg3) {
      TString spath = geopath;
      TString name = "";
      Int_t index=0;
      while (index>=0) {
         index = spath.Index("/", index+1);
         if (index>0) {
            name = spath(0, index);
            if (strstr(g3path, name.Data())) {
               common = name;
               continue;
            } else break;
         }
      }
      // if g3 response was given, cd to common path
      if (strlen(common.Data())) {
         while (strcmp(fGeoManager->GetPath(), common.Data()) && fGeoManager->GetLevel()) {
            nodegeo = fGeoManager->GetCurrentNode();
            fGeoManager->CdUp();
         }
         fGeoManager->cd(g3path);
         solg3 = fGeoManager->GetCurrentNode();
         while (strcmp(fGeoManager->GetPath(), common.Data()) && fGeoManager->GetLevel()) {
            nodeg3 = fGeoManager->GetCurrentNode();
            fGeoManager->CdUp();
         }
         if (!nodegeo) return 0;
         if (!nodeg3) return 0;
         fGeoManager->cd(common.Data());
         fGeoManager->MasterToLocal(fGeoManager->GetCurrentPoint(), &point[0]);
         Double_t xyz[3], local[3];
         for (Int_t i=0; i<npoints; i++) {
            xyz[0] = point[0] - eps[0] + 2*eps[0]*gRandom->Rndm();
            xyz[1] = point[1] - eps[1] + 2*eps[1]*gRandom->Rndm();
            xyz[2] = point[2] - eps[2] + 2*eps[2]*gRandom->Rndm();
            nodeg3->MasterToLocal(&xyz[0], &local[0]);
            if (!nodeg3->GetVolume()->Contains(&local[0])) continue;
            dist1 = TMath::Sqrt((xyz[0]-point[0])*(xyz[0]-point[0])+
                   (xyz[1]-point[1])*(xyz[1]-point[1])+(xyz[2]-point[2])*(xyz[2]-point[2]));
            if (dist1<dist) {
            // save node and closest point
               dist = dist1;
               node_close = solg3;
               // make the random box smaller
               eps[0] = TMath::Abs(point[0]-pointg[0]);
               eps[1] = TMath::Abs(point[1]-pointg[1]);
               eps[2] = TMath::Abs(point[2]-pointg[2]);
            }
         }
      }
      if (!node_close) dist = -1;
      return node_close;
   }

//   gRandom = new TRandom3();
   // save current point
   memcpy(&point[0], pointg, 3*sizeof(Double_t));
   for (Int_t i=0; i<npoints; i++) {
      // generate a random point in MARS
      fGeoManager->SetCurrentPoint(point[0] - eps[0] + 2*eps[0]*gRandom->Rndm(),
                                   point[1] - eps[1] + 2*eps[1]*gRandom->Rndm(),
                                   point[2] - eps[2] + 2*eps[2]*gRandom->Rndm());
      // check if new node is different from the old one
      if (node1!=node) {
         dist1 = TMath::Sqrt((point[0]-pointg[0])*(point[0]-pointg[0])+
                 (point[1]-pointg[1])*(point[1]-pointg[1])+(point[2]-pointg[2])*(point[2]-pointg[2]));
         if (dist1<dist) {
            dist = dist1;
            node_close = node1;
            memcpy(&closest[0], pointg, 3*sizeof(Double_t));
            // make the random box smaller
            eps[0] = TMath::Abs(point[0]-pointg[0]);
            eps[1] = TMath::Abs(point[1]-pointg[1]);
            eps[2] = TMath::Abs(point[2]-pointg[2]);
         }
      }
   }
   // restore the original point and path
   fGeoManager->FindNode(point[0], point[1], point[2]);  // really needed ?
   if (!node_close) dist=-1;
   return node_close;
}

//______________________________________________________________________________
void TGeoChecker::ShootRay(Double_t *start, Double_t dirx, Double_t diry, Double_t dirz, Double_t *array, Int_t &nelem, Int_t &dim, Double_t *endpoint) const
{
// Shoot one ray from start point with direction (dirx,diry,dirz). Fills input array
// with points just after boundary crossings.
//   Int_t array_dimension = 3*dim;
   nelem = 0;
   Int_t istep = 0;
   if (!dim) {
      printf("empty input array\n");
      return;
   }   
//   fGeoManager->CdTop();
   const Double_t *point = fGeoManager->GetCurrentPoint();
   TGeoNode *endnode;
   Bool_t is_entering;
   Double_t step, forward;
   Double_t dir[3];
   dir[0] = dirx;
   dir[1] = diry;
   dir[2] = dirz;
   fGeoManager->InitTrack(start, &dir[0]);
   fGeoManager->GetCurrentNode();
//   printf("Start : (%f,%f,%f)\n", point[0], point[1], point[2]);
   fGeoManager->FindNextBoundary();
   step = fGeoManager->GetStep();
//   printf("---next : at step=%f\n", step);
   if (step>1E10) return;
   endnode = fGeoManager->Step();
   is_entering = fGeoManager->IsEntering();
   while (step<1E10) {
      if (endpoint) {
         forward = dirx*(endpoint[0]-point[0])+diry*(endpoint[1]-point[1])+dirz*(endpoint[2]-point[2]);
         if (forward<1E-3) {
//            printf("exit : Passed start point. nelem=%i\n", nelem); 
            return;
         }
      }
      if (is_entering) {
         if (nelem>=dim) {
            Double_t *temparray = new Double_t[3*(dim+20)];
            memcpy(temparray, array, 3*dim*sizeof(Double_t));
            delete [] array;
            array = temparray;
                  dim += 20;
         }
         memcpy(&array[3*nelem], point, 3*sizeof(Double_t)); 
//         printf("%i (%f, %f, %f) step=%f\n", nelem, point[0], point[1], point[2], step);
         nelem++; 
      } else {
         if (endnode==0 && step>1E10) {
//            printf("exit : NULL endnode. nelem=%i\n", nelem); 
            return;
         }    
         if (!fGeoManager->IsEntering()) {
//            if (startnode) printf("stepping %f from (%f, %f, %f) inside %s...\n", step,point[0], point[1], point[2], startnode->GetName());
//            else printf("stepping %f from (%f, %f, %f) OUTSIDE...\n", step,point[0], point[1], point[2]);
            istep = 0;
         }    
         while (!fGeoManager->IsEntering()) {
            istep++;
            if (istep>1E3) {
//               Error("ShootRay", "more than 1000 steps. Step was %f", step);
               nelem = 0;
               return;
            }   
            fGeoManager->SetStep(1E-5);
            endnode = fGeoManager->Step();
         }
         if (istep>0) printf("%i steps\n", istep);   
         if (nelem>=dim) {
            Double_t *temparray = new Double_t[3*(dim+20)];
            memcpy(temparray, array, 3*dim*sizeof(Double_t));
            delete [] array;
            array = temparray;
                  dim += 20;
         }
         memcpy(&array[3*nelem], point, 3*sizeof(Double_t)); 
//         if (endnode) printf("%i (%f, %f, %f) step=%f\n", nelem, point[0], point[1], point[2], step);
         nelem++;   
         is_entering = kTRUE;
      }
      fGeoManager->FindNextBoundary();
      step = fGeoManager->GetStep();
//      printf("---next at step=%f\n", step);
      endnode = fGeoManager->Step();
      is_entering = fGeoManager->IsEntering();
   }
//   printf("exit : INFINITE step. nelem=%i\n", nelem);
}

//______________________________________________________________________________
void TGeoChecker::Test(Int_t npoints, Option_t *option)
{
   // Check time of finding "Where am I" for n points.
   gRandom= new TRandom3();
   Bool_t recheck = !strcmp(option, "RECHECK");
   if (recheck) printf("RECHECK\n");
   const TGeoShape *shape = fGeoManager->GetTopVolume()->GetShape();
   Double_t dx = ((TGeoBBox*)shape)->GetDX();
   Double_t dy = ((TGeoBBox*)shape)->GetDY();
   Double_t dz = ((TGeoBBox*)shape)->GetDZ();
   Double_t ox = (((TGeoBBox*)shape)->GetOrigin())[0];
   Double_t oy = (((TGeoBBox*)shape)->GetOrigin())[1];
   Double_t oz = (((TGeoBBox*)shape)->GetOrigin())[2];
   Double_t *xyz = new Double_t[3*npoints];
   TStopwatch *timer = new TStopwatch();
   printf("Random box : %f, %f, %f\n", dx, dy, dz);
   timer->Start(kFALSE);
   Int_t i;
   for (i=0; i<npoints; i++) {
      xyz[3*i] = ox-dx+2*dx*gRandom->Rndm();
      xyz[3*i+1] = oy-dy+2*dy*gRandom->Rndm();
      xyz[3*i+2] = oz-dz+2*dz*gRandom->Rndm();
   }
   timer->Stop();
   printf("Generation time :\n");
   timer->Print();
   timer->Reset();
   TGeoNode *node, *node1;
   printf("Start... %i points\n", npoints);
   timer->Start(kFALSE);
   for (i=0; i<npoints; i++) {
      fGeoManager->SetCurrentPoint(xyz+3*i);
      if (recheck) fGeoManager->CdTop();
      node = fGeoManager->FindNode();
      if (recheck) {
         node1 = fGeoManager->FindNode();
         if (node1 != node) {
            printf("Difference for x=%g y=%g z=%g\n", xyz[3*i], xyz[3*i+1], xyz[3*i+2]);
            printf(" from top : %s\n", node->GetName());
            printf(" redo     : %s\n", fGeoManager->GetPath());
         }
      }
   }
   timer->Stop();
   timer->Print();
   delete [] xyz;
   delete timer;
}

//______________________________________________________________________________
void TGeoChecker::TestOverlaps(const char* path)
{
//--- Geometry overlap checker based on sampling. 
   if (fGeoManager->GetTopVolume()!=fGeoManager->GetMasterVolume()) fGeoManager->RestoreMasterVolume();
   printf("Checking overlaps for path :\n");
   if (!fGeoManager->cd(path)) return;
   TGeoNode *checked = fGeoManager->GetCurrentNode();
   checked->InspectNode();
   // shoot 1E4 points in the shape of the current volume
   gRandom= new TRandom3();
   Int_t npoints = 1000000;
   Double_t big = 1E6;
   Double_t xmin = big;
   Double_t xmax = -big;
   Double_t ymin = big;
   Double_t ymax = -big;
   Double_t zmin = big;
   Double_t zmax = -big;
   TObjArray *pm = new TObjArray(128);
   TPolyMarker3D *marker = 0;
   TPolyMarker3D *markthis = new TPolyMarker3D();
   markthis->SetMarkerColor(5);
   TNtuple *ntpl = new TNtuple("ntpl","random points","x:y:z");
   TGeoShape *shape = fGeoManager->GetCurrentNode()->GetVolume()->GetShape();
   Double_t *point = new Double_t[3];
   Double_t dx = ((TGeoBBox*)shape)->GetDX();
   Double_t dy = ((TGeoBBox*)shape)->GetDY();
   Double_t dz = ((TGeoBBox*)shape)->GetDZ();
   Double_t ox = (((TGeoBBox*)shape)->GetOrigin())[0];
   Double_t oy = (((TGeoBBox*)shape)->GetOrigin())[1];
   Double_t oz = (((TGeoBBox*)shape)->GetOrigin())[2];
   Double_t *xyz = new Double_t[3*npoints];
   Int_t i=0;
   printf("Generating %i points inside %s\n", npoints, fGeoManager->GetPath());
   while (i<npoints) {
      point[0] = ox-dx+2*dx*gRandom->Rndm();
      point[1] = oy-dy+2*dy*gRandom->Rndm();
      point[2] = oz-dz+2*dz*gRandom->Rndm();
      if (!shape->Contains(point)) continue;
      // convert each point to MARS
//      printf("local  %9.3f %9.3f %9.3f\n", point[0], point[1], point[2]);
      fGeoManager->LocalToMaster(point, &xyz[3*i]);
//      printf("master %9.3f %9.3f %9.3f\n", xyz[3*i], xyz[3*i+1], xyz[3*i+2]);
      xmin = TMath::Min(xmin, xyz[3*i]);
      xmax = TMath::Max(xmax, xyz[3*i]);
      ymin = TMath::Min(ymin, xyz[3*i+1]);
      ymax = TMath::Max(ymax, xyz[3*i+1]);
      zmin = TMath::Min(zmin, xyz[3*i+2]);
      zmax = TMath::Max(zmax, xyz[3*i+2]);
      i++;
   }
   delete [] point;
   ntpl->Fill(xmin,ymin,zmin);
   ntpl->Fill(xmax,ymin,zmin);
   ntpl->Fill(xmin,ymax,zmin);
   ntpl->Fill(xmax,ymax,zmin);
   ntpl->Fill(xmin,ymin,zmax);
   ntpl->Fill(xmax,ymin,zmax);
   ntpl->Fill(xmin,ymax,zmax);
   ntpl->Fill(xmax,ymax,zmax);
   ntpl->Draw("z:y:x");

   // shoot the poins in the geometry
   TGeoNode *node;
   TString cpath;
   Int_t ic=0;
   TObjArray *overlaps = new TObjArray();
   printf("using FindNode...\n");
   for (Int_t j=0; j<npoints; j++) {
      // always start from top level (testing only)
      fGeoManager->CdTop();
      fGeoManager->SetCurrentPoint(&xyz[3*j]);
      node = fGeoManager->FindNode();
      cpath = fGeoManager->GetPath();
      if (cpath.Contains(path)) {
         markthis->SetNextPoint(xyz[3*j], xyz[3*j+1], xyz[3*j+2]);
         continue;
      }
      // current point is found in an overlapping node
      if (!node) ic=128;
      else ic = node->GetVolume()->GetLineColor();
      if (ic >= 128) ic = 0;
      marker = (TPolyMarker3D*)pm->At(ic);
      if (!marker) {
         marker = new TPolyMarker3D();
         marker->SetMarkerColor(ic);
         pm->AddAt(marker, ic);
      }
      // draw the overlapping point
      marker->SetNextPoint(xyz[3*j], xyz[3*j+1], xyz[3*j+2]);
      if (node) {
         if (overlaps->IndexOf(node) < 0) overlaps->Add(node);
      }
   }
   // draw all overlapping points
   for (Int_t m=0; m<128; m++) {
      marker = (TPolyMarker3D*)pm->At(m);
//      if (marker) marker->Draw("SAME");
   }
   markthis->Draw("SAME");
   if (gPad) gPad->Update();
   // display overlaps
   if (overlaps->GetEntriesFast()) {
      printf("list of overlapping nodes :\n");
      for (i=0; i<overlaps->GetEntriesFast(); i++) {
         node = (TGeoNode*)overlaps->At(i);
         if (node->IsOverlapping()) printf("%s  MANY\n", node->GetName());
         else printf("%s  ONLY\n", node->GetName());
      }
   } else printf("No overlaps\n");
   delete ntpl;
   delete pm;
   delete [] xyz;
   delete overlaps;
}

//______________________________________________________________________________
Double_t TGeoChecker::Weight(Double_t precision, Option_t *option)
{
// Estimate weight of top level volume with a precision SIGMA(W)/W
// better than PRECISION. Option can be "v" - verbose (default).
   TList *matlist = fGeoManager->GetListOfMaterials();
   Int_t nmat = matlist->GetSize();
   if (!nmat) return 0;
   Int_t *nin = new Int_t[nmat];
   memset(nin, 0, nmat*sizeof(Int_t));
   gRandom = new TRandom3();
   TString opt = option;
   opt.ToLower();
   Bool_t isverbose = opt.Contains("v");
   TGeoBBox *box = (TGeoBBox *)fGeoManager->GetTopVolume()->GetShape();
   Double_t dx = box->GetDX();
   Double_t dy = box->GetDY();
   Double_t dz = box->GetDZ();
   Double_t ox = (box->GetOrigin())[0];
   Double_t oy = (box->GetOrigin())[1];
   Double_t oz = (box->GetOrigin())[2];
   Double_t x,y,z;
   TGeoNode *node;
   TGeoMaterial *mat;
   Double_t vbox = 0.000008*dx*dy*dz; // m3
   Bool_t end = kFALSE;
   Double_t weight=0, sigma, eps, dens;
   Double_t eps0=1.;
   Int_t indmat;
   Int_t igen=0;
   Int_t iin = 0;
   while (!end) {
      x = ox-dx+2*dx*gRandom->Rndm();
      y = oy-dy+2*dy*gRandom->Rndm();
      z = oz-dz+2*dz*gRandom->Rndm();
      node = fGeoManager->FindNode(x,y,z);
      igen++;
      if (!node) continue;
      mat = node->GetVolume()->GetMedium()->GetMaterial();
      indmat = mat->GetIndex();
      if (indmat<0) continue;
      nin[indmat]++;
      iin++;
      if ((iin%100000)==0 || igen>1E8) {
         weight = 0;
         sigma = 0;
         for (indmat=0; indmat<nmat; indmat++) {
            mat = (TGeoMaterial*)matlist->At(indmat);
            dens = mat->GetDensity(); //  [g/cm3]
            if (dens<1E-2) dens=0;
            dens *= 1000.;            // [kg/m3]
            weight += dens*Double_t(nin[indmat]);
            sigma  += dens*dens*nin[indmat];
         }
               sigma = TMath::Sqrt(sigma);
               eps = sigma/weight;
               weight *= vbox/Double_t(igen);
               sigma *= vbox/Double_t(igen);
               if (eps<precision || igen>1E8) {
                  if (isverbose) {
                     printf("=== Weight of %s : %g +/- %g [kg]\n", 
                            fGeoManager->GetTopVolume()->GetName(), weight, sigma);
                  }
                  end = kTRUE;                      
               } else {
                  if (isverbose && eps<0.5*eps0) {
                     printf("%8dK: %14.7g kg  %g %%\n", 
                       igen/1000, weight, eps*100);
               eps0 = eps;
            } 
         }
      }
   }      
   delete [] nin;
   return weight;
}
//______________________________________________________________________________
Double_t TGeoChecker::CheckVoxels(TGeoVolume *vol, TGeoVoxelFinder *voxels, Double_t *xyz, Int_t npoints)
{
// count voxel timing
   TStopwatch timer;
   Double_t time;
   TGeoShape *shape = vol->GetShape();
   TGeoNode *node;
   TGeoMatrix *matrix;
   Double_t *point;
   Double_t local[3];
   Int_t *checklist;
   Int_t ncheck;

   timer.Start();
   for (Int_t i=0; i<npoints; i++) {
      point = xyz + 3*i;
      if (!shape->Contains(point)) continue;
      checklist = voxels->GetCheckList(point, ncheck);
      if (!checklist) continue;
      if (!ncheck) continue;
      for (Int_t id=0; id<ncheck; id++) {
         node = vol->GetNode(checklist[id]);
         matrix = node->GetMatrix();
         matrix->MasterToLocal(point, &local[0]);
         if (node->GetVolume()->GetShape()->Contains(&local[0])) break;
      }   
   }
   time = timer.CpuTime();
   return time;
}   

//______________________________________________________________________________
Bool_t TGeoChecker::TestVoxels(TGeoVolume * /*vol*/, Int_t /*npoints*/)
{
// Returns optimal voxelization type for volume vol.
//   kFALSE - cartesian
//   kTRUE  - cylindrical
   return kFALSE;
}
