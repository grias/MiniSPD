#!/bin/bash


makespd
root -b -q "standDigitsToHits.C(780)"
root -b -q "SiliconHitsAnalysisHist.C(780, 1)"
