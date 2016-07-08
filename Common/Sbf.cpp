/////////////////////////////////////////////////////////////////////
// Ver.2.1.2    14.05.2011
//              Встроен обход лишнего захвата памяти в GenSbfMid_11
// Ver.2.1.1    23.04.2007
//              Поправка в функции Sbf_toSop
//              Добавлена функция RemoveAll()
// Ver.2.1.0    07.07.2006
//              Переход к LINUX

/////////////////////////////////////////////////////////////////////
// Ver.2.0.7    28.04.2006
//              Между матрицами только один пробел, а не 2 (ToChar(StringArray)
// Ver.2.0.6    18.04.2006
//              удалена функция Sbf_to_Sop(C2SOP) - класс сделан независимым
//              от С2SOP - взамен в этот класс добавлена функция получения
//              объекта от CSBF
// Ver.2.0.5    05.04.2006
//              поправки в ToChar - переставлены параметры 
//              (WithNum, WithClear, max)
// Ver.2.0.4    25.03.2006
//               ToChar (для массива строк добавлен параметр, говорящий 
//               о необходимости предварительной очистки массива результата
// Ver.2.0.3    24.03.2006
//               - добавлена операция serialize
// Ver.2.0.2    21.03.2006
//              Генераторы случайных перестроены на возврат *this
//              - устранена старая ошибка в конструкторе по размерам
// Ver.2.0.1    19.03.2006
//              - поправка деструктора - очистка памяти матриц
// Ver.2.0.0    17.03.2006
//              - добавки Торопова и поправка форматирования

// Ver.1.1.1    14.02.2005   Russian == English
///////////////////////////////////////////////////////////////////////////////
#ifdef _JOINT
#include "Sbf.h"
#else
#include "../Common/Sbf.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// CSBF

//-----------------------------------------------------------------------------
CSBF::CSBF() { }
CSBF::CSBF(const CSBF& s)
{ 
  ASSERT(s.m_tmCon.GetCountR() == s.m_tmFun.GetCountR());
  m_tmCon=s.m_tmCon;   m_tmFun=s.m_tmFun;
}
CSBF::CSBF(const CTM tm, const CTM bm)
{ 
  ASSERT(m_tmCon.GetCountR() == m_tmFun.GetCountR());
  m_tmCon=tm;    m_tmFun=bm;
}
CSBF::CSBF(int nArg,int nFun,int nCon)
{ m_tmCon.SetSize(nCon,nArg); m_tmFun.SetSize(nCon,nFun);  } //21.03.2006 

CSBF::~CSBF() { m_tmCon.RemoveAll(); m_tmFun.RemoveAll(); }

//-----------------------------------------------------------------------------
const CSBF& CSBF::operator =(const CSBF& MySBF)
{  m_tmCon=MySBF.m_tmCon; m_tmFun=MySBF.m_tmFun;  return *this; }

//-----------------------------------------------------------------------------
int CSBF::AddCon(const CTV ConRow,const CTV FunRow)
{ ASSERT(FunRow.GetBitLength() == m_tmFun.GetCountC());
  ASSERT(ConRow.GetBitLength() == m_tmCon.GetCountC());
  m_tmCon.Add(ConRow);
  return m_tmFun.Add(FunRow);
}

