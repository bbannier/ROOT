// @(#)root/eve:$Id$
// Author: Matevz Tadel 2007

/*************************************************************************
 * Copyright (C) 1995-2007, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TEveCaloData.h"
#include "TEveCalo.h"

#include "TAxis.h"
#include "THStack.h"
#include "TH2.h"
#include "TMath.h"
#include "TList.h"

#include <cassert>

//==============================================================================
// TEveCaloData
//==============================================================================

//______________________________________________________________________________
//
//  A central manager for calorimeter event data. It provides a list of
//  cells within requested phi and eta range.
//

ClassImp(TEveCaloData);

//______________________________________________________________________________
TEveCaloData::TEveCaloData():
   TEveRefBackPtr(),

   fEtaAxis(0),
   fPhiAxis(0),

   fMaxValEt(0),
   fMaxValE(0),

   fEps(0)
{
   // Constructor.
}

//______________________________________________________________________________
void TEveCaloData::SetSliceThreshold(Int_t slice, Float_t val)
{
   // Set threshold for given slice.

   fSliceInfos[slice].fThreshold = val;
   InvalidateUsersCellIdCache();
}

//______________________________________________________________________________
Float_t TEveCaloData::GetSliceThreshold(Int_t slice) const
{
   // Get threshold for given slice.

   return fSliceInfos[slice].fThreshold;
}

//______________________________________________________________________________
void TEveCaloData::SetSliceColor(Int_t slice, Color_t col)
{
   // Set color for given slice.

   fSliceInfos[slice].fColor = col;
   StampBackPtrElements(TEveElement::kCBObjProps);
}

//______________________________________________________________________________
Color_t TEveCaloData::GetSliceColor(Int_t slice) const
{
   // Get color for given slice.

   return fSliceInfos[slice].fColor;
}

//______________________________________________________________________________
void TEveCaloData::InvalidateUsersCellIdCache()
{
   // Invalidate cell ids cache on back ptr references.

   TEveCaloViz* calo;
   std::list<TEveElement*>::iterator i = fBackRefs.begin();
   while (i != fBackRefs.end())
   {
      calo = dynamic_cast<TEveCaloViz*>(*i);
      calo->InvalidateCellIdCache();
      calo->StampObjProps();
      ++i;
   }
}

//______________________________________________________________________________
void TEveCaloData::DataChanged()
{
   // Tell users (TEveCaloViz instances using this data) that data
   // has changed and they should update the limits/scales etc.
   // This is done by calling TEveCaloViz::DataChanged().

   TEveCaloViz* calo;
   std::list<TEveElement*>::iterator i = fBackRefs.begin();
   while (i != fBackRefs.end())
   {
      calo = dynamic_cast<TEveCaloViz*>(*i);
      calo->DataChanged();
      calo->StampObjProps();
      ++i;
   }
}

//______________________________________________________________________________
Float_t TEveCaloData::EtaToTheta(Float_t eta)
{
   using namespace TMath;

   if (eta < 0)
      return Pi() - 2*ATan(Exp(- Abs(eta)));
   else
      return 2*ATan(Exp(- Abs(eta)));
}


//------------------------------------------------------------------------------
// TEveCaloData::CellGeom_t
//------------------------------------------------------------------------------

//______________________________________________________________________________
void TEveCaloData::CellGeom_t::Configure(Float_t etaMin, Float_t etaMax, Float_t phiMin, Float_t phiMax)
{
   fEtaMin = etaMin;
   fEtaMax = etaMax;

   fPhiMin = phiMin;
   fPhiMax = phiMax;

   fThetaMin = EtaToTheta(fEtaMax);
   fThetaMax = EtaToTheta(fEtaMin);
}


//______________________________________________________________________________
void TEveCaloData::CellGeom_t::Dump() const
{
   // Print member data.

   printf("%f, %f %f, %f \n", fEtaMin, fEtaMax, fPhiMin, fPhiMax);
}

//------------------------------------------------------------------------------
// TEveCaloData::CellData_t
//------------------------------------------------------------------------------

//______________________________________________________________________________
Float_t TEveCaloData::CellData_t::Value(Bool_t isEt) const
{
   // Return energy value associated with the cell, usually Et.
   // If isEt is false it is transformed into energy E.

   if (isEt)
      return fValue;
   else
      return TMath::Abs(fValue/TMath::Cos(Theta()));
}

//______________________________________________________________________________
void TEveCaloData::CellData_t::Dump() const
{
   // Print member data.

   printf("%f, %f %f, %f \n", fEtaMin, fEtaMax, fPhiMin, fPhiMax);
}


//==============================================================================
// TEveCaloDataVec
//==============================================================================

//______________________________________________________________________________
//
// Blabla.

ClassImp(TEveCaloDataVec);

//______________________________________________________________________________
TEveCaloDataVec::TEveCaloDataVec(Int_t nslices):
   TEveCaloData(),

   fTower(0),
   fEtaMin( 10),
   fEtaMax(-10),
   fPhiMin( 10),
   fPhiMax(-10)
{
   // Constructor.

   fSliceInfos.assign(nslices, SliceInfo_t());

   fSliceVec.assign(nslices, std::vector<Float_t> ());
}

//______________________________________________________________________________
TEveCaloDataVec::~TEveCaloDataVec()
{
   // Destructor.

   if (fEtaAxis) delete fEtaAxis;
   if (fPhiAxis) delete fPhiAxis;
}

//______________________________________________________________________________
Int_t TEveCaloDataVec::AddTower(Float_t etaMin, Float_t etaMax, Float_t phiMin, Float_t phiMax)
{
   // Add tower within eta/phi range.

   assert (etaMin < etaMax);
   assert (phiMin < phiMax);

   fGeomVec.push_back(CellGeom_t(etaMin, etaMax, phiMin, phiMax));

   for (vvFloat_i it=fSliceVec.begin(); it!=fSliceVec.end(); ++it)
      (*it).push_back(0);

   if (etaMin < fEtaMin) fEtaMin = etaMin;
   if (etaMax > fEtaMax) fEtaMax = etaMax;

   if (phiMin < fPhiMin) fPhiMin = phiMin;
   if (phiMax > fPhiMax) fPhiMax = phiMax;

   fTower = fGeomVec.size() - 1;
   return fTower;
}

//______________________________________________________________________________
void TEveCaloDataVec::FillSlice(Int_t slice, Float_t val)
{
   // Fill given slice in the current tower.

   fSliceVec[slice][fTower] = val;
}

//______________________________________________________________________________
void TEveCaloDataVec::GetCellList(Float_t eta, Float_t etaD,
                                  Float_t phi, Float_t phiD,
                                  TEveCaloData::vCellId_t &out) const
{
   // Get list of cell-ids for given eta/phi range. 

   using namespace TMath;

   Float_t etaMin = eta - etaD*0.5;
   Float_t etaMax = eta + etaD*0.5;

   Float_t phiMin = phi - phiD*0.5;
   Float_t phiMax = phi + phiD*0.5;

   Int_t nS = fSliceVec.size();

   Int_t tower = 0;
   Float_t fracx=0, fracy=0, frac;
   Float_t minQ, maxQ;

   for(vCellGeom_ci i=fGeomVec.begin(); i!=fGeomVec.end(); i++)
   {
      const CellGeom_t &cg = *i;
      fracx = TEveUtil::GetFraction(etaMin, etaMax, cg.fEtaMin, cg.fEtaMax);
      if (fracx > 1e-3)
      {
         minQ = cg.fPhiMin;
         maxQ = cg.fPhiMax;

         if (maxQ < phiMin)
         {
            minQ += TwoPi(); maxQ += TwoPi();
         }
         else if (minQ > phiMax)
         {
            minQ -= TwoPi(); maxQ -= TwoPi();
         }

         if (maxQ >= phiMin && minQ <= phiMax)
         {
            fracy = TEveUtil::GetFraction(phiMin, phiMax, minQ, maxQ);
            if (fracy > 1e-3)
            {
               frac = fracx*fracy;
               for (Int_t s=0; s<nS; s++)
               {
                  if (fSliceVec[s][tower] > fSliceInfos[s].fThreshold)
                     out.push_back(CellId_t(tower, s, frac));
               }
            }
         }
      }
      tower++;
   }
}

//______________________________________________________________________________
void TEveCaloDataVec::GetCellData(const TEveCaloData::CellId_t &id,
                                  TEveCaloData::CellData_t& cellData) const
{
   // Get cell geometry and value from cell ID.

   cellData.CellGeom_t::operator=( fGeomVec[id.fTower] );
   cellData.fValue = fSliceVec[id.fSlice][id.fTower]*id.fFraction;
}

//______________________________________________________________________________
void TEveCaloDataVec::DataChanged()
{
   // Update limits and notify data users.

   using namespace TMath;

   // update max E/Et values

   fMaxValE = 0;
   fMaxValEt = 0;
   Float_t sum=0, cos=0;
   //   printf("geom vec %d slices %d\n",fGeomVec.size(), fSliceVec.size() );

   for (UInt_t tw=0; tw<fGeomVec.size(); tw++)
   {
      sum=0;
      for (vvFloat_i it=fSliceVec.begin(); it!=fSliceVec.end(); ++it)
         sum += (*it)[tw];

      if (sum > fMaxValEt ) fMaxValEt=sum;

      cos = Cos(2*ATan(Exp( -Abs(fGeomVec[tw].Eta()))));
      sum /= Abs(cos);
      if (sum > fMaxValE) fMaxValE=sum;
   }

   TEveCaloData::DataChanged();
}


//==============================================================================
// TEveCaloDataHist
//==============================================================================

//______________________________________________________________________________
//
// A central manager for calorimeter data of an event written in TH2F.
// X axis is used for eta and Y axis for phi.
//

ClassImp(TEveCaloDataHist);

//______________________________________________________________________________
TEveCaloDataHist::TEveCaloDataHist():
   TEveCaloData(),

   fHStack(0)
{
   // Constructor.

   fHStack = new THStack();
   fEps    = 1e-5;
}

//______________________________________________________________________________
TEveCaloDataHist::~TEveCaloDataHist()
{
   // Destructor.

   delete fHStack;
}

//______________________________________________________________________________
void TEveCaloDataHist::DataChanged()
{ 
   // Update limits and notify data users. 

   using namespace TMath;

   // update max E/Et values
   fMaxValE  = 0;
   fMaxValEt = 0;

   if (GetNSlices() < 1) return;

   TH2  *ah = (TH2*) RefSliceInfo(0).fHist;
   fEtaAxis = ah->GetXaxis();
   fPhiAxis = ah->GetYaxis();

   for (Int_t ieta = 1; ieta <= fEtaAxis->GetNbins(); ++ieta)
   {
      Double_t eta = fEtaAxis->GetBinCenter(ieta); // conversion E/Et
      for (Int_t iphi = 1; iphi <= fPhiAxis->GetNbins(); ++iphi)
      {
         Double_t value = 0;
         for (Int_t i = 0; i < GetNSlices(); ++i)
         {
            Int_t bin = RefSliceInfo(i).fHist->GetBin(ieta, iphi);
            value += RefSliceInfo(i).fHist->GetBinContent(bin);
         }

         if (value > fMaxValEt ) fMaxValEt = value;

         Double_t cos = Cos(2*ATan(Exp(-Abs(eta))));
         value /= Abs(cos);
         if (value > fMaxValE) fMaxValE = value;
      }
   }
   TEveCaloData::DataChanged();
}

//______________________________________________________________________________
void TEveCaloDataHist::GetCellList(Float_t eta, Float_t etaD,
                                   Float_t phi, Float_t phiD,
                                   TEveCaloData::vCellId_t &out) const
{
   // Get list of cell IDs in given eta and phi range.

   using namespace TMath;

   Float_t etaMin = eta - etaD*0.5 -fEps;
   Float_t etaMax = eta + etaD*0.5 +fEps;

   Float_t phiMin = phi - phiD*0.5 -fEps;
   Float_t phiMax = phi + phiD*0.5 +fEps;

   Int_t nEta = fEtaAxis->GetNbins();
   Int_t nPhi = fPhiAxis->GetNbins();
   Int_t nSlices = GetNSlices();


   TH2   *h0  = fSliceInfos[0].fHist;
   Int_t  bin = 0;

   for (Int_t ieta = 1; ieta <= nEta; ++ieta)
   {
      if (fEtaAxis->GetBinLowEdge(ieta) >= etaMin && fEtaAxis->GetBinUpEdge(ieta) <= etaMax)
      {
         for (Int_t iphi = 1; iphi <= nPhi; ++iphi)
         {
            if (TEveUtil::IsU1IntervalContainedByMinMax
                (phiMin, phiMax, fPhiAxis->GetBinLowEdge(iphi), fPhiAxis->GetBinUpEdge(iphi)))
            {

               bin = h0->GetBin(ieta, iphi);
               for (Int_t s = 0; s < nSlices; ++s)
               {
                  if (fSliceInfos[s].fHist->GetBinContent(bin) > fSliceInfos[s].fThreshold )
                     out.push_back(TEveCaloData::CellId_t(bin, s));
               } // hist slices
            }
         } // phi bins
      }
   } // eta bins
}

//______________________________________________________________________________
void TEveCaloDataHist::GetCellData(const TEveCaloData::CellId_t &id,
                                   TEveCaloData::CellData_t& cellData) const
{
   // Get cell geometry and value from cell ID.

   TH2F* hist  = fSliceInfos[id.fSlice].fHist;

   Int_t x, y, z;
   hist->GetBinXYZ(id.fTower, x, y, z);

   cellData.fValue =  hist->GetBinContent(id.fTower);
   cellData.Configure(hist->GetXaxis()->GetBinLowEdge(x),
                      hist->GetXaxis()->GetBinUpEdge(x),
                      hist->GetYaxis()->GetBinLowEdge(y),
                      hist->GetYaxis()->GetBinUpEdge(y));
}


//______________________________________________________________________________
Int_t TEveCaloDataHist::AddHistogram(TH2F* hist)
{
   // Add new slice to calo tower. Updates cached variables fMaxValE
   // and fMaxValEt
   // Return last index in the vector of slice infos.

   fHStack->Add(hist);

   Int_t id = fSliceInfos.size();
   fSliceInfos.push_back(SliceInfo_t(hist));
   fSliceInfos[id].fName  = hist->GetName();
   fSliceInfos[id].fColor = hist->GetLineColor();
   fSliceInfos[id].fID    = id;

   DataChanged();

   return id;
}

//______________________________________________________________________________
void TEveCaloDataHist::GetEtaLimits(Double_t &min, Double_t &max) const
{
   // Get eta limits.

   min = fEtaAxis->GetXmin();
   max = fEtaAxis->GetXmax();
}

//______________________________________________________________________________
void TEveCaloDataHist::GetPhiLimits(Double_t &min, Double_t &max) const
{
   // Get phi limits.

   min = fPhiAxis->GetXmin();
   max = fPhiAxis->GetXmax();
}
