// @(#)root/fit:$Id: inc/Fit/Chi2FCN.h,v 1.0 2006/01/01 12:00:00 moneta Exp $
// Author: L. Moneta Tue Sep  5 09:13:32 2006

/**********************************************************************
 *                                                                    *
 * Copyright (c) 2006  LCG ROOT Math Team, CERN/PH-SFT                *
 *                                                                    *
 *                                                                    *
 **********************************************************************/

// Header file for class Chi2FCN

#ifndef ROOT_Fit_Chi2FCN
#define ROOT_Fit_Chi2FCN

#ifndef ROOT_Math_FitMethodunction
#include "Math/FitMethodFunction.h"
#endif

#ifndef ROOT_Math_IParamFunctionfwd
#include "Math/IParamFunctionfwd.h"
#endif

#ifndef ROOT_Math_IParamFunction
#include "Math/IParamFunction.h"
#endif

#ifndef ROOT_Fit_DataVectorfwd
#include "Fit/DataVectorfwd.h"
#endif

#ifndef ROOT_Fit_DataVector
#include "Fit/DataVector.h"
#endif


#ifndef ROOT_Fit_FitUtil
#include "Fit/FitUtil.h"
#endif

//#define ROOT_FIT_PARALLEL

#ifdef ROOT_FIT_PARALLEL
#ifndef ROOT_Fit_FitUtilParallel
#include "Fit/FitUtilParallel.h"
#endif
#endif

/** 
@defgroup FitMethodFunc Fit Method Function Classes 
*/


namespace ROOT { 


   namespace Fit { 


/** 
   Chi2FCN class for binnned fits using the least square methods 

   @ingroup  FitMethodFunc   
*/ 
template<class FunType> 
class Chi2FCN : public ROOT::Math::BasicFitMethodFunction<FunType> {

public: 


   typedef  ROOT::Math::BasicFitMethodFunction<FunType> BaseObjFunction; 
   typedef typename  BaseObjFunction::BaseFunction BaseFunction; 

   typedef  ROOT::Math::IParamMultiFunction IModelFunction;

   /** 
      Constructor from data set (binned ) and model function 
   */ 
   Chi2FCN (const BinData & data, IModelFunction & func) : 
   fData(data), 
   fFunc(func), 
   fNDim(func.NPar() ), 
   fNPoints(data.Size()),      
   fNEffPoints(0),
   fNCalls(0), 
   fGrad ( std::vector<double> ( func.NPar() ) )
   {
   }

   /** 
      Destructor (no operations)
   */ 
   virtual ~Chi2FCN ()  {}  

#ifdef LATER
private:

   // usually copying is non trivial, so we make this unaccessible

   /** 
      Copy constructor
   */ 
   Chi2FCN(const Chi2FCN &); 

   /** 
      Assignment operator
   */ 
   Chi2FCN & operator = (const Chi2FCN & rhs); 

#endif
public: 

   virtual BaseFunction * Clone() const { 
      // clone the function
      Chi2FCN * fcn =  new Chi2FCN(fData,fFunc); 
      return fcn; 
   }
 

   virtual unsigned int NDim() const { return fNDim; }

   using BaseObjFunction::operator();

   // count number of function calls
   virtual unsigned int NCalls() const { return fNCalls; } 

   // size of the fit data points (required by interface)
   virtual unsigned int NPoints() const { return fNPoints; }

   // effective points used in the fit (exclude the rejected one)
   virtual unsigned int NFitPoints() const { return fNEffPoints; }

   virtual void ResetNCalls() { fNCalls = 0; }

   /// i-th chi-square residual  
   virtual double DataElement(const double * x, unsigned int i) const { 
      return FitUtil::EvaluateChi2Residual(fFunc, fData, x, i); 
   }

   void Gradient(const double *x, double *g) const { 
      // evaluate the chi2 gradient
      // FitUtil::EvaluateChi2Gradient(fFunc, fData, x, g, fNEffPoints);
   }

protected: 

   /// access to reference to the data 
   virtual const BinData & Data() const { return fData; }

   /// set number of fit points (need to be called in const methods, make it const) 
   virtual void SetNFitPoints(unsigned int n) const { fNEffPoints = n; }

private: 

   /**
      Evaluation of the  function (required by interface)
    */
   double DoEval (const double * x) const { 
      fNCalls++;
#ifdef ROOT_FIT_PARALLEL
      return FitUtilParallel::EvaluateChi2(fFunc, fData, x, fNEffPoints); 
#else 
      return FitUtil::EvaluateChi2(fFunc, fData, x, fNEffPoints); 
#endif
   } 

   // for derivatives 
   double  DoDerivative(const double * x, unsigned int icoord ) const { 
      Gradient(x, &fGrad[0]); 
      return fGrad[icoord]; 
   }

   const BinData & fData; 
   mutable IModelFunction & fFunc; 

   unsigned int fNDim; 
   unsigned int fNPoints;   // size of the data
   mutable unsigned int fNEffPoints;  // number of effective points used in the fit 
   mutable unsigned int fNCalls;

   mutable std::vector<double> fGrad; // for derivatives


}; 

   } // end namespace Fit

} // end namespace ROOT


#endif /* ROOT_Fit_Chi2FCN */
