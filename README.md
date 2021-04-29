# MiniSPD
Minispd is a sofware dedicated to MiniSPD stand data reconstruction.

The project was tested on fresh installed Linux Mint 20.1 with ROOT 6.24.

# Installation

   1. Install packages from [ROOT Dependencies](https://root.cern/install/dependencies/);
   2. Install CERN ROOT package using [this guide](https://root.cern/install/#build-from-source), don't forget to set ROOTSYS environment variable;
   3. Create build directory;
   4. Generate makefile using _cmake <./path/to/sourceDir/>_;
   5. compile minispd using _make_ command;
   6. Run _. config.sh_;
   7. Try using testPipeline.sh script in macro directory.

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
