using namespace TMath;

TTree *fTreeTracks;
TBranch *fBranchSiTracks;
TClonesArray *fSiliconTracks;

TTree *fTreeDigits;
TBranch *fBranchStrawDigits;
TClonesArray *fStrawDigits;

TH2D* hRtTotal;
TH2D* hRtSingle;
TH1D* hRtProfile;
TH1D* hRtSigma;
TH1D* hRtChiNdf;
TH1D* hRtNdf;
TH1D* hRtFder;
TH1D* hRtSigmaU;
TH1I* h1OccupancyArr[2];

constexpr Int_t fRtNBinsX = 80;
// constexpr Int_t fRtNBinsX = 100;
constexpr Double_t rRange = 3.;

array<Double_t, fRtNBinsX> fFitMeanArray;
array<Double_t, fRtNBinsX> fFitSigmaArray;

void OpenInput(UInt_t runId);
void CreateHisto();
void Analyze();
void DrawHisto(UInt_t runId);
Double_t CalculateDistanceToWire(array<Double_t, 2> trackParameters, array<Double_t, 2> wirePosition);
Bool_t IsTrackFromLeft(array<Double_t, 2> trackParameters, array<Double_t, 2> wirePosition);

void createRT(UInt_t runId)
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

    TString inDigitsFileName = Form("data/stand_run%04d_digits.root", runId);
    TFile *inDigitsFile = new TFile(inDigitsFileName);
    if (!inDigitsFile) return;
    fTreeDigits = (TTree *)inDigitsFile->Get("cbmsim");
    fBranchStrawDigits = fTreeDigits->GetBranch("DCH");
    if (!fBranchStrawDigits) return;
    fBranchStrawDigits->SetAutoDelete(kTRUE);
    fTreeDigits->SetBranchAddress("DCH", &fStrawDigits);
}

void CreateHisto()
{
    TString histName;
    TString histDiscription;

    histName = Form("h2_rt_total");
    histDiscription = Form("RT relation;R [mm]; T [ns]");
    hRtTotal = new TH2D(histName, histDiscription, fRtNBinsX, -rRange, rRange, 80, 0, 100);

    histName = Form("h2_rt_single");
    histDiscription = Form("RT relation;R [mm]; T [ns]");
    hRtSingle = new TH2D(histName, histDiscription, fRtNBinsX, -rRange, rRange, 80, 0, 100);

    histName = Form("h2_rt_profile");
    histDiscription = Form("RT profile;R [mm];T [ns]");
    hRtProfile = new TH1D(histName, histDiscription, fRtNBinsX, -rRange, rRange);

    histName = Form("h2_rt_sigma");
    histDiscription = Form("#sigma_{T} distribution;R [mm];#sigma_{T} [ns]");
    hRtSigma = new TH1D(histName, histDiscription, fRtNBinsX, -rRange, rRange);

    histName = Form("h2_rt_chi_ndf");
    histDiscription = Form("#Chi^{2}/NDF;R [mm];#Chi^{2}/NDF");
    hRtChiNdf = new TH1D(histName, histDiscription, fRtNBinsX, -rRange, rRange);

    histName = Form("h2_rt_ndf");
    histDiscription = Form("NDF;R [mm];NDF");
    hRtNdf = new TH1D(histName, histDiscription, fRtNBinsX, -rRange, rRange);

    histName = Form("h2_rt_FDer");
    histDiscription = Form("F' distribution;R [mm];F'");
    hRtFder = new TH1D(histName, histDiscription, fRtNBinsX, -rRange, rRange);

    histName = Form("h2_rt_sigmaU");
    histDiscription = Form("#sigma_{R} distribution;R [mm];#sigma_{R}");
    hRtSigmaU = new TH1D(histName, histDiscription, fRtNBinsX, -rRange, rRange);

    const Int_t h1COLORS[2] = {2, 9};
    for (size_t iStation = 0; iStation < 2; iStation++)
    {
        histName = Form("h1_occup_plane%zu", iStation);
        histDiscription = Form("Straw tracker occupancy;Channel;");
        h1OccupancyArr[iStation] = new TH1I(histName, histDiscription, 64, 0 - 0.5, 64 - 0.5);
        h1OccupancyArr[iStation]->SetLineWidth(3);
        h1OccupancyArr[iStation]->SetLineColor(h1COLORS[iStation]);
        h1OccupancyArr[iStation]->GetXaxis()->SetNdivisions(32);
        h1OccupancyArr[iStation]->GetXaxis()->SetLabelSize(0.02);
    }
}

