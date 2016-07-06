///////////////////////////////////////////////////////////////////////////////
// Ver.2.1.2    25.09.2008
//               Поправка для функции построения множества точек 
//               из заданного множества интервалов - CTMtoCBM
//               Поправки в функциях 
//               BOOL CTM::IsCover(const BYTE* mask,int nRow,const CBV& bv) const
//               BOOL CTM::IsCover(int nRow,const CBV& bv) const
// Ver.2.1.1    22.02.2007
//               Поправка для конструкторов по поводу m_nAllocLength
//               Поправка Add - теперь можно всю матрицу при втором 
//               операнде = -1 Теперь правильно будет работать CTMtoCBM
// Ver.2.1.0    07.07.2006
//               Переход к LINUX

// Ver.2.0.9     19.06.2006
//               поправка BitChar - более точный захват памяти под результат
// Ver.2.0.7     21.04.2006
//               поправки в конструкторах матриц для случая создания
//               новых с числом строк =0 и числом столбцов !=0 + контроль этого
// Ver.2.0.6     05.04.2006
//               поправки в BitChar - переставлены параметры 
//               (WithNum, WithClear)
// Ver.2.0.4     25.03.2006
//               поправки в BitChat - добавлены новые параметры 
//               (WithNum, WithClear)
// Ver.2.0.3     24.03.2006
//               поправка операции Serialize
// Ver.2.0.2     21.03.2006
//                Генераторы случайных перестроены на возврат *this
//                FindRow - будет возвращать -1 для пустых матриц
// Ver.2.0.1     18.03.2006
//               - Добавлены функции Торопова из TM_Equiv
// Ver.2.0.0     04.03.2006
//               - Добавлены генераторы случайных
//               - Добавлены функции Торопова из TM_InserS и TM_Degen

// Ver.1.1.1      03.12.2004
// Ver.1.1.0      21.11.2004   Russian == English
///////////////////////////////////////////////////////////////////////////////

#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef _JOINT
#include "BaseBool.h"
#include "BaseTern.h"
#else
#include "../Common/BaseBool.h"
#include "../Common/BaseTern.h"
#endif

#define SIZE_T_MAX  UINT_MAX            /* max size for a size_t */
/*
#ifndef _LINUX
IMPLEMENT_SERIAL(CTM, CObject, 0)
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#define new DEBUG_NEW
#endif //_LINUX
*/

///////////////////////////////////////////////////////////////////////////////
// Генерация псевдослучайной <nRow>*<nCol> троичной матрицы //
// с равномерным распределением значений '-', '0' и '1'     //
// по элементам c вероятностями p(-)=1/2 и p(0)=p(1)=1/4.   //
//////////////////////////////////////////////////////////////

CTM CTM::GenRtm(int nRow, int nCol)
{
  int i;
  CTV tv;
  SetSize (nRow, nCol);
  for (i=0; i<nRow; i++) SetRowTv (i, tv.GenRtv (nCol));
  return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Генерация псевдослучайной троичной <nRow>*<nCol> матрицы   //
// со средним числом <nRang> определенных компонент в строке  //
// (дисперсия 1/16), принимающих значение'0'или'1'c вероятн.1/2 
// (при nRang>=nCol все элементы матрицы имеют определенное значение) 
////////////////////////////////////////////////////////////////

CTM CTM::GenRtmMid (int nRow, int nCol, int nRang)
{
  int i;
  CTV tv;
  SetSize (nRow, nCol);
  for (i=0; i<nRow; i++) SetRowTv (i, tv.GenRtvMid (nCol, nRang));
  return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Генерация псевдослучайной троичной <nRow>*<nCol> матрицы //
// с фиксированным числом <nRang> определенных компонент    // 
// в строке, принимающих знач.'0'или'1'c вероятностью 1/2   
// (при nRang>=nCol все элементы матрицы имеют определенное значение) 
//////////////////////////////////////////////////////////////

CTM CTM::GenRtmFix(int nRow, int nCol, int nRang)
{
  int i;
  CTV tv;
  SetSize (nRow, nCol);
  for (i=0; i<nRow; i++) SetRowTv (i, tv.GenRtvFix(nCol, nRang));
  return *this;
}

//******************************* Constructors\Destructor *********************
//----------------- CTM() -----------------------------------------------------
CTM::CTM() { Init(); }

//----------------- CTM(const CTM& tm, BOOL Fl/* = TRUE */) -------------------
CTM::CTM(const CTM& tm, BOOL Fl/* = TRUE */)
{int i,j;
 if(tm.m_nSize == 0) {
   m_nSize=m_nMaxSize=0; 
   m_nAllocLength=m_nByteLength=tm.m_nByteLength; // 22.02.2007
   m_nBitLength=tm.m_nBitLength; m_nGrowBy = tm.m_nGrowBy; m_pData = NULL;
   return;
 }  
 if(tm.m_nBitLength == 0) { Init(); return; }  // 21.04.2006
 if (Fl)  { //copy tm in our matrix
   AllocMatrix(tm.m_nSize,tm.m_nBitLength);
   for (i=0;i < m_nSize;i++) {
     memcpy(m_pData[i],tm.m_pData[i],m_nByteLength);
     memcpy(m_pData[i]+m_nByteLength,tm.m_pData[i]+tm.m_nAllocLength,
            m_nByteLength);
//  m_nGrowBy = tm.m_nGrowBy;
   }
 }
 else {    //copy tm in our matrix after transponir
   AllocMatrix(tm.m_nBitLength,tm.m_nSize);
   for (j=0;j < m_nBitLength;j++)
     for (i=0;i < m_nSize;i++) SetBitAt(i,j,tm.GetBitAt(j,i));
 }
 m_nGrowBy = tm.m_nGrowBy;
}

//----------------- CTM(const CBM& bm1,const CBM& bm2,BOOL Fl) ----------------
CTM::CTM(const CBM& bm1,const CBM& bm2,BOOL Fl/* = TRUE */)
{int i,j;
 BOOL a1, a0;
 ASSERT(bm1.GetCountR() == bm2.GetCountR() &&
     bm1.GetCountC() == bm2.GetCountC());

 if(bm1.GetCountR() == 0) {                 // 21.04.2006
   m_nSize = m_nMaxSize = 0; 
   m_nAllocLength = m_nByteLength = LEN_BYTE(bm1.GetCountC());
   m_nBitLength = bm1.GetCountC(); m_nGrowBy = 10; m_pData = NULL;
   return;
 }  
 if(bm1.GetCountC() == 0) { Init(); return; }  // 21.04.2006
 if (Fl)  { //copy tm in our matrix
   AllocMatrix(bm1.GetCountR(),bm1.GetCountC());
   for (i=0;i < m_nSize;i++) {
     memcpy(m_pData[i],bm1.GetRow(i),m_nByteLength);
     memcpy(m_pData[i]+m_nByteLength,bm2.GetRow(i),m_nByteLength);
   }
 }
 else {    //copy tm in our matrix after transponir
   AllocMatrix(bm1.GetCountR(),bm1.GetCountC());
   for (j=0;j < m_nBitLength;j++)
     for (i=0;i < m_nSize;i++) {
       a1=bm1.GetBitAt(j,i); a0=bm2.GetBitAt(j,i);
       if(a1 && !a0) SetBitAt(i,j,'1');
       if(!a1 && a0) SetBitAt(i,j,'0');
       if(a1 && a0) SetBitAt(i,j,'+');
     }  
 } 
   
  m_nGrowBy = bm1.m_nGrowBy;  
}

//----------------- CTM(int nRow,int nColumn,char symb='-') -------------------
CTM::CTM(int nRow, int nColumn,char symb/*='-'*/)
{  ASSERT(nRow >= 0 && nColumn >= 0);
 int i,nByte = LEN_BYTE(nColumn),  w = S_1-ADR_BIT(nColumn);
  if (nRow == 0) {                            //01.02.2002
    if ( nColumn == 0) Init();
    else {
      m_nSize=m_nMaxSize=0; m_nAllocLength=m_nByteLength=nByte; // 22.02.2007
      m_nBitLength=nColumn; m_nGrowBy = 10; m_pData = NULL;
    }
    return;
  }
  if ( nColumn == 0) { Init(); return; }    // 21.04.2006
  AllocMatrix(nRow,nColumn);
  if(symb == '1' || symb == '+') 
    for (i=0; i < nRow; i++) {          ///////изменения 17.12.99г.
      memset(m_pData[i],0xFF,nByte);/* one's fill*/
      if (w != S_1)  m_pData[i][nByte-1] = (m_pData[i][nByte-1]>>w)<<w;
    }
  if(symb == '0' || symb == '+') 
    for (i=0; i < nRow; i++) {
      memset(m_pData[i]+nByte,0xFF,nByte);/* zero's fill*/  
      if (w != S_1) 
        m_pData[i][(nByte<<1)-1] = (m_pData[i][(nByte<<1)-1]>>w)<<w;
    }
  m_nGrowBy = 10;  
}
/*
#ifndef _LINUX
//----------------- CTM(const char* pch) --------------------------------------
CTM::CTM(const char* pch)
{
  CString s(pch);
  Init();
  if (s.GetLength()) CharBit(s);
}

//----------------- CTM(CStringArray StrAr) -----------------------------------
CTM::CTM(const CStringArray& StrAr)
{ Init(); CharBit(StrAr); }
#else
*/
CTM::CTM(const char* pch)
{
  char * s;
  Init();
  s = strdup(pch);

  if (strlen(s)>0) CharBit(s);
  free(s);
}

//#endif //_LINUX


//----------------- ~CTM() ----------------------------------------------------
CTM::~CTM()
{ 
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  SafeDelete(); }

///////////////////////////////////////////////////////////////////////////////
//***************** Working with memory ***************************************
//----------------- SetSize ---------------------------------------------------
void CTM::SetSize(int nRow,int nColumn,int nGrowBy/*=-1*/,int nColGrow/*=0*/)
{ int i,nByte, nmaxRow, nBytemax, w;
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(nRow >= 0 && nColumn >= 0);
#endif
 if (nGrowBy != -1) m_nGrowBy = nGrowBy;  // set new size
 nmaxRow=nRow+m_nGrowBy; nBytemax=LEN_BYTE(nColumn+nColGrow);
 nByte = LEN_BYTE(nColumn);
 if (nRow == 0) { 
   SafeDelete(); m_nBitLength=nColumn;       //Init();
   m_nSize=m_nMaxSize=0;   m_nByteLength=nByte;
   m_nAllocLength=nBytemax;
   return;
 }
 //------------------ create one with exact size
 if (m_pData == NULL) { // create one with exact size
#ifdef SIZE_T_MAX
   ASSERT((long)nmaxRow * sizeof(BYTE*) <= SIZE_T_MAX); // no overflow
#endif
  m_pData = (BYTE**) new BYTE[nmaxRow * sizeof(BYTE*)];
  for (i=0; i < nRow; i++) {
    m_pData[i] = (BYTE*) new BYTE[nBytemax<<1];
    memset(m_pData[i],0,nBytemax<<1);    }
  m_nSize = nRow; m_nMaxSize =nmaxRow;
  m_nBitLength = nColumn; m_nAllocLength =nBytemax;
  m_nByteLength=nByte;
  return;
 }
 //------------------ Set columns size
 if (nByte > m_nAllocLength) {
   BYTE * ptr;
   for (i=0; i < m_nSize; i++) {
     ptr= (BYTE*) new BYTE[nBytemax<<1];
     memset(ptr, 0, nBytemax<<1);
     if (m_pData[i]!=NULL) {
       memcpy(ptr, m_pData[i], m_nByteLength);   // copy new data from the old
       memcpy(ptr+nBytemax, m_pData[i]+m_nAllocLength,m_nByteLength);
       delete [] (BYTE*)m_pData[i]; 
     }
     m_pData[i] = ptr;
   }
  m_nAllocLength=nBytemax;
 }
 else {               //when(nByte < m_nAllocLength)  new 022.12.99!!!!!!!!!!
   int nost;
   if (nColumn < m_nBitLength) {
   w = S_1-ADR_BIT(nColumn);  nost=nByte;   }
   else   {
   w = S_1-ADR_BIT(m_nBitLength); nost=m_nByteLength;   }
   if (w==8) w = 0;                     //new 02.02.99!!!!!!!!!!
     for (i=0; i < m_nSize; i++) {
       memset(m_pData[i]+nost,0,(m_nAllocLength-nost));
                                            //new 22.12.99 m_nByteLength-nByte
       memset(m_pData[i]+m_nAllocLength+nost,0,(m_nAllocLength-nost));
       m_pData[i][nost-1] = (m_pData[i][nost-1]>>w)<<w;    //nByte
       m_pData[i][m_nAllocLength+nost-1] = 
                            (m_pData[i][m_nAllocLength+nost-1]>>w)<<w; //nByte
     }
  }
 m_nBitLength = nColumn; m_nByteLength = nByte;
//------------------ Set rows size
 if (nRow <= m_nMaxSize) {
   if (nRow > m_nSize) { 
     for (i=m_nSize; i < nRow; i++) {
       m_pData[i] = (BYTE*) new BYTE[m_nAllocLength<<1];
       memset(m_pData[i],0,m_nAllocLength<<1);     }
   }
   else 
     for (i=m_nSize-1; i >= nRow; i--) 
       if (m_pData[i]!=NULL) {
       delete [] (BYTE*)m_pData[i]; m_pData[i] = NULL; }
   m_nSize = nRow;
   return;
 }
 //-------------- Otherwise grow array
                     //new 22.12.99
#ifdef SIZE_T_MAX
 ASSERT((long)nmaxRow * sizeof(BYTE*) <= SIZE_T_MAX);  // no overflow
#endif
 BYTE** pNewData = (BYTE**) new BYTE[(nmaxRow * sizeof(BYTE*))<<1];
 memcpy(pNewData, m_pData, m_nSize * sizeof(BYTE*)); // copy new data from old
 for (i=m_nSize; i < nRow; i++) {
   pNewData[i] = (BYTE*) new BYTE[m_nAllocLength<<1];
   memset(pNewData[i], 0,m_nAllocLength<<1);
 }
 for (i=nRow; i < nmaxRow; i++) pNewData[i] = NULL;
 delete [] (BYTE*)m_pData;  m_pData = pNewData;
 m_nSize = nRow;  m_nMaxSize = nmaxRow;
}

//----------------- FreeExtra -------------------------------------------------
void CTM::FreeExtra()
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  int i;
  BYTE * ptr;
  if (m_nByteLength < m_nAllocLength) { // Free row memory
    for (i=0; i < m_nSize; i++) {
      ptr= (BYTE*) new BYTE[m_nByteLength<<1];
      if (m_pData[i]!=NULL) {
        memcpy(ptr, m_pData[i], m_nByteLength); // copy new data from old
        memcpy(ptr+m_nByteLength, m_pData[i]+m_nAllocLength, m_nByteLength);
        delete [] (BYTE*)m_pData[i];
      }    
      m_pData[i] = ptr;
   }
   m_nAllocLength = m_nByteLength;
  }
  if (m_nSize != m_nMaxSize) { // shrink to desired size
#ifdef SIZE_T_MAX
    ASSERT((long)m_nSize * sizeof(BYTE*) <= SIZE_T_MAX);  // no overflow
#endif
    BYTE** pNewData = NULL;
    if (m_nSize != 0) {         // copy new data from old
      pNewData = (BYTE**) new BYTE[m_nSize * sizeof(BYTE*)];
      memcpy(pNewData, m_pData, m_nSize * sizeof(BYTE*)); 
    }
    // get rid of old stuff (note: no destructors called)
    delete [] (BYTE*)m_pData;
    m_pData = pNewData;
    m_nMaxSize = m_nSize;
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////// Data Reading ////////////////////////////////////////////
//----------------- GetBitAt --------------------------------------------------
char CTM::GetBitAt(int nRow,int nColumn) const
{ ASSERT(nColumn >= 0 && nColumn < m_nBitLength && nRow >= 0 && nRow < m_nSize);
 int a1, a0;
 a1=m_pData[nRow][BIT_BYTE(nColumn)] & OB[ADR_BIT(nColumn)];
 a0=m_pData[nRow][m_nAllocLength+BIT_BYTE(nColumn)] & OB[ADR_BIT(nColumn)];
 if(a1 != 0 && a0 == 0)  return '1';
 if(a0 != 0 && a1 == 0)  return '0';
 if(a0 == 0 && a1 == 0)  return '-';
 else return '+';
}

//----------------- GetRowTv --------------------------------------------------
CTV CTM::GetRowTv(int nRow) const
{ ASSERT(nRow >= 0 && nRow < m_nSize);
 CTV tv(m_pData[nRow],m_pData[nRow]+m_nAllocLength,m_nBitLength);
 return tv;
}
 
//----------------- GetRowTv --------------------------------------------------
CTV CTM::GetRowTv(int nRow,BYTE* maska) const
{ ASSERT(nRow >= 0 && nRow < m_nSize);
  CTV tv(GetRowTv(nRow));
  tv&=maska;
  return tv;
}

//----------------- GetRowTv --------------------------------------------------
CTV CTM::GetRowTv(int nRow,CBV& maska) const
{ ASSERT(nRow >= 0 && nRow < m_nSize && m_nBitLength == maska.GetBitLength());
  CTV tv(GetRowTv(nRow));
  tv&=maska;
  return tv;
}

//----------------- GetColumnTv -----------------------------------------------
CTV CTM::GetColumnTv(int nColumn) const
{ASSERT(nColumn >= 0 && nColumn < m_nBitLength);
 CTV tv(m_nSize);
 int i;
 for (i=0;i < m_nSize;i++) tv.SetBitAt(i,GetBitAt(i,nColumn));
 return tv;
}

//----------------- GetColumnTv -----------------------------------------------
CTV CTM::GetColumnTv(int nColumn,BYTE* maska) const
{ASSERT(nColumn >= 0 && nColumn < m_nBitLength);
 CTV tv(m_nSize);
 int i;
 for (i=0;i < m_nSize;i++) tv.SetBitAt(i,GetBitAt(i,nColumn));
 tv&=maska;
 return tv;
}  

//----------------- GetColumnTv -----------------------------------------------
CTV CTM::GetColumnTv(int nColumn,CBV& maska) const
{
  ASSERT(nColumn >= 0 && nColumn < m_nBitLength && 
         m_nSize == maska.GetBitLength());
  CTV tv(m_nSize);
  int i;
  for (i=0;i < m_nSize;i++) tv.SetBitAt(i,GetBitAt(i,nColumn));
  tv&=maska;
  return tv;
}
/*
#ifndef _LINUX
//----------------- BitChar ---------------------------------------------------
CString CTM::BitChar(char One,char Zero,char Dont,char Rest,BOOL WithNum)
{
  int i,j,pos = 0;
  CString ch, s;
  if(WithNum) {
    if (m_nSize>9999)     { ch = "%5d: "; i=9; }
    else if (m_nSize>999) { ch = "%4d: "; i=8; }
    else if (m_nSize>99)  { ch = "%3d: "; i=7; }
    else                  { ch = "%2d: "; i=6; }
  }
  else i=2;
  CString res('\0',(m_nBitLength+i) * m_nSize);
  for (j=0;j<m_nSize;j++) {
    if(WithNum) {
      s.Format((const char*)ch,j);
      for (i=0;i<m_nSize;i++) res.SetAt(pos++,s[i]);
    }
  for (i=0;i<m_nBitLength;i++)
    switch (GetBitAt(j,i)) {
      case '1': res.SetAt(pos++,One); break;
      case '0': res.SetAt(pos++,Zero); break;
      case '-': res.SetAt(pos++,Dont); break;
      default:  res.SetAt(pos++,Rest);
    }
    res.SetAt(pos++,'\n'); //res.SetAt(pos++,'\r'); 
  }
  return res;
}

//------------------------------------------------- BitChar(char One,char Zero)
void CTM::BitChar(CStringArray & StrRes, BOOL WithClear, BOOL WithNum, 
                  char One, char Zero, char Dont, char Rest)
{ int i,j;
  if (WithClear) StrRes.RemoveAll();
  CString ch,s;
  if (m_nSize>9999)     { ch = "%5d: ";}
  else if (m_nSize>999) { ch = "%4d: ";}
  else if (m_nSize>99)  { ch = "%3d: ";}
  else                  { ch = "%2d: ";}

  for (j=0;j<m_nSize;j++) {
    if(WithNum) s.Format((const char*)ch,j);
    else        s.Empty();
    for (i=0;i<m_nBitLength;i++)
      switch (GetBitAt(j,i)) {
        case '1': s+=One; break;
        case '0': s+=Zero; break;
        case '-': s+=Dont; break;
        default:  s+=Rest;
      }  
    StrRes.Add(s);
  }
}
#else
*/
//------------------------------------------------- BitChar(char One,char Zero)
void CTM::BitChar(vector<string>& StrRes, BOOL WithClear, BOOL WithNum, 
  char One, char Zero, char Dont, char Rest)
{ int i,j;
if (WithClear) StrRes.clear();
string ch,s;
char bbf[20];
if (m_nSize>9999)     { ch = "%5d: ";}
else if (m_nSize>999) { ch = "%4d: ";}
else if (m_nSize>99)  { ch = "%3d: ";}
else                  { ch = "%2d: ";}

for (j=0;j<m_nSize;j++) {
  if(WithNum) {
    sprintf(bbf,(const char*)ch.c_str(),j);
    s = bbf;
  }
  else        s.clear();
  for (i=0;i<m_nBitLength;i++)
    switch (GetBitAt(j,i)) {
    case '1': s+=One; break;
    case '0': s+=Zero; break;
    case '-': s+=Dont; break;
    default:  s+=Rest;
  }  
  StrRes.push_back(s);
}
}

char* CTM::BitChar(char One,char Zero,char Dont,char Rest,BOOL WithNum)
{
  int i,j,pos = 0;
  char ch[20], s[20];
  if(WithNum) {
    if (m_nSize>9999)     { strcpy(ch,"%5d: "); i=9; }
    else if (m_nSize>999) { strcpy(ch,"%4d: "); i=8; }
    else if (m_nSize>99)  { strcpy(ch,"%3d: "); i=7; }
    else                  { strcpy(ch,"%2d: "); i=6; }
  }
  else i=2;
  char* res;
  res = (char*)malloc((m_nBitLength+i) * m_nSize);
  memset(res,0,(m_nBitLength+i) * m_nSize);

  for (j=0;j<m_nSize;j++) {
    if(WithNum) {
      sprintf(s,(const char*)ch,j);
      for (i=0;i<strlen(s);i++) 
        res[pos++] = s[i];
    }
    for (i=0; i<m_nBitLength; i++)
      switch (GetBitAt(j,i)) {
      case '1': res[pos++] = One; break;
      case '0': res[pos++] = Zero; break;
      case '-': res[pos++] = Dont; break;
      default:  res[pos++] = Rest;
    }
    res[pos++] = '\n';  //res[pos++] = '\r'; 
  }
  return res;
}


//#endif //_LINUX

//--------------------------- GetUnDefs ---------------------------------------
CBM CTM::GetUnDefs() const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int i,j;
 BYTE maska;
 CBM bmTag(m_nSize,m_nBitLength);
 maska=((j=ADR_BIT(m_nBitLength)) == 0)?0xff:(~(0xff>>j));
 for (i=0; i < m_nSize; i++) {
   for (j=0; j < m_nByteLength-1; j++) 
     bmTag.SetByteAt(i,j,~(m_pData[i][j]|m_pData[i][m_nAllocLength+j]));
   bmTag.SetByteAt(i,j,(~(m_pData[i][j]|m_pData[i][m_nAllocLength+j]))&maska);
 }
 return  bmTag;
}   

//--------------------------- GetDefs -----------------------------------------
CBM CTM::GetDefs() const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int i,j;
 CBM bmTag(m_nSize,m_nBitLength);
 for (i=0; i < m_nSize; i++) 
   for (j=0; j < m_nByteLength; j++) 
     bmTag.SetByteAt(i,j,m_pData[i][j]|m_pData[i][m_nAllocLength+j]);
 return  bmTag;
}   

//--------------------------- GetOnes -----------------------------------------
CBM CTM::GetOnes() const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int i,j;
 CBM bmTag(m_nSize,m_nBitLength);
 for (i=0; i < m_nSize; i++) 
   for (j=0; j < m_nByteLength; j++) 
     bmTag.SetByteAt(i,j,m_pData[i][j]);
 return  bmTag;
}   

