#ifndef STANDSTRAWGEOMAPPER_H
#define STANDSTRAWGEOMAPPER_H

#include <TObject.h>
#include <TVector3.h>
#include <array>

#include <cmath>

class StandStrawGeoMapper : public TObject
{
public:
    static std::array<Double_t, 2> GetWireGlobalPositionZX(Int_t plane, Int_t wire);

    static Double_t fFirstWirePosX[2][2];
    static Double_t fWirePosZ[2][2];
    static Double_t fStationPositionCorrectionX[2];


    // static constexpr Double_t fTubeRadius = 3.073; /* mm */
    static constexpr Double_t fTubeRadius = 3.073; /* mm */
    static constexpr Double_t fDistanceBtwStations = 40.6; /* mm */
    static constexpr Double_t fDistanceBtwWiresX = 2*fTubeRadius; /* mm */
    // static constexpr Double_t fDistanceBtwWiresX = 6.1; /* mm */
    static constexpr Double_t fDistanceBtwWiresZ = 7.; /* mm */

    static constexpr Double_t fPositionCorrectionX = 0.1; /* mm */
    static constexpr Double_t fPositionCorrectionZ = 0.0; /* mm */

private:
    ClassDef(StandStrawGeoMapper, 1)
};

#endif // STANDSTRAWGEOMAPPER_H
