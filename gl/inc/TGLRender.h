// @(#)root/gl:$Name:  $:$Id: TGLRender.h,v 1.7 2004/10/19 13:04:55 brun Exp $
// Author:  Timur Pocheptsov  03/08/2004

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TGLRender
#define ROOT_TGLRender

#include <utility>

#ifndef ROOT_TObjArray
#include "TObjArray.h"
#endif

class TGLSceneObject;
class TGLSelection;
class TGLCamera;

class TGLRender {
private:
   TObjArray      fGLObjects;
   TObjArray      fGLCameras;

   Bool_t         fAllActive;
   Bool_t         fIsPicking;
   Bool_t         fBoxInList;
   Int_t          fActiveCam;
   Int_t          fDList;
   UInt_t         fSelected;

   TGLSceneObject *fFirstT;
   TGLSceneObject *fSelectedObj;
   TGLSelection   *fSelectionBox;
   //clipping plane equation A*x+B*y+C*z+D=0
   Double_t       fPlaneEqn[4];
   Bool_t         fClipping;
   Bool_t         fAxes;
   //temporary
   Bool_t         fPxs;
   typedef std::pair<Double_t, Double_t> PDD_t;
   PDD_t          fAxeD[3];

public:
   TGLRender();
   ~TGLRender();
   void Traverse();
   void SetAllActive()
   {
      fAllActive = kTRUE;
   }
   void SetActive(UInt_t cam);
   void AddNewObject(TGLSceneObject *newObject);
   void AddNewCamera(TGLCamera *newCamera);
   TGLSceneObject *SelectObject(Int_t x, Int_t y, Int_t);
   void MoveSelected(Double_t x, Double_t y, Double_t z);
   Bool_t ResetPlane()
   {
      return fClipping = !fClipping;
   }
   void SetPlane(const Double_t *newEqn);
   Int_t GetSize()const
   {
      return fGLObjects.GetEntriesFast();
   }
   void EndMovement();
   void Invalidate();
   void SetAxes(const PDD_t &x, const PDD_t &y, const PDD_t &z);
   void ResetAxes()
   {
      fAxes = !fAxes;
   }

   void SetFamilyColor(const Float_t *newColor);

private:
   void BuildGLList(Bool_t execute = kFALSE);
   void RunGLList();
   void DrawAxes();

   TGLRender(const TGLRender &);
   TGLRender & operator = (const TGLRender &);
};

#endif
