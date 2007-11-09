// @(#)root/fit:$Id: inc/Fit/FitUtil.h,v 1.0 2006/01/01 12:00:00 moneta Exp $
// Author: L. Moneta Tue Nov 28 10:52:47 2006

/**********************************************************************
 *                                                                    *
 * Copyright (c) 2006  LCG ROOT Math Team, CERN/PH-SFT                *
 *                                                                    *
 *                                                                    *
 **********************************************************************/

// Header file for class FitUtil

#ifndef ROOT_Fit_FitUtil
#define ROOT_Fit_FitUtil

#ifndef ROOT_Math_IParamFunctionfwd
#include "Math/IParamFunctionfwd.h"
#endif

#ifndef ROOT_Fit_DataVectorfwd
#include "Fit/DataVectorfwd.h"
#endif


namespace ROOT { 

   namespace Fit { 


   

/** 
   FitUtil namespace defining free functions using in Fit 
*/ 
namespace FitUtil {

   typedef  ROOT::Math::IParamMultiFunction IModelFunction;
   typedef  ROOT::Math::IParamMultiGradFunction IGradModelFunction;

   /** 
       evaluate the Chi2 given a model function and the data at the point x. 
       return also nPoints as the effective number of used points in the Chi2 evaluation
   */ 
   double EvaluateChi2(IModelFunction & func, const BinData & data, const double * x, unsigned int & nPoints);  

   /** 
       evaluate the Chi2 gradient given a model function and the data at the point x. 
       return also nPoints as the effective number of used points in the Chi2 evaluation
   */ 
   void EvaluateChi2Gradient(IModelFunction & func, const BinData & data, const double * x, double * grad, unsigned int & nPoints);  

   /** 
       evaluate the LogL given a model function and the data at the point x. 
       return also nPoints as the effective number of used points in the LogL evaluation
   */ 
   double EvaluateLogL(IModelFunction & func, const UnBinData & data, const double * x, unsigned int & nPoints);  

   /** 
       evaluate the Poisson LogL given a model function and the data at the point x. 
       return also nPoints as the effective number of used points in the LogL evaluation
   */ 
   double EvaluatePoissonLogL(IModelFunction & func, const BinData & data, const double * x, unsigned int & nPoints);  

   /** 
       Parallel evaluate the Chi2 given a model function and the data at the point x. 
       return also nPoints as the effective number of used points in the Chi2 evaluation
   */ 
   double ParallelEvalChi2(IModelFunction & func, const BinData & data, const double * x, unsigned int & nPoints);  

   /** 
       evaluate the residual contribution to the Chi2 given a model function and the BinPoint data 
   */ 
   double EvaluateChi2Residual(IModelFunction & func, const BinData & data, const double * x, unsigned int ipoint);  

   /** 
       evaluate the pdf contribution to the LogL given a model function and the BinPoint data 
   */ 
   double EvaluatePdf(IModelFunction & func, const UnBinData & data, const double * x, unsigned int ipoint); 

   /** 
       evaluate the pdf contribution to the Poisson LogL given a model function and the BinPoint data 
   */ 
   double EvaluatePoissonBinPdf(IModelFunction & func, const BinData & data, const double * x, unsigned int ipoint);  
   




} // end namespace FitUtil 

   } // end namespace Fit

} // end namespace ROOT


#endif /* ROOT_Fit_FitUtil */
