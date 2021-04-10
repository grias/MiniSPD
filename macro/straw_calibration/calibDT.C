
Int_t fRunId = 0;
TH1D *hTime;
constexpr Int_t fMaxTime = 120; /* ns */
constexpr Int_t fTDCIntervals = fMaxTime*10;

void kalib(Int_t ntube);
void joit(Int_t ntube, Int_t tdcmin, Int_t tdcmax);
void joit1();

void calibDT(Int_t runId = 1)
{
    fRunId = runId;
    Int_t ntube, b, tdcmin, tdcmax;

    cout << " Ntube " << endl;
    cin >> ntube;
    kalib(ntube);
    cout << "take b 1 or 0" << endl;
    cin >> b;
    if (b == 1)
    {
        cout << " take tdcmin          tdcmax   " << endl;
        cin >> tdcmin >> tdcmax;
        //
        joit(ntube, tdcmin, tdcmax);
    }
    else
        joit1();
    cout << " a = -1 " << endl;
    return 0;
}

void kalib(Int_t ntube)
{
    Int_t fetchStation = ntube / 64;
    Int_t fetchWire = ntube % 64;
    TString inputFileName = Form("data/stand_run%04d_digits.root", fRunId);
    printf("Reading file: %s\n", inputFileName.Data());
    printf("Looking for wire%d_%d\n", fetchStation, fetchWire);

    TFile *inputFile = new TFile(inputFileName);
    if (!inputFile) return;

    TTree *inputTree = (TTree *)inputFile->Get("cbmsim");

    TBranch *branch = inputTree->GetBranch("DCH");
    if (!branch) return;
    branch->SetAutoDelete(kTRUE);

    TClonesArray *StrawDigits = 0;
    inputTree->SetBranchAddress("DCH", &StrawDigits);

    TString outFileName = Form("Ntube%d.dat", ntube);
    ofstream outw(outFileName);

    Int_t nEvents = inputTree->GetEntries();
    printf("Number of events in file: %d\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        // printf("\nEvent %d\n", iEv);
        inputTree->GetEntry(iEv);

        Int_t nDigits = StrawDigits->GetEntriesFast();
        // printf("Number of straws triggered in event: %d\n", nDigits);

        for (Int_t iDigit = 0; iDigit < nDigits; iDigit++)
        {
            BmnDchDigit *strawDigit = (BmnDchDigit *)StrawDigits->At(iDigit);

            Int_t plane = strawDigit->GetPlaneInt();
            Int_t wire = strawDigit->GetWireNumber();
            Double_t time = strawDigit->GetTime();

            time = floor(time*10);

            if (plane == fetchStation && wire == fetchWire)
            {
                outw << setw(3) << time << '\n';
            }
            // printf("%d) Plane: %d, Wire: %d, GetTime: %f\n", iDigit, plane, wire, time);
        }
    }
    outw.close();
}

