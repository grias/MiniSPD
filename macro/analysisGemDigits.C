TTree *fTreeDigits;
TBranch *fBranchGemDigits;
TClonesArray *fGemDigits;

const Int_t stationMap[4] = {0, 0, 1, 1};
const Int_t sideMap[4] = {0, 1, 0, 1};
const Int_t nHistMap[2][2] = {{0, 1}, {2, 3}};
const Int_t lowBound[4] = {240, 1, 0, 1};
const Int_t topBound[4] = {420, 1, 0, 1};
const Int_t boundaries[2][2][2] = {{{240, 420}, {260, 440}}, {{240, 400}, {260, 500}}};

map<Int_t, TH1I *> hOccupMap;
map<Int_t, TH1I *> hNDigitsMap;
TH1D* h1Signal[2];
array<TH1D*, 2> h1NDigits;

void OpenInput(UInt_t runId);
void CreateHisto();
void Analyze();
void DrawHisto(UInt_t runId);

void analysisGemDigits(UInt_t runId)
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
    fBranchGemDigits = fTreeDigits->GetBranch("GEM");
    if (!fBranchGemDigits) return;
    fBranchGemDigits->SetAutoDelete(kTRUE);
    fTreeDigits->SetBranchAddress("GEM", &fGemDigits);
}

void CreateHisto()
{
    TString histName;
    TString histDiscription;

    for (size_t iHist = 0; iHist < 4; iHist++)
    {
        Int_t key = 10 * stationMap[iHist] + sideMap[iHist];

        histName = Form("h1_occupancy_station%d_side%d", stationMap[iHist], sideMap[iHist]);
        histDiscription = Form("Strips occupancies (station %d, side %d)", stationMap[iHist], sideMap[iHist]);
        auto hist = new TH1I(histName, histDiscription, 800, 0 - 0.5, 800 - 0.5);
        hOccupMap.insert({key, hist});

        histName = Form("h1_n_digits_station%d_side%d", stationMap[iHist], sideMap[iHist]);
        histDiscription = Form("Number of digits per event (station %d, side %d);n digits;", stationMap[iHist], sideMap[iHist]);
        auto histDidits = new TH1I(histName, histDiscription, 20, -0.5, 20 -0.5);
        hNDigitsMap.insert({key, histDidits});
        
    }

    for (int iStation = 0; iStation < 2; iStation++)
    {
        histName = Form("h1_amplitude_station%d", iStation);
        histDiscription = Form("GEM signal amplitude (station %d);AU;", iStation);
        h1Signal[iStation] = new TH1D(histName, histDiscription, 300, -50, 1000);

        histName = Form("h1_n_digits_station%d", iStation);
        histDiscription = Form("Number of digits per event (station %d);n digits;", iStation);
        h1NDigits[iStation] = new TH1D(histName, histDiscription, 20, -0.5, 20 -0.5);
    }
    
}

void Analyze()
{
    // --- Event loop ---------------------------------------------------------------------
    Long64_t nEvents = fTreeDigits->GetEntries();
    printf("NEvents: %lld\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        // printf("\nEvent %d\n", iEv);
        fBranchGemDigits->GetEntry(iEv);

        array<UInt_t, 2> nDigitsInStation {0, 0};
        array<UInt_t, 4> nDigitsInStationSide {0, 0, 0, 0};

        UInt_t nDigits = fGemDigits->GetEntriesFast();
        // printf("nDigits: %d\n", nDigits);
        for (size_t iDigit = 0; iDigit < nDigits; iDigit++)
        {
            auto gemDigit = (BmnGemStripDigit *)fGemDigits->At(iDigit);

            if (!gemDigit->IsGoodDigit()) continue;

            Int_t station = gemDigit->GetStation();
            Int_t layer = gemDigit->GetStripLayer();
            Int_t strip = gemDigit->GetStripNumber();
            Double_t signal = gemDigit->GetStripSignal();
            Int_t key = 10 * station + layer;

            nDigitsInStation[station]++;

            nDigitsInStationSide[nHistMap[station][layer]]++;

            hOccupMap.find(key)->second->Fill(strip);

            if (strip < boundaries[station][layer][0] || strip > boundaries[station][layer][1]) continue;
            // if (strip < boundaries[station][layer][1]) continue;

            // h1Signal[station]->Fill(abs(signal));
            h1Signal[station]->Fill(signal);

            // printf("<SiDigit> Station: %d, Module: %d, Layer: %d, Strip: %d, Signal: %f\n", station, module, layer, strip, signal);
        }

        for (size_t iHist = 0; iHist < 4; iHist++)
        {
            Int_t key = 10 * stationMap[iHist] + sideMap[iHist];
            hNDigitsMap.find(key)->second->Fill(nDigitsInStationSide[iHist]);
        }
        

        for (size_t iStation = 0; iStation < 2; iStation++)
        {
            h1NDigits[iStation]->Fill(nDigitsInStation[iStation]);
        }
        
        
    } // end of event
}

void DrawHisto(UInt_t runId)
{
    gSystem->Exec("mkdir -p pictures");

    for (auto const &keyHistPair : hOccupMap)
    {
        auto key = keyHistPair.first;
        auto hist = keyHistPair.second;
        auto canvas = new TCanvas(Form("canvas%d", key), "", 800, 600);
        hist->Draw();
        // canvas->SetLogy();
        canvas->SaveAs(Form("pictures/run%04d_digits_gem_occupancy_mod%02d.png", runId, key));
        canvas->SaveAs(Form("pictures_C/run%04d_digits_gem_occupancy_mod%02d.C", runId, key));
    }

    for (auto const &keyHistPair : hNDigitsMap)
    {
        auto key = keyHistPair.first;
        auto hist = keyHistPair.second;
        auto cNDigitsStSide = new TCanvas(Form("cNDigitsStSide%d", key), "", 800, 600);
        hist->Draw();
        // cNDigitsStSide->SetLogy();
        cNDigitsStSide->SaveAs(Form("pictures/run%04d_digits_gem_nDigits_mod%02d.png", runId, key));
    }

    for (Int_t iStation = 0; iStation < 2; iStation++)
    {
        auto cSignal = new TCanvas(Form("c_signal_st%d", iStation), "GEM signal", 600, 800);
        // cSignal->SetLogy();
        h1Signal[iStation]->Draw();
        cSignal->SaveAs(Form("pictures/run%04d_digits_gem_amplitude_station%d.png", runId, iStation));
    }

    for (Int_t iStation = 0; iStation < 2; iStation++)
    {
        auto cNDigits = new TCanvas(Form("c_n_digits_st%d", iStation), "GEM n digits", 600, 800);
        // cNDigits->SetLogy();
        h1NDigits[iStation]->Draw();
        cNDigits->SaveAs(Form("pictures/run%04d_digits_gem_nDigits_station%d.png", runId, iStation));
    }
}
