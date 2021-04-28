#!/bin/bash

run=2

root -q -b "createMockDigits.C($run)"
root -b -q "standDigitsToHits.C($run, -1)"
root -b -q "standHitsToTracks.C($run, -1)"
root -b -q "analysisSiliconDigits.C($run)"
root -b -q "analysisSiliconHits.C($run)"
root -b -q "analysisSiliconTracks.C($run)"
root -q -b "analysisAlignment.C($run)"