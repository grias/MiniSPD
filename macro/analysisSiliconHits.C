TTree *fTreeHits;
TBranch *fBranchSiHits;
TClonesArray *fSiliconHits;

const Int_t stationMap[8] = {0, 0, 1, 1, 1, 1, 2, 2};
const Int_t modMap[8] = {0, 1, 0, 1, 2, 3, 0, 1};

map<Int_t, TH2D *> hOccupModuleMap;
map<Int_t, TH1D *> hOccupModuleXMap;
vector<TH2D *>hOccupStationVector;
map<Int_t, TH2D *> hAmplitudesCorrelMap;
TH1I* hClustersSizeX;
TH1I* hClustersSizeY;
array<array<TH1D *, 4>, 3> hClusterAmplitude;
TH1D* h1Signal[3];
TH1D* h1CoordU;

void OpenInput(UInt_t runId);
void CreateHisto();
void Analyze();
void DrawHisto(UInt_t runId);

void analysisSiliconHits(UInt_t runId)
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
    fBranchSiHits = fTreeHits->GetBranch("SiliconHits");
    if (!fBranchSiHits) return;
    fBranchSiHits->SetAutoDelete(kTRUE);
    fTreeHits->SetBranchAddress("SiliconHits", &fSiliconHits);
}

void CreateHisto()
{
    TString histName;
    TString histDiscription;
    TH2D *hist;
    TH1D *hist1d;
    for (size_t iHist = 0; iHist < 8; iHist++)
    {
        Int_t key = 10 * stationMap[iHist] + 1 * modMap[iHist];

        Int_t nBins = 75;

        // occupancy
        histName = Form("h2_station%d_mod%d_occupancy", stationMap[iHist], modMap[iHist]);
        histDiscription = Form("Module occupancy (station %d, module %d);X [mm];Y [mm]", stationMap[iHist], modMap[iHist]);
        if (key / 10 == 1)
        {
            hist = new TH2D(histName, histDiscription, nBins, 0, 63, nBins, -10, 70);
        }
        else
        {
            hist = new TH2D(histName, histDiscription, nBins, 0, 63, nBins*2, -10, 140);
        }
        hOccupModuleMap.insert({key, hist});

        // occupancyX
        histName = Form("h1_station%d_mod%d_occupancy_x", stationMap[iHist], modMap[iHist]);
        histDiscription = Form("Module occupancy X (station %d, module %d);X [mm]; count", stationMap[iHist], modMap[iHist]);
        hist1d = new TH1D(histName, histDiscription, 700, 0, 63);
        hOccupModuleXMap.insert({key, hist1d});

        // Amplitudes
        histName = Form("h2_station%d_mod%d_amplitudes", stationMap[iHist], modMap[iHist]);
        histDiscription = Form("Amplitudes (station %d, module %d);Cluster X amplitude [ae];Cluster Y amplitude [ae]", stationMap[iHist], modMap[iHist]);
        hist = new TH2D(histName, histDiscription, 120, 0, 1200, 120, 0, 1200);
        hAmplitudesCorrelMap.insert({key, hist});
    }

    for (int iStation = 0; iStation < 3; iStation++)
    {
        histName = Form("h2_station%d_occupancy", iStation);
        histDiscription = Form("Station %d occupancy;X [mm];Y [mm]", iStation);
        hist = new TH2D(histName, histDiscription, 150, 20, 170.1, 150, -170.1, -20);
        hOccupStationVector.push_back(hist);
    }
    
    hClustersSizeX = new TH1I("h1_clusters_x", "Cluster size;Number of strips", 15, 1-0.5, 6-0.5);
    hClustersSizeX->GetXaxis()->SetNdivisions(5);
    hClustersSizeY = new TH1I("h1_clusters_y", "Cluster size;Number of strips", 15, 1-0.5, 6-0.5);
    hClustersSizeY->GetXaxis()->SetNdivisions(5);


    for (int iStation = 0; iStation < 3; iStation++)
    for (int iClusterSize = 0; iClusterSize < 4; iClusterSize++)
    {
        histName = Form("h2_amplitude_size%d_station%d", iClusterSize+1, iStation);
        histDiscription = Form("Cluster amplitude (station %d);amplitude [AU]", iStation);
        hClusterAmplitude[iStation][iClusterSize] = new TH1D(histName, histDiscription, 200, 0, 2000);
    }

    for (int iStation = 0; iStation < 3; iStation++)
    {
        histName = Form("h1_amplitude_station%d", iStation);
        histDiscription = Form("Silicon signal amplitude (station %d);AU;", iStation);
        h1Signal[iStation] = new TH1D(histName, histDiscription, 80, 0, 800);
    }

    histName = Form("h1_coord_u");
    histDiscription = Form("Silicon hit U;U [mm];");
    h1CoordU = new TH1D(histName, histDiscription, 2000, 0, 200);

}

