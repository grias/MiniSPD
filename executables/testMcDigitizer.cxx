#include "McDigitizationManager.h"
#include "SiliconMcDataDigitizer.h"

#include "McHit.h"

#include <memory>

int main(int argc, char const *argv[])
{
    auto digiManager = std::make_shared<McDigitizationManager>();
    digiManager->AddDigitizer(std::make_shared<SiliconMcDataDigitizer>());
    digiManager->InitOutput();

    McHit hit(1,1,1,1,1,"Silicon");
    digiManager->AddHit(hit);
    digiManager->EndEvent();

    return 0;
}
