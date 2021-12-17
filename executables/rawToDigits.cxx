#include "BmnEnums.h"
#include "BmnRawDataDecoder.h"

#include <cstdlib>
#include <memory>

int main(int argc, char const *argv[])
{
    int run = argc >= 2 ? atoi(argv[1]) : 816;
    int nEvents = argc >= 3 ? atoi(argv[2]) : 0;

    TString file = Form("mpd_run_Straw_stand_%d.data",run);
    
    bool doConvert = true;
    
    auto decoder = make_shared<BmnRawDataDecoder>(file, nEvents);

    decoder->AddDetector(kTRIGGER);
    decoder->AddDetector(kSILICON);
    decoder->AddDetector(kGEM);
    decoder->AddDetector(kSTRAW);
    // decoder->AddDetector(kCALORIMETER);

    decoder->SetSiliconMapping("SILICON_map_run7.txt");
    decoder->SetTrigMapping("Trig_map_Run7.txt");
    // decoder->SetDchMapping("Straw_map_Run780.txt");
    decoder->SetDchMapping("Straw_map_Run916.txt");
    decoder->SetGemMapping("GEM_map_run.txt");

    decoder->InitMaps();
    if (doConvert) decoder->ConvertRawToRoot(); // Convert raw data in .data format into adc-,tdc-, ..., sync-digits in .root format
    decoder->DecodeDataToDigi(); // Decode data into detector-digits using current mappings.

    return 0;
}