//--------------------------- GetZeros ----------------------------------------
CBM CTM::GetZeros() const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int i,j;
 CBM bmTag(m_nSize,m_nBitLength);
 for (i=0; i < m_nSize; i++) 
   for (j=0; j < m_nByteLength; j++) 
     bmTag.SetByteAt(i,j,m_pData[i][m_nAllocLength+j]);
 return  bmTag;
}   

//--------------------------- GetRowUnDef -------------------------------------
CBV CTM::GetRowUnDef(int nRow) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(m_nSize >= nRow);
#endif
 int i;
 BYTE maska;
 maska=((i=ADR_BIT(m_nBitLength)) == 0)?0xff:(~(0xff>>i));
 CBV bvTag(0,m_nBitLength,TRUE);
 for (i=0; i < m_nByteLength-1; i++) 
   bvTag.SetByteAt(i,~(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]));
 bvTag.SetByteAt(i,(~(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]))&maska);
 return  bvTag;
}   

//--------------------------- GetRowUnDef -------------------------------------
CBV CTM::GetRowUnDef(int nRow,BYTE* maska) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(m_nSize >= nRow && nRow >= 0);
#endif
 int i;
 CBV bvTag(0,m_nBitLength,TRUE);
 for (i=0; i < m_nByteLength-1; i++) 
   bvTag.SetByteAt(i,(~(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]))&
                        maska[i]);
 return  bvTag;
} 

//--------------------------- GetRowDef ---------------------------------------
CBV CTM::GetRowDef(int nRow) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(m_nSize >= nRow);
#endif
 int i;
 CBV bvTag(0,m_nBitLength,TRUE);
 for (i=0; i < m_nByteLength; i++) 
   bvTag.SetByteAt(i,m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]);
 return  bvTag;
}   

//--------------------------- GetRowDef ---------------------------------------
CBV CTM::GetRowDef(int nRow,BYTE* maska) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(m_nSize >= nRow);
#endif
 int i;
 CBV bvTag(0,m_nBitLength,TRUE);
 for (i=0; i < m_nByteLength; i++) 
   bvTag.SetByteAt(i,(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i])&
                      maska[i]);
 return  bvTag;
}   

//--------------------------- GetRowOne ---------------------------------------
CBV CTM::GetRowOne(int nRow) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(m_nSize >= nRow);
#endif
 int i;
 CBV bvTag(0,m_nBitLength,TRUE);
 for (i=0; i < m_nByteLength; i++) 
   bvTag.SetByteAt(i,m_pData[nRow][i]);
 return  bvTag;
}   

//--------------------------- GetRowOne ---------------------------------------
CBV CTM::GetRowOne(int nRow,BYTE* maska) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(m_nSize >= nRow);
#endif
 int i;
 CBV bvTag(0,m_nBitLength,TRUE);
 for (i=0; i < m_nByteLength; i++) 
   bvTag.SetByteAt(i,(m_pData[nRow][i])&maska[i]);
 return  bvTag;
}   

//--------------------------- GetRowZero --------------------------------------
CBV CTM::GetRowZero(int nRow) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(m_nSize >= nRow);
#endif
 int i;
 CBV bvTag(0,m_nBitLength,TRUE);
 for (i=0; i < m_nByteLength; i++) 
   bvTag.SetByteAt(i,m_pData[nRow][m_nAllocLength+i]);
 return  bvTag;
}   

//--------------------------- GetRowZero --------------------------------------
CBV CTM::GetRowZero(int nRow,BYTE* maska) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(m_nSize >= nRow);
#endif
 int i;
 CBV bvTag(0,m_nBitLength,TRUE);
 for (i=0; i < m_nByteLength; i++) 
   bvTag.SetByteAt(i,(m_pData[nRow][m_nAllocLength+i])&maska[i]);
 return  bvTag;
}   

void CTM::Param(int* aPar,int nMaxLen)
// return the number of long rows - rows with weght more than nMaxLen,
// the average rows weght and the average long rows weght
{int nAll=0, nRang, nL, nLar=0, nLarAll=0;
 aPar[0]=0;
 for(nL=0; nL < m_nSize; nL++) {
   nRang=CountDefs(nL);
   nAll+=nRang;
   if(nRang > nMaxLen) {
   aPar[0]++; nLarAll+=nRang;
   }
 }
 aPar[1]=nAll/m_nSize;
 aPar[2]=nLarAll/aPar[0];
}

 
///////////////////////////////////////////////////////////////////////////////
//***************** Select of matrix ******************************************
//----------------- ExtractRows -----------------------------------------------
CTM CTM::ExtractRows(int nFirst, int nCount) const
{ASSERT(nFirst >= 0 && (nFirst + nCount) <= m_nSize && nCount > 0);
 CTM tm(nCount,m_nBitLength);
 int i;
 for (i=0; i < nCount; i++) tm.SetRowTv(i,GetRowTv(nFirst + i));
 return tm;
}

//----------------- ExtractColumns --------------------------------------------
CTM CTM::ExtractColumns(int nFirst, int nCount) const
{ASSERT(nFirst >= 0 && (nFirst + nCount) <=m_nBitLength && nCount > 0);
 CTM tm(m_nSize,nCount);
 int j,i,k,l_bit,r_bit,nSrcByte,AdrBeg;
 BYTE ch;
 k = S_1 - ADR_BIT(nCount);
 ch=(0xff >> k) << k;
 AdrBeg = BIT_BYTE(nFirst);
 l_bit = ADR_BIT(nFirst);
 r_bit = S_1 - l_bit;
 nSrcByte = BIT_BYTE(nFirst + nCount);
 for (j=0;j < m_nSize;j++)  {
   if (AdrBeg - nSrcByte)  {
     for (i=AdrBeg; i <= nSrcByte; i++)  {
       tm.m_pData[j][i-AdrBeg]=
                      ( m_pData[j][i]<<l_bit) | (m_pData[j][i+1]>>r_bit);
       tm.m_pData[j][i-AdrBeg+tm.m_nAllocLength]=
                      ( m_pData[j][i+m_nAllocLength]<<l_bit)|
                      (m_pData[j][i+1+m_nAllocLength]>>r_bit);       
     }
     if (k != S_1) {
       tm.m_pData[j][nSrcByte-AdrBeg] &= ch;     
       tm.m_pData[j][nSrcByte-AdrBeg+tm.m_nAllocLength] &= ch;
     }  
   }
   else  {
     tm.m_pData[j][0]=
                (m_pData[j][AdrBeg]<<l_bit) | (m_pData[j][AdrBeg+1]>>r_bit);
     tm.m_pData[j][tm.m_nAllocLength]=
                        (m_pData[j][AdrBeg+m_nAllocLength]<<l_bit) |
                        (m_pData[j][AdrBeg+1+m_nAllocLength]>>r_bit);
     if (k != S_1) {
       tm.m_pData[j][0] &= ch;
       tm.m_pData[j][tm.m_nAllocLength] &= ch;
     }  
   }
 }
 return tm;
}

