/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 * @(#)root/roofitcore:$Id$
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

// -- CLASS DESCRIPTION [AUX] --

#include "RooFit.h"
#include "RooSharedPropertiesList.h"
#include "RooSharedProperties.h"
#include "TIterator.h"
#include <iostream>
using std::cout ;
using std::endl ;

ClassImp(RooSharedPropertiesList)
;


RooSharedPropertiesList::RooSharedPropertiesList() 
{
} 


RooSharedPropertiesList::~RooSharedPropertiesList() 
{
//   cout << "RooSharedPropertiesList::dtor" << endl ;
  // Delete all objects in property list
  TIterator* iter = _propList.MakeIterator() ;
  RooSharedProperties* prop ;
  while((prop=(RooSharedProperties*)iter->Next())) {
//     cout << "deleting shared prop " << prop << endl ;
    delete prop ;
  }
  delete iter ;
} 


RooSharedProperties* RooSharedPropertiesList::registerProperties(RooSharedProperties* prop) 
{
  // Register property into list and take ownership. 
  //
  // If an existing entry has a UUID that identical to that of the argument prop, 
  // the argument prop is deleted and a pointer to the already stored is returned to
  // eliminate the duplication of instances with a unique identity.
  //
  // The caller should therefore not refer anymore to the input argument pointer as
  // as the object cannot be assumed to be live.

  if (prop==0) {
    cout << "RooSharedPropertiesList::ERROR null pointer!:" << endl ;
    return 0 ;
  }

  // Find property with identical uuid in list
  TIterator* iter = _propList.MakeIterator() ;
  RooSharedProperties* tmp ;
  while((tmp=(RooSharedProperties*)iter->Next())) {
    if (*tmp==*prop && tmp != prop) {
      // Found another instance of object with identical UUID 

      // Delete incoming instance, increase ref count of already stored instance
//       cout << "RooSharedProperties::reg deleting incoming prop " << prop << " recycling existing prop " << tmp << endl ;

      // Check if prop is in _propList
      if (_propList.FindObject(prop)) {
// 	cout << "incoming object to be deleted is in proplist!!" << endl ;
      } else {
// 	cout << "deleting prop object " << prop << endl ;
	//delete prop ;
      }

      // delete prop ;
      _propList.Add(tmp) ;

      delete iter ;

      // Return pointer to already-stored instance
      return tmp ;
    }
  }
  delete iter ;


//   cout << "RooSharedProperties::reg storing incoming prop " << prop << endl ;
  _propList.Add(prop) ;
  return prop ;
}


void RooSharedPropertiesList::unregisterProperties(RooSharedProperties* prop) 
{
  // Decrease reference count
//   cout << "RooSharedPropertiesList::unreg decreasing ref cout of prop " << prop << endl ;
  _propList.Remove(prop) ;

  // We own object if ref-counted list. If count drops to zero, delete object
  if (_propList.refCount(prop)==0) {
    //cout << "RooSharedPropertiesList::unregisterProperties: deleting property " << prop << endl ;
//     cout << "RooSharedPropertiesList::unreg deleting prop with zero ref count " << prop << endl ;
    delete prop ;
  }

}