void Analyze()
{
    Long64_t nEvents = fTreeHits->GetEntries();
    printf("NEvents: %lld\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        // printf("\nEvent %d\n", iEv);
        fBranchSiHits->GetEntry(iEv);

        Int_t nHits = fSiliconHits->GetEntriesFast();
        // printf("nHits: %d\n", nHits);
        for (size_t iHit = 0; iHit < nHits; iHit++)
        {
            auto siHit = (StandSiliconHit *)fSiliconHits->At(iHit);

            Int_t station = siHit->GetStation();
            Int_t module = siHit->GetModule();
            Double_t localX = siHit->GetLocalX();
            Double_t localY = siHit->GetLocalY();
            Double_t amplitudeX = std::abs(siHit->GetAmplitudeX());
            Double_t amplitudeY = std::abs(siHit->GetAmplitudeY());
            Double_t clusterSizeX = siHit->GetClusterSizeX();
            Double_t clusterSizeY = siHit->GetClusterSizeY();
            Double_t globalU = siHit->GetGlobalU();

            if (!StandSiliconGeoMapper::fIsActiveModule[station][module]) continue;

            // if (clusterSizeX > 1 || clusterSizeY > 1) continue;
            

            Int_t key = 10 * station + 1 * module;
            hOccupModuleMap.find(key)->second->Fill(localX, localY);

            hOccupModuleXMap.find(key)->second->Fill(localX);

            hAmplitudesCorrelMap.find(key)->second->Fill(amplitudeX, amplitudeY);
            hClustersSizeX->Fill(clusterSizeX);
            hClustersSizeY->Fill(clusterSizeY);

            TVector3 globalHitPos = StandSiliconGeoMapper::CalculateGlobalCoordinatesForHit(station, module, localX, localY);

            hOccupStationVector[station]->Fill(globalHitPos.X(), globalHitPos.Y());

            hClusterAmplitude[station][clusterSizeX-1]->Fill(amplitudeX);
            hClusterAmplitude[station][clusterSizeY-1]->Fill(amplitudeY);

            h1Signal[station]->Fill(abs(amplitudeX));
            h1Signal[station]->Fill(abs(amplitudeY));

            h1CoordU->Fill(globalU);

            // std::cout<<TMath::Abs(globalU / 0.99904822158 /*cos2.5deg*/ - globalHitPos.X())<<std::endl;
            
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

        TCanvas *canvas;
        if (key / 10 == 1)
        {
            canvas = new TCanvas(Form("canvas%d", key), "", 630, 630);
        }
        else
        {
            canvas = new TCanvas(Form("canvas%d", key), "", 630, 1260);
        }
        hist->Draw("COLZ");
        canvas->SaveAs(Form("pictures/run%04d_hits_si_occupancy_mod%02d.png", runId, key));
        delete canvas;
    }

    for (auto &&pair : hOccupModuleXMap)
    {
        auto key = pair.first;
        auto hist = pair.second;

        TCanvas *canvas;
        canvas = new TCanvas(Form("canvas%d", key), "", 630, 630);
        hist->Draw("");
        canvas->SaveAs(Form("pictures/run%04d_hits_si_occupancy_x_mod%02d.png", runId, key));
        delete canvas;
    }

    for (auto &&pair : hAmplitudesCorrelMap)
    {
        auto key = pair.first;
        auto hist = pair.second;

        TCanvas *canvas = new TCanvas(Form("canvas%d", key), "", 630, 630);
        hist->Draw("COLZ");
        canvas->SaveAs(Form("pictures/run%04d_hits_si_amplitude_correlation_mod%02d.png", runId, key));
        delete canvas;
    }

    Int_t iStation = 0;
    for (auto &&hist : hOccupStationVector)
    {
        TCanvas *canvas = new TCanvas(Form("canvas%d", iStation), "", 630, 630);
        hist->Draw("COL");
        canvas->SaveAs(Form("pictures/run%04d_hits_si_occupancy_station%d.png", runId, iStation++));
        delete canvas;
    }

    TCanvas *canvas = new TCanvas("canvas", "", 800, 600);
    hClustersSizeX->SetLineWidth(3);
    hClustersSizeX->SetLineColor(2);
    hClustersSizeY->SetLineWidth(3);
    hClustersSizeY->SetLineColor(9);
    hClustersSizeX->Draw("HIST");
    hClustersSizeY->Draw("SAME HIST");
    canvas->SetLogy(kTRUE);
    canvas->SaveAs(Form("pictures/run%04d_hits_si_cluster_size.png", runId));
    canvas->SetLogy(kFALSE);
    canvas->Clear();

    array<Int_t, 4> colors {2, 9, 8, 6};
    for (int iStation = 0; iStation < 3; iStation++)
    {
        for (int iClusterSize = 0; iClusterSize < 4; iClusterSize++)
        {
            hClusterAmplitude[iStation][iClusterSize]->SetLineColor(colors[iClusterSize]);
            hClusterAmplitude[iStation][iClusterSize]->Draw(!iClusterSize?"HIST":"HIST SAME");
        }
        canvas->SetLogy(kTRUE);
        canvas->SaveAs(Form("pictures/run%04d_hits_si_amplitude_station%d.png", runId, iStation));
        canvas->SetLogy(kFALSE);
        canvas->Clear();
    }

    auto cSignalAll = new TCanvas(Form("c_signalAll"), "Silicon signal", 800, 600);
    h1Signal[2]->SetLineWidth(3);
    h1Signal[0]->SetLineWidth(3);
    h1Signal[1]->SetLineWidth(3);
    h1Signal[2]->SetLineColor(9);
    h1Signal[0]->SetLineColor(2);
    h1Signal[1]->SetLineColor(1);
    h1Signal[2]->Draw("");
    h1Signal[0]->Draw("SAME");
    h1Signal[1]->Draw("SAME");
    // cSignalAll->BuildLegend();
    cSignalAll->SaveAs(Form("pictures/run%04d_hits_si_amplitude_all.png", runId));

    auto cGlobalU = new TCanvas(Form("c_global_u"), "Global U", 800, 600);
    h1CoordU->Draw();
    cGlobalU->SaveAs(Form("pictures/run%04d_hits_si_coord_u.png", runId));
}
