// @(#)root/reflex:$Name:  $:$Id$
// Author: Axel Naumann, 2007

// Copyright CERN, CH-1211 Geneva 23, 2004-2007, All rights reserved.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose is hereby granted without fee, provided that this copyright and
// permissions notice appear in all copies and derivatives.
//
// This software is provided "as is" without express or implied warranty.

#ifndef REFLEX_BUILD
#define REFLEX_BUILD
#endif

#include "Reflex/Catalog.h"
#include "CatalogImpl.h"

//-------------------------------------------------------------------------------
Reflex::Catalog::Catalog()
   : fImpl(&Internal::CatalogImpl::Instance()) {
//-------------------------------------------------------------------------------
// Construct a catalog object pointing to the static implementation object.
}

//-------------------------------------------------------------------------------
Reflex::Catalog::Catalog(const std::string& name)
   : fImpl(new Internal::CatalogImpl(name)) {
//-------------------------------------------------------------------------------
// Construct a new named catalog.
}

//-------------------------------------------------------------------------------
Reflex::Catalog::Catalog(Reflex::Internal::CatalogImpl* impl)
   : fImpl(impl) {
//-------------------------------------------------------------------------------
// Construct a new catalog given an implementation.
// Used internally by CatalogImpl.
}

//-------------------------------------------------------------------------------
Reflex::Scope
Reflex::Catalog::GlobalScope() const {
//-------------------------------------------------------------------------------
// Return the global scope
   return fImpl->Scopes().GlobalScope();
}


//-------------------------------------------------------------------------------
Reflex::Scope
Reflex::Catalog::__NIRVANA__() const {
//-------------------------------------------------------------------------------
// Return the scope "containing" the global scope
   return fImpl->Scopes().__NIRVANA__();
}


//-------------------------------------------------------------------------------
Reflex::Catalog
Reflex::Catalog::Instance() {
//-------------------------------------------------------------------------------
// Return the static catalog instance
   static Catalog sCatalog(&Internal::CatalogImpl::Instance());
   return sCatalog;
}

//-------------------------------------------------------------------------------
const Reflex::Callback&
Reflex::Catalog::RegisterCallback(const CallbackInterface* ci,
                                  const NotifySelection& ns) const {
//-------------------------------------------------------------------------------
// Register a callback object deriving from CallbackInterface.
// ns selects when the callback should be invoked.
   Callback* ret = new Callback(ci, ns);
   fImpl->RegisterCallback(ret);
   return *ret;
}

//-------------------------------------------------------------------------------
const Reflex::Callback&
Reflex::Catalog::RegisterCallback(FreeCallbackFunc_t callback,
                                  const NotifySelection& ns, void* userData) const {
//-------------------------------------------------------------------------------
// Register a callback function.
// ns selects when the callback should be invoked.
   Callback* ret = new Callback(callback, ns, userData);
   fImpl->RegisterCallback(ret);
   return *ret;
}

//-------------------------------------------------------------------------------
Reflex::Type
Reflex::Catalog::TypeByName(const std::string& name) const {
//-------------------------------------------------------------------------------
   if (name[0] == ':' && name[1] == ':')
      return fImpl->Types().ByName(name.substr(2));
   else
      return fImpl->Types().ByName(name);
}

//-------------------------------------------------------------------------------
Reflex::Type
Reflex::Catalog::TypeByTypeInfo(const std::type_info & ti) const {
//-------------------------------------------------------------------------------
   return fImpl->Types().ByTypeInfo(ti);
}


//-------------------------------------------------------------------------------
Reflex::Scope
Reflex::Catalog::ScopeByName(const std::string& name) const {
//-------------------------------------------------------------------------------
   return fImpl->Scopes().ByName(name);
}


//-------------------------------------------------------------------------------
Reflex::Type_Iterator
Reflex::Catalog::Type_Begin() const {
//-------------------------------------------------------------------------------
// iterator access
   return fImpl->Types().TypeVec().begin();
}


//-------------------------------------------------------------------------------
Reflex::Type_Iterator
Reflex::Catalog::Type_End() const {
//-------------------------------------------------------------------------------
// iterator access
   return fImpl->Types().TypeVec().end();
}


//-------------------------------------------------------------------------------
Reflex::Reverse_Type_Iterator
Reflex::Catalog::Type_RBegin() const {
//-------------------------------------------------------------------------------
// iterator access
   return fImpl->Types().TypeVec().rbegin();
}


//-------------------------------------------------------------------------------
Reflex::Reverse_Type_Iterator
Reflex::Catalog::Type_REnd() const {
//-------------------------------------------------------------------------------
// iterator access
   return fImpl->Types().TypeVec().rend();
}


//-------------------------------------------------------------------------------
Reflex::Type
Reflex::Catalog::TypeAt(size_t nth) const {
//-------------------------------------------------------------------------------
   if (ScopeSize() > nth)
      return fImpl->Types().TypeVec()[nth];
   return Dummy::Type();
}


//-------------------------------------------------------------------------------
size_t
Reflex::Catalog::TypeSize() const {
//-------------------------------------------------------------------------------
   return fImpl->Types().TypeVec().size();
}


//-------------------------------------------------------------------------------
Reflex::Scope_Iterator
Reflex::Catalog::Scope_Begin() const {
//-------------------------------------------------------------------------------
   return fImpl->Scopes().ScopeVec().begin();
}


//-------------------------------------------------------------------------------
Reflex::Scope_Iterator
Reflex::Catalog::Scope_End() const {
//-------------------------------------------------------------------------------
   return fImpl->Scopes().ScopeVec().end();
}


//-------------------------------------------------------------------------------
Reflex::Reverse_Scope_Iterator
Reflex::Catalog::Scope_RBegin() const {
//-------------------------------------------------------------------------------
   return fImpl->Scopes().ScopeVec().rbegin();
}


//-------------------------------------------------------------------------------
Reflex::Reverse_Scope_Iterator
Reflex::Catalog::Scope_REnd() const {
//-------------------------------------------------------------------------------
   return fImpl->Scopes().ScopeVec().rend();
}


//-------------------------------------------------------------------------------
Reflex::Scope
Reflex::Catalog::ScopeAt(size_t nth) const {
//-------------------------------------------------------------------------------
   if (ScopeSize() > nth)
      return fImpl->Scopes().ScopeVec()[nth];
   return Dummy::Scope();
}


//-------------------------------------------------------------------------------
size_t
Reflex::Catalog::ScopeSize() const {
//-------------------------------------------------------------------------------
   return fImpl->Scopes().ScopeVec().size();
}


//-------------------------------------------------------------------------------
void
Reflex::Catalog::UnregisterCallback(const Callback& cb) const {
//-------------------------------------------------------------------------------
   Callback* cbP = const_cast<Callback*>(&cb);
   fImpl->UnregisterCallback(cbP);
}


//-------------------------------------------------------------------------------
void
Reflex::Catalog::Unload() {
//-------------------------------------------------------------------------------
   if (fImpl != &Internal::CatalogImpl::Instance()) {
      delete fImpl;
      fImpl = 0;
   }
}
