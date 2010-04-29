// @(#)root/eve:$Id$
// Authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/*************************************************************************
 * Copyright (C) 1995-2007, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/


#include "TEveTrackPropagator.h"
#include "TEveTrack.h"
#include "TEveTrans.h"

#include "TMath.h"

#include <cassert>

namespace
{
   const Float_t kBMin     = 1e-6;
   const Float_t kPtMinSqr = 1e-20;
   const Float_t kAMin     = 1e-10;
   const Float_t kStepEps  = 1e-3;
}

//______________________________________________________________________________
TEveTrackPropagator::Helix_t::Helix_t() :
   fCharge(0),
   fMaxAng(45), fMaxStep(20.f), fDelta(0.1),
   fPhi(0), fValid(kFALSE),
   fLam(-1), fR(-1), fPhiStep(-1), fSin(-1), fCos(-1),
   fRKStep(20.0f),
   fPtMag(-1), fPlMag(-1), fLStep(-1)
{
   // Default constructor.
}

//______________________________________________________________________________
void TEveTrackPropagator::Helix_t::UpdateCommon(const TEveVector& p, const TEveVector& b)
{
   // Common update code for helix and RK propagation.

   fB = b;

   // base vectors
   fE1 = b;
   fE1.Normalize();
   fPlMag = p.Dot(fE1);
   fPl    = fE1*fPlMag;

   fPt    = p - fPl;
   fPtMag = fPt.Mag();
   fE2    = fPt;
   fE2.Normalize();
}

//______________________________________________________________________________
void TEveTrackPropagator::Helix_t::UpdateHelix(const TEveVector& p, const TEveVector& b,
                                               Bool_t full_update, Bool_t enforce_max_step)
{
   // Update helix parameters.

   UpdateCommon(p, b);

   // helix parameters
   TMath::Cross(fE1.Arr(), fE2.Arr(), fE3.Arr());
   if (fCharge < 0) fE3.NegateXYZ();

   if (full_update)
   {
      using namespace TMath;
      Float_t a = fgkB2C * b.Mag() * Abs(fCharge);
      if (a > kAMin && fPtMag*fPtMag > kPtMinSqr)
      {
         fValid = kTRUE;

         fR   = Abs(fPtMag / a);
         fLam = fPlMag / fPtMag;

         // get phi step, compare fMaxAng with fDelta
         fPhiStep = fMaxAng * DegToRad();
         if (fR > fDelta)
         {
            Float_t ang  = 2.0 * ACos(1.0f - fDelta/fR);
            if (ang < fPhiStep)
               fPhiStep = ang;
         }

         // check max step size
         Float_t curr_step = fR * fPhiStep * Sqrt(1.0f + fLam*fLam);
         if (curr_step > fMaxStep || enforce_max_step)
            fPhiStep *= fMaxStep / curr_step;

         fLStep = fR * fPhiStep * fLam;
         fSin   = Sin(fPhiStep);
         fCos   = Cos(fPhiStep);
      }
      else
      {
         fValid = kFALSE;
      }
   }
}

//______________________________________________________________________________
void TEveTrackPropagator::Helix_t::UpdateRK(const TEveVector& p, const TEveVector& b)
{
   // Update helix for stepper RungeKutta.

   UpdateCommon(p, b);

   if (fCharge)
   {
      fValid = kTRUE;

      // cached values for propagator
      fB = b;
      fPlMag = p.Dot(fB);
   }
   else
   {
      fValid = kFALSE;
   }
}

//______________________________________________________________________________
void TEveTrackPropagator::Helix_t::Step(const TEveVector4& v, const TEveVector& p,
                                        TEveVector4& vOut, TEveVector& pOut)
{
   // Step helix for given momentum p from vertex v.

   vOut = v;

   if (fValid)
   {
      TEveVector d = fE2*(fR*fSin) + fE3*(fR*(1-fCos)) + fE1*fLStep;
      vOut    += d;
      vOut.fT += fLStep;

      pOut = fPl + fE2*(fPtMag*fCos) + fE3*(fPtMag*fSin);

      fPhi += fPhiStep;
   }
   else
   {
      // case: pT < kPtMinSqr or B < kBMin
      // might happen if field directon changes pT ~ 0 or B becomes zero
      vOut += p * (fMaxStep / p.Mag());
      pOut  = p;
   }
}


//==============================================================================
// TEveTrackPropagator
//==============================================================================

