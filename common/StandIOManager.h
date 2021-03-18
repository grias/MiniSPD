#ifndef IOMANAGER_H
#define IOMANAGER_H

#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TClonesArray.h>

#include <iostream>
#include <map>
#include <set>


/*
------- TIPS:
FILLING:
new((*arr)[arr->GetEntriesFast()]) BmnDchDigit(map.plane, ch, tm, 0);
*/

class StandIOManager
{
public:
    StandIOManager();
    ~StandIOManager();

    void SetInputFileName(TString inputFileName) { fInputFileName = inputFileName; }
    void SetOutputFileName(TString outputFileName) { fOutputFileName = outputFileName; }

    void RegisterInputBranch(TString inputBranchName, TString className) { fInputBranchesNames.insert({inputBranchName, className}); }
    void RegisterOutputBranch(TString outputBranchName, TString className) { fOutputBranchesNames.insert({outputBranchName, className}); }

    void Init();
    void InitInput();
    void InitOutput();

    void Finish();
    void FinishInput();
    void FinishOutput();

    void FillEvent() { fOutputTree->Fill(); }
    void WriteTreeIntoOutputFile() { fOutputTree->Write(); }
    
    TClonesArray* GetInputDataArray(TString className);
    TClonesArray* GetOutputDataArray(TString className);
    void ReadInputEvent(Int_t nEvent) { fInputTree->GetEntry(nEvent); }
    void ClearArrays();
    Long64_t GetNumberOfInputEvents() { return fInputTree->GetEntries(); }

    void SetVerboseLevel(Int_t verbose) { fVerbose = verbose; }

private:
    void OpenInputFile();
    void CreateOutputFile();

    void OpenInputBranches();
    void CreateOutputBranches();


    Bool_t fIsInputInitialized = kFALSE;
    Bool_t fIsOutputInitialized = kFALSE;

    TString fInputFileName;
    TString fOutputFileName;
    TString fTreeName = "cbmsim";

    TFile* fInputRootFile;
    TFile* fOutputRootFile;
    TTree* fInputTree;
    TTree* fOutputTree;

    std::map<TString, TString> fInputBranchesNames;  /* Does set of TStrings work right? */
    std::map<TString, TString> fOutputBranchesNames;

    std::map<TString, TClonesArray*> fInputDataMap;
    std::map<TString, TClonesArray*> fOutputDataMap;

    Int_t fVerbose = 0;
    
};
#endif // IOMANAGER_H
