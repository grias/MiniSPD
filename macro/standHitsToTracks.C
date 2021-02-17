void standHitsToTracks(Int_t run, Int_t iEvent = -1)
{
    TString inputFileName = Form("stand_run%04d_hits.root", run);
    TString outputFileName = Form("stand_run%04d_tracks.root", run);

    StandTracksProducer* tracksProducer = new StandTracksProducer();

    tracksProducer->SetInputFileName(inputFileName);
    tracksProducer->SetOutputFileName(outputFileName);

    tracksProducer->Init();
    if (iEvent == -1)
    {
        tracksProducer->ProduceTracksFromAllEvents();
    }
    else if (iEvent == -2)
    {
        Int_t iEvent = 0;
        for (string line; getline(cin, line);) {
            tracksProducer->ProduceTracksFromOneEvent(iEvent++);
        }
    }
    else
    {
        tracksProducer->ProduceTracksFromOneEvent(iEvent);
    }
    tracksProducer->Finish();

    gApplication->Terminate();
}