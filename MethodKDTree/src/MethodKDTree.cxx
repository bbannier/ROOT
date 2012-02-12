// @(#)root/tmva $Id$
// Author: Alexander Voigt

#include "TMVA/MethodKDTree.h"
#include "TMVA/Tools.h"
#include "TMVA/ClassifierFactory.h"
#include "TMVA/Config.h"

#include "TFile.h"

#include <vector>
using namespace std;

REGISTER_METHOD(KDTree)

ClassImp(TMVA::MethodKDTree)

//_______________________________________________________________________
TMVA::MethodKDTree::MethodKDTree(const TString& jobName,
                                 const TString& methodTitle,
                                 DataSetInfo& dsi,
                                 const TString& theOption,
                                 TDirectory* theTargetDir)
   : MethodBase(jobName, Types::kKDTree, methodTitle, dsi, theOption, theTargetDir)
   , fKDTree()
   , fKDTreeName()
   , fVolFrac(0.1)
   , fBucketSize(300)
   , fCompress(kTRUE)
   , fRadius(0.0)
{
   // init KDTree objects
}

//_______________________________________________________________________
TMVA::MethodKDTree::MethodKDTree(DataSetInfo& dsi,
                                 const TString& theWeightFile,
                                 TDirectory* theTargetDir)
   : MethodBase(Types::kKDTree, dsi, theWeightFile, theTargetDir)
   , fKDTree()
   , fKDTreeName()
   , fVolFrac(0.1)
   , fBucketSize(300)
   , fCompress(kTRUE)
   , fRadius(0.0)
{
   // constructor from weight file
}

//_______________________________________________________________________
TMVA::MethodKDTree::~MethodKDTree(void)
{
   DeleteKDTrees();
}

//_______________________________________________________________________
void TMVA::MethodKDTree::DeleteKDTrees()
{
   // Delete all KDTrees
   for (TKDTreeContainer::iterator it = fKDTree.begin();
        it != fKDTree.end(); ++it) {
      delete *it;
   }
   fKDTree.clear();
}

//_______________________________________________________________________
void TMVA::MethodKDTree::Reset()
{
   // reset MethodKDTree:  delete all KDTrees
   DeleteKDTrees();
}

//_______________________________________________________________________
Bool_t TMVA::MethodKDTree::HasAnalysisType(Types::EAnalysisType type, UInt_t numberClasses, UInt_t /*numberTargets*/)
{
   // KDTree can handle classification with two classes
   if (type == Types::kClassification && numberClasses == 2) return kTRUE;
   if (type == Types::kMulticlass) return kFALSE;
   if (type == Types::kRegression) return kFALSE;
   return kFALSE;
}

//_______________________________________________________________________
void TMVA::MethodKDTree::Init(void)
{
   // default initialization called by all constructors
   fVolFrac        = 0.1;      // range searching volume
   fBucketSize     = 300;      // number of events in terminal nodes
   fCompress       = kTRUE;    // compress ROOT output file
   fRadius         = 0.0;      // radius of range searching volume

   fKDTreeName.clear();
   fKDTreeName.push_back("SignalKDTree");
   fKDTreeName.push_back("BgKDTree");
}

//_______________________________________________________________________
void TMVA::MethodKDTree::DeclareOptions()
{
   //
   // Declare MethodKDTree options
   //
   DeclareOptionRef(fVolFrac = 0.1,    "VolFrac",  "Relative size of range searching volume");
   DeclareOptionRef(fBucketSize = 300, "BucketSize", "Number of events in terminal nodes");
   DeclareOptionRef(fCompress = kTRUE, "Compress", "Compress ROOT output file");
}

//_______________________________________________________________________
void TMVA::MethodKDTree::ProcessOptions()
{
   // process user options
   if (!(fVolFrac >= 0. && fVolFrac <= 1.)) {
      Log() << kWARNING << "VolFrac not in [0,1] ==> using 0.1 instead" << Endl;
      fVolFrac = 0.1;
   }

   if (fBucketSize > GetNEvents()) {
      Log() << kWARNING << "BucketSize < number of events ==> adjusting BucketSize to "
            << GetNEvents() << Endl;
      fBucketSize = GetNEvents();
   }
}

