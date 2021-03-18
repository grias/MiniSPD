#!/bin/bash

run=1

SOURCEDIR=/home/gribowski/soft/spdroot/spdroot
BUILDDIR=$SOURCEDIR/../build

# rm -rf $BUILDDIR/*
# cd $BUILDDIR 
# cmake $SOURCEDIR &&
# make -j11 &&
# . config.sh &&
# cd $SOURCEDIR/macro
makespd &&

# root -b -q "standDataToRoot.C(\"data/mpd_run_Straw_stand_$run.data\")"
# root -b -q "SiliconDigitsAnalysisHist.C($run)"
# root -b -q "standDigitsToHits.C($run, -1)"
# root -b -q "SiliconHitsAnalysisHist.C($run, 1)"
root -b -q "standHitsToTracks.C($run, -1)"
root -b -q "standTracksToHisto.C($run, 1)"
root -q -b "standAlignment.C($run)"

cd $SOURCEDIR/macro