//----------------- Extract ---------------------------------------------------
CTM CTM::Extract(int nFirstRow, int nFirstColumn, 
                 int nCountRow, int nCountColumn) const
{
  ASSERT(nFirstRow >= 0 && nFirstColumn >= 0 && 
         nCountColumn > 0 && nCountRow > 0);
 ASSERT(((nFirstColumn + nCountColumn) <=m_nBitLength) && 
         (nFirstRow + nCountRow <=m_nSize));
 CTM tm(nCountRow,nCountColumn);
 int j,i,k,l_bit,r_bit,nSrcByte,AdrBeg;
 BYTE ch;
 k = S_1 - ADR_BIT(nCountColumn);
 ch=(0xff >> k) << k;
 AdrBeg = BIT_BYTE(nFirstColumn);
 l_bit = ADR_BIT(nFirstColumn);
 r_bit = S_1 - l_bit;
 nSrcByte = BIT_BYTE(nFirstColumn + nCountColumn);
 for (j=nFirstRow;j < nFirstRow+nCountRow;j++)  {
   if (AdrBeg - nSrcByte)  {
     for (i=AdrBeg; i <= nSrcByte; i++)  {
       tm.m_pData[j-nFirstRow][i-AdrBeg] = (m_pData[j][i] << l_bit) |
                                           (m_pData[j][i+1] >> r_bit);
       tm.m_pData[j-nFirstRow][i-AdrBeg+tm.m_nAllocLength] = 
                              (m_pData[j][i+m_nAllocLength] << l_bit)|
                              (m_pData[j][i+1+m_nAllocLength] >> r_bit);
     }
     if (k != S_1) {
       tm.m_pData[j-nFirstRow][nSrcByte-AdrBeg] &= ch;
       tm.m_pData[j-nFirstRow][nSrcByte-AdrBeg+tm.m_nAllocLength] &= ch;
     }  
   }
   else  {
     tm.m_pData[j-nFirstRow][0]=
       (m_pData[j][AdrBeg]<<l_bit)|(m_pData[j][AdrBeg+1]>>r_bit);
     tm.m_pData[j-nFirstRow][tm.m_nAllocLength]=
                        ((m_pData[j][AdrBeg]<<l_bit)+m_nAllocLength)|  
                        (m_pData[j][AdrBeg+1+m_nAllocLength]>>r_bit);
     if (k != S_1) {
       tm.m_pData[j-nFirstRow][0] &= ch;
       tm.m_pData[j-nFirstRow][tm.m_nAllocLength] &= ch;
     }  
   }
  }
 return tm;
}

//----------------- ExtractColumns --------------------------------------------
CTM CTM::ExtractColumns(const CBV& Columns) const
{ASSERT(Columns.GetBitLength() > 0);
 ASSERT(m_nBitLength >= Columns.GetBitLength());
 int i, j, LeftOneColumn;
 int NewColumnLen = Columns.CountBit();
 CTM tm(m_nSize,NewColumnLen);
 for(i=0; i < m_nSize; i++)  {
   LeftOneColumn = -1;
   for (j=0; j < NewColumnLen; j++)  {
     LeftOneColumn = Columns.LeftOne(LeftOneColumn);
     tm.SetBitAt(i,j,GetBitAt(i,LeftOneColumn));
   }
 }
 return tm;
}

//----------------- Minor -----------------------------------------------------
CTM CTM::Minor(const CBV& Rows, const CBV& Columns) const
{
  ASSERT(Columns.GetBitLength() > 0 && Rows.GetBitLength() >= 0);
  ASSERT(m_nBitLength >= Columns.GetBitLength() && 
         m_nSize >= Rows.GetBitLength());
 int i,j,LeftOneRow = -1,LeftOneColumn = -1;
 int  NewRowLen,NewColumnLen;
 NewRowLen = Rows.CountBit();
 NewColumnLen = Columns.CountBit();
 CTM tm(NewRowLen,NewColumnLen);
 for (i=0;i < NewRowLen;i++)  {
   LeftOneRow = Rows.LeftOne(LeftOneRow);
   for (j=0;j < NewColumnLen;j++)  {
     LeftOneColumn = Columns.LeftOne(LeftOneColumn);
     tm.SetBitAt(i,j,GetBitAt(LeftOneRow,LeftOneColumn));
   }
   LeftOneColumn = -1;
 }
 return tm;
}

//----------------- Trans -----------------------------------------------------
CTM CTM::Trans() const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 CTM tm(*this,FALSE);
 return tm;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////// Data Writing //////////////////////////////////////////////
//----------------- SetBitAt --------------------------------------------------
void CTM::SetBitAt(int nRow,int nColumn, char symb/*='-'*/)
{ASSERT(nRow >= 0 && nRow < m_nSize); 
 ASSERT(nColumn >= 0 && nColumn <= m_nBitLength);
 int i=BIT_BYTE(nColumn), j=ADR_BIT(nColumn);
 switch (symb) {
     case '1': m_pData[nRow][i] |=OB[j]; m_pData[nRow][m_nAllocLength+i]&=~OB[j]; break;
     case '0': m_pData[nRow][m_nAllocLength+i] |=OB[j]; m_pData[nRow][i]&=~OB[j]; break;
     case '-': m_pData[nRow][i]&=~OB[j]; m_pData[nRow][m_nAllocLength+i]&=~OB[j]; break;
     default:  m_pData[nRow][i] |=OB[j]; m_pData[nRow][m_nAllocLength+i] |=OB[j];
    } 
}

//----------------- SetRowTv --------------------------------------------------
void CTM::SetRowTv(int nRow, const CTV& tv)
{ ASSERT(nRow >= 0 && nRow < m_nSize && m_nBitLength == tv.GetBitLength());
  memcpy(m_pData[nRow],(const BYTE *)tv.GetOnes(),tv.GetByteLength());
  memcpy(&m_pData[nRow][m_nAllocLength],(const BYTE *)tv.GetZeros(),
    tv.GetByteLength());
}

//----------------- SetRowTv --------------------------------------------------
void CTM::SetRowTv(int nRow, const CTM& tm, int ntmRow)
{ ASSERT(nRow >= 0 && nRow < m_nSize && m_nBitLength == tm.GetCountC());
  memcpy(m_pData[nRow],tm.m_pData[ntmRow],m_nByteLength);
  memcpy(&m_pData[nRow][m_nAllocLength],
         &tm.m_pData[ntmRow][m_nAllocLength],m_nByteLength);
}

//--------------------------- SetPartAt ---------------------------------------
void CTM::SetPartAt(BOOL Part,const CBM& bm)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(m_nBitLength == bm.GetCountC() && m_nSize == bm.GetCountR());
#endif
 int i, j;
 if(Part)
   for (i=0;i < m_nSize; i++) 
     for (j=0; j < m_nByteLength; j++) 
       m_pData[i][j]=bm.GetByteAt(i,j);
 else 
   for (i=0;i < m_nSize; i++) 
     for (j=0; j < m_nByteLength; j++) 
       m_pData[i][m_nAllocLength+j]=bm.GetByteAt(i,j);
 return;
}   

//--------------------------- SetPartAt ---------------------------------------
void CTM::SetPartAt(BOOL Part,const CBV& bv,int Row)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(m_nBitLength == bv.GetBitLength() && m_nSize >= Row);
#endif
 int i;
 if(Part)
   for (i=0; i < m_nByteLength; i++) 
     m_pData[Row][i]=bv.GetByteAt(i);
 else 
   for (i=0; i < m_nByteLength; i++) 
     m_pData[Row][m_nAllocLength+i]=bv.GetByteAt(i);
 return;
}   

//--------------------------- SetRowZero --------------------------------------
void CTM::SetRowZero(int nRow,const BYTE* newRow)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 for (int i=0; i < m_nByteLength; i++) 
     m_pData[nRow][m_nAllocLength+i]=newRow[i];
 return;
}   

//--------------------------- SetRowOne ---------------------------------------
void CTM::SetRowOne(int nRow,const BYTE* newRow)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 for (int i=0; i < m_nByteLength; i++) 
   m_pData[nRow][i]=newRow[i];
 return;
}   
///////////////////////////////////////////////////////////////////////////////
//***************** Working with rows *****************************************
//----------------- Add -------------------------------------------------------
int CTM::Add(char symb/*='-'*/,int nCount/*=1*/)
{int i, first=m_nSize;
 if(m_nSize + nCount < m_nAllocLength) {
   m_nSize+=nCount;
   for(i=first; i < m_nSize; i++) {
     m_pData[i] = (BYTE*) new BYTE[m_nAllocLength<<1];
     memset(m_pData[i],0,m_nAllocLength<<1);
   }
 }
 else 
   SetSize(m_nSize+nCount,m_nBitLength,
           m_nGrowBy,m_nAllocLength*S_1-m_nBitLength);
 if(symb != '-') {
   BYTE maska=((i = ADR_BIT(m_nBitLength)) > 0 )?
             (0xff<<(S_1 - i)):0xff;
   for (; first < m_nSize; first++) {
    switch (symb) {
     case '1': memset(m_pData[first]+m_nAllocLength,0,m_nByteLength);
               memset(m_pData[first],0xff,m_nByteLength-1);
               m_pData[first][m_nByteLength-1]=maska; break;
     case '0': memset(m_pData[first]+m_nAllocLength,0xff,m_nByteLength-1);
               m_pData[first][m_nAllocLength+m_nByteLength-1]=maska;  
               memset(m_pData[first],0,m_nByteLength); break; 
     case '+': memset(m_pData[first]+m_nAllocLength,0xff,m_nByteLength-1);
               m_pData[first][m_nAllocLength+m_nByteLength-1]=maska;  
               memset(m_pData[first],0xff,m_nByteLength-1);
               m_pData[first][m_nByteLength-1]=maska;
    }
   }
  }
 return m_nSize-1;
 } 

//----------------- Add ------------------------------------------------
int CTM::Add(const CTM& tm, int ntmRow/*=-1*/)    //22.02.2007
{ int nIndex = m_nSize; 
  if (ntmRow >= 0) SetRowGrow(nIndex,tm,ntmRow);
  else InsertRow(nIndex, 0, tm, tm.GetCountR());
  return nIndex; 
}


//----------------- SetRowGrow ------------------------------------------------
void CTM::SetRowGrow(int nRow, const CTV& newRow)
{                 //change nRow with new vector
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(nRow >= 0);
  if (m_nSize > 0)
    ASSERT(m_nBitLength == newRow.GetBitLength());
#endif

 if (nRow >= m_nSize)
    SetSize(nRow+1,newRow.GetBitLength(),
            m_nGrowBy,m_nAllocLength*S_1-m_nBitLength);
 memcpy(m_pData[nRow],(const BYTE *)newRow.GetOnes(),m_nByteLength);
 memcpy(m_pData[nRow]+m_nAllocLength,
        (const BYTE *)newRow.GetZeros(),m_nByteLength);
}

//----------------- SetRowGrow ------------------------------------------------
void CTM::SetRowGrow(int nRow, const CTM& tm, int ntmRow)
{                   //change nRow with ntmRow from tm
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(nRow >= 0 && ntmRow < tm.m_nSize);
  if (m_nSize > 0) 
    ASSERT(m_nBitLength == tm.m_nBitLength);  //11.02.99
#endif
  if (nRow >= m_nSize) 
    SetSize(nRow+1,tm.m_nBitLength,m_nGrowBy,m_nAllocLength*S_1-m_nBitLength);
  memcpy(m_pData[nRow],tm.m_pData[ntmRow],m_nByteLength);
  memcpy(m_pData[nRow]+m_nAllocLength,
         tm.m_pData[ntmRow]+tm.m_nAllocLength,m_nByteLength);
}

//----------------- InsertRow -------------------------------------------------
void CTM::InsertRow(int nRow,const CTV& newRow, int nCount/*=1*/)
//Insert nCount copies of vector newRow before nRow row of TM
{             //insert nCount vector copies beginning from nRow
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(nRow >= 0 && newRow.GetBitLength() == m_nBitLength);
#endif
  BYTE *ptr;
  int i;
  if (nRow >= m_nSize) {     // adding after the end of the array
    SetSize(nRow + nCount,m_nBitLength,     // grow so nIndex is valid
            m_nGrowBy,m_nAllocLength*S_1-m_nBitLength);
  }
  else {                     // inserting in the middle of the array
    int out=m_nSize-nRow, begin=m_nSize-1;
    SetSize(m_nSize + nCount,m_nBitLength,  // grow it to new size
            m_nGrowBy,m_nAllocLength*S_1-m_nBitLength);
    for (i=1; i <= out; i++,begin--) {
      ptr=m_pData[begin]; 
      memmove(&m_pData[begin],&m_pData[m_nSize-i],sizeof(BYTE*));
      m_pData[m_nSize-i]=ptr;
    }
  }
// insert new value in the gap
  ASSERT(nRow + nCount <= m_nSize);
  for(i=nRow; i<nRow+nCount; i++) {
    memcpy(m_pData[i],newRow.GetOnes(),m_nByteLength);
    memcpy(m_pData[i]+m_nAllocLength,newRow.GetZeros(),m_nByteLength);
  }
}

//----------------- InsertRow -------------------------------------------------
void CTM::InsertRow(int nRow,int nStartRow, const CTM& tm,int nCount)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(tm.IsKindOf(RUNTIME_CLASS(CTM)));
  ASSERT(nRow >= 0 && nStartRow >= 0 && m_nBitLength == tm.m_nBitLength);
#endif
  if (tm.m_nSize > 0) {
    InsertRow(nRow,tm.GetRowTv(nStartRow),nCount);
    for (int i = 1; i < nCount; i++)
      SetRowTv(nRow + i,tm,nStartRow+i);
  }
}

//----------------- RemoveRow -------------------------------------------------
void CTM::RemoveRow(int nRow, int nCount /* = 1 */)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(nRow >= 0 && nCount >= 0 && nRow + nCount <= m_nSize);
#endif
  int nMoveCount = m_nSize - (nRow + nCount);     // just remove a range
  for (int i=0; i < nCount; i++) 
    if (m_pData[i+nRow]!=NULL)
      delete [] (BYTE*)m_pData[i+nRow];
  if (nMoveCount) {
      memcpy(&m_pData[nRow],&m_pData[nRow + nCount],nMoveCount * sizeof(BYTE*));
    }  
    m_nSize -= nCount;
}

//----------------- ExchangeRow -----------------------------------------------
void CTM::ExchangeRow(int nRow1,int nRow2,int nCount/*=1*/)
{ASSERT(nCount>0 && nRow1+nCount<=m_nSize && nRow2+nCount<=m_nSize);
 BYTE *ptr;
 while(nCount >0) {
   ptr=m_pData[nRow1]; m_pData[nRow1++]=m_pData[nRow2];
   m_pData[nRow2++]=ptr; nCount--; 
 }
 return;
}

