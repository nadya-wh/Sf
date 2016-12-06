/////////////////////////////////////////////////////////////////
// Ver.2.1.4    09.04.2008
//              GetUnDef - устранение ошибки
// Ver.2.1.3    28.03.2008
//              SetOnes и SetZeros - исправлены для реализации в другой интерпретации
//              в виде функций AddOnes и AddZeros
//              Поправка функции LeftUnDef
// Ver.2.1.1     22.01.2008
//               Добавлены функции конвертации данных базового класса троичных векторов
// Ver.2.1.0     07.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.1.1.0    24.06.2006   Russian == English
/////////////////////////////////////////////////////////////////
#include <iostream>
#include <limits.h>
#ifdef _JOINT
#include "ShortBool.h"
#include "ShortTern.h"
#include "BaseBool.h"
#include "BaseTern.h"
#else
#include "../ComBool/ShortBool.h"
#include "../ComBool/ShortTern.h"
#include "../Common/BaseBool.h"
#include "../Common/BaseTern.h"
#endif
/*
#ifndef _LINUX
#include "afxwin.h"
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#define new DEBUG_NEW

#endif
*/

//////////////////////// Construction/Destruction /////////////////////////////

//------------------------ CsTV ------------------------------------------------
CsTV::CsTV()   { m_bVect1=m_bVect0=m_nBitLength= 0; }

//------------------------ CsTV (copy of CsTV) ----------------------------------
CsTV::CsTV(const CsTV& tvSrc)
{
  if (tvSrc.m_nBitLength == 0) m_bVect1=m_bVect0=m_nBitLength= 0;
  else {
    m_bVect1 = tvSrc.m_bVect1;
    m_bVect0 = tvSrc.m_bVect0;
    m_nBitLength = tvSrc.m_nBitLength;
  }
}

//------------------------ CsTV (copy of boolean vectors pair) -----------------
CsTV::CsTV(const CsBV& bv1Src,const CsBV& bv0Src)
{
  ASSERT(bv1Src.GetBitLength() == bv0Src.GetBitLength());
  if (bv1Src.GetBitLength() == 0) m_bVect1=m_bVect0=m_nBitLength= 0;
  else {
    m_bVect1 = (ULONG)bv1Src;
    m_bVect0 = (ULONG)bv0Src;
    m_nBitLength = bv1Src.GetBitLength();
  }
}

//------------------------ CsTV (array of bits) --------------------------------
CsTV::CsTV(int nLenBit,char value)
{ 
  ULONG a1,a0;
  if (nLenBit < 1 || nLenBit > BITS_COUNT)
    m_bVect1=m_bVect0=m_nBitLength= 0; // return empty vector if invalid repeat count
  else {
    m_nBitLength = nLenBit;
    switch (value) {
    case '1': a1=0xffffffff; a0=0; break;
    case '0': a1=0; a0=0xffffffff; break;
    case '-': a1=0; a0=0; break;
    case '+': a1=0xffffffff; a0=0xffffffff; break;
    default: return;
    }
    m_bVect1 = a1 >> (BITS_COUNT - nLenBit) << (BITS_COUNT - nLenBit);
    m_bVect0 = a0 >> (BITS_COUNT - nLenBit) << (BITS_COUNT - nLenBit);
  }
}

//------------------------ CsTV (copy of byte arrays pair) ---------------------
CsTV::CsTV(const ULONG pbt1,const ULONG pbt0,int nLenBit)
{
  if (nLenBit==0) m_bVect1=m_bVect0=m_nBitLength= 0;
  else   {
    m_bVect1 = pbt1; m_bVect0 = pbt0; m_nBitLength = nLenBit;
  }
}

//------------------------ CsTV (copy of chararray) ----------------------------
CsTV::CsTV(const char* pch)
{ int nLenBit;
  nLenBit = strlen((char*)pch);
  if (nLenBit < 1 || nLenBit >BITS_COUNT ) m_bVect1=m_bVect0=m_nBitLength= 0;
  else   {
    CharBit(pch); m_nBitLength = nLenBit;
  }
}


///////////////////////////////////////////////////////////////////////////////
/////////////////////////// Data Reading //////////////////////////////////////

//------------------------- GetBitAt ------------------------------------------
char CsTV::GetBitAt(int nIndex) const
{
    //cout << nIndex << " " << m_nBitLength << "\n";
  ASSERT(nIndex >= 0);
  ASSERT(nIndex < m_nBitLength);
  ULONG a1, a0;
   
  a1 = m_bVect1 & sOB[nIndex];
  a0 = m_bVect0 & sOB[nIndex];
  if(a1 != 0 && a0 == 0)  return '1';
  if(a0 != 0 && a1 == 0)  return '0';
  if(a0 == 0 && a1 == 0)  return '-';
  return '+';
}

