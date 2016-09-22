/////////////////////////////////////////////////////////////////
// Ver.2.1.4    14.05.2011
//              Встроен обход лишнего захвата памяти в GenSopMid_11
// Ver.2.1.3    02.10.2007
//              поправки BM[a] - (const BYTE*)BM[a]
// Ver.2.1.2    28.05.2007
//              поправки функции генерирования псевдослучайной сети
// Ver.2.1.1    23.04.2007 25.04.2007
//              поправки функции генерирования псевдослучайной сети
// Ver.2.1.0    07.07.2006
//              Переход к LINUX. Поправки BitChar

/////////////////////////////////////////////////////////////////
// Ver.2.0.8     06.04.2006
//               после последнего уравнения будет записываться всегда ';'
//               по всем функциям формирования уравнений
// Ver.2.0.7     05.04.2006
//               поправки в ToChar - переставлены параметры 
//               (WithNum, WithClear, max)
// Ver.2.0.6     03.04.2006
//               FotmFun, FormEqv - изменения
// Ver.2.0.5     25.03.2006
//               ToChar (для массива строк добавлен параметр, говорящий 
//               о необходимости предварительной очистки массива результата
// Ver.2.0.4     21.03.2006
//               добавлена операция serialize
// Ver.2.0.3     21.03.2006
//                Генераторы случайных перестроены на возврат *this
//                ForMK_V(CBV &bvX) перенесена из класса CBM;
// Ver.2.0.2     19.03.2006
//              - поправка деструктора - очистка памяти матриц
//              - устранена старая ошибка в конструкторе по размерам
// Ver.2.0.1     17.03.2006
//              - в пустом конструкторе 0 в число внутренних переменных
// Ver.2.0.0     04.03.2006

// Ver.1.1.1     14.02.2005   Russian == English
/////////////////////////////////////////////////////////////////
// Sums of products System  - CSOP System

#ifdef _JOINT
#include "Sop.h"
#else
#include "../Common/Sop.h"
#endif

// #ifdef _DEBUG
// #undef THIS_FILE
// static char BASED_CODE THIS_FILE[] = __FILE__;
// #endif

/////////////////////////////////////////////////////////////////////
// CSOP

//-------------------------------------------------------------------
CSOP::CSOP() { m_NumInterVar = 0; }

//-------------------------------------------------------------------
CSOP::CSOP(int nArg,int nFun,int nCon, int nInterVar/*=0*/)
{ 
  m_tmCon.SetSize(nCon,nArg);  m_bmFun.SetSize(nCon,nFun); //19.03.2006
  m_NumInterVar = nInterVar; 
}

//-------------------------------------------------------------------
CSOP::CSOP(const CTM tm, const CBM bm, int nInterVar/*=0*/)
{ 
  ASSERT(m_tmCon.GetCountR() == m_bmFun.GetCountR());
  m_tmCon=tm;   m_bmFun=bm;  m_NumInterVar = nInterVar;
}

//-------------------------------------------------------------------
CSOP::CSOP(const CSOP& s)
{ 
  ASSERT(s.m_tmCon.GetCountR() == s.m_bmFun.GetCountR());
  m_tmCon=s.m_tmCon;  m_bmFun=s.m_bmFun;  
  m_NumInterVar = s.m_NumInterVar;
}

//-------------------------------------------------------------------
CSOP::~CSOP() { m_tmCon.RemoveAll(); m_bmFun.RemoveAll();}

//-------------------------------------------------------------------
int CSOP::GetnArgFun(int nFun) const
{ 
  ASSERT(nFun >= 0 && nFun < m_bmFun.GetCountC());
  CBV Arg ((BYTE)0,m_tmCon.GetCountC());
  for(int i=0; i < m_bmFun.GetCountR(); i++)
    if(m_bmFun.GetBitAt(i,nFun) == TRUE) Arg|=m_tmCon.GetRowDef(i);
  return Arg.CountBit();
}

//-------------------------------------------------------------------
int CSOP::GetnConFun(int nFun) const
{ ASSERT(nFun >= 0 && nFun < m_bmFun.GetCountC());
  int i, nCon=0;            //Monday 08 February 0100 (16:00:20)
  for(i=0; i < m_bmFun.GetCountR(); i++)
    if(m_bmFun.GetBitAt(i,nFun) == TRUE) nCon++;
 return nCon;
}

//-------------------------------------------------------------------
int CSOP::NumLongCon(int max) const
{ int i, num;
  for(i=num=0; i < m_tmCon.GetCountR(); i++)
  if(m_tmCon.CountDefs(i) > max) num++;
  return num;
}

//-----------------------------------------------------------------------------
int CSOP::EverageRange(void)
{ int pos, sum;
  for(pos=sum=0; pos < m_tmCon.GetCountR(); pos++)
    sum+=m_tmCon.CountDefs(pos);
  return sum/m_tmCon.GetCountR();
}

//***************** Reading the data ***************************
//-------------------------------------------------------------------
CBV CSOP::GetArg() const
{ CBV Arg((BYTE)0,m_tmCon.GetCountC());
  for(int i=0; i < m_tmCon.GetCountR(); Arg|=m_tmCon.GetRowDef(i++));
  return Arg;
}

//-------------------------------------------------------------------
CBV CSOP::GetFun() const
{ CBV Fun((BYTE)0,m_bmFun.GetCountC());
  for(int i=0; i < m_bmFun.GetCountR(); Fun|=m_bmFun.GetRowBv(i++));
  return Fun;
}

//-------------------------------------------------------------------
CBV CSOP::GetArgFun(int nFun) const
{ ASSERT(nFun >= 0 && nFun < m_bmFun.GetCountC());
  CBV Arg((BYTE)0,m_tmCon.GetCountC());
  for(int i=0; i < m_bmFun.GetCountR(); i++)
    if(m_bmFun.GetBitAt(i,nFun) == TRUE) Arg|=m_tmCon.GetRowDef(i);
  return Arg;
}


