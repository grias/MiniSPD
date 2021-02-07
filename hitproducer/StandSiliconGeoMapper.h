#ifndef STANDSILICONGEOMAPPER_H
#define STANDSILICONGEOMAPPER_H

#include <TObject.h>


class StandSiliconGeoMapper : public TObject
{

public:
    static Double_t CalculateLocalCoordinateForStrip(Int_t station, Int_t module, Int_t side, Int_t strip);
    static Double_t CalculateLocalY(Double_t localX, Double_t stripOffsetY);
    static Bool_t IsInSensitiveRange(Int_t station, Double_t localY);

private:
    StandSiliconGeoMapper();
    ~StandSiliconGeoMapper();

    static constexpr Double_t fTangentOfStripsYSlope = 0.0437;

    ClassDef(StandSiliconGeoMapper, 1)
};

// class StandSiliconGeoMapperX
// {
// public:
//     static StandSiliconGeoMapperX *Instance()
//     {
//         return (gInstance) ? gInstance : new StandSiliconGeoMapperX();
//     }
//     StandSiliconGeoMapperX();
//     ~StandSiliconGeoMapperX();

// private:
//     static StandSiliconGeoMapperX *gInstance;
// };

// StandSiliconGeoMapperX::StandSiliconGeoMapperX()
// {
//     if (gInstance)
//     {
//         std::cout << "<StandSiliconGeoMapperX::StandSiliconGeoMapperX> Singleton instance already exists."
//              << std::endl;
//         return;
//     }
//     gInstance = this;
    
//     return;
// }

// StandSiliconGeoMapperX::~StandSiliconGeoMapperX()
// {
// }

#endif // STANDSILICONGEOMAPPER_H
