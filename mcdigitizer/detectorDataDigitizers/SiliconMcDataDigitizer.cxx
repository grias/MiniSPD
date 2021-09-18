#include "SiliconMcDataDigitizer.h"

#include "StandReverseSiliconGeoMapper.h"
#include "BmnSiliconDigit.h"

#include "TRandom3.h"

#include <vector>
#include <array>

SiliconMcDataDigitizer::SiliconMcDataDigitizer():AbstractMcDataDigitizer("SILLICON", "BmnSiliconDigit", "Silicon")
{
}

SiliconMcDataDigitizer::~SiliconMcDataDigitizer()
{
}

void SiliconMcDataDigitizer::ProduceDigitFromMcHit(McHit mcHit)
{
    std::vector<std::pair<Int_t, Double_t>> stripAmpPairsXSide;
    std::vector<std::pair<Int_t, Double_t>> stripAmpPairsYSide;

    auto localHit = StandReverseSiliconGeoMapper::CalculateLocalCoordinatesForHit(mcHit.GetStation(), mcHit.GetModule(), mcHit.GetX(), mcHit.GetY());

    Int_t nStripsX = RollNStrips(0);

    if (nStripsX == 1)
    {
        auto stripX0 = StandReverseSiliconGeoMapper::CalculateStripForLocalCoordinate(mcHit.GetStation(), mcHit.GetModule(), 0, localHit[0]);
    }
    else if (nStripsX == 2)
    {
        auto stripFractionalX = StandReverseSiliconGeoMapper::CalculateInterStripForLocalCoordinate(mcHit.GetStation(), mcHit.GetModule(), 0, localHit[0]);
        std::vector<Double_t> amplitudesX = {100, 0};
        auto stripsXArr = StandReverseSiliconGeoMapper::DivideInterStripInTwo(stripFractionalX, amplitudesX[0], amplitudesX[1]);

        for (size_t iStrip = 0; iStrip < 2; iStrip++)
        {
            stripAmpPairsXSide.push_back({stripsXArr[iStrip], amplitudesX[iStrip]});
        }
    }
    

    

    auto offsetY = StandReverseSiliconGeoMapper::CalculateOffsetY(localHit[0], localHit[1], mcHit.GetStation());
    auto stripY = StandReverseSiliconGeoMapper::CalculateStripForLocalCoordinate(mcHit.GetStation(), mcHit.GetModule(), 1, offsetY);

    std::vector<Double_t> amplitudesY = {100};
    stripAmpPairsYSide.push_back({stripY, amplitudesY[0]});

    for (auto &&stripAmpPair : stripAmpPairsXSide)
    {
        FillNewSiliconDigit(mcHit.GetStation(), mcHit.GetModule(), 0, stripAmpPair.first, stripAmpPair.second);
    }

    for (auto &&stripAmpPair : stripAmpPairsYSide)
    {
        FillNewSiliconDigit(mcHit.GetStation(), mcHit.GetModule(), 1, stripAmpPair.first, stripAmpPair.second);
    }
}

void SiliconMcDataDigitizer::FillNewSiliconDigit(Int_t iStation, Int_t iModule, Int_t iLayer, Int_t iStrip, Double_t iSignal)
{
    new((*fDigitsArray)[fDigitsArray->GetEntriesFast()]) BmnSiliconDigit(iStation, iModule, iLayer, iStrip,iSignal);
}

Int_t SiliconMcDataDigitizer::RollNStrips(Int_t side)
{
    auto rand = new TRandom3();
    auto numb = rand->Uniform();
    std::vector<std::pair<Int_t, Double_t>> stripProb;
    stripProb.push_back({1, 0.851});
    stripProb.push_back({2, 0.851 + 0.128});
    stripProb.push_back({3, 0.851 + 0.128 + 0.021});

    for (auto &&pair : stripProb)
    {
        if (numb <= pair.second)
        {
            return pair.first;
        }
    }
    return -1;
}