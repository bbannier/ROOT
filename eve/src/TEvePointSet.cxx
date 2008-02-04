// @(#)root/eve:$Id$
// Authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/*************************************************************************
 * Copyright (C) 1995-2007, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TEvePointSet.h"

#include "TEveManager.h"
#include "TEveProjectionManager.h"
#include "TEveTrans.h"

#include "TTree.h"
#include "TTreePlayer.h"
#include "TF3.h"

#include "TColor.h"
#include "TCanvas.h"
#include "TBuffer3D.h"
#include "TBuffer3DTypes.h"
#include "TVirtualViewer3D.h"

//______________________________________________________________________________
// TEvePointSet
//
// TEvePointSet is a render-element holding a collection of 3D points with
// optional per-point TRef and an arbitrary number of integer ids (to
// be used for signal, volume-id, track-id, etc).
//
// 3D point representation is implemented in base-class TPolyMarker3D.
// Per-point TRef is implemented in base-class TPointSet3D.
//
// By using the TEvePointSelector the points and integer ids can be
// filled directly from a TTree holding the source data.
// Setting of per-point TRef's is not supported.
//
// TEvePointSet is a TEveProjectable: it can be projected by using the
// TEveProjectionManager class.

ClassImp(TEvePointSet)

//______________________________________________________________________________
TEvePointSet::TEvePointSet(Int_t n_points, ETreeVarType_e tv_type) :
   TEveElement(fMarkerColor),
   TPointSet3D(n_points),
   TEvePointSelectorConsumer(tv_type),

   fTitle          (),
   fIntIds         (0),
   fIntIdsPerPoint (0)
{
   // Constructor.

   fMarkerStyle = 20;
}

//______________________________________________________________________________
TEvePointSet::TEvePointSet(const Text_t* name, Int_t n_points, ETreeVarType_e tv_type) :
   TEveElement(fMarkerColor),
   TPointSet3D(n_points),
   TEvePointSelectorConsumer(tv_type),

   fTitle          (),
   fIntIds         (0),
   fIntIdsPerPoint (0)
{
   // Constructor.

   fMarkerStyle = 20;
   SetName(name);
}

//______________________________________________________________________________
TEvePointSet::~TEvePointSet()
{
   // Destructor.

   delete fIntIds;
}

/******************************************************************************/

//______________________________________________________________________________
const TGPicture* TEvePointSet::GetListTreeIcon() 
{ 
   //return pointset icon
   return TEveElement::fgListTreeIcons[3]; 
}

//______________________________________________________________________________
void TEvePointSet::ComputeBBox()
{
   // Override of virtual method from TAttBBox.

   TPointSet3D::ComputeBBox();
   AssertBBoxExtents(0.1);
}

//______________________________________________________________________________
void TEvePointSet::Reset(Int_t n_points, Int_t n_int_ids)
{
   // Drop all data and set-up the data structures to recive new data.
   // n_points   specifies the initial size of the arrays.
   // n_int_ids  specifies the number of integer ids per point.

   delete [] fP; fP = 0;
   fN = n_points;
   if(fN) fP = new Float_t [3*fN];
   memset(fP, 0, 3*fN*sizeof(Float_t));
   fLastPoint = -1;
   ClearIds();
   delete fIntIds; fIntIds = 0;
   fIntIdsPerPoint = n_int_ids;
   if (fIntIdsPerPoint > 0) fIntIds = new TArrayI(fIntIdsPerPoint*fN);
   ResetBBox();
}

//______________________________________________________________________________
Int_t TEvePointSet::GrowFor(Int_t n_points)
{
   // Resizes internal array to allow additional n_points to be stored.
   // Returns the old size which is also the location where one can
   // start storing new data.
   // The caller is *obliged* to fill the new point slots.

   Int_t old_size = Size();
   Int_t new_size = old_size + n_points;
   SetPoint(new_size - 1, 0, 0, 0);
   if (fIntIds)
      fIntIds->Set(fIntIdsPerPoint * new_size);
   return old_size;
}

/******************************************************************************/

