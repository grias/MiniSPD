TTree *fTreeHits;
TBranch *fBranchGemHits;
TClonesArray *fGemHits;
TTree *fTreeTracks;
TBranch *fBranchSiTracks;
TClonesArray *fSiliconTracks;

const Int_t stationMap[2] = {0, 1};
const Int_t modMap[2] = {0, 0};

map<Int_t, TH2D *> hOccupModuleMap;
map<Int_t, TH1D *> hOccupModuleXMap;
vector<TH2D *>hOccupStationVector;
map<Int_t, TH2D *> hAmplitudesCorrelMap;
TH1I* hClustersSizeX;
TH1I* hClustersSizeY;
array<array<TH1D *, 4>, 2> hClusterAmplitude;
array<TH2D *, 2> hResVsX;

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
    // --- Input Si tracks -----------------------------------------------------------------
    TString inTracksFileName = Form("data/stand_run%04d_tracks.root", runId);
    TFile *inTracksFile = new TFile(inTracksFileName);
    if (!inTracksFile) return;
    fTreeTracks = (TTree *)inTracksFile->Get("cbmsim");
    fBranchSiTracks = fTreeTracks->GetBranch("SiliconTracks");
    if (!fBranchSiTracks) return;
    fBranchSiTracks->SetAutoDelete(kTRUE);
    fTreeTracks->SetBranchAddress("SiliconTracks", &fSiliconTracks);

    // --- Input GEM hits -----------------------------------------------------------------
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

    for (Int_t iHist = 0; iHist < 2; iHist++)
    {
        histName = Form("h2_station%d_residuals_rxx", iHist);
        histDiscription = Form("ResX vs X (station %d); res X [mm]; X [mm]", iHist);
        hResVsX[iHist] = new TH2D(histName, histDiscription, 300, -2.5, 2.5, 20, 180, 380);
        // hResVsX[iHist] = new TH2D(histName, histDiscription, 500, -10, 10, 20, 180, 380);
        // hResVsX[iHist] = new TH2D(histName, histDiscription, 100, -0.5, 0.5, 10, 180, 380);
    }

}

void Analyze()
{
    Long64_t nEvents = fTreeHits->GetEntries();
    printf("NEvents: %lld\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        fBranchGemHits->GetEntry(iEv);
        fBranchSiTracks->GetEntry(iEv);
        Int_t nHits = fGemHits->GetEntriesFast();

        // printf("\nEvent %d\tnHits: %d\n", iEv, nHits);
        if (nHits > 2) continue;
        for (size_t iHit = 0; iHit < nHits; iHit++)
        {
            auto gemHit = (StandSiliconHit *)fGemHits->At(iHit);

            Int_t station = gemHit->GetStation();
            Int_t module = gemHit->GetModule();
            Double_t localX = gemHit->GetLocalX();
            Double_t localY = gemHit->GetLocalY();
            Double_t amplitudeX = std::abs(gemHit->GetAmplitudeX());
            Double_t amplitudeY = std::abs(gemHit->GetAmplitudeY());
            Double_t clusterSizeX = gemHit->GetClusterSizeX();
            Double_t clusterSizeY = gemHit->GetClusterSizeY();

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
            
        } // end of hit

        // Residuals calculation    
        if (!fSiliconTracks->GetEntriesFast()) continue;
        auto siTrack = (StandSiliconTrack *)fSiliconTracks->At(0);
        auto par0 = siTrack->GetParameterX(0);
        auto par1 = siTrack->GetParameterX(1);
        for (size_t iHit = 0; iHit < nHits; iHit++)
        {
            auto gemHit = (StandSiliconHit *)fGemHits->At(iHit);
            Int_t station = gemHit->GetStation();
            Int_t module = gemHit->GetModule();
            Double_t localX = gemHit->GetLocalX();
            Double_t localY = gemHit->GetLocalY();
            TVector3 globalHitPos = StandGemGeoMapper::CalculateGlobalCoordinatesForHit(station, module, localX, localY);
            Double_t trackX = globalHitPos.Z()*par1 + par0;
            Double_t residX = globalHitPos.X() - trackX;
            // printf("St: %d, ResidX: %f\n", station, residX);
            hResVsX[station]->Fill(residX, localX);
        } // end of hit
    } // end of event
}

void DrawHisto(UInt_t runId)
{
    gSystem->Exec("mkdir -p pictures");

    gStyle->SetOptFit();

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

    // 2D histo
    auto cResVsX = new TCanvas("c_resvsx", "", 800, 800);
    for (size_t iHist = 0; iHist < 2; iHist++)
    {
        hResVsX[iHist]->Draw("COL");
        cResVsX->SaveAs(Form("pictures/run%04d_hits_gem_resxvsx_st%zu.png", runId, iHist));
    }

    // profile
    for (size_t iHist = 0; iHist < 2; iHist++)
    {
        auto profile = hResVsX[iHist]->ProfileY();
        profile->GetYaxis()->SetTitle("Res X [mm]");
        profile->Draw();
        cResVsX->SaveAs(Form("pictures/run%04d_hits_gem_resxvsx_st%zu_profile.png", runId, iHist));
    }

    // projection
    TF1 *fitFun = new TF1("gausWithBackground","pol0(0)+gaus(1)",-0.2,0.2);
    fitFun->SetParName(0,"Base line");
    fitFun->SetParName(1,"Height");
    fitFun->SetParName(2,"Mean");
    fitFun->SetParName(3,"Sigma");
    fitFun->SetParameters(0,1,0,0.2);

    TF1 *gausFixMp = new TF1("gausWithFixedMP","gaus(0)",-0.01,0.5);
    gausFixMp->SetParName(0,"Height");
    gausFixMp->SetParName(1,"Mean");
    gausFixMp->SetParName(2,"Sigma");
    gausFixMp->SetParameters(1,0,0.2);
    gausFixMp->FixParameter(1, 0);
    
    // cResVsX->SetLogy(kTRUE);

    array<Double_t, 8> fitMeanArray {0, 0, 0, 0, 0, 0, 0, 0};
    array<Double_t, 8> fitSigmaArray {0, 0, 0, 0, 0, 0, 0, 0};
    for (size_t iHist = 0; iHist < 2; iHist++)
    {
        auto projection = hResVsX[iHist]->ProjectionX();
        // auto fitRes = projection->Fit("pol0", "SCQ+");
        // auto fitRes = projection->Fit("gaus", "SQ+", "", -0.2, 0.2);
        auto fitRes = projection->Fit("gaus", "SQ+", "");
        // auto fitRes = projection->Fit(fitFun, "SQ", "", -0.2, 0.2);
        
        if ((Int_t) fitRes == 0)
        {
            fitMeanArray[iHist] = fitRes->Parameter(1);
            fitSigmaArray[iHist] = fitRes->Parameter(2);
        }
        projection->Draw();
        cResVsX->SaveAs(Form("pictures/run%04d_hits_gem_resxvsx_st%zu_projection.png", runId, iHist));
    }

    printf("\nFit results:\n");
    printf("Module\tMean [mkm]\tSigma [mkm]\n");
    for (size_t iHist = 0; iHist < 2; iHist++)
    {
        printf("%zu\t%f\t%f\n", iHist, fitMeanArray[iHist]*1000, fitSigmaArray[iHist]*1000);
    }
}