//______________________________________________________________________________
//
// Holding structure for a number of track rendering parameters.
// Calculates path taking into account the parameters.
//
// This is decoupled from TEveTrack/TEveTrackList to allow sharing of the
// Propagator among several instances. Back references are kept so the
// tracks can be recreated when the parameters change.
//
// TEveTrackList has Get/Set methods for RnrStlye. TEveTrackEditor and
// TEveTrackListEditor provide editor access.

ClassImp(TEveTrackPropagator);

Float_t             TEveTrackPropagator::fgDefMagField = 0.5;
const Float_t       TEveTrackPropagator::fgkB2C        = 0.299792458e-2;
TEveTrackPropagator TEveTrackPropagator::fgDefault;

Float_t             TEveTrackPropagator::fgEditorMaxR  = 2000;
Float_t             TEveTrackPropagator::fgEditorMaxZ  = 4000;

//______________________________________________________________________________
TEveTrackPropagator::TEveTrackPropagator(const char* n, const char* t,
                                         TEveMagField *field) :
   TEveElementList(n, t),
   TEveRefBackPtr(),

   fStepper(kHelix),
   fMagFieldObj(field),
   fMaxR    (350),
   fMaxZ    (450),

   fNMax    (4096),
   fMaxOrbs (0.5),

   fEditPathMarks (kTRUE),
   fFitDaughters  (kTRUE),
   fFitReferences (kTRUE),
   fFitDecay      (kTRUE),
   fFitCluster2Ds (kTRUE),

   fRnrDaughters  (kFALSE),
   fRnrReferences (kFALSE),
   fRnrDecay      (kFALSE),
   fRnrCluster2Ds (kFALSE),
   fRnrFV         (kFALSE),

   fPMAtt(),
   fFVAtt(),

   fV()
{
   // Default constructor.

   fPMAtt.SetMarkerColor(kYellow);
   fPMAtt.SetMarkerStyle(2);
   fPMAtt.SetMarkerSize(2);

   fFVAtt.SetMarkerColor(kRed);
   fFVAtt.SetMarkerStyle(4);
   fFVAtt.SetMarkerSize(1.5);


   if (fMagFieldObj == 0)
      fMagFieldObj = new TEveMagFieldConst(0., 0., fgDefMagField);
}

//______________________________________________________________________________
TEveTrackPropagator::~TEveTrackPropagator()
{
   // Destructor.

   if (fOwnMagFiledObj)
   {
      delete fMagFieldObj;
   }
}

//______________________________________________________________________________
void TEveTrackPropagator::OnZeroRefCount()
{
   // Virtual from TEveRefBackPtr - track reference count has reached zero.

   CheckReferenceCount("TEveTrackPropagator::OnZeroRefCount ");
}

//______________________________________________________________________________
void TEveTrackPropagator::CheckReferenceCount(const TEveException& eh)
{
   // Check reference count - virtual from TEveElement.
   // Must also take into account references from TEveRefBackPtr.

   if (fRefCount <= 0)
   {
      TEveElementList::CheckReferenceCount(eh);
   }
}

//______________________________________________________________________________
void TEveTrackPropagator::ElementChanged(Bool_t update_scenes, Bool_t redraw)
{
   // Element-change notification.
   // Stamp all tracks as requiring display-list regeneration.
   // Virtual from TEveElement.

   TEveTrack* track;
   std::list<TEveElement*>::iterator i = fBackRefs.begin();
   while (i != fBackRefs.end())
   {
      track = dynamic_cast<TEveTrack*>(*i);
      track->StampObjProps();
      ++i;
   }
   TEveElementList::ElementChanged(update_scenes, redraw);
}

//==============================================================================

//______________________________________________________________________________
void TEveTrackPropagator::InitTrack(TEveVector &v,  Int_t charge)
{
   // Initialize internal data-members for given particle parameters.

   fV.fX = v.fX;
   fV.fY = v.fY;
   fV.fZ = v.fZ;

   fPoints.push_back(fV);

   // init helix
   fH.fPhi    = 0;
   fH.fCharge = charge;
}

//______________________________________________________________________________
void TEveTrackPropagator::ResetTrack()
{
   // Reset cache holding particle trajectory.

   fPoints.clear();

   // reset helix
   fH.fPhi = 0;
}

//______________________________________________________________________________
Bool_t TEveTrackPropagator::GoToVertex(TEveVector& v, TEveVector& p)
{
   // Propagate particle with momentum p to vertex v.

   Update(fV, p, kTRUE);

   if ((v-fV).Mag() < kStepEps)
   {
      fPoints.push_back(v);
      return kTRUE;
   }

   return fH.fValid ? LoopToVertex(v, p) : LineToVertex(v);
}

