/////////////////////////////////////////////////////////////////
// Ver.2.1.2     22.01.2008
//               Добавлены функции конвертации данных базового класса булевых векторов и матриц
// Ver.2.1.1     22.02.2007
//               В функции Add можно не указать второго операнда
//               и тогда к текущей добавятся все строки матрицы
//               Новая функция RemoveAllRows();
// Ver.2.1.0     07.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.1.1     11.02.2005
//              Change type 'unsigned int' on ULONG
// Ver.1.0     08.02.2005
/////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////
// Short Boolean vector
#ifndef _SBOOLEAN_
#define _SBOOLEAN_

#include <string.h>
#include <stdlib.h>

#ifdef _JOINT
#include "ShortBool.h"
#include "BaseBool.h"
#include "archive.h"
#else
#include "../ComBool/ShortBool.h"
#include "../Common/BaseBool.h"
#include "../Com_Lib/archive.h"
#endif

//#ifdef _LINUX
#undef _DEBUG
class CBV;
class CBM;
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
/*
#else
#include <afxwin.h>         // MFC core and standard components

#ifndef FO_
#define FO_
#define FSTD(_x) friend _x AFXAPI 
#define STD(_x) _x AFXAPI 
#endif

#endif
*/
const BYTE sTabC[256] = {
0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 
4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};  


const ULONG sOB[32]=
        { 0x80000000, 0x40000000, 0x20000000, 0x10000000, 
           0x8000000,  0x4000000,  0x2000000,  0x1000000, 
            0x800000,   0x400000,   0x200000,   0x100000, 
             0x80000,    0x40000,    0x20000,    0x10000, 
              0x8000,     0x4000,     0x2000,     0x1000, 
               0x800,      0x400,      0x200,      0x100, 
                0x80,       0x40,       0x20,       0x10, 
                 0x8,        0x4,        0x2,        0x1 };


inline int COUNTLONG(ULONG u)
{ 
 BYTE* pB= (BYTE*)&u;
 return (sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]]); 
}


