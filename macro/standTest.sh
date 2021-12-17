#!/bin/bash

# runsArray=(995 998 1002 1004 1024 1029 1030 1035 1038 1042 1044 1048 1050 1054 1062 1079 1090 1093 1097 1098 1100 1102 1104 1106 1107) # new 27.09.21
# runsArray=(5)
runsArray=(816)


SOURCEDIR=/home/gribowski/soft/spdroot/spdroot
BUILDDIR=$SOURCEDIR/../build

# rm -rf $BUILDDIR/*
# cd $BUILDDIR 
# cmake $SOURCEDIR &&
# make -j11 &&
# . config.sh &&
# cd $SOURCEDIR/macro

makespd &&

for run in ${runsArray[@]}; do
    # root -q -b "createMockDigits.C($run)"
    $BUILDDIR/bin/rawToDigits $run
    $BUILDDIR/bin/digitsToHits $run
    $BUILDDIR/bin/hitsToTracks $run
    # root -b -q "analysisStrawDigits.C($run)"
    # root -b -q "analysisGemDigits.C($run)"
    # root -b -q "analysisGemHits.C($run)"
    # root -b -q "analysisSiliconDigits.C($run)"
    # root -b -q "analysisSiliconHits.C($run)"
    # root -b -q "analysisSiliconTracks.C($run)"
    # root -b -q "analysisTracks.C($run)"
    # root -b -q "createRT.C($run)"
done

cd $SOURCEDIR/macro