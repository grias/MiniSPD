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
-------OUTPUT TIPS:
CREATION:
fDigiFileOut = new TFile(fDigiFileName, "recreate");
fDigiTree = new TTree("cbmsim", "bmn_digit");

dch = new TClonesArray("BmnDchDigit");
fDigiTree->Branch("DCH", &dch);

FILLING:
new((*arr)[arr->GetEntriesFast()]) BmnDchDigit(map.plane, ch, tm, 0);
fDigiTree->Fill();

SAVING:
fDigiTree->Write();


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
    void Finish();

    void FillEvent() { fOutputTree->Fill(); }
    void WriteTreeIntoOutputFile() { fOutputTree->Write(); }

    
    TClonesArray* GetInputDataArray(TString className);
    TClonesArray* GetOutputDataArray(TString className);
    void ReadInputEvent(Int_t nEvent) { fInputTree->GetEntry(nEvent); }
    void ClearArrays();
    Long64_t GetNumberOfInputEvents() { return fInputTree->GetEntries(); }
    

    

private:
    void OpenInputFile();
    void CreateOutputFile();

    void OpenInputBranches();
    void CreateOutputBranches();


    Bool_t fIsInitialized = kFALSE;

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
    
};
#endif // IOMANAGER_H
