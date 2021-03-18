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

TVector3 StandSiliconGeoMapper::CalculateGlobalCoordinatesForHit(Int_t station, Int_t module, Int_t localX, Int_t localY)
{
    const Double_t degToRad = 3.14159265359 / 180.;

    Int_t fIfReverseModuleX[3][4] = {{0,0,0,0},{1,0,0,1},{1,0,0,0}};
    Int_t fIfReverseModuleY[3][4] = {{0,0,0,0},{0,0,0,0},{0,0,0,0}};
    Double_t fSiModuleWidthX = 63.; /* mm */
    Double_t fSiModuleWidthY[3] = {126., 63., 126.}; /* mm */

    Double_t fModuleRotationCorrection[3][4][3] = 
    {
        {{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0}},
        {{0, 0, 0},{0, 0, 0},{0, 0, -0.3},{0, 0, 0}},
        {{0, 0, 0},{0, 0, 0},{0, 0, 0},{0, 0, 0}}
    }; /* deg */ /* "*"" may be wrong! */

    Double_t fModuleCSToStationCS[3][4][3] = 
    {
        {{60., -126., -15.3},{0., -126., 0},{0, 0, 0},{0, 0, 0}},
        // {{60., -68.1, 7.3},{0., -63., 0},{55. /* * */, -137., 0},{-5., -129., 7.3}},
        {{59.65, -68.1, 7.3},{0., -63., 0},{55. /* * */, -128., 0},{-6., -123., 7.3}},
        {{60., -131.14, 7.3},{0, -126., 0},{0, 0, 0},{0, 0, 0}}
    }; /* mm */ /* "*"" may be wrong! */

    Double_t fStationsRotation[3] = {0.001745, 0.001745, 0.002443}; /* rad */

    Double_t fStationCSToGlobalCS[3][3] =
    {
        {37.01, -27.99, 605.52},
        {36.85, -28.17, 245.42},
        {37.03, -28.17, -252.98}
    }; /* mm */

    Double_t fStationRotationCorrection[3][3] =
    {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    }; /* deg */

    Double_t fStationPositionCorrection[3][3] =
    {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0}
    }; /* mm */



    if (fIfReverseModuleX[station][module])
    {
        localX = fSiModuleWidthX - localX;
    }

    if (fIfReverseModuleY[station][module])
    {
        localY = fSiModuleWidthY[station] - localY;
    }

    TVector3 localModuleHit(localX, localY, 0);

    localX -= fSiModuleWidthX;
    localY -= fSiModuleWidthY[station];
    localModuleHit.RotateX(fModuleRotationCorrection[station][module][0]*degToRad);
    localModuleHit.RotateY(fModuleRotationCorrection[station][module][1]*degToRad);
    localModuleHit.RotateZ(fModuleRotationCorrection[station][module][2]*degToRad);
    localX += fSiModuleWidthX;
    localY += fSiModuleWidthY[station];

    TVector3 hit(localModuleHit.X() + fModuleCSToStationCS[station][module][0],
                 localModuleHit.Y() + fModuleCSToStationCS[station][module][1],
                          fModuleCSToStationCS[station][module][2]);

    hit.RotateZ(fStationsRotation[station]);

    // Station rotation correction
    hit.RotateX(fStationRotationCorrection[station][0]*degToRad);
    hit.RotateY(fStationRotationCorrection[station][1]*degToRad);
    hit.RotateZ(fStationRotationCorrection[station][2]*degToRad);

    // Station position correction
    hit += TVector3(fStationPositionCorrection[station][0], fStationPositionCorrection[station][1], fStationPositionCorrection[station][2]);


    hit += TVector3(fStationCSToGlobalCS[station][0], fStationCSToGlobalCS[station][1], fStationCSToGlobalCS[station][2]);

    return hit;
}

Double_t StandSiliconGeoMapper::CalculateLocalY(Double_t localX, Double_t stripOffsetY, Int_t station)
{
    Double_t clearance = 2.303; // clearance between two sensors in big modules
    Double_t localY = ((stripOffsetY - localX)/fTangentOfStripsYSlope);
    if (localY > 61.853)
    {
        if (station == 0 || station == 2)
        {
            localY += clearance;
        }
    }
    
    return localY;
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
