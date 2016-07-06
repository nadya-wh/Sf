/////////////////////////////////////////////////////////////////
// Ver.2.0    26.12.2011   Russian == English
/////////////////////////////////////////////////////////////////

#ifndef _DNF
#define _DNF

#ifdef _LINUX
#undef _DEBUG
typedef unsigned char BYTE; 
typedef unsigned long ULONG; 
typedef int BOOL;

#define TRUE 1
#define FALSE 0

#define ASSERT assert
#include <assert.h>
#ifndef FO_
#define FO_
#define FSTD(_x) friend _x
#define STD(_x) _x
#endif


#else
#include <afxwin.h>         // MFC core and standard components
#ifndef FO_
#define FO_
#define FSTD(_x) friend _x AFXAPI 
#define STD(_x) _x AFXAPI 
#endif

#endif

#include <vector>
#include <utility>
#include "../Common/BaseBool.h"
#include "../Common/BaseTern.h"

typedef std::vector<CTM> MTM;
typedef std::pair <CTV, int> PairTv_I;
typedef std::vector <PairTv_I> MTv_I;
typedef std::vector <MTv_I> MMTv_I;

class CDNF   : public CTM
{              
 public:

 //****************** Constructors/Destructor ***********************************
 
 CDNF(); 
 CDNF(const CDNF& dnf);
 CDNF(CTM tm);
 CDNF(const char* pch);
 CDNF(int nRow,int nColumn,char symb='-');
 ~CDNF();
 
 //**************** Operations with DNFs ****************************************
   
 bool Sum(CTV& tv);  
 int  SumCon(CTV tv);  
 bool Sum_check(CTV& tv);  
 void Sum(CDNF& dnf);
 int Sum(int end,int first,int last);
 bool Sum_cons(CTV& tv, int* curr);
 
 void Product(CTV& tv); 
 void Product(CDNF& dnf); 
 
 void ProductD(CTV& ed);
 CDNF Invertion(void);
 void ProductD_ext(CTV& ed);
 CDNF Invertion_ext(void);

 void Conc(void);
 void Check(int nRow);
 bool EqualTo(CDNF* dnf);
 CDNF Cofactor(int nVar, bool bBranch);
};

#endif