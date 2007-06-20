// @(#)root/tmva $Id: ModulekNN.h,v 1.7 2007/06/20 08:47:18 stelzer Exp $
// Author: Rustem Ospanov

/**********************************************************************************
 * Project: TMVA - a Root-integrated toolkit for multivariate data analysis       *
 * Package: TMVA                                                                  *
 * Class  : ModulekNN                                                             *
 * Web    : http://tmva.sourceforge.net                                           *
 *                                                                                *
 * Description:                                                                   *
 *      Module for k-nearest neighbor algorithm                                   *
 *                                                                                *
 * Author:                                                                        *
 *      Rustem Ospanov <rustem@fnal.gov> - U. of Texas at Austin, USA             *
 *                                                                                *
 * Copyright (c) 2007:                                                            *
 *      CERN, Switzerland                                                         * 
 *      MPI-K Heidelberg, Germany                                                 * 
 *      U. of Texas at Austin, USA                                                *
 *                                                                                *
 * Redistribution and use in source and binary forms, with or without             *
 * modification, are permitted according to the terms listed in LICENSE           *
 * (http://tmva.sourceforge.net/LICENSE)                                          *
 **********************************************************************************/

#ifndef ROOT_TMVA_KNN_ModulekNN
#define ROOT_TMVA_KNN_ModulekNN

// C++
#include <cassert>
#include <iosfwd>
#include <map>
#include <vector>

// ROOT
#include "Rtypes.h"

// Local
#ifndef ROOT_TMVA_KNN_Node
#include "TMVA/NodekNN.h"
#endif
#ifndef ROOT_TMVA_MsgLogger
#include "TMVA/MsgLogger.h"
#endif

namespace TMVA
{
   namespace kNN
   {
      typedef Float_t VarType;
      typedef std::vector<VarType> VarVec;
      
      class Event
      {
      public:

         Event();
         Event(const VarVec &vec, Double_t weight, Short_t type);
         ~Event();
      
         Double_t GetWeight() const;

         VarType GetVar(UInt_t i) const;

         UInt_t GetNVar() const;

         Short_t GetType() const;

         // keep these two function separate
         VarType GetDist(VarType var, UInt_t ivar) const;
         VarType GetDist(const Event &other) const;

         void Print() const;
         void Print(std::ostream& os) const;

      private:

         VarVec fVar;
 
         Double_t fWeight;
         Short_t fType;
      };

      typedef std::vector<TMVA::kNN::Event> EventVec;
      typedef std::pair<const Node<Event> *, VarType> Elem;
      typedef std::list<Elem> List;

      std::ostream& operator<<(std::ostream& os, const Event& event);

      class ModulekNN
      {
      public:

         typedef std::map<int, std::vector<Double_t> > VarMap;

      public:

         ModulekNN();
         ~ModulekNN();

         void Clear();

         void Add(const Event &event);

         Bool_t Fill(const UShort_t odepth, UInt_t ifrac, const std::string &option = "");

         Bool_t Find(Event event, UInt_t nfind = 100) const;
         Bool_t Find(UInt_t nfind, const std::string &option) const;
      
         const EventVec& GetEventVec() const;

         const List& GetkNNList() const;
         const Event& GetkNNEvent() const;

         const VarMap& GetVarMap() const;

         const std::map<Int_t, Double_t>& GetMetric() const;
      
         void Print() const;
         void Print(std::ostream &os) const;

      private:

         Node<Event>* Optimize(UInt_t optimize_depth);

         void ComputeMetric(UInt_t ifrac);

         const Event Scale(const Event &event) const;

      private:

         UInt_t fDimn;

         Node<Event> *fTree;

         std::map<Int_t, Double_t> fVarScale;

         mutable List  fkNNList;     // latest result from kNN search
         mutable Event fkNNEvent;    // latest event used for kNN search

         std::map<Short_t, UInt_t> fCount; // count number of events of each type

         EventVec fEvent; // vector of all events used to build tree and analysis
         VarMap   fVar;   // sorted map of variables in each dimension for all event types

         mutable MsgLogger fLogger; // message logger
      };

      //
      // inlined functions for Event class
      //
      inline VarType Event::GetDist(const VarType var1, const UInt_t ivar) const
      {
         const VarType var2 = GetVar(ivar);
         return (var1 - var2) * (var1 - var2);
      }
      inline Double_t Event::GetWeight() const
      {
         return fWeight;
      }
      inline VarType Event::GetVar(const UInt_t i) const
      {
         assert(i < GetNVar());
         return fVar[i];
      }
      inline UInt_t Event::GetNVar() const
      {
         return fVar.size();
      }
      inline Short_t Event::GetType() const
      {
         return fType;
      }

      //
      // inline functions for ModulekNN class
      //
      inline const List& ModulekNN::GetkNNList() const
      {
         return fkNNList;
      }
      inline const Event& ModulekNN::GetkNNEvent() const
      {
         return fkNNEvent;
      }
      inline const EventVec& ModulekNN::GetEventVec() const
      {
         return fEvent;
      }
      inline const ModulekNN::VarMap& ModulekNN::GetVarMap() const
      {
         return fVar;
      }
      inline const std::map<Int_t, Double_t>& ModulekNN::GetMetric() const
      {
         return fVarScale;
      }

   } // end of kNN namespace
} // end of TMVA namespace

#endif

