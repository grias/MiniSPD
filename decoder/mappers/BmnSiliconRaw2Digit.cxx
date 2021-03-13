#include "BmnSiliconRaw2Digit.h"
#include "TCanvas.h"

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit() {
    fEventId = -1;
    fMapFileName = "";
}

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer) : BmnAdcProcessor(period, run, "SILICON", ADC_N_CHANNELS, ADC128_N_SAMPLES, vSer) {

    cout << "Loading SILICON Map from FILE: Period " << period << ", Run " << run << "..." << endl;

    fEventId = 0;
    fMapFileName = Form("SILICON_map_run%d.txt", period);
    ReadMapFile();

    const Int_t kNStations = 3;
    const Int_t kNModules = 4;
    const Int_t kNLayers = 2;
    const Int_t kNStrips = 640;

    Int_t nEventsInHist = 12000;

    fSig = new TH1D("h1_sig", "Raw signal", nEventsInHist, 0-0.5, nEventsInHist-1-0.5);
    fSigMp = new TH1D("h1_sig_mped", "Raw signal minus pedestal", nEventsInHist, 0-0.5, nEventsInHist-1-0.5);
    fSigMpMc = new TH1D("h1_sig_mped_mcms", "Raw signal minus pedestal and CMS", nEventsInHist, 0-0.5, nEventsInHist-1-0.5);
    fPed = new TH1D("h1_ped", "Pedestal", nEventsInHist, 0-0.5, nEventsInHist-1-0.5);
    fSigma = new TH1D("h1_sigma", "Sigma", nEventsInHist, 0-0.5, nEventsInHist-1-0.5);
    fSigmaX3 = new TH1D("h1_sigma_x3", "Sigma*3", nEventsInHist, 0-0.5, nEventsInHist-1-0.5);
    fClearLine = new TH1D("h1_cut", "Noise level", nEventsInHist, 0-0.5, nEventsInHist-1-0.5);
    fClearLineX3 = new TH1D("h1_cut_x3", "Noise level *3", nEventsInHist, 0-0.5, nEventsInHist-1-0.5);
    fClearLineX6 = new TH1D("h1_cut_x6", "Noise level *6", nEventsInHist, 0-0.5, nEventsInHist-1-0.5);
    
    fSigProf = new TH1F***[kNStations];
    fNoisyChannels = new Bool_t***[kNStations];
    for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
        fSigProf[iSt] = new TH1F**[kNModules];
        fNoisyChannels[iSt] = new Bool_t**[kNModules];
        for (UInt_t iMod = 0; iMod < kNModules; ++iMod) {
            fSigProf[iSt][iMod] = new TH1F*[kNLayers];
            fNoisyChannels[iSt][iMod] = new Bool_t*[kNLayers];
            for (Int_t iLay = 0; iLay < kNLayers; ++iLay) {
                TString histName;
                histName.Form("SIL_%d_%d_%d", iSt, iMod, iLay);
                fSigProf[iSt][iMod][iLay] = new TH1F(histName, histName, kNStrips, 0, kNStrips);
                fNoisyChannels[iSt][iMod][iLay] = new Bool_t[kNStrips];
                for (Int_t iStrip = 0; iStrip < kNStrips; ++iStrip)
                    fNoisyChannels[iSt][iMod][iLay][iStrip] = kFALSE;
            }
        }
    }
}

