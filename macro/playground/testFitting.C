void testFitting()
{
    constexpr uint nHits = 3;
    array<double, nHits> hitX {0, 1, 3};

    array<double, nHits> hitZ {0, 10, 20}; // equidistant
    // array<double, nHits> hitZ {0, 50, 85}; // similar to minispd

    TLinearFitter fitter(1, "pol1");
    
    for (uint i = 0; i < nHits; i++)
    {
        double zArr[1] = {hitZ[i]};
        fitter.AddPoint(zArr, hitX[i]);
    }

    fitter.Eval();
    TVectorD pars;
    fitter.GetParameters(pars);

    printf("Fit parameters: p0=%f; p1=%f\n", pars[0], pars[1]);

    for (uint i = 0; i < nHits; i++)
    {
        printf("Hit %d: measured=%f; fitted=%f, residual=%f\n", i, hitX[i], (pars[0]+pars[1]*hitZ[i]), hitX[i] - (pars[0]+pars[1]*hitZ[i]));
    }
}