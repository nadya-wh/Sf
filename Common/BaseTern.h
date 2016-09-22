/////////////////////////////////////////////////////////////////
// Ver.2.1.3    28.03.2008
//              SetOnes и SetZeros - исправлены для реализации в другой интерпретации
//              в виде функций AddOnes и AddZeros
// Ver.2.1.1     22.02.2007
//               Новая функция RemoveAllRows - сохраняет размерность по столбцам
//               Поправка Add - теперь можно всю матрицу при втором 
//               операнде = -1
// Ver.2.1.0     07.07.2006
//               Переход к LINUX. Поправки в BitChar

/////////////////////////////////////////////////////////////////
// Ver.2.0.9     19.06.2006
// Ver.2.0.8     04.05.2006  // ошибка в inline GetByteOne
// Ver.2.0.7     21.04.2006
// Ver.2.0.6     05.04.2006
// Ver.2.0.5     02.04.2006
// Ver.2.0.4     25.03.2006
// Ver.2.0.3     24.03.2006
// Ver.2.0.2     21.03.2006
// Ver.2.0.1     18.03.2006
// Ver.2.0.0     04.03.2006

// Ver.1.1.1     03.12.2004   Russian == English

// Ver.1.0.2     18.12.2000
// Ver.1.0.1     21.10.1999
//               ошибка в  int CBM,CTM::Add(BOOL bit,int nCount)
// Ver.1.0.0     07.12.1998
//               В CTV добавлены ф-ии IsOne, IsZero,
//               В CTM - ExistOne, ExistZero

/////////////////////////////////////////////////////////////////
// Ternary vector
#ifndef _BASETERN
#define _BASETERN

#ifdef _JOINT
#include "BaseBool.h"
#include "archive.h"
#else
#include "../Common/BaseBool.h"
#include "../Com_Lib/archive.h"
#endif

////////////////////////////////////////////////////////////////////////////
//Черемисинова Л.Д.
// Ternary vector and Ternary matrix

/////////////////////////////////////////////////////////////////////////////
// Ternary vector

