#include "StandDataProducer.h"

StandDataProducer::StandDataProducer()
{
    fIOManager = new StandIOManager();
}

StandDataProducer::~StandDataProducer()
{
}

void StandTracksProducer::Init()
{
    cout << "-I-<StandTracksProducer::Init>" << endl;

    OpenInputOutputFiles();
}

void StandTracksProducer::Finish()
{
    cout << "-I-<StandTracksProducer::Finish>" << endl;

    CloseInputOutputFiles();
}
