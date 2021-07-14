#ifndef STANDTRACKSPRODUCERX_H
#define STANDTRACKSPRODUCERX_H

#include <TObject.h>
#include <TVector3.h>

#include "StandIOManager.h"
#include "StandSiliconHit.h"
#include "StandTracksProducer.h"

#include <array>
#include <vector>

using std::vector;

struct TrackCandidateX
{
    TrackCandidateX() {
        for (size_t iStation = 0; iStation < 5; iStation++)
        {
            isHitPresent[iStation] = 0;
        }
    }
    std::array<Bool_t, 5> isHitPresent;
    std::array<HitWrapper, 5> hits;
    Int_t nHits[5];
    Int_t modules[5];
    Double_t parsX[2];
    Double_t parsY[2];
    Double_t chiSquare[2];
    Double_t residualsX[5];
    Double_t residualsY[5];
};

class StandTracksProducerX : public TObject
{
public:
    StandTracksProducerX();
    ~StandTracksProducerX();
    
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

    void FindTrackCandidates(vector<TrackCandidateX> &trackCandidates);
    void FitTracks(vector<TrackCandidateX> &trackCandidates);
    void FitTrack(TrackCandidateX &trackCandidate, Int_t excludeStation = -1);
    Int_t FindBestTrack(vector<TrackCandidateX> &trackCandidates);
    void RefitTrackExcludingStation(TrackCandidateX &trackCandidate, Int_t excludedStation);
    void WriteTrackIntoTree(TrackCandidateX &trackCandidate);

    StandIOManager* fIOManager;

    TString fInputFileName;
    TString fOutputFileName;

    TClonesArray* fSiliconHitsArray;
    TClonesArray* fGemHitsArray;

    TClonesArray* fTracksArray;

    ClassDef(StandTracksProducerX, 1)
};

#endif // STANDTRACKSPRODUCERX_H
