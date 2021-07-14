void standHitsToTracksX(Int_t run, Int_t iEvent = -1)
{
    StandTracksProducerX* tracksProducer = new StandTracksProducerX();

    tracksProducer->SetInputFileName(Form("data/stand_run%04d_hits.root", run));
    tracksProducer->SetOutputFileName(Form("data/stand_run%04d_tracks.root", run));

    if (iEvent == -1)
    {
        tracksProducer->ProduceTracksFromAllEvents();
    }
    else
    {
        tracksProducer->ProduceTracksFromOneEvent(iEvent);
    }

    gApplication->Terminate();
}