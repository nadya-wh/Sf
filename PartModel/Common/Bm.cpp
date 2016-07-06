/////////////////////////////////////////////////////////////////
// Ver.2.1.1    22.02.2007
//               ѕоправки установки значени€ m_nAllocLength в конструкторах
//               и SetSize
//               »зменена и перенесена из inline функци€ 
//               Add(const CBM& bm, int nbmRow): в ней теперь разрешено 
//               nbmRow=-1, что означает дописывание всей матрицы
// Ver.2.1.0    07.07.2006
//               ѕереход к LINUX. ѕоправки BitChar

/////////////////////////////////////////////////////////////////
// Ver.2.0.6     19.06.2006
//               поправка BitChar - более точный захват пам€ти под результат
// Ver.2.0.5     21.04.2006
//               поправки в конструкторах матриц дл€ случа€ создани€
//               новых с числом строк =0 и числом столбцов !=0
// Ver.2.0.4     05.04.2006
//               поправки в BitChar - добавлены новые параметры 
//               (WithNum, WithClear)
// Ver.2.0.2     24.03.2006
//               поправка операции чтени€ из архива (отказ от WORD)
// Ver.2.0.1     21.03.2006
//               - √енераторы случайных перестроены на возврат *this
//                 ForMK_V(CBV &bvX) перенесена в класс CSOP;
// Ver.2.0.0     04.03.2006
//               - ƒобавлены генераторы случайных
//                 и  CBM ForMK_V(CBV &bvX);

// Ver.1.1.3     01.03.2006
//               - ƒобавлены функции записи логических операций на месте
// Ver.1.1.2     25.02.2006
//             - ѕоправка всех операторов ASSERT дл€ функций SetRowLog...
// Ver.1.1.1    08.03.2005   Russian == English

/////////////////////////////////////////////////////////////////////////////
//
// Implementation of Boolean Matrix class
//
/////////////////////////////////////////////////////////////////////////////

#include <limits.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#define SIZE_T_MAX  UINT_MAX            /* max size for a size_t */

#ifdef _JOINT
#include "BaseBool.h"
#include "archive.h"
#else
#include "../Common/BaseBool.h"
#include "../Com_Lib/archive.h"
#endif


//#ifndef _LINUX

//IMPLEMENT_SERIAL(CBM, CObject, 0);

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
//#define new DEBUG_NEW

//#else  //_LINUX
#undef _DEBUG
//#endif //_LINUX

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
// √енераци€ псевдослучайной <nRow>*<nCol> булевой матрицы //
// с равноверо€тным распределением нулей и единиц          //                                      //
/////////////////////////////////////////////////////////////

CBM CBM::GenRbm(int nRow, int nCol)
{
  int i;
  CBV bv;
  SetSize (nRow, nCol); 
  for (i=0; i<nRow; i++) SetRow (i, bv.GenRbv(nCol));
  return *this;
}

/////////////////////////////////////////////////////////////
// √енераци€ псевдослучайной <nRow>*<nCol> булевой матрицы //
// со средним числом nRang едириц в каждой строке          //
// (с дисперсией 1/16) (при nRang>=nCol все элементы матрицы равны 1)                                     //
/////////////////////////////////////////////////////////////

CBM CBM::GenRbmMid (int nRow, int nCol, int nRang)
{
  int i;
  CBV bv;
  SetSize (nRow, nCol); 
  for (i=0; i<nRow; i++) SetRow (i, bv.GenRbvMid(nCol, nRang));
  return *this;
}

/////////////////////////////////////////////////////////////
// √енераци€ псевдослучайной <nRow>*<nCol> булевой матрицы //
// с фиксированным числом <nRang> единиц в каждой строке 
// (при nRang>=nCol все элементы матрицы равны 1)
/////////////////////////////////////////////////////////////

CBM CBM::GenRbmFix (int nRow, int nCol, int nRang)
{ 
  int i;
  CBV bv;
  SetSize (nRow, nCol); 
  for (i=0; i<nRow; i++) SetRow (i, bv.GenRbvFix(nCol, nRang));
  return *this;
}

///*******************************************************// 
/////////////////////////////////////////////////////////////
// √енерирование псевдослучайной <nRow>*<nCol> булевой матрицы 
// с линейно независимыми строками и равноверо€тным распреде-
// лением нулей и единиц.
/////////////////////////////////////////////////////////////