//----------------- AddCol ----------------------------------------------------
int CTM::AddCol(int numCol/*=1*/)
//numCol columns adding to the matrix
//return the number of new last column
{ int i,nByte;
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(numCol > 0);
#endif
 m_nBitLength+=numCol;
 nByte = LEN_BYTE(m_nBitLength) * sizeof(BYTE);
 //------------------ Set columns size
 if (nByte > m_nAllocLength) {
   BYTE * ptr;
   for (i=0; i < m_nSize; i++) {
     ptr= (BYTE*) new BYTE[nByte<<1];
     memset(ptr,0,nByte<<1);  // zero fill
     memcpy(ptr, m_pData[i], m_nByteLength); // copy new data from old
     memcpy(ptr+nByte,m_pData[i]+m_nAllocLength, m_nByteLength);
     delete [] (BYTE*)m_pData[i];
     m_pData[i] = ptr; 
   }
   m_nAllocLength=nByte;
 }
 m_nByteLength = nByte;
 return m_nBitLength-1;
}
 

///////////////////////////////////////////////////////////////////////////////
/////////////////// Operators and functions of assignment /////////////////////
//----------------- Concat ----------------------------------------------------
//----------------- operator =(const CTM&) ------------------------------------
const CTM& CTM::operator =(const CTM& tm)
{int i;
 SetSize(tm.m_nSize,tm.m_nBitLength,tm.m_nGrowBy);
 for (i=0;i < tm.m_nSize;i++) SetRowTv(i,tm.GetRowTv(i));
 return *this;
}

//----------------- operator =(const char* ) ----------------------------------
const CTM& CTM::operator =(const char* pch)
{
  char * s;
#ifdef _LINUX
  s = strdup(pch);
#else
  s = _strdup(pch);
#endif
  CharBit(s);
  free(s);
  return *this;
}
/*
#ifndef _LINUX
//----------------- operator =(const CStringArray) ----------------------------
const CTM& CTM::operator =(const CStringArray& StrAr)
{ CharBit(StrAr);
 return *this;
}
#endif //_LINUX
*/
//----------------- Clear -----------------------------------------------------
void CTM::Clear(char symb/*='-'*/,int nRow/*=-1*/)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT (nRow>=-1);
#endif
 int i,k,first=0,last=m_nSize-1;
 BYTE maska; 
 if (nRow != -1) first=last=nRow;
 maska=((i = ADR_BIT(m_nBitLength)) > 0 )?(0xff<<(S_1 - i)):0xff;
 for (k=first; k <= last; k++) {
   memset(m_pData[k],0xff,m_nByteLength-1);
   m_pData[k][m_nByteLength-1]=maska;
   memset(m_pData[k]+m_nAllocLength,0xff,m_nByteLength-1);
   m_pData[k][m_nAllocLength+m_nByteLength-1]=maska;  
   switch (symb) {
     case '1': memset(m_pData[k]+m_nAllocLength,0,m_nByteLength); break;
     case '0': memset(m_pData[k],0,m_nByteLength); break; 
     case '-': memset(m_pData[k],0,m_nByteLength);
               memset(m_pData[k]+m_nAllocLength,0,m_nByteLength);break;
   }
 } 
}

///////////////////////////////////////////////////////////////////////////////
/////////////////// Logic operations //////////////////////////////////////////
//----------------- InvertDefs ------------------------------------------------
//инвертировать опр.значения строк или одной стр.

void CTM::InvertDefs(int nRow/*=-1*/)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 BYTE a, maska;
 maska=((j = ADR_BIT(m_nBitLength)) > 0 )?(0xff<<(S_1 - j)):0xff;
 if(nRow >=0) 
   for(j=0; j < m_nByteLength; j++) {
     a=m_pData[nRow][j];
     m_pData[nRow][j]=m_pData[nRow][j+m_nAllocLength];
     m_pData[nRow][j+m_nAllocLength]=a;
   }
 else
  for(nRow++; nRow < m_nSize; nRow++) 
   for(j=0; j < m_nByteLength; j++) {
     a=m_pData[nRow][j];
     m_pData[nRow][j]=m_pData[nRow][j+m_nAllocLength];
     m_pData[nRow][j+m_nAllocLength]=a;
   }
}    

///////////////////////////////////////////////////////////////////////////////
/////////////////// Operations of concatinations //////////////////////////////
//----------------- Concat ----------------------------------------------------
void CTM::Concat(const CTM& tm)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(m_nSize == tm.m_nSize);
#endif
 int i, j, OldLen = BIT_BYTE(m_nBitLength),
 l_bit = ADR_BIT(m_nBitLength), r_bit = S_1 - l_bit;
 SetSize(m_nSize,m_nBitLength + tm.m_nBitLength);
 for (i=0;i < m_nSize;i++)  {
   m_pData[i][OldLen] |= (tm.m_pData[i][0] >> l_bit);
   m_pData[i][OldLen+m_nAllocLength] |=
                            (tm.m_pData[i][tm.m_nAllocLength] >> l_bit);
   for (j=1;j < tm.m_nByteLength;j++) {
     m_pData[i][j+OldLen] =
     (tm.m_pData[i][j-1] << r_bit) | (tm.m_pData[i][j] >> l_bit);
     m_pData[i][j+OldLen+m_nAllocLength]=(tm.m_pData[i][tm.m_nAllocLength+j-1]
     << r_bit) | (tm.m_pData[i][j+tm.m_nAllocLength] >> l_bit);
   }
   m_pData[i][j+OldLen+m_nAllocLength] |= 
                            tm.m_pData[i][tm.m_nAllocLength+j-1]<<r_bit;
   m_pData[i][j+OldLen] |= tm.m_pData[i][j-1]<<r_bit;
 }
}

//----------------- Concat ----------------------------------------------------
void CTM::Concat(const CTM& tm1,const CTM& tm2)
{ASSERT(tm1.m_nSize == tm2.m_nSize);
 SetSize(tm1.m_nSize,tm1.m_nBitLength + tm2.m_nBitLength);
 int i,j,OldLen = BIT_BYTE(tm1.m_nBitLength),
 l_bit = ADR_BIT(tm1.m_nBitLength), r_bit = S_1 - l_bit;
 for (i=0;i < m_nSize;i++) {
   memcpy(m_pData[i],tm1.m_pData[i],tm1.m_nByteLength);
   memcpy(m_pData[i]+m_nAllocLength,
          tm1.m_pData[i]+tm1.m_nAllocLength,tm1.m_nByteLength);
 }
 for (i=0;i < m_nSize;i++)  {
   m_pData[i][OldLen] |= (tm2.m_pData[i][tm2.m_nAllocLength] >> l_bit);
   m_pData[i][OldLen+m_nAllocLength] |= 
                            (tm2.m_pData[i][tm2.m_nAllocLength] >> l_bit);
   for (j=1;j < tm2.m_nByteLength;j++) {
     m_pData[i][j+OldLen] = 
                  (tm2.m_pData[i][j-1]<<r_bit)|(tm2.m_pData[i][j]>>l_bit);
     m_pData[i][j+OldLen+m_nAllocLength] =
                          (tm2.m_pData[i][tm2.m_nAllocLength+j-1]<<r_bit)|
                          (tm2.m_pData[i][tm2.m_nAllocLength+j]>>l_bit);
   }
   m_pData[i][j+OldLen] |= (tm2.m_pData[i][j-1] << r_bit);
   m_pData[i][j+OldLen+m_nAllocLength] |= 
                        (tm2.m_pData[i][tm2.m_nAllocLength+j-1] << r_bit);
 }
}

///////////////////////////////////////////////////////////////////////////////
/////////////////// Operations of weighting, finding and casing ///////////////
//----------------- CountDef --------------------------------------------------
int CTM::CountDefs(int nRow/*=-1*/) const
{ASSERT (nRow >=-1 && nRow < m_nSize);
 int j,k,one=0,first,last;
 if (nRow==-1) { first=0; last=m_nSize-1; }
 else { first=nRow; last=nRow; }
 for (k=first; k <= last; k++)
   for (j=0; j < m_nByteLength; j++) 
     one+=TabC[m_pData[k][j]|m_pData[k][m_nAllocLength+j]];
 return one;
}

//----------------- CountOne --------------------------------------------------
int CTM::CountOnes(int nRow/*=-1*/) const
{ASSERT (nRow>=-1);
 int j,k,one=0,first,last;
 if (nRow==-1) { first=0; last=m_nSize-1; }
 else { first=nRow; last=nRow; }
 for (k=first; k <= last; k++)
   for (j=0; j < m_nByteLength; j++) 
     one+=TabC[m_pData[k][j]];
 return one;
}

//----------------- CountZero -------------------------------------------------
int CTM::CountZeros(int nRow/*=-1*/) const
{ASSERT (nRow>=-1);
 int j,k,one=0,first,last;
 if (nRow==-1) { first=0; last=m_nSize-1; }
 else { first=nRow; last=nRow; }
 for (k=first; k <= last; k++)
   for (j=0; j < m_nByteLength; j++) 
     one+=TabC[m_pData[k][m_nAllocLength+j]];
 return one;
}

//----------------- CountUndef ------------------------------------------------
int CTM::CountUnDefs(const BYTE* mask,int nRow) const
{ int j,k,one=0,first,last;
 ASSERT (nRow>=-1);
 if (nRow==-1) { first=0; last=m_nSize-1; }
 else { first=nRow; last=nRow; }
 for (k=first; k <=last; k++)
   for (j=0; j < m_nByteLength; j++)
   one+=TabC[(~(m_pData[k][j]|m_pData[k][m_nAllocLength+j]))&mask[j]];
 return one;
}

//----------------- CountDef --------------------------------------------------
int CTM::CountDefs(const BYTE* mask,int nRow/*=-1*/) const
{ int j,k,one=0,first,last;
 ASSERT (nRow>=-1);
 if (nRow==-1) { first=0; last=m_nSize-1; }
 else { first=nRow; last=nRow; }
 for (k=first; k <=last; k++)
   for (j=0; j < m_nByteLength; j++)
   one+=TabC[(m_pData[k][j]|m_pData[k][m_nAllocLength+j])&mask[j]];
 return one;
}

//----------------- CountOne --------------------------------------------------
int CTM::CountOnes(const BYTE* mask,int nRow/*=-1*/)  const
{ int j,k,one=0,first,last;
 ASSERT (nRow>=-1);
 if (nRow==-1) { first=0; last=m_nSize-1; }
 else { first=nRow; last=nRow; }
 for (k=first; k <=last; k++)
   for (j=0; j < m_nByteLength; j++)
   one+=TabC[m_pData[k][j]&mask[j]];
 return one;
}

//----------------- CountZero -------------------------------------------------
int CTM::CountZeros(const BYTE* mask,int nRow/*=-1*/)  const
{ int j,k,one=0,first,last;
 ASSERT (nRow>=-1);
 if (nRow==-1) { first=0; last=m_nSize-1; }
 else { first=nRow; last=nRow; }
 for (k=first; k <=last; k++)
   for (j=0; j < m_nByteLength; j++)
   one+=TabC[m_pData[k][m_nAllocLength+j]&mask[j]];
 return one;
}

