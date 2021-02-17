#include "StandTracksProducer.h"

#include "StandSiliconGeoMapper.h"
#include "StandSiliconTrack.h"

#include <TLinearFitter.h>

#include <iostream>
#include <cmath>

using std::cerr;
using std::cout;
using std::endl;

StandTracksProducer::StandTracksProducer()
{
    fIOManager = new StandIOManager();
}

StandTracksProducer::~StandTracksProducer()
{
}

void StandTracksProducer::Init()
{
    cout << "-I-<StandTracksProducer::Init>" << endl;

    OpenInputOutputFiles();
}

void StandTracksProducer::Finish()
{
    cout << "-I-<StandTracksProducer::Finish>" << endl;

    CloseInputOutputFiles();
}

void StandTracksProducer::ProduceTracksFromAllEvents()
{
    cout << "-I-<StandTracksProducer::ProduceTracksFromAllEvents>" << endl;

    Long64_t nEvents = fIOManager->GetNumberOfInputEvents();
    ProduceTracksFromEvents(0, nEvents - 1);
}

void StandTracksProducer::ProduceTracksFromOneEvent(Int_t event)
{
    cout << "-I-<StandTracksProducer::ProduceTracksFromOneEvent>" << endl;

    ProduceTracksFromEvents(event, event);
}

void StandTracksProducer::ProduceTracksFromEvents(Int_t startEvent, Int_t endEvent)
{
    if (startEvent > endEvent)
    {
        cerr
            << "-E-<StandTracksProducer::ProduceTracksFromEvents> startEvent = " << startEvent
            << " is more than endEvent = " << endEvent
            << endl;
        return;
    }

    Long64_t nEvents = fIOManager->GetNumberOfInputEvents();
    if (endEvent >= nEvents)
    {
        cerr
            << "-E-<StandTracksProducer::ProduceTracksFromEvents> endEvent = " << endEvent
            << " is more than events number in file = " << nEvents
            << endl;
        return;
    }

    for (size_t iEvent = startEvent; iEvent <= endEvent; iEvent++)
    {
        ProduceTracksFromEvent(iEvent);
    }

    fIOManager->WriteTreeIntoOutputFile();
}

void StandTracksProducer::ProduceTracksFromEvent(Int_t event)
{
    // cout << "-I-<StandTracksProducer::ProduceTracksFromEvent> Processing event " << event << endl;

    fIOManager->ClearArrays();
    fIOManager->ReadInputEvent(event);

    vector<TrackCandidate> trackCandidates;

    FindTrackCandidates(trackCandidates);

    FitTracks(trackCandidates);
    if (trackCandidates.size())
    {
        FindBestTrack(trackCandidates);
    }

    fIOManager->FillEvent();
}

void StandTracksProducer::FindTrackCandidates(vector<TrackCandidate> &trackCandidates)
{
    vector<HitWrapper> stationsHits[3] = {vector<HitWrapper>(), vector<HitWrapper>(), vector<HitWrapper>()};

    Int_t nHits = fSiliconHitsArray->GetEntriesFast();
    // cout << "-I-<StandTracksProducer::FindTrackCandidates> Total silicon hits: " << nHits << endl;

    for (size_t iHit = 0; iHit < nHits; iHit++)
    {
        auto localSiliconHit = (StandSiliconHit *)fSiliconHitsArray->At(iHit);
        // localSiliconHit->Print();
        Int_t station = localSiliconHit->GetStation();
        Int_t module = localSiliconHit->GetModule();
        Double_t localX = localSiliconHit->GetLocalX();
        Double_t localY = localSiliconHit->GetLocalY();

        HitWrapper hit(iHit, station);
        hit.module = module;
        

        hit.globalPosition = StandSiliconGeoMapper::CalculateGlobalCoordinatesForHit(station, module, localX, localY);
        
        stationsHits[hit.station].push_back(hit);
    }

    // cout<<"HitsInStations: "<<stationsHits[0].size()<<"\t"<<stationsHits[1].size()<<"\t"<<stationsHits[2].size()<<endl;

    for (auto &&iHit0 : stationsHits[0])
    for (auto &&iHit1 : stationsHits[1])
    for (auto &&iHit2 : stationsHits[2])
    {
        TrackCandidate candidate;
        candidate.hits.push_back(iHit0);
        candidate.hits.push_back(iHit1);
        candidate.hits.push_back(iHit2);
        trackCandidates.push_back(candidate);
    }

    // cout<<"Candidates: "<<trackCandidates.size()<<endl;
}

void StandTracksProducer::FitTracks(vector<TrackCandidate> &trackCandidates)
{
    for (auto &&trackCandidate : trackCandidates)
    {
        FitTrack(trackCandidate);
    }
}