BmnSiliconRaw2Digit::~BmnSiliconRaw2Digit() {

    // TCanvas *canvas = new TCanvas("canvas", "", 1200, 800);
    // fSigMpMc->SetLineColor(8);
    // fSigMpMc->Draw("HIST"); // to make right boundaries
    // fSigMp->SetLineColor(9);
    // fSigMp->Draw("HIST SAME");
    // fSig->SetLineColor(42);
    // fSig->Draw("HIST SAME");
    // fSigMpMc->Draw("HIST SAME"); // draw over other hists
    // fPed->SetLineColor(2);
    // fPed->Draw("HIST SAME");
    // fSigma->SetLineColor(6);
    // fSigma->Draw("HIST SAME");
    // fSigmaX3->SetLineColor(28);
    // fSigmaX3->Draw("HIST SAME");
    // fClearLine->SetLineColor(7);
    // fClearLine->Draw("HIST SAME");
    // fClearLineX3->SetLineColor(37);
    // fClearLineX3->Draw("HIST SAME");
    // fClearLineX6->SetLineColor(41);
    // fClearLineX6->Draw("HIST SAME");
    // TString name = TString(getenv("VMCWORKDIR")) + TString("/macro/pictures/") + TString("h1_ch_sig.png");
    // canvas->SaveAs(name);


    const Int_t kNStations = 3;
    const Int_t kNModules = 4;
    const Int_t kNLayers = 2;

    for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
        for (UInt_t iMod = 0; iMod < kNModules; ++iMod) {
            for (Int_t iLay = 0; iLay < kNLayers; ++iLay) {
                delete fSigProf[iSt][iMod][iLay];
                delete[] fNoisyChannels[iSt][iMod][iLay];
            }
            delete[] fSigProf[iSt][iMod];
            delete[] fNoisyChannels[iSt][iMod];
        }
        delete[] fSigProf[iSt];
        delete[] fNoisyChannels[iSt];
    }
    delete[] fNoisyChannels;
    delete[] fSigProf;
    
    delete fSig;
    delete fSigMp;
    delete fSigMpMc;
    delete fPed;
    delete fSigma;
    delete fSigmaX3;
    delete fClearLine;
    delete fClearLineX3;
    delete fClearLineX6;
}

BmnStatus BmnSiliconRaw2Digit::ReadMapFile() {
    UInt_t ser = 0;
    Int_t ch_lo = 0;
    Int_t ch_hi = 0;
    Int_t mod_adc = 0;
    Int_t mod = 0;
    Int_t lay = 0;
    Int_t station = 0;
    string dummy;

    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fMapFileName;
    ifstream inFile(name.Data());
    if (!inFile.is_open())
        cout << "Error opening map-file (" << name << ")!" << endl;
    getline(inFile, dummy); //comment line in input file
    getline(inFile, dummy); //comment line in input file
    getline(inFile, dummy); //comment line in input file
    getline(inFile, dummy); //comment line in input file

    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> ch_lo >> ch_hi >> mod_adc >> mod >> lay >> station;
        if (!inFile.good()) break;
        BmnSiliconMapping record;
        record.layer = lay;
        record.serial = ser;
        record.module = mod;
        record.channel_low = ch_lo;
        record.channel_high = ch_hi;
        record.station = station;
        fMap.push_back(record);
    }

    // for (auto &&map : fMap)
    // {
    //     printf("<BmnSiliconRaw2Digit::ReadMapFile> iStation: %d, serial: %d\n", map.station, map.serial);
    // }
    
}

BmnStatus BmnSiliconRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray *silicon, Int_t iEv) {
    fEventId++;
    for (auto it : fMap)
        for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
            BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
            if (adcDig->GetSerial() == it.serial && (adcDig->GetChannel() >= it.channel_low && adcDig->GetChannel() <= it.channel_high)) {
                ProcessDigit(adcDig, &it, silicon, kFALSE, iEv);
            }
        }
}

BmnStatus BmnSiliconRaw2Digit::FillProfiles(TClonesArray *adc, Int_t iEv) {
    for (auto it : fMap)
        for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
            BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
            if (adcDig->GetSerial() == it.serial && (adcDig->GetChannel() >= it.channel_low && adcDig->GetChannel() <= it.channel_high)) {
                ProcessDigit(adcDig, &it, NULL, kTRUE, iEv);
            }
        }
}

