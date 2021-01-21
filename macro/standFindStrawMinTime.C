// find minimum time in each TDC channel

void standFindStrawMinTime(TString inFileName)
{
   ofstream outFile;
   outFile.open("../input/TDCMinTime.txt");
   outFile
       << "Channel\tMinTime\n"
       << "================\n";
   //TFile *inFile=new TFile(inFileName);
   for (size_t iPlane = 0; iPlane < 2; iPlane++)
   {
      outFile << iPlane << endl;
      for (Int_t iCh = 0; iCh < 64; iCh++)
      {
         TFile *inFile = new TFile(inFileName);
         TTree *tree = (TTree *)inFile->Get("cbmsim");
         // TString condition = "DCH.fWire==" + TString::Itoa(iCh, 10);
         TString condition = "DCH.fWire==" + TString::Itoa(iCh, 10) + "&&" + "DCH.fPlane==" + TString::Itoa(iPlane, 10);
         tree->Draw(">>+elist", condition);
         TEventList *eventList = (TEventList *)gDirectory->Get("elist");
         tree->SetEventList(eventList);
         tree->Draw("DCH.fTime", "", "goff");
         auto minTime = tree->GetMinimum("DCH.fTime");
         auto maxTime = tree->GetMaximum("DCH.fTime");
         outFile << iCh << "\t" << minTime << endl;
         cout << "Min time for ch" << iCh << ": " << minTime << " Max time: " << maxTime << endl;
         tree = 0;
         eventList = 0;
         inFile->Close();
         inFile = 0;
         delete tree, eventList, inFile;
      }
   }
      outFile.close();
}
