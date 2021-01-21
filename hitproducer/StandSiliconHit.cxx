#include "StandSiliconHit.h"

#include <iostream>

StandSiliconHit::StandSiliconHit()
{
}

StandSiliconHit::StandSiliconHit(Int_t station, Int_t module) : 
fStation(station),
fModule(module),
fLocalX(0),
fLocalY(0)
{
}

StandSiliconHit::StandSiliconHit(Int_t station, Int_t module, Double_t localX, Double_t localY) : 
fStation(station),
fModule(module),
fLocalX(localX),
fLocalY(localY)
{
}

StandSiliconHit::~StandSiliconHit()
{
}

void StandSiliconHit::Print()
{
    printf("-I-<StandSiliconHit::Print> Station: %d, Module: %d, localXY(%f, %f)\n", fStation, fModule, fLocalX, fLocalY);
}