void joit(Int_t ntube, Int_t tdcmin, Int_t tdcmax)
{
    const Int_t m = fTDCIntervals; // total interval of  tdc
    Int_t tdc[100000];
    Int_t in = 0; // all values of tdc
    Int_t ncell[m] = {0};
    Double_t iftdc[m];
    Int_t i;
    Int_t nitdc;
    Double_t a = -1;
    Double_t r[m];
    Double_t ri[m] = {0};
    Double_t rcell[m] = {0};
    Double_t s = 0, sl = 0, sr = 0;

    Char_t fn[80] = {"matrix"};
    Char_t fr[80] = {""}; // tdc.dat
    Char_t fa[70] = {"tdc"};
    Char_t fi[70] = {""};
    strcat(fn, fr); // matrix.dat
    strcat(fn, ".dat");
    strcat(fa, fi);
    strcat(fa, ".dat");

    cout << ntube << "  " << tdcmin << "  " << tdcmax;

    TString inFileName = Form("Ntube%d.dat", ntube);
    ifstream inTdcFile(inFileName, ios::in);
    while (!inTdcFile.eof())
    {
        inTdcFile >> tdc[in];
        in++;
    }
    inTdcFile.close();

    cout << "\n in= " << in << endl;

    //background on the left
    for (i = 0; i < in; i++) 
    {
        if (tdc[i] < tdcmin)
            sl++;
    }
    sl = sl / tdcmin;

    // value of right background
    for (i = 0; i < in; i++) 
    {
        if (tdc[i] > tdcmax)
            sr++;
    }
    sr = sr / (fTDCIntervals - tdcmax);
    s = (sl + sr) / 2;
    //

    for (i = 0; i < in; i++) // step of TDC [tdcmin,tdcmax]
    {
        if (tdc[i] >= tdcmin && tdc[i] <= tdcmax)
        {
            nitdc = tdc[i];
            ncell[nitdc]++;
        }
    }
    for (i = tdcmin; i < tdcmax; i++)
    {
        rcell[i] = ncell[i] - s; // cut background
        cout << rcell[i] << endl;
    }
    cout << "\n  kalibrovka" << endl;
    //
    iftdc[tdcmin] = rcell[tdcmin]; //initial r(tdc_min)
    //

    for (i = tdcmin; i < tdcmax; i++) //nakaplivaem sums tdc v step
    {
        iftdc[i + 1] = iftdc[i] + rcell[i + 1];
    }
    //
    for (i = tdcmin; i < tdcmax; i++) // F(x)=P(X,x)
    {
        ri[i] = 3 * iftdc[i] / iftdc[tdcmax];
    }
    // сглаживание первой ступени
    ri[tdcmin] = ri[tdcmin] / 2;

    //
    for (i = 0; i < m; i++)
        r[i] = a;
    ofstream outRrcFile(fn, ios::app); //writing of matrix.dat
    if (!outRrcFile)
    {
        cerr << "File no open" << endl;
        exit(1);
    }
    //
    ofstream outRdcFile(fa, ios::out); // tdc.dat
    if (!outRdcFile)
    {
        cerr << "File no open" << endl;
        exit(1);
    }
    //
    r[tdcmin] = ri[tdcmin];

    //
    cout << " r[" << tdcmin - 1 << "] = " << r[tdcmin - 1] << endl;
    // сглаживание линии
    for (i = tdcmin; i < tdcmax - 1; i++)
    {
        r[i + 1] = (ri[i] + ri[i + 1]) / 2;
    }
    // поправка на вероятность попадания двух частиц
    for (i = tdcmin; i < tdcmax; i++)
    {
        // r[i] = 3 - r[i];
        r[i] = r[i] + 0.2 * r[i] * (r[i] / 3 - 1);
        cout << "   r[" << i << "] = " << r[i];
        outRdcFile << setw(5) << r[i] << '\n'; // tdc.dat
    }
    cout << " r[" << tdcmax << "] = " << r[tdcmax] << endl;
    outRdcFile.close();
    //
    outRrcFile.setf(ios_base::scientific, ios_base::floatfield);
    for (i = 0; i < m; i++) // matrix.dat
    {
        outRrcFile << setw(15) << r[i];
    }
    outRrcFile << endl;
    outRrcFile.close();
}

void joit1()
{
    const Int_t m = 513; // total interval of  tdc
    Int_t i;
    Double_t a = -1;
    Double_t r[m];
    Char_t fn[80] = {"matrix"};
    Char_t fr[80] = {""}; // tdc.dat
    strcat(fn, fr);       // matrix.dat
    strcat(fn, ".dat");
    cout << " - wrong tube " << endl;
    //
    for (i = 0; i < m; i++)
        r[i] = a;
    ofstream outRrcFile(fn, ios::app); //writing of matrix.dat
    if (!outRrcFile)
    {
        cerr << "File no open" << endl;
        exit(1);
    }
    //
    outRrcFile.setf(ios_base::scientific, ios_base::floatfield);
    for (i = 0; i < m; i++) // matrix.dat
    {
        outRrcFile << setw(15) << r[i];
    }
    outRrcFile << endl;
    outRrcFile.close();
}
