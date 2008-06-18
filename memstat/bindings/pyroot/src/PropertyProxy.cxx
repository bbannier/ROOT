// @(#)root/pyroot:$Id$
// Author: Wim Lavrijsen, Jan 2005

// Bindings
#include "PyROOT.h"
#include "PropertyProxy.h"
#include "ObjectProxy.h"
#include "PyBufferFactory.h"
#include "RootWrapper.h"

// ROOT
#include "TROOT.h"
#include "TClass.h"
#include "TDataMember.h"
#include "TGlobal.h"
#include "TDataType.h"
#include "TClassEdit.h"

// CINT
#include "Api.h"


namespace PyROOT {

namespace {

//= PyROOT property proxy property behaviour =================================
   PyObject* pp_get( PropertyProxy* pyprop, ObjectProxy* pyobj, PyObject* )
   {
   // normal getter access
      Long_t address = pyprop->GetAddress( pyobj );
      if ( PyErr_Occurred() )
         return 0;

   // not-initialized or public data accesses through class (e.g. by help())
      if ( address == 0 ) {
         Py_INCREF( pyprop );
         return (PyObject*)pyprop;
      }

   // for fixed size arrays
      void* ptr = (void*)address;
      if ( pyprop->fProperty & kIsArray )
         ptr = &address;

      if ( pyprop->fConverter != 0 )
         return pyprop->fConverter->FromMemory( ptr );

      PyErr_Format( PyExc_NotImplementedError,
         "no converter available for \"%s\"", pyprop->GetName().c_str() );
      return 0;
   }

//____________________________________________________________________________
   int pp_set( PropertyProxy* pyprop, ObjectProxy* pyobj, PyObject* value )
   {
      const int errret = -1;

   // filter const objects and enums to prevent changing their values
      if ( ( pyprop->fProperty & kIsConstant ) ||
           ( ! ( ~pyprop->fProperty & ( kIsEnum | G__BIT_ISSTATIC ) ) ) ) {
         PyErr_SetString( PyExc_TypeError, "assignment to const data not allowed" );
         return errret;
      }

      Long_t address = pyprop->GetAddress( pyobj );
      if ( ! address || PyErr_Occurred() )
         return errret;

   // for fixed size arrays
      void* ptr = (void*)address;
      if ( pyprop->fProperty & kIsArray )
         ptr = &address;

   // actual conversion; return on success
      if ( pyprop->fConverter && pyprop->fConverter->ToMemory( value, ptr ) )
         return 0;

   // set a python error, if not already done
      if ( ! PyErr_Occurred() )
         PyErr_SetString( PyExc_RuntimeError, "property type mismatch or assignment not allowed" );

   // failure ...
      return errret;
   }

//= PyROOT property proxy construction/destruction ===========================
   PropertyProxy* pp_new( PyTypeObject* pytype, PyObject*, PyObject* )
   {
      PropertyProxy* pyprop = (PropertyProxy*)pytype->tp_alloc( pytype, 0 );
      pyprop->fConverter = 0;
      new ( &pyprop->fName ) std::string();

      return pyprop;
   }

//____________________________________________________________________________
   void pp_dealloc( PropertyProxy* pyprop )
   {
      using namespace std;
      pyprop->fName.~string();
      delete pyprop->fConverter;
      pyprop->ob_type->tp_free( (PyObject*)pyprop );
   }


} // unnamed namespace


//= PyROOT property proxy type ===============================================
PyTypeObject PropertyProxy_Type = {
   PyObject_HEAD_INIT( &PyType_Type )
   0,                         // ob_size
   (char*)"ROOT.PropertyProxy",                  // tp_name
   sizeof(PropertyProxy),     // tp_basicsize
   0,                         // tp_itemsize
   (destructor)pp_dealloc,    // tp_dealloc
   0,                         // tp_print
   0,                         // tp_getattr
   0,                         // tp_setattr
   0,                         // tp_compare
   0,                         // tp_repr
   0,                         // tp_as_number
   0,                         // tp_as_sequence
   0,                         // tp_as_mapping
   0,                         // tp_hash
   0,                         // tp_call
   0,                         // tp_str
   0,                         // tp_getattro
   0,                         // tp_setattro
   0,                         // tp_as_buffer
   Py_TPFLAGS_DEFAULT,        // tp_flags
   (char*)"PyROOT property proxy (internal)",    // tp_doc
   0,                         // tp_traverse
   0,                         // tp_clear
   0,                         // tp_richcompare
   0,                         // tp_weaklistoffset
   0,                         // tp_iter
   0,                         // tp_iternext
   0,                         // tp_methods
   0,                         // tp_members
   0,                         // tp_getset
   0,                         // tp_base
   0,                         // tp_dict
   (descrgetfunc)pp_get,      // tp_descr_get
   (descrsetfunc)pp_set,      // tp_descr_set
   0,                         // tp_dictoffset
   0,                         // tp_init
   0,                         // tp_alloc
   (newfunc)pp_new,           // tp_new
   0,                         // tp_free
   0,                         // tp_is_gc
   0,                         // tp_bases
   0,                         // tp_mro
   0,                         // tp_cache
   0,                         // tp_subclasses
   0,                         // tp_weaklist

#if PY_MAJOR_VERSION >= 2 && PY_MINOR_VERSION >= 3
   0                          // tp_del
#endif
};

} // namespace PyROOT


