
/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Classes: PDEFoamDensity                                                        *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      This class provides an interface between the Binary search tree           *
 *      and the PDEFoam object.  In order to build-up the foam one needs to       *
 *      calculate the density of events at a given point (sampling during         *
 *      Foam build-up).  The function PDEFoamDensity::Density() does this job. It *
 *      uses a binary search tree, filled with training events, in order to       *
 *      provide this density.                                                     *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Tancredi Carli   - CERN, Switzerland                                      *
 *      Dominik Dannheim - CERN, Switzerland                                      *
 *      S. Jadach        - Institute of Nuclear Physics, Cracow, Poland           *
 *      Alexander Voigt  - CERN, Switzerland                                      *
 *      Peter Speckmayer - CERN, Switzerland                                      *
 *                                                                                *
 * Copyright (c) 2008, 2010:                                                            *
 *      CERN, Switzerland                                                         *
 *      MPI-K Heidelberg, Germany                                                 *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

//_____________________________________________________________________
//
// PDEFoamDensity
//
// This is an abstract class, which provides an interface for a
// PDEFoam density estimator.  Derived classes have to implement the
// Density(...) function, which returns the density of a certain
// quantity at a given phase-space point during the foam build-up.
//
// Variants of PDEFoamDensity are:
//
//   - PDEFoamEventDensity
//   - PDEFoamDiscriminantDensity
//   - PDEFoamTargetDensity
//   - PDEFoamDTDensity
//
// Usage:
//
// The user has to instantiate a sub class of PDEFoamDensity and set
// the pointer to the owner, which is a PDEFoam object:
//
//   PDEFoamDensity *dens = new MyDensity();
//   dens->SetPDEFoam(pdefoam);
//   pdefoam->SetDensity(dens);
//
// Afterwards the density must be initialized via (the binary search
// tree is created and initialized)
//
//   dens->Initialize();
//
// and binary search tree should be filled with TMVA events:
//
//   dens->FillBinarySearchTree(event);
// _____________________________________________________________________

#ifndef ROOT_TMVA_PDEFoamDensity
#include "TMVA/PDEFoamDensity.h"
#endif

ClassImp(TMVA::PDEFoamDensity)

//_____________________________________________________________________
TMVA::PDEFoamDensity::PDEFoamDensity() 
   : TObject(),
     fPDEFoam(NULL),
     fBst(NULL),
     fLogger( new MsgLogger("PDEFoamDensity"))
{}

//_____________________________________________________________________
TMVA::PDEFoamDensity::PDEFoamDensity(const PDEFoam *foam)
   : TObject(),
     fPDEFoam(foam),
     fBst(NULL),
     fLogger( new MsgLogger("PDEFoamDensity"))
{}

//_____________________________________________________________________
TMVA::PDEFoamDensity::~PDEFoamDensity() 
{
   if (fBst)    delete fBst;
   if (fLogger) delete fLogger;
}

//_____________________________________________________________________
TMVA::PDEFoamDensity::PDEFoamDensity(const PDEFoamDensity &distr)
   : TObject(),
     fPDEFoam         (distr.fPDEFoam),
     fBst             (distr.fBst),
     fLogger( new MsgLogger("PDEFoamDensity"))
{
   // Copy constructor
   Log() << kFATAL << "COPY CONSTRUCTOR NOT IMPLEMENTED" << Endl;
}

//_____________________________________________________________________
void TMVA::PDEFoamDensity::Initialize(const PDEFoam *foam)
{
   // Initialisation of binary search tree.  
   // Set dimension and create new BinarySearchTree.

   if (foam != NULL)
      SetPDEFoam(foam);
   if (!GetPDEFoam())
      Log() << kFATAL << "<PDEFoamDensity::Initialize()>: "
	    << "Pointer to owner not set!" << Endl;

   // create binary search tree
   if (fBst) delete fBst;
   fBst = new TMVA::BinarySearchTree();

   if (!fBst){
      Log() << kFATAL << "<PDEFoamDensity::Initialize> "
            << "ERROR: an not create binary tree !" << Endl;
   }

   // set periode (number of variables)
   fBst->SetPeriode(GetPDEFoam()->GetTotDim());
}

//_____________________________________________________________________
void TMVA::PDEFoamDensity::FillBinarySearchTree( const Event* ev )
{
   // This method inserts the given event 'ev' it into the binary
   // search tree.

   if (fBst == NULL)
      Log() << kFATAL << "<PDEFoamDensity::FillBinarySearchTree> "
            << "Binary tree is not set!" << Endl;

   // insert into binary search tree
   fBst->Insert(ev);
}