//#ifndef _LINUX
////-------------------------------------------------------------------
//CString CSOP::ToChar(char One, char Zero, char Dont,
//                     BOOL WithNum, int max)
//{ CString Buf;
//  CString s,ch;
//  if (m_tmCon.GetCountR()>9999)     { ch = "%5d: ";}
//  else if (m_tmCon.GetCountR()>999) { ch = "%4d: ";}
//  else if (m_tmCon.GetCountR()>99)  { ch = "%3d: ";}
//  else                              { ch = "%2d: ";}
//  int sum = m_bmFun.GetCountC() + m_tmCon.GetCountC();
  
//  for (int i=0; i < m_tmCon.GetCountR(); i++)  {
//    if(WithNum == TRUE) { s.Format((const char*)ch,i); Buf+=s; }
//    Buf += m_tmCon.GetRowTv(i).BitChar(One,Zero,Dont); Buf += " ";
//    if(max > 0 && sum > max) Buf += "\n";
//    Buf += m_bmFun.GetRowBv(i).BitChar(One,Zero); Buf += "\n";
//  }
//  return Buf;
//}

////-------------------------------------------------------------------
//void CSOP::ToChar(CStringArray & StrRes, BOOL WithClear, BOOL WithNum, int max,
//                  char One, char Zero, char Dont)
//{
//  CString Buf, ch;
//  if (WithClear) StrRes.RemoveAll();
//  if (m_tmCon.GetCountR()>9999)     { ch = "%5d: ";}
//  else if (m_tmCon.GetCountR()>999) { ch = "%4d: ";}
//  else if (m_tmCon.GetCountR()>99)  { ch = "%3d: ";}
//  else                              { ch = "%2d: ";}

//  int sum = m_bmFun.GetCountC() + m_tmCon.GetCountC();
//  for (int i=0; i < m_tmCon.GetCountR(); i++) {
//    if(WithNum == TRUE) Buf.Format((const char*)ch,i);
//    else                Buf.Empty();
//    Buf+=m_tmCon.GetRowTv(i).BitChar(One,Zero,Dont); Buf += " ";
//    if(max > 0 && sum > max) {
//      StrRes.Add(Buf); Buf.Empty();
//    }
//    Buf += m_bmFun.GetRowBv(i).BitChar(One,Zero);
//    StrRes.Add(Buf);
//  }
//}

//#else
//-------------------------------------------------------------------
char* CSOP::ToChar(char One, char Zero, char Dont, 
                     BOOL WithNum, int max)
{ char* Buf;
  char s[20],ch[20];
  if (m_tmCon.GetCountR()>9999)     { strcpy(ch,"%5d: ");}
  else if (m_tmCon.GetCountR()>999) { strcpy(ch,"%4d: ");}
  else if (m_tmCon.GetCountR()>99)  { strcpy(ch,"%3d: ");}
  else                              { strcpy(ch,"%2d: ");}
  int sum = m_bmFun.GetCountC() + m_tmCon.GetCountC();
  Buf = (char*)malloc(m_tmCon.GetCountR()*(sum + 10));
  Buf[0] = '\0';
  for (int i=0; i < m_tmCon.GetCountR(); i++)  {
    if(WithNum == TRUE) 
    { sprintf(s,ch,i); strcat(Buf,s); }
    strcat(Buf,m_tmCon.GetRowTv(i).BitChar(One,Zero,Dont));
    if(max > 0 && sum > max) strcat(Buf,"\n");
    else                     strcat(Buf," ");
    strcat(Buf, m_bmFun.GetRowBv(i).BitChar(One,Zero));
    strcat(Buf, "\n");
  }
  return Buf;
}

//#endif
//-------------------------------------------------------------------
CSOP CSOP::ExtractR(int nFirst, int nCount/*=1*/) const
{ ASSERT(nFirst >= 0 && nCount > 0);
  ASSERT((nFirst + nCount) <= m_bmFun.GetCountR());
  CSOP NewSOP(m_tmCon.ExtractRows(nFirst,nCount),
              m_bmFun.ExtractRows(nFirst,nCount));
  return NewSOP;
}

//-------------------------------------------------------------------
CTM CSOP::ExtractCon(CBV& bvFun)
{ ASSERT(bvFun.GetBitLength() == m_bmFun.GetCountC());
  int nRow=-1;
  CTM tmCon;
  while (m_bmFun.FindRow(bvFun,nRow) > 0)
    tmCon.Add(m_tmCon.GetRowTv(nRow));
  return tmCon;
}


//-------------------------------------------------------------------
void CSOP::SortDecr(int nBeg/*=0*/,int number/*=-1*/)
{ ASSERT(number == -1 || number > 1);
  BOOL bNotDone = TRUE;
  int pos, posNext, nEnd=(number < 0)?m_tmCon.GetCountR():(nBeg+number);
  ASSERT(nEnd <=m_tmCon.GetCountR());
  while (bNotDone) {
    bNotDone = FALSE;
    for(pos=nBeg,posNext=nBeg+1; pos < nEnd-1;pos++,posNext++)
      if(m_tmCon.CountDefs(pos) < m_tmCon.CountDefs(posNext)) {
        ExchangeR(pos,posNext);
        bNotDone = TRUE;
      }
  }
}

//-------------------------------------------------------------------
void CSOP::SortIncr(int nBeg/*=0*/,int number/*=-1*/)
{ ASSERT(number == -1 || number > 1);
  BOOL bNotDone = TRUE;
  int pos, posNext, nEnd=(number < 0)? m_tmCon.GetCountR() : (nBeg+number);
  ASSERT(nEnd <=m_tmCon.GetCountR());
  while (bNotDone) {
    bNotDone = FALSE;
    for(pos=nBeg,posNext=nBeg+1; pos < nEnd-1;pos++,posNext++)
      if(m_tmCon.CountDefs(pos) > m_tmCon.CountDefs(posNext)) {
        ExchangeR(pos,posNext);
        bNotDone = TRUE;
      }
  }
}

//#ifndef _LINUX

////******** Creation symbolic representation (equation)*****************

