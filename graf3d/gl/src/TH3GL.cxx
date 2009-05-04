// @(#)root/eve:$Id$
// Author: Matevz Tadel 2007

/*************************************************************************
 * Copyright (C) 1995-2007, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TH3GL.h"
#include "TH3.h"
#include "TVirtualPad.h"
#include "TAxis.h"
#include "TMath.h"

#include "TGLSurfacePainter.h"
#include "TGLHistPainter.h"
#include "TGLLegoPainter.h"
#include "TGLBoxPainter.h"
#include "TGLTF3Painter.h"
#include "TGLAxisPainter.h"
#include "TGLCamera.h"

#include "TGLRnrCtx.h"
#include "TGLIncludes.h"

//______________________________________________________________________________
// OpenGL renderer class for TH3.
//

ClassImp(TH3GL);

//______________________________________________________________________________
TH3GL::TH3GL() :
   TGLObject(), fM(0), fPlotPainter(0)
{
   // Constructor.

   fDLCache = kFALSE; // Disable display list.
}

//______________________________________________________________________________
TH3GL::~TH3GL()
{
   // Destructor.

   delete fPlotPainter;
}

/******************************************************************************/

//______________________________________________________________________________
Bool_t TH3GL::SetModel(TObject* obj, const Option_t* opt)
{
   // Set model object.

   TString option(opt);
   option.ToLower();

   if (SetModelCheckClass(obj, TH3::Class())) {
      fM = dynamic_cast<TH3*>(obj);
      if (option.Index("iso") != kNPOS)
         fPlotPainter = new TGLIsoPainter(fM, 0, &fCoord);
      else if (option.Index("box") != kNPOS)
         fPlotPainter = new TGLBoxPainter(fM, 0, &fCoord);

      fPlotPainter->AddOption(option);
      fPlotPainter->InitGeometry();
      return kTRUE;
   }
   return kFALSE;
}

//______________________________________________________________________________
void TH3GL::SetBBox()
{
   // Set bounding box.

   fBoundingBox.Set(fPlotPainter->RefBackBox().Get3DBox());
}

/******************************************************************************/

//______________________________________________________________________________
void TH3GL::DirectDraw(TGLRnrCtx & rnrCtx) const
{
   // Render with OpenGL.

   fPlotPainter->RefBackBox().FindFrontPoint();

   glPushAttrib(GL_ENABLE_BIT | GL_LIGHTING_BIT);

   glEnable(GL_NORMALIZE);
   glDisable(GL_COLOR_MATERIAL);

   fPlotPainter->InitGL();
   fPlotPainter->DrawPlot();

   glDisable(GL_CULL_FACE);
   glPopAttrib();

   // Axes
   TGLAxisPainterBox axe_painter;
   axe_painter.SetFontMode(TGLFont::kPixmap);
   axe_painter.PlotStandard(rnrCtx, fM, fBoundingBox);
}