class CsBV
{
public:
  friend class CBV;
//******************************* Constructors\Destructor *******************************
  CsBV();
  CsBV(const CsBV& bvSrc);
  CsBV(int nLength, BOOL Fl);
  CsBV(const ULONG UintVal, int nLenBit);
  CsBV(const char* pch);
  ~CsBV();

//  void ToShort(CBV &bv);
  void ToShort(CBV bv);
  void FromShort(CBV &bv);
//*********************** Functions for getting class's parametrs ***********************
  int GetBitLength() const;
//*************************************** Reading ***************************************
  operator ULONG () const;
  BOOL GetBitAt(int nIndex) const;
/*
#ifndef _LINUX
  CString BitChar(char One = '1',char Zero='0');
#else
*/
  char* BitChar(char One = '1',char Zero='0');
//#endif
  CsBV Extract(int nFirst,int nCount);

//*************************************** Writing ***************************************
  void SetBitAt(int nIndex, BOOL bit);
//*********************** Operators and functions of assignment *************************
  const CsBV& operator=(const CsBV& bvSrc);
  const CsBV& operator=(const ULONG pbt);
  const CsBV& operator=(const char* pch);
  void Zero();
  void One();
//************************** Operators of advanced assignment ***************************
  const CsBV& operator |=(const CsBV& bv1);
  const CsBV& operator |=(const ULONG Vect);
  const CsBV& operator &=(const CsBV& bv1);
  const CsBV& operator &=(const ULONG Vect);
  const CsBV& operator ^=(const CsBV& bv1);
  const CsBV& operator ^=(const ULONG Vect);
  const CsBV& operator -=(const CsBV& bv1);
  const CsBV& operator -=(const ULONG Vect);
  const CsBV& operator <<=(int nShift);
  const CsBV& operator >>=(int nShift);
//********************************** Logic operations ***********************************
  FSTD(CsBV) operator|(const CsBV& bv1,const CsBV& bv2);
  FSTD(CsBV) operator|(const CsBV& bv1,const ULONG Vect);
  FSTD(CsBV) operator|(const ULONG Vect,const CsBV& bv2);
  FSTD(CsBV) operator&(const CsBV& bv1,const CsBV& bv2);
  FSTD(CsBV) operator&(const ULONG Vect,const CsBV& bv2);
  FSTD(CsBV) operator&(const CsBV& bv1,const ULONG Vect);
  FSTD(CsBV) operator^(const CsBV& bv1,const CsBV& bv2);
  FSTD(CsBV) operator^(const ULONG Vect,const CsBV& bv2);
  FSTD(CsBV) operator^(const CsBV& bv1,const ULONG Vect);
  FSTD(CsBV) operator-(const CsBV& bv1,const CsBV& bv2);
  FSTD(CsBV) operator-(const ULONG Vect,const CsBV& bv2);
  FSTD(CsBV) operator-(const CsBV& bv1,const ULONG Vect);
  FSTD(CsBV) operator~(const CsBV& bv2);
  void Invert(const ULONG Vect);
  FSTD(CsBV) operator<<(const CsBV& bv1, int nShift);
  FSTD(CsBV) operator>>(const CsBV& bv1, int nShift);
  void LoopLeftShift(int nShift);
  void LoopRightShift(int nShift);
//******************** Operations of weighting, finding and casing **********************
  int  CountBit() const;
  int LeftOne(int nNext = -1) const;
  int RightOne(int nNext = -1) const;
//**************************** Operations of concatinations *****************************
  void Concat(const CsBV& bv);
  void Concat(const ULONG Vect, int nLength);
  void Concat(const CsBV& bv1,const CsBV& bv2);
  void Concat(BOOL Bit=FALSE);
//******************************* Compareing operations *********************************
  BOOL IsEmpty() const;
  BOOL IsZero() const;
  BOOL IsOne() const;
  FSTD(BOOL) operator==(const CsBV& bv1, const CsBV& bv2);
  FSTD(BOOL) operator==(const CsBV& bv1, const ULONG Vect);
  FSTD(BOOL) operator==(const ULONG Vect, const CsBV& bv2);
  FSTD(BOOL) operator!=(const CsBV& bv1, const CsBV& bv2);
  FSTD(BOOL) operator!=(const CsBV& bv1, const ULONG Vect);
  FSTD(BOOL) operator!=(const ULONG Vect, const CsBV& bv2);
  FSTD(BOOL) operator>(const CsBV& bv1, const CsBV& bv2);
  FSTD(BOOL) operator>(const CsBV& bv1, const ULONG Vect);
  FSTD(BOOL) operator>(const ULONG Vect, const CsBV& bv2);
  FSTD(BOOL) operator<(const CsBV& bv1, const CsBV& bv2);
  FSTD(BOOL) operator<(const CsBV& bv1, const ULONG Vect);
  FSTD(BOOL) operator<(const ULONG Vect, const CsBV& bv2);
  FSTD(BOOL) operator>=(const CsBV& bv1, const CsBV& bv2);
  FSTD(BOOL) operator>=(const CsBV& bv1, const ULONG Vect);
  FSTD(BOOL) operator>=(const ULONG Vect, const CsBV& bv2);
  FSTD(BOOL) operator<=(const CsBV& bv1, const CsBV& bv2);
  FSTD(BOOL) operator<=(const CsBV& bv1, const ULONG Vect);
  FSTD(BOOL) operator<=(const ULONG Vect, const CsBV& bv2);
/*
#ifndef _LINUX
//****************************** Input/Output operations ********************************
#ifdef _DEBUG
  friend CDumpContext& AFXAPI operator<<(CDumpContext& dc,const CsBV& bv);
#endif
  friend CArchive& AFXAPI operator<<(CArchive& ar, const CsBV& bv);
  friend CArchive& AFXAPI operator>>(CArchive& ar, CsBV& bv);
#endif
*/
//***************************** Advanced access to memory *******************************
  void Empty();
  void SetSize(int nNewLength);
protected:
  ULONG m_bVect;
  int m_nBitLength;
//******************************** Protected functions **********************************
  void CharBit(int nLenByte, const char* pch);
  void LeftShiftVect(const ULONG Vect, int Len, int nShift);
  void LeftShiftInPlace(int nShift);
  void RightShiftVect(const ULONG Vect, int Len, int nShift);
  void RightShiftInPlace(int nShift);
  void Diz(const ULONG Vect1, int Len1, const ULONG Vect2, int Len2);
  void DizInPlace(const ULONG Vect, int Len);
  void Con(const ULONG Vect1, int Len1, const ULONG Vect2, int Len2);
  void ConInPlace(const ULONG Vect1, int Len);
  void Add2(const ULONG Vect1, int Len1, const ULONG Vect2, int Len2);
  void Add2InPlace(const ULONG Vect, int Len);
  void Not(const ULONG Vect, int Len1);
  void ConNot(const ULONG Vect1, int Len1, const ULONG Vect2, int Len2);
  void ConNotInPlace(const ULONG Vect, int Len1);
  void Conc(const ULONG Vect, int Len);
  void Conc2(const ULONG Vect1, int Len1, const ULONG Vect2, int Len2);
  BOOL Equality(const ULONG Vect,int Len) const;
  BOOL Pogl(const ULONG Vect, int Len, BOOL Dist) const;
  BOOL PoglEq(const ULONG Vect, int Len, BOOL Dist) const;
  static int SafeStrlen(const char* pch);
};

