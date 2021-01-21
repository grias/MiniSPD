#ifndef STANDCLUSTERSCONTAINER_H
#define STANDCLUSTERSCONTAINER_H

#include <TObject.h>

#include "StandSiliconCluster.h"

#include <map>
#include <vector>

using std::map;
using std::vector;

class StandClustersContainer : public TObject
{

public:
    StandClustersContainer();
    ~StandClustersContainer();

    void AddCluster(StandSiliconCluster* cluster);
    vector<StandSiliconCluster*> GetClustersVector(Int_t station, Int_t module, Int_t side);

    map<Int_t, vector<StandSiliconCluster*>> GetClustersMap() { return fClustersMap; }

    void Print();

private:

    Int_t CreateKey(Int_t station, Int_t module, Int_t side);

    map<Int_t, vector<StandSiliconCluster*>> fClustersMap;

    ClassDef(StandClustersContainer, 1)
};

#endif /* STANDCLUSTERSCONTAINER_H */
