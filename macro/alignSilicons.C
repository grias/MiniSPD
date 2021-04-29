TTree *fTreeTracks;
TBranch *fBranchSiTracks;
TClonesArray *fSiliconTracks;

// array<array<int, 4>, 3> moduleGlobalNumirationMap {{ {{ 0, 1 }}, {{ 2, 3, 4, 5 }}, {{ 6, 7 }} }};
int moduleGlobalNumirationMap[3][4] { { 0, 1 }, { 2, 3, 4, 5 }, { 6, 7 } };

void OpenInput(UInt_t runId);
void Analyze(UInt_t runId);

void alignSilicons(UInt_t runId = 2)
{
    gSystem->Load("libTMillepede");

    OpenInput(runId);
    Analyze(runId);
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

void Analyze(UInt_t runId)
{
    int nPlane = 8;
    int nLocalPars = 2;
    int nConstr = 1;
    auto cMil = new TMillepede(nPlane, nLocalPars, nConstr);

    float *DerivativeGlobal = cMil->GetGlbDerPointer();
    float *DerivativeLocal = cMil->GetLocDerPointer();

    cMil->DefineParSigma(0, 0.0);
    cMil->DefineParSigma(1, 0.0);
    cMil->DefineParSigma(6, 0.0);
    cMil->DefineParSigma(7, 0.0);

    Long64_t nEvents = fTreeTracks->GetEntries();
    printf("NEvents: %lld\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        // if (iEv > 20) break;
        // printf("\nEvent %d\n", iEv);

        fBranchSiTracks->GetEntry(iEv);
        if (!fSiliconTracks->GetEntriesFast()) continue;
        
        auto siTrack = (StandSiliconTrack *)fSiliconTracks->At(0);

        for (uint iStation = 0; iStation < 3; iStation++)
        {
            int module = siTrack->GetModule(iStation);
            int globMod = moduleGlobalNumirationMap[iStation][module];
            double hitZ = siTrack->GetHitPositionZ(iStation);
            double hitX = siTrack->GetHitPositionX(iStation);

            // printf("<Hit> module: %d-%d(glob: %d); Z: %f, X: %f\n", iStation, module, globMod, hitZ, hitX);

            DerivativeGlobal[globMod] = 1.;
            DerivativeLocal[0] = 1.;
            DerivativeLocal[1] = static_cast<float>(hitZ);

            float measurment = static_cast<float>(hitX);
            float sigma = 0.1;

            cMil->StoreMeasurement(measurment, sigma);
        }
        cMil->FitLocal();
    }
    cMil->FitGlobal();

    for (int iMod = 0; iMod < nPlane; ++iMod)
        printf("Mod %d: %f\n", iMod, cMil->GetGlobal(iMod));
}