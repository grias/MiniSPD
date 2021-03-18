const bool kVERBOSE_MODE = false;

// vector<Int_t> runs = {780, 802, 803, 811, 816, 817, 818, 829, 831, 832, 838, 840, 842, 843, 844, 848, 851};
vector<Int_t> runs = {816, 780};
// vector<Int_t> runs = {816};

const Int_t stationMap[8] = {0, 0, 1, 1, 1, 1, 2, 2};
const Int_t modMap[8] = {0, 1, 0, 1, 2, 3, 0, 1};

map<Int_t, TH2D *> hOccupMap;
map<Int_t, TH2D *> hAmplitudesMap;
TH1I* hClustersSizeX;
TH1I* hClustersSizeY;
TH1D* hAmplitude1;
TH1D* hAmplitude2;

vector<TH2D *>hStationOccups;

void SiliconHitsAnalysisHist(UInt_t runId = 0, Int_t saveImages = 0)
{
    // --- HIST CREATION -----------------------------------------------------------------
    TString histName;
    TString histDiscription;
    TH2D *hist;
    for (size_t iHist = 0; iHist < 8; iHist++)
    {
        Int_t key = 10 * stationMap[iHist] + 1 * modMap[iHist];

        // occupancy
        histName = Form("h2_station%d_mod%d_occupancy", stationMap[iHist], modMap[iHist]);
        histDiscription = Form("Module occupancy (station %d, module %d);X [mm];Y [mm]", stationMap[iHist], modMap[iHist]);
        if (key / 10 == 1)
        {
            hist = new TH2D(histName, histDiscription, 630, 0, 63, 630, 0, 63);
        }
        else
        {
            hist = new TH2D(histName, histDiscription, 630, 0, 63, 1260, 0, 126);
        }
        hOccupMap.insert({key, hist});

        // Amplitudes
        histName = Form("h2_station%d_mod%d_amplitudes", stationMap[iHist], modMap[iHist]);
        histDiscription = Form("Amplitudes (station %d, module %d);Cluster X amplitude [ae];Cluster Y amplitude [ae]", stationMap[iHist], modMap[iHist]);
        hist = new TH2D(histName, histDiscription, 120, 0, 1200, 120, 0, 1200);
        hAmplitudesMap.insert({key, hist});
    }

    for (int iStation = 0; iStation < 3; iStation++)
    {
        histName = Form("h2_station%d_occupancy", iStation);
        histDiscription = Form("Station %d occupancy;X [mm];Y [mm]", iStation);
        hist = new TH2D(histName, histDiscription, 100, 200, 200, 100, -200, 200);
        hStationOccups.push_back(hist);
    }
    

    hClustersSizeX = new TH1I("h1_clusters_x", "Cluster size;n strips", 19, 1-0.5, 20-0.5);
    hClustersSizeY = new TH1I("h1_clusters_y", "Cluster size;n strips", 19, 1-0.5, 20-0.5);

    hAmplitude1 = new TH1D("h1_amplitude1", "Amplitude for clusters with size 1;amplitude [ae]", 120, 0, 1200);
    hAmplitude2 = new TH1D("h1_amplitude2", "Amplitude for clusters with size 2;amplitude [ae]", 120, 0, 1200);

    // --- Input -----------------------------------------------------------------
    TChain *inChain;
    TTree *tree;
    TClonesArray *SiliconHits = 0;
    TBranch *branchSILICON;
    StandSiliconHit *siHit = 0;

    if (runId < 1)
    {
        inChain = new TChain("cbmsim");
        for (auto &&iRunId : runs)
        {
            TString inFileName = Form("data/stand_run%04d_hits.root", iRunId);
            inChain->Add(inFileName);
        }

        branchSILICON = inChain->GetBranch("SiliconHits");
        if (!branchSILICON)
            return;
        branchSILICON->SetAutoDelete(kTRUE);
        inChain->SetBranchAddress("SiliconHits", &SiliconHits);
    }
    else
    {
        TString inFileName = Form("data/stand_run%04d_hits.root", runId);
        TFile *inFile = new TFile(inFileName);
        if (!inFile)
            return;
        tree = (TTree *)inFile->Get("cbmsim");

        branchSILICON = tree->GetBranch("SiliconHits");
        if (!branchSILICON)
            return;
        branchSILICON->SetAutoDelete(kTRUE);
        tree->SetBranchAddress("SiliconHits", &SiliconHits);
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

        // Silicon hits
        Int_t nHits = SiliconHits->GetEntriesFast();
        if (kVERBOSE_MODE)
            cout << "nHits: " << nHits << endl;
        for (size_t iHit = 0; iHit < nHits; iHit++)
        {
            siHit = (StandSiliconHit *)SiliconHits->At(iHit);

            Int_t station = siHit->GetStation();
            Int_t module = siHit->GetModule();
            Double_t localX = siHit->GetLocalX();
            Double_t localY = siHit->GetLocalY();
            Double_t amplitudeX = std::abs(siHit->GetAmplitudeX());
            Double_t amplitudeY = std::abs(siHit->GetAmplitudeY());
            Double_t clusterSizeX = siHit->GetClusterSizeX();
            Double_t clusterSizeY = siHit->GetClusterSizeY();

            Int_t key = 10 * station + 1 * module;
            hOccupMap.find(key)->second->Fill(localX, localY);
            hAmplitudesMap.find(key)->second->Fill(amplitudeX, amplitudeY);
            hClustersSizeX->Fill(clusterSizeX);
            hClustersSizeY->Fill(clusterSizeY);

            TVector3 globalHitPos = StandSiliconGeoMapper::CalculateGlobalCoordinatesForHit(station, module, localX, localY);

            hStationOccups[station]->Fill(globalHitPos.X(), globalHitPos.Y());

            if (clusterSizeX == 1)
            {
                hAmplitude1->Fill(amplitudeX);
            }
            else if (clusterSizeX == 2)
            {
                hAmplitude2->Fill(amplitudeX);
            }

            if (clusterSizeY == 1)
            {
                hAmplitude1->Fill(amplitudeY);
            }
            else if (clusterSizeY == 2)
            {
                hAmplitude2->Fill(amplitudeY);
            }
            
            if (kVERBOSE_MODE)
                printf("<StandSiliconHit> Station: %d, Module: %d, Local: (%f, %f)\n", station, module, localX, localY);
        }

    } // end of event

    // --- OUTPUT ------------------------------------------------------------------------
    TFile *outputFile = new TFile(Form("root_files/run%04d_si_occup.root", runId), "recreate");

    for (auto &&pair : hOccupMap)
    {
        auto key = pair.first;
        auto hist = pair.second;

        TCanvas *canvas;
        if (key / 10 == 1)
        {
            canvas = new TCanvas(Form("canvas%d", key), "", 630, 630);
        }
        else
        {
            canvas = new TCanvas(Form("canvas%d", key), "", 630, 1260);
        }
        hist->Write();
        hist->Draw("COLZ");
        if (saveImages)
        {
            canvas->SaveAs(Form("pictures/run%04d_si_occup_mod%02d.png", runId, key));
        }
        
        delete canvas;

    }

    for (auto &&pair : hAmplitudesMap)
    {
        auto key = pair.first;
        auto hist = pair.second;

        TCanvas *canvas = new TCanvas(Form("canvas%d", key), "", 630, 630);
        hist->Write();
        hist->Draw("COLZ");
        if (saveImages)
        {
            canvas->SaveAs(Form("pictures/run%04d_si_ampl_mod%02d.png", runId, key));
        }
        delete canvas;
    }

    Int_t iStation = 0;
    for (auto &&hist : hStationOccups)
    {
        TCanvas *canvas = new TCanvas(Form("canvas%d", iStation), "", 630, 630);
        hist->Draw("COL");
        hist->Write();
        if (saveImages)
        {
            canvas->SaveAs(Form("pictures/run%04d_si_station%d_occup.png", runId, iStation++));
        }
        delete canvas;
    }
    

    TCanvas *canvas = new TCanvas("canvas", "", 630, 630);
    hClustersSizeX->Write();
    hClustersSizeY->Write();
    hClustersSizeX->SetLineColor(2);
    hClustersSizeY->SetLineColor(9);
    canvas->SetLogy(kTRUE);
    hClustersSizeX->Draw("HIST");
    hClustersSizeY->Draw("SAME HIST");
    if (saveImages)
    {
        canvas->SaveAs(Form("pictures/run%04d_si_clusters.png", runId));
    }

    canvas->SetLogy(kFALSE);

    auto hAmplitudeSum = new TH1D("h1_amplitude_sum", "Cluster amplitude;amplitude [ae]", 120, 0, 1200);

    hAmplitudeSum->Add(hAmplitude1);
    hAmplitudeSum->Add(hAmplitude2);
    hAmplitudeSum->SetLineColor(1);
    hAmplitude1->SetLineColor(2);
    hAmplitude2->SetLineColor(9);
    hAmplitudeSum->Draw("HIST");
    hAmplitude1->Draw("HIST SAME");
    hAmplitude2->Draw("HIST SAME");

    if (saveImages)
    {
        canvas->SaveAs(Form("pictures/run%04d_si_amplitude.png", runId));
    }


    outputFile->Write();
    outputFile->Close();
}
