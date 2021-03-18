int main()
{
    // Имя входного файла
    TString inputFileName = "data/stand_run0816_digits.root";
    // printf(TString("Reading file: ") + inputFileName + TString("\n"));
    printf("Reading file: %s\n", inputFileName.Data());

    // Открываем входной .root файл
    TFile *inputFile = new TFile(inputFileName);
    if (!inputFile) return 1; // закончить если файл не найден

    // Открываем дерево
    TTree *inputTree = (TTree *)inputFile->Get("cbmsim");

    // Находим ветку с сигналами straw
    TBranch *branch = inputTree->GetBranch("DCH");
    if (!branch) return 1; // закончить если ветка не найдена
    branch->SetAutoDelete(kTRUE); // НЕ ОБЯЗАТЕЛЬНО удаление прочитанных событий, чтобы не занимали память

    // Привязываем ветку к переменной
    TClonesArray *StrawDigits = 0;
    inputTree->SetBranchAddress("DCH", &StrawDigits);

    // Находим количество событий в файле
    Int_t nEvents = inputTree->GetEntries();
    printf("Number of events in file: %d\n", nEvents);

    // Цикл по всем событиям
    for (Int_t iEv = 0; iEv < nEvents; iEv++)
    {
        // Вывод номера события
        printf("\nEvent %d\n", iEv);

        // Считываем данные о событии номер iEv
        inputTree->GetEntry(iEv);

        // Находим количество сработавших трубок
        Int_t nDigits = StrawDigits->GetEntriesFast();
        printf("Number of straws triggered in event: %d\n", nDigits);

        // Цикл по всем сработавшим трубкам
        for (Int_t iDigit = 0; iDigit < nDigits; iDigit++)
        {
            // Считываем сработавшую трубку
            BmnDchDigit *strawDigit = (BmnDchDigit *)StrawDigits->At(iDigit);

            // Номер станции сработавшей трубки (0 или 1)
            Int_t plane = strawDigit->GetPlaneInt();

            // Номер сработавшей трубки (от 0 до 63)
            Int_t wire = strawDigit->GetWireNumber();

            // время с TDC
            Double_t time = strawDigit->GetTime();

            printf("%d) Plane: %d, Wire: %d, GetTime: %f\n", iDigit, plane, wire, time);
        }
    }

    return 0;
}