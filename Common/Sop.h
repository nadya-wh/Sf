/////////////////////////////////////////////////////////////////
// Ver.2.1.1    23.04.2007
//              поправки функции генерирования псевдослучайной сети
//              добавлена функция RemoveAll()
// Ver.2.1.0    25.06.2006
//               Переход к LINUX. Поправки BitChar

/////////////////////////////////////////////////////////////////
// Ver.2.0.9     06.05.2006
// Ver.2.0.8     06.04.2006
// Ver.2.0.7     05.04.2006
// Ver.2.0.6     03.04.2006
// Ver.2.0.5     25.03.2006
// Ver.2.0.4     21.03.2006
// Ver.2.0.3     21.03.2006
// Ver.2.0.2     19.03.2006
// Ver.2.0.1     17.03.2006
// Ver.2.0.0     14.03.2006

// Ver.1.1.1     14.02.2005   Russian == English
/////////////////////////////////////////////////////////////////
// SOP.h : header file

#ifndef _SOP
#define _SOP

#include <vector>
#include <utility>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#ifndef _LINUX
//#include <afxwin.h>
//#else

typedef unsigned char BYTE; 
typedef unsigned long ULONG; 
typedef int BOOL;

//const unsigned int  TRUE=1;
//const unsigned int  FALSE=0;

#define ASSERT assert
//#endif

#ifdef _JOINT
#include "BaseBool.h"
#include "BaseTern.h"
#else
#include "../Common/BaseBool.h"
#include "../Common/BaseTern.h"
#endif

typedef std::vector<CTM> MTM;
typedef std::pair <CTV, int> PairTv_I;
typedef std::vector <PairTv_I> MTv_I;
typedef std::vector <MTv_I> MMTv_I;

typedef std::vector<CTM> MTM;

//------------------ Global function in Sop_Elim.cpp ----------------
extern CTM MapLog_ToFi(MTM &MTm, CBM &BmA, int y, int i);
extern CTM MapLog_ToFi(MTM &MTm, CBM &BmA, CBV &bvXY, int i);

//#ifndef _LINUX

