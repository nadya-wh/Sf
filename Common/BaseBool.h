/////////////////////////////////////////////////////////////////
// Ver.2.1.3     09.11.2007
//               Новый метод генерации векторов GenRbvN
// Ver.2.1.2     22.02.2006
//               RemoveAllRows - сохраняет число столбцов матрицы
//               Add для CBM перестала быть inline, допускает добавление
//               всей матрицы целиком при -1, принятом по умолчанию
// Ver.2.1.1     20.07.2006
//               разнообразие генераторов  
// Ver.2.1.0     07.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.2.0.6     19.06.2006
// Ver.2.0.5     21.04.2006
// Ver.2.0.4     05.04.2006
// Ver.2.0.3     25.03.2006
// Ver.2.0.2     24.03.2006
// Ver.2.0.1     21.03.2006
// Ver.2.0.0     04.03.2006

// Ver.1.1.2     01.03.2006
// Ver.1.1.1     03.12.2004   Russian == English

/////////////////////////////////////////////////////////////////
// Boolean vector
#ifndef _BASEBOOL
#define _BASEBOOL

#include <string.h>
#include <stdlib.h>
#include <string>
#include <vector>
using namespace std;

//#ifdef _LINUX
//#undef _DEBUG
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


//#else
//#include <afxwin.h>         // MFC core and standard components
#ifndef FO_
#define FO_
#define FSTD(_x) friend _x AFXAPI 
#define STD(_x) _x AFXAPI 
#endif

//#endif

#define S_1          8
#define S_4          32
#define BIT_BYTE(x_) ((x_)/S_1)
#define BIT_LONG(x_) ((x_)/S_4)
#define LEN_BYTE(x_) (((x_)+S_1-1)/S_1)
#define LEN_LONG(x_) (((x_)+S_4-1)/S_4)
#define ADR_BIT(x_)  ((x_)%S_1)
#define ADR_BITLONG(x_)  ((x_)%S_4)

#define BYTE_1 0xff
#define LONG_1 0xffffffff

const BYTE OB[8]={128,64,32,16,8,4,2,1};
const ULONG OB4[32]=
        { 0x80000000, 0x40000000, 0x20000000, 0x10000000, 
           0x8000000,  0x4000000,  0x2000000,  0x1000000, 
            0x800000,   0x400000,   0x200000,   0x100000, 
             0x80000,    0x40000,    0x20000,    0x10000, 
              0x8000,     0x4000,     0x2000,     0x1000, 
               0x800,      0x400,      0x200,      0x100, 
                0x80,       0x40,       0x20,       0x10, 
                 0x8,        0x4,        0x2,        0x1 };