////////////////////////////////////////////////////////////////////////////////////////
// Inlines for CsBV class
////////////////////////////////////////////////////////////////////////////////////////
//                           CsBV class
////////////////////////////////////////////////////////////////////////////////////////
inline int CsBV::GetBitLength() const  { return m_nBitLength; }

inline void CsBV::Empty() { m_nBitLength = m_bVect = 0; }
inline void CsBV::Zero()  { m_bVect = 0; }
inline BOOL CsBV::IsEmpty() const   { return m_nBitLength == 0; }

inline CsBV::operator ULONG() const { return m_bVect; }

inline BOOL CsBV::GetBitAt(int nIndex) const
{ ASSERT(nIndex >= 0); ASSERT(nIndex < m_nBitLength);
  return ((m_bVect & sOB[nIndex])!=0);
}

inline void CsBV::SetBitAt(int nIndex,  BOOL bit)
{ ASSERT(nIndex >= 0); ASSERT(nIndex < m_nBitLength);
  if (bit) m_bVect |=sOB[nIndex];
  else     m_bVect &=~sOB[nIndex];
}

inline const CsBV& CsBV::operator =(const CsBV& bvSrc)
{ m_nBitLength = bvSrc.m_nBitLength;  m_bVect = bvSrc.m_bVect;   return *this; }

inline const CsBV& CsBV::operator =(const char* pch)
{ m_nBitLength = SafeStrlen(pch);
  if (m_nBitLength > 0) CharBit(m_nBitLength, pch);
  return *this; 
}

inline const CsBV& CsBV::operator =(const ULONG IntVal)
{ if (m_nBitLength > 0)  m_bVect =IntVal >> (32 - m_nBitLength) << (32 - m_nBitLength);
  return *this;
}

inline void CsBV::DizInPlace(const ULONG Vect1, int Len1)
{ ASSERT(Len1 == m_nBitLength);   m_bVect |= Vect1; }

inline void CsBV::ConInPlace(const ULONG Vect1, int Len1)
{ ASSERT(Len1 == m_nBitLength);   m_bVect &= Vect1; }

inline void CsBV::Add2InPlace(const ULONG Vect1, int Len1)
{ ASSERT(Len1 == m_nBitLength);   m_bVect ^= Vect1; }

