#ifndef STANDTRACKSPRODUCER_H
#define STANDTRACKSPRODUCER_H

#include <TObject.h>
#include <TVector3.h>

#include "StandIOManager.h"
#include "StandSiliconHit.h"

#include <vector>

using std::vector;

const TString kINPUTTREENAME = "cbmsim";

struct HitWrapper
{
    HitWrapper(Int_t hitNumber, Int_t hitStation)
    {

        station = hitStation;
        nHit = hitNumber;
    }
    Int_t nHit;
    Int_t station;
    Int_t module;
    TVector3 globalPosition;
};

struct TrackCandidate
{
    vector<HitWrapper> hits;
    Int_t nHits[3];
    Int_t modules[3];
    Double_t parsX[2];
    Double_t parsY[2];
    Double_t chiSquare[2];
    Double_t residualsX[3];
    Double_t residualsY[3];
};

class StandTracksProducer : public TObject
{
public:
    StandTracksProducer();
    ~StandTracksProducer();
    
    void SetInputFileName(TString inputFileName) { fInputFileName = inputFileName; }
    void SetOutputFileName(TString outputFileName) { fOutputFileName = outputFileName; }

    void ProduceTracksFromAllEvents();
    void ProduceTracksFromOneEvent(Int_t iEvent);

private:
    void OpenInputOutputFiles();
    void CloseInputOutputFiles();
    void ConfigureInput();
    void ConfigureOutput();

    void GetInputData();
    void GetOutputData();

    void ProduceTracksFromCurrentEvent();
    void ProduceTracksFromEvents(Int_t startEvent, Int_t endEvent);

    void FindTrackCandidates(vector<TrackCandidate> &trackCandidates);
    void FitTracks(vector<TrackCandidate> &trackCandidates);
    void FitTrack(TrackCandidate &trackCandidate, Int_t excludeStation = -1);
    Int_t FindBestTrack(vector<TrackCandidate> &trackCandidates);
    void RefitTrackExcludingStation(TrackCandidate &trackCandidate, Int_t excludedStation);
    void WriteTrackIntoTree(TrackCandidate &trackCandidate);

    StandIOManager* fIOManager;

    TString fInputFileName;
    TString fOutputFileName;

    TClonesArray* fSiliconHitsArray;

    TClonesArray* fSiliconTracksArray;

    ClassDef(StandTracksProducer, 1)
};

#endif // STANDTRACKSPRODUCER_H
