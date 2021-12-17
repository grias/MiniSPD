#ifndef STANDGEMGEOMAPPER_H
#define STANDGEMGEOMAPPER_H

#include <TObject.h>
#include <TVector3.h>

class StandGemGeoMapper
{
public:
    static Double_t CalculateLocalCoordinateForStrip(Int_t station, Int_t module, Int_t side, Int_t strip);
    static Double_t CalculateLocalY(Double_t localX, Double_t stripOffsetY, Int_t station);
    static Bool_t IsInSensitiveRange(Double_t localY);
    static TVector3 CalculateGlobalCoordinatesForHit(Int_t station, Int_t module, Double_t localX, Double_t localY);

    static Double_t fPitch[2];
    static Double_t fFirstStripOffset[2];
    static Int_t fIfReverseModuleX[2];
    static Int_t fIfReverseModuleY[2];
    static Int_t fIsActiveModule[2];
    static Double_t fStationsRotation[2];
    static Double_t fStationsCenter[2][2];
    static Double_t fStationCSToGlobalCS[2][3];
    static Double_t fStationPositionCorrection[2][3];

    static constexpr Double_t fTangentOfStripsYSlope = 0.2679492;
    static constexpr Double_t degToRad = 3.14159265359 / 180.;
    static constexpr Double_t fLowerSensetiveBoundary = -100;
    static constexpr Double_t fSensetiveZoneLength = 500.;
    static constexpr Double_t fUpperSensetiveBoundary = fLowerSensetiveBoundary + fSensetiveZoneLength;
};

#endif // STANDGEMGEOMAPPER_H
