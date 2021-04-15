#!/bin/bash

# runsArray=(780 802 803 811 816 817 818 829 831 832 838 840 842 843 844 848 851) # old
runsArray=(585 586 587 589 591 593 693 709 716 721 744 758 849 857 859 861 868 869 871 916 957 966 967) # new

makespd

for run in ${runsArray[@]}; do
  root -b -q "analysisSiliconDigits.C($run)"
done