CBM CBM::RbmUndRow(int nRow, int nCol)
{
  RemoveAll();
  if(nRow>nCol)
  { 
#ifdef _DEBUG
    TRACE("RbmUndRow: Cтрок больше, чем столбцов!");
#endif
    return *this;
  }
  int i, j, k;
  SetSize(nRow, nCol); 
  CBM BmC(nRow, nCol); 
  CBV bv, bvH((BYTE)0,nCol);
  for (i=0; i<nRow; i++)
  {
    SetRow(i, bv=bv.GenRbv(nCol));
    for(j=0; j<i; j++)
      if(!(bv&BmC.GetRowBv(j,bvH)).IsZero())
        bv^=BmC.GetRow(j);
    if(bv.IsZero()) {i--; continue;}
    BmC.SetRow(i,bv);
    bvH.SetBitAt(k=bv.LeftOne(-1),1);
    for(j=0; j<i; j++)
      if(BmC.GetBitAt(j,k)) BmC.SetRow(j,BmC.GetRow(j)^bv);
  }
  return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////// Construction/Destruction ///////////////////////////////////////

//****************************** Protected function ***********************************//
//-------------------------------------------------------------------------------- Init()
void CBM::Init()
{ m_pData = NULL;
  m_nSize = m_nMaxSize =  m_nBitLength = m_nByteLength = m_nAllocLength = 0;
  m_nGrowBy = 10;
}

//---------------------------------------------------- AllocMatrix(int nRow, int nColumn)
void CBM::AllocMatrix(int nRow, int nColumn)
{ ASSERT(nRow > 0);
  ASSERT(nColumn > 0);
  m_pData = (BYTE**) new BYTE[nRow * sizeof(BYTE*)];
  int i,nByte = LEN_BYTE(nColumn) * sizeof(BYTE);
  for (i=0; i<nRow; i++) {
    m_pData[i] = (BYTE*) new BYTE[nByte];
    memset(m_pData[i], 0, nByte);  // zero fill
  }
  m_nSize = m_nMaxSize = nRow;
  m_nBitLength = nColumn;
  m_nAllocLength = m_nByteLength = nByte;
}

//-------------------------------------------------------------------------- SafeDelete()
void CBM::SafeDelete()
{ int i;
  for (i=0; i<m_nSize; i++) {
    if (m_pData[i]!=NULL) delete [] (BYTE*)m_pData[i]; }
  if(m_nMaxSize > 0) delete [] (BYTE*)m_pData;
  m_pData=NULL;
}

//******************************* Constructors\Destructor *******************************

//--------------------------------------------------------------------------------- CBM()
CBM::CBM() { Init();}

//----------------------------------------------- CBM(const CBM& bm, BOOL Fl/* = TRUE */)
CBM::CBM(const CBM& bm, BOOL Fl/* = TRUE */)
{ int i,j;
  if (bm.m_nSize == 0) {        // 21.04.2006
    Init();
    if (bm.m_nBitLength==0) return;   
    m_nBitLength = bm.m_nBitLength;
    m_nByteLength = bm.m_nByteLength;
    m_nAllocLength = bm.m_nByteLength;  // 22.02.2007
    m_nGrowBy = bm.m_nGrowBy;
    return;
  }
  if (bm.m_nBitLength==0) { Init(); return; }  // 21.04.2006
  if (Fl)  {
    AllocMatrix(bm.m_nSize,bm.m_nBitLength);
    for (i=0;i<m_nSize;i++) memcpy(m_pData[i],bm.m_pData[i],m_nByteLength);
  }
  else {
    AllocMatrix(bm.m_nBitLength,bm.m_nSize);
    for (j=0;j<m_nBitLength;j++)
      for (i=0;i<m_nSize;i++) SetBitAt(i,j,bm.GetBitAt(j,i));
  }
  m_nGrowBy = bm.m_nGrowBy;
}

//----------------------------------------- CBM(int nRow, int nColumn,BOOL Fl/*= FALSE*/)
CBM::CBM(int nRow, int nColumn,BOOL Fl /*= FALSE*/)
{  ASSERT(nRow >= 0);
  ASSERT(nColumn >= 0);
  int i,nByte = LEN_BYTE(nColumn),w= S_1-ADR_BIT(nColumn);

  if (nRow==0 )  {          //01.02.2002
    if (nColumn==0) Init();
    else {
      m_nSize=m_nMaxSize=0; m_nAllocLength=m_nByteLength=nByte; // 22.02.2007
      m_nBitLength=nColumn; m_nGrowBy = 10; m_pData = NULL;
    }
    return;
  }
  if (nColumn==0) { Init(); return; }  // 21.04.2006
  AllocMatrix(nRow,nColumn);
  if (Fl)
    for (i=0; i<nRow; i++) {
      memset(m_pData[i], 0xFF, nByte);  /* one's fill*/
      if (w != S_1) m_pData[i][nByte-1] = (m_pData[i][nByte-1]>>w)<<w; //!!! -1
    }
  m_nGrowBy = 10;  
}

//-------------------------------------------- CBM(const BYTE* pbt, int nRow,int nColumn)
CBM::CBM(const BYTE* pbt, int nRow,int nColumn)
{ 
  if (nRow==0) {          // 21.04.2006
    Init(); 
    if (nColumn==0) return;
    m_nBitLength = nColumn;
    m_nAllocLength = m_nByteLength = LEN_BYTE(nColumn); // 22.02.2007
    m_nGrowBy = 10;
    return; 
  } 
 ASSERT(nRow > 0);
 ASSERT(nColumn >= 0);
 if (nColumn==0) { Init(); return; }  // 21.04.2006
 int i,nByte = LEN_BYTE(nColumn),w = S_1-ADR_BIT(nColumn);
 AllocMatrix(nRow,nColumn);
 for (i=0; i<nRow; i++) {
   memcpy(m_pData[i], pbt, nByte);  /* one's fill*/
   if (w != S_1) m_pData[i][nByte-1] = (m_pData[i][nByte-1]>>w)<<w;   //!!! -1
 }
 m_nGrowBy = 10;
}

//------------------------------------------------------------------- CBM(const char pch)
//#ifndef _LINUX
//CBM::CBM(const char* pch)
//{
//  CString s(pch);
//  Init();
//  if (s.GetLength()) CharBit(s);
//}

////------------------------------------------------------------------- CBM(CStringArray StrAr)
//CBM::CBM(const CStringArray& StrAr)
//{ Init(); CharBit(StrAr); }
//#else

//------------------------------------------------------------------- CBM(const char pch)
CBM::CBM(const char* pch)
{
  string s = pch;
  Init();
  if ((int)s.length()>0) CharBit(s);
}
//#endif //_LINUX

//-------------------------------------------------------------------------------- ~CBM()
CBM::~CBM()
{
#ifdef _DEBUG
  ASSERT_VALID(this); 
#endif //_DEBUG
	SafeDelete(); 
}

/////////////////////////////////////////////////////////////////////////////////////////


//********************************* Working with memory *********************************
//-------------------------------- SetSize(int nRow, int nColumn, int nGrowBy /* = -1 */)
void CBM::SetSize(int nRow, int nColumn, int nGrowBy /* = -1 */,int nColGrow/*=0*/)
{ int i,nByte, nmaxRow, nBytemax;
  int w;                           //new 02.02.99
#ifdef _DEBUG
  ASSERT_VALID(this); 
  ASSERT(nRow >= 0);ASSERT(nColumn >= 0);
#endif //_DEBUG
 if (nGrowBy != -1) m_nGrowBy = nGrowBy;  // set new size
 nmaxRow=nRow+m_nGrowBy; nBytemax=LEN_BYTE(nColumn+nColGrow);
 nByte = LEN_BYTE(nColumn);
 //------------------ shrink to nothing
 if (nRow == 0) {
   SafeDelete(); m_nBitLength=nColumn;       //Init();
   m_nSize=m_nMaxSize=0; m_nByteLength=nByte;
   m_nAllocLength=nBytemax;
   return;
 }
 //------------------ create one with exact size
 if (m_pData == NULL) { // create one with exact size
#ifdef SIZE_T_MAX
   ASSERT((long)nmaxRow * sizeof(BYTE*) <= SIZE_T_MAX);  // no overflow
#endif
   //AllocMatrix(nRow, nColumn);
  m_pData = (BYTE**) new BYTE[nmaxRow * sizeof(BYTE*)];
  for (i=0; i < nRow; i++) {
    m_pData[i] = (BYTE*) new BYTE[nBytemax];
    memset(m_pData[i],0,nBytemax);           // zero fill
  }
  m_nSize = nRow; m_nMaxSize = nmaxRow;
  m_nBitLength = nColumn; m_nAllocLength =nBytemax;
  m_nByteLength=nByte;
  return;
 }
 //------------------ Set columns size
 if (nByte > m_nAllocLength) {
   BYTE * ptr;
   for (i=0; i < m_nSize; i++) {
     ptr= (BYTE*) new BYTE[nBytemax];
     memset(ptr, 0, nBytemax);  // zero fill
     if (m_pData[i]!=NULL) {
       memcpy(ptr, m_pData[i], m_nByteLength); // copy new data from old
       delete [] (BYTE*)m_pData[i];
     }
     m_pData[i] = ptr;
   }
   m_nAllocLength=nBytemax;
 }
 else {                //when(nByte < m_nAllocLength)  new 022.12.99!!!!!!!!!!
   int nost;           // new 022.12.99!!!!!!!!!!
   if (nColumn < m_nBitLength) {
   w = S_1-ADR_BIT(nColumn);  nost=nByte;   }
   else   {
   w = S_1-ADR_BIT(m_nBitLength); nost=m_nByteLength;   }
   if (w==8) w = 0;                                    //new 02.02.99!!!!!!!!!!
   for (i=0; i < m_nSize; i++) {
     memset(m_pData[i]+nost,0,(m_nAllocLength-nost)); //memset(m_pData[i]+nByte,0,(m_nByteLength-nByte));
     m_pData[i][nost-1] = (m_pData[i][nost-1]>>w)<<w;
   }
 }  
 m_nBitLength = nColumn; m_nByteLength = nByte;
 //------------------ Set rows size
 if (nRow <= m_nMaxSize) {
   if (nRow > m_nSize) {    // it fits // initialize the new elements
     for (i=m_nSize; i<nRow; i++) {
       m_pData[i] = (BYTE*) new BYTE[m_nAllocLength];
       memset(m_pData[i], 0,m_nAllocLength);
     }
   }
   else 
     for (i=m_nSize-1; i>=nRow; i--) 
       if (m_pData[i]!=NULL) { delete [] (BYTE*)m_pData[i]; m_pData[i] = NULL; }
   m_nSize = nRow;
   return;
 }
 //-------------- Otherwise grow array
 /*int nNewMax;
 if (nRow < m_nMaxSize + m_nGrowBy) nNewMax = m_nMaxSize + m_nGrowBy;  // granularity
 else                               nNewMax = nRow;                    // no slush
#ifdef SIZE_T_MAX
 ASSERT((long)nNewMax * sizeof(BYTE*) <= SIZE_T_MAX);  // no overflow
#endif
 BYTE** pNewData = (BYTE**) new BYTE[nNewMax * sizeof(BYTE*)];
 memcpy(pNewData, m_pData, m_nSize * sizeof(BYTE*)); // copy new data from old
 ASSERT(nRow > m_nSize);  // construct remaining elements
 for (i=m_nSize; i<nRow; i++) {
   pNewData[i] = (BYTE*) new BYTE[m_nAllocLength];
   memset(pNewData[i], 0,m_nAllocLength);
 }
 for (i=nRow; i < nNewMax; i++) pNewData[i] = NULL;
 // get rid of old stuff (note: no destructors called)
 //for (i=m_nSize-1; i>=0; i--)  if (m_pData[i]!=NULL) delete [] (BYTE*)m_pData[i];
 delete [] (BYTE*)m_pData;
 m_pData = pNewData;
 m_nSize = nRow;
 m_nMaxSize = nNewMax; */    //new 22.12.99
#ifdef SIZE_T_MAX
 ASSERT((long)nmaxRow * sizeof(BYTE*) <= SIZE_T_MAX);  // no overflow
#endif
 BYTE** pNewData = (BYTE**) new BYTE[nmaxRow * sizeof(BYTE*)];
 memcpy(pNewData, m_pData, m_nSize * sizeof(BYTE*)); // copy new data from old
 ASSERT(nRow > m_nSize);  // construct remaining elements
 for (i=m_nSize; i<nRow; i++) {
   pNewData[i] = (BYTE*) new BYTE[m_nAllocLength];
   memset(pNewData[i], 0,m_nAllocLength);
 }
 for (i=nRow; i < nmaxRow; i++) pNewData[i] = NULL;
 // get rid of old stuff (note: no destructors called)
 //for (i=m_nSize-1; i>=0; i--)  if (m_pData[i]!=NULL) delete [] (BYTE*)m_pData[i];
 delete [] (BYTE*)m_pData; m_pData = pNewData;
 m_nSize = nRow; m_nMaxSize = nmaxRow;
}


//---------------------------------------------------------------------  FreeExtra()
void CBM::FreeExtra()
{
#ifdef _DEBUG
  ASSERT_VALID(this); 
#endif //_DEBUG
  int i;
  BYTE * ptr;

  if (m_nByteLength<m_nAllocLength) { // Free row memory
    for (i=0; i<m_nSize; i++) {
      ptr= (BYTE*) new BYTE[m_nByteLength];
      memcpy(ptr, m_pData[i], m_nByteLength); // copy new data from old
      if (m_pData[i]!=NULL) delete [] (BYTE*)m_pData[i];
      m_pData[i] = ptr;
   }
   m_nAllocLength = m_nByteLength;
  }

  if (m_nSize != m_nMaxSize) { // shrink to desired size
#ifdef SIZE_T_MAX
    ASSERT((long)m_nSize * sizeof(BYTE*) <= SIZE_T_MAX);  // no overflow
#endif
    BYTE** pNewData = NULL;
    if (m_nSize != 0) {
      pNewData = (BYTE**) new BYTE[m_nSize * sizeof(BYTE*)];
      memcpy(pNewData, m_pData, m_nSize * sizeof(BYTE*));       // copy new data from old
    }
    // get rid of old stuff (note: no destructors called)
    delete [] (BYTE*)m_pData;
    m_pData = pNewData;
    m_nMaxSize = m_nSize;
  }
}

/////////////////////////////////////////////////////////////////////////////////////////


//********************************** Reading the data ***********************************
//#ifndef _LINUX
////----------------------------------------------------------- BitChar(char One,char Zero)
//void CBM::BitChar(CStringArray & StrRes, BOOL WithClear, BOOL WithNum,
//                  char One, char Zero)
//{
//  int i,j;
//  CString ch;
//  if (WithClear) StrRes.RemoveAll();
//  if (m_nSize>9999)     { ch = "%5d: ";}
//  else if (m_nSize>999) { ch = "%4d: ";}
//  else if (m_nSize>99)  { ch = "%3d: ";}
//  else                  { ch = "%2d: ";}

//  for (j=0;j<m_nSize;j++) {
//    CString s;
//    if(WithNum == TRUE) s.Format((const char*)ch,j);
//    for (i=0;i<m_nBitLength;i++)
//      if (GetBitAt(j,i)) s+=One;
//      else               s+=Zero;
//    StrRes.Add(s);
//  }
//}

////----------------------------------------------------------- BitChar(char One,char Zero)
//CString CBM::BitChar(char One, char Zero, BOOL WithNum)
//{
//  int i,j,pos = 0;
//  CString ch, s;
//  if(WithNum == TRUE) {
//    if (m_nSize>9999)     { ch = "%5d: "; i=9;}
//    else if (m_nSize>999) { ch = "%4d: "; i=8;}
//    else if (m_nSize>99)  { ch = "%3d: "; i=7;}
//    else                  { ch = "%2d: "; i=6;}
//  }
//  else i = 2;

//  CString res('\0',(m_nBitLength+i) * m_nSize);
//  for (j=0; j<m_nSize; j++) {
//    if(WithNum == TRUE) {
//      s.Format((const char*)ch,j);
//      for (i=0; i<s.GetLength(); i++)
//        res.SetAt(pos++,s[i]);
//    }
//   for (i=0; i<m_nBitLength; i++)
//     if (GetBitAt(j,i)) res.SetAt(pos++,One);
//     else               res.SetAt(pos++,Zero);
//   res.SetAt(pos++,'\n');
//  }
//  return res;
//}

//#else
//----------------------------------------------------------- BitChar(char One,char Zero)
char * CBM::BitChar(char One, char Zero, BOOL WithNum)
{
  int i,j,pos = 0;
  char ch[20], s[20];
  if(WithNum == TRUE) {
    if (m_nSize>9999)     { strcpy(ch,"%5d: "); i=9;}
    else if (m_nSize>999) { strcpy(ch,"%4d: "); i=8;}
    else if (m_nSize>99)  { strcpy(ch,"%3d: "); i=7;}
    else                  { strcpy(ch,"%2d: "); i=6;}
  }
  else i = 2;
  char * res;
  res = (char *)malloc((m_nBitLength+i) * m_nSize);
  memset(res,0,(m_nBitLength+i) * m_nSize);
  
  for (j=0;j<m_nSize;j++) {
    if(WithNum == TRUE) {
      sprintf(s,(const char*)ch,j);
      for (i=0;i<strlen(s);i++) 
        res[pos++] = s[i];
    }
   for (i=0;i<m_nBitLength;i++)
     if (GetBitAt(j,i)) res[pos++] = One;
     else               res[pos++] = Zero;
   res[pos++] = '\n';   // res[pos++] = '\r'; 
  }
  return res;
}

//#endif
//----------------------------------------------------------- BitChar(char One,char Zero)
void CBM::BitChar(vector <string>& StrRes, BOOL WithClear, BOOL WithNum, 
  char One, char Zero)
{ 
  int i,j;
  string ch;
  char bbf[20];
  if (WithClear) StrRes.clear();
  if (m_nSize>9999)     { ch = "%5d: ";}
  else if (m_nSize>999) { ch = "%4d: ";}
  else if (m_nSize>99)  { ch = "%3d: ";}
  else                  { ch = "%2d: ";}

  for (j=0;j<m_nSize;j++) {
    string s;
    if(WithNum == TRUE) {
      //s.Format((const char*)ch,j);
      sprintf(bbf,(const char*)ch.c_str(),j);
      s = bbf;
    }
    for (i=0;i<m_nBitLength;i++)
      if (GetBitAt(j,i)) s+=One;
      else               s+=Zero;
      StrRes.push_back(s);
  }
}

//---------------------------------------------------------------GetRowBv(int nIndex)
CBV CBM::GetRowBv(int nIndex) const
{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
  CBV bv(m_pData[nIndex],m_nBitLength);
  return bv;
}
 
//---------------------------------------------------------------GetRowBv(int nIndex)
CBV CBM::GetRowBv(int nIndex,BYTE* mask) const
{ ASSERT(nIndex >= 0 && nIndex < m_nSize);
  CBV bv(m_pData[nIndex],m_nBitLength);
  bv&=mask;
  return bv;
}

//---------------------------------------------------------------GetColumnBv(int nColumn)
CBV CBM::GetColumnBv(int nColumn) const
{ASSERT(nColumn >= 0);
 ASSERT(nColumn < m_nBitLength);
 CBV bv(0,m_nSize,TRUE);
 int i;
 for (i=0;i<m_nSize;i++) bv.SetBitAt(i,GetBitAt(i,nColumn));
 return bv;
}
  
//---------------------------------------------------------------GetColumnBv(int nColumn)
CBV CBM::GetColumnBv(int nColumn,BYTE* mask) const
{ASSERT(nColumn >= 0);
 ASSERT(nColumn < m_nBitLength);
 CBV bv(0,m_nSize,TRUE);
 int i;
 for (i=0;i<m_nSize;i++) bv.SetBitAt(i,GetBitAt(i,nColumn));
 bv&=mask;
 return bv;
}

  
//------------------------------------------------ExtractRows(int nFirst, int nCount)
CBM CBM::ExtractRows(int nFirst, int nCount) const
{ASSERT(nFirst >= 0);
 ASSERT((nFirst + nCount) <=m_nSize);
 ASSERT(nCount > 0);
 CBM bm(nCount,m_nBitLength);
 int i;
 for (i=0;i<nCount;i++) bm.SetRow(i,GetRow(nFirst + i));
 return bm;
}

//------------------------------------------------ExtractColumns(int nFirst, int nCount)
//CBM CBM::ExtractColumns(int nFirst, int nCount) const
//{ASSERT(nFirst >= 0);
// ASSERT((nFirst + nCount) <=m_nBitLength);
// ASSERT(nCount > 0);
// CBM bm(m_nSize,nCount);
// int j,i,k,l_bit,r_bit,nSrcByte,AdrBeg;
// BYTE ch;
// k = S_1 - ADR_BIT(nCount);
// AdrBeg = BIT_BYTE(nFirst);
// l_bit = ADR_BIT(nFirst);
// r_bit = S_1 - l_bit;
// nSrcByte = BIT_BYTE(nFirst + nCount);
// for (j=0;j<m_nSize;j++)  {
//   if (AdrBeg - nSrcByte)  {
//     for (i=AdrBeg; i<=nSrcByte; i++)  {
//       ch = ( m_pData[j][i] << l_bit) | (m_pData[j][i+1] >> r_bit);
//       bm.m_pData[j][i-AdrBeg] = ch;
//     }
//     ch = bm.m_pData[j][nSrcByte-AdrBeg];
//     if (k != S_1) ch = (ch >> k) << k;
//     bm.m_pData[j][nSrcByte-AdrBeg] = ch;
//   }
//   else  {
//     ch = ( m_pData[j][AdrBeg] << l_bit) | (m_pData[j][AdrBeg+1] >> r_bit);
//     if (k != S_1) ch = (ch >> k) << k;
//     bm.m_pData[j][0] = ch;
//   }
// }
// return bm;
//}

//------------------------------------------------ExtractColumns(int nFirst, int nCount)
CBM CBM::ExtractColumns(int nFirst, int nCount) const
{ASSERT(nFirst >= 0); ASSERT(nCount > 0);
 ASSERT((nFirst + nCount) <=m_nBitLength);
 int i;
 CBM bm(m_nSize,nCount);
 CBV bv(0,m_nBitLength,TRUE);
 CBV bv1;
 
 for (i=0; i<m_nSize; i++)  { 
   bv = m_pData[i]; 
   bv1 = bv.Extract(nFirst, nCount);
   bm.SetRow(i, bv1);
 }
 return bm;
}

//------------- Extract(int nFirstRow, int nFirstColumn, int nCountRow, int nCountColumn)
//CBM CBM::Extract(int nFirstRow, int nFirstColumn, int nCountRow, int nCountColumn) const
//{ASSERT(nFirstRow >= 0);
// ASSERT(nFirstColumn >= 0);
// ASSERT((nFirstColumn + nCountColumn) <=m_nBitLength);
// ASSERT((nFirstRow + nCountRow) <=m_nSize);
// ASSERT(nCountColumn > 0);
// ASSERT(nCountRow > 0);
// CBM bm(nCountRow,nCountColumn);
// int j,i,k,l_bit,r_bit,nSrcByte,AdrBeg;
// BYTE ch;
// k = S_1 - ADR_BIT(nCountColumn);
// AdrBeg = BIT_BYTE(nFirstColumn);
// l_bit = ADR_BIT(nFirstColumn);
// r_bit = S_1 - l_bit;
// nSrcByte = BIT_BYTE(nFirstColumn + nCountColumn);
// for (j=nFirstRow;j<nFirstRow+nCountRow;j++)  {
//   if (AdrBeg - nSrcByte)  {
//     for (i=AdrBeg; i<=nSrcByte; i++)  {
//       ch = ( m_pData[j][i] << l_bit) | (m_pData[j][i+1] >> r_bit);
//       bm.m_pData[j-nFirstRow][i-AdrBeg] = ch;
//     }
//     ch = bm.m_pData[j-nFirstRow][nSrcByte-AdrBeg];
//     if (k != S_1) ch = (ch >> k) << k;
//     bm.m_pData[j-nFirstRow][nSrcByte-AdrBeg] = ch;
//   }
//   else  {
//     ch = ( m_pData[j][AdrBeg] << l_bit) | (m_pData[j][AdrBeg+1] >> r_bit);
//     if (k != S_1) ch = (ch >> k) << k;
//     bm.m_pData[j-nFirstRow][0] = ch;
//   }
//  }
// return bm;
//}

//------------- Extract(int nFirstRow, int nFirstColumn, int nCountRow, int nCountColumn)
CBM CBM::Extract(int nFirstRow, int nFirstColumn, int nCountRow, int nCountColumn) const
{ASSERT(nFirstRow >= 0); ASSERT(nFirstColumn >= 0);
 ASSERT((nFirstColumn + nCountColumn) <=m_nBitLength);
 ASSERT((nFirstRow + nCountRow) <=m_nSize);
 ASSERT(nCountColumn > 0);  ASSERT(nCountRow > 0);
 CBM bm(nCountRow,nCountColumn); 
 CBV bv(0,m_nBitLength,TRUE);
 CBV bv1;
 int i;
 
 for (i=nFirstRow; i<nFirstRow+nCountRow; i++)  { 
   bv = m_pData[i]; 
   bv1 = bv.Extract(nFirstColumn, nCountColumn);
   bm.SetRow(i, bv1);
 }
 return bm;
} 

//-------------------------------------------- Minor(const CBV& Rows, const CBV& Columns)
CBM CBM::Minor(const CBV& Rows, const CBV& Columns) const
{int i,j,LeftOneRow = -1,LeftOneColumn = -1;
 int  NewRowLen,NewColumnLen;
 NewRowLen = Rows.CountBit();
 NewColumnLen = Columns.CountBit();
 CBM bm(NewRowLen,NewColumnLen);
 for (i=0;i<NewRowLen;i++)  {
   LeftOneRow = Rows.LeftOne(LeftOneRow);
   for (j=0;j<NewColumnLen;j++)  {
     LeftOneColumn = Columns.LeftOne(LeftOneColumn);
     bm.SetBitAt(i,j,GetBitAt(LeftOneRow,LeftOneColumn));
   }
   LeftOneColumn = -1;
 }
 return bm;
}

//------------------------------------------------------------------------- Trans() const
CBM CBM::Trans() const
{CBM bm(*this,FALSE);
 return bm;
}

/////////////////////////////////////////////////////////////////////////////////////////


//********************************* Working with data ***********************************
//-------------------------------------------------------SetRow(int nRow, const CBV& bv)
void CBM::SetRow(int nRow, const CBV& bv)
{ ASSERT(nRow >= 0 && nRow < m_nSize);
  ASSERT(m_nBitLength == bv.GetBitLength());
  memcpy(m_pData[nRow],(const BYTE *)bv,m_nByteLength);
}                                  //NEWS !!

//--------------------------------------------SetRow(int nRow, const CBM& bm, int nbmRow)
void CBM::SetRow(int nRow, const CBM& bm, int nbmRow)
{ ASSERT(nRow >= 0 && nRow < m_nSize);
  ASSERT(m_nBitLength == bm.GetCountC());
  memcpy(m_pData[nRow],bm.m_pData[nbmRow],m_nByteLength);
}

/////////////////////////////////////////////////////////////////////////////////////////


//********************************** Working with row ***********************************
//----------------------------------------------SetRowGrow(int nRow,const BYTE* newRow)
void CBM::SetRowGrow(int nRow,const BYTE* newRow)
{ 
#ifdef _DEBUG
  ASSERT_VALID(this); 
  ASSERT(nRow >= 0);
#endif //_DEBUG
  if (nRow >= m_nSize)
    SetSize(nRow+1,m_nBitLength,m_nGrowBy,m_nAllocLength*S_1-m_nBitLength);
  memcpy(m_pData[nRow],newRow,m_nByteLength);
}

//----------------------------------------------SetRowGrow(int nRow, const CBV& bv)
void CBM::SetRowGrow(int nRow, const CBV& bv)
{
#ifdef _DEBUG
  ASSERT_VALID(this); 
  ASSERT(nRow >= 0);
  if (m_nSize > 0)  ASSERT(m_nBitLength == bv.GetBitLength());
#endif //_DEBUG
  if (nRow >= m_nSize) 
    SetSize(nRow+1,bv.GetBitLength(),m_nGrowBy,m_nAllocLength*S_1-m_nBitLength);
  memcpy(m_pData[nRow],(const BYTE *)bv,m_nByteLength);
}

//---------------------------------------------SetRowGrow(int nRow, const CBM& bm, int nbmRow)
void CBM::SetRowGrow(int nRow, const CBM& bm, int nbmRow)
{
#ifdef _DEBUG
  ASSERT_VALID(this); 
  ASSERT(nRow >= 0 && nbmRow < bm.m_nSize);
  if (m_nSize > 0) ASSERT(m_nBitLength == bm.m_nBitLength); //11.02.99
#endif //_DEBUG
  if (nRow >= m_nSize)
    SetSize(nRow+1,m_nBitLength,m_nGrowBy,m_nAllocLength*S_1-m_nBitLength);
  memcpy(m_pData[nRow],bm.m_pData[nbmRow],m_nByteLength);
}

//-------------------------------------------- Add -------------------------
int CBM::Add(BOOL bit/*=FALSE*/,int nCount/*=1*/)
{int i, first=m_nSize;
 if(m_nSize + nCount < m_nMaxSize) {      //It was bifore 21.10.99 m_nSize < m_nMaxSize
   m_nSize+=nCount;
   for(i=first; i < m_nSize; i++) {
     m_pData[i] = (BYTE*) new BYTE[m_nAllocLength];
     memset(m_pData[i],0,m_nAllocLength);
   }
 }
 else 
   SetSize(m_nSize+nCount,m_nBitLength,m_nGrowBy,m_nAllocLength*S_1-m_nBitLength);
 if (bit == TRUE) {
   BYTE maska=((i = ADR_BIT(m_nBitLength)) > 0 )?
             (0xff<<(S_1 - i)):0xff;
   for (; first < m_nSize; first++) {
     memset(m_pData[first],0xff,m_nByteLength-1);
     m_pData[first][m_nByteLength-1]=maska;
   }
  }
 return m_nSize-1;
 } 

// 22.02.2007
//-----------------------------------Add(const CBM& bm, int nbmRow)
int CBM::Add(const CBM& bm, int nbmRow)
{ int nIndex = m_nSize; 
  if (nbmRow >= 0) SetRowGrow(nIndex,bm,nbmRow); 
  else InsertRow(nIndex, 0, bm, bm.GetCountR());
  return nIndex; 
}

//-----------------------------------InsertRow(int nRow,const BYTE* newRow, int nCount /*=1*/)
void CBM::InsertRow(int nRow,const BYTE* newRow, int nCount /*=1*/)
{ int i;
  BYTE * ptr;
#ifdef _DEBUG
  ASSERT_VALID(this); 
  ASSERT(nRow >= 0);       // will expand to meet need
  ASSERT(nCount > 0);      // zero or negative size not allowed
#endif //_DEBUG
  if (nRow >= m_nSize) {   // adding after the end of the array
    SetSize(nRow + nCount,m_nBitLength,m_nGrowBy,m_nAllocLength*S_1-m_nBitLength);  // grow so nIndex is valid
  }
  else {                   // inserting in the middle of the array
//    int nOldSize = m_nSize;
    SetSize(m_nSize + nCount,m_nBitLength,m_nGrowBy,m_nAllocLength*S_1-m_nBitLength);  // grow it to new size 
    for (i=0; i<nCount; i++) {
      ptr=m_pData[m_nSize-1];
      memmove(&m_pData[nRow+1], &m_pData[nRow],(m_nSize-nRow-1) * sizeof(BYTE*));
      m_pData[nRow]=ptr;
    }
  }
// insert new value in the gap
//                   ASSERT(nRow + nCount <= m_nSize);
  for (i=nRow; i<nRow+nCount; i++)   memcpy(m_pData[i],newRow,m_nByteLength);
}


//---------------------------------InsertRow(int nRow, const CBV& newRow, int nCount /*= 1*/)
void CBM::InsertRow(int nRow,const CBV& newRow, int nCount)
{
#ifdef _DEBUG
  ASSERT_VALID(this); 
  ASSERT(newRow != NULL);
  ASSERT(nRow >= 0);
  ASSERT(newRow.GetByteLength() == m_nByteLength);
#endif //_DEBUG
  InsertRow(nRow,(const BYTE*)newRow, nCount);
}

//---------------------InsertRow(int nRow,int nStartRow, const CBM& pNewBM,int nCount /*= 1*/)
void CBM::InsertRow(int nRow,int nStartRow, const CBM& NewBM,int nCount)
{
#ifdef _DEBUG
  ASSERT_VALID(this); 
  ASSERT(NewBM.IsKindOf(RUNTIME_CLASS(CBM)));
  ASSERT(nRow >= 0);  ASSERT(nStartRow >= 0); 
  ASSERT(nStartRow + nCount <= NewBM.m_nSize);
  ASSERT(m_nBitLength == NewBM.m_nBitLength);
#endif //_DEBUG
  if (NewBM.m_nSize > 0)    {  // 22.02.2007
    InsertRow(nRow, NewBM.m_pData[nStartRow], nCount);
    for (int i = 1; i < nCount; i++)
      SetRow(nRow + i, NewBM.m_pData[nStartRow + i]);
  }
}

//-----------------------------------------RemoveRow(int nIndex, int nCount /* = 1 */)
void CBM::RemoveRow(int nIndex, int nCount /* = 1 */)
{
#ifdef _DEBUG
  ASSERT_VALID(this); 
  ASSERT(nIndex >= 0);
  ASSERT(nCount >= 0);
  ASSERT(nIndex + nCount <= m_nSize);
#endif //_DEBUG
  int nMoveCount = m_nSize - (nIndex + nCount);     // just remove a range
  int i;
  for (i=0; i<nCount; i++) 
    if (m_pData[i+nIndex]!=NULL) delete [] (BYTE*)m_pData[i+nIndex];
  if (nMoveCount) {
      memcpy(&m_pData[nIndex], &m_pData[nIndex + nCount],   nMoveCount * sizeof(BYTE*));
    }  
    m_nSize -= nCount;
}

//-----------------------------------------ExchangeRow(int nRow1,int nRow2,int nCount/*=1*/))
void CBM::ExchangeRow(int nRow1,int nRow2,int nCount/*=1*/)
{
#ifdef _DEBUG
  ASSERT_VALID(this); 
 ASSERT(nCount>0 && nRow1+nCount<=m_nSize && nRow2+nCount<=m_nSize);
#endif //_DEBUG
 BYTE *ptr;
 while(nCount >0) {
   ptr=m_pData[nRow1]; m_pData[nRow1++]=m_pData[nRow2];
   m_pData[nRow2++]=ptr; nCount--; 
 }
 return;
}

//----------------- SortDecr ------------------------------------------------------------
void CBM::SortDecr(int nBeg/*=0*/,int number/*=-1*/)
{ASSERT(number == -1 || number > 1);
 BOOL bNotDone = TRUE;
 int pos, posNext, nEnd=(number < 0)?m_nSize:(nBeg+number);
 ASSERT(nEnd <=m_nSize);
 while (bNotDone) {
   bNotDone = FALSE;
   for(pos=nBeg,posNext=nBeg+1; pos < nEnd-1;pos++,posNext++)
     if(CountBit(pos) < CountBit(posNext)) {
       ExchangeRow(pos,posNext);
       bNotDone = TRUE;
     }
 }
}

//----------------- SortIncr ------------------------------------------------------------
void CBM::SortIncr(int nBeg/*=0*/,int number/*=-1*/)
{ASSERT(number == -1 || number > 1);
 BOOL bNotDone = TRUE;
 int pos, posNext, nEnd=(number < 0)?m_nSize:(nBeg+number);
 ASSERT(nEnd <=m_nSize);
 while (bNotDone) {
   bNotDone = FALSE;
   for(pos=nBeg,posNext=nBeg+1; pos < nEnd-1;pos++,posNext++)
     if(CountBit(pos) > CountBit(posNext)) {
       ExchangeRow(pos,posNext);
       bNotDone = TRUE;
     }
 }
}

//----------------- AddCol -----------------------------------------------------------
int CBM::AddCol(int numCol/*=1*/)
//numCol columns adding to the matrix
//return the number of new last column
{ int i,nByte;
#ifdef _DEBUG
  ASSERT_VALID(this); 
  ASSERT(numCol > 0);
#endif //_DEBUG
 m_nBitLength+=numCol;
 nByte = LEN_BYTE(m_nBitLength) * sizeof(BYTE);
 //------------------ Set columns size
 if (nByte > m_nAllocLength) {
   BYTE * ptr;

   for (i=0; i < m_nSize; i++) {
     ptr= (BYTE*) new BYTE[nByte];
     memset(ptr,0,nByte);  // zero fill
     memcpy(ptr, m_pData[i], m_nByteLength); // copy new data from old
     delete [] (BYTE*)m_pData[i];
     m_pData[i] = ptr; 
   }
 m_nAllocLength=nByte;
 }
 m_nByteLength = nByte;
 return m_nBitLength-1;
}

/////////////////////////////////////////////////////////////////////////////////////////
//*********************** Operators and functions of assignment *************************
//------------------------------------------------------ operator =(const CBM& bm0) const
const CBM& CBM::operator =(const CBM& bm)
{int i,w = bm.m_nSize;
 SetSize(w,bm.m_nBitLength,m_nGrowBy);
 for (i=0;i<w;i++) SetRow(i,bm.GetRow(i));
 return *this;
}

//------------------------------------------------------ operator =(const char* pch)
const CBM& CBM::operator =(const char* pch)
{
  CharBit((const char*) pch);
  return *this;
}

//------------------------------------------------------ operator =(CStringArray StrAr)
//#ifndef _LINUX
//const CBM& CBM::operator =(const CStringArray& StrAr)
//{ CharBit(StrAr);
// return *this;
//}
//#


//----------------------------------------------------------------------- One(int nRow)
void CBM::One(int nRow)
{ int i,k,first,last;
 ASSERT (nRow>=-1);
 if (nRow==-1) { first=0; last=m_nSize-1; }
 else { first=nRow; last=nRow; }
 i = S_1 - ADR_BIT(m_nBitLength);
 for (k=first; k<=last; k++) {
   memset(m_pData[k],0xff,m_nByteLength); 
   if (i != S_1) 
     m_pData[k][m_nByteLength-1] = (m_pData[k][m_nByteLength-1] >> i) << i;
 }    
}

//----------------------------------------------------------------------- Zero(int nRow)
void CBM::Zero(int nRow)
{ int k,first,last;
 ASSERT (nRow>=-1);
 if (nRow==-1) { first=0; last=m_nSize-1; }
 else { first=nRow; last=nRow; }
 for (k=first; k<=last; k++)
   memset(m_pData[k],0,m_nByteLength); 
}


/////////////////////////////////////////////////////////////////////////////////////////


//**************************** Operations of concatinations *****************************
//----------------------------------------------------------------- Concat(const CBM& bm)
void CBM::Concat(const CBM& bm)
{ASSERT(m_nSize == bm.m_nSize);
 int i,j,OldLen = BIT_BYTE(m_nBitLength),
 l_bit = ADR_BIT(m_nBitLength), r_bit = S_1 - l_bit;
 SetSize(m_nSize,m_nBitLength + bm.m_nBitLength,m_nGrowBy);
 for (i=0;i<m_nSize;i++)  {
   m_pData[i][OldLen] |= (bm.m_pData[i][0] >> l_bit);
   for (j=1;j<bm.m_nByteLength;j++)
     m_pData[i][j+OldLen] = (bm.m_pData[i][j-1] << r_bit) | (bm.m_pData[i][j] >> l_bit);
   m_pData[i][j+OldLen] = m_pData[i][j+OldLen] | (bm.m_pData[i][j-1] << r_bit); 
 }
}

//------------------------------------------------- Concat(const CBM& bm1,const CBM& bm2)
void CBM::Concat(const CBM& bm1,const CBM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 SetSize(bm1.m_nSize,bm1.m_nBitLength + bm2.m_nBitLength,m_nGrowBy);
 int i,j,OldLen = BIT_BYTE(bm1.m_nBitLength),
 l_bit = ADR_BIT(bm1.m_nBitLength), r_bit = S_1 - l_bit;
 for (i=0;i<m_nSize;i++)
   memcpy(m_pData[i],bm1.m_pData[i],bm1.m_nByteLength);
 for (i=0;i<m_nSize;i++)  {
   m_pData[i][OldLen] |= (bm2.m_pData[i][0] >> l_bit);
   for (j=1;j<bm2.m_nByteLength;j++)
     m_pData[i][j+OldLen] = (bm2.m_pData[i][j-1] << r_bit) | (bm2.m_pData[i][j] >> l_bit);
   m_pData[i][j+OldLen] = m_pData[i][j+OldLen] | (bm2.m_pData[i][j-1] << r_bit);
 }
}

/////////////////////////////////////////////////////////////////////////////////////////


//************************** Operators of advanced assignment ***************************
//------------------------------------------------------- operator|=(const CBM& bm) 
const CBM& CBM::operator|=(const CBM& bm)
{ASSERT(m_nSize == bm.m_nSize);
 ASSERT(m_nBitLength == bm.m_nBitLength);
 int i,j;
 for (i=0;i<m_nSize;i++)
   for (j=0;j<m_nByteLength;j++)
     m_pData[i][j] |= bm.m_pData[i][j];
 return *this;
}

//------------------------------------------------------- operator|=(const CBV& bv) 
const CBM& CBM::operator|=(const CBV& bv)
{ASSERT(m_nBitLength == bv.GetBitLength());
 int i,j;
 for (i=0;i<m_nSize;i++)
   for (j=0;j<m_nByteLength;j++)
     m_pData[i][j] |= bv.GetByteAt(j);
 return *this;
}

//------------------------------------------------------- operator|=(const BYTE * pb)
const CBM& CBM::operator|=(const BYTE * pb)
{ int i,j;
 for (i=0;i<m_nSize;i++)
   for (j=0;j<m_nByteLength;j++)
     m_pData[i][j] |= pb[j];
 return *this;
}

//------------------------------------------------------- operator&=(const CBM& bm)
const CBM& CBM::operator&=(const CBM& bm)
{ASSERT(m_nSize == bm.m_nSize);
 ASSERT(m_nBitLength == bm.m_nBitLength);
 int i,j;
 for (i=0;i<m_nSize;i++)
   for (j=0;j<m_nByteLength;j++)
     m_pData[i][j] &= bm.m_pData[i][j];
 return *this;
}

//------------------------------------------------------- operator&=(const CBV& bv)
const CBM& CBM::operator&=(const CBV& bv)
{ASSERT(m_nBitLength == bv.GetBitLength());
 int i,j;
 for (i=0;i<m_nSize;i++)
   for (j=0;j<m_nByteLength;j++)
     m_pData[i][j] &= bv.GetByteAt(j);
 return *this;
}

//------------------------------------------------------- operator&=(const BYTE *pb)
const CBM& CBM::operator&=(const BYTE *pb)
{ int i,j;
 for (i=0;i<m_nSize;i++)
   for (j=0;j<m_nByteLength;j++)
     m_pData[i][j] &= pb[j];
 return *this;
}

//------------------------------------------------------- operator^=(const CBM& bm)
const CBM& CBM::operator^=(const CBM& bm)
{ASSERT(m_nSize == bm.m_nSize);
 ASSERT(m_nBitLength == bm.m_nBitLength);
 int i,j;
 for (i=0;i<m_nSize;i++)
   for (j=0;j<m_nByteLength;j++)
     m_pData[i][j] ^= bm.m_pData[i][j];
 return *this;
}

//------------------------------------------------------- operator^=(const CBV& bv)
const CBM& CBM::operator^=(const CBV& bv)
{ASSERT(m_nBitLength == bv.GetBitLength());
 int i,j;
 for (i=0;i<m_nSize;i++)
   for (j=0;j<m_nByteLength;j++)
     m_pData[i][j] ^= bv.GetByteAt(j);
 return *this;
}

//------------------------------------------------------- operator^=(const BYTE *pb)
const CBM& CBM::operator^=(const BYTE *pb)
{ int i,j;
 for (i=0;i<m_nSize;i++)
   for (j=0;j<m_nByteLength;j++)
     m_pData[i][j] ^= pb[j];
 return *this;
}

//------------------------------------------------------- operator-=(const CBV& bv)
const CBM& CBM::operator-=(const CBV& bv)
{ASSERT(m_nBitLength == bv.GetBitLength());
 int i,j;
 for (i=0;i<m_nSize;i++)
   for (j=0;j<m_nByteLength;j++)
     m_pData[i][j] -= bv.GetByteAt(j);
 return *this;
}

//------------------------------------------------------- operator-=(const BYTE *pb)
const CBM& CBM::operator-=(const BYTE *pb)
{
 int i,j;
 for (i=0;i<m_nSize;i++)
   for (j=0;j<m_nByteLength;j++)
     m_pData[i][j] -= pb[j];
 return *this;
}

/////////////////////////////////////////////////////////////////////////////////////////

//********************************** Logic operations ***********************************
//--------------------------------------- operator|(const CBM& bm1,const CBM& bm2)
STD(CBM) operator|(const CBM& bm1,const CBM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
 int i,j;
 CBM bm(bm1.m_nSize,bm1.m_nBitLength);
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nByteLength;j++)
     bm.m_pData[i][j] = bm1.m_pData[i][j] | bm2.m_pData[i][j];
 return bm;
}