//----------------- LeftUnDef -------------------------------------------------
int CTM::LeftUnDef(int nRow, int nNext/*=-1*/)  const //next bit
{ASSERT((m_nBitLength - nNext) >= 0);  //испр. 28.04.99
 int i, j=0, pos = 0;
 if(m_nBitLength <= (nNext+1)) return (-1);
 BYTE ch, ch_last;
 i=ADR_BIT(m_nBitLength);
 ch_last = ((~(m_pData[nRow][m_nByteLength-1]|
            m_pData[nRow][m_nAllocLength+m_nByteLength-1]))>>(S_1-i))<<(S_1-i);
 i=0;
 if (nNext >= 0) {
   i=BIT_BYTE(nNext); j=ADR_BIT(nNext);
   if (++j==S_1) {j=0; i++; }
 }
 ch=(i == (m_nByteLength-1))?ch_last:
                        (~(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]));
 ch <<= j;
 if (ch)  {
   while (!(ch & OB[pos++]));
   return (nNext + pos);
 }
 for (j=i+1; j < m_nByteLength-1; j++)
   if ((ch=~(m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j]))) {
     while (!(ch & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 if (j < m_nByteLength) 
   if(ch_last) {
     while (!(ch_last & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 return (-1);
}

//----------------- LeftDef ---------------------------------------------------
int CTM::LeftDef(int nRow, int nNext/*=-1*/)  const //next bit
{ASSERT((m_nBitLength - nNext) >= 0);
 int i=0,j=0,pos = 0;
 BYTE ch;
 if (nNext >= 0) {
   i=BIT_BYTE(nNext); j=ADR_BIT(nNext);
   if (++j==S_1) {j=0; i++; }
 }
 if (i == m_nByteLength)  return (-1);
 ch = (m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]) << j;
 if (ch)  {
   while (!(ch & OB[pos++]));
   return (nNext + pos);
 }
 for (j=i+1;j < m_nByteLength;j++)
   if ((ch=m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j])) {
     while (!(ch & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 return (-1);
}

//----------------- LeftOne ---------------------------------------------------
int CTM::LeftOne(int nRow, int nNext/*=-1*/)  const //next bit
{ASSERT((m_nBitLength - nNext) >= 0);
 int i=0,j=0,pos = 0;
 BYTE ch;
 if (nNext >= 0) {
   i=BIT_BYTE(nNext); j=ADR_BIT(nNext);
   if (++j==S_1) {j=0; i++; }
 }
 if (i == m_nByteLength)  return (-1);
 ch = m_pData[nRow][i] << j;
 if (ch)  {
   while (!(ch & OB[pos++]));
   return (nNext + pos);
 }
 for (j=i+1;j < m_nByteLength;j++)
   if ((ch=m_pData[nRow][j])) {
     while (!(ch & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 return (-1);
}

//----------------- LeftZero --------------------------------------------------
int CTM::LeftZero(int nRow, int nNext/*=-1*/)  const //next bit
{ASSERT((m_nBitLength - nNext) >= 0);
 int i=0,j=0,pos = 0;
 BYTE ch;
 if (nNext >= 0) {
   i=BIT_BYTE(nNext); j=ADR_BIT(nNext);
   if (++j==S_1) {j=0; i++; }
 }
 if (i == m_nByteLength)  return (-1);
 ch = m_pData[nRow][m_nAllocLength+i] << j;
 if (ch)  {
   while (!(ch & OB[pos++]));
   return (nNext + pos);
 }
 for (j=i+1;j < m_nByteLength;j++)
   if ((ch=m_pData[nRow][m_nAllocLength+j])) {
     while (!(ch & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 return (-1);
}

//----------------- LeftUndef -------------------------------------------------
int CTM::LeftUnDef(int nRow, int nNext, BYTE* mask) const
{ASSERT((m_nBitLength - nNext) >= 0);
 int i=0,j=0,pos = 0;
 BYTE ch;
 if (nNext >= 0) {
   i=BIT_BYTE(nNext);  j=ADR_BIT(nNext);
   if (++j==S_1) {j=0; i++; }
 }
 if (i == m_nByteLength)  return (-1);
 ch = ((~(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]))& mask[i]) << j;
 if (ch)  {
   while (!(ch & OB[pos++]));
   return (nNext + pos);
 }
 for (j=i+1;j<m_nByteLength;j++)
   if ((ch=(~(m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j]))&mask[j])) {
     while (!(ch & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 return (-1);
}

//----------------- LeftDef ---------------------------------------------------
int CTM::LeftDef(int nRow, int nNext, BYTE* mask) const
{ASSERT((m_nBitLength - nNext) >= 0);
 int i=0,j=0,pos = 0;
 BYTE ch;
 if (nNext >= 0) {
   i=BIT_BYTE(nNext);  j=ADR_BIT(nNext);
   if (++j==S_1) {j=0; i++; }
 }
 if (i == m_nByteLength)  return (-1);
 ch = ((m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i])&mask[i]) << j;
 if (ch)  {
   while (!(ch & OB[pos++]));
   return (nNext + pos);
 }
 for (j=i+1;j<m_nByteLength;j++)
   if ((ch=(m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j])&mask[j])) {
     while (!(ch & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 return (-1);
}

//----------------- LeftOne ---------------------------------------------------
int CTM::LeftOne(int nRow, int nNext, BYTE* mask) const
{ASSERT((m_nBitLength - nNext) >= 0);
 int i=0,j=0,pos = 0;
 BYTE ch;
 if (nNext >= 0) {
   i=BIT_BYTE(nNext);  j=ADR_BIT(nNext);
   if (++j==S_1) {j=0; i++; }
 }
 if (i == m_nByteLength)  return (-1);
 ch = (m_pData[nRow][i]& mask[i]) << j;
 if (ch)  {
   while (!(ch & OB[pos++]));
   return (nNext + pos);
 }
 for (j=i+1;j<m_nByteLength;j++)
   if ((ch=m_pData[nRow][j]&mask[j])) {
     while (!(ch & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 return (-1);
}

//----------------- LeftZero --------------------------------------------------
int CTM::LeftZero(int nRow, int nNext, BYTE* mask) const
{ASSERT((m_nBitLength - nNext) >= 0);
 int i=0,j=0,pos = 0;
 BYTE ch;
 if (nNext >= 0) {
   i=BIT_BYTE(nNext);  j=ADR_BIT(nNext);
   if (++j==S_1) {j=0; i++; }
 }
 if (i == m_nByteLength)  return (-1);
 ch = (m_pData[nRow][m_nAllocLength+i]&mask[i]) << j;
 if (ch)  {
   while (!(ch & OB[pos++]));
   return (nNext + pos);
 }
 for (j=i+1;j<m_nByteLength;j++)
   if ((ch=m_pData[nRow][m_nAllocLength+j]&mask[j])) {
     while (!(ch & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 return (-1);
}

//----------------- RightUnDef ------------------------------------------------
int CTM::RightUnDef(int nRow, int nPrev/*=-1*/) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i=m_nByteLength-1, j=0, pos = 7;
 BYTE ch;
 if (nPrev >= 0) {
   i=BIT_BYTE(nPrev); j=S_1 - ADR_BIT(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = ((~(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i])) >> j) << j;
 if (ch)  {
   while (!(ch & OB[pos--]));
   return ((i*S_1)+pos+1);
 }
 for (j=i-1;j>=0;j--)
   if((ch=(~(m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j]))))  {
     while (!(ch & OB[pos--]));
     return ((j*S_1)+pos+1);
   }
 return (-1);
}

//----------------- RightDef --------------------------------------------------
int CTM::RightDef(int nRow, int nPrev/*=-1*/) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i=m_nByteLength-1, j=0, pos = 7;
 BYTE ch;
 if (nPrev >= 0) {
   i=BIT_BYTE(nPrev); j=S_1 - ADR_BIT(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = ((m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]) >> j) << j;
 if (ch)  {
   while (!(ch & OB[pos--]));
   return ((i*S_1)+pos+1);
 }
 for (j=i-1;j>=0;j--)
   if((ch=m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j]))  {
     while (!(ch & OB[pos--]));
     return ((j*S_1)+pos+1);
   }
 return (-1);
}

//----------------- RightOne --------------------------------------------------
int CTM::RightOne(int nRow, int nPrev/*=-1*/) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i=m_nByteLength-1, j=0, pos = 7;
 BYTE ch;
 if (nPrev >= 0) {
   i=BIT_BYTE(nPrev); j=S_1 - ADR_BIT(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = (m_pData[nRow][i] >> j) << j;
 if (ch)  {
   while (!(ch & OB[pos--]));
   return ((i*S_1)+pos+1);
 }
 for (j=i-1;j>=0;j--)
   if((ch=m_pData[nRow][j]))  {
     while (!(ch & OB[pos--]));
     return ((j*S_1)+pos+1);
   }
 return (-1);
}


//----------------- RightZero -------------------------------------------------
int CTM::RightZero(int nRow, int nPrev/*=-1*/) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i=m_nByteLength-1, j=0, pos = 7;
 BYTE ch;
 if (nPrev >= 0) {
   i=BIT_BYTE(nPrev); j=S_1 - ADR_BIT(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = (m_pData[nRow][m_nAllocLength+i] >> j) << j;
 if (ch)  {
   while (!(ch & OB[pos--]));
   return ((i*S_1)+pos+1);
 }
 for (j=i-1;j>=0;j--)
   if((ch=m_pData[nRow][m_nAllocLength+j]))  {
     while (!(ch & OB[pos--]));
     return ((j*S_1)+pos+1);
   }
 return (-1);
}

//----------------- RightUnDef ------------------------------------------------
int CTM::RightUnDef(int nRow, int nPrev/*=-1*/,BYTE* mask) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i=m_nByteLength-1, j=0, pos = 7;
 BYTE ch;
 if (nPrev >= 0) {
   i=BIT_BYTE(nPrev); j=S_1 - ADR_BIT(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = ((~(m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i]))& mask[i]>>j)<<j;
 if (ch)  {
   while (!(ch & OB[pos--]));
   return ((i*S_1)+pos+1);
 }
 for (j=i-1;j >= 0;j--)
   if((ch=(~(m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j]))& mask[i])) {
     while (!(ch & OB[pos--]));
     return ((j*S_1)+pos+1);
   }
 return (-1);
}

//----------------- RightDef --------------------------------------------------
int CTM::RightDef(int nRow, int nPrev/*=-1*/,BYTE* mask) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i=m_nByteLength-1, j=0, pos = 7;
 BYTE ch;
 if (nPrev >= 0) {
   i=BIT_BYTE(nPrev); j=S_1 - ADR_BIT(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = (((m_pData[nRow][i]|m_pData[nRow][m_nAllocLength+i])& mask[i]) >> j) << j;
 if (ch)  {
   while (!(ch & OB[pos--]));
   return ((i*S_1)+pos+1);
 }
 for (j=i-1;j >= 0;j--)
   if((ch=(m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j])& mask[i])) {
     while (!(ch & OB[pos--]));
     return ((j*S_1)+pos+1);
   }
 return (-1);
}

//----------------- RightOne --------------------------------------------------
int CTM::RightOne(int nRow, int nPrev/*=-1*/,BYTE* mask) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i=m_nByteLength-1, j=0, pos = 7;
 BYTE ch;
 if (nPrev >= 0) {
   i=BIT_BYTE(nPrev); j=S_1 - ADR_BIT(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = ((m_pData[nRow][i]& mask[i]) >> j) << j;
 if (ch)  {
   while (!(ch & OB[pos--]));
   return ((i*S_1)+pos+1);
 }
 for (j=i-1;j >= 0;j--)
   if((ch=m_pData[nRow][j]& mask[i])) {
     while (!(ch & OB[pos--]));
     return ((j*S_1)+pos+1);
   }
 return (-1);
}

//----------------- RightZero -------------------------------------------------
int CTM::RightZero(int nRow, int nPrev/*=-1*/,BYTE* mask) const
{ASSERT((m_nBitLength - nPrev) >= 0);
 int i=m_nByteLength-1, j=0, pos = 7;
 BYTE ch;
 if (nPrev >= 0) {
   i=BIT_BYTE(nPrev); j=S_1 - ADR_BIT(nPrev);
   if (j==8)  {j=0;  i--; }
 }
 ch = ((m_pData[nRow][m_nAllocLength+i]& mask[i]) >> j) << j;
 if (ch)  {
   while (!(ch & OB[pos--]));
   return ((i*S_1)+pos+1);
 }
 for (j=i-1;j >= 0;j--)
   if((ch=m_pData[nRow][m_nAllocLength+j]& mask[i])) {
     while (!(ch & OB[pos--]));
     return ((j*S_1)+pos+1);
   }
 return (-1);
}

//----------------- FindUndefRow ----------------------------------------------
int CTM::FindUnDefR (int nRow/*=-1*/) const
{ASSERT(nRow < m_nSize-1);
 int j;
 BYTE work;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=work=0;j < m_nByteLength;j++) 
      if((work |= m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j]) !=0)
        break;
   if (work == 0) return nRow;
 }    
 return -1;
}                               

//----------------- FindDefRow ------------------------------------------------
int CTM::FindDefR (int nRow/*=-1*/) const
{ASSERT(nRow < m_nSize-1);
 int j;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=0;j < m_nByteLength;j++) 
      if((m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j]) !=0)
        return nRow;
 }    
 return -1;
}                               

//----------------- FindOneRow ------------------------------------------------
int CTM::FindOneR (int nRow/*=-1*/) const
{ASSERT(nRow < m_nSize-1);
 int j;
 BYTE work, maska;
 maska=((j = ADR_BIT(m_nBitLength)) > 0 )?(0xff<<(S_1 - j)):0xff;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=0,work=m_pData[nRow][m_nByteLength-1]^maska;
        j < m_nByteLength-1;j++) 
      if((work |= m_pData[nRow][j]^0xff) !=0) break;
   if (work == 0) return nRow;
 }    
 return -1;
}                               

//----------------- FindZeroRow -----------------------------------------------
int CTM::FindZeroR (int nRow/*=-1*/) const
{ASSERT(nRow < m_nSize-1);
 int j;
 BYTE work, maska;
 maska=((j = ADR_BIT(m_nBitLength)) > 0 )?(0xff<<(S_1 - j)):0xff;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=0,work=m_pData[nRow][m_nAllocLength+m_nByteLength-1]^maska;
        j < m_nByteLength-1;j++) 
      if((work |= m_pData[nRow][m_nAllocLength+j]^0xff) !=0) break;
   if (work == 0) return nRow;
 }    
 return -1;
}                               

//----------------- FindUndefRow ----------------------------------------------
int CTM::FindUnDefR(BYTE* mask, int nRow/*=-1*/) const
{int j;
 BYTE work;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=work=0;j < m_nByteLength;j++) 
     if((work|=(m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j])&mask[j]) !=0)
        break;
   if (work == 0) return nRow;
 }   
 return -1;
}                               

//----------------- FindDefRow ------------------------------------------------
int CTM::FindDefR (BYTE* mask, int nRow/*=-1*/) const
{int j;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=0;j < m_nByteLength;j++) 
     if(((m_pData[nRow][j]|m_pData[nRow][m_nAllocLength+j])&mask[j]) !=0)
       return nRow;
 }   
 return -1;
}

//----------------- FindOneRow ------------------------------------------------
int CTM::FindOneR (BYTE* mask, int nRow/*=-1*/) const
{int j;
 BYTE work;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=0,work=m_pData[nRow][m_nByteLength-1]^mask[m_nByteLength-1];
        j < m_nByteLength-1;j++) 
      if((work |= m_pData[nRow][j]^mask[j]) !=0) break;
   if (work == 0) return nRow;
 }    
 return -1;
}                               

//----------------- FindZeroRow -----------------------------------------------
int CTM::FindZeroR (BYTE* mask, int nRow/*=-1*/) const
{int j;
 BYTE work;
 for (++nRow; nRow < m_nSize; nRow++)  {
   for (j=0,work=m_pData[nRow][m_nAllocLength+m_nByteLength-1]
                              ^mask[m_nByteLength-1]; j < m_nByteLength-1;j++) 
      if((work |= m_pData[nRow][m_nAllocLength+j]^mask[j]) !=0) break;
   if (work == 0) return nRow;
 }    
 return -1;
}                                                        

//----------------- FindRow ---------------------------------------------------
int CTM::FindR (const CTV& tv, int nRow) const
{
  if (IsEmpty()) return -1;
  ASSERT(m_nBitLength == tv.GetBitLength());
  for (nRow++; nRow < m_nSize; nRow++)  
    if((memcmp(tv.GetOnes(),m_pData[nRow],m_nByteLength) == 0) &&
       (memcmp(tv.GetZeros(),m_pData[nRow]+m_nAllocLength,m_nByteLength)== 0))
      return nRow;
  return -1;
}

//----------------- FindRow ---------------------------------------------------
int CTM::FindR (const CBV& bv, int nRow) const
{
  if (IsEmpty()) return -1;
  ASSERT(m_nBitLength == bv.GetBitLength());
  for (nRow++; nRow < m_nSize; nRow++)  
    if((memcmp((BYTE*)bv,m_pData[nRow],m_nByteLength) == 0) &&
       (memcmp((BYTE*)~bv,m_pData[nRow]+m_nAllocLength,m_nByteLength)== 0)) 
      return nRow;
  return -1;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////// Matrix Compareing and sorting operations //////////////////
//----------------- IsCorrect -------------------------------------------------
BOOL CTM::IsCorrect(int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i,k,first=0,last=m_nSize-1;
 if (nRow != -1) first=last=nRow;
 for (k=first; k <= last; k++)
   for (i=0;i < m_nByteLength;i++)
     if((m_pData[k][i]&m_pData[k][m_nAllocLength+i]) != 0) return FALSE;
 return TRUE;
}

//----------------- IsBool ----------------------------------------------------
BOOL CTM::IsBool(int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i,k,first=0,last=m_nSize-1;
 BYTE maska;
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 maska=((i = ADR_BIT(m_nBitLength)) > 0 )?(0xff<<(S_1 - i)):0xff;
 if (nRow != -1) first=last=nRow;
 for (k=first; k <= last; k++) {
   for (i=0;i < m_nByteLength-1;i++)
     if(((m_pData[k][i]|m_pData[k][m_nAllocLength+i])^0xff) != 0)
       return FALSE;
   if((((m_pData[k][m_nByteLength-1]|m_pData[k][m_nAllocLength+m_nByteLength-1])
      ^maska)) != 0) return FALSE;
 }
 return TRUE;
}

//----------------- IsBool ----------------------------------------------------
BOOL CTM::IsBool(const BYTE* mask,int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i,k,first=0,last=m_nSize-1;
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 if (nRow != -1) first=last=nRow;
 for (k=first; k <= last; k++) 
   for (i=0;i < m_nByteLength;i++)
     if(((m_pData[k][i]|m_pData[k][m_nAllocLength+i])^mask[i]) != 0)
       return FALSE;
 return TRUE;
}


//----------------- IsTrivial -------------------------------------------------
BOOL CTM::IsTrivial(int nRow/*=-1*/) const  //all components are '-'
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i,k,first=0,last=m_nSize-1;
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 if (nRow != -1) first=last=nRow;
 for (k=first; k <= last; k++) 
   for (i=0;i < m_nByteLength;i++)
     if((m_pData[k][i]|m_pData[k][m_nAllocLength+i]) != 0)
       return FALSE;
 return TRUE;
}

//----------------- IsTrivial -------------------------------------------------
//all components are '-'

BOOL CTM::IsTrivial(const BYTE* mask,int nRow/*=-1*/) const  
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i,k,first=0,last=m_nSize-1;
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 if (nRow != -1) first=last=nRow;
 for (k=first; k <= last; k++) 
   for (i=0;i < m_nByteLength;i++)
     if(((m_pData[k][i]|m_pData[k][m_nAllocLength+i])&mask[i]) != 0)
       return FALSE;
 return TRUE;
}

//----------------- IsOne -----------------------------------------------------
BOOL CTM::IsOne(int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i,k,first=0,last=m_nSize-1;
 BYTE maska;
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 maska=((i = ADR_BIT(m_nBitLength)) > 0 )?(0xff<<(S_1 - i)):0xff;
 if (nRow != -1) first=last=nRow;
 for (k=first; k <= last; k++) {
   for (i=0;i < m_nByteLength-1;i++)
     if((m_pData[k][i]^0xff) != 0)
       return FALSE;
   if((m_pData[k][m_nByteLength-1]^maska) != 0) return FALSE;
 }
 return TRUE;
}

//----------------- IsOne -----------------------------------------------------
BOOL CTM::IsOne(const BYTE* mask,int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i,k,first=0,last=m_nSize-1;
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 if (nRow != -1) first=last=nRow;
 for (k=first; k <= last; k++) 
   for (i=0;i < m_nByteLength;i++)
     if(((m_pData[k][i]&mask[i])^mask[i]) != 0)
       return FALSE;
 return TRUE;
}

//----------------- IsZero ----------------------------------------------------
BOOL CTM::IsZero(int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i,k,first=0,last=m_nSize-1;
 BYTE maska;
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 maska=((i = ADR_BIT(m_nBitLength)) > 0 )?(0xff<<(S_1 - i)):0xff;
 if (nRow != -1) first=last=nRow;
 for (k=first; k <= last; k++) {
   for (i=0;i < m_nByteLength-1;i++)
     if((m_pData[k][m_nAllocLength+i]^0xff) != 0)
       return FALSE;
   if((m_pData[k][m_nAllocLength+m_nByteLength-1]^maska) != 0)
     return FALSE;
 }
 return TRUE;
}

//----------------- IsZero ----------------------------------------------------
BOOL CTM::IsZero(const BYTE* mask,int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i,k,first=0,last=m_nSize-1;
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 if (nRow != -1) first=last=nRow;
 for (k=first; k <= last; k++)
   for (i=0;i < m_nByteLength;i++)
     if(((m_pData[k][m_nAllocLength+i]&mask[i])^mask[i]) != 0)
       return FALSE;
 return TRUE;
}

//--------------------------- ExistOne ----------------------------------------
BOOL CTM::ExistOne(int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i,k,first=0,last=m_nSize-1;
 if (nRow != -1) first=last=nRow;
 for (k=first; k <= last; k++) 
   for (i=0;i < m_nByteLength;i++)
     if(m_pData[k][i] != 0) return TRUE;
 return FALSE;
}

//--------------------------- ExistOne ----------------------------------------
BOOL CTM::ExistOne(const BYTE* mask,int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i,k,first=0,last=m_nSize-1;
 if (nRow != -1) first=last=nRow;
 for (k=first; k <= last; k++) 
   for (i=0;i < m_nByteLength;i++)
     if((m_pData[k][i]&mask[i])!= 0) return TRUE;
 return FALSE;
}

//--------------------------- ExistZero ---------------------------------------
BOOL CTM::ExistZero(int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i,k,first=0,last=m_nSize-1;
 if (nRow != -1) first=last=nRow;
 for (k=first; k <= last; k++) 
   for (i=0;i < m_nByteLength;i++)
     if(m_pData[k][m_nAllocLength+i] != 0) return TRUE;
 return FALSE;
}

//--------------------------- ExistZero ---------------------------------------
BOOL CTM::ExistZero(const BYTE* mask,int nRow/*=-1*/) const
{ASSERT (nRow>=-1 && nRow < m_nSize);
 int i,k,first=0,last=m_nSize-1;
 if (nRow != -1) first=last=nRow;
 for (k=first; k <= last; k++) 
   for (i=0;i < m_nByteLength;i++)
     if((m_pData[k][m_nAllocLength+i]&mask[i])!= 0) return TRUE;
 return FALSE;
}

//----------------- SortDecr --------------------------------------------------
void CTM::SortDecr(int nBeg/*=0*/,int number/*=-1*/)
{ASSERT(number == -1 || number > 1);
 BOOL bNotDone = TRUE;
 int pos, posNext, nEnd=(number < 0)?m_nSize:(nBeg+number);
 ASSERT(nEnd <=m_nSize);
 while (bNotDone) {
   bNotDone = FALSE;
   for(pos=nBeg,posNext=nBeg+1; pos < nEnd-1;pos++,posNext++)
    if(CountDefs(pos) < CountDefs(posNext)) {
      ExchangeRow(pos,posNext);
      bNotDone = TRUE;
    }
 }
}

//----------------- SortIncr --------------------------------------------------
void CTM::SortIncr(int nBeg/*=0*/,int number/*=-1*/)
{ASSERT(number == -1 || number > 1);
 BOOL bNotDone = TRUE;
 int pos, posNext, nEnd=(number < 0)?m_nSize:(nBeg+number);
 ASSERT(nEnd <=m_nSize);
 while (bNotDone) {
   bNotDone = FALSE;
   for(pos=nBeg,posNext=nBeg+1; pos < nEnd-1;pos++,posNext++)
     if(CountDefs(pos) > CountDefs(posNext)) {
       ExchangeRow(pos,posNext);
       bNotDone = TRUE;
     }
 }
}

//----------------- operator== ------------------------------------------------
STD(BOOL) operator==(const CTM& tm1,const CTM& tm2)
{if ((tm1.m_nSize != tm2.m_nSize) || (tm1.m_nBitLength != tm2.m_nBitLength))
   return FALSE;
 int i,j;
 for (i=0;i < tm1.m_nSize;i++)
   for (j=0;j < tm1.m_nByteLength;j++) 
     if ((tm1.m_pData[i][j] != tm2.m_pData[i][j]) ||
(tm1.m_pData[i][tm1.m_nAllocLength+j] != tm2.m_pData[i][tm2.m_nAllocLength+j])) 
       return FALSE;
 return TRUE;
}

//----------------- operator!= ------------------------------------------------
STD(BOOL) operator!=(const CTM& tm1,const CTM& tm2)
{if ((tm1.m_nSize != tm2.m_nSize) || (tm1.m_nBitLength != tm2.m_nBitLength))
   return TRUE;
 int i,j;
 for (i=0;i < tm1.m_nSize;i++)
   for (j=0;j < tm1.m_nByteLength;j++)
     if ((tm1.m_pData[i][j] != tm2.m_pData[i][j]) ||
(tm1.m_pData[i][tm1.m_nAllocLength+j] != tm2.m_pData[i][tm2.m_nAllocLength+j])) 
     return TRUE;
 return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////// Row compareing operations /////////////////////////////////
//----------------- AreEqual --------------------------------------------------
BOOL CTM::AreEqual(int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 for (j=0; j < m_nByteLength;j++)
     if (((m_pData[nRow1][j] != m_pData[nRow2][j]) ||
(m_pData[nRow1][m_nAllocLength+j] != m_pData[nRow2][m_nAllocLength+j])))
       return FALSE;
 return TRUE;
}

//----------------- AreEqual --------------------------------------------------
BOOL CTM::AreEqual(const BYTE* mask,int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 for (j=0; j < m_nByteLength;j++)
     if ((((m_pData[nRow1][m_nAllocLength+j]^m_pData[nRow2][m_nAllocLength+j])|
     (m_pData[nRow1][j]^m_pData[nRow2][j]))&mask[j]) != 0)
       return FALSE;
 return TRUE;
}
//----------------- AreEqual --------------------------------------------------
BOOL CTM::AreEqual(int nRow,const CTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != tv.GetBitLength()) return FALSE;
 int j;
 for (j=0; j < m_nByteLength;j++)
     if (((m_pData[nRow][j] != tv.m_bVect1[j]) ||
(m_pData[nRow][m_nAllocLength+j] != tv.m_bVect0[j])))
       return FALSE;
 return TRUE;
}

//----------------- AreEqual --------------------------------------------------
BOOL CTM::AreEqual(const BYTE* mask,int nRow,const CTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != tv.GetBitLength()) return FALSE;
 int j;
 for (j=0; j < m_nByteLength;j++)
 if ((((m_pData[nRow][m_nAllocLength+j]^tv.m_bVect0[j])|
     (m_pData[nRow][j]^tv.m_bVect1[j]))&mask[j]) != 0)
       return FALSE;
 return TRUE;
}

//----------------- AreNotEqual -----------------------------------------------
BOOL CTM::AreNotEqual(int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 for (j=0; j < m_nByteLength;j++)
     if (((m_pData[nRow1][j] != m_pData[nRow2][j]) ||
(m_pData[nRow1][m_nAllocLength+j] != m_pData[nRow2][m_nAllocLength+j]))) 
       return TRUE;
 return FALSE;
}

//----------------- AreNotEqual -----------------------------------------------
BOOL CTM::AreNotEqual(const BYTE* mask,int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 for (j=0; j < m_nByteLength;j++)
     if ((((m_pData[nRow1][m_nAllocLength+j]^m_pData[nRow2][m_nAllocLength+j])|
     (m_pData[nRow1][j]^m_pData[nRow2][j]))&mask[j]) != 0)
       return TRUE;
 return FALSE;
}
//----------------- AreNotEqual -----------------------------------------------
BOOL CTM::AreNotEqual(int nRow,const CTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != tv.GetBitLength()) return FALSE;
 int j;
 for (j=0; j < m_nByteLength;j++)
     if (((m_pData[nRow][j] != tv.m_bVect1[j]) ||
(m_pData[nRow][m_nAllocLength+j] != tv.m_bVect0[j])))
       return TRUE;
 return FALSE;
}

