#include "TMillepede.h"
#include <math.h>
#include <iostream>
#include <iomanip>

using namespace std;
double max(double a,double b){ if(a>b)return a; else return b;}; 

////////////////////////////////////////////////////////////////
//----------- TMillepede initialisation constructor ----------//
////////////////////////////////////////////////////////////////

TMillepede::TMillepede(int nGlobal,int nLocal,int nConstraint)
  :f_nLocal(nLocal),f_nGlobal(nGlobal),f_nConstraint(nConstraint),
   f_nCurrentConstraint(0), f_bResid(0){
  int nGlobConstr = f_nGlobal+f_nConstraint;
  
  f_pdC=new double[nGlobConstr*(nGlobConstr+1)/2];
  for(int i=0;i<nGlobConstr*(nGlobConstr+1)/2;i++)
    f_pdC[i]=0.0; 

  f_pdB=new double[nGlobConstr];
  for(int i=0;i<nGlobConstr;i++)
    f_pdB[i]=0.0;
  
  f_pdResid = new double[nGlobConstr];
  for(int i=0;i<nGlobConstr;i++)
    f_pdResid[i]=0.0;


  f_pdCCopy=new double[nGlobConstr*(nGlobConstr+1)/2];
  for(int i=0;i<nGlobConstr*(nGlobConstr+1)/2;i++)
    f_pdCCopy[i]=0.0; 

  f_pdBCopy=new double[nGlobConstr];
  for(int i=0;i<nGlobConstr;i++)
    f_pdBCopy[i]=0.0;

  f_pdGamma=new double[f_nLocal*(f_nLocal+1)/2]; 
  for(int i=0;i<f_nLocal*(f_nLocal+1)/2;i++)
    f_pdGamma[i]=0.0;

  f_pdBeta =new double[f_nLocal]; 
  for(int i=0;i<f_nLocal;i++)
    f_pdBeta[i]=0.0;

  f_pdG=new double[f_nLocal*f_nGlobal];
  for(int i=0;i<f_nLocal*f_nGlobal;i++)
    f_pdG[i]=0.0;
  
  f_pfFixed=new float [f_nGlobal];
  for(int i=0;i<f_nGlobal;i++)
    f_pfFixed[i]=-1.0;

  f_pfConstraint= new float [f_nGlobal*f_nConstraint];
  for(int i=0;i<f_nGlobal*f_nConstraint;i++)  
    f_pfConstraint[i]=1.0;

  f_pdCTemp = new double[f_nGlobal*(f_nGlobal+1)/2];
  f_pdBTemp = new double[f_nGlobal];
  
  f_piNonZero = new int[f_nGlobal];
  for(int i=0;i<f_nGlobal;i++)
    f_piNonZero[i]=0;
  
  f_pfLocDer   = new float[f_nLocal];
  for(int i=0;i<f_nLocal;i++)
    f_pfLocDer[i] =0.0;

  f_pfGlbDer   = new float[f_nGlobal];
  for(int i=0;i<f_nGlobal;i++)
    f_pfGlbDer[i] =0.0;
}

////////////////////////////////////////////////////////////////
//------------- TMillepede destructor ------------------------//
////////////////////////////////////////////////////////////////
TMillepede::~TMillepede(){ 
  delete[] f_pdC;
  delete[] f_pdB;
  delete[] f_pdCCopy;
  delete[] f_pdBCopy;
  delete[] f_pdGamma;
  delete[] f_pdBeta; 
  delete[] f_pdG;
  delete[] f_pfFixed; 
  delete[] f_pdTemp;
  delete[] f_pfLocDer;
  delete[] f_pfGlbDer;
  delete[] f_piNonZero; 
  delete[] f_pfConstraint;
  delete[] f_pdCTemp;
  delete[] f_pdBTemp;
};

////////////////////////////////////////////////////////////////
//------------- TMillepede::SetConstraint---------------------//
////////////////////////////////////////////////////////////////
void TMillepede::DefineConstraint(float *fValue, float rhs){ 
  if (f_nCurrentConstraint==f_nConstraint){
    cout<<"Error: too many constraints"<<endl;
    return;
  }
  for(int i=0;i<f_nGlobal;i++)
    f_pfConstraint[f_nGlobal*f_nCurrentConstraint+i]=fValue[i];
  f_pdB[f_nGlobal+f_nCurrentConstraint]=rhs;
  f_nCurrentConstraint++;
};