///////////////////////////////////////////////////////////////////////////////
////*****************************************************************
//// Получить аналитическое выражение в классе CString i-го уравнения
//// (или при i=-1 всех уравнений системы Sop) c учетом числа
//// промежуточных переменных при именовании переменных и функций.
////*****************************************************************
////-----------------------------------------------------------------------------
//CString CSOP::FormEqv (int i/*=-1*/)
//{ int  a, c, d, k, q, n=GetnArg()-m_NumInterVar, m=GetnFun()-m_NumInterVar;
//  ASSERT (m_NumInterVar>=0 && m_NumInterVar<GetnFun() && i<GetnFun());
//  CString ss;  char buff[20];
//  CBV bvCol(1,n+m_NumInterVar), bvR;
//  if(i==-1) {q=m+m_NumInterVar; i=0;}
//  else q=i+1;
//  for(i; i<q; i++) {
//    if(i<m)
//#ifndef _MSVC9
//		sprintf(buff,"f%d",i);
//#else
//		sprintf_s(buff, 20, "f%d",i);
//#endif
//    else
//#ifndef _MSVC9
//		sprintf(buff,"y%d",i-m);
//#else
//		sprintf_s(buff,20, "y%d",i-m);
//#endif
//    ss+=buff; ss+="=";
//    bvR=m_bmFun.GetColumnBv(i);
//    if((d=bvR.CountBit())>1) ss+='(';
//    else if(!d) ss+="[=0]";
//    c=-1;
//    while((c=bvR.LeftOne(c))!=-1) {
//      if(m_tmCon.IsTrivial(c)) ss+="[=1]";
//      k=-1; a=0;
//      while((k=m_tmCon.LeftDef(c,k))!=-1) {
//        if(a) ss+='*';
//        a=1;
//        if (m_tmCon.GetBitAt(c,k)=='0') ss+='^';
//        if(k<n)
//#ifndef _MSVC9
//          sprintf(buff,"x%d",k);
//#else
//          sprintf_s(buff, 20, "x%d",k);
//#endif
//        else
//#ifndef _MSVC9
//          sprintf(buff,"y%d",k-n);
//#else
//          sprintf_s(buff,20,"y%d",k-n);
//#endif
//        ss+=buff;
//      }
//      if(bvR.LeftOne(c)!=-1) ss+='+';
//    }
//    if(d>1) ss+=')';
//    ss += ';';   ss+="\r\n";
//  }
//  return ss;
//}

////***************************************************************
//// Получить аналитическое выражение в классе CString i-й функции
//// (или при i=-1) всех функций системы Sop) c учетом числа
//// промежуточных переменных при подстановке их выражений (после
//// заключения в скобки) в выражение функции.
////***************************************************************
////-----------------------------------------------------------------------------
//CString CSOP::FormFun (int i/*=-1*/)
//{ int a, b, c, d, g, k, q, n=GetnArg()-m_NumInterVar, m=GetnFun()-m_NumInterVar;
//  ASSERT (m_NumInterVar>=0 && m_NumInterVar<GetnFun() && i<GetnFun());
//  CString ss;
//  char buff[20];
//  CWordArray Pa;
//  Pa.SetSize(m_NumInterVar*4);
//  CTV tvC; CBV bvR;
//  if(i==-1) {q=m; i=0;}
//  else q=i+1;
//  for(i; i<q; i++)
//  {
//    if(i<m)
//#ifndef _MSVC9
//      sprintf(buff,"f%d",i);
//#else
//      sprintf_s(buff,20,"f%d",i);
//#endif
//    else
//#ifndef _MSVC9
//      sprintf(buff, "y%d", i-m);
//#else
//      sprintf_s(buff, 20,"y%d",i-m);
//#endif
//    ss+=buff; ss+="=";
//    b=i; g=0;
//M1: bvR=m_bmFun.GetColumnBv(b);
//    if((d=bvR.CountBit())>1) ss+='(';
//    else if(!d) ss+="[=0]";
//    c=-1; k=-1;
//M2: while((c=bvR.LeftOne(c))!=-1)
//    {
//      if(m_tmCon.IsTrivial(c)) ss+="[=1]";
//      a=0;
//      while((k=m_tmCon.LeftDef(c,k))!=-1)
//      {
//        if(a /*&& k<n*/) ss+='*';
//        a=1;
//        if (m_tmCon.GetBitAt(c,k)=='0') ss+='^';
//        if(k<n) {
//#ifndef _MSVC9
//          sprintf(buff,"x%d",k);
//#else
//          sprintf_s(buff, 20, "x%d",k);
//#endif
//          ss+=buff;
//        }
//        else {
//          g+=4; Pa[g-4]=b; Pa[g-3]=c; Pa[g-2]=k+1; Pa[g-1]=d;
//          b=k-n+m; c=-1; k=-1; goto M1;
//        }
//      }
//      if(bvR.LeftOne(c)!=-1) ss+='+';
//    }
//    if(d>1) ss+=')';
//    if(g)
//    {
//      b=Pa[g-4]; c=Pa[g-3]-1; k=Pa[g-2]-1; d=Pa[g-1]; g-=4;
//      bvR=m_bmFun.GetColumnBv(b);
//      goto M2;
//    }
//    ss+= ';';  ss+="\r\n";
//  }
//  return ss;
//}

