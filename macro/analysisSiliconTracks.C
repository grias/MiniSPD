TTree *fTreeTracks;
TBranch *fBranchSiTracks;
TClonesArray *fSiliconTracks;

const Double_t degToRad = 3.14159265359 / 180.;
const Double_t radToDeg = 180. / 3.14159265359;

TH1D *hChiSquare;
TH1D *hTrackAngleX;
TH1D *hTrackAngleY;

void OpenInput(UInt_t runId);
void CreateHisto();
void Analyze();
void DrawHisto(UInt_t runId);

void analysisSiliconTracks(UInt_t runId)
{
    OpenInput(runId);
    CreateHisto();
    Analyze();
    DrawHisto(runId);
}

void OpenInput(UInt_t runId)
{
    TString inTracksFileName = Form("data/stand_run%04d_tracks.root", runId);
    TFile *inTracksFile = new TFile(inTracksFileName);
    if (!inTracksFile) return;
    fTreeTracks = (TTree *)inTracksFile->Get("cbmsim");
    fBranchSiTracks = fTreeTracks->GetBranch("SiliconTracks");
    if (!fBranchSiTracks) return;
    fBranchSiTracks->SetAutoDelete(kTRUE);
    fTreeTracks->SetBranchAddress("SiliconTracks", &fSiliconTracks);
}

void CreateHisto()
{
    TString histName;
    TString histDiscription;

    hChiSquare = new TH1D("h1_chisquare", "Chi square distribution;Chi square", 500, 0, 100);

    hTrackAngleX = new TH1D("h1_trackangle_x", "Tracks slope distribution (plane XZ);Angle [deg]", 40, -20, 20);
    hTrackAngleY = new TH1D("h1_trackangle_y", "Tracks slope distribution (plane YZ);Angle [deg]", 40, -20, 20);
}

void Analyze()
{
    Long64_t nEvents = fTreeTracks->GetEntries();
    printf("NEvents: %lld\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        // printf("\nEvent %d\n", iEv);   
        fBranchSiTracks->GetEntry(iEv);

        if (!fSiliconTracks->GetEntriesFast()) continue;
        
        auto siTrack = (StandSiliconTrack *)fSiliconTracks->At(0);

        Double_t chiX = siTrack->GetChiSquare(0);
        Double_t chiY = siTrack->GetChiSquare(1);
        Double_t chiSquareSum = sqrt(chiX*chiX + chiY*chiY);
            
        // hChiSquare->Fill(chiSquareSum);
        hChiSquare->Fill(chiX);
        // hChiSquare->Fill(chiY);

        if (chiSquareSum > 10) continue;

        Double_t trackAngleX = atan(siTrack->GetParameterX(1))*radToDeg;
        Double_t trackAngleY = atan(siTrack->GetParameterY(1))*radToDeg;

        hTrackAngleX->Fill(trackAngleX);
        hTrackAngleY->Fill(trackAngleY);
    }
}

void DrawHisto(UInt_t runId)
{
    gStyle->SetOptFit();

    TCanvas *canvas = new TCanvas("canvas", "", 1000, 1000);

    canvas->SetLogy(kTRUE);
    hChiSquare->Draw("HIST");
    canvas->SaveAs(Form("pictures/run%04d_tracks_si_chi.png", runId));
    canvas->SetLogy(kFALSE);
    canvas->Clear();

    hTrackAngleX->Draw("HIST");
    canvas->SaveAs(Form("pictures/run%04d_tracks_si_angleX.png", runId));
    canvas->Clear();

    hTrackAngleY->Draw("HIST");
    canvas->SaveAs(Form("pictures/run%04d_tracks_si_angleY.png", runId));
    canvas->Clear();
}