////////////////////////////////////////////////////////////////
//------------- TMillepede::ApplyConstraint---------------------//
////////////////////////////////////////////////////////////////
void TMillepede::ApplyConstraint(){
  for(int i=0; i<f_nConstraint;i++){
    for(int j=0; j<f_nGlobal;j++){
      if(fabs(f_pfFixed[j])>1e-7)
	SetCPrime(f_nGlobal+i,j,f_pfConstraint[i*f_nGlobal+j]);
      else
	SetCPrime(f_nGlobal+i,j,0);
    }
  }
}

////////////////////////////////////////////////////////////////
//------------- TMillepede:: StoreMeasurement ----------------//
////////////////////////////////////////////////////////////////
void TMillepede::StoreMeasurement(float fMeasurement, float fSigma){
  //double w;
  float w; 
  w = 1.0/pow(fSigma,2);
  for (int i=0;i<f_nGlobal;i++){
    if(fabs(f_pfGlbDer[i])>1e-10){
      //  f_pdC[i*(i+3)/2] += pow(f_pfGlbDer[i],2)*w;
      f_pdB[i]         += f_pfGlbDer[i]*fMeasurement*w;
      for (int j=0;j<f_nLocal;j++)
	f_pdG[i*f_nLocal+j] += f_pfGlbDer[i]*f_pfLocDer[j]*w;
      f_piNonZero[i]=1;
    }
  }

  for (int i=0;i<f_nGlobal;i++)
    if(fabs(f_pfGlbDer[i])>1e-10)
      for (int j=i;j<f_nGlobal;j++)
	if(fabs(f_pfGlbDer[j])>1e-10)
	  {
	    //	    cout<<i<<"  "<<j<<"   "<<(i+1)*i/2+j<<endl;
	    if (i>j)f_pdC[(i+1)*i/2+j] += f_pfGlbDer[i]*f_pfGlbDer[j]*w;
	    else    f_pdC[(j+1)*j/2+i] += f_pfGlbDer[i]*f_pfGlbDer[j]*w;
	  }

  for (int i=0;i<f_nLocal;i++){     //column
    for (int j=0;j<=i;j++){         //raw
      f_pdGamma[i*(i+1)/2+j]+=f_pfLocDer[i]*f_pfLocDer[j]*w;
    }
    f_pdBeta[i]+=f_pfLocDer[i]*fMeasurement*w;
  }
  for(int i=0;i<f_nLocal;i++)                 f_pfLocDer[i] = 0.0;            
  for(int i=0;i<f_nGlobal;i++)                f_pfGlbDer[i] = 0.0;         
}

////////////////////////////////////////////////////////////////
//------------- TMillepede::FitLocal -------------------------//
////////////////////////////////////////////////////////////////
int TMillepede::FitLocal(){
  int rank;
  int m=0;
  double *diag = new double[f_nLocal];
  double *flag = new double[f_nLocal];
  rank = Invert(f_pdGamma,f_pdBeta,f_nLocal);
  AVAT(f_pdGamma,f_pdG,f_pdCTemp,f_piNonZero,f_nLocal,f_nGlobal);
  AX  (f_pdG, f_pdBeta,f_pdBTemp,f_piNonZero,f_nGlobal,f_nLocal); 
  for(int j=0;j<f_nGlobal*(f_nGlobal+1)/2;j++)
    f_pdC[j]-=f_pdCTemp[j];
    
  for(int j=0;j<f_nGlobal;j++)
    f_pdB[j]-=f_pdBTemp[j];
 
  for(int i=0;i<f_nLocal*(f_nGlobal);i++)         f_pdG[i] = 0.0;
  for(int i=0;i<f_nGlobal;i++)                f_piNonZero[i] = 0;
  for(int i=0;i<f_nLocal*(f_nLocal+1)/2;i++)   f_pdGamma[i] = 0.0;
  for(int i=0;i<f_nLocal;i++)                   f_pdBeta[i] = 0.0;
 
  delete[] diag;
  delete[] flag;

  return rank;  
}


