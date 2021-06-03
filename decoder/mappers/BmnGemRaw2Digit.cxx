#include "BmnGemRaw2Digit.h"

#include "TCanvas.h"

BmnGemRaw2Digit::BmnGemRaw2Digit() 
{
}

BmnGemRaw2Digit::BmnGemRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer, TString mapFileName) 
:BmnAdcProcessor(period, run, "GEM", ADC_N_CHANNELS, ADC32_N_SAMPLES, vSer)
{
    // const UInt_t nBins = 12000;
    // fRaw = new TH1D("h1_fRaw", "Raw signal", nBins, 0, nBins);
    // fRawMPed = new TH1D("h1_fRawMPed", "Raw minus pedestal", nBins, 0, nBins);
    // fRawMPedCMS = new TH1D("h1_fRawMPedCMS", "Raw minus pedestal, CMS", nBins, 0, nBins);
    // fPed = new TH1D("h1_fPed", "Pedestal", nBins, 0, nBins);
    // fPedRms = new TH1D("h1_fPedRms", "Pedestal RMS", nBins, 0, nBins);
    // fCMS = new TH1D("h1_fCMS", "CMS", nBins, 0, nBins);
    // fNoiseLvl = new TH1D("h1_fNoiseLvl", "Noise RMS", nBins, 0, nBins);
    // fTreshold = new TH1D("h1_fNoiseLvlX6", "Threshold", nBins, 0, nBins);

    string dummy;
    UInt_t ser = 0;
    UInt_t ch_lo = 0;
    UInt_t ch_hi = 0;
    UInt_t station = 0;
    UInt_t side = 0;
    UInt_t part = 0;
    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + mapFileName;

    ifstream inFile(name.Data());
    if (!inFile.is_open())
        cout << "Error opening map-file (" << name << ")!" << endl;

    for (Int_t i = 0; i < 3; ++i) getline(inFile, dummy); //comment line in input file

    while (!inFile.eof()) {
        inFile >> std::hex >> ser >> std::dec >> ch_lo >> ch_hi >> station >> side >> part;
        if (!inFile.good()) break;
        GemMapStructure record;
        record.serial = ser;
        record.channel_low = ch_lo;
        record.channel_high = ch_hi;
        record.station = station;
        record.side = side;
        record.part = part;
        fMap.push_back(record);
    }
    fEntriesInGlobMap = fMap.size();

    fSigProf = new TH1F**[N_STATIONS];
    fNoisyChannels = new Bool_t**[N_STATIONS];
    for (Int_t iSt = 0; iSt < N_STATIONS; ++iSt) {
        fSigProf[iSt] = new TH1F*[N_SIDES];
        fNoisyChannels[iSt] = new Bool_t*[N_SIDES];
        for (Int_t iSide = 0; iSide < N_SIDES; ++iSide) {
            TString histName;
            histName.Form("GEM_%d_%d", iSt, iSide);
            fSigProf[iSt][iSide] = new TH1F(histName, histName, N_STRIPS, 0, N_STRIPS);
            fNoisyChannels[iSt][iSide] = new Bool_t[N_STRIPS];
            for (Int_t iStrip = 0; iStrip < N_STRIPS; ++iStrip)
                fNoisyChannels[iSt][iSide][iStrip] = kFALSE;
        }
    }
}

BmnGemRaw2Digit::~BmnGemRaw2Digit() {
    //    if (fMap) delete[] fMap;

    for (Int_t iSt = 0; iSt < N_STATIONS; ++iSt) {
        for (Int_t iLay = 0; iLay < N_SIDES; ++iLay) {
            delete fSigProf[iSt][iLay];
            delete[] fNoisyChannels[iSt][iLay];
        }
        delete[] fSigProf[iSt];
        delete[] fNoisyChannels[iSt];
    }
    delete[] fNoisyChannels;
    delete[] fSigProf;

    auto canvas = new TCanvas("c1", "", 1000, 800);

    // fRawMPedCMS->Draw("HIST");
    // canvas->SaveAs("pictures/gemChannel_fRawMPedCMS.png");
    // fRaw->Draw("HIST");
    // canvas->SaveAs("pictures/gemChannel_fRaw.png");
    // fRawMPed->Draw("HIST");
    // canvas->SaveAs("pictures/gemChannel_fRawMPed.png");
    // fPed->Draw("HIST");
    // canvas->SaveAs("pictures/gemChannel_fPed.png");
    // fPedRms->Draw("HIST");
    // canvas->SaveAs("pictures/gemChannel_fPedRms.png");
    // fCMS->Draw("HIST");
    // canvas->SaveAs("pictures/gemChannel_fCMS.png");
    // fNoiseLvl->Draw("HIST");
    // canvas->SaveAs("pictures/gemChannel_fNoiseLvl.png");
    // fTreshold->Draw("HIST");
    // canvas->SaveAs("pictures/gemChannel_fTreshold.png");

    // fRawMPedCMS->SetLineColor(4);
    // fRaw->SetLineColor(38);
    // fRawMPed->SetLineColor(3);
    // fPed->SetLineColor(8);
    // fPedRms->SetLineColor(1);
    // fCMS->SetLineColor(5);
    // fNoiseLvl->SetLineColor(46);
    // fTreshold->SetLineColor(2);

    // fRawMPedCMS->Draw("HIST");
    // // fRaw->Draw("HIST SAME");
    // // fRawMPed->Draw("HIST SAME");
    // // fPed->Draw("HIST SAME");
    // // fPedRms->Draw("HIST SAME");
    // // fCMS->Draw("HIST SAME");
    // fNoiseLvl->Draw("HIST SAME");
    // fTreshold->Draw("HIST SAME");
    // canvas->SaveAs("pictures/gemChannel_ALL.png");

}

