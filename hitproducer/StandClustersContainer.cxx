#include "StandClustersContainer.h"

#include <iostream>

StandClustersContainer::StandClustersContainer()
{
}

StandClustersContainer::~StandClustersContainer()
{
    for (auto &&clustersPair : fClustersMap)
    {
        for (auto &&cluster : clustersPair.second)
        {
            delete cluster;
        }
        
    }
    
}

void StandClustersContainer::AddCluster(StandSiliconCluster* newCluster)
{
    Int_t key = CreateKey(newCluster->GetStation(), newCluster->GetModule(), newCluster->GetSide());
    auto iFoundClustersVector = fClustersMap.find(key);
    if (iFoundClustersVector != fClustersMap.end())
    {
        for (auto &&cluster : iFoundClustersVector->second)
        {
            if (cluster->IsNearby(newCluster))
            {
                cluster->MergeWithCluster(newCluster);
                return;
            }
        }
        iFoundClustersVector->second.push_back(newCluster);
    }
    else
    {
        vector<StandSiliconCluster*> clustersVector;
        clustersVector.push_back(newCluster);
        fClustersMap.insert({key, clustersVector});
    }
}

vector<StandSiliconCluster*> StandClustersContainer::GetClustersVector(Int_t station, Int_t module, Int_t side)
{
    Int_t key = CreateKey(station, module, side);
    auto iFoundClustersVector = fClustersMap.find(key);
    if (iFoundClustersVector != fClustersMap.end())
    {
        // return (*iFoundClustersVector).second;
        return iFoundClustersVector->second;
    }
    else
    {
        throw std::runtime_error("No clusters in this key");
    }
}

Int_t StandClustersContainer::CreateKey(Int_t station, Int_t module, Int_t side)
{
    return station * 100 + module * 10 + side;
}

void StandClustersContainer::Print()
{
    std::cout << "-I-<StandClustersContainer::Print> Clusters in container:"<<std::endl;
    for (auto &&clusters : fClustersMap)
    {
        printf("%03d\t%lu\n", clusters.first, clusters.second.size());
        for (auto &&cluster : clusters.second)
        {
            cluster->Print();
        }
        
    }
    
}
