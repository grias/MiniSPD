#include "StandGemGeoMapper.h"

#include <cmath>

Double_t StandGemGeoMapper::fPitch[2] = {0.8, 0.8};

Double_t StandGemGeoMapper::fFirstStripOffset[2] = {0, 0.26};

Int_t StandGemGeoMapper::fIfReverseModuleX[2] = {1, 0};
Int_t StandGemGeoMapper::fIfReverseModuleY[2] = {1, 1};

Int_t StandGemGeoMapper::fIsActiveModule[2] = {1, 1};

Double_t StandGemGeoMapper::fStationsRotation[2] = {0.0, 0.0}; /* deg */

Double_t StandGemGeoMapper::fStationCSToGlobalCS[2][3] =
{
    {366.91, -8.86, 389.17},
    {-181.0, 21.73, -387.37}
}; /* mm */

Double_t StandGemGeoMapper::fStationPositionCorrection[2][3] =
{
    {0, 0, 0},
    {0, 0, 0}
}; /* mm */

Double_t StandGemGeoMapper::CalculateLocalCoordinateForStrip(Int_t station, Int_t module, Int_t side, Int_t strip)
{
    return fPitch[side]*strip + fFirstStripOffset[side];
    // return fPitch[side]*strip + fFirstStripOffset[side] + fPitch[side]/2.;
}

TVector3 StandGemGeoMapper::CalculateGlobalCoordinatesForHit(Int_t station, Int_t module, Double_t localX, Double_t localY)
{
    if (fIfReverseModuleX[station])
        localX = -localX;

    if (fIfReverseModuleY[station])
        localY = -localY;

    TVector3 hit(localX, localY, 0);

    hit.RotateZ(fStationsRotation[station]*degToRad);

    hit += TVector3(fStationCSToGlobalCS[station][0], fStationCSToGlobalCS[station][1], fStationCSToGlobalCS[station][2]);
    hit += TVector3(fStationPositionCorrection[station][0], fStationPositionCorrection[station][1], fStationPositionCorrection[station][2]);

    return hit;
}

Double_t StandGemGeoMapper::CalculateLocalY(Double_t localX, Double_t stripOffsetY, Int_t station)
{
    Double_t localY = ((stripOffsetY - localX)/fTangentOfStripsYSlope);

    return localY;
}

Bool_t StandGemGeoMapper::IsInSensitiveRange(Double_t localY)
{
    if (localY > fLowerSensetiveBoundary && localY < fUpperSensetiveBoundary)
        return kTRUE;

    return kFALSE;
    // return kTRUE;
}
