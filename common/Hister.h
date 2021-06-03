#ifndef HISTER_H
#define HISTER_H

#include "TH1D.h"
#include <map>

class Hister
{
public:
    void Fill(TString histName, UInt_t nBins, Double_t min, Double_t max, Double_t value);
    void Set(TString histName, UInt_t nBins, Double_t min, Double_t max, Int_t iBin, Double_t value);

    static Hister* Inst(); // same as Instance() just shorter name
    static Hister* Instance();

    static void Delete();

private:
    std::map<TString, TH1D*> fHistMap;
    TString fSavePath;

    TH1D* GetHist(TString histName, UInt_t nBins, Double_t min, Double_t max);
    void DrawAllHistogramms();

    Hister();
    ~Hister();
    static Hister* fInstance;

    ClassDef(Hister, 1);
};

#endif /* HISTER_H */
