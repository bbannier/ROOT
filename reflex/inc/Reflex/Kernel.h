// @(#)root/reflex:$Name:  $:$Id: Kernel.h,v 1.25 2006/10/03 15:13:59 axel Exp $
// Author: Stefan Roiser 2004

// Copyright CERN, CH-1211 Geneva 23, 2004-2006, All rights reserved.
//
// Permission to use, copy, modify, and distribute this software for any
// purpose is hereby granted without fee, provided that this copyright and
// permissions notice appear in all copies and derivatives.
//
// This software is provided "as is" without express or implied warranty.

#ifndef ROOT_Reflex_Kernel
#define ROOT_Reflex_Kernel

// These macros will allow selection on exported symbols
// taken from http://www.nedprod.com/programs/gccvisibility.html
// Shared library support

#if __GNUC__ >= 4
  #define GCC_HASCLASSVISIBILITY
#endif

#ifdef WIN32
  #define RFLX_IMPORT __declspec(dllimport)
  #define RFLX_EXPORT __declspec(dllexport)
  #define RFLX_DLLLOCAL
  #define RFLX_DLLPUBLIC
#else
  #ifdef GCC_HASCLASSVISIBILITY
    #define RFLX_EXPORT __attribute__((visibility("default")))
    #define RFLX_DLLLOCAL __attribute__((visibility("hidden")))
    #define RFLX_DLLPUBLIC __attribute__((visibility("default")))
  #else
    #define RFLX_EXPORT
    #define RFLX_DLLLOCAL
    #define RFLX_DLLPUBLIC
  #endif
  #define RFLX_IMPORT
#endif

// Define RFLX_API for DLL builds
#ifdef REFLEX_DLL
  #ifdef REFLEX_BUILD
    #define RFLX_API RFLX_EXPORT
  #else
    #define RFLX_API  RFLX_IMPORT
  #endif // REFLEX_BUILD
#else
  #define RFLX_API
#endif // REFLEX_DLL

// Throwable classes must always be visible on GCC in all binaries
#ifdef WIN32
  #define RFLX_EXCEPTIONAPI(api) api
#elif defined(GCC_HASCLASSVISIBILITY)
  #define RFLX_EXCEPTIONAPI(api) RFLX_EXPORT
#else
  #define RFLX_EXCEPTIONAPI(api)
#endif
// end macros for symbol selection


// include config.h generated by autoconf
#if defined (HAVE_CONFIG)
#include "config.h"
#endif

#if defined (_AIX)
#define ANSICPP
#define NEED_STRCASECMP
#endif

#if defined(__alpha) && !defined(__linux)
#ifndef __USE_STD_IOSTREAM
#define __USE_STD_IOSTREAM
#endif
#endif

// Some pragmas to avoid warnings in VisualC
#ifdef _WIN32
// windows.h defines CONST
#ifdef CONST
#undef CONST
#endif
// Disable warning C4786: identifier was truncated to '255' characters in the debug information
#pragma warning ( disable : 4786 )
// Disable warning C4291: no matching operator delete found; memory will not be freed if 
// initialization throws an exception
#pragma warning ( disable : 4291 )
// Disable warning C4250: inheritance via dominance
#pragma warning ( disable : 4250 )
#endif

// some compilers define the macros below in limits
#include <limits>

// Large integer definition depends of the platform
#ifdef _WIN32
typedef __int64 longlong;
typedef unsigned __int64 ulonglong;
#elif defined(__linux) || defined(sun) || defined(__APPLE__) || (defined(__CYGWIN__)&&defined(__GNUC__)) || defined(_AIX) || (defined(__alpha)&&!defined(__linux)) || defined(__sgi) || defined(__FreeBSD__)
typedef long long int longlong; /* */
typedef unsigned long long int /**/ ulonglong;
#endif
#ifndef LONGLONG_MAX
#define LONGLONG_MAX 0x7FFFFFFFFFFFFFFFLL
#endif
#ifndef LONGLONG_MIN
#define LONGLONG_MIN 0x8000000000000000LL
#endif
#ifndef ULONGLONG_MAX
#define ULONGLONG_MAX 0xFFFFFFFFFFFFFFFFLL
#endif
#ifndef ULONGLONG_MIN
#define ULONGLONG_MIN 0x0000000000000000LL
#endif


// Include files
#include <string>
#include <vector>
#include <utility>
#include <exception>

namespace ROOT {
   namespace Reflex {

      // forward declarations
      class Any;
      class Type;
      class Base;
      class Scope;
      class Object;
      class Member;
      class PropertyList;
      class TypeTemplate;
      class MemberTemplate;

      typedef std::vector<std::string> StdString_Cont_Type_t;
      typedef StdString_Cont_Type_t::const_iterator StdString_Iterator;
      typedef StdString_Cont_Type_t::const_reverse_iterator Reverse_StdString_Iterator;

