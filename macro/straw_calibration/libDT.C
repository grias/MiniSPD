#include <iostream>
#include <fstream>
#include <iomanip>
int libDT(int ntube)
{
  /////////////////////////////////
  //
  //drawing of tdc histogram
  //fulling of matrix
  //
  ////////////////////////////////

  const Int_t m = 1200; // total interval of  tdc
  Int_t tdc[100000];
  Int_t in = 0; // all values of tdc
  Int_t i;
  //
  Char_t ftube[3] = {""};
  Char_t ftitle[30] = {"TDC, Ntube="};
  Char_t fname[17] = {"Ntube"};

  sprintf(ftube, "%d", ntube);
  strcat(ftitle, ftube);
  strcat(fname, ftube);
  strcat(fname, ".dat");
  //
  cout << "  fname  " << fname << endl;
  //
  TH1F *Tdc_u1 = new TH1F("TDC_u1", ftitle, m, 0, m); // u1
  ifstream outTdcFile(fname, ios::in);
  if (!outTdcFile)
  {
    cerr << "File no open " << endl;
    exit(1);
  }
  while (!outTdcFile.eof())
  {
    outTdcFile >> tdc[in];
    Tdc_u1->Fill(tdc[in]);
    in++;
  }
  cout << "\n in= " << in << endl;
  //
  outTdcFile.close();
  //
  TCanvas *c1 = new TCanvas("c1", "c1", 800, 1000);
  //
  Tdc_u1->SetLineColor(98);
  Tdc_u1->SetFillColor(7);
  //   g1 = new TF1("g1","gaus",210,270);
  //  g1->SetLineColor(Color_t color=62);
  //  Tdc_u1->Fit(g1,"R+");
  Tdc_u1->GetYaxis()->SetTitle("N");
  Tdc_u1->GetXaxis()->SetTitle("       TDC     ");

  Tdc_u1->Draw();

  gStyle->SetOptStat(111);

  return 0;
}
