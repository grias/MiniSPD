
//_______________________________________________________________________________
void DisplayStandEvent(Int_t nevent = 0)
{
  TString  InputFile  = "run_stand.root";
  TString  ParFile    = "params_stand.root";
  TString  OutFile    = "draw_stand.root";
  
  if (gSystem->AccessPathName(InputFile)) {
      cout << "No input data-file: " <<  InputFile << endl;
      return;
  }
  
  if (gSystem->AccessPathName(ParFile)) {
      cout << "No input pars-file: " <<  ParFile << endl;
      return;
  }
 
  SpdRunAna* Run = new SpdRunAna();
  
  Run->SetInputFile(InputFile.Data());
  Run->SetOutputFile(OutFile.Data());

  FairRuntimeDb* rtdb = Run->GetRuntimeDb();
  FairParRootFileIo* parInput1 = new FairParRootFileIo();
  parInput1->open(ParFile.Data());
  rtdb->setFirstInput(parInput1);
  
  SpdEventManager* Man = new SpdEventManager();
  
  SpdMCTracks* Tracks = new SpdMCTracks("GeoTracks");
  
  Man->AddTask(Tracks);
  Man->Init();
  
  Man->GotoEvent(nevent);
}
