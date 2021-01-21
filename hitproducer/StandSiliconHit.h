#ifndef STANDSILICONHIT_H
#define STANDSILICONHIT_H

#include <TObject.h>


class StandSiliconHit : public TObject
{
public:
    StandSiliconHit();
    StandSiliconHit(Int_t station, Int_t module);
    StandSiliconHit(Int_t station, Int_t module, Double_t localX, Double_t localY);
    ~StandSiliconHit();

    Int_t GetStation() { return fStation; }
    Int_t GetModule() { return fModule; }

    Double_t GetLocalX() { return fLocalX; }
    Double_t GetLocalY() { return fLocalY; }

    Double_t GetAmplitudeX() { return fAmplitudeX; }
    Double_t GetAmplitudeY() { return fAmplitudeY; }

    Int_t GetClusterSizeX() { return fClusterX; }
    Int_t GetClusterSizeY() { return fClusterY; }


    void SetLocalCoordinates(Double_t localX, Double_t localY) { fLocalX = localX; fLocalX = localY; }
    void SetAmplitudes(Double_t amplitudeX, Double_t amplitudeY) { fAmplitudeX = amplitudeX; fAmplitudeY = amplitudeY; }
    void SetClustersSizes(Int_t clusterSizeX, Int_t clusterSizeY) { fClusterX = clusterSizeX; fClusterY = clusterSizeY; }

    void SetLocalX(Double_t localX) { fLocalX = localX; }
    void SetLocalY(Double_t localY) { fLocalX = localY; }

    void Print();

private:

    Int_t fStation = -1;
    Int_t fModule = -1;

    Int_t fClusterX = 0;
    Int_t fClusterY = 0;

    Double_t fAmplitudeX = 0.;
    Double_t fAmplitudeY = 0.;
    

    Double_t fLocalX = 0.;
    Double_t fLocalY = 0.;

    ClassDef(StandSiliconHit, 1)
};
#endif // STANDSILICONHIT_H
