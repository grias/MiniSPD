#ifndef GEMHITMAKER_H
#define GEMHITMAKER_H

#include "AbstractHitMaker.h"

#include "BmnGemStripDigit.h"
#include "StandSiliconCluster.h"
#include "StandClustersContainer.h"

#define GEM_N_STATIONS                  2
#define GEM_N_MODULES                   1
#define GEM_MAX_CLUSTER_SIZE            3
#define GEM_TRIGGER_ZONE_MIN_STRIP    200
#define GEM_TRIGGER_ZONE_MAX_STRIP    500

class GemHitMaker : public AbstractHitMaker
{
private:
    void ProcessDigitsIntoClusters(StandClustersContainer &clustersContainer);
    StandSiliconCluster* CreateCluster(BmnGemStripDigit* digit);
    void CalculateLocalCoordinates(StandClustersContainer &clustersContainer);
    void CalculateLocalCoordinate(StandSiliconCluster* cluster);
    void ProcessClustersIntoHits(StandClustersContainer &clustersContainer);
    void ProcessClustersIntoHit(StandSiliconCluster* clusterX, StandSiliconCluster* clusterY);
    Double_t CalculateCenterOfMass(vector<pair<Double_t, Double_t>> coordinateAmplitudePairs);

    Bool_t IsStripInTriggerZone(Int_t stripNumber);
    Bool_t IsHitInSensitiveRange(Double_t localY);

public:
    GemHitMaker();
    ~GemHitMaker();

    void ProduceHitsFromCurrentEvent();
};

#endif /* GEMHITMAKER_H */
