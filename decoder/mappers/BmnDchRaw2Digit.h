#ifndef BMNDCHRAW2DIGIT_H
#define BMNDCHRAW2DIGIT_H

#include "TString.h"
#include "TClonesArray.h"
#include "BmnTDCDigit.h"
#include "BmnSyncDigit.h"
#include "BmnDchDigit.h"
#include <iostream>
#include "Riostream.h"
#include "BmnEnums.h"
#include "BmnTQDCADCDigit.h"
#include <cstdlib>
#include <bitset>
#include <map>
#include "TMath.h"

#include "db_structures.h"

#define DCH_TDC_TYPE (0x10) //TDC64V
#define DCH_TQDC_TYPE (0x56)

#define HPTIMEBIN 0.02344
#define INVHPTIMEBIN 42.6666
#define KNBDCHANNELS 40
#define ADC_CLOCK_TQDC16VS  8.0  // nano seconds
#define ADC_CLOCK          12.0
#define ADC_CLOCK_OLD      12.5
#define TDC_CLOCK          24.0
#define TDC_CLOCK_OLD      25.0

class BmnDchRaw2Digit {
public:
    BmnDchRaw2Digit(Int_t period, Int_t run);
    BmnDchRaw2Digit() {
        fMap1 = NULL;
    };
    ~BmnDchRaw2Digit() {
        if (fMap1) delete[] fMap1;
    };

    void FillEvent(TClonesArray *tdc, TClonesArray *tqdc_tdc, TClonesArray *dch);

private:

    DchMapStructure* fMap1;

    Int_t fEntriesInMap1; // member of entries in BD table for DCH1

    Int_t GetChTDC64v(UInt_t tdc, UInt_t ch);
    BmnStatus FindInMap(BmnTDCDigit* dig, Double_t time_scint, TClonesArray* arr);

    BmnStatus ReadMapFromFile(Int_t period);
    BmnStatus ReadMapFromDB(Int_t period, Int_t run);

    ClassDef(BmnDchRaw2Digit, 1);
};
#endif /* BMNDCHRAW2DIGIT_H */