//--------------------------------------- operator|(const CBM& bm1,const CBV& bv2)
STD(CBM) operator|(const CBM& bm1,const CBV& bv2)
{ASSERT(bm1.m_nBitLength == bv2.GetBitLength());
 int i,j;
 CBM bm(bm1.m_nSize,bm1.m_nBitLength);
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nByteLength;j++)
     bm.m_pData[i][j] = bm1.m_pData[i][j] | bv2.GetByteAt(j);
 return bm;
}

//--------------------------------------- operator|(const CBM& bm1,const BYTE *pb)
STD(CBM) operator|(const CBM& bm1, const BYTE *pb)
{
 int i,j;
 CBM bm(bm1.m_nSize,bm1.m_nBitLength);
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nByteLength;j++)
     bm.m_pData[i][j] = bm1.m_pData[i][j] | pb[j];
 return bm;
}

//------------------------------------------------------------------------- Disjunction()
CBV CBM::Disjunction()
{
#ifdef _DEBUG
  ASSERT_VALID(this); 
#endif //_DEBUG
 CBV bv(0,m_nBitLength,TRUE);
 int i,j;
 BYTE ch;
 for (i=0;i<m_nByteLength;i++)  {
   ch = 0;
   for (j=0;j<m_nSize;j++) ch |= m_pData[j][i];
   bv.SetByteAt(i,ch);
 }
 return bv;
}

