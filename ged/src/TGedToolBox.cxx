// @(#)root/ged:$Name:  $:$Id: TGedToolBox.cxx,v 1.1 2004/02/18 20:13:42 brun Exp $
// Author: Marek Biskup, Ilka Antcheva 17/07/2003

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGedToolBox                                                          //
//                                                                      //
// A toolbox is a composite frame that contains TGPictureButtons        //
// arranged in a matrix 4x5                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TGedToolBox.h"
#include "TList.h"
#include "TGButton.h"
#include "TGPicture.h"
#include "TROOT.h"
#include "TCanvas.h"

ClassImp(TGedToolBox)

enum {
   kToolBoxModify,
   kToolBoxArc,
   kToolBoxLine,
   kToolBoxArrow,
   kToolBoxButton,
   kToolBoxDiamond,
   kToolBoxEllipse,
   kToolBoxPad,
   kToolBoxPave,
   kToolBoxPLabel,
   kToolBoxPText,
   kToolBoxPsText,
   kToolBoxGraph,
   kToolBoxCurlyLine,
   kToolBoxCurlyArc,
   kToolBoxLatex,
   kToolBoxMarker,
   kToolBoxCutG
};

static ToolBarData_t gToolBoxData[] = {
  // { filename,      tooltip,            staydown,  id,              button}
   { "pointer.xpm",    "Modify",           kFALSE,    kToolBoxModify,   NULL },
   { "arc.xpm",        "Arc",              kFALSE,    kToolBoxArc,      NULL },
   { "line.xpm",       "Line",             kFALSE,    kToolBoxLine,     NULL },
   { "arrow.xpm",      "Arrow",            kFALSE,    kToolBoxArrow,    NULL },
   { "button.xpm",     "Button",           kFALSE,    kToolBoxButton,   NULL },
   { "diamond.xpm",    "Diamond",          kFALSE,    kToolBoxDiamond,  NULL },
   { "ellipse.xpm",    "Ellipse",          kFALSE,    kToolBoxEllipse,  NULL },
   { "pad.xpm",        "Pad",              kFALSE,    kToolBoxPad,      NULL },
   { "pave.xpm",       "Pave",             kFALSE,    kToolBoxPave,     NULL },
   { "pavelabel.xpm",  "Pave Label",       kFALSE,    kToolBoxPLabel,   NULL },
   { "pavetext.xpm",   "Pave Text",        kFALSE,    kToolBoxPText,    NULL },
   { "pavestext.xpm",  "Paves Text",       kFALSE,    kToolBoxPsText,   NULL },
   { "graph.xpm",      "Graph",            kFALSE,    kToolBoxGraph,    NULL },
   { "curlyline.xpm",  "Curly Line",       kFALSE,    kToolBoxCurlyLine,NULL },
   { "curlyarc.xpm",   "Curly Arc",        kFALSE,    kToolBoxCurlyArc, NULL },
   { "latex.xpm",      "Text/Latex",       kFALSE,    kToolBoxLatex,    NULL },
   { "marker.xpm",     "Marker",           kFALSE,    kToolBoxMarker,   NULL },
   { "cut.xpm",        "Graphical Cut",    kFALSE,    kToolBoxCutG,     NULL },
   { 0,                0,                  kFALSE,    0,                NULL }
};
                    
	
//______________________________________________________________________________
TGedToolBox::TGedToolBox(const TGWindow *p, UInt_t w, 
                         UInt_t h, UInt_t options, ULong_t back) 
   : TGToolBar(p, w, h, options, back)
{
   // Create toolbox widget.

   SetLayoutManager(new TGMatrixLayout(this, 5, 5, 2, 2));
   CreateButtons(gToolBoxData);

}

//______________________________________________________________________________
void TGedToolBox::CreateButtons(ToolBarData_t buttons[])
{
   // Adds buttons described by buttons table to the ToolBox.
   // the last element of buttons[] table has zero in the filed
   // fPixmap.
    
   for (int i = 0; buttons[i].fPixmap; i++)
      AddButton(this, &buttons[i], 0);

   return;
}

//______________________________________________________________________________
Bool_t TGedToolBox::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Handle menu and other command generated by the user.
   switch (GET_MSG(msg)) {

      case kC_COMMAND:
              
         switch (GET_SUBMSG(msg)) {

            case kCM_BUTTON:
            case kCM_MENU:
                    
               switch(parm1) {
                 
                  case kToolBoxModify:
                     gROOT->SetEditorMode();
                     break;
                  case kToolBoxArc:
                     gROOT->SetEditorMode("Arc");
                     break;
                  case kToolBoxLine:
                     gROOT->SetEditorMode("Line");
                     break;
                  case kToolBoxArrow:
                     gROOT->SetEditorMode("Arrow");
                     break;
                  case kToolBoxButton:
                     gROOT->SetEditorMode("Button");
                     break;
                  case kToolBoxDiamond:
                     gROOT->SetEditorMode("Diamond");
                     break;
                  case kToolBoxEllipse:
                     gROOT->SetEditorMode("Ellipse");
                     break;
                  case kToolBoxPad:
                     gROOT->SetEditorMode("Pad");
                     break;
                  case kToolBoxPave:
                     gROOT->SetEditorMode("Pave");
                     break;
                  case kToolBoxPLabel:
                     gROOT->SetEditorMode("PaveLabel");
                     break;
                  case kToolBoxPText:
                     gROOT->SetEditorMode("PaveText");
                     break;
                  case kToolBoxPsText:
                     gROOT->SetEditorMode("PavesText");
                     break;
                  case kToolBoxGraph:
                     gROOT->SetEditorMode("PolyLine");
                     break;
                  case kToolBoxCurlyLine:
                     gROOT->SetEditorMode("CurlyLine");
                     break;
                  case kToolBoxCurlyArc:
                     gROOT->SetEditorMode("CurlyArc");
                     break;
                  case kToolBoxLatex:
                     gROOT->SetEditorMode("Text");
                     break;
                  case kToolBoxMarker:
                     gROOT->SetEditorMode("Marker");
                     break;
                  case kToolBoxCutG:
                     gROOT->SetEditorMode("CutG");
                     break;
                  default:
                     break;
               }  // GET_SUBMSG
               break;
            default:
               break;
         }
         default:
            break;
   } // GET_MSG
   return kTRUE;
}
