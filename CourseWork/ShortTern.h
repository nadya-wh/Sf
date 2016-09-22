/////////////////////////////////////////////////////////////////
// Ver.2.1.5    28.03.2008
//              SetOnes и SetZeros - исправлены для реализации в другой интерпретации
//              в виде функций AddOnes и AddZeros
//              Поправка функции LeftUnDef
// Ver.2.1.4     22.01.2008
//               Добавлены функции конвертации данных базового класса троичных векторов и матриц
//               а также функции DorD для класса троичных матриц
// Ver.2.1.3     22.02.2007
//               В функции Add можно не указать второго операнда
//               и тогда к текущей добавятся все строки матрицы
//               Новая функция RemoveAllRows();
// Ver.2.1.2     10.07.2006
//               поправка AreConsens
// Ver.2.1.1     08.07.2006
//               поправка GetRowTv
// Ver.2.1.0     07.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.1.1     11.02.2005
//              Change type 'unsigned int' on ULONG
// Ver.1.0     08.02.2005
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// Ternary vector
#ifndef _SHORTTERN
#define _SHORTTERN
#ifdef _JOINT
#include "ShortBool.h"
#else
#include "../ComBool/ShortBool.h"
#endif

class CBV;
class CBM;
class CTV;
class CTM;
/////////////////////////////////////////////////////////////////////////////
// Ternary vector

class CsTV
{
public:
  friend class CTV;
  friend class CsBV;
  friend class CsBM;

//******************************* Constructors\Destructor *********************
  CsTV();
  CsTV(const CsTV& tvSrc);
  CsTV(const CsBV& bv1Src,const CsBV& bv0Src);
  CsTV(int nLenBit,char value='n'); 
      //1,0,-,+ - тр.вектор в кодировке 10,01,00,11,'n'-без инициализ-ии
  CsTV(const ULONG pbt1,const ULONG pbt0,int nLenBit);
  CsTV(const char* pch);
  ~CsTV(){};

