// @(#)root/eve:$Id$
// Author: Matevz Tadel 2007

/*************************************************************************
 * Copyright (C) 1995-2007, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TEveArrow
#define ROOT_TEveArrow

#include "TEveElement.h"
#include "TNamed.h"
#include "TAtt3D.h"
#include "TAttBBox.h"
#include "TEveVSDStructs.h"

class TEveArrow : public TEveElement,
                  public TNamed,
                  public TAtt3D,
                  public TAttBBox
{
   friend class TEveArrowGL;
   friend class TEveArrowEditor;

private:
   TEveArrow(const TEveArrow&);            // Not implemented
   TEveArrow& operator=(const TEveArrow&); // Not implemented

protected:
   Color_t     fColor;

   Float_t     fTubeR;
   Float_t     fConeR;
   Float_t     fConeL;

   TEveVector  fOrigin;
   TEveVector  fVector;

public:
   TEveArrow(Float_t xVec=0, Float_t yVec=0, Float_t zVec=1,
             Float_t x0=0, Float_t y0=0, Float_t z0=0);
   virtual ~TEveArrow() {}

   void SetTubeR(Float_t x) { fTubeR = x; }
   void SetConeR(Float_t x) { fConeR = x; }
   void SetConeL(Float_t x) { fConeL = x; }

   Float_t GetTubeR() const { return fTubeR; }
   Float_t GetConeR() const { return fConeR; }
   Float_t GetConeL() const { return fConeL; }

   TEveVector GetVector() { return fVector; }
   TEveVector GetOrigin() { return fOrigin; }

   virtual void ComputeBBox();
   virtual void Paint(Option_t* option="");

   virtual Bool_t CanEditMainColor() const { return kTRUE; }

   ClassDef(TEveArrow, 0); // Class for gl visualisation of arrow.
};

#endif