//----------------- AreNotEqual -----------------------------------------------
BOOL CTM::AreNotEqual(const BYTE* mask,int nRow,const CTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != tv.GetBitLength()) return FALSE;
 int j;
 for (j=0; j < m_nByteLength;j++)
 if ((((m_pData[nRow][m_nAllocLength+j]^tv.m_bVect0[j])|
     (m_pData[nRow][j]^tv.m_bVect1[j]))&mask[j]) != 0)
       return TRUE;
 return FALSE;
}

//----------------- AreOrthog -------------------------------------------------
BOOL CTM::AreOrthog(int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 for (j=0; j < m_nByteLength;j++)
     if ((m_pData[nRow1][j]&m_pData[nRow2][m_nAllocLength+j] ||
       m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][j]))
       return TRUE;
 return FALSE;
}

//----------------- AreOrthog -------------------------------------------------
BOOL CTM::AreOrthog(const BYTE* mask,int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 for (j=0; j < m_nByteLength;j++)
    if((((m_pData[nRow1][j]&m_pData[nRow2][m_nAllocLength+j])|
       (m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][j]))&mask[j]) !=0) 
       return TRUE;
 return FALSE;
}

//----------------- AreOrthog -------------------------------------------------
BOOL CTM::AreOrthog(int nRow,const CTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != tv.GetBitLength()) return FALSE;
 int j;
 for (j=0; j < m_nByteLength;j++)
     if (((m_pData[nRow][j]&tv.m_bVect0[j]) ||
(m_pData[nRow][m_nAllocLength+j]&tv.m_bVect1[j])))
     return TRUE;
 return FALSE;
}

//----------------- AreOrthog -------------------------------------------------
BOOL CTM::AreOrthog(const BYTE* mask,int nRow,const CTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != tv.GetBitLength()) return FALSE;
 int j;
 for (j=0; j < m_nByteLength;j++)
 if ((((m_pData[nRow][m_nAllocLength+j]&tv.m_bVect1[j])|
     (m_pData[nRow][j]&tv.m_bVect0[j]))&mask[j]) != 0)
       return TRUE;
 return FALSE;
}

//----------------- AreOrthog -------------------------------------------------
BOOL CTM::AreOrthog(int nRow,const CBV& bv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != bv.GetBitLength()) return FALSE;
 int j;
 for (j=0; j < m_nByteLength;j++)
   if (((m_pData[nRow][j]&(~bv.GetByteAt(j))) ||
(m_pData[nRow][m_nAllocLength+j]&bv.GetByteAt(j))))
     return TRUE;
 return FALSE;
}

//----------------- AreOrthog -------------------------------------------------
BOOL CTM::AreOrthog(const BYTE* mask,int nRow,const CBV& bv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != bv.GetBitLength()) return FALSE;
 int j;
 for (j=0; j < m_nByteLength;j++)
 if ((((m_pData[nRow][m_nAllocLength+j]&bv.GetByteAt(j))|
     (m_pData[nRow][j]&(~bv.GetByteAt(j))))&mask[j]) != 0)
       return TRUE;
 return FALSE;
}

//----------------- AreNeighb -------------------------------------------------
BOOL CTM::AreNeighb(int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j, i;
 BYTE  mas1, mas0;
 for (i=j=0; j < m_nByteLength;j++) {
   if(((mas0=m_pData[nRow1][m_nAllocLength+j]^m_pData[nRow2][m_nAllocLength+j])
     ^ (mas1=m_pData[nRow1][j]^m_pData[nRow2][j]))) return FALSE;
   i+=TabC[mas1];
 }
 if(i != 1) return FALSE;
 return TRUE;
}

//----------------- AreNeighb -------------------------------------------------
BOOL CTM::AreNeighb(const BYTE* mask,int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j, i;
 BYTE  mas1, mas0;
 for (j=i=0; j < m_nByteLength;j++) {
   if((((mas0=m_pData[nRow1][m_nAllocLength+j]^m_pData[nRow2][m_nAllocLength+j])
     ^ (mas1=m_pData[nRow1][j]^m_pData[nRow2][j]))&mask[j]) !=0) return FALSE;
   i+=TabC[mas1&mask[j]];
 }
 if(i != 1) return FALSE;
 return TRUE;
}

//----------------- AreNeighb -------------------------------------------------
BOOL CTM::AreNeighb(int nRow,const CTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != tv.GetBitLength()) return FALSE;
 int i,j;
 BYTE  mas1, mas0;
 for (i=j=0; j < m_nByteLength;j++) {
   if(((mas0=m_pData[nRow][m_nAllocLength+j]^tv.m_bVect0[j])
     ^ (mas1=m_pData[nRow][j]^tv.m_bVect1[j]))) return FALSE;
   i+=TabC[mas1];
 }
 if(i != 1) return FALSE;
 return TRUE;
}

//----------------- AreNeighb -------------------------------------------------
BOOL CTM::AreNeighb(const BYTE* mask,int nRow,const CTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != tv.GetBitLength()) return FALSE;
 int i,j;
 BYTE  mas1, mas0;
 for (j=i=0; j < m_nByteLength;j++) {
   if((((mas0=m_pData[nRow][m_nAllocLength+j]^tv.m_bVect0[j])
     ^ (mas1=m_pData[nRow][j]^tv.m_bVect1[j]))&mask[j]) !=0) return FALSE;
   i+=TabC[mas1&mask[j]];
 }
 if(i != 1) return FALSE;
 return TRUE;
}

//----------------- AreConcens ------------------------------------------------
BOOL CTM::AreConcens(int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j, i;
 BYTE  mas;
 for (i=j=0; j < m_nByteLength;j++) {
   if((mas=(m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][j]) |
     (m_pData[nRow1][j]&m_pData[nRow2][m_nAllocLength+j]))!= 0)
     i+=TabC[mas];
 }
 if(i != 1) return FALSE;
 return TRUE;
}

//----------------- AreConcens ------------------------------------------------
BOOL CTM::AreConcens(const BYTE* mask,int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j, i;
 BYTE  mas;
 for (j=i=0; j < m_nByteLength;j++) {
   if(((mas=(m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][j]) |
     (m_pData[nRow1][j]&m_pData[nRow2][m_nAllocLength+j]))&mask[j]) != 0)
     i+=TabC[mas&mask[j]];
 }
 if(i != 1) return FALSE;
 return TRUE;
}

