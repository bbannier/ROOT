/*****************************************************************************
 * Project: BaBar detector at the SLAC PEP-II B-factory
 * Package: RooFitCore
 *    File: $Id: RooArgSet.rdl,v 1.20 2001/08/23 23:43:42 david Exp $
 * Authors:
 *   DK, David Kirkby, Stanford University, kirkby@hep.stanford.edu
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu
 * History:
 *   07-Mar-2001 WV Created initial version
 *
 * Copyright (C) 2001 University of California
 *****************************************************************************/
#ifndef ROO_ARG_SET
#define ROO_ARG_SET

#include "THashList.h"
#include "TString.h"
#include "TClass.h"
#include "RooFitCore/RooAbsArg.hh"
#include "RooFitCore/RooPrintable.hh"

class RooArgSet : public TObject, public RooPrintable {
public:

  // Constructors, assignment etc.
  RooArgSet();
  RooArgSet(const char *name);
  RooArgSet(const RooAbsArg& var1, 
	    const char *name="");
  RooArgSet(const RooAbsArg& var1, const RooAbsArg& var2, 
	    const char *name="");
  RooArgSet(const RooAbsArg& var1, const RooAbsArg& var2,
	    const RooAbsArg& var3, 
	    const char *name="");
  RooArgSet(const RooAbsArg& var1, const RooAbsArg& var2,
	    const RooAbsArg& var3, const RooAbsArg& var4, 
	    const char *name="");
  RooArgSet(const RooAbsArg& var1, const RooAbsArg& var2,
	    const RooAbsArg& var3, const RooAbsArg& var4, 
	    const RooAbsArg& var5, 
	    const char *name="");
  RooArgSet(const RooAbsArg& var1, const RooAbsArg& var2,
	    const RooAbsArg& var3, const RooAbsArg& var4, 
	    const RooAbsArg& var5, const RooAbsArg& var6, 
	    const char *name="");
  RooArgSet(const RooAbsArg& var1, const RooAbsArg& var2,
            const RooAbsArg& var3, const RooAbsArg& var4, 
	    const RooAbsArg& var5, const RooAbsArg& var6, 
	    const RooAbsArg& var7, 
	    const char *name="");
  RooArgSet(const RooAbsArg& var1, const RooAbsArg& var2,
            const RooAbsArg& var3, const RooAbsArg& var4, 
	    const RooAbsArg& var5, const RooAbsArg& var6, 
	    const RooAbsArg& var7, const RooAbsArg& var8, 
	    const char *name="");
  RooArgSet(const RooAbsArg& var1, const RooAbsArg& var2,
            const RooAbsArg& var3, const RooAbsArg& var4, 
	    const RooAbsArg& var5, const RooAbsArg& var6, 
	    const RooAbsArg& var7, const RooAbsArg& var8, 
	    const RooAbsArg& var9, const char *name="");

  virtual ~RooArgSet();
  // Create a copy of an existing list. New variables cannot be added
  // to a copied list. The variables in the copied list are independent
  // of the original variables.
  RooArgSet(const RooArgSet& other, const char *name="");
  virtual TObject* Clone(const char* newname=0) const { return new RooArgSet(*this,newname); }
  RooArgSet& operator=(const RooArgSet& other);

  // Copy list and contents (and optionally 'deep' servers)
  RooArgSet *snapshot(Bool_t deepCopy=kTRUE) const ;

  // List content management
  virtual Bool_t add(const RooAbsArg& var, Bool_t silent=kFALSE) ;
  virtual RooAbsArg *addClone(const RooAbsArg& var, Bool_t silent=kFALSE) ;
  virtual Bool_t replace(const RooAbsArg& var1, const RooAbsArg& var2) ;
  virtual Bool_t remove(const RooAbsArg& var, Bool_t silent=kFALSE) ;
  virtual void removeAll() ;
  Bool_t add(const RooArgSet& list) ;
  Bool_t replace(const RooArgSet &other);
  Bool_t remove(const RooArgSet& list, Bool_t silent=kFALSE) ;

  // Group operations on AbsArgs
  void setAttribAll(const Text_t* name, Bool_t value=kTRUE) ;

  // List search methods
  RooAbsArg *find(const char *name) const ;
  Bool_t contains(const RooAbsArg& var) const { return (0 == find(var.GetName())) ? kFALSE:kTRUE; }
  RooArgSet* selectByAttrib(const char* name, Bool_t value) const ;

  // export subset of THashList interface
  inline TIterator* createIterator(Bool_t dir = kIterForward) const { return _list.MakeIterator(dir); }
  inline Int_t getSize() const { return _list.GetSize(); }
  // first() returns the first element that the iterator would return, which is not necessarily
  // the first element added to this set!
  inline RooAbsArg *first() const { return (RooAbsArg*)_list.First(); }

  // I/O streaming interface (machine readable)
  virtual Bool_t readFromStream(istream& is, Bool_t compact, Bool_t verbose=kFALSE) ;
  virtual void writeToStream(ostream& os, Bool_t compact) ;
  
  // Printing interface (human readable)
  virtual void printToStream(ostream& os, PrintOption opt= Standard, TString indent= "") const;
  inline virtual void Print(Option_t *options= 0) const {
    printToStream(defaultStream(),parseOptions(options));
  }

  void setName(const char *name) { _name= name; }
  const char* GetName() const { return _name.Data() ; }

protected:

  THashList _list; // An indexed hash table of pointers to the arguments we "contain"
  Bool_t _isCopy;  // Flag to identify a list that owns its contents.
  TString _name;   // Our name.

  // Support for snapshot method 
  void addServerClonesToList(const RooAbsArg& var) ;

private:

  // !!! temporary kludges that export ArgSet list semantics !!!
  inline Int_t IndexOf(const TObject* obj) const { return _list.IndexOf(obj); }
  inline TObject* At(Int_t idx) const { return _list.At(idx); }
  inline void Sort(Bool_t order = kSortAscending) { _list.Sort(order); }
  inline const TCollection &getCollection() const { return _list; }

  friend class RooConvolutedPdf;     // needs At(), IndexOf()
  friend class RooFitContext;        // needs At(), Sort()
  friend class RooSimFitContext;     // needs At(), Sort()
  friend class RooFitResult;         // needs At(), IndexOf()
  friend class RooMultiCatIter;      // needs getCollection()
  friend class RooPdfCustomizer;     // needs Sort()
  friend class RooRealBinding;       // needs At()
  friend class RooBRArrayPdf;        // needs At()

  ClassDef(RooArgSet,1) // Set of RooAbsArg objects
};

#endif