BmnStatus BmnSiliconRaw2Digit::FillNoisyChannels() {
    const Int_t kNStations = 3;
    const Int_t kNModules = 4;
    const Int_t kNLayers = 2;
    const Int_t kNStrips = 640;
    const Int_t kNStripsInBunch = 32;
    const Int_t kNBunches = kNStrips / kNStripsInBunch;
    const Int_t kNThresh = 3;

    struct BmnSiliconNoisCh {
        UInt_t station; //X = 0 or X' = 1
        UInt_t module;
        UInt_t layer;
        UInt_t strip;
    };

    vector<BmnSiliconNoisCh> noisymap;

    Int_t noisMod = 0;
    Int_t noisLay = 0;
    Int_t noisStat = 0;
    Int_t noisStrip = 0;
    string dummy;


    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/SILICON_noisy_channels.txt");
    ifstream inFile(name.Data());
    if (!inFile.is_open())
        cout << "Error opening map-file (" << name << ")!" << endl;
    getline(inFile, dummy); //comment line in input file
    getline(inFile, dummy); //comment line in input file

    while (!inFile.eof()) {
        inFile >> std::dec >> noisStat >> noisMod >> noisLay >> noisStrip;
        if (!inFile.good()) break;
        BmnSiliconNoisCh record;
        record.layer = noisLay;
        record.module = noisMod;
        record.station = noisStat;
        record.strip = noisStrip;
        noisymap.push_back(record);
    }

    // fNoisyChannels[0][1][1][415]=kTRUE; //this is KOSTYL
    // for (Int_t iSt = 0; iSt < kNStations; ++iSt)
    //     for (UInt_t iMod = 0; iMod < kNModules; ++iMod)
    //         for (Int_t iLay = 0; iLay < kNLayers; ++iLay) {
    //             TH1F* prof = fSigProf[iSt][iMod][iLay];
    //             for (Int_t iBunch = 0; iBunch < kNBunches; ++iBunch) {
    //                 Double_t meanDiff = 0.0;
    //                 for (Int_t iStrip = 0; iStrip < kNStripsInBunch - 1; ++iStrip) {
    //                     Int_t strip = iStrip + iBunch * kNStripsInBunch;
    //                     Double_t curr = prof->GetBinContent(strip);
    //                     Double_t next = prof->GetBinContent(strip + 1);
    //                     meanDiff += Abs(next - curr);
    //                 }
    //                 meanDiff /= kNStripsInBunch;
    //                 for (Int_t iStrip = 0; iStrip < kNStripsInBunch - 1; ++iStrip) {
    //                     Int_t strip = iStrip + iBunch * kNStripsInBunch;
    //                     for (auto it : noisymap)
    //                     {
    //                         if (iSt==it.station && iMod==it.module && iLay==it.layer && strip==it.strip)
    //                         fNoisyChannels[iSt][iMod][iLay][strip] = kTRUE;
    //                     }
    //                     Double_t curr = prof->GetBinContent(strip);
    //                     Double_t next = prof->GetBinContent(strip + 1);
    //                     if (kNThresh * meanDiff < next - curr)
    //                         fNoisyChannels[iSt][iMod][iLay][strip] = kTRUE;
    //                 }
    //             }
    //         }

    // for (Int_t iSt = 0; iSt < kNStations; ++iSt) {
    //     for (UInt_t iMod = 0; iMod < kNModules; ++iMod) {
    //         for (Int_t iLay = 0; iLay < kNLayers; ++iLay) {
    //             for (Int_t iStrip = 0; iStrip < kNStrips; ++iStrip)
    //             {
    //                 if (fNoisyChannels[iSt][iMod][iLay][iStrip])
    //                 {
    //                     printf("\t%d\t%d\t%d\t%d\n", iSt, iMod, iLay, iStrip);
    //                 }
                    
    //             }

                    
    //         }
    //     }
    // }

    for (auto it : noisymap)
    {
        fNoisyChannels[it.station][it.module][it.layer][it.strip] = kTRUE;
    }


}