//#ifndef _LINUX
//class CTV   : public CObject
//#else  //_LINUX
#undef _DEBUG
class CTV
//#endif
{
public:
  friend class CBV;
  friend class CBM;

//******************************* Constructors\Destructor *********************
  CTV();
  CTV(const CTV& tvSrc);
  CTV(const CBV& bv1Src,const CBV& bv0Src);
  CTV(int nLenBit,char value='n'); 
      //1,0,-,+ - тр.вектор в кодировке 10,01,00,11,'n'-без инициализ-ии
  CTV(const BYTE* pbt1,const BYTE* pbt0,int nLenBit);
  CTV(const char* pch);
  ~CTV();

//*********************** Functions for getting class's parametrs *************
  int GetByteLength() const;
  int GetBitLength() const;
  int GetAllocLength() const;

//*********************** Functions for generation ****************************
  CTV GenRtv ( int nCol);
  CTV GenRtvMid (int nCol, int nRang);
  CTV GenRtvFix (int nCol, int nRang);

//*************************************** Data Reading ************************
  char GetBitAt(int nIndex) const;
  CTV Extract(int nFirst,int nCount);

  CBV GetOnes() const;
  CBV GetZeros() const;
  CBV GetDefs() const;
  CBV GetUnDefs() const;
  BYTE* GetOnesB() const;
  BYTE* GetZerosB() const;

//*************************************** Data Writing ************************
  void SetBitAt(int nIndex, char symb);
  void SetVect(CBV bv1Src,CBV bv0Src);
  void SetOnes(const CBV& bv);
  void SetZeros(const CBV& bv);
  void AddOnes(const CBV& bv);   // 28.03.2008
  void AddZeros(const CBV& bv);  // 28.03.2008
  void SetUnDefs(const CBV& bv);
  void SetOnes(const BYTE* bv);
  void SetZeros(const BYTE* bv);
  void AddOnes(const BYTE* bv);
  void AddZeros(const BYTE* bv);

//*********************** Operators and functions of assignment ***************
  const CTV& operator=(const CTV& tvSrc);
  const CTV& operator=(const CBV& bvSrc);
  const CTV& operator=(const char* pch);
  void Clear(char symb='-');
  void Empty();

//*********************** Operators and functions of expanded assignment ******
  const CTV& operator <<=(int nShift);
  const CTV& operator >>=(int nShift);
  const CTV& operator &=(const BYTE* maska);
  const CTV& operator &=(const CBV& maska);
  const CTV& operator &=(const CTV& tv);

//********************************** Logic operations *************************
  FSTD(CTV) operator<<(const CTV& tv, int nShift);
  FSTD(CTV) operator>>(const CTV& tv, int nShift);
  FSTD(CTV) operator&=(const CTV& tv, const BYTE* maska);
  FSTD(CTV) operator&=(const CTV& tv, const CBV& maska);
  FSTD(CTV) operator&=(const CTV& tv1, const CTV& tv2);
  void LoopLeftShift(int nShift);
  void LoopRightShift(int nShift);
  void InvertDefs();

//************ Logic Operations of weighting, finding and casing **************

  int  CountDefs() const;       // число 1 и 0
  int  CountUnDefs() const;       // число 1 и 0
  int  CountOnes() const;       // число 1
  int  CountZeros() const;      // число и 0
  int LeftUnDef(int nNext = -1) const;
  int LeftDef(int nNext = -1) const;
  int LeftOne(int nNext = -1) const;
  int LeftZero(int nNext = -1) const;
  int RightUnDef(int nNext = -1) const;
  int RightDef(int nNext = -1) const;
  int RightOne(int nNext = -1) const;
  int RightZero(int nNext = -1) const;

//**************************** Operations of concatinations *******************
  void Concat(const CTV& tv);
  void Concat(const CTV& tv1,const CTV& tv2);
  void Concat(char symb='-');

//******************************* Compareing operations ***********************
  BOOL IsEmpty() const;
  BOOL IsCorrect() const;  // кодировка 11 не корректна
  BOOL IsBool() const;     // вектор булев ?
  BOOL IsTrivial() const;  // все '-'
  BOOL IsOne() const;
  BOOL IsZero() const;
  BOOL ExistZero() const;
  BOOL ExistOne() const;
  BOOL IsOrthog(const CTV& tv) const;
  BOOL IsOrthog(const CBV& bv) const;
  BOOL IsNeighb(const CTV& tv) const; //соседство
  BOOL IsNeighb(const CBV& bv) const; //соседство
  BOOL IsAdjac(const CTV& tv) const;  //смежность
  BOOL IsAdjac(const CBV& bv) const;  //смежность
  BOOL IsCover(const CTV& tv) const;  // 1 поглощает 2
  BOOL IsCover(const CBV& bv) const;  // 1 поглощает 2
  BOOL IsIntersec(const CTV& tv) const;//пересекаютс
  FSTD(BOOL) operator==(const CTV& tv1, const CTV& tv2);
  FSTD(BOOL) operator==(const CTV& tv1, const CBV& bv2);
  FSTD(BOOL) operator==(const CBV& bv1, const CTV& tv2);
  FSTD(BOOL) operator!=(const CTV& tv1, const CTV& tv2);
  FSTD(BOOL) operator!=(const CTV& tv1, const CBV& bv2);
  FSTD(BOOL) operator!=(const CBV& bv1, const CTV& tv2);
  FSTD(BOOL) operator >(const CTV& tv1, const CTV& tv2);
  FSTD(BOOL) operator <(const CTV& tv1, const CTV& tv2);
  FSTD(BOOL) operator>=(const CTV& tv1, const CTV& tv2);
  FSTD(BOOL) operator<=(const CTV& tv1, const CTV& tv2);

//************************** Operations on intervals **************************

  int NumNeighb(const CTV& tv) const;  //номер компоненты соседства
  int NumNeighb(const CBV& bv) const;  //номер компоненты соседства
  int NumAdjac(const CTV& tv) const;   //номер компоненты смежности
  int NumAdjac(const CBV& bv) const;   //номер компоненты смежности
  CBV Orthogon(const CTV& tv) const;
  CBV Orthogon(const CBV& bv) const;
  CTV Intersec(const CTV& tv) const;  //CTV = Empty,если не пересекаютс
  CTV Concens(const CTV& tv) const;   //рез-т обобщ.склеивани
  CTV Concens(const CBV& tv) const;   //рез-т обобщ.склеивани
  CTV MinCover(const CTV& tv) const;
  CTV MinCover(const CBV& tv) const;
  CBM CTVtoCBM() const;               //перевод тр.вектора в мн-во булевых

//**************************** Operations on TERNARY VECTOR PARTS *************
//********************************** Logic operations *************************
  void OrInPlace(BOOL Part,const CBV& bv);
  void AndInPlace(BOOL Part,const CBV& bv);
  void XorInPlace(BOOL Part,const CBV& bv);
  void SubInPlace(BOOL Part,const CBV& bv);
  void InvertBitsInPlace(BOOL Part);
  CBV  InvertBits(BOOL Part) const;

//*******************************************************************

//#ifndef _LINUX
//  CString BitChar(char One = '1',char Zero='0',char Dont='-',
//    char Rest='+', int Max=0);
////****************************** Input/Output operations **********************
#ifdef _DEBUG
  friend CDumpContext& AFXAPI operator<<(CDumpContext& dc,const CTV& tv);
#endif //_DEBUG
//  friend CArchive& AFXAPI operator<<(CArchive& ar, const CTV& tv);
//  friend CArchive& AFXAPI operator>>(CArchive& ar, CTV& tv);
//#else
  char* BitChar(char One = '1',char Zero='0',char Dont='-', char Rest='+', int Max=0);
//#endif //_LINUX

  FSTD(CArch&) operator<<(CArch& ar, const CTV& tv);
  FSTD(CArch&) operator>>(CArch& ar, CTV& tv);

//*******************************************************************


//***************************** Advanced access to memory *********************
  BYTE* GetBuffer(int nMinBufLength);
  void ReleaseBuffer(int nNewLength = -1);
  BYTE* SetSize(int nNewLength,int nNewAllocLength=-1);
      //BYTE* GetBufferSetLength(int nNewLength,int nNewAllocLength=-1);

//*****************************************************************************
  BYTE* m_bVect1;
  BYTE* m_bVect0;

  protected:
  int m_nBitLength;
  int m_nByteLength;
  int m_nAllocLength;

//******************************** Protected functions ************************
  void AllocBuffer(int nLen);
  void AllocCopy(CTV& dest, int nCopyLen, int nCopyIndex, int nExtraLen);
  void AssignChar(int nLenBit, const char* pch);
  void AssignCopy(int nLenBit,int nLenByte, const BYTE* pbtSrcDat1,
                  const BYTE* pbtSrcDat0);

  void CharBit(int nLenByte,const char* pch);

  void Conc(const BYTE* SrcVect1, const BYTE* SrcVect0, int SrcBitLen);
  void Conc2(const BYTE* Vect11,const BYTE* Vect10,int SrcBitLen1,
             const BYTE* Vect21,const BYTE* Vect20,int SrcBitLen2);
  BOOL Equality(const BYTE* Vect1,const BYTE* Vect0,int BitLen) const;
  BOOL EqualBool(const BYTE* Vect,int BitLen) const;
  void Extr(const BYTE* SrcVect1,const BYTE* SrcVect0,
            int SrcBitLen,int nFirst,int nCount);
  void LeftShiftVect(const BYTE* SrcVect1,const BYTE* SrcVect0,
                     int nSrcLen,int nShift);
  BOOL Pogl(const BYTE* Vect1,const BYTE* Vect0,int BitLen) const;
  void RightShiftVect(const BYTE* SrcVect1,const BYTE* SrcVect0,
                      int nSrcLen,int nShift);
  void Init();
};