//--------------------------------------- operator&(const CBM& bm1,const CBM& bm2)
STD(CBM) operator&(const CBM& bm1,const CBM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
 int i,j;
 CBM bm(bm1.m_nSize,bm1.m_nBitLength);
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nByteLength;j++)
     bm.m_pData[i][j] = bm1.m_pData[i][j] & bm2.m_pData[i][j];
 return bm;
}

//--------------------------------------- operator&(const CBM& bm1,const CBV& bv2)
STD(CBM) operator&(const CBM& bm1,const CBV& bv2)
{ASSERT(bm1.m_nBitLength == bv2.GetBitLength());
 int i,j;
 CBM bm(bm1.m_nSize,bm1.m_nBitLength);
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nByteLength;j++)
     bm.m_pData[i][j] = bm1.m_pData[i][j] & bv2.GetByteAt(j);
 return bm;
}

//--------------------------------------- operator&(const CBM& bm1, const BYTE *pb)
STD(CBM) operator&(const CBM& bm1, const BYTE *pb)
{ int i,j;
 CBM bm(bm1.m_nSize,bm1.m_nBitLength);
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nByteLength;j++)
     bm.m_pData[i][j] = bm1.m_pData[i][j] & pb[j];
 return bm;
}

//------------------------------------------------------------------------- Conjunction()
CBV CBM::Conjunction()
{
#ifdef _DEBUG
  ASSERT_VALID(this); 
#endif //_DEBUG
 CBV bv(0,m_nBitLength,TRUE);
 int i,j;
 BYTE ch;
 for (i=0;i<m_nByteLength;i++)  {
   ch = 0xFF;
   for (j=0;j<m_nSize;j++) ch &= m_pData[j][i];
   bv.SetByteAt(i,ch);
 }
 return bv;
}

