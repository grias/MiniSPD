TTree *fTreeTracks;
TTree *fTreeHits;
TBranch *fBranchTracks;
TBranch *fBranchSiHits;
TClonesArray *fTracks;
TClonesArray *fSiliconHits;

const Double_t degToRad = 3.14159265359 / 180.;
const Double_t radToDeg = 180. / 3.14159265359;

const Int_t fStationMap[8] = {0, 0, 1, 1, 1, 1, 2, 2};
const Int_t fModMap[8] = {0, 1, 0, 1, 2, 3, 0, 1};
const Int_t fStationModToHistMap[3][4] = {{0,1,-1,-1}, {2,3,4,5}, {6,7,-1,-1}};

array<TH2D *, 8> hResVsX;
array<TH2D *, 3> hStationOccups;
TH2D *hCoordCorrelX;
TMultiGraph *HitsZX;
TMultiGraph *HitsZY;
TMultiGraph *HitsXY;
TH1D *hTrackResolutionX;
TH1D *hTrackAngleX;

void OpenInput(UInt_t runId);
void CreateHisto();
void Analyze();
void DrawHisto(UInt_t runId);

void analysisTracks(Int_t runId)
{
    OpenInput(runId);
    CreateHisto();
    Analyze();
    DrawHisto(runId);    
}

void OpenInput(UInt_t runId)
{
    // --- Input Tracks -----------------------------------------------------------------
    TString inTracksFileName = Form("data/stand_run%04d_tracks.root", runId);
    TFile *inTracksFile = new TFile(inTracksFileName);
    if (!inTracksFile) return;
    fTreeTracks = (TTree *)inTracksFile->Get("cbmsim");
    fBranchTracks = fTreeTracks->GetBranch("Tracks");
    if (!fBranchTracks) return;
    fBranchTracks->SetAutoDelete(kTRUE);
    fTreeTracks->SetBranchAddress("Tracks", &fTracks);

    // --- Input Hits -----------------------------------------------------------------
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

    for (Int_t iHist = 0; iHist < 8; iHist++)
    {
        histName = Form("h2_station%d_mod%d_residuals_rxx", fStationMap[iHist], fModMap[iHist]);
        histDiscription = Form("ResX vs X (station %d, module %d); res X [mm]; X [mm]", fStationMap[iHist], fModMap[iHist]);
        hResVsX[iHist] = new TH2D(histName, histDiscription, 300, -2.5, 2.5, 10, 0, 63);
        // hResVsX[iHist] = new TH2D(histName, histDiscription, 100, -0.5, 0.5, 10, 0, 63);

    }

    for (int iStation = 0; iStation < 3; iStation++)
    {
        histName = Form("h2_station%d_occupancy", iStation);
        histDiscription = Form("Station %d occupancy;X [mm];Y [mm]", iStation);
        hStationOccups[iStation] = new TH2D(histName, histDiscription, 150, 20, 170.1, 150, -170.1, -20);
    }

    histName = Form("h2_coordinate_correlation_x");
    histDiscription = Form("Correlation X;St1 X [mm]; St2 X [mm]");
    hCoordCorrelX = new TH2D(histName, histDiscription, 100, 200, 200, 100, 0, 200);

    histName = Form("h1_track_resolution_x");
    histDiscription = Form("Track resolution X;resid [mm];");
    hTrackResolutionX = new TH1D(histName, histDiscription, 100, 0, 0.5);

    hTrackAngleX = new TH1D("h1_trackangle_x", "Tracks slope distribution (plane XZ);Angle [deg]", 100, -10, 10);

    HitsZX = new TMultiGraph();
    HitsZY = new TMultiGraph();
    HitsXY = new TMultiGraph();
}