  void ToShort(CTV tv);
  void FromShort(CTV &Tv);

//*********************** Functions for getting class's parametrs *************
  int GetBitLength() const;

//*************************************** Data Reading ************************
  char GetBitAt(int nIndex) const;
  CsTV Extract(int nFirst,int nCount);
#ifndef _LINUX
  CString BitChar(char One = '1',char Zero='0',char Dont='-',char Rest='+');
#else
  char* BitChar(char One = '1',char Zero='0',char Dont='-',char Rest='+');
#endif
  CsBV GetOnes() const;
  CsBV GetZeros() const;
  CsBV GetDefs() const;
  CsBV GetUnDefs() const;
  ULONG GetOnesB() const;
  ULONG GetZerosB() const;

//*************************************** Data Writing ************************
  void SetBitAt(int nIndex, char symb);
  void SetVect(CsBV bv1Src, CsBV bv0Src);
  void SetOnes(const CsBV& bv);
  void SetZeros(const CsBV& bv);
  void AddOnes(const CsBV& bv);
  void AddZeros(const CsBV& bv);
  void SetUnDefs(const CsBV& bv);
  void SetOnes(const ULONG bv);
  void SetZeros(const ULONG bv);
  void AddOnes(const ULONG bv);
  void AddZeros(const ULONG bv);

//*********************** Operators and functions of assignment ***************
  const CsTV& operator=(const CsTV& tvSrc);
  const CsTV& operator=(const CsBV& bvSrc);
  const CsTV& operator=(const char* pch);
  void Clear(char symb='-');
  void Empty();

//*********************** Operators and functions of expanded assignment ******
  const CsTV& operator <<=(int nShift);
  const CsTV& operator >>=(int nShift);
  const CsTV& operator &=(const ULONG maska);
  const CsTV& operator &=(const CsBV& maska);
  const CsTV& operator &=(const CsTV& tv);

//********************************** Logic operations *************************
  FSTD(CsTV) operator<<(const CsTV& tv, int nShift);
  FSTD(CsTV) operator>>(const CsTV& tv, int nShift);
  FSTD(CsTV) operator&=(const CsTV& tv, const ULONG maska);
  FSTD(CsTV) operator&=(const CsTV& tv, const CsBV& maska);
  FSTD(CsTV) operator&=(const CsTV& tv1, const CsTV& tv2);
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

//******************************* Compareing operations ***********************
  BOOL IsEmpty() const;
  BOOL IsCorrect() const;  // кодировка 11 не корректна
  BOOL IsBool() const;     // вектор булев ?
  BOOL IsTrivial() const;  // все '-'
  BOOL IsOne() const;
  BOOL IsZero() const;
  BOOL ExistZero() const;
  BOOL ExistOne() const;
  BOOL IsOrthog(const CsTV& tv) const;
  BOOL IsOrthog(const CsBV& bv) const;
  BOOL IsNeighb(const CsTV& tv) const; //соседство
  BOOL IsNeighb(const CsBV& bv) const; //соседство
  BOOL IsAdjac(const CsTV& tv) const;  //смежность
  BOOL IsAdjac(const CsBV& bv) const;  //смежность
  BOOL IsCover(const CsTV& tv) const;  // 1 поглощает 2
  BOOL IsCover(const CsBV& bv) const;  // 1 поглощает 2
  BOOL IsIntersec(const CsTV& tv) const;//пересекаютс
  FSTD(BOOL) operator==(const CsTV& tv1, const CsTV& tv2);
  FSTD(BOOL) operator==(const CsTV& tv1, const CsBV& bv2);
  FSTD(BOOL) operator==(const CsBV& bv1, const CsTV& tv2);
  FSTD(BOOL) operator!=(const CsTV& tv1, const CsTV& tv2);
  FSTD(BOOL) operator!=(const CsTV& tv1, const CsBV& bv2);
  FSTD(BOOL) operator!=(const CsBV& bv1, const CsTV& tv2);
  FSTD(BOOL) operator >(const CsTV& tv1, const CsTV& tv2);
  FSTD(BOOL) operator <(const CsTV& tv1, const CsTV& tv2);
  FSTD(BOOL) operator>=(const CsTV& tv1, const CsTV& tv2);
  FSTD(BOOL) operator<=(const CsTV& tv1, const CsTV& tv2);

//************************** Operations on intervals **************************

  int NumNeighb(const CsTV& tv) const;  //номер компоненты соседства
  int NumNeighb(const CsBV& bv) const;  //номер компоненты соседства
  int NumAdjac(const CsTV& tv) const;   //номер компоненты смежности
  int NumAdjac(const CsBV& bv) const;   //номер компоненты смежности
  CsBV Orthogon(const CsTV& tv) const;
  CsBV Orthogon(const CsBV& bv) const;
  CsTV Intersec(const CsTV& tv) const;  //CsTV = Empty,если не пересекаютс
  CsTV Concens(const CsTV& tv) const;   //рез-т обобщ.склеивани
  CsTV Concens(const CsBV& tv) const;   //рез-т обобщ.склеивани
  CsTV MinCover(const CsTV& tv) const;
  CsTV MinCover(const CsBV& tv) const;
  CsBM CsTVtoCsBM() const;        //перевод тр.вектора в мн-во булевых

//**************************** Operations on TERNARY VECTOR PARTS *************
//********************************** Logic operations *************************
  void OrInPlace(BOOL Part,const CsBV& bv);
  void AndInPlace(BOOL Part,const CsBV& bv);
  void XorInPlace(BOOL Part,const CsBV& bv);
  void SubInPlace(BOOL Part,const CsBV& bv);
  void InvertBitsInPlace(BOOL Part);
  CsBV  InvertBits(BOOL Part) const;

#ifndef _LINUX
//****************************** Input/Output operations **********************
  friend CArchive& AFXAPI operator<<(CArchive& ar, const CsTV& tv);
  friend CArchive& AFXAPI operator>>(CArchive& ar, CsTV& tv);
#endif
  void SetSize(int nNewLength);

//*****************************************************************************
  ULONG m_bVect1;
  ULONG m_bVect0;

