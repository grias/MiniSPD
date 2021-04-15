void standDigitsToHits(Int_t run, Int_t iEvent = -1)
{
    StandHitsProducer* hitsProducer = new StandHitsProducer();

    hitsProducer->SetInputFileName(Form("data/stand_run%04d_digits.root",run));
    hitsProducer->SetOutputFileName(Form("data/stand_run%04d_hits.root",run));

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