// 25.01.98 -----------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
// Ternary matrix

//#ifndef _LINUX
//class CTM : public CObject
//{
//  DECLARE_SERIAL(CTM)
//#else
class CTM
{
//#endif //_LINUX

public:
  friend class CTV;
  friend class CBV;
  friend class CBM;

//******************************* Constructors\Destructor *********************
  CTM();
  CTM(const CTM& tm, BOOL Fl = TRUE );
  CTM(const CBM& bm1, const CBM& bm2, BOOL Fl = TRUE);
  CTM(int nRow, int nColumn,char symb='-');
  CTM(const char* pch);
  ~CTM();

//*********************** Functions for getting class's parametrs *************
  int GetByteLength() const;
  int GetAllocLength() const;
  int GetCountC() const;
  int GetCountR() const;
  int GetUpperBound() const;

//*********************** Functions for generation ****************************
  CTM GenRtm (int nRowl, int nCol);
  CTM GenRtmMid (int nRowl, int nCol, int nRang);
  CTM GenRtmFix (int nRowl, int nCol, int nRang);

//********************************* Working with memory ***********************
  void SetSize(int nRow,int nColumn,int nGrowBy = 10,int nColGrow=0);
  void RemoveAll();
  void RemoveAllRows();
  void FreeExtra();

//********************************** Data Reading *****************************
  char GetBitAt(int nRow,int nColumn) const;
  CTV GetRowTv(int nRow) const;
  CTV GetRowTv(int nRow,BYTE* maska) const;
  CTV GetRowTv(int nRow,CBV& maska) const;
  CTV GetColumnTv(int nColumn) const;
  CTV GetColumnTv(int nColumn,BYTE* maska) const;
  CTV GetColumnTv(int nColumn,CBV& maska) const;

//*******************************************************************

//#ifndef _LINUX
//  CTM(const CStringArray& StrAr);
//  const CTM& operator =(const CStringArray& StrAr);
//  CString BitChar(char One='1',char Zero='0',char Dont='-',
//                  char Rest='+',BOOL WithNum=FALSE);
//  void BitChar(CStringArray & StrRes, BOOL WithClear=TRUE, BOOL WithNum=FALSE,
//               char One = '1',char Zero='0', char Dont='-', char Rest='+');
#ifdef _DEBUG
  void Dump(CDumpContext&) const;
  void AssertValid() const;
#endif
//  void Serialize(CArchive& ar);
//#else
  CTM(const vector <string>& StrAr);
  const CTM& operator =(const vector <string>& StrAr);
  char * BitChar(char One='1',char Zero='0',char Dont='-', char Rest='+',BOOL WithNum=FALSE);
//#endif //_LINUX

