
void standMergeDigits()
{
    // vector<Int_t> runsToMerge = {780, 802, 803, 811, 816, 817, 818, 829, 831, 832, 838, 840, 842, 843, 844, 848, 851}; // old
    // vector<Int_t> runsToMerge = {585, 586, 587, 589, 591, 593, 693, 709, 716, 721, 744, 758, 849, 857, 859, 861, 868, 869, 871, 916, 957, 966, 967}; // new
    // vector<Int_t> runsToMerge = {693, 709, 716, 721, 744, 758, 849, 857, 859, 861, 868, 869, 871, 916, 957, 966, 967}; // new without noisy events

    vector<Int_t> runsToMerge = {780, 802, 803, 811, 816, 817, 818, 829, 831, 832, 838, 840, 842, 843, 844, 848, 851, 693, 709, 716, 721, 744, 758, 849, 857, 859, 861, 868, 869, 871, 916, 957, 966, 967}; // mix

    

    auto ioman = new StandIOManager();
    ioman->SetOutputFileName("data/stand_run0001_digits.root");
    ioman->RegisterOutputBranch("SILICON", "BmnSiliconDigit");
    ioman->RegisterOutputBranch("DCH", "BmnDchDigit");
    ioman->InitOutput();

    auto outSiDigits = ioman->GetOutputDataArray("BmnSiliconDigit");
    auto outDchDigits = ioman->GetOutputDataArray("BmnDchDigit"); 

    auto inChain = new TChain("cbmsim");
    for (auto &&runId : runsToMerge)
        inChain->Add(Form("data/stand_run%04d_digits.root", runId));

    auto branchSi = inChain->GetBranch("SILICON");
    if (!branchSi) return;
    branchSi->SetAutoDelete(kTRUE);
    TClonesArray *inSiDigits = 0;
    inChain->SetBranchAddress("SILICON", &inSiDigits);
    BmnSiliconDigit *inSiDigit;

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
        ioman->ClearArrays();
        inChain->GetEntry(iEv);

        // Si digits
        for (size_t iDigit = 0; iDigit < inSiDigits->GetEntriesFast(); iDigit++)
        {
            inSiDigit = (BmnSiliconDigit *)inSiDigits->At(iDigit);
            if (!inSiDigit->IsGoodDigit()) continue;
            new((*outSiDigits)[outSiDigits->GetEntriesFast()]) BmnSiliconDigit(inSiDigit);
        }
        // printf("Si Digits: %lld\n", outSiDigits->GetEntriesFast());

        // DCH digits
        for (size_t iDigit = 0; iDigit < inDchDigits->GetEntriesFast(); iDigit++)
        {
            inDchDigit = (BmnDchDigit *)inDchDigits->At(iDigit);
            new((*outDchDigits)[outDchDigits->GetEntriesFast()]) BmnDchDigit(inDchDigit);
        }
        // printf("Dch Digits: %lld\n", outDchDigits->GetEntriesFast());

        ioman->FillEvent();
    } // end of event

    ioman->WriteTreeIntoOutputFile();
    ioman->FinishOutput();
}
