const bool kVERBOSE_MODE = false;

vector<Int_t> runs = {780, 802, 803, 811, 816, 817, 818, 829, 831, 832, 838, 840, 842, 843, 844, 848, 851};

const Int_t stationMap[16] = {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
const Int_t modMap[16] = {0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 0, 0, 1, 1};
const Int_t sideMap[16] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};

const Int_t nStations = 3;
const Int_t nMods = 4;
const Int_t nSides = 2;
const Int_t nChannels = 640;

map<Int_t, TH1I *> hOccupMap;
TH1D* h1Signal[3];

void SiliconDigitsAnalysisHist(UInt_t runId = 0)
{
    // --- HIST CREATION -----------------------------------------------------------------
    for (size_t iHist = 0; iHist < 16; iHist++)
    {
        TString histName = 
        Form("h1_station%d_mod%d_side%d", stationMap[iHist], modMap[iHist], sideMap[iHist]);
        TString histDiscription = 
        Form("Strips occupancies (station %d, module %d, side %d)", stationMap[iHist], modMap[iHist], sideMap[iHist]);
        Int_t key = 100 * stationMap[iHist] + 10 * modMap[iHist] + sideMap[iHist];
        TH1I *hist = new TH1I(histName, histDiscription, nChannels, 0 - 0.5, 640 - 0.5);
        hOccupMap.insert({key, hist});
    }

    h1Signal[0] = new TH1D("h1_signal_st0", "Silicon signal amplitude (station 0);AU;", 80, 0, 800);
    h1Signal[1] = new TH1D("h1_signal_st1", "Silicon signal amplitude (station 1);AU;", 80, 0, 800);
    h1Signal[2] = new TH1D("h1_signal_st2", "Silicon signal amplitude (station 2);AU;", 80, 0, 800);


    // --- OPEN FILE -----------------------------------------------------------------
    TChain *inChain;
    TTree *tree;
    TClonesArray *SiliconDigits;
    TBranch *branchSILICON;
    BmnSiliconDigit *siDigit = 0;

    if (runId < 1)
    {
        inChain = new TChain("cbmsim");
        for (auto &&iRunId : runs)
        {
            inChain->Add(Form("data/stand_run%04d_digits.root", iRunId));
        }

        SiliconDigits = 0;
        branchSILICON = inChain->GetBranch("SILICON");
        if (!branchSILICON) return;
        branchSILICON->SetAutoDelete(kTRUE);
        inChain->SetBranchAddress("SILICON", &SiliconDigits);
    }
    else
    {
        TString inFileName = Form("data/stand_run%04d_digits.root", runId);
        TFile *inFile = new TFile(inFileName);
        if (!inFile) return;
        tree = (TTree *)inFile->Get("cbmsim");

        SiliconDigits = 0;
        branchSILICON = tree->GetBranch("SILICON");
        if (!branchSILICON) return;
        branchSILICON->SetAutoDelete(kTRUE);
        tree->SetBranchAddress("SILICON", &SiliconDigits);
    }

    // --- Event loop ---------------------------------------------------------------------
    Long64_t nEvents;
    if (runId < 1)
    {
        nEvents = inChain->GetEntries();
    }
    else
    {
        nEvents = tree->GetEntries();
    }
    printf("NEvents: %lld\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        if (kVERBOSE_MODE)
        printf("\nEvent %d\n", iEv);

        branchSILICON->GetEntry(iEv);

        // Silicon digits
        for (size_t iDigit = 0; iDigit < SiliconDigits->GetEntriesFast(); iDigit++)
        {
            siDigit = (BmnSiliconDigit *)SiliconDigits->At(iDigit);

            if (!siDigit->IsGoodDigit()) continue;

            Int_t station = siDigit->GetStation();
            Int_t module = siDigit->GetModule();
            Int_t layer = siDigit->GetStripLayer();
            Int_t strip = siDigit->GetStripNumber();
            Double_t signal = siDigit->GetStripSignal();
            Int_t key = 100 * station + 10 * module + layer;

            hOccupMap.find(key)->second->Fill(strip);
            h1Signal[station]->Fill(abs(signal));
            // h1Signal[station]->Fill(signal);

            if (kVERBOSE_MODE)
            printf("<SiDigit> Station: %d, Module: %d, Layer: %d, Strip: %d, Signal: %f\n", station, module, layer, strip, signal);
        }

    } // end of event
    printf("File ended\n");

    // --- DRAW ------------------------------------------------------------------------
    // for (auto const &[key, hist] : hOccupMap)
    // {
    //     TCanvas *canvas = new TCanvas(Form("canvas%d", key), "", 1200, 600);
    //     canvas->SetLogy();
    //     hist->Draw();
    //     canvas->SaveAs(Form("pictures/run%04d_occup_mod%03d.png", runId, key));
    //     canvas->SaveAs(Form("pictures_C/run%04d_occup_mod%03d.C", runId, key));
    // }

    // for (size_t i = 0; i < 3; i++)
    // {
    //     auto cSignal = new TCanvas("cSignal", "Silicon signal", 800, 600);
    //     // cSignal->SetLogy(kTRUE);
    //     h1Signal[i]->Draw();
    //     cSignal->SaveAs(Form("pictures/run%04d_signal_st%d.png", runId, i));
    // }
    auto cSignal = new TCanvas("cSignal", "Silicon signal", 800, 600);
    h1Signal[0]->Draw();
}