//______________________________________________________________________________
inline void TEvePointSet::AssertIntIdsSize()
{
   // Assert that size of IntId array is compatible with the size of
   // the point array.

   Int_t exp_size = GetN()*fIntIdsPerPoint;
   if (fIntIds->GetSize() < exp_size)
      fIntIds->Set(exp_size);
}

//______________________________________________________________________________
Int_t* TEvePointSet::GetPointIntIds(Int_t p) const
{
   // Return a pointer to integer ids of point with index p.
   // Existence of integer id array is checked, 0 is returned if it
   // does not exist.
   // Validity of p is *not* checked.

   if (fIntIds)
      return fIntIds->GetArray() + p*fIntIdsPerPoint;
   return 0;
}

//______________________________________________________________________________
Int_t TEvePointSet::GetPointIntId(Int_t p, Int_t i) const
{
   // Return i-th integer id of point with index p.
   // Existence of integer id array is checked, kMinInt is returned if
   // it does not exist.
   // Validity of p and i is *not* checked.

   if (fIntIds)
      return * (fIntIds->GetArray() + p*fIntIdsPerPoint + i);
   return kMinInt;
}

//______________________________________________________________________________
void TEvePointSet::SetPointIntIds(Int_t* ids)
{
   // Set integer ids for the last point that was registerd (most
   // probably via TPolyMarker3D::SetNextPoint(x,y,z)).

   SetPointIntIds(fLastPoint, ids);
}

//______________________________________________________________________________
void TEvePointSet::SetPointIntIds(Int_t n, Int_t* ids)
{
   // Set integer ids for point with index n.

   if (!fIntIds) return;
   AssertIntIdsSize();
   Int_t* x = fIntIds->GetArray() + n*fIntIdsPerPoint;
   for (Int_t i=0; i<fIntIdsPerPoint; ++i)
      x[i] = ids[i];
}

/******************************************************************************/

//______________________________________________________________________________
void TEvePointSet::Paint(Option_t* /*option*/)
{
   // Paint point-set.

   static const TEveException eh("TEvePointSet::Paint ");

   if (fRnrSelf == kFALSE) return;

   TBuffer3D buff(TBuffer3DTypes::kGeneric);

   // Section kCore
   buff.fID           = this;
   buff.fColor        = GetMainColor();
   buff.fTransparency = GetMainTransparency();
   if (HasMainTrans())
      RefMainTrans().SetBuffer3D(buff);
   buff.SetSectionsValid(TBuffer3D::kCore);

   Int_t reqSections = gPad->GetViewer3D()->AddObject(buff);
   if (reqSections != TBuffer3D::kNone)
      Error(eh, "only direct GL rendering supported.");
}

/******************************************************************************/

//______________________________________________________________________________
void TEvePointSet::InitFill(Int_t subIdNum)
{
   // Initialize point-set for new filling.
   // subIdNum gives the number of integer ids that can be assigned to
   // each point.

   if (subIdNum > 0) {
      fIntIdsPerPoint = subIdNum;
      if (!fIntIds)
         fIntIds = new TArrayI(fIntIdsPerPoint*GetN());
      else
         fIntIds->Set(fIntIdsPerPoint*GetN());
   } else {
      delete fIntIds; fIntIds = 0;
      fIntIdsPerPoint = 0;
   }
}

