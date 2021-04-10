int arin2(int ntube = 32)
{
  const Int_t ndu=600;
  Int_t i;
  Double_t idu1[ndu],k[ndu];
  for (i=0;i<ndu;i++) {
    k[i]=200+i;
    //   cout<< "k[" <<i << "]="<< k[i] << '\n';
  }

      ifstream outRiFile("tdc.dat",ios::in);
   if (!outRiFile ) {
     cerr<<"File no open"<<endl;
     exit(1);
   }

   Char_t ftube[30]={""};
   Char_t ftitle[30]={"R(TDC), Ntube="};
   sprintf(ftube,"%d\n",ntube);
   strcat(ftitle,ftube);

   Int_t n1=0;
// 
  while(!outRiFile.eof())
     { 
       outRiFile >> idu1[n1];
       cout << "idu1[]="<< idu1[n1]<< '\n';
	  n1++;
     }
    TGraph *gr = new TGraph(ndu,k,idu1);
    TCanvas *c1 =new TCanvas("c1","Kalibr",200,20,700,800);
   gr->SetLineColor(98);
   gr->SetFillColor(33);
   gr->SetLineWidth(8);
   gr->GetYaxis()->SetTitle(ftitle);
   gr->GetXaxis()->SetTitle("TDC,u1");
  gr->Draw("AL*");

      outRiFile.close();

      cout << "n1= " << n1 << endl;
   return 0;
}