void Analyze()
{
    UInt_t goodTracks = 0;
    Long64_t nEvents = fTreeTracks->GetEntries();
    printf("NEvents: %lld\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        // printf("Event %d\n", iEv);
        fBranchTracks->GetEntry(iEv);
        fBranchSiHits->GetEntry(iEv);

        if (!fTracks->GetEntriesFast()) continue;

        auto track = (StandTrack *)fTracks->At(0);

        // --- Cuts -----------------------------------------------------------

        // if (track->GetChiSquare(0) > 0.1) continue;
        // if (track->GetChiSquare(0) < 0.5) continue;

        // // pick only tracks with clusters of size 1
        // Bool_t isBigClusterPresent = false;
        // for (size_t iHit = 0; iHit < 3; iHit++)
        // {
        //     Int_t hitId = track->GetHitId(iHit);
        //     auto siHit = (StandSiliconHit *)fSiliconHits->At(hitId);

        //     Int_t clusterSizeX = siHit->GetClusterSizeX();
        //     Int_t clusterSizeY = siHit->GetClusterSizeY();

        //     if (clusterSizeX > 1 || clusterSizeY > 1)
        //     {
        //         isBigClusterPresent = true;
        //         break;
        //     }
        // }
        // if (isBigClusterPresent) continue;

        // pick only tracks with clusters of size 1 at center station
        // Bool_t isBigClusterPresent = false;
        // Int_t hitId = track->GetHitId(1);
        // auto siHit = (StandSiliconHit *)fSiliconHits->At(hitId);
        // Int_t clusterSizeX = siHit->GetClusterSizeX();
        // Int_t clusterSizeY = siHit->GetClusterSizeY();
        // if (clusterSizeX > 1)
        // {
        //     isBigClusterPresent = true;
        // }
        // if (isBigClusterPresent) continue;

        // pick only tracks ~ parallel to Z axis
        Double_t angleCut = 1; /* deg */
        // if (abs(track->GetParameterX(1)) > tan(angleCut*degToRad)) continue;


        goodTracks++;
        // --- Data processing -----------------------------------------------------------

        if (goodTracks > 5) continue;

        for (size_t iStation = 0; iStation < 5; iStation++)
        {
            cout<<track->GetModule(iStation)<<"\t";
        }
        cout<<endl;

        for (size_t iStation = 0; iStation < 3; iStation++)
        {
            Double_t hitPosX = track->GetHitPositionX(iStation);
            Double_t hitPosY = track->GetHitPositionY(iStation);
            hStationOccups[iStation]->Fill(hitPosX, hitPosY);
        }

        Double_t sigma = 0;
        for (size_t iStation = 0; iStation < 3; iStation++)
        {
            Int_t module = track->GetModule(iStation);
            Double_t hitX = track->GetHitPositionX(iStation);
            Double_t residX = track->GetResidualX(iStation);
            Int_t hitId = track->GetHitId(iStation);
            auto siHit = (StandSiliconHit *)fSiliconHits->At(hitId);

            Int_t histNumber = fStationModToHistMap[iStation][module];
            hResVsX[histNumber]->Fill(residX, siHit->GetLocalX());

            sigma += residX*residX/0.03;
        }
        sigma = sqrt(sigma);
        hTrackResolutionX->Fill(sigma);

        hCoordCorrelX->Fill(track->GetHitPositionX(0), track->GetHitPositionX(2));

        Double_t trackAngleX = atan(track->GetParameterX(1))*radToDeg;
        hTrackAngleX->Fill(trackAngleX);

        Double_t hitPosX[5] = {track->GetHitPositionX(0), track->GetHitPositionX(3), track->GetHitPositionX(1), track->GetHitPositionX(2), track->GetHitPositionX(4)};
        Double_t hitPosY[5] = {track->GetHitPositionY(0), track->GetHitPositionY(3), track->GetHitPositionY(1), track->GetHitPositionY(2), track->GetHitPositionY(4)};
        Double_t hitPosZ[5] = {track->GetHitPositionZ(0), track->GetHitPositionZ(3), track->GetHitPositionZ(1), track->GetHitPositionZ(2), track->GetHitPositionZ(4)};

        

        TGraph *graphZX = new TGraph(5, hitPosZ, hitPosX);
        TGraph *graphZY = new TGraph(5, hitPosZ, hitPosY);
        TGraph *graphXY = new TGraph(5, hitPosX, hitPosY);

        HitsZX->Add(graphZX, "C*");
        HitsZY->Add(graphZY, "C*");
        HitsXY->Add(graphXY, "C*");
        
    } // end of event
    printf("\nGood tracks: %d\n", goodTracks);
}

