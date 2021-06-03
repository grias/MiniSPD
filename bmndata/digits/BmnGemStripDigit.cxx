#include "BmnGemStripDigit.h"

BmnGemStripDigit::BmnGemStripDigit() {
    fStation = -1;
    fLayer = 0;
    fStripNumber = 0;
    fStripSignal = 0.0;
    fIsGoodDigit = kTRUE;
}

BmnGemStripDigit::BmnGemStripDigit(BmnGemStripDigit* digit) {
    fStation = digit->fStation;
    fLayer = digit->fLayer;
    fStripNumber = digit->fStripNumber;
    fStripSignal = digit->fStripSignal;
    fIsGoodDigit = digit->fIsGoodDigit;
}

BmnGemStripDigit::BmnGemStripDigit(Int_t iStation, Int_t iStripLayer, Int_t iStripNumber, Double_t iStripSignal) {
    fStation = iStation;
    fLayer = iStripLayer;
    fStripNumber = iStripNumber;
    fStripSignal = iStripSignal;
}

BmnGemStripDigit::~BmnGemStripDigit() {

}

ClassImp(BmnGemStripDigit)
