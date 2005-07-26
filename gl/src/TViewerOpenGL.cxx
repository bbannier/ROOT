// @(#)root/gl:$Name:  $:$Id: TViewerOpenGL.cxx,v 1.69 2005/07/14 19:13:04 brun Exp $
// Author:  Timur Pocheptsov  03/08/2004

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
#include "TViewerOpenGL.h"

#include "TPluginManager.h"
#include "TRootHelpDialog.h"
#include "TContextMenu.h"
#include "KeySymbols.h"
#include "TGShutter.h"
#include "TGLKernel.h"
#include "TVirtualGL.h"
#include "TGButton.h"
#include "TGClient.h"
#include "TGCanvas.h"
#include "HelpText.h"
#include "Buttons.h"
#include "TAtt3D.h"
#include "TGMenu.h"
#include "TColor.h"
#include "TMath.h"
#include "TSystem.h"

#include "TGLSceneObject.h"
#include "TGLRenderArea.h"
#include "TGLEditor.h"
#include "TGLCamera.h"

#include "TGLPhysicalShape.h"

#include "TVirtualPad.h"

#include "gl2ps.h"

#include <assert.h>

const char gHelpViewerOpenGL[] = "\
     PRESS \n\
     \tw\t--- wireframe mode\n\
     \tr\t--- filled polygons mode\n\
     \tj\t--- zoom in\n\
     \tk\t--- zoom out\n\n\
	  \tArrow Keys\tpan (truck) across scene\n\
     You can ROTATE (ORBIT) the scene by holding the left \n\
     mouse button and moving the mouse (pespective camera only).\n\
     You can PAN (TRUCK) the camera using the middle mouse\n\
     button or arrow keys.\n\
     You can ZOOM (DOLLY) the camera by dragging side\n\
     to side holding the right mouse button or using the\n\
     mouse wheel.\n\
     RESET the camera by double clicking any button\n\
     SELECT an object with Shift+Left mouse button click.\n\
     MOVE the object using Shift+Mid mouse drag.\n\
     Invoked the CONTEXT menu with Shift+Right mouse click.\n\
     PROJECTIONS\n\n\
     You can select the different plane projections\n\
     in \"Projections\" menu.\n\n\
     COLOR\n\n\
     After you selected an object or a light source,\n\
     you can modify object's material and light\n\
     source color.\n\n\
     \tLIGHT SOURCES.\n\n\
     \tThere are two pickable light sources in\n\
     \tthe current implementation. They are shown as\n\
     \tspheres. Each light source has three light\n\
     \tcomponents : DIFFUSE, AMBIENT, SPECULAR.\n\
     \tEach of this components is defined by the\n\
     \tamounts of red, green and blue light it emits.\n\
     \tYou can EDIT this parameters:\n\
     \t1. Select light source sphere.\n" //hehe, too long string literal :)))
"    \t2. Select light component you want to modify\n\
     \t   by pressing one of radio buttons.\n\
     \t3. Change RGB by moving sliders\n\n\
     \tMATERIAL\n\n\
     \tObject's material is specified by the percentage\n\
     \tof red, green, blue light it reflects. A surface can\n\
     \treflect diffuse, ambient and specular light. \n\
     \tA surface has two additional parameters: EMISSION\n\
     \t- you can make surface self-luminous; SHININESS -\n\
     \tmodifying this parameter you can change surface\n\
     \thighlights.\n\
     \tSometimes changes are not visible, or light\n\
     \tsources seem not to work - you should understand\n\
     \tthe meaning of diffuse, ambient etc. light and material\n\
     \tcomponents. For example, if you define material, wich has\n\
     \tdiffuse component (1., 0., 0.) and you have a light source\n\
     \twith diffuse component (0., 1., 0.) - you surface does not\n\
     \treflect diffuse light from this source. For another example\n\
     \t- the color of highlight on the surface is specified by:\n\
     \tlight's specular component, material specular component.\n\
     \tAt the top of the color editor there is a small window\n\
     \twith sphere. When you are editing surface material,\n\
     \tyou can see this material applyed to sphere.\n\
     \tWhen edit light source, you see this light reflected\n\
     \tby sphere whith DIFFUSE and SPECULAR components\n\
     \t(1., 1., 1.).\n\n\
     OBJECT'S GEOMETRY\n\n\
     You can edit object's location and stretch it by entering\n\
     desired values in respective number entry controls.\n\n"
"    SCENE PROPERTIES\n\n\
     You can add clipping plane by clicking the checkbox and\n\
     specifying the plane's equation A*x+B*y+C*z+D=0.";