  protected:
  int m_nBitLength;

//******************************** Protected functions ************************
  void AssignChar(int nLenBit, const char* pch);
  void CharBit(const char* pch);
  BOOL Equality(const ULONG Vect1,const ULONG Vect0,int BitLen) const;
  BOOL EqualBool(const ULONG Vect,int BitLen) const;
  void Extr(const ULONG SrcVect1,const ULONG SrcVect0,
            int SrcBitLen,int nFirst,int nCount);
  void LeftShiftVect(const ULONG SrcVect1,const ULONG SrcVect0,
                     int nSrcLen,int nShift);
  void RightShiftVect(const ULONG SrcVect1,const ULONG SrcVect0,
                      int nSrcLen,int nShift);
  BOOL Pogl(const ULONG Vect1,const ULONG Vect0,int BitLen) const;
  void Init();
};


/////////////////////////////////////////////////////////////////////////////
// Ternary matrix
#ifndef _LINUX
class CsTM : public CObject
{
  DECLARE_SERIAL(CsTM)
#else
class CsTM
{
#endif

public:
  friend class CsTV;
  friend class CsBV;
  friend class CsBM;
  friend class CTM;
//******************************* Constructors\Destructor *********************
  CsTM();
  CsTM(const CsTM& tm, BOOL Fl = TRUE );
  CsTM(const CsBM& bm1, const CsBM& bm2, BOOL Fl = TRUE);
  CsTM(int nRow, int nColumn,char symb='-');
  CsTM(const char* pch);
#ifndef _LINUX
  CsTM(const CStringArray& StrAr);
#else
  CsTM(const vector <string>& StrAr);
#endif
  ~CsTM();

  void ToShort(CTM &Tm);
  void FromShort(CTM &Tm);

//*********************** Functions for getting class's parametrs *************
  int GetCountC() const;
  int GetCountR() const;
  int GetUpperBound() const;

//********************************* Working with memory ***********************
  void SetSize(int nRow,int nColumn,int nGrowBy = 10);
  void RemoveAll();
  void RemoveAllRows();
  void FreeExtra();

//********************************** Data Reading *****************************
  char GetBitAt(int nRow,int nColumn) const;
  CsTV GetRowTv(int nRow) const;
  CsTV GetRowTv(int nRow, ULONG maska) const;
  CsTV GetRowTv(int nRow, CsBV& maska) const;
  CsTV GetColumnTv(int nColumn) const;
  CsTV GetColumnTv(int nColumn, ULONG maska) const;
  CsTV GetColumnTv(int nColumn, CsBV& maska) const;
#ifndef _LINUX
  CString BitChar(char One='1',char Zero='0',char Dont='-',
                  char Rest='+',BOOL WithNum=FALSE);
  void BitChar(CStringArray & StrRes, BOOL WithClear=TRUE, BOOL WithNum=FALSE,
               char One = '1',char Zero='0', char Dont='-', char Rest='+');
#else
  char* BitChar(char One='1',char Zero='0',char Dont='-',
                char Rest='+',BOOL WithNum=FALSE);
#endif //_LINUX
  void BitChar(vector <string> & StrRes, BOOL WithClear=TRUE, BOOL WithNum=FALSE,
               char One = '1',char Zero='0', char Dont='-', char Rest='+');

  CsBM GetOnes() const;
  CsBM GetZeros() const;
  CsBV GetRowUnDef(int nRow) const;
  CsBV GetRowDef(int nRow) const;
  CsBV GetRowOne(int nRow) const;
  CsBV GetRowZero(int nRow) const;
  CsBV GetRowUnDef(int nRow, ULONG maska) const;
  CsBV GetRowDef(int nRow, ULONG maska) const;
  CsBV GetRowOne(int nRow, ULONG maska) const;
  CsBV GetRowZero(int nRow, ULONG maska) const;
  ULONG GetLongZero(int nRow) const;
  ULONG GetLongOne(int nRow) const;