//--------------------------------------- operator^(const CBM& bm1,const CBM& bm2)
STD(CBM) operator^(const CBM& bm1,const CBM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
 int i,j;
 CBM bm(bm1.m_nSize,bm1.m_nBitLength);
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nByteLength;j++)
     bm.m_pData[i][j] = bm1.m_pData[i][j] ^ bm2.m_pData[i][j];
 return bm;
}

//--------------------------------------- operator^(const CBM& bm1,const CBV& bv2)
STD(CBM) operator^(const CBM& bm1,const CBV& bv2)
{ASSERT(bm1.m_nBitLength == bv2.GetBitLength());
 int i,j;
 CBM bm(bm1.m_nSize,bm1.m_nBitLength);
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nByteLength;j++)
     bm.m_pData[i][j] = bm1.m_pData[i][j] ^ bv2.GetByteAt(j);
 return bm;
}

//--------------------------------------- operator^(const CBM& bm1,const BYTE *pb)
STD(CBM) operator^(const CBM& bm1, const BYTE *pb)
{ int i,j;
 CBM bm(bm1.m_nSize,bm1.m_nBitLength);
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nByteLength;j++)
     bm.m_pData[i][j] = bm1.m_pData[i][j] ^ pb[j];
 return bm;
}

//------------------------------------------------------------------------- BitwiseExclusive()
CBV CBM::BitwiseExclusive()
{
#ifdef _DEBUG
  ASSERT_VALID(this); 
#endif //_DEBUG
 CBV bv(0,m_nBitLength,TRUE);
 int i,j;
 BYTE ch;
 for (i=0;i<m_nByteLength;i++)  {
   ch = 0;
   for (j=0;j<m_nSize;j++) ch ^= m_pData[j][i];
   bv.SetByteAt(i,ch);
 }
 return bv;
}

