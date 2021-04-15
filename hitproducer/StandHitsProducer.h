#ifndef STANDHITSPRODUCER_H
#define STANDHITSPRODUCER_H

#include <TObject.h>
#include <TString.h>
#include <TClonesArray.h>

#include "BmnSiliconDigit.h"
#include "StandSiliconCluster.h"
#include "StandClustersContainer.h"
#include "StandIOManager.h"

const TString kINPUTTREENAME = "cbmsim";

class StandHitsProducer : public TObject
{
public:
    StandHitsProducer();
    virtual ~StandHitsProducer();

    void SetInputFileName(TString inputFileName) { fInputFileName = inputFileName; }
    void SetOutputFileName(TString outputFileName) { fOutputFileName = outputFileName; }

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

    void ProcessSiliconDigitsIntoClusters(StandClustersContainer &clustersContainer);
    StandSiliconCluster* CreateSiliconCluster(BmnSiliconDigit* siliconDigit);
    void CalculateLocalCoordinates(StandClustersContainer &clustersContainer);
    void CalculateLocalCoordinate(StandSiliconCluster* siliconCluster);
    void ProcessSiliconClustersIntoHits(StandClustersContainer &clustersContainer);
    void ProcessSiliconClustersIntoHit(StandSiliconCluster* clusterX, StandSiliconCluster* clusterY);

    StandIOManager* fIOManager;

    TString fInputFileName;
    TString fOutputFileName;

    TClonesArray* fInputEventHeader;
    TClonesArray* fSiliconDigitsArray;
    TClonesArray* fStrawDigitsArray;

    TClonesArray* fOutputEventHeader;
    TClonesArray* fSiliconHitsArray; 

    ClassDef(StandHitsProducer, 1)
};
#endif /* STANDHITSPRODUCER_H */
