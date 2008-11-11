// @(#)root/roostats:$Id: LikelihoodInterval.h 24970 2008-10-10  cranmer $
// Author: Kyle Cranmer, Lorenzo Moneta, Gregory Schott, Wouter Verkerke
/*************************************************************************
 * Copyright (C) 1995-2008, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

/*****************************************************************************
 * Project: RooStats
 * Package: RooFit/RooStats  
 * @(#)root/roofit/roostats:$Id$
 * Authors:                     
 *   Kyle Cranmer, Lorenzo Moneta, Gregory Schott, Wouter Verkerke
 *
 *****************************************************************************/



// This class implements the ConfInterval interface for a simple 1-d interval of the form [a,b]

#ifndef RooStats_LikelihoodInterval
#include "RooStats/LikelihoodInterval.h"
#endif
#include "RooAbsReal.h"
#include "RooMsgService.h"
#include "RooStats/RooStatsUtils.h"

// Without this macro the THtml doc for TMath can not be generated
#if !defined(R__ALPHA) && !defined(R__SOLARIS) && !defined(R__ACC) && !defined(R__FBSD)
NamespaceImp(RooStats)
#endif

ClassImp(RooStats::LikelihoodInterval) ;

using namespace RooStats;

//____________________________________________________________________
LikelihoodInterval::LikelihoodInterval() : fLikelihoodRatio(0)
{
  // Default constructor
}

//____________________________________________________________________
LikelihoodInterval::LikelihoodInterval(const char* name) :
  ConfInterval(name,name), fLikelihoodRatio(0)
{
  // Alternate constructor
}

//____________________________________________________________________
LikelihoodInterval::LikelihoodInterval(const char* name, const char* title) :
  ConfInterval(name,title), fLikelihoodRatio(0)
{
  // Alternate constructor
}

//____________________________________________________________________
LikelihoodInterval::LikelihoodInterval(const char* name, RooAbsReal* lr) :
  ConfInterval(name,name)
{
  // Alternate constructor
  fLikelihoodRatio = lr;
}

//____________________________________________________________________
LikelihoodInterval::LikelihoodInterval(const char* name, const char* title, RooAbsReal* lr) :
  ConfInterval(name,title)
{
  // Alternate constructor
  fLikelihoodRatio = lr;
}

//____________________________________________________________________
LikelihoodInterval::~LikelihoodInterval()
{
  // Destructor

}


//____________________________________________________________________
Bool_t LikelihoodInterval::IsInInterval(RooArgSet &parameterPoint) 
{  

  RooMsgService::instance().setGlobalKillBelow(RooMsgService::FATAL) ;
  // Method to determine if a parameter point is in the interval
  if( !this->CheckParameters(parameterPoint) )
    return false; 

  if(!fLikelihoodRatio) {
    std::cout << "likelihood ratio not set" << std::endl;
    return false;
  }

  // set parameters
  SetParameters(&parameterPoint, fLikelihoodRatio->getVariables() );


  // evaluate likelihood ratio, see if it's bigger than threshold
  if (fLikelihoodRatio->getVal()<0){
    std::cout << "The likelihood ratio is < 0, indicates a bad minimum or numerical precision problems.  Will return true" << std::endl;
    return true;
  }
  if ( TMath::Prob( 2* fLikelihoodRatio->getVal(), parameterPoint.getSize()) < (1.-fConfidenceLevel) )
    return false;


  RooMsgService::instance().setGlobalKillBelow(RooMsgService::DEBUG) ;
  return true;
  
}

/* Not complete.  Add a binary search to get lower/upper limit for a given parameter.
Need to be careful because of a few issues:
 - Need to profile other parameters of interest, and the fLikelihoodRatio may keep the other POI fixed to their current values.  Probably need to make a custom POI.
 - Need to cut on the profile LR based on a chi^2 dist with the correct number of number of POI.
//____________________________________________________________________
Bool_t LikelihoodInterval::LowerLimit(RooRealVar& param, RooArgSet &parameterPoint) 
{  
  //  double target = 1.64; // target of sqrt(-2log lambda)
  RooArgSet* theseParams = parameterPoint.clone("tempParams");

  Double_t paramRange = param.getMax()-param.getMin();
  Double_t step = paramRange/2.;
  Double_t lastDiff = -.5, diff=-.5;
  Double_t thisMu = param.getMin();
  int nIterations = 0, maxIterations = 20;
  cout << "about to do binary search" << endl;
  while(fabs(diff) > 0.01*paramRange && nIterations < maxIterations ){
    nIterations++;
    if(diff<0)
      thisMu += step;
    else
      thisMu -= step;
    if(lastDiff*diff < 0) step /=2; 

    
    mu.setVal( thisMu );
    lastDiff = diff;
    // abs below to protect small negative numbers from numerical precision
    //    diff = sqrt(2.*fabs(muprofile.getVal())) - target; 
    diff = (Double_t) IsInInterval(theseParams) - 0.5;
    cout << "diff = " << diff << endl;
  }
  

}
*/