//--------------------------------------- operator-(const CBM& bm1,const CBV& bv2)
STD(CBM) operator-(const CBM& bm1,const CBV& bv2)
{ASSERT(bm1.m_nBitLength == bv2.GetBitLength());
 int i,j;
 CBM bm(bm1.m_nSize,bm1.m_nBitLength);
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nByteLength;j++)
     bm.m_pData[i][j] = bm1.m_pData[i][j] & ~bv2.GetByteAt(j);  //?????? 
 return bm;
}

//--------------------------------------- operator-(const CBM& bm1, const BYTE *pb)
STD(CBM) operator-(const CBM& bm1, const BYTE *pb)
{ int i,j;
 CBM bm(bm1.m_nSize,bm1.m_nBitLength);
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nByteLength;j++)
     bm.m_pData[i][j] = bm1.m_pData[i][j] & ~pb[j];  //?????? 
 return bm;
}

//------------------------------------------------------- operator~(const CBM& bm)
STD(CBM) operator~(const CBM& bm)
{
#ifdef _DEBUG
  ASSERT_VALID(&bm); 
#endif //_DEBUG
 int i,j; 
 CBM bm1(bm.m_nSize,bm.m_nBitLength);
 for (i=0;i<bm1.m_nSize;i++)   {
   for (j=0;j<bm1.m_nByteLength;j++)
     bm1.m_pData[i][j] = ~bm.m_pData[i][j];
   if ((j = ADR_BIT(bm1.m_nBitLength)) > 0 ) { //new - 16.12
     j = S_1 - j;        
     bm1.m_pData[i][bm1.m_nByteLength-1] = (bm1.m_pData[i][bm1.m_nByteLength-1] >> j) << j;  
   }
 }    
 return bm1;                                   //new - 16.12
}

/////////////////////////////////////////////////////////////////////////////////////////


//******************** Operations of weighting, finding and casing **********************
//-------------------------------------------------------------------- CountBit(int nRow)
int CBM::CountBit(int nRow) const
{ int j,k,one=0,first,last;
 ASSERT (nRow>=-1);
 if (nRow==-1) { first=0; last=m_nSize-1; }
 else { first=nRow; last=nRow; }
 for (k=first; k<=last; k++)
   for (j=0; j<m_nByteLength; j++) one+=TabC[m_pData[k][j]];
 return one;
}

//-------------------------------------------------------------------- CountBit(int nRow)
int CBM::CountBit(const BYTE* mask,int nRow) const 
{ int j,k,one=0,first,last;
 ASSERT (nRow>=-1);
 if (nRow==-1) { first=0; last=m_nSize-1; }
 else { first=nRow; last=nRow; }
 for (k=first; k<=last; k++)
   for (j=0; j<m_nByteLength; j++)  one+=TabC[m_pData[k][j]&mask[j]];
 return one;
}

