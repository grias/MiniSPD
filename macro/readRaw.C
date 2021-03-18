#include <stdio.h>

const Bool_t kVERBOSE_MODE = kTRUE;

const size_t kWORDSIZE = sizeof (UInt_t);
const UInt_t kSYNC1 = 0x2A502A50;
const Short_t kNBYTESINWORD = 4;

void readRaw(TString inputFileName)
{

    ULong_t breakAtEvent = 228;
    UInt_t maxWordsInEvent = 100000;
    Bool_t isWrite = kTRUE;

    UInt_t metaData;
    UInt_t eventData[1000000];
    // UInt_t *eventData = new UInt_t(maxWordsInEvent*100); // POSSIBLE ISSUE WITH DYNAMIC ARRAY SIZE
    UInt_t iEvent = 0;
    Long64_t inputFileCurrentPosition;
    UInt_t eventId;

    FILE *inputFile = fopen(inputFileName, "rb");
    FILE *outputFile;
    if (isWrite)
    {
        TString outputFileName = inputFileName + TString("_cut.data");
        outputFile = fopen(outputFileName, "wb");
    }

    if (inputFile == NULL) {
        printf("-E- Cannot open file %s\n", inputFileName.Data());
        return;
    }

    fseeko64(inputFile, 0, SEEK_END);
    Long64_t inputFileLength = ftello64(inputFile);
    rewind(inputFile);

    printf("Input file: %s\n", inputFileName.Data());
    printf("File length: %.3f MB\n", inputFileLength / 1024. / 1024.);

    for (;;)
    {
        if (breakAtEvent > 0 && iEvent == breakAtEvent) break;

        fread(&metaData, kWORDSIZE, 1, inputFile);
        if (outputFile)
        fwrite(&metaData, kWORDSIZE, 1, outputFile);
        inputFileCurrentPosition = ftello64(inputFile);

        if (inputFileCurrentPosition >= inputFileLength) break;

        if (metaData == kSYNC1) //search for start of event
        {
            // read number of bytes in event
            if (fread(&metaData, kWORDSIZE, 1, inputFile) != 1) continue;
            if (outputFile)
            fwrite(&metaData, kWORDSIZE, 1, outputFile);

            metaData = metaData / kNBYTESINWORD + 1; // bytes --> words
            if (kVERBOSE_MODE)
                printf("-I- Event %d\n", iEvent);
            if (metaData >= maxWordsInEvent)
            {
                printf("\t-E- Wrong data size: %d:  skip this event\n", metaData);
                fread(eventData, kWORDSIZE, metaData, inputFile);
            }
            else
            {
                //read array of current event data
                if (fread(eventData, kWORDSIZE, metaData, inputFile) != metaData) continue;
                if (outputFile)
                fwrite(eventData, kWORDSIZE, metaData, outputFile);
                eventId = eventData[0];
                // printf("\t-I- Event ID: %d\n", eventId);
                if (eventId <= 0) continue; // skip bad events
                // ProcessEvent(eventData, metaData);
                iEvent++;
            }
        }
    }
    inputFileCurrentPosition = ftello64(inputFile);
    printf("-I- Read %d events; %lld bytes (%.3f Mb)\n\n", iEvent, inputFileCurrentPosition, inputFileCurrentPosition / 1024. / 1024.);

    fclose(inputFile);
    if (outputFile)
    fclose(outputFile);
    printf("End.");
}