////************************************************************
//// Получить аналитическое выражение в классе CStringArray i-го
//// уравнения (или при i=-1 всех уравнений системы Sop) c учетом
//// максимальной длины lengRow строки, а также числа промежуточных
//// переменных при именовании переменных и функций.
////******************************************************************
////-----------------------------------------------------------------------------
//void CSOP::FormEqv (CStringArray &Ass, int i/*=-1*/, int lengRow/*=80*/)
//{ int a, a1=0, c, d, e, k, q,
//      n=GetnArg()-m_NumInterVar, m=GetnFun()-m_NumInterVar;
//  ASSERT (lengRow>10 && m_NumInterVar>=0 &&
//          m_NumInterVar<GetnFun() && i<GetnFun());
//  CString ss, ss0, ss1; char buff[20];
//  CBV bvCol(1,n+m_NumInterVar), bvR;
//  if(i==-1) {q=m+m_NumInterVar; i=0;}
//  else q=i+1;
//  for(i; i<q; i++) {
//    if(i<m)
//#ifndef _MSVC9
//      sprintf(buff,"f%d",i);
//#else
//      sprintf_s(buff, 20, "f%d",i);
//#endif
//    else
//#ifndef _MSVC9
//      sprintf(buff,"y%d",i-m);
//#else
//      sprintf_s(buff, 20, "y%d",i-m);
//#endif
//    ss=buff; ss+="=";
//    bvR=m_bmFun.GetColumnBv(i);
//    if((d=bvR.CountBit())>1) ss+='(';
//    else if(!d) ss+="[=0]";
//    c=-1; k=-1;
//    while((c=bvR.LeftOne(c))!=-1)
//    {
//      if(m_tmCon.IsTrivial(c)) ss+="[=1]";
//      a=0;
//      while((k=m_tmCon.LeftDef(c,k))!=-1)
//      {
//        if(a) ss+='*';
//        a=1;
//        if (m_tmCon.GetBitAt(c,k)=='0') ss+='^';
//        if(k<n)
//#ifndef _MSVC9
//          sprintf(buff,"x%d",k);
//#else
//          sprintf_s(buff,20,"x%d",k);
//#endif
//        else
//#ifndef _MSVC9
//          sprintf(buff,"y%d",k-n);
//#else
//          sprintf_s(buff,20,"y%d",k-n);
//#endif
//        ss+=buff;
//      }
//      if((e=ss.GetLength())>lengRow)
//      {
//        if(a1<=lengRow) {Ass.Add(ss.Left(a1)); e-=a1;}
//        while(e>lengRow)
//        {
//          Ass.Add(ss.Mid(a1,lengRow)); e-=lengRow, a1+=lengRow;
//        }
//        ss1=ss.Right(e); ss=ss0+ss1;
//      }
//      if(bvR.LeftOne(c)!=-1) {a1=ss.GetLength(); ss+='+';}
//    }
//    if(d>1) ss+=')';
//    ss+= ';'; Ass.Add(ss); ss.Empty();
//  }
//}

////***********************************************************
//// Получить аналитическое выражение в классе CStringArray i-й
//// функции (или при i=-1 всех уравнений системы Sop) c учетом
//// максимальной длины lengRow строки и числа промежуточных
//// переменных при подстановке их выражений (после заключения в
//// скобки) в выражение функции.
////***************************************************************
////-----------------------------------------------------------------------------
//void CSOP::FormFun (CStringArray &Ass, int i/*=-1*/, int lengRow/*=80*/)
//{ int a, a1=0, b, c, d, e, g, k, q,
//      n=GetnArg()-m_NumInterVar, m=GetnFun()-m_NumInterVar;
//  ASSERT (lengRow>10 && m_NumInterVar>=0 &&
//          m_NumInterVar<GetnFun() && i<GetnFun());
//  CString ss, ss0, ss1; char buff[20];
//  CWordArray Pa;
//  Pa.SetSize(m_NumInterVar*4);
//  CTV tvC; CBV bvR;
//  if(i==-1) {q=m; i=0;}
//  else q=i+1;
//  for(i; i<q; i++)
//  {
//    if(i<m)
//#ifndef _MSVC9
//      sprintf(buff,"f%d",i);
//#else
//      sprintf_s(buff, 20, "f%d",i);
//#endif
//    else
//#ifndef _MSVC9
//      sprintf(buff,"y%d",i-m);
//#else
//      sprintf_s(buff, 20, "y%d",i-m);
//#endif
//    ss=buff; ss+="=";
//    b=i; g=0;
//M1: bvR=m_bmFun.GetColumnBv(b);
//    if((d=bvR.CountBit())>1) ss+='(';
//    else if(!d) ss+="[=0]";
//    c=-1; k=-1;
//M2: while((c=bvR.LeftOne(c))!=-1)
//    {
//      if(m_tmCon.IsTrivial(c)) ss+="[=1]";
//      a=0;
//      while((k=m_tmCon.LeftDef(c,k))!=-1)
//      {
//        if(a /*&& k<n*/) ss+='*';
//        a=1;
//        if (m_tmCon.GetBitAt(c,k)=='0') ss+='^';
//        if(k<n) {
//#ifndef _MSVC9
//          sprintf(buff,"x%d",k);
//#else
//          sprintf_s(buff,20,"x%d",k);
//#endif
//          ss+=buff;
//        }
//        else
//        {
//          g+=4; Pa[g-4]=b; Pa[g-3]=c; Pa[g-2]=k+1; Pa[g-1]=d;
//          b=k-n+m; c=-1; k=-1; goto M1;
//        }
//      }
//      if((e=ss.GetLength())>lengRow)
//      {
//        if(a1<=lengRow) {Ass.Add(ss.Left(a1)); e-=a1;}
//        while(e>lengRow)
//        {
//          Ass.Add(ss.Mid(a1,lengRow)); e-=lengRow, a1+=lengRow;
//        }
//        ss1=ss.Right(e); ss=ss0+ss1;
//      }
//      if(bvR.LeftOne(c)!=-1) {a1=ss.GetLength(); ss+='+';}
//    }
//    if(d>1) ss+=')';
//    if(g)
//    {
//      b=Pa[g-4]; c=Pa[g-3]-1; k=Pa[g-2]-1; d=Pa[g-1]; g-=4;
//      bvR=m_bmFun.GetColumnBv(b);
//      goto M2;
//    }
//    ss+= ';'; Ass.Add(ss);
//  }
//}
//#else
//******** Creation symbolic representation (equation)*****************