//---------------------------------------------------------- LeftOne(int nRow, int nNext)
int CBM::LeftOne(int nRow, int nNext) const
{int i,j,pos = 0;
 BYTE ch;
 ASSERT((m_nBitLength - nNext) >= 0);
 if (nNext == -1) {i=0;j=0;}
 else {i=BIT_BYTE(nNext);  j=ADR_BIT(nNext); j++;}
 if (j==S_1)  {j=0;  i++; }
 if (i == m_nByteLength)  return (-1);
 ch = m_pData[nRow][i] << j;
 if (ch)  {
   while (!(ch & OB[pos++]));
   return (nNext + pos);
 }
 for (j=i+1;j<m_nByteLength;j++)
   if (m_pData[nRow][j])  {
     while (!(m_pData[nRow][j] & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 return (-1);
}

//----------------------------------------------------------- LeftOne(int nRow, BYTE& bt)
int CBM::LeftOne(int nRow, BYTE& bt) const
{int i,pos = 0;
 for (i=0;i<m_nByteLength;i++)
   if (m_pData[nRow][i]) {
     while (!(bt=(m_pData[nRow][i] & OB[pos++])));
     return (i);
   }
 return(-1);
}

//----------------------------------------------- LeftOne(int nRow, int nNext,BYTE* mask)
int CBM::LeftOne(int nRow, int nNext, BYTE* mask) const
{int i,j,pos = 0;
 BYTE ch;
 ASSERT((m_nBitLength - nNext) >= 0);
 if (nNext == -1) {i=0;j=0;}
 else {i=BIT_BYTE(nNext);  j=ADR_BIT(nNext); j++;}
 if (j==S_1)  {j=0;  i++; }
 if (i == m_nByteLength)  return (-1);
 ch = (m_pData[nRow][i] & mask[i]) << j;
 if (ch)  {
   while (!(ch & OB[pos++]));
   return (nNext + pos);
 }
 for (j=i+1;j<m_nByteLength;j++)
   if (m_pData[nRow][j] & mask[j])  {
     while (!(m_pData[nRow][j] & mask[j] & OB[pos++]));
     return ((j*S_1)+pos-1);
   }
 return (-1);
}

//------------------------------------------------------------------- RightOne(int nPrev)
int CBM::RightOne(int nRow, int nPrev) const
{int i,j,pos = 7;
 BYTE ch;
 ASSERT((m_nBitLength - nPrev) >= 0);
 if (nPrev == -1) {i=m_nByteLength-1;j=0;}
 else {i=BIT_BYTE(nPrev);  j=S_1 - ADR_BIT(nPrev);}
 if (j==8)  {j=0;  i--; }
 ch = (m_pData[nRow][i] >> j) << j;
 if (ch)  {
   while (!(ch & OB[pos--]));
   return ((i*S_1)+pos+1);
 }
 for (j=i-1;j>=0;j--)
   if(m_pData[nRow][j])  {
     while (!(m_pData[nRow][j] & OB[pos--]));
     return ((j*S_1)+pos+1);
   }
 return (-1);
}

//-------------------------------------------------------------------- RightOne(BYTE& bt)
int CBM::RightOne(int nRow, BYTE& bt) const
{int i,pos = 7;
 for (i=m_nByteLength-1;i>=0;i--)
   if (m_pData[nRow][i])  {
     while (!(bt=(m_pData[nRow][i] & OB[pos--])));
     return (i);
   }
 return (-1);
}

//------------------------------------------------------------------- RightOne(int nPrev)
int CBM::RightOne(int nRow, int nPrev,BYTE* mask) const
{int i,j,pos = 7;
 BYTE ch;
 ASSERT((m_nBitLength - nPrev) >= 0);
 if (nPrev == -1) {i=m_nByteLength-1;j=0;}
 else {i=BIT_BYTE(nPrev);  j=S_1 - ADR_BIT(nPrev);}
 if (j==8)  {j=0;  i--; }
 ch = ((m_pData[nRow][i] & mask[i]) >> j) << j;
 if (ch)  {
   while (!(ch & OB[pos--]));
   return ((i*S_1)+pos+1);
 }
 for (j=i-1;j>=0;j--)
   if(m_pData[nRow][j] & mask[j])  {
     while (!(m_pData[nRow][j] & mask[j] & OB[pos--]));
     return ((j*S_1)+pos+1);
   }
 return (-1);
}

//------------------------------------------------------------------- FindRow (BOOL Empty, int nFRow)
int CBM::FindRow (BOOL Empty, int nFRow) const
{ int i,j;
  BYTE work; 
  
 if (m_nSize == 0) return -1;       //new 24.01.00
 ASSERT(nFRow < m_nSize-1);
 nFRow++; 
 for (i=nFRow; i<=m_nSize-1; i++)  {
   work=0;
   for (j=0;j<m_nByteLength;j++)   work |= m_pData[i][j];
   if (work == 0 && Empty == 1) return i;
   if (work != 0 && Empty == 0) return i;
 }   
 return -1;
}                               

//------------------------------------------------- FindRow (BOOL Empty, BYTE* mask, int nFRow)
int CBM::FindRow (BOOL Empty, BYTE* mask, int nFRow) const
{ int i,j;
  BYTE work; 
 if (m_nSize == 0) return -1;       //new 24.01.00  
 ASSERT(nFRow < m_nSize-1);
 nFRow++; 
 for (i=nFRow; i<=m_nSize-1; i++)  {
   work=0;
   for (j=0;j<m_nByteLength;j++)   work |= m_pData[i][j] & mask[j];
   if (work == 0 && Empty == 1) return i;
   if (work != 0 && Empty == 0) return i;
 }   
 return -1;
}                               

//------------------------------------------------- FindRow (const CBV& bv, int nFRow)
int CBM::FindRow (const CBV& bv, int nFRow) const
{ int i;
 if (m_nSize == 0) return -1;       //new 24.01.00    
 ASSERT(nFRow < m_nSize-1);
 nFRow++; 
 for (i=nFRow; i<m_nSize; i++)  
   if (memcmp(bv, m_pData[i],m_nByteLength)==0) return i;  
 return -1;
}                               

//------------------------------------------------- FindRow (const BYTE* pt, int nFRow)
int CBM::FindRow (const BYTE* pt, int nFRow) const
{ int i;
 if (m_nSize == 0) return -1;       //new 24.01.00    
 ASSERT(nFRow < m_nSize-1);
 nFRow++; 
 for (i=nFRow; i<m_nSize; i++)  
    if (memcmp(pt, m_pData[i],m_nByteLength)==0) return i;  
 return -1;
}                               

/////////////////////////////////////////////////////////////////////////////////////////


//******************************* Compareing operations *********************************
//-----------------------------------------------------------IsZero(int nRow)
BOOL CBM::IsZero(int nRow) const
{int i,k,first,last;
 ASSERT (nRow>=-1);
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 if (nRow==-1) { first=0; last=m_nSize-1; }
 else { first=nRow; last=nRow; }
 for (k=first; k<=last; k++)
   for (i=0;i<m_nByteLength;i++)
     if (m_pData[k][i] != 0) return FALSE;
 return TRUE;
}

//-----------------------------------------------------------IsZero(int nRow)
BOOL CBM::IsZero(const BYTE* mask,int nRow) const
{int i,k,first,last;
 ASSERT (nRow>=-1);
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 if (nRow==-1) { first=0; last=m_nSize-1; }
 else { first=nRow; last=nRow; }
 for (k=first; k<=last; k++)
   for (i=0;i<m_nByteLength;i++)
     if ((m_pData[k][i]& mask[i]) != 0) return FALSE;
 return TRUE;
}

//-----------------------------------------------------------IsOne(int nRow)
BOOL CBM::IsOne(int nRow) const
{int i,k,first,last;
 ASSERT (nRow>=-1);
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 BYTE b1,b2;                                 //New 29.09.98
 if (nRow==-1) { first=0; last=m_nSize-1; }
 else { first=nRow; last=nRow; }
 for (k=first; k<=last; k++) {
   for (i=0;i<m_nByteLength-1;i++)
     if (m_pData[k][i] != 0xFF) return FALSE;
   b1 = ~(m_pData[k][m_nByteLength-1])+1;     //New 29.09.98
   b2 = OB[ADR_BIT(m_nBitLength-1)];          //New 29.09.98
//   if ((~(m_pData[k][m_nByteLength-1])+1) != OB[ADR_BIT(m_nBitLength)]) 
   if (b1 != b2)       return FALSE;   
 }
 return TRUE;
}

//-----------------------------------------------------------IsOne(int nRow)
BOOL CBM::IsOne(const BYTE* mask,int nRow) const
{int i,k,first,last;
 ASSERT (nRow>=-1);
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 BYTE b1,b2;                                 //New 29.09.98
 if (nRow==-1) { first=0; last=m_nSize-1; }
 else { first=nRow; last=nRow; }
 for (k=first; k<=last; k++) {
   for (i=0;i<m_nByteLength-1;i++)
     if ((m_pData[k][i] & mask[i]) != 0xFF) return FALSE;
   b1 = ~(m_pData[k][m_nByteLength-1])&mask[m_nByteLength-1]+1;   //New 29.09.98
   b2 = OB[ADR_BIT(m_nBitLength-1)];                              //New 29.09.98
   if (b1 != b2)       return FALSE;   
//   if ((~((m_pData[k][m_nByteLength-1]&mask[m_nByteLength-1])+1)) != 
//           OB[ADR_BIT(m_nBitLength)]) return FALSE;
 }
 return TRUE;
}


//-------------------------------------- operator==(const CBM& bm1,const CBM& bm2)
STD(BOOL) operator==(const CBM& bm1,const CBM& bm2)
{if ((bm1.m_nSize != bm2.m_nSize) || (bm1.m_nBitLength != bm2.m_nBitLength)) return FALSE;
 int i,j;
  for (i=0;i<bm1.m_nSize;i++) 
   for (j=0;j<bm1.m_nByteLength;j++)       // New 16.11.98
     if (bm1.m_pData[i][j] != bm2.m_pData[i][j]) return FALSE;
 return TRUE;
}

//-------------------------------------- operator!=(const CBM& bm1,const CBM& bm2)
STD(BOOL) operator!=(const CBM& bm1,const CBM& bm2)
{if ((bm1.m_nSize != bm2.m_nSize) || (bm1.m_nBitLength != bm2.m_nBitLength)) return TRUE;
 int i,j;
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nByteLength;j++)            // New 16.11.98
     if (bm1.m_pData[i][j] != bm2.m_pData[i][j]) return TRUE;
 return FALSE;
}

//--------------------------------------- operator>(const CBM& bm1,const CBM& bm2)
STD(BOOL) operator>(const CBM& bm1,const CBM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
 int i,j;
 for (i=0;i<bm1.m_nSize;i++) {
   for (j=0;j<bm1.m_nByteLength;j++)      // New 16.11.98
     if (bm2.m_pData[i][j] != (bm1.m_pData[i][j] & bm2.m_pData[i][j])) return FALSE;
   for (j=0;j<bm1.m_nByteLength;j++)            // New 16.11.98
     if (bm2.m_pData[i][j] != bm1.m_pData[i][j]) return TRUE;
 }
 return FALSE;
}

//--------------------------------------- operator<(const CBM& bm1,const CBM& bm2)
STD(BOOL) operator<(const CBM& bm1,const CBM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
 int i,j;
 for (i=0;i<bm1.m_nSize;i++) {
   for (j=0;j<bm1.m_nByteLength;j++)             // New 16.11.98
     if (bm1.m_pData[i][j] != (bm2.m_pData[i][j] & bm1.m_pData[i][j])) return FALSE;
   for (j=0;j<bm1.m_nByteLength;j++)             // New 16.11.98
     if (bm2.m_pData[i][j] != bm1.m_pData[i][j]) return TRUE;
 }
 return FALSE;
}

//-------------------------------------- operator>=(const CBM& bm1,const CBM& bm2)
STD(BOOL) operator>=(const CBM& bm1,const CBM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
 int i,j;
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nByteLength;j++)             // New 16.11.98
     if (bm2.m_pData[i][j] != (bm1.m_pData[i][j] & bm2.m_pData[i][j])) return FALSE;
 return TRUE;
}

//-------------------------------------- operator<=(const CBM& bm1,const CBM& bm2)
STD(BOOL) operator<=(const CBM& bm1,const CBM& bm2)
{ASSERT(bm1.m_nSize == bm2.m_nSize);
 ASSERT(bm1.m_nBitLength == bm2.m_nBitLength);
 int i,j;
 for (i=0;i<bm1.m_nSize;i++)
   for (j=0;j<bm1.m_nByteLength;j++)             // New 16.11.98
     if (bm1.m_pData[i][j] != (bm2.m_pData[i][j] & bm1.m_pData[i][j])) return FALSE;
 return TRUE;
}

//--------------------------------------------------CoverRow(int nRow1, int nRow2)
int CBM::CoverRow(int nRow1, int nRow2)
{
  int pi,pj,k;
  BYTE work,worki,workj;
  
  for (k=pi=pj=0; k<m_nByteLength; k++) { 
    worki = GetByteAt(nRow1,k); workj = GetByteAt(nRow2,k);
    work = worki & workj;
    if (work == worki) pi++;
    if (work == workj) pj++;
  }
  if (pi==m_nByteLength) return 1;       /* i-а€ строка поглощаетс€ j-ой */
  if (pj==m_nByteLength) return -1;      /* i-а€ строка поглощает j-ую */
  return 0;                     
}

//---------------------------------CoverRow(int nRow1, int nRow2, const BYTE * mask)
int CBM::CoverRow(int nRow1, int nRow2, const BYTE * mask)
{
  int pi,pj,k;
  BYTE work,worki,workj;
  
  for (k=pi=pj=0; k<m_nByteLength; k++) { 
    worki = GetByteAt(nRow1,k) & mask[k]; workj = GetByteAt(nRow2,k)&mask[k];
    work = worki & workj;
    if (work == worki) pi++;
    if (work == workj) pj++;
  }
  if (pi==m_nByteLength) return 1;       /* i-а€ строка поглощаетс€ j-ой */
  if (pj==m_nByteLength) return -1;      /* i-а€ строка поглощает j-ую */
  return 0;                     
}

/////////////////////////////////////////////////////////////////////////////////////////


//#ifndef _LINUX

///////////////////////////////////////////////////////////////////////////////
//// Serialization

////---------------------------------------------------------------Serialize(CArchive& ar)
//void CBM::Serialize(CArchive& ar)
//{
//#ifdef _DEBUG
//  ASSERT_VALID(this);
//#endif //_DEBUG
//  int i,j;
  
//  if (ar.IsStoring()) {
//    ar << m_nSize;
//    ar << m_nBitLength;
//    ar << m_nByteLength;
//    for (i=0; i<m_nSize; i++)
//      for (j=0; j<m_nByteLength; j++)
//        ar << m_pData[i][j];
//  }
//  else {
//    ar >> m_nSize;
//    ar >> m_nBitLength;
//    ar >> m_nByteLength;
//    SetSize(m_nSize, m_nBitLength,m_nGrowBy);
//    for (i=0; i<m_nSize; i++)
//      for (j=0; j<m_nByteLength; j++)
//        ar >> m_pData[i][j];
//  }
//}

///////////////////////////////////////////////////////////////////////////////
//// Diagnostics

////-----------------------------------------------------------Dump(CDumpContext& dc) const
//#ifdef _DEBUG
//void CBM::Dump(CDumpContext& dc) const
//{
//   ASSERT_VALID(this);
//#define MAKESTRING(x) #x
//    AFX_DUMP1(dc, "a " MAKESTRING(CBM) " with ", m_nSize);
//    AFX_DUMP0(dc, " elements");
//#undef MAKESTRING
//    if (dc.GetDepth() > 0) {
//      AFX_DUMP0(dc, "\n");
//      for (int i = 0; i < m_nSize; i++) {
//        AFX_DUMP1(dc, "\n\t[", i);
//        AFX_DUMP1(dc, "] = ",NULL);
//      }
//    }
//}

////-----------------------------------------------------------AssertValid() const
//void CBM::AssertValid() const
//{
//  if (m_pData == NULL) {
//    ASSERT(m_nSize >= 0);
//    ASSERT(m_nMaxSize >= 0);
//    ASSERT(m_nBitLength >= 0);
//    ASSERT(m_nByteLength >= 0);
//    ASSERT(m_nAllocLength >= 0);
//  }
//  else {
//    ASSERT(m_nSize >= 0);
//    ASSERT(m_nMaxSize >= 0);
//    ASSERT(m_nSize <= m_nMaxSize);
//    ASSERT(AfxIsValidAddress(m_pData, m_nMaxSize * sizeof(BYTE*)));
//    ASSERT(m_nBitLength >= 0);
//    ASSERT(m_nByteLength == LEN_BYTE(m_nBitLength));
//    ASSERT(m_nAllocLength >= 0);
//    ASSERT(m_nGrowBy > 0);
//  }
//}
//#endif //_DEBUG



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////// Operation of weighting,finding and casing ///////////////////////
////----------------------------------------------------------------------- CharBit(CString s)
//void CBM::CharBit(const CString s)
//{int j = 0,i = 0,max = 0;
// CString w = s;
// if (w.GetLength() <= 2) { Init(); return;}
// if (w[w.GetLength()-2] != '\r') w = w + "\r\n";
// while (!w.IsEmpty()) {
//   j = w.Find('\n');
//   if (j > max + 1) max = j - 1;
//   w = w.Right(w.GetLength() - j-1);
//   i++;
// }
// if (m_pData == NULL) AllocMatrix(i,max);
// else { SetSize(i,max,m_nGrowBy); Zero(); }
// w = s; j = 0;
// if (w[w.GetLength()-2] != '\r') w = w + "\r\n";
// while (!w.IsEmpty()) {
//   max = w.Find('\n');
//   for(i=0;i<max-1;i++)
//     if (w[i] == '1') SetBitAt(j,i,TRUE);
//     else             ASSERT(w[i] == '0'|| w[i] == '.' || w[i] == '-');    //new 11.02
//   w = w.Right(w.GetLength() - max-1);
//   j++;
// }
//}

////----------------------------------------------------------------------- CharBit(CStringArray s)
//void CBM::CharBit(const CStringArray& s)     //new 11.02
//{int k,i,max = 0;
// if (s.GetSize()==0) { Init(); return;}
 
// for (k=0; k<s.GetSize(); k++) {
//   i = s[k].GetLength();
//   if (i > max) max = i;    // max - length of row
// }
// if (m_pData == NULL) AllocMatrix(s.GetSize(),max);
// else { SetSize(s.GetSize(),max,m_nGrowBy); Zero(); }
// for (k=0; k<s.GetSize(); k++) {
//   max = s[k].GetLength();
//   CString w(s[k]);
//   for(i=0;i<max;i++)
//     if (w[i] == '1') SetBitAt(k,i,TRUE);
//     else             ASSERT(w[i] == '0' || w[i] == '.' || w[i] == '-');
// }
//}

//#endif

//----------------------------------------------------------------------- CharBit(CString s)
void CBM::CharBit(const string s)
{int j = 0,i = 0,max = 0;
  string w = s;
  if (w.length() <= 2) { Init(); return;}
  if (w[w.length()-2] != '\r') w = w + "\r\n";
  while (!w.empty()) {
    j = w.find('\n');
    if (j > max + 1) max = j - 1;
    w.erase(0,j+1);    //w =     Right(w.GetLength() - j-1);
    i++;
  }
  if (m_pData == NULL) AllocMatrix(i,max);
  else { SetSize(i,max,m_nGrowBy); Zero(); }
  w = s; j = 0;
  if (w[w.length()-2] != '\r') w = w + "\r\n";
  while (!w.empty()) {
    max = w.find('\n');
    for(i=0;i<max-1;i++)
      if (w[i] == '1') SetBitAt(j,i,TRUE);
      else             ASSERT(w[i] == '0'|| w[i] == '.' || w[i] == '-');    //new 11.02
    w.erase(0, max+1); // w =    Right(w.GetLength() - max-1);
    j++;
  }
}

//----------------------------------------------------------------------- CharBit(CStringArray s)
void CBM::CharBit(const vector <string>& s)     //new 11.02
{ int k,i,max = 0;
  if (s.size()==0) { Init(); return;}

  for (k=0; k<(int)s.size(); k++) {
  i = s[k].length();
  if (i > max) max = i;    // max - length of row
  }  
  if (m_pData == NULL) AllocMatrix(s.size(),max);
  else { SetSize(s.size(),max,m_nGrowBy); Zero(); }
  for (k=0; k<(int)s.size(); k++) {
    max = s[k].length();
    string w = s[k];
    for(i=0;i<max;i++)
      if (w[i] == '1') SetBitAt(k,i,TRUE);
      else             ASSERT(w[i] == '0' || w[i] == '.' || w[i] == '-');
  }
}



//---------------------------------------------------------------Serialize(CArchive& ar)
void CBM::Serialize(CArch& ar)
{ 
  int i,j;

  if (ar.IsStoring()) {
    ar << m_nSize;
    ar << m_nBitLength;
    ar << m_nByteLength; 
    for (i=0; i<m_nSize; i++)  
      for (j=0; j<m_nByteLength; j++)         
        ar << m_pData[i][j];
  }
  else {
    ar >> m_nSize;
    ar >> m_nBitLength;
    ar >> m_nByteLength;
    SetSize(m_nSize, m_nBitLength,m_nGrowBy);
    for (i=0; i<m_nSize; i++)  
      for (j=0; j<m_nByteLength; j++)         
        ar >> m_pData[i][j];
  }
}


//----------------------------------------------------------------------- CharBit(CString s)
void CBM::CharBit(const char* s)
{
  int j=0, i=0, max=0, len;
  char *w, *pdest;
  len = strlen(s);
  w =(char*)malloc(len+2);
#ifndef _MSVC9
  strcpy(w,s);
#else
  strcpy_s(w, len+2, s);
#endif
  if (len<=2) { Init(); return;}
  if (w[len-2] != '\r') 
#ifndef _MSVC9
	  strcat(w,"r\n");
#else
	  strcat_s(w, len+2, "\r\n");
#endif
  while (w[0] !='\0') {
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
  else { SetSize(i,max,m_nGrowBy); Zero(); }
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
    for(i=0;i<max-1;i++)
      if (w[i] == '1') SetBitAt(j,i,TRUE);
      else             ASSERT(w[i] == '0'|| w[i] == '.' || w[i] == '-');    //new 11.02
#ifndef _MSVC9
    strcpy(w, pdest+1);
#else
    strcpy_s(w, len+2, pdest+1);
#endif
    j++;
  }
  free(w);
}



//**************************************************************************************

//-------------------------------------------------------------------
void CBM::SetRowDiz(int nRow, const BYTE* v1)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] |= v1[i];
}

