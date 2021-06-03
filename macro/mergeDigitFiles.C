
void mergeDigitFiles(int outRunId)
{
    // vector<Int_t> runsToMerge = {780, 802, 803, 811, 816, 817, 818, 829, 831, 832, 838, 840, 842, 843, 844, 848, 851}; // old
    // vector<Int_t> runsToMerge = {585, 586, 587, 589, 591, 593, 693, 709, 716, 721, 744, 758, 849, 857, 859, 861, 868, 869, 871, 916, 957, 966, 967}; // new
    // vector<Int_t> runsToMerge = {693, 709, 716, 721, 744, 758, 849, 857, 859, 861, 868, 869, 871, 916, 957, 966, 967}; // new without noisy events

    vector<Int_t> runsToMerge = {780, 802, 803, 811, 816, 817, 818, 829, 831, 832, 838, 840, 842, 843, 844, 848, 851, 693, 709, 716, 721, 744, 758, 849, 857, 859, 861, 868, 869, 871, 916, 957, 966, 967}; // old + new - newNoisy

    auto ioman = new StandIOManager();
    TString outFileName = Form("data/stand_run%04d_digits.root", outRunId);
    ioman->SetOutputFileName(outFileName);
    ioman->RegisterOutputBranch("SILICON", "BmnSiliconDigit");
    ioman->RegisterOutputBranch("GEM", "BmnGemStripDigit");
    ioman->RegisterOutputBranch("DCH", "BmnDchDigit");
    ioman->InitOutput();

    auto outSiDigits = ioman->GetOutputDataArray("SILICON");
    auto outGemDigits = ioman->GetOutputDataArray("GEM");
    auto outDchDigits = ioman->GetOutputDataArray("DCH"); 

    auto inChain = new TChain("cbmsim");
    for (auto &&runId : runsToMerge)
        inChain->Add(Form("data/stand_run%04d_digits.root", runId));

    auto branchSi = inChain->GetBranch("SILICON");
    if (!branchSi) return;
    branchSi->SetAutoDelete(kTRUE);
    TClonesArray *inSiDigits = 0;
    inChain->SetBranchAddress("SILICON", &inSiDigits);
    BmnSiliconDigit *inSiDigit;

    auto branchGem = inChain->GetBranch("GEM");
    if (!branchGem) return;
    branchGem->SetAutoDelete(kTRUE);
    TClonesArray *inGemDigits = 0;
    inChain->SetBranchAddress("GEM", &inGemDigits);
    BmnGemStripDigit *inGemDigit;

    auto branchDch = inChain->GetBranch("DCH");
    if (!branchDch) return;
    branchDch->SetAutoDelete(kTRUE);
    TClonesArray *inDchDigits = 0;
    inChain->SetBranchAddress("DCH", &inDchDigits);
    BmnDchDigit *inDchDigit = 0;

    Long64_t nEvents = inChain->GetEntries();
    printf("NEvents: %lld\n", nEvents);
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        // printf("\nEvent %d\n", iEv);
        inChain->GetEntry(iEv);

        // Si digits
        for (size_t iDigit = 0; iDigit < inSiDigits->GetEntriesFast(); iDigit++)
        {
            inSiDigit = (BmnSiliconDigit *)inSiDigits->At(iDigit);
            if (!inSiDigit->IsGoodDigit()) continue;
            new((*outSiDigits)[outSiDigits->GetEntriesFast()]) BmnSiliconDigit(inSiDigit);
        }

        // Gem digits
        for (size_t iDigit = 0; iDigit < inGemDigits->GetEntriesFast(); iDigit++)
        {
            inGemDigit = (BmnGemStripDigit *)inGemDigits->At(iDigit);
            if (!inGemDigit->IsGoodDigit()) continue;
            new((*outGemDigits)[outGemDigits->GetEntriesFast()]) BmnGemStripDigit(inGemDigit);
        }

        // Straw digits
        for (size_t iDigit = 0; iDigit < inDchDigits->GetEntriesFast(); iDigit++)
        {
            inDchDigit = (BmnDchDigit *)inDchDigits->At(iDigit);
            new((*outDchDigits)[outDchDigits->GetEntriesFast()]) BmnDchDigit(inDchDigit);
        }

        ioman->EndEvent();
    } // end of event

    ioman->FinishOutput();
}