  void BitChar (vector <string>& StrRes, BOOL WithClear=TRUE, BOOL WithNum=FALSE,
    char One = '1',char Zero='0', char Dont='-', char Rest='+');
  void Serialize(CArch& ar);

  //*******************************************************************


  CBM GetDefs() const;
  CBM GetUnDefs() const;
  CBM GetOnes() const;
  CBM GetZeros() const;
  CBV GetRowUnDef(int nRow) const;
  CBV GetRowDef(int nRow) const;
  CBV GetRowOne(int nRow) const;
  CBV GetRowZero(int nRow) const;
  CBV GetRowUnDef(int nRow,BYTE* maska) const;
  CBV GetRowDef(int nRow,BYTE* maska) const;
  CBV GetRowOne(int nRow,BYTE* maska) const;
  CBV GetRowZero(int nRow,BYTE* maska) const;
  BYTE* GetByteZero(int nRow) const;
  BYTE* GetByteOne(int nRow) const;

  void Param(int* aPar,int nMaxLen);

//********************************** Select of matrix *************************
  CTM ExtractRows(int nFirst, int nCount) const;
  CTM ExtractColumns(int nFirst, int nCount) const;
  CTM Extract(int nFirstRow, int nFirstColumn, int nCountRow, int nCountColumn) const;
  CTM ExtractColumns(const CBV& Columns) const;
  CTM Minor(const CBV& Rows, const CBV& Columns) const;
  CTM Trans() const;

//********************************** Data Writing *****************************
  void SetBitAt(int nRow,int nColumn, char symb='-');
  void SetRowTv(int nRow, const CTV& newRow);
  void SetRowTv(int nRow, const CTM& tm, int ntmRow);
  void SetPartAt(BOOL Part,const CBM& bm);
  void SetPartAt(BOOL Part, const CBV& bv,int nRow);
  void SetRowZero(int nRow,const BYTE* newRow);
  void SetRowOne(int nRow,const BYTE* newRow);

//**************************** Working with rows (changing their number) ******
  int Add(const CTV& tv);
  int Add(const CTM& tm, int ntmRow=-1);  //22.02.2007 перестала inline
  int Add(char symb='-',int nCount=1);
  void SetRowGrow(int nRow, const CTV& newRow);
  void SetRowGrow(int nRow, const CTM& tm, int ntmRow);
  void InsertRow(int nRow, const CTV& newRow, int nCount = 1);  //несколько копий
  void InsertRow(int nRow,int nStartRow, const CTM& tm,int nCount = 1);
  void RemoveRow(int nRow, int nCount = 1);
  void ExchangeRow(int nRow1,int nRow2,int nCount = 1);
  int AddCol(int numCol=1);