inline void CsBV::ConNotInPlace(const ULONG Vect1, int Len1)
{ ASSERT(Len1 == m_nBitLength);   m_bVect &= ~Vect1; }

inline void CsBV:: Invert(const ULONG Vect)
{
  m_bVect = ~Vect >> (32 - m_nBitLength) << (32 - m_nBitLength);
}

inline int CsBV::SafeStrlen(const char* pch)
{ return (pch == NULL || strlen((char*)pch) == 0) ? 0 : strlen((char*)pch); }





/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Limited on width Boolean matrix
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

/*
#ifndef _LINUX
class CsBM : public CObject
{
   DECLARE_SERIAL(CsBM)
#else
*/
class CsBM 
{
//#endif

public:
   friend class CsBV;
   friend class CBM;

//******************************* Constructors\Destructor *******************************
   CsBM();
   CsBM(const CsBM& bm, BOOL Fl = TRUE );
   CsBM(int nRow, int nColumn,BOOL Fl = FALSE);
   CsBM(const ULONG pbt, int nRow,int nColumn);
   CsBM(const char* pch);
/*
#ifndef _LINUX
   CsBM(const CStringArray& StrAr);
#else
*/
   CsBM(const vector <string>& StrAr);
//#endif
   ~CsBM();
   void ToShort(CBM &Bm);
   void FromShort(CBM &Bm);
//*********************** Functions for getting class's parametrs ***********************
   int GetCountC() const;
   int GetCountR() const;
   int GetUpperBound() const;

//********************************* Working with memory *********************************
   void SetSize(int nRow, int nColumn, int nGrowBy = -1);
   void RemoveAll();
   void RemoveAllRows();
   void FreeExtra();

//********************************** Reading the data ***********************************

