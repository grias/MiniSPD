#include "AbstractHitMaker.h"

AbstractHitMaker::AbstractHitMaker(TString inBranchName, TString outBranchName, TString inClassName, TString outClassName):
fInputBranchName(inBranchName),
fOutputBranchName(outBranchName),
fInputClassName(inClassName),
fOutputClassName(outClassName),
fDigitsArray(NULL),
fHitsArray(NULL)
{
}

AbstractHitMaker::AbstractHitMaker():
fInputBranchName(""),
fOutputBranchName(""),
fInputClassName(""),
fOutputClassName(""),
fDigitsArray(NULL),
fHitsArray(NULL)
{
}

AbstractHitMaker::~AbstractHitMaker()
{
}