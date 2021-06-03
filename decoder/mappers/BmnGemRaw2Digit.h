#ifndef BMNGEMRAW2DIGIT_H
#define BMNGEMRAW2DIGIT_H

#include "TString.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TMath.h"

#include "BmnGemStripDigit.h"
#include "BmnADCDigit.h"
#include "BmnEnums.h"
#include "BmnAdcProcessor.h"

#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <vector>

#define N_CH_IN_CRATE 2048 //number of channels in one crate (64ch x 32smpl))
#define N_CH_IN_SMALL_GEM 512 //number of channels in small GEM stations (sum of all redout channels)
#define N_CH_IN_MID_GEM 2176 //number of channels in middle GEM stations (sum of all redout channels)
#define N_CH_IN_BIG_GEM 3200 //number of channels in one part of big GEM stations (sum of all redout channels)
#define N_CH_IN_BIG_GEM_0 1023//988 //number of channels in hot zone of one part of big GEM stations (sum of redout channels from X0 and Y0)
#define N_CH_IN_BIG_GEM_1 2176//2100 //number of channels in big zone of one part of big GEM stations (sum of redout channels from X1 and Y1)
#define N_CH_BUF 4096
#define N_STATIONS 2
#define N_SIDES 2
#define N_STRIPS 768

using namespace std;
using namespace TMath;

struct GemMapStructure
{
    unsigned int serial;
    int channel_low;
    int channel_high;
    int station;
    int side;
    int part;
};

struct BmnGemMap {
    Int_t strip;
    Int_t lay; // strip type: 0 - x, 1 - y
    Int_t mod; //hot zones: 1 - inner zone, 0 - outer zone

    BmnGemMap(Int_t s, Int_t l, Int_t m) : strip(s), lay(l), mod(m) {
    }

    BmnGemMap() : strip(0), lay(0), mod(0) {
    }
};

class BmnGemRaw2Digit : public BmnAdcProcessor {
public:
    BmnGemRaw2Digit(Int_t period, Int_t run, vector<UInt_t> vSer, TString mapFileName);
    BmnGemRaw2Digit();
    ~BmnGemRaw2Digit();

    BmnStatus FillEvent(TClonesArray *adc, TClonesArray *gem, UInt_t event);
    BmnStatus FillProfiles(TClonesArray *adc);
    BmnStatus FillNoisyChannels();

private:
        
    TH1F*** fSigProf;
    Bool_t*** fNoisyChannels;

    TH1D* fRaw;
    TH1D* fRawMPed;
    TH1D* fRawMPedCMS;
    TH1D* fPed;
    TH1D* fPedRms;
    TH1D* fCMS;
    TH1D* fNoiseLvl;
    TH1D* fTreshold;
    
    TString fMapFileName;

    vector<GemMapStructure> fMap;

    void ProcessDigit(BmnADCDigit* adcDig, GemMapStructure* gemM, TClonesArray *gem, Bool_t doFill, UInt_t event);

    Int_t fEntriesInGlobMap; // number of entries in BD table for Global Mapping

    ClassDef(BmnGemRaw2Digit, 1);
};

#endif /* BMNGEMRAW2DIGIT_H */

