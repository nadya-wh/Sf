/////////////////////////////////////////////////////////////////////
// Ver.2.1.1    23.04.2007
//              Поправка в функции Sbf_toSop
//              Добавлена функция RemoveAll()
// Ver.2.1.0    07.07.2006
//              Переход к LINUX

/////////////////////////////////////////////////////////////////////
// Ver.2.0.7    28.04.2006
// Ver.2.0.6    18.04.2006
// Ver.2.0.5    05.04.2006
// Ver.2.0.4    25.03.2006
// Ver.2.0.3    24.03.2006
// Ver.2.0.2    21.03.2006
// Ver.2.0.1    19.03.2006
// Ver.2.0.0    17.03.2006

// Ver.1.1.1    14.02.2005   Russian == English
/////////////////////////////////////////////////////////////////////
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifdef _JOINT
#include "BaseBool.h"
#include "BaseTern.h"
#include "Sop.h"
#include "archive.h"

#else
#include "../Common/BaseBool.h"
#include "../Common/BaseTern.h"
#include "../Common/Sop.h"
#include "../Com_Lib/archive.h"
#endif

//#ifdef _LINUX
#undef _DEBUG
typedef unsigned char BYTE; 
typedef unsigned long ULONG; 
typedef int BOOL;

#define ASSERT assert
//#else
//#include "afxwin.h"
//#endif

class CBV;
class CTV;
class CTM;
class CSOP;

/////////////////////////////////////////////////////////////////////
// CSBF view
#ifndef _SBF
#define _SBF

class CSBF
{
public:

//***************** Сonstructors/Destructors ************************
  CSBF();
  CSBF(const CSBF& s);
  CSBF(const CTM tm, const CTM bm);
  CSBF(int nArg, int nFun, int nCon);
  ~CSBF();

//***************** Access to parameters ****************************
  int GetnCon() const;
  int GetnFun() const;
  int GetnArg() const;

//***************** Writing the data ********************************
  const CSBF& operator =(const CSBF& MySBF);

  int AddCon(const CTV ConRow,const CTV FunRow);
  void RemoveAll();
  void RemoveR(int nRow,int Count=1);

//***************** Generation **************************************
  CSBF GenSbfMid (int nCon, int nArg,int nFun, int ranArg, int ranFun);
  CSBF GenSbfMid_11 (int nCon, int nArg,int nFun, int ranArg, int ranFun);
  CSBF GenSbfFix (int nCon, int nArg,int nFun, int ranArg, int ranFun);

//***************** Check of equivalation****************************
  int NoEquiv(CSOP &Sop1);
  int NoEquiv(CSBF &Sbf2);

  void Sbf_to_Sop(CSOP &Sop_1, CSOP &Sop_0);

//*******************************************************************
/*
#ifndef _LINUX
  CString ToChar(char One='1', char Zero='0', char Dont='-', 
                 BOOL WithNum=TRUE, int max=0) const;
  void ToChar(CStringArray & StrRes, BOOL WithClear=TRUE, BOOL WithNum=TRUE, 
              int max=0, char One='1', char Zero='0', char Dont='-') const;
  void Serialize(CArchive&);
#else   //_LINUX
*/
  char * ToChar(char One='1', char Zero='0', char Dont='-', 
    BOOL WithNum=TRUE, int max=0) const;
  void ToChar(vector <string>& StrRes, BOOL WithClear=TRUE, BOOL WithNum=TRUE, 
    int max=0, char One='1', char Zero='0', char Dont='-') const;
//#endif  //_LINUX

  void Serialize(CArch&);

//*******************************************************************

public:
  CTM m_tmFun;
  CTM m_tmCon; 
};

////////////////////Inline function for getting class parametrs
  inline int CSBF::GetnCon() const { return m_tmCon.GetCountR(); }
  inline int CSBF::GetnFun() const { return m_tmFun.GetCountC(); }
  inline int CSBF::GetnArg() const { return m_tmCon.GetCountC(); }
  inline void CSBF::RemoveAll()
  { m_tmCon.RemoveAll(); m_tmFun.RemoveAll(); }

#endif