   BOOL GetBitAt(int nRow,int nColumn) const;
   BOOL GetBitAt(int nRow,int nColumn, ULONG mask) const;
   ULONG GetRow(int nIndex) const;
   CsBV GetRowBv(int nRow) const;
   CsBV GetRowBv(int nIndex,ULONG mask) const;
   CsBV GetColumnBv(int nColomn) const;
   CsBV GetColumnBv(int nColumn,ULONG mask) const;
   ULONG operator[](int nIndex) const;
/*
#ifndef _LINUX
   CString BitChar(char One = '1',char Zero = '0');
   void BitChar(CStringArray & StrRes, char One = '1',char Zero = '0');
#else
*/
   char* BitChar(char One = '1',char Zero = '0');
//#endif
   void BitChar(vector <string> & StrRes, char One = '1',char Zero = '0');
//********************************** Select of matrix ***********************************
   CsBM ExtractRows(int nFirst, int nCount) const;
   CsBM ExtractColumns(int nFirst, int nCount) const;
   CsBM Extract(int nFirstRow, int nFirstColumn, int nCountRow, int nCountColumn) const;
   CsBM Minor(const CsBV& Rows, const CsBV& Columns) const;
   CsBM Trans() const;

//********************************** Writing the data ***********************************
   void SetBitAt(int nRow,int nColumn, BOOL bit);
   void SetRow(int nRow,const ULONG newRow);
   void SetRow(int nRow, const CsBV& newRow);
   void SetRow(int nRow, const CsBM& bm, int nbmRow);

//********************************** Working with row ***********************************
   void SetRowGrow(int nIndex, const ULONG newRow);
   void SetRowGrow(int nIndex, const CsBV& newRow);
   void SetRowGrow(int nRow, const CsBM& bm, int nbmRow);
   int Add(const ULONG newRow);
   int Add(const CsBV& bv);
   int Add(const CsBM& bm, int nbmRow=-1);
   int Add(BOOL bit=FALSE,int nCount=1);
   void InsertRow(int nRow,const ULONG newRow, int nCount = 1);
   void InsertRow(int nRow, const CsBV& newRow, int nCount = 1);
   void InsertRow(int nRow,int nStartIndex, const CsBM& NewBM,int nCount = 1);
   void RemoveRow(int nIndex, int nCount = 1);
   void ExchangeRow(int nRow1,int nRow2,int nCount=1);
   void SortDecr(int nBeg=0,int number=-1);
   void SortIncr(int nBeg=0,int number=-1);
   int AddCol(int numCol=1);

//*********************** Operators and functions of assignment *************************
   const CsBM& operator =(const CsBM& bm);
   const CsBM& operator =(const char* pch);
/*
#ifndef _LINUX
   const CsBM& operator =(const CStringArray& StrAr);
#else
*/
   const CsBM& operator =(const vector <string>& StrAr);
//#endif
   void Zero(int nRow=-1);
   void One(int nRow=-1);

//**************************** Operations of concatinations *****************************
   void Concat(const CsBM& bm);
   void Concat(const CsBM& bm1,const CsBM& bm2);

//************************** Operators of advanced assignment ***************************
   const CsBM& operator|=(const CsBM& bm);
   const CsBM& operator|=(const CsBV& bv);
   const CsBM& operator|=(const ULONG pt);
   const CsBM& operator&=(const CsBM& bm);
   const CsBM& operator&=(const CsBV& bv);
   const CsBM& operator&=(const ULONG pt);
   const CsBM& operator^=(const CsBM& bm);
   const CsBM& operator^=(const CsBV& bv);
   const CsBM& operator^=(const ULONG pt);
   const CsBM& operator-=(const CsBV& bv);
   const CsBM& operator-=(const ULONG pt);

//********************************** Logic operations ***********************************
   FSTD(CsBM) operator|(const CsBM& bm1,const CsBM& bm2);
   FSTD(CsBM) operator|(const CsBM& bm1,const CsBV& bv2);
   FSTD(CsBM) operator|(const CsBM& bm, const ULONG pt);
   CsBV Disjunction();
   FSTD(CsBM) operator&(const CsBM& bm1,const CsBM& bm2);
   FSTD(CsBM) operator&(const CsBM& bm1,const CsBV& bv2);
   FSTD(CsBM) operator&(const CsBM& bm, const ULONG pt);
   CsBV Conjunction();
   FSTD(CsBM) operator^(const CsBM& bm1,const CsBM& bm2);
   FSTD(CsBM) operator^(const CsBM& bm1,const CsBV& bv2);
   FSTD(CsBM) operator^(const CsBM& bm, const ULONG pt);
   CsBV BitwiseExclusive();
   FSTD(CsBM) operator-(const CsBM& bm1,const CsBV& bv2);
   FSTD(CsBM) operator-(const CsBM& bm, const ULONG pt);
   FSTD(CsBM) operator~(const CsBM& bm);

//******************** Operations of weighting, finding and casing **********************
   int CountBit(int nRow = -1) const;
   int CountBit(const ULONG mask,int nRow= -1) const;
   int LeftOne(int nRow, int nNext = -1) const;
   int LeftOne(int nRow, int nNext, ULONG mask) const;
   int RightOne(int nRow, int nPrev = -1) const;
   int RightOne(int nRow, int nPrev,ULONG mask) const;
   int FindRow (BOOL Empty, int nFRow=-1) const;
   int FindRow (BOOL Empty, ULONG mask, int nFRow=-1) const;
   int FindRow (const CsBV& bv, int nFRow=-1) const;
   int FindRow(const ULONG pt, int nFRow=-1) const;

//******************************* Compareing operations *********************************
   BOOL IsEmpty() const;
   BOOL IsZero(int nRow = -1) const;
   BOOL IsZero(const ULONG mask,int nRow) const;
   BOOL IsOne(int nRow = -1) const;
   BOOL IsOne(const ULONG mask,int nRow) const;
   FSTD(BOOL) operator==(const CsBM& bm1,const CsBM& bm2);
   FSTD(BOOL) operator!=(const CsBM& bm1,const CsBM& bm2);
   FSTD(BOOL) operator>(const CsBM& bm1,const CsBM& bm2);
   FSTD(BOOL) operator<(const CsBM& bm1,const CsBM& bm2);
   FSTD(BOOL) operator>=(const CsBM& bm1,const CsBM& bm2);
   FSTD(BOOL) operator<=(const CsBM& bm1,const CsBM& bm2);
   int CoverRow(int nRow1, int nRow2, const ULONG mask);
   int CoverRow(int nRow1, int nRow2);
/*
#ifndef _LINUX
//****************************** Input/Output operations ********************************
    void Serialize(CArchive&);
#ifdef _DEBUG
    void Dump(CDumpContext&) const;
    void AssertValid() const;
#endif
#endif
*/
    void Serialize(CArch&);

protected:
   ULONG* m_pData;
   int m_nSize;
   int m_nMaxSize;
   int m_nGrowBy;
   int m_nBitLength;

//******************************** Protected functions **********************************
    void Init();
    void AllocMatrix(int nRow, int nColumn);
/*
#ifndef _LINUX
    void CharBit(const CString s);
    void CharBit(const CStringArray& s);
#endif
*/
    void CharBit(const vector <string>& s);
    void CharBit(const string s);
    void CharBit(char* s);
};


