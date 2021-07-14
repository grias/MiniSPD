#include "StandTracksProducerX.h"

#include "StandSiliconGeoMapper.h"
#include "StandGemGeoMapper.h"
#include "StandTrack.h"

#include <TLinearFitter.h>

#include <iostream>
#include <cmath>

using std::cerr;
using std::cout;
using std::endl;

StandTracksProducerX::StandTracksProducerX()
{
    fIOManager = new StandIOManager();
}

StandTracksProducerX::~StandTracksProducerX()
{
}

void StandTracksProducerX::ProduceTracksFromAllEvents()
{
    cout << "-I-<StandTracksProducerX::ProduceTracksFromAllEvents>" << endl;

    OpenInputOutputFiles();

    Long64_t nEvents = fIOManager->GetNumberOfInputEvents();
    ProduceTracksFromEvents(0, nEvents - 1);

    CloseInputOutputFiles();
}

void StandTracksProducerX::ProduceTracksFromOneEvent(Int_t event)
{
    cout << "-I-<StandTracksProducerX::ProduceTracksFromOneEvent>" << endl;

    OpenInputOutputFiles();

    ProduceTracksFromEvents(event, event);

    CloseInputOutputFiles();
}

void StandTracksProducerX::ProduceTracksFromEvents(Int_t startEvent, Int_t endEvent)
{
    if (startEvent > endEvent)
    {
        cerr
            << "-E-<StandTracksProducerX::ProduceTracksFromEvents> startEvent = " << startEvent
            << " is more than endEvent = " << endEvent
            << endl;
        return;
    }

    Long64_t nEvents = fIOManager->GetNumberOfInputEvents();
    if (endEvent >= nEvents)
    {
        cerr
            << "-E-<StandTracksProducerX::ProduceTracksFromEvents> endEvent = " << endEvent
            << " is more than events number in file = " << nEvents
            << endl;
        return;
    }

    for (size_t iEvent = startEvent; iEvent <= endEvent; iEvent++)
    {
        // cout << "-I-<StandTracksProducerX::ProduceTracksFromEvents> Processing event " << iEvent << endl;

        fIOManager->StartEvent(iEvent);
        ProduceTracksFromCurrentEvent();
        fIOManager->EndEvent();
    }
}

void StandTracksProducerX::ProduceTracksFromCurrentEvent()
{
    vector<TrackCandidateX> trackCandidates;

    FindTrackCandidates(trackCandidates);

    if (trackCandidates.empty()) return;

    FitTracks(trackCandidates);

    auto bestCandidate = trackCandidates[FindBestTrack(trackCandidates)];

    // Int_t stationToExclude = 1;
    // FitTrack(bestCandidate, stationToExclude);

    WriteTrackIntoTree(bestCandidate);
}

void StandTracksProducerX::FindTrackCandidates(vector<TrackCandidateX> &trackCandidates)
{
    vector<HitWrapper> stationsHits[5] = {vector<HitWrapper>(), vector<HitWrapper>(), vector<HitWrapper>(), vector<HitWrapper>(), vector<HitWrapper>()};

    Int_t nSiliconHits = fSiliconHitsArray->GetEntriesFast();
    if (nSiliconHits > 50) return; // bad events
    if (nSiliconHits != 3) return; // for testing purpose
    
    for (size_t iHit = 0; iHit < nSiliconHits; iHit++)
    {
        auto localSiliconHit = (StandSiliconHit *)fSiliconHitsArray->At(iHit);
        Int_t station = localSiliconHit->GetStation();
        Int_t module = localSiliconHit->GetModule();
        Double_t globalX = localSiliconHit->GetGlobalX();
        Double_t globalY = localSiliconHit->GetGlobalY();
        Double_t globalZ = localSiliconHit->GetGlobalZ();

        HitWrapper hit(iHit, station, kSILICON); 
        hit.module = module;
        hit.globalPosition = TVector3(globalX, globalY, globalZ);

        if (!StandSiliconGeoMapper::fIsActiveModule[station][module]) continue;
        
        stationsHits[hit.station].push_back(hit);
    }

    Int_t nGemHits = fGemHitsArray->GetEntriesFast();
    if (nGemHits > 20) return; // bad events
    if (nGemHits != 2) return; // for testing purpose

    for (size_t iHit = 0; iHit < nGemHits; iHit++)
    {
        auto localGemHit = (StandSiliconHit *)fGemHitsArray->At(iHit);
        Int_t station = localGemHit->GetStation() + 3;
        Int_t module = localGemHit->GetModule();
        Double_t globalX = localGemHit->GetGlobalX();
        Double_t globalY = localGemHit->GetGlobalY();
        Double_t globalZ = localGemHit->GetGlobalZ();

        HitWrapper hit(iHit, station, kGEM); 
        hit.module = module;
        hit.globalPosition = TVector3(globalX, globalY, globalZ);

        if (!StandGemGeoMapper::fIsActiveModule[station - 3]) continue;
        
        stationsHits[hit.station].push_back(hit); // Gem stations are 3 and 4
    }
    
    for (auto &&iHit0 : stationsHits[0])
    for (auto &&iHit1 : stationsHits[1])
    for (auto &&iHit2 : stationsHits[2])
    for (auto &&iHit3 : stationsHits[3])
    for (auto &&iHit4 : stationsHits[4])
    {
        TrackCandidateX candidate;
        candidate.hits[0] = iHit0;
        candidate.hits[1] = iHit1;
        candidate.hits[2] = iHit2;
        candidate.hits[3] = iHit3;
        candidate.hits[4] = iHit4;
        trackCandidates.push_back(candidate);
    }
}

