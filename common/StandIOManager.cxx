#include "StandIOManager.h"

StandIOManager::StandIOManager()
{
}

StandIOManager::~StandIOManager()
{
    Finish();
}

void StandIOManager::Init()
{
    InitInput();
    InitOutput();
}

void StandIOManager::InitInput()
{
    if (fIsInputInitialized)
    throw "-E-<StandIOManager::Init> Attempt of second input initialization!";

    OpenInputFile();  
    OpenInputBranches();

    fIsInputInitialized = kTRUE;
}

void StandIOManager::InitOutput()
{
    if (fIsOutputInitialized)
    throw "-E-<StandIOManager::Init> Attempt of second output initialization!";

    CreateOutputFile();
    CreateOutputBranches();

    fIsOutputInitialized = kTRUE;
}

void StandIOManager::Finish()
{
    FinishInput();
    FinishOutput();
}

void StandIOManager::FinishInput()
{
    fInputFileName = "";
    fInputBranchesNames.clear();
    for (auto &&branchPair : fInputDataMap)
    {
        if (branchPair.second) delete branchPair.second;
    }
    fInputDataMap.clear();
    if (fInputTree) 
    {
        delete fInputTree;
        fInputTree = nullptr;
    }
    fInputRootFile->Close();
    if (fInputRootFile) delete fInputRootFile;
    
    fIsInputInitialized = kFALSE;
}

void StandIOManager::FinishOutput()
{
    WriteTreeIntoOutputFile();

    fOutputFileName = "";
    fOutputBranchesNames.clear();
    for (auto &&branchPair : fOutputDataMap)
    {
        delete branchPair.second;
    }
    fOutputDataMap.clear();
    if (fOutputTree) 
    {
        delete fOutputTree;
        fOutputTree = nullptr;
    }
    fOutputRootFile->Close();
    if (fOutputRootFile) delete fOutputRootFile;
    
    fIsOutputInitialized = kFALSE;
}

void StandIOManager::StartEvent(UInt_t nEvent)
{
    ClearArrays();
    ReadInputEvent(nEvent);
}

void StandIOManager::EndEvent()
{
    FillEvent();
    ClearArrays();
}

void StandIOManager::ClearArrays()
{
    for (auto &&branchPair : fOutputDataMap)
    {
        branchPair.second->Clear();
    }
}

TClonesArray* StandIOManager::GetInputDataArray(TString branchName)
{
    return GetDataArray(fInputDataMap, branchName);
}

TClonesArray* StandIOManager::GetOutputDataArray(TString branchName)
{
    return GetDataArray(fOutputDataMap, branchName);
}

TClonesArray* StandIOManager::GetDataArray(std::map<TString, TClonesArray*> dataMap, TString branchName)
{
    auto dataArray = dataMap[branchName];
    if (!dataArray)
    {
        std::cerr<<"-E-<StandIOManager::GetDataArray> Cant find data with branchName \""<<branchName<<"\""<< std::endl;
        throw std::runtime_error("-E-<StandIOManager::GetDataArray> Cant find data array");
    }
    if (fVerbose)
        std::cout<<"-I-<StandIOManager::GetDataArray> Found "<<branchName<<std::endl;
    return dataArray;
}

void StandIOManager::OpenInputFile()
{
    if (fVerbose)
    std::cout<<"-I-<StandIOManager::OpenInputFile>"<<std::endl;
    fInputRootFile = new TFile(fInputFileName, "READ");
    if (!fInputRootFile) std::cerr << "-E-<StandIOManager::OpenInputFile> Cant find input file"<< std::endl;

    fInputTree = (TTree *)fInputRootFile->Get(fTreeName);
    if (fVerbose)
    std::cout<<"-I-<StandIOManager::OpenInputFile> Events in file: "<<GetNumberOfInputEvents()<<std::endl;
}

void StandIOManager::CreateOutputFile()
{
    if (fVerbose)
    std::cout<<"-I-<StandIOManager::CreateOutputFile> Output filename: "<<fOutputFileName<<std::endl;
    if (fOutputFileName)
    {
        fOutputRootFile = new TFile(fOutputFileName, "recreate");
        fOutputTree = new TTree(fTreeName, "TREE");
    }
    else
    {
        throw std::runtime_error("-E-<StandIOManager::CreateOutputFile> No output filename");
    }
}

void StandIOManager::OpenInputBranches()
{
    for (auto &&inputBranchPair : fInputBranchesNames)
    {
        auto clonesArray = new TClonesArray(inputBranchPair.second);
        fInputTree->SetBranchAddress(inputBranchPair.first, &clonesArray);

        fInputDataMap.insert({inputBranchPair.first, clonesArray});
        if (fVerbose)
        printf("-I-<StandIOManager::OpenInputBranches> Input branch %s registered\n", inputBranchPair.first.Data());
    }
}

void StandIOManager::CreateOutputBranches()
{
    for (auto &&outputBranchPair : fOutputBranchesNames)
    {
        auto clonesArray = new TClonesArray(outputBranchPair.second);
        fOutputTree->Branch(outputBranchPair.first, &clonesArray);

        fOutputDataMap.insert({outputBranchPair.first, clonesArray});
        if (fVerbose)
        printf("-I-<StandIOManager::CreateOutputBranches> Output branch %s created\n", outputBranchPair.first.Data());
    }
}
