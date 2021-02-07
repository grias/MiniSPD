const bool kVERBOSE_MODE = false;

#include "BmnRawDataDecoder.h"

#include "BmnMath.h"

#include <sys/stat.h>
#include <arpa/inet.h> /* For ntohl for Big Endian LAND. */

using namespace std;


BmnRawDataDecoder::BmnRawDataDecoder() {
    fRunId = 0;
    fPeriodId = 0;
    fEventId = 0;
    fNevents = 0;
    fMaxEvent = 0;
    fLengthRawFile = 0;
    fCurentPositionRawFile = 0;
    runHeaderDAQ = NULL;
    eventHeaderDAQ = NULL;
    runHeader = NULL;
    eventHeader = NULL;
    fTime_ns = 0;
    fTime_s = 0;
    fT0Time = 0.0;
    fRawTree = NULL;
    fDigiTree = NULL;
    fRootFileIn = NULL;
    fRootFileOut = NULL;
    fRawFileIn = NULL;
    fDigiFileOut = NULL;
    sync = NULL;
    tdc = NULL;
    tqdc_tdc = NULL;
    tqdc_adc = NULL;
    adc32 = NULL;
    adc128 = NULL;
    adc = NULL;
    dch = NULL;
    ecal = NULL;
    gem = NULL;
    silicon = NULL;
    fRootFileName = "";
    fRawFileName = "";
    fDigiFileName = "";
    fDchMapFileName = "";
    fTrigMapFileName = "";
    fTrigINLFileName = "";
    fGemMapFileName = "";
    fECALCalibrationFileName = "";
    fECALMapFileName = "";
    fDat = 0;
    fGemMapper = NULL;
    fDchMapper = NULL;
    fTrigMapper = NULL;
    fECALMapper = NULL;
    fDataQueue = NULL;
    fTimeStart_s = 0;
    fTimeStart_ns = 0;
    syncCounter = 0;
    fPedoCounter = 0;
    fGemMap = NULL;
    fEvForPedestals = N_EV_FOR_PEDESTALS;
    fBmnSetup = kBMNSETUP;
    fT0Map = NULL;
}

BmnRawDataDecoder::BmnRawDataDecoder(TString file, ULong_t nEvents, ULong_t period) {
    runHeaderDAQ = NULL;
    eventHeaderDAQ = NULL;
    runHeader = NULL;
    eventHeader = NULL;
    fTime_ns = 0;
    fTime_s = 0;
    fT0Time = 0.0;
    fRawTree = NULL;
    fDigiTree = NULL;
    fRootFileIn = NULL;
    fRootFileOut = NULL;
    fRawFileIn = NULL;
    fDigiFileOut = NULL;
    sync = NULL;
    tdc = NULL;
    tqdc_tdc = NULL;
    tqdc_adc = NULL;
    adc32 = NULL;
    adc128 = NULL;
    adc = NULL;
    dch = NULL;
    ecal = NULL;
    gem = NULL;
    silicon = NULL;
    fRawFileName = file;
    fEventId = 0;
    fNevents = 0;
    fMaxEvent = nEvents;
    fPeriodId = period;
    fRunId = GetRunIdFromFile(fRawFileName);
    fRootFileName = Form("bmn_run%04d_raw.root", fRunId);
    fDigiFileName = Form("bmn_run%04d_digi.root", fRunId);
    fDchMapFileName = "";
    fTrigMapFileName = "";
    fTrigINLFileName = "";
    fGemMapFileName = "";
    fECALCalibrationFileName = "";
    fECALMapFileName = "";
    fDat = 0;
    fGemMapper = NULL;
    fDchMapper = NULL;
    fTrigMapper = NULL;
    fECALMapper = NULL;
    fDataQueue = NULL;
    fTimeStart_s = 0;
    fTimeStart_ns = 0;
    syncCounter = 0;
    fPedoCounter = 0;
    fGemMap = NULL;
    fEvForPedestals = N_EV_FOR_PEDESTALS;
    fBmnSetup = kBMNSETUP;
    fT0Map = NULL;
    //InitMaps();
}

BmnRawDataDecoder::~BmnRawDataDecoder() {
}