enum EGLViewerCommands {
   kGLHelpAbout,
   kGLHelpOnViewer,
   kGLXOY,
   kGLXOZ,
   kGLYOZ,
   kGLPersp,
   kGLPrintEPS_SIMPLE,
   kGLPrintEPS_BSP,
   kGLPrintPDF_SIMPLE,
   kGLPrintPDF_BSP,
   kGLExit
};

ClassImp(TViewerOpenGL)

const Int_t TViewerOpenGL::fgInitX = 0;
const Int_t TViewerOpenGL::fgInitY = 0;
const Int_t TViewerOpenGL::fgInitW = 780;
const Int_t TViewerOpenGL::fgInitH = 670;

int format = GL2PS_EPS;
int sortgl = GL2PS_BSP_SORT;

//______________________________________________________________________________
TViewerOpenGL::TViewerOpenGL(TVirtualPad * pad) :
   TGMainFrame(gClient->GetDefaultRoot(), fgInitW, fgInitH),
   fMainFrame(0), fV1(0), fV2(0), fShutter(0), fShutItem1(0), fShutItem2(0), 
   fShutItem3(0), fShutItem4(0), fL1(0), fL2(0), fL3(0), fL4(0),
   fCanvasLayout(0), fMenuBar(0), fFileMenu(0), fViewMenu(0), fHelpMenu(0),
   fMenuBarLayout(0), fMenuBarItemLayout(0), fMenuBarHelpLayout(0),
   fContextMenu(0), fCanvasWindow(0), fCanvasContainer(0),
   fColorEditor(0), fGeomEditor(0), fSceneEditor(0), fLightEditor(0),
   fAction(kNone), fStartPos(0,0), fLastPos(0,0), fActiveButtonID(0),
   fLightMask(0x1b), fPad(pad)
{
   // Create OpenGL viewer.
   static Bool_t init = kFALSE;
   if (!init) {
      TPluginHandler *h;
      if ((h = gROOT->GetPluginManager()->FindHandler("TVirtualGLImp"))) {
         if (h->LoadPlugin() == -1)
            return;
         TVirtualGLImp * imp = (TVirtualGLImp *) h->ExecPlugin(0);
         new TGLKernel(imp);
      }
      init = kTRUE;
   }

   CreateViewer();
}

