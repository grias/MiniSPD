#include "BmnSiliconRaw2Digit.h"

#include "BmnSiliconDigit.h"

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit()
{
    fMapFileName = "";
}

BmnSiliconRaw2Digit::BmnSiliconRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer) 
: BmnAdcProcessor(period, run, "SILICON", ADC_N_CHANNELS, ADC128_N_SAMPLES, vSer)
{
    fMapFileName = Form("SILICON_map_run%d.txt", period);
    ReadMapFile();
    
    fNoisyChannels = new Bool_t***[N_SI_STATIONS];
    for (Int_t iSt = 0; iSt < N_SI_STATIONS; ++iSt) {
        fNoisyChannels[iSt] = new Bool_t**[N_SI_MODULES];
        for (UInt_t iMod = 0; iMod < N_SI_MODULES; ++iMod) {
            fNoisyChannels[iSt][iMod] = new Bool_t*[N_SI_LAYERS];
            for (Int_t iLay = 0; iLay < N_SI_LAYERS; ++iLay) {
                fNoisyChannels[iSt][iMod][iLay] = new Bool_t[N_SI_STRIPS];
                for (Int_t iStrip = 0; iStrip < N_SI_STRIPS; ++iStrip)
                    fNoisyChannels[iSt][iMod][iLay][iStrip] = kFALSE;
            }
        }
    }
}

BmnSiliconRaw2Digit::~BmnSiliconRaw2Digit()
{
    for (Int_t iSt = 0; iSt < N_SI_STATIONS; ++iSt) {
        for (UInt_t iMod = 0; iMod < N_SI_MODULES; ++iMod) {
            for (Int_t iLay = 0; iLay < N_SI_LAYERS; ++iLay) {
                delete[] fNoisyChannels[iSt][iMod][iLay];
            }
            delete[] fNoisyChannels[iSt][iMod];
        }
        delete[] fNoisyChannels[iSt];
    }
    delete[] fNoisyChannels;    
}

BmnStatus BmnSiliconRaw2Digit::ReadMapFile()
{
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

    for (size_t iLine = 0; iLine < 4; iLine++)
        getline(inFile, dummy); //comment lines in input file

    while (!inFile.eof())
    {
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
    return kBMNSUCCESS;
}

BmnStatus BmnSiliconRaw2Digit::FillEvent(TClonesArray *adc, TClonesArray *silicon, Int_t iEv)
{
    for (auto it : fMap)
        for (Int_t iAdc = 0; iAdc < adc->GetEntriesFast(); ++iAdc) {
            BmnADCDigit* adcDig = (BmnADCDigit*) adc->At(iAdc);
            if (adcDig->GetSerial() == it.serial && (adcDig->GetChannel() >= it.channel_low && adcDig->GetChannel() <= it.channel_high)) {
                ProcessDigit(adcDig, &it, silicon, kFALSE, iEv);
            }
        }
    return kBMNSUCCESS;
}

BmnStatus BmnSiliconRaw2Digit::FillNoisyChannels()
{
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

    while (!inFile.eof())
    {
        inFile >> std::dec >> noisStat >> noisMod >> noisLay >> noisStrip;
        if (!inFile.good()) break;

        fNoisyChannels[noisStat][noisMod][noisLay][noisStrip] = kTRUE;
    }
    return kBMNSUCCESS;
}

void BmnSiliconRaw2Digit::ProcessDigit(BmnADCDigit* adcDig, BmnSiliconMapping* silM, TClonesArray *silicon, Bool_t doFill, Int_t iEv)
{
    const UInt_t nSmpl = adcDig->GetNSamples();
    UInt_t ch = adcDig->GetChannel();

    BmnSiliconDigit candDig[nSmpl];

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl) {
        BmnSiliconDigit dig;
        dig.SetStation(silM->station);
        dig.SetModule(silM->module);
        dig.SetStripLayer(silM->layer);
        dig.SetStripNumber((ch - silM->channel_low) * nSmpl + 1 + iSmpl);
        Double_t sig = (Double_t) ((adcDig->GetShortValue())[iSmpl] / 16);
        dig.SetStripSignal(sig);
        candDig[iSmpl] = dig;
    }

    vector<Double_t> vSamples;
    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
    {
        vSamples.push_back((candDig[iSmpl]).GetStripSignal());
    }
    Double_t CMS = CalcCMS(silM->station, ch, vSamples);

    Double_t*** vNoiseLvl = GetNoiseLvls();
    Double_t*** vPed = GetPedestals();
    Double_t baseTresholds[3] = {120., 40., 50.};

    for (Int_t iSmpl = 0; iSmpl < nSmpl; ++iSmpl)
    {
        BmnSiliconDigit * dig = &candDig[iSmpl];
        
        Double_t ped = vPed[silM->station][ch][iSmpl];
        Double_t sig = dig->GetStripSignal() - ped - CMS;
        Double_t noiseLvl = vNoiseLvl[silM->station][ch][iSmpl];
        // Double_t threshold = 6*noiseLvl - CMS;
        Double_t threshold = 6*noiseLvl;

        if ( abs(sig) < baseTresholds[silM->station] || (dig->GetStripLayer() == 0 && sig < threshold) || (dig->GetStripLayer() == 1 && sig > -threshold) )
        {
            continue;
        }

        BmnSiliconDigit * resDig = new((*silicon)[silicon->GetEntriesFast()]) BmnSiliconDigit(dig->GetStation(), dig->GetModule(), dig->GetStripLayer(), dig->GetStripNumber(), sig);
        
        Bool_t isNoisy = fNoisyChannels[dig->GetStation()][dig->GetModule()][dig->GetStripLayer()][dig->GetStripNumber() + 1];
        resDig->SetIsGoodDigit(!isNoisy);
    }
}

ClassImp(BmnSiliconRaw2Digit)
