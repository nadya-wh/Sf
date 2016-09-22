/////////////////////////////////////////////////////////////////////
// Ver.2.1.3    28.03.2008
//              SetOnes и SetZeros - исправлены для реализации в другой интерпретации
//              в виде функций AddOnes и AddZeros
// Ver.2.1.2    26.12.2007
//              LeftUnDef - исправлены ошибки поиска в последнем байте вектора
//
// Ver.2.1.1    22.02.2007
//              AssignCopy - добавил чистку пространства от Len до AllocLen
//
// Ver.2.1.0     07.07.2006
//               Переход к LINUX

// Ver.2.0.9     19.06.2006
//               Поправки BitChar с ограничением длины строки
// Ver.2.0.5     02.04.2006
//               поправка старой ошибки в Zeros
// Ver.2.0.3     24.03.2006
//               поправка операции чтения из архива (отказ от WORD)
// Ver.2.0.2     21.03.2006
//               - Генераторы случайных перестроены на возврат *this
//               Поправка в SetSize - сразу возврат, если новый равен старому
//               Поправка в SetVect - разрешена запись в пустой вектор
// Ver.2.0.0     04.03.2006
//               - Добавлены генераторы случайных

// Ver.1.1.1     03.12.2004
// Ver.1.1.0     21.11.2004   Russian == English

// Ver.1.0.3     10.01.2002
//               IsCorrect, Conc, SetSize

/////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#ifdef _JOINT
#include "BaseBool.h"
#include "BaseTern.h"
#include "archive.h"
#else
#include "../Common/BaseBool.h"
#include "../Common/BaseTern.h"
#include "../Com_Lib/archive.h"
#endif

/*
#ifndef _LINUX

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#define new DEBUG_NEW

#endif //_LINUX
*/
///////////////////////////////////////////////////////////////////////////////
// Генерация псевдослучайного <nCol>-компонентного троичного
// вектора с равномерным распределением значений '-','0'и'1'
// по компонентам c вероятностями p(-)=1/2 и p(0)=p(1)=1/4.
//-----------------------------------------------------------------------------

CTV CTV::GenRtv(int nCol)
{ 
  CBV Rbv, Rbv_1;
  Rbv.GenRbv (nCol);
  Rbv_1 = Rbv & Rbv_1.GenRbv (nCol);
  SetSize(nCol);
  SetVect(Rbv_1, Rbv & ~Rbv_1);
  return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Генерация псевдослучайного <nCol>-компонентного троичного
// вектора со средним числом <nRang> определенных компонент
// (дисперсия 1/16), принимающих значение'0'или'1'c вероятн.1/2
// (при nRang>=nCol все комп. вектора имеют определенное значение) 
//-----------------------------------------------------------------------------

CTV CTV::GenRtvMid(int nCol, int nRang)
{
  CBV Rbv, Rbv_1; 
  Rbv.GenRbvMid(nCol, nRang); 
  Rbv_1 = Rbv & Rbv_1.GenRbv(nCol);
  SetSize(nCol);
  SetVect(Rbv_1, Rbv & ~Rbv_1);
  return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Генерация псевдослучайного <nCol>-компонентного троичного 
// вектора с фиксированным числом <nRang> определенных компо-
// нент, принимающих значение '0' или '1' c вероятностью 1/2 
// (при nRang>=nCol все комп. вектора имеют определенное значение) 
//-----------------------------------------------------------------------------

CTV CTV::GenRtvFix (int nCol, int nRang)
{
  CBV Rbv, Rbv_1; 
  Rbv.GenRbvFix(nCol, nRang); 
  Rbv_1 = Rbv & Rbv_1.GenRbv(nCol);
  SetSize(nCol);
  SetVect(Rbv_1, Rbv & ~Rbv_1);
  return *this;
}


//////////////////////// Construction/Destruction /////////////////////////////

//------------------------ CTV ------------------------------------------------
CTV::CTV()   { Init(); }

//------------------------ CTV (copy of CTV) ----------------------------------
CTV::CTV(const CTV& tvSrc)
{
 if (tvSrc.m_nByteLength == 0) Init();
 else {
   AllocBuffer(tvSrc.m_nByteLength);
   memcpy(m_bVect1, tvSrc.m_bVect1,tvSrc.m_nByteLength);
   memcpy(m_bVect0, tvSrc.m_bVect0,tvSrc.m_nByteLength);
   m_nBitLength = tvSrc.m_nBitLength;
 }
}

//------------------------ CTV (copy of boolean vectors pair) -----------------
CTV::CTV(const CBV& bv1Src,const CBV& bv0Src)
{ int len=bv1Src.GetByteLength();
 ASSERT(bv1Src.GetBitLength() == bv0Src.GetBitLength());
 if (len == 0) Init();
 else {
   AllocBuffer(len);
   memcpy(m_bVect1, (BYTE*)bv1Src,len);
   memcpy(m_bVect0, (BYTE*)bv0Src,len);
   m_nBitLength = bv1Src.GetBitLength();
 }
}

//------------------------ CTV (array of bits) --------------------------------
CTV::CTV(int nLenBit,char value)
{ int nLenByte,w;
 BYTE a1,a0;
  if (nLenBit < 1) Init();      // return empty vector if invalid repeat count
  else {
    nLenByte = LEN_BYTE(nLenBit);
    AllocBuffer(nLenByte);
    m_nBitLength = nLenBit;
     switch (value) {
   case '1': a1=0xff; a0=0; break;
   case '0': a1=0; a0=0xff; break;
   case '-': a1=0; a0=0; break;
   case '+': a1=1; a0=1; break;
   default: return;
     }
   memset(m_bVect1,a1,nLenByte); memset(m_bVect0,a0,nLenByte);
   if (w = ADR_BIT(nLenBit)) {
     m_bVect1[nLenByte-1] = (m_bVect1[nLenByte-1]>>(S_1-w))<<(S_1-w);
     m_bVect0[nLenByte-1] = (m_bVect0[nLenByte-1]>>(S_1-w))<<(S_1-w);
   }
  }
}

//------------------------ CTV (copy of byte arrays pair) ---------------------
CTV::CTV(const BYTE* pbt1,const BYTE* pbt0,int nLenBit)
{ int nLenByte;
  if (nLenBit==0) Init();
  else   {
    nLenByte = LEN_BYTE(nLenBit);
    AllocBuffer(nLenByte);
    memcpy(m_bVect1, pbt1, nLenByte);
    memcpy(m_bVect0, pbt0, nLenByte);
    m_nBitLength = nLenBit;
  }
}

//------------------------ CTV (copy of chararray) ----------------------------
CTV::CTV(const char* pch)
{ int nLenByte,nLenBit;
  if ((nLenBit = strlen((char*)pch)) == 0) Init();
  else   {
    nLenByte = LEN_BYTE(nLenBit);        // Translate from symbol to bit
    AllocBuffer(nLenByte);
    CharBit(nLenByte,pch);
    m_nBitLength = nLenBit;
  }
}

//-------------------------~CTV() ---------------------------------------------
CTV::~CTV()       //  free any attached data
{ if (m_bVect1 != NULL) delete [] m_bVect1;  }


///////////////////////////////////////////////////////////////////////////////
/////////////////////////// Data Reading //////////////////////////////////////

//------------------------- GetBitAt ------------------------------------------
char CTV::GetBitAt(int nIndex) const
{ ASSERT(nIndex >= 0); ASSERT(nIndex < m_nBitLength);
  BYTE a1, a0;
  a1=m_bVect1[BIT_BYTE(nIndex)] & OB[ADR_BIT(nIndex)];
  a0=m_bVect0[BIT_BYTE(nIndex)] & OB[ADR_BIT(nIndex)];
  if(a1 != 0 && a0 == 0)  return '1';
  if(a0 != 0 && a1 == 0)  return '0';
  if(a0 == 0 && a1 == 0)  return '-';
  return '+';
}

//----------------------------  Extract ---------------------------------------
CTV CTV::Extract(int nFirst,int nCount)
{ CTV s;
 s.Extr(m_bVect1,m_bVect0,m_nBitLength,nFirst,nCount);  return s;
}

//------------------------- BitChar -------------------------------------------
/*
#ifndef _LINUX
CString CTV::BitChar(char One,char Zero,char Dont,char Rest,int Max)
{ int i,j=0;
  CString res('\0',m_nBitLength);
  for (i=0; i < m_nBitLength; i++, j++)
  {
    switch (GetBitAt(i)) {
     case '1': res.SetAt(j,One); break;
     case '0': res.SetAt(j,Zero); break;
     case '-': res.SetAt(j,Dont); break;
     default:  res.SetAt(j,Rest);
    }
    if (Max!=0)
      if ((Max==1)||((i+1)%Max == 0))
      { res.SetAt(++j,'\n'); }  //res.SetAt(++j,'\r'); 
  }
  return res;
}
#else
*/
char* CTV::BitChar(char One,char Zero,char Dont,char Rest,int Max)
{ int i,j=0;
  if (Max!=0)  j = m_nBitLength/Max + 1;
  char *res;
  res = (char *)malloc (m_nBitLength + 1 + 2*j);
  memset(res, 0 , (m_nBitLength + 1 + 2*j));
  for (i=0; i < m_nBitLength; i++, j++)
  {
    switch (GetBitAt(i)) {
     case '1': res[j] = One; break;
     case '0': res[j] = Zero; break;
     case '-': res[j] = Dont; break;
     default:  res[j] = Rest;
    }
    if (Max!=0)
      if ((Max==1)||((i+1)%Max == 0))
      { res[++j] = '\n'; }   // res[++j] = '\r'; 
  }
  return res;
}

//#endif //_LINUX

//-----------------------------------------------
CBV CTV::GetDefs() const
{ CBV bv(m_bVect0, m_nBitLength);
 int i;
 for(i=0; i < m_nByteLength; i++)
    bv.SetByteAt(i,m_bVect1[i]|m_bVect0[i]);
 return bv;
}

//------------------------- CBV GetUnDefs -------------------------------------
CBV CTV::GetUnDefs() const
{ CBV bv(m_bVect0, m_nBitLength);
 int i;
 for(i=0; i < m_nByteLength-1; i++)
    bv.SetByteAt(i,~(m_bVect1[i]|m_bVect0[i]));
 BYTE maska=0xff;
 if ((i = ADR_BIT(m_nBitLength)) > 0 ) {
   i = S_1 - i; maska = 0xff << i; }
 bv.SetByteAt(m_nByteLength-1,
 (~(m_bVect1[m_nByteLength-1]|m_bVect0[m_nByteLength-1]))&maska);
  return bv;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// Data Writing //////////////////////////////////////

//---------------------------- SetBitAt ---------------------------------------
void CTV::SetBitAt(int nIndex, char symb)
{ ASSERT(nIndex >= 0 && nIndex < m_nBitLength);
  int i,j;
  i=BIT_BYTE(nIndex); j=ADR_BIT(nIndex);
  switch (symb) {
     case '1': m_bVect1[i] |=OB[j]; m_bVect0[i] &=~OB[j];break;
     case '0': m_bVect0[i] |=OB[j]; m_bVect1[i] &=~OB[j]; break;
     case '-': m_bVect1[i] &=~OB[j]; m_bVect0[i] &=~OB[j]; break;
     default:  m_bVect1[i] |=OB[j]; m_bVect0[i] |=OB[j];
    }
}

//--------------------------- SetVect -----------------------------------------
void CTV::SetVect(CBV bv1Src,CBV bv0Src)  //new  =-1-неудача
{ 
  int len=bv1Src.GetByteLength();
  ASSERT(bv1Src.GetBitLength()==bv0Src.GetBitLength()&&
        (len<= m_nAllocLength||m_nAllocLength==0));
 if (len == 0) Init();
 else {
   SetSize(bv1Src.GetBitLength());        // 20.03.2006
   memcpy(m_bVect1,(BYTE*)bv1Src,len);
   memcpy(m_bVect0,(BYTE*)bv0Src,len);
 }
}

//--------------------------- SetOnes -----------------------------------------
void CTV::SetOnes(const CBV& bv)
{int i;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=0; i < m_nByteLength; i++) {
   m_bVect1[i] = bv.GetByteAt(i); m_bVect0[i] &= ~bv.GetByteAt(i);
 }
}

//--------------------------- SetZeros ----------------------------------------
void CTV::SetZeros(const CBV& bv)
{int i;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=0; i < m_nByteLength; i++) {
   m_bVect0[i] = bv.GetByteAt(i); m_bVect1[i] &= ~bv.GetByteAt(i);
 }
}

//--------------------------- AddOnes -----------------------------------------
void CTV::AddOnes(const CBV& bv)
{int i;
ASSERT(m_nBitLength==bv.GetBitLength());
for (i=0; i < m_nByteLength; i++) {
  m_bVect1[i]|=bv.GetByteAt(i); m_bVect0[i]&=~bv.GetByteAt(i);
}
}

//--------------------------- AddZeros ----------------------------------------
void CTV::AddZeros(const CBV& bv)
{int i;
ASSERT(m_nBitLength==bv.GetBitLength());
for (i=0; i < m_nByteLength; i++) {
  m_bVect0[i]|=bv.GetByteAt(i); m_bVect1[i]&=~bv.GetByteAt(i);
}
}

//--------------------------- SetUnDefs ---------------------------------------
void CTV::SetUnDefs(const CBV& bv)
{int i;
 BYTE maska;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=0; i < m_nByteLength; i++) {
     maska=~bv.GetByteAt(i);
   m_bVect1[i]&=maska; m_bVect0[i]&=maska;
 }
}