/////////////////////////////////////////////////////////////////////////////
//*****************************************************************
// Получить аналитическое выражение в классе CString i-го уравнения
// (или при i=-1 всех уравнений системы Sop) c учетом числа
// промежуточных переменных при именовании переменных и функций.
//*****************************************************************
//-----------------------------------------------------------------------------
string CSOP::FormEqv (int i/*=-1*/)
{ int  a, c, d, k, q, n=GetnArg()-m_NumInterVar, m=GetnFun()-m_NumInterVar;
ASSERT (m_NumInterVar>=0 && m_NumInterVar<GetnFun() && i<GetnFun());
string ss;  char buff[20];  
CBV bvCol(1,n+m_NumInterVar), bvR;
if(i==-1) {q=m+m_NumInterVar; i=0;}
else q=i+1;
for(i; i<q; i++) {
  if(i<m) 

    sprintf(buff,"f%d",i);
  else 
    sprintf(buff,"y%d",i-m); 
  ss+=buff; ss+="="; 
  bvR=m_bmFun.GetColumnBv(i);
  if((d=bvR.CountBit())>1) ss+='(';
  else if(!d) ss+="[=0]";
  c=-1; 
  while((c=bvR.LeftOne(c))!=-1) {
    if(m_tmCon.IsTrivial(c)) ss+="[=1]";
    k=-1; a=0; 
    while((k=m_tmCon.LeftDef(c,k))!=-1) {
      if(a) ss+='*'; 
      a=1;
      if (m_tmCon.GetBitAt(c,k)=='0') ss+='^';
      if(k<n) 

        sprintf(buff,"x%d",k);
      else 
        sprintf(buff,"y%d",k-n);
      ss+=buff;
    }
    if(bvR.LeftOne(c)!=-1) ss+='+';
  }
  if(d>1) ss+=')'; 
  ss += ';';   
#ifndef _LINUX
  ss+="\r\n";
#else 
  ss+="\n";
#endif
}
return ss;
}

//***************************************************************
// Получить аналитическое выражение в классе CString i-й функции 
// (или при i=-1) всех функций системы Sop) c учетом числа
// промежуточных переменных при подстановке их выражений (после
// заключения в скобки) в выражение функции.
//***************************************************************
//-----------------------------------------------------------------------------
string CSOP::FormFun (int i/*=-1*/)
{ int a, b, c, d, g, k, q, n=GetnArg()-m_NumInterVar, m=GetnFun()-m_NumInterVar;
ASSERT (m_NumInterVar>=0 && m_NumInterVar<GetnFun() && i<GetnFun());
string ss;
char buff[20];  
vector <unsigned short> Pa;
Pa.resize(m_NumInterVar*4,0);
CTV tvC; CBV bvR;
if(i==-1) {q=m; i=0;}
else q=i+1;
for(i; i<q; i++) 
{
  if(i<m) 

    sprintf(buff,"f%d",i);
  else 
    sprintf(buff, "y%d", i-m); 
  ss+=buff; ss+="="; 
  b=i; g=0; 
M1: bvR=m_bmFun.GetColumnBv(b);
  if((d=bvR.CountBit())>1) ss+='(';
  else if(!d) ss+="[=0]";
  c=-1; k=-1; 
M2: while((c=bvR.LeftOne(c))!=-1) 
    {
      if(m_tmCon.IsTrivial(c)) ss+="[=1]";
      a=0; 
      while((k=m_tmCon.LeftDef(c,k))!=-1) 
      {
        if(a /*&& k<n*/) ss+='*'; 
        a=1;
        if (m_tmCon.GetBitAt(c,k)=='0') ss+='^';
        if(k<n) {
          sprintf(buff,"x%d",k); 
          ss+=buff;
        }
        else {
          g+=4; Pa[g-4]=b; Pa[g-3]=c; Pa[g-2]=k+1; Pa[g-1]=d;
          b=k-n+m; c=-1; k=-1; goto M1;
        }
      }
      if(bvR.LeftOne(c)!=-1) ss+='+';
    }
    if(d>1) ss+=')';
    if(g) 
    {
      b=Pa[g-4]; c=Pa[g-3]-1; k=Pa[g-2]-1; d=Pa[g-1]; g-=4;
      bvR=m_bmFun.GetColumnBv(b);
      goto M2;
    }
    ss+= ';';  
#ifdef _LINUX
    ss+="\n"; 
#else
    ss+="\r\n"; 
#endif
}
return ss;
}
//#endif
//************************************************************
// Получить аналитическое выражение в классе CStringArray i-го
// уравнения (или при i=-1 всех уравнений системы Sop) c учетом
// максимальной длины lengRow строки, а также числа промежуточных
// переменных при именовании переменных и функций.
//******************************************************************
//-----------------------------------------------------------------------------
void CSOP::FormEqv (vector <string> &Ass, int i/*=-1*/, int lengRow/*=80*/)
{ int a, a1=0, c, d, e, k, q, 
n=GetnArg()-m_NumInterVar, m=GetnFun()-m_NumInterVar;
ASSERT (lengRow>10 && m_NumInterVar>=0 && 
  m_NumInterVar<GetnFun() && i<GetnFun());
string ss, ss0, ss1; char buff[20];  
CBV bvCol(1,n+m_NumInterVar), bvR;
if(i==-1) {q=m+m_NumInterVar; i=0;}
else q=i+1;
for(i; i<q; i++) {
  if(i<m) 

    sprintf(buff,"f%d",i);
  else 
    sprintf(buff,"y%d",i-m); 
  ss=buff; ss+="=";
  bvR=m_bmFun.GetColumnBv(i);
  if((d=bvR.CountBit())>1) ss+='(';
  else if(!d) ss+="[=0]";
  c=-1; k=-1;
  while((c=bvR.LeftOne(c))!=-1) 
  {
    if(m_tmCon.IsTrivial(c)) ss+="[=1]";
    a=0;
    while((k=m_tmCon.LeftDef(c,k))!=-1) 
    {
      if(a) ss+='*'; 
      a=1;
      if (m_tmCon.GetBitAt(c,k)=='0') ss+='^';
      if(k<n) 
        sprintf(buff,"x%d",k);
      else 
        sprintf(buff,"y%d",k-n);
      ss+=buff;
    }
    if((e=ss.length())>lengRow) 
    {
      if(a1<=lengRow) {Ass.push_back(ss.substr(0,a1)); e-=a1;}
      while(e>lengRow) 
      {
        Ass.push_back(ss.substr(a1,lengRow)); e-=lengRow, a1+=lengRow;
      }
      ss1=ss.substr(ss.length()-e,e); // Right(e); 
      ss=ss0+ss1;
    }
    if(bvR.LeftOne(c)!=-1) {a1=ss.length(); ss+='+';}
  }
  if(d>1) ss+=')'; 
  ss+= ';'; Ass.push_back(ss); ss.clear();
}
}

