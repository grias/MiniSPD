#include "StandSiliconGeoMapper.h"

StandSiliconGeoMapper::StandSiliconGeoMapper()
{
}

StandSiliconGeoMapper::~StandSiliconGeoMapper()
{
}

Double_t StandSiliconGeoMapper::CalculateLocalCoordinateForStrip(Int_t station, Int_t module, Int_t side, Int_t strip)
{

    Int_t fStripsNumberCorrection[3][4][2] = 
    {
        {{0, 639},{0, 639},{0, 639},{0, 639}},
        {{0, 613},{0, 613},{639, 0},{639, 0}},
        {{0, 639},{0, 639},{0, 639},{0, 639}}
    };

    Double_t fPitch[2] = {0.095, 0.103};
    Double_t fFirstStripOffset[2] = {1.148, 1.287};

    if (fStripsNumberCorrection[station][module][side])
    {
        strip = fStripsNumberCorrection[station][module][side] - strip;
    }
    return fPitch[side]*strip + fFirstStripOffset[side];
}

Double_t StandSiliconGeoMapper::CalculateLocalY(Double_t localX, Double_t stripOffsetY)
{
    return ((stripOffsetY - localX)/fTangentOfStripsYSlope);
}

Bool_t StandSiliconGeoMapper::IsInSensitiveRange(Int_t station, Double_t localY)
{
    if (station == 1)
    {
        if (localY >= 1.148 && localY <=61.853)
        {
            return kTRUE;
        }
    }
    else
    {
        if (localY >= 1.148 && localY <=124.861)
        {
            return kTRUE;
        }
    }
    
    return kFALSE;
}
