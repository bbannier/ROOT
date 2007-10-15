// Author: Wim Lavrijsen, Dec 2006

#ifndef PYROOT_TCUSTOMPYTYPES_H
#define PYROOT_TCUSTOMPYTYPES_H

// ROOT
#include "DllImport.h"


namespace PyROOT {

/** Custom builtins, detectable by type, for pass by ref
      @author  WLAV
      @date    12/13/2006
      @version 1.0
 */

//- custom float object type and type verification ---------------------------
   R__EXTERN PyTypeObject TCustomFloat_Type;

   template< typename T >
   inline Bool_t TCustomFloat_Check( T* object )
   {
      return object && PyObject_TypeCheck( object, &TCustomFloat_Type );
   }

   template< typename T >
   inline Bool_t TCustomFloat_CheckExact( T* object )
   {
      return object && object->ob_type == &TCustomFloat_Type;
   }

//- custom long object type and type verification ----------------------------
   R__EXTERN PyTypeObject TCustomInt_Type;

   template< typename T >
   inline Bool_t TCustomInt_Check( T* object )
   {
      return object && PyObject_TypeCheck( object, &TCustomInt_Type );
   }

   template< typename T >
   inline Bool_t TCustomInt_CheckExact( T* object )
   {
      return object && object->ob_type == &TCustomInt_Type;
   }

} // namespace PyROOT

#endif // !PYROOT_TCUSTOMPYTYPES_H
