#ifndef BMNSILICONRAW2DIGIT_H
#define BMNSILICONRAW2DIGIT_H

#include "TString.h"
#include "TTree.h"
#include "TClonesArray.h"
#include <iostream>
#include "TH1F.h"
#include "TH1D.h"
#include "BmnADCDigit.h"
#include "BmnEnums.h"
#include "TMath.h"
#include <fstream>
#include <list>
#include <map>
#include <vector>
#include "BmnSiliconDigit.h"
#include "BmnAdcProcessor.h"

using namespace std;
using namespace TMath;

struct BmnSiliconMapping {
    Short_t layer; //X = 0 or X' = 1
    UInt_t serial;
    Short_t module;
    Short_t channel_low;
    Short_t channel_high;
    Short_t station;
};

class BmnSiliconRaw2Digit : public BmnAdcProcessor {
public:
    BmnSiliconRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer);
    BmnSiliconRaw2Digit();
    virtual ~BmnSiliconRaw2Digit();

    BmnStatus FillEvent(TClonesArray *adc, TClonesArray *sts, Int_t iEv);
    BmnStatus FillProfiles(TClonesArray *adc, Int_t iEv);
    BmnStatus FillNoisyChannels();

private:
    vector<BmnSiliconMapping> fMap;
    Int_t fEventId;
    TString fMapFileName;

    TH1F**** fSigProf;
    TH1D* fSig;
    TH1D* fSigMp;
    TH1D* fSigMpMc;
    TH1D* fPed;
    TH1D* fSigma;
    TH1D* fSigmaX3;
    TH1D* fClearLine;
    TH1D* fClearLineX3;
    TH1D* fClearLineX6;
    Bool_t**** fNoisyChannels;

    BmnStatus ReadMapFile();
    void ProcessDigit(BmnADCDigit* adcDig, BmnSiliconMapping* silM, TClonesArray *silicon, Bool_t doFill, Int_t iEv);

    ClassDef(BmnSiliconRaw2Digit, 1);
};

#endif /* BMNSILICONRAW2DIGIT_H */
