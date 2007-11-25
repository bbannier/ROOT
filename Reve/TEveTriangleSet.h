// @(#)root/eve:$Id$
// Authors: Matevz Tadel & Alja Mrak-Tadel: 2006, 2007

/*************************************************************************
 * Copyright (C) 1995-2007, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TEveTriangleSet
#define ROOT_TEveTriangleSet

#include <TEveElement.h>
#include <TNamed.h>
#include <TAttBBox.h>
#include <TAtt3D.h>

#include <TEveTrans.h>

class TGeoMatrix;

class TEveTriangleSet : public TEveElement,
                        public TNamed,
                        public TAttBBox,
                        public TAtt3D
{
   friend class TEveTriangleSetEditor;
   friend class TEveTriangleSetGL;

   TEveTriangleSet(const TEveTriangleSet&);            // Not implemented
   TEveTriangleSet& operator=(const TEveTriangleSet&); // Not implemented

protected:

   // Vertex data
   Int_t    fNVerts;
   Float_t* fVerts;        //[3*fNVerts]

   // Triangle data
   Int_t    fNTrings;
   Int_t*   fTrings;       //[3*fNTrings]
   Float_t* fTringNorms;   //[3*fNTrings]
   UChar_t* fTringCols;    //[3*fNTrings]

   // --------------------------------------------------------------

   Color_t  fColor;
   UShort_t fTransp;
   TEveTrans   fHMTrans;

public:

   TEveTriangleSet(Int_t nv, Int_t nt, Bool_t norms=false, Bool_t cols=false);
   ~TEveTriangleSet();

   virtual Bool_t CanEditMainColor() { return kTRUE; }

   Float_t* Vertex(Int_t i)         { return &(fVerts[3*i]);      }
   Int_t*   Triangle(Int_t i)       { return &(fTrings[3*i]);     }
   Float_t* TriangleNormal(Int_t i) { return &(fTringNorms[3*i]); }
   UChar_t* TriangleColor(Int_t i)  { return &(fTringCols[3*i]);  }

   void SetVertex(Int_t i, Float_t x, Float_t y, Float_t z)
   { Float_t* v = Vertex(i); v[0] = x; v[1] = y; v[2] = z; }
   void SetTriangle(Int_t i, Int_t v0, Int_t v1, Int_t v2)
   { Int_t* t = Triangle(i); t[0] = v0; t[1] = v1; t[2] = v2; }
   void SetTriangleColor(Int_t i, UChar_t r, UChar_t g, UChar_t b, UChar_t a=255)
   { UChar_t* c = TriangleColor(i); c[0] = r; c[1] = g; c[2] = b; c[3] = a; }

   void GenerateTriangleNormals();
   void GenerateRandomColors();
   void GenerateZNormalColors(Float_t fac=20, Int_t min=-20, Int_t max=20,
                              Bool_t interp=kFALSE, Bool_t wrap=kFALSE);

   virtual void ComputeBBox();
   virtual void Paint(Option_t* = "");

   Color_t GetColor() const { return fColor; }
   void SetColor(Color_t c) { fColor = c; }

   UShort_t GetTransparency() const { return fTransp; }
   void SetTransparency(UShort_t tr) { fTransp = tr; } // *MENU*

   TEveTrans& RefHMTrans() { return fHMTrans; }
   void SetTransMatrix(Double_t* carr)        { fHMTrans.SetFrom(carr); }
   void SetTransMatrix(const TGeoMatrix& mat) { fHMTrans.SetFrom(mat);  }

   static TEveTriangleSet* ReadTrivialFile(const char* file);

   ClassDef(TEveTriangleSet, 0); // Generic mesh or soup of triangles with per-triangle normals and colors.
};

#endif
