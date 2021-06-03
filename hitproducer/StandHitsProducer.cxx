#include "StandHitsProducer.h"

#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

StandHitsProducer::StandHitsProducer()
{
    fIOManager = new StandIOManager();
}

StandHitsProducer::~StandHitsProducer()
{
}

void StandHitsProducer::ProduceHitsFromAllEvents()
{
    cout << "-I-<StandHitsProducer::ProduceHitsFromAllEvents>" << endl;

    OpenInputOutputFiles();

    Long64_t nEvents = fIOManager->GetNumberOfInputEvents();
    ProduceHitsFromEvents(0, nEvents - 1);

    CloseInputOutputFiles();
}

void StandHitsProducer::ProduceHitsFromOneEvent(Int_t event)
{
    cout << "-I-<StandHitsProducer::ProduceHitsFromOneEvent>" << endl;

    OpenInputOutputFiles();

    ProduceHitsFromEvents(event, event);

    CloseInputOutputFiles();
}

void StandHitsProducer::ProduceHitsFromEvents(Int_t startEvent, Int_t endEvent)
{
    if (startEvent > endEvent)
    {
        cerr
            << "-E-<StandHitsProducer::ProduceHitsFromEvents> startEvent = " << startEvent
            << " is more than endEvent = " << endEvent
            << endl;
        return;
    }

    Long64_t nEvents = fIOManager->GetNumberOfInputEvents();
    if (endEvent >= nEvents)
    {
        cerr
            << "-E-<StandHitsProducer::ProduceHitsFromEvents> endEvent = " << endEvent
            << " is more than events number in file = " << nEvents
            << endl;
        return;
    }

    for (size_t iEvent = startEvent; iEvent <= endEvent; iEvent++)
    {
        fIOManager->StartEvent(iEvent);
        ProduceHitsFromCurrentEvent();
        fIOManager->EndEvent();
    }
}

void StandHitsProducer::ProduceHitsFromCurrentEvent()
{
    // cout << "-I-<StandHitsProducer::ProduceHitsFromCurrentEvent> Processing event " << iEvent << endl;

    for (auto &&hitMaker : fHitMakersCollection)
    {
        hitMaker->ProduceHitsFromCurrentEvent();
    }
}

void StandHitsProducer::OpenInputOutputFiles()
{
    cout << "-I-<StandHitsProducer::OpenInputOutputFiles>" << endl;

    ConfigureInput();
    ConfigureOutput();

    fIOManager->Init();

    GetInputData();
    GetOutputData();
}

void StandHitsProducer::ConfigureInput()
{
    cout << "-I-<StandHitsProducer::ConfigureInput>" << endl;

    fIOManager->SetInputFileName(fInputFileName);

    for (auto &&hitMaker : fHitMakersCollection)
    {
        fIOManager->RegisterInputBranch(hitMaker->GetInputBranchName(), hitMaker->GetInputClassName());
    }
}

void StandHitsProducer::ConfigureOutput()
{
    cout << "-I-<StandHitsProducer::ConfigureOutput> Name: "<< fOutputFileName << endl;

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
    cout << "-I-<StandHitsProducer::CloseInputOutputFiles>" << endl;
    fIOManager->Finish();
}