//----------------------------  Extract ---------------------------------------
CsTV CsTV::Extract(int nFirst,int nCount)
{ 
  CsTV s;
  s.Extr(m_bVect1,m_bVect0,m_nBitLength,nFirst,nCount);  return s;
}
/*
#ifndef _LINUX
//------------------------- BitChar -------------------------------------------
CString CsTV::BitChar(char One,char Zero,char Dont,char Rest)
{ 
  int i;
  CString res('\0',m_nBitLength);
  for (i=0; i < m_nBitLength; i++)
    switch (GetBitAt(i)) {
      case '1': res.SetAt(i,One); break;
      case '0': res.SetAt(i,Zero); break;
      case '-': res.SetAt(i,Dont); break;
      default:  res.SetAt(i,Rest);
    }
  return res;
}
#else
*/
char * CsTV::BitChar(char One,char Zero,char Dont,char Rest)
{ 
  int i;
  char * res;
  res = (char*)malloc(m_nBitLength+1);
  res[m_nBitLength]='\0';
  for (i=0; i < m_nBitLength; i++)
    switch (GetBitAt(i)) {
      case '1': res[i] = One; break;
      case '0': res[i] = Zero; break;
      case '-': res[i] = Dont; break;
      default:  res[i] = Rest;
    }
  return res;
}

//#endif

//------------------------- CsBV GetDefs ---------------------------------------
CsBV CsTV::GetDefs() const
{
  CsBV bv(m_bVect0, m_nBitLength);
  bv = m_bVect1 | m_bVect0;
  return bv;
}

