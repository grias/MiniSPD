#ifndef BMNGEMSTRIPDIGIT_H
#define	BMNGEMSTRIPDIGIT_H

#include <iostream>
#include <vector>
#include "Rtypes.h"
#include "TNamed.h"

using namespace std;

class BmnGemStripDigit : public TNamed {
public:
    BmnGemStripDigit();
    BmnGemStripDigit(BmnGemStripDigit* digit);
    BmnGemStripDigit(Int_t iStation, Int_t iStripLayer, Int_t iStripNumber, Double_t iStripSignal);

    virtual ~BmnGemStripDigit();

    Int_t GetStation() { return fStation; }
    Int_t GetStripLayer() { return fLayer; }
    Int_t GetStripNumber() { return fStripNumber; }
    Double_t GetStripSignal() { return fStripSignal; }
    Bool_t IsGoodDigit() { return fIsGoodDigit; }

    void SetStation(Int_t station) { fStation = station; }
    void SetStripLayer(Int_t layer) { fLayer = layer; }
    void SetStripNumber(Int_t num) { fStripNumber = num; }
    void SetStripSignal(Double_t signal) { fStripSignal = signal; }
    void SetIsGoodDigit(Bool_t tmp) { fIsGoodDigit = tmp; }

private:
    Int_t fStation;
    Int_t fLayer;
    Int_t fStripNumber;
    Double_t fStripSignal;
    Bool_t fIsGoodDigit;

    ClassDef(BmnGemStripDigit,1);
};

#endif

