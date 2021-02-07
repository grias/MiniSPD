#!/bin/bash

run=816

SOURCEDIR=/home/gribowski/soft/spdroot/spdroot
BUILDDIR=$SOURCEDIR/../build

rm -rf $BUILDDIR/*
cd $BUILDDIR 
cmake $SOURCEDIR &&
make -j11 &&
. config.sh &&
cd $SOURCEDIR/macro
root -b -q "standDataToRoot.C(\"mpd_run_Straw_stand_$run.data\")"
root -b -q "standDigitsToHits.C($run, -1)"
root -b -q "SiliconHitsAnalysisHist.C($run, 0)"
cd $SOURCEDIR/macro