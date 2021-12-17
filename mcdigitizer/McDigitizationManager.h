#ifndef McDIGITIZER_H
#define McDIGITIZER_H

#include "detectorDataDigitizers/AbstractMcDataDigitizer.h"
#include "McHit.h"

#include "TClonesArray.h"
#include "TString.h"

#include <memory>
#include <map>

class StandIOManager;

class McDigitizationManager
{
private:
    std::shared_ptr<StandIOManager> fIOManager;
    std::shared_ptr<StandIOManager> fIOManagerMc;

    TClonesArray* fMcHitsArray;
    
    std::map<TString, std::shared_ptr<AbstractMcDataDigitizer>> fDigitizersMap;

    void ConfigureIOManagerForDigits(int runId);
    void ConfigureIOManagerForMc(int runId);

    void RegisterDigitizersInIOManager();
    void SendClonesArraysToDigitizers();

public:
    McDigitizationManager(int runId);
    McDigitizationManager(); // runId = 8
    ~McDigitizationManager();

    void AddDigitizer(std::shared_ptr<AbstractMcDataDigitizer> digitizer);

    void InitOutput();

    void AddHit(McHit mcHit);
    void EndEvent();

    void SaveOutputFiles();
};


#endif // McDIGITIZER_H
