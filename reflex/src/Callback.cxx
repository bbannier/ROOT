// @(#)root/reflex:$Name:  $:$Id: Callback.cxx,v 1.4 2006/03/06 12:51:46 roiser Exp $
// Author: Stefan Roiser 2004

// Copyright CERN, CH-1211 Geneva 23, 2004-2006, All rights reserved.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose is hereby granted without fee, provided that this copyright and
// permissions notice appear in all copies and derivatives.
//
// This software is provided "as is" without express or implied warranty.

#define REFLEX_BUILD

#include "Reflex/Kernel.h"
#include "Reflex/Callback.h"
#include <list>

// typedef std::list<ROOT::Reflex::ICallback*> CbList;
// On RH7.3 the callback list is destructed before clients Get
// the chance to uninstall their callbacks. So, let's build some
// protection. 
class  CbList : public std::list<ROOT::Reflex::ICallback*> 
{
public:
   CbList() : fAlive(true) {}
   ~CbList() { fAlive = false; }
   bool IsAlive() { return fAlive; }
private:
   typedef bool Bool_t;
   Bool_t fAlive;
};

//------------------------------------------------------------------------------
static CbList & sClassCallbacks() {
//------------------------------------------------------------------------------
   static CbList m;
   return m;
}

//-------------------------------------------------------------------------------
void ROOT::Reflex::InstallClassCallback( ROOT::Reflex::ICallback * cb ) {
//-------------------------------------------------------------------------------
   sClassCallbacks().push_back( cb );
}

//-------------------------------------------------------------------------------
void ROOT::Reflex::UninstallClassCallback( ROOT::Reflex::ICallback * cb ) {
//-------------------------------------------------------------------------------
   if( sClassCallbacks().IsAlive() ) {
      sClassCallbacks().remove( cb );
   }
}

//-------------------------------------------------------------------------------
void ROOT::Reflex::FireClassCallback( const ROOT::Reflex::Type & ty ) {
//-------------------------------------------------------------------------------
   for ( CbList::const_iterator i = sClassCallbacks().begin(); 
         i != sClassCallbacks().end(); i++ ) {
      (**i)(ty);
   }
}

//-------------------------------------------------------------------------------
void ROOT::Reflex::FireFunctionCallback( const ROOT::Reflex::Member & mem ) {
//-------------------------------------------------------------------------------
   for ( CbList::const_iterator i = sClassCallbacks().begin(); 
         i != sClassCallbacks().end(); i++ ) {
      (**i)(mem);
   }
}