void StandTracksProducer::FitTrack(TrackCandidate &trackCandidate)
{
    TLinearFitter linFitX(1);
    linFitX.SetFormula("pol1");
    TLinearFitter linFitY(1);
    linFitY.SetFormula("pol1");

    for (auto &&hit : trackCandidate.hits)
    {
        Double_t x = hit.globalPosition.X();
        Double_t y = hit.globalPosition.Y();
        Double_t z[1] = {hit.globalPosition.Z()};

        linFitX.AddPoint(z, x);
        linFitY.AddPoint(z, y);
    }

    linFitX.Eval();
    linFitY.Eval();

    TVectorD paramsX, paramsY;
    linFitX.GetParameters(paramsX);
    linFitY.GetParameters(paramsY);

    // cout<<"ParsX: "<<paramsX(0)<<"\t"<<paramsX(1)<<"\tChiSquare: "<<linFitX.GetChisquare()<<endl;
    // cout<<"ParsY: "<<paramsY(0)<<"\t"<<paramsY(1)<<"\tChiSquare: "<<linFitY.GetChisquare()<<endl;

    trackCandidate.parsX[0] = paramsX(0);
    trackCandidate.parsX[1] = paramsX(1);

    trackCandidate.parsY[0] = paramsY(0);
    trackCandidate.parsY[1] = paramsY(1);

    trackCandidate.chiSquare[0] = linFitX.GetChisquare();
    trackCandidate.chiSquare[1] = linFitY.GetChisquare();

    for (auto &&hit : trackCandidate.hits)
    {
        Int_t station = hit.station;
        trackCandidate.modules[station] = hit.module;
        trackCandidate.nHits[station] = hit.nHit;
        Double_t x = hit.globalPosition.X();
        Double_t y = hit.globalPosition.Y();
        Double_t z = hit.globalPosition.Z();

        Double_t residX = x - (paramsX(0) + paramsX(1)*z);
        Double_t residY = y - (paramsY(0) + paramsY(1)*z);

        trackCandidate.residualsX[station] = residX;
        trackCandidate.residualsY[station] = residY;

        // cout<<"Residuals: "<<residX<<"\t"<<residY<<endl;
    }
}

void StandTracksProducer::FindBestTrack(vector<TrackCandidate> &trackCandidates)
{
    Int_t nCandidates = trackCandidates.size();
    Int_t nBest = 0;
    Double_t bestChiSquare = 999999999;

    for (size_t iCandidate = 0; iCandidate < nCandidates; iCandidate++)
    {
        TrackCandidate candidate = trackCandidates[iCandidate];
        Double_t candidateChiSquare = 
            std::sqrt(candidate.chiSquare[0]*candidate.chiSquare[0] + candidate.chiSquare[1]*candidate.chiSquare[1]);
        if (candidateChiSquare <= bestChiSquare)
        {
            nBest = iCandidate;
        }
    }
    TrackCandidate bestCandidate = trackCandidates[nBest];

    StandSiliconTrack* track = 
        new((*fSiliconTracksArray)[fSiliconTracksArray->GetEntriesFast()]) StandSiliconTrack();

    Double_t hitPosX[3] = {bestCandidate.hits[0].globalPosition.X(),bestCandidate.hits[1].globalPosition.X(),bestCandidate.hits[2].globalPosition.X()};
    Double_t hitPosY[3] = {bestCandidate.hits[0].globalPosition.Y(),bestCandidate.hits[1].globalPosition.Y(),bestCandidate.hits[2].globalPosition.Y()};
    Double_t hitPosZ[3] = {bestCandidate.hits[0].globalPosition.Z(),bestCandidate.hits[1].globalPosition.Z(),bestCandidate.hits[2].globalPosition.Z()};

    

    track->SetHitId(bestCandidate.nHits);
    track->SetHitPositionX(hitPosX);
    track->SetHitPositionY(hitPosY);
    track->SetHitPositionZ(hitPosZ);
    track->SetModules(bestCandidate.modules);
    track->SetResidualsX(bestCandidate.residualsX);
    track->SetResidualsY(bestCandidate.residualsY);
    track->SetParsX(bestCandidate.parsX);
    track->SetParsY(bestCandidate.parsY);
    track->SetChiSquare(bestCandidate.chiSquare);
}

void StandTracksProducer::OpenInputOutputFiles()
{
    cout << "-I-<StandTracksProducer::OpenInputOutputFiles>" << endl;

    ConfigureInput();
    ConfigureOutput();

    fIOManager->Init();

    GetInputData();
    GetOutputData();
}

void StandTracksProducer::ConfigureInput()
{
    cout << "-I-<StandTracksProducer::ConfigureInput>" << endl;

    fIOManager->SetInputFileName(fInputFileName);
    fIOManager->RegisterInputBranch("SiliconHits", "StandSiliconHit");
}

void StandTracksProducer::ConfigureOutput()
{
    cout << "-I-<StandTracksProducer::ConfigureOutput> Name: "<< fOutputFileName << endl;
    fIOManager->SetOutputFileName(fOutputFileName);

    fIOManager->RegisterOutputBranch("SiliconTracks", "StandSiliconTrack");
}

void StandTracksProducer::GetInputData()
{
    fSiliconHitsArray = fIOManager->GetInputDataArray("StandSiliconHit");
}

void StandTracksProducer::GetOutputData()
{
    fSiliconTracksArray = fIOManager->GetOutputDataArray("StandSiliconTrack");
}

void StandTracksProducer::CloseInputOutputFiles()
{
    cout << "-I-<StandTracksProducer::CloseInputOutputFiles>" << endl;
    fIOManager->Finish();
}