  void Param(int* aPar,int nMaxLen);

//********************************** Select of matrix *************************
  CsTM ExtractRows(int nFirst, int nCount) const;
  CsTM ExtractColumns(int nFirst, int nCount) const;
  CsTM Extract(int nFirstRow, int nFirstColumn, int nCountRow, int nCountColumn) const;
  CsTM ExtractColumns(const CsBV& Columns) const;

//********************************** Data Writing *****************************
  void SetBitAt(int nRow,int nColumn, char symb='-');
  void SetRowTv(int nRow, const CsTV& newRow);
  void SetRowTv(int nRow, const CsTM& tm, int ntmRow);
  void SetPartAt(BOOL Part,const CsBM& bm);
  void SetPartAt(BOOL Part, const CsBV& bv,int nRow);
  void SetRowZero(int nRow, const ULONG newRow);
  void SetRowOne(int nRow, const ULONG newRow);

//**************************** Working with rows (changing their number) ******
  int Add(const CsTV& tv);
  int Add(const CsTM& tm, int ntmRow);
  int Add(ULONG t1, ULONG t0);
  int Add(char symb='-',int nCount=1);
  void SetRowGrow(int nRow, const CsTV& newRow);
  void SetRowGrow(int nRow, const CsTM& tm, int ntmRow);
  void InsertRow(int nRow, const CsTV& newRow, int nCount = 1);  //несколько копий
  void InsertRow(int nRow,int nStartRow, const CsTM& tm,int nCount = 1);
  void RemoveRow(int nRow, int nCount = 1);
  void ExchangeRow(int nRow1,int nRow2,int nCount = 1);
  int AddCol(int numCol=1);

  void SortDecr(int nBeg=0,int number=-1);
  void SortIncr(int nBeg=0,int number=-1);

//*********************** Operators and functions of assignment ***************
  const CsTM& operator =(const CsTM& tm);
  const CsTM& operator =(const char* pch);
#ifndef _LINUX
  const CsTM& operator =(const CStringArray& StrAr);
#else
  const CsTM& operator =(const vector <string>& StrAr);
#endif
  void Clear(char symb='-',int nRow=-1);

//********************************** Logic operations *************************
  void InvertDefs(int nRow=-1); //инвертировать опр.значения строк или одной стр.

//******************** Operations of weighting, finding and casing ************
  int CountUnDefs(int nRow = -1) const;
  int CountDefs(int nRow = -1) const;
  int CountOnes(int nRow = -1) const;
  int CountZeros(int nRow = -1) const;

  int CountUnDefs(const ULONG mask,int nRow= -1) const;
  int CountDefs(const ULONG mask,int nRow= -1) const;
  int CountOnes(const ULONG mask,int nRow= -1) const;
  int CountZeros(const ULONG mask,int nRow= -1) const;
  int LeftUnDef(int nRow, int nNext = -1) const;
  int LeftDef(int nRow, int nNext = -1) const;
  int LeftOne(int nRow, int nNext = -1) const;
  int LeftZero(int nRow, int nNext = -1) const;
  int LeftUnDef(int nRow, int nNext, ULONG mask) const;
  int LeftDef(int nRow, int nNext, ULONG mask) const;
  int LeftOne(int nRow, int nNext, ULONG mask) const;
  int LeftZero(int nRow, int nNext, ULONG mask) const;

  int RightUnDef(int nRow, int nPrev = -1) const;
  int RightDef(int nRow, int nPrev = -1) const;
  int RightOne(int nRow, int nPrev = -1) const;
  int RightZero(int nRow, int nPrev = -1) const;
  int RightUnDef(int nRow, int nPrev, ULONG mask) const;
  int RightDef(int nRow, int nPrev, ULONG mask) const;
  int RightOne(int nRow, int nPrev, ULONG mask) const;
  int RightZero(int nRow, int nPrev, ULONG mask) const;

