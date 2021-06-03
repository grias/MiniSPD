//------------------------------------------------------------------
//
// TMillepede.h - C++ TMillepede class definition.
//
// Implementation of alignment method proposed by V.Blobel
//
// 1999     V.Blobel     idea and FORTRAN code
// ?        M.Braeuer    C++ interface
// 2001-02  I.Belotelov  C++ code
// 07/2002  I.Belotelov  Common interface
// 
//
//------------------------------------------------------------------
#ifndef _T_MILLEPEDE_H_
#define _T_MILLEPEDE_H_
class TMillepede {
  int      f_nLocal             ; // Number of local  parameters
  int      f_nGlobal            ; // Number of global paramters
  int      f_nCurrentConstraint ; // Number of current filled constraint
  int      f_nConstraint        ; // Number of constraint
  double * f_pdC                ; // C' with constraints
  double * f_pdB                ; // Global solution
  double * f_pdCCopy            ; // Copy of C'
  double * f_pdBCopy            ; // Copy of B
  double * f_pdBeta             ; // Local  solution
  double * f_pdGamma            ; // (nLocal)x(nLocal) part of C
  double * f_pdG                ; // (nLocal)x(nGlobal) part of C
  float  * f_pfFixed            ; // Vector to fix some parameters 
  float  * f_pfConstraint       ; // (nConstraint)x(nGlobal) vector 
  ;                               //    of constraints (all of them) 
  double * f_pdTemp             ; // temp vector used to calc AWAT
  double * f_pdCTemp            ; // temp matrix C
  double * f_pdBTemp            ; // temp vector B
  double * f_pdResid            ; // solution residual
  int    * f_piNonZero          ; // index vector of planes with hits 
  int      f_bResid             ; // flag for residual calculation
  int      f_bPrint             ; // flag for prinout
  float  * f_pfLocDer           ; // vector of local  derivatives
  float  * f_pfGlbDer           ; // vector of global derivatives
  int    Invert(double*,double*,int);
  void   AX   (double* A, double* X, double* Y,int* NZ,int n, int m);
  void   AVAT (double* V, double* A, double* W,int* NZ,int n, int m);
  void   SYMAX(double* A, double* X,double* Y,int m);
  TMillepede(){};

 public: 
  TMillepede(int nGlobal,int nLocal,int nConstraint);
  ~TMillepede();
  // main stuff
  void    StoreMeasurement(float fMesurement, float fSigma);
  int     FitLocal();
  int     FitGlobal();
  int     GlobalInvert();
  
  // getters
  float * GetLocDerPointer(){return f_pfLocDer;};
  float * GetGlbDerPointer(){return f_pfGlbDer;};

  double  GetGlobal(int i){return f_pdB[i];};
  double  GetLocal(int i) {return f_pdBeta[i];};   
  double  GetCPrime(int i, int j);
  double* GetCPrime(){return f_pdC;};
  double  GetGlobalCopy(int i){return f_pdBCopy[i];};
  double  GetCPrimeCopy(int i, int j);
  double  GetResid(int i){return f_pdResid[i];};
  // setters
  void    DefineParSigma(int i, float v=0.0) {f_pfFixed[i]= v;};
  void    DefineConstraint(float* fValue, float rhs);
  void    ApplyConstraint();
  int     ApplyFix();
  void    SetCPrime(int i, int j, double v);
  void    ComputeResid(int f=1){f_bResid=f;};
  // resetters
  void    Reset();
  void    ScaleCPrime();
};
#endif //_T_MILLEPEDE_H_





