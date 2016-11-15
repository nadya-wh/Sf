/////////////////////////////////////////////////////////////////
// Ver.2.1.2     10.03.2010
//               Поправлена функция int CsBV::RightOne(int nPrev) const
// Ver.2.1.1     22.01.2008
//               Добавлены функции конвертации данных базового класса булевых векторов
// Ver.2.1.0     07.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.1.1        11.02.2005
//                Change type 'unsigned int' on ULONG
// Ver.1.0        08.02.2005
/////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <limits.h>
#include <iostream>

#ifdef _JOINT
#include "ShortBool.h"
#include "BaseBool.h"
#include "archive.h"
#else
#include "../ComBool/ShortBool.h"
#include "../Common/BaseBool.h"
#include "../Com_Lib/archive.h"
#endif


class CBV;
class CBM;
#ifndef _LINUX
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////
//////////////////////// Construction/Destruction ///////////////////

//-------------------------------------------------------------------
CsBV::CsBV()   { m_nBitLength = m_bVect = 0; }

//-------------------------------------------------------------------
CsBV::CsBV(const CsBV& bvSrc)
{m_bVect =bvSrc.m_bVect;
 m_nBitLength = bvSrc.m_nBitLength;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////// More sophisticated construction /////////

//-------------------------------------------------------------------
CsBV::CsBV(int nLength, BOOL Fl)
{                      // return empty vector if invalid repeat count
  if (nLength < 1 || nLength > BITS_COUNT) {
      m_nBitLength = m_bVect = 0;
  }
  else {
    if (Fl) {   // Bits
      m_bVect = 0xffffffff >> (BITS_COUNT - nLength) << (BITS_COUNT - nLength);
    }
    else m_bVect = 0;
    m_nBitLength = nLength;
  }
}

//-------------------------------------------------------------------
CsBV::CsBV(const ULONG UintVal, int Len)
{ 
  if (Len == 0) { m_nBitLength = m_bVect = 0; }
  else   {
    m_bVect = UintVal >> (BITS_COUNT - Len) << (BITS_COUNT - Len);
    m_nBitLength = Len;
  }
}

//-------------------------------------------------------------------
CsBV::CsBV(const char* pch)
{ int nLenBit;
  if ((nLenBit = SafeStrlen(pch)) == 0) { m_nBitLength = m_bVect = 0; }
  else   {
    CharBit(nLenBit, pch);
    m_nBitLength = nLenBit;
  }
}

//-------------------------------------------------------------------
CsBV::~CsBV() {}

/////////////////////////////////////////////////////////////////////
////////////////////////////////// Reading and writing //////////////
/*
#ifndef _LINUX
//-------------------------------------------------------------------
CString CsBV::BitChar(char One, char Zero)
{ int i;
  CString res('\0', m_nBitLength);
  for (i=0; i<m_nBitLength; i++)
    if (GetBitAt(i)) res.SetAt(i, One);
    else             res.SetAt(i, Zero);
  return res;
}
#else
*/
//-------------------------------------------------------------------
char* CsBV::BitChar(char One, char Zero)
{ int i;
  char * res;
  res = (char*)malloc(m_nBitLength+1);
  
  for (i=0; i<m_nBitLength; i++)
    if (GetBitAt(i)) res[i] = One;
    else             res[i] = Zero;
  res[m_nBitLength]= 0;
  return res;
}
//#endif

/////////////////////////////////////////////////////////////////////
////////////////////// Assignment operator and functions ////////////

//-------------------------------------------------------------------
void CsBV::CharBit(int nLen, const char* pch)
{ 
  for (int j=0; j<nLen; j++) {
    if (pch[j]=='1')       m_bVect |= sOB[j];
    else 
      { ASSERT(pch[j]=='0' || pch[j]=='-'|| pch[j]=='.'); }
  }
}

//-------------------------------------------------------------------
void CsBV::One()
{ if (m_nBitLength==0) return;
  m_bVect = 0xffffffff >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
}

//-------------------------------------------------------------------
CsBV CsBV::Extract(int nFirst, int nCount)
{ CsBV s;
  ASSERT(nFirst > 0 && nFirst <BITS_COUNT);
  ASSERT(nCount <= BITS_COUNT);
  ASSERT(nFirst + nCount <=BITS_COUNT);
 
  s.m_bVect = m_bVect >> (BITS_COUNT - (nFirst + nCount)) << (BITS_COUNT - nCount);
  s.m_nBitLength = nCount;
 return s;
}


/////////////////////////////////////////////////////////////////////
//////////////////////////////////// Concatination //////////////////

//****************************** Protected function *****************//
//-------------------------------------------------------------------
void CsBV::Conc2(const ULONG Vect1, int Len1, const ULONG Vect2, int Len2)
{ 
  m_nBitLength = Len1 + Len2;
  ASSERT(m_nBitLength <= BITS_COUNT);
  if (m_nBitLength == 0) 
    { m_bVect = 0; return;}
  m_bVect = Vect1 | (Vect2 >> (BITS_COUNT - Len2) << (BITS_COUNT - m_nBitLength));
}

//-------------------------------------------------------------------
void CsBV::Concat(BOOL Bit/*=FALSE*/)
{ Concat(Bit ? 0x80000000:0, 1); }

//-------------------------------------------------------------------
void CsBV::Concat(const ULONG Vect, int Len)
{ int NewBitLen;
  NewBitLen = m_nBitLength + Len;
  ASSERT(NewBitLen <= BITS_COUNT);
  if (NewBitLen == 0) 
    { m_nBitLength = m_bVect = 0; return;}
  m_bVect |= Vect >> (BITS_COUNT - Len) << (BITS_COUNT - NewBitLen);
  return;
}

//-------------------------------------------------------------------
void CsBV::Concat(const CsBV& bv)
{
    Concat(bv.m_bVect, bv.m_nBitLength);
    return;
}

//-------------------------------------------------------------------
void CsBV::Concat(const CsBV& bv1, const CsBV& bv2)
{ Conc2(bv1.m_bVect, bv1.m_nBitLength, bv2.m_bVect, bv2.m_nBitLength); return; }


/////////////////////////////////////////////////////////////////////
//////////////////////// Advanced assignment operators //////////////

//-------------------------------------------------------------------
void CsBV::LeftShiftInPlace(int nShift)
{ ASSERT(nShift >= 0);
  if (nShift >= m_nBitLength)
    { m_bVect = m_nBitLength = 0; return; }
  m_bVect <<= nShift;
}

//-------------------------------------------------------------------
void CsBV::RightShiftInPlace(int nShift)
{ ASSERT(nShift >= 0);
  if (nShift >= m_nBitLength)
    { m_bVect = m_nBitLength = 0; return; }
  m_bVect = m_bVect >> (BITS_COUNT - m_nBitLength + nShift) << (BITS_COUNT - m_nBitLength);
}

//-------------------------------------------------------------------
const CsBV&  CsBV::operator <<=(int nShift)
{ LeftShiftInPlace(nShift);  return *this; }

//-------------------------------------------------------------------
const CsBV& CsBV::operator >>=(int nShift)
{ RightShiftInPlace(nShift); return *this;}

//-------------------------------------------------------------------
const CsBV& CsBV::operator |=(const CsBV& bv)
{ DizInPlace(bv.m_bVect, bv.m_nBitLength);    return *this;}

//-------------------------------------------------------------------
const CsBV& CsBV::operator |=(const ULONG Vect)
{ ASSERT(m_nBitLength>0); 
  DizInPlace(Vect>>(BITS_COUNT-m_nBitLength)<<(BITS_COUNT-m_nBitLength), m_nBitLength);
  return *this;
}

//-------------------------------------------------------------------
const CsBV& CsBV::operator &=(const CsBV& bv)
{ ConInPlace(bv.m_bVect, bv.m_nBitLength);    return *this;}

//-------------------------------------------------------------------
const CsBV& CsBV::operator &=(const ULONG Vect)
{ ASSERT(m_nBitLength>0); 
  ConInPlace(Vect>>(BITS_COUNT-m_nBitLength)<<(BITS_COUNT-m_nBitLength), m_nBitLength);
  return *this;
}

//-------------------------------------------------------------------
const CsBV& CsBV::operator ^=(const CsBV& bv)
{ Add2InPlace(bv.m_bVect, bv.m_nBitLength);    return *this;}

//-------------------------------------------------------------------
const CsBV& CsBV::operator ^=(const ULONG Vect)
{ ASSERT(m_nBitLength>0); 
  Add2InPlace(Vect>>(BITS_COUNT-m_nBitLength)<<(BITS_COUNT-m_nBitLength), m_nBitLength);
  return *this;
}

//-------------------------------------------------------------------
const CsBV& CsBV::operator -=(const CsBV& bv)
{ ConNotInPlace(bv.m_bVect, bv.m_nBitLength);    return *this;}

//-------------------------------------------------------------------
const CsBV& CsBV::operator -=(const ULONG Vect)
{ ASSERT(m_nBitLength>0); 
  ConNotInPlace(Vect>>(BITS_COUNT-m_nBitLength)<<(BITS_COUNT-m_nBitLength), m_nBitLength);
  return *this;
}

/////////////////////////////////////////////////////////////////////
////////////////////////////////// Logic operations /////////////////

//-------------------------------------------------------------------
void CsBV::LeftShiftVect(const ULONG Vect, int Len, int nShift)
{
 ASSERT(nShift >= 0);
  if (nShift >= Len)
    { m_bVect = m_nBitLength = 0; return; }
  m_bVect = Vect << nShift;
  m_nBitLength = Len;
}

//-------------------------------------------------------------------
void CsBV::RightShiftVect(const ULONG Vect, int Len, int nShift)
{ ASSERT(nShift >= 0);
  if (nShift >= Len)
    { m_bVect = m_nBitLength = 0; return; }
  m_bVect = Vect >> (BITS_COUNT - Len + nShift) << (BITS_COUNT - Len);
  m_nBitLength = Len;
}

//-------------------------------------------------------------------
void CsBV::Diz(const ULONG Vect1, int Len1, const ULONG Vect2, int Len2)
{ ASSERT(Len1 == Len2);
  m_bVect = Vect1 | Vect2;
  m_nBitLength = Len1;
}

//-------------------------------------------------------------------
void CsBV::Con(const ULONG Vect1, int Len1, const ULONG Vect2, int Len2)
{ ASSERT(Len1 == Len2);
  m_bVect = Vect1 & Vect2;
  m_nBitLength = Len1;
}

//-------------------------------------------------------------------
void CsBV::Add2(const ULONG Vect1, int Len1, const ULONG Vect2, int Len2)
{ ASSERT(Len1 == Len2);
  m_bVect = Vect1 ^ Vect2;
  m_nBitLength = Len1;
}

//-------------------------------------------------------------------
void CsBV::ConNot(const ULONG Vect1, int Len1, const ULONG Vect2, int Len2)
{ ASSERT(Len1 == Len2);
  m_bVect = Vect1 & ~Vect2;
  m_nBitLength = Len1;
}

//-------------------------------------------------------------------
void CsBV::Not(const ULONG Vect, int Len)
{ m_nBitLength = Len;
  m_bVect = ~Vect >> (BITS_COUNT - Len) << (BITS_COUNT - Len);
}

//-------------------------------------------------------------------
STD(CsBV) operator <<(const CsBV& bv, int nShift)
{ CsBV s;
  s.LeftShiftVect(bv.m_bVect, bv.m_nBitLength, nShift);
  return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator >>(const CsBV& bv, int nShift)
{ CsBV s;
  s.RightShiftVect(bv.m_bVect, bv.m_nBitLength, nShift);
  return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator |(const CsBV& bv1, const CsBV& bv2)
{ CsBV s;
  s.Diz(bv1.m_bVect, bv1.m_nBitLength, bv2.m_bVect, bv2.m_nBitLength);
  return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator |(const CsBV& bv, const ULONG Vect)
{ CsBV s;
  s.Diz(bv.m_bVect, bv.m_nBitLength, 
        Vect >> (BITS_COUNT - bv.m_nBitLength) << (BITS_COUNT - bv.m_nBitLength),
        bv.m_nBitLength);
  return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator |(const ULONG Vect, const CsBV& bv)
{ CsBV s;
  s.Diz(Vect >> (BITS_COUNT - bv.m_nBitLength) << (BITS_COUNT - bv.m_nBitLength),
        bv.m_nBitLength, bv.m_bVect, bv.m_nBitLength);
  return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator &(const CsBV& bv1, const CsBV& bv2)
{ CsBV s;
  s.Con(bv1.m_bVect, bv1.m_nBitLength, bv2.m_bVect, bv2.m_nBitLength);
  return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator &(const ULONG Vect, const CsBV& bv)
{ CsBV s;
  s.Con(Vect >> (BITS_COUNT - bv.m_nBitLength) << (BITS_COUNT - bv.m_nBitLength),
        bv.m_nBitLength, bv.m_bVect, bv.m_nBitLength);
  return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator &(const CsBV& bv, const ULONG Vect)
{ CsBV s;
  s.Con(bv.m_bVect, bv.m_nBitLength, 
        Vect >> (BITS_COUNT - bv.m_nBitLength) << (BITS_COUNT - bv.m_nBitLength),
        bv.m_nBitLength);
  return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator ^(const CsBV& bv1, const CsBV& bv2)
{ CsBV s;
  s.Add2(bv1.m_bVect, bv1.m_nBitLength, bv2.m_bVect, bv2.m_nBitLength);
  return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator ^(const ULONG Vect, const CsBV& bv)
{ CsBV s;
  s.Add2(Vect >> (BITS_COUNT - bv.m_nBitLength) << (BITS_COUNT - bv.m_nBitLength),
         bv.m_nBitLength, bv.m_bVect, bv.m_nBitLength);
  return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator ^(const CsBV& bv, const ULONG Vect)
{ CsBV s;
  s.Add2(bv.m_bVect, bv.m_nBitLength, 
         Vect >> (BITS_COUNT - bv.m_nBitLength) << (BITS_COUNT - bv.m_nBitLength),
         bv.m_nBitLength);
  return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator -(const CsBV& bv1, const CsBV& bv2)
{ CsBV s;
  s.ConNot(bv1.m_bVect, bv1.m_nBitLength, bv2.m_bVect, bv2.m_nBitLength);
  return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator -(const ULONG Vect, const CsBV& bv)
{ CsBV s;
  s.ConNot(Vect >> (BITS_COUNT - bv.m_nBitLength) << (BITS_COUNT - bv.m_nBitLength),
           bv.m_nBitLength, bv.m_bVect, bv.m_nBitLength);
  return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator -(const CsBV& bv, const ULONG Vect)
{ CsBV s;
  s.ConNot(bv.m_bVect, bv.m_nBitLength, 
           Vect >> (BITS_COUNT - bv.m_nBitLength) << (BITS_COUNT - bv.m_nBitLength),
           bv.m_nBitLength);
  return s;
}

//-------------------------------------------------------------------
STD(CsBV) operator ~(const CsBV& bv)
{ CsBV s;
  s.Not(bv.m_bVect, bv.m_nBitLength);
  return s;
}

//-------------------------------------------------------------------
void CsBV::LoopLeftShift(int nShift)
{ ASSERT(nShift >= 0);
  nShift %= m_nBitLength;
  ULONG w = m_bVect >> (BITS_COUNT - nShift);
  m_bVect <<= nShift;
  m_bVect |= w << (BITS_COUNT - m_nBitLength);
}

//-------------------------------------------------------------------
void CsBV::LoopRightShift(int nShift)
{ ASSERT(nShift >= 0);
  nShift %= m_nBitLength;
  ULONG w = m_bVect >> (BITS_COUNT - (m_nBitLength - nShift));
  m_bVect <<= nShift;
  m_bVect = (m_bVect << (m_nBitLength - nShift)) | (w << (BITS_COUNT - nShift));
}


/////////////////////////////////////////////////////////////////////
////////////////////// Operation of weighting,finding and casing ////
//-------------------------------------------------------------------
int CsBV::CountBit() const
{
    BYTE* pB = (BYTE*)&m_bVect;
    if (BITS_COUNT == 32) {
        return (sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]]);
    } else {
         return (sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]] + sTabC[pB[4]] + sTabC[pB[5]] + sTabC[pB[6]] + sTabC[pB[7]]);
    }
}

//-------------------------------------------------------------------
int CsBV::LeftOne(int nNext) const
{ ASSERT((nNext >= -1) && (m_nBitLength > nNext) );
  for (int i = nNext + 1; i < m_nBitLength; i++)
    if (m_bVect & sOB[i]) return i;
  return (-1);
}

//-------------------------------------------------------------------
int CsBV::RightOne(int nPrev) const
{  ASSERT(m_nBitLength >= nPrev); // && (nPrev > 0)
  if (nPrev == -1) nPrev=m_nBitLength;   // Добавлено 24.03.2010
  for (int i = nPrev - 1; i >= 0; i--)
    if (m_bVect & sOB[i]) return i;
  return (-1);
}

/////////////////////////////////////////////////////////////////////
/////////////////////////// Comparing operations ////////////////////
//-------------------------------------------------------------------
BOOL CsBV::Equality(const ULONG Vect, int Len) const
{ if (m_nBitLength != Len)  return FALSE;
  if (m_bVect != (Vect >>(BITS_COUNT-m_nBitLength)<<(BITS_COUNT-m_nBitLength))) return FALSE;
 return TRUE;
}

//-------------------------------------------------------------------
BOOL CsBV::Pogl(const ULONG Vect, int Len, BOOL Dist) const
{ ASSERT(m_nBitLength == Len);
  ULONG V = Vect >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
  if (Dist) { 
    if ((m_bVect & V) != V)   return FALSE; 
  }
  else { 
    if ((m_bVect & V) != m_bVect) return FALSE; 
  }
  if (m_bVect == V) return FALSE; 
  return TRUE;
}

//-------------------------------------------------------------------
BOOL CsBV::PoglEq(const ULONG Vect, int Len, BOOL Dist) const
{ ASSERT(m_nBitLength == Len);
  ULONG V = Vect >> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
  if (Dist) { 
    if ((m_bVect & V) != V)   return FALSE; 
  }
  else { 
    if ((m_bVect & V) != m_bVect) return FALSE; 
  }
  return TRUE;
}
//*****************************************************************//
//-------------------------------------------------------------------
BOOL CsBV::IsZero() const
{ if (m_nBitLength == 0) return FALSE;
  return (m_bVect==0); }

//-------------------------------------------------------------------
BOOL CsBV::IsOne() const
{ if (m_nBitLength == 0) return FALSE;
  return (((~m_bVect) >> (BITS_COUNT - m_nBitLength)) == 0);
}

//-------------------------------------------------------------------
STD(BOOL) operator ==(const CsBV& bv1, const CsBV& bv2)
{ return bv1.Equality(bv2.m_bVect, bv2.m_nBitLength); }

//-------------------------------------------------------------------
STD(BOOL) operator ==(const CsBV& bv, const ULONG Vect)
{ return bv.Equality(Vect, bv.m_nBitLength); }

//-------------------------------------------------------------------
STD(BOOL) operator ==(const  ULONG Vect, const CsBV& bv)
{ return bv.Equality(Vect, bv.m_nBitLength); }

//-------------------------------------------------------------------

STD(BOOL) operator !=(const CsBV& bv1, const CsBV& bv2)
{ return !(bv1.Equality(bv2.m_bVect, bv2.m_nBitLength)); }

//-------------------------------------------------------------------
STD(BOOL) operator !=(const CsBV& bv, const  ULONG Vect)
{ return !(bv.Equality(Vect, bv.m_nBitLength));}

//-------------------------------------------------------------------
STD(BOOL) operator !=(const  ULONG Vect, const CsBV& bv)
{ return !(bv.Equality(Vect, bv.m_nBitLength)); }

//-------------------------------------------------------------------
STD(BOOL) operator >(const CsBV& bv1, const CsBV& bv2)
{ return (bv1.Pogl(bv2.m_bVect, bv2.m_nBitLength,TRUE)); }

//-------------------------------------------------------------------
STD(BOOL) operator >(const CsBV& bv, const  ULONG Vect)
{ return (bv.Pogl(Vect, bv.m_nBitLength,TRUE)); }

//-------------------------------------------------------------------
STD(BOOL) operator >(const  ULONG Vect, const CsBV& bv)
{ return (bv.Pogl(Vect, bv.m_nBitLength,FALSE)); }

//-------------------------------------------------------------------
STD(BOOL) operator <(const CsBV& bv1, const CsBV& bv2)
{ return (bv2.Pogl(bv1.m_bVect, bv1.m_nBitLength,TRUE));}

//-------------------------------------------------------------------
STD(BOOL) operator <(const CsBV& bv, const ULONG Vect)
{ return (bv.Pogl(Vect, bv.m_nBitLength,FALSE)); }

//-------------------------------------------------------------------
STD(BOOL) operator <(const  ULONG Vect, const CsBV& bv)
{ return (bv.Pogl(Vect, bv.m_nBitLength,TRUE)); }

//-------------------------------------------------------------------
STD(BOOL) operator>=(const CsBV& bv1, const CsBV& bv2)
{ return (bv1.PoglEq(bv2.m_bVect, bv2.m_nBitLength,TRUE)); }

//-------------------------------------------------------------------
STD(BOOL) operator>=(const CsBV& bv, const ULONG Vect)
{ return (bv.PoglEq(Vect, bv.m_nBitLength,TRUE)); }

//-------------------------------------------------------------------
STD(BOOL) operator>=(const ULONG Vect, const CsBV& bv)
{ return (bv.PoglEq(Vect, bv.m_nBitLength,FALSE)); }

//-------------------------------------------------------------------
STD(BOOL) operator <=(const CsBV& bv1, const CsBV& bv2)
{ return (bv2.PoglEq(bv1.m_bVect, bv1.m_nBitLength,TRUE)); }

//-------------------------------------------------------------------
STD(BOOL) operator <=(const CsBV& bv, const ULONG Vect)
{ return (bv.PoglEq(Vect, bv.m_nBitLength,FALSE)); }

//-------------------------------------------------------------------
STD(BOOL) operator<=(const ULONG Vect, const CsBV& bv)
{ return (bv.PoglEq(Vect, bv.m_nBitLength,TRUE)); }


//-------------------------------------------------------------------
// Конвертирование булева вектора bv из стандартного
// представления в "короткое"(bvS)  

//void CsBV::ToShort(CBV &bv)
void CsBV::ToShort(CBV bv)
{
    ASSERT(bv.GetBitLength()<=BITS_COUNT);
    int i, n=bv.GetBitLength();
    Empty();

    m_nBitLength=n;
        int num = 56;
    if (BITS_COUNT == 32) {
        num = 24;
    }
    for(i = 0; i * 8 < n; i++) {
        m_bVect |= (ULONG)(bv.GetByteAt(i))<<(num-(i*8)); // 56 -> 24
    }
    return;
}

//-------------------------------------------------------------------
// Конвертирование булева вектора из "короткого"
// представления (bvS) в стандартное (bv)  

void  CsBV::FromShort(CBV &bv)
{
  int i;
  bv.SetSize(m_nBitLength);
  for(i=0; i*8<m_nBitLength; i++)
    bv.SetByteAt(i,(BYTE)((m_bVect<<(i*8))>>24));
  return;
}


void  CsBV::SetSize(int nNewLength)
{ int i;
  ASSERT(nNewLength >= 0);
  ASSERT(nNewLength <=31);
  if(m_nBitLength>nNewLength) {
    ULONG l=0;
    for (i=0; i< nNewLength; i++) l|=sOB[i];
    m_bVect &= l;
  }
  m_nBitLength = nNewLength;
  return;
}
/*
#ifndef _LINUX
/////////////////////////////////////////////////////////////////////
///////////////////////// Input/output operations ///////////////////
//-------------------------------------------------------------------
#ifdef _DEBUG
CDumpContext& AFXAPI operator <<(CDumpContext& dc, const CsBV& bv)
{int i;
 CString res('\0', bv.m_nBitLength);
 for (i=0; i < bv.m_nBitLength; i++)
   if (bv.GetBitAt(i)) res.SetAt(i, '1');
   else             res.SetAt(i, '0');
 dc << res; return dc;
}
#endif //_DEBUG

//-------------------------------------- AFXAPI operator<<(CArchive& ar, const CsBV& bv)
CArchive& AFXAPI operator<<(CArchive& ar, const CsBV& bv)
{ ar << bv.m_nBitLength;
  ar << (int) bv.m_bVect;
  return ar;
}

//-------------------------------------- AFXAPI operator>>(CArchive& ar, const CsBV& bv)
CArchive& AFXAPI operator>>(CArchive& ar, CsBV& bv)
{ ar >> bv.m_nBitLength;
  ar >> bv.m_bVect;
  return ar;
}

#endif //_LINUX
*/
