#include "../bmndata/BmnEnums.h"

//file: full path to raw-file
//nEvents: if 0 then decode all events
//doConvert: convert RAW --> ROOT before decoding or use file converted before

void standDataToRoot(TString file, Long_t nEvents = 0, Bool_t doConvert = kTRUE) {
    UInt_t period = 7;
    BmnSetup stp = kBMNSETUP; // use kSRCSETUP for Short-Range Correlation program and kBMNSETUP otherwise
    BmnRawDataDecoder* decoder = new BmnRawDataDecoder(file, nEvents, period);
    decoder->SetBmnSetup(stp);

    Bool_t setup[10]; //array of flags to determine BM@N setup
    //Just put "0" to exclude detector from decoding
    setup[0] = 1; // TRIGGERS
    setup[1] = 0; // MWPC
    setup[2] = 1; // SILICON
    setup[3] = 0; // GEM
    setup[4] = 0; // TOF-400
    setup[5] = 0; // TOF-700
    setup[6] = 1; // DCH
    setup[7] = 0; // ZDC
    setup[8] = 0; // ECAL
    setup[9] = 0; // LAND
    decoder->SetDetectorSetup(setup);

    decoder->SetSiliconMapping("SILICON_map_run7.txt");
    decoder->SetTrigMapping("Trig_map_Run7.txt");
    decoder->SetDchMapping("Straw_map_Run584.txt");
    // decoder->SetDchMapping("Straw_map_Run780.txt");

    decoder->InitMaps();
    if (doConvert) decoder->ConvertRawToRoot(); // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format
    decoder->DecodeDataToDigi(); // Decode data into detector-digits using current mappings.

    delete decoder;
}
