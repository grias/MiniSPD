#include "../bmndata/BmnEnums.h"

//file: raw-file name in macro/data/
//nEvents: if 0 then decode all events
//doConvert: convert RAW --> ROOT before decoding or use file converted before

void standDataToRoot(TString file, Long_t nEvents = 0, Bool_t doConvert = kFALSE) {

    auto decoder = make_shared<BmnRawDataDecoder>(file, nEvents);

    decoder->AddDetector(kTRIGGER);
    decoder->AddDetector(kSILICON);
    decoder->AddDetector(kGEM);
    decoder->AddDetector(kSTRAW);
    // decoder->AddDetector(kCALORIMETER);

    decoder->SetSiliconMapping("SILICON_map_run7.txt");
    decoder->SetTrigMapping("Trig_map_Run7.txt");
    decoder->SetDchMapping("Straw_map_Run780.txt");
    decoder->SetGemMapping("GEM_map_run.txt");

    decoder->InitMaps();
    if (doConvert) decoder->ConvertRawToRoot(); // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format
    decoder->DecodeDataToDigi(); // Decode data into detector-digits using current mappings.
}
