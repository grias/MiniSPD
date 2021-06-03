#ifndef BMNSILICONRAW2DIGIT_H
#define BMNSILICONRAW2DIGIT_H

#include "TClonesArray.h"
#include "BmnAdcProcessor.h"
#include "BmnADCDigit.h"
#include "BmnEnums.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define N_SI_STATIONS    3
#define N_SI_MODULES     4
#define N_SI_LAYERS      2
#define N_SI_STRIPS    640

struct BmnSiliconMapping {
    Short_t layer;
    UInt_t serial;
    Short_t module;
    Short_t channel_low;
    Short_t channel_high;
    Short_t station;
};

class BmnSiliconRaw2Digit : public BmnAdcProcessor
{
public:
    BmnSiliconRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer);
    BmnSiliconRaw2Digit();
    virtual ~BmnSiliconRaw2Digit();

    BmnStatus FillEvent(TClonesArray *adc, TClonesArray *sts, Int_t iEv);
    BmnStatus FillNoisyChannels();

private:
    vector<BmnSiliconMapping> fMap;
    TString fMapFileName;

    Bool_t**** fNoisyChannels;

    BmnStatus ReadMapFile();
    void ProcessDigit(BmnADCDigit* adcDig, BmnSiliconMapping* silM, TClonesArray *silicon, Bool_t doFill, Int_t iEv);

    ClassDef(BmnSiliconRaw2Digit, 1);
};

#endif /* BMNSILICONRAW2DIGIT_H */
