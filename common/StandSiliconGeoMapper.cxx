#include "StandSiliconGeoMapper.h"

#include <cmath>

ModuleType StandSiliconGeoMapper::fModulesTypes[3] = {MODULE_LONG, MODULE_SHORT, MODULE_LONG};

Int_t StandSiliconGeoMapper::fStripsNumberCorrection[3][4][2] = 
{
    {{639, 0},{639, 0}},
    {{0, 613},{0, 613},{0, 613},{0, 613}},
    {{0, 639},{0, 639}}
};

Double_t StandSiliconGeoMapper::fPitch[2] = {0.095, 0.103};
// Double_t StandSiliconGeoMapper::fFirstStripOffset[2] = {1.148, 1.287};
Double_t StandSiliconGeoMapper::fFirstStripOffset[2] = {1.148, 1.237};

Int_t StandSiliconGeoMapper::fIfReverseModuleX[3][4] = {{1,1},{1,0,1,0},{1,0}};
Int_t StandSiliconGeoMapper::fIfReverseModuleY[3][4] = {{1,1},{0,0,1,1},{0,0}};

Int_t StandSiliconGeoMapper::fIfSwitchModuleSideX[3][4] = {{1,1},{1,0,1,0},{1,0}};
Int_t StandSiliconGeoMapper::fIfSwitchModuleSideY[3][4] = {{0,0},{1,1,0,0},{1,1}};
Double_t StandSiliconGeoMapper::fSiModuleWidthX = 63.; /* mm */
Double_t StandSiliconGeoMapper::fSiModuleWidthY[3] = {126., 63., 126.}; /* mm */

Double_t StandSiliconGeoMapper::fModuleCSToStationCS[3][4][3] = 
{
    {{60., 0.,-15.3}, {0., 0., 0}},
    {{60., -5., 7.3}, {0., 0., 0}, {55., -65., 0}, {-5., -60., 7.3}},
    {{60., -5., 7.3}, {0., 0., 0}}
}; /* mm */

// // GOOD
// Double_t StandSiliconGeoMapper::fModulePositionCorrection[3][4][3] =
// {
//     {{0, 0, 0},{-0.115, 0.080, 0}},
//     {{-0.133, 2.568, 0},{-0.232, 2.7, 0},{-0.103, -9.030, 0},{-0.220, -9.107, 0}},
//     {{0.000, 0, 0},{+0.200, 0, 0}}
// }; /* mm */

// parts
Double_t StandSiliconGeoMapper::fModulePositionCorrection[3][4][3] =
{
    {{0, 0, 0},{-0.115, 0.080, 0}},
    {{-0.133, 2.568, 0},{-0.225, 2.7, 0},{-0.090, -9.045, 0},{-0.216, -9.107, 0}},
    {{0.000, 0, 0},{+0.200, 0, 0}}
}; /* mm */

Int_t StandSiliconGeoMapper::fIsActiveModule[3][4] = 
// {{1, 0},{1, 0, 1, 0},{1, 0}}; // part1
{{0, 1},{0, 1, 0, 1},{0, 1}}; // part2
// {{1, 1},{1, 1, 1, 1},{1, 1}}; // All


Double_t StandSiliconGeoMapper::fModuleRotationCorrection[3][4][3] = 
{
    {{0, 0, 0},{0, 0, 0}},
    {{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0}},
    {{0, 0, 0},{0, 0, 0}}
}; /* deg */

Double_t StandSiliconGeoMapper::fStationsRotation[3] = 
// {-0.001745, -0.001745, -0.002443}; /* rad */ // good
{0.0, 0.0, 0.0}; /* rad */
// {0.0872665, 0.0, 0.0}; /* rad */
// {-0.0872665, 0.0, 0.0}; /* rad */

Double_t StandSiliconGeoMapper::fStationCSToGlobalCS[3][3] =
{
    {37.01, -27.99, 605.52},
    {36.85, -28.17, 245.42},
    {37.03, -28.17, -252.98}
}; /* mm */