  int FindUnDefR (int nRow=-1) const; // все '-'
  int FindDefR (int nRow=-1) const;   // нет '-'
  int FindOneR (int nRow=-1) const;   // все '1'
  int FindZeroR (int nRow=-1) const;  // все '0'
  int FindUnDefR (ULONG mask, int nFRow=-1) const;
  int FindDefR (ULONG mask,int nFRow=-1) const;   // нет '-'
  int FindOneR (ULONG mask, int nFRow=-1) const;
  int FindZeroR (ULONG mask, int nFRow=-1) const;
  int FindR (const CsTV& tv, int nFRow=-1) const;
  int FindR (const CsBV& bv, int nFRow=-1) const;

//******************************* Matrix compareing operations ****************
  BOOL IsCorrect(int nRow = -1) const;  //new кодировка 11 не корректна
  BOOL IsBool(int nRow = -1) const;     //new матрица булева ?
  BOOL IsBool(const ULONG mask,int nRow=-1)  const;
  BOOL IsTrivial(int nRow = -1) const;  //new все '-'
  BOOL IsTrivial(const ULONG mask,int nRow=-1) const;
  BOOL IsOne(int nRow = -1) const;
  BOOL IsOne(const ULONG mask,int nRow)  const;
  BOOL IsZero(int nRow = -1) const;
  BOOL IsZero(const ULONG mask,int nRow=-1) const;
  BOOL ExistOne(int nRow/*=-1*/) const;
  BOOL ExistOne(const ULONG mask,int nRow/*=-1*/) const;
  BOOL ExistZero(int nRow/*=-1*/) const;
  BOOL ExistZero(const ULONG mask,int nRow/*=-1*/) const;
  BOOL IsEmpty() const;
  FSTD(BOOL) operator==(const CsTM& tm1,const CsTM& tm2);
  FSTD(BOOL) operator!=(const CsTM& tm1,const CsTM& tm2);

//***************************** Row compareing operations *********************

  BOOL AreEqual(int nRow1,int nRow2) const;
  BOOL AreEqual(const ULONG mask,int nRow1,int nRow2) const;
  BOOL AreEqual(int nRow1,const CsTV& tv) const;
  BOOL AreEqual(const ULONG mask,int nRow1,const CsTV& tv) const;

  BOOL AreNotEqual(int nRow1,int nRow2) const;
  BOOL AreNotEqual(const ULONG mask,int nRow1,int nRow2) const;
  BOOL AreNotEqual(int nRow1,const CsTV& tv) const;
  BOOL AreNotEqual(const ULONG mask,int nRow1,const CsTV& tv) const;

  BOOL AreOrthog(int nRow1,int nRow2) const;
  BOOL AreOrthog(const ULONG mask,int nRow1,int nRow2) const;
  BOOL AreOrthog(int nRow1,const CsTV& tv) const;
  BOOL AreOrthog(const ULONG mask,int nRow1,const CsTV& tv) const;
  BOOL AreOrthog(int nRow1,const CsBV& bv) const;
  BOOL AreOrthog(const ULONG mask,int nRow1,const CsBV& bv) const;

  BOOL AreNeighb(int nRow1,int nRow2) const;
  BOOL AreNeighb(const ULONG mask,int nRow1,int nRow2) const;
  BOOL AreNeighb(int nRow1,const CsTV& tv) const;
  BOOL AreNeighb(const ULONG mask,int nRow1,const CsTV& tv) const;

  BOOL AreConcens(int nRow1,int nRow2) const;
  BOOL AreConcens(const ULONG mask,int nRow1,int nRow2) const;
  BOOL AreConcens(int nRow1,const CsTV& tv) const;
  BOOL AreConcens(const ULONG mask,int nRow1,const CsTV& tv) const;

