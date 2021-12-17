void gausSquareSum()
{
    array<double, 10> sigmas {70, 90, 50, 40, 10, 60, 40, 70, 20, 80};
    constexpr int nPoints = 5;

    auto fRand = new TRandom3();
    array<double, nPoints> residArray;

    auto hRes = new TH1D("hRes", "", 1000, -300, 300);
    auto hRss = new TH1D("hRss", "", 1000, 0, 300);

    for (size_t i = 0; i < 1000000; i++)
    {

        for (size_t iPoint = 0; iPoint < nPoints; iPoint++)
        {
            residArray[iPoint] = fRand->Gaus(0, sigmas[iPoint]);
        }
        
        double rss = 0;
        for (auto &&resid : residArray)
        {
            rss += resid*resid;
        }
        rss = sqrt(rss);
        hRss->Fill(rss);
        hRes->Fill(residArray[1]);
        // printf("%f (%f, %f, %f)\n", rss, residArray[0], residArray[1], residArray[2]);
    }

    auto canvas = new TCanvas("c1", "", 1000);

    hRss->Draw("HIST");
    canvas->SaveAs("gausSquareSum_rss.png");

    hRes->Draw("HIST");
    canvas->SaveAs("gausSquareSum_res.png");
}