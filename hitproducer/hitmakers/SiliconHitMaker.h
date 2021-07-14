#ifndef SILICONHITMAKER_H
#define SILICONHITMAKER_H

#include "AbstractHitMaker.h"

#include "BmnSiliconDigit.h"
#include "StandSiliconCluster.h"
#include "StandClustersContainer.h"

#define N_SILICON_STATIONS      3
#define N_SILICON_MODULES       4

class SiliconHitMaker : public AbstractHitMaker
{
private:
    void ProcessDigitsIntoClusters(StandClustersContainer &clustersContainer);
    StandSiliconCluster* CreateCluster(BmnSiliconDigit* digit);
    void CalculateLocalCoordinates(StandClustersContainer &clustersContainer);
    void CalculateLocalCoordinate(StandSiliconCluster* cluster);
    void ProcessClustersIntoHits(StandClustersContainer &clustersContainer);
    void ProcessClustersIntoHit(StandSiliconCluster* clusterX, StandSiliconCluster* clusterY);
    Bool_t IsHitInSensitiveRange(Int_t station, Double_t localY);

    ClassDef(SiliconHitMaker, 1);

public:
    SiliconHitMaker();
    ~SiliconHitMaker();

    void ProduceHitsFromCurrentEvent();
};

#endif /* SILICONHITMAKER_H */
