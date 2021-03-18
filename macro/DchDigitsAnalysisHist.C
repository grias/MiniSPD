const bool kVERBOSE_MODE = false;
// const bool kVERBOSE_MODE = true;

const Int_t h1COLORS[2] = {2, 9}; // blue, red
vector<Int_t> runs = {780, 802, 803, 811, 816, 817, 818, 829, 831, 832, 838, 840, 842, 843, 844, 848, 851};

TH1I *h1OccupancyArr[2];
TH1I *h1ClusterArr[2];
TH1D *h1Time;
TH1D *h1TimeSum;
TH1D *h1TimeSumPlane[2];
TH2D *h1TimeSum2;
TH1D *h1TimeOneCh;
Int_t oneChIStation = 1;
Int_t oneChICh = 16;

void DchDigitsAnalysisHist(UInt_t runId = 0, Int_t drawHistKey = -1)
{
    for (size_t iHist = 0; iHist < 2; iHist++)
    {
        TString h1OccupancyName = Form("h1_occup_plane%zu", iHist);
        TString h1OccupancyDisc = Form("Straw occupancies (plane %zu);Ch;", iHist);
        h1OccupancyArr[iHist] = new TH1I(h1OccupancyName, h1OccupancyDisc, 64, 0 - 0.5, 64 - 0.5);
        h1OccupancyArr[iHist]->SetLineWidth(3);
        h1OccupancyArr[iHist]->SetLineColor(h1COLORS[iHist]);
        h1OccupancyArr[iHist]->GetXaxis()->SetNdivisions(64);
        h1OccupancyArr[iHist]->GetXaxis()->SetLabelSize(0.02);

        TString h1ClusterName = Form("h1_cluster_plane%zu", iHist);
        TString h1ClusterDisc = Form("Straw cluster size (plane %zu);Size;", iHist);
        h1ClusterArr[iHist] = new TH1I(h1ClusterName, h1ClusterDisc, 64, 0 - 0.5, 64 - 0.5);
        h1ClusterArr[iHist]->SetLineWidth(3);
        h1ClusterArr[iHist]->SetLineColor(h1COLORS[iHist]);
        h1ClusterArr[iHist]->GetXaxis()->SetNdivisions(64);
        h1ClusterArr[iHist]->GetXaxis()->SetLabelSize(0.02);
    }

    h1Time = new TH1D("h1_time", "Straw electron drift time;ns", 100, 0, 100);
    h1TimeOneCh = new TH1D("h1_time_one_channel", Form("Straw electron drift time (station %d, wire %d);ns", oneChIStation, oneChICh), 100, 0, 100);
    h1TimeSum = new TH1D("h1_timeSum", "Drift time sum;ns", 200, 0, 200);
    h1TimeSum2 = new TH2D("h2_timeSum2d", "Drift time;wire1 time [ns]; wire2 time [ns]", 100, 0, 100, 100, 0, 100);

    // Chain ---------------------------------------------------------------------------
    TChain *inChain = new TChain("cbmsim");
    for (auto &&iRunId : runs)
    {
        inChain->Add(Form("data/stand_run%04d_digits.root", iRunId));
    }

    // BmnDchDigit branch
    TClonesArray *DchDigits = 0;
    TBranch *branchDCH = inChain->GetBranch("DCH");
    branchDCH->SetAutoDelete(kTRUE);
    if (!branchDCH)
        return;
    inChain->SetBranchAddress("DCH", &DchDigits);

    BmnDchDigit *dchDigit = 0;

    // Event loop ---------------------------------------------------------------------
    // Long64_t nEvents = tree->GetEntries();
    Long64_t nEvents = inChain->GetEntries();
    printf("NEvents: %lld\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        if (kVERBOSE_MODE)
            printf("\nEvent %d\n", iEv);

        inChain->GetEntry(iEv);

        Int_t clusterSizePlane0 = 0;
        Int_t clusterSizePlane1 = 0;

        Int_t nPairsPlane0 = 0;
        Int_t nPairsPlane1 = 0;

        Int_t prevPlane;
        Int_t prevWire;
        Double_t prevTime;
        bool compareToPrev = false;

        // Dch digits
        Long64_t nDigits = DchDigits->GetEntriesFast();
        if (nDigits != 4) continue;

        if (kVERBOSE_MODE)
            printf("NDigits: %lld\n", nDigits);
        for (size_t iDigit = 0; iDigit < nDigits; iDigit++)
        {
            dchDigit = (BmnDchDigit *)DchDigits->At(iDigit);

            Int_t plane = dchDigit->GetPlaneInt();
            Int_t wire = dchDigit->GetWireNumber();
            if (plane == 1)
                wire = 63 - wire;
            Double_t time = dchDigit->GetTime();
            Int_t refId = dchDigit->GetRefId();

            if (kVERBOSE_MODE)
                printf("<DCHDigit> Plane: %d, Wire: %d, GetTime: %f, RefId: %d, CompareToPrev %d\n", plane, wire, time, refId, compareToPrev);

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

                    if (kVERBOSE_MODE)
                        cout << "<Pair> Plane: " << plane << ", wires: " << prevWire << " " << wire << endl;
                }
            }

            if (plane == oneChIStation && wire == oneChICh)
            {
                h1TimeOneCh->Fill(time);
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
                dchDigit = (BmnDchDigit *)DchDigits->At(iDigit);

                Int_t plane = dchDigit->GetPlaneInt();
                Int_t wire = dchDigit->GetWireNumber();
                if (plane == 1)
                    wire = 63 - wire;
                Double_t time = dchDigit->GetTime();

                h1Time->Fill(time);
                h1OccupancyArr[plane]->Fill(wire);

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

    // --- DRAW ------------------------------------------------------------------------

    TString outFileName = Form("data/stand_run%04d_hists_dch.root", runId);
    TFile *outputRootFile = new TFile(outFileName, "recreate");

    if (drawHistKey == -1)
    {
        // for (size_t iHist = 0; iHist < 2; iHist++)
        // {
        //     TCanvas *canvas = new TCanvas(Form("canvas%zu", iHist), "", 1200, 600);
        //     canvas->SetLogy();
        //     h1OccupancyArr[iHist]->Draw();
        //     canvas->SaveAs(Form("pictures/h1_dch_occup_plane%zu.png",iHist));
        //     canvas->SetLogy(0);
        //     h1ClusterArr[iHist]->Draw();
        //     canvas->SaveAs(Form("pictures/h1_dch_clust_plane%zu.png",iHist));
        // }

        TCanvas *canvas = new TCanvas("canvas", "", 1200, 800);

        auto h1OccupancySum = new TH1I("h1_occup_sum", "Straw occupancies;Ch;", 64, 0 - 0.5, 64 - 0.5);
        h1OccupancySum->Add(h1OccupancyArr[0], h1OccupancyArr[1]);
        h1OccupancySum->GetYaxis()->SetRangeUser(0, 1.1 * h1OccupancySum->GetMaximum());
        h1OccupancySum->SetLineWidth(3);
        h1OccupancySum->SetLineColor(3);
        h1OccupancySum->GetXaxis()->SetNdivisions(64);
        h1OccupancySum->GetXaxis()->SetLabelSize(0.02);
        canvas->SetLogy(0);
        h1OccupancySum->Draw();
        h1OccupancyArr[0]->Draw("SAME");
        h1OccupancyArr[1]->Draw("SAME");
        canvas->BuildLegend();
        canvas->SaveAs(Form("pictures/run%04d_dch_occup.png", runId));

        canvas->SetLogy(0);
        h1ClusterArr[0]->Draw();
        h1ClusterArr[1]->Draw("SAME");
        canvas->BuildLegend();
        canvas->SaveAs(Form("pictures/run%04d_dch_clust.png", runId));
        h1Time->SetLineWidth(3);
        h1Time->Draw();
        canvas->SaveAs(Form("pictures/run%04d_dch_time.png", runId));

        h1TimeSum->Draw();
        h1TimeSum->SetLineWidth(3);
        canvas->SaveAs(Form("pictures/run%04d_dch_timeSum.png", runId));

        h1TimeSum2->Draw("COL");
        canvas->SaveAs(Form("pictures/run%04d_dch_timeSum2d.png", runId));

        h1TimeOneCh->Scale(100);
        h1TimeOneCh->SetLineWidth(3);
        h1TimeOneCh->SetLineColor(2);
        h1TimeOneCh->Draw("HIST");
        h1Time->Draw("HIST SAME");
        h1TimeOneCh->Write();

        canvas->SaveAs(Form("pictures/run%04d_dch_timeOneCh.png", runId));
    }
    else if (drawHistKey >= 0)
    {
        cout << "Drawing plane " << drawHistKey << endl;
        TCanvas *canvas = new TCanvas(Form("canvas%d", drawHistKey), "", 1200, 600);
        canvas->SetLogy();
        h1OccupancyArr[drawHistKey]->Draw();
    }

    outputRootFile->Close();
}
