#!/bin/bash

BINDIR=/home/gribowski/soft/spdroot/build/bin

# runsArray=(780 802 803 811 816 817 818 829 831 832 838 840 842 843 844 848 851) # old
# runsArray=(585 586 587 589 591 593 693 709 716 721 744 758 849 857 859 861 868 869 871 916 957 966 967) # new
runsArray=(780 802 803 811 816 817 818 829 831 832 838 840 842 843 844 848 851 693 709 716 721 744 758 849 857 859 861 868 869 871 916 957 966 967) # ald + new - newNoisy
# runsArray=(816)

makespd

for run in ${runsArray[@]}; do
  $BINDIR/rawToDigits $run
  # root -b -q "standDataToRoot.C(\"data/mpd_run_Straw_stand_$run.data\")"
done
