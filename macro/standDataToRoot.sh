#!/bin/bash

BINDIR=/home/gribowski/soft/spdroot/build/bin

# runsArray=(780 802 803 811 816 817 818 829 831 832 838 840 842 843 844 848 851) # old
# runsArray=(585 586 587 589 591 593 693 709 716 721 744 758 849 857 859 861 868 869 871 916 957 966 967) # new
# runsArray=(780 802 803 811 816 817 818 829 831 832 838 840 842 843 844 848 851 693 709 716 721 744 758 849 857 859 861 868 869 871 916 957 966 967) # ald + new - newNoisy
# runsArray=(816)
runsArray=(995 998 1002 1004 1024 1029 1030 1035 1038 1042 1044 1048 1050 1054 1062 1079 1090 1093 1097 1098 1100 1102 1104 1106 1107) # new 27.09.21


makespd

for run in ${runsArray[@]}; do
  $BINDIR/rawToDigits $run
  # root -b -q "standDataToRoot.C(\"data/mpd_run_Straw_stand_$run.data\")"
done
