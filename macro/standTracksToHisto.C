const bool kVERBOSE_MODE = false;

// vector<Int_t> runs = {780, 802, 803, 811, 816, 817, 818, 829, 831, 832, 838, 840, 842, 843, 844, 848, 851};
vector<Int_t> runs = {816, 780};
// vector<Int_t> runs = {816};
const Double_t degToRad = 3.14159265359 / 180.;
const Double_t radToDeg = 180. / 3.14159265359;

const Int_t stationMap[8] = {0, 0, 1, 1, 1, 1, 2, 2};
const Int_t modMap[8] = {0, 1, 0, 1, 2, 3, 0, 1};

TH2D *hResidAll;
array<TH2D *, 3> hResidStation;
map<Int_t, TH2D *> hResidMapRxRy;
map<Int_t, TH2D *> hResidMapRxY;
TH1D *hChiSquare;
TH1D *hTrackAngleX;
TH1D *hTrackAngleY;
TMultiGraph *HitsZX;
TMultiGraph *HitsZY;
TMultiGraph *HitsXY;

void standTracksToHisto(UInt_t runId = 0, Int_t saveImages = 0)
{
    // --- HIST CREATION -----------------------------------------------------------------
    TString histName;
    TString histDiscription;
    TH2D *hist;

    histName = Form("h2_residuals_rxry");
    histDiscription = Form("Res X vs res Y; res X [mm]; res Y [mm]");
    hResidAll = new TH2D(histName, histDiscription, 100, -5, 5, 100, -5, 5);
    
    for (size_t iHist = 0; iHist < 8; iHist++)
    {
        Int_t key = 10 * stationMap[iHist] + 1 * modMap[iHist];

        // resX vs resY
        histName = Form("h2_station%d_mod%d_residuals_rxry", stationMap[iHist], modMap[iHist]);
        histDiscription = Form("Residuals (station %d, module %d);X [mm];Y [mm]", stationMap[iHist], modMap[iHist]);
        hist = new TH2D(histName, histDiscription, 100, -2, 2, 100, -2, 2);
        hResidMapRxRy.insert({key, hist});

        // resX vs Y
        histName = Form("h2_station%d_mod%d_residuals_rxy", stationMap[iHist], modMap[iHist]);
        histDiscription = Form("ResX vs Y (station %d, module %d); res X [mm];Y [mm]", stationMap[iHist], modMap[iHist]);
        hist = new TH2D(histName, histDiscription, 100, -5, 5, 240, -240, 0);
        hResidMapRxY.insert({key, hist});
    }

    for (size_t iStation = 0; iStation < 3; iStation++)
    {
        histName = Form("h2_station%zu_residuals_rxry", iStation);
        histDiscription = Form("Res X vs res Y (station %zu); res X [mm]; res Y [mm]", iStation);
        hResidStation[iStation] = new TH2D(histName, histDiscription, 100, -5, 5, 100, -5, 5);
    }

    hChiSquare = new TH1D("h1_chisquare", "Chi square distribution;Chi square", 500, 0, 100);

    hTrackAngleX = new TH1D("h1_trackangle_x", "Tracks slope distribution (plane XZ);Angle [deg]", 40, -20, 20);
    hTrackAngleY = new TH1D("h1_trackangle_y", "Tracks slope distribution (plane YZ);Angle [deg]", 40, -20, 20);

    HitsZX = new TMultiGraph();
    HitsZY = new TMultiGraph();
    HitsXY = new TMultiGraph();

    // --- Input -----------------------------------------------------------------
    TChain *inChain;
    TTree *tree;
    TClonesArray *SiliconTracks = 0;
    TBranch *branchSILICON;
    StandSiliconHit *siHit = 0;
    StandSiliconTrack *siTrack = 0;

    if (runId < 1)
    {
        inChain = new TChain("cbmsim");
        for (auto &&iRunId : runs)
        {
            TString inFileName = Form("data/stand_run%04d_tracks.root", iRunId);
            inChain->Add(inFileName);
        }

        branchSILICON = inChain->GetBranch("SiliconTracks");
        if (!branchSILICON)
            return;
        branchSILICON->SetAutoDelete(kTRUE);
        inChain->SetBranchAddress("SiliconTracks", &SiliconTracks);
    }
    else
    {
        TString inFileName = Form("data/stand_run%04d_tracks.root", runId);
        TFile *inFile = new TFile(inFileName);
        if (!inFile)
            return;
        tree = (TTree *)inFile->Get("cbmsim");

        branchSILICON = tree->GetBranch("SiliconTracks");
        if (!branchSILICON)
            return;
        branchSILICON->SetAutoDelete(kTRUE);
        tree->SetBranchAddress("SiliconTracks", &SiliconTracks);
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
        if (kVERBOSE_MODE) printf("\nEvent %d\n", iEv);

        // if (iEv == 10000) break;
        

        branchSILICON->GetEntry(iEv);

        // Silicon hits
        Int_t nTracks = SiliconTracks->GetEntriesFast();
        if (kVERBOSE_MODE)
            cout << "nTracks: " << nTracks << endl;
        for (size_t iTrack = 0; iTrack < nTracks; iTrack++)
        {
            siTrack = (StandSiliconTrack *)SiliconTracks->At(iTrack);

            Double_t chiX = siTrack->GetChiSquare(0);
            Double_t chiY = siTrack->GetChiSquare(1);
            Double_t chiSquareSum = sqrt(chiX*chiX + chiY*chiY);
            // hChiSquare->Fill(chiSquareSum);
            hChiSquare->Fill(chiX);
            if (chiSquareSum > 10) continue;
            // if (siTrack->GetModule(1) != 3) continue;
            

            // hChiSquare->Fill(chiX);
            // hChiSquare->Fill(chiY);

            Double_t hitPosX[3] = {siTrack->GetHitPositionX(0), siTrack->GetHitPositionX(1), siTrack->GetHitPositionX(2)};
            Double_t hitPosY[3] = {siTrack->GetHitPositionY(0), siTrack->GetHitPositionY(1), siTrack->GetHitPositionY(2)};
            Double_t hitPosZ[3] = {siTrack->GetHitPositionZ(0), siTrack->GetHitPositionZ(1), siTrack->GetHitPositionZ(2)};

            for (size_t iStation = 0; iStation < 3; iStation++)
            {
                Int_t module = siTrack->GetModule(iStation);
                Double_t residX = siTrack->GetResidualX(iStation);
                Double_t residY = siTrack->GetResidualY(iStation);

                Int_t key = 10 * iStation + 1 * module;

                hResidAll->Fill(residX, residY);
                hResidStation[iStation]->Fill(residX, residY);
                hResidMapRxRy.find(key)->second->Fill(residX, residY);
                hResidMapRxY.find(key)->second->Fill(residX, hitPosY[iStation]);
            }

            TGraph *graphZX = new TGraph(3, hitPosZ, hitPosX);
            TGraph *graphZY = new TGraph(3, hitPosZ, hitPosY);
            TGraph *graphXY = new TGraph(3, hitPosX, hitPosY);

            HitsZX->Add(graphZX, "C*");
            HitsZY->Add(graphZY, "C*");
            HitsXY->Add(graphXY, "C*");

            Double_t trackAngleX = atan(siTrack->GetParameterX(1))*radToDeg;
            Double_t trackAngleY = atan(siTrack->GetParameterY(1))*radToDeg;

            hTrackAngleX->Fill(trackAngleX);
            hTrackAngleY->Fill(trackAngleY);
        }
    } // end of event

    // --- OUTPUT ------------------------------------------------------------------------
    TFile *outputFile = new TFile(Form("root_files/run%04d_si_tracks.root", runId), "recreate");
    auto cResidAll = new TCanvas("hResidAll", "", 1000, 1000);
    hResidAll->Draw("COLZ");
    if (saveImages)
    {
        cResidAll->SaveAs(Form("pictures/run%04d_si_track_residRxRy_all.png", runId));
    }
    

    for (size_t iStation = 0; iStation < 3; iStation++)
    {
        TCanvas *canvas = new TCanvas(Form("canvas_st%zu", iStation), "", 1000, 1000);
        hResidStation[iStation]->Write();
        hResidStation[iStation]->Draw("COLZ");
        if (saveImages)
        {
            canvas->SaveAs(Form("pictures/run%04d_si_track_residRxRy_st%zu.png", runId, iStation));
        }
        delete canvas;
    }    

    for (auto &&pair : hResidMapRxRy)
    {
        auto key = pair.first;
        auto hist = pair.second;

        TCanvas *canvas = new TCanvas(Form("canvas%d", key), "", 1000, 1000);
        hist->Write();
        hist->Draw("COLZ");
        if (saveImages)
        {
            canvas->SaveAs(Form("pictures/run%04d_si_track_residRxRy_mod%02d.png", runId, key));
        }
        delete canvas;
    }

    // for (auto &&pair : hResidMapRxY)
    // {
    //     auto key = pair.first;
    //     auto hist = pair.second;

    //     TCanvas *canvas = new TCanvas(Form("canvas%d", key), "", 1000, 1000);
    //     hist->Write();
    //     hist->Draw("COLZ");
    //     if (saveImages)
    //     {
    //         canvas->SaveAs(Form("pictures/run%04d_si_track_residRxY_mod%02d.png", runId, key));
    //     }
    //     delete canvas;
    // }

    TCanvas *canvas = new TCanvas("canvas", "", 1000, 1000);

    canvas->SetLogy(kTRUE);
    hChiSquare->Draw("HIST");
    hChiSquare->Write();
    if (saveImages)
    canvas->SaveAs(Form("pictures/run%04d_si_track_chi.png", runId));
    canvas->SetLogy(kFALSE);
    canvas->Clear();

    HitsZX->Draw("a");
    if (saveImages)
    canvas->SaveAs(Form("pictures/run%04d_si_tracks_ZX.png", runId));
    canvas->Clear();

    HitsZY->Draw("a");
    if (saveImages)
    canvas->SaveAs(Form("pictures/run%04d_si_tracks_ZY.png", runId));
    canvas->Clear();

    HitsXY->Draw("a");
    if (saveImages)
    canvas->SaveAs(Form("pictures/run%04d_si_tracks_XY.png", runId));
    canvas->Clear();

    hTrackAngleX->Draw("HIST");
    hTrackAngleX->Write();
    if (saveImages)
    canvas->SaveAs(Form("pictures/run%04d_si_tracks_angleX.png", runId));
    canvas->Clear();

    hTrackAngleY->Draw("HIST");
    hTrackAngleY->Write();
    if (saveImages)
    canvas->SaveAs(Form("pictures/run%04d_si_tracks_angleY.png", runId));
    canvas->Clear();

    outputFile->Write();
    outputFile->Close();
}
