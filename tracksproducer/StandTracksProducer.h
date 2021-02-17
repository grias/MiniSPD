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

    void Init();
    void Finish();

    void ProduceTracksFromAllEvents();
    void ProduceTracksFromOneEvent(Int_t iEvent);

private:
    void OpenInputOutputFiles();
    void CloseInputOutputFiles();
    void ConfigureInput();
    void ConfigureOutput();

    void GetInputData();
    void GetOutputData();

    void ProduceTracksFromEvent(Int_t event);
    void ProduceTracksFromEvents(Int_t startEvent, Int_t endEvent);

    void FindTrackCandidates(vector<TrackCandidate> &trackCandidates);
    void FitTracks(vector<TrackCandidate> &trackCandidates);
    void FitTrack(TrackCandidate &trackCandidate);
    void FindBestTrack(vector<TrackCandidate> &trackCandidates);

    /* data members */
    StandIOManager* fIOManager;

    TString fInputFileName;
    TString fOutputFileName;

    /* Input arrays */
    TClonesArray* fSiliconHitsArray;

    /* Output arrays */
    TClonesArray* fSiliconTracksArray;

    ClassDef(StandTracksProducer, 1)
};

#endif // STANDTRACKSPRODUCER_H
