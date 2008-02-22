// @(#)root/eve:$Id$
// Author: Matevz Tadel 2007

/*************************************************************************
 * Copyright (C) 1995-2007, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TEveSelection.h"
#include "TEveProjectionBases.h"
#include "TEveManager.h"

#include "TClass.h"

//______________________________________________________________________________
//
// Make sure there is a SINGLE running TEveSelection for each
// selection type (select/highlight).

ClassImp(TEveSelection);

//______________________________________________________________________________
TEveSelection::TEveSelection(const Text_t* n, const Text_t* t) :
   TEveElementList(n, t),
   fPickToSelect  (kPS_Element),
   fActive        (kTRUE),
   fIsMaster      (kTRUE)
{
   // Constructor.

   fSelElement       = &TEveElement::SelectElement;
   fIncImpSelElement = &TEveElement::IncImpliedSelected;
   fDecImpSelElement = &TEveElement::DecImpliedSelected;
}

void TEveSelection::SetHighlightMode()
{
   fPickToSelect = kPS_Projectable;
   fIsMaster     = kFALSE;

   fSelElement       = &TEveElement::HighlightElement;
   fIncImpSelElement = &TEveElement::IncImpliedHighlighted;
   fDecImpSelElement = &TEveElement::DecImpliedHighlighted;
}


/******************************************************************************/
// Protected helpers
/******************************************************************************/

//______________________________________________________________________________
void TEveSelection::DoElementSelect(TEveSelection::SelMap_i entry)
{
   TEveElement *el  = entry->first;
   Set_t       &set = entry->second;

   (el->*fSelElement)(kTRUE);
   el->FillImpliedSelectedSet(set);
   for (Set_i i = set.begin(); i != set.end(); ++i)
      ((*i)->*fIncImpSelElement)();
}

//______________________________________________________________________________
void TEveSelection::DoElementUnselect(TEveSelection::SelMap_i entry)
{
   TEveElement *el  = entry->first;
   Set_t       &set = entry->second;

   for (Set_i i = set.begin(); i != set.end(); ++i)
      ((*i)->*fDecImpSelElement)();
   set.clear();
   (el->*fSelElement)(kFALSE);
}


/******************************************************************************/
// Overrides of child-element-management virtuals from TEveElement
/******************************************************************************/

//______________________________________________________________________________
Bool_t TEveSelection::AcceptElement(TEveElement* el)
{
   // Pre-addition check. Deny addition if el is already selected.
   // Virtual from TEveElement.

   return el != this && fImpliedSelected.find(el) == fImpliedSelected.end() &&
          el->IsA()->InheritsFrom(TEveSelection::Class()) == kFALSE;
}

//______________________________________________________________________________
void TEveSelection::AddElement(TEveElement* el)
{
   // Add an element into selection.
   // Virtual from TEveElement.

   TEveElementList::AddElement(el);

   SelMap_i i = fImpliedSelected.insert(std::make_pair(el, Set_t())).first;
   if (fActive)
   {
      DoElementSelect(i);
   }
}

//______________________________________________________________________________
void TEveSelection::RemoveElementLocal(TEveElement* el)
{
   // Virtual from TEveElement.

   SelMap_i i = fImpliedSelected.find(el);

   if (i != fImpliedSelected.end())
   {
      if (fActive)
      {
         DoElementUnselect(i);
      }
      fImpliedSelected.erase(i);
   }
   else
   {
      Warning("TEveSelection::RemoveElementLocal", "element not found in map.");
   }
}

//______________________________________________________________________________
void TEveSelection::RemoveElementsLocal()
{
   // Virtual from TEveElement.

   if (fActive)
   {
      for (SelMap_i i = fImpliedSelected.begin(); i != fImpliedSelected.end(); ++i)
         DoElementUnselect(i);
   }
   fImpliedSelected.clear();
}


/******************************************************************************/
// Activation / deactivation of selection
/******************************************************************************/

//______________________________________________________________________________
void TEveSelection::ActivateSelection()
{
   for (SelMap_i i = fImpliedSelected.begin(); i != fImpliedSelected.end(); ++i)
      DoElementSelect(i);
   fActive = kTRUE;
}

//______________________________________________________________________________
void TEveSelection::DeactivateSelection()
{
   fActive = kFALSE;
   for (SelMap_i i = fImpliedSelected.begin(); i != fImpliedSelected.end(); ++i)
      DoElementUnselect(i);
}


/******************************************************************************/
// User input processing
/******************************************************************************/

//______________________________________________________________________________
TEveElement* TEveSelection::MapPickedToSelected(TEveElement* el)
{
   switch (fPickToSelect)
   {
      case kPS_Ignore:
      {
         return 0;
      }
      case kPS_Element:
      {
         return el;
      }
      case kPS_Projectable:
      {
         TEveProjected* p = dynamic_cast<TEveProjected*>(el);
         if (p) {
            TEveElement* pm = dynamic_cast<TEveElement*>(p->GetProjectable());
            if (pm)
               return pm;
         }
         return el;
      }
      case kPS_Compound:
      {
         Error("TEveSelection::MapPickedToSelected", "Compound pick-to-select mode not supported.");
         return el;
      }  
   }
   return el;
}

//______________________________________________________________________________
void TEveSelection::UserPickedElement(TEveElement* el, Bool_t multi)
{
   el = MapPickedToSelected(el);

   if (el || GetNChildren() > 0)
   {
      if (!multi)
         RemoveElements();
      if (el)
      {
         if (HasChild(el))
             RemoveElement(el);
         else
            AddElement(el);
      }
      if (fIsMaster)
         gEve->ElementSelect(el);
      gEve->Redraw3D();
   }
}
