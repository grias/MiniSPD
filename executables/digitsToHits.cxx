#include "StandHitsProducer.h"

#include <cstdlib>

int main(int argc, char const *argv[])
{
    int run = atoi(argv[1]);
    int iEvent = atoi(argv[2]);

    StandHitsProducer* hitsProducer = new StandHitsProducer();

    hitsProducer->SetInputFileName(Form("stand_run%04d_digits.root",run));
    hitsProducer->SetOutputFileName(Form("stand_run%04d_hits.root",run));

    if (iEvent == -1)
    {
        hitsProducer->ProduceHitsFromAllEvents();
    }
    else
    {
        hitsProducer->ProduceHitsFromOneEvent(iEvent);
    }

    return 0;
}
