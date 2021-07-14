#ifndef STANDTRACK_H
#define STANDTRACK_H

#include <TObject.h>

#include "array"

#define STANDTRACKNHITS 5

class StandTrack : public TObject
{
private:
    std::array<Bool_t, STANDTRACKNHITS> fIsStationIncluded;
    std::array<Int_t, STANDTRACKNHITS> fHitId;
    std::array<Double_t, STANDTRACKNHITS> fHitPositionX;
    std::array<Double_t, STANDTRACKNHITS> fHitPositionY;
    std::array<Double_t, STANDTRACKNHITS> fHitPositionZ;
    std::array<Int_t, STANDTRACKNHITS> fIModule;
    std::array<Double_t, STANDTRACKNHITS> fResidualX;
    std::array<Double_t, STANDTRACKNHITS> fResidualY;
    std::array<Double_t, 2> fParsX;
    std::array<Double_t, 2> fParsY;
    std::array<Double_t, 2> fChiSquare;

    ClassDef(StandTrack, 1);

public:
    StandTrack();
    ~StandTrack();

    Bool_t IsStationIncluded(Int_t station) { return fIsStationIncluded[station]; }
    Int_t GetHitId(Int_t station) { return fHitId[station]; }
    Double_t GetHitPositionX(Int_t station) { return fHitPositionX[station]; }
    Double_t GetHitPositionY(Int_t station) { return fHitPositionY[station]; }
    Double_t GetHitPositionZ(Int_t station) { return fHitPositionZ[station]; }
    Int_t GetModule(Int_t station) { return fIModule[station]; }
    Double_t GetResidualX(Int_t station) { return fResidualX[station]; }
    Double_t GetResidualY(Int_t station) { return fResidualY[station]; }
    Double_t GetParameterX(Int_t nPar) { return fParsX[nPar]; }
    Double_t GetParameterY(Int_t nPar) { return fParsY[nPar]; }
    Double_t GetChiSquare(Int_t coord) { return fChiSquare[coord]; }

    void IncludeStation(Int_t station) { fIsStationIncluded[station] = kTRUE; }
    void SetHitId(Int_t station, Int_t hitId) { fHitId[station] = hitId; }
    void SetHitPositionX(Int_t station, Double_t pos) { fHitPositionX[station] = pos; }
    void SetHitPositionY(Int_t station, Double_t pos) { fHitPositionY[station] = pos; }
    void SetHitPositionZ(Int_t station, Double_t pos) { fHitPositionZ[station] = pos; }
    void SetModules(Int_t station, Int_t iModule) { fIModule[station] = iModule; }
    void SetResidualsX(Int_t station, Double_t res) { fResidualX[station] = res; }
    void SetResidualsY(Int_t station, Double_t res) { fResidualY[station] = res; }
    void SetParsX(Double_t pars[2]) { fParsX[0] = pars[0]; fParsX[1] = pars[1]; }
    void SetParsY(Double_t pars[2]) { fParsY[0] = pars[0]; fParsY[1] = pars[1]; }
    void SetChiSquare(Double_t chi[2]) { fChiSquare[0] = chi[0]; fChiSquare[1] = chi[1]; }
};

#endif // STANDTRACK_H
