#include "StandHitsProducer.h"
#include "SiliconHitMaker.h"
#include "GemHitMaker.h"

#include <cstdlib>
#include <memory>

int main(int argc, char const *argv[])
{
    int run = argc >= 2 ? atoi(argv[1]) : 816;
    int iEvent = argc >= 3 ? atoi(argv[2]) : -1;

    auto hitsProducer = make_shared<StandHitsProducer>();

    auto inputFilePath = TString(getenv("VMCWORKDIR")) + TString("/macro/data/") + Form("stand_run%04d_digits.root",run);
    auto outputFilePath = TString(getenv("VMCWORKDIR")) + TString("/macro/data/") + Form("stand_run%04d_hits.root",run);

    hitsProducer->SetInputFileName(inputFilePath);
    hitsProducer->SetOutputFileName(outputFilePath);

    hitsProducer->AddHitMaker(make_shared<SiliconHitMaker>());
    hitsProducer->AddHitMaker(make_shared<GemHitMaker>());

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