Double_t StandSiliconGeoMapper::fStationPositionCorrection[3][3] =
{
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}
}; /* mm */

Double_t StandSiliconGeoMapper::CalculateLocalCoordinateForStrip(Int_t station, Int_t module, Int_t side, Int_t strip)
{
    if (fStripsNumberCorrection[station][module][side])
        strip = fStripsNumberCorrection[station][module][side] - strip;

    return fPitch[side]*strip + fFirstStripOffset[side];
    // return fPitch[side]*strip + fFirstStripOffset[side] + fPitch[side]/2.;
}

TVector3 StandSiliconGeoMapper::CalculateGlobalCoordinatesForHit(Int_t station, Int_t module, Double_t localX, Double_t localY)
{
    if (fIfReverseModuleX[station][module])
        localX = -localX;

    if (fIfReverseModuleY[station][module])
        localY = -localY;

    if (fIfSwitchModuleSideX[station][module])
        localX = localX + fSiModuleWidthX;

    if (fIfSwitchModuleSideY[station][module])
        localY = localY - fSiModuleWidthY[station];

    TVector3 hit(localX, localY, 0);

    hit += TVector3(fModuleCSToStationCS[station][module][0], fModuleCSToStationCS[station][module][1], fModuleCSToStationCS[station][module][2]);
    hit += TVector3(fModulePositionCorrection[station][module][0], fModulePositionCorrection[station][module][1], fModulePositionCorrection[station][module][2]);

    hit.RotateZ(fStationsRotation[station]);

    hit += TVector3(fStationCSToGlobalCS[station][0], fStationCSToGlobalCS[station][1], fStationCSToGlobalCS[station][2]);
    hit += TVector3(fStationPositionCorrection[station][0], fStationPositionCorrection[station][1], fStationPositionCorrection[station][2]);

    return hit;
}

Double_t StandSiliconGeoMapper::CalculateLocalY(Double_t localX, Double_t stripOffsetY, Int_t station)
{
    Double_t localY = ((stripOffsetY - localX)/fTangentOfStripsYSlope);
    
    if (fModulesTypes[station] == MODULE_LONG && localY > fUpperSensetiveBoundaryShortModule)
        localY += fClearance;

    return localY;
}

Bool_t StandSiliconGeoMapper::IsInSensitiveRange(Int_t station, Double_t localY)
{
    switch (fModulesTypes[station])
    {
    case MODULE_SHORT:
        if (localY > fLowerSensetiveBoundary && localY < fUpperSensetiveBoundaryShortModule)
            return kTRUE;
        break;

    case MODULE_LONG:
        if (localY > fLowerSensetiveBoundary && localY < fUpperSensetiveBoundaryLongModule)
            return kTRUE;
        break;
    
    default:
        break;
    }

    return kFALSE;
    // return kTRUE;
}

Double_t StandSiliconGeoMapper::CalculateGlobalU(Int_t station, Int_t module, Double_t offsetY)
{
    // Double_t b = offsetY / fTangentOfStripsYSlope;
    // auto globalPoint1 = StandSiliconGeoMapper::CalculateGlobalCoordinatesForHit(station, module, offsetY, 0);
    // auto globalPoint2 = StandSiliconGeoMapper::CalculateGlobalCoordinatesForHit(station, module, 0, b);

    // Double_t x1 = globalPoint1.X();
    // Double_t x2 = globalPoint2.X();
    // Double_t y1 = globalPoint1.Y();
    // Double_t y2 = globalPoint2.Y();

    // auto globalU = TMath::Abs((x2 - x1)*y1 - (y2 - y1) * x1) / TMath::Sqrt( TMath::Sq(x2 - x1) + TMath::Sq(y2 - y1));

    auto globalU = StandSiliconGeoMapper::CalculateGlobalCoordinatesForHit(station, module, offsetY, 0).X() * 0.99904822158; /*cos 2.5 deg*/
    return globalU;
}
