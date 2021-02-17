#include "StandHitsProducer.h"

#include "BmnEventHeader.h"
#include "BmnDchDigit.h"
#include "StandSiliconHit.h"
#include "StandSiliconGeoMapper.h"

#include <iostream>
#include <map>
#include <vector>

using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::vector;

/*
TODO: Move IO logic to a separate class
*/

StandHitsProducer::StandHitsProducer()
{
    fIOManager = new StandIOManager();
}

StandHitsProducer::~StandHitsProducer()
{
}

void StandHitsProducer::Init()
{
    cout << "-I-<StandHitsProducer::Init>" << endl;

    OpenInputOutputFiles();
}

void StandHitsProducer::Finish()
{
    cout << "-I-<StandHitsProducer::Finish>" << endl;

    CloseInputOutputFiles();
}

void StandHitsProducer::ProduceHitsFromAllEvents()
{
    cout << "-I-<StandHitsProducer::ProduceHitsFromAllEvents>" << endl;

    Long64_t nEvents = fIOManager->GetNumberOfInputEvents();
    ProduceHitsFromEvents(0, nEvents - 1);
}

void StandHitsProducer::ProduceHitsFromOneEvent(Int_t event)
{
    cout << "-I-<StandHitsProducer::ProduceHitsFromOneEvent>" << endl;

    ProduceHitsFromEvents(event, event);
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
        ProduceHitsFromEvent(iEvent);
    }

    fIOManager->WriteTreeIntoOutputFile();
}

void StandHitsProducer::ProduceHitsFromEvent(Int_t event)
{
    // cout << "-I-<StandHitsProducer::ProduceHitsFromEvent> Processing event " << event << endl;

    fIOManager->ClearArrays();
    fIOManager->ReadInputEvent(event);

    StandClustersContainer clustersContainer;
    ProcessSiliconDigitsIntoClusters(clustersContainer);

    CalculateLocalCoordinates(clustersContainer);

    ProcessSiliconClustersIntoHits(clustersContainer);

    fIOManager->FillEvent();
}

void StandHitsProducer::ProcessSiliconDigitsIntoClusters(StandClustersContainer &clustersContainer)
{
    Int_t nDigits = fSiliconDigitsArray->GetEntriesFast();
    // cout << "-I-<StandHitsProducer::ProcessSiliconDigitsIntoClusters> Total silicon digits: " << nDigits << endl;

    for (size_t iDigit = 0; iDigit < nDigits; iDigit++)
    {
        auto siliconDigit = (BmnSiliconDigit *)fSiliconDigitsArray->At(iDigit);
        if (!siliconDigit->IsGoodDigit())
            continue;

        StandSiliconCluster* cluster = ProcessSiliconDigitIntoCluster(siliconDigit);

        clustersContainer.AddCluster(cluster);
    }
    // clustersContainer.Print();
}

StandSiliconCluster* StandHitsProducer::ProcessSiliconDigitIntoCluster(BmnSiliconDigit *siliconDigit)
{
    Int_t station = siliconDigit->GetStation();
    Int_t module = siliconDigit->GetModule();
    Int_t side = siliconDigit->GetStripLayer();
    Int_t strip = siliconDigit->GetStripNumber();
    Double_t signal = siliconDigit->GetStripSignal();

    StandSiliconCluster* cluster = new StandSiliconCluster(station, module, side, strip, signal);

    // printf("-I-<StandHitsProducer::ProcessSiliconDigitIntoCluster> Station: %d, module: %d, side: %d, strip: %d, signal: %f\n",
    //        station, module, side, strip, signal);

    return cluster;
}

void StandHitsProducer::CalculateLocalCoordinates(StandClustersContainer &clustersContainer)
{
    for (auto &&clustersVector : clustersContainer.GetClustersMap())
    for (auto &&cluster : clustersVector.second)
    {
        CalculateLocalCoordinate(cluster);
        // cluster->Print();
    }

}