void StandTracksProducerX::FitTracks(vector<TrackCandidateX> &trackCandidates)
{
    for (auto &&trackCandidate : trackCandidates)
    {
        FitTrack(trackCandidate);
    }
}

void StandTracksProducerX::FitTrack(TrackCandidateX &trackCandidate, Int_t excludeStation)
{
    TLinearFitter linFitX(1, "pol1");
    TLinearFitter linFitY(1, "pol1");

    for (size_t iStation = 0; iStation < 5; iStation++)
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
    }
}

void StandTracksProducerX::RefitTrackExcludingStation(TrackCandidateX &trackCandidate, Int_t excludedStation)
{
    FitTrack(trackCandidate, excludedStation);
}

Int_t StandTracksProducerX::FindBestTrack(vector<TrackCandidateX> &trackCandidates)
{
    Int_t nCandidates = trackCandidates.size();
    Int_t nBest = -1;
    Double_t bestChiSquare = 999999999;

    for (size_t iCandidate = 0; iCandidate < nCandidates; iCandidate++)
    {
        TrackCandidateX candidate = trackCandidates[iCandidate];
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

void StandTracksProducerX::WriteTrackIntoTree(TrackCandidateX &trackCandidate)
{
    auto track = new((*fTracksArray)[fTracksArray->GetEntriesFast()]) StandTrack();

    for (size_t iStation = 0; iStation < 5; iStation++)
    {
        track->IncludeStation(iStation);
        track->SetHitId(iStation, trackCandidate.nHits[iStation]);
        track->SetHitPositionX(iStation, trackCandidate.hits[iStation].globalPosition.X());
        track->SetHitPositionY(iStation, trackCandidate.hits[iStation].globalPosition.Y());
        track->SetHitPositionZ(iStation, trackCandidate.hits[iStation].globalPosition.Z());
        track->SetModules(iStation, trackCandidate.modules[iStation]);
        track->SetResidualsX(iStation, trackCandidate.residualsX[iStation]);
        track->SetResidualsY(iStation, trackCandidate.residualsY[iStation]);
    }
    
    track->SetParsX(trackCandidate.parsX);
    track->SetParsY(trackCandidate.parsY);
    track->SetChiSquare(trackCandidate.chiSquare);
}

void StandTracksProducerX::OpenInputOutputFiles()
{
    cout << "-I-<StandTracksProducerX::OpenInputOutputFiles>" << endl;

    ConfigureInput();
    ConfigureOutput();

    fIOManager->Init();

    GetInputData();
    GetOutputData();
}

void StandTracksProducerX::ConfigureInput()
{
    cout << "-I-<StandTracksProducerX::ConfigureInput> Filename: " << fInputFileName << endl;

    fIOManager->SetInputFileName(fInputFileName);
    fIOManager->RegisterInputBranch("SiliconHits", "StandSiliconHit");
    fIOManager->RegisterInputBranch("GemHits", "StandSiliconHit");
    // fIOManager->RegisterInputBranch("GemHitsasdasd", "StandSiliconHit");
}

void StandTracksProducerX::ConfigureOutput()
{
    cout << "-I-<StandTracksProducerX::ConfigureOutput> Filename: "<< fOutputFileName << endl;
    fIOManager->SetOutputFileName(fOutputFileName);

    fIOManager->RegisterOutputBranch("Tracks", "StandTrack");
}

void StandTracksProducerX::GetInputData()
{
    fSiliconHitsArray = fIOManager->GetInputDataArray("SiliconHits");
    fGemHitsArray = fIOManager->GetInputDataArray("GemHits");
}

void StandTracksProducerX::GetOutputData()
{
    fTracksArray = fIOManager->GetOutputDataArray("Tracks");
}

void StandTracksProducerX::CloseInputOutputFiles()
{
    cout << "-I-<StandTracksProducerX::CloseInputOutputFiles>" << endl;
    fIOManager->Finish();
}