////////////////////////////////////////////////////////////////
//------------- TMillepede::ApplyFix--------------------------//
////////////////////////////////////////////////////////////////
int TMillepede::ApplyFix(){
  int nFixed=0;
  for(int i=0;i<f_nGlobal;i++){
    if (fabs(f_pfFixed[i])<1e-10){
      nFixed++;
      for(int j=0;j<f_nGlobal;j++)
	SetCPrime(i,j,0.0);
    }
    else if (f_pfFixed[i]>0.0)
      SetCPrime(i,i,GetCPrime(i,i)+1.0/f_pfFixed[i]);
  }
  return nFixed;
}

////////////////////////////////////////////////////////////////
//------------- TMillepede::FitGlobal ------------------------//
////////////////////////////////////////////////////////////////
int TMillepede::FitGlobal(){
  int rank;
  ApplyFix();  
  ApplyConstraint();
  rank = GlobalInvert();
  return rank;
}

////////////////////////////////////////////////////////////////
//------------- TMillepede::GlobalInvert  --------------------//
////////////////////////////////////////////////////////////////
int TMillepede::GlobalInvert(){
  int size=f_nGlobal+f_nConstraint;
  int rank; 
  if(f_bResid) for(int i=0; i<size;i++)f_pdBCopy[i]=f_pdB[i];
  rank = Invert(f_pdC,f_pdB,size);
  if(f_bResid) {
    SYMAX(f_pdC,f_pdBCopy,f_pdResid,size); 
    for(int i=0; i<size;i++)f_pdResid[i]-=f_pdB[i];
  }
  return rank;
}

////////////////////////////////////////////////////////////////
//------------- TMillepede::Reset ----------------------------//
////////////////////////////////////////////////////////////////
void TMillepede::Reset(){
  int nGlobConstr = f_nGlobal+f_nConstraint;
  for(int i=0;i<nGlobConstr*(nGlobConstr+1)/2;i++)f_pdC     [i] = 0.0;
  for(int i=0;i<nGlobConstr;i++)                  f_pdB     [i] = 0.0;
  for(int i=0;i<f_nLocal*nGlobConstr;i++)         f_pdG     [i] = 0.0;
  for(int i=0;i<f_nLocal*(f_nLocal+1)/2;i++)      f_pdGamma [i] = 0.0;
  for(int i=0;i<f_nLocal;i++)                     f_pdBeta  [i] = 0.0;
  for(int i=0;i<f_nLocal;i++)                     f_pfLocDer[i] = 0.0;
  for(int i=0;i<f_nGlobal;i++)                    f_pfGlbDer[i] = 0.0;
}


double TMillepede::GetCPrime(int i, int j){
  if (i<=j) return  f_pdC[j*(j+1)/2+i];
  else      return  f_pdC[i*(i+1)/2+j];
}

void  TMillepede::SetCPrime(int i, int j, double v){
  if (i<=j)  f_pdC[j*(j+1)/2+i]=v;
  else       f_pdC[i*(i+1)/2+j]=v;
}

double TMillepede::GetCPrimeCopy(int i, int j){
  if (i<=j) return  f_pdCCopy[j*(j+1)/2+i];
  else      return  f_pdCCopy[i*(i+1)/2+j];
}

