// @(#)root/tmva $\Id$
// Author: Andreas Hoecker, Joerg Stelzer, Fredrik Tegenfeldt, Helge Voss

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : Rule                                                                  *
 *                                                                                *
 * Description:                                                                   *
 *      A class describing a 'rule cut'                                           * 
 *                                                                                *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Fredrik Tegenfeldt <Fredrik.Tegenfeldt@cern.ch> - Iowa State U., USA      *
 *                                                                                *
 * Copyright (c) 2005:                                                            *
 *      CERN, Switzerland,                                                        * 
 *      Iowa State U.                                                             *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/
#include <algorithm>
#include <list>

#include "Riostream.h"
#include "TMVA/RuleCut.h"
#include "TMVA/DecisionTree.h"

//_______________________________________________________________________
TMVA::RuleCut::RuleCut(  const std::vector< const TMVA::Node * > & nodes )
   : fCutNeve(0),
     fPurity(0),
     fNcuts(0),
     fLogger("RuleFit")
{
   // main constructor
   MakeCuts( nodes );
}

//_______________________________________________________________________
TMVA::RuleCut::RuleCut()
   : fCutNeve(0),
     fPurity(0),
     fNcuts(0),
     fLogger("RuleFit")
{
   // empty constructor
}

//_______________________________________________________________________
void TMVA::RuleCut::MakeCuts( const std::vector< const TMVA::Node * > & nodes )
{
   // Construct the cuts from the given array of nodes

   // Atleast 2 nodes are required
   UInt_t nnodes = nodes.size();
   if (nnodes<2) {
      fLogger << kWARNING << "<MakeCuts()> Empty cut created." << Endl;
      return;
   }

   // Set number of events and S/S+B in last node
   fCutNeve = dynamic_cast<const TMVA::DecisionTreeNode*>(nodes.back())->GetNEvents();
   fPurity = dynamic_cast<const TMVA::DecisionTreeNode*>(nodes.back())->GetPurity();

   // some local typedefs
   typedef std::pair<Double_t,Int_t> CutDir_t; // first is cut value, second is direction
   typedef std::pair<Int_t,CutDir_t> SelCut_t;

   // Clear vectors
   fSelector.clear();
   fCutMin.clear();
   fCutMax.clear();
   fCutDoMin.clear();
   fCutDoMax.clear();

   // Count the number of variables in cut
   // Exclude last node since that does not lead to a cut
   std::list<SelCut_t> allsel;
   Int_t    sel;
   Double_t val;
   Int_t    dir;
   const TMVA::Node *nextNode;
   for ( UInt_t i=0; i<nnodes-1; i++) {
      nextNode = nodes[i+1];
      sel = dynamic_cast<const TMVA::DecisionTreeNode*>(nodes[i])->GetSelector();
      val = dynamic_cast<const TMVA::DecisionTreeNode*>(nodes[i])->GetCutValue();
      if (nodes[i]->GetRight() == nextNode) { // val>cut
         dir = 1;
      } else if (nodes[i]->GetLeft() == nextNode) { // val<cut
         dir = -1;
      } else {
         fLogger << kFATAL << "<MakeTheRule> BUG! Should not be here - an end-node before the end!" << Endl;
         dir = 0;
      }
      allsel.push_back(SelCut_t(sel,CutDir_t(val,dir)));
   }
   // sort after the selector (first element of CutDir_t)
   allsel.sort();
   Int_t prevSel=-1;
   Int_t nsel=0;
   Bool_t firstMin=kTRUE;
   Bool_t firstMax=kTRUE;
   for ( std::list<SelCut_t>::const_iterator it = allsel.begin(); it!=allsel.end(); it++ ) {
      sel = (*it).first;
      val = (*it).second.first;
      dir = (*it).second.second;
      if (sel!=prevSel) { // a new selector!
         firstMin = kTRUE;
         firstMax = kTRUE;
         nsel++;
         fSelector.push_back(sel);
         fCutMin.resize( fSelector.size(),0);
         fCutMax.resize( fSelector.size(),0);
         fCutDoMin.resize( fSelector.size(), kFALSE);
         fCutDoMax.resize( fSelector.size(), kFALSE);
      }
      switch ( dir ) {
      case 1:
         if ((val<fCutMin[nsel-1]) || firstMin) {
            fCutMin[nsel-1]   = val;
            fCutDoMin[nsel-1] = kTRUE;
            firstMin = kFALSE;
         }
         break;
      case -1:
         if ((val>fCutMax[nsel-1]) || firstMax) {
            fCutMax[nsel-1]   = val;
            fCutDoMax[nsel-1] = kTRUE;
            firstMax = kFALSE;
         }
      default:
         break;
      }
      prevSel = sel;
   }
   fNcuts = fSelector.size();
}

//_______________________________________________________________________
Bool_t TMVA::RuleCut::GetCutRange(Int_t sel,Double_t &rmin, Double_t &rmax, Bool_t &dormin, Bool_t &dormax) const
{
   // get cut range for a given selector
   dormin=kFALSE;
   dormax=kFALSE;
   Bool_t done=kFALSE;
   Bool_t foundIt=kFALSE;
   UInt_t ind=0;
   while (!done) {
      foundIt = (Int_t(fSelector[ind])==sel);
      ind++;
      done = (foundIt || (ind==fNcuts));
   }
   if (!foundIt) return kFALSE;
   rmin = fCutMin[ind-1];
   rmax = fCutMax[ind-1];
   dormin = fCutDoMin[ind-1];
   dormax = fCutDoMax[ind-1];
   return kTRUE;
}
