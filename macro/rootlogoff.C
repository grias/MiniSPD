
void rootlogoff() 
{
  //cout << "::rootlogoff::" << endl;
  if (gROOT) {
     if (gROOT->GetVersionInt() > 61000) gROOT->~TROOT();
  }
}