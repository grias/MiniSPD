#include "BmnMath.h"

#include <iostream>
#include <cmath>

using namespace std;

void DrawBar(UInt_t iEv, UInt_t nEv) {
    cout.flush();
    Float_t progress = iEv * 1.0 / nEv;
    Int_t barWidth = 70;

    Int_t pos = barWidth * progress;
    for (Int_t i = 0; i < barWidth; ++i) {
        if (i <= pos) printf( " " );
        else printf( " " );
    }

    printf( "[%d%%]\r" , Int_t(progress * 100.0 + 0.5));
    cout.flush();
}

void DrawBar(Long64_t iEv, Long64_t nEv) {
    cout.flush();
    Float_t progress = iEv * 1.0 / nEv;
    Int_t barWidth = 70;

    Int_t pos = barWidth * progress;
    for (Int_t i = 0; i < barWidth; ++i) {
        if (i <= pos) printf( " " );
        else printf( " " );
    }

    printf( "[%d%%]\r" , Int_t(progress * 100.0 + 0.5));
    cout.flush();
}
