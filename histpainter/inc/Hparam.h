/* @(#)root/histpainter:$Name$:$Id$ */

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_Hparam
#define ROOT_Hparam


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// THparam                                                              //
//                                                                      //
// structure to store current histogram parameters.                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef ROOT_Htypes
#include "Htypes.h"
#endif

typedef struct Hparam_t {
//*-*-     structure to store current histogram parameters
//*-*      ===============================================
//*-*
   Int_t    xfirst;        //first bin number along X
   Int_t    xlast;         //last bin number along X
   Float_t  xbinsize;      //bin size in case of equidistant bins
   Float_t  xlowedge;      //low edge of axis
   Float_t  xmin;          //minimum value along X
   Float_t  xmax;          //maximum value along X
   Int_t    yfirst;        //first bin number along Y
   Int_t    ylast;         //last bin number along Y
   Float_t  ybinsize;      //bin size in case of equidistant bins
   Float_t  ylowedge;      //low edge of axis
   Float_t  ymin;          //minimum value along y
   Float_t  ymax;          //maximum value along y
   Int_t    zfirst;        //first bin number along Z
   Int_t    zlast;         //last bin number along Z
   Float_t  zbinsize;      //bin size in case of equidistant bins
   Float_t  zlowedge;      //low edge of axis
   Float_t  zmin;          //minimum value along Z
   Float_t  zmax;          //maximum value along Z
   Float_t  factor;        //multiplication factor (normalization)
   Stat_t   allchan;       //integrated sum of contents
   Float_t  baroffset;     //offset of bin for bars or legos [0,1]
   Float_t  barwidth;      //width of bin for bars and legos [0,1]
} Hparam_t;

#endif
