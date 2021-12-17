#include "StandTrack.h"

StandTrack::StandTrack()
{
    for (size_t iStation = 0; iStation < STANDTRACKNHITS; iStation++)
    {
        fIsStationIncluded[iStation] = 0;
    }
}

StandTrack::~StandTrack()
{
}