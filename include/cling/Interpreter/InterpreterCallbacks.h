//------------------------------------------------------------------------------
// CLING - the C++ LLVM-based InterpreterG :)
// version: $Id$
// author:  Vassil Vassilev <vasil.georgiev.vasilev@cern.ch>
//------------------------------------------------------------------------------

#ifndef CLING_INTERPRETER_CALLBACKS_H
#define CLING_INTERPRETER_CALLBACKS_H

#include "cling/Interpreter/Value.h"

namespace cling {
  /// \brief  This interface provides a way to observe the actions of the 
  /// interpreter as it does its thing.  Clients can define their hooks here to
  /// implement interpreter level tools.
  class InterpreterCallbacks {
    /// \brief This callback is invoked whenever the interpreter needs to
    /// resolve the type and the adress of an object, which has been marked for
    /// delayed evaluation from the interpreter's dynamic lookup extension
    virtual bool LookupObject(const char* name, Value& obj) = 0;
  };
} // end namespace cling

#endif CLING_INTERPRETER_CALLBACKS_H
