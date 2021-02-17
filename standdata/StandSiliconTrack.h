#ifndef STANDSILICONTRACK_H
#define STANDSILICONTRACK_H

#include <TObject.h>

class StandSiliconTrack : public TObject
{

public:
    StandSiliconTrack();
    ~StandSiliconTrack();

    Int_t GetHitId(Int_t station) { return HitsId[station]; }
    Double_t GetHitPositionX(Int_t station) { return hitPosX[station]; }
    Double_t GetHitPositionY(Int_t station) { return hitPosY[station]; }
    Double_t GetHitPositionZ(Int_t station) { return hitPosZ[station]; }
    Int_t GetModule(Int_t station) { return modules[station]; }
    Double_t GetResidualX(Int_t station) { return residualsX[station]; }
    Double_t GetResidualY(Int_t station) { return residualsY[station]; }
    Double_t GetParameterX(Int_t nPar) { return parsX[nPar]; }
    Double_t GetParameterY(Int_t nPar) { return parsY[nPar]; }
    Double_t GetChiSquare(Int_t coord) { return chiSquare[coord]; }

    void SetHitId(Int_t nhits[3]) {  HitsId[0] = nhits[0]; HitsId[1] = nhits[1]; HitsId[2] = nhits[2]; }
    void SetHitPositionX(Double_t pos[3]) {  hitPosX[0] = pos[0]; hitPosX[1] = pos[1]; hitPosX[2] = pos[2]; }
    void SetHitPositionY(Double_t pos[3]) {  hitPosY[0] = pos[0]; hitPosY[1] = pos[1]; hitPosY[2] = pos[2]; }
    void SetHitPositionZ(Double_t pos[3]) {  hitPosZ[0] = pos[0]; hitPosZ[1] = pos[1]; hitPosZ[2] = pos[2]; }
    void SetModules(Int_t newmod[3]) { modules[0] = newmod[0]; modules[1] = newmod[1]; modules[2] = newmod[2]; }
    void SetResidualsX(Double_t res[3]) { residualsX[0] = res[0]; residualsX[1] = res[1]; residualsX[2] = res[2]; }
    void SetResidualsY(Double_t res[3]) { residualsY[0] = res[0]; residualsY[1] = res[1]; residualsY[2] = res[2]; }
    void SetParsX(Double_t pars[2]) { parsX[0] = pars[0]; parsX[1] = pars[1]; }
    void SetParsY(Double_t pars[2]) { parsY[0] = pars[0]; parsY[1] = pars[1]; }
    void SetChiSquare(Double_t newChi[2]) { chiSquare[0] = newChi[0]; chiSquare[1] = newChi[1]; }

private:
    Int_t HitsId[3];
    Double_t hitPosX[3];
    Double_t hitPosY[3];
    Double_t hitPosZ[3];
    Int_t modules[3];
    Double_t residualsX[3];
    Double_t residualsY[3];
    Double_t parsX[2];
    Double_t parsY[2];
    Double_t chiSquare[2];

    ClassDef(StandSiliconTrack, 1);
};

#endif // STANDSILICONTRACK_H
