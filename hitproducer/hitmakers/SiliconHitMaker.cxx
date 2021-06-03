#include "SiliconHitMaker.h"

#include "StandSiliconHit.h"
#include "StandSiliconGeoMapper.h"

SiliconHitMaker::SiliconHitMaker():AbstractHitMaker("SILICON", "SiliconHits", "BmnSiliconDigit", "StandSiliconHit")
{
}

SiliconHitMaker::~SiliconHitMaker()
{
}

void SiliconHitMaker::ProduceHitsFromCurrentEvent()
{
    StandClustersContainer clustersContainer;
    ProcessDigitsIntoClusters(clustersContainer);
    CalculateLocalCoordinates(clustersContainer);
    ProcessClustersIntoHits(clustersContainer);
}

void SiliconHitMaker::ProcessDigitsIntoClusters(StandClustersContainer &clustersContainer)
{
    Int_t nDigits = fDigitsArray->GetEntriesFast();
    // cout << "-I-<SiliconHitMaker::ProcessDigitsIntoClusters> Total digits: " << nDigits << endl;

    for (size_t iDigit = 0; iDigit < nDigits; iDigit++)
    {
        auto digit = (BmnSiliconDigit *)fDigitsArray->At(iDigit);
        if (!digit->IsGoodDigit())
            continue;

        StandSiliconCluster* cluster = CreateCluster(digit);

        clustersContainer.AddCluster(cluster);
    }
    // clustersContainer.Print();
}

StandSiliconCluster* SiliconHitMaker::CreateCluster(BmnSiliconDigit *digit)
{
    Int_t station = digit->GetStation();
    Int_t module = digit->GetModule();
    Int_t side = digit->GetStripLayer();
    Int_t strip = digit->GetStripNumber();
    Double_t signal = digit->GetStripSignal();

    StandSiliconCluster* cluster = new StandSiliconCluster(station, module, side, strip, signal);

    // printf("-I-<SiliconHitMaker::CreateCluster> Station: %d, module: %d, side: %d, strip: %d, signal: %f\n",
    //        station, module, side, strip, signal);

    return cluster;
}

void SiliconHitMaker::CalculateLocalCoordinates(StandClustersContainer &clustersContainer)
{
    for (auto &&clustersVector : clustersContainer.GetClustersMap())
    for (auto &&cluster : clustersVector.second)
    {
        CalculateLocalCoordinate(cluster);
        // cluster->Print();
    }
}

void SiliconHitMaker::CalculateLocalCoordinate(StandSiliconCluster* cluster)
{
    Int_t station = cluster->GetStation();
    Int_t module = cluster->GetModule();
    Int_t side = cluster->GetSide();
    vector<pair<Double_t, Double_t>> coordinateAmplitudePairs;
    for (auto &&stripSignalPair : cluster->GetStripSignalPairs())
    {
        Double_t coordinate = StandSiliconGeoMapper::CalculateLocalCoordinateForStrip(station, module, side, stripSignalPair.first);
        // std::cout<<"Strip: "<<stripSignalPair.first<<" Coordinate: "<<coordinate<<std::endl;
        coordinateAmplitudePairs.push_back({coordinate, stripSignalPair.second});
    }

    // calculate center of mass
    Double_t coordAmpSum = 0;
    Double_t amplitudeSum = 0;
    for (auto &&coordAmpPair : coordinateAmplitudePairs)
    {
        coordAmpSum += coordAmpPair.first * coordAmpPair.second;
        amplitudeSum += coordAmpPair.second;
    }
    Double_t localCoordinate = coordAmpSum / amplitudeSum;
    cluster->SetLocalCoordinate(localCoordinate);
}

void SiliconHitMaker::ProcessClustersIntoHits(StandClustersContainer &clustersContainer)
{
    // cout<<"-I-<SiliconHitMaker::ProcessClustersIntoHits>"<<endl;

    for (size_t iStation = 0; iStation < N_SILICON_STATIONS; iStation++)
    for (size_t iModule = 0; iModule < N_SILICON_MODULES; iModule++)
    {
        vector<StandSiliconCluster*> clustersSideX;
        vector<StandSiliconCluster*> clustersSideY;
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
            if (clusterX->GetClusterSize() > 4)
                continue;

            for (auto &&clusterY : clustersSideY)
            {
                if (clusterY->GetClusterSize() > 4)
                    continue;
            
                ProcessClustersIntoHit(clusterX, clusterY);
            }
        }
    }
}

void SiliconHitMaker::ProcessClustersIntoHit(StandSiliconCluster* clusterX, StandSiliconCluster* clusterY)
{
    Int_t station = clusterX->GetStation();
    Int_t module = clusterX->GetModule();
    Double_t localX = clusterX->GetLocalCoordinate();
    Double_t localY = StandSiliconGeoMapper::CalculateLocalY(localX, clusterY->GetLocalCoordinate(), station);
    // std::cout<<clusterY->GetLocalCoordinate() - clusterX->GetLocalCoordinate()<<"\t"<<localY<<std::endl;

    if (StandSiliconGeoMapper::IsInSensitiveRange(clusterX->GetStation(), localY))
    {
        StandSiliconHit* hit = new((*fHitsArray)[fHitsArray->GetEntriesFast()]) StandSiliconHit(station, module, localX, localY);
        hit->SetAmplitudes(clusterX->GetAmplitude(), clusterY->GetAmplitude());
        hit->SetClustersSizes(clusterX->GetClusterSize(), clusterY->GetClusterSize());
        // hit->Print();
    }
}