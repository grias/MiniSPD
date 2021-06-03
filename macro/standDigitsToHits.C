#include "StandHitsProducer.h"
#include "SiliconHitMaker.h"
#include "GemHitMaker.h"

#include <cstdlib>
#include <memory>

void standDigitsToHits(Int_t run, Int_t iEvent = -1)
{
    auto hitsProducer = make_shared<StandHitsProducer>();

    auto inputFilePath = TString(getenv("VMCWORKDIR")) + TString("/macro/data/") + Form("stand_run%04d_digits.root",run);
    hitsProducer->SetInputFileName(inputFilePath);

    auto outputFilePath = TString(getenv("VMCWORKDIR")) + TString("/macro/data/") + Form("stand_run%04d_hits.root",run);
    hitsProducer->SetOutputFileName(outputFilePath);

    // hitsProducer->AddHitMaker(make_shared<SiliconHitMaker>());
    hitsProducer->AddHitMaker(make_shared<GemHitMaker>());

    if (iEvent == -1)
    {
        hitsProducer->ProduceHitsFromAllEvents();
    }
    else
    {
        hitsProducer->ProduceHitsFromOneEvent(iEvent);
    }

    gApplication->Terminate();
}