/*****************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: RooFitCore
 *    File: $Id$
 * Authors:
 *   DK, David Kirkby, Stanford University, kirkby@hep.stanford.edu
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu
 * History:
 *   07-Mar-2001 WV Created initial version
 *
 * Copyright (C) 2001 University of California
 *****************************************************************************/

#include "RooFitCore/RooSetProxy.hh"
#include "RooFitCore/RooArgSet.hh"
#include "RooFitCore/RooAbsArg.hh"

ClassImp(RooSetProxy)
;


RooSetProxy::RooSetProxy(const char* name, const char* desc, RooAbsArg* owner, RooArgSet& set,
			 Bool_t valueServer, Bool_t shapeServer) : 
  RooAbsProxy(name,desc,valueServer,shapeServer), _set(&set)
{
  owner->registerProxy(*this) ;
}


RooSetProxy::RooSetProxy(const char* name, RooAbsArg* owner, const RooSetProxy& other) : 
  RooAbsProxy(name,other), _set(other._set)
{
  owner->registerProxy(*this) ;
}


Bool_t RooSetProxy::changePointer(const RooArgSet& newServerList) 
{
  TIterator* iter = _set->MakeIterator() ;
  RooAbsArg* arg ;
  Bool_t ok(kTRUE) ;
  while (arg=(RooAbsArg*)iter->Next()) {
    RooAbsArg* newArg = newServerList.find(arg->GetName()) ;
    if (newArg) ok |= _set->replace(*arg,*newArg) ;
  }

  return ok ;
}

