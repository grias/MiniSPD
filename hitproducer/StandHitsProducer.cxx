#include "StandHitsProducer.h"

#include <iostream>

StandHitsProducer::StandHitsProducer()
{
    fIOManager = new StandIOManager();
}

StandHitsProducer::~StandHitsProducer()
{
}

void StandHitsProducer::ProduceHitsFromAllEvents()
{
    std::cout << "-I-<StandHitsProducer::ProduceHitsFromAllEvents>" << std::endl;

    OpenInputOutputFiles();

    Long64_t nEvents = fIOManager->GetNumberOfInputEvents();
    ProduceHitsFromEvents(0, nEvents - 1);

    CloseInputOutputFiles();
}

void StandHitsProducer::ProduceHitsFromOneEvent(UInt_t event)
{
    std::cout << "-I-<StandHitsProducer::ProduceHitsFromOneEvent>" << std::endl;

    OpenInputOutputFiles();

    ProduceHitsFromEvents(event, event);

    CloseInputOutputFiles();
}

void StandHitsProducer::ProduceHitsFromEvents(UInt_t startEvent, UInt_t endEvent)
{
    if (!IsEventsExist(startEvent, endEvent)) return;

    for (size_t iEvent = startEvent; iEvent <= endEvent; iEvent++)
    {
        fIOManager->StartEvent(iEvent);
        ProduceHitsFromCurrentEvent();
        fIOManager->EndEvent();
    }
}

void StandHitsProducer::ProduceHitsFromCurrentEvent()
{
    // std::cout << "-I-<StandHitsProducer::ProduceHitsFromCurrentEvent> Processing event " << iEvent << std::endl;

    for (auto &&hitMaker : fHitMakersCollection)
    {
        hitMaker->ProduceHitsFromCurrentEvent();
    }
}

void StandHitsProducer::OpenInputOutputFiles()
{
    std::cout << "-I-<StandHitsProducer::OpenInputOutputFiles>" << std::endl;

    ConfigureInput();
    ConfigureOutput();

    fIOManager->Init();

    GetInputData();
    GetOutputData();
}

void StandHitsProducer::ConfigureInput()
{
    std::cout << "-I-<StandHitsProducer::ConfigureInput>" << std::endl;

    fIOManager->SetInputFileName(fInputFileName);

    for (auto &&hitMaker : fHitMakersCollection)
    {
        fIOManager->RegisterInputBranch(hitMaker->GetInputBranchName(), hitMaker->GetInputClassName());
    }
}

void StandHitsProducer::ConfigureOutput()
{
    std::cout << "-I-<StandHitsProducer::ConfigureOutput> Name: "<< fOutputFileName << std::endl;

    fIOManager->SetOutputFileName(fOutputFileName);

    for (auto &&hitMaker : fHitMakersCollection)
    {
        fIOManager->RegisterOutputBranch(hitMaker->GetOutputBranchName(), hitMaker->GetOutputClassName());
    }
}

void StandHitsProducer::GetInputData()
{
    for (auto &&hitMaker : fHitMakersCollection)
    {
        auto inputClonesArray = fIOManager->GetInputDataArray(hitMaker->GetInputBranchName());
        hitMaker->SetInputDigitsArray(inputClonesArray);
    }
}

void StandHitsProducer::GetOutputData()
{
    for (auto &&hitMaker : fHitMakersCollection)
    {
        auto outputClonesArray = fIOManager->GetOutputDataArray(hitMaker->GetOutputBranchName());
        hitMaker->SetOutputHitsArray(outputClonesArray);
    }
}

void StandHitsProducer::CloseInputOutputFiles()
{
    std::cout << "-I-<StandHitsProducer::CloseInputOutputFiles>" << std::endl;
    fIOManager->Finish();
}

Bool_t StandHitsProducer::IsEventsExist(UInt_t startEvent, UInt_t endEvent)
{
    if (startEvent > endEvent)
    {
        std::cerr
            << "-E-<StandHitsProducer::ProduceHitsFromEvents> startEvent = " << startEvent
            << " is more than endEvent = " << endEvent
            << std::endl;
        return kFALSE;
    }

    Long64_t nEvents = fIOManager->GetNumberOfInputEvents();
    if (endEvent >= nEvents)
    {
        std::cerr
            << "-E-<StandHitsProducer::ProduceHitsFromEvents> endEvent = " << endEvent
            << " is more than events number in file = " << nEvents
            << std::endl;
        return kFALSE;
    }

    return kTRUE;
}
