// @(#)root/g3d:$Name$:$Id$
// Author: Nenad Buncic   17/08/95

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TPolyLine3D
#define ROOT_TPolyLine3D


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPolyLine3D                                                          //
//                                                                      //
// A 3-D PolyLine.                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef ROOT_TList
#include "TList.h"
#endif
#ifndef ROOT_TTString
#include "TString.h"
#endif
#ifndef ROOT_TAttLine
#include "TAttLine.h"
#endif
#ifndef ROOT_TAtt3D
#include "TAtt3D.h"
#endif
#ifndef ROOT_X3DBuffer
#include "X3DBuffer.h"
#endif


class TPolyLine3D : public TObject, public TAttLine, public TAtt3D {

protected:
        Int_t        fN;            //Number of points
        Float_t      *fP;           //[3*fN] Array of 3-D coordinates  (x,y,z)
        TString      fOption;       //options
        UInt_t       fGLList;       //!The list number for OpenGL view
        Int_t        fLastPoint;    //The index of the last filled point

public:
        TPolyLine3D();
        TPolyLine3D(Int_t n, Option_t *option="");
        TPolyLine3D(Int_t n, Float_t *p, Option_t *option="");
        TPolyLine3D(Int_t n, Float_t *x, Float_t *y, Float_t *z, Option_t *option="");
        TPolyLine3D(const TPolyLine3D &polylin);
        virtual ~TPolyLine3D();

        virtual void      Copy(TObject &polyline);
        virtual Int_t     DistancetoPrimitive(Int_t px, Int_t py);
        virtual void      Draw(Option_t *option="");
        static  void      DrawOutlineCube(TList *outline, Float_t *rmin, Float_t *rmax);
        virtual void      DrawPolyLine(Int_t n, Float_t *p, Option_t *option="");
        virtual void      ExecuteEvent(Int_t event, Int_t px, Int_t py);
        Int_t             GetLastPoint() {return fLastPoint;}
        Int_t             GetN() {return fN;}
        Float_t           *GetP() {return fP;}
        Option_t          *GetOption() const {return fOption.Data();}
        virtual void      ls(Option_t *option="");
        virtual void      Paint(Option_t *option="");
        virtual void      PaintPolyLine(Int_t n, Float_t *p, Option_t *option="");
        virtual void      Print(Option_t *option="");
        virtual void      SavePrimitive(ofstream &out, Option_t *option);
        virtual Int_t     SetNextPoint(Float_t x, Float_t y, Float_t z); // *MENU*
        virtual void      SetOption(Option_t *option="") {fOption = option;}
        virtual void      SetPoint(Int_t point, Float_t x, Float_t y, Float_t z); // *MENU*
        virtual void      SetPolyLine(Int_t n, Float_t *p=0, Option_t *option="");
        virtual void      Sizeof3D() const;
        virtual Int_t     Size() const { return fLastPoint+1;}

        ClassDef(TPolyLine3D,1)  //A 3-D PolyLine
};

#endif

