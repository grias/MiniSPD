// void standDigitsToHits(Int_t iEvent = -1)
void standDigitsToHits(Int_t run, Int_t iEvent = -1)
{
    TString inputFileName = Form("bmn_run%04d_digi.root",run);
    TString outputFileName = Form("stand_run%04d_hits.root",run);

    StandHitsProducer* hitsProducer = new StandHitsProducer();

    hitsProducer->SetInputFileName(inputFileName);
    hitsProducer->SetOutputFileName(outputFileName);

    hitsProducer->Init();
    if (iEvent == -1)
    {
        hitsProducer->ProduceHitsFromAllEvents();
    }
    else if (iEvent == -2)
    {
        Int_t iEvent = 0;
        for (string line; getline(cin, line);) {
            hitsProducer->ProduceHitsFromOneEvent(iEvent++);
        }
    }
    else
    {
        hitsProducer->ProduceHitsFromOneEvent(iEvent);
    }
    hitsProducer->Finish();

    gApplication->Terminate();
}