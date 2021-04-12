TTree *fTreeDigits;
TBranch *fBranchSiDigits;
TClonesArray *fSiliconDigits;

const Int_t stationMap[16] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
const Int_t modMap[16] = {0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 0, 0, 1, 1};
const Int_t sideMap[16] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};

map<Int_t, TH1I *> hOccupMap;
TH1D* h1Signal[3];

void OpenInput(UInt_t runId);
void CreateHisto();
void Analyze();
void DrawHisto(UInt_t runId);

void analysisSiliconDigits(UInt_t runId)
{
    OpenInput(runId);
    CreateHisto();
    Analyze();
    DrawHisto(runId);
}

void OpenInput(UInt_t runId)
{
    TString inDigitsFileName = Form("data/stand_run%04d_digits.root", runId);
    TFile *inDigitsFile = new TFile(inDigitsFileName);
    if (!inDigitsFile) return;
    fTreeDigits = (TTree *)inDigitsFile->Get("cbmsim");
    fBranchSiDigits = fTreeDigits->GetBranch("SILICON");
    if (!fBranchSiDigits) return;
    fBranchSiDigits->SetAutoDelete(kTRUE);
    fTreeDigits->SetBranchAddress("SILICON", &fSiliconDigits);
}

void CreateHisto()
{
    TString histName;
    TString histDiscription;

    for (size_t iHist = 0; iHist < 16; iHist++)
    {
        histName = Form("h1_occupancy_station%d_mod%d_side%d", stationMap[iHist], modMap[iHist], sideMap[iHist]);
        histDiscription = Form("Strips occupancies (station %d, module %d, side %d)", stationMap[iHist], modMap[iHist], sideMap[iHist]);
        Int_t key = 100 * stationMap[iHist] + 10 * modMap[iHist] + sideMap[iHist];
        auto hist = new TH1I(histName, histDiscription, 640, 0 - 0.5, 640 - 0.5);
        hOccupMap.insert({key, hist});
    }

    for (int iStation = 0; iStation < 3; iStation++)
    {
        histName = Form("h1_amplitude_station%d", iStation);
        histDiscription = Form("Silicon signal amplitude (station %d);AU;", iStation);
        h1Signal[iStation] = new TH1D(histName, histDiscription, 80, 0, 800);
    }
}

void Analyze()
{
    int goodEvents = 0;

    // --- Event loop ---------------------------------------------------------------------
    Long64_t nEvents = fTreeDigits->GetEntries();
    printf("NEvents: %lld\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        // printf("\nEvent %d\n", iEv);
        fBranchSiDigits->GetEntry(iEv);

        array<int, 3> stationHasActiveStrips {0, 0, 0};
        int moduleHasActiveStrips[3][4] = {
            {0,0,0,0},
            {0,0,0,0},
            {0,0,0,0}
        };

        UInt_t nDigits = fSiliconDigits->GetEntriesFast();
        // printf("nDigits: %d\n", nDigits);
        for (size_t iDigit = 0; iDigit < nDigits; iDigit++)
        {
            auto siDigit = (BmnSiliconDigit *)fSiliconDigits->At(iDigit);

            if (!siDigit->IsGoodDigit()) continue;

            Int_t station = siDigit->GetStation();
            Int_t module = siDigit->GetModule();
            Int_t layer = siDigit->GetStripLayer();
            Int_t strip = siDigit->GetStripNumber();
            Double_t signal = siDigit->GetStripSignal();
            Int_t key = 100 * station + 10 * module + layer;

            if (layer == 0)
            {
                moduleHasActiveStrips[station][module] += moduleHasActiveStrips[station][module] % 10 == 0 ? 1 : 0;
            }
            else
            {
                moduleHasActiveStrips[station][module] += moduleHasActiveStrips[station][module] / 10 == 0 ? 10 : 0;
            }

            hOccupMap.find(key)->second->Fill(strip);
            h1Signal[station]->Fill(abs(signal));

            // printf("<SiDigit> Station: %d, Module: %d, Layer: %d, Strip: %d, Signal: %f\n", station, module, layer, strip, signal);
        }

        for (size_t iMod = 0; iMod < 4; iMod++)
        {
            if (moduleHasActiveStrips[0][iMod] == 11)
                stationHasActiveStrips[0] = 1;

            if (moduleHasActiveStrips[1][iMod] == 11)
                stationHasActiveStrips[1] = 1;

            if (moduleHasActiveStrips[2][iMod] == 11)
                stationHasActiveStrips[2] = 1;
        }

        if (stationHasActiveStrips[0] && stationHasActiveStrips[1] && stationHasActiveStrips[2])
            goodEvents++;
    } // end of event

    printf("Good events %d/%lld\n", goodEvents, nEvents);
}

void DrawHisto(UInt_t runId)
{
    for (auto const &keyHistPair : hOccupMap)
    {
        auto key = keyHistPair.first;
        auto hist = keyHistPair.second;
        auto canvas = new TCanvas(Form("canvas%d", key), "", 800, 600);
        hist->Draw();
        canvas->SetLogy();
        canvas->SaveAs(Form("pictures/run%04d_digits_si_occupancy_mod%03d.png", runId, key));
        canvas->SaveAs(Form("pictures_C/run%04d_digits_si_occupancy_mod%03d.C", runId, key));
    }

    for (Int_t iStation = 0; iStation < 3; iStation++)
    {
        auto cSignal = new TCanvas(Form("c_signal_st%d", iStation), "Silicon signal", 600, 800);
        h1Signal[iStation]->Draw();
        cSignal->SaveAs(Form("pictures/run%04d_digits_si_amplitude_station%d.png", runId, iStation));
    }
}
