#ifndef STANDDATAPRODUCER_H
#define STANDDATAPRODUCER_H

#include <TObject.h>

#include "StandIOManager.h"

class StandDataProducer : public TObject
{
public:
    StandDataProducer();
    ~StandDataProducer();

    void SetInputFileName(TString inputFileName) { fInputFileName = inputFileName; }
    void SetOutputFileName(TString outputFileName) { fOutputFileName = outputFileName; }

    void Init();
    void Finish();

private:
    
    /* data members */
    StandIOManager* fIOManager;

    TString fInputFileName;
    TString fOutputFileName;

    ClassDef(StandDataProducer, 1)
};

#endif // STANDDATAPRODUCER_H