//______________________________________________________________________________
void TViewerOpenGL::CreateViewer()
{
   // Menus creation
   fFileMenu = new TGPopupMenu(fClient->GetRoot());
   fFileMenu->AddEntry("&Print EPS", kGLPrintEPS_SIMPLE);
   fFileMenu->AddEntry("&Print EPS (High quality)", kGLPrintEPS_BSP);
   fFileMenu->AddEntry("&Print PDF", kGLPrintPDF_SIMPLE);
   fFileMenu->AddEntry("&Print PDF (High quality)", kGLPrintPDF_BSP);
   fFileMenu->AddEntry("&Exit", kGLExit);
   fFileMenu->Associate(this);

   fViewMenu = new TGPopupMenu(fClient->GetRoot());
   fViewMenu->AddEntry("&XOY plane", kGLXOY);
   fViewMenu->AddEntry("XO&Z plane", kGLXOZ);
   fViewMenu->AddEntry("&YOZ plane", kGLYOZ);
   fViewMenu->AddEntry("&Perspective view", kGLPersp);
   fViewMenu->Associate(this);

   fHelpMenu = new TGPopupMenu(fClient->GetRoot());
   fHelpMenu->AddEntry("&About ROOT...", kGLHelpAbout);
   fHelpMenu->AddSeparator();
   fHelpMenu->AddEntry("Help on OpenGL Viewer...", kGLHelpOnViewer);
   fHelpMenu->Associate(this);

   // Create menubar layout hints
   fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);

   // Create menubar
   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame | kRaisedFrame);
   fMenuBar->AddPopup("&File", fFileMenu, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Projections", fViewMenu, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help",    fHelpMenu,    fMenuBarHelpLayout);
   AddFrame(fMenuBar, fMenuBarLayout);

   // Frames creation
   fMainFrame = new TGCompositeFrame(this, 100, 100, kHorizontalFrame | kRaisedFrame);
   fV1 = new TGVerticalFrame(fMainFrame, 150, 10, kSunkenFrame | kFixedWidth);
   fShutter = new TGShutter(fV1, kSunkenFrame | kFixedWidth);
   fShutItem1 = new TGShutterItem(fShutter, new TGHotString("Color"), 5001);
   fShutItem2 = new TGShutterItem(fShutter, new TGHotString("Object's geometry"), 5002);
   fShutItem3 = new TGShutterItem(fShutter, new TGHotString("Scene"), 5003);
   fShutItem4 = new TGShutterItem(fShutter, new TGHotString("Lights"), 5004);
   fShutter->AddItem(fShutItem1);
   fShutter->AddItem(fShutItem2);
   fShutter->AddItem(fShutItem3);
   fShutter->AddItem(fShutItem4);

   TGCompositeFrame *shutCont = (TGCompositeFrame *)fShutItem1->GetContainer();
   fColorEditor = new TGLColorEditor(shutCont, this);
   fL4 = new TGLayoutHints(kLHintsTop | kLHintsCenterX | kLHintsExpandX | kLHintsExpandY, 2, 5, 1, 2);
   shutCont->AddFrame(fColorEditor, fL4);
   fV1->AddFrame(fShutter, fL4);
   fL1 = new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 2, 0, 2, 2);
   fMainFrame->AddFrame(fV1, fL1);

   shutCont = (TGCompositeFrame *)fShutItem2->GetContainer();
   fGeomEditor = new TGLGeometryEditor(shutCont, this);
   shutCont->AddFrame(fGeomEditor, fL4);

   shutCont = (TGCompositeFrame *)fShutItem3->GetContainer();
   fSceneEditor = new TGLSceneEditor(shutCont, this);
   shutCont->AddFrame(fSceneEditor, fL4);

   shutCont = (TGCompositeFrame *)fShutItem4->GetContainer();
   fLightEditor = new TGLLightEditor(shutCont, this);
   shutCont->AddFrame(fLightEditor, fL4);

   fV2 = new TGVerticalFrame(fMainFrame, 10, 10, kSunkenFrame);
   fL3 = new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsExpandY,0,2,2,2);
   fMainFrame->AddFrame(fV2, fL3);

   fCanvasWindow = new TGCanvas(fV2, 10, 10, kSunkenFrame | kDoubleBorder);
   fCanvasContainer = new TGLRenderArea(fCanvasWindow->GetViewPort()->GetId(), fCanvasWindow->GetViewPort());

   TGLWindow * glWin = fCanvasContainer->GetGLWindow();
   glWin->Connect("HandleButton(Event_t*)", "TViewerOpenGL", this, "HandleContainerButton(Event_t*)");
   glWin->Connect("HandleDoubleClick(Event_t*)", "TViewerOpenGL", this, "HandleContainerDoubleClick(Event_t*)");
   glWin->Connect("HandleKey(Event_t*)", "TViewerOpenGL", this, "HandleContainerKey(Event_t*)");
   glWin->Connect("HandleMotion(Event_t*)", "TViewerOpenGL", this, "HandleContainerMotion(Event_t*)");
   glWin->Connect("HandleExpose(Event_t*)", "TViewerOpenGL", this, "HandleContainerExpose(Event_t*)");
   glWin->Connect("HandleConfigureNotify(Event_t*)", "TViewerOpenGL", this, "HandleContainerConfigure(Event_t*)");

   fCanvasWindow->SetContainer(glWin);
   fCanvasLayout = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY);
   fV2->AddFrame(fCanvasWindow, fCanvasLayout);
   AddFrame(fMainFrame, fCanvasLayout);

   SetWindowName("OpenGL experimental viewer");
   SetClassHints("GLViewer", "GLViewer");
   SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
   MapSubwindows();
   Resize(GetDefaultSize());
   MoveResize(fgInitX, fgInitY, fgInitW, fgInitH);
   SetWMPosition(fgInitX, fgInitY);
   Show();
}

//______________________________________________________________________________
TViewerOpenGL::~TViewerOpenGL()
{
   delete fFileMenu;
   delete fViewMenu;
   delete fHelpMenu;
   delete fMenuBar;
   delete fMenuBarLayout;
   delete fMenuBarHelpLayout;
   delete fMenuBarItemLayout;
   delete fCanvasContainer;
   delete fCanvasWindow;
   delete fCanvasLayout;
   delete fV1;
   delete fV2;
   delete fMainFrame;
   delete fL1;
   delete fL2;
   delete fL3;
   delete fL4;
   delete fContextMenu;
   delete fShutter;
   delete fShutItem1;
   delete fShutItem2;
   delete fShutItem3;
   delete fShutItem4;
}