//- public members -----------------------------------------------------------
void PyROOT::PropertyProxy::Set( TDataMember* dm )
{
// initialize from <dm> info
   fOffset    = dm->GetOffsetCint();
   std::string fullType = dm->GetFullTypeName();
   if ( (int)dm->GetArrayDim() != 0 || ( ! dm->IsBasic() && dm->IsaPointer() ) ) {
      fullType.append( "*" );
   }
   fProperty  = (Long_t)dm->Property();
   fConverter = CreateConverter( fullType, dm->GetMaxIndex( 0 ) );
   fName      = dm->GetName();

   if ( dm->GetClass()->GetClassInfo() ) {
      fOwnerTagnum = ((G__ClassInfo*)dm->GetClass()->GetClassInfo())->Tagnum();
      fOwnerIsNamespace = ((G__ClassInfo*)dm->GetClass()->GetClassInfo())->Property() & G__BIT_ISNAMESPACE;
   }
}

//____________________________________________________________________________
void PyROOT::PropertyProxy::Set( TGlobal* gbl )
{
// initialize from <gbl> info
   fOffset    = (Long_t)gbl->GetAddress();
   fProperty  = gbl->Property() | kIsStatic;    // force static flag
   std::string fullType = gbl->GetFullTypeName();
   if ( fullType == "void*" ) // actually treated as address to void*
      fullType = "void**";
   fConverter = CreateConverter( fullType, gbl->GetMaxIndex( 0 ) );
   fName      = gbl->GetName();

// no owner (global scope)
   fOwnerTagnum = -1;
   fOwnerIsNamespace = 0;
}

//____________________________________________________________________________
#ifdef PYROOT_USE_REFLEX
void PyROOT::PropertyProxy::Set( const ROOT::Reflex::Member& mb )
{
// initialize from Reflex <mb> info
   fOffset    = (Long_t)mb.Offset();
   fProperty  = ( mb.IsStatic()         ? kIsStatic : 0 ) |
                ( mb.TypeOf().IsEnum()  ? kIsEnum   : 0 ) |
                ( mb.TypeOf().IsArray() ? kIsArray  : 0 );
   fConverter = CreateConverter( mb.TypeOf().Name( ROOT::Reflex::SCOPED | ROOT::Reflex::FINAL ) );
   fName      = mb.Name();

// unknown owner (TODO: handle this case)
   fOwnerTagnum = -1;
   fOwnerIsNamespace = 0;
}
#endif

//____________________________________________________________________________
Long_t PyROOT::PropertyProxy::GetAddress( ObjectProxy* pyobj ) {
// class attributes, global properties
   if ( fProperty & kIsStatic )
      return fOffset;
   if ( (fOwnerTagnum > -1) && fOwnerIsNamespace ) {
      return fOffset;
   }

// special case: non-static lookup through class
   if ( ! pyobj )
      return 0;

// instance attributes; requires valid object for full address
   if ( ! ObjectProxy_Check( pyobj ) ) {
      PyErr_Format( PyExc_TypeError,
         "object instance required for access to property \"%s\"", GetName().c_str() );
      return 0;
   }

   void* obj = pyobj->GetObject();
   if ( ! obj ) {
      PyErr_SetString( PyExc_ReferenceError, "attempt to access a null-pointer" );
      return 0;
   }

   Long_t offset = 0;
   if ( 0 < fOwnerTagnum ) {
      Int_t realTagnum = ((G__ClassInfo*)pyobj->ObjectIsA()->GetClassInfo())->Tagnum();
      if ( fOwnerTagnum != realTagnum )
         offset = G__isanybase( fOwnerTagnum, realTagnum, (Long_t)obj );
   }

   return (Long_t)obj + offset + fOffset;
}