  BOOL IsCover(int nRow1,int nRow2) const;
  BOOL IsCover(const ULONG mask,int nRow1,int nRow2) const;
  BOOL IsCover(int nRow1,const CsTV& tv) const;
  BOOL IsCover(const ULONG mask,int nRow1,const CsTV& tv) const;
  BOOL IsCover(int nRow1,const CsBV& bv) const;
  BOOL IsCover(const ULONG mask,int nRow1,const CsBV& bv) const;

  BOOL IsCovered(int nRow1,const CsTV& tv) const;
  BOOL IsCovered(const ULONG mask,int nRow1,const CsTV& tv) const;

//******************************* Row interval operations  ********************
  CsBV Orthogon(int nRow1,int nRow2) const;
  CsBV Orthogon(const ULONG mask,int nRow1,int nRow2) const;
  CsBV Orthogon(int nRow1,const CsTV& tv) const;
  CsBV Orthogon(const ULONG mask,int nRow1,const CsTV& tv) const;
  CsBV Orthogon(int nRow1,const CsBV& bv) const;
  CsBV Orthogon(const ULONG mask,int nRow1,const CsBV& bv) const;

  CsTV Intersec(int nRow1,int nRow2) const;
  CsTV Intersec(const ULONG mask,int nRow1,int nRow2) const;
  CsTV Intersec(int nRow1,const CsTV& tv) const;
  CsTV Intersec(const ULONG mask,int nRow1,const CsTV& tv) const;

  CsTV Concens(int nRow1,int nRow2) const;
  CsTV Concens(const ULONG mask,int nRow1,int nRow2) const;
  CsTV Concens(int nRow1,const CsTV& tv) const;
  CsTV Concens(const ULONG mask,int nRow1,const CsTV& tv) const;
  CsTV Concens(int nRow1,const CsBV& bv) const;
  CsTV Concens(const ULONG mask,int nRow1,const CsBV& bv) const;

  CsTV MinCover(int nRow1,int nRow2) const;
  CsTV MinCover(const ULONG mask,int nRow1,int nRow2) const;
  CsTV MinCover(int nRow1,const CsTV& tv) const;
  CsTV MinCover(const ULONG mask,int nRow1,const CsTV& tv) const;
  CsTV MinCover(int nRow1,const CsBV& bv) const;
  CsTV MinCover(const ULONG mask,int nRow1,const CsBV& bv) const;
  CsBM CsTMtoCsBM(void) const; // раскрыть интервалы

//**************************** Operations on TERNARY VECTOR PARTS *************
//****************************** Row Logic operations *************************
  void AndInPlace(BOOL Part,const CsBV& bv,int Row);
  void OrInPlace(BOOL Part,const CsBV& bv,int Row);
  void XorInPlace(BOOL Part,const CsBV& bv,int Row);
  void SubInPlace(BOOL Part,const CsBV& bv,int Row);
  void InvertBitsInPlace(BOOL Part,int Row);
  void InvertBitsInPlace(BOOL Part);

