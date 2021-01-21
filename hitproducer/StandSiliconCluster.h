#ifndef STANDSILICONCLUSTER_H
#define STANDSILICONCLUSTER_H

#include <TObject.h>
#include <set>

class StandSiliconCluster : public TObject
{
public:
    StandSiliconCluster();
    StandSiliconCluster(Int_t station, Int_t module, Int_t side);
    StandSiliconCluster(Int_t station, Int_t module, Int_t side, Int_t strip);
    StandSiliconCluster(Int_t station, Int_t module, Int_t side, Int_t strip, Double_t signal);
    ~StandSiliconCluster();

    std::set<std::pair<Int_t, Double_t>> GetStripSignalPairs() { return fStripSignalPairs; }
    Int_t GetStation() { return fStation; }
    Int_t GetModule() { return fModule; }
    Int_t GetSide() { return fSide; }
    Double_t GetLocalCoordinate() { return fLocalCoordinate; }
    Int_t GetClusterSize() { return fStripSignalPairs.size(); }
    Double_t GetAmplitude();

    

    void SetLocalCoordinate(Double_t coordinate) { fLocalCoordinate = coordinate; }

    void AddStrip(Int_t strip, Double_t signal) { fStripSignalPairs.insert({strip, signal}); }
    void MergeWithCluster(StandSiliconCluster* cluster);
    Bool_t IsNearby(Int_t strip);
    Bool_t IsNearby(StandSiliconCluster* cluster);
    Bool_t IsEmpty() { return fStripSignalPairs.empty(); }

    void Print();

private:
    std::set<std::pair<Int_t, Double_t>> fStripSignalPairs;
    Int_t fStation;
    Int_t fModule;
    Int_t fSide;

    Double_t fLocalCoordinate = 0;

    ClassDef(StandSiliconCluster, 1)
};

#endif /* STANDSILICONCLUSTER_H */
