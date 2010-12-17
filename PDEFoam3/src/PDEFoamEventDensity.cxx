
/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Classes: PDEFoamEventDensity                                                   *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      The TFDSITR class provides an interface between the Binary search tree    *
 *      and the PDEFoam object.  In order to build-up the foam one needs to       *
 *      calculate the density of events at a given point (sampling during         *
 *      Foam build-up).  The function PDEFoamEventDensity::Density() does         *
 *      this job.  It                                                             *
 *      uses a binary search tree, filled with training events, in order to       *
 *      provide this density.                                                     *
 *                                                                                *
 * Authors (alphabetical):                                                        *
 *      Tancredi Carli   - CERN, Switzerland                                      *
 *      Dominik Dannheim - CERN, Switzerland                                      *
 *      S. Jadach        - Institute of Nuclear Physics, Cracow, Poland           *
 *      Alexander Voigt  - TU Dresden, Germany                                    *
 *      Peter Speckmayer - CERN, Switzerland                                      *
 *                                                                                *
 * Copyright (c) 2008, 2010:                                                      *
 *      CERN, Switzerland                                                         *
 *      MPI-K Heidelberg, Germany                                                 *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

//_____________________________________________________________________
//
// PDEFoamEventDensity
//
// This is a concrete implementation of PDEFoam.  Density(...)
// estimates the event (weight) density at a given phase-space point
// using range-searching.
// _____________________________________________________________________

#include <cmath>

#ifndef ROOT_TMVA_PDEFoamEventDensity
#include "TMVA/PDEFoamEventDensity.h"
#endif

ClassImp(TMVA::PDEFoamEventDensity)

//_____________________________________________________________________
TMVA::PDEFoamEventDensity::PDEFoamEventDensity()
   : PDEFoamDensityBase()
{}

//_____________________________________________________________________
TMVA::PDEFoamEventDensity::PDEFoamEventDensity(std::vector<Double_t> box)
   : PDEFoamDensityBase(box)
{}

//_____________________________________________________________________
TMVA::PDEFoamEventDensity::PDEFoamEventDensity(const PDEFoamEventDensity &distr)
   : PDEFoamDensityBase(distr)
{
   // Copy constructor
   Log() << kFATAL << "COPY CONSTRUCTOR NOT IMPLEMENTED" << Endl;
}

//_____________________________________________________________________
Double_t TMVA::PDEFoamEventDensity::Density(std::vector<Double_t> &Xarg, Double_t &event_density)
{
   // This function is needed during the foam buildup.  It return the
   // event density within volume (specified by fBox).
   //
   // Parameters:
   //
   // - Xarg - event vector (in [fXmin,fXmax])
   //
   // - event_density - here the event density is stored

   if (!fBst)
      Log() << kFATAL << "<PDEFoamEventDensity::Density()> Binary tree not found!" << Endl;

   //create volume around point to be found
   std::vector<Double_t> lb(fBox.size());
   std::vector<Double_t> ub(fBox.size());

   // probevolume relative to hypercube with edge length 1:
   const Double_t probevolume_inv = 1.0 / GetBoxVolume();

   // set upper and lower bound for search volume
   for (UInt_t idim = 0; idim < fBox.size(); ++idim) {
      lb[idim] = Xarg[idim] - fBox.at(idim) / 2.0;
      ub[idim] = Xarg[idim] + fBox.at(idim) / 2.0;
   }

   TMVA::Volume volume(&lb, &ub);                        // volume to search in
   std::vector<const TMVA::BinarySearchTreeNode*> nodes; // BST nodes found

   // do range searching
   fBst->SearchVolume(&volume, &nodes);

   // store density based on total number of events
   event_density = nodes.size() * probevolume_inv;

   Double_t weighted_count = 0.; // number of events found (sum of weights)
   for (std::vector<const TMVA::BinarySearchTreeNode*>::const_iterator it = nodes.begin();
        it != nodes.end(); ++it) {
      weighted_count += (*it)->GetWeight();
   }

   // return:  N_total(weighted) / cell_volume
   return (weighted_count + 0.1) * probevolume_inv;
}
