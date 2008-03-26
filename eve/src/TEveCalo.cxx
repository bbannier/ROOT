// @(#)root/eve:$Id$
// Author: Matevz Tadel 2007

/*************************************************************************
 * Copyright (C) 1995-2007, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TEveCalo.h"
#include "TEveCaloData.h"
#include "TEveProjections.h"
#include "TEveProjectionManager.h"
#include "TEveRGBAPalette.h"
#include "TEveText.h"

#include "TClass.h"
#include "TMathBase.h"
#include "TMath.h"
#include "TBuffer3D.h"
#include "TBuffer3DTypes.h"
#include "TVirtualPad.h"
#include "TVirtualViewer3D.h"

#include "TGLUtil.h"

//______________________________________________________________________________
//
// Base class for calorimeter data visualization.
// See TEveCalo2D and TEveCalo3D for concrete implementations.

ClassImp(TEveCaloViz);

//______________________________________________________________________________
TEveCaloViz::TEveCaloViz(const Text_t* n, const Text_t* t) :
   TEveElement(),
   TNamed(n, t),
   TEveProjectable(),

   fData(0),

   fEtaLowLimit(-5.f),
   fEtaHighLimit(5.f),
   fEtaMin(fEtaLowLimit),
   fEtaMax(fEtaHighLimit),

   fPhi(0.),
   fPhiRng(TMath::Pi()),

   fThreshold(0.001f),

   fBarrelRadius(-1.f),
   fEndCapPos(-1.f),

   fCellZScale(1.),

   fDefaultValue(5),
   fValueIsColor(kTRUE),
   fPalette(0),

   fCacheOK(kFALSE)
{
   // Constructor.

   SetElementName("TEveCaloViz");
}

//______________________________________________________________________________
TEveCaloViz::TEveCaloViz(TEveCaloData* data, const Text_t* n, const Text_t* t) :
   TEveElement(),
   TNamed(n, t),

   fData(0),

   fEtaLowLimit(-5.f),
   fEtaHighLimit(5.f),
   fEtaMin(fEtaLowLimit),
   fEtaMax(fEtaHighLimit),

   fPhi(0.),
   fPhiRng(TMath::Pi()),

   fThreshold(0.001f),

   fBarrelRadius(-1.f),
   fEndCapPos(-1.f),

   fCellZScale(1.),

   fDefaultValue(5),
   fValueIsColor(kTRUE),
   fPalette(0),

   fCacheOK(kFALSE)
{
   // Constructor.

   SetData(data);
}

//______________________________________________________________________________
TEveCaloViz::~TEveCaloViz()
{
   // Destructor.

   if (fPalette) fPalette->DecRefCount();
   if (fData) fData->DecRefCount();
}

//______________________________________________________________________________
Float_t TEveCaloViz::GetTransitionTheta() const
{
   // Get transition angle between barrel and end-cap cells.

   return TMath::ATan(fBarrelRadius/fEndCapPos);
}

//______________________________________________________________________________
Float_t TEveCaloViz::GetTransitionEta() const
{
   // Get transition eta between barrel and end-cap cells.

   using namespace TMath;
   Float_t t = GetTransitionTheta()*0.5f;
   return -Log(Tan(t));
}

//______________________________________________________________________________
void TEveCaloViz::SetData(TEveCaloData* data)
{
   // Set calorimeter event data.

   if (data == fData) return;
   if (fData) fData->DecRefCount();
   fData = data;
   if (fData) fData->IncRefCount();
   InvalidateCache();
}

//______________________________________________________________________________
void TEveCaloViz::AssignCaloVizParameters(TEveCaloViz* m)
{
   // Assign paramteres from given model.

   SetData(m->fData);

   fEtaMin    = m->fEtaMin;
   fEtaMax    = m->fEtaMax;
   fEtaLowLimit = m->fEtaLowLimit;
   fEtaHighLimit = m->fEtaHighLimit;

   fPhi       = m->fPhi;
   fPhiRng    = m->fPhiRng;
   fThreshold = m->fThreshold;

   fBarrelRadius = m->fBarrelRadius;
   fEndCapPos    = m->fEndCapPos;

   TEveRGBAPalette& mp = * m->fPalette;
   TEveRGBAPalette* p = new TEveRGBAPalette(mp.GetMinVal(), mp.GetMaxVal(),
                                          mp.GetInterpolate());
   p->SetDefaultColor(mp.GetDefaultColor());
   SetPalette(p);
}


//______________________________________________________________________________
void TEveCaloViz::SetPalette(TEveRGBAPalette* p)
{
   // Set TEveRGBAPalette object pointer.

   if ( fPalette == p) return;
   if (fPalette) fPalette->DecRefCount();
   fPalette = p;
   if (fPalette) fPalette->IncRefCount();
}

//______________________________________________________________________________
TEveRGBAPalette* TEveCaloViz::AssertPalette()
{
   // Make sure the TEveRGBAPalette pointer is not null.
   // If it is not set, a new one is instantiated and the range is set
   // to current min/max signal values.

   if (fPalette == 0) {
      fPalette = new TEveRGBAPalette;
   }
   return fPalette;
}
//______________________________________________________________________________
void TEveCaloViz::Paint(Option_t* /*option*/)
{
   // Paint this object. Only direct rendering is supported.

   static const TEveException eH("TEvecaloViz::Paint ");

   if ( ! fData ) {
      throw(eH, "data uninitialized.");
      return;
   }

   TBuffer3D buff(TBuffer3DTypes::kGeneric);

   // Section kCore
   buff.fID           = this;
   buff.fTransparency = 0;
   buff.SetSectionsValid(TBuffer3D::kCore);

   Int_t reqSections = gPad->GetViewer3D()->AddObject(buff);
   if (reqSections != TBuffer3D::kNone)
      Error(eH, "only direct GL rendering supported.");
}

