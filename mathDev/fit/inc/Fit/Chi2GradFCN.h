// @(#)root/fit:$Id: inc/Fit/Chi2GradFCN.h,v 1.0 2006/01/01 12:00:00 moneta Exp $
// Author: L. Moneta Tue Nov 28 10:53:06 2006

/**********************************************************************
 *                                                                    *
 * Copyright (c) 2006  LCG ROOT Math Team, CERN/PH-SFT                *
 *                                                                    *
 *                                                                    *
 **********************************************************************/

// Header file for class Chi2GradFCN

#ifndef ROOT_Fit_Chi2GradFCN
#define ROOT_Fit_Chi2GradFCN

#ifndef ROOT_Math_IFunction
#include "Math/IFunction.h"
#endif

#ifndef ROOT_Fit_Chi2FCN
#include "Fit/Chi2FCN.h"
#endif

#ifndef ROOT_Fit_DataVectorfwd
#include "Fit/DataVectorfwd.h"
#endif


#ifndef ROOT_Fit_FitUtil
#include "Fit/FitUtil.h"
#endif

#ifndef ROOT_Math_IParamFunction
#include "Math/IParamFunction.h"
#endif

#include <vector>

namespace ROOT { 

   namespace Fit { 


/** 
   Chi2GradFCN class for chi2 function with analytical derivatives

   @ingroup FitMethodFunction
*/ 
class Chi2GradFCN :  public ROOT::Math::IMultiGradFunction, 
                     public ROOT::Fit::Chi2FCN {
         
public: 
         

     
   typedef  ROOT::Math::IMultiGradFunction BaseGradFunction; 

   typedef  ROOT::Fit::Chi2FCN::BaseFunction     BaseFunction; 
   typedef  ROOT::Fit::Chi2FCN::BaseObjFunction  BaseObjFunction; 

   typedef  ROOT::Math::IParamMultiFunction IModelFunction;
   typedef  ROOT::Math::IParamMultiGradFunction IGradModelFunction;


   /** 
      Default constructor
   */ 
   Chi2GradFCN (const BinData & data, IGradModelFunction & func); 

   /** 
      Destructor (no operations)
   */ 
   virtual ~Chi2GradFCN () {} 

private:
   // usually copying is non trivial, so we make this unaccessible

   /** 
      Copy constructor
   */ 
   Chi2GradFCN(const Chi2GradFCN &); 

   /** 
      Assignment operator
   */ 
   Chi2GradFCN & operator = (const Chi2GradFCN & rhs); 

public: 

   // no support for covariant return type on Windows
   BaseFunction * Clone() const; 

   //using BaseObjFunction::operator();
   using Chi2FCN::NDim; 

   void Gradient(const double * p, double * g ) const; 

private: 


   double DoEval (const double * x) const { 
      unsigned int n = 0; 
      double chi2 =  FitUtil::EvaluateChi2( fFunc, Data(), x, n); 
      SetNFitPoints(n); 
      return chi2; 
   } 

   double  DoDerivative(const double * x, unsigned int icoord ) const { 
      Gradient(x, &fGrad[0]); 
      return fGrad[icoord]; 
   }

   // data members 

   mutable IGradModelFunction & fFunc; 

   mutable std::vector<double> fGrad; 


}; 

   } // end namespace Fit

} // end namespace ROOT


#endif /* ROOT_Fit_Chi2GradFCN */