  void SortDecr(int nBeg=0,int number=-1);
  void SortIncr(int nBeg=0,int number=-1);

//*********************** Operators and functions of assignment ***************
  const CTM& operator =(const CTM& tm);
  const CTM& operator =(const char* pch);
  void Clear(char symb='-',int nRow=-1);

//********************************** Logic operations *************************
  void InvertDefs(int nRow=-1); //инвертировать опр.значения строк или одной стр.

//**************************** Operations of concatinations *******************
  void Concat(const CTM& tm);
  void Concat(const CTM& tm1,const CTM& tm2);

//******************** Operations of weighting, finding and casing ************
  int CountUnDefs(int nRow = -1) const;
  int CountDefs(int nRow = -1) const;
  int CountOnes(int nRow = -1) const;
  int CountZeros(int nRow = -1) const;

  int CountUnDefs(const BYTE * mask,int nRow= -1) const;
  int CountDefs(const BYTE * mask,int nRow= -1) const;
  int CountOnes(const BYTE * mask,int nRow= -1) const;
  int CountZeros(const BYTE * mask,int nRow= -1) const;
  int LeftUnDef(int nRow, int nNext = -1) const;
  int LeftDef(int nRow, int nNext = -1) const;
  int LeftOne(int nRow, int nNext = -1) const;
  int LeftZero(int nRow, int nNext = -1) const;
  int LeftUnDef(int nRow, int nNext, BYTE* mask) const;
  int LeftDef(int nRow, int nNext, BYTE* mask) const;
  int LeftOne(int nRow, int nNext, BYTE* mask) const;
  int LeftZero(int nRow, int nNext, BYTE* mask) const;

  int RightUnDef(int nRow, int nPrev = -1) const;
  int RightDef(int nRow, int nPrev = -1) const;
  int RightOne(int nRow, int nPrev = -1) const;
  int RightZero(int nRow, int nPrev = -1) const;
  int RightUnDef(int nRow, int nPrev,BYTE* mask) const;
  int RightDef(int nRow, int nPrev,BYTE* mask) const;
  int RightOne(int nRow, int nPrev,BYTE* mask) const;
  int RightZero(int nRow, int nPrev,BYTE* mask) const;

