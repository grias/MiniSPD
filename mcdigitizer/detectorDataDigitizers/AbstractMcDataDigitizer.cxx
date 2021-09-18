#include "AbstractMcDataDigitizer.h"

AbstractMcDataDigitizer::AbstractMcDataDigitizer(TString outBranchName, TString outClassName, TString detectorName):
fOutputBranchName(outBranchName),
fOutputClassName(outClassName),
fDetectorName(detectorName),
fDigitsArray(NULL)
{
}

AbstractMcDataDigitizer::AbstractMcDataDigitizer():
fOutputBranchName(""),
fOutputClassName(""),
fDetectorName(""),
fDigitsArray(NULL)
{
}

AbstractMcDataDigitizer::~AbstractMcDataDigitizer()
{
}
