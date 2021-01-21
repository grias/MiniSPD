#include "StandSiliconCluster.h"

#include <iostream>

Bool_t StripsAreNearby(Int_t strip1, Int_t strip2);

StandSiliconCluster::StandSiliconCluster() : fStation(0),
                                   fModule(0),
                                   fSide(0)
{
}

StandSiliconCluster::StandSiliconCluster(Int_t station, Int_t module, Int_t side) : fStation(station),
                                                                          fModule(module),
                                                                          fSide(side)
{
}

StandSiliconCluster::StandSiliconCluster(Int_t station, Int_t module, Int_t side, Int_t strip, Double_t signal) : fStation(station),
                                                                          fModule(module),
                                                                          fSide(side)
{
    AddStrip(strip, signal);
}


StandSiliconCluster::~StandSiliconCluster()
{
}

void StandSiliconCluster::MergeWithCluster(StandSiliconCluster* cluster)
{
    for (auto &&stripSignalPair : cluster->GetStripSignalPairs())
    {
        AddStrip(stripSignalPair.first, stripSignalPair.second);
    }
}


Bool_t StandSiliconCluster::IsNearby(Int_t newStrip)
{
    for (auto &&stripSignalPair : fStripSignalPairs)
    {
        if (StripsAreNearby(stripSignalPair.first, newStrip))
        {
            return kTRUE;
        }
    }
    return kFALSE;
}

Bool_t StandSiliconCluster::IsNearby(StandSiliconCluster* cluster)
{
    for (auto &&thisPair : fStripSignalPairs)
    {
        for (auto &&otherPair : cluster->GetStripSignalPairs())
        {
            if (StripsAreNearby(thisPair.first, otherPair.first))
            {
                return kTRUE;
            }
        }
    }
    return kFALSE;
}

Double_t StandSiliconCluster::GetAmplitude()
{
    Double_t amplitude = 0;
    for (auto &&pair : fStripSignalPairs)
    {
        amplitude += pair.second;
    }
    return amplitude;
}

void StandSiliconCluster::Print()
{
    std::cout << "-I-<StandSiliconCluster::Print> Station: " 
    << fStation << ", Module: " << fModule << ", Side: " << fSide << ", Strips("<<fStripSignalPairs.size()<<"): ";
    for (auto &&stripSignalPair : fStripSignalPairs)
    {
        std::cout << stripSignalPair.first<<"("<<stripSignalPair.second << ") ";
    }
    std::cout << "LocalCoord: " << fLocalCoordinate << std::endl;
}

Bool_t StripsAreNearby(Int_t strip1, Int_t strip2)
{
    return (abs(strip1 - strip2) <= 1);
}