//-------------------------------------------------------------------
void CBM::SetRowDiz(int nRow, const BYTE* v1, const BYTE* v2)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] = v1[i] | v2[i];
}

//-------------------------------------------------------------------
void CBM::SetRowDiz(const BYTE* mask, int nRow, const BYTE* v1)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] |= v1[i] & mask[i];
}

//-------------------------------------------------------------------
void CBM::SetRowDiz(const BYTE* mask, int nRow, const BYTE* v1, const BYTE* v2)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] = (v1[i] | v2[i]) & mask[i];
}

//-------------------------------------------------------------------
void CBM::SetRowDiz(int nRow, int Num, BYTE* v1, ...)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);

  BYTE * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, BYTE* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nByteLength; i++) {
    m_pData[nRow][i] = v[0][i];
    for (int j=1; j< Num; j++)     m_pData[nRow][i] |= v[j][i];     
  }
}

//-------------------------------------------------------------------
void CBM::SetRowDiz(const BYTE* mask, int nRow, int Num, BYTE* v1, ...)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);

  BYTE * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, BYTE* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nByteLength; i++) {
    m_pData[nRow][i] = v[0][i];
    for (int j=1; j< Num; j++)  m_pData[nRow][i] |= v[j][i];     
    m_pData[nRow][i] &= mask[i];
  }
}


//-------------------------------------------------------------------
void CBM::SetRowCon(int nRow, const BYTE* v1)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] &= v1[i];
}

//-------------------------------------------------------------------
void CBM::SetRowCon(int nRow, const BYTE* v1, const BYTE* v2)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] = v1[i] & v2[i];
}

//-------------------------------------------------------------------
void CBM::SetRowCon(const BYTE* mask, int nRow, const BYTE* v1)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] &= v1[i] & mask[i];
}

//-------------------------------------------------------------------
void CBM::SetRowCon(const BYTE* mask, int nRow, const BYTE* v1, const BYTE* v2)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] = v1[i] & v2[i] & mask[i];
}

//-------------------------------------------------------------------
void CBM::SetRowCon(int nRow, int Num, BYTE* v1, ...)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);

  BYTE * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, BYTE* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nByteLength; i++) {
    m_pData[nRow][i] = v[0][i];
    for (int j=1; j< Num; j++)    m_pData[nRow][i] &= v[j][i];     
  }
}

//-------------------------------------------------------------------
void CBM::SetRowCon(const BYTE* mask, int nRow, int Num, BYTE* v1, ...)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);

  BYTE * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, BYTE* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nByteLength; i++) {
    m_pData[nRow][i] = v[0][i] & mask[i];
    for (int j=1; j< Num; j++) m_pData[nRow][i] &= v[j][i];     
  }
}


//-------------------------------------------------------------------
void CBM::SetRowXor(int nRow, const BYTE* v1)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] ^= v1[i];
}

//-------------------------------------------------------------------
void CBM::SetRowXor(int nRow, const BYTE* v1, const BYTE* v2)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] = v1[i] ^ v2[i];
}

//-------------------------------------------------------------------
void CBM::SetRowXor(const BYTE* mask, int nRow, const BYTE* v1)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] ^= v1[i] & mask[i];
}

//-------------------------------------------------------------------
void CBM::SetRowXor(const BYTE* mask, int nRow, const BYTE* v1, const BYTE* v2)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] = (v1[i] ^ v2[i]) & mask[i];
}
//-------------------------------------------------------------------
void CBM::SetRowXor(int nRow, int Num, BYTE* v1, ...)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);

  BYTE * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, BYTE* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nByteLength; i++) {
    m_pData[nRow][i] = v[0][i];
    for (int j=1; j< Num; j++)    m_pData[nRow][i] ^= v[j][i];
  }
}

//-------------------------------------------------------------------
void CBM::SetRowXor(const BYTE* mask, int nRow, int Num, BYTE* v1, ...)
{
  ASSERT(nRow >= 0);  ASSERT(nRow < m_nSize);

  BYTE * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, BYTE* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nByteLength; i++) {
    m_pData[nRow][i] = v[0][i];
    for (int j=1; j< Num; j++) m_pData[nRow][i] &= v[j][i];   
    m_pData[nRow][i] &= mask[i];
  }
}

//-------------------------------------------------------------------
void CBM::SetRowDif(int nRow, const BYTE* v1)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] &= ~v1[i];
}

//-------------------------------------------------------------------
void CBM::SetRowDif(int nRow, const BYTE* v1, const BYTE* v2)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] = v1[i] & ~v2[i];
}

//-------------------------------------------------------------------
void CBM::SetRowDif(const BYTE* mask, int nRow, const BYTE* v1)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] &= ~(v1[i] & mask[i]);
}

//-------------------------------------------------------------------
void CBM::SetRowDif(const BYTE* mask, int nRow, const BYTE* v1, const BYTE* v2)
{
  ASSERT(nRow >= 0); ASSERT(nRow < m_nSize);
  for (int i=0; i<m_nByteLength; i++) m_pData[nRow][i] = (v1[i] & ~v2[i]) & mask[i];
}



