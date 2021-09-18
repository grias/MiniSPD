#if !defined(SILLICONMCDATADIGITIZER_H)
#define SILLICONMCDATADIGITIZER_H

#include "AbstractMcDataDigitizer.h"

class SiliconMcDataDigitizer : public AbstractMcDataDigitizer
{
private:
    void FillNewSiliconDigit(Int_t iStation, Int_t iModule, Int_t iLayer, Int_t iStrip, Double_t iSignal);
    Int_t RollNStrips(Int_t side);
public:
    SiliconMcDataDigitizer();
    ~SiliconMcDataDigitizer();

    void ProduceDigitFromMcHit(McHit mcHit);
};

#endif // SILLICONMCDATADIGITIZER_H
