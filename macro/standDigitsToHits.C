// void standDigitsToHits(Int_t iEvent = -1)
void standDigitsToHits(Int_t run, Int_t iEvent = -1)
{
    TString inputFileName = Form("data/stand_run%04d_digits.root",run);
    TString outputFileName = Form("data/stand_run%04d_hits.root",run);

    StandHitsProducer* hitsProducer = new StandHitsProducer();

    hitsProducer->SetInputFileName(inputFileName);
    hitsProducer->SetOutputFileName(outputFileName);

    hitsProducer->Init();
    if (iEvent == -1)
    {
        hitsProducer->ProduceHitsFromAllEvents();
    }
    else
    {
        hitsProducer->ProduceHitsFromOneEvent(iEvent);
    }
    hitsProducer->Finish();

    gApplication->Terminate();
}