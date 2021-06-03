TTree *fTreeHits;
TBranch *fBranchGemHits;
TClonesArray *fGemHits;

const Int_t stationMap[2] = {0, 1};
const Int_t modMap[2] = {0, 0};

map<Int_t, TH2D *> hOccupModuleMap;
map<Int_t, TH1D *> hOccupModuleXMap;
vector<TH2D *>hOccupStationVector;
map<Int_t, TH2D *> hAmplitudesCorrelMap;
TH1I* hClustersSizeX;
TH1I* hClustersSizeY;
array<array<TH1D *, 4>, 2> hClusterAmplitude;

void OpenInput(UInt_t runId);
void CreateHisto();
void Analyze();
void DrawHisto(UInt_t runId);

void analysisGemHits(UInt_t runId)
{
    OpenInput(runId);
    CreateHisto();
    Analyze();
    DrawHisto(runId);
}

void OpenInput(UInt_t runId)
{
    TString inHitsFileName = Form("data/stand_run%04d_hits.root", runId);
    TFile *inHitsFile = new TFile(inHitsFileName);
    if (!inHitsFile) return;
    fTreeHits = (TTree *)inHitsFile->Get("cbmsim");
    fBranchGemHits = fTreeHits->GetBranch("GemHits");
    if (!fBranchGemHits) return;
    fBranchGemHits->SetAutoDelete(kTRUE);
    fTreeHits->SetBranchAddress("GemHits", &fGemHits);
}

void CreateHisto()
{
    TString histName;
    TString histDiscription;
    TH2D *hist;
    TH1D *hist1d;
    for (size_t iHist = 0; iHist < 2; iHist++)
    {
        Int_t key = 10 * stationMap[iHist] + 1 * modMap[iHist];

        Int_t nBins = 150;

        // occupancy
        histName = Form("h2_station%d_mod%d_occupancy", stationMap[iHist], modMap[iHist]);
        histDiscription = Form("Module occupancy (station %d, module %d);X [mm];Y [mm]", stationMap[iHist], modMap[iHist]);
        hist = new TH2D(histName, histDiscription, nBins, -10, 800, nBins, -100, 800);
        hOccupModuleMap.insert({key, hist});

        // occupancyX
        histName = Form("h1_station%d_mod%d_occupancy_x", stationMap[iHist], modMap[iHist]);
        histDiscription = Form("Module occupancy X (station %d, module %d);X [mm]; count", stationMap[iHist], modMap[iHist]);
        hist1d = new TH1D(histName, histDiscription, nBins, -100, 800);
        hOccupModuleXMap.insert({key, hist1d});

        // Amplitudes
        histName = Form("h2_station%d_mod%d_amplitudes", stationMap[iHist], modMap[iHist]);
        histDiscription = Form("Amplitudes (station %d, module %d);Cluster X amplitude [AU];Cluster Y amplitude [AU]", stationMap[iHist], modMap[iHist]);
        hist = new TH2D(histName, histDiscription, 120, 0, 1200, 120, 0, 1200);
        hAmplitudesCorrelMap.insert({key, hist});
    }

    for (int iStation = 0; iStation < 2; iStation++)
    {
        histName = Form("h2_station%d_occupancy", iStation);
        histDiscription = Form("Station %d occupancy;X [mm];Y [mm]", iStation);
        hist = new TH2D(histName, histDiscription, 150, 0, 1000, 150, 200, -500);
        hOccupStationVector.push_back(hist);
    }
    
    hClustersSizeX = new TH1I("h1_clusters_x", "Cluster size;n strips", 19, 1-0.5, 20-0.5);
    hClustersSizeY = new TH1I("h1_clusters_y", "Cluster size;n strips", 19, 1-0.5, 20-0.5);

    for (int iStation = 0; iStation < 2; iStation++)
    for (int iClusterSize = 0; iClusterSize < 4; iClusterSize++)
    {
        histName = Form("h2_amplitude_size%d_station%d", iClusterSize+1, iStation);
        histDiscription = Form("Cluster amplitude (station %d);amplitude [AU]", iStation);
        hClusterAmplitude[iStation][iClusterSize] = new TH1D(histName, histDiscription, 100, 0, 1000);
    }

}