//----------------- AreConcens ------------------------------------------------
BOOL CTM::AreConcens(int nRow,const CTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != tv.GetBitLength()) return FALSE;
 int i, j;
 BYTE  mas;
 for (i=j=0; j < m_nByteLength;j++) {
   if((mas=(m_pData[nRow][m_nAllocLength+j]&tv.m_bVect1[j]) |
     (m_pData[nRow][j]&tv.m_bVect0[j]))!= 0)
    i+=TabC[mas];
 }
 if(i != 1) return FALSE;
 return TRUE;
}

//----------------- AreConcens ------------------------------------------------
BOOL CTM::AreConcens(const BYTE* mask,int nRow,const CTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != tv.GetBitLength()) return FALSE;
 int i,j;
 BYTE  mas;
 for (j=i=0; j < m_nByteLength;j++) {
   if(((mas=(m_pData[nRow][m_nAllocLength+j]&tv.m_bVect1[j]) |
     (m_pData[nRow][j]&tv.m_bVect0[j]))&mask[j]) != 0)
     i+=TabC[mas&mask[j]];
 }
 if(i != 1) return FALSE;
 return TRUE;
}

//----------------- IsCover ---------------------------------------------------
BOOL CTM::IsCover(int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 for (j=0; j < m_nByteLength;j++) { 
   if((((m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][m_nAllocLength+j])^
      m_pData[nRow1][m_nAllocLength+j]) ==0) &&
     (((m_pData[nRow1][j]&m_pData[nRow2][j])^m_pData[nRow1][j]) == 0)) continue;
     return FALSE;
 }
 return TRUE;
}

//----------------- IsCover ---------------------------------------------------
BOOL CTM::IsCover(const BYTE* mask,int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 for (j=0; j < m_nByteLength;j++) 
   if((((m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][m_nAllocLength+j])^
      m_pData[nRow1][m_nAllocLength+j])|
     ((m_pData[nRow1][j]&m_pData[nRow2][j])^m_pData[nRow1][j])&mask[j]) !=0)
     return FALSE;
 return TRUE;
}

//----------------- IsCover ---------------------------------------------------
BOOL CTM::IsCover(int nRow,const CTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != tv.GetBitLength()) return FALSE;
 int j;
 for (j=0; j < m_nByteLength;j++) {
   if((((m_pData[nRow][m_nAllocLength+j]&tv.m_bVect0[j])
        ^m_pData[nRow][m_nAllocLength+j]) ==0) &&
        (((m_pData[nRow][j]&tv.m_bVect1[j])^m_pData[nRow][j]) == 0)) continue;
   return FALSE; }
 return TRUE;
}

//----------------- IsCover ---------------------------------------------------
BOOL CTM::IsCover(const BYTE* mask,int nRow,const CTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
if (m_nBitLength != tv.GetBitLength()) return FALSE;
 int i,j;
 for (j=i=0; j < m_nByteLength;j++) 
   if((((m_pData[nRow][m_nAllocLength+j]&tv.m_bVect0[j])
          ^m_pData[nRow][m_nAllocLength+j])|
          ((m_pData[nRow][j]&tv.m_bVect1[j])^m_pData[nRow][j])&mask[j]) !=0) 
     return FALSE;
 return TRUE;
}

//----------------- IsCover ---------------------------------------------------
BOOL CTM::IsCover(int nRow,const CBV& bv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != bv.GetBitLength()) return FALSE;
 BYTE part;
 int j;
 for (j=0; j < m_nByteLength;j++) {
   part=bv.GetByteAt(j);     // 25.09.2008
   //   if((((m_pData[nRow][m_nAllocLength+j]&(~part))
   //     ^m_pData[nRow][m_nAllocLength+j]) ==0) ||
   //     (((m_pData[nRow][j]&part)^m_pData[nRow][j]) == 0)) continue;
   if((m_pData[nRow][m_nAllocLength+j]&part) ||
          (m_pData[nRow][j]&(~part)))
     return FALSE;
 }
 return TRUE;
}

//----------------- IsCover ---------------------------------------------------
BOOL CTM::IsCover(const BYTE* mask,int nRow,const CBV& bv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != bv.GetBitLength()) return FALSE;
 int j;
 BYTE  part;
 for (j=0; j < m_nByteLength;j++) {
   part=bv.GetByteAt(j);
//   if(((((m_pData[nRow][m_nAllocLength+j]&(~part))      // 25.09.2008
//          ^m_pData[nRow][m_nAllocLength+j])|
//      ((m_pData[nRow][j]&part)^m_pData[nRow][j]))&mask[j]) !=0) return FALSE;
   if((((m_pData[nRow][m_nAllocLength+j]&part)|
      (m_pData[nRow][j]&(~part))) &mask[j]) !=0) return FALSE;
 }
 return TRUE;
}

//----------------- IsCovered -------------------------------------------------
BOOL CTM::IsCovered(int nRow,const CTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != tv.GetBitLength()) return FALSE;
 int j;
 for (j=0; j < m_nByteLength;j++) {
   if((((m_pData[nRow][m_nAllocLength+j]&tv.m_bVect0[j])^tv.m_bVect0[j]) ==0)
      &&(((m_pData[nRow][j]&tv.m_bVect1[j])^tv.m_bVect1[j]) == 0)) continue;
     return FALSE;
 }
 return TRUE;
}

//----------------- IsCovered -------------------------------------------------
BOOL CTM::IsCovered(const BYTE* mask,int nRow,const CTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 if (m_nBitLength != tv.GetBitLength()) return FALSE;
 int i, j;
 for (j=i=0; j < m_nByteLength;j++) 
   if((((m_pData[nRow][m_nAllocLength+j]&tv.m_bVect0[j])^tv.m_bVect0[j])
     |((m_pData[nRow][j]&tv.m_bVect1[j])^tv.m_bVect1[j])&mask[j]) !=0) 
     return FALSE;
 return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////// Row interval operations ///////////////////////////////////
//--------------------------- Orthogon ----------------------------------------
CBV CTM::Orthogon(int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 CBV bvTag(0,m_nBitLength,TRUE);
 for (j=0; j < m_nByteLength;j++)
   bvTag.SetByteAt(j,((m_pData[nRow1][j]&m_pData[nRow2][m_nAllocLength+j]|
    (m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][j])))); 
return bvTag;
}

//--------------------------- Orthogon ----------------------------------------
CBV CTM::Orthogon(const BYTE* mask,int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 CBV bvTag(0,m_nBitLength,TRUE);
 for (j=0; j < m_nByteLength;j++)
   bvTag.SetByteAt(j,((m_pData[nRow1][j]&m_pData[nRow2][m_nAllocLength+j]|
    (m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][j]))&mask[j]));
return bvTag;
}

//--------------------------- Orthogon ----------------------------------------
CBV CTM::Orthogon(int nRow,const CTV& tv) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 CBV bvTag(0,m_nBitLength,TRUE);
 for (j=0; j < m_nByteLength;j++)
   bvTag.SetByteAt(j,((m_pData[nRow][j]&tv.m_bVect0[j]|
    (m_pData[nRow][m_nAllocLength+j]&tv.m_bVect1[j]))));
return bvTag;
}

//--------------------------- Orthogon ----------------------------------------
//мн-во ортогон.компонент

CBV CTM::Orthogon(const BYTE* mask,int nRow,const CTV& tv) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 CBV bvTag(0,m_nBitLength,TRUE);
 for (j=0; j < m_nByteLength;j++)
   bvTag.SetByteAt(j,((m_pData[nRow][j]&tv.m_bVect0[j]|
    (m_pData[nRow][m_nAllocLength+j]&tv.m_bVect1[j]))&mask[j]));
return bvTag;
}

//--------------------------- Orthogon ----------------------------------------
CBV CTM::Orthogon(int nRow,const CBV& bv) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 BYTE part;
 CBV bvTag(0,m_nBitLength,TRUE);
 for (j=0; j < m_nByteLength;j++) {
   part=bv.GetByteAt(j);
   bvTag.SetByteAt(j,((m_pData[nRow][j]&(~part))|
    (m_pData[nRow][m_nAllocLength+j]&part)));
 }
return bvTag;
}

//--------------------------- Orthogon ----------------------------------------
//мн-во ортогон.компонент

CBV CTM::Orthogon(const BYTE* mask,int nRow,const CBV& bv) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 BYTE part;
 CBV bvTag(0,m_nBitLength,TRUE);
 for (j=0; j < m_nByteLength;j++) {
   part=bv.GetByteAt(j);
   bvTag.SetByteAt(j,((m_pData[nRow][j]&(~part))|
    (m_pData[nRow][m_nAllocLength+j]&part))&mask[j]);
 }
return bvTag;
}

//--------------------------- Intersec ----------------------------------------
CTV CTM::Intersec(int nRow1,int nRow2) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   tvTag.m_bVect1[j]=m_pData[nRow1][j]|m_pData[nRow2][j];
   tvTag.m_bVect0[j]=m_pData[nRow1][m_nAllocLength+j]|
       m_pData[nRow2][m_nAllocLength+j];
 }
 return tvTag;
}

//--------------------------- Intersec ----------------------------------------
//мн-во ортогон.компонент

CTV CTM::Intersec(const BYTE* mask,int nRow1,int nRow2) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   tvTag.m_bVect1[j]=(m_pData[nRow1][j]|m_pData[nRow2][j])&mask[j];
   tvTag.m_bVect0[j]=(m_pData[nRow1][m_nAllocLength+j]|
       m_pData[nRow2][m_nAllocLength+j])&mask[j];
 }
 return tvTag;
}

//--------------------------- Intersec ----------------------------------------
CTV CTM::Intersec(int nRow,const CTV& tv) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   tvTag.m_bVect1[j]=m_pData[nRow][j]|tv.m_bVect1[j];
   tvTag.m_bVect0[j]=m_pData[nRow][m_nAllocLength+j]|tv.m_bVect0[j];
 }
 return tvTag;
}

//--------------------------- Intersec ----------------------------------------
//мн-во ортогон.компонент

CTV CTM::Intersec(const BYTE* mask,int nRow,const CTV& tv) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   tvTag.m_bVect1[j]=(m_pData[nRow][j]|tv.m_bVect1[j])&mask[j];
   tvTag.m_bVect0[j]=(m_pData[nRow][m_nAllocLength+j]|tv.m_bVect0[j])&mask[j];
 }
 return tvTag;
}

//--------------------------- Concens -----------------------------------------
CTV CTM::Concens(int nRow1,int nRow2) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 BYTE maska;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   maska=(m_pData[nRow1][j]&m_pData[nRow2][m_nAllocLength+j])|
      (m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][j]);
   tvTag.m_bVect1[j]=(m_pData[nRow1][j]|m_pData[nRow2][j])^maska;
   tvTag.m_bVect0[j]=(m_pData[nRow1][m_nAllocLength+j]|
         m_pData[nRow2][m_nAllocLength+j])^maska;
 }
 return tvTag;
}

//--------------------------- Concens -----------------------------------------
//мн-во ортогон.компонент

CTV CTM::Concens(const BYTE* mask,int nRow1,int nRow2) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 BYTE maska;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   maska=(m_pData[nRow1][j]&m_pData[nRow2][m_nAllocLength+j])|
      (m_pData[nRow1][m_nAllocLength+j]&m_pData[nRow2][j]);
   tvTag.m_bVect1[j]=((m_pData[nRow1][j]|m_pData[nRow2][j])^maska)&mask[j];
   tvTag.m_bVect0[j]=((m_pData[nRow1][m_nAllocLength+j]|
         m_pData[nRow2][m_nAllocLength+j])^maska)&mask[j];
 }
 return tvTag;
}

//--------------------------- Concens -----------------------------------------
CTV CTM::Concens(int nRow,const CTV& tv) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 BYTE maska;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   maska=(m_pData[nRow][j]&tv.m_bVect0[j])|
      (m_pData[nRow][m_nAllocLength+j]&tv.m_bVect1[j]);
   tvTag.m_bVect1[j]=(m_pData[nRow][j]|tv.m_bVect1[j])^maska;
   tvTag.m_bVect0[j]=(m_pData[nRow][m_nAllocLength+j]|tv.m_bVect0[j])^maska;
 }
 return tvTag;
}

//--------------------------- Concens -----------------------------------------
//мн-во ортогон.компонент

CTV CTM::Concens(const BYTE* mask,int nRow,const CTV& tv) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 BYTE maska;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   maska=(m_pData[nRow][j]&tv.m_bVect0[j])|
      (m_pData[nRow][m_nAllocLength+j]&tv.m_bVect1[j]);
   tvTag.m_bVect1[j]=((m_pData[nRow][j]|tv.m_bVect1[j])^maska)&mask[j];
   tvTag.m_bVect0[j]=((m_pData[nRow][m_nAllocLength+j]|tv.m_bVect0[j])^maska)&mask[j];
 }
 return tvTag;
}

//--------------------------- Concens -----------------------------------------
//мн-во ортогон.компонент

CTV CTM::Concens(const BYTE* mask,int nRow,const CBV& bv) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 BYTE maska, part;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   part=bv.GetByteAt(j);
   maska=(~((m_pData[nRow][j]&(~part)|
      (m_pData[nRow][m_nAllocLength+j]&part))))&mask[j];
   tvTag.m_bVect1[j]=part&maska;
   tvTag.m_bVect0[j]=(~part)&maska;
 }
 return tvTag;
}

//--------------------------- Concens -----------------------------------------
CTV CTM::Concens(int nRow,const CBV& bv) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 BYTE maska, part;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   part=bv.GetByteAt(j);
   maska=~((m_pData[nRow][j]&(~part))|
      (m_pData[nRow][m_nAllocLength+j]&part));
   tvTag.m_bVect1[j]=part&maska;
   tvTag.m_bVect0[j]=(~part)&maska;
 }
 return tvTag;
}

//--------------------------- MinCover ----------------------------------------
CTV CTM::MinCover(int nRow1,int nRow2) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   tvTag.m_bVect1[j]=m_pData[nRow1][j]&m_pData[nRow2][j];
   tvTag.m_bVect0[j]=m_pData[nRow1][m_nAllocLength+j]&
       m_pData[nRow2][m_nAllocLength+j];
 }
 return tvTag;
}

//--------------------------- MinCover ----------------------------------------
//мн-во ортогон.компонент

CTV CTM::MinCover(const BYTE* mask,int nRow1,int nRow2) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   tvTag.m_bVect1[j]=m_pData[nRow1][j]&m_pData[nRow2][j]&mask[j];
   tvTag.m_bVect0[j]=m_pData[nRow1][m_nAllocLength+j]&
       m_pData[nRow2][m_nAllocLength+j]&mask[j];
 }
 return tvTag;
}

//--------------------------- MinCover ----------------------------------------
CTV CTM::MinCover(int nRow,const CTV& tv) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   tvTag.m_bVect1[j]=m_pData[nRow][j]&tv.m_bVect1[j];
   tvTag.m_bVect0[j]=m_pData[nRow][m_nAllocLength+j]&tv.m_bVect0[j];
 }
 return tvTag;
}

//--------------------------- MinCover ----------------------------------------
//мн-во ортогон.компонент
CTV CTM::MinCover(const BYTE* mask,int nRow,const CTV& tv) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   tvTag.m_bVect1[j]=m_pData[nRow][j]&tv.m_bVect1[j]&mask[j];
   tvTag.m_bVect0[j]=m_pData[nRow][m_nAllocLength+j]&
       tv.m_bVect0[j]&mask[j];
 }
 return tvTag;
}

