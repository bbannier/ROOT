// @(#)root/gui:$Id$
// Author: Fons Rademakers   17/01/98

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
/**************************************************************************

    This source is based on Xclass95, a Win95-looking GUI toolkit.
    Copyright (C) 1996, 1997 David Barth, Ricky Ralston, Hector Peraza.

    Xclass95 is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

**************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGListView, TGLVContainer and TGLVEntry                              //
//                                                                      //
// A list view is a widget that can contain a number of items           //
// arranged in a grid or list. The items can be represented either      //
// by a string or by an icon.                                           //
//                                                                      //
// The TGListView is user callable. The other classes are service       //
// classes of the list view.                                            //
//                                                                      //
// A list view can generate the following events:                       //
// kC_CONTAINER, kCT_SELCHANGED, total items, selected items.           //
// kC_CONTAINER, kCT_ITEMCLICK, which button, location (y<<16|x).       //
// kC_CONTAINER, kCT_ITEMDBLCLICK, which button, location (y<<16|x).    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TGListView.h"
#include "TGPicture.h"
#include "TGButton.h"
#include "TGScrollBar.h"
#include "TGResourcePool.h"
#include "TList.h"
#include "TSystem.h"
#include "TGMimeTypes.h"
#include "TObjString.h"
#include "Riostream.h"

const TGFont *TGLVEntry::fgDefaultFont = 0;
TGGC         *TGLVEntry::fgDefaultGC = 0;

const TGFont *TGListView::fgDefaultFont = 0;
TGGC         *TGListView::fgDefaultGC = 0;


ClassImp(TGLVEntry)
ClassImp(TGLVContainer)
ClassImp(TGListView)

//______________________________________________________________________________
TGLVEntry::TGLVEntry(const TGWindow *p, const TGPicture *bigpic,
                     const TGPicture *smallpic, TGString *name,
                     TGString **subnames, EListViewMode viewMode,
                     UInt_t options, ULong_t back) :
   TGFrame(p, 10, 10, options, back)
{
   // Create a list view item.

   fSelPic = 0;

   fCurrent  =
   fBigPic   = bigpic;
   fSmallPic = smallpic;

   fCheckMark  = fClient->GetPicture("checkmark_t.xpm");
   fChecked = kFALSE;

   fItemName = name;
   fSubnames = subnames;
   fUserData = 0;

   fCpos  =
   fJmode = 0;

   fActive = kFALSE;

   fFontStruct = GetDefaultFontStruct();
   fNormGC     = GetDefaultGC()();

   Int_t max_ascent, max_descent;
   fTWidth = gVirtualX->TextWidth(fFontStruct,
                                  fItemName ? fItemName->GetString() : "",
                                  fItemName ? fItemName->GetLength() : 0);
   gVirtualX->GetFontProperties(fFontStruct, max_ascent, max_descent);
   fTHeight = max_ascent + max_descent;

   if (fSubnames) {
      Int_t i;
      for (i = 0; fSubnames[i] != 0; ++i)
         ;
      fCtw = new int[i+1];
      fCtw[i] = 0;
      for (i = 0; fSubnames[i] != 0; ++i) {
         fCtw[i] = gVirtualX->TextWidth(fFontStruct, fSubnames[i]->GetString(),
                                        fSubnames[i]->GetLength());
      }
   } else {
      fCtw = 0;
   }

   fViewMode = (EListViewMode)-1;
   SetViewMode(viewMode);
}

//______________________________________________________________________________
TGLVEntry::TGLVEntry(const TGLVContainer *p, const TString& name,
                     const TString& cname, TGString **subnames,
                     UInt_t options, Pixel_t back) :
   TGFrame(p, 10, 10, options, back)
{
   // Create a list view item.
   //
   // name - is name of item.
   // cname - is name of icon. In most cases this is class name of object
   //         associated with this item.

   fSelPic = 0;

   fCurrent  =
   fBigPic   = fClient->GetMimeTypeList()->GetIcon(cname, kFALSE);
   fCheckMark  = fClient->GetPicture("checkmark_t.xpm");
   fChecked = kFALSE;

   if (!fBigPic) {
      fBigPic = fClient->GetPicture("doc_s.xpm");
   }
   fSmallPic = fClient->GetMimeTypeList()->GetIcon(cname, kTRUE);
   if (!fSmallPic) {
      fSmallPic = fClient->GetPicture("doc_t.xpm");
   }

   fItemName = new TGString(name);
   fSubnames = subnames;
   fUserData = 0;

   fCpos  =
   fJmode = 0;

   fActive = kFALSE;

   fFontStruct = GetDefaultFontStruct();
   fNormGC     = GetDefaultGC()();

   Int_t max_ascent, max_descent;
   fTWidth = gVirtualX->TextWidth(fFontStruct, fItemName->GetString(), fItemName->GetLength());
   gVirtualX->GetFontProperties(fFontStruct, max_ascent, max_descent);
   fTHeight = max_ascent + max_descent;

   if (fSubnames) {
      Int_t i;
      for (i = 0; fSubnames[i] != 0; ++i)
         ;
      fCtw = new int[i+1];
      fCtw[i] = 0;
      for (i = 0; fSubnames[i] != 0; ++i) {
         fCtw[i] = gVirtualX->TextWidth(fFontStruct, fSubnames[i]->GetString(),
                                        fSubnames[i]->GetLength());
      }
   } else {
      fCtw = 0;
   }

   fViewMode = (EListViewMode)-1;
   SetViewMode((EListViewMode)p->GetViewMode());
}

//______________________________________________________________________________
TGLVEntry::~TGLVEntry()
{
   // Delete a list view item.

   if (fItemName) delete fItemName;
   if (fSelPic) delete fSelPic;
   if (fSubnames) {
      for (Int_t i = 0; fSubnames[i] != 0; ++i) delete fSubnames[i];
      delete [] fSubnames;
      delete [] fCtw;
   }
}

//______________________________________________________________________________
void TGLVEntry::SetSubnames(const char* n1,const char* n2,const char* n3,
                            const char* n4,const char* n5,const char* n6,
                            const char* n7,const char* n8,const char* n9,
                            const char* n10,const char* n11,const char* n12)
{
   // Sets new subnames.

   if (fSubnames) {
      for (Int_t i = 0; fSubnames[i] != 0; ++i) delete fSubnames[i];
      delete [] fSubnames;
      delete [] fCtw;
   }

   Int_t ncol = 0;
   fSubnames = 0;

   if (n12 && strlen(n12)) ncol=12;
   else if (n11 && strlen(n11)) ncol=11;
   else if (n10 && strlen(n10)) ncol=10;
   else if (n9 && strlen(n9)) ncol=9;
   else if (n8 && strlen(n8)) ncol=8;
   else if (n7 && strlen(n7)) ncol=7;
   else if (n6 && strlen(n6)) ncol=6;
   else if (n5 && strlen(n5)) ncol=5;
   else if (n4 && strlen(n4)) ncol=4;
   else if (n3 && strlen(n3)) ncol=3;
   else if (n2 && strlen(n2)) ncol=2;
   else if (n1 && strlen(n1)) ncol=1;

   if (!ncol) return;

   fSubnames = new TGString* [ncol+1];

   if (ncol>11) fSubnames[11] = new TGString(n12);
   if (ncol>10) fSubnames[10] = new TGString(n11);
   if (ncol>9) fSubnames[9] = new TGString(n10);
   if (ncol>8) fSubnames[8] = new TGString(n9);
   if (ncol>7) fSubnames[7] = new TGString(n8);
   if (ncol>6) fSubnames[6] = new TGString(n7);
   if (ncol>5) fSubnames[5] = new TGString(n6);
   if (ncol>4) fSubnames[4] = new TGString(n5);
   if (ncol>3) fSubnames[3] = new TGString(n4);
   if (ncol>2) fSubnames[2] = new TGString(n3);
   if (ncol>1) fSubnames[1] = new TGString(n2);
   if (ncol>0) fSubnames[0] = new TGString(n1);
   fSubnames[ncol] = 0;

   fCtw = new int[ncol];
   fCtw[ncol-1] = 0;

   for (int i = 0; i<ncol; i++) {
      fCtw[i] = gVirtualX->TextWidth(fFontStruct, fSubnames[i]->GetString(),
                                     fSubnames[i]->GetLength());
   }
}

//______________________________________________________________________________
void TGLVEntry::Activate(Bool_t a)
{
   // Make list view item active.

   if (fActive == a) return;
   fActive = a;

   if (fActive) {
      fSelPic = new TGSelectedPicture(gClient, fCurrent);
   } else {
      if (fSelPic) delete fSelPic;
      fSelPic = 0;
   }
   DoRedraw();
}

//______________________________________________________________________________
void TGLVEntry::SetViewMode(EListViewMode viewMode)
{
   // Set the view mode for this list item.

   if (viewMode != fViewMode) {
      fViewMode = viewMode;
      if (viewMode == kLVLargeIcons)
         fCurrent = fBigPic;
      else
         fCurrent = fSmallPic;
      if (fActive) {
         if (fSelPic) delete fSelPic;
         fSelPic = new TGSelectedPicture(gClient, fCurrent);
      }
      gVirtualX->ClearWindow(fId);
      Resize(GetDefaultSize());
      fClient->NeedRedraw(this);
   }
}

//______________________________________________________________________________
void TGLVEntry::SetPictures(const TGPicture *bigpic, const TGPicture *smallpic)
{
   // change pictures

   if (!bigpic || !smallpic) return;

   gVirtualX->ClearWindow(fId);

   fBigPic = bigpic;
   fSmallPic = smallpic;
   fCurrent = fViewMode == kLVLargeIcons ? fBigPic : fSmallPic;

   if (fSelPic) delete fSelPic;
   fSelPic = new TGSelectedPicture(gClient, fCurrent);

   Resize();
   gVirtualX->ClearWindow(fId);
}

//______________________________________________________________________________
void TGLVEntry::DoRedraw()
{
   // Redraw list view item.
   // List view item is placed and layouted in the container frame,
   // but is drawn in viewport.

   DrawCopy(fId, 0, 0);
}

//______________________________________________________________________________
void TGLVEntry::DrawCopy(Handle_t id, Int_t x, Int_t y)
{
   // Draw list view item in other window.
   // List view item is placed and layout in the container frame,
   // but is drawn in viewport.

   Int_t ix, iy, lx, ly;
   Int_t max_ascent, max_descent;

   gVirtualX->GetFontProperties(fFontStruct, max_ascent, max_descent);
   fTWidth = gVirtualX->TextWidth(fFontStruct, fItemName->GetString(), fItemName->GetLength());
   fTHeight = max_ascent + max_descent;

   if (fViewMode == kLVLargeIcons) {
      ix = (fWidth - fCurrent->GetWidth()) >> 1;
      iy = 0;
      lx = (fWidth - fTWidth) >> 1;
      ly = fHeight - (fTHeight + 1) - 2;
   } else {
      ix = 0;
      iy = (fHeight - fCurrent->GetHeight()) >> 1;
      lx = fCurrent->GetWidth() + 2;
      ly = (fHeight - (fTHeight + 1)) >> 1;
   }

   if ((fChecked) && (fCheckMark)) {
      if (fViewMode == kLVLargeIcons) {
         fCheckMark->Draw(id, fNormGC, x + ix + 8, y + iy + 8);
         gVirtualX->SetForeground(fNormGC, fgWhitePixel);
         gVirtualX->FillRectangle(id, fNormGC, x + lx, y + ly, fTWidth, fTHeight + 1);
         gVirtualX->SetForeground(fNormGC, fgBlackPixel);
      }
      else {
         fCheckMark->Draw(id, fNormGC, x + ix, y + iy);
         gVirtualX->SetForeground(fNormGC, fgWhitePixel);
         gVirtualX->FillRectangle(id, fNormGC, x + lx, y + ly, fTWidth, fTHeight + 1);
         gVirtualX->SetForeground(fNormGC, fgBlackPixel);
      }
   }
   // This if tries to print the elements with ... appened at the end if
   // the widht of the string is longer than that of the column
   if (fViewMode == kLVDetails && fSubnames && fCpos && fJmode && fCtw) {
      TString tmpString = *fItemName;
      Int_t ftmpWidth = gVirtualX->TextWidth(fFontStruct, tmpString,
                                             tmpString.Length());
      if ( ftmpWidth > (fCpos[0] - lx) ) {
         for (Int_t j = fItemName->Length() - 1 ; j > 0; j--) {
            tmpString =  (*fItemName)(0,j) + "...";
            ftmpWidth = gVirtualX->TextWidth(GetDefaultFontStruct(), tmpString,
                                             tmpString.Length());
            if ( ftmpWidth <= (fCpos[0] - lx) ) {
               break;
            }
         }
      }
      if (fActive) {
         if (fSelPic) fSelPic->Draw(id, fNormGC, x + ix, y + iy);
         gVirtualX->SetForeground(fNormGC, fgDefaultSelectedBackground);
         gVirtualX->FillRectangle(id, fNormGC, x + lx, y + ly, ftmpWidth, fTHeight + 1);
         gVirtualX->SetForeground(fNormGC, fClient->GetResourcePool()->GetSelectedFgndColor());
      } else {
         fCurrent->Draw(id, fNormGC, x + ix, y + iy);
         gVirtualX->SetForeground(fNormGC, fgWhitePixel);
         gVirtualX->FillRectangle(id, fNormGC, x + lx, y + ly, ftmpWidth, fTHeight + 1);
         gVirtualX->SetForeground(fNormGC, fgBlackPixel);
      }

      TGString tmpTGString(tmpString);
      tmpTGString.Draw(id, fNormGC, x+lx, y+ly + max_ascent);
   } else {
      if (fActive) {
         if (fSelPic) fSelPic->Draw(id, fNormGC, x + ix, y + iy);
         gVirtualX->SetForeground(fNormGC, fgDefaultSelectedBackground);
         gVirtualX->FillRectangle(id, fNormGC, x + lx, y + ly, fTWidth, fTHeight + 1);
         gVirtualX->SetForeground(fNormGC, fClient->GetResourcePool()->GetSelectedFgndColor());
      } else {
         fCurrent->Draw(id, fNormGC, x + ix, y + iy);
         gVirtualX->SetForeground(fNormGC, fgWhitePixel);
         gVirtualX->FillRectangle(id, fNormGC, x + lx, y + ly, fTWidth, fTHeight + 1);
         gVirtualX->SetForeground(fNormGC, fgBlackPixel);
      }
      fItemName->Draw(id, fNormGC, x+lx, y+ly + max_ascent);
   }
   gVirtualX->SetForeground(fNormGC, fgBlackPixel);

   if (fViewMode == kLVDetails) {
      if (fSubnames && fCpos && fJmode && fCtw) {
         int i;

         // Again fixes the size of the strings
         for (i = 0; fSubnames[i] != 0; ++i) {
            TString tmpString = *fSubnames[i];
            Int_t ftmpWidth = gVirtualX->TextWidth(fFontStruct, tmpString,
                                                   tmpString.Length());
            if ( ftmpWidth > (fCpos[i+1] - fCpos[i]) ) {
               for (int j = fSubnames[i]->Length() - 1 ; j > 0; j--) {
                  tmpString =  (*fSubnames[i])(0,j) + "...";
                  ftmpWidth = gVirtualX->TextWidth(GetDefaultFontStruct(),
                                                   tmpString,
                                                   tmpString.Length());
                  if ( ftmpWidth <= (fCpos[i+1] - fCpos[i]) ) {
                     break;
                  }
               }
            }
            if (fCpos[i] == 0)
               break;
            if (fJmode[i] == kTextRight)
               lx = fCpos[i+1] - ftmpWidth - 2;
            else if (fJmode[i] == kTextCenterX)
               lx = (fCpos[i] + fCpos[i+1] - ftmpWidth) >> 1;
            else // default to TEXT_LEFT
               lx = fCpos[i] + 2;

            //if (x + lx < 0) continue; // out of left boundary or mess in name
            TGString tmpTGString(tmpString);
            tmpTGString.Draw(id, fNormGC, x + lx, y + ly + max_ascent);
         }
      }
   }
}

//______________________________________________________________________________
TGDimension TGLVEntry::GetDefaultSize() const
{
   // Get default size of list item.

   TGDimension size;
   TGDimension isize(fCurrent ? fCurrent->GetWidth() : 0,
                     fCurrent ? fCurrent->GetHeight() : 0);
   TGDimension lsize(fTWidth, fTHeight+1);

   switch (fViewMode) {
      default:
      case kLVLargeIcons:
         size.fWidth = TMath::Max(isize.fWidth, lsize.fWidth);
         size.fHeight = isize.fHeight + lsize.fHeight + 6;
         break;

      case kLVSmallIcons:
      case kLVList:
      case kLVDetails:
         size.fWidth = isize.fWidth + lsize.fWidth + 4;
         size.fHeight = TMath::Max(isize.fHeight, lsize.fHeight);
         break;
   }
   return size;
}

//______________________________________________________________________________
FontStruct_t TGLVEntry::GetDefaultFontStruct()
{
   // Return the default font structure in use.

   if (!fgDefaultFont)
      fgDefaultFont = gClient->GetResourcePool()->GetIconFont();
   return fgDefaultFont->GetFontStruct();
}

//______________________________________________________________________________
const TGGC &TGLVEntry::GetDefaultGC()
{
   // Return the default graphics context in use.

   if (!fgDefaultGC) {
      fgDefaultGC = new TGGC(*gClient->GetResourcePool()->GetFrameGC());
      fgDefaultGC->SetFont(fgDefaultFont->GetFontHandle());
   }
   return *fgDefaultGC;
}


//______________________________________________________________________________
TGLVContainer::TGLVContainer(const TGWindow *p, UInt_t w, UInt_t h,
                             UInt_t options, ULong_t back) :
   TGContainer(p, w, h, options, back)
{
   // Create a list view container. This is the (large) frame that contains
   // all the list items. It will be shown through a TGViewPort (which is
   // created by the TGCanvas derived TGListView).

   fListView = 0;
   fLastActive = 0;
   fCpos = fJmode = 0;
   fMultiSelect = kFALSE;

   fViewMode = kLVLargeIcons;
   fItemLayout = new TGLayoutHints(kLHintsExpandY | kLHintsCenterX);

   SetLayoutManager(new TGTileLayout(this, 8));
}

//______________________________________________________________________________
TGLVContainer::TGLVContainer(TGCanvas *p,UInt_t options, ULong_t back) :
   TGContainer(p,options, back)
{
   // Create a list view container. This is the (large) frame that contains
   // all the list items. It will be shown through a TGViewPort (which is
   // created by the TGCanvas derived TGListView).

   fListView = 0;
   fLastActive = 0;
   fCpos = fJmode = 0;
   fMultiSelect = kFALSE;

   fViewMode = kLVLargeIcons;
   fItemLayout = new TGLayoutHints(kLHintsExpandY | kLHintsCenterX);

   SetLayoutManager(new TGTileLayout(this, 8));

   if (p->InheritsFrom(TGListView::Class())) fListView = (TGListView*)p;
}

//______________________________________________________________________________
TGLVContainer::~TGLVContainer()
{
   // Delete list view container.

   if (!MustCleanup()) {
      RemoveAll();
      delete fItemLayout;
   }
}

//______________________________________________________________________________
void  TGLVContainer::SetColHeaders(const char* n1,const char* n2,const char* n3,
                                   const char* n4,const char* n5,const char* n6,
                                   const char* n7,const char* n8,const char* n9,
                                   const char* n10,const char* n11,const char* n12)
{
   // set columns headers

   if (!fListView) return;

   Int_t ncol = -1;
   if (n12 && strlen(n12)) ncol=12;
   else if (n11 && strlen(n11)) ncol=11;
   else if (n10 && strlen(n10)) ncol=10;
   else if (n9 && strlen(n9)) ncol=9;
   else if (n8 && strlen(n8)) ncol=8;
   else if (n7 && strlen(n7)) ncol=7;
   else if (n6 && strlen(n6)) ncol=6;
   else if (n5 && strlen(n5)) ncol=5;
   else if (n4 && strlen(n4)) ncol=4;
   else if (n3 && strlen(n3)) ncol=3;
   else if (n2 && strlen(n2)) ncol=2;
   else if (n1 && strlen(n1)) ncol=1;

   if (ncol<0) return;

   fListView->SetHeaders(ncol);
   if (ncol>0) fListView->SetHeader(n1, kTextCenterX, kTextLeft , 0);
   if (ncol>1) fListView->SetHeader(n2, kTextCenterX, kTextLeft , 1);
   if (ncol>2) fListView->SetHeader(n3, kTextCenterX, kTextLeft , 2);
   if (ncol>3) fListView->SetHeader(n4, kTextCenterX, kTextLeft , 3);
   if (ncol>4) fListView->SetHeader(n5, kTextCenterX, kTextLeft , 4);
   if (ncol>5) fListView->SetHeader(n6, kTextCenterX, kTextLeft , 5);
   if (ncol>6) fListView->SetHeader(n7, kTextCenterX, kTextLeft , 6);
   if (ncol>7) fListView->SetHeader(n8, kTextCenterX, kTextLeft , 7);
   if (ncol>8) fListView->SetHeader(n9, kTextCenterX, kTextLeft , 8);
   if (ncol>9) fListView->SetHeader(n10, kTextCenterX, kTextLeft , 9);
   if (ncol>10) fListView->SetHeader(n11, kTextCenterX, kTextLeft , 10);
   if (ncol>11) fListView->SetHeader(n12, kTextCenterX, kTextLeft , 11);

   fListView->Layout();
}

//______________________________________________________________________________
void TGLVContainer::SetViewMode(EListViewMode viewMode)
{
   // Set list view mode for container.

   if (fViewMode != viewMode) {
      TGLayoutHints *oldLayout = fItemLayout;

      EListViewMode old = fViewMode;
      fViewMode = viewMode;
      if (fListView) fListView->SetViewMode(viewMode);

      if (viewMode == kLVLargeIcons)
         fItemLayout = new TGLayoutHints(kLHintsExpandY | kLHintsCenterX);
      else
         fItemLayout = new TGLayoutHints(kLHintsLeft | kLHintsCenterY);

      TGFrameElement *el;
      TIter next(fList);
      while ((el = (TGFrameElement *) next())) {
         el->fLayout = fItemLayout;
         ((TGLVEntry *) el->fFrame)->SetViewMode(viewMode);
      }
      delete oldLayout;

      switch (viewMode) {
         default:
         case kLVLargeIcons:
            SetLayoutManager(new TGTileLayout(this, 8));
            break;

         case kLVSmallIcons:
            SetLayoutManager(new TGTileLayout(this, 2));
            break;

         case kLVList:
            SetLayoutManager(new TGListLayout(this, 2));
            break;

         case kLVDetails:
            SetLayoutManager(new TGListDetailsLayout(this, 2));
            break;
      }

      TGCanvas *canvas = (TGCanvas *) this->GetParent()->GetParent();

      // layout and adjust position after layout
      UInt_t height = fHeight;
      UInt_t width = fWidth;
      TGPosition pos = GetPagePosition();
      canvas->Layout();
      pos.fX = (pos.fX*fWidth)/width;
      pos.fY = (pos.fY*fHeight)/height;

      if (old == kLVList) { // switch x <-> y
         SetVsbPosition(pos.fX);
         SetHsbPosition(0);
      } else if (fViewMode == kLVList) {
         SetHsbPosition(pos.fY);
      } else {
         SetVsbPosition(pos.fY);
         SetHsbPosition(pos.fX);
      }
   }
}

//______________________________________________________________________________
void TGLVContainer::SetColumns(Int_t *cpos, Int_t *jmode)
{
   // Set column information for list items.

   fCpos  = cpos;
   fJmode = jmode;

   TGFrameElement *el;
   TIter next(fList);
   while ((el = (TGFrameElement *) next())) {
      ((TGLVEntry *) el->fFrame)->SetColumns(fCpos, fJmode);
   }
   Layout();
}

//______________________________________________________________________________
TGDimension TGLVContainer::GetMaxItemSize() const
{
   // Get size of largest item in container.

   TGDimension csize, maxsize(0,0);

   TGFrameElement *el;
   TIter next(fList);
   while ((el = (TGFrameElement *) next())) {
      csize = el->fFrame->GetDefaultSize();
      maxsize.fWidth  = TMath::Max(maxsize.fWidth, csize.fWidth);
      maxsize.fHeight = TMath::Max(maxsize.fHeight, csize.fHeight);
   }
   if (fViewMode == kLVLargeIcons) {
      maxsize.fWidth  += 8;
      maxsize.fHeight += 8;
   } else {
      maxsize.fWidth  += 2;
      maxsize.fHeight += 2;
   }
   return maxsize;
}

//______________________________________________________________________________
Int_t TGLVContainer::GetMaxSubnameWidth(Int_t idx) const
{
   // Get width of largest subname in container.

   if (idx == 0) {
      return GetMaxItemSize().fWidth;
   }

   Int_t width, maxwidth = 0;

   TGFrameElement *el;
   TIter next(fList);
   while ((el = (TGFrameElement *) next())) {
      TGLVEntry *entry = (TGLVEntry *) el->fFrame;
      width = entry->GetSubnameWidth(idx-1);
      maxwidth = TMath::Max(maxwidth, width);
   }
   return maxwidth;
}

//______________________________________________________________________________
void TGLVContainer::RemoveItemWithData(void *userData)
{
   // Remove item with fUserData == userData from container.

   TGFrameElement *el;
   TIter next(fList);
   while ((el = (TGFrameElement *) next())) {
      TGLVEntry *f = (TGLVEntry *) el->fFrame;
      if (f->GetUserData() == userData) {
         RemoveItem(f);
         break;
      }
   }
}

//______________________________________________________________________________
void TGLVContainer::ActivateItem(TGFrameElement* el)
{
   // Select/activate item.

   TGContainer::ActivateItem(el);
   fLastActive = (TGLVEntry*)el->fFrame;
}

//______________________________________________________________________________
void TGLVContainer::DeActivateItem(TGFrameElement* el)
{
   // Unselect/deactivate item.

   TGContainer::DeActivateItem(el);
   fLastActive = (TGLVEntry*)el->fFrame;
}

//______________________________________________________________________________
Bool_t TGLVContainer::HandleButton(Event_t* event)
{
   // Handle mouse button event in container.

   if ( !fMultiSelect ) {
      return TGContainer::HandleButton(event);
   }

   Int_t total = 0, selected = fSelected, page = 0;

   TGPosition pos = GetPagePosition();
   TGDimension dim = GetPageDimension();
   Int_t newpos;
   page = dim.fHeight/4;

   if (event->fCode == kButton4) {
      //scroll up
      newpos = pos.fY - page;
      if (newpos < 0) newpos = 0;
      fCanvas->SetVsbPosition(newpos);
      return kTRUE;
   }
   if (event->fCode == kButton5) {
      // scroll down
      newpos = fCanvas->GetVsbPosition() + page;
      fCanvas->SetVsbPosition(newpos);
      return kTRUE;
   }

   Int_t xx = pos.fX + event->fX; // translate coordinates
   Int_t yy = pos.fY + event->fY;

   if (event->fType == kButtonPress) {
      gVirtualX->SetInputFocus(fId);

      fXp = pos.fX + event->fX;
      fYp = pos.fY + event->fY;

      TGFrameElement *el;
      TIter next(fList);
      Bool_t select_frame = kFALSE;

      if (event->fState & kKeyShiftMask) {
         Bool_t inSelection = kFALSE;
         TGLVEntry* last = fLastActive;

         while ((el = (TGFrameElement *) next())) {
            select_frame = kFALSE;

            if (!fMapSubwindows) {
               if ((Int_t(el->fFrame->GetY()) + (Int_t)el->fFrame->GetHeight() > yy ) &&
                  (Int_t(el->fFrame->GetX()) + (Int_t)el->fFrame->GetWidth() > xx ) &&
                  (Int_t(el->fFrame->GetY()) < yy) &&
                  (Int_t(el->fFrame->GetX()) < xx))  {
                  select_frame = kTRUE;
               }
            } else {
               if (el->fFrame->GetId() == (Window_t)event->fUser[0]) {
                  select_frame = kTRUE;
               }
            }

            if (select_frame || last==el->fFrame)
               inSelection = !inSelection;
            if (inSelection || select_frame) {
               if ( !el->fFrame->IsActive() ) {
                  selected++;
                  ActivateItem(el);
               }
               Clicked(el->fFrame, event->fCode);
               Clicked(el->fFrame, event->fCode, event->fXRoot, event->fYRoot);
            }
            total++;
         }
      } else if (event->fState & kKeyControlMask) {
         // DO NOTHING!
      } else {
         UnSelectAll();
         total = selected = 0;
      }

      select_frame = kFALSE;
      while ((el = (TGFrameElement *) next())) {
         select_frame = kFALSE;

         if (!fMapSubwindows) {
            if ((Int_t(el->fFrame->GetY()) + (Int_t)el->fFrame->GetHeight() > yy ) &&
               (Int_t(el->fFrame->GetX()) + (Int_t)el->fFrame->GetWidth() > xx ) &&
               (Int_t(el->fFrame->GetY()) < yy) &&
               (Int_t(el->fFrame->GetX()) < xx))  {
               select_frame = kTRUE;
            }
         } else {
            if (el->fFrame->GetId() == (Window_t)event->fUser[0]) {
               select_frame = kTRUE;
            }
         }

         if (select_frame) {
            if ( el->fFrame->IsActive() ) {
               selected--;
               DeActivateItem(el);
            } else {
               selected++;
               ActivateItem(el);
            }
            Clicked(el->fFrame, event->fCode);
            Clicked(el->fFrame, event->fCode, event->fXRoot, event->fYRoot);
         }
         total++;
      }

      if (fTotal != total || fSelected != selected) {
         fTotal = total;
         fSelected = selected;
         SendMessage(fMsgWindow, MK_MSG(kC_CONTAINER, kCT_SELCHANGED),
                     fTotal, fSelected);
      }

      if ( selected == 0 ) {
         fDragging = kTRUE;
         fX0 = fXf = fXp;
         fY0 = fYf = fYp;
         //if (fMapSubwindows)
            gVirtualX->DrawRectangle(fId, GetLineGC()(), fX0, fY0, fXf-fX0,
                                     fYf-fY0);
      }
   }

   if (event->fType == kButtonRelease) {
      gVirtualX->SetInputFocus(fId);

      if (fDragging) {
         fDragging = kFALSE;
         fScrolling = kFALSE;

         if (gSystem) gSystem->RemoveTimer(fScrollTimer);
         //if (fMapSubwindows)
            gVirtualX->DrawRectangle(fId, GetLineGC()(), fX0, fY0, fXf-fX0,
                                     fYf-fY0);
      } else {
         SendMessage(fMsgWindow, MK_MSG(kC_CONTAINER, kCT_ITEMCLICK),
                     event->fCode, (event->fYRoot << 16) | event->fXRoot);
      }
   }
   fClient->NeedRedraw(this);
   return kTRUE;
}

//______________________________________________________________________________
TList *TGLVContainer::GetSelectedItems()
{
   // Get list of selected items in container.

   TGFrameElement *el;
   TIter next(fList);
   TList *ret = new TList();

   while ((el = (TGFrameElement *) next())) {
      if (el->fFrame->IsActive()) {
         ret->Add(new TObjString(((TGLVEntry*)el->fFrame)->GetItemName()->GetString()));
      }
   }
   return ret;
}

//______________________________________________________________________________
void TGLVContainer::LineLeft(Bool_t select)
{
   // Move current position one column left.

   TGPosition pos = GetPagePosition();
   TGDimension dim = GetPageDimension();

   TGFrameElement *fe = (TGFrameElement*)fList->First();
   if (!fe) return; // empty list

   TGFrameElement *old = fLastActiveEl;

   if (old) DeActivateItem(old);   //
   else fLastActiveEl = fe;

   TGFrameElement *la = fLastActiveEl;

   TGDimension ms = fListView->GetMaxItemSize();
   Int_t dx = ms.fWidth;
   Int_t dy = ms.fHeight;
   Int_t y = la->fFrame->GetY();
   Int_t x = la->fFrame->GetX() - dx;

   Int_t hw = pos.fX + dim.fWidth;

   TGHScrollBar *hb = GetHScrollbar();
   if (x<=0 && (hb && !hb->IsMapped())) { // move to previous line
      x = hw;
      y = y - la->fFrame->GetDefaultHeight() - dy;
   }

   fe = FindFrame(x, y);
   if (!fe) fe = (TGFrameElement*)fList->First();

   if (!select) fSelected=1;

   ActivateItem(fe);
   AdjustPosition();
}

//______________________________________________________________________________
void TGLVContainer::LineRight(Bool_t select)
{
   // Move current position one column right.

   TGPosition pos = GetPagePosition();
   TGDimension dim = GetPageDimension();

   TGFrameElement *fe = (TGFrameElement*)fList->Last();
   if (!fe) return;

   TGFrameElement *old = fLastActiveEl;

   if (old) DeActivateItem(old);
   else fLastActiveEl = (TGFrameElement*)fList->First();

   TGDimension ms = fListView->GetMaxItemSize();
   Int_t dx = ms.fWidth;
   Int_t dy = ms.fHeight;

   Int_t y = fLastActiveEl->fFrame->GetY();
   Int_t x = fLastActiveEl->fFrame->GetX() + dx;

   Int_t hw = pos.fX + dim.fWidth - dx;

   TGHScrollBar *hb =  GetHScrollbar();
   if (x >= hw && (hb && !hb->IsMapped())) { // move one line down
      x = 0;
      y = y + dy;
   }

   fe = FindFrame(x, y);
   if (!fe) fe = (TGFrameElement*)fList->Last();
   if (!select) fSelected = 1;

   ActivateItem(fe);
   AdjustPosition();
}

//______________________________________________________________________________
void TGLVContainer::LineUp(Bool_t select)
{
   // Make current position first line in window by scrolling up.

   TGFrameElement *fe = (TGFrameElement*)fList->First();
   if (!fe) return;

   TGFrameElement *old = fLastActiveEl;

   if (old) {
      DeActivateItem(old);
   } else {
      fLastActiveEl = (TGFrameElement*)fList->First();
   }

   TGDimension ms = fListView->GetMaxItemSize();
   Int_t dy = ms.fHeight;

   Int_t y = fLastActiveEl->fFrame->GetY() - dy;
   Int_t x = fLastActiveEl->fFrame->GetX();

   fe = FindFrame(x, y);
   if (!fe) fe = (TGFrameElement*)fList->First();
   if (fe->fFrame->GetY() > fLastActiveEl->fFrame->GetY()) fe = fLastActiveEl;
   if (!select) fSelected = 1;

   ActivateItem(fe);
   AdjustPosition();
}

//______________________________________________________________________________
void TGLVContainer::LineDown(Bool_t select)
{
   // Move one line down.

   TGFrameElement *fe = (TGFrameElement*)fList->Last();
   if (!fe) return;

   TGFrameElement *old = fLastActiveEl;

   if (old) DeActivateItem(old);
   else fLastActiveEl = (TGFrameElement*)fList->First();

   TGDimension ms = fListView->GetMaxItemSize();
   Int_t dy = ms.fHeight;

   Int_t y = fLastActiveEl->fFrame->GetY() + dy;
   Int_t x = fLastActiveEl->fFrame->GetX();

   fe = FindFrame(x, y);
   if (!fe) fe = (TGFrameElement*)fList->Last();
   if (fe->fFrame->GetY() < fLastActiveEl->fFrame->GetY()) fe = fLastActiveEl;
   if (!select) fSelected = 1;

   ActivateItem(fe);
   AdjustPosition();
}


//______________________________________________________________________________
TGDimension TGLVContainer::GetPageDimension() const
{
   // Returns page dimension.

   TGDimension ret;
   if (!fViewPort) return ret;

   ret.fWidth = fViewPort->GetWidth();
   ret.fHeight = fViewPort->GetHeight();
   return ret;
}

//______________________________________________________________________________
TGListView::TGListView(const TGWindow *p, UInt_t w, UInt_t h,
                       UInt_t options, ULong_t back) :
   TGCanvas(p, w, h, options, back)
{
   // Create a list view widget.

   fViewMode    = kLVLargeIcons;
   fNColumns    = 0;
   fColumns     = 0;
   fJmode       = 0;
   fColHeader   = 0;
   fColNames    = 0;
   fSplitHeader = 0;
   fJustChanged = kFALSE;
   fMinColumnSize = 25;
   fFontStruct = GetDefaultFontStruct();
   fNormGC     = GetDefaultGC()();
   if (fHScrollbar)
      fHScrollbar->Connect("PositionChanged(Int_t)", "TGListView",
                           this, "ScrollHeader(Int_t)");
   fHeader = new TGHeaderFrame(fVport, 20, 20, kChildFrame | kFixedWidth);

   SetDefaultHeaders();
}

//______________________________________________________________________________
TGListView::~TGListView()
{
   // Delete a list view widget.

   if (fNColumns) {
      delete [] fColumns;
      delete [] fJmode;
      for (int i = 0; i < fNColumns; i++) {
         delete fColHeader[i];
         delete fSplitHeader[i];
      }
      delete [] fColHeader;
      delete [] fColNames;
      delete [] fSplitHeader;
      delete fHeader;
   }
}

//______________________________________________________________________________
void TGListView::ScrollHeader(Int_t pos)
{
   // Scroll header buttons with horizontal scrollbar

   Int_t  i, xl = - pos;
   if (fViewMode == kLVDetails) {
      for (i = 0; i < fNColumns-1; ++i) {
         fColHeader[i]->Move(xl, 0);
         xl += fColHeader[i]->GetWidth();
         fSplitHeader[i]->Move(xl,fSplitHeader[i]->GetHeight());
      }
      fColHeader[i]->Move(xl, 0);
      xl += fColHeader[i]->GetWidth();
      fSplitHeader[i]->Move(xl,fSplitHeader[i]->GetHeight());
   }
}

//______________________________________________________________________________
void TGListView::SetHeaders(Int_t ncolumns)
{
   // Set number of headers, i.e. columns that will be shown in detailed view.
   // This method must be followed by exactly ncolumns SetHeader() calls,
   // making sure that every header (i.e. idx) is set (for and example see
   // SetDefaultHeaders()).

   if (ncolumns <= 0) {
      Error("SetHeaders", "number of columns must be > 0");
      return;
   }

   if (fNColumns) {
      delete [] fColumns;
      delete [] fJmode;
      for (int i = 0; i < fNColumns; i++) {
         if (fColHeader[i]) fColHeader[i]->DestroyWindow();
         delete fColHeader[i];
         delete fSplitHeader[i];
      }
      delete [] fColHeader;
      delete [] fSplitHeader;
      delete [] fColNames;
   }

   fNColumns  = ncolumns+1;    // one extra for the blank filler header
   fColumns   = new int[fNColumns];
   fJmode     = new int[fNColumns];
   fColHeader = new TGTextButton* [fNColumns];
   fColNames  = new TString [fNColumns];
   fSplitHeader = new TGVFileSplitter* [fNColumns];

   for (int i = 0; i < fNColumns; i++) {
      fColHeader[i] = 0;
      fJmode[i] = kTextLeft;
      fSplitHeader[i] = new TGVFileSplitter(fHeader, 10);
      fSplitHeader[i]->Connect("LayoutListView()", "TGListView",
                               this, "Layout()");
      fSplitHeader[i]->Connect("LayoutHeader(TGFrame *)", "TGListView",
                               this, "LayoutHeader(TGFrame *)");
      fSplitHeader[i]->Connect("DoubleClicked(TGVFileSplitter*)", "TGListView",
                               this, "SetDefaultColumnWidth(TGVFileSplitter*)");
   }

   // create blank filler header
   fColNames[fNColumns-1] = "";
   fColHeader[fNColumns-1] = new TGTextButton(fHeader, new TGHotString(""), -1,
                                    fNormGC, fFontStruct, kRaisedFrame |
                                    kDoubleBorder | kFixedWidth);
   fColHeader[fNColumns-1]->Associate(this);
   fColHeader[fNColumns-1]->SetTextJustify(kTextCenterX | kTextCenterY);
   fColHeader[fNColumns-1]->SetState(kButtonDisabled);
   fColHeader[fNColumns-1]->SetState(kButtonDisabled);
   fJmode[fNColumns-1]   = kTextCenterX;
   fColumns[fNColumns-1] = 0;
   fSplitHeader[fNColumns-1]->SetFrame(fColHeader[fNColumns-1], kTRUE);
   fHeader->SetColumnsInfo(fNColumns, fColHeader, fSplitHeader);
   fJustChanged = kTRUE;
}

//______________________________________________________________________________
void TGListView::SetHeader(const char *s, Int_t hmode, Int_t cmode, Int_t idx)
{
   // Set header button idx [0-fNColumns>, hmode is the x text alignmode
   // (ETextJustification) for the header text and cmode is the x text
   // alignmode for the item text.

   if (idx < 0 || idx >= fNColumns-1) {
      Error("SetHeader", Form("header index must be [0 - %d>", fNColumns-1));
      return;
   }
   delete fColHeader[idx];

   fColNames[idx] = s;
   fColHeader[idx] = new TGTextButton(fHeader, new TGHotString(s), idx,
                                      fNormGC, fFontStruct, kRaisedFrame |
                                      kDoubleBorder | kFixedWidth);
   TString txt = s;
   txt.ToLower();
   if (txt.Contains("modified")) txt += " date";
   if (txt.Contains("attributes")) txt = "type";
   fColHeader[idx]->SetToolTipText(Form("Click to sort by %s", txt.Data()));
   fColHeader[idx]->Associate(this);
   fColHeader[idx]->SetTextJustify(hmode | kTextCenterY);
   fSplitHeader[idx]->SetFrame(fColHeader[idx], kTRUE);

   // fJmode and fColumns contain values for columns idx > 0. idx==0 is
   // the small icon with the object name
   if (idx > 0)
      fJmode[idx-1] = cmode;

   if (!fColHeader[0]) return;
   int xl = fColHeader[0]->GetDefaultWidth() + 10 + fSplitHeader[0]->GetDefaultWidth();
   for (int i = 1; i < fNColumns; i++) {
      fColumns[i-1] = xl;
      if (!fColHeader[i]) break;
      xl += fColHeader[i]->GetDefaultWidth() + fSplitHeader[i]->GetDefaultWidth();
   }
}

//______________________________________________________________________________
const char *TGListView::GetHeader(Int_t idx) const
{
   // Returns name of header idx. If illegal idx or header not set for idx
   // 0 is returned.

   if (idx >= 0 && idx < fNColumns-1 && fColHeader[idx])
      return (const char*) fColNames[idx];
   return 0;
}

//______________________________________________________________________________
void TGListView::SetDefaultHeaders()
{
   // Default headers are: Name, Attributes, Size, Owner, Group, Modified.
   // The default is good for file system items.

   SetHeaders(6);
   SetHeader("Name",       kTextLeft,    kTextLeft,    0);
   SetHeader("Attributes", kTextCenterX, kTextCenterX, 1);
   SetHeader("Size",       kTextRight,   kTextRight,   2);
   SetHeader("Owner",      kTextCenterX, kTextCenterX, 3);
   SetHeader("Group",      kTextCenterX, kTextCenterX, 4);
   SetHeader("Modified",   kTextCenterX, kTextCenterX, 5);
}

//______________________________________________________________________________
void TGListView::SetViewMode(EListViewMode viewMode)
{
   // Set list view mode.

   TGLVContainer *container;

   if (fViewMode != viewMode) {
      fJustChanged = kTRUE;
      fViewMode = viewMode;
      container = (TGLVContainer *) fVport->GetContainer();
      if (container) container->SetViewMode(viewMode);
      Layout();
   }
}

//______________________________________________________________________________
void TGListView::SetContainer(TGFrame *f)
{
   // Set list view container. Container must be at least of type
   // TGLVContainer.

   if (f->InheritsFrom(TGLVContainer::Class())) {
      TGCanvas::SetContainer(f);
      ((TGLVContainer *) f)->SetColumns(fColumns, fJmode);
      ((TGLVContainer *) f)->SetListView(this);
   } else
      Error("SetContainer", "frame must inherit from TGLVContainer");
}

//______________________________________________________________________________
void TGListView::SetIncrements(Int_t hInc, Int_t vInc)
{
   // Set horizontal and vertical scrollbar increments.

   fHScrollbar->SetSmallIncrement(hInc);
   fVScrollbar->SetSmallIncrement(vInc);
}

//______________________________________________________________________________
void TGListView::SetDefaultColumnWidth(TGVFileSplitter* splitter)
{
   // Set default column width of the columns headers.

   TGLVContainer *container = (TGLVContainer *) fVport->GetContainer();

   if (!container) {
      Error("SetDefaultColumnWidth", "no listview container set yet");
      return;
   }
   container->ClearViewPort();

   for (int i = 0; i < fNColumns; ++i) {
      if ( fSplitHeader[i] == splitter ) {
         TString dt = fColHeader[i]->GetString();
         UInt_t bsize = gVirtualX->TextWidth(fColHeader[i]->GetFontStruct(),
                                             dt.Data(), dt.Length());
         UInt_t w = TMath::Max(fColHeader[i]->GetDefaultWidth(), bsize + 20);
         if (i == 0) w = TMath::Max(fMaxSize.fWidth + 10, w);
         if (i > 0)  w = TMath::Max(container->GetMaxSubnameWidth(i) + 40, (Int_t)w);
         fColHeader[i]->Resize(w, fColHeader[i]->GetHeight());
         Layout();
      }
   }
}

//______________________________________________________________________________
void TGListView::ResizeColumns()
{
   // Resize column headers to show whole item names. 

   for (int i = 0; i < fNColumns; ++i) {
      TGLVContainer *container = (TGLVContainer *) fVport->GetContainer();
      if (!container) {
         Error("ResizeColumns", "no listview container set yet");
         return;
      }
      fMaxSize = container->GetMaxItemSize();
      SetDefaultColumnWidth(fSplitHeader[i]);
   }
}

//______________________________________________________________________________
void TGListView::Layout()
{
   // Layout list view components (container and contents of container).

   Int_t  i, xl = 0;
   UInt_t w, h = 0;

   TGLVContainer *container = (TGLVContainer *) fVport->GetContainer();

   if (!container) {
      Error("Layout", "no listview container set yet");
      return;
   }

   fMaxSize = container->GetMaxItemSize();

   if (fViewMode == kLVDetails) {
      h = fColHeader[0]->GetDefaultHeight()-4;
      fHeader->MoveResize(0, 0, fWidth, h);
      fHeader->MapWindow();
      for (i = 0; i < fNColumns-1; ++i) {
         fColHeader[i]->SetText(fColNames[i]);

         if ( fJustChanged ) {
            w = fColHeader[i]->GetDefaultWidth();
            if (i == 0) w = TMath::Max(fMaxSize.fWidth + 10, w);
            if (i > 0)  w = TMath::Max(container->GetMaxSubnameWidth(i) + 40, (Int_t)w);
         } else {
            w = fColHeader[i]->GetWidth();
         }
         w = TMath::Max( fMinColumnSize, (Int_t)w);
         if ( fColHeader[i]->GetDefaultWidth() > w ) {
            for (int j = fColNames[i].Length() - 1 ; j > 0; j--) {
               fColHeader[i]->SetText( fColNames[i](0,j) + "..." );
               if ( fColHeader[i]->GetDefaultWidth() < w )
                  break;
            }
         }

         fColHeader[i]->MoveResize(xl, 0, w, h);
         fColHeader[i]->MapWindow();
         xl += w;
         fSplitHeader[i]->Move(xl, 0);
         fSplitHeader[i]->MapWindow();
         fColumns[i] = xl-2;  // -2 is fSep in the layout routine
      }
      fColHeader[i]->MoveResize(xl, 0, fVport->GetWidth()-xl, h);
      fColHeader[i]->MapWindow();
      fSplitHeader[i]->Move(fVport->GetWidth(), fSplitHeader[i]->GetHeight());
      fSplitHeader[i]->MapWindow();
      fVScrollbar->RaiseWindow();

      container->SetColumns(fColumns, fJmode);

   } else {
      for (i = 0; i < fNColumns; ++i) {
         fColHeader[i]->UnmapWindow();
         fSplitHeader[i]->UnmapWindow();
      }
      fHeader->UnmapWindow();
   }
   TGLayoutManager *lm = container->GetLayoutManager();

   lm->SetDefaultWidth(xl);
   TGCanvas::Layout();
 
   if (fViewMode == kLVDetails) {
      container->Resize(container->GetWidth(), container->GetHeight()+h);
      fVScrollbar->SetRange((Int_t)container->GetHeight(), 
                            (Int_t)fVport->GetHeight());
      if (fJustChanged) {
         fVport->MoveResize(fBorderWidth, fBorderWidth, fVport->GetWidth(), 
                            fVport->GetHeight());
         container->Move(0, h);
      } else {
         container->DrawRegion(0, 0, fVport->GetWidth(), fVport->GetHeight());
      }
      fColHeader[i]->MoveResize(xl, 0, fVport->GetWidth()-xl, h);
      fColHeader[i]->MapWindow();
   } else {
      fVport->MoveResize(fBorderWidth, fBorderWidth, fVport->GetWidth(), 
                         fVport->GetHeight());
      container->Move(0, 0);
   }

   fJustChanged = kFALSE;
}

//______________________________________________________________________________
void TGListView::LayoutHeader(TGFrame *head)
{
   // Layout list view components (container and contents of container).

   Int_t  i, xl = 0;
   UInt_t w, h = 0;
   static Int_t oldPos = 0;
   if (head == 0) oldPos = 0;

   TGLVContainer *container = (TGLVContainer *) fVport->GetContainer();

   if (!container) {
      Error("Layout", "no listview container set yet");
      return;
   }
   fMaxSize = container->GetMaxItemSize();
   Int_t posx = container->GetPagePosition().fX;

   if (fViewMode == kLVDetails) {
      h = fColHeader[0]->GetDefaultHeight()-4;
      fHeader->MoveResize(0, 0, fWidth, h);
      fHeader->MapWindow();
      for (i = 0; i < fNColumns-1; ++i) {
         fColHeader[i]->SetText(fColNames[i]);

         if ( fJustChanged ) {
            w = fColHeader[i]->GetDefaultWidth();
            if (i == 0) w = TMath::Max(fMaxSize.fWidth + 10, w);
            if (i > 0)  w = TMath::Max(container->GetMaxSubnameWidth(i) + 40, (Int_t)w);
         } else {
            w = fColHeader[i]->GetWidth();
         }
         w = TMath::Max( fMinColumnSize, (Int_t)w);
         if ( fColHeader[i]->GetDefaultWidth() > w ) {
            for (int j = fColNames[i].Length() - 1 ; j > 0; j--) {
               fColHeader[i]->SetText( fColNames[i](0,j) + "..." );
               if ( fColHeader[i]->GetDefaultWidth() < w )
                  break;
            }
         }

         if ((TGFrame *)fColHeader[i] == head) {
            if (oldPos > 0) {
               gVirtualX->DrawLine(container->GetId(), container->GetLineGC()(),
                                   oldPos - posx, 0, oldPos - posx, fVport->GetHeight());
            }
            gVirtualX->DrawLine(container->GetId(), container->GetLineGC()(),
                                xl + w - posx, 0, xl + w - posx, fVport->GetHeight());
            oldPos = xl + w;
         }

         fColHeader[i]->MoveResize(xl - posx, 0, w, h);
         fColHeader[i]->MapWindow();
         xl += w;
         fSplitHeader[i]->Move(xl, 0);
         fSplitHeader[i]->MapWindow();
         fColumns[i] = xl-2;  // -2 is fSep in the layout routine
      }
      fColHeader[i]->MoveResize(xl - posx, 0, fVport->GetWidth()-xl, h);
      fColHeader[i]->MapWindow();
      fSplitHeader[i]->Move(fVport->GetWidth(),  fSplitHeader[i]->GetHeight());
      fSplitHeader[i]->MapWindow();
      fVScrollbar->RaiseWindow();
   }
   fJustChanged = kFALSE;
}

//______________________________________________________________________________
Bool_t TGListView::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // Handle messages generated by the list view components.

   TGLVContainer *cnt = (TGLVContainer*)GetContainer();

   const TGLVEntry *entry;
   void *p = 0;

   entry = (TGLVEntry *) cnt->GetNextSelected(&p);

   switch (GET_SUBMSG(msg)) {
      case kCT_ITEMCLICK:
         if ((cnt->NumSelected() == 1) && (entry != 0)) {
            Int_t x = (Int_t)(parm2 & 0xffff);
            Int_t y = (Int_t)((parm2 >> 16) & 0xffff);
            Clicked((TGLVEntry*)entry, (Int_t)parm1);
            Clicked((TGLVEntry*)entry, (Int_t)parm1, x, y);
         }
         break;
      case kCT_ITEMDBLCLICK:
         if ((cnt->NumSelected() == 1) && (entry!=0)) {
            Int_t x = (Int_t)(parm2 & 0xffff);
            Int_t y = (Int_t)((parm2 >> 16) & 0xffff);
            DoubleClicked((TGLVEntry*)entry, (Int_t)parm1);
            DoubleClicked((TGLVEntry*)entry, (Int_t)parm1, x, y);
         }
         break;
      case kCT_SELCHANGED:
         SelectionChanged();
         break;
      default:
         break;
   }
   return TGCanvas::ProcessMessage(msg, parm1, parm2);
}

//______________________________________________________________________________
void TGListView::Clicked(TGLVEntry *entry, Int_t btn)
{
   // Emit Clicked() signal.

   Long_t args[2];

   args[0] = (Long_t)entry;
   args[1] = btn;

   Emit("Clicked(TGLVEntry*,Int_t)", args);
}

//______________________________________________________________________________
void TGListView::Clicked(TGLVEntry *entry, Int_t btn, Int_t x, Int_t y)
{
   // Emit Clicked() signal.

   Long_t args[4];

   args[0] = (Long_t)entry;
   args[1] = btn;
   args[2] = x;
   args[3] = y;

   Emit("Clicked(TGLVEntry*,Int_t,Int_t,Int_t)", args);
}

//______________________________________________________________________________
void TGListView::DoubleClicked(TGLVEntry *entry, Int_t btn)
{
   // Emit DoubleClicked() signal.

   Long_t args[2];

   args[0] = (Long_t)entry;
   args[1] = btn;

   Emit("DoubleClicked(TGLVEntry*,Int_t)", args);
}

//______________________________________________________________________________
void TGListView::DoubleClicked(TGLVEntry *entry, Int_t btn, Int_t x, Int_t y)
{
   // Emit DoubleClicked() signal.

   Long_t args[4];

   args[0] = (Long_t)entry;
   args[1] = btn;
   args[2] = x;
   args[3] = y;

   Emit("DoubleClicked(TGLVEntry*,Int_t,Int_t,Int_t)", args);
}

//______________________________________________________________________________
FontStruct_t TGListView::GetDefaultFontStruct()
{
   // Return the default font structure in use.

   if (!fgDefaultFont)
      fgDefaultFont = gClient->GetResourcePool()->GetIconFont();
   return fgDefaultFont->GetFontStruct();
}

//______________________________________________________________________________
const TGGC &TGListView::GetDefaultGC()
{
   // Return the default graphics context in use.

   if (!fgDefaultGC) {
      fgDefaultGC = new TGGC(*gClient->GetResourcePool()->GetFrameGC());
      fgDefaultGC->SetFont(fgDefaultFont->GetFontHandle());
   }
   return *fgDefaultGC;
}

//______________________________________________________________________________
void TGListView::SavePrimitive(ostream &out, Option_t *option /*= ""*/)
{
   // Save a list view widget as a C++ statement(s) on output stream out.

   if (fBackground != GetDefaultFrameBackground()) SaveUserColor(out, option);

   out << endl << "   // list view" << endl;
   out <<"   TGListView *";
   out << GetName() << " = new TGListView(" << fParent->GetName()
       << "," << GetWidth() << "," << GetHeight();

   if (fBackground == GetDefaultFrameBackground()) {
      if (GetOptions() == (kSunkenFrame | kDoubleBorder)) {
         out <<");" << endl;
      } else {
         out << "," << GetOptionString() <<");" << endl;
      }
   } else {
      out << "," << GetOptionString() << ",ucolor);" << endl;
   }

   GetContainer()->SavePrimitive(out, option);

   out << endl;
   out << "   " << GetName() << "->SetContainer(" << GetContainer()->GetName()
                << ");" << endl;
   out << "   " << GetName() << "->SetViewMode(";
   switch (fViewMode) {
      case kLVLargeIcons:
         out << "kLVLargeIcons";
         break;
      case kLVSmallIcons:
         out << "kLVSmallIcons";
         break;
      case kLVList:
         out << "kLVList";
         break;
      case kLVDetails:
         out << "kLVDetails";
         break;
   }
   out << ");" << endl;

   out << "   " << GetContainer()->GetName() << "->Resize();" << endl;

   if (fHScrollbar && fHScrollbar->IsMapped()) {
   out << "   " << GetName() << "->SetHsbPosition(" << GetHsbPosition()
       << ");" << endl;
   }

   if (fVScrollbar && fVScrollbar->IsMapped()) {
   out << "   " << GetName() << "->SetVsbPosition(" << GetVsbPosition()
       << ");" << endl;
   }
}

//______________________________________________________________________________
void TGLVContainer::SavePrimitive(ostream &out, Option_t *option /*= ""*/)
{
   // Save a list view container as a C++ statement(s) on output stream out.

   if (fBackground != GetDefaultFrameBackground()) SaveUserColor(out, option);

   out << endl << "   // list view container" << endl;
   out << "   TGLVContainer *";

   if ((fParent->GetParent())->InheritsFrom(TGCanvas::Class())) {
      out << GetName() << " = new TGLVContainer(" << GetCanvas()->GetName();
   } else {
      out << GetName() << " = new TGLVContainer(" << fParent->GetName();
      out << "," << GetWidth() << "," << GetHeight();
   }
   if (fBackground == GetDefaultFrameBackground()) {
      if (GetOptions() == (kSunkenFrame | kDoubleBorder)) {
         out <<");" << endl;
      } else {
         out << "," << GetOptionString() <<");" << endl;
      }
   } else {
      out << "," << GetOptionString() << ",ucolor);" << endl;
   }
}