void StandHitsProducer::CalculateLocalCoordinate(StandSiliconCluster* siliconCluster)
{
    Int_t station = siliconCluster->GetStation();
    Int_t module = siliconCluster->GetModule();
    Int_t side = siliconCluster->GetSide();
    vector<pair<Double_t, Double_t>> coordinateAmplitudePairs;
    for (auto &&stripSignalPair : siliconCluster->GetStripSignalPairs())
    {
        Double_t coordinate = StandSiliconGeoMapper::CalculateLocalCoordinateForStrip(station, module, side, stripSignalPair.first);
        // std::cout<<"Strip: "<<stripSignalPair.first<<" Coordinate: "<<coordinate<<std::endl;
        coordinateAmplitudePairs.push_back({coordinate, stripSignalPair.second});
    }

    Double_t coordAmpSum = 0;
    Double_t amplitudeSum = 0;
    for (auto &&coordAmpPair : coordinateAmplitudePairs)
    {
        coordAmpSum += coordAmpPair.first * coordAmpPair.second;
        amplitudeSum += coordAmpPair.second;
    }
    Double_t localCoordinate = coordAmpSum / amplitudeSum;
    siliconCluster->SetLocalCoordinate(localCoordinate);
}

void StandHitsProducer::ProcessSiliconClustersIntoHits(StandClustersContainer &clustersContainer)
{
    // cout<<"-I-<StandHitsProducer::ProcessSiliconClustersIntoHits>"<<endl;

    for (size_t iStation = 0; iStation < 3; iStation++)
    for (size_t iModule = 0; iModule < 4; iModule++)
    {
        vector<StandSiliconCluster*> clustersSideX;
        vector<StandSiliconCluster*> clustersSideY;
        try
        {
            clustersSideX = clustersContainer.GetClustersVector(iStation, iModule, 0);
            clustersSideY = clustersContainer.GetClustersVector(iStation, iModule, 1);
        }
        catch(const std::exception& e)
        {
            continue;
        }

        for (auto &&clusterX : clustersSideX)
        {
            if (clusterX->GetClusterSize() > 4)
            {
                continue;
            }

            for (auto &&clusterY : clustersSideY)
            {
                if (clusterY->GetClusterSize() > 4)
                {
                    continue;
                }
            
                ProcessSiliconClustersIntoHit(clusterX, clusterY);
            }
        }
        
    }
}

void StandHitsProducer::ProcessSiliconClustersIntoHit(StandSiliconCluster* clusterX, StandSiliconCluster* clusterY)
{
    Int_t station = clusterX->GetStation();
    Int_t module = clusterX->GetModule();
    Double_t localX = clusterX->GetLocalCoordinate();
    Double_t localY = StandSiliconGeoMapper::CalculateLocalY(localX, clusterY->GetLocalCoordinate());
    // std::cout<<clusterY->GetLocalCoordinate() - clusterX->GetLocalCoordinate()<<"\t"<<localY<<std::endl;

    if (StandSiliconGeoMapper::IsInSensitiveRange(clusterX->GetStation(), localY))
    {
        StandSiliconHit* hit = new((*fSiliconHitsArray)[fSiliconHitsArray->GetEntriesFast()]) StandSiliconHit(station, module, localX, localY);
        hit->SetAmplitudes(clusterX->GetAmplitude(), clusterY->GetAmplitude());
        hit->SetClustersSizes(clusterX->GetClusterSize(), clusterY->GetClusterSize());
        // hit->Print();
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
    fIOManager->RegisterInputBranch("EventHeader", "BmnEventHeader");
    fIOManager->RegisterInputBranch("SILICON", "BmnSiliconDigit");
    fIOManager->RegisterInputBranch("DCH", "BmnDchDigit");
}

void StandHitsProducer::ConfigureOutput()
{
    cout << "-I-<StandHitsProducer::ConfigureOutput> Name: "<< fOutputFileName << endl;

    fIOManager->SetOutputFileName(fOutputFileName);

    // fIOManager->RegisterOutputBranch("EventHeader", "BmnEventHeader");
    fIOManager->RegisterOutputBranch("SiliconHits", "StandSiliconHit");
}

void StandHitsProducer::GetInputData()
{
    fInputEventHeader = fIOManager->GetInputDataArray("BmnEventHeader");
    fSiliconDigitsArray = fIOManager->GetInputDataArray("BmnSiliconDigit");
    fStrawDigitsArray = fIOManager->GetInputDataArray("BmnDchDigit");
}

void StandHitsProducer::GetOutputData()
{
    // fOutputEventHeader = fIOManager->GetOutputDataArray("BmnEventHeader");
    fSiliconHitsArray = fIOManager->GetOutputDataArray("StandSiliconHit");
}

void StandHitsProducer::CloseInputOutputFiles()
{
    cout << "-I-<StandHitsProducer::CloseInputOutputFiles>" << endl;
    fIOManager->Finish();
}