// BmnStatus BmnGemRaw2Digit::FillProfiles(TClonesArray *adc) {
//     for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
//         BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
//         UInt_t ch = adcDig->GetChannel() * adcDig->GetNSamples();
//         for (Int_t iMap = 0; iMap < fEntriesInGlobMap; ++iMap) {
//             GemMapStructure gemM = fMap[iMap];
//             if (adcDig->GetSerial() == gemM.serial && ch <= gemM.channel_high && ch >= gemM.channel_low) {
//                 ProcessDigit(adcDig, &gemM, NULL, kTRUE);
//                 break;
//             }
//         }
//     }
// }

BmnStatus BmnGemRaw2Digit::FillNoisyChannels() {
    // const Int_t kNStations = 10;
    // const Int_t kNStrips = 1300;
    // const Int_t kNStripsInBunch = 32;
    // const Int_t kNBunches = kNStrips / kNStripsInBunch;
    // const Int_t kNThresh = 3;

    // for (Int_t iSt = 0; iSt < kNStations; ++iSt)
    //     for (UInt_t iMod = 0; iMod < N_MODULES; ++iMod)
    //         for (Int_t iLay = 0; iLay < N_LAYERS; ++iLay) {
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
    //                     Double_t curr = prof->GetBinContent(strip);
    //                     Double_t next = prof->GetBinContent(strip + 1);
    //                     if (kNThresh * meanDiff < next - curr)
    //                         fNoisyChannels[iSt][iMod][iLay][strip] = kTRUE;
    //                 }
    //             }
    //         }
    return kBMNSUCCESS;
}

BmnStatus BmnGemRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray * gem, UInt_t event) {
    for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
        BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
        UInt_t ch = adcDig->GetChannel();
        for (Int_t iMap = 0; iMap < fEntriesInGlobMap; ++iMap) {
            GemMapStructure gemM = fMap[iMap];
            if (adcDig->GetSerial() == gemM.serial && ch <= gemM.channel_high && ch >= gemM.channel_low) {
                ProcessDigit(adcDig, &gemM, gem, kFALSE, event);
                break;
            }
        }
    }
    return kBMNSUCCESS;
}

void BmnGemRaw2Digit::ProcessDigit(BmnADCDigit* adcDig, GemMapStructure* gemM, TClonesArray * gem, Bool_t doFill, UInt_t event)
{
    const UInt_t nSmpl = adcDig->GetNSamples();
    UInt_t ch = adcDig->GetChannel();
    UInt_t ser = adcDig->GetSerial();

    BmnGemStripDigit candDig[nSmpl];

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        BmnGemStripDigit dig;
        dig.SetStation(gemM->station);
        dig.SetStripLayer(gemM->side);
        // Int_t strip = gemM->part * 128 + (ch - gemM->channel_low) * nSmpl + iSmpl;
        // Int_t strip = gemM->part * 128 + 128 - ((ch - gemM->channel_low) * nSmpl + iSmpl); // reverse part
        Int_t strip = gemM->part * 128 + 128 - ((ch - gemM->channel_low) * nSmpl + 32 - iSmpl);// reverse part and channels
        dig.SetStripNumber(strip);
        Double_t sig = ((Double_t) ((adcDig->GetShortValue())[iSmpl] / 16));
        dig.SetStripSignal(sig);
        candDig[iSmpl] = dig;
    }

    vector<Double_t> vSamples;
    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
    {
        vSamples.push_back((candDig[iSmpl]).GetStripSignal());
    }
    Double_t CMS = CalcCMS(gemM->station, ch, vSamples);

    Double_t*** vNoiseLvl = GetNoiseLvls();
    Double_t*** vPed = GetPedestals();
    Double_t*** vPedRMS = GetPedestalsRMS();
    Double_t baseTreshold = 5.;

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
    {
        BmnGemStripDigit* dig = &candDig[iSmpl];

        Double_t ped = vPed[gemM->station][ch][iSmpl];
        Double_t pedRms = vPedRMS[gemM->station][ch][iSmpl];
        Double_t sig = dig->GetStripSignal();
        sig -= (ped + CMS);
        Double_t noiseLvl = vNoiseLvl[gemM->station][ch][iSmpl];
        Double_t threshold = 6*noiseLvl - CMS;
        // Double_t threshold = 6*noiseLvl;
        // Double_t threshold = 3*noiseLvl;
        // threshold *=7.;

        // if (dig->GetStation() == 0 && dig->GetStripLayer() == 1 && dig->GetStripNumber() == 600)
        // {
        //     fRaw->Fill(event, dig->GetStripSignal());
        //     fRawMPed->Fill(event, dig->GetStripSignal() - ped);
        //     fRawMPedCMS->Fill(event, sig);
        //     fPed->Fill(event, ped);
        //     fPedRms->Fill(event, pedRms);
        //     fCMS->Fill(event, CMS);
        //     fNoiseLvl->Fill(event, noiseLvl);
        //     fTreshold->Fill(event, threshold);
        // }

        // if ( Abs(sig) < baseTreshold) continue;
        if ( sig < threshold ) continue;
        // if ( sig > -threshold ) continue;

        BmnGemStripDigit* savedDigit = new ((*gem)[gem->GetEntriesFast()]) BmnGemStripDigit(dig);
        savedDigit->SetStripSignal(sig);
        Bool_t isNoisy = fNoisyChannels[dig->GetStation()][dig->GetStripLayer()][dig->GetStripNumber()];
        savedDigit->SetIsGoodDigit(!isNoisy);
    }
}

ClassImp(BmnGemRaw2Digit)