void Analyze()
{
    Long64_t nEvents = fTreeTracks->GetEntries();
    printf("NEvents: %lld\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        // printf("\nEvent %d\n", iEv);
        fBranchSiTracks->GetEntry(iEv);
        fBranchStrawDigits->GetEntry(iEv);

        if (!fSiliconTracks->GetEntriesFast()) continue;
        
        auto siTrack = (StandSiliconTrack *)fSiliconTracks->At(0);

        // --- Cuts -----------------------------------------------------------
        if (siTrack->GetChiSquare(0) > 0.1) continue;

        // --- Data processing ------------------------------------------------

        Double_t b = siTrack->GetParameterX(0);
        Double_t k = siTrack->GetParameterX(1);

        array<Double_t, 2> trackPars {b, k};

        Long64_t nDigits = fStrawDigits->GetEntriesFast();
        if (nDigits != 4) continue;
        
        // printf("NDigits: %lld\n", nDigits);
        for (size_t iDigit = 0; iDigit < nDigits; iDigit++)
        {
            auto strawDigit = (BmnDchDigit *)fStrawDigits->At(iDigit);

            Int_t plane = strawDigit->GetPlaneInt();
            Int_t wire = strawDigit->GetWireNumber();
            Double_t time = strawDigit->GetTime();

            auto wirePos = StandStrawGeoMapper::GetWireGlobalPositionZX(plane, wire);

            Double_t distToWire = CalculateDistanceToWire(trackPars, wirePos);
            if (distToWire > rRange) continue;
            distToWire = IsTrackFromLeft(trackPars, wirePos) ? (-distToWire) : distToWire;
            
            if (plane == 1) wire = 63-wire;
            
            h1OccupancyArr[plane]->Fill(wire);

            // if (plane == 1 && wire == 45)
            hRtTotal->Fill(distToWire, time);
        }
    }
    
    array<Double_t, fRtNBinsX> nEntries;
    array<Double_t, fRtNBinsX> time;
    array<Double_t, fRtNBinsX> sigmaTime;
    array<Double_t, fRtNBinsX> sigmaTimeError;
    array<Double_t, fRtNBinsX> coord;
    Double_t                   sigmaCoord = 2*rRange/fRtNBinsX;

    auto projectionX = hRtTotal->ProjectionX("projjj", 1, 2); // KOSTYL to get X bin center

    for (int iBin = 0; iBin < fRtNBinsX; iBin++)
    {
        auto projection = hRtTotal->ProjectionY(Form("proj_%02d", iBin+1), iBin+1, iBin+1);
        
        auto fitRes = projection->Fit("gaus", "SQN0");

        nEntries[iBin] = projection->GetEntries();
        time[iBin] = ((Int_t) fitRes == 0) ? fitRes->Parameter(1) : 0;
        sigmaTime[iBin] = ((Int_t) fitRes == 0) ? fitRes->Parameter(2) : 0; // divide by sqrt(N)?
        sigmaTimeError[iBin] = ((Int_t) fitRes == 0) ? fitRes->ParError(2) : 0;
        coord[iBin] = projectionX->GetBinCenter(iBin+1);

        // sigmaTimeError[iBin] = sigmaTimeError[iBin] / Sqrt(fitRes->Ndf());

        if ((Int_t) fitRes == 0)
        {
            hRtProfile->SetBinContent(iBin+1, time[iBin]);
            hRtProfile->SetBinError(iBin+1, sigmaTime[iBin]);
            hRtSigma->SetBinContent(iBin+1, sigmaTime[iBin]);
            hRtSigma->SetBinError(iBin+1, sigmaTimeError[iBin]);
            hRtChiNdf->SetBinContent(iBin+1, fitRes->Chi2()/fitRes->Ndf());
            hRtNdf->SetBinContent(iBin+1, fitRes->Ndf());
        }
    }

    array<Double_t, fRtNBinsX - 2> FDer;
    array<Double_t, fRtNBinsX - 2> dFDer_2;
    array<Double_t, fRtNBinsX - 2> SigU;
    array<Double_t, fRtNBinsX - 2> dSigU_2;

    Double_t deltaDU_2 = Sq(sigmaCoord) * 2;
    for (int iBin = 0; iBin < fRtNBinsX - 2; iBin++)
    {
        Double_t dT1 = time[iBin+2] - time[iBin+1];
        Double_t dT2 = time[iBin+1] - time[iBin];
        Double_t dU1 = coord[iBin+2] - coord[iBin+1];
        Double_t dU2 = coord[iBin+1] - coord[iBin];

        Double_t deltaDT1_2 = Sq(sigmaTime[iBin+2]) + Sq(sigmaTime[iBin+1]);
        Double_t deltaDT2_2 = Sq(sigmaTime[iBin+1]) + Sq(sigmaTime[iBin]);

        Double_t g1 = dT1 / dU1;
        Double_t g2 = dT2 / dU2;

        // Double_t dG1_2 = Sq(-dT1/Sq(dU1))*deltaDU_2 + Sq(1./dU1)*deltaDT1_2;
        // Double_t dG2_2 = Sq(-dT2/Sq(dU2))*deltaDU_2 + Sq(1./dU2)*deltaDT2_2;
        Double_t dG1_2 = Sq(-dT1/Sq(dU1))*deltaDU_2;
        Double_t dG2_2 = Sq(-dT2/Sq(dU2))*deltaDU_2;

        FDer[iBin] = (g1 + g2)/2.;
        dFDer_2[iBin] = (dG1_2 + dG2_2)/4.;

        SigU[iBin] = sigmaTime[iBin+1]/abs(FDer[iBin]);
        dSigU_2[iBin] = Sq(sigmaTimeError[iBin+1] / FDer[iBin]) + Sq(-sigmaTime[iBin+1]/Sq(FDer[iBin]))*dFDer_2[iBin];
    }

    for (int iBin = 0; iBin < fRtNBinsX - 2; iBin++)
    {
        hRtFder->SetBinContent(iBin+2, FDer[iBin]);
        hRtFder->SetBinError(iBin+2, sqrt(dFDer_2[iBin]));

        if (SigU[iBin] > 10 || sqrt(dSigU_2[iBin]) > 20) continue;
        hRtSigmaU->SetBinContent(iBin+2, SigU[iBin]);
        hRtSigmaU->SetBinError(iBin+2, sqrt(dSigU_2[iBin]));
    }

    Double_t resCoordTop = 0;
    Double_t resCoordBot = 0;

    Int_t excludedExtremeBins = 0;
    for (int iBin = excludedExtremeBins; iBin < fRtNBinsX - 2 - excludedExtremeBins; iBin++)
    {
        // if (SigU[iBin] > 10 || sqrt(dSigU_2[iBin]) > 20) continue;
        resCoordTop += SigU[iBin]/dSigU_2[iBin];
        resCoordBot += 1./dSigU_2[iBin];
    }

    Double_t res = resCoordTop/resCoordBot;
    Double_t sigmaRes = 1./Sqrt(resCoordBot);
    
    printf("Resolution: %f; sigma: %f\n", res, sigmaRes);
}