      typedef std::vector< Type > Type_Cont_Type_t;
      typedef Type_Cont_Type_t::const_iterator Type_Iterator;
      typedef Type_Cont_Type_t::const_reverse_iterator Reverse_Type_Iterator;

      typedef std::vector< Base > Base_Cont_Type_t;
      typedef Base_Cont_Type_t::const_iterator Base_Iterator;
      typedef Base_Cont_Type_t::const_reverse_iterator Reverse_Base_Iterator;

      typedef std::vector< Scope > Scope_Cont_Type_t;
      typedef Scope_Cont_Type_t::const_iterator Scope_Iterator;
      typedef Scope_Cont_Type_t::const_reverse_iterator Reverse_Scope_Iterator;

      typedef std::vector< Object > Object_Cont_Type_t;
      typedef Object_Cont_Type_t::const_iterator Object_Iterator;
      typedef Object_Cont_Type_t::const_reverse_iterator Reverse_Object_Iterator;

      typedef std::vector< Member > Member_Cont_Type_t;
      typedef Member_Cont_Type_t::const_iterator Member_Iterator;
      typedef Member_Cont_Type_t::const_reverse_iterator Reverse_Member_Iterator;

      typedef std::vector< TypeTemplate > TypeTemplate_Cont_Type_t;
      typedef TypeTemplate_Cont_Type_t::const_iterator TypeTemplate_Iterator;
      typedef TypeTemplate_Cont_Type_t::const_reverse_iterator Reverse_TypeTemplate_Iterator;

      typedef std::vector< MemberTemplate > MemberTemplate_Cont_Type_t;
      typedef MemberTemplate_Cont_Type_t::const_iterator MemberTemplate_Iterator;
      typedef MemberTemplate_Cont_Type_t::const_reverse_iterator Reverse_MemberTemplate_Iterator;


      namespace Dummy {
         RFLX_API const StdString_Cont_Type_t & StdStringCont();
         RFLX_API const Type_Cont_Type_t & TypeCont();        
         RFLX_API const Base_Cont_Type_t & BaseCont();
         RFLX_API const Scope_Cont_Type_t & ScopeCont();
         RFLX_API const Object_Cont_Type_t & ObjectCont();
         RFLX_API const Member_Cont_Type_t & MemberCont();
         RFLX_API const TypeTemplate_Cont_Type_t & TypeTemplateCont();
         RFLX_API const MemberTemplate_Cont_Type_t & MemberTemplateCont();
         RFLX_API Any & Any();
         RFLX_API const Object & Object();
         RFLX_API const Type & Type();
         RFLX_API const TypeTemplate & TypeTemplate();
         RFLX_API const Base & Base();
         RFLX_API const PropertyList & PropertyList();
         RFLX_API const Member & Member();
         RFLX_API const MemberTemplate & MemberTemplate();
         RFLX_API const Scope & Scope();
         template< class T > RFLX_API const T & Get() {
            static T t;
            return t;
         }
      }


      /** some general information about the Reflex package */
      class RFLX_API Reflex {
         public:
      
         /** default constructor */
         Reflex();

         /** destructor */
         ~Reflex();

         static void Shutdown();
      