//______________________________________________________________________________
void TEvePointSet::TakeAction(TEvePointSelector* sel)
{
   // Called from TEvePointSelector when internal arrays of the tree-selector
   // are filled up and need to be processed.
   // Virtual from TEvePointSelectorConsumer.

   static const TEveException eh("TEvePointSet::TakeAction ");

   if(sel == 0)
      throw(eh + "selector is <null>.");

   Int_t    n = sel->GetNfill();
   Int_t  beg = GrowFor(n);

   // printf("TEvePointSet::TakeAction beg=%d n=%d size=%d nsubid=%d dim=%d\n",
   //        beg, n, Size(), sel->GetSubIdNum(), sel->GetDimension());

   Double_t *vx = sel->GetV1(), *vy = sel->GetV2(), *vz = sel->GetV3();
   Float_t  *p  = fP + 3*beg;

   switch(fSourceCS) {
      case kTVT_XYZ:
         while(n-- > 0) {
            p[0] = *vx; p[1] = *vy; p[2] = *vz;
            p += 3;
            ++vx; ++vy; ++vz;
         }
         break;
      case kTVT_RPhiZ:
         while(n-- > 0) {
            p[0] = *vx * TMath::Cos(*vy); p[1] = *vx * TMath::Sin(*vy); p[2] = *vz;
            p += 3;
            ++vx; ++vy; ++vz;
         }
         break;
      default:
         throw(eh + "unknown tree variable type.");
   }

   if (fIntIds) {
      Double_t** subarr = new Double_t* [fIntIdsPerPoint];
      for (Int_t i=0; i<fIntIdsPerPoint; ++i) {
         subarr[i] = sel->GetVal(sel->GetDimension() - fIntIdsPerPoint + i);
         if (subarr[i] == 0)
            throw(eh + "sub-id array not available.");
      }
      Int_t* ids = fIntIds->GetArray() + fIntIdsPerPoint*beg;
      n = sel->GetNfill();
      while (n-- > 0) {
         for (Int_t i=0; i<fIntIdsPerPoint; ++i) {
            ids[i] = TMath::Nint(*subarr[i]);
            ++subarr[i];
         }
         ids += fIntIdsPerPoint;
      }
      delete [] subarr;
   }
}

/******************************************************************************/

//______________________________________________________________________________
TClass* TEvePointSet::ProjectedClass() const
{
   // Virtual from TEveProjectable, returns TEvePointSetProjected class.

   return TEvePointSetProjected::Class();
}

//______________________________________________________________________________
void TEvePointSet::PointSelected(Int_t id)
{
   // Virtual method of base class TPointSet3D. The fuction call is invoked with secondary selection 
   // in TPointSet3DGL.

   Emit("PointSelected(Int_t)", id);
}

/******************************************************************************/
/******************************************************************************/

//______________________________________________________________________________
// TEvePointSetArray
//
// An array of point-sets with each point-set playing a role of a bin
// in a histogram. When a new point is added to a TEvePointSetArray, an
// additional separating quantity needs to be specified: it determines
// into which TEvePointSet (bin) the point will actually be stored.
//
// By using the TEvePointSelector the points and the separating
// quantities can be filled directly from a TTree holding the source
// data.
// Setting of per-point TRef's is not supported.
//
// After the filling, the range of separating variable can be
// controlled with a slider to choose a sub-set of PointSets that are
// actually shown.
//

ClassImp(TEvePointSetArray)

//______________________________________________________________________________
TEvePointSetArray::TEvePointSetArray(const Text_t* name,
                                     const Text_t* title) :
   TEveElement(fMarkerColor),
   TNamed(name, title),

   fBins(0), fDefPointSetCapacity(128), fNBins(0), fLastBin(-1),
   fMin(0), fCurMin(0), fMax(0), fCurMax(0),
   fBinWidth(0),
   fQuantName()
{
   // Constructor.
}

//______________________________________________________________________________
TEvePointSetArray::~TEvePointSetArray()
{
   // Destructor: deletes the fBins array. Actual removal of
   // elements done by TEveElement.

   // printf("TEvePointSetArray::~TEvePointSetArray()\n");
   delete [] fBins; fBins = 0;
}

//______________________________________________________________________________
void TEvePointSetArray::Paint(Option_t* option)
{
   // Paint the subjugated TEvePointSet's.

   static const TEveException eh("TEvePointSetArray::Paint ");

   if (fRnrSelf) {
      for (List_i i=fChildren.begin(); i!=fChildren.end(); ++i) {
         if ((*i)->GetRnrSelf())
            (*i)->GetObject(eh)->Paint(option);
      }
   }
}

//______________________________________________________________________________
void TEvePointSetArray::RemoveElementLocal(TEveElement* el)
{
   // Virtual from TEveElement, provide bin management.

   for (Int_t i=0; i<fNBins; ++i) {
      if (fBins[i] == el) {
         fBins[i] = 0;
         break;
      }
   }
}

//______________________________________________________________________________
void TEvePointSetArray::RemoveElementsLocal()
{
   // Virtual from TEveElement, provide bin management.

   delete [] fBins; fBins = 0; fLastBin = -1;
}

/******************************************************************************/