//______________________________________________________________________________
void TEveTrackPropagator::GoToBounds(TEveVector& p)
{
   // Propagate particle to bounds.
   // Return TRUE if hit bounds.

   Update(fV, p, kTRUE);

   fH.fValid ? LoopToBounds(p): LineToBounds(p);
}

//______________________________________________________________________________
void TEveTrackPropagator::Update(const TEveVector4& v, const TEveVector& p,
                                 Bool_t full_update, Bool_t enforce_max_step)
{
   // Update helix / B-field projection state.

   if (fStepper == kHelix)
   {
      fH.UpdateHelix(p, fMagFieldObj->GetField(v), !fMagFieldObj->IsConst() || full_update, enforce_max_step);
   }
   else
   {
      fH.UpdateRK(p, fMagFieldObj->GetField(v));

      if (full_update)
      {
         using namespace TMath;

         Float_t a = fgkB2C * fMagFieldObj->GetMaxFieldMag() * Abs(fH.fCharge);
         fH.fR = p.Mag() / a;

         // get phi step, compare fDelta with MaxAng
         fH.fPhiStep = fH.fMaxAng * DegToRad();
         if (fH.fR > fH.fDelta )
         {
            Float_t ang  = 2.0 * ACos(1.0f - fH.fDelta/fH.fR);
            if (ang < fH.fPhiStep)
               fH.fPhiStep = ang;
         }

         // check against maximum step-size
         fH.fRKStep = fH.fR * fH.fPhiStep * Sqrt(1 + fH.fLam*fH.fLam);
         if (fH.fRKStep > fH.fMaxStep || enforce_max_step)
         {
            fH.fPhiStep *= fH.fMaxStep / fH.fRKStep;
            fH.fRKStep   = fH.fMaxStep;
         }
      }
   }
}

//______________________________________________________________________________
void TEveTrackPropagator::Step(const TEveVector4 &v, const TEveVector &p, TEveVector4 &vOut, TEveVector &pOut)
{
   // Wrapper to step helix.

   if (fStepper == kHelix)
   {
      fH.Step(v, p, vOut, pOut);
   }
   else
   {
      Double_t vecRKIn[7];
      vecRKIn[0] = v.fX;
      vecRKIn[1] = v.fY;
      vecRKIn[2] = v.fZ;
      Double_t pm = p.Mag();
      Double_t nm = 1.0 / pm;
      vecRKIn[3] = p.fX*nm;
      vecRKIn[4] = p.fY*nm;
      vecRKIn[5] = p.fZ*nm;
      vecRKIn[6] = p.Mag();

      Double_t vecRKOut[7];
      StepRungeKutta(fH.fRKStep, vecRKIn, vecRKOut);

      vOut.fX = vecRKOut[0];
      vOut.fY = vecRKOut[1];
      vOut.fZ = vecRKOut[2];
      vOut.fT = v.fT + fH.fRKStep;
      pm = vecRKOut[6];
      pOut.fX = vecRKOut[3]*pm;
      pOut.fY = vecRKOut[4]*pm;
      pOut.fZ = vecRKOut[5]*pm;
   }
}

//______________________________________________________________________________
void TEveTrackPropagator::LoopToBounds(TEveVector& p)
{
   // Propagate charged particle with momentum p to bounds.
   // It is expected that Update() with full-update was called before.

   const Float_t maxRsq = fMaxR*fMaxR;

   TEveVector4 currV(fV);
   TEveVector4 forwV(fV);
   TEveVector  forwP (p);

   Int_t np = fPoints.size();
   Float_t maxPhi = fMaxOrbs*TMath::TwoPi();

   while (fH.fPhi < maxPhi && np<fNMax)
   {
      Step(currV, p, forwV, forwP);

      // cross R
      if (forwV.Perp2() > maxRsq)
      {
         Float_t t = (fMaxR - currV.R()) / (forwV.R() - currV.R());
         if (t < 0 || t > 1)
         {
            Warning("HelixToBounds", "In MaxR crossing expected t>=0 && t<=1: t=%f, r1=%f, r2=%f, MaxR=%f.",
                    t, currV.R(), forwV.R(), fMaxR);
            return;
         }
         TEveVector d(forwV);
         d -= currV;
         d *= t;
         d += currV;
         fPoints.push_back(d);
         return;
      }

      // cross Z
      else if (TMath::Abs(forwV.fZ) > fMaxZ)
      {
         Float_t t = (fMaxZ - TMath::Abs(currV.fZ)) / TMath::Abs((forwV.fZ - currV.fZ));
         if (t < 0 || t > 1)
         {
            Warning("HelixToBounds", "In MaxZ crossing expected t>=0 && t<=1: t=%f, z1=%f, z2=%f, MaxZ=%f.",
                    t, currV.fZ, forwV.fZ, fMaxZ);
            return;
         }
         TEveVector d(forwV -currV);
         d *= t;
         d += currV;
         fPoints.push_back(d);
         return;
      }

      currV = forwV;
      p     = forwP;
      Update(currV, p);

      fPoints.push_back(currV);
      ++np;
   }
}

