#ifndef STANDREVERSESILICONGEOMAPPER_H
#define STANDREVERSESILICONGEOMAPPER_H

#include "StandSiliconGeoMapper.h"

#include <array>

class StandReverseSiliconGeoMapper : public StandSiliconGeoMapper
{
public:
    static std::array<Double_t, 2> CalculateLocalCoordinatesForHit(Int_t station, Int_t module, Double_t globalX, Double_t globalY);
    static Int_t CalculateStripForLocalCoordinate(Int_t station, Int_t module, Int_t side, Double_t localCoord);
    static Double_t CalculateInterStripForLocalCoordinate(Int_t station, Int_t module, Int_t side, Double_t localCoord);
    static std::array<Int_t, 2> DivideInterStripInTwo(Double_t interStrip, Double_t fixedAmplitude, Double_t &varAmplitude);

private:

    ClassDef(StandReverseSiliconGeoMapper, 1)
};

#endif
