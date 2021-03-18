#!/bin/bash

# runsArray=(780 802 803 811 816 817 818 829 831 832 838 840 842 843 844 848 851)
runsArray=(780)

makespd 

for run in ${runsArray[@]}; do
  root -b -q "standHitsToTracks.C($run, -1)"
done