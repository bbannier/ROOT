//*CMZ :  2.22/06 16/06/99  16.25.01  by  Rene Brun
//*CMZ :  2.00/10 17/07/98  14.31.02  by  Fons Rademakers
//*CMZ :  2.00/00 14/01/98  14.33.41  by  Fons Rademakers
//*CMZ :  1.00/00 17/02/97  16.44.11  by  Fons Rademakers
//*-- Author :    Rene Brun   19/08/96
//______________________________________________________________________________
//*-*-*-*-*-*-*Simple examples with histograms created/filled and saved*-*-*-*
//*-*          ========================================================
//*-*
//*-*  This program creates :
//*-*    - a one dimensional histogram
//*-*    - a two dimensional histogram
//*-*    - a profile histogram
//*-*    - an ntuple
//*-*
//*-*  These objects are filled with some random numbers and saved on a file.
//*-*
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

int hsimple();

#ifndef __CINT__
#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TNtuple.h"
#include "TRandom.h"


//______________________________________________________________________________
int main()
{
  TROOT simple("simple","Test of histogramming and I/O");
  return hsimple();
}
#endif

int hsimple()
{
  // Create a new ROOT binary machine independent file.
  // Note that this file may contain any kind of ROOT objects, histograms,
  // pictures, graphics objects, detector geometries, tracks, events, etc..
  // This file is now becoming the current directory.
  TFile hfile("hsimple.root","RECREATE","Demo ROOT file with histograms");

  // Create some histograms, a profile histogram and an ntuple
  TH1F *hpx   = new TH1F("hpx","This is the px distribution",100,-4,4);
  TH2F *hpxpy = new TH2F("hpxpy","py vs px",40,-4,4,40,-4,4);
  TProfile *hprof = new TProfile("hprof","Profile of pz versus px",100,-4,4,0,20);
  TNtuple *ntuple = new TNtuple("ntuple","Demo ntuple","px:py:pz:random:i");

  // Fill histograms randomly
  Float_t px, py, pz;
  for ( Int_t i=0; i<10000; i++) {
     gRandom->Rannor(px,py); //px and py will be two gaussian random numbers
     pz = px*px + py*py;
     Float_t random = gRandom->Rndm(1);
     hpx->Fill(px);
     hpxpy->Fill(px,py);
     hprof->Fill(px,pz);
     ntuple->Fill(px,py,pz,random,i);
  }

  // Save all objects in this file
  hfile.Write();

  // Close the file. Note that this is automatically done when you leave
  // the application.
  hfile.Close();

  return 0;
}