//------------------------- CsBV GetUnDefs -------------------------------------
CsBV CsTV::GetUnDefs() const
{ // Исправил Торопов 09.04.08
  CsBV bv(m_bVect0, m_nBitLength);
  //bv = (~m_bVect1 | m_bVect0) >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
  bv = (~(m_bVect1 | m_bVect0)) >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
return bv;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// Data Writing //////////////////////////////////////

//---------------------------- SetBitAt ---------------------------------------
void CsTV::SetBitAt(int nIndex, char symb)
{ 
  ASSERT(nIndex >= 0 && nIndex < m_nBitLength);
  switch (symb) {
    case '1': m_bVect1 |= sOB[nIndex]; m_bVect0 &= ~sOB[nIndex];break;
    case '0': m_bVect0 |= sOB[nIndex]; m_bVect1 &=~sOB[nIndex]; break;
    case '-': m_bVect1 &= ~sOB[nIndex]; m_bVect0 &=~sOB[nIndex]; break;
    default:  m_bVect1 |= sOB[nIndex]; m_bVect0 |=sOB[nIndex];
  }
}

//--------------------------- SetVect -----------------------------------------
void CsTV::SetVect(CsBV bv1Src, CsBV bv0Src)
{ 
  ASSERT(bv1Src.GetBitLength()==bv0Src.GetBitLength());
  if (bv1Src.GetBitLength() == 0) m_bVect1=m_bVect0=m_nBitLength= 0;
  else {
    m_nBitLength = bv1Src.GetBitLength();
    m_bVect1 = (ULONG)bv1Src;
    m_bVect0 = (ULONG)bv0Src;
  }
}

//--------------------------- SetOnes -----------------------------------------
void CsTV::SetOnes(const CsBV& bv)
{
  ASSERT(m_nBitLength==bv.GetBitLength());
  m_bVect1 = (ULONG)bv; m_bVect0 &=~(ULONG)bv;
}

//--------------------------- SetZeros ----------------------------------------
void CsTV::SetZeros(const CsBV& bv)
{
  ASSERT(m_nBitLength==bv.GetBitLength());
  m_bVect0 = (ULONG)bv; m_bVect1 &= ~(ULONG)bv;
}

//--------------------------- AddOnes -----------------------------------------
void CsTV::AddOnes(const CsBV& bv)
{
  ASSERT(m_nBitLength==bv.GetBitLength());
  m_bVect1 |= (ULONG)bv; m_bVect0 &=~(ULONG)bv;
}

//--------------------------- AddZeros ----------------------------------------
void CsTV::AddZeros(const CsBV& bv)
{
  ASSERT(m_nBitLength==bv.GetBitLength());
  m_bVect0 |= (ULONG)bv; m_bVect1 &= ~(ULONG)bv;
}

//--------------------------- SetUnDefs ---------------------------------------
void CsTV::SetUnDefs(const CsBV& bv)
{
  ASSERT(m_nBitLength==bv.GetBitLength());
  m_bVect1 &= ~(ULONG)bv; 
  m_bVect0 &= ~(ULONG)bv;
}

//--------------------------- SetOnes -----------------------------------------
void CsTV::SetOnes(const ULONG bv) { m_bVect1 = bv; m_bVect0 &= ~bv; }

//--------------------------- SetZeros ----------------------------------------
void CsTV::SetZeros(const ULONG bv){ m_bVect0 = bv; m_bVect1 &= ~bv; }

//--------------------------- AddOnes -----------------------------------------
void CsTV::AddOnes(const ULONG bv) { m_bVect1 |= bv; m_bVect0 &= ~bv; }

//--------------------------- AddZeros ----------------------------------------
void CsTV::AddZeros(const ULONG bv){ m_bVect0 |= bv; m_bVect1 &= ~bv; }

///////////////////////////////////////////////////////////////////////////////
/////////////////////////// Operators and functions of assignment /////////////

//--------------------------- Clear -------------------------------------------
void CsTV::Clear(char symb)
{
  switch (symb) {
    case '1': m_bVect1 = 0xffffffff >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
              m_bVect0 = 0; break;
    case '0': m_bVect0 = 0xffffffff >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
              m_bVect1 = 0; break;
    case '-': m_bVect1 = 0;m_bVect0 = 0; break;
  }
}

///////////////////////////////////////////////////////////////////////////////
////////// Operators and functions of expanded assignment /////////////////////

const CsTV&  CsTV::operator <<=(int nShift)
{ 
  ASSERT(nShift >= 0 && m_nBitLength >= nShift);
  m_nBitLength -= nShift;
  if (m_nBitLength==0) {
    m_bVect1 = 0; m_bVect0 = 0; return *this;
  }
  if (nShift==0) return *this;
  m_bVect1 <<= nShift;
  m_bVect0 <<= nShift;
  return *this;
}

//--------------------------- operator >> -------------------------------------
const CsTV& CsTV::operator >>=(int nShift)
{
  ASSERT(nShift >= 0); ASSERT((m_nBitLength - nShift) >= 0);
  if (nShift == 0) return *this;
  if ((m_nBitLength - nShift)==0)  {
    m_bVect1 = 0; m_bVect0 = 0; return *this;
  }
  m_bVect1 = m_bVect1 >> (BITS_COUNT+nShift - m_nBitLength) << (BITS_COUNT - m_nBitLength);
  m_bVect0 = m_bVect1 >> (BITS_COUNT+nShift - m_nBitLength) << (BITS_COUNT - m_nBitLength);
  return *this;
}

//--------------------------- operator &= -------------------------------------
const CsTV& CsTV::operator &=(const ULONG maska)
{ m_bVect1 &= maska; m_bVect0 &= maska; return *this; }

//--------------------------- operator &= -------------------------------------
const CsTV& CsTV::operator &=(const CsBV& maska)
{ 
  ASSERT(m_nBitLength == maska.GetBitLength());
  m_bVect1 &= (ULONG)maska; m_bVect0 &= (ULONG)maska; return *this;
}

//--------------------------- operator &= -------------------------------------
const CsTV& CsTV::operator &=(const CsTV& tv)
{ 
  ASSERT(m_nBitLength == tv.GetBitLength());
  m_bVect1 |= tv.m_bVect1; m_bVect0 |= tv.m_bVect0; return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Logic operations ////////////////////////////////
//--------------------------- operator << -------------------------------------

//--------------------------- operator << ------------------------------
STD(CsTV) operator <<(const CsTV& tv, int nShift)
{ 
  CsTV s;
  s.LeftShiftVect(tv.m_bVect1, tv.m_bVect0, tv.m_nBitLength, nShift);  
  return s;
}

//--------------------------- operator >> ------------------------------
STD(CsTV) operator >>(const CsTV& tv, int nShift)
{ 
  CsTV s;
  s.RightShiftVect(tv.m_bVect1,tv.m_bVect0,tv.m_nBitLength,nShift);  
  return s;
}

//--------------------------- operator &= ------------------------------
STD(CsTV) operator&=(const CsTV& tv, const ULONG maska)
{
  CsTV tvTag;
  tvTag.m_bVect1 = (ULONG)tv.m_bVect1 & maska;
  tvTag.m_bVect0 = (ULONG)tv.m_bVect0 & maska;
  tvTag.m_nBitLength = tv.m_nBitLength;
  return tvTag;
}

//--------------------------- operator &= ------------------------------
STD(CsTV) operator&=(const CsTV& tv, const CsBV& maska)
{ 
  ASSERT(tv.m_nBitLength == maska.GetBitLength());
  CsTV tvTag;
  tvTag.m_bVect1 = (ULONG)tv.m_bVect1 & (ULONG)maska;
  tvTag.m_bVect0 = (ULONG)tv.m_bVect0 & (ULONG)maska;
  tvTag.m_nBitLength = tv.m_nBitLength;
  return tvTag;
}

//--------------------------- operator &= ------------------------------
STD(CsTV) operator&=(const CsTV& tv1, const CsTV& tv2)
{ 
  ASSERT(tv1.m_nBitLength == tv2.GetBitLength());
  CsTV tvTag;
  tvTag.m_bVect1 = (ULONG)tv1.m_bVect1 & (ULONG)tv2.m_bVect1;
  tvTag.m_bVect0 = (ULONG)tv1.m_bVect0 & (ULONG)tv2.m_bVect0;
  tvTag.m_nBitLength = tv1.m_nBitLength;
  return tvTag;
}


//--------------------------- InvertDefBit ------------------------------------
void CsTV::InvertDefs()
{ ASSERT(m_nBitLength > 0);
  ULONG a;
  a = m_bVect0; m_bVect0 = m_bVect1; m_bVect1 = a;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////// Logic Operations of weighting, finding and casing /////

//--------------------------- CountDefs ---------------------------------------
int CsTV::CountDefs() const
{ return COUNTLONG(m_bVect1 | m_bVect0); }

//--------------------------- CountUnDefs -------------------------------------
int CsTV::CountUnDefs() const
{ return (m_nBitLength-COUNTLONG(m_bVect1 | m_bVect0)); }

//--------------------------- CountOnes ---------------------------------------
int CsTV::CountOnes() const
{ return COUNTLONG(m_bVect1); }

//--------------------------- CountZeros --------------------------------------
int CsTV::CountZeros() const
{ return COUNTLONG(m_bVect0); }

//--------------------------- LeftUnDef ---------------------------------------
int CsTV::LeftUnDef(int nNext) const
{
  ASSERT((nNext >= -1) && (m_nBitLength > nNext) );
  for (int i = nNext + 1; i < m_nBitLength; i++)
    if (!(m_bVect0 & sOB[i]) && !(m_bVect1 & sOB[i])) return i;
  return (-1);
}

//--------------------------- LeftDef -----------------------------------------
int CsTV::LeftDef(int nNext) const
{
  ASSERT((nNext >= -1) && (m_nBitLength > nNext) );
  for (int i = nNext + 1; i < m_nBitLength; i++)
    if ((m_bVect0^m_bVect1) & sOB[i]) return i;
  return (-1);
}

//--------------------------- LeftOne -----------------------------------------
int CsTV::LeftOne(int nNext) const
{
  ASSERT((nNext >= -1) && (m_nBitLength > nNext) );
  for (int i = nNext + 1; i < m_nBitLength; i++)
    if ((m_bVect1 & sOB[i]) && !(m_bVect0 & sOB[i])) return i;
  return (-1);
}

//--------------------------- LeftZero ----------------------------------------
int CsTV::LeftZero(int nNext) const
{
  ASSERT((nNext >= -1) && (m_nBitLength > nNext) );
  for (int i = nNext + 1; i < m_nBitLength; i++)
    if ((m_bVect0 & sOB[i]) && !(m_bVect1 & sOB[i])) return i;
  return (-1);
}

//--------------------------- RightUnDef --------------------------------------
int CsTV::RightUnDef(int nPrev) const
{
  ASSERT((m_nBitLength >= nPrev) && (nPrev > 0));
  for (int i = nPrev - 1; i >= 0; i--)
    if ((m_bVect0 & sOB[i]) && (m_bVect1 & sOB[i])) return i;
  return (-1);
}

//--------------------------- RightDef ----------------------------------------
int CsTV::RightDef(int nPrev) const
{
  ASSERT((m_nBitLength >= nPrev) && (nPrev > 0));
  for (int i = nPrev - 1; i >= 0; i--)
    if ((m_bVect0 ^ m_bVect1) & sOB[i]) return i;
  return (-1);
}

//--------------------------- RightOne ----------------------------------------
int CsTV::RightOne(int nPrev) const
{
  ASSERT((m_nBitLength >= nPrev) && (nPrev > 0));
  for (int i = nPrev - 1; i >= 0; i--)
    if (!(m_bVect0 & sOB[i]) && (m_bVect1 & sOB[i])) return i;
  return (-1);
}

//--------------------------- RightZero ---------------------------------------
int CsTV::RightZero(int nPrev) const
{
  ASSERT((m_nBitLength >= nPrev) && (nPrev > 0));
  for (int i = nPrev - 1; i >= 0; i--)
    if ((m_bVect0 & sOB[i]) && !(m_bVect1 & sOB[i])) return i;
  return (-1);
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////// Comparing operations /////////////////////////////

//--------------------------- IsCorrect ---------------------------------------
BOOL CsTV::IsCorrect() const
{ return ((m_bVect1 & m_bVect0)==0); }

//--------------------------- IsBool ------------------------------------------
BOOL CsTV::IsBool() const
{
 if (m_nBitLength == 0) return FALSE;     // 27.05.02
 ULONG mask = 0xffffffff >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
 return ((m_bVect1 | m_bVect0)==mask); 
}

//--------------------------- IsTrivial ---------------------------------------
BOOL CsTV::IsTrivial() const
{ return ((m_bVect1 | m_bVect0)==0); }

//----------------- IsOne -----------------------------------------------------
BOOL CsTV::IsOne() const
{
  if (m_nBitLength == 0) return FALSE;
  ULONG mask = 0xffffffff >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
  if((m_bVect1^mask) != 0)  return FALSE;
  return TRUE;
}

//----------------- IsZero ----------------------------------------------------
BOOL CsTV::IsZero() const
{
  if (m_nBitLength == 0) return FALSE;
  ULONG mask = 0xffffffff >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
  if((m_bVect0^mask) != 0)  return FALSE;
  return TRUE;
}

//--------------------------- ExistOne ----------------------------------------
BOOL CsTV::ExistOne() const
{
  ULONG mask = 0xffffffff >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
  if((m_bVect1&mask) == 0)  return FALSE;
  return TRUE;
}

//--------------------------- ExistZero ---------------------------------------
BOOL CsTV::ExistZero() const
{
  ULONG mask = 0xffffffff >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
  if((m_bVect0&mask) == 0)  return FALSE;
  return TRUE;
}

//--------------------------- IsOrthog ----------------------------------------
BOOL CsTV::IsOrthog(const CsTV& tv) const
{ 
  return ((m_bVect1 & tv.m_bVect0)||(m_bVect0 & tv.m_bVect1) != 0);
}

//--------------------------- IsOrthog ----------------------------------------
BOOL CsTV::IsOrthog(const CsBV& bv) const
{
  return ((m_bVect1 & ~(ULONG)bv) || (m_bVect0 & (ULONG)bv) != 0);
}

//--------------------------- IsNeighb ----------------------------------------
BOOL CsTV::IsNeighb(const CsTV& tv) const   //соседство
{
  ULONG  mas1, mas0;
  ASSERT(m_nBitLength==tv.m_nBitLength);
  mas1 = m_bVect1 ^ tv.m_bVect1;
  mas0 = m_bVect0 ^ tv.m_bVect0;
  if (mas1 != mas0) return FALSE;
  if(COUNTLONG(mas1) != 1) return FALSE;
  else return TRUE;
}

//--------------------------- IsNeighb ----------------------------------------
BOOL CsTV::IsNeighb(const CsBV& bv) const
{
  ULONG  mas1, mas0;
  ASSERT(m_nBitLength==bv.GetBitLength());
  mas1 = m_bVect1 ^ (ULONG)bv;
  mas0 = m_bVect0 ^ ~(ULONG)bv;
  if (mas1 != mas0) return FALSE;
  if(COUNTLONG(mas1) != 1) return FALSE;
  else return TRUE;
}

//--------------------------- IsAdjac -----------------------------------------
BOOL CsTV::IsAdjac(const CsTV& tv) const    //смежность
{
  ULONG maska;
  ASSERT(m_nBitLength==tv.m_nBitLength);
  maska = (m_bVect1 & tv.m_bVect0)|(m_bVect0 & tv.m_bVect1);
  if(COUNTLONG(maska) != 1) return FALSE;
  else return TRUE;
}

//--------------------------- IsAdjac -----------------------------------------
BOOL CsTV::IsAdjac(const CsBV& bv) const
{
  ULONG maska;
  ASSERT(m_nBitLength==bv.GetBitLength());
  maska = (m_bVect1 & ~(ULONG)bv)|(m_bVect0 & (ULONG)bv);
  if(COUNTLONG(maska) != 1) return FALSE;
  else return TRUE;
}

//--------------------------- IsCover -----------------------------------------
BOOL CsTV::IsCover(const CsTV& tv) const
{
  ASSERT(m_nBitLength==tv.m_nBitLength);
  return !((((m_bVect1 & tv.m_bVect1)^m_bVect1)!= 0) ||
          (((m_bVect0 & tv.m_bVect0)^m_bVect0)) != 0);
}

//--------------------------- IsCover -----------------------------------------
BOOL CsTV::IsCover(const CsBV& bv) const
{
  ASSERT(m_nBitLength==bv.GetBitLength());
  return !((m_bVect1 & ~(ULONG)bv) || (m_bVect0 & (ULONG)bv));
}

//--------------------------- IsIntersec --------------------------------------
BOOL CsTV::IsIntersec(const CsTV& tv) const
{
  ASSERT(m_nBitLength==tv.m_nBitLength);
  return !((m_bVect1 & tv.m_bVect0)||(m_bVect0 & tv.m_bVect1));
}

//--------------------------- operator == -------------------------------------
STD(BOOL) operator ==(const CsTV& tv1,const CsTV& tv2)
{ return tv1.Equality(tv2.m_bVect1,tv2.m_bVect0,tv2.m_nBitLength);}

//--------------------------- operator == -------------------------------------
STD(BOOL) operator ==(const CsTV& tv1,const CsBV& bv2)
{ return tv1.EqualBool((ULONG)bv2,bv2.GetBitLength());}

//--------------------------- operator == -------------------------------------
STD(BOOL) operator ==(const CsBV& bv1,const CsTV& tv2)
{ return tv2.EqualBool((ULONG)bv1,bv1.GetBitLength());}

//--------------------------- operator != -------------------------------------
STD(BOOL) operator !=(const CsTV& tv1,const CsTV& tv2)
{ return !(tv1.Equality(tv2.m_bVect1,tv2.m_bVect0,tv2.m_nBitLength));}

//--------------------------- operator != -------------------------------------
STD(BOOL) operator !=(const CsTV& tv1,const CsBV& bv2)
{ return !(tv1.EqualBool((ULONG)bv2,bv2.GetBitLength()));}

//--------------------------- operator != -------------------------------------
STD(BOOL) operator !=(const CsBV& bv1,const CsTV& tv2)
{ return !(tv2.EqualBool((ULONG)bv1,bv1.GetBitLength()));}

//--------------------------- operator > --------------------------------------
STD(BOOL) operator >(const CsTV& tv1,const CsTV& tv2)
{ return (tv1.Pogl(tv2.m_bVect1,tv2.m_bVect0,tv2.m_nBitLength)) ; }

//--------------------------- operator < --------------------------------------
STD(BOOL) operator <(const CsTV& tv1,const CsTV& tv2)
{ return (tv2.Pogl(tv1.m_bVect1,tv1.m_bVect0,tv1.m_nBitLength)) ; }

//--------------------------- operator >= -------------------------------------
STD(BOOL) operator>=(const CsTV& tv1, const CsTV& tv2)
{ return tv1.IsCover(tv2); }

//--------------------------- operator <= -------------------------------------
STD(BOOL) operator<=(const CsTV& tv1, const CsTV& tv2)
{ return tv2.IsCover(tv1); }

///////////////////////////////////////////////////////////////////////////////
//////////////////////////// Analizing operations /////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////// Operations on intervals /////////////////////////////
//--------------------------- NumNeighb ---------------------------------------
//номер компоненты, по кот.соседство,иначе -1
int CsTV::NumNeighb(const CsTV& tv) const   
{
  int num=-1;
  ULONG  mas1, mas0;
  mas1 = m_bVect1 ^ tv.m_bVect1;
  mas0 = m_bVect0 ^ tv.m_bVect0;
  if (mas1 != mas0) return -1;
  if(COUNTLONG(mas1) != 1) return -1;
  while (!(mas1 & sOB[++num]));
  return num;
}
//--------------------------- NumNeighb ---------------------------------------
//номер компоненты, по кот.соседство,иначе -1

int CsTV::NumNeighb(const CsBV& bv) const   
{
  int num=-1;
  ULONG  mas1, mas0;
  mas1 = m_bVect1 ^ (ULONG)bv;
  mas0 = m_bVect0 ^ ~(ULONG)bv;
  if (mas1 != mas0) return -1;
  if(COUNTLONG(mas1) != 1) return -1;
  while (!(mas1 & sOB[++num]));
  return num;
}
//--------------------------- NumAdjacent -------------------------------------
//номер компоненты, по кот.сьежны,иначе -1

int CsTV::NumAdjac(const CsTV& tv) const  
{
  int num=-1;
  ULONG maska;
  ASSERT(m_nBitLength==tv.m_nBitLength);
  maska = (m_bVect1 & tv.m_bVect0)|(m_bVect0 & tv.m_bVect1);
  if(COUNTLONG(maska) != 1) return -1;
  while (!(maska & sOB[++num]));
  return num;
}

//--------------------------- NumAdjacent -------------------------------------
//номер компоненты, по кот.смежны,иначе -1

int CsTV::NumAdjac(const CsBV& bv) const
{
  int num=-1;
  ULONG maska;
  ASSERT(m_nBitLength==bv.GetBitLength());
  maska = (m_bVect1 & ~(ULONG)bv)|(m_bVect0 & (ULONG)bv);
  if(COUNTLONG(maska) != 1) return -1;
  while (!(maska & sOB[++num]));
  return num;
}

//--------------------------- Orthogon ----------------------------------------
CsBV CsTV::Orthogon(const CsTV& tv) const    //мн-во ортогон.компонент
{
  ASSERT(m_nBitLength==tv.m_nBitLength);
  CsBV bvTag((m_bVect1 & tv.m_bVect0)|(m_bVect0 & tv.m_bVect1), 
             tv.m_nBitLength);
  return bvTag;
}

//--------------------------- Orthogon ----------------------------------------
CsBV CsTV::Orthogon(const CsBV& bv) const    //мн-во ортогон.компонент
{ 
  ASSERT(m_nBitLength==bv.GetBitLength());
  CsBV bvTag((m_bVect1 & ~(ULONG)bv) |(m_bVect0 & (ULONG)bv),
             m_nBitLength);
  return bvTag;
}

//--------------------------- Intersec ----------------------------------------
CsTV CsTV::Intersec(const CsTV& tv) const //рез-т миним.покрытия,если орт.- NULL
{
  ASSERT(m_nBitLength==tv.m_nBitLength);
  CsTV tvTag;
  if(((m_bVect1 & tv.m_bVect0)|(m_bVect0 & tv.m_bVect1)) != 0) 
    { tvTag.Empty(); }
  else
  {
    tvTag.m_nBitLength = tv.m_nBitLength;
    tvTag.m_bVect1 = (m_bVect1 | tv.m_bVect1);
    tvTag.m_bVect0 = (m_bVect0 | tv.m_bVect0);
  }
  return tvTag;
}

//--------------------------- Concens -----------------------------------------
CsTV CsTV::Concens(const CsTV& tv) const    //рез-т обобщ.склеивани
{
  ULONG maska;
  ASSERT(m_nBitLength==tv.m_nBitLength);
  CsTV tvTag;
  tvTag.m_nBitLength = tv.m_nBitLength;
  maska=(m_bVect1 & tv.m_bVect0)|(m_bVect0 & tv.m_bVect1);
  tvTag.m_bVect1 = (m_bVect1 | tv.m_bVect1) ^ maska;
  tvTag.m_bVect0 = (m_bVect0 | tv.m_bVect0) ^ maska;
  return tvTag;
}

//--------------------------- Concens -----------------------------------------
CsTV CsTV::Concens(const CsBV& bv) const    //рез-т обобщ.склеивани
{
  ULONG maska;
  ASSERT(m_nBitLength==bv.GetBitLength());
  CsTV tvTag;
  tvTag.m_nBitLength = bv.GetBitLength();
  maska=(m_bVect1 & ~(ULONG)bv)|(m_bVect0 & (ULONG)bv);
  tvTag.m_bVect1 = (m_bVect1 | (ULONG)bv) ^ maska;
  tvTag.m_bVect0 = (m_bVect0 | ~(ULONG)bv) ^ maska;
  return tvTag;
}

//--------------------------- MinCover ----------------------------------------
CsTV CsTV::MinCover(const CsTV& tv) const    //рез-т миним.покрыт
{
  ASSERT(m_nBitLength==tv.m_nBitLength);
  CsTV tvTag;
  tvTag.m_nBitLength = tv.m_nBitLength;
  tvTag.m_bVect1 = (m_bVect1 & tv.m_bVect1);
  tvTag.m_bVect0 = (m_bVect0 & tv.m_bVect0);
  return tvTag;
}

//--------------------------- MinCover ----------------------------------------
CsTV CsTV::MinCover(const CsBV& bv) const    //рез-т миним.покрыти
{
  ASSERT(m_nBitLength==bv.GetBitLength());
  CsTV tvTag;
  tvTag.m_nBitLength = bv.GetBitLength();
  tvTag.m_bVect1 = (m_bVect1 & (ULONG)bv);
  tvTag.m_bVect0 = (m_bVect0 & ~(ULONG)bv);
  return tvTag;
}

//--------------------------- CuTVtoCuBM --------------------------------------------------
CsBM CsTV::CsTVtoCsBM() const
{
  int i, j, nex, ves, count, newR;
  CsBV maska(m_nBitLength, FALSE);
  maska = (~(m_bVect1 | m_bVect0))>>(BITS_COUNT-m_nBitLength)<<(BITS_COUNT-m_nBitLength);

  ves=maska.CountBit();
  CsBM bmTag(1<<ves, m_nBitLength); bmTag.SetSize(0, m_nBitLength);
  bmTag.Add(m_bVect1);
  for(i=0, count=1, nex=-1; i < ves; i++) {
    nex = maska.LeftOne(nex);
    for(j=0, newR = count; j < count; j++, newR++) {
      bmTag.Add(bmTag, j); bmTag.SetBitAt(newR, nex,1);
    }
    count<<=1;
  }
  return bmTag;
}


//////////////////// Operations on TERNARY VECTOR PARTS ///////////////////////
/////////////////////////// Logic operations //////////////////////////////////
//--------------------------- OrInPlace ---------------------------------------
void CsTV::OrInPlace(BOOL Part,const CsBV& bv)
{
  ASSERT(bv.GetBitLength() == m_nBitLength);
  if(Part) m_bVect1 |= (ULONG)bv;
  else     m_bVect0 |= (ULONG)bv;
  return;
}

//--------------------------- AndInPlace --------------------------------------
void CsTV::AndInPlace(BOOL Part,const CsBV& bv)
{
  ASSERT(bv.GetBitLength() == m_nBitLength);
  if(Part) m_bVect1 &= (ULONG)bv;
  else     m_bVect0 &= (ULONG)bv;
  return;
}

//--------------------------- XorInPlace --------------------------------------
void CsTV::XorInPlace(BOOL Part,const CsBV& bv)
{
  ASSERT(bv.GetBitLength() == m_nBitLength);
  if(Part) m_bVect1 ^= (ULONG)bv;
  else     m_bVect0 ^= (ULONG)bv;
  return;
}

//--------------------------- SubInPlace --------------------------------------
void CsTV::SubInPlace(BOOL Part,const CsBV& bv)
{
  ASSERT(bv.GetBitLength() == m_nBitLength);
  if(Part) m_bVect1 &= ~(ULONG)bv;
  else     m_bVect0 &= ~(ULONG)bv;
  return;
}

//--------------------------- InvertBitsInPlace -------------------------------
void CsTV::InvertBitsInPlace(BOOL Part)
{
  ULONG mask = 0xffffffff >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
  if(Part) { m_bVect1 = ~m_bVect1 & mask; }
  else     { m_bVect0 = ~m_bVect0 & mask; }
}

//--------------------------- InvertBits --------------------------------------
CsBV CsTV::InvertBits(BOOL Part) const
{
  ULONG mask = 0xffffffff >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
  CsBV bvTag(m_nBitLength,FALSE);
  if(Part) { bvTag = ~m_bVect1 & mask; }
  else     { bvTag = ~m_bVect0 & mask; }
  return bvTag;
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////// Input/output operations ////////////////////////////
///
///
/*
#ifndef _LINUX
//---------------- operator<<(CArchive& ar, const CsTV& tv) -------------

CArchive& AFXAPI operator<<(CArchive& ar, const CsTV& tv)
{
  ar << tv.m_nBitLength;
  ar << (ULONG)tv.m_bVect1;
  ar << (ULONG)tv.m_bVect0;
  return ar;
}

//---------------- operator>>(CArchive& ar, const CsTV& tv) -------------
CArchive& AFXAPI operator>>(CArchive& ar, CsTV& tv)
{
  tv.Empty();
  ar >> tv.m_nBitLength;
  ar >> (ULONG)tv.m_bVect1;
  ar >> (ULONG)tv.m_bVect0;
  return ar;
}
#endif
*/
///////////////////////////////////////////////////////////////////////////////
////////////////////////////// Advanced dump of memmory ///////////////////////
//----------------- SetSize(int nNewLength,int nNewAllocLength)----------------
void CsTV::SetSize(int nNewLength)
{
  if (nNewLength==m_nBitLength || nNewLength>BITS_COUNT)  return;
  ASSERT(nNewLength >= 0);
  if (nNewLength > m_nBitLength) 
    m_nBitLength = nNewLength;
  else
  {
    m_nBitLength = nNewLength;
    m_bVect1 = m_bVect1 >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
    m_bVect0 = m_bVect0 >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
  }
}


//********************* Protected functions ***********************************

//---------------------- AssignChar -------------------------------------------
void CsTV::AssignChar(int nLenBit, const char* pch)
{
  ASSERT(nLenBit <= BITS_COUNT);
  CharBit(pch);
  m_nBitLength = nLenBit;
}

//----------------------- CharBit ---------------------------------------------
void CsTV::CharBit(const char* pch)
{ int i;
  m_bVect1 = 0;
  m_bVect0 = 0;
  for (i=0; i < BITS_COUNT && *pch; i++, pch++) {
    if (*pch=='1' || *pch=='+') m_bVect1 |= sOB[i];
    else if (*pch=='0' || *pch=='+') m_bVect0 |= sOB[i];
  }
}

//--------------------------- EqualBool ---------------------------------------
BOOL CsTV::EqualBool(const ULONG Vect,int BitLen) const
{
  if ((Vect==0 && m_bVect1 != 0) || (m_nBitLength != BitLen)) return FALSE;
  if ((m_bVect1 != Vect) || (m_bVect0 != ~Vect)) return FALSE;
  return TRUE;
}

//--------------------------- Equality ----------------------------------------
BOOL CsTV::Equality(const ULONG Vect1,const ULONG Vect0,int BitLen) const
{
  if ((Vect1==0 && m_bVect1 != 0) || (m_nBitLength != BitLen)) return FALSE;
  if ((m_bVect1 != Vect1) || (m_bVect0 != Vect0)) return FALSE;
  return TRUE;
}

//--------------------------- Extr --------------------------------------------
void CsTV::Extr(const ULONG SrcVect1,const ULONG SrcVect0,
                int SrcBitLen,int nFirst,int nCount)
{ 
  ASSERT(nCount >= 0);  ASSERT(nFirst >= 0);
  ASSERT((nCount+nFirst) <= SrcBitLen);
  m_nBitLength = nCount;
  m_bVect1 = SrcVect1;
  m_bVect0 = SrcVect0;
  if (SrcBitLen == nCount) return; 
  if (nFirst>0)
  { m_bVect1 <<= nFirst; m_bVect0 <<= nFirst; }
  m_bVect1 = m_bVect1 >> (BITS_COUNT - nCount) << (BITS_COUNT - nCount);
  m_bVect0 = m_bVect0 >> (BITS_COUNT - nCount) << (BITS_COUNT - nCount);
}

//--------------------------- LeftShiftVect -----------------------------------
void CsTV::LeftShiftVect(const ULONG SrcVect1,const ULONG SrcVect0,
                        int nSrcLen,int nShift)
{
  ASSERT(nShift >= 0 && nSrcLen >= nShift);
  m_nBitLength = nSrcLen;
  if (m_nBitLength==0)  return;
  if (nShift==0)        return;
  m_bVect1 <<= nShift;
  m_bVect0 <<= nShift;
}

//-------------------------- RightShiftVect -----------------------------------
void CsTV::RightShiftVect(const ULONG SrcVect1,const ULONG SrcVect0,
                         int nSrcLen,int nShift)
{
  m_nBitLength = nSrcLen;
  if ((nSrcLen - nShift)==0)  return;
  if (nShift==0) {
    m_bVect1 = SrcVect1; m_bVect0 = SrcVect0; return;
  }
  m_bVect1 = SrcVect1 >> (BITS_COUNT+nShift - nSrcLen) << (BITS_COUNT - nSrcLen);
  m_bVect0 = SrcVect0 >> (BITS_COUNT+nShift - nSrcLen) << (BITS_COUNT - nSrcLen);
}

//--------------------------- Pogl --------------------------------------------
BOOL CsTV::Pogl(const ULONG Vect1,const ULONG Vect0,int BitLen) const
{
//  int j=0;
  ASSERT(m_nBitLength==BitLen);
  if ((m_bVect1 == Vect1) && (m_bVect0 == Vect0)) return FALSE;
  if ((m_bVect1 & Vect1) != m_bVect1)             return FALSE;
  if ((m_bVect0 & Vect0) != m_bVect0)             return FALSE;
  return TRUE;
}


//---------------------------------------------------------------------------
// Конвертирование вектора tv из стандартного
// представления в "короткое"(tvS)  

void CsTV::ToShort(CTV tv)
{
  ASSERT(tv.GetBitLength()<=BITS_COUNT);
  int i, n=tv.GetBitLength();
  Empty();
  m_nBitLength=n;
  for(i=0; i*8<n; i++)
  {
    m_bVect1 |= (ULONG)(tv.GetOnes()[i])<<(24-(i*8));
    m_bVect0 |= (ULONG)(tv.GetZeros()[i])<<(24-(i*8));
  }
  return;
}

//---------------------------------------------------------------------------
// Конвертирование троичного вектора из "короткого"
// представления (tvS) в стандартное (tv)  

void CsTV::FromShort(CTV &tv)
{
  int i;
  int arLength = BITS_COUNT == 32 ? 4 : 8;
  BYTE w1[arLength], w0[arLength];

  int num = 56;
  if (BITS_COUNT == 32) {
      num = 24;
  }

  for(i=0; i*8<m_nBitLength; i++)
  {  

    w1[i] = (BYTE)((m_bVect1<<(i*8))>>num);
    w0[i] = (BYTE)((m_bVect0<<(i*8))>>num);
  }
  SetSize(m_nBitLength);
  tv.SetOnes(w1);
  tv.SetZeros(w0);
  return;
}

