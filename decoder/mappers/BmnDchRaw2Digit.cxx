#include "BmnDchRaw2Digit.h"

BmnDchRaw2Digit::BmnDchRaw2Digit(Int_t period, Int_t run) {
    //ReadMapFromDB(period, run);
    ReadMapFromFile(run); // actually run, not period
}


BmnStatus BmnDchRaw2Digit::ReadMapFromFile(Int_t period) { // actually run, not period
    // cout<<"DCH ReadMapFromFile"<<endl;
    fEntriesInMap1 = 2;

    fMap1 = new DchMapStructure[fEntriesInMap1];
    // TString fileName = Form("Straw_map_Run%d.txt", period); // [CHANGED]
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
    while (!inFile.eof()) {
        inFile >> name >> group >> std::hex >> ser >> std::dec >> slot >> ch_l >> ch_h;
        if (!inFile.good()) break;
        
        printf("%s\t%d\t0x%x\t%d\t%d\t%d\n", name.Data(), group, ser, slot, ch_l, ch_h);

        Int_t planeId;
        for (Int_t iPlane = 0; iPlane < 2; ++iPlane)
        {
            if (name != planes[iPlane]) continue;
            planeId = iPlane;
        }

        fMap1[iMod].plane = planeId;
        fMap1[iMod].group = group;
        fMap1[iMod].crate = ser;
        fMap1[iMod].slot = slot;
        fMap1[iMod].channel_low = ch_l;
        fMap1[iMod].channel_high = ch_h;
        iMod++;
            
    }
}

void BmnDchRaw2Digit::FillEvent(TClonesArray *tdc, TClonesArray *tqdc_tdc, TClonesArray *dch) {
    // cout<<"DCH FillEvent"<<endl;
    BmnTDCDigit *tqdc_digit;
    Int_t numbTQDC=0;
    UInt_t val_tqdc0=0;
    UInt_t val_tqdc1=0;
    UInt_t val_tqdc2=0;
    UInt_t val_tqdc3=0;
    UInt_t tqdc_tstamp0=0;
    UInt_t tqdc_tstamp1=0;
    UInt_t tqdc_tstamp2=0;
    UInt_t tqdc_tstamp3=0;
    Double_t time_scint;
    for (Int_t iTqdc = 0; iTqdc < tqdc_tdc->GetEntriesFast(); ++iTqdc) 
    {
        if (tqdc_tdc->GetEntriesFast()>5) break;
        tqdc_digit = (BmnTDCDigit*) tqdc_tdc->At(iTqdc);
        if (tqdc_digit->GetChannel()==0) {val_tqdc0=tqdc_digit->GetValue(); tqdc_tstamp0 = tqdc_digit->GetTimestamp(); }
        if (tqdc_digit->GetChannel()==1) {val_tqdc1=tqdc_digit->GetValue(); tqdc_tstamp1 = tqdc_digit->GetTimestamp(); }
        if (tqdc_digit->GetChannel()==2) {val_tqdc2=tqdc_digit->GetValue(); tqdc_tstamp2 = tqdc_digit->GetTimestamp(); }
        if (tqdc_digit->GetChannel()==3) {val_tqdc3=tqdc_digit->GetValue(); tqdc_tstamp3 = tqdc_digit->GetTimestamp(); }
        numbTQDC++;
    }
    // printf("<TQDC_TDC Timestamp> Ch0: %d, Ch1: %d, Ch2: %d, Ch3: %d\n", tqdc_tstamp0, tqdc_tstamp1, tqdc_tstamp2, tqdc_tstamp3);
    // printf("<TQDC_TDC Value> Ch0: %x, Ch1: %x, Ch2: %x, Ch3: %x\n", val_tqdc0, val_tqdc1, val_tqdc2, val_tqdc3);
    //cout<<"numbTQDC = "<< numbTQDC <<endl;
    // time_scint=(val_tqdc2+val_tqdc3)*0.5;// - 25*(tqdc_tstamp2+tqdc_tstamp3)*0.5;
    time_scint=(val_tqdc0 + val_tqdc1 + val_tqdc2 + val_tqdc3)*0.25;// - 25*(tqdc_tstamp2+tqdc_tstamp3)*0.5; [CHANGED]
    time_scint=25*time_scint/1000;
    for (Int_t i = 0; i < tdc->GetEntriesFast(); i++) {
        //cout<<"Loop in FillEvent"<<endl;
        BmnTDCDigit *digit = (BmnTDCDigit*) tdc->At(i);
        //BmnTDCDigit *tqdc_digit=(BmnTDCDigit*) tqdc_tdc->At(i);
        if (digit->GetType() != DCH_TDC_TYPE) continue;
        // if (tdc->GetEntriesFast()!=2) break; // [HERE] Что если сработает больше двух трубок
        if ((val_tqdc2+val_tqdc3)*0.5<300) break;
        //map<UInt_t, Long64_t>::iterator it = ts->find(digit->GetSerial());
        //Long64_t timeShift = (it != ts->end()) ? it->second : 0;
        FindInMap(digit, time_scint, dch);
    }
}