//______________________________________________________________________________
void TViewerOpenGL::InitGL()
{
   // Actual GL window/context creation should have already been done in CreateViewer()
   assert(!fInitGL && fCanvasContainer && fCanvasContainer->GetGLWindow());

   // GL initialisation 
   glEnable(GL_LIGHTING);
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glEnable(GL_CULL_FACE);
   glCullFace(GL_BACK);
   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClearDepth(1.0);

   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
   Float_t lmodelAmb[] = {0.5f, 0.5f, 1.f, 1.f};
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodelAmb);
   
   // Calculate light source positions
   // Arrange round an expanded sphere of scene BB
   // TODO: These need to be positioned before each scene draw, after the camera MV translation so that they don't shift relative to objects.
   TGLBoundingBox box = fScene.BoundingBox();
   Double_t radius = box.Extents().Mag() * 4.0;
   
   // 0: Front
   // 1: Right
   // 2: Bottom
   // 3: Left
   // 4: Top   
   Float_t pos0[] = {0.0, 0.0, 0.0, 1.0};
   Float_t pos1[] = {box.Center().X() + radius, box.Center().Y()         , -box.Center().Z() - radius, 1.0};
   Float_t pos2[] = {box.Center().X()         , box.Center().Y() - radius, -box.Center().Z() - radius, 1.0};
   Float_t pos3[] = {box.Center().X() - radius, box.Center().Y()         , -box.Center().Z() - radius, 1.0};
   Float_t pos4[] = {box.Center().X()         , box.Center().Y() + radius, -box.Center().Z() - radius, 1.0};

   Float_t whiteCol[] = {0.7, 0.7, 0.7, 1.0};
   gVirtualGL->GLLight(kLIGHT0, kPOSITION, pos0);
   gVirtualGL->GLLight(kLIGHT0, kDIFFUSE, whiteCol);
   gVirtualGL->GLLight(kLIGHT1, kPOSITION, pos1);
   gVirtualGL->GLLight(kLIGHT1, kDIFFUSE, whiteCol);
   gVirtualGL->GLLight(kLIGHT2, kPOSITION, pos2);
   gVirtualGL->GLLight(kLIGHT2, kDIFFUSE, whiteCol);
   gVirtualGL->GLLight(kLIGHT3, kPOSITION, pos3);
   gVirtualGL->GLLight(kLIGHT3, kDIFFUSE, whiteCol);
   gVirtualGL->GLLight(kLIGHT4, kPOSITION, pos4);
   gVirtualGL->GLLight(kLIGHT4, kDIFFUSE, whiteCol);
   
   if (fLightMask & 1) gVirtualGL->EnableGL(kLIGHT4);
   if (fLightMask & 2) gVirtualGL->EnableGL(kLIGHT1);
   if (fLightMask & 4) gVirtualGL->EnableGL(kLIGHT2);
   if (fLightMask & 8) gVirtualGL->EnableGL(kLIGHT3);
   if (fLightMask & 16) gVirtualGL->EnableGL(kLIGHT0);

   TGLUtil::CheckError();
   fInitGL = kTRUE;
}

//______________________________________________________________________________
void TViewerOpenGL::Invalidate(UInt_t redrawLOD)
{
   if (fScene.IsLocked()) {
      Error("TViewerOpenGL::Invalidate", "scene is %s", TGLScene::LockName(fScene.CurrentLock()));
      return;
   }

   TGLViewer::Invalidate(redrawLOD);
   
   // Mark the window as requiring a redraw - the GUI thread
   // will call our DoRedraw() method
   fClient->NeedRedraw(this);

   if (gDebug>3) {
      Info("TViewerOpenGL::Invalidate", "invalidated at %d LOD", fNextSceneLOD);
   }
}

//______________________________________________________________________________
void TViewerOpenGL::MakeCurrent() const
{
   fCanvasContainer->GetGLWindow()->MakeCurrent();
}

//______________________________________________________________________________
void TViewerOpenGL::SwapBuffers() const
{
   if (fScene.CurrentLock() != TGLScene::kDrawLock && 
      fScene.CurrentLock() != TGLScene::kSelectLock) {
      Error("TViewerOpenGL::MakeCurrent", "scene is %s", TGLScene::LockName(fScene.CurrentLock()));   
   }
   fCanvasContainer->GetGLWindow()->SwapBuffers();
}

//______________________________________________________________________________
void TViewerOpenGL::FillScene()
{
   // TODO: Just marking modified doesn't seem to result in pad repaint - need to check on
   //fPad->Modified();
   fPad->Paint();
}

//______________________________________________________________________________
Bool_t TViewerOpenGL::HandleContainerEvent(Event_t *event)
{
   if (event->fType == kFocusIn) {
      assert(fAction == kNone);
      fAction = kNone;
   }
   if (event->fType == kFocusOut) {
      fAction = kNone;
   }

   return kTRUE;
}