void DrawHisto(UInt_t runId)
{
    gSystem->Exec("mkdir -p pictures");

    gStyle->SetOptFit();

    for (int iStation = 0; iStation < 3; iStation++)
    {
        auto canvas = new TCanvas(Form("canvas%d", iStation), "", 630, 630);
        hStationOccups[iStation]->Draw("COL");
        canvas->SaveAs(Form("pictures/run%04d_tracks_si_station%d_occup.png", runId, iStation));
        delete canvas;
    }
    
    // 2D histo
    auto cResVsX = new TCanvas("c_resvsx", "", 800, 800);
    for (size_t iHist = 0; iHist < 8; iHist++)
    {
        hResVsX[iHist]->Draw("COL");
        cResVsX->SaveAs(Form("pictures/run%04d_tracks_si_resxvsx_st%d_mod%d.png", runId, fStationMap[iHist], fModMap[iHist]));
    }

    // profile
    for (size_t iHist = 0; iHist < 8; iHist++)
    {
        auto profile = hResVsX[iHist]->ProfileY();
        profile->GetYaxis()->SetTitle("Res X [mm]");
        profile->Draw();
        cResVsX->SaveAs(Form("pictures/run%04d_tracks_si_resxvsx_st%d_mod%d_profile.png", runId, fStationMap[iHist], fModMap[iHist]));
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
    for (size_t iHist = 0; iHist < 8; iHist++)
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
        cResVsX->SaveAs(Form("pictures/run%04d_tracks_si_resxvsx_st%d_mod%d_projection.png", runId, fStationMap[iHist], fModMap[iHist]));
    }

    // auto cCoordCorrelX = new TCanvas("c_CoordCorrelX", "", 1000, 1000);
    // hCoordCorrelX->Draw("COL");
    // cCoordCorrelX->SaveAs(Form("pictures/run%04d_tracks_ccordCorrel.png", runId));

    auto cResolutionX = new TCanvas("c_resol_x", "", 1000, 1000);
    // hTrackResolutionX->Fit("gaus", "Q+", "", 0, 0.5);
    hTrackResolutionX->Fit(gausFixMp, "BQ+", "", 0, 0.2);
    hTrackResolutionX->Draw();
    cResolutionX->SaveAs(Form("pictures/run%04d_tracks_si_resolution.png", runId));

    auto cTrackAngleX = new TCanvas("cTrackAngleX", "", 1000, 1000);
    hTrackAngleX->Draw("HIST");
    cTrackAngleX->SaveAs(Form("pictures/run%04d_tracks_si_angleX.png", runId));

    auto canvas = new TCanvas("canvas_graph", "", 1000, 1000);
    HitsZX->Draw("a");
    canvas->SaveAs(Form("pictures/run%04d_tracks_si_ZX.png", runId));
    canvas->Clear();

    HitsZY->Draw("a");
    canvas->SaveAs(Form("pictures/run%04d_tracks_si_ZY.png", runId));
    canvas->Clear();

    HitsXY->Draw("a");
    canvas->SaveAs(Form("pictures/run%04d_tracks_si_XY.png", runId));
    canvas->Clear();

    printf("\nFit results:\n");
    printf("Module\tMean [mkm]\tSigma [mkm]\n");
    for (size_t iHist = 0; iHist < 8; iHist++)
    {
        printf("%d-%d\t%f\t%f\n", fStationMap[iHist], fModMap[iHist], fitMeanArray[iHist]*1000, fitSigmaArray[iHist]*1000);
    }
}