//______________________________________________________________________________
Bool_t TEveTrackPropagator::LoopToVertex(TEveVector& v, TEveVector& p)
{
   // Propagate charged particle with momentum p to vertex v.
   // It is expected that Update() with full-update was called before.

   const Float_t maxRsq = fMaxR * fMaxR;

   TEveVector4 currV(fV);
   TEveVector4 forwV(fV);
   TEveVector  forwP(p);

   Int_t first_point = fPoints.size();
   Int_t np          = first_point;

   Float_t prod0=0, prod1;

   do
   {
      Step(currV, p, forwV, forwP);
      Update(forwV, forwP);

      if (PointOverVertex(v, forwV, &prod1))
      {
         break;
      }

      if (IsOutsideBounds(forwV, maxRsq, fMaxZ))
      {
         fV = currV;
         return kFALSE;
      }

      fPoints.push_back(forwV);
      currV = forwV;
      p     = forwP;
      prod0 = prod1;
      ++np;
   } while (np < fNMax);

   // make the remaining fractional step
   if (np > first_point)
   {
      TEveVector d1 = v;
      d1 -= currV;

      if (d1.Mag() > kStepEps)
      {
         Float_t step_frac = prod0 / (prod0 - prod1);
         if (step_frac > 0)
         {
            // Step for fraction of previous step size.
            // We pass 'enforce_max_step' flag to Update().
            Float_t orig_max_step = fH.fMaxStep;
            fH.fMaxStep *= step_frac;
            Update(currV, p, kTRUE, kTRUE);
            Step(currV, p, forwV, forwP);
            p     = forwP;
            currV = forwV;
            fPoints.push_back(currV);
            ++np;
            fH.fMaxStep = orig_max_step;
         }

         // Distribute offset to desired crossing point over all segment.

         TEveVector off(v); off -= currV;
         off *= 1.0f / currV.fT;

         // Calculate the required momentum rotation.
         // lpd - last-points-delta
         TEveVector lpd0(fPoints[np-1]);
         lpd0 -= fPoints[np-2];
         lpd0.Normalize();

         for (Int_t i = first_point; i < np; ++i)
         {
            fPoints[i] += off * fPoints[i].fT;
         }

         TEveVector lpd1(fPoints[np-1]);
         lpd1 -= fPoints[np-2];
         lpd1.Normalize();

         TEveTrans tt;
         tt.SetupFromToVec(lpd0, lpd1);

         // TEveVector pb4(p);
         // printf("Rotating momentum: p0 = "); p.Dump();
         tt.RotateIP(p);
         // printf("                   p1 = "); p.Dump();
         // printf("  n1=%f, n2=%f, dp = %f deg\n", pb4.Mag(), p.Mag(),
         //        TMath::RadToDeg()*TMath::ACos(p.Dot(pb4)/(pb4.Mag()*p.Mag())));

         fV = v;
         return kTRUE;
      }
   }

   fPoints.push_back(v);
   fV = v;
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TEveTrackPropagator::LineToVertex(TEveVector& v)
{
   // Propagate neutral particle to vertex v.

   TEveVector4 currV = v;

   currV.fX = v.fX;
   currV.fY = v.fY;
   currV.fZ = v.fZ;
   fPoints.push_back(currV);

   fV = v;
   return kTRUE;
}

//______________________________________________________________________________
void TEveTrackPropagator::LineToBounds(TEveVector& p)
{
   // Propagatate neutral particle with momentum p to bounds.

   Float_t tZ = 0, tR = 0, tB = 0;

   // time where particle intersect +/- fMaxZ
   if (p.fZ > 0)
      tZ = (fMaxZ - fV.fZ) / p.fZ;
   else if (p.fZ < 0)
      tZ = - (fMaxZ + fV.fZ) / p.fZ;

   // time where particle intersects cylinder
   Double_t a = p.fX*p.fX + p.fY*p.fY;
   Double_t b = 2.0 * (fV.fX*p.fX + fV.fY*p.fY);
   Double_t c = fV.fX*fV.fX + fV.fY*fV.fY - fMaxR*fMaxR;
   Double_t d = b*b - 4.0*a*c;
   if (d >= 0) {
      Double_t sqrtD = TMath::Sqrt(d);
      tR = (-b - sqrtD) / (2.0 * a);
      if (tR < 0) {
         tR = (-b + sqrtD) / (2.0 * a);
      }
      tB = tR < tZ ? tR : tZ; // compare the two times
   } else {
      tB = tZ;
   }
   TEveVector nv(fV.fX + p.fX*tB, fV.fY + p.fY*tB, fV.fZ + p.fZ*tB);
   LineToVertex(nv);
}

//______________________________________________________________________________
Bool_t TEveTrackPropagator::HelixIntersectPlane(const TEveVector& p,
                                                const TEveVector& point,
                                                const TEveVector& normal,
                                                TEveVector& itsect)
{
   // Intersect helix with a plane. Current position and argument p define
   // the helix.

   TEveVector pos(fV);
   TEveVector mom(p);
   if (fMagFieldObj->IsConst())
      fH.UpdateHelix(mom, fMagFieldObj->GetField(pos), kFALSE, kFALSE);

   TEveVector n(normal);
   TEveVector delta = pos - point;
   Float_t d = delta.Dot(n);
   if (d > 0) {
      n.NegateXYZ(); // Turn normal around so that we approach from negative side of the plane
      d = -d;
   }

   TEveVector4 forwV;
   TEveVector  forwP;
   TEveVector4 pos4(pos);
   while (kTRUE)
   {
      Update(pos4, mom);
      Step(pos4, mom, forwV , forwP);
      Float_t new_d = (forwV - point).Dot(n);
      if (new_d < d)
      {
         // We are going further away ... fail intersect.
         Warning("HelixIntersectPlane", "going away from the plane.");
         return kFALSE;
      }
      if (new_d > 0)
      {
         delta = forwV - pos;
         itsect = pos + delta * (d / (d - new_d));
         return kTRUE;
      }
      pos4 = forwV;
      mom  = forwP;
   }
}

//______________________________________________________________________________
Bool_t TEveTrackPropagator::LineIntersectPlane(const TEveVector& p,
                                               const TEveVector& point,
                                               const TEveVector& normal,
                                                     TEveVector& itsect)
{
   // Intersect line with a plane. Current position and argument p define
   // the line.

   TEveVector pos(fV.fX, fV.fY, fV.fZ);
   TEveVector delta = pos - point;

   Float_t d = delta.Dot(normal);
   if (d == 0) {
      itsect = pos;
      return kTRUE;
   }

   Float_t t = (p.Dot(normal)) / d;
   if (t < 0) {
      return kFALSE;
   } else {
      itsect = pos + p*t;
      return kTRUE;
   }
}

//______________________________________________________________________________
Bool_t TEveTrackPropagator::IntersectPlane(const TEveVector& p,
                                           const TEveVector& point,
                                           const TEveVector& normal,
                                                 TEveVector& itsect)
{
   // Find intersection of currently propagated track with a plane.
   // Current track position is used as starting point.
   //
   // Args:
   //  p        - track momentum to use for extrapolation
   //  point    - a point on a plane
   //  normal   - normal of the plane
   //  itsect   - output, point of intersection
   // Returns:
   //  kFALSE if intersection can not be found, kTRUE otherwise.

   if (fH.fCharge && fMagFieldObj && p.Perp2() > kPtMinSqr)
      return HelixIntersectPlane(p, point, normal, itsect);
   else
      return LineIntersectPlane(p, point, normal, itsect);
}

//______________________________________________________________________________
void TEveTrackPropagator::FillPointSet(TEvePointSet* ps) const
{
   // Reset ps and populate it with points in propagation cache.

   Int_t size = TMath::Min(fNMax, (Int_t)fPoints.size());
   ps->Reset(size);
   for (Int_t i = 0; i < size; ++i)
   {
      const TEveVector4& v = fPoints[i];
      ps->SetNextPoint(v.fX, v.fY, v.fZ);
   }
}

/******************************************************************************/

//______________________________________________________________________________
void TEveTrackPropagator::RebuildTracks()
{
   // Rebuild all tracks using this render-style.

   TEveTrack* track;
   std::list<TEveElement*>::iterator i = fBackRefs.begin();
   while (i != fBackRefs.end())
   {
      track = dynamic_cast<TEveTrack*>(*i);
      track->MakeTrack();
      track->StampObjProps();
      ++i;
   }
}

//______________________________________________________________________________
void TEveTrackPropagator::SetMagField(Float_t bX, Float_t bY, Float_t bZ)
{
   // Set constant magnetic field and rebuild tracks.

   if (fMagFieldObj) delete fMagFieldObj;

   fMagFieldObj = new TEveMagFieldConst(bX, bY, bZ);
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::SetMagFieldObj(TEveMagField *mff, Bool_t ownFieldObj)
{
   // Set constant magnetic field and rebuild tracks.

   fOwnMagFiledObj = ownFieldObj;
   if (fMagFieldObj && fOwnMagFiledObj) delete fMagFieldObj;

   fMagFieldObj = mff;
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::PrintMagField(Float_t x, Float_t y, Float_t z) const
{
   if (fMagFieldObj) fMagFieldObj->PrintField(x, y, z);
}

//______________________________________________________________________________
void TEveTrackPropagator::SetMaxR(Float_t x)
{
   // Set maximum radius and rebuild tracks.

   fMaxR = x;
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::SetMaxZ(Float_t x)
{
   // Set maximum z and rebuild tracks.

   fMaxZ = x;
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::SetMaxOrbs(Float_t x)
{
   // Set maximum number of orbits and rebuild tracks.

   fMaxOrbs = x;
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::SetMinAng(Float_t x)
{
   // Set maximum step angle and rebuild tracks.
   // WARNING -- this method / variable was mis-named.

   Warning("SetMinAng", "This method was mis-named, use SetMaxAng() instead!");
   SetMaxAng(x);
}
//______________________________________________________________________________
Float_t TEveTrackPropagator::GetMinAng() const
{
   // Get maximum step angle.
   // WARNING -- this method / variable was mis-named.

   Warning("GetMinAng", "This method was mis-named, use GetMaxAng() instead!");
   return GetMaxAng();
}

//______________________________________________________________________________
void TEveTrackPropagator::SetMaxAng(Float_t x)
{
   // Set maximum step angle and rebuild tracks.

   fH.fMaxAng = x;
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::SetMaxStep(Float_t x)
{
   // Set maximum step-size and rebuild tracks.

   fH.fMaxStep = x;
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::SetDelta(Float_t x)
{
   // Set maximum error and rebuild tracks.

   fH.fDelta = x;
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::SetFitDaughters(Bool_t x)
{
   // Set daughter creation point fitting and rebuild tracks.

   fFitDaughters = x;
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::SetFitReferences(Bool_t x)
{
   // Set track-reference fitting and rebuild tracks.

   fFitReferences = x;
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::SetFitDecay(Bool_t x)
{
   // Set decay fitting and rebuild tracks.

   fFitDecay = x;
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::SetFitCluster2Ds(Bool_t x)
{
   // Set 2D-cluster fitting and rebuild tracks.

   fFitCluster2Ds = x;
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::SetRnrDecay(Bool_t rnr)
{
   // Set decay rendering and rebuild tracks.

   fRnrDecay = rnr;
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::SetRnrCluster2Ds(Bool_t rnr)
{
   // Set rendering of 2D-clusters and rebuild tracks.

   fRnrCluster2Ds = rnr;
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::SetRnrDaughters(Bool_t rnr)
{
   // Set daughter rendering and rebuild tracks.

   fRnrDaughters = rnr;
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::SetRnrReferences(Bool_t rnr)
{
   // Set track-reference rendering and rebuild tracks.

   fRnrReferences = rnr;
   RebuildTracks();
}

//______________________________________________________________________________
void TEveTrackPropagator::StepRungeKutta(Double_t step,
                                         Double_t* vect, Double_t* vout)
{
  // Wrapper to step with method RungeKutta.

  ///	******************************************************************
  ///	*								 *
  ///	*  Runge-Kutta method for tracking a particle through a magnetic *
  ///	*  field. Uses Nystroem algorithm (See Handbook Nat. Bur. of	 *
  ///	*  Standards, procedure 25.5.20)				 *
  ///	*								 *
  ///	*  Input parameters						 *
  ///	*	CHARGE    Particle charge				 *
  ///	*	STEP	  Step size					 *
  ///	*	VECT	  Initial co-ords,direction cosines,momentum	 *
  ///	*  Output parameters						 *
  ///	*	VOUT	  Output co-ords,direction cosines,momentum	 *
  ///	*  User routine called  					 *
  ///	*	CALL GUFLD(X,F) 					 *
  ///	*								 *
  ///	*    ==>Called by : <USER>, GUSWIM				 *
  ///	*	Authors    R.Brun, M.Hansroul  *********		 *
  ///	*		   V.Perevoztchikov (CUT STEP implementation)	 *
  ///	*								 *
  ///	*								 *
  ///	******************************************************************

  Double_t h2, h4, f[4];
  Double_t xyzt[3], a, b, c, ph,ph2;
  Double_t secxs[4],secys[4],seczs[4],hxp[3];
  Double_t g1, g2, g3, g4, g5, g6, ang2, dxt, dyt, dzt;
  Double_t est, at, bt, ct, cba;
  Double_t f1, f2, f3, f4, rho, tet, hnorm, hp, rho1, sint, cost;

  Double_t x;
  Double_t y;
  Double_t z;

  Double_t xt;
  Double_t yt;
  Double_t zt;

  // Double_t maxit = 1992;
  Double_t maxit = 10;
  Double_t maxcut = 11;

  const Double_t hmin   = 1e-4; // !!! MT ADD,  should be member
  const Double_t kdlt   = 1e-3; // !!! MT CHANGE from 1e-4, should be member
  const Double_t kdlt32 = kdlt/32.;
  const Double_t kthird = 1./3.;
  const Double_t khalf  = 0.5;
  const Double_t kec    = 2.9979251e-3;

  const Double_t kpisqua = 9.86960440109;
  const Int_t kix  = 0;
  const Int_t kiy  = 1;
  const Int_t kiz  = 2;
  const Int_t kipx = 3;
  const Int_t kipy = 4;
  const Int_t kipz = 5;

  // *.
  // *.    ------------------------------------------------------------------
  // *.
  // *             this constant is for units cm,gev/c and kgauss
  // *
  Int_t iter = 0;
  Int_t ncut = 0;
  for(Int_t j = 0; j < 7; j++)
    vout[j] = vect[j];

  Double_t pinv   = kec * fH.fCharge / vect[6];
  Double_t tl     = 0.;
  Double_t h      = step;
  Double_t rest;

  do {
    rest  = step - tl;
    if (TMath::Abs(h) > TMath::Abs(rest))
       h = rest;

    f[0] = -fH.fB.fX;
    f[1] = -fH.fB.fY;
    f[2] = -fH.fB.fZ;

    // * start of integration
    x      = vout[0];
    y      = vout[1];
    z      = vout[2];
    a      = vout[3];
    b      = vout[4];
    c      = vout[5];

    h2     = khalf * h;
    h4     = khalf * h2;
    ph     = pinv * h;
    ph2    = khalf * ph;
    secxs[0] = (b * f[2] - c * f[1]) * ph2;
    secys[0] = (c * f[0] - a * f[2]) * ph2;
    seczs[0] = (a * f[1] - b * f[0]) * ph2;
    ang2 = (secxs[0]*secxs[0] + secys[0]*secys[0] + seczs[0]*seczs[0]);
    if (ang2 > kpisqua) break;

    dxt    = h2 * a + h4 * secxs[0];
    dyt    = h2 * b + h4 * secys[0];
    dzt    = h2 * c + h4 * seczs[0];
    xt     = x + dxt;
    yt     = y + dyt;
    zt     = z + dzt;

    // * second intermediate point
    est = TMath::Abs(dxt) + TMath::Abs(dyt) + TMath::Abs(dzt);
    if (est > h) {
      if (ncut++ > maxcut) break;
      h *= khalf;
      continue;
    }

    xyzt[0] = xt;
    xyzt[1] = yt;
    xyzt[2] = zt;

    fH.fB = fMagFieldObj->GetField(xt, yt, zt);
    f[0] = -fH.fB.fX;
    f[1] = -fH.fB.fY;
    f[2] = -fH.fB.fZ;

    at     = a + secxs[0];
    bt     = b + secys[0];
    ct     = c + seczs[0];

    secxs[1] = (bt * f[2] - ct * f[1]) * ph2;
    secys[1] = (ct * f[0] - at * f[2]) * ph2;
    seczs[1] = (at * f[1] - bt * f[0]) * ph2;
    at     = a + secxs[1];
    bt     = b + secys[1];
    ct     = c + seczs[1];
    secxs[2] = (bt * f[2] - ct * f[1]) * ph2;
    secys[2] = (ct * f[0] - at * f[2]) * ph2;
    seczs[2] = (at * f[1] - bt * f[0]) * ph2;
    dxt    = h * (a + secxs[2]);
    dyt    = h * (b + secys[2]);
    dzt    = h * (c + seczs[2]);
    xt     = x + dxt;
    yt     = y + dyt;
    zt     = z + dzt;
    at     = a + 2.*secxs[2];
    bt     = b + 2.*secys[2];
    ct     = c + 2.*seczs[2];

    est = TMath::Abs(dxt)+TMath::Abs(dyt)+TMath::Abs(dzt);
    if (est > 2.*TMath::Abs(h)) {
      if (ncut++ > maxcut) break;
      h *= khalf;
      continue;
    }

    xyzt[0] = xt;
    xyzt[1] = yt;
    xyzt[2] = zt;

    fH.fB = fMagFieldObj->GetField(xt, yt, zt);
    f[0] = -fH.fB.fX;
    f[1] = -fH.fB.fY;
    f[2] = -fH.fB.fZ;

    z      = z + (c + (seczs[0] + seczs[1] + seczs[2]) * kthird) * h;
    y      = y + (b + (secys[0] + secys[1] + secys[2]) * kthird) * h;
    x      = x + (a + (secxs[0] + secxs[1] + secxs[2]) * kthird) * h;

    secxs[3] = (bt*f[2] - ct*f[1])* ph2;
    secys[3] = (ct*f[0] - at*f[2])* ph2;
    seczs[3] = (at*f[1] - bt*f[0])* ph2;
    a      = a+(secxs[0]+secxs[3]+2. * (secxs[1]+secxs[2])) * kthird;
    b      = b+(secys[0]+secys[3]+2. * (secys[1]+secys[2])) * kthird;
    c      = c+(seczs[0]+seczs[3]+2. * (seczs[1]+seczs[2])) * kthird;

    est    = TMath::Abs(secxs[0]+secxs[3] - (secxs[1]+secxs[2]))
      + TMath::Abs(secys[0]+secys[3] - (secys[1]+secys[2]))
      + TMath::Abs(seczs[0]+seczs[3] - (seczs[1]+seczs[2]));

    if (est > kdlt && TMath::Abs(h) > hmin) {
      if (ncut++ > maxcut) break;
      h *= khalf;
      continue;
    }

    ncut = 0;
    // * if too many iterations, go to helix
    if (iter++ > maxit) break;

    tl += h;
    if (est < kdlt32)
      h *= 2.;
    cba    = 1./ TMath::Sqrt(a*a + b*b + c*c);
    vout[0] = x;
    vout[1] = y;
    vout[2] = z;
    vout[3] = cba*a;
    vout[4] = cba*b;
    vout[5] = cba*c;
    rest = step - tl;
    if (step < 0.) rest = -rest;
    if (rest < 1.e-5*TMath::Abs(step))
    {
       Float_t dot = (vout[3]*vect[3] + vout[4]*vect[4] + vout[5]*vect[5]);
       fH.fPhi += TMath::ACos(dot);
       return;
    }

  } while(1);

  // angle too big, use helix

  f1  = f[0];
  f2  = f[1];
  f3  = f[2];
  f4  = TMath::Sqrt(f1*f1+f2*f2+f3*f3);
  rho = -f4*pinv;
  tet = rho * step;

  hnorm = 1./f4;
  f1 = f1*hnorm;
  f2 = f2*hnorm;
  f3 = f3*hnorm;

  hxp[0] = f2*vect[kipz] - f3*vect[kipy];
  hxp[1] = f3*vect[kipx] - f1*vect[kipz];
  hxp[2] = f1*vect[kipy] - f2*vect[kipx];

  hp = f1*vect[kipx] + f2*vect[kipy] + f3*vect[kipz];

  rho1 = 1./rho;
  sint = TMath::Sin(tet);
  cost = 2.*TMath::Sin(khalf*tet)*TMath::Sin(khalf*tet);

  g1 = sint*rho1;
  g2 = cost*rho1;
  g3 = (tet-sint) * hp*rho1;
  g4 = -cost;
  g5 = sint;
  g6 = cost * hp;

  vout[kix] = vect[kix] + g1*vect[kipx] + g2*hxp[0] + g3*f1;
  vout[kiy] = vect[kiy] + g1*vect[kipy] + g2*hxp[1] + g3*f2;
  vout[kiz] = vect[kiz] + g1*vect[kipz] + g2*hxp[2] + g3*f3;

  vout[kipx] = vect[kipx] + g4*vect[kipx] + g5*hxp[0] + g6*f1;
  vout[kipy] = vect[kipy] + g4*vect[kipy] + g5*hxp[1] + g6*f2;
  vout[kipz] = vect[kipz] + g4*vect[kipz] + g5*hxp[2] + g6*f3;

  fH.fPhi += tet;
}
