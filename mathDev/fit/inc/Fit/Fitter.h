// @(#)root/fit:$Id: inc/Fit/Fitter.h,v 1.0 2006/01/01 12:00:00 moneta Exp $
// Author: L. Moneta Wed Aug 30 11:05:19 2006

/**********************************************************************
 *                                                                    *
 * Copyright (c) 2006  LCG ROOT Math Team, CERN/PH-SFT                *
 *                                                                    *
 *                                                                    *
 **********************************************************************/

// Header file for class Fitter

#ifndef ROOT_Fit_Fitter
#define ROOT_Fit_Fitter

#ifndef ROOT_Fit_DataVectorfwd
#include "Fit/DataVectorfwd.h"
#endif

#ifndef ROOT_Fit_FitConfig
#include "Fit/FitConfig.h"
#endif

#ifndef ROOT_Fit_FitResult
#include "Fit/FitResult.h"
#endif

#ifndef ROOT_Math_IParamFunctionfwd
#include "Math/IParamFunctionfwd.h"
#endif



namespace ROOT { 


   namespace Math { 
      class Minimizer;
   } 

   namespace Fit { 

/** 
   Fitter class. 
   Fits are performed using the generic ROOT::Fit::Fitter::Fit method. 
   The inputs are the data points and a model function (using a ROOT::Math::IParamFunction)
   The result of the fit is returned and kept internally in the  ROOT::Fit::FitResult class. 
   The configuration of the fit (parameters, options, etc...) are specified in the 
   ROOT::Math::FitConfig class. 


*/ 
class Fitter {

public: 

   typedef ROOT::Math::IParamMultiFunction       IModelFunction; 
   typedef  ROOT::Math::IParamMultiGradFunction  IGradModelFunction;
   typedef ROOT::Math::IParamFunction            IModel1DFunction; 

   /** 
      Default constructor
   */ 
   Fitter (); 

   /** 
      Destructor
   */ 
   ~Fitter (); 


   

private:
   // usually copying is non trivial, so we make this unaccessible

   /** 
      Copy constructor
   */ 
   Fitter(const Fitter &);

   /** 
      Assignment operator
   */ 
   Fitter & operator = (const Fitter & rhs);  


public: 

   /** 
       fit a binned data set using any  generic model  function
       Pre-requisite on the function: 
   */ 
   template < class Data , class Function> 
   bool Fit( const Data & data, const Function & func) { 
      SetFunction(func);
      return Fit(data);
   }

   /** 
       fit a binned data set (default method: use chi2)
       To be implemented option to do likelihood bin fit
   */ 
   bool Fit(const BinData & data) { 
      return DoLeastSquareFit(data); 
   } 
   /** 
       fit an binned data set using loglikelihood method
   */ 
   bool Fit(const UnBinData & data) { 
      return DoLikelihoodFit(data); 
   } 

   /**
      binned likelihood fit 
    */
   bool LikelihoodBinFit(const BinData & data) { 
      return DoLikelihoodFit(data);
   }

   /** 
       Set the fitted function (model function) from a parametric function interface
   */ 
   void  SetFunction(const IModelFunction & func); 
   /**
      Set the fitted function from 1D parametric function interface
    */
   void  SetFunction(const IModel1DFunction & func); 

//    /** 
//        Set the fitted function from a generic callable object
//    */ 
//    void  SetFunction(const BinData * data); 

   /// get fit result
   const FitResult & Result() const { return fResult; } 

   /// access to the configuration (const method)
   const FitConfig & Config() const { return fConfig; } 

   /// access to the configuration (non const method)
   FitConfig & Config() { return fConfig; } 



protected: 

   /// least square fit 
   bool DoLeastSquareFit(const BinData & data); 
   /// binned likelihood fit
   bool DoLikelihoodFit(const BinData & data); 
   /// un-binned likelihood fit
   bool DoLikelihoodFit(const UnBinData & data); 
   /// linear least square fit 
   bool DoLinearFit(const BinData & data);

   /// do minimization
   template<class ObjFunc> 
   bool DoMinimization(ROOT::Math::Minimizer & min, const ObjFunc & f, unsigned int dataSize ); 

private: 

   /// copy of the fitted  function which will contain on output the fit result
   IModelFunction * fFunc; 

   /// object containing the result of the fit
   FitResult fResult; 


   /// fitter configuration (options and parameter settings
   FitConfig fConfig; 

   

}; 

   } // end namespace Fit

} // end namespace ROOT


#endif /* ROOT_Fit_Fitter */
