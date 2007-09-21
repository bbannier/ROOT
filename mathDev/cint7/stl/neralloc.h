/* -*- C++ -*- */

/************************************************************************
 *
 * Copyright(c) 1995~2006  Masaharu Goto (cint@pcroot.cern.ch)
 *
 * For the licensing terms see the file COPYING
 *
 ************************************************************************/

/*
 *
 * Copyright (c) 1994
 * Hewlett-Packard Company
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Hewlett-Packard Company makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

#ifndef NEARALLOC_H
#define NEARALLOC_H

#ifdef FARALLOC_H
#undef FARALLOC_H
#define __FARALLOC_WAS_DEFINED
#endif

#define __far __near
#define far_allocator near_allocator
#include <faralloc.h>
#undef __far
#undef far_allocator

#undef FARALLOC_H

#ifdef __FARALLOC_WAS_DEFINED
#define FARALLOC_H
#undef  __FARALLOC_WAS_DEFINED
#endif

#endif

