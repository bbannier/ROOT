// @(#)root/tree:$Name:  $:$Id: TLeafB.h,v 1.2 2000/06/13 09:27:08 brun Exp $
// Author: Rene Brun   12/01/96

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TLeafB
#define ROOT_TLeafB


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TLeafB                                                               //
//                                                                      //
// A TLeaf for an 8 bit Integer data type.                              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TLeaf
#include "TLeaf.h"
#endif

class TLeafB : public TLeaf {

protected:
    Char_t       fMinimum;         //Minimum value if leaf range is specified
    Char_t       fMaximum;         //Maximum value if leaf range is specified
    Char_t       *fValue;          //!Pointer to data buffer
    Char_t       **fPointer;       //!Address of a pointer to data buffer!

public:
    TLeafB();
    TLeafB(const char *name, const char *type);
    virtual ~TLeafB();

    virtual void    Export(TClonesArray *list, Int_t n);
    virtual void    FillBasket(TBuffer &b);
    const char     *GetTypeName() const;
    Double_t        GetValue(Int_t i=0);
    virtual void   *GetValuePointer() {return fValue;}
    virtual void    Import(TClonesArray *list, Int_t n);
    virtual void    Print(Option_t *option="");
    virtual void    ReadBasket(TBuffer &b);
    virtual void    ReadBasketExport(TBuffer &b, TClonesArray *list, Int_t n);
    virtual void    SetAddress(void *add=0);

    ClassDef(TLeafB,1)  //A TLeaf for an 8 bit Integer data type.
};

inline Double_t TLeafB::GetValue(Int_t i)
  { return (IsUnsigned())? (UChar_t)(fValue[i]):fValue[i]; }

#endif
