const bool kVERBOSE_MODE = false;

TH1D *histU;
TH1D *histI;

Int_t serialMap[3] = {0x080C3B50, 0x0611D0C2, 0x0611E01A}; // top, mid, down

void AdcDigitsAnalysisHist(UInt_t runId = 0, Int_t drawEvent = -1, Int_t drawHistKey = -1)
{
    Int_t serialKey = drawHistKey / 100;
    Int_t channelKey = drawHistKey - serialKey * 100;
    printf("Drawing Serial: %d, Channel: %d, Event: %d\n", serialKey, channelKey, drawEvent);

    TString histNameU = Form("h1_u_ser%d_ch%d_ev%d", serialKey, channelKey, drawEvent);
    TString histDiscriptionU = Form("Channel readout (Serial: %d, Channel: %d, Event: %d)", serialKey, channelKey, drawEvent);
    TH1D *histU = new TH1D(histNameU, histDiscriptionU, 128, 0 - 0.5, 128 - 0.5);
    histU->GetYaxis()->SetTitle("U [???]");

    TString histNameI = Form("h1_i_ser%d_ch%d_ev%d", serialKey, channelKey, drawEvent);
    TString histDiscriptionI = Form("Channel readout (Serial: %d, Channel: %d, Event: %d)", serialKey, channelKey, drawEvent);
    TH1D *histI = new TH1D(histNameI, histDiscriptionI, 128, 0 - 0.5, 128 - 0.5);
    histI->GetYaxis()->SetTitle("I [???]");

    TString inFileName = Form("bmn_run%04d_raw.root", runId);
    TFile *inFile = new TFile(inFileName);
    if (!inFile)
        return;
    TTree *tree = (TTree *)inFile->Get("BMN_RAW");

    // BmnADCDigit branch
    TClonesArray *AdcDigits = 0;
    TBranch *branchADC = tree->GetBranch("ADC128");
    branchADC->SetAutoDelete(kTRUE);
    if (!branchADC)
        return;
    tree->SetBranchAddress("ADC128", &AdcDigits);

    BmnADCDigit *adcDigit = 0;

    // Event loop ---------------------------------------------------------------------
    Long64_t nEvents = tree->GetEntries();
    printf("NEvents: %lld\n", nEvents);
    for (Long64_t iEv = 0; iEv < nEvents; iEv++)
    {
        if (kVERBOSE_MODE)
            printf("\nEvent %lld\t", iEv);

        branchADC->GetEntry(iEv);

        // Adc digits
        Long64_t nDigits = AdcDigits->GetEntriesFast();
        if (kVERBOSE_MODE)
        printf("NDigits: %lld\n", nDigits);
        for (size_t iDigit = 0; iDigit < nDigits; iDigit++)
        {
            adcDigit = (BmnADCDigit *)AdcDigits->At(iDigit);

            Int_t serial = adcDigit->GetSerial();
            Int_t channel = adcDigit->GetChannel();
            Int_t nSamples = adcDigit->GetNSamples(); // NUMBER OF STRIPS?
            UShort_t *valueU = adcDigit->GetUShortValue();
            Short_t *valueI = adcDigit->GetShortValue();

            if (kVERBOSE_MODE)
                printf("<AdcDigit> Serial: %#010X, Channel: %d, NSamples: %d\n", serial, channel, nSamples);

            if (drawEvent > -1 && drawHistKey > -1)
            {

                if (serial == serialMap[serialKey] && channel == channelKey && iEv == drawEvent)
                {
                    printf("<AdcDigit> DRAW Serial: %#010X, Channel: %d, Event: %d\n", serial, channel, drawEvent);
                    for (size_t iSample = 0; iSample < nSamples; iSample++)
                    {
                        histU->Fill(iSample, valueU[iSample]);
                        histI->Fill(iSample, valueI[iSample]);
                    }
                }
            }
        }

    } // end of event

    // --- DRAW ------------------------------------------------------------------------

    if (drawEvent > -1 && drawHistKey > -1)
    {
        cout << "Drawing channel " << drawHistKey << endl;
    TCanvas *canvasU = new TCanvas(Form("canvasU%d", drawHistKey), "", 1200, 600);
    histU->Draw("HIST");
    canvasU->SaveAs(Form("pictures/h1_run%d_adc128_ser%d_ch%d_ev%d_u.png", runId, serialKey, drawHistKey, drawEvent));
    TCanvas *canvasI = new TCanvas(Form("canvasI%d", drawHistKey), "", 1200, 600);
    histI->Draw("HIST");
    canvasI->SaveAs(Form("pictures/h1_run%d_adc128_ser%d_ch%d_ev%d_i.png", runId, serialKey, drawHistKey, drawEvent));
    }

    
}