//-----------------------------------------------------------------------------
void CSBF::RemoveR(int nRow,int Count/*=1*/)
{
  ASSERT(nRow >= 0 && nRow+Count <= m_tmFun.GetCountR());
  m_tmFun.RemoveRow(nRow,Count);
  m_tmCon.RemoveRow(nRow,Count);
}
/*
#ifndef _LINUX
//-----------------------------------------------------------------------------
CString CSBF::ToChar(char One,char Zero,char Dont,BOOL WithNum,int max) const
{
  CString Buf;
  CString s,ch;
  int l;
  if (m_tmCon.GetCountR()>9999)     { ch = "%5d: ";}
  else if (m_tmCon.GetCountR()>999) { ch = "%4d: ";}
  else if (m_tmCon.GetCountR()>99)  { ch = "%3d: ";}
  else                              { ch = "%2d: ";}

  for (int i=l=0; i < m_tmCon.GetCountR(); i++)  {
    if(WithNum == TRUE)  { s.Format((const char *)ch,i); Buf+=s; }
    Buf += m_tmCon.GetRowTv(i).BitChar(One,Zero,Dont); Buf += " ";
    if(max > 0 && (Buf.GetLength()-l) >=max) Buf += "\n";
    Buf += m_tmFun.GetRowTv(i).BitChar(One,Zero,Dont); Buf += "\n";
    l = Buf.GetLength();
  }
  return Buf;
}

//-----------------------------------------------------------------------------
void CSBF::ToChar(CStringArray & StrRes, BOOL WithClear, BOOL WithNum,
                  int max, char One, char Zero, char Dont) const
{ 
  CString Buf, ch;
  if (WithClear) StrRes.RemoveAll();
  if (m_tmCon.GetCountR()>9999)     { ch = "%5d: ";}
  else if (m_tmCon.GetCountR()>999) { ch = "%4d: ";}
  else if (m_tmCon.GetCountR()>99)  { ch = "%3d: ";}
  else                              { ch = "%2d: ";}

  for (int i=0; i < m_tmCon.GetCountR(); i++)  {
    if(WithNum)  Buf.Format((const char *)ch,i);
    else         Buf.Empty( );
    Buf += m_tmCon.GetRowTv(i).BitChar(One,Zero,Dont); Buf += " "; //28.04.2006
    if(max > 0 && Buf.GetLength() >= max);
    else Buf += m_tmFun.GetRowTv(i).BitChar(One,Zero,Dont);
    StrRes.Add(Buf);
  }
  return ;
}

//------------------------------------------Serialize(CArchive& ar)
void CSBF::Serialize(CArchive& ar)
{
  m_tmCon.Serialize(ar);
  m_tmFun.Serialize(ar);
}

#else
*/
//-----------------------------------------------------------------------------
char * CSBF::ToChar(char One,char Zero,char Dont,BOOL WithNum,int max) const
{
  char* Buf;
  char s[20], ch[20], endl[3];
  int l, len;
/*
#ifndef _LINUX
  endl[0]='\r'; endl[1] = '\n'; endl[2] = 0;
#else
*/
  endl[0]='\n'; endl[1] = 0;
//#endif

  if (m_tmCon.GetCountR()>9999)     { strcat(ch,"%5d: ");}
  else if (m_tmCon.GetCountR()>999) { strcat(ch,"%4d: ");}
  else if (m_tmCon.GetCountR()>99)  { strcat(ch,"%3d: ");}
  else                              { strcat(ch,"%2d: ");}
  Buf = (char*)malloc(m_tmCon.GetCountR()*
    (m_tmCon.GetCountC()+m_tmFun.GetCountR()+10));
  Buf[0] = '\0';
  for (int i=l=0; i < m_tmCon.GetCountR(); i++)  {
    if(WithNum == TRUE)  { sprintf(s, ch, i); strcat(Buf,s); }
    strcat(Buf,m_tmCon.GetRowTv(i).BitChar(One,Zero,Dont));
    strcat(Buf, " ");
    len = strlen(Buf);
    if(max > 0 && (len-l) >=max) strcat(Buf, endl);
    strcat(Buf, m_tmFun.GetRowTv(i).BitChar(One,Zero,Dont));
    strcat(Buf, endl);
    l = strlen(Buf);
  }
  return Buf;
}


//-----------------------------------------------------------------------------
void CSBF::ToChar(vector <string>& StrRes, BOOL WithClear, BOOL WithNum,
                  int max, char One, char Zero, char Dont) const
{ 
  string Buf, ch;
  char bbf[20];
  if (WithClear) StrRes.clear();
  if (m_tmCon.GetCountR()>9999)     { ch = "%5d: ";}
  else if (m_tmCon.GetCountR()>999) { ch = "%4d: ";}
  else if (m_tmCon.GetCountR()>99)  { ch = "%3d: ";}
  else                              { ch = "%2d: ";}

  for (int i=0; i < m_tmCon.GetCountR(); i++)  {
    if(WithNum) { 
      sprintf(bbf,(const char *)ch.c_str(), i);
      Buf = bbf;//.Format((const char *)ch,i);
    }
    else        { Buf.clear( ); }
    Buf += m_tmCon.GetRowTv(i).BitChar(One,Zero,Dont); Buf += " "; //28.04.2006
    if(max > 0 && Buf.length() >= max);
    else Buf += m_tmFun.GetRowTv(i).BitChar(One,Zero,Dont);
    StrRes.push_back(Buf);
  }
  return ;
}
//#endif