void Analyze()
{
    Long64_t nEvents = fTreeHits->GetEntries();
    printf("NEvents: %lld\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        fBranchGemHits->GetEntry(iEv);
        Int_t nHits = fGemHits->GetEntriesFast();
        // printf("\nEvent %d\tnHits: %d\n", iEv, nHits);
        if (nHits > 2) continue;
        for (size_t iHit = 0; iHit < nHits; iHit++)
        {
            auto siHit = (StandSiliconHit *)fGemHits->At(iHit);

            Int_t station = siHit->GetStation();
            Int_t module = siHit->GetModule();
            Double_t localX = siHit->GetLocalX();
            Double_t localY = siHit->GetLocalY();
            Double_t amplitudeX = std::abs(siHit->GetAmplitudeX());
            Double_t amplitudeY = std::abs(siHit->GetAmplitudeY());
            Double_t clusterSizeX = siHit->GetClusterSizeX();
            Double_t clusterSizeY = siHit->GetClusterSizeY();

            if (!StandGemGeoMapper::fIsActiveModule[station]) continue;

            // if (clusterSizeX > 1 || clusterSizeY > 1) continue;
            

            Int_t key = 10 * station + 1 * module;
            hOccupModuleMap.find(key)->second->Fill(localX, localY);

            hOccupModuleXMap.find(key)->second->Fill(localX);

            hAmplitudesCorrelMap.find(key)->second->Fill(amplitudeX, amplitudeY);
            hClustersSizeX->Fill(clusterSizeX);
            hClustersSizeY->Fill(clusterSizeY);

            TVector3 globalHitPos = StandGemGeoMapper::CalculateGlobalCoordinatesForHit(station, module, localX, localY);

            hOccupStationVector[station]->Fill(globalHitPos.X(), globalHitPos.Y());

            hClusterAmplitude[station][clusterSizeX-1]->Fill(amplitudeX);
            hClusterAmplitude[station][clusterSizeY-1]->Fill(amplitudeY);
            
            // siHit->Print();
        } // end of hit
    } // end of event
}

void DrawHisto(UInt_t runId)
{
    gSystem->Exec("mkdir -p pictures");

    for (auto &&pair : hOccupModuleMap)
    {
        auto key = pair.first;
        auto hist = pair.second;

        auto canvas = new TCanvas(Form("canvas%d", key), "", 630, 630);

        hist->Draw("COLZ");
        canvas->SaveAs(Form("pictures/run%04d_hits_gem_occupancy_mod%02d.png", runId, key));
        delete canvas;
    }

    for (auto &&pair : hOccupModuleXMap)
    {
        auto key = pair.first;
        auto hist = pair.second;

        auto canvas = new TCanvas(Form("canvas%d", key), "", 630, 630);
        hist->Draw("");
        canvas->SaveAs(Form("pictures/run%04d_hits_gem_occupancy_x_mod%02d.png", runId, key));
        delete canvas;
    }

    for (auto &&pair : hAmplitudesCorrelMap)
    {
        auto key = pair.first;
        auto hist = pair.second;

        auto canvas = new TCanvas(Form("canvas%d", key), "", 630, 630);
        hist->Draw("COLZ");
        canvas->SaveAs(Form("pictures/run%04d_hits_gem_amplitude_correlation_mod%02d.png", runId, key));
        delete canvas;
    }

    Int_t iStation = 0;
    for (auto &&hist : hOccupStationVector)
    {
        auto canvas = new TCanvas(Form("canvas%d", iStation), "", 630, 630);
        hist->Draw("COL");
        canvas->SaveAs(Form("pictures/run%04d_hits_gem_occupancy_station%d.png", runId, iStation++));
        delete canvas;
    }

    auto canvas = new TCanvas("canvas", "", 600, 800);
    hClustersSizeX->SetLineColor(2);
    hClustersSizeY->SetLineColor(9);
    hClustersSizeX->Draw("HIST");
    hClustersSizeY->Draw("SAME HIST");
    canvas->SetLogy();
    canvas->SaveAs(Form("pictures/run%04d_hits_gem_cluster_size.png", runId));
    canvas->SetLogy(kFALSE);
    canvas->Clear();

    array<Int_t, 4> colors {2, 9, 8, 6};
    for (int iStation = 0; iStation < 2; iStation++)
    {
        for (int iClusterSize = 0; iClusterSize < 4; iClusterSize++)
        {
            hClusterAmplitude[iStation][iClusterSize]->SetLineColor(colors[iClusterSize]);
            hClusterAmplitude[iStation][iClusterSize]->Draw(!iClusterSize?"HIST":"HIST SAME");
        }
        // canvas->SetLogy();
        canvas->SaveAs(Form("pictures/run%04d_hits_gem_amplitude_station%d.png", runId, iStation));
        // canvas->SetLogy(kFALSE);
        canvas->Clear();
    }
}
