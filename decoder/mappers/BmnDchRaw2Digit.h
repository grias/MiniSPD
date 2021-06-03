#ifndef BMNDCHRAW2DIGIT_H
#define BMNDCHRAW2DIGIT_H

#include "TClonesArray.h"
#include "BmnTDCDigit.h"
#include "BmnDchDigit.h"
#include "Riostream.h"
#include "BmnEnums.h"
#include "BmnTQDCADCDigit.h"
#include <cstdlib>

#define DCH_TDC_TYPE (0x10) //TDC64V

#define N_TRIGGER_CHANNELS    5

struct DchMapStructure
{
    int plane;
    unsigned int crate;
    int slot;
    int channel_low;
    int channel_high;
};

class BmnDchRaw2Digit
{
public:
    BmnDchRaw2Digit(Int_t period, Int_t run);
    BmnDchRaw2Digit();
    ~BmnDchRaw2Digit();

    void FillEvent(TClonesArray *tdc, TClonesArray *tqdc_tdc, TClonesArray *dch);

private:
    size_t fNMapEntries;
    DchMapStructure* fMap;

    Int_t GetChTDC64v(UInt_t tdc, UInt_t ch);
    BmnStatus FindInMap(BmnTDCDigit* dig, Double_t time_scint, TClonesArray* arr);
    BmnStatus ReadMapFromFile(Int_t period);

    ClassDef(BmnDchRaw2Digit, 1);
};
#endif /* BMNDCHRAW2DIGIT_H */
