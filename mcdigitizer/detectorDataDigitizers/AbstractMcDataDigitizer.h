#ifndef BASEDETECTORDATADIGITIZER_H
#define BASEDETECTORDATADIGITIZER_H

#include "TString.h"
#include "TClonesArray.h"

#include "McHit.h"

#include <memory>

class AbstractMcDataDigitizer
{
private:
    TString fOutputBranchName;
    TString fOutputClassName;

    TString fDetectorName;
protected:
    TClonesArray* fDigitsArray;

public:
    AbstractMcDataDigitizer(TString outBranchName, TString outClassName, TString detectorName);

    AbstractMcDataDigitizer();
    ~AbstractMcDataDigitizer();

    TString GetOutputBranchName() { return fOutputBranchName; }
    TString GetOutputClassName() { return fOutputClassName; }

    TString GetDetectorName() { return fDetectorName; }

    void SetOutputDigitsArray(TClonesArray* outputClonesArray) { fDigitsArray = outputClonesArray; }

    virtual void ProduceDigitFromMcHit(McHit mcHit) = 0;
};

#endif // BASEDETECTORDATADIGITIZER_H
