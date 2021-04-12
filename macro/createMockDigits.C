TRandom3* fRand;
StandIOManager* fIOManager;
TClonesArray* fSiliconDigitsArray;

Double_t st0RangeX[2] {40, 100};
Double_t st0RangeY[2] {-90, -30};
Double_t st2RangeX[2] {38, 98};
Double_t st2RangeY[2] {-90, -35};

Double_t posZ[3] {605.52, 245.42, -252.98};

array<Double_t, 2> GetTrackParams(array<Double_t, 2> point1, array<Double_t, 2> point2);
void FillNewSiliconDigit(Int_t iStation, Int_t iModule, Int_t iLayer, Int_t iStrip, Double_t iSignal);
void InitIOManager();

void createMockDigits()
{
    fRand = new TRandom3();
    InitIOManager();

    Int_t nEvents = 100000;
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        // cout<<"Event: "<<iEv<<endl;
        Double_t x0 = fRand->Uniform(st0RangeX[0], st0RangeX[1]);
        Double_t y0 = fRand->Uniform(st0RangeY[0], st0RangeY[1]);

        Double_t x2 = fRand->Uniform(st2RangeX[0], st2RangeX[1]);
        Double_t y2 = fRand->Uniform(st2RangeY[0], st2RangeY[1]);

        // Double_t x2 = x0;
        // Double_t y2 = y0;

        auto paramsX = GetTrackParams({posZ[0], x0}, {posZ[2], x2});
        auto paramsY = GetTrackParams({posZ[0], y0}, {posZ[2], y2});

        Double_t x1 = paramsX[0] + paramsX[1]*posZ[1];
        x1 += 0.1;
        Double_t y1 = paramsY[0] + paramsY[1]*posZ[1];

        auto localHit0 = StandReverseSiliconGeoMapper::CalculateLocalCoordinatesForHit(0, 1, x0, y0);
        auto localHit1 = StandReverseSiliconGeoMapper::CalculateLocalCoordinatesForHit(1, 1, x1, y1);
        auto localHit2 = StandReverseSiliconGeoMapper::CalculateLocalCoordinatesForHit(2, 1, x2, y2);

        auto stripX0 = StandReverseSiliconGeoMapper::CalculateStripForLocalCoordinate(0, 1, 0, localHit0[0]);
        // auto stripFractionalX0 = StandReverseSiliconGeoMapper::CalculateInterStripForLocalCoordinate(0, 1, 0, localHit0[0]);
        // Double_t amplitudeX01 = 100;
        // Double_t amplitudeX02 = 0;
        // auto stripsX0Arr = StandReverseSiliconGeoMapper::DivideInterStripInTwo(stripFractionalX0, amplitudeX01, amplitudeX02);

        auto stripX1 = StandReverseSiliconGeoMapper::CalculateStripForLocalCoordinate(1, 1, 0, localHit1[0]);
        // auto stripFractionalX1 = StandReverseSiliconGeoMapper::CalculateInterStripForLocalCoordinate(1, 1, 0, localHit1[0]);
        // Double_t amplitudeX11 = 100;
        // Double_t amplitudeX12 = 0;
        // auto stripsX1Arr = StandReverseSiliconGeoMapper::DivideInterStripInTwo(stripFractionalX1, amplitudeX11, amplitudeX12);

        auto stripX2 = StandReverseSiliconGeoMapper::CalculateStripForLocalCoordinate(2, 1, 0, localHit2[0]);
        // auto stripFractionalX2 = StandReverseSiliconGeoMapper::CalculateInterStripForLocalCoordinate(2, 1, 0, localHit2[0]);
        // Double_t amplitudeX21 = 100;
        // Double_t amplitudeX22 = 0;
        // auto stripsX2Arr = StandReverseSiliconGeoMapper::DivideInterStripInTwo(stripFractionalX2, amplitudeX21, amplitudeX22);

        auto offsetY0 = StandReverseSiliconGeoMapper::CalculateOffsetY(localHit0[0], localHit0[1], 0);
        auto offsetY1 = StandReverseSiliconGeoMapper::CalculateOffsetY(localHit1[0], localHit1[1], 1);
        auto offsetY2 = StandReverseSiliconGeoMapper::CalculateOffsetY(localHit2[0], localHit2[1], 2);

        auto stripY0 = StandReverseSiliconGeoMapper::CalculateStripForLocalCoordinate(0, 1, 1, offsetY0);
        auto stripY1 = StandReverseSiliconGeoMapper::CalculateStripForLocalCoordinate(1, 1, 1, offsetY1);
        auto stripY2 = StandReverseSiliconGeoMapper::CalculateStripForLocalCoordinate(2, 1, 1, offsetY2);

        Double_t defaultAmplitude = 100;

        FillNewSiliconDigit(0, 1, 0, stripX0, defaultAmplitude);
        // FillNewSiliconDigit(0, 1, 0, stripsX0Arr[0], amplitudeX01);
        // FillNewSiliconDigit(0, 1, 0, stripsX0Arr[1], amplitudeX02);
        FillNewSiliconDigit(0, 1, 1, stripY0, defaultAmplitude);

        FillNewSiliconDigit(1, 1, 0, stripX1, defaultAmplitude);
        // FillNewSiliconDigit(1, 1, 0, stripsX1Arr[0], amplitudeX11);
        // FillNewSiliconDigit(1, 1, 0, stripsX1Arr[1], amplitudeX12);
        FillNewSiliconDigit(1, 1, 1, stripY1, defaultAmplitude);

        FillNewSiliconDigit(2, 1, 0, stripX2, defaultAmplitude);
        // FillNewSiliconDigit(2, 1, 0, stripsX2Arr[0], amplitudeX21);
        // FillNewSiliconDigit(2, 1, 0, stripsX2Arr[1], amplitudeX22);
        FillNewSiliconDigit(2, 1, 1, stripY2, defaultAmplitude);

        fIOManager->EndEvent();
    }
    
    fIOManager->FinishOutput();    
}

array<Double_t, 2> GetTrackParams(array<Double_t, 2> point1, array<Double_t, 2> point2)
{
    Double_t k = (point2[1] - point1[1])/(point2[0] - point1[0]);
    Double_t b = (point1[1]*point2[0] - point2[1]*point1[0])/(point2[0] - point1[0]);
    return {b, k};
}

void InitIOManager()
{
    fIOManager = new StandIOManager();
    fIOManager->SetOutputFileName("data/stand_run0002_digits.root");
    fIOManager->RegisterOutputBranch("SILICON", "BmnSiliconDigit");
    fIOManager->RegisterOutputBranch("DCH", "BmnDchDigit");
    fIOManager->InitOutput();
    fSiliconDigitsArray = fIOManager->GetOutputDataArray("BmnSiliconDigit");
}

void FillNewSiliconDigit(Int_t iStation, Int_t iModule, Int_t iLayer, Int_t iStrip, Double_t iSignal)
{
    new((*fSiliconDigitsArray)[fSiliconDigitsArray->GetEntriesFast()]) BmnSiliconDigit(iStation, iModule, iLayer, iStrip,iSignal);
}