//***********************************************************
// Получить аналитическое выражение в классе CStringArray i-й
// функции (или при i=-1 всех уравнений системы Sop) c учетом
// максимальной длины lengRow строки и числа промежуточных
// переменных при подстановке их выражений (после заключения в
// скобки) в выражение функции.
//***************************************************************
//-----------------------------------------------------------------------------
void CSOP::FormFun (vector <string> &Ass, int i/*=-1*/, int lengRow/*=80*/)
{ int a, a1=0, b, c, d, e, g, k, q, 
n=GetnArg()-m_NumInterVar, m=GetnFun()-m_NumInterVar;
ASSERT (lengRow>10 && m_NumInterVar>=0 && 
  m_NumInterVar<GetnFun() && i<GetnFun());
string ss, ss0, ss1; char buff[20];  
vector <unsigned short> Pa;
Pa.resize(m_NumInterVar*4,0);
CTV tvC; CBV bvR;
if(i==-1) {q=m; i=0;}
else q=i+1;
for(i; i<q; i++) 
{ 
  if(i<m) 

    sprintf(buff,"f%d",i);
  else 
    sprintf(buff,"y%d",i-m); 
  ss=buff; ss+="="; 
  b=i; g=0; 
M1: bvR=m_bmFun.GetColumnBv(b);
  if((d=bvR.CountBit())>1) ss+='(';
  else if(!d) ss+="[=0]";
  c=-1; k=-1; 
M2: while((c=bvR.LeftOne(c))!=-1) 
    {
      if(m_tmCon.IsTrivial(c)) ss+="[=1]";
      a=0;
      while((k=m_tmCon.LeftDef(c,k))!=-1) 
      {
        if(a /*&& k<n*/) ss+='*'; 
        a=1;
        if (m_tmCon.GetBitAt(c,k)=='0') ss+='^';
        if(k<n) {

          sprintf(buff,"x%d",k);
          ss+=buff;
        }
        else 
        {
          g+=4; Pa[g-4]=b; Pa[g-3]=c; Pa[g-2]=k+1; Pa[g-1]=d;
          b=k-n+m; c=-1; k=-1; goto M1;
        }
      }
      if((e=ss.length())>lengRow)
      {
        if(a1<=lengRow) {Ass.push_back(ss.substr(0,a1)); e-=a1;}
        while(e>lengRow) 
        {
          Ass.push_back(ss.substr(a1,lengRow)); e-=lengRow, a1+=lengRow;
        }
        ss1=ss.substr(ss.length()-e,e); // Right(e); 
        ss=ss0+ss1;
      }
      if(bvR.LeftOne(c)!=-1) {a1=ss.length(); ss+='+';}
    }
    if(d>1) ss+=')';
    if(g) 
    {
      b=Pa[g-4]; c=Pa[g-3]-1; k=Pa[g-2]-1; d=Pa[g-1]; g-=4;
      bvR=m_bmFun.GetColumnBv(b);
      goto M2;
    }
    ss+= ';'; Ass.push_back(ss); 
}
}



/////////////////////////////////////////////////////////////////////
//**************************************************************
// Генерирование системы ДНФ для <nFun> полностью определенных 
// булевых функций от <nArg> аргументов. Каждая из <nCon> конъюнкций
// имеет средний ранг <ranCon> и входит в среднем в <ranFun> ДНФ 
// функций системы.
//    При ranCon>=nArg ранг всех nCon конъюнкций равен  nArg, что 
// соответствует заданию функций системы в совершенной ДНФ,  а при
// ranFun>=nFun все указанные конъюнкции входят во все ДНФ системы.
//**************************************************************

CSOP CSOP::GenSopMid(int nCon, int nArg,int nFun, int ranCon, int ranFun)
{
  m_tmCon.GenRtmMid (nCon, nArg, ranCon);
  m_bmFun.GenRbmMid (nCon, nFun, ranFun);
  return *this;
} 

//**************************************************************
// Генерирование системы ДНФ для <nFun> полностью определенных 
// булевых функций от <nArg> аргументов. Каждая из <nCon> конъюнкций
// имеет средний ранг <ranCon> и входит в среднем в <ranFun> ДНФ 
// функций системы.
//    При ranCon>=nArg ранг всех nCon конъюнкций равен  nArg, что 
// соответствует заданию функций системы в совершенной ДНФ,  а при
// ranFun>=nFun все указанные конъюнкции входят во все ДНФ системы.
//    Ранг любой из конъюнкций больше нуля и каждая из них обращает
// в единицу хотя бы одну функцию системы; в ДНФ любой функции
// есть хотя бы одна конюнкция.
// (Внесена по просьбе пользователей 14.12.01)   
//**************************************************************

CSOP CSOP::GenSopMid_11(int nCon,int nArg,int nFun,int ranCon,int ranFun)
{
  m_tmCon.SetSize(0,nArg);  m_bmFun.SetSize(0,nFun); //21.03.2006
  int i,j; CTV tv; CBV bv, bvD((BYTE)0,nFun);
  for(i=0; i<nCon; i++)
  {
    do { 
      CTV tt = tv.GenRtvMid(nArg, ranCon);
      if (tt.IsTrivial()==0)   break;
    } while(1);

    while(bv.GenRbvMid(nFun, ranFun).IsZero());
    bvD|=bv; AddCon(tv,bv);
  }
  i=-1; bvD=~bvD; 
  while((i=bvD.LeftOne(i))!=-1)
  {
    j= GetRandN()%nCon;   // j=((GetRandN()<<16)|GetRandN())%nCon; 
    m_bmFun.SetBitAt(j,i,1);
  }
  return *this;
} 

