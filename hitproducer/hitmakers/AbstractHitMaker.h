#ifndef ABSTRACTHITMAKER_H
#define ABSTRACTHITMAKER_H

#include "TString.h"
#include "TClonesArray.h"

class AbstractHitMaker
{
private:
    TString fInputBranchName;
    TString fOutputBranchName;

    TString fInputClassName;
    TString fOutputClassName;

protected:
    TClonesArray* fDigitsArray;
    TClonesArray* fHitsArray;

public:
    AbstractHitMaker(TString inBranchName, TString outBranchName, TString inClassName, TString outClassName);

    AbstractHitMaker();
    ~AbstractHitMaker();

    TString GetInputBranchName() { return fInputBranchName; }
    TString GetOutputBranchName() { return fOutputBranchName; }

    TString GetInputClassName() { return fInputClassName; }
    TString GetOutputClassName() { return fOutputClassName; }

    void SetInputDigitsArray(TClonesArray* inputClonesArray) { fDigitsArray = inputClonesArray; }
    void SetOutputHitsArray(TClonesArray* outputClonesArray) { fHitsArray = outputClonesArray; }

    virtual void ProduceHitsFromCurrentEvent() = 0;
};

#endif /* ABSTRACTHITMAKER_H */