//______________________________________________________________________________
void TEvePointSetArray::SetMarkerColor(Color_t tcolor)
{
   // Set marker color, propagate to children.

   static const TEveException eh("TEvePointSetArray::SetMarkerColor ");

   for (List_i i=fChildren.begin(); i!=fChildren.end(); ++i) {
      TAttMarker* m = dynamic_cast<TAttMarker*>((*i)->GetObject(eh));
      if (m && m->GetMarkerColor() == fMarkerColor)
         m->SetMarkerColor(tcolor);
   }
   TAttMarker::SetMarkerColor(tcolor);
}

//______________________________________________________________________________
void TEvePointSetArray::SetMarkerStyle(Style_t mstyle)
{
   // Set marker style, propagate to children.

   static const TEveException eh("TEvePointSetArray::SetMarkerStyle ");

   for (List_i i=fChildren.begin(); i!=fChildren.end(); ++i) {
      TAttMarker* m = dynamic_cast<TAttMarker*>((*i)->GetObject(eh));
      if (m && m->GetMarkerStyle() == fMarkerStyle)
         m->SetMarkerStyle(mstyle);
   }
   TAttMarker::SetMarkerStyle(mstyle);
}

//______________________________________________________________________________
void TEvePointSetArray::SetMarkerSize(Size_t msize)
{
   // Set marker size, propagate to children.

   static const TEveException eh("TEvePointSetArray::SetMarkerSize ");

   for (List_i i=fChildren.begin(); i!=fChildren.end(); ++i) {
      TAttMarker* m = dynamic_cast<TAttMarker*>((*i)->GetObject(eh));
      if (m && m->GetMarkerSize() == fMarkerSize)
         m->SetMarkerSize(msize);
   }
   TAttMarker::SetMarkerSize(msize);
}

/******************************************************************************/

//______________________________________________________________________________
void TEvePointSetArray::TakeAction(TEvePointSelector* sel)
{
   // Called from TEvePointSelector when internal arrays of the tree-selector
   // are filled up and need to be processed.
   // Virtual from TEvePointSelectorConsumer.

   static const TEveException eh("TEvePointSetArray::TakeAction ");

   if (sel == 0)
      throw(eh + "selector is <null>.");

   Int_t n = sel->GetNfill();

   // printf("TEvePointSetArray::TakeAction n=%d\n", n);

   Double_t *vx = sel->GetV1(), *vy = sel->GetV2(), *vz = sel->GetV3();
   Double_t *qq = sel->GetV4();

   if(qq == 0)
      throw(eh + "requires 4-d varexp.");

   switch(fSourceCS) {
      case kTVT_XYZ:
         while(n-- > 0) {
            Fill(*vx, *vy, *vz, *qq);
            ++vx; ++vy; ++vz; ++qq;
         }
         break;
      case kTVT_RPhiZ:
         while(n-- > 0) {
            Fill(*vx * TMath::Cos(*vy), *vx * TMath::Sin(*vy), *vz, *qq);
            ++vx; ++vy; ++vz; ++qq;
         }
         break;
      default:
         throw(eh + "unknown tree variable type.");
   }
}

/******************************************************************************/

//______________________________________________________________________________
void TEvePointSetArray::InitBins(const Text_t* quant_name,
                                 Int_t nbins, Double_t min, Double_t max,
                                 Bool_t addRe)
{
   // Initialize internal point-sets with given binning parameters.

   static const TEveException eh("TEvePointSetArray::InitBins ");

   if (nbins < 1) throw(eh + "nbins < 1.");
   if (min > max) throw(eh + "min > max.");

   RemoveElements();

   fQuantName = quant_name;
   fNBins     = nbins;
   fLastBin   = -1;
   fMin = fCurMin = min;
   fMax = fCurMax = max;
   fBinWidth  = (fMax - fMin)/fNBins;

   fBins = new TEvePointSet*[fNBins];
   for (Int_t i=0; i<fNBins; ++i) {
      fBins[i] = new TEvePointSet
         (Form("Slice %d [%4.3lf, %4.3lf]", i, fMin + i*fBinWidth, fMin + (i+1)*fBinWidth),
          fDefPointSetCapacity);
      fBins[i]->SetMarkerColor(fMarkerColor);
      fBins[i]->SetMarkerStyle(fMarkerStyle);
      fBins[i]->SetMarkerSize(fMarkerSize);
      if (addRe)
         gEve->AddElement(fBins[i], this);
      else
         AddElement(fBins[i]);
   }
}

