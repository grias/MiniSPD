#include "BmnDchRaw2Digit.h"



#include <iostream>
#include<array>

BmnDchRaw2Digit::BmnDchRaw2Digit(Int_t period, Int_t run)
{
    ReadMapFromFile(run);
}

BmnDchRaw2Digit::BmnDchRaw2Digit()
{
    fMap = NULL;
}

BmnDchRaw2Digit::~BmnDchRaw2Digit()
{
    if (fMap) delete[] fMap;
}

BmnStatus BmnDchRaw2Digit::ReadMapFromFile(Int_t period)
{
    fMap = new DchMapStructure[N_MAP_ENTRIES];
    TString fileName = Form("Straw_map_Run%d.txt", 780);
    TString path = TString(getenv("VMCWORKDIR")) + "/input/" + fileName;

    TString dummy;  
    UInt_t ser = 0;
    Int_t ch_l = 0;
    Int_t ch_h = 0;
    Int_t slot = 0;
    Int_t group = 0;
    TString name;

    TString planes[2];
    planes[0] = "BOT_OLD";
    planes[1] = "TOP_NEW";

    ifstream inFile(path.Data());
    if (!inFile.is_open())
        cout << "<DCH> Error opening map-file (" << path << ")!" << endl;
    inFile >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;
    inFile >> dummy;

    Int_t iMod = 0;
    while (!inFile.eof())
    {
        inFile >> name >> group >> std::hex >> ser >> std::dec >> slot >> ch_l >> ch_h;
        if (!inFile.good()) break;
        
        Int_t planeId;
        for (Int_t iPlane = 0; iPlane < 2; ++iPlane)
        {
            if (name != planes[iPlane]) continue;
            planeId = iPlane;
        }

        fMap[iMod].plane = planeId;
        fMap[iMod].group = group;
        fMap[iMod].crate = ser;
        fMap[iMod].slot = slot;
        fMap[iMod].channel_low = ch_l;
        fMap[iMod].channel_high = ch_h;
        iMod++;
    }
     
    return kBMNSUCCESS;
}

void BmnDchRaw2Digit::FillEvent(TClonesArray *tdc, TClonesArray *tqdc_tdc, TClonesArray *dch)
{
    std::array<UInt_t, N_TRIGGER_CHANNELS> val_tqdc;

    for (Int_t iTqdc = 0; iTqdc < tqdc_tdc->GetEntriesFast(); ++iTqdc) 
    {
        auto tqdc_digit = (BmnTDCDigit*) tqdc_tdc->At(iTqdc);
        val_tqdc[tqdc_digit->GetChannel()] = tqdc_digit->GetValue();
    }

    Double_t time_scint = (val_tqdc[0] + val_tqdc[1] + val_tqdc[2] + val_tqdc[3])*0.25;
    time_scint = 25*time_scint/1000; // channels to nanoseconds ???
    for (Int_t iTdc = 0; iTdc < tdc->GetEntriesFast(); iTdc++)
    {
        auto tdcDigit = (BmnTDCDigit*) tdc->At(iTdc);

        if (tdcDigit->GetType() != DCH_TDC_TYPE) continue;

        if ((val_tqdc[2]+val_tqdc[3])*0.5 < 300) 
        {
            printf("<BmnDchRaw2Digit::FillEvent> val_tqdc[2]+val_tqdc[3])*0.5 < 300\n");
            break; // what is this ???
        }
        FindInMap(tdcDigit, time_scint, dch);
    }
}

Int_t BmnDchRaw2Digit::GetChTDC64v(UInt_t tdc, UInt_t ch)
{
    const Int_t tdc64v_tdcch2ch[2][32] = {
        { 31, 15, 30, 14, 13, 29, 28, 12, 11, 27, 26, 10, 25, 9, 24, 8, 23, 7, 22, 6, 21, 5, 20, 4, 19, 3, 18, 2, 17, 1, 16, 0},
        { 31, 15, 30, 14, 29, 13, 28, 12, 27, 11, 26, 10, 25, 9, 24, 8, 23, 7, 22, 6, 21, 5, 20, 4, 19, 3, 18, 2, 17, 1, 16, 0}
    };
    Int_t val = tdc64v_tdcch2ch[tdc - 1][ch];
    if (tdc == 2) val += 32;
    return val;
};

BmnStatus BmnDchRaw2Digit::FindInMap(BmnTDCDigit* dig, Double_t time_scint, TClonesArray* arr)
{
    Int_t lattency_tqdc = 280;

    for (Int_t iMap = 0; iMap < N_MAP_ENTRIES; ++iMap)
    {
        DchMapStructure map = fMap[iMap];
        if (dig->GetSlot() != map.slot) continue;
        UInt_t hptdcid = dig->GetHptdcId();
        UInt_t dig_ch = dig->GetChannel();

        if (hptdcid == 2) dig_ch -= 32;
        UInt_t ch = GetChTDC64v(dig->GetHptdcId(), dig_ch);
        if (ch > map.channel_high || ch < map.channel_low) continue;
        Double_t tm = dig->GetValue()/10 - time_scint - lattency_tqdc; //divide by 10 for conversion (100 ps -> ns)
        if (tm < 0) continue;
        new((*arr)[arr->GetEntriesFast()]) BmnDchDigit(map.plane, ch, tm, 0);

        return kBMNSUCCESS;
    }
    return kBMNERROR;
}

ClassImp(BmnDchRaw2Digit)
