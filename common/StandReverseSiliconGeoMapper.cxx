#include "StandReverseSiliconGeoMapper.h"

Int_t StandReverseSiliconGeoMapper::CalculateStripForLocalCoordinate(Int_t station, Int_t module, Int_t side, Double_t localCoord)
{
    Int_t strip = std::round((localCoord - fFirstStripOffset[side]) / fPitch[side]);

    if (fStripsNumberCorrection[station][module][side])
        strip = fStripsNumberCorrection[station][module][side] - strip;

    return strip;
}

Double_t StandReverseSiliconGeoMapper::CalculateInterStripForLocalCoordinate(Int_t station, Int_t module, Int_t side, Double_t localCoord)
{
    Double_t strip = (localCoord - fFirstStripOffset[side]) / fPitch[side];

    if (fStripsNumberCorrection[station][module][side])
        strip = fStripsNumberCorrection[station][module][side] - strip;

    return strip;
}

std::array<Int_t, 2> StandReverseSiliconGeoMapper::DivideInterStripInTwo(Double_t interStrip, Double_t fixedAmplitude, Double_t &varAmplitude)
{
    Int_t strip1 = std::floor(interStrip);
    Int_t strip2 = std::ceil(interStrip);

    varAmplitude = fixedAmplitude*(interStrip - strip1)/(strip2 - interStrip);
    
    return {strip1, strip2};
}

std::array<Double_t, 2> StandReverseSiliconGeoMapper::CalculateLocalCoordinatesForHit(Int_t station, Int_t module, Double_t globalX, Double_t globalY)
{
    TVector3 hit(globalX, globalY, 0);

    hit -= TVector3(fStationPositionCorrection[station][0], fStationPositionCorrection[station][1], fStationPositionCorrection[station][2]);
    hit -= TVector3(fStationCSToGlobalCS[station][0], fStationCSToGlobalCS[station][1], fStationCSToGlobalCS[station][2]);

    // hit.RotateZ(-fStationsRotation[station]);

    hit -= TVector3(fModuleCSToStationCS[station][module][0], fModuleCSToStationCS[station][module][1], 0);
    hit -= TVector3(fModulePositionCorrection[station][module][0], fModulePositionCorrection[station][module][1], 0);

    Double_t localX = hit.X();
    Double_t localY = hit.Y();

    if (fIfSwitchModuleSideX[station][module])
        localX = localX - fSiModuleWidthX;

    if (fIfSwitchModuleSideY[station][module])
        localY = localY + fSiModuleWidthY[station];

    if (fIfReverseModuleX[station][module])
        localX = -localX;

    if (fIfReverseModuleY[station][module])
        localY = -localY;

    return {localX, localY};
}
