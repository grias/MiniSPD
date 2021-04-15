void standHitsToTracks(Int_t run, Int_t iEvent = -1)
{
    StandTracksProducer* tracksProducer = new StandTracksProducer();

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