void DrawHisto(UInt_t runId)
{
    gSystem->Exec("mkdir -p pictures");

    gStyle->SetOptFit();

    TCanvas *cRtTotal = new TCanvas("cRtTotal", "", 1000, 1000);
    hRtTotal->Draw("");
    cRtTotal->SaveAs(Form("pictures/run%04d_rt_total.png", runId));

    // TCanvas *cRtSingle = new TCanvas("cRtSingle", "", 1000, 1000);
    // hRtSingle->Draw("COL");
    // cRtSingle->SaveAs(Form("pictures/run%04d_rt_single.png", runId));

    TCanvas *cRtProfile = new TCanvas("cRtProfile", "", 1000, 1000);
    hRtProfile->SetLineWidth(2);
    hRtProfile->Fit("pol6", "Q+", "",-3.1, 0);
    hRtProfile->Fit("pol6", "Q+", "",0, 3.1);
    hRtTotal->Draw("COL");
    hRtProfile->Draw("SAME");
    cRtProfile->SaveAs(Form("pictures/run%04d_rt_total_profile.png", runId));

    TCanvas *cRtSigma = new TCanvas("cRtSigma", "", 2000, 1000);
    hRtSigma->SetAxisRange(0, 10,"Y");
    hRtSigma->Draw("P*");
    cRtSigma->SaveAs(Form("pictures/run%04d_rt_sigma.png", runId));

    TCanvas *cRtChiNdf = new TCanvas("cRtChiNdf", "", 1000, 1000);
    cRtChiNdf->SetLogy();
    hRtChiNdf->Draw("P*");
    cRtChiNdf->SaveAs(Form("pictures/run%04d_rt_chi_ndf.png", runId));

    TCanvas *cRtNdf = new TCanvas("cRtNdf", "", 1000, 1000);
    hRtNdf->Draw("P*");
    cRtNdf->SaveAs(Form("pictures/run%04d_rt_ndf.png", runId));

    TCanvas *cRtFder = new TCanvas("cRtFder", "", 2000, 1000);
    hRtFder->SetAxisRange(-100, 100,"Y");
    hRtFder->Draw("E");
    cRtFder->SaveAs(Form("pictures/run%04d_rt_FDer.png", runId));

    TCanvas *cRtSigmaU = new TCanvas("cRtSigmaU", "", 2000, 1000);
    hRtSigmaU->SetAxisRange(-2, 2,"Y");
    hRtSigmaU->Draw("E");
    cRtSigmaU->SaveAs(Form("pictures/run%04d_rt_sigmaU.png", runId));

    TCanvas *cOccupancy = new TCanvas("cOccupancy", "", 800, 800);
    h1OccupancyArr[1]->Draw("");
    h1OccupancyArr[0]->Draw("SAME");
    // cOccupancy->BuildLegend();
    cOccupancy->SaveAs(Form("pictures/run%04d_rt_straw_occupancy.png", runId));

    Int_t nBin = 33;
    TCanvas *cFitExample = new TCanvas("cFitExample", "", 1000, 1000);
    auto projection = hRtTotal->ProjectionY(Form("proj_%02d", nBin), nBin, nBin);
    projection->SetTitle("RT slice");
    auto fitRes = projection->Fit("gaus", "Q+");
    projection->Draw();
    cFitExample->SaveAs(Form("pictures/run%04d_rt_fit_example.png", runId));

}

Double_t CalculateDistanceToWire(array<Double_t, 2> trackParameters, array<Double_t, 2> wirePosition)
{
    Double_t z0 = wirePosition[0];
    Double_t x0 = wirePosition[1];

    Double_t z1 = 0;
    Double_t x1 = trackParameters[0] + trackParameters[1]* z1;

    Double_t z2 = 1;
    Double_t x2 = trackParameters[0] + trackParameters[1]* z2;

    Double_t distance = abs((x2 - x1)*z0 - (z2 - z1)*x0 + z2*x1 - x2*z1);
    distance /= sqrt((x2-x1)*(x2-x1) + (z2-z1)*(z2-z1));

    return distance;
}

Bool_t IsTrackFromLeft(array<Double_t, 2> trackParameters, array<Double_t, 2> wirePosition)
{
    Double_t trackX = trackParameters[0] + trackParameters[1]* wirePosition[0];

    return (trackX < wirePosition[1]);
}
