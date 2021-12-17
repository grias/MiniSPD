#include "McHit.h"

McHit::McHit(Double_t x, Double_t y, Double_t z, Int_t station, Int_t module, TString detName):
fX(x),
fY(y),
fZ(z),
fStation(station),
fModule(module),
fDetectorName(detName)
{
}

McHit::McHit():
fX(0.),
fY(0.),
fZ(0.),
fStation(0),
fModule(0),
fDetectorName("")
{
}

McHit::~McHit()
{
}