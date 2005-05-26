// Author:  Richard Maunder  25/05/2005

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TGLViewer
#define ROOT_TGLViewer

#ifndef ROOT_TGLOrthoCamera
#include "TGLScene.h"
#endif
#ifndef ROOT_TGLPerspectiveCamera
#include "TGLPerspectiveCamera.h"
#endif
#ifndef ROOT_TGLOrthoCamera
#include "TGLOrthoCamera.h"
#endif
#ifndef ROOT_TTimer
#include "TTimer.h"
#endif

#include <vector>

class TGLRedrawTimer;

/*************************************************************************
 * TGLViewer - TODO
 *
 *
 *
 *************************************************************************/
class TGLViewer // TODO: Find a better name to avoid confusion with TViewerOpenGL
{
public:
   enum ECamera { kPerspective, kXOY, kYOZ, kXOZ };

private:   
          
   // Fields
   TGLRedrawTimer     * fRedrawTimer;     
   UInt_t               fNextSceneLOD;       //! 
   TGLCamera          * fCurrentCamera;      //!
   // TODO: Put in vector and allow external creation
   TGLPerspectiveCamera fPerspectiveCamera;  //!
   TGLOrthoCamera       fOrthoXOYCamera;     //!
   TGLOrthoCamera       fOrthoYOZCamera;     //!
   TGLOrthoCamera       fOrthoXOZCamera;     //!

   // Methods
   void PreDraw();
   void PostDraw();

   // Non-copyable class
   TGLViewer(const TGLViewer &);
   TGLViewer & operator=(const TGLViewer &);

protected:
   // Fields
   // Scene is created/owned internally. 
   // In future it will be shaped between multiple viewers
   TGLScene       fScene;     //! the GL scene - owned by viewer at present
   TGLRect        fViewport;  //! viewport - drawn area
   Bool_t         fInitGL;    //! has GL been initialised?

   // Methods
   virtual void InitGL()                            = 0;
   virtual void MakeCurrent()  const                = 0;
   virtual void SwapBuffers()  const                = 0;
   virtual void RebuildScene()                      = 0;
        
   void   SetViewport(Int_t x, Int_t y, UInt_t width, UInt_t height); 

   void SetupCameras(const TGLBoundingBox & box);
   void SetCurrentCamera(ECamera camera);
   TGLCamera & CurrentCamera() const { return *fCurrentCamera; }

public:
   TGLViewer();
   virtual ~TGLViewer();
         
   void WindowToGL(TGLRect & rect)      const { rect.Y() = fViewport.Height() - rect.Y(); }
   void WindowToGL(TGLVertex3 & vertex) const { vertex.Y() = fViewport.Height() - vertex.Y(); }
   
   // Once TVirtualGL dropped these can move back to protected
   void   Draw();
   Bool_t Select(const TGLRect & rect); // Window coords origin top left

   // TODO: Once better solution to TGLRedrawTimer found make this
   // protected again.
   virtual void Invalidate(UInt_t redrawLOD = kMed) = 0;
   
   ClassDef(TGLViewer,0) // GL viewer generic base class
};

#endif // ROOT_TGLViewer
