
#define LOAD_MACROPATH
#define LOAD_SPDLIBRARIES
//#define LOAD_SPDINCLUDES

void SetMacroPath();
void LoadLibraries();
void LoadIncludes();
void SetStyle();
 
void rootlogon() 
{
  //cout << "::rootlogon::" << endl;
 
  if (!gROOT) return;
  
#ifdef LOAD_MACROPATH
  SetMacroPath();
#endif
  
#ifdef LOAD_SPDLIBRARIES
  LoadLibraries();
#endif  
  
#ifdef LOAD_SPDINCLUDES
  LoadIncludes();
#endif    

  SetStyle();
  
}

//______________________________________________________________
void SetMacroPath()
{
  //cout << ":SetMacroPath:" << endl;
 
  TString PROJECT = gSystem->Getenv("VMCWORKDIR");
  if (PROJECT == "") PROJECT = ".";

  //cout << "Project install directory: " << PROJECT <<endl;

  TString macropath;   
 
  //macropath = gROOT->GetMacroPath();   
  //cout << "macropath: " << macropath << endl;
 
  TString mpath = gSystem->ConcatFileName(PROJECT,"macro");
  macropath = ".:" + mpath + ":";
  
  gROOT->SetMacroPath(macropath);
  
  // add macro subdirectories
  // macropath += mpath + "/geom" + ":";
  // macropath += mpath + "/analysis" + ":";
  // macropath += mpath + "/tracking" + ":";
  //macropath += mpath + "/analysis/its" + ":";
  //macropath += mpath + "/analysis/tst" + ":";
  
  //cout << "macropath: " << macropath << endl;
  
  gROOT->SetMacroPath(macropath);
 
  //return;
  
  // print macro path
  int id(0),idx(0);  
  cout << "path to ROOT macros: " << endl;
  while ((idx = macropath.Index(":",id)) != -1) {
         cout << macropath(id,idx-id) << endl;
         id = idx+1;
  }  
}

//______________________________________________________________
void LoadLibraries()
{
  //cout << ":LoadLibraries:" << endl;
  
  cout << "load libraries ... ";

  gSystem->Load("libDecoder");
  gSystem->Load("libBmnData");
  gSystem->Load("libStandHitsProducer");

  //gSystem->ListLibraries();
    
  cout << "Ok " << endl;     
}

//______________________________________________________________
void LoadIncludes()
{
  //cout << ":LoadIncludes:" << endl;

  cout << "load includes  ... ";

  gROOT->ProcessLine(".include $SIMPATH/include/");
  
  gROOT->ProcessLine(".include ../bmndata/"); 
  gROOT->ProcessLine(".include ../decoder"); 
  gROOT->ProcessLine(".include ../hitproducer"); 
  
  cout << " Ok " << endl;
}  

void SetStyle()
{
  gStyle->SetCanvasColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetFrameFillColor(0);
  gStyle->SetFrameBorderMode(0);

  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);

  gStyle->SetTextFont(62);
  gStyle->SetTitleFont(62, "XYZ");
  gStyle->SetTitleFont(62, "A");
  gStyle->SetLabelFont(62, "XYZ");
  gStyle->SetLegendFont(132); // Times
  gStyle->SetStatFont(62);

  gStyle->SetLabelSize(0.03, "XYZ");
  gStyle->SetTitleSize(0.03, "XYZ");
  gStyle->SetTitleXOffset(0.98);
  gStyle->SetTitleYOffset(0.98);

  gStyle->SetTitleStyle(1001);
  gStyle->SetTitleBorderSize(2);
  gStyle->SetTitleFillColor(19);
  gStyle->SetTitleAlign(13);
  gStyle->SetTitleX(0.2);

  gStyle->SetStatColor(19);

  gStyle->SetPadBottomMargin(0.14);
  gStyle->SetPadTopMargin(0.04);
  gStyle->SetPadLeftMargin(0.14);
  gStyle->SetPadRightMargin(0.04);
  gStyle->SetCanvasDefH(600);
  gStyle->SetCanvasDefW(800);

  gStyle->SetOptStat(10);

  gROOT->ForceStyle();
}
  
  
  