//_______________________________________________________________________
void TMVA::MethodKDTree::Train()
{
   // Train the method
   //
   // Create two KDTrees, one filled with signal, the other filled
   // with background events.

   static const UInt_t nKDTrees = 2;

   for (UInt_t i = 0; i < nKDTrees; ++i) {
      // i == 0 : signal
      // i == 1 : background

      // count number of events to be filled into the KDTree
      Long64_t nEvents = 0;
      for (Long64_t k = 0; k < GetNEvents(); ++k) {
         const Event* ev = GetEvent(k);
         if ((i == 0 && DataInfo().IsSignal(ev)) || (i == 1 && !DataInfo().IsSignal(ev))) {
            if (!(IgnoreEventsWithNegWeightsInTraining() && ev->GetWeight() <= 0)) {
               ++nEvents;
            }
         }
      }

      // create vectors of data columns
      vector<Value*> data;
      for (UInt_t ivar = 0; ivar < GetNvar(); ++ivar) {
         data.push_back(new Value[nEvents]);
      }

      Log() << kVERBOSE << "Filling " << nEvents << " events into \""
            << fKDTreeName.at(i) << "\"" << Endl;

      // fill event arrays
      Long64_t eventCount = 0;
      for (Long64_t k = 0; k < GetNEvents(); ++k) {
         const Event* ev = GetEvent(k);
         if ((i == 0 && DataInfo().IsSignal(ev)) || (i == 1 && !DataInfo().IsSignal(ev))) {
            if (!(IgnoreEventsWithNegWeightsInTraining() && ev->GetWeight() <= 0)) {
               for (UInt_t ivar = 0; ivar < GetNvar(); ++ivar) {
                  data[ivar][eventCount] = ev->GetValue(ivar);
               }
               ++eventCount;
            }
         }
      }
      assert(eventCount == nEvents);
      Log() << kINFO << "Creating KDTree \"" << fKDTreeName.at(i) << "\"" << Endl;

      TKDTreeType *kdtree = new TKDTreeType(nEvents, GetNvar(), fBucketSize);
      for (UInt_t ivar = 0; ivar < GetNvar(); ++ivar) {
         kdtree->SetData(ivar, data[ivar]);
      }
      // make sure the KDTree deletes the data vector
      kdtree->SetOwner(kTRUE);

      Log() << kVERBOSE << "Building KDTree \"" << fKDTreeName.at(i) << "\" ... ";
      kdtree->Build();
      Log() << kVERBOSE << "done (" << kdtree->GetNNodes() << " nodes created)" << Endl;
      fKDTree.push_back(kdtree);
   }

   // calculate radius
   CalculateRadius();
}

//_______________________________________________________________________
void TMVA::MethodKDTree::CalculateRadius()
{
   // Calculate the radius of the range searching sphere

   const UInt_t kDim = GetNvar(); // == Data()->GetNVariables();
   Float_t *xmin = new Float_t[kDim];
   Float_t *xmax = new Float_t[kDim];

   // set default values
   for (UInt_t dim = 0; dim < kDim; ++dim) {
      xmin[dim] = FLT_MAX;
      xmax[dim] = FLT_MIN;
   }

   // loop over all testing singnal and background events and
   // calculate minimal and maximal value of every variable
   for (Long64_t i = 0; i < GetNEvents() ; ++i) {
      const Event* ev = GetEvent(i);
      for (UInt_t dim = 0; dim < kDim; ++dim) {
         const Float_t val = ev->GetValue(dim);
         if (val < xmin[dim])
            xmin[dim] = val;
         if (val > xmax[dim])
            xmax[dim] = val;
      }
   }

   // calculate the range searching volume
   Float_t volume = 1.0;
   for (UInt_t dim = 0; dim < kDim; ++dim) {
      volume *= xmax[dim] - xmin[dim];
   }
   volume *= fVolFrac;

   // calculate radius of n-dimensional sphere with that volume
   fRadius = TMath::Power(volume * TMath::Gamma(1 + kDim / 2.0), 1. / kDim) / TMath::Sqrt(TMath::Pi());

   // clean up
   delete[] xmin;
   delete[] xmax;
}

//_______________________________________________________________________
Double_t TMVA::MethodKDTree::GetMvaValue(Double_t* err, Double_t* errUpper)
{
   // Calculate the discriminant
   //
   //    D = N_sig / (N_bkg + N_sig)
   //
   // where
   //
   //    N_sig = # signal events in range searching sphere
   //    N_bkg = # background events in range searching sphere
   //
   // If N_bkg + N_sig == 0, then 0.5 is returned.

   const Event* ev = GetEvent();
   std::vector<Float_t> xvec(ev->GetValues());

   vector<Int_t> sigNodes;
   fKDTree.at(0)->FindInRange(&(xvec.front()), fRadius, sigNodes);
   vector<Int_t> bkgNodes;
   fKDTree.at(1)->FindInRange(&(xvec.front()), fRadius, bkgNodes);

   const size_t nSig = sigNodes.size();
   const size_t nBkg = bkgNodes.size();

   // calculate the mva value
   Double_t mvaValue = 0.;
   if (nSig + nBkg > 0)
      mvaValue = 1.0 * nSig / (nSig + nBkg);
   else
      mvaValue = 0.5; // assume 50% signal probability, if no events found (bad assumption, but can be overruled by cut on error)

   return mvaValue;
}

//_______________________________________________________________________
void TMVA::MethodKDTree::ReadWeightsFromXML(void* wghtnode)
{
   // read KDTree variables from xml weight file
   gTools().ReadAttr(wghtnode, "VolFrac",           fVolFrac);
   gTools().ReadAttr(wghtnode, "BucketSize",        fBucketSize);
   gTools().ReadAttr(wghtnode, "Compress",          fCompress);
   gTools().ReadAttr(wghtnode, "Radius",            fRadius);

   DeleteKDTrees();
   ReadKDTreesFromFile();
}