//------------------------------------------Serialize(CArch& ar)
void CSBF::Serialize(CArch& ar)
{
  m_tmCon.Serialize(ar);
  m_tmFun.Serialize(ar);
}


///////////////////////////////////////////////////////////////////////////////
// Генерирование системы ДНФ для <nFun> частичных булевых функций
// от <nArg> аргументов. Каждая из <nCon> конъюнкций имеет средний
// ранг <ranCon> и входит в среднем в <ranFun> ДНФ функций системы.
//    При ranCon>=nArg ранг всех nCon конъюнкций равен nArg, что 
// соответствует заданию функций системы в совершенной ДНФ, а при
// ranFun>=nFun все указанные конъюнкции входят во все ДНФ системы.
//-----------------------------------------------------------------------------

CSBF CSBF::GenSbfMid (int nCon, int nArg,int nFun, int ranCon, int ranFun)
{
  int i, j;
  CBV bvOrtG ((BYTE)0, nFun), bvOrt = bvOrtG, bv, bv0, bv1;
  CTV tvMeanG (nFun, '-');
  CTM tm;
//  CSBF Sbf(tm.GenRtmMid(nCon, nArg, ranCon),tm.GenRtmMid(nCon, nFun, ranFun));
  m_tmCon.GenRtmMid(nCon, nArg, ranCon);
  m_tmFun.GenRtmMid(nCon, nFun, ranFun);
  for (i=0; i<nCon; i++)
  {
    bvOrt.Zero();
    for (j=0; j<nCon; j++)
    {
      if ((j!=i) && !m_tmCon.AreOrthog(i,j) 
                 && (!(m_tmFun.GetRowDef(i)
                      & m_tmFun.GetRowDef(j)).IsZero()))
        bvOrt |= m_tmFun.GetRowDef(j);
    }
    if (!(bvOrt &= m_tmFun.GetRowDef(i)).IsZero())
    {
      bv=bvOrt & ~bvOrtG;
      tvMeanG.OrInPlace(0, m_tmFun.GetRowZero(i,bv));
      tvMeanG.OrInPlace(1, m_tmFun.GetRowOne(i,bv));
      bv = m_tmFun.GetRowDef(i,bvOrt & bvOrtG);
      bv0 = m_tmFun.GetRowZero(i,~(bvOrt & bvOrtG)) |
            tvMeanG.GetZeros() & bv;
      bv1 = m_tmFun.GetRowOne(i,~(bvOrt & bvOrtG)) |
            tvMeanG.GetOnes() & bv;
      m_tmFun.SetRowZero(i,bv0);
      m_tmFun.SetRowOne(i,bv1);
      bvOrtG |= bvOrt;
    }
  }    
// Проверка на непротиворечивость значений функций системы
// (противоречивые значения отмечаются в Sbf.m_tmFun символом '+'.
  for (i=0; i<nCon-1; i++)
    for (j=i; j<nCon; j++)
      if ((j!=i) && m_tmFun.AreOrthog(i,j) && !m_tmCon.AreOrthog(i,j))
      {
        bvOrt = m_tmFun.Orthogon (i, j);
        m_tmFun.OrInPlace(0, bvOrt, j);
        m_tmFun.OrInPlace(1, bvOrt, j);
      }
  return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Генерирование системы ДНФ для <nFun> частичных булевых функций
// от <nArg> аргументов. Каждая из <nCon> конъюнкций имеет средний
// ранг <ranCon> и входит в среднем в <ranFun> ДНФ функций системы.
//    При ranCon>=nArg ранг всех nCon конъюнкций равен nArg, что 
// соответствует заданию функций системы в совершенной ДНФ, а при
// ranFun>=nFun все указанные конъюнкции входят во все ДНФ системы.
//    Ранг любой из конъюнкций больше нуля, каждая из них входит хотя
// бы в одну ДНФ системы.
//    (Внесена по просьбе пользователей 14.12.01)   
//-----------------------------------------------------------------------------

CSBF CSBF::GenSbfMid_11 (int nCon, int nArg,int nFun, int ranCon, int ranFun)
{
  int i, j;
  CBV bvOrtG ((BYTE)0, nFun), bvOrt = bvOrtG, bv, bv0, bv1;
  CTV tv, tv1, tvMeanG (nFun, '-');
  //CSBF Sbf(nArg,  nFun, nCon);
  m_tmCon.SetSize(0, nArg);
  m_tmFun.SetSize(0, nFun);

  for(i=0; i<nCon; i++)
  {
    do { 
      CTV tt = tv.GenRtvMid(nArg, ranCon);
      if (tt.IsTrivial()==0)  break;
    } while(1);                   //    while(tv.GenRtvMid(nArg, ranCon).IsTrivial());
    while(tv1.GenRtvMid(nFun,ranFun).IsTrivial());
    AddCon(tv,tv1);
  } 
  for (i=0; i<nCon; i++)
  {
    bvOrt.Zero();
    for (j=0; j<nCon; j++)
    {
      if ((j!=i) && !m_tmCon.AreOrthog(i,j) 
           && (!(m_tmFun.GetRowDef(i)
            & m_tmFun.GetRowDef(j)).IsZero()))
        bvOrt |= m_tmFun.GetRowDef(j);
    }
    if (!(bvOrt &= m_tmFun.GetRowDef(i)).IsZero())
    {
      bv=bvOrt & ~bvOrtG;     //sbvOrt=bvOrt.BitChar();
      tvMeanG.OrInPlace(0, m_tmFun.GetRowZero(i,bv));
      tvMeanG.OrInPlace(1, m_tmFun.GetRowOne(i,bv));
      bv = m_tmFun.GetRowDef(i,bvOrt & bvOrtG);
      bv0 = m_tmFun.GetRowZero(i,~(bvOrt & bvOrtG)) |
            tvMeanG.GetZeros() & bv;
      bv1 = m_tmFun.GetRowOne(i,~(bvOrt & bvOrtG)) |
            tvMeanG.GetOnes() & bv;
      m_tmFun.SetRowZero(i,bv0);
      m_tmFun.SetRowOne(i,bv1);   //sSbf=Sbf.ToChar();
      bvOrtG|=bvOrt;          //sbvOrtG=bvOrtG.BitChar();
    }
  }
// Проверка на непротиворечивость значений функций системы
// (противоречивые значения отмечаются в Sbf.m_tmFun символом '+'.
  for (i=0; i<nCon-1; i++)
    for (j=i; j<nCon; j++)
    if ((j!=i) && m_tmFun.AreOrthog(i,j) && !m_tmCon.AreOrthog(i,j))
    {
      bvOrt = m_tmFun.Orthogon (i, j);
      m_tmFun.OrInPlace(0, bvOrt, j);
      m_tmFun.OrInPlace(1, bvOrt, j);
    }
  return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Генерирование системы ДНФ для <nFun> частичных булевых функций
// от <nArg> аргументов. Каждая из <nCon> конъюнкций имеет 
// фиксированный ранг <ranCon> и входит в <ranFun> ДНФ функций системы.
//    При ranCon>=nArg ранг всех nCon конъюнкций равен nArg, что 
// соответствует заданию функций системы в совершенной ДНФ, а при
// ranFun>=nFun все указанные конъюнкции входят во все ДНФ системы.
//-----------------------------------------------------------------------------

CSBF CSBF::GenSbfFix (int nCon, int nArg,int nFun, int ranCon, int ranFun)
{
  CBV bvOrtG ((BYTE)0, nFun), bvOrt = bvOrtG, bv, bv0, bv1;
  CTV tvMeanG (nFun, '-');
  int i, j;
  m_tmCon.GenRtmFix (nCon, nArg, ranCon); 
  m_tmFun.GenRtmFix (nCon, nFun, ranFun);
  for (i=0; i<nCon; i++)
  { 
    bvOrt.Zero();
    for (j=0; j<nCon; j++)
    {
      if ((j!=i) && !m_tmCon.AreOrthog(i,j) 
           && (!(m_tmFun.GetRowDef(i)
            & m_tmFun.GetRowDef(j)).IsZero()))
        bvOrt |= m_tmFun.GetRowDef(j);
    }
    if (!(bvOrt &= m_tmFun.GetRowDef(i)).IsZero())
    {
      bv=bvOrt & ~bvOrtG;
      tvMeanG.OrInPlace(0, m_tmFun.GetRowZero(i,bv));
      tvMeanG.OrInPlace(1, m_tmFun.GetRowOne(i,bv));
      bv = m_tmFun.GetRowDef(i,bvOrt & bvOrtG);
      bv0 = m_tmFun.GetRowZero(i,~(bvOrt & bvOrtG)) |
            tvMeanG.GetZeros() & bv;
      bv1 = m_tmFun.GetRowOne(i,~(bvOrt & bvOrtG)) |
            tvMeanG.GetOnes() & bv;
      m_tmFun.SetRowZero(i,bv0);
      m_tmFun.SetRowOne(i,bv1);
      bvOrtG |= bvOrt;
    }
  }    
// Проверка на непротиворечивость значений функций системы
// (противоречивые значения отмечаются в m_tmFun символом '+'.
  for (i=0; i<nCon-1; i++)
    for (j=i; j<nCon; j++)
      if ((j!=i) && m_tmFun.AreOrthog(i,j) && !m_tmCon.AreOrthog(i,j))
      {
        bvOrt = m_tmFun.Orthogon (i, j);
        m_tmFun.OrInPlace(0, bvOrt, j);
        m_tmFun.OrInPlace(0, bvOrt, j);
      }
  return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Проверка отношения реализуемости системой ДНФ Sop1
// системы частичных булевых функций this 
//-----------------------------------------------------------------------------

int CSBF::NoEquiv(CSOP &Sop1)
{
  CSOP Sop2_1 (m_tmCon, m_tmFun.GetOnes());
  CSOP Sop2_0 (m_tmCon, m_tmFun.GetZeros());
  return Sop1.NoEquiv (Sop2_1, Sop2_0);
}

///////////////////////////////////////////////////////////////////////////////
// Проверка отношения эквивалентности двух систем ДНФ 
// частичных булевых функций (с точностью до их реализации)
//-----------------------------------------------------------------------------

int CSBF::NoEquiv(CSBF &Sbf2)
{
  int z;
  CSOP Sop1_1 (m_tmCon, m_tmFun.GetOnes());
  CSOP Sop2_0 (Sbf2.m_tmCon, Sbf2.m_tmFun.GetZeros());
  if (z= Sop1_1.CrossI(Sop2_0)) return ((z==-1)?z:z=-z-1);
  CSOP Sop1_0 (m_tmCon, m_tmFun.GetZeros());
  CSOP Sop2_1 (Sbf2.m_tmCon, Sbf2.m_tmFun.GetOnes());
  return (Sop1_0.CrossI(Sop2_1));
}

///////////////////////////////////////////////////////////////////////////////
// Разложение системы частичных функций ДНФ на две
// с определением 1- и 0- значений функций
//-----------------------------------------------------------------------------

void CSBF::Sbf_to_Sop(CSOP &Sop_1,CSOP &Sop_0)
{
  CBV bv;
  int i;
  Sop_1.RemoveAll();    //Ver.2.1.1
  Sop_0.RemoveAll();    //Ver.2.1.1
  for (i=0; i < GetnCon(); i++)
  {
    if (!(bv=m_tmFun.GetRowOne(i)).IsZero())
      Sop_1.AddCon(m_tmCon.GetRowTv(i),bv);
    if (!(bv=m_tmFun.GetRowZero(i)).IsZero())
      Sop_0.AddCon(m_tmCon.GetRowTv(i),bv);
  }
}

