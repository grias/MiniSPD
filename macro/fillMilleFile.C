TTree *fTreeTracks;
TBranch *fBranchSiTracks;
TClonesArray *fSiliconTracks;

// array<array<int, 4>, 3> moduleGlobalNumirationMap {{ {{ 0, 1 }}, {{ 2, 3, 4, 5 }}, {{ 6, 7 }} }};
int moduleGlobalNumirationMap[3][4] { { 0, 1 }, { 2, 3, 4, 5 }, { 6, 7 } };

void OpenInput(UInt_t runId);
void Analyze(UInt_t runId);

void fillMilleFile(UInt_t runId = 1)
{
    gSystem->Load("libMille");

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
    TString milleFileName = Form("stand_run%04d_mille.dat", runId);
    // Mille mil(milleFileName, false, true);  // REMOVE AFTER DEBUGGING
    Mille mil(milleFileName);

    


    Long64_t nEvents = fTreeTracks->GetEntries();
    printf("NEvents: %lld\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        // if (iEv > 20) break;
        // printf("\nEvent %d\n", iEv);   

        fBranchSiTracks->GetEntry(iEv);
        if (!fSiliconTracks->GetEntriesFast()) continue;

        constexpr int nLocal = 2;
        constexpr int nGlobal = 8;
        int globalLabels[nGlobal] = {1, 2, 3, 4, 5, 6, 7, 8};
        // int globalLabels[nGlobal] = {10, 20, 30, 40, 50, 60, 70, 80};
        
        
        auto siTrack = (StandSiliconTrack *)fSiliconTracks->At(0);

        for (uint iStation = 0; iStation < 3; iStation++)
        {
            int module = siTrack->GetModule(iStation);
            int globMod = moduleGlobalNumirationMap[iStation][module];
            double hitZ = siTrack->GetHitPositionZ(iStation);
            double hitX = siTrack->GetHitPositionX(iStation);

            // printf("<Hit> module: %d-%d(glob: %d); Z: %f, X: %f\n", iStation, module, globMod, hitZ, hitX);


            float localPars[nLocal] = {1, static_cast<float>(hitZ)};
            float globalPars[nGlobal] = {0, 0, 0, 0, 0, 0, 0, 0};
            globalPars[globMod] = 1;
            float measurment = static_cast<float>(hitX);
            float sigma = 0.095;

            mil.mille(nLocal, localPars, nGlobal, globalPars, globalLabels, measurment, sigma);
        }
        mil.end();
        mil.kill();
    }
    
    
}