//class CSOP : public CObject
//#else
class CSOP
//#endif
{
public:
//***************** Сonstructors/Destructors ************************
  CSOP();
  CSOP(const CSOP& s);
  CSOP(int nArg, int nFun, int nCon, int nInterVar=0);
  CSOP(const CTM tm, const CBM bm, int nInterVar=0);
  ~CSOP();

//***************** Functiom for getting class parametrs ************
  int GetnArg() const;        //inline
  int GetnFun() const;        //inline
  int GetnCon() const;        //inline
  int GetnInterVar() const;   //inline

  //*********** Functions for getting of special characteres ********
  int GetnArgFun(int nFun) const;
  int GetnConFun(int nFun) const;
  int GetnArgR(int nRow) const;
  int NumLongCon(int max) const;
  int EverageRange(void);

//*********************** Functions for generation ******************
  CSOP GenSopMid(int nCon, int nArg,int nFun, int ranArg, int ranFun);
  CSOP GenSopMid_11(int nCon, int nArg,int nFun, int ranArg, int ranFun);
  CSOP GenSopFix(int nCon, int nArg,int nFun, int ranCon, int ranFun);
  CSOP GenLoNet(int k, int n, int m, int p_, int e_, int rn, int rf);
  CSOP GenLoNet(int p, int n, int m, int y, int nRn, int nRy);

//*********************** Reading the data **************************
  CBV GetArg() const;
  CBV GetFun() const;
  CBV GetArgFun(int nFun) const;
  CBV GetConFun(int nFun) const;
  CTV GetConR(int nRow) const;
  CBV GetArgR(int nRow) const;
  CBV GetFunR(int nRow) const;
  CTM GetConTM() const;
  CBM GetFunBM() const;

//*******************************************************************

//#ifndef _LINUX
//  //******** Creation symbolic representation (equation)*****************
//  //                             Четыре функции Торопова  2.07.2002 22:06.
//  CString FormEqv (int i=-1);
//  CString FormFun (int i=-1);
//  void FormEqv (CStringArray &Ass, int i=-1, int lengRow=80);
//  void FormFun (CStringArray &Ass, int i=-1, int lengRow=80);

//  CString ToChar(char One='1', char Zero='0', char Dont='-', BOOL WithNum=TRUE,int max=0);
//  void ToChar(CStringArray & StrRes, BOOL WithClear=TRUE, BOOL WithNum=TRUE,
//              int max=0, char One='1',char Zero='0', char Dont='-');
//  void Serialize(CArchive&);
//#else
  string FormEqv (int i=-1);
  string FormFun (int i=-1);
  char* ToChar(char One='1', char Zero='0', char Dont='-',
               BOOL WithNum=TRUE,int max=0);
//#endif
  void FormEqv (vector <string>  &Ass, int i=-1, int lengRow=80);
  void FormFun (vector <string>  &Ass, int i=-1, int lengRow=80);
  void Serialize(CArch&);
//*******************************************************************

  
  //********************** Writing the data ***************************
  void SetnInterVar(int Num);     //inline
  int AddCon(const CTV ConRow, const CBV FunRow);
  void InsertCon(const CTV ConRow, const CBV FunRow, int nRow);
  void SetRowSOP(int nRow, const CSOP NewSOP, int nSOPRowl);
  void ExchangeR(int nRow1, int nRow2, int nCount = 1);

  void RemoveR(int nRow, int Count=1);
  void RemoveAll();
  CSOP ExtractR(int nFirst, int nCount=1) const;
  CTM ExtractCon(CBV& bvFun);

  const CSOP& operator =(const CSOP& MySOP);
  void SortDecr(int nBeg=0, int number=-1);
  void SortIncr(int nBeg=0, int number=-1);

//********************* Find and analize the data *******************
  int FindCon(const CTV ConRow) const;  //-1, if noone
  int FindFun(const CBV FunRow) const; //-1, if noone
  int FindPair(const CTV ConRow, const CBV FunRow) const;  //-1, if noone


//**************** Implementation in file Sop_Invers ****************
  CSOP InverS(int simpl);
  CSOP InverSR (int simpl);
  CSOP InverS (int simpl, CBV &bvF);
  CSOP InverS (int simpl,CTV tv,CBV bv,CBV bvR);

  int Differ (CTV &tv1,CBV &bv1,CBV bvR);
  void AdExpans (CSOP &Sop0,CTV &tv,CBV &bv,CBV &bvC,CBV &bvW);
  int DelOrBT(CTV &tv, CBV &bv, CBV &bvR);
  void DorD(int Simpl, CSOP Sop1);
  void DorD (int q, CTV tva, CBV bva);

//**************** Implementation in file Sop_Equiv ****************
  int NoEquiv_D_R(CSOP &Sop_R);
  int NoEquiv_Net(CSOP &Sop2);
  int NoEquiv_Net(CBV &bvXY1, CSOP &Sop2, CBV & bvXY2);
  int NoEquiv(CSOP &Sop2);
  int NoEquiv(CSOP &Sop2_1, CSOP &Sop2_0);
  int NoEquiv(CBV &bvP, CSOP &Sop2);
  int NoEquiv(CBV &bvP, CSOP &Sop2_1, CSOP &Sop2_0);

  int NoImpl(CSOP &Sop2);
  int NoImpl(CTV &tv, CBV &bv);
  int NoImpl(CTV &tv, CBV &bv, CBV &bvR);
  int NoImpl(CBV &bvP, CSOP &Sop1);
  int CrossI(CSOP &Sop2);
  void DivPolD(CBV &bv0, CSOP &Sop1, CSOP &Sop0);
  CBM ForMA_V(CBV &bvX);
  CBM ForMK_V (CBM &BmA, CBV &bvX);
  void ForLDNF_V(CBV &bvFY, CBM &BmA, CBM &BmK, MTM &MTm);

  CSOP Sop_toMTM(MTM &Mtm);
  CSOP RedSeq();
  CSOP SeqSdnf_F();
  CSOP DelInt_VR(CBV &bvXY, int rest);

//*******************************************************************
public:
  CTM m_tmCon; //ч.строк=ч.групп ф-й, ч.столб=арг.ДНФ
  CBM m_bmFun; 
  int m_NumInterVar;
};

//**************Inline function for getting class parametrs *********
inline int CSOP::GetnArg() const  { return m_tmCon.GetCountC(); }
inline int CSOP::GetnFun() const  { return m_bmFun.GetCountC(); }
inline int CSOP::GetnCon() const  { return m_tmCon.GetCountR(); }

inline int CSOP::GetnInterVar() const  { return m_NumInterVar; }
inline void CSOP::SetnInterVar(int Num) { m_NumInterVar = Num; }

