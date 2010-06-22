// @(#)root/proofx:$Id:$
// Author:

/*************************************************************************
 * Copyright (C) 1995-2005, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TSelHist
#define ROOT_TSelHist

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TSelHist                                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TSelector
#include <TSelector.h>
#endif

#ifndef ROOT_TProofBenchRun
#include "TProofBenchRun.h"
#endif

class TH1F;
class TH2F;
class TH3F;
class TRandom3;
class TCanvas;

class TSelHist : public TSelector {
public :

   // Specific members
   TProofBenchRun::EHistType	    fHistType;
   Int_t            fNHists;
   Bool_t 	    fDraw;
   TH1F           **fHist1D;//[fNHists]
   TH2F           **fHist2D;//[fNHists]
   TH3F           **fHist3D;//[fNHists]
   TRandom3        *fRandom;
   TCanvas	*fCHist1D;
   TCanvas	*fCHist2D;
   TCanvas	*fCHist3D;

   TSelHist();
   virtual ~TSelHist();
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual Bool_t  Process(Long64_t entry);
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();
   virtual void    Print(Option_t* option = "") const;

   ClassDef(TSelHist,0);
};

#endif