Int_t BmnDchRaw2Digit::GetChTDC64v(UInt_t tdc, UInt_t ch) {
    //this is some Vishnevsky's magic!
    //FIXME! What is going here?!
    // cout<<"DCH GetChTDC64v"<<endl;
    const Int_t tdc64v_tdcch2ch[2][32] = {
        { 31, 15, 30, 14, 13, 29, 28, 12, 11, 27, 26, 10, 25, 9, 24, 8, 23, 7, 22, 6, 21, 5, 20, 4, 19, 3, 18, 2, 17, 1, 16, 0},
        { 31, 15, 30, 14, 29, 13, 28, 12, 27, 11, 26, 10, 25, 9, 24, 8, 23, 7, 22, 6, 21, 5, 20, 4, 19, 3, 18, 2, 17, 1, 16, 0}
    };
    Int_t val = tdc64v_tdcch2ch[tdc - 1][ch];
    //cout<<"hptdcid= "<<tdc<<" channel= "<<val<<endl;
    if (tdc == 2) val += 32;
    // printf("<BmnDchRaw2Digit> OldCh: %d, NewCh: %d\n", ch, val);
    return val;
};

BmnStatus BmnDchRaw2Digit::FindInMap(BmnTDCDigit* dig, Double_t time_scint, TClonesArray* arr) {
    // cout<<"DCH FindInMap!"<<endl;
    DchMapStructure* mapArr = fMap1;
    Int_t nEntriesInMap =fEntriesInMap1;
    Int_t lattency_tqdc = 280;
    // printf("<BmnDchRaw2Digit> Type: %d, Serial: 0x%x, Slot: %d\n", dig->GetType(), dig->GetSerial(), dig->GetSlot());
    // cout<<"BmnTDCDigit\tType: "<<dig->GetType()<<"\tSlot: "<<hex<<dig->GetSlot()<<"\tChannel: "<<dig->GetChannel()<<endl;
    for (Int_t iMap = 0; iMap < nEntriesInMap; ++iMap) {
        DchMapStructure map = mapArr[iMap];
        if (dig->GetSlot() != map.slot) continue;
        UInt_t hptdcid = dig->GetHptdcId();
        UInt_t dig_ch=dig->GetChannel();
        //cout<<"dig_ch= "<<dig_ch<<endl;
        if (hptdcid==2) dig_ch-=32;
        UInt_t ch = GetChTDC64v(dig->GetHptdcId(), dig_ch);
        //cout<<"TDC Channel= "<< ch<< " HptdcId= "<< hptdcid <<endl;
        if (ch > map.channel_high || ch < map.channel_low) continue;
        Double_t tm = dig->GetValue()/10 - time_scint - lattency_tqdc; //divide by 10 for conversion (100 ps -> ns)
        if (tm<0) continue;
        new((*arr)[arr->GetEntriesFast()]) BmnDchDigit(map.plane, ch, tm, 0);
        // printf("<BmnDchRaw2Digit>New DCH digit Plane: %d, Channel: 0x%x, Time: %d\n", map.plane, ch, tm);
        return kBMNSUCCESS;
    }
    //return kBMNERROR;
}

ClassImp(BmnDchRaw2Digit)