//--------------------------- MinCover ----------------------------------------
CTV CTM::MinCover(int nRow,const CBV& bv) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 BYTE part;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   part=bv.GetByteAt(j);
   tvTag.m_bVect1[j]=m_pData[nRow][j]&part;
   tvTag.m_bVect0[j]=m_pData[nRow][m_nAllocLength+j]&(~part);
 }
 return tvTag;
}

//--------------------------- MinCover ----------------------------------------
//мн-во ортогон.компонент
CTV CTM::MinCover(const BYTE* mask,int nRow,const CBV& bv) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int j;
 BYTE part;
 CTV tvTag(m_nBitLength);
 for (j=0; j < m_nByteLength;j++) {
   part=bv.GetByteAt(j);
   tvTag.m_bVect1[j]=m_pData[nRow][j]&part&mask[j];
   tvTag.m_bVect0[j]=m_pData[nRow][m_nAllocLength+j]&(~part)&mask[j];
 }
 return tvTag;
}

//----------------- CTMtoCBM --------------------------------------------------
CBM CTM::CTMtoCBM(void) const    // раскрыть интервалы
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
 int i;
 CBM bm(m_nBitLength,0);
// CBM bmTag(m_nSize,m_nBitLength);
 CBM bmTag(0,m_nBitLength);    // 25.09.2008
 CTV tv(m_nBitLength);
 for(i=0; i < m_nSize; i++) {
   tv=GetRowTv(i); 
   bm=tv.CTVtoCBM();   
   bmTag.Add(bm,-1);
 }  
 return bmTag;
}

//////////////////// Operations on TERNARY VECTOR PARTS ///////////////////////
/////////////////////////// Row Logic operations //////////////////////////////
//--------------------------- AndInPlace --------------------------------------
void CTM::AndInPlace(BOOL Part,const CBV& bv,int Row)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(m_nBitLength == bv.GetBitLength());
#endif
 int i;
 if(Part)
   for (i=0; i < m_nByteLength; i++) 
     m_pData[Row][i]&=bv.GetByteAt(i);
 else 
   for (i=0; i < m_nByteLength; i++) 
     m_pData[Row][m_nAllocLength+i]&=bv.GetByteAt(i);
 return;
}   

//--------------------------- OrInPlace ---------------------------------------
void CTM::OrInPlace(BOOL Part,const CBV& bv,int Row)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(m_nBitLength == bv.GetBitLength());
#endif
 int i;
 if(Part)
   for (i=0; i < m_nByteLength; i++) 
     m_pData[Row][i]|=bv.GetByteAt(i);
 else 
   for (i=0; i < m_nByteLength; i++) 
     m_pData[Row][m_nAllocLength+i]|=bv.GetByteAt(i);
 return;
}   

//--------------------------- XorInPlace --------------------------------------
void CTM::XorInPlace(BOOL Part,const CBV& bv,int Row)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(m_nBitLength == bv.GetBitLength());
#endif
 int i;
 if(Part)
   for (i=0; i < m_nByteLength; i++) 
     m_pData[Row][i]^=bv.GetByteAt(i);
 else 
   for (i=0; i < m_nByteLength; i++) 
     m_pData[Row][m_nAllocLength+i]^=bv.GetByteAt(i);
 return;
}   

//--------------------------- SubInPlace --------------------------------------
void CTM::SubInPlace(BOOL Part,const CBV& bv,int Row)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
  ASSERT(m_nBitLength == bv.GetBitLength());
#endif
 int i;
 if(Part)
   for (i=0; i < m_nByteLength; i++) 
     m_pData[Row][i]&=~bv.GetByteAt(i);
 else 
   for (i=0; i < m_nByteLength; i++) 
     m_pData[Row][m_nAllocLength+i]&=~bv.GetByteAt(i);
 return;
} 

//--------------------------- InvertBitsInPlace -------------------------------
void CTM::InvertBitsInPlace(BOOL Part)
{BYTE maska;
 ASSERT(m_nBitLength != 0);
  int i, j;
 maska=((i=ADR_BIT(m_nBitLength)) == 0)?0xff:(~(0xff>>i));
 if(Part)
   for (i=0;i < m_nSize; i++) {
     for (j=0; j < m_nByteLength; j++) m_pData[i][j]=~m_pData[i][j];
     m_pData[i][m_nAllocLength-1]&=maska;
   } 
 else 
   for (i=0;i < m_nSize; i++) {
     for (j=0; j < m_nByteLength; j++)
        m_pData[i][j]=~m_pData[i][m_nAllocLength+j];
     m_pData[i][m_nAllocLength+m_nByteLength-1]&=maska;
   }
return;
}   

//--------------------------- InvertBitsInPlace -------------------------------
void CTM::InvertBitsInPlace(BOOL Part,int Row)
{int j;
 BYTE maska;
 ASSERT(m_nBitLength != 0);
 maska=((j=ADR_BIT(m_nBitLength)) == 0)?0xff:(~(0xff>>j));
 if(Part) {
   for (j=0; j < m_nByteLength; j++) m_pData[Row][j]=~m_pData[Row][j];
   m_pData[Row][m_nAllocLength-1]&=maska;
 }
 else {
   for (j=0; j < m_nByteLength; j++)
      m_pData[Row][j]=~m_pData[Row][m_nByteLength+j];
   m_pData[Row][m_nAllocLength+m_nByteLength-1]&=maska;
 }
return;
}   

#if 0

//------------------- operator>(const CTM& bm1,const CTM& bm2)
STD(BOOL) operator>(const CTM& bm1,const CTM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
 int i,j;
 for (i=0;i<bm1.m_nSize;i++) {
   for (j=0;j<bm1.m_nBitLength;j++)
     if (bm2.m_pData[i][j] != (bm1.m_pData[i][j] & bm2.m_pData[i][j])) 
       return FALSE;
   for (j=0;j<bm1.m_nBitLength;j++)
     if (bm2.m_pData[i][j] != bm1.m_pData[i][j]) return TRUE;
 }
 return FALSE;
}

//-------------------- operator<(const CTM& bm1,const CTM& bm2)
STD(BOOL) operator<(const CTM& bm1,const CTM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
 int i,j;
 for (i=0;i<bm1.m_nSize;i++) {
   for (j=0;j<bm1.m_nBitLength;j++)
     if (bm1.m_pData[i][j] != (bm2.m_pData[i][j] & bm1.m_pData[i][j])) 
       return FALSE;
   for (j=0;j<bm1.m_nBitLength;j++)
     if (bm2.m_pData[i][j] != bm1.m_pData[i][j]) return TRUE;
 }
 return FALSE;
}

//-------------------- operator>=(const CTM& bm1,const CTM& bm2)
STD(BOOL) operator>=(const CTM& bm1,const CTM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
 int i,j;
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nBitLength;j++)
     if (bm2.m_pData[i][j] != (bm1.m_pData[i][j] & bm2.m_pData[i][j])) 
       return FALSE;
 return TRUE;
}

//-------------------- operator<=(const CTM& bm1,const CTM& bm2)
STD(BOOL) operator<=(const CTM& bm1,const CTM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
 int i,j;
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nBitLength;j++)
     if (bm1.m_pData[i][j] != (bm2.m_pData[i][j] & bm1.m_pData[i][j])) 
       return FALSE;
 return TRUE;
}


#endif
/*
#ifndef _LINUX

///////////////////////////////////////////////////////////////////////////////
// Serialization

//--------------------------------------------------Serialize(CArchive& ar)
void CTM::Serialize(CArchive& ar)
{ 
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  int i,j;
  
  if (ar.IsStoring()) {
    ar << m_nSize;
    ar << m_nBitLength;
    ar << m_nByteLength; 
    for (i=0; i<m_nSize; i++)  
      for (j=0; j<m_nByteLength; j++) {        
        ar << m_pData[i][j];
        ar << m_pData[i][m_nAllocLength+j];
      }
  }
  else {
    ar >> m_nSize;
    ar >> m_nBitLength;
    ar >> m_nByteLength;
    SetSize(m_nSize, m_nBitLength);
    for (i=0; i<m_nSize; i++)  
      for (j=0; j<m_nByteLength; j++) {
        ar >> m_pData[i][j];
        ar >> m_pData[i][m_nAllocLength+j];
      }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Diagnostics
//------------------------------------------------Dump(CDumpContext& dc) const
#ifdef _DEBUG
void CTM::Dump(CDumpContext& dc) const
{
  ASSERT_VALID(this);
#define MAKESTRING(x) #x
    AFX_DUMP1(dc, "a " MAKESTRING(CTM) " with ", m_nSize);
    AFX_DUMP0(dc, " elements");
#undef MAKESTRING
    if (dc.GetDepth() > 0) {
      AFX_DUMP0(dc, "\n");
      for (int i = 0; i < m_nSize; i++) {
        AFX_DUMP1(dc, "\n\t[", i);
        AFX_DUMP1(dc, "] = ",NULL);
      }
    }
}

//---------------------------------------------------AssertValid() const
void CTM::AssertValid() const
{
  if (m_pData == NULL) {
    ASSERT(m_nSize >= 0);
    ASSERT(m_nMaxSize >= 0);
    ASSERT(m_nBitLength >= 0);
    ASSERT(m_nByteLength >= 0);
    ASSERT(m_nAllocLength >= 0);
  }
  else {
    ASSERT(m_nSize >= 0);
    ASSERT(m_nMaxSize >= 0);
    ASSERT(m_nSize <= m_nMaxSize);
    ASSERT(AfxIsValidAddress(m_pData, m_nMaxSize * sizeof(BYTE*)));
    ASSERT(m_nBitLength >= 0);
    ASSERT(m_nByteLength == LEN_BYTE(m_nBitLength));
    ASSERT(m_nAllocLength >= 0);
    ASSERT(m_nGrowBy >= 0);
  }
}
#endif //_DEBUG

#endif //_LINUX
*/

//--------------------------------------------------Serialize(CArchive& ar)
void CTM::Serialize(CArch& ar)
{ 
  int i,j;

  if (ar.IsStoring()) {
    ar << m_nSize;
    ar << m_nBitLength;
    ar << m_nByteLength; 
    for (i=0; i<m_nSize; i++)  
      for (j=0; j<m_nByteLength; j++) {        
        ar << m_pData[i][j];
        ar << m_pData[i][m_nAllocLength+j];
      }
  }
  else {
    ar >> m_nSize;
    ar >> m_nBitLength;
    ar >> m_nByteLength;
    SetSize(m_nSize, m_nBitLength);
    for (i=0; i<m_nSize; i++)  
      for (j=0; j<m_nByteLength; j++) {
        ar >> m_pData[i][j];
        ar >> m_pData[i][m_nAllocLength+j];
      }
  }
}


///////////////////////////////////////////////////////////////////////////////
//********************* Protected functions ***********************************
//---------------------- AllocMatrix-------------------------------------------
void CTM::AllocMatrix(int nRow, int nColumn)
{ ASSERT(nRow >= 0 && nColumn >= 0);
  m_pData = (BYTE**) new BYTE[nRow * sizeof(BYTE*)];
  int i,nByte= LEN_BYTE(nColumn);
  for (i=0; i < nRow; i++) {
    m_pData[i] = (BYTE*) new BYTE[nByte<<1];
    memset(m_pData[i], 0, nByte<<1);                   // zero fill
  }
  m_nSize = m_nMaxSize = nRow;
  m_nBitLength = nColumn;
  m_nAllocLength = m_nByteLength = nByte;
}
/*
#ifndef _LINUX
//----------------- CharBit ---------------------------------------------------
void CTM::CharBit(const CString s)
{int j = 0,i = 0,max = 0;
 CString w = s;
 if (w.GetLength() <= 2) { Init(); return; }
 if (w[w.GetLength()-2] != '\r') w = w + "\r\n";
 while (!w.IsEmpty()) {
   j = w.Find('\n');
   if (j > max + 1) max = j - 1;
   w = w.Right(w.GetLength() - j-1);
   i++;
 }
 if (m_pData == NULL) AllocMatrix(i,max);
 else { SetSize(i,max,m_nGrowBy); Clear(); }
 w = s; j = 0;
 if (w[w.GetLength()-2] != '\r') w = w + "\r\n";
 while (!w.IsEmpty()) {
   max = w.Find('\n');
   for(i=0;i < max-1;i++) {
     ASSERT(w[i] == '0' || w[i] == '1' || w[i] == '-' || w[i] == '+');
     if(w[i] != '-') SetBitAt(j,i,w[i]);
   } 
   w = w.Right(w.GetLength() - max-1);
   j++;
 }
}    

//----------------- CharBit ---------------------------------------------------
void CTM::CharBit(const CStringArray& s)   //new 11.02
{int k,i,max = 0;
 if (s.GetSize()==0) { Init(); return;}
 for (k=0; k < s.GetSize(); k++) {
   i = s[k].GetLength();
   if (i > max) max = i;    // max - length of row
 }  
 if (m_pData == NULL) AllocMatrix(s.GetSize(),max);
 else { SetSize(s.GetSize(),max,m_nGrowBy); Clear(); }
 for (k=0; k < s.GetSize(); k++) { 
   max = s[k].GetLength();
   CString w(s[k]);
   for(i=0;i < max;i++) {
     ASSERT(w[i] == '0' || w[i] == '1' || w[i] == '-' || w[i] == '+');
     if (w[i] != '-') SetBitAt(k,i,w[i]);
   }
 }
}
#endif //_LINUX
*/

void CTM::CharBit(char* s)
{
  int j = 0, i = 0, max = 0, len;
  char* w, *pdest;
  len = strlen(s);
  w =(char*)malloc(len+2);
#ifndef _MSVC9
    strcpy(w,s);
#else
    strcpy_s(w, len+2, s);
#endif
  if (len <= 2) { Init(); return; }
  if (w[len-2] != '\r') 
#ifndef _MSVC9
	  strcat(w,"\r\n");
#else
    strcpy_s(w, len+2, "\r\n");
#endif
  while (w[0] !=0) {
    pdest = strchr(w,'\n');
    j = (int)(pdest - w + 1);
    if (j > max + 1) max = j - 1;
#ifndef _MSVC9
    strcpy(w, pdest+1);
#else
    strcpy_s(w, len+2, pdest+1);
#endif
    i++;
  }

  if (m_pData == NULL) AllocMatrix(i,max);
  else { SetSize(i,max,m_nGrowBy); Clear(); }
#ifndef _MSVC9
    strcpy(w,s); 
#else
    strcpy_s(w, len+2, s);
#endif
  j = 0;
  if (w[len-2] != '\r') 
#ifndef _MSVC9
    strcat(w,"\r\n");
#else
    strcat_s(w, len+2, "\r\n");
#endif
  while (w[0]!='\0') {
    pdest = strchr(w,'\n');
    max = (int)(pdest - w + 1);
    for(i=0;i < max-1;i++) {
      ASSERT(w[i] == '0' || w[i] == '1' || w[i] == '-' || w[i] == '+');
      if(w[i] != '-') SetBitAt(j,i,w[i]);
    } 
#ifndef _MSVC9
    strcpy(w, pdest+1);
#else
    strcpy_s(w, len+2, pdest+1);
#endif
    j++;
  }
}    

//----------------- SafeDelete() ----------------------------------------------
void CTM::SafeDelete()
{ int i;
  for (i=0; i < m_nSize; i++) { 
    if (m_pData[i]!=NULL) delete [] (BYTE*)m_pData[i]; }
  if(m_nMaxSize > 0) delete [] (BYTE*)m_pData;
  m_pData=NULL;
}