////////////////////////////////////////////////////////////////////////////
// Boolean matrix

inline int CsBM::GetCountR() const     { return m_nSize; }
inline int CsBM::GetCountC() const     { return m_nBitLength; }
inline int CsBM::GetUpperBound() const { return m_nSize-1; }

inline void CsBM::RemoveAll() { SetSize(0,0); }

inline void CsBM::RemoveAllRows() { SetSize(0,m_nBitLength); }

inline ULONG CsBM::GetRow(int nIndex) const
{ ASSERT(nIndex >= 0 && nIndex < m_nSize); return m_pData[nIndex]; }

inline void CsBM::SetRow(int nIndex,const ULONG newRow)
{ ASSERT(nIndex >= 0 && nIndex < m_nSize);  m_pData[nIndex] = newRow; }

inline int CsBM::Add(const ULONG newRow)
{ SetRowGrow(m_nSize, newRow); return (m_nSize-1); }

inline int CsBM::Add(const CsBV& bv)
{ SetRowGrow(m_nSize, bv); return (m_nSize-1); }

inline BOOL CsBM::IsEmpty() const
{ return (m_pData==NULL && m_nSize==0); }

inline ULONG CsBM::operator [](int nIndex) const
{ return GetRow(nIndex); }

inline BOOL CsBM::GetBitAt(int nRow,int nColumn) const
{ ASSERT(nColumn >= 0); ASSERT(nRow >= 0);
  ASSERT(nColumn < m_nBitLength); ASSERT(nRow < m_nSize);
  return ((m_pData[nRow] & sOB[nColumn])!=0);
}

inline BOOL CsBM::GetBitAt(int nRow,int nColumn, ULONG mask) const
{ ASSERT(nColumn >= 0); ASSERT(nRow >= 0);
  ASSERT(nColumn < m_nBitLength); ASSERT(nRow < m_nSize);
  return ((m_pData[nRow] & mask & sOB[nColumn])!=0);
}

inline void CsBM::SetBitAt(int nRow,int nColumn,  BOOL bit)
{ ASSERT(nColumn >= 0); ASSERT(nRow >= 0);
  ASSERT(nColumn < m_nBitLength); ASSERT(nRow < m_nSize);
  if (bit) m_pData[nRow] |= sOB[nColumn];
  else     m_pData[nRow] &=~sOB[nColumn];
}

#ifndef _LINUX
#undef AFXAPP_DATA
#define AFXAPP_DATA     NEAR
#endif
#endif
