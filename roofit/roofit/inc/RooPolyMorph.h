/*****************************************************************************
 * Project: RooFit                                                           *
 *                                                                           *
  * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/

#ifndef ROOPOLYMORPH
#define ROOPOLYMORPH

#include "RooAbsPdf.h"
#include "RooRealProxy.h"
#include "RooCategoryProxy.h"
#include "RooAbsReal.h"
#include "RooAbsCategory.h"
#include "RooSetProxy.h"
#include "RooArgList.h"

#include "TMatrixD.h"
#include "TVectorD.h"

#include <vector>
#include <string>
class RooChangeTracker ;

class RooPolyMorph : public RooAbsPdf {
public:

  enum Setting { Linear, NonLinear, NonLinearPosFractions, NonLinearLinFractions } ;

  RooPolyMorph() {} ; 
  RooPolyMorph(const char *name, const char *title,
	       RooAbsReal& _m,
	       const RooArgList& varList,
	       const RooArgList& pdfList,
               const TVectorD& mrefpoints,
               const Setting& setting = NonLinearPosFractions
              );
  RooPolyMorph(const RooPolyMorph& other, const char* name=0) ;
  virtual TObject* clone(const char* newname) const { return new RooPolyMorph(*this,newname); }
  virtual ~RooPolyMorph();

  void     calculateFractions(Bool_t verbose=kTRUE) const;
  void     setMode(const Setting& setting) { _setting = setting; }

  Bool_t redirectServersHook(const RooAbsCollection& /*newServerList*/, Bool_t /*mustReplaceAll*/, Bool_t /*nameChange*/, Bool_t /*isRecursive*/) ;  

protected:

  Double_t evaluate() const ;

  void     initialize();
  void     constructMorphPdf() const ;

  inline   const Int_t ij(const Int_t& i, const Int_t& j) const { return (i*_varList.getSize()+j); }
  int      idxmin(const double& m) const;
  int      idxmax(const double& m) const;

  RooRealVar* frac(Int_t i ) { return ((RooRealVar*)_frac.at(i)) ; }
  const RooRealVar* frac(Int_t i ) const { return ((RooRealVar*)_frac.at(i)) ; }

  mutable RooAbsPdf* _sumPdf; //!
  mutable RooChangeTracker* _tracker ;  //!
  mutable RooArgList _frac ;
 
  RooRealProxy m ;
  RooListProxy _varList ;
  RooListProxy _pdfList ;
  mutable TVectorD* _mref;

  TIterator* _varItr ;   //! do not persist
  TIterator* _pdfItr ;   //!
  mutable TMatrixD* _M; //!

  Setting _setting;

  ClassDef(RooPolyMorph,1) // Your description goes here...
};
 
#endif


