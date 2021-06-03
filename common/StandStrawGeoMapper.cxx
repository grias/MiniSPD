#include "StandStrawGeoMapper.h"

Double_t StandStrawGeoMapper::fFirstWirePosX[2][2] = 
{
    {-fDistanceBtwWiresX/2., 0},
    {-fDistanceBtwWiresX/2., 0}
};

Double_t StandStrawGeoMapper::fWirePosZ[2][2] = 
{
    {fDistanceBtwWiresZ, 0},
    {fDistanceBtwStations, fDistanceBtwWiresZ + fDistanceBtwStations}
};

Double_t StandStrawGeoMapper::fStationPositionCorrectionX[2] {0.0, 0.0};

std::array<Double_t, 2> StandStrawGeoMapper::GetWireGlobalPositionZX(Int_t plane, Int_t wire)
{
    if (plane == 0) 
    {
        wire = 63 - wire;
    }

    Int_t isWireOdd = wire % 2;

    Double_t posX = fFirstWirePosX[plane][isWireOdd] + fDistanceBtwWiresX*( wire/2 );
    posX  += fStationPositionCorrectionX[plane] + fPositionCorrectionX;
    Double_t posZ = fWirePosZ[plane][isWireOdd] + fPositionCorrectionZ;
    
    return {posZ, posX};
}