  int FindUnDefR (int nRow=-1) const; // все '-'
  int FindDefR (int nRow=-1) const;   // нет '-'
  int FindOneR (int nRow=-1) const;   // все '1'
  int FindZeroR (int nRow=-1) const;  // все '0'
  int FindUnDefR (BYTE* mask, int nFRow=-1) const;
  int FindDefR (BYTE* mask,int nFRow=-1) const;   // нет '-'
  int FindOneR (BYTE* mask, int nFRow=-1) const;
  int FindZeroR (BYTE* mask, int nFRow=-1) const;
  int FindR (const CTV& tv, int nFRow=-1) const;
  int FindR (const CBV& bv, int nFRow=-1) const;

//******************************* Matrix compareing operations ****************
  BOOL IsCorrect(int nRow = -1) const;  //new кодировка 11 не корректна
  BOOL IsBool(int nRow = -1) const;     //new матрица булева ?
  BOOL IsBool(const BYTE* mask,int nRow=-1)  const;
  BOOL IsTrivial(int nRow = -1) const;  //new все '-'
  BOOL IsTrivial(const BYTE* mask,int nRow=-1) const;
  BOOL IsOne(int nRow = -1) const;
  BOOL IsOne(const BYTE* mask,int nRow)  const;
  BOOL IsZero(int nRow = -1) const;
  BOOL IsZero(const BYTE* mask,int nRow=-1) const;
  BOOL ExistOne(int nRow/*=-1*/) const;
  BOOL ExistOne(const BYTE* mask,int nRow/*=-1*/) const;
  BOOL ExistZero(int nRow/*=-1*/) const;
  BOOL ExistZero(const BYTE* mask,int nRow/*=-1*/) const;
  BOOL IsEmpty() const;
  FSTD(BOOL) operator==(const CTM& tm1,const CTM& tm2);
  FSTD(BOOL) operator!=(const CTM& tm1,const CTM& tm2);

//***************************** Row compareing operations *********************

  BOOL AreEqual(int nRow1,int nRow2) const;
  BOOL AreEqual(const BYTE* mask,int nRow1,int nRow2) const;
  BOOL AreEqual(int nRow1,const CTV& tv) const;
  BOOL AreEqual(const BYTE* mask,int nRow1,const CTV& tv) const;

  BOOL AreNotEqual(int nRow1,int nRow2) const;
  BOOL AreNotEqual(const BYTE* mask,int nRow1,int nRow2) const;
  BOOL AreNotEqual(int nRow1,const CTV& tv) const;
  BOOL AreNotEqual(const BYTE* mask,int nRow1,const CTV& tv) const;

  BOOL AreOrthog(int nRow1,int nRow2) const;
  BOOL AreOrthog(const BYTE* mask,int nRow1,int nRow2) const;
  BOOL AreOrthog(int nRow1,const CTV& tv) const;
  BOOL AreOrthog(const BYTE* mask,int nRow1,const CTV& tv) const;
  BOOL AreOrthog(int nRow1,const CBV& bv) const;
  BOOL AreOrthog(const BYTE* mask,int nRow1,const CBV& bv) const;

  BOOL AreNeighb(int nRow1,int nRow2) const;
  BOOL AreNeighb(const BYTE* mask,int nRow1,int nRow2) const;
  BOOL AreNeighb(int nRow1,const CTV& tv) const;
  BOOL AreNeighb(const BYTE* mask,int nRow1,const CTV& tv) const;

  BOOL AreConcens(int nRow1,int nRow2) const;
  BOOL AreConcens(const BYTE* mask,int nRow1,int nRow2) const;
  BOOL AreConcens(int nRow1,const CTV& tv) const;
  BOOL AreConcens(const BYTE* mask,int nRow1,const CTV& tv) const;

  BOOL IsCover(int nRow1,int nRow2) const;
  BOOL IsCover(const BYTE* mask,int nRow1,int nRow2) const;
  BOOL IsCover(int nRow1,const CTV& tv) const;
  BOOL IsCover(const BYTE* mask,int nRow1,const CTV& tv) const;
  BOOL IsCover(int nRow1,const CBV& bv) const;
  BOOL IsCover(const BYTE* mask,int nRow1,const CBV& bv) const;

  BOOL IsCovered(int nRow1,const CTV& tv) const;
  BOOL IsCovered(const BYTE* mask,int nRow1,const CTV& tv) const;

//******************************* Row interval operations  ********************
  CBV Orthogon(int nRow1,int nRow2) const;
  CBV Orthogon(const BYTE* mask,int nRow1,int nRow2) const;
  CBV Orthogon(int nRow1,const CTV& tv) const;
  CBV Orthogon(const BYTE* mask,int nRow1,const CTV& tv) const;
  CBV Orthogon(int nRow1,const CBV& bv) const;
  CBV Orthogon(const BYTE* mask,int nRow1,const CBV& bv) const;

