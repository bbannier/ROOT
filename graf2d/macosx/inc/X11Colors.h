//Author: Timur Pocheptsov 28/11/2011

#ifndef ROOT_X11Colors
#define ROOT_X11Colors

/////////////////////////////////////////////////////////////////
//                                                             //
// ROOT's GUI depends and relies on X11 XParseColors.          //
// Color can be specified by name (found in X11 rgb.txt)       //
// or as RGB triplet: #rgb #rrggbb #rrrgggbbb #rrrrggggbbbb.   //
//                                                             //
/////////////////////////////////////////////////////////////////

#include <map>

#ifndef ROOT_GuiTypes
#include "GuiTypes.h"
#endif
#ifndef ROOT_TString
#include "TString.h"
#endif

namespace ROOT {
namespace MacOSX {
namespace X11 {

class ColorParser {
public:
   ColorParser();

   bool ParseColor(const TString &colorString, ColorStruct_t &color)const;

private:
   bool ParseRGBTriplet(const TString &rgb, ColorStruct_t &color)const;
   bool LookupColorByName(const TString &colorName, ColorStruct_t &color)const;

   struct RGB_t {
      RGB_t()
         : fRed(0), fGreen(0), fBlue(0)
      {
      }
      RGB_t(unsigned r, unsigned g, unsigned b)
         : fRed(r), fGreen(g), fBlue(b)
      {
      }
      unsigned fRed;
      unsigned fGreen;
      unsigned fBlue;
   };

   std::map<TString, RGB_t> fX11RGB;//X11's rgb.txt
};

}//X11
}//MacOSX
}//ROOT

#endif
