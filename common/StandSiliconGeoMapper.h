#ifndef STANDSILICONGEOMAPPER_H
#define STANDSILICONGEOMAPPER_H

#include <TObject.h>
#include <TVector3.h>

enum ModuleType { MODULE_LONG, MODULE_SHORT };

class StandSiliconGeoMapper : public TObject
{
public:
    static Double_t CalculateLocalCoordinateForStrip(Int_t station, Int_t module, Int_t side, Int_t strip);
    static Double_t CalculateLocalY(Double_t localX, Double_t stripOffsetY, Int_t station);
    static Bool_t IsInSensitiveRange(Int_t station, Double_t localY);
    static TVector3 CalculateGlobalCoordinatesForHit(Int_t station, Int_t module, Double_t localX, Double_t localY);
    static Double_t CalculateGlobalU(Int_t station, Int_t module, Double_t offsetY);

    static Int_t fStripsNumberCorrection[3][4][2];
    static Double_t fPitch[2];
    static Double_t fFirstStripOffset[2];
    static Int_t fIfReverseModuleX[3][4];
    static Int_t fIfReverseModuleY[3][4];
    static Int_t fIfSwitchModuleSideX[3][4];
    static Int_t fIfSwitchModuleSideY[3][4];
    static Double_t fSiModuleWidthX;
    static Double_t fSiModuleWidthY[3];
    static Double_t fModuleCSToStationCS[3][4][3];
    static Double_t fModulePositionCorrection[3][4][3];
    static Int_t fIsActiveModule[3][4];
    static Double_t fModuleRotationCorrection[3][4][3];
    static Double_t fStationsRotation[3];
    static Double_t fStationCSToGlobalCS[3][3];
    static Double_t fStationPositionCorrection[3][3];
    static ModuleType fModulesTypes[3];

    static constexpr Double_t fTangentOfStripsYSlope = 0.0437;
    static constexpr Double_t degToRad = 3.14159265359 / 180.;
    static constexpr Double_t fClearance = 2.303; // clearance between two sensors in big modules
    static constexpr Double_t fLowerSensetiveBoundary = 1.148;
    static constexpr Double_t fSensetiveZoneLength = 60.705;
    static constexpr Double_t fUpperSensetiveBoundaryShortModule = fLowerSensetiveBoundary + fSensetiveZoneLength;
    static constexpr Double_t fUpperSensetiveBoundaryLongModule = fLowerSensetiveBoundary + 2*fSensetiveZoneLength + fClearance;

private:

    ClassDef(StandSiliconGeoMapper, 1)
};

#endif // STANDSILICONGEOMAPPER_H