  CTV Intersec(int nRow1,int nRow2) const;
  CTV Intersec(const BYTE* mask,int nRow1,int nRow2) const;
  CTV Intersec(int nRow1,const CTV& tv) const;
  CTV Intersec(const BYTE* mask,int nRow1,const CTV& tv) const;

  CTV Concens(int nRow1,int nRow2) const;
  CTV Concens(const BYTE* mask,int nRow1,int nRow2) const;
  CTV Concens(int nRow1,const CTV& tv) const;
  CTV Concens(const BYTE* mask,int nRow1,const CTV& tv) const;
  CTV Concens(int nRow1,const CBV& bv) const;
  CTV Concens(const BYTE* mask,int nRow1,const CBV& bv) const;

  CTV MinCover(int nRow1,int nRow2) const;
  CTV MinCover(const BYTE* mask,int nRow1,int nRow2) const;
  CTV MinCover(int nRow1,const CTV& tv) const;
  CTV MinCover(const BYTE* mask,int nRow1,const CTV& tv) const;
  CTV MinCover(int nRow1,const CBV& bv) const;
  CTV MinCover(const BYTE* mask,int nRow1,const CBV& bv) const;
  CBM CTMtoCBM(void) const; // раскрыть интервалы

//**************************** Operations on TERNARY VECTOR PARTS *************
//****************************** Row Logic operations *************************
  void AndInPlace(BOOL Part,const CBV& bv,int Row);
  void OrInPlace(BOOL Part,const CBV& bv,int Row);
  void XorInPlace(BOOL Part,const CBV& bv,int Row);
  void SubInPlace(BOOL Part,const CBV& bv,int Row);
  void InvertBitsInPlace(BOOL Part,int Row);
  void InvertBitsInPlace(BOOL Part);

//********** Implementation in files Tm_Degen,Tm_Invers,Tm_Equiv **************
  CTV Degen ();
  CTV Degen(CBV &bvX, CBV &bvY);
  int Degen(CTM &TmT, CTV &tvW, CBV &bvY);
  int UnAltCol(CBV &bvX,CBV &bvY,CTV &tv);
  int UnAltRow(CTM &mtT,CBV &bvX,CBV &bvY,CTV &tv);

  CTM InverS(int simpl);
  CTM InverS(int simpl, CBV bvR);
  CTM InverS(int simpl, CTV tv, CBV bvR);
  CTV Differ(CTV tv, CBV &bvR);
  void AdExpans(CTM &Tm0, CTV &tv, CBV &bvC, CBV &bvW);


  int NoEquiv(CTM &Tm);
  int NoImpl(CTM &Tm);
  int NoImpl(CTV &tv);
  int NoImpl(CTV &tv, CBV &bvR);
  int CrossI(CTM &Tm);

  CTM DistAL(int e, CBV &bv, CBV &bvR);
  void DisConDis(CTV &tv, CTM &TmEE);

  void DorD(int Regime, CTV tva);
  void DorD(int Regime, CTM Tm1);


protected:
  BYTE** m_pData;
  int m_nSize;
  int m_nMaxSize;
  int m_nGrowBy;
  int m_nBitLength;
  int m_nByteLength;
  int m_nAllocLength;

//******************************** Protected functions ************************
  void Init();
  void SafeDelete();
  void AllocMatrix(int nRow, int nColumn);
//#ifndef _LINUX
//  void CharBit(const CString s);
//  void CharBit(const CStringArray& s);
//#endif //_LINUX
  void CharBit(char* s);

};


///////////////////////////////////////////////////////////////////////////////
// Inline function declarations

//*********************** Functions for getting class's parametrs *************
inline int CTV::GetByteLength() const                      //GetByteLength
{ return m_nByteLength; }

inline int CTV::GetBitLength() const                       //GetBitLength
{ return m_nBitLength; }

inline int CTV::GetAllocLength() const                     //GetAllocLength
{ return m_nAllocLength; }

//*************************************** Data Reading ************************
//------------------------------ GetOneVect -----------------------------------
inline CBV CTV::GetOnes() const
{ CBV bv(m_bVect1, m_nBitLength); return bv; }

//------------------------- CBV GetZeroVect -----------------------------------
inline CBV CTV::GetZeros() const
{ CBV bv(m_bVect0, m_nBitLength); return bv; }