//**************************************************************
// Генерирование системы ДНФ для <nFun> полностью определенных 
// булевых функций от <nArg> аргументов. Каждая из <nCon> 
// конъюнкций имеет фиксированный ранг <ranCon> и входит в 
// <ranFun> ДНФ функций системы.
//    При ranCon>=nArg ранг всех nCon конъюнкций равен  nArg, что 
// соответствует заданию функций системы в совершенной ДНФ,  а при
// ranFun>=nFun все указанные конъюнкции входят во все ДНФ системы.
//**************************************************************

CSOP CSOP::GenSopFix(int nCon, int nArg,int nFun, int ranCon, int ranFun)
{
  m_tmCon.GenRtmFix (nCon, nArg, ranCon);
  m_bmFun.GenRbmFix (nCon, nFun, ranFun);
  return *this;
} 

//================================================================
// Генерирование многокаскадной логической сети в формате Sop.
// k - число двухуровневых каскадов, реализующих системы ДНФ). 
// n - число входных полюсов сети,
// m - число выходных полюсов сети,
// p - общее число конъюнкторов,
// e - общее число дизъюнкторов (за вычетом m выходных),
// rn - среднее число входов конъюкторов,
// rf - среднее число выходов конъюнкторов.
// Каждый конъюнктор имеет отличные от нуля числа входов и выходов.
//   После генерирования сеть упрощается, что может привести к тому,
// что числа конъюнкторов и/или каскадов в ней могут оказаться меньше
// заданных p и k, а некоторые из m+e дизъюнкторов реализовывать
// константу "1".
// 21.04.07 
//================================================================

CSOP CSOP::GenLoNet(int k, int n, int m, int p, int e, int rn, int rf)

{ int i, j, xx=e+n, ff=0, pb, pi=p/(k+1), pr=p%(k+1), ee, ei=e/k, er=e%k;
  CBV bvXM(1,n+e), bvFM(1,m+e), bvXX=bvXM<<e, bv, bvX, bvF;
  CTV tv;
  RemoveR(0,GetnCon());
  for(i=0; i<k+1; xx-=(ee=(i<er)?ei+1:ei), ff+=ee, i++)
  {
    bvX=bvXM>>xx | bvXX; bvF=(bvFM<<ff);
    pb=(i>=pr)?pi:pi+1;
    for(j=0; j<pb; j++)
    {
      do {tv=tv.GenRtvMid(n+e, rn); tv&=bvX;}
      while (tv.GetDefs().IsZero()) ;
      do bv=bv.GenRbvMid(m+e, rf) & bvF;
      while (bv.IsZero()) ;
      AddCon(tv, bv);
    }
  }
//CString ZZSop=ToChar();

  SetnInterVar(e);
  CSOP Sop; 
  Sop.DorD(2,*this);
  Sop.SetnInterVar(GetnInterVar());
  *this=Sop;
  return *this;
}

//******************************************************************
// Генерирование  системы логических уравнений,
// представляющих псевдослучайную комбинационную сеть 
//******************************************************************

CSOP CSOP::GenLoNet(int p, int n, int m, int y, int nRn, int nRy)
{
  int a, b, c, e, i, k;
  m_tmCon.SetSize(0,n+y);  m_bmFun.SetSize(0,m+y); //21.03.2006

  CTV tv, tvr, tvX(n, '-'), tvY(y,'-'); 
  CBM Bm;
  CBV bv, bv1, bvB, bvyD((BYTE) 0, y), bvY((BYTE) 0, y), bvRR,
      bvfD((BYTE) 0, m), bvF((BYTE) 0, m), bvC((BYTE) 0, n+y),
      bvy((BYTE) 0, n+y),bvL(1,n+y), bvR(1,y+m), bvx((BYTE)0,n),
      bvXL(bvL<<y), bvYL(bvL>>n), bvYR(bvR>>m), bvFR(bvR<<y),
      bvD((BYTE)0, n+y), bvDC((BYTE)0, p);

  //if(m>y || p<(m+y)) return *this;
  if(m>y) return *this;

  for (i=0; i<m; i++)
  {
    while (!(bv.GenRbvFix(y,1)&bvyD).IsZero()) ;
    bvyD|=bv;     //sbv=bv.BitChar();
    tvr=tvY; tvr.SetOnes(bv);   //stvr=tvr.BitChar();
    tv=tvX; tv.Concat(tvr);     //tv.Concat(tvX, tvr); //stv=tv.BitChar();
    while (!(bvB.GenRbvFix(m,1)&bvfD).IsZero()) ;
    bvfD|=bvB;    //sbv=bvB.BitChar();
    bv.Concat(bvB, bvY);
    AddCon(tv, bv);   //sbv=bv.BitChar();   //sSop=Sop.ToChar(); 
  }

  a=((p-m)/3+((p-m)%3+1)/3); b=((p-m)-2*a);
  c=((nRn*y)/(n+y)+((nRn*y)%(n+y)+1)/(n+y));
  for (i=0; i<a; i++)
  {
    tv=tvX; tv.Concat(tvr.GenRtvFix(y,c));
    bv=bvF; bv.Concat(bvRR.GenRbvFix(y,nRy));
    AddCon(tv, bv);
  }   //sSop=Sop.ToChar(); 

  for (i=0; i<a; i++)
  {
    tv.GenRtvFix(n+y,nRn);
    bv=bvF; bv.Concat(bvRR.GenRbvFix(y,nRy));
    AddCon(tv, bv);
  }   //sSop=Sop.ToChar(); 

  e=((nRn*n)/(n+y)+((nRn*n)%(n+y)+1)/(n+y));
  for (i=0; i<b; i++)
  {
    tv.GenRtvFix(n,e); tv.Concat(tvY);
    bv=bvF; bv.Concat(bvRR.GenRbvFix(y,nRy));
    AddCon(tv, bv);
  }
  bvyD.Zero();
  bvL=bvYL; bv=bvFR; b=0;
  while(!bvL.IsZero())
  {
    bvy.Zero(); //sbvL=bvL.BitChar();
    k=-1; bvDC.Zero();
    while((k=bv.LeftOne(k))!=-1)
      bvDC|=m_bmFun.GetColumnBv(b+k);   //sbvDC=bvDC.BitChar();
    i=-1;  
    while((i=bvDC.LeftOne(i))!=-1)
      bvy|=m_tmCon.GetRowDef(i,bvYL);   //sbvy=bvy.BitChar();
    if((bv=bvy.Extract(n,y)).IsZero()) break;   //sbv=bv.BitChar();
    bvC.Zero();
    for(a=0, c=0; a<Bm.GetCountR(); a++)
    {
      if(!(bvB=Bm.GetRowBv(a,bv)).IsZero())
        if((const BYTE*)Bm[a]<=bv) 
        {
          e=bvB.LeftOne(-1);
          Bm.Zero(a);
          Bm.SetBitAt(a,e,1);
          bv.SetBitAt(e,0);
          bvC.SetBitAt(n+e,1); c=1;
          if(bv.IsZero())  break;   
          //sbv=bv.BitChar(); //sbvB=bvB.BitChar(); //sbvC=bvC.BitChar();
        }
        else Bm.SetRow(a,(const BYTE*)Bm[a]-bvB);
    }
    if(c)
    {
      k=-1;
      while((k=bvDC.LeftOne(k))!=-1)
      {
        tv=m_tmCon.GetRowTv(k);
        tv.SetUnDefs(bvC);    //stv=tv.BitChar();
        m_tmCon.SetRowTv(k,tv);   //sSop=Sop.ToChar();
      }
    }
    if(bv.IsZero()) break;
    Bm.Add(bv); b=m;      //sBm=Bm.BitChar();   //sbv=bv.BitChar();
    bvL.Concat(bvx,bv);   //sbvL=bvL.BitChar();
  }
  SetnInterVar(y);
  CSOP Sop; 
  Sop.DorD(2,*this);
  Sop.SetnInterVar(GetnInterVar());
  *this=Sop;
  return *this;
}

