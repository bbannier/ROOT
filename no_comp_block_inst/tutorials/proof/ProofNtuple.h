//////////////////////////////////////////////////////////
//
// Example of TSelector implementation to do generic
// processing (filling a set of histograms in this case)
//
//////////////////////////////////////////////////////////

#ifndef ProofNtuple_h
#define ProofNtuple_h

#include <TSelector.h>

class TFile;
class TProofOutputFile;
class TNtuple;
class TRandom3;

class ProofNtuple : public TSelector {
public :

   // Specific members
   TFile            *fFile;
   TProofOutputFile *fProofFile; // For optimized merging of the ntuple
   TNtuple          *fNtp;
   TRandom3         *fRandom;

   ProofNtuple() : fFile(0), fProofFile(0), fNtp(0), fRandom(0) { }
   virtual ~ProofNtuple();
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual Bool_t  Process(Long64_t entry);
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   void PlotNtuple(TNtuple *, const char *);

   ClassDef(ProofNtuple,0);
};

#endif
