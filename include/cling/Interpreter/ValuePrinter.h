//--------------------------------------------------------------------*- C++ -*-
// CLING - the C++ LLVM-based InterpreterG :)
// version: $Id$
// author:  Axel Naumann <axel@cern.ch>
//------------------------------------------------------------------------------

#ifndef CLING_VALUEPRINTER_H
#define CLING_VALUEPRINTER_H

#include "cling/Interpreter/ValuePrinterInfo.h"

namespace llvm {
  class raw_ostream;
}

namespace cling {
  
  // Can be re-implemented to print type-specific details, e.g. as
  //   template <typename ACTUAL>
  //   void dumpPtr(llvm::raw_ostream& o, const clang::Decl* a, ACTUAL* ac,
  //                int flags, const char* tname);
  template <typename TY>
  void printValue(llvm::raw_ostream& o, const void* const p,
                  TY* const u, const ValuePrinterInfo& VPI);

  void printValueDefault(llvm::raw_ostream& o, const void* const p, 
                         const ValuePrinterInfo& PVI);

  void flushOStream(llvm::raw_ostream& o);

  namespace valuePrinterInternal {

    template <typename T>
    void PrintValue(void* o, clang::Expr* E, clang::ASTContext* C, T value) {
      ValuePrinterInfo VPI(E, C);
      llvm::raw_ostream &ostream = *(llvm::raw_ostream*)o;
      printValue(ostream, value, value, VPI);
      // Only because we don't to include llvm::raw_ostream in the header
      flushOStream(ostream);
    }
    
  } // namespace valuePrinterInternal
  
  
  // Can be re-implemented to print type-specific details, e.g. as
  //   template <typename ACTUAL>
  //   void dumpPtr(llvm::raw_ostream& o, const clang::Decl* a,
  //                ACTUAL* ap, int flags, const char* tname);
  template <typename TY>
  void printValue(llvm::raw_ostream& o, const void* const p,
                  TY* const u, const ValuePrinterInfo& PVI) {
    printValueDefault(o, p, PVI);
  }

}

#endif // CLING_VALUEPRINTER_H
