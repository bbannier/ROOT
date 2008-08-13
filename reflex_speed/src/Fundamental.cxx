// @(#)root/reflex:$Id$
// Author: Stefan Roiser 2004

// Copyright CERN, CH-1211 Geneva 23, 2004-2006, All rights reserved.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose is hereby granted without fee, provided that this copyright and
// permissions notice appear in all copies and derivatives.
//
// This software is provided "as is" without express or implied warranty.

#ifndef REFLEX_BUILD
#define REFLEX_BUILD
#endif

#include "Fundamental.h"

#include "Reflex/Type.h"
#include "OwnedMember.h"

//-------------------------------------------------------------------------------
Reflex::Internal::Fundamental::Fundamental(const char * typ,
                                           size_t size,
                                           const std::type_info & ti,
                                           const Catalog& cat) 
//-------------------------------------------------------------------------------
   : TypeBase(typ, 0, size, kFundamental, ti, cat) {
   // Create dictionary info for this fundamental type.
}