  void DorD(int Regime, CsTM Tm1);
  void DorD(int Regime, CsTV tva);

#ifndef _LINUX
//****************************** Input/Output operations **********************
  void Serialize(CArchive& ar);
#ifdef _DEBUG
  void Dump(CDumpContext&) const;
  void AssertValid() const;
#endif
#endif
  void Serialize(CArch& ar);

protected:
  ULONG* m_pData;
  int m_nSize;
  int m_nMaxSize;
  int m_nGrowBy;
  int m_nBitLength;

//******************************** Protected functions ************************
  void Init();
  void SafeDelete();
  void AllocMatrix(int nRow, int nColumn);
#ifndef _LINUX
  void CharBit(const CString s);
  void CharBit(const CStringArray& s);
#else
  void CharBit(const vector <string>& s);
#endif
  void CharBit(char* s);

};


///////////////////////////////////////////////////////////////////////////////
// Inline function declarations

//*********************** Functions for getting class's parametrs *************

inline int CsTV::GetBitLength() const                       //GetBitLength
{ return m_nBitLength; }

//*************************************** Data Reading ************************
//------------------------------ GetOneVect -----------------------------------
inline CsBV CsTV::GetOnes() const
{ CsBV bv(m_bVect1, m_nBitLength); return bv; }

//------------------------- CsBV GetZeroVect -----------------------------------
inline CsBV CsTV::GetZeros() const
{ CsBV bv(m_bVect0, m_nBitLength); return bv; }

//------------------------- ULONG GetOnesB ------------------------------------
inline ULONG CsTV::GetOnesB() const
{ return ( ULONG)m_bVect1; }

//------------------------- ULONG GetZerosB -----------------------------------
inline ULONG CsTV::GetZerosB() const
{ return ( ULONG)m_bVect0; }

//*********************** Operators and functions of assignment ***************
inline const CsTV& CsTV::operator =(const CsTV& tvSrc)        //operator =
{ m_bVect0 = tvSrc.m_bVect0;
  m_bVect1 = tvSrc.m_bVect1;
  m_nBitLength = tvSrc.m_nBitLength;
  return *this;
}

inline const CsTV& CsTV::operator =(const CsBV& bvSrc)        //operator =
{ 
  m_nBitLength = bvSrc.GetBitLength();
  m_bVect1 = (ULONG)bvSrc;
  m_bVect0 = (~(ULONG)bvSrc)>> (BITS_COUNT - m_nBitLength) << (BITS_COUNT - m_nBitLength);
  return *this;
}

inline const CsTV& CsTV::operator =(const char* pch)         // operator =
{ AssignChar(strlen((char*)pch), pch); return *this; }                             

inline void CsTV::Empty()                                   // Empty()
{ m_nBitLength = 0; m_bVect1 = m_bVect0 = 0; }

//******************************* Compareing operations ***********************
inline BOOL CsTV::IsEmpty() const                           //IsEmpty
{ return m_nBitLength == 0; }





// Inlines for CsTM classes - Ternary matrix
///////////////////////////////////////////////////////////////////////////////

//***************** Functions for getting class's parametrs **********

inline int CsTM::GetCountC() const
{ return m_nBitLength; }

inline int CsTM::GetCountR() const
{ return m_nSize; }

inline int CsTM::GetUpperBound() const
{ return m_nMaxSize; }

//********************************* Working with memory ***********************
inline void CsTM::RemoveAll()
{ SetSize(0,0); }

inline void CsTM::RemoveAllRows()
{ SetSize(0,m_nBitLength); }

//********************************** Data Reading *****************************

inline ULONG CsTM::GetLongZero(int nRow) const
{ return ( ULONG)m_pData[nRow+m_nMaxSize]; }

inline ULONG CsTM::GetLongOne(int nRow) const
{ return ( ULONG)m_pData[nRow];}

//**************************** Working with rows (changing their number) ******
inline int CsTM::Add(const CsTV& tv)
{ int nIndex = m_nSize; SetRowGrow(nIndex, tv); return nIndex; }

//******************************* Matrix compareing operations ****************
inline BOOL CsTM::IsEmpty() const
{ return (m_nSize==0); }  //(m_pData==NULL) & (


inline int CsTM::CountUnDefs(int nRow/*=-1*/) const
{ int i=CountDefs(nRow);
  return ((nRow==-1)?((m_nSize*m_nBitLength)-i):(m_nBitLength-i));
}


//----------------- Init() ----------------------------------------------------
inline void CsTM::Init()
{ m_pData = NULL;
  m_nSize = m_nMaxSize =  m_nBitLength = 0;
  m_nGrowBy = 10;
}

#ifndef _LINUX
#undef AFXAPP_DATA
#define AFXAPP_DATA     NEAR
#endif
#endif
