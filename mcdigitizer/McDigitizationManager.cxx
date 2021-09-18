#include "McDigitizationManager.h"

#include "StandIOManager.h"

#include "TString.h"

McDigitizationManager::McDigitizationManager(int runId)
{
    ConfigureIOManagerForDigits(runId);
    // ConfigureIOManagerForMc(runId);
}

McDigitizationManager::McDigitizationManager() : McDigitizationManager(8)
{
}

McDigitizationManager::~McDigitizationManager()
{
    SaveOutputFiles();
}

void McDigitizationManager::AddDigitizer(std::shared_ptr<AbstractMcDataDigitizer> digitizer)
{
    fDigitizersMap.insert({digitizer->GetDetectorName(), digitizer});
}

void McDigitizationManager::AddHit(McHit mcHit)
{
    fDigitizersMap.at(mcHit.GetDetectorName())->ProduceDigitFromMcHit(mcHit);
}

void McDigitizationManager::EndEvent()
{
    fIOManager->EndEvent();
}

void McDigitizationManager::ConfigureIOManagerForDigits(int runId)
{
    fIOManager = std::make_shared<StandIOManager>();
    fIOManager->SetVerboseLevel(1);
    auto outFileName = TString(getenv("VMCWORKDIR")) + Form("/macro/data/stand_run%04d_digits.root", runId);
    fIOManager->SetOutputFileName(outFileName);
}

void McDigitizationManager::ConfigureIOManagerForMc(int runId)
{
    fIOManagerMc = std::make_shared<StandIOManager>();
    fIOManagerMc->SetVerboseLevel(1);
    auto outFileNameMc = TString(getenv("VMCWORKDIR")) + Form("/macro/data/stand_run%04d_mchits.root", runId);
    fIOManagerMc->SetOutputFileName(outFileNameMc);
    fIOManagerMc->RegisterOutputBranch("MCHITS", "McHit");
    fIOManagerMc->InitOutput();
    fMcHitsArray = fIOManagerMc->GetOutputDataArray("MCHITS");
}

void McDigitizationManager::RegisterDigitizersInIOManager()
{
    for (auto const& digitizerPair : fDigitizersMap)
    {
        fIOManager->RegisterOutputBranch(digitizerPair.second->GetOutputBranchName(), digitizerPair.second->GetOutputClassName());
    }
}

void McDigitizationManager::SendClonesArraysToDigitizers()
{
    for (auto const& digitizerPair : fDigitizersMap)
    {
        auto detectorName = digitizerPair.first;
        auto digitizer = digitizerPair.second;
        auto outputClonesArray = fIOManager->GetOutputDataArray(digitizer->GetOutputBranchName());
        digitizer->SetOutputDigitsArray(outputClonesArray);
    }
}

void McDigitizationManager::InitOutput()
{
    RegisterDigitizersInIOManager();
    fIOManager->InitOutput();
    SendClonesArraysToDigitizers();
}

void McDigitizationManager::SaveOutputFiles()
{
    fIOManager->Finish();
    // fIOManagerMc->Finish();
}