//_______________________________________________________________________
void TMVA::MethodKDTree::ReadKDTreesFromFile()
{
   // read KDTrees from file

   TString rootFileName(GetWeightFileName());

   // replace in case of txt weight file
   rootFileName.ReplaceAll(TString(".") + gConfig().GetIONames().fWeightFileExtension + ".txt", ".xml");

   // add kdtree indicator to distinguish from main weight file
   rootFileName.ReplaceAll(".xml", "_kdtree.root");

   Log() << kINFO << "Read KDTrees from file: " << gTools().Color("lightblue")
         << rootFileName << gTools().Color("reset") << Endl;

   TFile *rootFile = new TFile(rootFileName, "READ");
   if (rootFile->IsZombie())
      Log() << kFATAL << "Cannot open file \"" << rootFileName << "\"" << Endl;

   // read trees from file
   fKDTree.push_back(ReadClonedKDTreeFromFile(rootFile, fKDTreeName.at(0)));
   fKDTree.push_back(ReadClonedKDTreeFromFile(rootFile, fKDTreeName.at(1)));

   rootFile->Close();
   delete rootFile;

   for (TKDTreeContainer::iterator it = fKDTree.begin(); it != fKDTree.end(); ++it) {
      if (!(*it))
         Log() << kFATAL << "Could not load KDTree!" << Endl;
   }
}

//_______________________________________________________________________
TMVA::MethodKDTree::TKDTreeType* TMVA::MethodKDTree::ReadClonedKDTreeFromFile(TFile* file, const TString& treeName) const
{
   if (file == NULL) {
      Log() << kWARNING << "<ReadClonedKDTreeFromFile>: NULL pointer given" << Endl;
      return NULL;
   }

   // try to load the kdtree from the file
   TKDTreeType *kdtree = (TKDTreeType*) file->Get(treeName);
   if (kdtree == NULL) {
      return NULL;
   }
   // try to clone the kdtree
   kdtree = (TKDTreeType*) kdtree->Clone();
   if (kdtree == NULL) {
      Log() << kWARNING << "<ReadClonedKDTreeFromFile>: " << treeName
            << " could not be cloned!" << Endl;
      return NULL;
   }

   return kdtree;
}

//_______________________________________________________________________
void TMVA::MethodKDTree::AddWeightsXMLTo(void* parent) const
{
   // create XML output of KDTree method variables
   void* wght = gTools().AddChild(parent, "Weights");
   gTools().AddAttr(wght, "VolFrac",           fVolFrac);
   gTools().AddAttr(wght, "BucketSize",        fBucketSize);
   gTools().AddAttr(wght, "Compress",          fCompress);
   gTools().AddAttr(wght, "Radius",            fRadius);

   WriteKDTreesToFile();
}

//_______________________________________________________________________
void TMVA::MethodKDTree::WriteKDTreesToFile() const
{
   // Write KDTrees to file

   TString rootFileName(GetWeightFileName());

   // replace in case of txt weight file
   rootFileName.ReplaceAll(TString(".") + gConfig().GetIONames().fWeightFileExtension + ".txt", ".xml");

   // add kdtree indicator to distinguish from main weight file
   rootFileName.ReplaceAll(".xml", "_kdtree.root");

   TFile *rootFile = NULL;
   if (fCompress) rootFile = new TFile(rootFileName, "RECREATE", "KDTreeFile", 9);
   else           rootFile = new TFile(rootFileName, "RECREATE");

   // write the trees
   for (UInt_t i = 0; i < fKDTree.size(); ++i) {
      Log() << kVERBOSE << "writing tree \"" << fKDTreeName.at(i) << "\" to file" << Endl;
      fKDTree.at(i)->Write(fKDTreeName.at(i).Data());
   }

   rootFile->Close();
   Log() << kINFO << "KDTrees written to file: "
         << gTools().Color("lightblue") << rootFileName
         << gTools().Color("reset") << Endl;
}

//_______________________________________________________________________
void TMVA::MethodKDTree::GetHelpMessage() const
{
   Log() << Endl;
   Log() << gTools().Color("bold") << "--- Short description:" << gTools().Color("reset") << Endl;
   Log() << Endl;
   Log() << "MethodKDTree is a self-adapting binning method to divide" << Endl;
   Log() << "the multi-dimensional variable space into a finite number of" << Endl;
   Log() << "subspaces using ROOT's TKDTree splitting algorithm." << Endl;
   Log() << Endl;
   Log() << gTools().Color("bold") << "--- Use of booking options:" << gTools().Color("reset") << Endl;
   Log() << Endl;
   Log() << "The following options can be set (the listed values are found" << Endl;
   Log() << "to be a good starting point for most applications):" << Endl;
   Log() << Endl;
   Log() << "              VolFrac     0.1   Volume fraction of the phase space to use for" << Endl;
   Log() << "                                the range-searching" << Endl;
   Log() << "           BucketSize     300   Number of events in a node required to split node" << Endl;
   Log() << "             Compress    True   Compress KDTree output file" << Endl;
}