//******************** Inline implementation ************************
//-------------------------------------------------------------------
inline int CSOP::GetnArgR(int nRow) const
{ ASSERT(nRow >= 0 && nRow < m_tmCon.GetCountR());
  return m_tmCon.CountDefs(nRow); }

//-------------------------------------------------------------------
inline CBV CSOP::GetConFun(int nFun) const
{ ASSERT(nFun >= 0 && nFun < m_bmFun.GetCountC());
  return m_bmFun.GetColumnBv(nFun); }

//-------------------------------------------------------------------
inline CTV CSOP::GetConR(int nRow) const
{ ASSERT(nRow >= 0 && nRow < m_bmFun.GetCountR());
  return m_tmCon.GetRowTv(nRow); }

//-------------------------------------------------------------------
inline CBV CSOP::GetArgR(int nRow) const
{ ASSERT(nRow >= 0 && nRow < m_bmFun.GetCountR());
  return m_tmCon.GetRowDef(nRow); }

//-------------------------------------------------------------------
inline CBV CSOP::GetFunR(int nRow) const
{ ASSERT(nRow >= 0 && nRow < m_bmFun.GetCountR());
  return m_bmFun.GetRowBv(nRow); }

//-------------------------------------------------------------------
inline CTM CSOP::GetConTM() const { return m_tmCon; }
inline CBM CSOP::GetFunBM() const { return m_bmFun; }

//--------------------------------------------------------------------
inline int CSOP::AddCon(const CTV ConRow, const CBV FunRow)
{ 
#ifdef _DEBUG
  if (GetnCon()>0 ) {
    ASSERT(FunRow.GetBitLength() == m_bmFun.GetCountC());
    ASSERT(ConRow.GetBitLength() == m_tmCon.GetCountC());
  }
#endif
  m_tmCon.Add(ConRow); return m_bmFun.Add(FunRow); 
}

//--------------------------------------------------------------------
inline void CSOP::InsertCon(const CTV ConRow, const CBV FunRow, int nRow)
{ 
  ASSERT(FunRow.GetBitLength() == m_bmFun.GetCountC());
  ASSERT(ConRow.GetBitLength() == m_tmCon.GetCountC());
  ASSERT(nRow >= 0 && nRow < m_bmFun.GetCountR());
  m_tmCon.InsertRow(nRow,ConRow); m_bmFun.InsertRow(nRow,FunRow); 
}

//--------------------------------------------------------------------
inline void CSOP::SetRowSOP(int nRow, const CSOP NewSOP, int nSOPRowl)
{
  ASSERT(m_bmFun.GetCountC() == NewSOP.m_bmFun.GetCountC());
  ASSERT(m_tmCon.GetCountC() == NewSOP.m_tmCon.GetCountC());
  int dl = nRow + nSOPRowl;
  ASSERT(nRow >= 0 && dl <= m_bmFun.GetCountR());
  ASSERT(dl <= NewSOP.m_bmFun.GetCountR());
  m_tmCon.SetRowTv(nRow, NewSOP.m_tmCon, nSOPRowl);
  m_bmFun.SetRow(nRow, NewSOP.m_bmFun, nSOPRowl);
}

//--------------------------------------------------------------------
inline void CSOP::RemoveR(int nRow,int Count/*=1*/)
{  
  ASSERT(nRow >= 0 && nRow+Count <= m_bmFun.GetCountR());
  m_bmFun.RemoveRow(nRow, Count); m_tmCon.RemoveRow(nRow, Count); 
}

//--------------------------------------------------------------------
inline void CSOP::RemoveAll()
{ m_tmCon.RemoveAll(); m_bmFun.RemoveAll(); m_NumInterVar	=0;}

//--------------------------------------------------------------------
inline void CSOP::ExchangeR(int nRow1, int nRow2, int nCount/*=1*/)
{ m_tmCon.ExchangeRow(nRow1, nRow2, nCount); 
  m_bmFun.ExchangeRow(nRow1, nRow2, nCount);
}

//--------------------------------------------------------------------
inline const CSOP& CSOP::operator =(const CSOP& MySOP)
{ m_tmCon=MySOP.m_tmCon; m_bmFun=MySOP.m_bmFun; 
  m_NumInterVar = MySOP.m_NumInterVar; return *this; 
}

#endif