//______________________________________________________________________________
Bool_t TViewerOpenGL::HandleContainerButton(Event_t *event)
{
   if (fScene.IsLocked()) {
      if (gDebug>2) {
         Info("TViewerOpenGL::HandleContainerButton", "ignored - scene is %s", TGLScene::LockName(fScene.CurrentLock()));
      }
      return kFALSE;
   }

   // Only process one action/button down/up pairing - block others
   if (fAction != kNone) {
      if (event->fType == kButtonPress ||
          (event->fType == kButtonRelease && event->fCode != fActiveButtonID)) {
         return kFALSE;
      }
   }
   
   // Button DOWN
   if (event->fType == kButtonPress) {
      Bool_t grabPointer = kFALSE;

      // Record active button for release
      fActiveButtonID = event->fCode;

      // Record mouse start
      fStartPos.fX = fLastPos.fX = event->fX;
      fStartPos.fY = fLastPos.fY = event->fY;
      
      switch(event->fCode) {
         // LEFT mouse button
         case(kButton1): {
            if (event->fState & kKeyShiftMask) {
               DoSelect(event, kFALSE); // without context menu

               // TODO: If no selection start a box select
            } else {
               fAction = kRotate;
               grabPointer = kTRUE;
            }
            break;
         }
         // MID mouse button
         case(kButton2): {
            if (event->fState & kKeyShiftMask) {
               DoSelect(event, kFALSE); // without context menu
               // Start object drag
               if (fScene.GetSelected()) {
                  fAction = kDrag;
                  grabPointer = kTRUE;
               }
            } else {
               fAction = kTruck;
               grabPointer = kTRUE;
            }
            break;
         }
         // RIGHT mouse button
         case(kButton3): {
            // Shift + Right mouse - select+context menu
            if (event->fState & kKeyShiftMask) {
               DoSelect(event, kTRUE); // with context menu
            } else {
               fAction = kDolly;
               grabPointer = kTRUE;
            }
            break;
         }
      }
   }
   // Button UP
   else if (event->fType == kButtonRelease) {
      // TODO: Check on Linux - on Win32 only see button release events
      // for mouse wheel
      switch(event->fCode) {
         // Buttons 4/5 are mouse wheel
         case(kButton4): {
            // Zoom out (adjust camera FOV)
            if (CurrentCamera().Zoom(-30, event->fState & kKeyControlMask, 
                                          event->fState & kKeyShiftMask)) { //TODO : val static const somewhere
               Invalidate();
            }
            break;
         }
         case(kButton5): {
            // Zoom in (adjust camera FOV)
            if (CurrentCamera().Zoom(+30, event->fState & kKeyControlMask, 
                                          event->fState & kKeyShiftMask)) { //TODO : val static const somewhere
               Invalidate();
            }
            break;
         }
      }
      fAction = kNone;
   }

   return kTRUE;
}

