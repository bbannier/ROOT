/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 *    File: $Id: RooCmdArg.h,v 1.10 2007/05/11 09:11:30 verkerke Exp $
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

#ifndef ROO_CMD_ARG
#define ROO_CMD_ARG

#include "TNamed.h"
#include "TString.h"
#include "RooLinkedList.h"
class RooAbsData ;


class RooCmdArg : public TNamed {
public:

  RooCmdArg();
  RooCmdArg(const char* name, 
	    Int_t i1=0, Int_t i2=0, 
	    Double_t d1=0, Double_t d2=0, 
	    const char* s1=0, const char* s2=0, 
	    const TObject* o1=0, const TObject* o2=0, const RooCmdArg* ca=0, const char* s3=0) ;
  RooCmdArg(const RooCmdArg& other) ;
  RooCmdArg& operator=(const RooCmdArg& other) ;
  void addArg(const RooCmdArg& arg) ;
  void setProcessRecArgs(Bool_t flag) { 
    // If true flag this object as containing recursive arguments
    _procSubArgs = flag ; 
  }

  const RooLinkedList& subArgs() const { 
    // Return list of sub-arguments in this RooCmdArg
    return _argList ; 
  }

  virtual TObject* Clone(const char* newName=0) const {
    RooCmdArg* newarg = new RooCmdArg(*this) ;
    if (newName) { newarg->SetName(newName) ; }
    return newarg ;
  }

  ~RooCmdArg();

  static const RooCmdArg& none() ;

  const char* opcode() const { 
    // Return operator code
    return strlen(GetName()) ? GetName() : 0 ; 
  }
  Int_t getInt(Int_t idx) const { 
    // Return integer stored in slot idx
    return _i[idx] ; 
  }
  Double_t getDouble(Int_t idx) const { 
    // Return double stored in slot idx
    return _d[idx] ; 
  }
  const char* getString(Int_t idx) const { 
    // Return string stored in slot idx
      return _s[idx] ; 
  }
  const TObject* getObject(Int_t idx) const { 
  // Return TObject stored in slot idx
    return _o[idx] ; 
  }

protected:

  static const RooCmdArg _none  ; // Static instance of null object
  friend class RooCmdConfig ;

private:

  friend class RooAbsCollection ;

  // Payload
  Double_t _d[2] ;       // Payload doubles
  Int_t _i[2] ;          // Payload integers
  const char* _s[3] ;    // Payload strings
  TObject* _o[2] ;       // Payload objects
  Bool_t _procSubArgs ;  // If true argument requires recursive processing
  RooLinkedList _argList ; // Payload sub-arguments
	
  ClassDef(RooCmdArg,0) // Generic named argument container
};

#endif