void BmnSiliconRaw2Digit::ProcessDigit(BmnADCDigit* adcDig, BmnSiliconMapping* silM, TClonesArray *silicon, Bool_t doFill, Int_t iEv) {
    const UInt_t nSmpl = adcDig->GetNSamples();
    UInt_t ch = adcDig->GetChannel();
    UInt_t ser = adcDig->GetSerial();

    BmnSiliconDigit candDig[nSmpl];

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        BmnSiliconDigit dig;
        dig.SetStation(silM->station);
        dig.SetModule(silM->module);
        dig.SetStripLayer(silM->layer);
        dig.SetStripNumber((ch - silM->channel_low) * nSmpl + 1 + iSmpl);
        // Double_t sig = Abs((Double_t) ((adcDig->GetShortValue())[iSmpl] / 16));
        Double_t sig = (Double_t) ((adcDig->GetShortValue())[iSmpl] / 16);
        // if (ch == 6 && iSmpl == 15)
        // printf("from proc: %f\n", sig);
        // if (silM->station == 2)
        // {
        //     printf("Station %d\n", silM->station);
        // }
        dig.SetStripSignal(sig);
        candDig[iSmpl] = dig;
    }

    // Double_t signals[nSmpl];
    // for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) signals[iSmpl] = 0.0;
    // Int_t nOk = 0;
    // for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
    //     if ((candDig[iSmpl]).GetStripSignal() == 0) continue;
    //     signals[iSmpl] = (candDig[iSmpl]).GetStripSignal();
    //     nOk++;
    // }

    vector<Double_t> vSamples;
    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
    {
        vSamples.push_back((candDig[iSmpl]).GetStripSignal());
    }

    Double_t*** vNoiseLvl = GetNoiseLvls();
    Double_t*** vPed = GetPedestals();
    Double_t*** vPedRMS = GetPedestalsRMS();
    // Double_t baseTresholds[3] = {170., 150., 150.};
    Double_t baseTresholds[3] = {220., 120., 120.};
    // Double_t baseTresholds[3] = {0., 0., 0.};
    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        auto candStation = (candDig[iSmpl]).GetStation();
        // if (ch == 6 && iSmpl == 15)
        // printf("stat proc: %d\n", candStation);
        if (candStation == -1) continue;

        BmnSiliconDigit * dig = &candDig[iSmpl];
        Double_t CMS = CalcCMS(candStation, ch, vSamples);
        Double_t ped = vPed[candStation][ch][iSmpl];
        Double_t pedRms = vPedRMS[candStation][ch][iSmpl];
        // Double_t sig = Abs(dig->GetStripSignal()) - ped - CMS;
        // Double_t sig = Abs(dig->GetStripSignal() - ped - CMS);
        Double_t sig = dig->GetStripSignal() - ped - CMS;
        // Double_t sig = Abs(dig->GetStripSignal());
        Double_t threshold = 0.0;
        // threshold = baseTresholds[candStation] + 3*vPedRMS[candStation][ch][iSmpl];
        Double_t noiseLvl = vNoiseLvl[candStation][ch][iSmpl];
        threshold = 6*noiseLvl;
        // if (iEv == 200)
        // {
        //     printf("%d %d %d\n", candStation, ch, iSmpl);
        // }
        if (candStation == 0 && ch == 6 && iSmpl == 100)
        {
            Double_t pedRmsRightSign = dig->GetStripLayer() == 1 ? -pedRms : pedRms;
            Double_t noiseLvlRightSign = dig->GetStripLayer() == 1 ? -noiseLvl : noiseLvl;
            fSig->SetBinContent(fSig->FindBin(iEv), dig->GetStripSignal());
            fSigMp->SetBinContent(fSig->FindBin(iEv), dig->GetStripSignal() - ped);
            fSigMpMc->SetBinContent(fSig->FindBin(iEv), dig->GetStripSignal() - ped - CMS);
            fPed->SetBinContent(fSig->FindBin(iEv), ped);
            fSigma->SetBinContent(fSig->FindBin(iEv), pedRmsRightSign);
            fSigmaX3->SetBinContent(fSig->FindBin(iEv), pedRmsRightSign*3);
            fClearLine->SetBinContent(fSig->FindBin(iEv), noiseLvlRightSign);
            fClearLineX3->SetBinContent(fSig->FindBin(iEv), noiseLvlRightSign*3);
            fClearLineX6->SetBinContent(fSig->FindBin(iEv), noiseLvlRightSign*6);
        }
        
        if ( sig == 0.0 || (dig->GetStripLayer() == 0 && sig < threshold) || (dig->GetStripLayer() == 1 && sig > -threshold) )
        {
            continue;
        }

        if (doFill) {
            fSigProf[dig->GetStation()][dig->GetModule()][dig->GetStripLayer()]->Fill(dig->GetStripNumber());
        } else {
            BmnSiliconDigit * resDig = new((*silicon)[silicon->GetEntriesFast()]) BmnSiliconDigit(dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), sig);
            if (fNoisyChannels[dig->GetStation()][dig->GetModule()][dig->GetStripLayer()][dig->GetStripNumber() + 1])
                resDig->SetIsGoodDigit(kFALSE);
                // resDig->SetIsGoodDigit(kTRUE);

            else
                resDig->SetIsGoodDigit(kTRUE);
                // resDig->SetIsGoodDigit(kFALSE);

        }
    }

}

ClassImp(BmnSiliconRaw2Digit)