//______________________________________________________________________________
Bool_t TViewerOpenGL::HandleContainerDoubleClick(Event_t *event)
{
   if (fScene.IsLocked()) {
      if (gDebug>3) {
         Info("TViewerOpenGL::HandleContainerDoubleClick", "ignored - scene is %s", TGLScene::LockName(fScene.CurrentLock()));
      }
      return kFALSE;
   }

   // Reset interactive camera mode on button double
   // click (unless mouse wheel)
   if (event->fCode != kButton4 && event->fCode != kButton5) {
      CurrentCamera().Reset();
      fStartPos.fX = fLastPos.fX = event->fX;
      fStartPos.fY = fLastPos.fY = event->fY;
      Invalidate();
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TViewerOpenGL::HandleContainerConfigure(Event_t *event)
{
   if (fScene.IsLocked()) {
      if (gDebug>3) {
         Info("TViewerOpenGL::HandleContainerConfigure", "ignored - scene is %s", TGLScene::LockName(fScene.CurrentLock()));
      }
      return kFALSE;
   }

   if (event) {
      SetViewport(event->fX, event->fY, event->fWidth, event->fHeight);
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TViewerOpenGL::HandleContainerKey(Event_t *event)
{
   if (fScene.IsLocked()) {
      if (gDebug>3) {
         Info("TViewerOpenGL::HandleContainerKey", "ignored - scene is %s", TGLScene::LockName(fScene.CurrentLock()));
      }
      return kFALSE;
   }

   char tmp[10] = {0};
   UInt_t keysym = 0;
   Float_t black[] = {0.f, 0.f, 0.f, 1.f};
   Float_t white[] = {1.f, 1.f, 1.f, 1.f};

   gVirtualX->LookupString(event, tmp, sizeof(tmp), keysym);
   
   Bool_t invalidate = kFALSE;

   switch (keysym) {
   case kKey_Plus:
   case kKey_J:
   case kKey_j:
      invalidate = CurrentCamera().Dolly(10, event->fState & kKeyControlMask, 
                                             event->fState & kKeyShiftMask); //TODO : val static const somewhere
      break;
   case kKey_Minus:
   case kKey_K:
   case kKey_k:
      invalidate = CurrentCamera().Dolly(-10, event->fState & kKeyControlMask, 
                                              event->fState & kKeyShiftMask); //TODO : val static const somewhere
      break;
   case kKey_R:
   case kKey_r:
      gVirtualGL->EnableGL(kLIGHTING);
      gVirtualGL->EnableGL(kCULL_FACE);
      gVirtualGL->PolygonGLMode(kFRONT, kFILL);
      gVirtualGL->ClearGLColor(black[0], black[1], black[2], black[3]);
      fScene.SetDrawMode(TGLScene::kFill);
      invalidate = kTRUE;
      break;
   case kKey_W:
   case kKey_w:
      gVirtualGL->DisableGL(kCULL_FACE);
      gVirtualGL->DisableGL(kLIGHTING);
      gVirtualGL->PolygonGLMode(kFRONT_AND_BACK, kLINE);
      gVirtualGL->ClearGLColor(black[0], black[1], black[2], black[3]);
      fScene.SetDrawMode(TGLScene::kWireFrame);
      invalidate = kTRUE;
      break;
   case kKey_T:
   case kKey_t:
      gVirtualGL->EnableGL(kLIGHTING);
      gVirtualGL->EnableGL(kCULL_FACE);
      gVirtualGL->PolygonGLMode(kFRONT, kFILL);
      gVirtualGL->ClearGLColor(white[0], white[1], white[2], white[3]);
      fScene.SetDrawMode(TGLScene::kOutline);
      invalidate = kTRUE;
      break;
   case kKey_Up:
      invalidate = CurrentCamera().Truck(fViewport.CenterX(), fViewport.CenterY(), 0, 5);
      break;
   case kKey_Down:
      invalidate = CurrentCamera().Truck(fViewport.CenterX(), fViewport.CenterY(), 0, -5);
      break;
   case kKey_Left:
      invalidate = CurrentCamera().Truck(fViewport.CenterX(), fViewport.CenterY(), -5, 0);
      break;
   case kKey_Right:
      invalidate = CurrentCamera().Truck(fViewport.CenterX(), fViewport.CenterY(), 5, 0);
      break;
   // Toggle debugging mode
   case kKey_D:
   case kKey_d:
      fDebugMode = !fDebugMode;
      invalidate = kTRUE;
      Info("OpenGL viewer debug mode : ", fDebugMode ? "ON" : "OFF");
      break;
   // Forced rebuild for debugging mode
   case kKey_Space:
      if (fDebugMode) {
         Info("OpenGL viewer FORCED rebuild", "");
         RebuildScene();
      }
   }

   if (invalidate) {
      Invalidate();
   }
   
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TViewerOpenGL::HandleContainerMotion(Event_t *event)
{
   if (fScene.IsLocked()) {
      if (gDebug>3) {
         Info("TViewerOpenGL::HandleContainerMotion", "ignored - scene is %s", TGLScene::LockName(fScene.CurrentLock()));
      }
      return kFALSE;
   }

   if (!event) {
      return kFALSE;
   }
   
   Bool_t invalidate = kFALSE;
   
   Int_t xDelta = event->fX - fLastPos.fX;
   Int_t yDelta = event->fY - fLastPos.fY;
   
   // Camera interface requires GL coords - Y inverted
   if (fAction == kRotate) {
      invalidate = CurrentCamera().Rotate(xDelta, -yDelta);
   } else if (fAction == kTruck) {
      invalidate = CurrentCamera().Truck(event->fX, fViewport.Y() - event->fY, xDelta, -yDelta);
   } else if (fAction == kDolly) {
      invalidate = CurrentCamera().Dolly(xDelta, event->fState & kKeyControlMask, 
                                                 event->fState & kKeyShiftMask);
   } else if (fAction == kDrag) {
      TGLPhysicalShape * selected = fScene.GetSelected();
      if (selected) {
         TGLVector3 shift = CurrentCamera().ProjectedShift(selected->BoundingBox().Center(), xDelta, -yDelta);
         selected->Shift(shift);
         fGeomEditor->SetCenter(selected->GetTranslation().CArr());
         fScene.SelectedModified();
         Invalidate();
      }
   }

   fLastPos.fX = event->fX;
   fLastPos.fY = event->fY;
   
   if (invalidate) {
      Invalidate();
   }
   
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TViewerOpenGL::HandleContainerExpose(Event_t *)
{
   if (fScene.IsLocked()) {
      if (gDebug>3) {
         Info("TViewerOpenGL::HandleContainerExpose", "ignored - scene is %s", TGLScene::LockName(fScene.CurrentLock()));
      }
      return kFALSE;
   }

   Invalidate(kHigh);
   return kTRUE;
}

//______________________________________________________________________________
void TViewerOpenGL::DoSelect(Event_t *event, Bool_t invokeContext)
{
   // Take select lock on scene immediately we enter here - it is released
   // in the other (drawing) thread - see TGLViewer::Select()
   // Removed when gVirtualGL removed
   if (!fScene.TakeLock(TGLScene::kSelectLock)) {
      return;
   }

   // TODO: Check only the GUI thread ever enters here & DoSelect.
   // Then TVirtualGL and TGLKernel can be obsoleted.
   TGLRect selectRect(event->fX, event->fY, 3, 3); // TODO: Constant somewhere
   gVirtualGL->SelectViewer(this, &selectRect); 
      
   // Do this regardless of whether selection actually changed - safe and 
   // the context menu may need to be invoked anyway
   TGLPhysicalShape * selected = fScene.GetSelected();
   if (selected) {
      fColorEditor->SetRGBA(selected->GetColor());
      fGeomEditor->SetCenter(selected->GetTranslation().CArr());
      fGeomEditor->SetScale(selected->GetScale().CArr());
      if (invokeContext) {
         if (!fContextMenu) fContextMenu = new TContextMenu("glcm", "glcm");
         
         // Defer creating the context menu to the actual object
         selected->InvokeContextMenu(*fContextMenu, event->fXRoot, event->fYRoot);
      }
   } else { // No selection
      fColorEditor->Disable();
      fGeomEditor->Disable();
   }
}

//______________________________________________________________________________
void TViewerOpenGL::Show()
{
   if (fScene.IsLocked()) {
      Error("TViewerOpenGL::Show", "scene is %s", TGLScene::LockName(fScene.CurrentLock()));   
   }
   MapRaised();

   // Must NOT Invalidate() here as for some reason it throws the win32
   // GL kernel impl into a blank locked state? Poss related to having an
   // empty viewer - nothing drawn. TODO: Investigate why....
}

//______________________________________________________________________________
void TViewerOpenGL::CloseWindow() 
{
   fPad->ReleaseViewer3D();   
   TTimer::SingleShot(50, IsA()->GetName(), this, "ReallyDelete()");
}

//______________________________________________________________________________
void TViewerOpenGL::DoRedraw()
{
   // Take draw lock on scene immediately we enter here - it is released
   // in the other (drawing) drawing thread - see TGLViewer::Draw()
   // Removed when gVirtualGL removed
   if (!fScene.TakeLock(TGLScene::kDrawLock)) {
      // If taking drawlock fails the previous draw is still in progress
      // set timer to do this one later
      if (gDebug>3) {
         Info("TViewerOpenGL::DoRedraw", "scene drawlocked - requesting another draw");
      }
      fRedrawTimer->RequestDraw(100, fNextSceneLOD);
      return;
   }

   if (gDebug>3) {
      Info("TViewerOpenGL::DoRedraw", "request draw at %d LOD on this = %d", fNextSceneLOD, this);
   }

   // TODO: Check only the GUI thread ever enters here, DoSelect() and PrintObjects().
   // Then TVirtualGL and TGLKernel can be obsoleted and all GL context work done
   // in GUI thread.
   gVirtualGL->DrawViewer(this);
}

//______________________________________________________________________________
void TViewerOpenGL::PrintObjects()
{
   if (fScene.IsLocked()) {
      if (gDebug>3) {
         Info("TViewerOpenGL::PrintObjects", "ignored - scene is %s", TGLScene::LockName(fScene.CurrentLock()));
      }
      return;
   }

   // Generates a PostScript or PDF output of the OpenGL scene. They are vector
   // graphics files and can be huge and long to generate.
    TGLBoundingBox sceneBox = fScene.BoundingBox();
    gVirtualGL->PrintObjects(format, sortgl, this, fCanvasContainer->GetGLWindow(),
                             sceneBox.Extents().Mag(), sceneBox.Center().Y(), sceneBox.Center().Z());
}

//______________________________________________________________________________
Bool_t TViewerOpenGL::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   if (fScene.IsLocked()) {
      if (gDebug>3) {
         Info("TViewerOpenGL::ProcessMessage", "ignored - scene is %s", TGLScene::LockName(fScene.CurrentLock()));
      }
      return kFALSE;
   }

   switch (GET_MSG(msg)) {
   case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {
      case kCM_BUTTON:
	   case kCM_MENU:
	      switch (parm1) {
         case kGLHelpAbout: {
            char str[32];
            sprintf(str, "About ROOT %s...", gROOT->GetVersion());
            TRootHelpDialog * hd = new TRootHelpDialog(this, str, 600, 400);
            hd->SetText(gHelpAbout);
            hd->Popup();
            break;
         }
         case kGLHelpOnViewer: {
            TRootHelpDialog * hd = new TRootHelpDialog(this, "Help on GL Viewer...", 600, 400);
            hd->SetText(gHelpViewerOpenGL);
            hd->Popup();
            break;
         }
         case kGLPrintEPS_SIMPLE:
            format = GL2PS_EPS;
	         sortgl = GL2PS_SIMPLE_SORT;
            PrintObjects();
            break;
         case kGLPrintEPS_BSP:
            format = GL2PS_EPS;
	         sortgl = GL2PS_BSP_SORT;
            PrintObjects();
            break;
         case kGLPrintPDF_SIMPLE:
            format = GL2PS_PDF;
	         sortgl = GL2PS_SIMPLE_SORT;
            PrintObjects();
            break;
         case kGLPrintPDF_BSP:
            format = GL2PS_PDF;
	         sortgl = GL2PS_BSP_SORT;
            PrintObjects();
            break;
            case kGLXOY:
            SetCurrentCamera(kXOY);
            break;
         case kGLXOZ:
            SetCurrentCamera(kXOZ);
            break;
         case kGLYOZ:
            SetCurrentCamera(kYOZ);
            break;
        case kGLPersp:
           SetCurrentCamera(kPerspective);
           break;
         case kGLExit:
            CloseWindow();
            break;
	      default:
	         break;
	      }
	   default:
	      break;
      }
   default:
      break;
   }

   return kTRUE;
}

//______________________________________________________________________________
void TViewerOpenGL::ModifyScene(Int_t wid)
{
   if (!fScene.TakeLock(TGLScene::kModifyLock)) {
      return;
   }

   MakeCurrent();

   TGLPhysicalShape * selected = fScene.GetSelected();
   switch (wid) {
   case kTBa:
      if (selected) {
         selected->SetColor(fColorEditor->GetRGBA());
      }
      break;
   case kTBaf:
      if (selected) {
         fScene.SetPhysicalsColorByLogical(selected->GetLogical().ID(), 
                                           fColorEditor->GetRGBA());
      }
      break;
   case kTBa1:
      {
         if (selected) {
            TGLVertex3 trans;
            TGLVector3 scale;
            fGeomEditor->GetObjectData(trans.Arr(), scale.Arr());
            selected->SetTranslation(trans);
            selected->SetScale(scale);
            fScene.SelectedModified();
         }
      }
      break;
   case kTBda:
      fDrawAxes = !fDrawAxes;
      break;
   case kTBcp:
      fUseClipPlane = !fUseClipPlane;
   case kTBcpm:
      {
         Double_t eqn[4] = {0.};
         fSceneEditor->GetPlaneEqn(eqn);
         fClipPlane.Set(eqn, kFALSE); // Don't normalise
         break;
      }
   case kTBTop:
      if ((fLightMask ^= 1) & 1) gVirtualGL->EnableGL(kLIGHT4);
      else gVirtualGL->DisableGL(kLIGHT4);
      break;
   case kTBRight:
      if ((fLightMask ^= 2) & 2) gVirtualGL->EnableGL(kLIGHT1);
      else gVirtualGL->DisableGL(kLIGHT1);
      break;
   case kTBBottom:
      if ((fLightMask ^= 4) & 4) gVirtualGL->EnableGL(kLIGHT2);
      else gVirtualGL->DisableGL(kLIGHT2);
      break;
   case kTBLeft:
      if ((fLightMask ^= 8) & 8) gVirtualGL->EnableGL(kLIGHT3);
      else gVirtualGL->DisableGL(kLIGHT3);
      break;
   case kTBFront:
      if ((fLightMask ^= 16) & 16) gVirtualGL->EnableGL(kLIGHT0);
      else gVirtualGL->DisableGL(kLIGHT0);
      break;
   }

   fScene.ReleaseLock(TGLScene::kModifyLock);

   Invalidate();
}