const BYTE TabC[256] = {
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


inline BYTE COUNTBIT(BYTE a) {
BYTE w=a;
 w=(w&0x55)+((w>>1)&0x55);  w=(w&0x33)+((w>>2)&0x33); return((w&0x0f)+((w>>4)&0x0f));
}

//----------- Внеклассовые функции для работы с датчиком случайных ------------
extern void SetRgrain( unsigned long NewRgrain);
extern unsigned long GetRgrain();
extern unsigned int GetRandN();
extern void SetRandMode(BOOL Fl = TRUE);
extern BOOL GetRandMode();

class CArch;

class CBV
{
public:
//******************************* Constructors\Destructor *******************************
  CBV();
  CBV(const CBV& bvSrc);
  CBV(BYTE ch, int nRepeat = 1,BOOL Fl = TRUE);
  CBV(const BYTE* pbt, int nLenBit);
  CBV(const char* pch);
  ~CBV();

//*********************** Functions for getting class's parametrs ***********************
  int GetByteLength() const;
  int GetBitLength() const;
  int GetAllocLength() const;

//*********************** Functions for generation **************************************
  CBV GenRbv (int nCol);
  CBV GenRbvMid(int nCol, int nRang);
  CBV GenRbvFix (int nCol, int nRang);

  CBV GenRbvN(int n);   // 09.11.2007
//*************************************** Reading ***************************************
  operator BYTE*() const;
  BYTE operator[](int nIndex) const;
  BYTE GetByteAt(int nIndex) const;
  BOOL GetBitAt(int nIndex) const;

  char* BitChar(char One = '1',char Zero='0', int Max=0);
  CBV Extract(int nFirst,int nCount);

//*************************************** Writing ***************************************
  void SetByteAt(int nIndex, BYTE ch);
  void SetBitAt(int nIndex, BOOL bit);

//*********************** Operators and functions of assignment *************************
  const CBV& operator=(const CBV& bvSrc);
  const CBV& operator=(const BYTE* pbt);
  const CBV& operator=(const char* pch);
  void Zero();
  void One();

//************************** Operators of advanced assignment ***************************
  const CBV& operator |=(const CBV& bv1);
  const CBV& operator |=(const BYTE* pbt);
  const CBV& operator &=(const CBV& bv1);
  const CBV& operator &=(const BYTE* pbt);
  const CBV& operator ^=(const CBV& bv1);
  const CBV& operator ^=(const BYTE* pbt);
  const CBV& operator -=(const CBV& bv1);
  const CBV& operator -=(const BYTE* pbt);
  const CBV& operator <<=(int nShift);
  const CBV& operator >>=(int nShift);

//********************************** Logic operations ***********************************
  FSTD(CBV) operator|(const CBV& bv1,const CBV& bv2);
  FSTD(CBV) operator|(const CBV& bv1,const BYTE* pbt);
  FSTD(CBV) operator|(const BYTE* pbt,const CBV& bv2);
  FSTD(CBV) operator&(const CBV& bv1,const CBV& bv2);
  FSTD(CBV) operator&(const BYTE* pbt,const CBV& bv2);
  FSTD(CBV) operator&(const CBV& bv1,const BYTE* pbt);
  FSTD(CBV) operator^(const CBV& bv1,const CBV& bv2);
  FSTD(CBV) operator^(const BYTE* pbt,const CBV& bv2);
  FSTD(CBV) operator^(const CBV& bv1,const BYTE* pbt);
  FSTD(CBV) operator-(const CBV& bv1,const CBV& bv2);
  FSTD(CBV) operator-(const BYTE* pbt,const CBV& bv2);
  FSTD(CBV) operator-(const CBV& bv1,const BYTE* pbt);
  FSTD(CBV) operator~(const CBV& bv2);
  void Invert(const BYTE* pbt);
  FSTD(CBV) operator<<(const CBV& bv1, int nShift);
  FSTD(CBV) operator>>(const CBV& bv1, int nShift);
  void LoopLeftShift(int nShift);
  void LoopRightShift(int nShift);

//******************** Operations of weighting, finding and casing **********************
  int CountBit() const;
  int LeftOne(int nNext = -1) const;
  int LeftOne(BYTE& bt) const;
  int RightOne(int nNext = -1) const;
  int RightOne(BYTE& bt) const;

//**************************** Operations of concatinations *****************************
  void Concat(const CBV& bv);
  void Concat(const BYTE* pbt, int nLength);
  void Concat(const CBV& bv1,const CBV& bv2);
  void Concat(BOOL Bit=FALSE);

//******************************* Compareing operations *********************************
  BOOL IsEmpty() const;
  BOOL IsZero() const;
  BOOL IsOne() const;
  FSTD(BOOL) operator==(const CBV& bv1, const CBV& bv2);
  FSTD(BOOL) operator==(const CBV& bv1, const BYTE* pbt);
  FSTD(BOOL) operator==(const BYTE* pbt, const CBV& bv2);
  FSTD(BOOL) operator!=(const CBV& bv1, const CBV& bv2);
  FSTD(BOOL) operator!=(const CBV& bv1, const BYTE* pbt);
  FSTD(BOOL) operator!=(const BYTE* pbt, const CBV& bv2);
  FSTD(BOOL) operator>(const CBV& bv1, const CBV& bv2);
  FSTD(BOOL) operator>(const CBV& bv1, const BYTE* pbt);
  FSTD(BOOL) operator>(const BYTE* pbt, const CBV& bv2);
  FSTD(BOOL) operator<(const CBV& bv1, const CBV& bv2);
  FSTD(BOOL) operator<(const CBV& bv1, const BYTE* pbt);
  FSTD(BOOL) operator<(const BYTE* pbt, const CBV& bv2);
  FSTD(BOOL) operator>=(const CBV& bv1, const CBV& bv2);
  FSTD(BOOL) operator>=(const CBV& bv1, const BYTE* pbt);
  FSTD(BOOL) operator>=(const BYTE* pbt, const CBV& bv2);
  FSTD(BOOL) operator<=(const CBV& bv1, const CBV& bv2);
  FSTD(BOOL) operator<=(const CBV& bv1, const BYTE* pbt);
  FSTD(BOOL) operator<=(const BYTE* pbt, const CBV& bv2);

//#ifndef _LINUX

//****************************** Input/Output operations ********************************
#ifdef _DEBUG
  friend CDumpContext& AFXAPI operator<<(CDumpContext& dc,const CBV& bv);
#endif
//  friend CArchive& AFXAPI operator<<(CArchive& ar, const CBV& bv);
//  friend CArchive& AFXAPI operator>>(CArchive& ar, CBV& bv);
//#endif
  FSTD(CArch&) operator<<(CArch& ar, const CBV& bv);
  FSTD(CArch&) operator>>(CArch& ar, CBV& bv);

//***************************** Advanced access to memory *******************************
  void Empty();
  BYTE* GetBuffer(int nMinBufLength);
  void ReleaseBuffer(int nNewLength = -1);
  BYTE* SetSize(int nNewLength,int nNewAllocLength=-1);

  void AssignDiz(int nBitLength, const BYTE* v1, const BYTE* v2);
  void AssignDiz(int nBitLength, int Num, BYTE* v1, ...);
  void AssignCon(int nBitLength, const BYTE* v1, const BYTE* v2);
  void AssignCon(int nBitLength, int Num, BYTE* v1, ...);
  void AssignXor(int nBitLength, const BYTE* v1, const BYTE* v2);
  void AssignXor(int nBitLength, int Num, BYTE* v1, ...);
  void AssignDif(int nBitLength, const BYTE* v1, const BYTE* v2);

protected:
  BYTE* m_bVect;
  int m_nBitLength;
  int m_nByteLength;
  int m_nAllocLength;

//******************************** Protected functions **********************************
  void AllocCopy(CBV& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
  void AllocBuffer(int nLen);
  void CharBit(int nLenByte,const char* pch);
  void AssignChar(int nLenBit, const char* pch);
  void AssignCopy(int nLenBit,int nLenByte, const BYTE* pbtSrcData);
  void ConcatCopy(int nSrc1Len, const BYTE* pbtSrc1Data, int nSrc2Len, const BYTE* pbtSrc2Data);
  void ConcatInPlace(int nSrcLen, const BYTE* pbtSrcData);
  void LeftShiftVect(const BYTE* SrcVect, int nSrcLen, int nShift);
  void LeftShiftInPlace(int nShift);
  void RightShiftVect(const BYTE* SrcVect, int nSrcLen, int nShift);
  void RightShiftInPlace(int nShift);
  void Diz(const BYTE* Vect1,int BitLen1,const BYTE* Vect2,int BitLen2);
  void DizInPlace(const BYTE* Vect1,int BitLen1);
  void Con(const BYTE* Vect1,int BitLen1,const BYTE* Vect2,int BitLen2);
  void ConInPlace(const BYTE* Vect1,int BitLen1);
  void Add2(const BYTE* Vect1,int BitLen1,const BYTE* Vect2,int BitLen2);
  void Add2InPlace(const BYTE* Vect1,int BitLen1);
  void Not(const BYTE* Vect1,int BitLen1);
  void ConNot(const BYTE* Vect1,int BitLen1,const BYTE* Vect2,int BitLen2);
  void ConNotInPlace(const BYTE* Vect1,int BitLen1);
  void Extr(const BYTE* SrcVect, int SrcBitLen,int nFirst,int nCount);
  void Conc(const BYTE* SrcVect, int SrcBitLen);
  void Conc2(const BYTE* Vect1, int SrcBitLen1,const BYTE* Vect2, int SrcBitLen2);
  BOOL Equality(const BYTE* Vect2,int BitLen2) const;
  BOOL Pogl(const BYTE* Vect2,int BitLen2,BOOL Dist) const;
  BOOL PoglEq(const BYTE* Vect2,int BitLen2,BOOL Dist) const;
  static void SafeDelete(BYTE* pbv);
  static int SafeStrlen(const char* pch);
  void Init();
};


extern CBV GetRandV();

// 30.01.97 -----------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// Boolean matrix

//#ifndef _LINUX
//class CBM : public CObject
//{
//    DECLARE_SERIAL(CBM);
//#else
class CBM
{
//#endif
public:
  friend class CBV;
  friend class CTV;
  friend class CTM;

//******************************* Constructors\Destructor *******************************
  CBM();
  CBM(const CBM& bm, BOOL Fl = TRUE );
  CBM(int nRow, int nColumn,BOOL Fl = FALSE);
  CBM(const BYTE* pbt, int nRow,int nColumn);
  CBM(const char* pch);
//#ifndef _LINUX
//  CBM(const CStringArray& StrAr);
//#endif
  ~CBM();

//*********************** Functions for getting class's parametrs ***********************
  int GetByteLength() const;
  int GetCountC() const;
  int GetAllocLength() const;
  int GetCountR() const;
  int GetUpperBound() const;

//*********************** Functions for generation **************************************
  CBM GenRbm(int nRow, int nCol);
  CBM GenRbmMid(int nRowl, int nCol, int nRang);
  CBM GenRbmFix(int nRowl, int nCol, int nRang);
  CBM RbmUndRow(int nRow, int nCol);

//********************************* Working with memory *********************************
  void SetSize(int nRow, int nColumn, int nGrowBy = -1,int nColGrow=0);
  void RemoveAll();
  void RemoveAllRows();
  void FreeExtra();

//********************************** Reading the data ***********************************

  BOOL GetBitAt(int nRow,int nColumn) const;
  BOOL GetBitAt(int nRow,int nColumn,BYTE * mask) const;
  BYTE GetByteAt(int nRow,int nIndex) const;
  BYTE GetByteAt(int nRow,int nIndex,BYTE * mask) const;
  BYTE* GetRow(int nIndex) const;
  CBV GetRowBv(int nRow) const;
  CBV GetRowBv(int nIndex,BYTE* mask) const;
  CBV GetColumnBv(int nColomn) const;
  CBV GetColumnBv(int nColumn,BYTE* mask) const;
  BYTE* operator[](int nIndex) const;

//#ifndef _LINUX
//  CString BitChar(char One = '1',char Zero = '0',BOOL WithNum=FALSE);
//  void BitChar(CStringArray& StrRes, BOOL WithClear=TRUE, BOOL WithNum=FALSE,
//               char One = '1',char Zero='0');
//#else
  char* BitChar(char One = '1', char Zero = '0', BOOL WithNum=FALSE);
//#endif
  void BitChar(vector <string>& StrRes, BOOL WithClear=TRUE, BOOL WithNum=FALSE, 
    char One = '1',char Zero='0');

//********************************** Select of matrix ***********************************
  CBM ExtractRows(int nFirst, int nCount) const;
  CBM ExtractColumns(int nFirst, int nCount) const;
  CBM Extract(int nFirstRow, int nFirstColumn, int nCountRow, int nCountColumn) const;
  CBM Minor(const CBV& Rows, const CBV& Columns) const;
  CBM Trans() const;

//********************************** Writing the data ***********************************
  void SetBitAt(int nRow,int nColumn, BOOL bit);
  void SetByteAt(int nRow,int nIndex, BYTE ch);
  void SetRow(int nRow,const BYTE* newRow);
  void SetRow(int nRow, const CBV& newRow);
  void SetRow(int nRow, const CBM& bm, int nbmRow);

//********************************** Working with row ***********************************
  void SetRowGrow(int nIndex, const BYTE* newRow);
  void SetRowGrow(int nIndex, const CBV& newRow);
  void SetRowGrow(int nRow, const CBM& bm, int nbmRow);
  int Add(const BYTE* newRow);
  int Add(const CBV& bv);
  int Add(const CBM& bm, int nbmRow=-1);    // 22.02.2007
  int Add(BOOL bit=FALSE,int nCount=1);
  void InsertRow(int nRow,const BYTE* newRow, int nCount = 1);
  void InsertRow(int nRow, const CBV& newRow, int nCount = 1);
  void InsertRow(int nRow,int nStartIndex, const CBM& NewBM,int nCount = 1);
  void RemoveRow(int nIndex, int nCount = 1);
  void ExchangeRow(int nRow1,int nRow2,int nCount=1);
  void SortDecr(int nBeg=0,int number=-1);
  void SortIncr(int nBeg=0,int number=-1);
  int AddCol(int numCol=1);

//*********************** Operators and functions of assignment *************************
  const CBM& operator =(const CBM& bm);
  const CBM& operator =(const char* pch);
//#ifndef _LINUX
//  const CBM& operator =(const CStringArray& StrAr);
//#endif
  void Zero(int nRow=-1);
  void One(int nRow=-1);

//**************************** Operations of concatinations *****************************
  void Concat(const CBM& bm);
  void Concat(const CBM& bm1,const CBM& bm2);

//************************** Operators of advanced assignment ***************************
  const CBM& operator|=(const CBM& bm);
  const CBM& operator|=(const CBV& bv);
  const CBM& operator|=(const BYTE* pt);
  const CBM& operator&=(const CBM& bm);
  const CBM& operator&=(const CBV& bv);
  const CBM& operator&=(const BYTE* pt);
  const CBM& operator^=(const CBM& bm);
  const CBM& operator^=(const CBV& bv);
  const CBM& operator^=(const BYTE* pt);
  const CBM& operator-=(const CBV& bv);
  const CBM& operator-=(const BYTE* pt);

//********************************** Logic operations ***********************************
  FSTD(CBM) operator|(const CBM& bm1,const CBM& bm2);
  FSTD(CBM) operator|(const CBM& bm1,const CBV& bv2);
  FSTD(CBM) operator|(const CBM& bm, const BYTE* pt);
  CBV Disjunction();
  FSTD(CBM) operator&(const CBM& bm1,const CBM& bm2);
  FSTD(CBM) operator&(const CBM& bm1,const CBV& bv2);
  FSTD(CBM) operator&(const CBM& bm, const BYTE* pt);
  CBV Conjunction();
  FSTD(CBM) operator^(const CBM& bm1,const CBM& bm2);
  FSTD(CBM) operator^(const CBM& bm1,const CBV& bv2);
  FSTD(CBM) operator^(const CBM& bm, const BYTE* pt);
  CBV BitwiseExclusive();
  FSTD(CBM) operator-(const CBM& bm1,const CBV& bv2);
  FSTD(CBM) operator-(const CBM& bm, const BYTE* pt);
  FSTD(CBM) operator~(const CBM& bm);

//******************** Operations of weighting, finding and casing **********************
  int CountBit(int nRow = -1) const;
  int CountBit(const BYTE * mask,int nRow= -1) const;
  int LeftOne(int nRow, int nNext = -1) const;
  int LeftOne(int nRow, BYTE& bt) const;
  int LeftOne(int nRow, int nNext, BYTE* mask) const;
  int RightOne(int nRow, int nPrev = -1) const;
  int RightOne(int nRow, BYTE& bt) const;
  int RightOne(int nRow, int nPrev,BYTE* mask) const;
  int FindRow (BOOL Empty, int nFRow=-1) const;
  int FindRow (BOOL Empty, BYTE* mask, int nFRow=-1) const;
  int FindRow (const CBV& bv, int nFRow=-1) const;
  int FindRow(const BYTE* pt, int nFRow=-1) const;

//******************************* Compareing operations *********************************
  BOOL IsEmpty() const;
  BOOL IsZero(int nRow = -1) const;
  BOOL IsZero(const BYTE* mask,int nRow) const;
  BOOL IsOne(int nRow = -1) const;
  BOOL IsOne(const BYTE* mask,int nRow) const;
  FSTD(BOOL) operator==(const CBM& bm1,const CBM& bm2);
  FSTD(BOOL) operator!=(const CBM& bm1,const CBM& bm2);
  FSTD(BOOL) operator>(const CBM& bm1,const CBM& bm2);
  FSTD(BOOL) operator<(const CBM& bm1,const CBM& bm2);
  FSTD(BOOL) operator>=(const CBM& bm1,const CBM& bm2);
  FSTD(BOOL) operator<=(const CBM& bm1,const CBM& bm2);
  int CoverRow(int nRow1, int nRow2, const BYTE * mask);
  int CoverRow(int nRow1, int nRow2);

//#ifndef _LINUX
//****************************** Input/Output operations ********************************
//  void Serialize(CArchive&);
#ifdef _DEBUG
  void Dump(CDumpContext&) const;
  void AssertValid() const;
#endif
//#endif //_LINUX
  void Serialize(CArch&);

  void SetRowDiz(int nRow, const BYTE* v1);
  void SetRowDiz(int nRow, const BYTE* v1, const BYTE* v2);
  void SetRowDiz(const BYTE* mask, int nRow, const BYTE* v1);
  void SetRowDiz(const BYTE* mask, int nRow, const BYTE* v1, const BYTE* v2);
  void SetRowDiz(int nRow, int Num, BYTE* v1, ...);
  void SetRowDiz(const BYTE* mask, int nRow, int Num, BYTE* v1, ...);

  void SetRowCon(int nRow, const BYTE* v1);
  void SetRowCon(int nRow, const BYTE* v1, const BYTE* v2);
  void SetRowCon(const BYTE* mask, int nRow, const BYTE* v1);
  void SetRowCon(const BYTE* mask, int nRow, const BYTE* v1, const BYTE* v2);
  void SetRowCon(int nRow, int Num, BYTE* v1, ...);
  void SetRowCon(const BYTE* mask, int nRow, int Num, BYTE* v1, ...);

  void SetRowXor(int nRow, const BYTE* v1);
  void SetRowXor(int nRow, const BYTE* v1, const BYTE* v2);
  void SetRowXor(const BYTE* mask, int nRow, const BYTE* v1);
  void SetRowXor(const BYTE* mask, int nRow, const BYTE* v1, const BYTE* v2);
  void SetRowXor(int nRow, int Num, BYTE* v1, ...);
  void SetRowXor(const BYTE* mask, int nRow, int Num, BYTE* v1, ...);

  void SetRowDif(int nRow, const BYTE* v1);
  void SetRowDif(int nRow, const BYTE* v1, const BYTE* v2);
  void SetRowDif(const BYTE* mask, int nRow, const BYTE* v1);
  void SetRowDif(const BYTE* mask, int nRow, const BYTE* v1, const BYTE* v2);

protected:
  BYTE** m_pData;
  int m_nSize;
  int m_nMaxSize;
  int m_nGrowBy;
  int m_nBitLength;
  int m_nByteLength;
  int m_nAllocLength;

//******************************** Protected functions **********************************
  void Init();
  void SafeDelete();
  void AllocMatrix(int nRow, int nColumn);
//#ifndef _LINUX
//  void CharBit(const CString s);
//  void CharBit(const CStringArray& s);
//#endif
  void CharBit(const vector <string>& s);
  void CharBit(const string s);
  void CharBit(const char* s);
};


/////////////////////////////////////////////////////////////////////////////
// Inline function declarations
////////////////////////////////////////////////////////////////////////////////////////
//                                                CBV class
////////////////////////////////////////////////////////////////////////////////////////

inline int CBV::GetByteLength() const                               //GetByteLength
{ return m_nByteLength; }

inline int CBV::GetBitLength() const                                //GetBitLength
{ return m_nBitLength; }

inline int CBV::GetAllocLength() const                              //GetAllocLength
{ return m_nAllocLength; }

inline CBV::operator BYTE*() const                     //operator const BYTE*()
{ return ( BYTE*)m_bVect; }

inline BYTE CBV::operator[](int nIndex) const                         //operator[]
{ ASSERT(nIndex >= 0); ASSERT(nIndex < m_nByteLength);
  return m_bVect[nIndex];
}

inline BYTE CBV::GetByteAt(int nIndex) const                          //GetByteAt
{ ASSERT(nIndex >= 0); ASSERT(nIndex < m_nByteLength);
  return m_bVect[nIndex];
}

inline BOOL CBV::GetBitAt(int nIndex) const                           //GetBitAt
{ ASSERT(nIndex >= 0); ASSERT(nIndex < m_nBitLength);
  return ((m_bVect[BIT_BYTE(nIndex)] & OB[ADR_BIT(nIndex)])!=0);
}

inline void CBV::SetByteAt(int nIndex, BYTE ch)                       //SetByteAt
{ ASSERT(nIndex >= 0); ASSERT(nIndex < m_nByteLength);
  m_bVect[nIndex] = ch;
}

inline void CBV::SetBitAt(int nIndex,  BOOL bit)                      //SetBitAt
{ ASSERT(nIndex >= 0); ASSERT(nIndex < m_nBitLength);
  if (bit) m_bVect[BIT_BYTE(nIndex)] |=OB[ADR_BIT(nIndex)];
  else     m_bVect[BIT_BYTE(nIndex)] &=~OB[ADR_BIT(nIndex)];
}

inline void CBV::Empty()                                              // Empty()
{ SafeDelete(m_bVect);   Init();
  ASSERT(m_nBitLength == 0); ASSERT(m_nByteLength == 0);
  ASSERT(m_nAllocLength == 0);
}

inline BOOL CBV::IsEmpty() const                                      //IsEmpty
{ return m_nByteLength == 0; }

inline const CBV& CBV::operator =(const CBV& bvSrc)              //operator =
{ AssignCopy(bvSrc.m_nBitLength,bvSrc.m_nByteLength, bvSrc.m_bVect);
  return *this;
}

inline const CBV& CBV::operator =(const char* pch)               // operator =
{ AssignChar(SafeStrlen(pch), pch); return *this; }

inline const CBV& CBV::operator =(const BYTE* pbt)                //operator =
{ ASSERT(m_nBitLength>0);
  AssignCopy(m_nBitLength,m_nByteLength, pbt);
  return *this;
}

inline void CBV::Zero()                                                // Zero
{ ASSERT(m_nByteLength>=0);         //new 24.01.00
  memset(m_bVect,0,m_nByteLength); }

inline void CBV::Init()                                        // Init() (protect)
{ m_nBitLength = m_nByteLength = m_nAllocLength = 0;
  m_bVect = NULL;
}

inline void CBV::SafeDelete(BYTE* pch)                        //SafeDelete (protect)
{ if (pch != NULL) delete [] pch; }

inline int CBV::SafeStrlen(const char* pch)                   //SafeStrlen (protect)
{ return (pch == NULL) ? 0 : strlen((char*)pch); }



////////////////////////////////////////////////////////////////////////////
// Boolean matrix

inline int CBM::GetCountR() const
{ return m_nSize; }

inline int CBM::GetUpperBound() const
{ return m_nSize-1; }

inline void CBM::RemoveAll()
{ SetSize(0,0); }

inline void CBM::RemoveAllRows()
{ SetSize(0,m_nBitLength); }   // 22.02.2007

inline BYTE* CBM::GetRow(int nIndex) const
{ ASSERT(nIndex >= 0 && nIndex < m_nSize); return m_pData[nIndex]; }

inline void CBM::SetRow(int nIndex,const BYTE* newRow)
{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
  memcpy(m_pData[nIndex],newRow,m_nByteLength);
}

//inline BYTE*& CBM::RowAt(int nIndex)
//  { ASSERT(nIndex >= 0 && nIndex < m_nSize); return m_pData[nIndex]; }

inline int CBM::Add(const BYTE* newRow)
{ int nIndex = m_nSize; SetRowGrow(nIndex, newRow); return nIndex; }

inline int CBM::Add(const CBV& bv)
{ int nIndex = m_nSize; SetRowGrow(nIndex, bv); return nIndex; }

inline BOOL CBM::IsEmpty() const
{ return (m_pData==NULL && m_nSize==0); }

inline BYTE* CBM::operator [](int nIndex) const
{return GetRow(nIndex); }

//inline BYTE*& CBM::operator [](int nIndex)
//  { ASSERT(nIndex >= 0 && nIndex < m_nSize); return m_pData[nIndex]; }

////////////////////////////////////////////////////////////////////////////
// Boolean matrix - new

inline int CBM::GetByteLength() const
{ return m_nByteLength; }

inline int CBM::GetCountC() const
{ return m_nBitLength; }

inline int CBM::GetAllocLength() const
{ return m_nAllocLength; }

inline BYTE CBM::GetByteAt(int nRow,int nIndex) const
{ ASSERT(nIndex >= 0); ASSERT(nRow >= 0);
  ASSERT(nIndex < m_nByteLength); ASSERT(nRow < m_nSize);
  return m_pData[nRow][nIndex];
}

inline BYTE CBM::GetByteAt(int nRow,int nIndex,BYTE * mask) const
{ ASSERT(nIndex >= 0); ASSERT(nRow >= 0);
  ASSERT(nIndex < m_nByteLength); ASSERT(nRow < m_nSize);
  return m_pData[nRow][nIndex] & mask[nIndex];
}


inline BOOL CBM::GetBitAt(int nRow,int nColumn) const
{ ASSERT(nColumn >= 0); ASSERT(nRow >= 0);
  ASSERT(nColumn < m_nBitLength); ASSERT(nRow < m_nSize);
  return ((m_pData[nRow][BIT_BYTE(nColumn)] & OB[ADR_BIT(nColumn)])!=0);
}


inline BOOL CBM::GetBitAt(int nRow,int nColumn, BYTE * mask) const
{ ASSERT(nColumn >= 0); ASSERT(nRow >= 0);
  ASSERT(nColumn < m_nBitLength); ASSERT(nRow < m_nSize);
  return ((m_pData[nRow][BIT_BYTE(nColumn)] &
           mask[BIT_BYTE(nColumn)] & OB[ADR_BIT(nColumn)])!=0);
}


inline void CBM::SetByteAt(int nRow,int nIndex, BYTE ch)
{ ASSERT(nIndex >= 0); ASSERT(nRow >= 0);
  ASSERT(nIndex < m_nByteLength); ASSERT(nRow < m_nSize);
  m_pData[nRow][nIndex] = ch;
}

inline void CBM::SetBitAt(int nRow,int nColumn,  BOOL bit)
{ ASSERT(nColumn >= 0); ASSERT(nRow >= 0);
  ASSERT(nColumn < m_nBitLength); ASSERT(nRow < m_nSize);
  if (bit) m_pData[nRow][BIT_BYTE(nColumn)] |=OB[ADR_BIT(nColumn)];
  else     m_pData[nRow][BIT_BYTE(nColumn)] &=~OB[ADR_BIT(nColumn)];
}

#undef AFXAPP_DATA
#define AFXAPP_DATA     NEAR
#endif
