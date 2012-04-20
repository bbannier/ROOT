//Author: Timur Pocheptsov 16/02/2012

#ifndef ROOT_X11Drawable
#define ROOT_X11Drawable

#import <Cocoa/Cocoa.h>

#import "GuiTypes.h"

@class PassiveKeyGrab;
@class QuartzWindow;
@class QuartzPixmap;
@class QuartzImage;
@class QuartzView;

/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// Protocol for "drawables". It can be window, view (child window), pixmap.        //
//                                                                                 //
/////////////////////////////////////////////////////////////////////////////////////

@protocol X11Drawable 
@optional

@property (nonatomic, assign) unsigned fID;   //Drawable's id for GUI and TGCocoa.

//In X11 drawable is window or pixmap, ROOT's GUI
//also has this ambiguity. So I have a property
//to check in TGCocoa, what's the object.
- (BOOL) fIsPixmap;
//Special case for OpenGL.
- (BOOL) fIsOpenGLWidget;

//If we draw "in a normal mode" == drawing operation were called
//from view's drawRect method, this property is not nil.
//TGCocoa, when some drawing method is called, will ask view/window
//about context, if it's not nil - we'll use it, but if it is nil,
//we try to get current context and lock focus on the current view.
@property (nonatomic, readonly) CGContextRef  fContext;

//Geometry: (readonly)
- (int)      fX;
- (int)      fY; //top-left corner system.
- (unsigned) fWidth;
- (unsigned) fHeight;
//Geometry: setters.
- (void)     setDrawableSize : (NSSize) newSize;
- (void)     setX : (int) x Y : (int) y width : (unsigned) w height : (unsigned) h;
- (void)     setX : (int) x Y : (int) y;

//Functions to copy one drawable into another or
//get access to pixel data.
//Point_t, Rectangle_t are in GuiTypes.h
- (void)     copy : (id<X11Drawable>) src area : (Rectangle_t) area withMask : (QuartzImage *)mask 
             clipOrigin : (Point_t) origin toPoint : (Point_t) dstPoint;

- (unsigned char *) readColorBits : (Rectangle_t) area;

@end

@protocol X11Window <X11Drawable>
@optional

//I have to somehow emulate X11's behavior to make ROOT's GUI happy,
//that's why I have this bunch of properties here to be set/read from a window.
//Some of them are used, some are just pure "emulation".
//Properties, which are used, are commented in a declaration.

/////////////////////////////////////////////////////////////////
//SetWindowAttributes_t/WindowAttributes_t

@property (nonatomic, assign) long          fEventMask; //Specifies which events must be processed by widget.
@property (nonatomic, assign) int           fClass;
@property (nonatomic, assign) int           fDepth;
@property (nonatomic, assign) int           fBitGravity;
@property (nonatomic, assign) int           fWinGravity;
@property (nonatomic, assign) unsigned long fBackgroundPixel;//Used by TGCocoa::ClearArea.
@property (nonatomic, readonly) int         fMapState;

//End of SetWindowAttributes_t/WindowAttributes_t
/////////////////////////////////////////////////////////////////


//"Back buffer" is a bitmap, used by canvas window (only).
@property (nonatomic, assign) QuartzPixmap *fBackBuffer;
//Parent view can be only QuartzView.
@property (nonatomic, assign) QuartzView *fParentView;
@property (nonatomic, assign) unsigned    fLevel;//Window's "level" in a hierarchy.
//Window has a content view, self is a content view for a view.
//I NSView is a parent for QuartzView and ROOTOpenGLView.
@property (nonatomic, readonly) NSView<X11Window> *fContentView;
@property (nonatomic, readonly) QuartzWindow      *fQuartzWindow;

//Passive button grab emulation.
//ROOT's GUI does not use several passive button
//grabs on the same window, so no containers,
//just one grab.
@property (nonatomic, assign) int      fGrabButton;
@property (nonatomic, assign) unsigned fGrabButtonEventMask;
@property (nonatomic, assign) unsigned fGrabKeyModifiers;
@property (nonatomic, assign) BOOL     fOwnerEvents;

//Nested views ("windows").
//Child can be any view, inherited
//from NSView adopting X11Window protocol.
- (void) addChild : (NSView<X11Window> *) child;

//X11/ROOT GUI's attributes
- (void) getAttributes : (WindowAttributes_t *) attr;
- (void) setAttributes : (const SetWindowAttributes_t *) attr;

//X11's XMapWindow etc.
- (void) mapRaised;
- (void) mapWindow;
- (void) mapSubwindows;
- (void) unmapWindow;
- (void) raiseWindow;
- (void) lowerWindow;

- (BOOL) fIsOverlapped;
- (void) setOverlapped : (BOOL) overlap;
- (void) updateLevel : (unsigned) newLevel;
- (void) configureNotifyTree;

- (void) addPassiveKeyGrab : (unichar) keyCode modifiers : (NSUInteger) modifiers;
- (void) removePassiveKeyGrab : (unichar) keyCode modifiers : (NSUInteger) modifiers;
- (PassiveKeyGrab *) findPassiveKeyGrab : (unichar) keyCode modifiers : (NSUInteger) modifiers;
- (PassiveKeyGrab *) findPassiveKeyGrab : (unichar) keyCode;

@end

#endif