//______________________________________________________________________________
TClass* TEveCaloViz::ProjectedClass() const
{
   // Virtual from TEveProjectable, returns TEveCalo2D class.

   return TEveCalo2D::Class();
}


//______________________________________________________________________________
void TEveCaloViz::SetupColorHeight(Float_t value, Int_t slice,
                                   Float_t &outH, Bool_t &viz) const
{
   // Set color and height for a given value and slice using TEveRGBAPalette.

   Int_t val = (Int_t) value;
   outH = GetDefaultCellHeight();
   Bool_t visible = kFALSE;

   if (fPalette->GetShowDefValue())
   {
      if (value > fPalette->GetMinVal() && value < fPalette->GetMaxVal())
      {
         TGLUtil::Color(fPalette->GetDefaultColor() + slice);
         outH *= ((value -fPalette->GetMinVal())*fData->GetNSlices()
                  / (fPalette->GetHighLimit() - fPalette->GetLowLimit()));
         visible = kTRUE;
      }
   }

   if (fPalette->GetShowDefValue() == kFALSE && fPalette->WithinVisibleRange(val))
   {
      UChar_t c[4];
      fPalette->ColorFromValue(val, c);
      TGLUtil::Color4ubv(c);
      visible = kTRUE;
   }

   viz = visible;
}

//______________________________________________________________________________
//
// Visualization of a calorimeter event data in 3D.

ClassImp(TEveCalo3D);

//______________________________________________________________________________
void TEveCalo3D::ResetCache()
{
   // Clear list of drawn cell IDs. See TEveCalo3DGL::DirectDraw().

   fCellList.clear();
}

//______________________________________________________________________________
void TEveCalo3D::ComputeBBox()
{
   // Fill bounding-box information of the base-class TAttBBox (virtual method).
   // If member 'TEveFrameBox* fFrame' is set, frame's corners are used as bbox.

   BBoxInit();

   Float_t th = GetDefaultCellHeight()*fData->GetNSlices();

   fBBox[0] = -fBarrelRadius - th;
   fBBox[1] =  fBarrelRadius + th;
   fBBox[2] =  fBBox[0];
   fBBox[3] =  fBBox[1];
   fBBox[4] = -fEndCapPos - th;
   fBBox[5] =  fEndCapPos + th;
}

//______________________________________________________________________________
//
// Visualization of a calorimeter event data in 2D.

ClassImp(TEveCalo2D);

//______________________________________________________________________________
TEveCalo2D::TEveCalo2D(const Text_t* n, const Text_t* t):
   TEveCaloViz(n, t),
   TEveProjected(),
   fOldProjectionType(TEveProjection::kPT_Unknown)
{
   // Constructor.

}

