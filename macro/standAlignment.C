array<TH2D *, 4> hResVsX;
array<TH2D *, 4> hResVsY;
TH2D * hResVsXTot;

void standAlignment(Int_t runId = 1)
{
    TString histName;
    TString histDiscription;

    histName = "h2_station1_residuals_rxx";
    histDiscription = "ResX vs X; res X [mm]; X [mm]";
    hResVsXTot = new TH2D(histName, histDiscription, 100, -5, 5, 20, 0, 160);
    

    for (Int_t iModule = 0; iModule < 4; iModule++)
    {
        histName = Form("h2_station1_mod%d_residuals_rxx", iModule);
        histDiscription = Form("ResX vs X (module %d); res X [mm]; X [mm]", iModule);
        hResVsX[iModule] = new TH2D(histName, histDiscription, 100, -5, 5, 20, 0, 63);

        // histName = Form("h2_station%d_residuals_ryy", iModule);
        // histDiscription = Form("ResY vs Y (module %d); res Y [mm]; Y [mm]", iModule);
        // hResVsY[iModule] = new TH2D(histName, histDiscription, 1000, -5, 5, 2000, 0, 63);
    }
    

    // --- Input Tracks -----------------------------------------------------------------
    TClonesArray *SiliconTracks = 0;
    StandSiliconTrack *siTrack = 0;

    TString inTracksFileName = Form("data/stand_run%04d_tracks.root", runId);
    TFile *inTracksFile = new TFile(inTracksFileName);
    if (!inTracksFile)
        return;
    TTree *treeTracks = (TTree *)inTracksFile->Get("cbmsim");

    TBranch *branchSiTracks = treeTracks->GetBranch("SiliconTracks");
    if (!branchSiTracks)
        return;
    branchSiTracks->SetAutoDelete(kTRUE);
    treeTracks->SetBranchAddress("SiliconTracks", &SiliconTracks);

    // --- Input Hits -----------------------------------------------------------------
    TClonesArray *SiliconHits = 0;
    StandSiliconHit *siHit = 0;

    TString inHitsFileName = Form("data/stand_run%04d_hits.root", runId);
    TFile *inHitsFile = new TFile(inHitsFileName);
    if (!inHitsFile)
        return;
    TTree *treeHits = (TTree *)inHitsFile->Get("cbmsim");

    TBranch *branchSiHits = treeHits->GetBranch("SiliconHits");
    if (!branchSiHits)
        return;
    branchSiHits->SetAutoDelete(kTRUE);
    treeHits->SetBranchAddress("SiliconHits", &SiliconHits);

    // --- Event loop ---------------------------------------------------------------------
    Long64_t nEvents = treeTracks->GetEntries();

    UInt_t goodTracks = 0;

    printf("NEvents: %lld\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {

        branchSiTracks->GetEntry(iEv);
        branchSiHits->GetEntry(iEv);

        Int_t nTracks = SiliconTracks->GetEntriesFast();
        Int_t nHits = SiliconHits->GetEntriesFast();

        if (!nTracks) continue;

        siTrack = (StandSiliconTrack *)SiliconTracks->At(0);

        // pick only tracks with clusters of size 1
        Bool_t breakEvent = false;
        for (size_t iHit = 0; iHit < 3; iHit++)
        {
            Int_t hitId = siTrack->GetHitId(iHit);

            siHit = (StandSiliconHit *)SiliconHits->At(hitId);

            Int_t clusterSizeX = siHit->GetClusterSizeX();
            Int_t clusterSizeY = siHit->GetClusterSizeY();

            if (clusterSizeX > 1 || clusterSizeY > 1)
            {
                breakEvent = true;
                break;
            }
        }
        if (breakEvent) continue;

        // printf("Event %d\n", iEv);
        // printf("Tracks: %d, Hits: %d\n", nTracks, nHits);
        goodTracks++;

        // fitting
        TLinearFitter linFitX(1);
        linFitX.SetFormula("pol1");
        TLinearFitter linFitY(1);
        linFitY.SetFormula("pol1");

        siTrack->GetHitPositionX(0);

        Double_t z0[1] = {siTrack->GetHitPositionZ(0)};
        linFitX.AddPoint(z0, siTrack->GetHitPositionX(0));
        linFitY.AddPoint(z0, siTrack->GetHitPositionY(0));

        Double_t z2[1] = {siTrack->GetHitPositionZ(2)};
        linFitX.AddPoint(z2, siTrack->GetHitPositionX(2));
        linFitY.AddPoint(z2, siTrack->GetHitPositionY(2));

        linFitX.Eval();
        linFitY.Eval();

        TVectorD paramsX, paramsY;
        linFitX.GetParameters(paramsX);
        linFitY.GetParameters(paramsY);

        Int_t module = siTrack->GetModule(1);
        Double_t hitX = siTrack->GetHitPositionX(1);
        Double_t hitY = siTrack->GetHitPositionY(1);
        Double_t hitZ = siTrack->GetHitPositionZ(1);

        Double_t residX = hitX - (paramsX(0) + hitZ* paramsX(1));
        Double_t residY = hitY - (paramsY(0) + hitZ* paramsY(1));

        Int_t hitId = siTrack->GetHitId(1);
        siHit = (StandSiliconHit *)SiliconHits->At(hitId);

        // if (abs(residX) > 3) continue;

        hResVsXTot->Fill(residX, hitX);
        hResVsX[module]->Fill(residX, siHit->GetLocalX());
        // hResVsY[module]->Fill(residY, siHit->GetLocalY());

    } // end of event
    printf("\nGood tracks: %d\n", goodTracks);

    // --- OUTPUT ------------------------------------------------------------------------
    array<Int_t, 4> histPlacementMap {2, 1, 4, 3};

    auto cResVsXTot = new TCanvas("c_resvsx_tot", "", 1000, 1000);
    hResVsXTot->Draw("COL");
    cResVsXTot->SaveAs(Form("pictures/run%04d_si_tracks_resxvsx_tot.png", runId));

    auto cResVsXTotProf = new TCanvas("c_resvsx_tot_prof", "", 1000, 1000);
    auto profileTot = hResVsXTot->ProfileY();
    profileTot->Draw();
    cResVsXTotProf->SaveAs(Form("pictures/run%04d_si_tracks_resxvsx_tot_profile.png", runId));

    auto cResVsX = new TCanvas("c_resvsx", "", 1000, 1000);
    cResVsX->Divide(2, 2, 0, 0);
    for (Int_t iModule = 0; iModule < 4; iModule++)
    {
        cResVsX->cd(histPlacementMap[iModule]);
        hResVsX[iModule]->Draw("COL");
    }
    cResVsX->SaveAs(Form("pictures/run%04d_si_tracks_resxvsx.png", runId));

    auto cResVsXS = new TCanvas("c_resvsx_single", "", 1000, 1000);
    for (Int_t iModule = 0; iModule < 4; iModule++)
    {
        hResVsX[iModule]->Draw("COL");
        cResVsXS->SaveAs(Form("pictures/run%04d_si_tracks_resxvsx_mod%d.png", runId, iModule));
    }   
    
    auto cResVsXProf = new TCanvas("c_resvsx_prof", "", 1000, 1000);
    cResVsXProf->Divide(2, 2, 0, 0);
    for (Int_t iModule = 0; iModule < 4; iModule++)
    {
        cResVsXProf->cd(histPlacementMap[iModule]);
        auto profile = hResVsX[iModule]->ProfileY();
        profile->Draw();
    }
    cResVsXProf->SaveAs(Form("pictures/run%04d_si_tracks_resxvsx_profile.png", runId));


    auto cResVsXProfS = new TCanvas("c_resvsx_single_prof", "", 1000, 1000);
    for (Int_t iModule = 0; iModule < 4; iModule++)
    {
        auto profile = hResVsX[iModule]->ProfileY();
        profile->Draw();
        cResVsXProfS->SaveAs(Form("pictures/run%04d_si_tracks_resxvsx_profile_mod%d.png", runId, iModule));
    }

}