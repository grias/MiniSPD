# MiniSPD stand
THIS PROJECT WAS A PART OF SPDROOT ONCE. OCCASIONALLY YOU CAN FIND ITS REMAINS HERE AND THERE

# SPD software

This repository contains software project devoted to the [SPD](http://spd.jinr.ru/doku.php)
 experiment at the [NICA](http://nica.jinr.ru) collider. 

# Installation
Refer to the spdroot git repo [installation guide](https://git.jinr.ru/nica/spdroot/-/wikis/Installation-Guide).

# Running

Root macros are located in macro/ directory:
   1. standDataToRoot.C -- decode .data file into digits and save it as a .root file
   2. standDigitsToHits.C -- create hits from digits
   3. standHitsToTracks.C -- create tracks from hits
   4. analysisStrawDigits.C -- read straw digits and build some histograms (saved at macro/pictures)
   5. analysisSiliconDigits.C -- read silicon digits and build some histograms (saved at macro/pictures)
   6. analysisSiliconHits.C -- read silicon hits and build some histograms (saved at macro/pictures)
   7. analysisSiliconTracks.C -- read silicon tracks and build some histograms (saved at macro/pictures)
   8. analysisAlignment.C -- read silicon tracks and build some histograms related to alignment process (saved at macro/pictures)
   9. createMockDigits.C -- create mock digits from genereted straight tracks
   10. standMergeDigits.C -- merge selected root-files with digits into one


# Documentation

There is no documentation on MiniSPD software yet.

# Contacts

The SPD Software Coordinator: Alexey Zhemchugov [zhemchugov@jinr.ru]

This MiniSPD software developer: Alexander Gribowski [grias1994@gmail.com]