         /** the Name of the package - used for messages */
         static const std::string & Argv0(); // returns "Seal Reflex";

      }; // struct Reflex

    
      // these defines are used for the modifiers they are used in the following 
      // classes
      // BA = BASE
      // CL = CLASS
      // FU = FUNCTION
      // DM = DATAMEMBER
      // FM = FUNCTIONMEMBER 
      // TY = TYPE
      // ME = MEMBER
      //                              BA  CL  DM  FM  TY  ME 
      enum ENTITY_DESCRIPTION {
         PUBLIC          = (1<<0),  //  X       X   X       X
         PROTECTED       = (1<<1),  //  X       X   X       X
         PRIVATE         = (1<<2),  //  X       X   X       X
         REGISTER        = (1<<3),  //          X   X       X
         STATIC          = (1<<4),  //          X   X       X
         CONSTRUCTOR     = (1<<5),  //              X       X
         DESTRUCTOR      = (1<<6) , //              X       X
         EXPLICIT        = (1<<7),  //              X       X
         EXTERN          = (1<<8) , //          X   X       X
         COPYCONSTRUCTOR = (1<<9) , //              X       X
         OPERATOR        = (1<<10), //              X       X
         INLINE          = (1<<11), //              X       X
         CONVERTER       = (1<<12), //              X       X
         AUTO            = (1<<13), //          X           X
         MUTABLE         = (1<<14), //          X           X
         CONST           = (1<<15), //          X       X   X
         VOLATILE        = (1<<16), //          X       X   X
         REFERENCE       = (1<<17), //          X           X
         ABSTRACT        = (1<<18), //      X           X
         VIRTUAL         = (1<<19), //  X   X           X
         TRANSIENT       = (1<<20), //          X           X
         ARTIFICIAL      = (1<<21), //  X   X   X   X   X   X
         // the bits 31 - 28 are reserved for template default arguments 
         TEMPLATEDEFAULTS1  = (0<<31)&(0<<30)&(0<<29)&(1<<28),
         TEMPLATEDEFAULTS2  = (0<<31)&(0<<30)&(1<<29)&(0<<28),
         TEMPLATEDEFAULTS3  = (0<<31)&(0<<30)&(1<<29)&(1<<28),
         TEMPLATEDEFAULTS4  = (0<<31)&(1<<30)&(0<<29)&(0<<28),
         TEMPLATEDEFAULTS5  = (0<<31)&(1<<30)&(0<<29)&(1<<28),
         TEMPLATEDEFAULTS6  = (0<<31)&(1<<30)&(1<<29)&(0<<28),
         TEMPLATEDEFAULTS7  = (0<<31)&(1<<30)&(1<<29)&(1<<28),
         TEMPLATEDEFAULTS8  = (1<<31)&(0<<30)&(0<<29)&(0<<28),
         TEMPLATEDEFAULTS9  = (1<<31)&(0<<30)&(0<<29)&(1<<28),
         TEMPLATEDEFAULTS10 = (1<<31)&(0<<30)&(1<<29)&(0<<28),
         TEMPLATEDEFAULTS11 = (1<<31)&(0<<30)&(1<<29)&(1<<28),
         TEMPLATEDEFAULTS12 = (1<<31)&(1<<30)&(0<<29)&(0<<28),
         TEMPLATEDEFAULTS13 = (1<<31)&(1<<30)&(0<<29)&(1<<28),
         TEMPLATEDEFAULTS14 = (1<<31)&(1<<30)&(1<<29)&(0<<28),
         TEMPLATEDEFAULTS15 = (1<<31)&(1<<30)&(1<<29)&(1<<28)
      };


      /** enum for printing names */
      enum ENTITY_HANDLING {
         FINAL     = (1<<0),
         QUALIFIED = (1<<1),
         SCOPED    = (1<<2),
         F         = (1<<4),
         Q         = (1<<5),
         S         = (1<<6)
      };
      

      /** enum containing all possible types and scopes */
      enum TYPE { 
         CLASS = 0,
         STRUCT,
         ENUM, 
         FUNCTION, 
         ARRAY, 
         FUNDAMENTAL, 
         POINTER, 
         POINTERTOMEMBER, 
         TYPEDEF, 
         UNION, 
         TYPETEMPLATEINSTANCE, 
         MEMBERTEMPLATEINSTANCE, 
         NAMESPACE, 
         DATAMEMBER, 
         FUNCTIONMEMBER,
         UNRESOLVED
      };


      /** the max unsigned int */
      size_t NPos();


      /**
       * typedef for function member type (necessary for return value of
       * getter function)
       */
      typedef void * (* StubFunction) ( void *, const std::vector < void * > &, void *);

      /** typedef for function for Offset calculation */
      typedef size_t( * OffsetFunction ) ( void * );

      /** dummy types for type_info purposes */
      class RFLX_API NullType {};
      class RFLX_API UnknownType {};
      /** place holders for protected types */
      class RFLX_API ProtectedClass {};
      class RFLX_API ProtectedEnum {};
      class RFLX_API ProtectedStruct {};
      class RFLX_API ProtectedUnion {};
      /** place holders for private types */
      class RFLX_API PrivateClass {};
      class RFLX_API PrivateEnum {};
      class RFLX_API PrivateStruct {};
      class RFLX_API PrivateUnion {};
      /** place holders for unnamed types (also typeinfo purposes) */
      class RFLX_API UnnamedClass {};
      class RFLX_API UnnamedEnum {};
      class RFLX_API UnnamedNamespace {};
      class RFLX_API UnnamedStruct {};
      class RFLX_API UnnamedUnion {};
      
      /** exception classes */
      class RFLX_EXCEPTIONAPI(RFLX_API) RuntimeError : public std::exception {
      public:
         RuntimeError(const std::string& msg) : fMsg(Reflex::Argv0() + ": " + msg) { }
         ~RuntimeError() throw() {}
         virtual const char * what() const throw( ) { return fMsg.c_str();}
         std::string fMsg;
      };

   } // namespace Reflex
} // namespace ROOT


//-------------------------------------------------------------------------------
inline size_t ROOT::Reflex::NPos() {
//-------------------------------------------------------------------------------
   return (size_t) -1; 
}

#endif // ROOT_Reflex_Kernel

