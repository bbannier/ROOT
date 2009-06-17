// @(#)root/mathmore:$Id$
// Author: L. Moneta June  2009

/**********************************************************************
 *                                                                    *
 * Copyright (c) 2006  LCG ROOT Math Team, CERN/PH-SFT                *
 *                                                                    *
 * This library is free software; you can redistribute it and/or      *
 * modify it under the terms of the GNU General Public License        *
 * as published by the Free Software Foundation; either version 2     *
 * of the License, or (at your option) any later version.             *
 *                                                                    *
 * This library is distributed in the hope that it will be useful,    *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU   *
 * General Public License for more details.                           *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * along with this library (see file COPYING); if not, write          *
 * to the Free Software Foundation, Inc., 59 Temple Place, Suite      *
 * 330, Boston, MA 02111-1307 USA, or contact the author.             *
 *                                                                    *
 **********************************************************************/

// Implementation file for class MinimTransformFunction

#include "Math/MinimTransformFunction.h"

//#include <iostream>
#include <cmath>
#include <cassert>

namespace ROOT { 

   namespace Math { 

MinimTransformFunction::MinimTransformFunction ( const IMultiGradFunction * f, const std::vector<EMinimVariableType> & types, 
                                                 const std::vector<double> & values, 
                                                 const std::map<unsigned int, std::pair<double, double> > & bounds) : 
   fX( values ),
   fFunc(f)
{
   // constructor of the class from a pointer to the function (which is managed) 
   // vector specifying the variable types (free, bounded or fixed, defined in enum EMinimVariableTypes )
   // variable values (used for the fixed ones) and a map with the bounds (for the bounded variables)

   unsigned int ntot = NTot();   // NTot is fFunc->NDim()
   assert ( types.size() == ntot ); 
   fVariables.reserve(ntot); 
   fIndex.reserve(ntot); 
   for (unsigned int i = 0; i < ntot; ++i ) { 
      if (types[i] ==  kFix ) 
         fVariables.push_back( MinimizerVariable( values[i]) );           
      else { 
         fIndex.push_back(i); 

         if ( types[i] ==  kDefault)
            fVariables.push_back( MinimizerVariable() );
         else { 
            std::map<unsigned int, std::pair<double,double> >::const_iterator itr = bounds.find(i); 
            assert ( itr != bounds.end() );
            double low = itr->second.first; 
            double up = itr->second.second;
            if (types[i] ==  kBounds )   
               fVariables.push_back( MinimizerVariable( low, up, new SinVariableTransformation()));
            else if (types[i] ==  kLowBound)  
               fVariables.push_back( MinimizerVariable( low, new SqrtLowVariableTransformation()));
            else if (types[i] ==  kUpBound)
               fVariables.push_back( MinimizerVariable( up, new SqrtUpVariableTransformation()));
         }
      }
   }
}

const double * MinimTransformFunction::Transformation( const double * x) const { 
   // transform from internal to external 
   // result is cached inside the class
   unsigned int nfree = fIndex.size(); 

//       std::cout << "Transform:  internal "; 
//       for (int i = 0; i < nfree; ++i) std::cout << x[i] << "  "; 
//       std::cout << "\t\t";

   for (unsigned int i = 0; i < nfree; ++i ) { 
      unsigned int extIndex = fIndex[i]; 
      const MinimizerVariable & var = fVariables[ extIndex ]; 
      if (var.IsLimited() )  
         fX[ extIndex ] = var.InternalToExternal( x[i] ); 
      else
         fX[ extIndex ] = x[i]; 
   }

//       std::cout << "Transform:  external "; 
//       for (int i = 0; i < fX.size(); ++i) std::cout << fX[i] << "  "; 
//       std::cout << "\n";

   return &fX.front(); 
}

void  MinimTransformFunction::InvTransformation(const double * xExt,  double * xInt) const { 
   // inverse function transformation (external -> internal)
   for (unsigned int i = 0; i < NDim(); ++i ) { 
      unsigned int extIndex = fIndex[i]; 
      const MinimizerVariable & var = fVariables[ extIndex ];          
      assert ( !var.IsFixed() );
      if (var.IsLimited() )  
         xInt[ i ] = var.ExternalToInternal( xExt[extIndex] ); 
      else 
         xInt[ i ] = xExt[extIndex]; 
   }
}

void  MinimTransformFunction::InvStepTransformation(const double * x, const double * sExt,  double * sInt) const { 
   // inverse function transformation for steps (external -> internal)
   for (unsigned int i = 0; i < NDim(); ++i ) { 
      unsigned int extIndex = fIndex[i]; 
      const MinimizerVariable & var = fVariables[ extIndex ];          
      assert ( !var.IsFixed() );
      if (var.IsLimited() )  {
         // bound variables 
         double x2 = x[extIndex] + sExt[extIndex]; 
         if (var.HasUpperBound() && x2 >= var.UpperBound() ) 
            x2 = x[extIndex] - sExt[extIndex]; 
         // transform x and x2 
         double xint = var.ExternalToInternal ( x[extIndex] );
         double x2int = var.ExternalToInternal( x2 ); 
         sInt[i] =  std::abs( x2int - xint);  
      }
      else 
         sInt[ i ] = sExt[extIndex]; 
   }
}


   } // end namespace Math

} // end namespace ROOT

