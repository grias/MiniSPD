#if !defined(McHIT_H)
#define McHIT_H

#include "TString.h"
#include "TObject.h"

class McHit : public TObject
{
private:
    Double_t fX;
    Double_t fY;
    Double_t fZ;

    Int_t fStation;
    Int_t fModule;

    TString fDetectorName;

    ClassDef(McHit, 1);
public:
    McHit(Double_t x, Double_t y, Double_t z, Int_t station, Int_t module, TString detName);

    McHit();
    ~McHit();

    Double_t GetX() { return fX; }
    Double_t GetY() { return fY; }
    Double_t GetZ() { return fZ; }
    Int_t GetStation() { return fStation; }
    Int_t GetModule() { return fModule; }
    TString GetDetectorName() { return fDetectorName; }
};

#endif // McHIT_H
