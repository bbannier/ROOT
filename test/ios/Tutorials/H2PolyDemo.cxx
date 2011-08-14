#include <stdexcept>
#include <iostream>

#include "H2PolyDemo.h"

#include "TRandom.h"
#include "TH2Poly.h"
#include "TFile.h"
#include "TMath.h"
#include "TKey.h"


#include "H2PolyDemo.h"

namespace ROOT_iOS {
namespace Demos {

//______________________________________________________________________________
H2PolyDemo::H2PolyDemo(const char *fileName)
{
   std::auto_ptr<TFile> inputFile(TFile::Open(fileName, "read"));
   if (!inputFile.get())
      return;
      
   fPoly.reset(dynamic_cast<TH2Poly *>(inputFile->Get("h2poly")));
   fPoly->SetDirectory(0);
}

//______________________________________________________________________________
H2PolyDemo::~H2PolyDemo()
{
   //For auto_ptr dtor only.
}

//______________________________________________________________________________
void H2PolyDemo::PresentDemo()
{
   if (fPoly.get())
      fPoly->Draw("COLZ");
}


}
}
