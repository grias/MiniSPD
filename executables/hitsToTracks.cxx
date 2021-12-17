#include "StandTracksProducer.h"
#include "StandTracksProducerX.h"

#include <cstdlib>
#include <memory>

int main(int argc, char const *argv[])
{
    int run = argc >= 2 ? atoi(argv[1]) : 816;
    int iEvent = argc >= 3 ? atoi(argv[2]) : -1;

    auto tracksProducer = std::make_shared<StandTracksProducer>();
    // auto tracksProducer = std::make_shared<StandTracksProducerX>();

    auto inputFilePath = TString(getenv("VMCWORKDIR")) + TString("/macro/data/") + Form("stand_run%04d_hits.root",run);
    auto outputFilePath = TString(getenv("VMCWORKDIR")) + TString("/macro/data/") + Form("stand_run%04d_tracks.root",run);

    tracksProducer->SetInputFileName(inputFilePath);
    tracksProducer->SetOutputFileName(outputFilePath);

    if (iEvent == -1)
    {
        tracksProducer->ProduceTracksFromAllEvents();
    }
    else
    {
        tracksProducer->ProduceTracksFromOneEvent(iEvent);
    }

    return 0;
}
