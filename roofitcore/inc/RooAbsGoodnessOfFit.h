/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 *    File: $Id: RooAbsGoodnessOfFit.rdl,v 1.3 2002/09/05 04:33:05 verkerke Exp $
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2002, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/
#ifndef ROO_ABS_GOODNESS_OF_FIT
#define ROO_ABS_GOODNESS_OF_FIT

#include "RooFitCore/RooAbsReal.hh"
#include "RooFitCore/RooSetProxy.hh"

class RooArgSet ;
class RooAbsData ;
class RooAbsPdf ;
class RooSimultaneous ;
class RooRealMPFE ;

class RooAbsGoodnessOfFit ;
typedef RooAbsGoodnessOfFit* pRooAbsGoodnessOfFit ;
typedef RooAbsData* pRooAbsData ;
typedef RooRealMPFE* pRooRealMPFE ;

class RooAbsGoodnessOfFit : public RooAbsReal {
public:

  // Constructors, assignment etc
  inline RooAbsGoodnessOfFit() { }
  RooAbsGoodnessOfFit(const char *name, const char *title, RooAbsPdf& pdf, RooAbsData& data,
		      const RooArgSet& projDeps, Int_t nCPU=1) ;
  RooAbsGoodnessOfFit(const RooAbsGoodnessOfFit& other, const char* name=0);
  virtual ~RooAbsGoodnessOfFit();
  virtual RooAbsGoodnessOfFit* create(const char *name, const char *title, RooAbsPdf& pdf, RooAbsData& data,
				      const RooArgSet& projDeps, Int_t nCPU=1) = 0 ;

  virtual void constOptimize(ConstOpCode opcode) ;
  virtual Double_t combinedValue(RooAbsReal** gofArray, Int_t nVal) const = 0 ;

protected:

  virtual Bool_t redirectServersHook(const RooAbsCollection& newServerList, Bool_t mustReplaceAll, Bool_t nameChange, Bool_t isRecursive) ;
  virtual Double_t evaluate() const ;

  virtual Double_t evaluatePartition(Int_t firstEvent, Int_t lastEvent) const = 0 ;

  void setMPSet(Int_t setNum, Int_t numSets) ; 
  void setSimCount(Int_t simCount) { _simCount = simCount ; }

  RooSetProxy _paramSet ;

  enum GOFOpMode { SimMaster,MPMaster,Slave } ;
  GOFOpMode operMode() const { return _gofOpMode ; }

  // Original arguments
  RooAbsPdf* _pdf ;
  RooAbsData* _data ;
  const RooArgSet* _projDeps ;
  Int_t _simCount ;

private:  

  Bool_t initialize() ;
  void initSimMode(RooSimultaneous* pdf, RooAbsData* data, const RooArgSet* projDeps) ;    
  void initMPMode(RooAbsPdf* pdf, RooAbsData* data, const RooArgSet* projDeps) ;

  mutable Bool_t _init ;
  GOFOpMode   _gofOpMode ;

  Int_t       _nEvents ;
  Int_t       _setNum ;
  Int_t       _numSets ;

  // Simultaneous mode data
  Int_t          _nGof        ; // Number of sub-contexts 
  pRooAbsGoodnessOfFit* _gofArray ; //! Array of sub-contexts representing part of the total NLL

  // Parallel mode data
  Int_t          _nCPU ;
  pRooRealMPFE*  _mpfeArray ; //! Array of parallel execution frond ends

  ClassDef(RooAbsGoodnessOfFit,1) // Abstract real-valued variable
};

#endif