//--------------------------- SetOnes -----------------------------------------
void CTV::SetOnes(const BYTE* bv)
{int i;
  for (i=0; i < m_nByteLength; i++) {
   m_bVect1[i] = bv[i]; m_bVect0[i] &= ~bv[i];
 }
}

//--------------------------- SetZeros ----------------------------------------
void CTV::SetZeros(const BYTE* bv)
{int i;
 for (i=0; i < m_nByteLength; i++) {
   m_bVect0[i] = bv[i]; m_bVect1[i] &= ~bv[i];
 }
}

//--------------------------- AddOnes -----------------------------------------
void CTV::AddOnes(const BYTE* bv)
{int i;
for (i=0; i < m_nByteLength; i++) {
  m_bVect1[i] |= bv[i]; m_bVect0[i] &= ~bv[i];
}
}

//--------------------------- AddZeros ----------------------------------------
void CTV::AddZeros(const BYTE* bv)
{int i;
for (i=0; i < m_nByteLength; i++) {
  m_bVect0[i] |= bv[i]; m_bVect1[i] &= ~bv[i];
}
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// Operators and functions of assignment /////////////

//--------------------------- Clear -------------------------------------------
void CTV::Clear(char symb)
{ ASSERT(m_nByteLength > 0);
  BYTE maska=0xff;
  int i;
  if ((i = ADR_BIT(m_nBitLength)) > 0 ) {
    i = S_1 - i; maska = 0xff << i; }
  memset(m_bVect1,0xff,m_nByteLength-1); m_bVect1[m_nByteLength-1]=maska;
  memset(m_bVect0,0xff,m_nByteLength-1); m_bVect0[m_nByteLength-1]=maska;
  switch (symb) {
    case '1': memset(m_bVect0,0,m_nByteLength); break;
    case '0': memset(m_bVect1,0,m_nByteLength); break;
    case '-': memset(m_bVect1,0,m_nByteLength); 
              memset(m_bVect0,0,m_nByteLength); break;
  }
}

///////////////////////////////////////////////////////////////////////////////
////////// Operators and functions of expanded assignment /////////////////////

const CTV&  CTV::operator <<=(int nShift)
{ int i,j,l_bit,r_bit,nNewLenByte,nNewLenBit,AdrBeg;
 ASSERT(nShift >= 0 && m_nBitLength >= nShift);
 nNewLenBit = m_nBitLength - nShift;
 if (nNewLenBit==0) {
   memset(m_bVect1,0,m_nByteLength); 
   memset(m_bVect0,0,m_nByteLength); 
   return *this;
 }
 if (nShift==0) return *this;
 nNewLenByte = LEN_BYTE(nNewLenBit);
 AdrBeg = BIT_BYTE(nShift);
 l_bit = ADR_BIT(nShift);
 if (l_bit)    {
   r_bit = S_1 - l_bit;  j=0;
   for (i=AdrBeg; i<m_nByteLength-1; i++,j++) {
     m_bVect1[j] = (m_bVect1[i] << l_bit) | (m_bVect1[i+1] >> r_bit);
     m_bVect0[j] = (m_bVect0[i] << l_bit) | (m_bVect0[i+1] >> r_bit);
   }
   if ((j*S_1) < nShift) {
     m_bVect1[nNewLenByte-1] = m_bVect1[m_nByteLength-1] << l_bit;
     m_bVect0[nNewLenByte-1] = m_bVect0[m_nByteLength-1] << l_bit;
   }
 }
 else {
   memcpy(m_bVect1, m_bVect1+AdrBeg, nNewLenByte);
   memcpy(m_bVect0, m_bVect0+AdrBeg, nNewLenByte);
 }
 i= m_nByteLength - nNewLenByte;
 memset(m_bVect1+nNewLenByte, 0, i);
 memset(m_bVect0+nNewLenByte, 0, i);
 return *this;
}

//--------------------------- operator >> -------------------------------------
const CTV& CTV::operator >>=(int nShift)
{int i,j,l_bit,r_bit,AdrBeg;
 BYTE *work1, *work0;
 ASSERT(nShift >= 0);
 ASSERT((m_nBitLength - nShift) >= 0);
 if (nShift == 0) return *this;
 if ((m_nBitLength - nShift)==0)  {
   memset(m_bVect1,0,m_nByteLength);
   memset(m_bVect0,0,m_nByteLength); return *this;
   }                                            // may throw an exception
 work1 = new BYTE[m_nByteLength<<1]; work0 = work1+m_nByteLength;
 memset(work1,0,m_nByteLength<<1);
 AdrBeg = BIT_BYTE(nShift);
 r_bit = ADR_BIT(nShift);
 if (r_bit)    {
   l_bit = S_1 - r_bit;  j=0; work1[AdrBeg] = (m_bVect1[0] >> r_bit);
   work0[AdrBeg] = (m_bVect0[0] >> r_bit);
   for (i=AdrBeg+1; i<m_nByteLength; i++) {
     work1[i] = (m_bVect1[j] << l_bit) | (m_bVect1[j+1] >> r_bit); j++;
     work0[i] = (m_bVect0[j] << l_bit) | (m_bVect0[j+1] >> r_bit); j++;
   }
 }
 else {
   memcpy(work1,m_bVect1,m_nByteLength-AdrBeg);
   memcpy(work0,m_bVect0,m_nByteLength-AdrBeg);
 }
 if (i=ADR_BIT(m_nBitLength)) {
   work1[m_nByteLength-1] = (work1[m_nByteLength-1] >> (S_1-i)) << (S_1-i);
   work0[m_nByteLength-1] = (work0[m_nByteLength-1] >> (S_1-i)) << (S_1-i);
 }
 memcpy(m_bVect1,work1, m_nByteLength); memcpy(m_bVect0,work0, m_nByteLength);
 delete [] work1;
 return *this;
}

//--------------------------- operator &= -------------------------------------
const CTV& CTV::operator &=(const BYTE* maska)
{ ASSERT(m_nBitLength > 0);
 int i;
 for (i=0; i<m_nByteLength; i++) {
   m_bVect1[i] &= maska[i];
   m_bVect0[i] &= maska[i];
 }
 return *this;
}

//--------------------------- operator &= -------------------------------------
const CTV& CTV::operator &=(const CBV& maska)
{ ASSERT(m_nBitLength == maska.GetBitLength());
 int i;
 for (i=0; i<m_nByteLength; i++) {
   m_bVect1[i] &= maska.GetByteAt(i);
   m_bVect0[i] &= maska.GetByteAt(i);
 }
 return *this;
}

//--------------------------- operator &= -------------------------------------
const CTV& CTV::operator &=(const CTV& tv)
{ ASSERT(m_nBitLength == tv.GetBitLength());
 int i;
 for (i=0; i < m_nByteLength; i++) {
   m_bVect1[i] |= tv.m_bVect1[i];     //??????????
   m_bVect0[i] |= tv.m_bVect0[i];
 }
 return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Logic operations ////////////////////////////////
//--------------------------- operator << -------------------------------------

//--------------------------- operator << ------------------------------
STD(CTV) operator <<(const CTV& tv, int nShift)
{ CTV s;
 s.LeftShiftVect(tv.m_bVect1, tv.m_bVect0, tv.m_nBitLength, nShift);  return s;
}

//--------------------------- operator >> ------------------------------
STD(CTV) operator >>(const CTV& tv, int nShift)
{ CTV s;
 s.RightShiftVect(tv.m_bVect1,tv.m_bVect0,tv.m_nBitLength,nShift);  return s;
}

//--------------------------- operator &= ------------------------------
STD(CTV) operator&=(const CTV& tv, const BYTE* maska)
{
 CTV tvTag(tv);
 tvTag &= maska;
 return tvTag;
}

//--------------------------- operator &= ------------------------------
STD(CTV) operator&=(const CTV& tv, const CBV& maska)
{ASSERT(tv.m_nBitLength == maska.GetBitLength());
 CTV tvTag(tv);
 tvTag &= maska;
 return tvTag;
}

//--------------------------- operator &= ------------------------------
STD(CTV) operator&=(const CTV& tv1, const CTV& tv2)
{ASSERT(tv1.m_nBitLength == tv2.GetBitLength());
 CTV tvTag(tv1);
 tvTag &= tv2;
 return tvTag;
}

//--------------------------- LoopLeftShift -----------------------------------
void CTV::LoopLeftShift(int nShift)
{  // LeftShift ternary vector  -- assume that 'this' is a new CTV object
 int i,j,l_bit,r_bit,nNewLenByte,nNewLenBit,nEndLenByte;
 BYTE *work1, *work0;
 ASSERT(nShift >= 0);
 nNewLenBit = m_nBitLength - nShift;
 ASSERT(nNewLenBit >= 0);
 if (nNewLenBit==0 || nShift==0) return;
 nNewLenByte = LEN_BYTE(nNewLenBit);
 work1 = new BYTE[m_nByteLength<<1];               // may throw an exception
 work0=work1+m_nByteLength;
 memset(work1,0,m_nByteLength<<1);
 i = BIT_BYTE(nShift);
 l_bit = ADR_BIT(nShift);
 if (l_bit)    {
   r_bit = S_1 - l_bit;
   memcpy(work1, m_bVect1+i, nNewLenByte+1);
   memcpy(work0, m_bVect0+i, nNewLenByte+1);
   for (i=0; i<nNewLenByte; i++)  {
     work1[i] = (work1[i] << l_bit) | (work1[i+1] >> r_bit);
     work0[i] = (work0[i] << l_bit) | (work0[i+1] >> r_bit);
   }
   work1[i] <<= l_bit; work0[i] <<= l_bit;
 }
 else  {
   memcpy(work1, m_bVect1+i, nNewLenByte);
   memcpy(work0, m_bVect0+i, nNewLenByte);
 }
 nEndLenByte = m_nByteLength - nNewLenByte;
 j = BIT_BYTE(nNewLenBit);
 r_bit = ADR_BIT(nNewLenBit);
 if (r_bit == 0 ) {
   memcpy(work1+j, m_bVect1, nEndLenByte);
   memcpy(work0+j, m_bVect0, nEndLenByte);
 }
 else {
   l_bit = S_1 - r_bit;
   work1[j] = (work1[j] >> l_bit) << l_bit;
   work0[j] = (work0[j] >> l_bit) << l_bit;
   work1[j++] |= (m_bVect1[0] >> r_bit);
   work0[j++] |= (m_bVect0[0] >> r_bit);
   for (i=0; i<nEndLenByte; i++) {
     work1[j++] = (m_bVect1[i] << l_bit) | (m_bVect1[i+1] >> r_bit);
     work0[j++] = (m_bVect0[i] << l_bit) | (m_bVect0[i+1] >> r_bit);
   }
   j = S_1 - ADR_BIT(m_nBitLength);
   work1[m_nByteLength-1] = (work1[m_nByteLength-1] >> j)<<j;
   work0[m_nByteLength-1] = (work0[m_nByteLength-1] >> j)<<j;
 }
 memcpy(m_bVect1,work1, m_nByteLength); memcpy(m_bVect0,work0, m_nByteLength);
 delete [] work1;
}

//--------------------------- LoopRightShift ----------------------------------
void CTV::LoopRightShift(int nShift)
{  // LeftShift bool vector  -- assume that 'this' is a new CTV object
  int i,j,l_bit,r_bit,AdrBeg,AdrEnd,BitEnd;
  BYTE *work1, *work0;
  ASSERT(nShift >= 0 && m_nBitLength >= nShift);
  if (nShift == 0) return;
  if ((m_nBitLength - nShift)==0) {
    memset(m_bVect1,0,m_nByteLength);
    memset(m_bVect0,0,m_nByteLength); return;
  }
  work1 = new BYTE[m_nByteLength<<1];    // may throw an exception
  work0=work1+m_nByteLength;
  memset(work1,0,m_nByteLength<<1);
  AdrBeg = BIT_BYTE(nShift);
  AdrEnd = BIT_BYTE(m_nBitLength - nShift);
  BitEnd = ADR_BIT(m_nBitLength);
  if (BitEnd < nShift) {
    l_bit = ADR_BIT(m_nBitLength - nShift);
    r_bit = S_1 - l_bit;     j = AdrEnd;
    for (i=0; i<AdrBeg+1; i++,j++) {
      work1[i] = (m_bVect1[j] << l_bit) | (m_bVect1[j+1] >> r_bit);
      work0[i] = (m_bVect0[j] << l_bit) | (m_bVect0[j+1] >> r_bit);
    }
  }
  else {
    l_bit = BitEnd - nShift; work1[0] = m_bVect1[AdrEnd] << l_bit;
    work0[0] = m_bVect0[AdrEnd] << l_bit;
  }
  r_bit = ADR_BIT(nShift); l_bit = S_1 - r_bit;
  work1[AdrBeg] = (work1[AdrBeg] >> l_bit) << l_bit;
  work0[AdrBeg] = (work0[AdrBeg] >> l_bit) << l_bit;
  if (r_bit)    {
    j=0;
    work1[AdrBeg] |= (m_bVect1[0] >> r_bit);
    work0[AdrBeg] |= (m_bVect0[0] >> r_bit);
    for (i=AdrBeg+1; i < m_nByteLength; i++,j++) {
      work1[i] = (m_bVect1[j] << l_bit) | (m_bVect1[j+1] >> r_bit);
      work0[i] = (m_bVect0[j] << l_bit) | (m_bVect0[j+1] >> r_bit);
    }
  }
  else {
    memcpy(work1+AdrBeg, m_bVect1, m_nByteLength-AdrBeg);
    memcpy(work0+AdrBeg, m_bVect0, m_nByteLength-AdrBeg);
  }
  if (i=ADR_BIT(m_nBitLength)) {
    work1[m_nByteLength-1] = (work1[m_nByteLength-1] >> (S_1-i)) << (S_1-i);
    work0[m_nByteLength-1] = (work0[m_nByteLength-1] >> (S_1-i)) << (S_1-i);
  }
  memcpy(m_bVect1,work1, m_nByteLength); memcpy(m_bVect0,work0, m_nByteLength);
  delete [] work1;
}

//--------------------------- InvertDefBit ------------------------------------
void CTV::InvertDefs()
{ASSERT(m_nByteLength > 0);
    int i;
 BYTE a;
 for(i=0; i < m_nByteLength; i++) {
   a=m_bVect0[i]; m_bVect0[i]=m_bVect1[i]; m_bVect1[i]=a;
 }
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////// Logic Operations of weighting, finding and casing /////

//--------------------------- CountDefs ---------------------------------------
int CTV::CountDefs() const
{ int j,one=0;
 for (j=0; j < m_nByteLength; j++) one+=COUNTBIT(m_bVect1[j]|m_bVect0[j]);
 return one;
}

//--------------------------- CountUnDefs -------------------------------------
int CTV::CountUnDefs() const
{ int j,one=0;
 for (j=0; j < m_nByteLength; j++) one+=COUNTBIT(m_bVect1[j]|m_bVect0[j]);
 return m_nBitLength-one;
}

//--------------------------- CountOnes ---------------------------------------
int CTV::CountOnes() const
{ int j,one=0;
 for (j=0; j < m_nByteLength; j++) one+=COUNTBIT(m_bVect1[j]);
 return one;
}

//--------------------------- CountZeros --------------------------------------
int CTV::CountZeros() const
{ int j,one=0;
 for (j=0; j < m_nByteLength; j++) one+=COUNTBIT(m_bVect0[j]);
 return one;
}

//--------------------------- LeftUnDef ---------------------------------------
int CTV::LeftUnDef(int nNext) const
{int i,j,pos = 0;
 BYTE ch, vec, ch_last;
 ASSERT(m_nBitLength >= nNext);
 if(m_nBitLength == (nNext+1)) return (-1);
 i=ADR_BIT(m_nBitLength);
 ch_last = ~(m_bVect1[m_nByteLength-1]|m_bVect0[m_nByteLength-1]);
 if (i) ch_last = (ch_last >>(S_1-i))<<(S_1-i);
 if (nNext == -1) { i=0; j=0; }
 else {i=BIT_BYTE(nNext);  j=ADR_BIT(nNext); j++;}
 if (j == S_1) { j=0;  i++; }
 if (i == m_nByteLength)  return (-1);
 ch=(i == m_nByteLength-1)?
    (ch_last << j):((~(m_bVect1[i]|m_bVect0[i])) << j);
 if (ch)  {
   while (!(ch & OB[pos++]));
   return (nNext + pos);
 }
 for (j=i+1; j < m_nByteLength-1; j++)
   if ((vec=~(m_bVect1[j]|m_bVect0[j])))  {
     while (!(vec & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 if (j < m_nByteLength)
   if(ch_last) {
     while (!(ch_last & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 return (-1);
}

//--------------------------- LeftDef -----------------------------------------
int CTV::LeftDef(int nNext) const
{int i,j,pos = 0;
 BYTE ch, vec;
 ASSERT(m_nBitLength >= nNext);
 if (nNext == -1) { i=0;j=0; }
 else {i=BIT_BYTE(nNext);  j=ADR_BIT(nNext); j++;}
 if (j==S_1)  {  j=0;  i++; }
 if (i == m_nByteLength)  return (-1);
 ch = (m_bVect1[i]|m_bVect0[i]) << j;
 if (ch)  {
   while (!(ch & OB[pos++]));
   return (nNext + pos);
 }
 for (j=i+1;j < m_nByteLength;j++)
   if ((vec=m_bVect1[j]|m_bVect0[j]))  {
     while (!(vec & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 return (-1);
}

//--------------------------- LeftOne -----------------------------------------
int CTV::LeftOne(int nNext) const
{int i,j,pos = 0;
 BYTE ch;
 ASSERT(m_nBitLength >= nNext);
 if (nNext == -1) {i=0;j=0;}
 else {i=BIT_BYTE(nNext);  j=ADR_BIT(nNext); j++;}
 if (j==S_1)  {j=0;  i++; }
 if (i==m_nByteLength)  return (-1);
 ch = m_bVect1[i] << j;
 if (ch)  {
   while (!(ch & OB[pos++]));
   return (nNext + pos);
 }
 for (j=i+1;j<m_nByteLength;j++)
   if (m_bVect1[j])  {
     while (!(m_bVect1[j] & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 return (-1);
}

//--------------------------- LeftZero ----------------------------------------
int CTV::LeftZero(int nNext) const
{int i,j,pos = 0;
 BYTE ch;
 ASSERT(m_nBitLength >= nNext);
 if (nNext == -1) {i=0;j=0;}
 else {i=BIT_BYTE(nNext);  j=ADR_BIT(nNext); j++;}
 if (j==S_1)  {j=0;  i++; }
 if (i==m_nByteLength)  return (-1);
 ch = m_bVect0[i] << j;
 if (ch)  {
   while (!(ch & OB[pos++]));
   return (nNext + pos);
 }
 for (j=i+1; j<m_nByteLength; j++)
   if (m_bVect0[j])  {
     while (!(m_bVect0[j] & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 return (-1);
}

//--------------------------- RightUnDef --------------------------------------
int CTV::RightUnDef(int nPrev) const
{int i, j, pos = 7, ii;
 BYTE ch, vec;
 ASSERT(m_nBitLength >= nPrev);
 if (nPrev == -1) { i=m_nByteLength-1; j=0; }
 else {i=BIT_BYTE(nPrev);  j=S_1 - ADR_BIT(nPrev);}
 if (j==8)  {j=0;  i--; }
 ch = ~(m_bVect1[i] | m_bVect0[i]);
 if(i == m_nByteLength-1) {
   ii=ADR_BIT(m_nBitLength);
   ch=(ch>>(S_1-ii))<<(S_1-ii);
 }
 ch = (ch>> j) << j;
 if (ch)  {
   while (!(ch & OB[pos--]));
   return ((i*S_1)+pos+1);
 }
 for (j=i-1; j>=0; j--)
   if((vec=~(m_bVect1[j] | m_bVect0[j])))  {
     while (!(vec & OB[pos--]));
     return ((j*S_1)+pos+1);
   }
 return (-1);
}

//--------------------------- RightDef ----------------------------------------
int CTV::RightDef(int nPrev) const
{int i,j,pos = 7;
 BYTE ch, vec;
 ASSERT(m_nBitLength >= nPrev);
 if (nPrev == -1) {i=m_nByteLength-1;j=0;}
 else {i=BIT_BYTE(nPrev);  j=S_1 - ADR_BIT(nPrev);}
 if (j==8)  {j=0;  i--; }
 ch = ((m_bVect1[i] | m_bVect0[i])>> j) << j;
 if (ch)  {
   while (!(ch & OB[pos--]));
   return ((i*S_1)+pos+1);
 }
 for (j=i-1;j>=0;j--)
   if((vec=m_bVect1[j] | m_bVect0[j]))  {
     while (!(vec & OB[pos--]));
     return ((j*S_1)+pos+1);
   }
 return (-1);
}

//--------------------------- RightOne ----------------------------------------
int CTV::RightOne(int nPrev) const
{int i,j,pos = 7;
 BYTE ch;
 ASSERT(m_nBitLength >= nPrev);
 if (nPrev == -1) {i=m_nByteLength-1;j=0;}
 else {i=BIT_BYTE(nPrev);  j=S_1 - ADR_BIT(nPrev);}
 if (j==8)  {j=0;  i--; }
 ch = (m_bVect1[i] >> j) << j;
 if (ch)  {
   while (!(ch & OB[pos--]));
   return ((i*S_1)+pos+1);
 }
 for (j=i-1;j>=0;j--)
   if(m_bVect1[j])  {
     while (!(m_bVect1[j] & OB[pos--]));
     return ((j*S_1)+pos+1);
   }
 return (-1);
}

//--------------------------- RightZero ---------------------------------------
int CTV::RightZero(int nPrev) const
{int i,j,pos = 7;
 BYTE ch;
 ASSERT(m_nBitLength >= nPrev);
 if (nPrev == -1) {i=m_nByteLength-1;j=0;}
 else {i=BIT_BYTE(nPrev);  j=S_1 - ADR_BIT(nPrev);}
 if (j==8)  {j=0;  i--; }
 ch = (m_bVect0[i] >> j) << j;
 if (ch)  {
   while (!(ch & OB[pos--]));
   return ((i*S_1)+pos+1);
 }
 for (j=i-1;j>=0;j--)
   if(m_bVect0[j])  {
     while (!(m_bVect0[j] & OB[pos--]));
     return ((j*S_1)+pos+1);
   }
 return (-1);
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////// Operations of concatinations /////////////////////

//--------------------------- Concat ------------------------------------------
void CTV::Concat(char symb/*='-'*/)
{ int nByteLength, nLen=m_nBitLength+1;
  if((nByteLength = LEN_BYTE(nLen)) > m_nAllocLength)
    GetBuffer(nLen);
  m_nByteLength = nByteLength;
  m_nBitLength = nLen; SetBitAt(nLen-1,symb);
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////// Comparing operations /////////////////////////////

//--------------------------- IsCorrect ---------------------------------------
BOOL CTV::IsCorrect() const
{ int i;
 for (i=0; i < m_nByteLength; i++)
   if ((m_bVect1[i] & m_bVect0[i]) != 0) return FALSE;  //12.12.2002
 return TRUE;
}

//--------------------------- IsBool ------------------------------------------
BOOL CTV::IsBool() const
{ int i;
 BYTE LastB;
 if (m_nByteLength == 0) return FALSE;     // 27.05.02
 for (i=0; i < m_nByteLength-1; i++)
   if((m_bVect1[i] | m_bVect0[i]) != 0xff) return FALSE;
 if((i = ADR_BIT(m_nBitLength)) == 0) LastB=0xff;
 else LastB=~(0xff>>i);
 if((m_bVect1[i] | m_bVect0[i]) != LastB) return FALSE;
 return TRUE;
}

//--------------------------- IsTrivial ---------------------------------------
BOOL CTV::IsTrivial() const
{ int i;
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 for (i=0; i < m_nByteLength; i++)
   if (m_bVect1[i] != 0 || m_bVect0[i] != 0) return FALSE;
 return TRUE;
}

//----------------- IsOne -----------------------------------------------------
BOOL CTV::IsOne() const
{int i;
 BYTE maska;
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 maska=((i = ADR_BIT(m_nBitLength)) > 0 )?(0xff<<(S_1 - i)):0xff;
 for (i=0; i < m_nByteLength-1; i++)
  if ((m_bVect1[i] ^0xff) != 0) return FALSE;
 if((m_bVect1[i] ^maska) != 0)  return FALSE;
 return TRUE;
}

//----------------- IsZero ----------------------------------------------------
BOOL CTV::IsZero() const
{int i;
 BYTE maska;
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 maska=((i = ADR_BIT(m_nBitLength)) > 0 )?(0xff<<(S_1 - i)):0xff;
 for (i=0; i < m_nByteLength-1; i++)
  if ((m_bVect0[i] ^0xff) != 0) return FALSE;
 if((m_bVect0[i] ^maska) != 0)  return FALSE;
 return TRUE;
}

//--------------------------- ExistOne ----------------------------------------
BOOL CTV::ExistOne() const
{ int i;
 for (i=0; i < m_nByteLength; i++)  if (m_bVect1[i]) return TRUE;
 return FALSE;
}

//--------------------------- ExistZero ---------------------------------------
BOOL CTV::ExistZero() const
{ int i;
 for (i=0; i < m_nByteLength; i++)  if (m_bVect0[i]) return TRUE;
 return FALSE;
}

//--------------------------- IsOrthog ----------------------------------------
BOOL CTV::IsOrthog(const CTV& tv) const
{ int i;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 for (i=0; i < m_nByteLength; i++)
   if ((m_bVect1[i] & tv.m_bVect0[i])||(m_bVect0[i] & tv.m_bVect1[i]) != 0)
     return TRUE;
 return FALSE;
}

//--------------------------- IsOrthog ----------------------------------------
BOOL CTV::IsOrthog(const CBV& bv) const
{ int i;
 BYTE* bv1=(BYTE*)bv;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=0; i < m_nByteLength; i++)
   if ((m_bVect1[i] & ~bv1[i]) || (m_bVect0[i] & bv1[i]) != 0)
     return TRUE;
 return FALSE;
}

//--------------------------- IsNeighb ----------------------------------------
BOOL CTV::IsNeighb(const CTV& tv) const   //соседство
{ int i, j;
 BYTE  mas1, mas0;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 for (i=j=0; i < m_nByteLength && j < 2; i++) {
   if ((mas1=m_bVect1[i]^tv.m_bVect1[i]) !=
      (mas0=m_bVect0[i]^tv.m_bVect0[i])) return FALSE;
//   maska=(m_bVect1[i] & tv.m_bVect0[i])|(m_bVect0[i] & tv.m_bVect1[i]);
   j+=COUNTBIT(mas1);
 }
 if(j != 1) return FALSE;
 else return TRUE;
}

//--------------------------- IsNeighb ----------------------------------------
BOOL CTV::IsNeighb(const CBV& bv) const
{ int i, j;
 BYTE  mas1, mas0, *bvS=(BYTE*)bv;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=j=0; i < m_nByteLength && j < 2; i++) {
   if ((mas1=m_bVect1[i]^bvS[i]) !=
      (mas0=m_bVect0[i]^(~bvS[i]))) return FALSE;
//   maska=(m_bVect1[i] & tv.m_bVect0[i])|(m_bVect0[i] & tv.m_bVect1[i]);
   j+=COUNTBIT(mas1);
 }
 if(j != 1) return FALSE;
 else return TRUE;
}

//--------------------------- IsAdjac -----------------------------------------
BOOL CTV::IsAdjac(const CTV& tv) const    //смежность
{ int i, j;
 BYTE  maska;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 for (i=j=0; i < m_nByteLength && j < 2; i++) {
   if ((maska=(m_bVect1[i] & tv.m_bVect0[i])|(m_bVect0[i] & tv.m_bVect1[i]))!=0)
     j+=COUNTBIT(maska);
 }
 if(j != 1) return FALSE;
 else return TRUE;
}

//--------------------------- IsAdjac -----------------------------------------
BOOL CTV::IsAdjac(const CBV& bv) const
{ int i, j;
 BYTE  maska, *bvS=(BYTE*)bv;;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=j=0; i < m_nByteLength && j < 2; i++) {
   if ((maska=(m_bVect1[i] & ~bvS[i]) | (m_bVect0[i] & bvS[i])) != 0)
     j+=COUNTBIT(maska);
 }
 if(j != 1) return FALSE;
 else return TRUE;
}

//--------------------------- IsCover -----------------------------------------
BOOL CTV::IsCover(const CTV& tv) const
{ int i, j;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 for (i=j=0; i < m_nByteLength; i++)
   if ((((m_bVect1[i] & tv.m_bVect1[i])^m_bVect1[i])!= 0) ||
       (((m_bVect0[i] & tv.m_bVect0[i])^m_bVect0[i])) != 0) return FALSE;
 return TRUE;
}

//--------------------------- IsCover -----------------------------------------
BOOL CTV::IsCover(const CBV& bv) const
{ int i;
 BYTE *bvS=(BYTE*)bv;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=0; i < m_nByteLength; i++)
   if ((m_bVect1[i] & ~bvS[i]) || (m_bVect0[i] & bvS[i]))
     return FALSE;
 return TRUE;
}

//--------------------------- IsIntersec --------------------------------------
BOOL CTV::IsIntersec(const CTV& tv) const
{ int i;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 for (i=0; i < m_nByteLength; i++)
   if ((m_bVect1[i] & tv.m_bVect0[i])||(m_bVect0[i] & tv.m_bVect1[i]))
     return FALSE;
 return TRUE;
}

//--------------------------- operator == -------------------------------------
STD(BOOL) operator ==(const CTV& tv1,const CTV& tv2)
{ return tv1.Equality(tv2.m_bVect1,tv2.m_bVect0,tv2.m_nBitLength);}

//--------------------------- operator == -------------------------------------
STD(BOOL) operator ==(const CTV& tv1,const CBV& bv2)
{ return tv1.EqualBool((BYTE*)bv2,bv2.GetBitLength());}

//--------------------------- operator == -------------------------------------
STD(BOOL) operator ==(const CBV& bv1,const CTV& tv2)
{ return tv2.EqualBool((BYTE*)bv1,bv1.GetBitLength());}

//--------------------------- operator != -------------------------------------
STD(BOOL) operator !=(const CTV& tv1,const CTV& tv2)
{ return !(tv1.Equality(tv2.m_bVect1,tv2.m_bVect0,tv2.m_nBitLength));}

//--------------------------- operator != -------------------------------------
STD(BOOL) operator !=(const CTV& tv1,const CBV& bv2)
{ return !(tv1.EqualBool((BYTE*)bv2,bv2.GetBitLength()));}

//--------------------------- operator != -------------------------------------
STD(BOOL) operator !=(const CBV& bv1,const CTV& tv2)
{ return !(tv2.EqualBool((BYTE*)bv1,bv1.GetBitLength()));}

//--------------------------- operator > --------------------------------------
STD(BOOL) operator >(const CTV& tv1,const CTV& tv2)
{ return (tv1.Pogl(tv2.m_bVect1,tv2.m_bVect0,tv2.m_nBitLength)) ; }

//--------------------------- operator < --------------------------------------
STD(BOOL) operator <(const CTV& tv1,const CTV& tv2)
{ return (tv2.Pogl(tv1.m_bVect1,tv1.m_bVect0,tv1.m_nBitLength)) ; }

//--------------------------- operator >= -------------------------------------
STD(BOOL) operator>=(const CTV& tv1, const CTV& tv2)
{ return tv1.IsCover(tv2); }

//--------------------------- operator <= -------------------------------------
STD(BOOL) operator<=(const CTV& tv1, const CTV& tv2)
{ return tv2.IsCover(tv1); }

///////////////////////////////////////////////////////////////////////////////
//////////////////////////// Analizing operations /////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////// Operations on intervals /////////////////////////////
//--------------------------- NumNeighb ---------------------------------------
//номер компоненты, по кот.соседство,иначе -1

int CTV::NumNeighb(const CTV& tv) const   
{ int i, num;
 BYTE  maska;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 for (i=0,num=-1; i < m_nByteLength; i++) {
   if ((m_bVect1[i] == tv.m_bVect1[i]) && (m_bVect0[i] == tv.m_bVect0[i])) continue;
   if(num != -1) return -1;
   maska=(m_bVect1[i] & tv.m_bVect0[i])|(m_bVect0[i] & tv.m_bVect1[i]);
   while (!(maska & OB[++num]));
   maska<<=(num+1);
   if(maska != 0) return -1;
   num +=i*S_1;
 }
 return num;
}

//--------------------------- NumNeighb ---------------------------------------
//номер компоненты, по кот.соседство,иначе -1

int CTV::NumNeighb(const CBV& bv) const   
{ int i, num;
 BYTE  maska, *bvS=(BYTE*)bv;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=0,num=-1; i < m_nByteLength; i++) {
   if ((m_bVect1[i] == bvS[i]) && (m_bVect0[i] == ~bvS[i])) continue;
   if(num != -1) return -1;
   maska=(m_bVect1[i] & ~bvS[i])|(m_bVect0[i] & bvS[i]);
   while (!(maska & OB[++num]));
   maska<<=(num+1);
   if(maska != 0) return -1;
   num +=i*S_1;
 }
 return num;
}

//--------------------------- NumAdjacent -------------------------------------
//номер компоненты, по кот.сьежны,иначе -1

int CTV::NumAdjac(const CTV& tv) const  
{ int i, j=0, num;
  BYTE  maska;
  ASSERT(m_nBitLength==tv.m_nBitLength);
  for (i=0,num=-1; i < m_nByteLength; i++) {
    if ((maska=(m_bVect1[i] & tv.m_bVect0[i])|(m_bVect0[i] & tv.m_bVect1[i]))!=0)
      j+=COUNTBIT(maska);
    if(num != -1) return -1;
    while (!(maska & OB[++num]));
    maska<<=(num+1);
    if(maska != 0) return -1;
    num +=i*S_1;
  }
  return num;
}

//--------------------------- NumAdjacent -------------------------------------
//номер компоненты, по кот.сьежны,иначе -1

int CTV::NumAdjac(const CBV& bv) const
{ int i, j=0, num;
 BYTE  maska, *bvS=(BYTE*)bv;
 ASSERT(m_nBitLength==bv.GetBitLength());
 for (i=0,num=-1; i < m_nByteLength; i++) {
    if ((maska=(m_bVect1[i] & ~bvS[i])|(m_bVect0[i] & bvS[i])) != 0)
     j+=COUNTBIT(maska);
   if(num != -1) return -1;
   while (!(maska & OB[++num]));
   maska<<=(num+1);
   if(maska != 0) return -1;
   num +=i*S_1;
 }
 return num;
}

//--------------------------- Orthogon ----------------------------------------
CBV CTV::Orthogon(const CTV& tv) const    //мн-во ортогон.компонент
{ int i;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 CBV bvTag(0,m_nBitLength,TRUE);
 for (i=0; i < m_nByteLength; i++)
   bvTag.SetByteAt(
              i,(m_bVect1[i] & tv.m_bVect0[i])|(m_bVect0[i] & tv.m_bVect1[i]));
 return bvTag;
}

//--------------------------- Orthogon ----------------------------------------
CBV CTV::Orthogon(const CBV& bv) const    //мн-во ортогон.компонент
{ int i;
 ASSERT(m_nBitLength==bv.GetBitLength());
 BYTE *bvS=(BYTE*)bv;
 CBV bvTag(0,m_nBitLength,TRUE);
 for (i=0; i < m_nByteLength; i++)
   bvTag.SetByteAt(i,(m_bVect1[i] & ~bvS[i])|(m_bVect0[i] & bvS[i]));
 return bvTag;
}

//--------------------------- Intersec ----------------------------------------
CTV CTV::Intersec(const CTV& tv) const    //рез-т миним.покрытия,если орт.- NULL
{ int i;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 CTV tvTag(m_nBitLength,TRUE);
 for (i=0; i < m_nByteLength; i++) {
   if(((m_bVect1[i] & tv.m_bVect0[i])|(m_bVect0[i] & tv.m_bVect1[i])) != 0) {
     tvTag.Empty(); break;
   }
   tvTag.m_bVect1[i]=(m_bVect1[i] | tv.m_bVect1[i]);
   tvTag.m_bVect0[i]=(m_bVect0[i] | tv.m_bVect0[i]);
 }
 return tvTag;
}

//--------------------------- Concens -----------------------------------------
CTV CTV::Concens(const CTV& tv) const    //рез-т обобщ.склеивани
{ int i;
 BYTE maska;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 CTV tvTag(m_nBitLength);
 for (i=0; i < m_nByteLength; i++) {
   maska=(m_bVect1[i] & tv.m_bVect0[i])|(m_bVect0[i] & tv.m_bVect1[i]);
   tvTag.m_bVect1[i]=(m_bVect1[i] | tv.m_bVect1[i])^maska;
   tvTag.m_bVect0[i]=(m_bVect0[i] | tv.m_bVect0[i])^maska;
 }
 return tvTag;
}

//--------------------------- Concens -----------------------------------------
CTV CTV::Concens(const CBV& bv) const    //рез-т обобщ.склеивани
{ int i;
 BYTE maska, *bvS=(BYTE*)bv;;
 ASSERT(m_nBitLength==bv.GetBitLength());
 CTV tvTag(m_nBitLength);
 for (i=0; i < m_nByteLength; i++) {
   maska=(m_bVect1[i] & ~bvS[i])|(m_bVect0[i] & bvS[i]);
   tvTag.m_bVect1[i]=(m_bVect1[i] | bvS[i])^maska;
   tvTag.m_bVect0[i]=(m_bVect0[i] | ~bvS[i])^maska;
 }
 return tvTag;
}

//--------------------------- MinCover ----------------------------------------
CTV CTV::MinCover(const CTV& tv) const    //рез-т миним.покрыти
{ int i;
 ASSERT(m_nBitLength==tv.m_nBitLength);
 CTV tvTag(m_nBitLength);
 for (i=0; i < m_nByteLength; i++) {
   tvTag.m_bVect1[i]=(m_bVect1[i] & tv.m_bVect1[i]);
   tvTag.m_bVect0[i]=(m_bVect0[i] & tv.m_bVect0[i]);
 }
 return tvTag;
}

//--------------------------- MinCover ----------------------------------------
CTV CTV::MinCover(const CBV& bv) const    //рез-т миним.покрыти
{ int i;
 BYTE *bvS=(BYTE*)bv;
 ASSERT(m_nBitLength==bv.GetBitLength());
 CTV tvTag(m_nBitLength);
 for (i=0; i < m_nByteLength; i++) {
   tvTag.m_bVect1[i]=(m_bVect1[i] & bvS[i]);
   tvTag.m_bVect0[i]=(m_bVect0[i] & ~bvS[i]);
 }
 return tvTag;
}

//--------------------------- CTVtoCBM ----------------------------------------
CBM CTV::CTVtoCBM() const
{int i, j, nex, ves, count, newR;
 CBV maska(0,m_nBitLength,TRUE);
 BYTE *maskaB=(BYTE *)maska;
 for(i=0; i < m_nByteLength; i++)
   maskaB[i]=~(m_bVect1[i] | m_bVect0[i]);
 if ((i = ADR_BIT(m_nBitLength)) > 0 ) {
   i = S_1 - i; maskaB[m_nByteLength-1] &= (0xff << i);
 }
 ves=maska.CountBit();
 CBM bmTag(1<<ves,m_nBitLength); bmTag.SetSize(0,m_nBitLength);
 bmTag.Add(m_bVect1);
 for(i=0,count=1,nex=-1; i < ves; i++) {
   nex=maska.LeftOne(nex);
   for(j=0,newR=count; j < count; j++,newR++) {
     bmTag.Add(bmTag,j); bmTag.SetBitAt(newR,nex,1);
   }
   count<<=1;
 }
 return bmTag;
}

//////////////////// Operations on TERNARY VECTOR PARTS ///////////////////////
/////////////////////////// Logic operations //////////////////////////////////
//--------------------------- OrInPlace ---------------------------------------
void CTV::OrInPlace(BOOL Part,const CBV& bv)
{int i;
 ASSERT(bv.GetBitLength() == m_nBitLength);
 for (i=0; i < m_nByteLength; i++)
   if(Part) m_bVect1[i] |= bv.GetByteAt(i);
   else  m_bVect0[i] |= bv.GetByteAt(i);
 return;
}

//--------------------------- AndInPlace --------------------------------------
void CTV::AndInPlace(BOOL Part,const CBV& bv)
{int i;
 ASSERT(bv.GetBitLength() == m_nBitLength);
 for (i=0; i < m_nByteLength; i++)
   if(Part) m_bVect1[i] &= bv.GetByteAt(i);
   else  m_bVect0[i] &= bv.GetByteAt(i);
 return;
}

//--------------------------- XorInPlace --------------------------------------
void CTV::XorInPlace(BOOL Part,const CBV& bv)
{int i;
 ASSERT(bv.GetBitLength() == m_nBitLength);
 for (i=0; i < m_nByteLength; i++)
   if(Part) m_bVect1[i] ^= bv.GetByteAt(i);
   else  m_bVect0[i] ^= bv.GetByteAt(i);
 return;
}

//--------------------------- SubInPlace --------------------------------------
void CTV::SubInPlace(BOOL Part,const CBV& bv)
{int i;
 ASSERT(bv.GetBitLength() == m_nBitLength);
 for (i=0; i < m_nByteLength; i++)
   if(Part) m_bVect1[i] &= ~bv.GetByteAt(i);
   else  m_bVect0[i] &= ~bv.GetByteAt(i);
 return;
}

//--------------------------- InvertBitsInPlace -------------------------------
void CTV::InvertBitsInPlace(BOOL Part)
{int i;
 BYTE maska;
 ASSERT(m_nBitLength != 0);
 maska=((i=ADR_BIT(m_nBitLength)) == 0)?0xff:(~(0xff>>i));
 if(Part) {
   for (i=0; i < m_nByteLength; i++)
     m_bVect1[i]=~m_bVect1[i];
   m_bVect1[m_nByteLength-1] &= maska;
 }
 else {
   for (i=0; i < m_nByteLength; i++)
     m_bVect0[i]= ~m_bVect0[i];
   m_bVect0[m_nByteLength-1] &= maska;
 }
 return;
}

//--------------------------- InvertBits --------------------------------------
CBV CTV::InvertBits(BOOL Part) const
{int i;
 BYTE maska=0xff;
 ASSERT(m_nBitLength != 0);
 i=ADR_BIT(m_nBitLength);
 maska=(i == 0)?0xff:(~(0xff>>i));
 CBV bvTag(0,m_nBitLength,TRUE);
 if(Part) {
   for (i=0; i < m_nByteLength-1; i++)
     bvTag.SetByteAt(i,~m_bVect1[i]);
   bvTag.SetByteAt(m_nByteLength-1,(~m_bVect1[i]&maska));
 }
 else {
   for (i=0; i < m_nByteLength-1; i++)
     bvTag.SetByteAt(i,~m_bVect0[i]);
   bvTag.SetByteAt(m_nByteLength-1,(~m_bVect0[i]&maska));
 }
 return bvTag;
}
/*
#ifndef _LINUX
///////////////////////////////////////////////////////////////////////////////
////////////////////////// Input/output operations ////////////////////////////
//---------------- operator <<(CDumpContext& dc, const CTV& tv) --------
#ifdef _DEBUG
CDumpContext& AFXAPI operator <<(CDumpContext& dc, const CTV& tv)
{int i;
 CString res('\0',tv.m_nBitLength);
 for (i=0; i < tv.m_nBitLength; i++)
   switch(tv.GetBitAt(i)) {
   case '1': res.SetAt(i,'1'); break;
   case '0': res.SetAt(i,'0'); break;
   case '-': res.SetAt(i,'-'); break;
   default:  res.SetAt(i,'+');  }
 dc << res; return dc;
}
#endif //_DEBUG

// --------- AFXAPI operator<<(CArchive& ar, const CTV& tv) -------------
CArchive& AFXAPI operator<<(CArchive& ar, const CTV& tv)
{ar << tv.m_nByteLength;
 ar << tv.m_nBitLength;
 ar.Write(tv.m_bVect1, tv.m_nByteLength);
 ar.Write(tv.m_bVect0, tv.m_nByteLength);
 return ar;
}

//---------------- AFXAPI operator>>(CArchive& ar, const CTV& tv) -------------
CArchive& AFXAPI operator>>(CArchive& ar, CTV& tv)
{
  tv.Empty();
  ar >> tv.m_nByteLength;
  ar >> tv.m_nBitLength;
  if (tv.m_nByteLength != 0) {
    tv.AllocBuffer(tv.m_nByteLength);
    if (ar.Read(tv.m_bVect1, tv.m_nByteLength) != 
        (unsigned int) tv.m_nByteLength)
      AfxThrowArchiveException(CArchiveException::endOfFile);
    if (ar.Read(tv.m_bVect0, tv.m_nByteLength) != 
        (unsigned int) tv.m_nByteLength)
      AfxThrowArchiveException(CArchiveException::endOfFile);
  }
  return ar;
}

#endif //_LINUX
*/


  // --------- AFXAPI operator<<(CArchive& ar, const CTV& tv) -------------
STD(CArch&) operator<<(CArch& ar, const CTV& tv)
{ ar << tv.m_nByteLength;
  ar << tv.m_nBitLength;
  ar.Write(tv.m_bVect1, tv.m_nByteLength);
  ar.Write(tv.m_bVect0, tv.m_nByteLength);
  return ar;
}

//---------------- AFXAPI operator>>(CArchive& ar, const CTV& tv) -------------
STD(CArch&) operator>>(CArch& ar, CTV& tv)
{
  tv.Empty();
  ar >> tv.m_nByteLength;
  ar >> tv.m_nBitLength;
  if (tv.m_nByteLength != 0) {
    tv.AllocBuffer(tv.m_nByteLength);
    ar.Read(tv.m_bVect1, tv.m_nByteLength);
    ar.Read(tv.m_bVect0, tv.m_nByteLength);
  }
  return ar;
}

  
  
///////////////////////////////////////////////////////////////////////////////
////////////////////////////// Advanced dump of memmory ///////////////////////
//---------------- GetBuffer(int nMinBufLength) -------------------------------

BYTE* CTV::GetBuffer(int nMinBufLength)
{ ASSERT(nMinBufLength >= 0);
 if (nMinBufLength > m_nBitLength) {
 BYTE* pbtOldData1 = m_bVect1;
 BYTE* pbtOldData0 = m_bVect0;
 int nOldLen = m_nByteLength; int nOldBit = m_nBitLength;
 AllocBuffer(LEN_BYTE(nMinBufLength));
 memcpy(m_bVect1, pbtOldData1, nOldLen);
 memcpy(m_bVect0, pbtOldData0, nOldLen);
 m_nByteLength = nOldLen; m_nBitLength = nOldBit;
 if (pbtOldData1 != NULL) delete [] pbtOldData1;
 }
 return m_bVect1;
}

//---------------- ReleaseBuffer(int nNewLength) ------------------------------
void CTV::ReleaseBuffer(int nNewLength)
{ int nNewByte;
 if (nNewLength == -1) nNewLength = m_nBitLength;
 nNewByte = LEN_BYTE(nNewLength);
 ASSERT(nNewByte <= m_nAllocLength);
 m_nByteLength = nNewByte;
 m_nBitLength = nNewLength;
}

// 10.02.2002------ Romanov
//----------------- SetSize(int nNewLength,int nNewAllocLength)----------------
BYTE* CTV::SetSize(int nNewLength,int nNewAllocLength/*-1*/)
{
  if (nNewLength==m_nBitLength)  return m_bVect1;   // 21.03.2006
  ASSERT(nNewLength >= 0);
  int nLen, i;
  int nOldByte = m_nByteLength;
  nLen=(nNewAllocLength > 0)?nNewAllocLength:nNewLength;
  ASSERT(nNewLength <=nLen);
  GetBuffer(nLen);
  m_nByteLength = LEN_BYTE(nNewLength);
  m_nBitLength = nNewLength;
//  new 10.02.2001
  if (i=ADR_BIT(m_nBitLength)) {
     m_bVect1[m_nByteLength-1]=(m_bVect1[m_nByteLength-1]>>(S_1-i))<<(S_1-i);
     m_bVect0[m_nByteLength-1]=(m_bVect0[m_nByteLength-1]>>(S_1-i))<<(S_1-i);
  }
  if (nOldByte > m_nByteLength) {
    memset(m_bVect1+m_nByteLength,0,nOldByte-m_nByteLength);
    memset(m_bVect0+m_nByteLength,0,nOldByte-m_nByteLength);
  }
  return m_bVect1;
}


//********************* Protected functions ***********************************
//---------------------- AllocBuffer-------------------------------------------
void CTV::AllocBuffer(int nLen)
{ASSERT(nLen >= 0);
 ASSERT(nLen <= INT_MAX - 1);    // max size (enough room for 1 extra)
 if (nLen == 0)     {   Init();     }
 else   {
   m_bVect1 = new BYTE[nLen<<1];       // may throw an exception
   m_bVect0 = m_bVect1 + nLen;
   m_nByteLength = nLen;
   m_nAllocLength = nLen;
   memset(m_bVect1,0,nLen<<1);
 }
}

//--------------------- AllocCopy ---------------------------------------------
void CTV::AllocCopy(CTV& dest, int nCopyLen, int nCopyIndex,int nExtraLen)
{int nNewLen = nCopyLen + nExtraLen;
 if (nNewLen == 0) { dest.Init(); }
 else {
   dest.AllocBuffer(nNewLen);
   memcpy(dest.m_bVect1, &m_bVect1[nCopyIndex], nCopyLen);
   memcpy(dest.m_bVect0, &m_bVect0[nCopyIndex], nCopyLen);
 }
}

//---------------------- AssignChar -------------------------------------------
void CTV::AssignChar(int nLenBit, const char* pch)
{ int nLenByte = LEN_BYTE(nLenBit);
  if (nLenByte > m_nAllocLength) {
    delete [] m_bVect1;
    AllocBuffer(nLenByte);
    m_nBitLength = nLenBit;
  }
  else {
    memset(m_bVect1,0,nLenByte);
    memset(m_bVect0,0,nLenByte);
  }
  CharBit(nLenByte,pch);
  m_nByteLength = nLenByte;
  m_nBitLength = nLenBit;
}

//----------------------- AssignCopy ------------------------------------------
void CTV::AssignCopy(int nLenBit,int nLenByte, 
                     const BYTE* pbtSrcDat1, const BYTE* pbtSrcDat0)
{ //--- check if it will fit
  if (nLenByte > m_nAllocLength) { // it won't fit, allocate another one
    Empty();
    AllocBuffer(nLenByte);
  }
  else {
   memset(m_bVect1+nLenByte,0,m_nAllocLength-nLenByte);   // 22.02.2007
   memset(m_bVect0+nLenByte,0,m_nAllocLength-nLenByte);   // 22.02.2007
  }

  if (nLenByte != 0) {
    memcpy(m_bVect1, pbtSrcDat1, nLenByte);
    memcpy(m_bVect0, pbtSrcDat0, nLenByte);
  }
  m_nByteLength = nLenByte;
  m_nBitLength = nLenBit;
  return;
}

//----------------------- CharBit ---------------------------------------------
void CTV::CharBit(int nLenByte,const char* pch)
{ int i,j;
  for (j=0; j < nLenByte; j++) {
    for (i=0; i < S_1 && *pch; i++) {
      if (*pch=='1' || *pch=='+') m_bVect1[j] |= OB[i];
      else
        if (*pch=='0' || *pch=='+') m_bVect0[j] |= OB[i];
      ++pch;
    }
  }
}

//--------------------------- Conc---------------------------------------------
void CTV::Conc(const BYTE* SrcVect1, const BYTE* SrcVect0, int SrcBitLen)
{ int NewBitLen, NewByteLen, SrcByteLen, OldByte, r_bit,l_bit,i,j;
  BYTE *work1, *work0;
 NewBitLen = m_nBitLength + SrcBitLen;
 if (NewBitLen == 0) { Init(); return;}
 NewByteLen = LEN_BYTE(NewBitLen);
 OldByte=m_nByteLength;
 r_bit = ADR_BIT(m_nBitLength);
 if (m_nAllocLength < NewByteLen)  {
   if (m_nByteLength) {
     work1 = new BYTE[m_nByteLength<<1];
     work0 = work1+m_nByteLength;
     memcpy(work1, m_bVect1, m_nByteLength);
     memcpy(work0, m_bVect0, m_nByteLength);
     Empty();
     AllocBuffer(NewByteLen);
     memcpy(m_bVect1, work1, OldByte); memcpy(m_bVect0, work0, OldByte);
     delete [] work1;
   }
   else { AllocBuffer(NewByteLen); }
 }
 else                                   // new 16.11.99
  if(m_nByteLength < NewByteLen) {      // new 16.11.99
    m_nByteLength = NewByteLen;
    m_bVect1[m_nByteLength-1]=0;        // new 16.11.99
    m_bVect0[m_nByteLength-1]=0;        // new 16.11.99
  }
 m_nBitLength = NewBitLen;
 SrcByteLen = LEN_BYTE(SrcBitLen);
 if (r_bit==0)  {
   memcpy(m_bVect1+OldByte, SrcVect1, SrcByteLen);
   memcpy(m_bVect0+OldByte, SrcVect0, SrcByteLen);  return;
 }
 l_bit = S_1-r_bit; i=OldByte;
 m_bVect1[i-1] |= SrcVect1[0]>>r_bit; m_bVect0[i-1] |= SrcVect0[0]>>r_bit;
 for (j=0; j < SrcByteLen-1; j++) {
   m_bVect1[i] = (SrcVect1[j] << l_bit) | (SrcVect1[j+1] >> r_bit);
                                // Monday 01 February 0100 (13:04:55) Cher
   m_bVect0[i++] = (SrcVect0[j] << l_bit) | (SrcVect0[j+1] >> r_bit);
 }
 if (j*S_1+l_bit < SrcBitLen) {
   m_bVect1[m_nByteLength-1] |= SrcVect1[SrcByteLen-1] << l_bit;
   m_bVect0[m_nByteLength-1] |= SrcVect0[SrcByteLen-1] << l_bit;
 }
 if (j=ADR_BIT(m_nBitLength)) {
   i = S_1 - j;
   m_bVect1[m_nByteLength-1] = (m_bVect1[m_nByteLength-1] >> i) << i;
   m_bVect0[m_nByteLength-1] = (m_bVect0[m_nByteLength-1] >> i) << i;
 }
}

//--------------------------- Conc2 -------------------------------------------
void CTV::Conc2(const BYTE* Vect11,const BYTE* Vect10,int SrcBitLen1,
const BYTE* Vect21,const BYTE* Vect20,int SrcBitLen2)
{ int NewBitLen, NewByteLen, SrcByteLen, OldByte, r_bit,l_bit,i,j;
 NewBitLen = SrcBitLen1 + SrcBitLen2;
 if (NewBitLen == 0) { Init(); return;}
 NewByteLen = LEN_BYTE(NewBitLen);
 if (m_nAllocLength < NewByteLen)  {
   Empty(); AllocBuffer(NewByteLen);
 }
 else                                   // new 16.11.99
  if(m_nByteLength < NewByteLen) {      // new 16.11.99
    m_nByteLength = NewByteLen;
    m_bVect1[m_nByteLength-1]=0;        // new 16.11.99
    m_bVect0[m_nByteLength-1]=0;        // new 16.11.99
 }
 m_nBitLength = NewBitLen;
 OldByte = LEN_BYTE(SrcBitLen1);
 memcpy(m_bVect1, Vect11, OldByte);
 memcpy(m_bVect0, Vect10, OldByte);
 r_bit = ADR_BIT(SrcBitLen1);
 SrcByteLen = LEN_BYTE(SrcBitLen2);
 if (r_bit==0)  {
   memcpy(m_bVect1+OldByte, Vect21, SrcByteLen);
   memcpy(m_bVect0+OldByte, Vect20, SrcByteLen); return;
 }
 l_bit = S_1-r_bit; i=OldByte;
 m_bVect1[OldByte-1] |= Vect21[0]>>r_bit;
 m_bVect0[OldByte-1] |= Vect20[0]>>r_bit;
 for (j=0; j<SrcByteLen-1; j++) {
   m_bVect1[i++] = (Vect21[j] << l_bit) | (Vect21[j+1] >> r_bit);
   m_bVect0[i++] = (Vect20[j] << l_bit) | (Vect20[j+1] >> r_bit);
 }
 if (j*S_1+l_bit < SrcBitLen2) {
   m_bVect1[m_nByteLength-1] |= Vect21[SrcByteLen-1] << l_bit;
   m_bVect0[m_nByteLength-1] |= Vect20[SrcByteLen-1] << l_bit;
 }
 if (j=ADR_BIT(m_nBitLength)) {
   i = S_1 - j;
   m_bVect1[m_nByteLength-1] = (m_bVect1[m_nByteLength-1] >> i) << i;
   m_bVect0[m_nByteLength-1] = (m_bVect0[m_nByteLength-1] >> i) << i;
 }
}

//--------------------------- EqualBool ---------------------------------------
BOOL CTV::EqualBool(const BYTE* Vect,int BitLen) const
{int i;
 if ((Vect==NULL && m_bVect1 != NULL) || (m_nBitLength != BitLen)) return FALSE;
 for (i=0;i < m_nByteLength;i++)
   if ((m_bVect1[i] != Vect[i]) || (m_bVect0[i] != ~Vect[i])) return FALSE;
 return TRUE;
}

//--------------------------- Equality ----------------------------------------
BOOL CTV::Equality(const BYTE* Vect1,const BYTE* Vect0,int BitLen) const
{int i;
 if ((Vect1==NULL && m_bVect1 != NULL) || (m_nBitLength != BitLen)) return FALSE;
 for (i=0;i < m_nByteLength;i++)
   if ((m_bVect1[i] != Vect1[i]) || (m_bVect0[i] != Vect0[i])) return FALSE;
 return TRUE;
}

//--------------------------- Extr --------------------------------------------
void CTV::Extr(const BYTE* SrcVect1,const BYTE* SrcVect0,
               int SrcBitLen,int nFirst,int nCount)
{ int i, j, nLenByte,l_bit,r_bit,AdrBeg;
 ASSERT(nCount >= 0);  ASSERT(nFirst >= 0);
 ASSERT((nCount+nFirst) <= SrcBitLen);
 nLenByte = LEN_BYTE(nCount);
 AllocBuffer(nLenByte);
 m_nBitLength = nCount;
 if (SrcBitLen == nCount) {
   memcpy((BYTE*)m_bVect1,SrcVect1,nLenByte);
   memcpy((BYTE*)m_bVect0,SrcVect0,nLenByte); return;
 }
 AdrBeg = BIT_BYTE(nFirst);
 memcpy((BYTE*)m_bVect1,SrcVect1+AdrBeg,nLenByte);
 memcpy((BYTE*)m_bVect0,SrcVect0+AdrBeg,nLenByte);
 l_bit = ADR_BIT(nFirst);
 if (l_bit) {
   r_bit = S_1 - l_bit;
   if (nLenByte == LEN_BYTE(SrcBitLen)) j=nLenByte-1;   //new, 4.12.97
   else                                 j=nLenByte;     //new, 4.12.97
   for (i=0; i < nLenByte; i++) {
     m_bVect1[i]=(SrcVect1[AdrBeg+i]<< l_bit) | (SrcVect1[AdrBeg+i+1] >>r_bit);
     m_bVect0[i]=(SrcVect0[AdrBeg+i]<< l_bit) | (SrcVect0[AdrBeg+i+1] >>r_bit);
   }
   if (j == nLenByte-1) {
     m_bVect1[j] = (SrcVect1[AdrBeg+j] << l_bit);  //new, 4.12.97
     m_bVect0[j] = (SrcVect0[AdrBeg+j] << l_bit);  //new, 4.12.97
   }
 }
 else {
   memcpy((BYTE*)m_bVect1,SrcVect1+AdrBeg,nLenByte);
   memcpy((BYTE*)m_bVect0,SrcVect0+AdrBeg,nLenByte);
 }
 if (ADR_BIT(m_nBitLength)) {
   i = S_1 - ADR_BIT(m_nBitLength);
   m_bVect1[m_nByteLength-1] = (m_bVect1[m_nByteLength-1] >> i) << i;
   m_bVect0[m_nByteLength-1] = (m_bVect0[m_nByteLength-1] >> i) << i;
 }
}

//--------------------------- LeftShiftVect -----------------------------------
void CTV::LeftShiftVect(const BYTE* SrcVect1,const BYTE* SrcVect0,
                        int nSrcLen,int nShift)
{int j,i,l_bit,r_bit,nNewLenByte,nNewLenBit,nSrcByte,AdrBeg;
 ASSERT(nShift >= 0 && nSrcLen >= nShift);
 nNewLenBit = nSrcLen - nShift;
 nNewLenByte = LEN_BYTE(nNewLenBit);
 nSrcByte = LEN_BYTE(nSrcLen);
 AllocBuffer(nSrcByte);
 m_nBitLength = nSrcLen;
 if (nNewLenBit==0)  return;
 if (nShift==0) {
   memcpy(m_bVect1,SrcVect1,nSrcByte);
   memcpy(m_bVect0,SrcVect0,nSrcByte); return;
 }
 AdrBeg = BIT_BYTE(nShift);
 l_bit = ADR_BIT(nShift);
 if (l_bit)    {
   r_bit = S_1 - l_bit;  j=0;
   for (i=AdrBeg; i<nSrcByte-1; i++) {
     m_bVect1[j++] = (SrcVect1[i] << l_bit) | (SrcVect1[i+1] >> r_bit);
     m_bVect0[j++] = (SrcVect0[i] << l_bit) | (SrcVect0[i+1] >> r_bit);
   }
   if ((j*S_1) < nShift) {
     m_bVect1[nNewLenByte-1] = SrcVect1[nSrcByte-1] << l_bit;
     m_bVect0[nNewLenByte-1] = SrcVect0[nSrcByte-1] << l_bit;
   }
 }
 else {
   memcpy(m_bVect1, SrcVect1+AdrBeg, nNewLenByte);
   memcpy(m_bVect0, SrcVect0+AdrBeg, nNewLenByte);
 }
 i= nSrcByte - nNewLenByte;
 memset(m_bVect1+nNewLenByte, 0, i);
 memset(m_bVect0+nNewLenByte, 0, i);
}

//--------------------------- Pogl --------------------------------------------
BOOL CTV::Pogl(const BYTE* Vect1,const BYTE* Vect0,int BitLen) const
{ int i,j=0;
 ASSERT(m_nBitLength==BitLen);
 for (i=0; i < m_nByteLength; i++) {
   if ((m_bVect1[i] == Vect1[i]) && (m_bVect0[i] == Vect0[i])) continue;
   if ((m_bVect1[i] & Vect1[i]) != m_bVect1[i])   return FALSE;
   if ((m_bVect0[i] & Vect0[i]) != m_bVect0[i])   return FALSE;
   j=1;
 }
 if (j == 0) return FALSE;
 return TRUE;
}

//-------------------------- RightShiftVect -----------------------------------
void CTV::RightShiftVect(const BYTE* SrcVect1,const BYTE* SrcVect0,
                         int nSrcLen,int nShift)
{int i,j,l_bit,r_bit,nSrcByte,AdrBeg;
 ASSERT(nShift >= 0 && nSrcLen >= nShift);
 nSrcByte = LEN_BYTE(nSrcLen);
 AllocBuffer(nSrcByte);
 m_nBitLength = nSrcLen;
 if ((nSrcLen - nShift)==0)  return;
 if (nShift==0) {
   memcpy(m_bVect1,SrcVect1,nSrcByte);
   memcpy(m_bVect0,SrcVect0,nSrcByte); return;
 }
 AdrBeg = BIT_BYTE(nShift);
 r_bit = ADR_BIT(nShift);
 if (r_bit)    {
   l_bit = S_1 - r_bit;  j=0;
   m_bVect1[AdrBeg] = (SrcVect1[0] >> r_bit);
   m_bVect0[AdrBeg] = (SrcVect0[0] >> r_bit);
   for (i=AdrBeg+1; i<nSrcByte; i++) {
     m_bVect1[i] = (SrcVect1[j] << l_bit) | (SrcVect1[j+1] >> r_bit);
     m_bVect0[i] = (SrcVect0[j] << l_bit) | (SrcVect0[j+1] >> r_bit); j++;
   }
 }
 else {
   memcpy(m_bVect1+AdrBeg, SrcVect1, nSrcByte-AdrBeg);
   memcpy(m_bVect0+AdrBeg, SrcVect0, nSrcByte-AdrBeg);
 }
 if (i=ADR_BIT(m_nBitLength)) {
   m_bVect1[nSrcByte-1] = (m_bVect1[nSrcByte-1] >> (S_1 - i)) << (S_1 - i);
   m_bVect0[nSrcByte-1] = (m_bVect0[nSrcByte-1] >> (S_1 - i)) << (S_1 - i);
 }
}
