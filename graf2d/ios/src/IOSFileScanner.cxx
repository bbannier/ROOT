#include <stdexcept>
#include <utility>
#include <memory>

#include "IOSFileScanner.h"
#include "TMultiGraph.h"
#include "TIterator.h"
#include "TFile.h"
#include "TList.h"
#include "TKey.h"
#include "TH1.h"
#include "TF2.h"

namespace ROOT_iOS {
namespace FileUtils {

//__________________________________________________________________________________________________________________________
TObject *ReadObjectForKey(TFile *inputFile, const TKey *key, std::string &option)
{
   option = "";

   TObject *objPtr = inputFile->Get(key->GetName());
   if (!objPtr)
      throw std::runtime_error("bad key in ReadObjectForKey");
   //Objects of some types are onwed by the file. So I have to make
   //them free from such ownership to make
   //their processing later more uniform.
   if (TH1 *hist = dynamic_cast<TH1 *>(objPtr))
      hist->SetDirectory(0);

   //The code below can throw, so I use auto_ptr.
   std::auto_ptr<TObject> obj(objPtr);

   //This is the trick, since ROOT seems not to preserve
   //Draw's option in a file.
   if (dynamic_cast<TF2 *>(obj.get()))
      option = "surf1";
   if (dynamic_cast<TMultiGraph *>(obj.get()))
      option = "AC";

   return obj.release();
}


//__________________________________________________________________________________________________________________________
void ScanFileForVisibleObjects(TFile *inputFile, const std::set<std::string> &visibleTypes, std::vector<TObject *> &objects, std::vector<std::string> &options)
{
   //Find objects of visible types in a root file.
   const TList *keys = inputFile->GetListOfKeys();
   TIter next(keys);
   std::vector<TObject *>tmp;
   std::vector<std::string> opts;
   std::string option;
   
   try {
      std::auto_ptr<TObject> newObject;
      
      while (const TKey *k = static_cast<TKey *>(next())) {
         //Check, if object, pointed by the key, is supported.
         if (visibleTypes.find(k->GetClassName()) != visibleTypes.end()) {
            newObject.reset(ReadObjectForKey(inputFile, k, option));//can throw std::runtimer_error (me) || std::bad_alloc (ROOT)
            tmp.push_back(newObject.get());//bad_alloc.
            opts.push_back(option);
            newObject.release();
         }
      }
   } catch (const std::exception &) {
      for (std::vector<TObject*>::size_type i = 0; i < tmp.size(); ++i)
         delete tmp[i];
      throw;
   }
   
   objects.swap(tmp);
   options.swap(opts);
}

}
}

