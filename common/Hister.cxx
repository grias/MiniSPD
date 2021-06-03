#include "Hister.h"

#include "TCanvas.h"

void Hister::Fill(TString histName, UInt_t nBins, Double_t min, Double_t max, Double_t value)
{
    auto hist = GetHist(histName, nBins, min, max);
    hist->Fill(value);
}

void Hister::Set(TString histName, UInt_t nBins, Double_t min, Double_t max, Int_t iBin, Double_t value)
{
    auto hist = GetHist(histName, nBins, min, max);
    hist->SetBinContent(iBin, value);
}

TH1D* Hister::GetHist(TString histName, UInt_t nBins, Double_t min, Double_t max)
{
    auto search = fHistMap.find(histName);
    if (search != fHistMap.end())
    {
        return search->second;
    }
    else
    {
        auto hist = new TH1D(histName, histName, nBins, min, max);
        fHistMap.insert({histName, hist});
        return hist;
    }
}

void Hister::DrawAllHistogramms()
{
    auto canvas = new TCanvas("hister_canvas", "canvas", 1000, 1000);
    for (auto &&histPair : fHistMap)
    {
        histPair.second->Draw("HIST");
        auto pictureName = fSavePath + TString("hister_") + histPair.first + TString(".png");
        canvas->SaveAs(pictureName);
        canvas->Clear();
    }
    delete canvas;
}

void Hister::Delete()
{
    if (fInstance)
    {
        delete fInstance;
    }
}

Hister::Hister()
{
    fSavePath = TString(getenv("VMCWORKDIR")) + TString("/macro/pictures/");
}

Hister::~Hister()
{
    DrawAllHistogramms();
    for (auto &&histPair : fHistMap)
    {
        delete histPair.second;
    }
}

Hister* Hister::Inst()
{
    return Instance();
}

Hister* Hister::Instance()
{
    if (!fInstance)
    {
        fInstance = new Hister();        
    }

    return fInstance;
}

Hister* Hister::fInstance;