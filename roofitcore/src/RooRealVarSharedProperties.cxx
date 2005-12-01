/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 *    File: $Id$
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

#include "RooFitCore/RooFit.hh"
#include "RooFitCore/RooRealVarSharedProperties.hh"

ClassImp(RooRealVarSharedProperties)
;


RooRealVarSharedProperties::RooRealVarSharedProperties()
{
} 


RooRealVarSharedProperties::~RooRealVarSharedProperties() 
{
  _altBinning.Delete() ;
} 