BmnStatus BmnRawDataDecoder::ConvertRawToRoot() {
    if (InitConverter(fRawFileName) == kBMNERROR)
        return kBMNERROR;
    fseeko64(fRawFileIn, 0, SEEK_END);
    fLengthRawFile = ftello64(fRawFileIn);
    rewind(fRawFileIn);
    printf("\nRAW FILE: ");
    printf(  "%s"  , fRawFileName.Data());
    printf("\nRAW FILE LENGTH: ");
    printf(  "%.3f MB\n"  , fLengthRawFile / 1024. / 1024.);
    fRootFileOut = new TFile(fRootFileName, "recreate");

    for (;;) {
        if (fMaxEvent > 0 && fNevents == fMaxEvent) break;
        //if (fread(&dat, kWORDSIZE, 1, fRawFileIn) != 1) return kBMNERROR;
        fread(&fDat, kWORDSIZE, 1, fRawFileIn);
        fCurentPositionRawFile = ftello64(fRawFileIn);
        if (fCurentPositionRawFile >= fLengthRawFile) break;
        if (fDat == kSYNC1) { //search for start of event
            // read number of bytes in event
            if (fread(&fDat, kWORDSIZE, 1, fRawFileIn) != 1) continue;
            fDat = fDat / kNBYTESINWORD + 1; // bytes --> words
            if (kVERBOSE_MODE)
            printf("--- EVENT %d -----------------------------------------\n", fNevents);
            if (fDat >= 100000) { // what the constant?
                printf("Wrong data size: %d:  skip this event\n", fDat);
                fread(data, kWORDSIZE, fDat, fRawFileIn);
            } else {

                //read array of current event data and process them
                if (fread(data, kWORDSIZE, fDat, fRawFileIn) != fDat) continue;
                fEventId = data[0];
                if (fEventId <= 0) continue; // skip bad events
                ProcessEvent(data, fDat);
                fNevents++;
                fRawTree->Fill();
            }
        }
    }

    fRawTree->Branch("RunHeader", &runHeaderDAQ);
    runHeaderDAQ->SetRunId(fRunId);
    TTimeStamp startT = TTimeStamp(time_t(fTimeStart_s), fTimeStart_ns);
    TTimeStamp finishT = TTimeStamp(time_t(fTime_s), fTime_ns);
    fRunStartTime = TDatime(Int_t(startT.GetDate(kFALSE)), Int_t(startT.GetTime(kFALSE)));
    fRunEndTime = TDatime(Int_t(finishT.GetDate(kFALSE)), Int_t(finishT.GetTime(kFALSE)));
    runHeaderDAQ->SetStartTime(fRunStartTime);
    runHeaderDAQ->SetFinishTime(fRunEndTime);
    runHeaderDAQ->SetNEvents(fNevents);
    fRawTree->Fill();

    fCurentPositionRawFile = ftello64(fRawFileIn);
    printf("Read %d events; %lld bytes (%.3f Mb)\n\n", fNevents, fCurentPositionRawFile, fCurentPositionRawFile / 1024. / 1024.);

    fRawTree->Write();
    fRootFileOut->Close();
    fclose(fRawFileIn);

    delete sync;
    delete adc32;
    delete adc128;
    delete adc;
    delete tdc;
    delete tqdc_tdc;
    delete tqdc_adc;

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::InitConverter(deque<UInt_t> *dq) {
    fDataQueue = dq;
    fRawTree = new TTree("BMN_RAW", "BMN_RAW");
    fLengthRawFile = fDataQueue->size();
    InitConverter(fRawFileName);
    return kBMNSUCCESS;

}

BmnStatus BmnRawDataDecoder::InitConverter(TString FileName) {
    printf(  "\n================ CONVERTING ================\n"  );
    fRawFileName = FileName;
    fRawFileIn = fopen(fRawFileName, "rb");
    if (fRawFileIn == NULL) {
        printf("\n!!!!!\ncannot open file %s\nConvertRawToRoot are stopped\n!!!!!\n\n", fRawFileName.Data());
        return kBMNERROR;
    }
    return InitConverter();
}

BmnStatus BmnRawDataDecoder::InitConverter() {

    fRawTree = new TTree("BMN_RAW", "BMN_RAW");
    sync = new TClonesArray("BmnSyncDigit");
    adc32 = new TClonesArray("BmnADCDigit");
    adc128 = new TClonesArray("BmnADCDigit");
    adc = new TClonesArray("BmnADCDigit");
    tdc = new TClonesArray("BmnTDCDigit");
    tqdc_adc = new TClonesArray("BmnTQDCADCDigit");
    tqdc_tdc = new TClonesArray("BmnTDCDigit");
    eventHeaderDAQ = new TClonesArray("BmnEventHeader");
    runHeaderDAQ = new BmnRunHeader();
    fRawTree->Branch("SYNC", &sync);
    fRawTree->Branch("ADC32", &adc32);
    fRawTree->Branch("ADC128", &adc128);
    fRawTree->Branch("ADC", &adc);
    fRawTree->Branch("TDC", &tdc);
    fRawTree->Branch("TQDC_ADC", &tqdc_adc);
    fRawTree->Branch("TQDC_TDC", &tqdc_tdc);
    fRawTree->Branch("EventHeader", &eventHeaderDAQ);
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::wait_stream(deque<UInt_t> *que, Int_t len, UInt_t limit) {
    Int_t t;
    Int_t dt = 10000;
    while (que->size() < len) {
        if (t > limit)
            return kBMNERROR;
        usleep(dt);
        t += dt;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::wait_file(Int_t len, UInt_t limit) {
    Long_t pos = ftello64(fRawFileIn);
    Int_t t = 0;
    Int_t dt = 1000000;
    while (fLengthRawFile < pos + len) {
        //        gSystem->ProcessEvents();
        if (t > limit)
            return kBMNERROR;
        usleep(dt);
        fseeko64(fRawFileIn, 0, SEEK_END);
        fLengthRawFile = ftello64(fRawFileIn);
        fseeko64(fRawFileIn, pos - fLengthRawFile, SEEK_CUR);
        t += dt;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::ProcessEvent(UInt_t *d, UInt_t len) {
    sync->Delete();
    tdc->Delete();
    tqdc_adc->Delete();
    tqdc_tdc->Delete();
    adc32->Delete();
    adc128->Delete();
    adc->Delete();
    eventHeaderDAQ->Delete();
    BmnTrigInfo* trigInfo = new BmnTrigInfo();

    DrawBar(fCurentPositionRawFile, fLengthRawFile);

    Long64_t idx = 1;
    BmnEventType evType = kBMNPAYLOAD;

    while (idx < len) {
        // if (fEventId<501) evType=kBMNPEDESTAL; [CHANGED]
        // else evType = kBMNPAYLOAD;
        evType = kBMNPAYLOAD;

        UInt_t serial = d[idx++];
        UInt_t id = (d[idx] >> 24);

        TString modname;
        switch (serial)
        {
        case 0x611d0c2:
            modname = "Silicon middle";
            break;

        case 0x611e01a:
            modname = "Silicon lower";
            break;

        case 0x80c3b50:
            modname = "Silicon top";
            break;

        case 0x7a959b4:
            modname = "FVME";
            break;

        case 0x76ca7ce:
            modname = "Straw new lower";
            break;

        case 0x46f37ee:
            modname = "Straw new top";
            break;
            
        case 0xa79ef8c:
            modname = "Calorimeter";
            break;

        case 0x80bcccf:
            modname = "Gem top";
            break;

        case 0x611d0da:
            modname = "Gem lower";
            break;

        default:
            modname = "UNCNOWN MODULE";
            break;
        }
        if (kVERBOSE_MODE)
        printf("SERIAL: %x\tID: %x\tMODULE: %s\n", serial, id, modname.Data());

        UInt_t payload = (d[idx++] & 0xFFFFFF) / kNBYTESINWORD;
        if (payload > 2000000) {
            printf("[WARNING] Event %d:\n serial = 0x%06X\n id = Ox%02X\n payload = %d\n", fEventId, serial, id, payload);
            break;
        }
        switch (id) {
            case kADC64VE_XGE:
            case kADC64VE:
            {
                Bool_t isFound = kFALSE;
                for (Int_t iSer = 0; iSer < fNSiliconSerials; ++iSer)
                    if (serial == fSiliconSerials[iSer]) {
                        Process_ADC64VE(&d[idx], payload, serial, 128, adc128);
                        isFound = kTRUE;
                        break;
                    }
                if (isFound) break;
                for (Int_t iSer = 0; iSer < fNGemSerials; ++iSer)
                    if (serial == fGemSerials[iSer]) {
                        Process_ADC64VE(&d[idx], payload, serial, 32, adc32);
                        isFound = kTRUE;
                        break;
                    }
                if (isFound) break;
            }
            case kADC64WR:
            {
                Bool_t isECAL = kFALSE;
                for (Int_t iSer = 0; (iSer < fNECALSerials); ++iSer) {
                    if (serial == fECALSerials[iSer]) {
                        isECAL = kTRUE;
                        break;
                    }
                };
                if (isECAL)
                    Process_ADC64WR(&d[idx], payload, serial, adc);

                break;
            }
            case kFVME:
                Process_FVME(&d[idx], payload, serial, evType, trigInfo);
                break;
        }
        idx += payload;
    }
    //printf("eventHeaderDAQ->GetEntriesFast() %d  eventID %d\n", eventHeaderDAQ->GetEntriesFast(), fEventId);
    new((*eventHeaderDAQ)[eventHeaderDAQ->GetEntriesFast()]) BmnEventHeader(fRunId, fEventId, TDatime(Int_t(TTimeStamp(time_t(fTime_s), fTime_ns).GetDate(kFALSE)), Int_t(TTimeStamp(time_t(fTime_s), fTime_ns).GetTime(kFALSE))), evType, kFALSE, trigInfo);
}

BmnStatus BmnRawDataDecoder::Process_ADC64VE(UInt_t *d, UInt_t len, UInt_t serial, UInt_t nSmpl, TClonesArray *arr) {
    const UChar_t kNCH = 64;
    const UChar_t kNSTAMPS = nSmpl;

    UShort_t valU[kNSTAMPS];
    Short_t valI[kNSTAMPS];
    for (Int_t i = 0; i < kNSTAMPS; ++i) {
        valU[i] = 0;
        valI[i] = 0;
    }

    UInt_t i = 0;
    while (i < len) {
        UInt_t subType = d[i] & 0x3;
        if (subType == 0) {
            i += 5; //skip unused words
            UInt_t iCh = 0;
            while (iCh < kNCH - 1 && i < len) {
                iCh = d[i] >> 24;
                if (iCh > 64) printf("serial = 0x%X     iCh = %d  nSmpl = %d\n", serial, iCh, nSmpl);
                i += 3; // skip two timestamp words (they are empty)
                TClonesArray& ar_adc = *arr;

                if (kTRUE) {
                    TakeDataWordShort(kNSTAMPS, d, i, valI);
                    new(ar_adc[arr->GetEntriesFast()]) BmnADCDigit(serial, iCh, kNSTAMPS, valI);
                } else {
                    TakeDataWordUShort(kNSTAMPS, d, i, valU);
                    new(ar_adc[arr->GetEntriesFast()]) BmnADCDigit(serial, iCh, kNSTAMPS, valU);
                }
                i += (kNSTAMPS / 2); //skip words (we've processed them above)
            }
        } else break;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::TakeDataWordShort(UChar_t n, UInt_t *d, UInt_t i, Short_t* valI) {
    for (Int_t iWord = 0; iWord < n / 2; ++iWord) {
        valI[2 * iWord + 1] = d[i + iWord] & 0xFFFF; //take 16 lower bits and put them into corresponded cell of data-array
        valI[2 * iWord] = (d[i + iWord] >> 16) & 0xFFFF; //take 16 higher bits and put them into corresponded cell of data-array
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::TakeDataWordUShort(UChar_t n, UInt_t *d, UInt_t i, UShort_t* valU) {
    for (Int_t iWord = 0; iWord < n / 2; ++iWord) {
        valU[2 * iWord + 1] = d[i + iWord] & 0xFFFF; //take 16 lower bits and put them into corresponded cell of data-array
        valU[2 * iWord] = (d[i + iWord] >> 16) & 0xFFFF; //take 16 higher bits and put them into corresponded cell of data-array
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::Process_ADC64WR(UInt_t *d, UInt_t len, UInt_t serial, TClonesArray *arr) {
    const UChar_t kNCH = 64;
    const UChar_t kNSTAMPS = 128;

    UShort_t val[kNSTAMPS];
    for (Int_t i = 0; i < kNSTAMPS; ++i) val[i] = 0;

    UInt_t i = 0;
    UInt_t ns = 0;
    while (i < len) {
        UInt_t subType = d[i] & 0x3;
        if (subType == 0) {
            i += 5; //skip unused words
            UInt_t iCh = 0;
            while (iCh < kNCH - 1 && i < len) {
                iCh = d[i] >> 24;
                ns = (d[i] & 0xFFF) / 2 - 4;
                i += 3; // skip two timestamp words (they are empty)
                for (Int_t iWord = 0; iWord < ns / 2; ++iWord) {
                    val[2 * iWord + 1] = d[i + iWord] & 0xFFFF; //take 16 lower bits and put them into corresponded cell of data-array
                    val[2 * iWord] = (d[i + iWord] >> 16) & 0xFFFF; //take 16 higher bits and put them into corresponded cell of data-array
                }

                TClonesArray& ar_adc = *arr;
                if (iCh >= 0 && iCh < kNCH) {
                    if (kVERBOSE_MODE)
                    printf("ns == %d, serial == 0x%0x, chan == %d\n", ns, serial, iCh);
                    new(ar_adc[arr->GetEntriesFast()]) BmnADCDigit(serial, iCh, ns, val);
                }
                i += (ns / 2); //skip words (we've processed them)
            }
        } else break;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::Process_FVME(UInt_t *d, UInt_t len, UInt_t serial, BmnEventType &evType, BmnTrigInfo* spillInfo) {
    UInt_t type = 0;
    UInt_t modId = 0;
    UInt_t slot = 0;
    for (UInt_t i = 0; i < len; i++) {
        type = d[i] >> 28;
        if (kVERBOSE_MODE)
        printf("\t<Process_FVME> New word 0x%d: ",type);
        switch (type) {
            case kEVHEADER:
            case kEVTRAILER:
            case kMODTRAILER:
            case kSPILLHEADER:
            case kSPILLTRAILER:
            case kSTATUS:
            case kPADDING:
                if (kVERBOSE_MODE)
                    printf("\t\tskipped\n");
                break;
            case kMODHEADER:
                modId = (d[i] >> 16) & 0x7F;
                slot = (d[i] >> 23) & 0x1F;
                if (kVERBOSE_MODE)
                    printf("MODHEADER. modid 0x%X slot %d serial 0x%X\n", modId, slot, serial);
                break;
            default: //data
            {
                if (kVERBOSE_MODE)
                    printf("DATA\n");
                switch (modId) {
                    case kTDC64V:
                    case kTDC64VHLE:
                    case kTDC72VHL:
                    case kTDC32VL:
                        FillTDC(d, serial, slot, modId, i);
                        break;
                    case kTQDC16VS:
                        FillTQDC(d, serial, slot, modId, i);
                        break;
                    case kTRIG:
                        FillSYNC(d, serial, i);
                        break;
                    case kU40VE_RC:
                        FillU40VE(d, evType, slot, i, spillInfo);
                        break;
                }
            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillU40VE(UInt_t *d, BmnEventType &evType, UInt_t slot, UInt_t & idx, BmnTrigInfo* trigInfo) {
    if (kVERBOSE_MODE)
    printf("\t\tIt's U40VE\n");
    BmnTriggerType trType;
    UInt_t type = d[idx] >> 28;
    Bool_t countersDone = kFALSE;
    while (type == 2 || type == 3 || type == 4) {
        //printf("type %d  slot %d\n", type, slot);
        if (fPeriodId > 4 && type == kGEMTRIGTYPE && slot == kEVENTTYPESLOT) {
            trType = ((d[idx] & 0x7) == kTRIGMINBIAS) ? kBMNMINBIAS : kBMNBEAM;
            trigInfo->SetTrigType(trType);
            //                            evType = ((d[i] & 0x8) >> 3) ? kBMNPEDESTAL : kBMNPAYLOAD;
            evType = (d[idx] & 0x8) ? kBMNPEDESTAL : kBMNPAYLOAD;
            //printf("evType %d\n", evType);
            if (evType == kBMNPEDESTAL)
                fPedoCounter++;
        }
        if (type == 4 && !countersDone) {
            trigInfo->SetTrigCand(d[idx + 0] & 0x1FFFFFFF);
            trigInfo->SetTrigAccepted(d[idx + 1] & 0x1FFFFFFF);
            trigInfo->SetTrigBefo(d[idx + 2] & 0x1FFFFFFF);
            trigInfo->SetTrigAfter(d[idx + 3] & 0x1FFFFFFF);
            trigInfo->SetTrigRjct(d[idx + 4] & 0x1FFFFFFF);
            idx += 5;
            //                printf("cand %04u, acc %04u, bef %04u, after %04u, rjct %04u\n",
            //                        trigInfo->GetTrigCand(),
            //                        trigInfo->GetTrigAccepted(),
            //                        trigInfo->GetTrigBefo(),
            //                        trigInfo->GetTrigAfter(),
            //                        trigInfo->GetTrigRjct());
            countersDone = kTRUE;
        }
        idx++; //go to the next DATA-word
        type = d[idx] >> 28;
    }
    idx--;
}

BmnStatus BmnRawDataDecoder::FillTDC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t & idx) {
    if (kVERBOSE_MODE)
    printf("\t\tIt's TDC\n");
    UInt_t type = d[idx] >> 28;
    //    printf("fiiltdc\n");
    while (type != kMODTRAILER) { //data will be finished when module trailer appears
        if (type == 4 || type == 5) { // 4 - leading, 5 - trailing
            UInt_t tdcId = (d[idx] >> 24) & 0xF;
            UInt_t time = (modId == kTDC64V) ? (d[idx] & 0x7FFFF) : ((d[idx] & 0x7FFFF) << 2) | ((d[idx] & 0x180000) >> 19);
            UInt_t channel = (modId == kTDC64V) ? (d[idx] >> 19) & 0x1F : (d[idx] >> 21) & 0x7;
            if (modId == kTDC64V && tdcId == 2) channel += 32;
            if (kVERBOSE_MODE)
            cout<<"\t\tID: "<<tdcId<<", channel: "<<dec<<channel<<", time: "<<dec<<time<<endl;
            TClonesArray &ar_tdc = *tdc;
            new(ar_tdc[tdc->GetEntriesFast()]) BmnTDCDigit(serial, modId, slot, (type == 4), channel, tdcId, time);
        }
        idx++; //go to the next DATA-word
        type = d[idx] >> 28;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillTQDC(UInt_t *d, UInt_t serial, UInt_t slot, UInt_t modId, UInt_t & idx) {
    if (kVERBOSE_MODE)
    printf("\t\tIt's TQDC\n");
    UInt_t type = d[idx] >> 28; // good
    UShort_t trigTimestamp = 0;
    UShort_t adcTimestamp = 0;
    UShort_t tdcTimestamp = 0;
    UInt_t iSampl = 0;
    UInt_t channel = 0;
    Short_t valI[ADC_SAMPLING_LIMIT];
    Bool_t inADC = kFALSE;
    if (type == 6) {
        fprintf(stderr, "TQDC Error: %d\n", d[idx++] & 0xF); // @TODO logging
        return kBMNSUCCESS;
    }
    while (type != kMODTRAILER) {
        UInt_t mode = (d[idx] >> 26) & 0x3; // good
        if (!inADC) {
            if (kVERBOSE_MODE)
            printf("\t\tIn TDC\tType: %d, Mode: %d\n", type, mode);
            if ((mode == 0) && (type == 4 || type == 5)) {
                UInt_t rcdata = (d[idx] >> 24) & 0x3;
                channel = (d[idx] >> 19) & 0x1F;
                UInt_t time = 4 * (d[idx] & 0x7FFFF) + rcdata; // in 25 ps
                new((*tqdc_tdc)[tqdc_tdc->GetEntriesFast()]) BmnTDCDigit(serial, modId, slot, (type == 4), channel, 0, time, tdcTimestamp);

            } else if ((type == 4) && (mode == 2)) {
                channel = (d[idx] >> 19) & 0x1F;
                trigTimestamp = d[idx++] & 0xFF;
                adcTimestamp = d[idx] & 0xFF;
                inADC = kTRUE;
                if (kVERBOSE_MODE)
                printf("\t\tADC: channel %d trigTimestamp %d  adcTimestamp %d\n", channel, trigTimestamp, adcTimestamp);
            } else if ((type == 2) && (mode == 0)) {
                UInt_t iEv = (d[idx] >> 12) & 0x1FFF;
                tdcTimestamp = d[idx] & 0xFFF;
                // printf("\t\tTDC ev header: %d\n", iEv);
            } else if ((type == 3) && (mode == 0)) {
                UInt_t iEv = (d[idx] >> 12) & 0x1FFF;
                // printf("\t\tTDC ev trailer: %d\n", iEv);
            }
        } else {
            if (kVERBOSE_MODE)
            printf("\t\tIn ADC\tType: %d, Mode: %d\n", type, mode);
            if ((type == 5) && ((mode == 2) || (mode == 1)) && (iSampl < ADC_SAMPLING_LIMIT)) {
                Short_t val = (d[idx] & ((1 << 14) - 1)) - (1 << (14 - 1));
                valI[iSampl++] = val;
            } else {
                new((*tqdc_adc)[tqdc_adc->GetEntriesFast()]) BmnTQDCADCDigit(serial, channel, slot, iSampl, valI, trigTimestamp, adcTimestamp);
                inADC = kFALSE;
                iSampl = 0;
            }
        }
        type = d[++idx] >> 28;
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillSYNC(UInt_t *d, UInt_t serial, UInt_t & idx) {
    if (kVERBOSE_MODE)
    printf("\t\tIt's SYNC\n");
    UInt_t d0 = d[idx + 0];
    UInt_t d1 = d[idx + 1];
    UInt_t d2 = d[idx + 2];
    UInt_t d3 = d[idx + 3];
    if (kVERBOSE_MODE)
    cout<< "\t\td0, d1, d2, d3 "<<hex<< d0 << " , "<<d1<< " , "<< d2 << " , "<<d3<<endl;
    if ((d0 >> 28) != 2 || (d1 >> 28) != 2 || (d2 >> 28) != 2 || (d3 >> 28) != 2) return kBMNERROR; //check TAI code
    Long64_t ts_t0_s = -1;
    Long64_t ts_t0_ns = -1;
    Long64_t GlobalEvent = -1;

    if (kTRUE) {
        ts_t0_ns = d0 & 0x0FFFFFFF | ((d1 & 0x3) << 28);
        ts_t0_s = ((d1 >> 4) & 0xFFFFFF) | ((d2 & 0xFFFF) << 24);
        GlobalEvent = ((d3 & 0x0FFFFFFF) << 12) | ((d2 >> 16) & 0xFFF);
        if (kVERBOSE_MODE)
        {
            cout<<"\t\tts_t0_ns= "<<ts_t0_ns<<endl;
            cout<<"\t\tts_t0_s= "<<ts_t0_s<<endl;
            cout<<"\t\tGlobalEvent= "<<dec<<GlobalEvent<<endl;
        }
        
    }

    fTime_ns = ts_t0_ns;
    fTime_s = ts_t0_s;

    if (fEventId == 1) {
        fTimeStart_s = ts_t0_s;
        fTimeStart_ns = ts_t0_ns;
    }

    TClonesArray &ar_sync = *sync;
    new(ar_sync[sync->GetEntriesFast()]) BmnSyncDigit(serial, GlobalEvent, ts_t0_s, ts_t0_ns);

    idx += 3; //skip next 3 words (we've processed them)
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::DecodeDataToDigi() {

    printf(  "================= DECODING =================\n"  );

    fRootFileIn = new TFile(fRootFileName, "READ");
    if (fRootFileIn->IsOpen() == false) {
        printf("\n!!!!\ncannot open file %s \nDecodeDataToDigi are stopped\n!!!!\n", fRootFileName.Data());
        return kBMNERROR;
    } else {
        printf("\nINPUT ROOT FILE: ");
        printf(  "%s"  , fRootFileName.Data());
        printf("\nOUTPUT DIGI FILE: ");
        printf(  "%s\n\n"  , fDigiFileName.Data());
    }
    fRawTree = (TTree *) fRootFileIn->Get("BMN_RAW");
    tdc = new TClonesArray("BmnTDCDigit");
    tqdc_adc = new TClonesArray("BmnTQDCADCDigit");
    tqdc_tdc = new TClonesArray("BmnTDCDigit");
    sync = new TClonesArray("BmnSyncDigit");
    adc32 = new TClonesArray("BmnADCDigit");
    adc128 = new TClonesArray("BmnADCDigit");
    adc = new TClonesArray("BmnADCDigit");
    eventHeaderDAQ = new TClonesArray("BmnEventHeader");
    runHeaderDAQ = new BmnRunHeader();
    fRawTree->SetBranchAddress("TDC", &tdc);
    fRawTree->SetBranchAddress("TQDC_ADC", &tqdc_adc);
    fRawTree->SetBranchAddress("TQDC_TDC", &tqdc_tdc);
    fRawTree->SetBranchAddress("SYNC", &sync);
    fRawTree->SetBranchAddress("ADC32", &adc32);
    fRawTree->SetBranchAddress("ADC128", &adc128);
    fRawTree->SetBranchAddress("ADC", &adc);
    fRawTree->SetBranchAddress("EventHeader", &eventHeaderDAQ);
    fRawTree->SetBranchAddress("RunHeader", &runHeaderDAQ);

    fDigiFileOut = new TFile(fDigiFileName, "recreate");
    InitDecoder();
    BmnEventType curEventType = kBMNPAYLOAD;
    BmnEventType prevEventType = curEventType;

    if (fGemMapper || fSiliconMapper) {
        printf("\n[INFO]");
        printf(  " Collecting data for ADC pedestals calculation:\n"  );
        printf("\tNumber of requested pedestal events is ");
        printf(  "%d\n"  , fEvForPedestals);
        for (UInt_t iEv = 0; iEv < fNevents; ++iEv) {
            DrawBar(fPedEvCntr, fEvForPedestals);
            fRawTree->GetEntry(iEv);

            BmnEventHeader* headDAQ = (BmnEventHeader*) eventHeaderDAQ->At(0);
            if (!headDAQ) continue;
            curEventType = headDAQ->GetType();

            if (curEventType != kBMNPEDESTAL) continue; // [CHANGED]
            if (fPedEvCntr != fEvForPedestals - 1) {
                CopyDataToPedMap(adc32, adc128, fPedEvCntr);
                fPedEvCntr++;
            } else break;
        }
        if (fPedEvCntr != fEvForPedestals - 1) {
            printf(  "\n[WARNING]"  );
            printf(" Not enough pedestal events (%d instead of %d)\n", fPedEvCntr, fEvForPedestals);
        }
        if (fGemMapper) fGemMapper->RecalculatePedestals();
        if (fSiliconMapper) fSiliconMapper->RecalculatePedestals();
        fPedEvCntr = 0;

        UInt_t nEvForNoiseCorrection = 10000;
        printf("\n[INFO]");
        printf(  " Clear noisy channels:\n"  );
        printf("\tFilling signal profiles for station-module-layer histograms\n");
        printf("\tNumber of requested events is ");
        printf(  "%d\n"  , nEvForNoiseCorrection);
        printf("\tActual number of events is ");
        printf(  "%d\n"  , fNevents);

        // UInt_t n = Min(fNevents, nEvForNoiseCorrection);
        // for (UInt_t iEv = 0; iEv < n; ++iEv) {
        //     fRawTree->GetEntry(iEv);
        //     if (fGemMapper) fGemMapper->FillProfiles(adc32);
        //     if (fSiliconMapper) fSiliconMapper->FillProfiles(adc128);
        //     DrawBar(iEv, n);
        // }

        printf("\n\tChecking noisy channels\n");
        if (fGemMapper) fGemMapper->FillNoisyChannels();
        if (fSiliconMapper) fSiliconMapper->FillNoisyChannels();
        printf("\tNoisy channels marked\n");
    }

    Int_t nEv = -1;
    Double_t fSize = 0.0;
    UInt_t runId = 0;
    vector<UInt_t> startTripEvent;
    vector<UInt_t> endTripEvent;

    printf("\n[INFO]");
    printf(  " Main loop over events:\n"  );
    for (UInt_t iEv = 0; iEv < fNevents; ++iEv) {
        // if (kVERBOSE_MODE)
        // printf("--- EVENT %d -----------------------------------------\n", iEv);

        DrawBar(iEv, fNevents);
        ClearArrays();

        fRawTree->GetEntry(iEv);
        FillTimeShiftsMap();

        BmnEventHeader* headDAQ = (BmnEventHeader*) eventHeaderDAQ->At(0);
        if (!headDAQ) continue;

        if (iEv == 0) {

            nEv = (Int_t) runHeaderDAQ->GetNEvents();
            fSize = Double_t(fLengthRawFile / 1024. / 1024.);
            runId = runHeaderDAQ->GetRunId();
            fRunStartTime = runHeaderDAQ->GetStartTime();
            fRunEndTime = runHeaderDAQ->GetFinishTime();

        }

        curEventType = headDAQ->GetType();

        Bool_t isTripEvent = kFALSE;
        for (Int_t iTrip = 0; iTrip < startTripEvent.size(); ++iTrip) {
            if (headDAQ->GetEventId() > startTripEvent[iTrip] && headDAQ->GetEventId() < endTripEvent[iTrip]) {
                isTripEvent = kTRUE;
                break;
            }
        }

        if (fTrigMapper) {
            fTrigMapper->FillEvent(tqdc_tdc, tqdc_adc);
            fTrigMapper->FillEvent(tdc);
        }
        fT0Time = 0.;
        GetT0Info(fT0Time, fT0Width);
        new((*eventHeader)[eventHeader->GetEntriesFast()]) BmnEventHeader(headDAQ->GetRunId(), headDAQ->GetEventId(), headDAQ->GetEventTime(), curEventType, isTripEvent, headDAQ->GetTrigInfo(), fTimeShifts);
        BmnEventHeader* evHdr = (BmnEventHeader*) eventHeader->At(eventHeader->GetEntriesFast() - 1);
        evHdr->SetStartSignalInfo(fT0Time, fT0Width);
        if (curEventType == kBMNPEDESTAL) {
            if (fPedEvCntr == fEvForPedestals - 1) continue;
            CopyDataToPedMap(adc32, adc128, fPedEvCntr);
            fPedEvCntr++;
        } 
        else // payload
        { 
            if (prevEventType == kBMNPEDESTAL && fPedEvCntr == fEvForPedestals - 1) 
            {
                if (fGemMapper) fGemMapper->RecalculatePedestals();
                if (fSiliconMapper) fSiliconMapper->RecalculatePedestals();
                fPedEvCntr = 0;
            }
            if (fGemMapper) fGemMapper->FillEvent(adc32, gem);
            if (fSiliconMapper) fSiliconMapper->FillEvent(adc128, silicon);
            if (fDchMapper) fDchMapper->FillEvent(tdc, tqdc_tdc, dch);

        }

        fDigiTree->Fill();
        prevEventType = curEventType;
    }

    printf(  "\n=============== RUN"  );
    printf(  " %04d "  , runId);
    printf(  "SUMMARY ===============\n"  );
    printf("START (event 1):\t%s\n", fRunStartTime.AsSQLString());
    printf("FINISH (event %d):\t%s\n", fNevents, fRunEndTime.AsSQLString());
    printf(  "================================================\n"  );

    fDigiTree->Branch("RunHeader", &runHeader);
    runHeader->SetRunId(runId);
    runHeader->SetStartTime(fRunStartTime);
    runHeader->SetFinishTime(fRunEndTime);
    runHeader->SetNEvents(nEv);

    fDigiTree->Write();
    DisposeDecoder();
    fDigiFileOut->Close();
    fRootFileIn->Close();

    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::InitDecoder() {
    //    fDigiFileOut = new TFile(fDigiFileName, "recreate");
    fDigiTree = new TTree("cbmsim", "bmn_digit");

    eventHeader = new TClonesArray("BmnEventHeader");
    runHeader = new BmnRunHeader();
    fDigiTree->Branch("EventHeader", &eventHeader);
    fNevents = (fMaxEvent > fRawTree->GetEntries() || fMaxEvent == 0) ? fRawTree->GetEntries() : fMaxEvent;

    if (fDetectorSetup[0]) {
        fTrigMapper = new BmnTrigRaw2Digit(fTrigMapFileName, fTrigINLFileName, fDigiTree);
        if (fT0Map == NULL) {
            BmnTrigMapping tm = fTrigMapper->GetT0Map();
            printf("T0 serial 0x%X got from trig mapping\n", tm.serial);
            if (tm.serial > 0) {
                fT0Map = new TriggerMapStructure();
                fT0Map->channel = tm.channel;
                fT0Map->serial = tm.serial;
                fT0Map->slot = tm.slot;
            }
        }
        fTrigMapper->SetSetup(fBmnSetup);
    }
    if (fDetectorSetup[2]) {
        silicon = new TClonesArray("BmnSiliconDigit");
        fDigiTree->Branch("SILICON", &silicon);
        fSiliconMapper = new BmnSiliconRaw2Digit(fPeriodId, fRunId, fSiliconSerials);
    }

    if (fDetectorSetup[3]) {
        gem = new TClonesArray("BmnGemStripDigit");
        fDigiTree->Branch("GEM", &gem);
        fGemMapper = new BmnGemRaw2Digit(fPeriodId, fRunId, fGemSerials, fGemMapFileName);
    }

    if (fDetectorSetup[6]) {
        dch = new TClonesArray("BmnDchDigit");
        fDigiTree->Branch("DCH", &dch);
        fDchMapper = new BmnDchRaw2Digit(fPeriodId, fRunId);
    }

    if (fDetectorSetup[8]) {
        ecal = new TClonesArray("BmnECALDigit");
        fDigiTree->Branch("ECAL", &ecal);
        fECALMapper = new BmnECALRaw2Digit(fECALMapFileName, fRootFileName, fECALCalibrationFileName);
    }

    fPedEvCntr = 0; // counter for pedestal events between two spills
    fPedEnough = kFALSE;
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::ClearArrays() {
    if (dch) dch->Delete();
    if (gem) gem->Delete();
    if (silicon) silicon->Delete();
    if (ecal) ecal->Delete();
    if (fTrigMapper) fTrigMapper->ClearArrays();
    eventHeader->Delete();
    fTimeShifts.clear();
    return kBMNSUCCESS;
}

void BmnRawDataDecoder::ResetDecoder(TString file) {
    fNevents = 0;
    syncCounter = 0;
    fRawFileName = file;
    if (fRawFileIn) {
        fclose(fRawFileIn);
        fRawFileIn = NULL;
    }
    fRunId = GetRunIdFromFile(fRawFileName);
    fRawFileIn = fopen(fRawFileName, "rb");
    if (fRawFileIn == NULL) {
        printf("\n!!!!!\ncannot open file %s\nConvertRawToRoot are stopped\n!!!!!\n\n", fRawFileName.Data());
        return;
    }
    fseeko64(fRawFileIn, 0, SEEK_END);
    fLengthRawFile = ftello64(fRawFileIn);
    rewind(fRawFileIn);
    printf("\nRawData File %s;\nLength RawData - %lld bytes (%.3f Mb)\n", fRawFileName.Data(), fLengthRawFile, fLengthRawFile / 1024. / 1024.);
    fRawTree->Reset();
    fDigiTree->Reset();
}

BmnStatus BmnRawDataDecoder::DisposeDecoder() {
    if (fGemMap) delete[] fGemMap;
    if (fT0Map) delete[] fT0Map;
    if (fGemMapper) delete fGemMapper;
    if (fSiliconMapper) delete fSiliconMapper;
    if (fDchMapper) delete fDchMapper;
    if (fTrigMapper) delete fTrigMapper;
    if (fECALMapper) delete fECALMapper;

    delete sync;
    delete adc32;
    delete adc128;
    delete adc;
    delete tdc;
    delete tqdc_adc;
    delete tqdc_tdc;

    if (gem) delete gem;
    if (dch) delete dch;
    if (silicon) delete silicon;
    if (ecal) delete ecal;

    delete eventHeader;
    delete runHeader;
    if (runHeaderDAQ) delete runHeaderDAQ;
    if (eventHeaderDAQ) delete eventHeaderDAQ;
    if (fRawTree) fRawTree->Delete();
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillTimeShiftsMap() {
    if (fT0Map == NULL) return kBMNERROR;
    Long64_t t0time = 0;
    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*) sync->At(i);
        if (syncDig->GetSerial() == fT0Map->serial) {
            t0time = syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
            break;
        }
    }
    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*) sync->At(i);
        Long64_t syncTime = (t0time == 0.0) ? 0 : syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
        fTimeShifts.insert(pair<UInt_t, Long64_t>(syncDig->GetSerial(), syncTime - t0time));
    }

    return kBMNSUCCESS;
}
BmnStatus BmnRawDataDecoder::CopyDataToPedMap(TClonesArray* adcGem, TClonesArray* adcSil, UInt_t ev) {
    if (fGemMapper) {
        Double_t**** pedData = fGemMapper->GetPedData();
        for (UInt_t iAdc = 0; iAdc < adcGem->GetEntriesFast(); ++iAdc) {
            BmnADCDigit* adcDig = (BmnADCDigit*) adcGem->At(iAdc);

            for (Int_t iSer = 0; iSer < fNGemSerials; ++iSer) {
                if (adcDig->GetSerial() != fGemSerials[iSer]) continue;
                for (UInt_t iSmpl = 0; iSmpl < adcDig->GetNSamples(); ++iSmpl)
                    if (kTRUE) {
                        //                        printf("ser = 0x%x, iSer = %d, ev= %d, ch = %d, iSmpl = %d, sig = %f\n", adcDig->GetSerial(), iSer, ev, adcDig->GetChannel(), iSmpl, (Double_t) (adcDig->GetShortValue())[iSmpl] / 16);
                        pedData[iSer][ev][adcDig->GetChannel()][iSmpl] = (Double_t) (adcDig->GetShortValue())[iSmpl] / 16;
                    } else
                        pedData[iSer][ev][adcDig->GetChannel()][iSmpl] = (Double_t) (adcDig->GetUShortValue())[iSmpl] / 16;
                break;
            }
        }
    }
    if (fSiliconMapper) {
        Double_t**** pedData = fSiliconMapper->GetPedData();
        for (UInt_t iAdc = 0; iAdc < adcSil->GetEntriesFast(); ++iAdc) {
            BmnADCDigit* adcDig = (BmnADCDigit*) adcSil->At(iAdc);

            for (Int_t iSer = 0; iSer < fNSiliconSerials; ++iSer) {
                if (adcDig->GetSerial() != fSiliconSerials[iSer]) continue;
                for (UInt_t iSmpl = 0; iSmpl < adcDig->GetNSamples(); ++iSmpl) {
                    if (kTRUE)
                        pedData[iSer][ev][adcDig->GetChannel()][iSmpl] = (Double_t) (adcDig->GetShortValue())[iSmpl] / 16;
                    else
                        pedData[iSer][ev][adcDig->GetChannel()][iSmpl] = (Double_t) (adcDig->GetUShortValue())[iSmpl] / 16;
                }
                break;
            }
        }
    }
    return kBMNSUCCESS;
}

BmnStatus BmnRawDataDecoder::FillTimeShiftsMapNoDB(UInt_t t0serial) {

    Long64_t t0time = -1;
    //    printf(" sync size %d, t0serial 0x%0x\n", sync->GetEntriesFast(), t0serial);
    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*) sync->At(i);
        //	printf(" have 0x%0x requred 0x%0x\n", syncDig->GetSerial(), t0serial);
        if (syncDig->GetSerial() == t0serial) {
            t0time = syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
            break;
        }
    }

    if (t0time == -1) {
        //        cerr << "No T0 digit found in tree" << endl;
        return kBMNERROR;
    }

    for (Int_t i = 0; i < sync->GetEntriesFast(); ++i) {
        BmnSyncDigit* syncDig = (BmnSyncDigit*) sync->At(i);
        Long64_t syncTime = syncDig->GetTime_ns() + syncDig->GetTime_sec() * 1000000000LL;
        fTimeShifts.insert(pair<UInt_t, Long64_t>(syncDig->GetSerial(), syncTime - t0time));
    }

    return kBMNSUCCESS;
}

Int_t BmnRawDataDecoder::GetRunIdFromFile(TString name) {
    Int_t runId = -1;
    FILE * file = fopen(name.Data(), "rb");
    if (file == NULL) {
        printf("File %s is not open!!!\n", name.Data());
        return -1;
    }
    UInt_t word;
    while (fread(&word, kWORDSIZE, 1, file)) {
        if (word == kRUNNUMBERSYNC) {
            fread(&word, kWORDSIZE, 1, file); //skip word
            fread(&runId, kWORDSIZE, 1, file);
            return runId;
        }
    }
    fclose(file);
    if (runId <= 0) {
        Int_t run = 0;
        //sscanf(&(((char *)name.Data())[strlen(name.Data())-9]), "%d", &run);
        run = ((TString) name(name.Length() - 9, name.Length() - 5)).Atoi();
        return run;
    } else return runId;
}

BmnStatus BmnRawDataDecoder::InitMaps() {

    string dummy;
    UInt_t ser = 0;
    set<UInt_t> seials;
    TString name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fGemMapFileName;

    seials.clear();
    name = TString(getenv("VMCWORKDIR")) + TString("/input/") + fSiliconMapFileName;
    ifstream inFileSil(name.Data());
    if (!inFileSil.is_open())
        cout << "Error opening map-file (" << name << ")!" << endl;
    for (Int_t i = 0; i < 4; ++i) getline(inFileSil, dummy); //comment line in input file

    while (!inFileSil.eof()) {
        inFileSil >> std::hex >> ser >> std::dec >> dummy >> dummy >> dummy >> dummy >> dummy >> dummy;
        if (!inFileSil.good()) break;
        seials.insert(ser);

    }
    for (auto s : seials) fSiliconSerials.push_back(s);
    fNSiliconSerials = fSiliconSerials.size();
}

BmnStatus BmnRawDataDecoder::GetT0Info(Double_t& t0time, Double_t &t0width) {
    vector<TClonesArray*>* trigArr = fTrigMapper->GetTrigArrays();
    BmnTrigDigit* dig = 0;
    for (auto ar : *trigArr) {
        if (fPeriodId > 6) {
            if (strcmp(ar->GetName(), "BC2")) continue;
        } else {
            if (strcmp(ar->GetName(), "T0")) continue;
        }
        for (int i = 0; i < ar->GetEntriesFast(); i++) {
            dig = (BmnTrigDigit*) ar->At(i);
            if (fPeriodId > 6) {
                if (dig->GetMod() == 0) {
                    t0time = dig->GetTime();
                    t0width = dig->GetAmp();
                    //		printf(" t0 %f t0w %f n %d\n", t0time, t0width, ar->GetEntriesFast());
                    return kBMNSUCCESS;
                }
            } else {
                t0time = dig->GetTime();
                t0width = dig->GetAmp();
                return kBMNSUCCESS;
            }
        }
    }
    return kBMNERROR;
}
