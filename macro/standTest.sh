#!/bin/bash

run=3

SOURCEDIR=/home/gribowski/soft/spdroot/spdroot
BUILDDIR=$SOURCEDIR/../build

# rm -rf $BUILDDIR/*
# cd $BUILDDIR 
# cmake $SOURCEDIR &&
# make -j11 &&
# . config.sh &&
# cd $SOURCEDIR/macro

makespd &&

# root -q -b "createMockDigits.C($run)"
# $BUILDDIR/bin/rawToDigits $run
$BUILDDIR/bin/digitsToHits $run
$BUILDDIR/bin/hitsToTracks $run
# root -b -q "analysisStrawDigits.C($run)"
# root -b -q "analysisGemDigits.C($run)"
# root -b -q "analysisGemHits.C($run)"
# root -b -q "analysisSiliconDigits.C($run)"
# root -b -q "analysisSiliconHits.C($run)"
root -b -q "analysisSiliconTracks.C($run)"
# root -b -q "analysisTracks.C($run)"
# root -q -b "createRT.C($run)"

cd $SOURCEDIR/macro