#include "GemHitMaker.h"

#include "StandSiliconHit.h"
#include "StandGemGeoMapper.h"

#include "TVector3.h"

GemHitMaker::GemHitMaker():AbstractHitMaker("GEM", "GemHits", "BmnGemStripDigit", "StandSiliconHit")
{
}

GemHitMaker::~GemHitMaker()
{
}

void GemHitMaker::ProduceHitsFromCurrentEvent()
{
    StandClustersContainer clustersContainer;
    ProcessDigitsIntoClusters(clustersContainer);
    CalculateLocalCoordinates(clustersContainer);
    ProcessClustersIntoHits(clustersContainer);
}

void GemHitMaker::ProcessDigitsIntoClusters(StandClustersContainer &clustersContainer)
{
    UInt_t nDigits = fDigitsArray->GetEntriesFast();

    for (size_t iDigit = 0; iDigit < nDigits; iDigit++)
    {
        auto digit = (BmnGemStripDigit *)fDigitsArray->At(iDigit);

        if (!digit->IsGoodDigit())
            continue;

        if (IsStripInTriggerZone(digit->GetStripNumber()))
            continue; // optimization

        StandSiliconCluster* cluster = CreateCluster(digit);

        clustersContainer.AddCluster(cluster);
    }
}

StandSiliconCluster* GemHitMaker::CreateCluster(BmnGemStripDigit *digit)
{
    Int_t station = digit->GetStation();
    Int_t side = digit->GetStripLayer();
    Int_t strip = digit->GetStripNumber();
    Double_t signal = digit->GetStripSignal();

    return new StandSiliconCluster(station, 0, side, strip, signal);
}

void GemHitMaker::CalculateLocalCoordinates(StandClustersContainer &clustersContainer)
{
    for (auto &&clustersVector : clustersContainer.GetClustersMap())
    for (auto &&cluster : clustersVector.second)
    {
        CalculateLocalCoordinate(cluster);
    }
}

void GemHitMaker::CalculateLocalCoordinate(StandSiliconCluster* cluster)
{
    Int_t station = cluster->GetStation();
    Int_t module = cluster->GetModule();
    Int_t side = cluster->GetSide();
    vector<pair<Double_t, Double_t>> coordinateAmplitudePairs;
    for (auto &&stripSignalPair : cluster->GetStripSignalPairs())
    {
        Double_t coordinate = StandGemGeoMapper::CalculateLocalCoordinateForStrip(station, module, side, stripSignalPair.first);

        coordinateAmplitudePairs.push_back({coordinate, stripSignalPair.second});
    }

    Double_t localCoordinate = CalculateCenterOfMass(coordinateAmplitudePairs);

    cluster->SetLocalCoordinate(localCoordinate);
}

void GemHitMaker::ProcessClustersIntoHits(StandClustersContainer &clustersContainer)
{
    for (size_t iStation = 0; iStation < GEM_N_STATIONS; iStation++)
    for (size_t iModule = 0; iModule < GEM_N_MODULES; iModule++)
    {
        vector<StandSiliconCluster*> clustersSideX, clustersSideY;
        try
        {
            clustersSideX = clustersContainer.GetClustersVector(iStation, iModule, 0);
            clustersSideY = clustersContainer.GetClustersVector(iStation, iModule, 1);
        }
        catch(const std::exception& e)
        {
            continue;
        }

        for (auto &&clusterX : clustersSideX)
        {
            if (clusterX->GetClusterSize() > GEM_MAX_CLUSTER_SIZE)
                continue;

            for (auto &&clusterY : clustersSideY)
            {
                if (clusterY->GetClusterSize() > GEM_MAX_CLUSTER_SIZE)
                    continue;
            
                ProcessClustersIntoHit(clusterX, clusterY);
            }
        }
    }
}

void GemHitMaker::ProcessClustersIntoHit(StandSiliconCluster* clusterX, StandSiliconCluster* clusterY)
{
    Int_t station = clusterX->GetStation();
    Int_t module = clusterX->GetModule();
    Double_t localX = clusterX->GetLocalCoordinate();
    Double_t localY = StandGemGeoMapper::CalculateLocalY(localX, clusterY->GetLocalCoordinate(), station);

    if (!IsHitInSensitiveRange(localY)) return;

    StandSiliconHit* hit = new((*fHitsArray)[fHitsArray->GetEntriesFast()]) StandSiliconHit(station, module, localX, localY);
    hit->SetAmplitudes(clusterX->GetAmplitude(), clusterY->GetAmplitude());
    hit->SetClustersSizes(clusterX->GetClusterSize(), clusterY->GetClusterSize());
    TVector3 globalHitPos = StandGemGeoMapper::CalculateGlobalCoordinatesForHit(station, module, localX, localY);
    hit->SetGlobalCoordinates(globalHitPos.X(), globalHitPos.Y(), globalHitPos.Z());
}

Double_t GemHitMaker::CalculateCenterOfMass(vector<pair<Double_t, Double_t>> coordinateAmplitudePairs)
{
    Double_t coordAmpSum = 0;
    Double_t amplitudeSum = 0;
    for (auto &&coordAmpPair : coordinateAmplitudePairs)
    {
        coordAmpSum += coordAmpPair.first * coordAmpPair.second;
        amplitudeSum += coordAmpPair.second;
    }
    return coordAmpSum / amplitudeSum;
}

Bool_t GemHitMaker::IsStripInTriggerZone(Int_t stripNumber)
{ 
    return (stripNumber < GEM_TRIGGER_ZONE_MIN_STRIP || stripNumber > GEM_TRIGGER_ZONE_MAX_STRIP);
}

Bool_t GemHitMaker::IsHitInSensitiveRange(Double_t localY)
{
    return StandGemGeoMapper::IsInSensitiveRange(localY);
}
    