//______________________________________________________________________________
void TEvePointSetArray::Fill(Double_t x, Double_t y, Double_t z, Double_t quant)
{
   // Add a new point. Appropriate point-set will be chosen based on
   // the value of the separating quantity 'quant'.

   fLastBin = Int_t( (quant - fMin)/fBinWidth );
   if (fLastBin >= 0 && fLastBin < fNBins && fBins[fLastBin] != 0)
      fBins[fLastBin]->SetNextPoint(x, y, z);
   else
      fLastBin = -1;
}

//______________________________________________________________________________
void TEvePointSetArray::SetPointId(TObject* id)
{
   // Set external object id of the last added point.

   if (fLastBin >= 0)
      fBins[fLastBin]->SetPointId(id);
}

//______________________________________________________________________________
void TEvePointSetArray::CloseBins()
{
   // Call this after all the points have been filled.
   // At this point we can calculate bounding-boxes of individual
   // point-sets.

   for (Int_t i=0; i<fNBins; ++i) {
      if (fBins[i] != 0) {
         // HACK! PolyMarker3D does half-management of array size.
         // In fact, the error is mine, in pointset3d(gl) i use fN instead of Size().
         // Fixed in my root, but not elsewhere.
         fBins[i]->fN = fBins[i]->fLastPoint;

         fBins[i]->ComputeBBox();
      }
   }
   fLastBin = -1;
}

/******************************************************************************/

//______________________________________________________________________________
void TEvePointSetArray::SetOwnIds(Bool_t o)
{
   // Propagate id-object ownership to children.

   for (Int_t i=0; i<fNBins; ++i)
   {
      if (fBins[i] != 0)
         fBins[i]->SetOwnIds(o);
   }
}

/******************************************************************************/

//______________________________________________________________________________
void TEvePointSetArray::SetRange(Double_t min, Double_t max)
{
   // Set active range of the separating quantity.
   // Appropriate point-sets are tagged for rendering.

   using namespace TMath;

   fCurMin = min; fCurMax = max;
   Int_t  low_b = (Int_t) Max(Double_t(0),       Floor((min-fMin)/fBinWidth));
   Int_t high_b = (Int_t) Min(Double_t(fNBins-1), Ceil((max-fMin)/fBinWidth));
   for (Int_t i=0; i<fNBins; ++i) {
      if (fBins[i] != 0)
         fBins[i]->SetRnrSelf(i>=low_b && i<=high_b);
   }
}


/******************************************************************************/
/******************************************************************************/

//______________________________________________________________________________
// TEvePointSetProjected
//
// Projected copy of a TEvePointSet.

ClassImp(TEvePointSetProjected)

//______________________________________________________________________________
TEvePointSetProjected::TEvePointSetProjected() :
   TEvePointSet  (),
   TEveProjected ()
{
   // Default contructor.
}

//______________________________________________________________________________
void TEvePointSetProjected::SetProjection(TEveProjectionManager* proj,
                                          TEveProjectable* model)
{
   // Set projection manager and projection model.
   // Virtual from TEveProjected.

   TEveProjected::SetProjection(proj, model);
   * (TAttMarker*)this = * dynamic_cast<TAttMarker*>(fProjectable);
}

//______________________________________________________________________________
void TEvePointSetProjected::UpdateProjection()
{
   // Reapply the projection.
   // Virtual from TEveProjected.

   TEveProjection& proj = * fManager->GetProjection();
   TEvePointSet     & ps   = * dynamic_cast<TEvePointSet*>(fProjectable);

   Int_t n = ps.GetN();
   Reset(n);
   Float_t *o = ps.GetP(), *p = GetP();
   for (Int_t i = 0; i < n; ++i, o+=3, p+=3)
   {
      p[0] = o[0]; p[1] = o[1]; p[2] = o[2];
      proj.ProjectPoint(p[0], p[1], p[2]);
      p[2] = fDepth;
   }
   fLastPoint = n - 1;
}