//#ifndef _LINUX
////------------------------------------------Serialize(CArchive& ar)
//void CSOP::Serialize(CArchive& ar)
//{
//  if (ar.IsStoring()) {
//    ar << m_NumInterVar;
//  }
//  else {
//    ar >> m_NumInterVar;
//  }
//  m_tmCon.Serialize(ar);
//  m_bmFun.Serialize(ar);
//}

//#endif

//------------------------------------------Serialize(CArch& ar)
void CSOP::Serialize(CArch& ar)
{
  if (ar.IsStoring()) {
    ar << m_NumInterVar;
  }
  else {
    ar >> m_NumInterVar;
  }
  m_tmCon.Serialize(ar);
  m_bmFun.Serialize(ar);
}


//------------------- SOPtoSF ----------------------------------
/*void CSOP::SOPtoSF(CStringArray& New_SF,CSF* pSF)
{ int i=0, k, l, nOut, nInp, nInt, nsim;
#define maxdl 40000
#define Title "TITLE %s\n"\
"FORMAT SF\n" \
"AUTHOR %s\n" \
"DATE %s\n"\
"PROJECT %s\n"\
"DCL_PIN\n"\
"EXT"
 CString Cur, date, Name;
 CSFVar *pVar;
 CBV InpPAL;
 CTime timeNow = CTime::GetCurrentTime();
 date = timeNow.Format(
  "%d/"   //   Day of month as decimal number (01 – 31)
    "%m/"  //  Number of month
  "%y"  // Year without century, as decimal number
 );
 Cur.Format(Title,pSF->GetTitle(),pSF->GetAuthor(),
     date,pSF->GetProject());
 New_SF.Add(Cur);
 nInp=pSF->GetNumInVar(); nOut=pSF->GetNumOutVar();
 nInt=pSF->GetNumInterVar();
 if((k=GetnArg()-(nInp+nInt)) > 0) {
   for(i=0; i < k; i++) {
     Name.Format("ev%d",i);
   pVar=new CSFVar(Name,'t');
     pSF->AddVar(pVar);
   }
 }
 nInt=pSF->GetNumInterVar();
 Cur="INP\n"; nsim=4; i=0;
// добавляет переменную pVar к массиву входных переменных SF
 while(pSF->GetNextInVar(i, pVar)) {
   if((l=Cur.GetLength())-nsim >= maxdl) { Cur+='\n'; nsim=l; }
   else Cur+=' ';
   Cur+=pVar->GetName();
 }
 New_SF.Add(Cur); Cur="OUT\n"; i=0; nsim=4;
 while(pSF->GetNextOutVar(i, pVar)) {
// добавляет переменную pVar к массиву выходных переменных SF
   if((l=Cur.GetLength())-nsim >= maxdl) { Cur+='\n'; nsim=l; }
   else Cur+=' ';
   Cur+=pVar->GetName();
 }
 New_SF.Add(Cur); Cur="INTER\n"; i=0; nsim=6;
 while(pSF->GetNextInterVar(i, pVar)) {
// добавляет переменную pVar к массиву внутренних переменных SF
   if((l=Cur.GetLength())-nsim >= maxdl) { Cur+='\n'; nsim=l; }
   else Cur+=' ';
   Cur+=pVar->GetName();
 }
 Cur.TrimRight( ); New_SF.Add(Cur);
//форм-е тройки параметров
 Cur.Format("END_PIN\nFUNCTION\nSDF\n%d %d %d",
   nInp+nInt,nOut+nInt,GetnCon()); New_SF.Add(Cur);
//форм-е пары матриц
 Cur=ToChar('1','0','-',FALSE,maxdl);
   Cur.TrimRight(); New_SF.Add(Cur);
   Cur.Format("END_SDF\nEND_FUNCTION\nEND_%s",pSF->GetTitle());
   New_SF.Add(Cur);
}*/

/*CDNF CSOP::ExtractDNF(int nDNF)
{ ASSERT(nDNF < m_bmFun.GetCountC());
 int nRow=-1;
 CDNF tmCon;
 for(int i=0; i < m_bmFun.GetCountR(); i++)
   if(m_bmFun.GetBitAt(i,nDNF))
     tmCon.Add(m_tmCon.GetRowTv(i));
 return tmCon;
}*/