void TMillepede::AVAT (double* V, double* A, double* W,int* NZ,int n, int m ){
  double c;
  int il=-n, ijs=0,i,l,j,k,lkl,lk,jk,js,ij;
  for ( i=0; i<(m+1)*m/2;i++)
    W[i]=0.0;
  for ( i=0; i<m; i++){
    ijs+=i-1;
    il+=n;
    lkl=-1;
    if(!NZ[i])continue;
    for ( k=0; k<n; k++){
      c=0.0;
      lkl+=k;
      lk=lkl;
      for ( l=0; l<k+1; l++){
	lk++;
	c+=A[il+l]*V[lk];
      };
      for ( l=k+1; l<n; l++){
	lk+=l;
	c+=A[il+l]*V[lk];
      };
      jk=k;
      ij=ijs+i;
      for ( j=0; j<i+1; j++){
	ij++;
	W[ij]+=c*A[jk];
	jk+=n;
      }
    }
  }
}
void TMillepede::AX(double* A, double* X,double* Y,int* NZ,int m, int n){
  int i,j,ij=-1;
  for( i=0; i<m; i++){
    Y[i]=0.0; 
    for ( j=0; j<n; j++){
      ij++;
      Y[i]+=A[ij]*X[j];
    }
  }
}
void TMillepede::SYMAX(double* A, double* X,double* Y,int m){
  int l,j,k,lkl,lk,jk,js,ij;
  lkl=-1;
  for (k=0; k<m; k++){
    Y[k]=0.0;
    lkl+=k;
    lk=lkl;
    //cout<<"k="<<k<<" lk="<<lk<<endl;
    for ( l=0; l<k+1; l++){
      lk++;
      Y[k]+=A[lk]*X[l];
      //cout<<"  +1 lk="<<lk<<"  l="<<l<<endl;
    };
    for ( l=k+1; l<m; l++){
      lk+=l;
      Y[k]+=A[lk]*X[l];
      //cout<<"  +2 lk="<<lk<<"  l="<<l<<endl;
    }
  }
}
////////////////////////////////////////////////////////////////
//------------- TMillepede::Invert ---------------------------//
////////////////////////////////////////////////////////////////
int TMillepede::Invert(double*V, double* B,int N){
  const double  eps=1.0e-10;
  double *diag ,VKK,VJK;
  int I,J,K,L,JJ,KK,JK,JL,LK,*flag,rank;
  flag=new int[N];
  diag=new double[N];
  for(int i=0;i<N;i++){ flag[i]=1; diag[i] = fabs(V[i*(i+3)/2]);}
  rank=0;
  for(I=0;I<N;I++){
    K=-1;
    JJ=-1;
    KK=0;
    VKK=0.0;
    for(J=0;J<N;J++){
      JJ=JJ+J+1;
      if(flag[J])                 // not used so far
	if ((fabs(V[JJ])>max(fabs(VKK),eps*diag[J]))){
	  VKK=V[JJ];            // pivot candidat
	  K  =J ;               // index of pivot
	  KK =JJ;               // index of diagonal elemant
	} 	
    }
    if(K!=-1){
      rank++;                     // increase rank
      flag[K]=0;                  // reset flag
      VKK = 1.0/VKK;
      V[KK]=-VKK;
      B[K]*=VKK;
      JK=KK-K-1;
      JL=-1;
      for (J=0;J<N;J++)           // elimination
	if(J==K){
	  JK=KK;
	  JL+=J+1;
	}
	else{
	  if(J<K)  JK+=1;
	  else     JK+=J;
	  VJK=V[JK];
	  V[JK]=VKK*VJK;
	  B[J]-=B[K]*VJK;
	  LK=KK-K-1;
	  for (L=0;L<J+1;L++){
	    JL+=1;
	    if(L==K)
	      LK=KK;
	    else {
	      if (L<K)  LK+=1;
	      else	  LK+=L;  
	      V[JL]-=V[LK]*VJK;
	    };
	  }
	}
    }
    else {               //pivot not found
      for (K=0;K<N;K++)
	if(flag[K]) {
	  B[K]=0.0;
	  for(J=0;J<=K;J++) if(flag[J]) V[K*(K+1)/2+J]=0.0;
	}
      
      break;
    };
  }
  for(int i=0;i<(N*N+N)/2;i++)V[i]=-V[i];
  delete [] flag;
  delete [] diag;
  return rank;
}
void TMillepede::ScaleCPrime(){
  double rowNorm;
  double minNorm=10e20;
  int    minRow=-1;
  double constrNorm;
  double scalingFactor;
  cout.setf(ios::fixed);
  for(int i=0; i<f_nGlobal;i++){
    rowNorm=0.;
    for(int j=0; j<f_nGlobal;j++){
      rowNorm+=pow(GetCPrime(i,j),2);
    }
    if(minNorm>rowNorm){
      minNorm = rowNorm;
      minRow  = i;
    }
  }
  minNorm=sqrt(minNorm);
  cout<<"Row No. "<<minRow<<" has minimal norm: "<<minNorm<<endl;
  for(int i=0; i<f_nConstraint;i++){
    constrNorm=0.;
    for(int j=0; j<f_nGlobal;j++)
      constrNorm+=pow(f_pfConstraint[f_nGlobal*i+j],2);
    constrNorm=sqrt(constrNorm);
    scalingFactor=minNorm/constrNorm;

    cout<<"Constraint "<<i<<" norm: ";
    cout<<setw(10)<<setprecision(3)<<constrNorm;
    cout<<"; scaling factor: ";
    cout<<setw(10)<<setprecision(3)<<scalingFactor;
    cout<<endl;
    for(int j=0; j<f_nGlobal;j++)
      f_pfConstraint[f_nGlobal*i+j]*= scalingFactor;
  }
}