//______________________________________________________________________________
void TEveCalo2D::UpdateProjection()
{
   // This is virtual method from base-class TEveProjected.

   if (fManager->GetProjection()->GetType() != fOldProjectionType)
   {
      fCacheOK=kFALSE;
      fOldProjectionType = fManager->GetProjection()->GetType();
   }
   ComputeBBox();
}

//______________________________________________________________________________
void TEveCalo2D::SetProjection(TEveProjectionManager* mng, TEveProjectable* model)
{
   // Set projection manager and model object.

   TEveProjected::SetProjection(mng, model);
   TEveCaloViz* viz = dynamic_cast<TEveCaloViz*>(model);
   AssignCaloVizParameters(viz);
}

//______________________________________________________________________________
void TEveCalo2D::ResetCache()
{
   // Clear lists of drawn cell IDs. See TEveCalo2DGL::DirecDraw().

   for (std::vector<TEveCaloData::vCellId_t*>::iterator it = fCellLists.begin(); it != fCellLists.end(); it++)
   {
      delete *it;
   }
   fCellLists.clear();
}

//______________________________________________________________________________
void TEveCalo2D::ComputeBBox()
{
   // Fill bounding-box information of the base-class TAttBBox (virtual method).
   // If member 'TEveFrameBox* fFrame' is set, frame's corners are used as bbox.

   BBoxZero();

   Float_t x, y, z;
   Float_t th = GetDefaultCellHeight()*fData->GetNSlices();
   Float_t r  = fBarrelRadius + th;
   Float_t ze = fEndCapPos + th;

   x = r, y = 0, z = 0;
   fManager->GetProjection()->ProjectPoint(x, y, z);
   BBoxCheckPoint(x, y, z);

   x = 0, y = 0, z = 0;
   fManager->GetProjection()->ProjectPoint(x, y, z);
   BBoxCheckPoint(x, y, z);

   x = 0, y = 0, z = ze;
   fManager->GetProjection()->ProjectPoint(x, y, z);
   BBoxCheckPoint(x, y, z);

   x = 0, y = 0, z = -ze;
   fManager->GetProjection()->ProjectPoint(x, y, z);
   BBoxCheckPoint(x, y, z);

   x = 0, y = r, z = 0;
   fManager->GetProjection()->ProjectPoint(x, y, z);
   BBoxCheckPoint(x, y, z);

   x = 0, y = -r, z = 0;
   fManager->GetProjection()->ProjectPoint(x, y, z);
   BBoxCheckPoint(x, y, z);

   AssertBBoxExtents(0.1);
}


//______________________________________________________________________________
//
// Visualization of a calorimeter eta, phi histogram

ClassImp(TEveCaloLego);

//______________________________________________________________________________
TEveCaloLego::TEveCaloLego(const Text_t* n, const Text_t* t):
   TEveCaloViz(n, t),
   fFontColor(0),
   fGridColor(kGray+3)
{
   // Constructor.

   fCellZScale =0.5;
}

//______________________________________________________________________________
TEveCaloLego::TEveCaloLego(TEveCaloData* data):
   TEveCaloViz(data),
   fFontColor(0),
   fGridColor(kGray+3)
{
   // Constructor.

   SetElementName("TEveCaloLego");
   fMainColorPtr = &fGridColor;
   fCellZScale =0.5;
}

//______________________________________________________________________________
Float_t TEveCaloLego::GetDefaultCellHeight() const
{

   // Get default cell height.

   return TMath::TwoPi()/fData->GetNSlices();
}

//______________________________________________________________________________
void TEveCaloLego::ResetCache()
{
   // Clear list of drawn cell IDs. For more information see TEveCaloLegoGL:DirectDraw().

   fCellList.clear();
}

//______________________________________________________________________________
void TEveCaloLego::ComputeBBox()
{
   // Fill bounding-box information of the base-class TAttBBox (virtual method).
   // If member 'TEveFrameBox* fFrame' is set, frame's corners are used as bbox.

   BBoxInit();

   fBBox[0] = 1.2f*fEtaMin;
   fBBox[1] = 1.2f*fEtaMax;

   fBBox[2] = fPhi - 1.2f*fPhiRng;
   fBBox[3] = fPhi + 1.2f*fPhiRng;

   fBBox[4] = 0;
   fBBox[5] = GetDefaultCellHeight()*fData->GetNSlices();
}
