#ifndef STANDHITSPRODUCER_H
#define STANDHITSPRODUCER_H

#include <TObject.h>
#include <TString.h>
#include <TClonesArray.h>

#include "StandIOManager.h"
#include "hitmakers/AbstractHitMaker.h"

#include <vector>
#include <memory>

const TString kINPUTTREENAME = "cbmsim";

class StandHitsProducer : public TObject
{
public:
    StandHitsProducer();
    virtual ~StandHitsProducer();

    void SetInputFileName(TString inputFileName) { fInputFileName = inputFileName; }
    void SetOutputFileName(TString outputFileName) { fOutputFileName = outputFileName; }

    void AddHitMaker(std::shared_ptr<AbstractHitMaker> hitMaker) { fHitMakersCollection.push_back(hitMaker); }

    void ProduceHitsFromAllEvents();
    void ProduceHitsFromOneEvent(Int_t iEvent);

private:
    void OpenInputOutputFiles();
    void CloseInputOutputFiles();
    void ConfigureInput();
    void ConfigureOutput();

    void GetInputData();
    void GetOutputData();

    void ProduceHitsFromCurrentEvent();
    void ProduceHitsFromEvents(Int_t startEvent, Int_t endEvent);

    StandIOManager* fIOManager;
    std::vector<std::shared_ptr<AbstractHitMaker>> fHitMakersCollection;

    TString fInputFileName;
    TString fOutputFileName;

    ClassDef(StandHitsProducer, 1)
};
#endif /* STANDHITSPRODUCER_H */
