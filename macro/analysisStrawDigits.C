TTree *fTreeDigits;
TBranch *fBranchStrawDigits;
TClonesArray *fStrawDigits;

Int_t oneChIStation = 1;
Int_t oneChICh = 16;

TH1I *h1OccupancyArr[2];
TH1I *h1ClusterArr[2];
TH1D *h1Time;
TH1D *h1TimeSum;
TH1D *h1TimeSumPlane[2];
TH2D *h1TimeSum2;

void OpenInput(UInt_t runId);
void CreateHisto();
void Analyze();
void DrawHisto(UInt_t runId);

void analysisStrawDigits(UInt_t runId)
{
    OpenInput(runId);
    CreateHisto();
    Analyze();
    DrawHisto(runId);
}

void OpenInput(UInt_t runId)
{
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

    const Int_t h1COLORS[2] = {2, 9};
    for (size_t iStation = 0; iStation < 2; iStation++)
    {
        histName = Form("h1_occup_plane%zu", iStation);
        histDiscription = Form("Straw occupancies (plane %zu);Channel;", iStation);
        h1OccupancyArr[iStation] = new TH1I(histName, histDiscription, 64, 0 - 0.5, 64 - 0.5);
        h1OccupancyArr[iStation]->SetLineWidth(3);
        h1OccupancyArr[iStation]->SetLineColor(h1COLORS[iStation]);
        h1OccupancyArr[iStation]->GetXaxis()->SetNdivisions(64);
        h1OccupancyArr[iStation]->GetXaxis()->SetLabelSize(0.02);

        histName = Form("h1_cluster_plane%zu", iStation);
        histDiscription = Form("Straw cluster size (plane %zu);Size;", iStation);
        h1ClusterArr[iStation] = new TH1I(histName, histDiscription, 64, 0 - 0.5, 64 - 0.5);
        h1ClusterArr[iStation]->SetLineWidth(3);
        h1ClusterArr[iStation]->SetLineColor(h1COLORS[iStation]);
        h1ClusterArr[iStation]->GetXaxis()->SetNdivisions(64);
        h1ClusterArr[iStation]->GetXaxis()->SetLabelSize(0.02);
    }

    h1Time = new TH1D("h1_time", "Straw electron drift time;ns", 100, 0, 100);
    h1TimeSum = new TH1D("h1_timeSum", "Drift time sum;ns", 200, 0, 200);
    h1TimeSum2 = new TH2D("h2_timeSum2d", "Drift time;wire1 time [ns]; wire2 time [ns]", 100, 0, 100, 100, 0, 100);
}

void Analyze()
{
    Long64_t nEvents = fTreeDigits->GetEntries();
    printf("NEvents: %lld\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        // printf("\nEvent %d\n", iEv);
        fBranchStrawDigits->GetEntry(iEv);

        Int_t clusterSizePlane0 = 0;
        Int_t clusterSizePlane1 = 0;

        Int_t nPairsPlane0 = 0;
        Int_t nPairsPlane1 = 0;

        Int_t prevPlane;
        Int_t prevWire;
        Double_t prevTime;
        bool compareToPrev = false;

        // Dch digits
        Long64_t nDigits = fStrawDigits->GetEntriesFast();
        // printf("NDigits: %lld\n", nDigits);
        if (nDigits != 4) continue;
        for (size_t iDigit = 0; iDigit < nDigits; iDigit++)
        {
            auto strawDigit = (BmnDchDigit *)fStrawDigits->At(iDigit);

            Int_t plane = strawDigit->GetPlaneInt();
            Int_t wire = strawDigit->GetWireNumber();
            if (plane == 1)
                wire = 63 - wire;
            Double_t time = strawDigit->GetTime();
            Int_t refId = strawDigit->GetRefId();

            h1Time->Fill(time);
            h1OccupancyArr[plane]->Fill(wire);

            // printf("<DCHDigit> Plane: %d, Wire: %d, GetTime: %f, RefId: %d, CompareToPrev %d\n", plane, wire, time, refId, compareToPrev);

            if (compareToPrev && prevPlane == plane && abs(prevWire - wire) == 1)
            {
                if (plane == oneChIStation)
                // if (abs(prevWire - oneChICh) <= 1 && abs(wire - oneChICh) <= 1)
                if (true)
                {
                    h1TimeSum->Fill(time + prevTime);
                    if (wire%2 == 1)
                    {
                        h1TimeSum2->Fill(time, prevTime);
                    }
                    else
                    {
                        h1TimeSum2->Fill(prevTime, time);
                    }

                    if (plane == 0)
                    {
                        nPairsPlane0++;
                    }
                    else if (plane == 1)
                    {
                        nPairsPlane1++;
                    }

                    // cout << "<Pair> Plane: " << plane << ", wires: " << prevWire << " " << wire << endl;
                }
            }
            
            if (compareToPrev == false)
            {
                prevPlane = plane;
                prevWire = wire;
                prevTime = time;
                compareToPrev = true;
            }
            else
            {
                compareToPrev = false;
            }
        }

        if (nPairsPlane0 == 1 && nPairsPlane1 == 1)
        {
            for (size_t iDigit = 0; iDigit < nDigits; iDigit++)
            {
                auto strawDigit = (BmnDchDigit *)fStrawDigits->At(iDigit);

                Int_t plane = strawDigit->GetPlaneInt();
                Int_t wire = strawDigit->GetWireNumber();
                if (plane == 1)
                    wire = 63 - wire;
                Double_t time = strawDigit->GetTime();

                if (plane == 0)
                {
                    clusterSizePlane0++;
                }
                else if (plane == 1)
                {
                    clusterSizePlane1++;
                }
            }
        }

        // if (clusterSizePlane0)
        h1ClusterArr[0]->Fill(clusterSizePlane0);
        // if (clusterSizePlane1)
        h1ClusterArr[1]->Fill(clusterSizePlane1);
    } // end of event
}

void DrawHisto(UInt_t runId)
{
    gSystem->Exec("mkdir -p pictures");
    
    TCanvas *canvas = new TCanvas("canvas", "", 1200, 800);

    h1OccupancyArr[0]->Draw("");
    h1OccupancyArr[1]->Draw("SAME");
    canvas->BuildLegend();
    canvas->SaveAs(Form("pictures/run%04d_digits_straw_occupancy.png", runId));
    canvas->Clear();

    canvas->SetLogy(0);
    h1ClusterArr[0]->Draw();
    h1ClusterArr[1]->Draw("SAME");
    canvas->BuildLegend();
    canvas->SaveAs(Form("pictures/run%04d_digits_straw_cluster_size.png", runId));
    canvas->Clear();

    h1Time->SetLineWidth(3);
    h1Time->Draw();
    canvas->SaveAs(Form("pictures/run%04d_digits_straw_time.png", runId));
    canvas->Clear();

    h1TimeSum->Draw();
    h1TimeSum->SetLineWidth(3);
    canvas->SaveAs(Form("pictures/run%04d_digits_straw_time_sum.png", runId));
    canvas->Clear();

    h1TimeSum2->Draw("COL");
    canvas->SaveAs(Form("pictures/run%04d_digits_straw_time_correlation.png", runId));
    canvas->Clear();
}