//------------------------- BYTE* GetOnesB ------------------------------------
inline BYTE* CTV::GetOnesB() const
{ return ( BYTE*)m_bVect1; }

//------------------------- BYTE* GetZerosB -----------------------------------
inline BYTE* CTV::GetZerosB() const
{ return ( BYTE*)m_bVect0; }

//*********************** Operators and functions of assignment ***************
inline const CTV& CTV::operator =(const CTV& tvSrc)        //operator =
{ AssignCopy(tvSrc.m_nBitLength,tvSrc.m_nByteLength, tvSrc.m_bVect1, tvSrc.m_bVect0);
  return *this;
}

inline const CTV& CTV::operator =(const CBV& bvSrc)        //operator =
{ CBV invSrc=~bvSrc;
AssignCopy(bvSrc.GetBitLength(),bvSrc.GetByteLength(),(BYTE*)bvSrc,(BYTE*)invSrc);
  return *this;
}

inline const CTV& CTV::operator =(const char* pch)         // operator =
{ AssignChar(strlen((char*)pch), pch); return *this; }                             

inline void CTV::Empty()                                   // Empty()
{ if (m_bVect1 != NULL) delete [] m_bVect1; Init(); }

//******************************* Compareing operations ***********************
inline BOOL CTV::IsEmpty() const                           //IsEmpty
{ return m_nByteLength == 0; }

inline void CTV::Init()                                    // Init() (protect)
{ m_nBitLength = m_nByteLength = m_nAllocLength = 0;
  m_bVect1 = m_bVect0 = NULL;
}

//**************************** Operations of concatinations *******************
inline void CTV::Concat(const CTV& tv)                     // Concat
{  Conc(tv.m_bVect1,tv.m_bVect0,tv.m_nBitLength); return; }

inline void CTV::Concat(const CTV& tv1,const CTV& tv2)      // Concat
{  
  Conc2(tv1.m_bVect1,tv1.m_bVect0,tv1.m_nBitLength,tv2.m_bVect1,tv2.m_bVect0,
        tv2.m_nBitLength); return; }




// Inlines for CTM classes - Ternary matrix
///////////////////////////////////////////////////////////////////////////////


//***************** Functions for getting class's parametrs **********
inline int CTM::GetByteLength() const
{ return m_nByteLength; }

inline int CTM::GetAllocLength() const
{ return m_nAllocLength; }

inline int CTM::GetCountC() const
{ return m_nBitLength; }

inline int CTM::GetCountR() const
{ return m_nSize; }

inline int CTM::GetUpperBound() const
{ return m_nMaxSize; }

//********************************* Working with memory ***********************
inline void CTM::RemoveAll()
{ SetSize(0,0); }

inline void CTM::RemoveAllRows()
{ SetSize(0,m_nBitLength); }

//********************************** Data Reading *****************************

inline BYTE* CTM::GetByteZero(int nRow) const
{ return ( BYTE*)&m_pData[nRow][m_nAllocLength]; }

inline BYTE* CTM::GetByteOne(int nRow) const
{ return ( BYTE*)&m_pData[nRow][0]; }   // 04.05.2006

//**************************** Working with rows (changing their number) ******
inline int CTM::Add(const CTV& tv)
{ int nIndex = m_nSize; SetRowGrow(nIndex, tv); return nIndex; }

//******************************* Matrix compareing operations ****************
inline BOOL CTM::IsEmpty() const
{ return ((m_pData==NULL) & (m_nSize==0)); }


inline int CTM::CountUnDefs(int nRow/*=-1*/) const
{ int i=CountDefs(nRow);
  return ((nRow=-1)?((m_nSize*m_nBitLength)-i):(m_nBitLength-i));
}



//----------------- Init() ----------------------------------------------------
inline void CTM::Init()
{ m_pData = NULL;
  m_nSize = m_nMaxSize =  m_nBitLength = m_nByteLength = m_nAllocLength = 0;
  m_nGrowBy = 10;
}


#undef AFXAPP_DATA
#define AFXAPP_DATA     NEAR
#endif


