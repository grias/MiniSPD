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

void StandTracksProducer::ProduceTracksFromAllEvents()
{
    cout << "-I-<StandTracksProducer::ProduceTracksFromAllEvents>" << endl;

    OpenInputOutputFiles();

    Long64_t nEvents = fIOManager->GetNumberOfInputEvents();
    ProduceTracksFromEvents(0, nEvents - 1);

    CloseInputOutputFiles();
}

void StandTracksProducer::ProduceTracksFromOneEvent(Int_t event)
{
    cout << "-I-<StandTracksProducer::ProduceTracksFromOneEvent>" << endl;

    OpenInputOutputFiles();

    ProduceTracksFromEvents(event, event);

    CloseInputOutputFiles();
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
        fIOManager->StartEvent(iEvent);

        ProduceTracksFromCurrentEvent();

        fIOManager->EndEvent();
    }
}

void StandTracksProducer::ProduceTracksFromCurrentEvent()
{
    // cout << "-I-<StandTracksProducer::ProduceTracksFromCurrentEvent> Processing event " << event << endl;

    vector<TrackCandidate> trackCandidates;

    FindTrackCandidates(trackCandidates);

    if (trackCandidates.size())
    {
        FitTracks(trackCandidates);

        auto bestCandidate = trackCandidates[FindBestTrack(trackCandidates)];

        // Int_t stationToExclude = 1;
        // FitTrack(bestCandidate, stationToExclude);

        WriteTrackIntoTree(bestCandidate);
    }
}

void StandTracksProducer::FindTrackCandidates(vector<TrackCandidate> &trackCandidates)
{
    vector<HitWrapper> stationsHits[3] = {vector<HitWrapper>(), vector<HitWrapper>(), vector<HitWrapper>()};

    Int_t nHits = fSiliconHitsArray->GetEntriesFast();
    if (nHits > 50) return; // bad events
    // if (nHits > 3) return;
    
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

        Int_t isActiveModule[3][4] = 
        {
            {1, 1},
            {1, 1, 1, 1},
            {1, 1}
        };

        if (!isActiveModule[station][module]) continue;
        
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

void StandTracksProducer::FitTrack(TrackCandidate &trackCandidate, Int_t excludeStation)
{
    TLinearFitter linFitX(1, "pol1");
    TLinearFitter linFitY(1, "pol1");

    for (size_t iStation = 0; iStation < 3; iStation++)
    {
        if (iStation == excludeStation) continue;
        auto hit = trackCandidate.hits[iStation];

        Double_t x = hit.globalPosition.X();
        Double_t y = hit.globalPosition.Y();
        Double_t z[1] = {hit.globalPosition.Z()};

        linFitX.AddPoint(z, x);
        linFitY.AddPoint(z, y);
    }

    // for (auto &&hit : trackCandidate.hits)
    // {
    //     Double_t x = hit.globalPosition.X();
    //     Double_t y = hit.globalPosition.Y();
    //     Double_t z[1] = {hit.globalPosition.Z()};

    //     linFitX.AddPoint(z, x);
    //     linFitY.AddPoint(z, y);
    // }

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

    if (excludeStation == -1)
    {
        trackCandidate.chiSquare[0] = linFitX.GetChisquare();
        trackCandidate.chiSquare[1] = linFitY.GetChisquare();
    }

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

void StandTracksProducer::RefitTrackExcludingStation(TrackCandidate &trackCandidate, Int_t excludedStation)
{
    FitTrack(trackCandidate, excludedStation);
}

Int_t StandTracksProducer::FindBestTrack(vector<TrackCandidate> &trackCandidates)
{
    Int_t nCandidates = trackCandidates.size();
    Int_t nBest = -1;
    Double_t bestChiSquare = 999999999;

    for (size_t iCandidate = 0; iCandidate < nCandidates; iCandidate++)
    {
        TrackCandidate candidate = trackCandidates[iCandidate];
        // Double_t candidateChiSquare = 
        //     std::sqrt(candidate.chiSquare[0]*candidate.chiSquare[0] + candidate.chiSquare[1]*candidate.chiSquare[1]);

        Double_t candidateChiSquare = candidate.chiSquare[0]; // for now only X counts

        if (candidateChiSquare <= bestChiSquare)
        {
            nBest = iCandidate;
        }
    }
    
    return nBest;
}

void StandTracksProducer::WriteTrackIntoTree(TrackCandidate &trackCandidate)
{
    StandSiliconTrack* track = new((*fSiliconTracksArray)[fSiliconTracksArray->GetEntriesFast()]) StandSiliconTrack();

    Double_t hitPosX[3] = {trackCandidate.hits[0].globalPosition.X(),trackCandidate.hits[1].globalPosition.X(),trackCandidate.hits[2].globalPosition.X()};
    Double_t hitPosY[3] = {trackCandidate.hits[0].globalPosition.Y(),trackCandidate.hits[1].globalPosition.Y(),trackCandidate.hits[2].globalPosition.Y()};
    Double_t hitPosZ[3] = {trackCandidate.hits[0].globalPosition.Z(),trackCandidate.hits[1].globalPosition.Z(),trackCandidate.hits[2].globalPosition.Z()};

    track->SetHitId(trackCandidate.nHits);
    track->SetHitPositionX(hitPosX);
    track->SetHitPositionY(hitPosY);
    track->SetHitPositionZ(hitPosZ);
    track->SetModules(trackCandidate.modules);
    track->SetResidualsX(trackCandidate.residualsX);
    track->SetResidualsY(trackCandidate.residualsY);
    track->SetParsX(trackCandidate.parsX);
    track->SetParsY(trackCandidate.parsY);
    track->SetChiSquare(trackCandidate.chiSquare);
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