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
    Double_t baseTresholds[3] = {120., 40., 50.};
    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        auto candStation = (candDig[iSmpl]).GetStation();
        if (candStation == -1) continue;

        BmnSiliconDigit * dig = &candDig[iSmpl];
        Double_t CMS = CalcCMS(candStation, ch, vSamples);
        Double_t ped = vPed[candStation][ch][iSmpl];
        Double_t pedRms = vPedRMS[candStation][ch][iSmpl];
        Double_t sig = dig->GetStripSignal() - ped - CMS;
        Double_t noiseLvl = vNoiseLvl[candStation][ch][iSmpl];
        // Double_t threshold = 6*noiseLvl - CMS;
        // Double_t threshold = 6*noiseLvl;
        Double_t threshold = 3*noiseLvl;

        if ( Abs(sig) < baseTresholds[candStation] || (dig->GetStripLayer() == 0 && sig < threshold) || (dig->GetStripLayer() == 1 && sig > -threshold) )
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
