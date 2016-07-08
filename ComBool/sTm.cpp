/////////////////////////////////////////////////////////////////
// Ver.2.1.4     22.01.2008
//               Добавлены функции конвертации данных базового класса троичных векторов и матриц
//               а также функции DorD для класса троичных матриц
// Ver.2.1.3     22.02.2007
//               В функции Add можно не указать второго операнда
//               и тогда к текущей добавятся все строки матрицы
// Ver.2.1.2     10.07.2006
//               поправка AreConsens
// Ver.2.1.1     08.07.2006
//               поправка GetRowTv
// Ver.2.1.0     07.07.2006
//               Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.1.1.0    24.06.2006   Russian == English
/////////////////////////////////////////////////////////////////
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

#include <limits.h>
#define SIZE_T_MAX  UINT_MAX            /* max size for a size_t */
/*
#ifndef _LINUX

#include "afxwin.h"        
IMPLEMENT_SERIAL(CsTM, CObject, 0)
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#define new DEBUG_NEW
#else
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#undef _DEBUG

//#endif

//******************************* Constructors\Destructor *********************
//----------------- CsTM() -----------------------------------------------------
CsTM::CsTM() { Init(); }

//----------------- CsTM(const CsTM& tm, BOOL Fl/* = TRUE */) -------------------
CsTM::CsTM(const CsTM& tm, BOOL Fl/* = TRUE */)
{
  int i,j;
  if(tm.m_nSize == 0) {
    m_nSize=m_nMaxSize=0;
    m_nBitLength=tm.m_nBitLength; m_nGrowBy = tm.m_nGrowBy; m_pData = NULL;
    return;
  }  
  if(tm.m_nBitLength == 0) { Init(); return; }
  if (Fl || tm.m_nSize>32)  { //copy tm in our matrix
    AllocMatrix(tm.m_nSize,tm.m_nBitLength);
    memcpy(m_pData,tm.m_pData,tm.m_nSize*sizeof(ULONG));
    memcpy(m_pData+m_nMaxSize,tm.m_pData+tm.m_nMaxSize,tm.m_nSize*sizeof(ULONG));
  }
  else {    //copy tm in our matrix after transponir
    AllocMatrix(tm.m_nBitLength,tm.m_nSize);
    for (j=0; j < m_nBitLength; j++)
      for (i=0; i < m_nSize; i++) SetBitAt(i,j,tm.GetBitAt(j,i));
  }
  m_nGrowBy = tm.m_nGrowBy;
}

//----------------- CsTM(const CBM& bm1,const CBM& bm2,BOOL Fl) ----------------
CsTM::CsTM(const CsBM& bm1,const CsBM& bm2,BOOL Fl/* = TRUE */)
{
  int i,j;
  BOOL a1, a0;
  ASSERT(bm1.GetCountR() == bm2.GetCountR() &&
         bm1.GetCountC() == bm2.GetCountC());

  if(bm1.GetCountR() == 0) {
    m_nSize = m_nMaxSize = 0; 
    m_nBitLength = bm1.GetCountC(); m_nGrowBy = 10; m_pData = NULL;
    return;
  }  
  if(bm1.GetCountC() == 0) { Init(); return; }
  if (Fl || bm1.GetCountR()>32)  { //copy tm in our matrix
    AllocMatrix(bm1.GetCountR(),bm1.GetCountC());
    for (i=0;i < m_nSize;i++) {
      m_pData[i] = bm1.GetRow(i);
      m_pData[i+m_nMaxSize] = bm2.GetRow(i);
    }
  }
  else {    //copy tm in our matrix after transponir
    AllocMatrix(bm1.GetCountC(),bm1.GetCountR());
    for (j=0;j < m_nBitLength;j++)
      for (i=0;i < m_nSize;i++) {
        a1=bm1.GetBitAt(j,i); a0=bm2.GetBitAt(j,i);
        if(a1 && !a0) SetBitAt(i,j,'1');
        if(!a1 && a0) SetBitAt(i,j,'0');
        if(a1 && a0) SetBitAt(i,j,'+');
      }  
  } 
   
  m_nGrowBy = 10;  
}

//----------------- CsTM(int nRow,int nColumn,char symb='-') -------------------
CsTM::CsTM(int nRow, int nColumn,char symb/*='-'*/)
{ 
  ASSERT(nRow >= 0 && nColumn >= 0);
  int i;
  if (nRow == 0) {
    if ( nColumn == 0) Init();
    else {
      m_nSize=m_nMaxSize=0;
      m_nBitLength=nColumn; m_nGrowBy = 10; m_pData = NULL;
    }
    return;
  }
  if ( nColumn == 0) { Init(); return; }
  AllocMatrix(nRow,nColumn);
  if(symb == '1' || symb == '+') 
    for (i=0; i < nRow; i++) {
      m_pData[i] = 0xffffffff >> (32 - m_nBitLength) << (32 - m_nBitLength);
    }
  if(symb == '0' || symb == '+') 
    for (i=0; i < nRow; i++) {
      m_pData[i+m_nMaxSize]= 0xffffffff >> (32 - m_nBitLength) << (32 - m_nBitLength);
    }
  m_nGrowBy = 10;  
}
   
//----------------- CsTM(const char* pch) --------------------------------------
CsTM::CsTM(const char* pch)
{
/*
#ifndef _LINUX
	CString s(pch);
  Init();
  if (s.GetLength()) CharBit(s);
#else
*/
  char * s;
  s = strdup(pch);
  Init();
  if (s[0]!=0) CharBit(s);
  free(s);	
//#endif
}
/*
#ifndef _LINUX
//----------------- CsTM(CStringArray StrAr) -----------------------------------
CsTM::CsTM(const CStringArray& StrAr)
{ Init(); CharBit(StrAr); }
#else
*/
CsTM::CsTM(const vector <string>& StrAr)
{ Init(); CharBit(StrAr); }
//#endif

//----------------- ~CsTM() ----------------------------------------------------
CsTM::~CsTM()
{ 
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif

  SafeDelete(); }

///////////////////////////////////////////////////////////////////////////////
//***************** Working with memory ***************************************
//----------------- SetSize ---------------------------------------------------
void CsTM::SetSize(int nRow,int nColumn,int nGrowBy/*=-1*/)
{ 
  int i;
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(nRow >= 0 && nColumn >= 0);
  if (nGrowBy != -1) m_nGrowBy = nGrowBy;  // set new size
  
  if (nRow == 0) { 
    SafeDelete(); m_nBitLength = nColumn; m_nSize = m_nMaxSize = 0; return;
  }
 //------------------ create one with exact size
  if (m_pData == NULL) { // create one with exact size
#ifdef SIZE_T_MAX
    ASSERT((long)(nRow+m_nGrowBy) * sizeof(ULONG) <= SIZE_T_MAX); // no overflow
#endif
    m_pData = (ULONG*) new ULONG[(nRow+m_nGrowBy)*2];
    memset(&m_pData[0],0,(nRow+m_nGrowBy)*sizeof(ULONG)*2);
    m_nSize = nRow; m_nMaxSize = nRow+m_nGrowBy;
    m_nBitLength = nColumn;
    return;
  }
  //------------------ Set columns size
  if (nColumn < m_nBitLength) {
    for (i=0; i < m_nSize; i++) {
      m_pData[i] = m_pData[i] >> (32 - m_nBitLength) << (32 - m_nBitLength);
      m_pData[i+m_nMaxSize] = 
        m_pData[i+m_nMaxSize] >> (32 - m_nBitLength) << (32 - m_nBitLength);
    }
  }
  m_nBitLength = nColumn;
  //------------------ Set rows size
  if (nRow <= m_nMaxSize) {
    if (nRow > m_nSize)
      for (i=m_nSize; i < nRow; i++) 
        m_pData[i] = m_pData[i+m_nMaxSize] = 0;
    else 
      for (i=m_nSize-1; i >= nRow; i--) 
        m_pData[i] = m_pData[i+m_nMaxSize] = 0;
    m_nSize = nRow;
    return;
  }
 //-------------- Otherwise grow array
#ifdef SIZE_T_MAX
  ASSERT((long)(nRow+m_nGrowBy) * sizeof(ULONG) <= SIZE_T_MAX);  // no overflow
#endif
  ULONG* pNewData = (ULONG*) new ULONG[(nRow+m_nGrowBy) * 2];
  memcpy(pNewData, m_pData, m_nSize * sizeof(ULONG));
  memcpy(pNewData+(nRow+m_nGrowBy), m_pData+m_nMaxSize, 
                                    m_nSize * sizeof(ULONG));
  m_nSize = nRow;  m_nMaxSize = (nRow+m_nGrowBy);
  for (i=m_nSize; i < m_nMaxSize; i++) {
    pNewData[i] = 0; pNewData[i+m_nMaxSize] = 0;
  }
// get rid of old stuff (note: no destructors called)
  delete [] m_pData;
  m_pData = pNewData;
}
//----------------- FreeExtra -------------------------------------------------
void CsTM::FreeExtra()
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nSize != m_nMaxSize) { // shrink to desired size
#ifdef SIZE_T_MAX
    ASSERT((long)m_nSize * sizeof(BYTE*) <= SIZE_T_MAX);  // no overflow
#endif
    ULONG* pNewData = NULL;
    if (m_nSize != 0) {         // copy new data from old
      pNewData = (ULONG*) new ULONG[m_nSize * 2];
      memcpy(pNewData, m_pData, m_nSize * sizeof(ULONG)); 
      memcpy(pNewData+m_nSize, m_pData+m_nMaxSize, m_nSize * sizeof(ULONG)); 
    }
    // get rid of old stuff (note: no destructors called)
    delete [] m_pData;
    m_pData = pNewData;
    m_nMaxSize = m_nSize;
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////// Data Reading ////////////////////////////////////////////
//----------------- GetBitAt --------------------------------------------------
char CsTM::GetBitAt(int nRow,int nColumn) const
{ 
  ASSERT(nColumn>=0 && nColumn<m_nBitLength && nRow>=0 && nRow<m_nSize);
  int a1, a0;
  a1 = m_pData[nRow] & sOB[nColumn];
  a0 = m_pData[nRow+m_nMaxSize] & sOB[nColumn];
  if(a1 != 0 && a0 == 0)  return '1';
  if(a0 != 0 && a1 == 0)  return '0';
  if(a0 == 0 && a1 == 0)  return '-';
  else return '+';
}

//----------------- GetRowTv --------------------------------------------------
CsTV CsTM::GetRowTv(int nRow) const
{ 
  ASSERT(nRow >= 0 && nRow < m_nSize);
  CsTV tv(m_pData[nRow],m_pData[nRow+m_nMaxSize],m_nBitLength);
  return tv;
}
 
//----------------- GetRowTv --------------------------------------------------
CsTV CsTM::GetRowTv(int nRow, ULONG maska) const
{ 
  ASSERT(nRow >= 0 && nRow < m_nSize);
  CsTV tv(GetRowTv(nRow));
  tv&=maska;
  return tv;
}

//----------------- GetRowTv --------------------------------------------------
CsTV CsTM::GetRowTv(int nRow, CsBV& maska) const
{ 
  ASSERT(nRow >= 0 && nRow < m_nSize && m_nBitLength == maska.GetBitLength());
  CsTV tv(GetRowTv(nRow));
  tv&=maska;
  return tv;
}

//----------------- GetColumnTv -----------------------------------------------
CsTV CsTM::GetColumnTv(int nColumn) const
{
  ASSERT(nColumn >= 0 && nColumn < m_nBitLength);
  ASSERT(m_nSize <= 32);
  CsTV tv(m_nSize);
  int i;
  for (i=0; i < m_nSize; i++) tv.SetBitAt(i,GetBitAt(i,nColumn));
  return tv;
}

//----------------- GetColumnTv -----------------------------------------------
CsTV CsTM::GetColumnTv(int nColumn, ULONG maska) const
{
  ASSERT(nColumn >= 0 && nColumn < m_nBitLength);
  ASSERT(m_nSize <= 32);
  CsTV tv(m_nSize);
  int i;
  for (i=0; i < m_nSize; i++) tv.SetBitAt(i,GetBitAt(i,nColumn));
  tv&=maska;
  return tv;
}  

//----------------- GetColumnTv -----------------------------------------------
CsTV CsTM::GetColumnTv(int nColumn,CsBV& maska) const
{
  ASSERT(nColumn >= 0 && nColumn < m_nBitLength && 
         m_nSize == maska.GetBitLength());
  CsTV tv(m_nSize);
  int i;
  for (i=0; i < m_nSize; i++) tv.SetBitAt(i,GetBitAt(i,nColumn));
  tv&=maska;
  return tv;
}
/*
#ifndef _LINUX
//----------------- BitChar ---------------------------------------------------
CString CsTM::BitChar(char One,char Zero,char Dont,char Rest,BOOL WithNum)
{
  int i,j,pos = 0;
  CString ch, s;
  if (WithNum) {
    if (m_nSize>9999)     { ch = "%5d: "; j = 9; }
    else if (m_nSize>999) { ch = "%4d: "; j = 8; }
    else if (m_nSize>99)  { ch = "%3d: "; j = 7; }
    else                  { ch = "%2d: "; j = 6; }
  }
  else
    j=2;
  CString res('\0',(m_nBitLength+j) * m_nSize);
  for (j=0; j<m_nSize; j++) {
    if(WithNum) {
      s.Format((const char*)ch,j);
      for (i=0;i<s.GetLength();i++) res.SetAt(pos++,s[i]);
    }
  for (i=0;i<m_nBitLength;i++)
    switch (GetBitAt(j,i)) {
      case '1': res.SetAt(pos++,One); break;
      case '0': res.SetAt(pos++,Zero); break;
      case '-': res.SetAt(pos++,Dont); break;
      default:  res.SetAt(pos++,Rest);
    }
    res.SetAt(pos++,'\n');  //res.SetAt(pos++,'\r'); 
  }
  return res;
}

//------------------------------------------------- BitChar(char One,char Zero)
void CsTM::BitChar(CStringArray & StrRes, BOOL WithClear, BOOL WithNum, 
                  char One, char Zero, char Dont, char Rest)
{ int i,j;
  if (WithClear) StrRes.RemoveAll();
  CString ch,s;
  if (WithNum) {
    if (m_nSize>9999)     { ch = "%5d: "; }
    else if (m_nSize>999) { ch = "%4d: "; }
    else if (m_nSize>99)  { ch = "%3d: "; }
    else                  { ch = "%2d: "; }
  }

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
char* CsTM::BitChar(char One,char Zero,char Dont,char Rest,BOOL WithNum)
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
      for (i=0;i<(int)strlen(s);i++) 
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
//#endif

//------------------------------------------------- BitChar(char One,char Zero)
void CsTM::BitChar(vector <string>& StrRes, BOOL WithClear, BOOL WithNum, 
                   char One, char Zero, char Dont, char Rest)
{ int i,j;
if (WithClear) StrRes.clear();
string ch,s;
char buf[100];
if (WithNum) {
  if (m_nSize>9999)     { ch = "%5d: "; }
  else if (m_nSize>999) { ch = "%4d: "; }
  else if (m_nSize>99)  { ch = "%3d: "; }
  else                  { ch = "%2d: "; }
}

for (j=0;j<m_nSize;j++) {
  if(WithNum) { 
    sprintf(buf,(const char*)ch.c_str(),j);
    s = buf; //.Format((const char*)ch,j);
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

//--------------------------- GetOnes -----------------------------------------
CsBM CsTM::GetOnes() const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  int i,j;
  CsBM bmTag(m_nSize,m_nBitLength);
  for (i=0; i < m_nSize; i++) 
    for (j=0; j < m_nBitLength; j++) 
      bmTag.SetBitAt(i,j, m_pData[i] & sOB[j]);
  return  bmTag;
}   

//--------------------------- GetZeros ----------------------------------------
CsBM CsTM::GetZeros() const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  int i,j;
  CsBM bmTag(m_nSize,m_nBitLength);
  for (i=0; i < m_nSize; i++) 
    for (j=0; j < m_nBitLength; j++) 
      bmTag.SetBitAt(i,j,m_pData[i+m_nMaxSize] & sOB[j]);
  return  bmTag;
}   

//--------------------------- GetRowUnDef -------------------------------------
CsBV CsTM::GetRowUnDef(int nRow) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(m_nSize >= nRow);
  CsBV bvTag(m_nBitLength,FALSE);
  ULONG w = m_pData[nRow] & m_pData[nRow+m_nMaxSize];
  bvTag = w;
  return  bvTag;
}   

//--------------------------- GetRowUnDef -------------------------------------
CsBV CsTM::GetRowUnDef(int nRow, ULONG maska) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(m_nSize >= nRow && nRow >= 0);
  CsBV bvTag(m_nBitLength,FALSE);
  ULONG w = m_pData[nRow] & m_pData[nRow+m_nMaxSize];
  bvTag = w&maska;
  return  bvTag;
} 

//--------------------------- GetRowDef ---------------------------------------
CsBV CsTM::GetRowDef(int nRow) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(m_nSize >= nRow && nRow >= 0);
  CsBV bvTag(m_nBitLength,FALSE);
  ULONG w = ~(m_pData[nRow] & m_pData[nRow+m_nMaxSize]);
  bvTag = w >> (32 - m_nBitLength) << (32 - m_nBitLength);
  return  bvTag;
}   

//--------------------------- GetRowDef ---------------------------------------
CsBV CsTM::GetRowDef(int nRow, ULONG maska) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(m_nSize >= nRow && nRow >= 0);
  CsBV bvTag(m_nBitLength,FALSE);
  ULONG w = ~(m_pData[nRow] & m_pData[nRow+m_nMaxSize]);
  w = w >> (32 - m_nBitLength) << (32 - m_nBitLength);
  bvTag = w&maska;
  return  bvTag;
}   

//--------------------------- GetRowOne ---------------------------------------
CsBV CsTM::GetRowOne(int nRow) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(m_nSize >= nRow);
  CsBV bvTag(m_nBitLength,FALSE);
  ULONG w = m_pData[nRow]& ~(m_pData[nRow] & m_pData[nRow+m_nMaxSize]);
  bvTag = w;
  return  bvTag;
}   

//--------------------------- GetRowOne ---------------------------------------
CsBV CsTM::GetRowOne(int nRow, ULONG maska) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(m_nSize >= nRow);
  CsBV bvTag(m_nBitLength,FALSE);
  ULONG w = m_pData[nRow]& ~(m_pData[nRow] & m_pData[nRow+m_nMaxSize]);
  bvTag = w&maska;
  return  bvTag;
}   

//--------------------------- GetRowZero --------------------------------------
CsBV CsTM::GetRowZero(int nRow) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(m_nSize >= nRow);
  CsBV bvTag(m_nBitLength,FALSE);
  ULONG w = m_pData[nRow+m_nMaxSize] & 
            ~(m_pData[nRow] & m_pData[nRow+m_nMaxSize]);
  bvTag = w;
  return  bvTag;
}   

//--------------------------- GetRowZero --------------------------------------
CsBV CsTM::GetRowZero(int nRow, ULONG maska) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(m_nSize >= nRow);
  CsBV bvTag(m_nBitLength,FALSE);
  ULONG w = m_pData[nRow+m_nMaxSize] & 
            ~(m_pData[nRow] & m_pData[nRow+m_nMaxSize]);
  bvTag = w&maska;
  return  bvTag;
}   

void CsTM::Param(int* aPar,int nMaxLen)
// return the number of long rows - rows with weght more than nMaxLen,
// the average rows weght and the average long rows weght
{
  int nAll=0, nRang, nL, nLarAll=0;
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
CsTM CsTM::ExtractRows(int nFirst, int nCount) const
{
  ASSERT(nFirst >= 0 && (nFirst + nCount) <= m_nSize && nCount > 0);
  CsTM tm(nCount,m_nBitLength);
  int i;
  for (i=0; i < nCount; i++) tm.SetRowTv(i,GetRowTv(nFirst + i));
  return tm;
}

//----------------- ExtractColumns --------------------------------------------
CsTM CsTM::ExtractColumns(int nFirst, int nCount) const
{
  ASSERT(nFirst >= 0 && (nFirst + nCount) <=m_nBitLength && nCount > 0);
  CsTM tm(m_nSize,nCount);
  int j;
  for (j=0;j < m_nSize;j++)  {
    tm.m_pData[j] = (m_pData[j]<<nFirst)>> (32 - nCount) << (32 - nCount);
    tm.m_pData[j + tm.m_nMaxSize] =
      (m_pData[j + m_nMaxSize] <<nFirst)>> (32 - nCount) << (32 - nCount);
  }
  return tm;
}

//----------------- Extract ---------------------------------------------------
CsTM CsTM::Extract(int nFirstRow, int nFirstColumn, 
                 int nCountRow, int nCountColumn) const
{
  ASSERT(nFirstRow >= 0 && nFirstColumn >= 0 && 
         nCountColumn > 0 && nCountRow > 0);
  ASSERT(((nFirstColumn + nCountColumn) <=m_nBitLength) && 
         (nFirstRow + nCountRow <=m_nSize));
  CsTM tm(nCountRow,nCountColumn);
  int j;
  for (j=nFirstRow;j < nFirstRow+nCountRow;j++)  {
    tm.m_pData[j-nFirstRow] = 
        (m_pData[j]<<nFirstColumn)
            >> (32 - nCountColumn) << (32 - nCountColumn);
    tm.m_pData[j-nFirstRow + tm.m_nMaxSize] =
        (m_pData[j + m_nMaxSize] <<nFirstColumn)
            >> (32 - nCountColumn) << (32 - nCountColumn);
  }
  return tm;
}

//----------------- ExtractColumns --------------------------------------------
CsTM CsTM::ExtractColumns(const CsBV& Columns) const
{
  ASSERT(Columns.GetBitLength() > 0);
  ASSERT(m_nBitLength >= Columns.GetBitLength());
  int i, j, LeftOneColumn;
  int NewColumnLen = Columns.CountBit();
  CsTM tm(m_nSize,NewColumnLen);
  for(i=0; i < m_nSize; i++)  {
    LeftOneColumn = -1;
    for (j=0; j < NewColumnLen; j++)  {
      LeftOneColumn = Columns.LeftOne(LeftOneColumn);
      tm.SetBitAt(i,j,GetBitAt(i,LeftOneColumn));
    }
  }
  return tm;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////// Data Writing //////////////////////////////////////////////
//----------------- SetBitAt --------------------------------------------------
void CsTM::SetBitAt(int nRow, int nColumn, char symb/*='-'*/)
{
  ASSERT(nRow >= 0 && nRow < m_nSize); 
  ASSERT(nColumn >= 0 && nColumn <= m_nBitLength);
  switch (symb) {
    case '1': m_pData[nRow] |=sOB[nColumn]; 
              m_pData[nRow+m_nMaxSize] &=~sOB[nColumn]; break;
    case '0': m_pData[nRow+m_nMaxSize] |=sOB[nColumn]; 
              m_pData[nRow] &=~sOB[nColumn]; break;
    case '-': m_pData[nRow] &= ~sOB[nColumn]; 
              m_pData[nRow+m_nMaxSize] &= ~sOB[nColumn]; break;
    default:  m_pData[nRow] |= sOB[nColumn]; 
              m_pData[nRow+m_nMaxSize] |= sOB[nColumn];
  } 
}

//----------------- SetRowTv --------------------------------------------------
void CsTM::SetRowTv(int nRow, const CsTV& tv)
{ 
  ASSERT(nRow >= 0 && nRow < m_nSize && m_nBitLength == tv.GetBitLength());
  m_pData[nRow] = (ULONG)tv.GetOnes();
  m_pData[nRow+m_nMaxSize] = (ULONG)tv.GetZeros();
}

//----------------- SetRowTv --------------------------------------------------
void CsTM::SetRowTv(int nRow, const CsTM& tm, int ntmRow)
{ 
  ASSERT(nRow >= 0 && nRow < m_nSize && m_nBitLength == tm.GetCountC());
  m_pData[nRow] = tm.m_pData[ntmRow];
  m_pData[nRow+m_nMaxSize] = tm.m_pData[ntmRow+tm.m_nMaxSize];
}

//--------------------------- SetPartAt ---------------------------------------
void CsTM::SetPartAt(BOOL Part,const CsBM& bm)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(m_nBitLength == bm.GetCountC() && m_nSize == bm.GetCountR());
  int i;
  if(Part)
    for (i=0; i < m_nSize; i++) 
      m_pData[i]= (ULONG)bm.GetRow(i);
  else 
    for (i=0;i < m_nSize; i++) 
      m_pData[i+m_nMaxSize]= (ULONG)bm.GetRow(i);
  return;
}   

//--------------------------- SetPartAt ---------------------------------------
void CsTM::SetPartAt(BOOL Part, const CsBV& bv, int Row)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(m_nBitLength == bv.GetBitLength() && m_nSize >= Row);
  if(Part)
  {
      m_pData[Row] = (ULONG)bv;
      m_pData[Row+m_nMaxSize] &= ~(ULONG)bv;
  }
  else 
  {
      m_pData[Row] &= ~(ULONG)bv;
      m_pData[Row+m_nMaxSize] = (ULONG)bv;
  }
  return;
}   

//--------------------------- SetRowZero --------------------------------------
void CsTM::SetRowZero(int nRow,const ULONG newRow)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  m_pData[nRow+m_nMaxSize] = newRow; m_pData[nRow] &= ~newRow;
  return;
}   

//--------------------------- SetRowOne ---------------------------------------
void CsTM::SetRowOne(int nRow,const ULONG newRow)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  m_pData[nRow] = newRow; m_pData[nRow+m_nMaxSize] &= ~newRow;
  return;
}   
///////////////////////////////////////////////////////////////////////////////
//***************** Working with rows *****************************************
//----------------- Add -------------------------------------------------------
int CsTM::Add(char symb/*='-'*/,int nCount/*=1*/)
{
  int i, first=m_nSize;
  if(m_nSize + nCount <= m_nMaxSize) {
    m_nSize += nCount;
    for(i=first; i < m_nSize; i++) {
      m_pData[i] = m_pData[i+m_nMaxSize] =0;
    }
  }
  else 
    SetSize(m_nSize + nCount, m_nBitLength, m_nGrowBy);
  if(symb != '-') {
    for (; first < m_nSize; first++) {
      switch (symb) {
      case '1': m_pData[first]=
                    0xffffffff >> (32 - m_nBitLength) << (32 - m_nBitLength);
                m_pData[first+m_nMaxSize]=0; break;
      case '0': m_pData[first+m_nMaxSize]=
                    0xffffffff >> (32 - m_nBitLength) << (32 - m_nBitLength);
                m_pData[first]=0; break;
      case '+': m_pData[first]=
                    0xffffffff >> (32 - m_nBitLength) << (32 - m_nBitLength);
                m_pData[first+m_nMaxSize]=
                    0xffffffff >> (32 - m_nBitLength) << (32 - m_nBitLength);
                break;
      }
    }
  }
  return m_nSize-1;
} 

//----------------- Add -------------------------------------------------------
int CsTM::Add(ULONG t1, ULONG t0)
{
  if(m_nSize + 1 <= m_nMaxSize) {
    m_pData[m_nSize] = m_pData[m_nSize+m_nMaxSize] =0;
    m_nSize ++;
  }
  else 
    SetSize(m_nSize + 1, m_nBitLength, m_nGrowBy);
  m_pData[m_nSize-1] = t1;
  m_pData[m_nSize+m_nMaxSize-1] = t0;
  return m_nSize-1;
} 

inline int CsTM::Add(const CsTM& tm, int ntmRow/*=-1*/)
{ int nIndex = m_nSize; 
  if (ntmRow>=0) SetRowGrow(nIndex,tm,ntmRow);
  else InsertRow(nIndex, 0, tm, tm.GetCountR());
  return nIndex; 
}


//----------------- SetRowGrow ------------------------------------------------
void CsTM::SetRowGrow(int nRow, const CsTV& newRow)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(nRow >= 0);
  ASSERT(m_nBitLength == 0 || m_nBitLength == newRow.GetBitLength());
  if (nRow >= m_nSize) SetSize(nRow+1,newRow.GetBitLength(), m_nGrowBy);
  m_nBitLength = newRow.GetBitLength();
  m_pData[nRow] = (ULONG)newRow.GetOnes();
  m_pData[nRow+m_nMaxSize] = (ULONG)newRow.GetZeros();
}

//----------------- SetRowGrow ------------------------------------------------
void CsTM::SetRowGrow(int nRow, const CsTM& tm, int ntmRow)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(nRow >= 0 && ntmRow < tm.m_nSize);
  ASSERT(m_nBitLength == 0 || m_nBitLength == tm.m_nBitLength);
  if (nRow >= m_nSize) SetSize(nRow+1,tm.m_nBitLength,m_nGrowBy);
  m_nBitLength = tm.m_nBitLength;
  m_pData[nRow] = tm.m_pData[ntmRow];
  m_pData[nRow+m_nMaxSize] = tm.m_pData[ntmRow + tm.m_nMaxSize];
}

//----------------- InsertRow -------------------------------------------------
void CsTM::InsertRow(int nRow,const CsTV& newRow, int nCount/*=1*/)
//Insert nCount copies of vector newRow before nRow row of TM
{ 
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(nRow >= 0 && 
         (m_nBitLength==0 || newRow.GetBitLength() == m_nBitLength));
  int i;
  if (nRow >= m_nSize) {     // adding after the end of the array
    SetSize(nRow + nCount,newRow.GetBitLength(),m_nGrowBy);  // grow so nIndex is valid
  }
  else {                     // inserting in the middle of the array
    int out = m_nSize-nRow, begin = nRow+nCount;
    SetSize(m_nSize + nCount,newRow.GetBitLength(),m_nGrowBy);
    memmove(&m_pData[begin],&m_pData[nRow],sizeof(ULONG)*out);
    memmove(&m_pData[begin+m_nMaxSize],
            &m_pData[nRow+m_nMaxSize],sizeof(ULONG)*out);
  }
// insert new value in the gap
  ASSERT(nRow + nCount <= m_nSize);
  for(i=nRow; i<nRow+nCount; i++) {
    m_pData[i] = (ULONG)newRow.GetOnes();
    m_pData[i+m_nMaxSize] = (ULONG)newRow.GetZeros();
  }
}

//----------------- InsertRow -------------------------------------------------
void CsTM::InsertRow(int nRow,int nStartRow, const CsTM& tm,int nCount)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(nRow >= 0 && nStartRow >= 0 && m_nBitLength == tm.m_nBitLength);
  if (tm.m_nSize > 0) {
    InsertRow(nRow,tm.GetRowTv(nStartRow),nCount);
    for (int i = 1; i < nCount; i++)
      SetRowTv(nRow + i,tm,nStartRow+i);
  }
}

//----------------- RemoveRow -------------------------------------------------
void CsTM::RemoveRow(int nRow, int nCount /* = 1 */)
{ 
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(nRow >= 0 && nCount >= 0 && nRow + nCount <= m_nSize);
  int nMoveCount = m_nSize - (nRow + nCount);     // just remove a range
  if (nMoveCount) {
    memcpy(&m_pData[nRow],&m_pData[nRow + nCount],
            nMoveCount * sizeof(ULONG));
    memcpy(&m_pData[nRow+m_nMaxSize],&m_pData[nRow+m_nMaxSize + nCount],
            nMoveCount * sizeof(ULONG));
  }  
  m_nSize -= nCount;
}

//----------------- ExchangeRow -----------------------------------------------
void CsTM::ExchangeRow(int nRow1,int nRow2,int nCount/*=1*/)
{
  ASSERT(nCount>0 && nRow1+nCount<=m_nSize && nRow2+nCount<=m_nSize);
  ULONG w;
  while(nCount >0) {
    w = m_pData[nRow1]; m_pData[nRow1]=m_pData[nRow2]; m_pData[nRow2]=w;
    w = m_pData[nRow1+m_nMaxSize]; 
    m_pData[nRow1+m_nMaxSize] = m_pData[nRow2+m_nMaxSize]; 
    m_pData[nRow2+m_nMaxSize] = w;
    nCount--; 
  }
  return;
}

//----------------- AddCol ----------------------------------------------------
int CsTM::AddCol(int numCol/*=1*/)
//numCol columns adding to the matrix; return the number of new last column
{ 
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(numCol > 0);
  if (m_nBitLength==32) return m_nBitLength-1;
  m_nBitLength+=numCol;
  return m_nBitLength-1;
}
 

///////////////////////////////////////////////////////////////////////////////
/////////////////// Operators and functions of assignment /////////////////////
//----------------- operator =(const CsTM&) ------------------------------------
const CsTM& CsTM::operator =(const CsTM& tm)
{
  int i;
  SetSize(tm.m_nSize,tm.m_nBitLength,tm.m_nGrowBy);
  for (i=0;i < tm.m_nSize;i++) SetRowTv(i,tm.GetRowTv(i));
  return *this;
}

//----------------- operator =(const char* ) ----------------------------------
const CsTM& CsTM::operator =(const char* pch)
{
/*
#ifndef _LINUX
	CharBit(pch);
#else
*/
	char * s = strdup(pch);
	CharBit(s);
	free(s);
//#endif
  return *this;

}
/*
#ifndef _LINUX
//----------------- operator =(const CStringArray) ----------------------------
const CsTM& CsTM::operator =(const CStringArray& StrAr)
{ 
  CharBit(StrAr);
  return *this;
}
#else
*/
//----------------- operator =(const vector <string>) ----------------------------
const CsTM& CsTM::operator =(const vector <string>& StrAr)
{ 
  CharBit(StrAr);
  return *this;
}

//#endif

//----------------- Clear -----------------------------------------------------
void CsTM::Clear(char symb/*='-'*/,int nRow/*=-1*/)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT (nRow>=-1 && nRow < m_nSize);
  int k,first=0,last=m_nSize-1;
  if (nRow != -1) first=last=nRow;
  for (k=first; k <= last; k++) {
    m_pData[k]=0;
    m_pData[k+m_nMaxSize] = 0;
    switch (symb) {
      case '1': m_pData[k] = 0xffffffff >> 
                            (32 - m_nBitLength) << (32 - m_nBitLength);
                break; 
      case '0': m_pData[k+m_nMaxSize] = 0xffffffff >> 
                            (32 - m_nBitLength) << (32 - m_nBitLength);
                  break;
      case '+': m_pData[k+m_nMaxSize] = 0xffffffff >> 
                            (32 - m_nBitLength) << (32 - m_nBitLength);
                m_pData[k] = 0xffffffff >> 
                            (32 - m_nBitLength) << (32 - m_nBitLength);
                break;
    }
  } 
}

///////////////////////////////////////////////////////////////////////////////
/////////////////// Logic operations //////////////////////////////////////////
//----------------- InvertDefs ------------------------------------------------
//инвертировать опр.значения строк или одной стр.

void CsTM::InvertDefs(int nRow/*=-1*/)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ULONG a;
  if(nRow >=0) {
    a = m_pData[nRow];
    m_pData[nRow]=m_pData[nRow+m_nMaxSize];
    m_pData[nRow+m_nMaxSize]=a;
  }
  else
    for(nRow++; nRow < m_nSize; nRow++) {
      a=m_pData[nRow];
      m_pData[nRow]=m_pData[nRow+m_nMaxSize];
      m_pData[nRow+m_nMaxSize]=a;
    }
}    

///////////////////////////////////////////////////////////////////////////////
/////////////////// Operations of weighting, finding and casing ///////////////
//----------------- CountDef --------------------------------------------------
int CsTM::CountDefs(int nRow/*=-1*/) const
{
  ASSERT (nRow >=-1 && nRow < m_nSize);
  int k, one=0, first, last;
  ULONG a;
  if (nRow==-1) { first=0; last=m_nSize-1; }
  else { first=nRow; last=nRow; }
  for (k=first; k <= last; k++) {
    a = m_pData[k] ^ m_pData[k+m_nMaxSize];
    BYTE* pB= (BYTE*)&a;
    one += sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]];
  }
  return one;
}

//----------------- CountOne --------------------------------------------------
int CsTM::CountOnes(int nRow/*=-1*/) const
{
  ASSERT (nRow >=-1 && nRow < m_nSize);
  int k, one=0, first, last;
  ULONG a;
  if (nRow==-1) { first=0; last=m_nSize-1; }
  else { first=nRow; last=nRow; }
  for (k=first; k <= last; k++) {
    a = (m_pData[k]^m_pData[k+m_nMaxSize]) & m_pData[k];
    BYTE* pB= (BYTE*)&a;
    one += (sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]]);
  }
  return one;
}

//----------------- CountZero -------------------------------------------------
int CsTM::CountZeros(int nRow/*=-1*/) const
{
  ASSERT (nRow >=-1 && nRow < m_nSize);
  int k, one=0, first, last;
  ULONG a;
  if (nRow==-1) { first=0; last=m_nSize-1; }
  else { first=nRow; last=nRow; }
  for (k=first; k <= last; k++) {
    a = (m_pData[k]^m_pData[k+m_nMaxSize]) & m_pData[k+m_nMaxSize];
    BYTE* pB= (BYTE*)&a;
    one += (sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]]);
  }
  return one;
}

//----------------- CountUndef ------------------------------------------------
int CsTM::CountUnDefs(const ULONG mask,int nRow) const
{ 
  ASSERT (nRow >=-1 && nRow < m_nSize);
  int k, one=0, first, last;
  ULONG a;
  if (nRow==-1) { first=0; last=m_nSize-1; }
  else { first=nRow; last=nRow; }
  for (k=first; k <= last; k++) {
    a = m_pData[k] ^ m_pData[k+m_nMaxSize];
    BYTE* pB= (BYTE*)&a;
    one += m_nBitLength - (sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]]);
  }
  return one;
}

//----------------- CountDef --------------------------------------------------
int CsTM::CountDefs(const ULONG mask,int nRow/*=-1*/) const
{
  ASSERT (nRow >=-1 && nRow < m_nSize);
  int k, one=0, first, last;
  ULONG a;
  if (nRow==-1) { first=0; last=m_nSize-1; }
  else { first=nRow; last=nRow; }
  for (k=first; k <= last; k++) {
    a = (m_pData[k] ^ m_pData[k+m_nMaxSize]) & mask;
    BYTE* pB= (BYTE*)&a;
    one += 32 - (sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]]);
  }
  return one;
}

//----------------- CountOne --------------------------------------------------
int CsTM::CountOnes(const ULONG mask,int nRow/*=-1*/)  const
{
  ASSERT (nRow >=-1 && nRow < m_nSize);
  int k, one=0, first, last;
  ULONG a;
  if (nRow==-1) { first=0; last=m_nSize-1; }
  else { first=nRow; last=nRow; }
  for (k=first; k <= last; k++) {
    a = (m_pData[k]^m_pData[k+m_nMaxSize]) &
          m_pData[k] & mask;
    BYTE* pB= (BYTE*)&a;
    one += sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]];
  }
  return one;
}

//----------------- CountZero -------------------------------------------------
int CsTM::CountZeros(const ULONG mask,int nRow/*=-1*/)  const
{
  ASSERT (nRow >=-1 && nRow < m_nSize);
  int k, one=0, first, last;
  ULONG a;
  if (nRow==-1) { first=0; last=m_nSize-1; }
  else { first=nRow; last=nRow; }
  for (k=first; k <= last; k++) {
    a = (m_pData[k]^m_pData[k+m_nMaxSize]) &
          m_pData[k+m_nMaxSize] & mask;
    BYTE* pB= (BYTE*)&a;
    one += sTabC[pB[0]] + sTabC[pB[1]] + sTabC[pB[2]] + sTabC[pB[3]];
  }
  return one;
}

//----------------- LeftUnDef -------------------------------------------------
int CsTM::LeftUnDef(int nRow, int nNext/*=-1*/)  const //next bit
{
  int pos = 0;
  if(m_nBitLength <= (nNext+1)) return (-1);
  ULONG a;
  a = (~(m_pData[nRow] ^ m_pData[nRow+m_nMaxSize]))>> 
                            (32 - m_nBitLength) << (32 - m_nBitLength);
  a <<= (nNext+1);
  if (a)  {
    while (!(a & sOB[pos++]));
    return (nNext + pos);
  }
  return (-1);
}

//----------------- LeftDef ---------------------------------------------------
int CsTM::LeftDef(int nRow, int nNext/*=-1*/)  const //next bit
{
  int pos = 0;
  if(m_nBitLength <= (nNext+1)) return (-1);
  ULONG a;
  a = m_pData[nRow] ^ m_pData[nRow+m_nMaxSize];
  a <<= (nNext+1);
  if (a)  {
    while (!(a & sOB[pos++]));
    return (nNext + pos);
  }
  return (-1);
}

//----------------- LeftOne ---------------------------------------------------
int CsTM::LeftOne(int nRow, int nNext/*=-1*/)  const //next bit
{
  int pos = 0;
  if(m_nBitLength <= (nNext+1)) return (-1);
  ULONG a;
  a = ((m_pData[nRow]^m_pData[nRow+m_nMaxSize]) & 
        m_pData[nRow]) << (nNext+1);
  if (a)  {
    while (!(a & sOB[pos++]));
    return (nNext + pos);
  }
  return (-1);
}

//----------------- LeftZero --------------------------------------------------
int CsTM::LeftZero(int nRow, int nNext/*=-1*/)  const //next bit
{
  int pos = 0;
  if(m_nBitLength <= (nNext+1)) return (-1);
  ULONG a;
  a = ((m_pData[nRow]^m_pData[nRow+m_nMaxSize]) & 
        m_pData[nRow+m_nMaxSize]) << (nNext+1);
  if (a)  {
    while (!(a & sOB[pos++]));
    return (nNext + pos);
  }
  return (-1);
}

//----------------- LeftUndef -------------------------------------------------
int CsTM::LeftUnDef(int nRow, int nNext, ULONG mask) const
{
  int pos = 0;
  if(m_nBitLength <= (nNext+1)) return (-1);
  ULONG a;
  a = mask & ((~(m_pData[nRow] ^ m_pData[nRow+m_nMaxSize]))>> 
                            (32 - m_nBitLength) << (32 - m_nBitLength));
  a <<= (nNext+1);
  if (a)  {
    while (!(a & sOB[pos++]));
    return (nNext + pos);
  }
  return (-1);
}

//----------------- LeftDef ---------------------------------------------------
int CsTM::LeftDef(int nRow, int nNext, ULONG mask) const
{
  int pos = 0;
  if(m_nBitLength <= (nNext+1)) return (-1);
  ULONG a;
  a = m_pData[nRow] ^ m_pData[nRow+m_nMaxSize];
  a = (a & mask) << (nNext+1);
  if (a)  {
    while (!(a & sOB[pos++]));
    return (nNext + pos);
  }
  return (-1);
}

//----------------- LeftOne ---------------------------------------------------
int CsTM::LeftOne(int nRow, int nNext, ULONG mask) const
{
  int pos = 0;
  if(m_nBitLength <= (nNext+1)) return (-1);
  ULONG a;
  a = ((m_pData[nRow]^m_pData[nRow+m_nMaxSize]) & 
        m_pData[nRow] & mask) << (nNext+1);
  if (a)  {
    while (!(a & sOB[pos++]));
    return (nNext + pos);
  }
  return (-1);
}

//----------------- LeftZero --------------------------------------------------
int CsTM::LeftZero(int nRow, int nNext, ULONG mask) const
{
  int pos = 0;
  if(m_nBitLength <= (nNext+1)) return (-1);
  ULONG a;
  a = ((m_pData[nRow]^m_pData[nRow+m_nMaxSize]) & 
        m_pData[nRow+m_nMaxSize] & mask) << (nNext+1);
  if (a)  {
    while (!(a & sOB[pos++]));
    return (nNext + pos);
  }
  return (-1);
}

//----------------- RightUnDef ------------------------------------------------
int CsTM::RightUnDef(int nRow, int nPrev/*=-1*/) const
{
  int j;
  ULONG a;
  if (nPrev==-1 || nPrev>=m_nBitLength) j = m_nBitLength - 1;
  else  j = nPrev; 

  a = (~(m_pData[nRow] ^ m_pData[nRow+m_nMaxSize]))>> 
                            (32 - m_nBitLength) << (32 - m_nBitLength);
  for (; j>=0; j--)
    if (a & sOB[j]) return j;
  return (-1);
}

//----------------- RightDef --------------------------------------------------
int CsTM::RightDef(int nRow, int nPrev/*=-1*/) const
{
  int j;
  ULONG a;
  if (nPrev==-1 || nPrev>=m_nBitLength) j = m_nBitLength - 1;
  else  j = nPrev; 

  a = m_pData[nRow] ^ m_pData[nRow+m_nMaxSize];
  for (; j>=0; j--) 
    if (a & sOB[j]) return j;
  return (-1);
}

//----------------- RightOne --------------------------------------------------
int CsTM::RightOne(int nRow, int nPrev/*=-1*/) const
{
  int j;
  ULONG a;
  if (nPrev==-1 || nPrev>=m_nBitLength) j = m_nBitLength - 1;
  else  j = nPrev; 

  a = (m_pData[nRow]^m_pData[nRow+m_nMaxSize]) & m_pData[nRow];
  for (; j>=0; j--) 
    if (a & sOB[j]) return j;
  return (-1);
}


//----------------- RightZero -------------------------------------------------
int CsTM::RightZero(int nRow, int nPrev/*=-1*/) const
{
  int j;
  ULONG a;
  if (nPrev==-1 || nPrev>=m_nBitLength) j = m_nBitLength - 1;
  else  j = nPrev; 

  a = (m_pData[nRow]^m_pData[nRow+m_nMaxSize]) & m_pData[nRow+m_nMaxSize];
  for (; j>=0; j--) 
    if (a & sOB[j]) return j;
  return (-1);
}

//----------------- RightUnDef ------------------------------------------------
int CsTM::RightUnDef(int nRow, int nPrev/*=-1*/, ULONG mask) const
{
  int j;
  ULONG a;
  if (nPrev==-1 || nPrev>=m_nBitLength) j = m_nBitLength - 1;
  else  j = nPrev; 

  a = mask & ((~(m_pData[nRow] ^ m_pData[nRow+m_nMaxSize]))>> 
                            (32 - m_nBitLength) << (32 - m_nBitLength));
  for (; j>=0; j--)
    if (a & sOB[j]) return j;
  return (-1);
}

//----------------- RightDef --------------------------------------------------
int CsTM::RightDef(int nRow, int nPrev/*=-1*/, ULONG mask) const
{
  int j;
  ULONG a;
  if (nPrev==-1 || nPrev>=m_nBitLength) j = m_nBitLength - 1;
  else  j = nPrev; 

  a = (m_pData[nRow] ^ m_pData[nRow+m_nMaxSize]) & mask;
  for (; j>=0; j--) 
    if (a & sOB[j]) return j;
  return (-1);
}

//----------------- RightOne --------------------------------------------------
int CsTM::RightOne(int nRow, int nPrev/*=-1*/, ULONG mask) const
{
  int j;
  ULONG a;
  if (nPrev==-1 || nPrev>=m_nBitLength) j = m_nBitLength - 1;
  else  j = nPrev; 

  a = (m_pData[nRow]^m_pData[nRow+m_nMaxSize]) & m_pData[nRow] & mask;
  for (; j>=0; j--) 
    if (a & sOB[j]) return j;
  return (-1);
}

//----------------- RightZero -------------------------------------------------
int CsTM::RightZero(int nRow, int nPrev/*=-1*/, ULONG mask) const
{
  int j;
  ULONG a;
  if (nPrev==-1 || nPrev>=m_nBitLength) j = m_nBitLength - 1;
  else  j = nPrev; 

  a = (m_pData[nRow]^m_pData[nRow+m_nMaxSize]) & 
                                     m_pData[nRow+m_nMaxSize] & mask;
  for (; j>=0; j--) 
    if (a & sOB[j]) return j;
  return (-1);
}

//----------------- FindUndefRow ----------------------------------------------
int CsTM::FindUnDefR (int nRow/*=-1*/) const
{
  ASSERT(nRow < m_nSize-1);
  for (++nRow; nRow < m_nSize; nRow++)  {
   if((m_pData[nRow] | m_pData[nRow+m_nMaxSize]) ==0)
     return nRow;
  }    
  return -1;
}                               

//----------------- FindDefRow ------------------------------------------------
int CsTM::FindDefR (int nRow/*=-1*/) const
{
  ASSERT(nRow < m_nSize-1);
  ULONG mask = 0xffffffff >> (32 - m_nBitLength) << (32 - m_nBitLength);
  for (++nRow; nRow < m_nSize; nRow++)  {
   if((m_pData[nRow] ^ m_pData[nRow+m_nMaxSize]) == mask)
     return nRow;
  }    
  return -1;
}                               

//----------------- FindOneRow ------------------------------------------------
int CsTM::FindOneR (int nRow/*=-1*/) const
{
  ASSERT(nRow < m_nSize-1);
  ULONG mask = 0xffffffff >> (32 - m_nBitLength) << (32 - m_nBitLength);
  for (++nRow; nRow < m_nSize; nRow++)  {
   if(m_pData[nRow] == mask)
     return nRow;
  }    
  return -1;
}                               

//----------------- FindZeroRow -----------------------------------------------
int CsTM::FindZeroR (int nRow/*=-1*/) const
{
  ASSERT(nRow < m_nSize-1);
  ULONG mask = 0xffffffff >> (32 - m_nBitLength) << (32 - m_nBitLength);
  for (++nRow; nRow < m_nSize; nRow++)  {
   if(m_pData[nRow+m_nMaxSize] == mask)
     return nRow;
  }    
  return -1;
}                               

//----------------- FindUndefRow ----------------------------------------------
int CsTM::FindUnDefR(ULONG mask, int nRow/*=-1*/) const
{
  ASSERT(nRow < m_nSize-1);
  for (++nRow; nRow < m_nSize; nRow++)  {
   if(((m_pData[nRow] | m_pData[nRow+m_nMaxSize])& mask) ==0)
     return nRow;
  }    
  return -1;
}                               

//----------------- FindDefRow ------------------------------------------------
int CsTM::FindDefR (ULONG mask, int nRow/*=-1*/) const
{
  ASSERT(nRow < m_nSize-1);
  for (++nRow; nRow < m_nSize; nRow++)  {
   if(((m_pData[nRow] ^ m_pData[nRow+m_nMaxSize])&mask) == mask)
     return nRow;
  }    
  return -1;
}

//----------------- FindOneRow ------------------------------------------------
int CsTM::FindOneR (ULONG mask, int nRow/*=-1*/) const
{
  ASSERT(nRow < m_nSize-1);
  for (++nRow; nRow < m_nSize; nRow++)  {
   if((m_pData[nRow] & mask) == mask)
     return nRow;
  }    
  return -1;
}                               

//----------------- FindZeroRow -----------------------------------------------
int CsTM::FindZeroR (ULONG mask, int nRow/*=-1*/) const
{
  ASSERT(nRow < m_nSize-1);
  for (++nRow; nRow < m_nSize; nRow++)  {
   if((m_pData[nRow+m_nMaxSize] & mask) == mask)
     return nRow;
  }    
  return -1;
}                                                        

//----------------- FindRow ---------------------------------------------------
int CsTM::FindR (const CsTV& tv, int nRow) const
{
  if (IsEmpty()) return -1;
  ASSERT(m_nBitLength == tv.GetBitLength());
  for (nRow++; nRow < m_nSize; nRow++)  
    if((ULONG)tv.GetOnes() ==  m_pData[nRow] &&
       (ULONG)tv.GetZeros() == m_pData[nRow+m_nMaxSize])
      return nRow;
  return -1;
}

//----------------- FindRow ---------------------------------------------------
int CsTM::FindR (const CsBV& bv, int nRow) const
{
  if (IsEmpty()) return -1;
  ASSERT(m_nBitLength == bv.GetBitLength());
  for (nRow++; nRow < m_nSize; nRow++)  
    if((ULONG)bv == m_pData[nRow] &&
       (~(ULONG)bv) == m_pData[nRow+m_nMaxSize]) 
      return nRow;
  return -1;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////// Matrix Compareing and sorting operations //////////////////
//----------------- IsCorrect -------------------------------------------------
BOOL CsTM::IsCorrect(int nRow/*=-1*/) const
{
  ASSERT (nRow>=-1 && nRow < m_nSize);
  int k,first=0,last=m_nSize-1;
  if (nRow != -1) first=last=nRow;
  for (k=first; k <= last; k++)
    if((m_pData[k] & m_pData[k+m_nMaxSize]) != 0) return FALSE;
  return TRUE;
}

//----------------- IsBool ----------------------------------------------------
BOOL CsTM::IsBool(int nRow/*=-1*/) const
{
  ASSERT (nRow>=-1 && nRow < m_nSize);
  int k,first=0,last=m_nSize-1;
  ULONG mask = 0xffffffff >> (32 - m_nBitLength) << (32 - m_nBitLength);
  if (m_nBitLength == 0) return FALSE;
  if (nRow != -1) first=last=nRow;
  for (k=first; k <= last; k++) {
    if((m_pData[k]|m_pData[k+m_nMaxSize])!= mask)
      return FALSE;
  }
  return TRUE;
}

//----------------- IsBool ----------------------------------------------------
BOOL CsTM::IsBool(const ULONG mask,int nRow/*=-1*/) const
{
  ASSERT (nRow>=-1 && nRow < m_nSize);
  int k,first=0,last=m_nSize-1;
  if (m_nBitLength == 0) return FALSE;
  if (nRow != -1) first=last=nRow;
  for (k=first; k <= last; k++) {
    if(((m_pData[k]|m_pData[k+m_nMaxSize])&mask)!= mask)
      return FALSE;
  }
  return TRUE;
}


//----------------- IsTrivial -------------------------------------------------
BOOL CsTM::IsTrivial(int nRow/*=-1*/) const  //all components are '-'
{
  ASSERT (nRow>=-1 && nRow < m_nSize);
  int k,first=0,last=m_nSize-1;
  if (m_nBitLength == 0) return FALSE;
  if (nRow != -1) first=last=nRow;
  for (k=first; k <= last; k++) 
    if((m_pData[k]|m_pData[k+m_nMaxSize]) != 0)
      return FALSE;
  return TRUE;
}

//----------------- IsTrivial -------------------------------------------------
//all components are '-'

BOOL CsTM::IsTrivial(const ULONG mask,int nRow/*=-1*/) const  
{
  ASSERT (nRow>=-1 && nRow < m_nSize);
  int k,first=0,last=m_nSize-1;
  if (m_nBitLength == 0) return FALSE;
  if (nRow != -1) first=last=nRow;
  for (k=first; k <= last; k++) 
    if(((m_pData[k]|m_pData[k+m_nMaxSize])&mask) != 0)
      return FALSE;
  return TRUE;
}

//----------------- IsOne -----------------------------------------------------
BOOL CsTM::IsOne(int nRow/*=-1*/) const
{
  ASSERT (nRow>=-1 && nRow < m_nSize);
  int k,first=0,last=m_nSize-1;
  if (m_nBitLength == 0) return FALSE;
  if (nRow != -1) first=last=nRow;
  for (k=first; k <= last; k++) {
    if(m_pData[k] != 0) return FALSE;
  }
  return TRUE;
}

//----------------- IsOne -----------------------------------------------------
BOOL CsTM::IsOne(const ULONG mask,int nRow/*=-1*/) const
{
  ASSERT (nRow>=-1 && nRow < m_nSize);
  int k,first=0,last=m_nSize-1;
  if (m_nBitLength == 0) return FALSE;
  if (nRow != -1) first=last=nRow;
  for (k=first; k <= last; k++) {
    if((m_pData[k] & mask) != mask) return FALSE;
  }
  return TRUE;
}

//----------------- IsZero ----------------------------------------------------
BOOL CsTM::IsZero(int nRow/*=-1*/) const
{
  ASSERT (nRow>=-1 && nRow < m_nSize);
  int k,first=0,last=m_nSize-1;
  if (m_nBitLength == 0) return FALSE;
  if (nRow != -1) first=last=nRow;
  for (k=first; k <= last; k++) {
    if(m_pData[k+m_nMaxSize] != 0) return FALSE;
  }
  return TRUE;
}

//----------------- IsZero ----------------------------------------------------
BOOL CsTM::IsZero(const ULONG mask,int nRow/*=-1*/) const
{
  ASSERT (nRow>=-1 && nRow < m_nSize);
  int k,first=0,last=m_nSize-1;
  if (m_nBitLength == 0) return FALSE;
  if (nRow != -1) first=last=nRow;
  for (k=first; k <= last; k++) {
    if((m_pData[k+m_nMaxSize] & mask) != mask) return FALSE;
  }
  return TRUE;
}

//--------------------------- ExistOne ----------------------------------------
BOOL CsTM::ExistOne(int nRow/*=-1*/) const
{
  ASSERT (nRow>=-1 && nRow < m_nSize);
  int k,first=0,last=m_nSize-1;
  if (nRow != -1) first=last=nRow;
  for (k=first; k <= last; k++) 
    if(m_pData[k] != 0) return TRUE;
  return FALSE;
}

//--------------------------- ExistOne ----------------------------------------
BOOL CsTM::ExistOne(const ULONG mask,int nRow/*=-1*/) const
{
  ASSERT (nRow>=-1 && nRow < m_nSize);
  int k,first=0,last=m_nSize-1;
  if (nRow != -1) first=last=nRow;
  for (k=first; k <= last; k++) 
    if((m_pData[k] & mask) != 0) return TRUE;
  return FALSE;
}

//--------------------------- ExistZero ---------------------------------------
BOOL CsTM::ExistZero(int nRow/*=-1*/) const
{
  ASSERT (nRow>=-1 && nRow < m_nSize);
  int k,first=0,last=m_nSize-1;
  if (nRow != -1) first=last=nRow;
  for (k=first; k <= last; k++) 
    if(m_pData[k+m_nMaxSize] != 0) return TRUE;
  return FALSE;
}

//--------------------------- ExistZero ---------------------------------------
BOOL CsTM::ExistZero(const ULONG mask,int nRow/*=-1*/) const
{
  ASSERT (nRow>=-1 && nRow < m_nSize);
  int k,first=0,last=m_nSize-1;
  if (nRow != -1) first=last=nRow;
  for (k=first; k <= last; k++) 
    if((m_pData[k+m_nMaxSize] & mask) != 0) return TRUE;
  return FALSE;
}

//----------------- SortDecr --------------------------------------------------
void CsTM::SortDecr(int nBeg/*=0*/,int number/*=-1*/)
{
  ASSERT(number == -1 || number > 1);
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
void CsTM::SortIncr(int nBeg/*=0*/,int number/*=-1*/)
{
  ASSERT(number == -1 || number > 1);
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
STD(BOOL) operator==(const CsTM& tm1,const CsTM& tm2)
{
  if ((tm1.m_nSize != tm2.m_nSize) || (tm1.m_nBitLength != tm2.m_nBitLength))
    return FALSE;
  int i;
  for (i=0;i < tm1.m_nSize;i++)
    if ((tm1.m_pData[i] != tm2.m_pData[i]) ||
       (tm1.m_pData[i+tm1.m_nMaxSize] != tm2.m_pData[i+tm2.m_nMaxSize])) 
      return FALSE;
  return TRUE;
}

//----------------- operator!= ------------------------------------------------
STD(BOOL) operator!=(const CsTM& tm1,const CsTM& tm2)
{
  if ((tm1.m_nSize != tm2.m_nSize) || (tm1.m_nBitLength != tm2.m_nBitLength))
   return TRUE;
  int i;
  for (i=0;i < tm1.m_nSize;i++)
    if ((tm1.m_pData[i] != tm2.m_pData[i]) ||
        (tm1.m_pData[i+tm1.m_nMaxSize] != tm2.m_pData[i+tm2.m_nMaxSize])) 
      return TRUE;
  return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////// Row compareing operations /////////////////////////////////
//----------------- AreEqual --------------------------------------------------
BOOL CsTM::AreEqual(int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (((m_pData[nRow1] != m_pData[nRow2]) ||
      (m_pData[nRow1+m_nMaxSize] != m_pData[nRow2+m_nMaxSize])))
    return FALSE;
  return TRUE;
}

//----------------- AreEqual --------------------------------------------------
BOOL CsTM::AreEqual(const ULONG mask,int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (((mask & m_pData[nRow1]) != (mask & m_pData[nRow2])) ||
      ((mask & m_pData[nRow1+m_nMaxSize]) != (mask &m_pData[nRow2+m_nMaxSize])))
    return FALSE;
  return TRUE;
}
//----------------- AreEqual --------------------------------------------------
BOOL CsTM::AreEqual(int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  if ((m_pData[nRow] != (ULONG)tv.m_bVect1) ||
      (m_pData[nRow+m_nMaxSize] != (ULONG)tv.m_bVect0))
    return FALSE;
  return TRUE;
}

//----------------- AreEqual --------------------------------------------------
BOOL CsTM::AreEqual(const ULONG mask,int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  if ((((m_pData[nRow+m_nMaxSize] ^ (ULONG)tv.m_bVect0)|
        (m_pData[nRow] ^ (ULONG)tv.m_bVect1))&mask) != 0)
    return FALSE;
  return TRUE;
}

//----------------- AreNotEqual -----------------------------------------------
BOOL CsTM::AreNotEqual(int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if ((m_pData[nRow1] != m_pData[nRow2]) ||
      (m_pData[nRow1+m_nMaxSize] != m_pData[nRow2+m_nMaxSize]))
    return TRUE;
  return FALSE;
}

//----------------- AreNotEqual -----------------------------------------------
BOOL CsTM::AreNotEqual(const ULONG mask,int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (((m_pData[nRow1+m_nMaxSize] ^ m_pData[nRow2+m_nMaxSize]) |
        ((m_pData[nRow1] ^ m_pData[nRow2]) & mask )) != 0)
    return TRUE;
  return FALSE;
}
//----------------- AreNotEqual -----------------------------------------------
BOOL CsTM::AreNotEqual(int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != tv.GetBitLength()) return TRUE;
  if ((m_pData[nRow] != (ULONG)tv.m_bVect1) ||
      (m_pData[nRow+m_nMaxSize] != (ULONG)tv.m_bVect0))
    return TRUE;
  return FALSE;
}

//----------------- AreNotEqual -----------------------------------------------
BOOL CsTM::AreNotEqual(const ULONG mask,int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != tv.GetBitLength()) return TRUE;
  if ((((m_pData[nRow+m_nMaxSize] ^ (ULONG)tv.m_bVect0)|
        (m_pData[nRow] ^ (ULONG)tv.m_bVect1))&mask) != 0)
    return TRUE;
  return FALSE;
}

//----------------- AreOrthog -------------------------------------------------
BOOL CsTM::AreOrthog(int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if ((m_pData[nRow1] & m_pData[nRow2+m_nMaxSize] ||
       m_pData[nRow1+m_nMaxSize] & m_pData[nRow2]))
    return TRUE;
  return FALSE;
}

//----------------- AreOrthog -------------------------------------------------
BOOL CsTM::AreOrthog(const ULONG mask,int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if((((m_pData[nRow1] & m_pData[nRow2+m_nMaxSize])|
       (m_pData[nRow1+m_nMaxSize] & m_pData[nRow2]))&mask) !=0) 
    return TRUE;
  return FALSE;
}

//----------------- AreOrthog -------------------------------------------------
BOOL CsTM::AreOrthog(int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  if ((m_pData[nRow] & (ULONG)tv.m_bVect0) ||
       (m_pData[nRow+m_nMaxSize] & (ULONG)tv.m_bVect1))
    return TRUE;
  return FALSE;
}

//----------------- AreOrthog -------------------------------------------------
BOOL CsTM::AreOrthog(const ULONG mask,int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  if ((((m_pData[nRow+m_nMaxSize] & (ULONG)tv.m_bVect1) |
        (m_pData[nRow] & (ULONG)tv.m_bVect0))&mask) != 0)
    return TRUE;
  return FALSE;
}

//----------------- AreOrthog -------------------------------------------------
BOOL CsTM::AreOrthog(int nRow,const CsBV& bv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != bv.GetBitLength()) return FALSE;
  if ((m_pData[nRow] & (~(ULONG)bv))||(m_pData[nRow+m_nMaxSize] & (ULONG)bv))
    return TRUE;
  return FALSE;
}

//----------------- AreOrthog -------------------------------------------------
BOOL CsTM::AreOrthog(const ULONG mask,int nRow,const CsBV& bv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != bv.GetBitLength()) return FALSE;
  if ((((m_pData[nRow+m_nMaxSize] & (ULONG)bv)|
        (m_pData[nRow] & (~(ULONG)bv)))&mask) != 0)
       return TRUE;
 return FALSE;
}

//----------------- AreNeighb -------------------------------------------------
BOOL CsTM::AreNeighb(int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ULONG mas1, mas0;
  mas0=m_pData[nRow1+m_nMaxSize]^m_pData[nRow2+m_nMaxSize];
  mas1=m_pData[nRow1]^m_pData[nRow2];
  if (mas0^mas1) return FALSE;
  if (COUNTLONG(mas1) != 1) return FALSE;
  return TRUE;
}

//----------------- AreNeighb -------------------------------------------------
BOOL CsTM::AreNeighb(const ULONG mask,int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ULONG mas1, mas0;
  mas0=m_pData[nRow1+m_nMaxSize]^m_pData[nRow2+m_nMaxSize];
  mas1=m_pData[nRow1]^m_pData[nRow2];
  if ((mas0^mas1)&mask) return FALSE;
  mas1&=mask;
  if (COUNTLONG(mas1) != 1) return FALSE;
  return TRUE;
}

//----------------- AreNeighb -------------------------------------------------
BOOL CsTM::AreNeighb(int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ULONG mas1, mas0;
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  mas0=m_pData[nRow+m_nMaxSize]^tv.m_bVect0;
  mas1=m_pData[nRow]^tv.m_bVect1;
  if (mas0^mas1) return FALSE;
  if (COUNTLONG(mas1) != 1) return FALSE;
  return TRUE;
}

//----------------- AreNeighb -------------------------------------------------
BOOL CsTM::AreNeighb(const ULONG mask,int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ULONG mas1, mas0;
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  mas0=m_pData[nRow+m_nMaxSize]^tv.m_bVect0;
  mas1=m_pData[nRow]^tv.m_bVect1;
  if ((mas0^mas1)&mask) return FALSE;
  mas1&=mask;
  if (COUNTLONG(mas1) != 1) return FALSE;
  return TRUE;
}

//----------------- AreConcens ------------------------------------------------
BOOL CsTM::AreConcens(int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  int i=0;
  ULONG mas;
  mas =((m_pData[nRow1+m_nMaxSize] & m_pData[nRow2]) |
        (m_pData[nRow1] & m_pData[nRow2+m_nMaxSize]));
  if((mas )!= 0) i = COUNTLONG(mas);
  if(i != 1) return FALSE;
  return TRUE;
}

//----------------- AreConcens ------------------------------------------------
BOOL CsTM::AreConcens(const ULONG mask,int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  int i=0;
  ULONG mas;
  mas=((m_pData[nRow1+m_nMaxSize] & m_pData[nRow2])| 
       (m_pData[nRow1] & m_pData[nRow2+m_nMaxSize]))&mask;
  if(mas != 0) i = COUNTLONG(mas);
  if(i != 1) return FALSE;
  return TRUE;
}

//----------------- AreConcens ------------------------------------------------
BOOL CsTM::AreConcens(int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  int i=0;
  ULONG  mas;
  mas=((m_pData[nRow+m_nMaxSize] & tv.m_bVect1)|
       (m_pData[nRow] & tv.m_bVect0));
  if(mas != 0) i = COUNTLONG(mas);
  if(i != 1) return FALSE;
  return TRUE;
}

//----------------- AreConcens ------------------------------------------------
BOOL CsTM::AreConcens(const ULONG mask,int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  int i=0;
  ULONG mas;
//  BYTE* pm = (BYTE*)&mas;
  mas=((m_pData[nRow+m_nMaxSize] & tv.m_bVect1)|
       (m_pData[nRow] & tv.m_bVect0))&mask;
  if(mas != 0) i = COUNTLONG(mas);
  if(i != 1) return FALSE;
  return TRUE;
}

//----------------- IsCover ---------------------------------------------------
BOOL CsTM::IsCover(int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if((((m_pData[nRow1+m_nMaxSize]&m_pData[nRow2+m_nMaxSize])^
      m_pData[nRow1+m_nMaxSize]) ==0) &&
     (((m_pData[nRow1]&m_pData[nRow2])^m_pData[nRow1]) == 0))  return TRUE;
  return FALSE;
}

//----------------- IsCover ---------------------------------------------------
BOOL CsTM::IsCover(const ULONG mask,int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if((((m_pData[nRow1+m_nMaxSize]&m_pData[nRow2+m_nMaxSize])^
      m_pData[nRow1+m_nMaxSize])|
     (((m_pData[nRow1]&m_pData[nRow2])^m_pData[nRow1])&mask)) !=0) return FALSE;
 return TRUE;
}

//----------------- IsCover ---------------------------------------------------
BOOL CsTM::IsCover(int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  if((((m_pData[nRow+m_nMaxSize]&tv.m_bVect0)^m_pData[nRow+m_nMaxSize]) ==0) &&
        (((m_pData[nRow]&tv.m_bVect1)^m_pData[nRow]) == 0)) return TRUE;
  return FALSE; 
}

//----------------- IsCover ---------------------------------------------------
BOOL CsTM::IsCover(const ULONG mask,int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  if((((m_pData[nRow+m_nMaxSize]&tv.m_bVect0) ^ m_pData[nRow+m_nMaxSize])|
       (((m_pData[nRow]&tv.m_bVect1) ^ m_pData[nRow])&mask)) !=0) 
     return FALSE;
  return TRUE;
}

//----------------- IsCover ---------------------------------------------------
BOOL CsTM::IsCover(int nRow,const CsBV& bv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != bv.GetBitLength()) return FALSE;
  if((((m_pData[nRow+m_nMaxSize]&(~(ULONG)bv))^m_pData[nRow+m_nMaxSize])==0) ||
       (((m_pData[nRow]&(ULONG)bv)^m_pData[nRow]) == 0)) return TRUE;
  return FALSE;
}

//----------------- IsCover ---------------------------------------------------
BOOL CsTM::IsCover(const ULONG mask,int nRow,const CsBV& bv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != bv.GetBitLength()) return FALSE;
  if(((((m_pData[nRow+m_nMaxSize]&(~(ULONG)bv)) ^ m_pData[nRow+m_nMaxSize])|
      ((m_pData[nRow]&(ULONG)bv)^m_pData[nRow]))&mask) !=0) return FALSE;
  return TRUE;
}

//----------------- IsCovered -------------------------------------------------
BOOL CsTM::IsCovered(int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  if((((m_pData[nRow+m_nMaxSize]&tv.m_bVect0)^tv.m_bVect0) ==0) &&
      (((m_pData[nRow]&tv.m_bVect1)^tv.m_bVect1) == 0)) return TRUE;
  return FALSE;
}

//----------------- IsCovered -------------------------------------------------
BOOL CsTM::IsCovered(const ULONG mask,int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  if (m_nBitLength != tv.GetBitLength()) return FALSE;
  if((((m_pData[nRow+m_nMaxSize]&tv.m_bVect0)^tv.m_bVect0) |
      (((m_pData[nRow]&tv.m_bVect1)^tv.m_bVect1)&mask)) !=0) return FALSE;
  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////// Row interval operations ///////////////////////////////////
//--------------------------- Orthogon ----------------------------------------
CsBV CsTM::Orthogon(int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsBV bvTag(m_nBitLength,FALSE);
  bvTag = ((m_pData[nRow1] & m_pData[nRow2+m_nMaxSize]) |
          (m_pData[nRow1+m_nMaxSize] & m_pData[nRow2])); 
  return bvTag;
}

//--------------------------- Orthogon ----------------------------------------
CsBV CsTM::Orthogon(const ULONG mask,int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsBV bvTag(m_nBitLength,FALSE);
  bvTag = (((m_pData[nRow1] & m_pData[nRow2+m_nMaxSize]) |
            (m_pData[nRow1+m_nMaxSize] & m_pData[nRow2]))& mask);
  return bvTag;
}

//--------------------------- Orthogon ----------------------------------------
CsBV CsTM::Orthogon(int nRow,const CsTV& tv) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsBV bvTag(m_nBitLength,FALSE);
  bvTag = ((m_pData[nRow] & tv.m_bVect0) |
           (m_pData[nRow+m_nMaxSize] & tv.m_bVect1));
  return bvTag;
}

//--------------------------- Orthogon ----------------------------------------
CsBV CsTM::Orthogon(const ULONG mask,int nRow,const CsTV& tv) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsBV bvTag(m_nBitLength,FALSE);
  bvTag = ((m_pData[nRow]&tv.m_bVect0)|
           (m_pData[nRow+m_nMaxSize]&tv.m_bVect1))&mask;
  return bvTag;
}

//--------------------------- Orthogon ----------------------------------------
CsBV CsTM::Orthogon(int nRow,const CsBV& bv) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsBV bvTag(m_nBitLength,FALSE);
  bvTag = (m_pData[nRow]&(~(ULONG)bv))|(m_pData[nRow+m_nMaxSize]&(ULONG)bv);
  return bvTag;
}

//--------------------------- Orthogon ----------------------------------------
CsBV CsTM::Orthogon(const ULONG mask,int nRow,const CsBV& bv) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsBV bvTag(m_nBitLength,FALSE);
  bvTag = ((m_pData[nRow]&(~(ULONG)bv))|
    (m_pData[nRow+m_nMaxSize]&(ULONG)bv)) & mask;
  return bvTag;
}

//--------------------------- Intersec ----------------------------------------
CsTV CsTM::Intersec(int nRow1,int nRow2) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsTV tvTag(m_nBitLength);
  tvTag.m_bVect1 = m_pData[nRow1] | m_pData[nRow2];
  tvTag.m_bVect0 = m_pData[nRow1+m_nMaxSize] | m_pData[nRow2+m_nMaxSize];
  return tvTag;
}

//--------------------------- Intersec ----------------------------------------
CsTV CsTM::Intersec(const ULONG mask,int nRow1,int nRow2) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsTV tvTag(m_nBitLength);
  tvTag.m_bVect1 = (m_pData[nRow1]|m_pData[nRow2]) & mask;
  tvTag.m_bVect0 =(m_pData[nRow1+m_nMaxSize]|
                   m_pData[nRow2+m_nMaxSize])&mask;
  return tvTag;
}

//--------------------------- Intersec ----------------------------------------
CsTV CsTM::Intersec(int nRow,const CsTV& tv) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsTV tvTag(m_nBitLength);
  tvTag.m_bVect1 = m_pData[nRow] | tv.m_bVect1;
  tvTag.m_bVect0 = m_pData[nRow+m_nMaxSize] | tv.m_bVect0;
  return tvTag;
}

//--------------------------- Intersec ----------------------------------------
CsTV CsTM::Intersec(const ULONG mask,int nRow,const CsTV& tv) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsTV tvTag(m_nBitLength);
  tvTag.m_bVect1 = (m_pData[nRow] | tv.m_bVect1) & mask;
  tvTag.m_bVect0 =(m_pData[nRow+m_nMaxSize] | tv.m_bVect0) & mask;
  return tvTag;
}

//--------------------------- Concens -----------------------------------------
CsTV CsTM::Concens(int nRow1,int nRow2) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ULONG maska;
  CsTV tvTag(m_nBitLength);
  maska=(m_pData[nRow1] & m_pData[nRow2+m_nMaxSize])|
        (m_pData[nRow1+m_nMaxSize] & m_pData[nRow2]);
  tvTag.m_bVect1 = (m_pData[nRow1]|m_pData[nRow2])^maska;
  tvTag.m_bVect0 = (m_pData[nRow1+m_nMaxSize]|
                    m_pData[nRow2+m_nMaxSize])^maska;
  return tvTag;
}

//--------------------------- Concens -----------------------------------------
CsTV CsTM::Concens(const ULONG mask,int nRow1,int nRow2) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ULONG maska;
  CsTV tvTag(m_nBitLength);
  maska=(m_pData[nRow1] & m_pData[nRow2+m_nMaxSize])|
        (m_pData[nRow1+m_nMaxSize] & m_pData[nRow2]);
  tvTag.m_bVect1 = ((m_pData[nRow1]|m_pData[nRow2])^maska)&mask;
  tvTag.m_bVect0 = ((m_pData[nRow1+m_nMaxSize]|
                     m_pData[nRow2+m_nMaxSize])^maska)&mask;
  return tvTag;
}

//--------------------------- Concens -----------------------------------------
CsTV CsTM::Concens(int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ULONG maska;
  CsTV tvTag(m_nBitLength);
  maska=(m_pData[nRow] & tv.m_bVect0)|
        (m_pData[nRow+m_nMaxSize]&tv.m_bVect1);
  tvTag.m_bVect1 = (m_pData[nRow]|tv.m_bVect1)^maska;
  tvTag.m_bVect0 = (m_pData[nRow+m_nMaxSize]|tv.m_bVect0)^maska;
  return tvTag;
}

//--------------------------- Concens -----------------------------------------
CsTV CsTM::Concens(const ULONG mask,int nRow,const CsTV& tv) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ULONG maska;
  CsTV tvTag(m_nBitLength);
  maska=(m_pData[nRow]&tv.m_bVect0) | (m_pData[nRow+m_nMaxSize]&tv.m_bVect1);
  tvTag.m_bVect1 = ((m_pData[nRow]|tv.m_bVect1)^maska)&mask;
  tvTag.m_bVect0 = ((m_pData[nRow+m_nMaxSize]|tv.m_bVect0)^maska)&mask;
  return tvTag;
}

//--------------------------- Concens -----------------------------------------
CsTV CsTM::Concens(int nRow,const CsBV& bv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ULONG maska;
  CsTV tvTag(m_nBitLength);
  maska = ~((m_pData[nRow] & (~(ULONG)bv))|
            (m_pData[nRow+m_nMaxSize] & (ULONG)bv));
  tvTag.m_bVect1 = (ULONG)bv & maska;
  tvTag.m_bVect0 = (~(ULONG)bv) & maska;
  return tvTag;
}

//--------------------------- Concens -----------------------------------------
CsTV CsTM::Concens(const ULONG mask,int nRow,const CsBV& bv) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ULONG maska;
  CsTV tvTag(m_nBitLength);
  maska=(~((m_pData[nRow]&(~(ULONG)bv)) |
           (m_pData[nRow+m_nMaxSize]&(ULONG)bv))) & mask;
  tvTag.m_bVect1 = (ULONG)bv & maska;
  tvTag.m_bVect0 = (~(ULONG)bv) & maska;
  return tvTag;
}


//--------------------------- MinCover ----------------------------------------
CsTV CsTM::MinCover(int nRow1,int nRow2) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsTV tvTag(m_nBitLength);
  tvTag.m_bVect1 = m_pData[nRow1] & m_pData[nRow2];
  tvTag.m_bVect0 = m_pData[nRow1+m_nMaxSize] & m_pData[nRow2+m_nMaxSize];
  return tvTag;
}

//--------------------------- MinCover ----------------------------------------
CsTV CsTM::MinCover(const ULONG mask,int nRow1,int nRow2) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsTV tvTag(m_nBitLength);
  tvTag.m_bVect1 = m_pData[nRow1] & m_pData[nRow2] & mask;
  tvTag.m_bVect0 = m_pData[nRow1+m_nMaxSize]&m_pData[nRow2+m_nMaxSize]&mask;
  return tvTag;
}

//--------------------------- MinCover ----------------------------------------
CsTV CsTM::MinCover(int nRow,const CsTV& tv) const
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsTV tvTag(m_nBitLength);
  tvTag.m_bVect1 = m_pData[nRow] & tv.m_bVect1;
  tvTag.m_bVect0 = m_pData[nRow+m_nMaxSize] & tv.m_bVect0;
  return tvTag;
}

//--------------------------- MinCover ----------------------------------------
CsTV CsTM::MinCover(const ULONG mask,int nRow,const CsTV& tv) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsTV tvTag(m_nBitLength);
  tvTag.m_bVect1 = m_pData[nRow] & tv.m_bVect1 & mask;
  tvTag.m_bVect0 = m_pData[nRow+m_nMaxSize] & tv.m_bVect0 & mask;
  return tvTag;
}

//--------------------------- MinCover ----------------------------------------
CsTV CsTM::MinCover(int nRow,const CsBV& bv) const    //мн-во ортогон.компонент
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsTV tvTag(m_nBitLength);
  tvTag.m_bVect1 = m_pData[nRow] & (ULONG)bv;
  tvTag.m_bVect0 = m_pData[nRow+m_nMaxSize] & (~(ULONG)bv);
  return tvTag;
}

//--------------------------- MinCover ----------------------------------------
CsTV CsTM::MinCover(const ULONG mask,int nRow,const CsBV& bv) const    
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  CsTV tvTag(m_nBitLength);
  tvTag.m_bVect1 = m_pData[nRow] & (ULONG)bv & mask;
  tvTag.m_bVect0 = m_pData[nRow+m_nMaxSize] & (~(ULONG)bv) & mask;
  return tvTag;
}

//----------------- CsTMtoCBM --------------------------------------------------
CsBM CsTM::CsTMtoCsBM(void) const    // раскрыть интервалы
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  int i;
  CsBM bm(m_nBitLength,0);
  CsBM bmTag(m_nSize,m_nBitLength);
  CsTV tv(m_nBitLength);
  for(i=0; i < m_nSize; i++) {
    tv=GetRowTv(i); bm=tv.CsTVtoCsBM();   
    bmTag.Add(bm,-1);
  }  
  return bmTag;
}

//////////////////// Operations on TERNARY VECTOR PARTS ///////////////////////
/////////////////////////// Row Logic operations //////////////////////////////
//--------------------------- AndInPlace --------------------------------------
void CsTM::AndInPlace(BOOL Part,const CsBV& bv,int Row)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(m_nBitLength == bv.GetBitLength());
  if(Part)    m_pData[Row] &= (ULONG)bv;
  else        m_pData[Row+m_nMaxSize] &= (ULONG)bv;
}   

//--------------------------- OrInPlace ---------------------------------------
void CsTM::OrInPlace(BOOL Part,const CsBV& bv,int Row)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(m_nBitLength == bv.GetBitLength());
  if(Part)    m_pData[Row] |= (ULONG)bv;
  else        m_pData[Row+m_nMaxSize] |= (ULONG)bv;
}   

//--------------------------- XorInPlace --------------------------------------
void CsTM::XorInPlace(BOOL Part,const CsBV& bv,int Row)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(m_nBitLength == bv.GetBitLength());
  if(Part)    m_pData[Row] ^= (ULONG)bv;
  else        m_pData[Row+m_nMaxSize] ^= (ULONG)bv;
}   

//--------------------------- SubInPlace --------------------------------------
void CsTM::SubInPlace(BOOL Part,const CsBV& bv,int Row)
{
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  ASSERT(m_nBitLength == bv.GetBitLength());
  if(Part)    m_pData[Row] &= ~(ULONG)bv;
  else        m_pData[Row+m_nMaxSize] &= ~(ULONG)bv;
} 

//--------------------------- InvertBitsInPlace -------------------------------
void CsTM::InvertBitsInPlace(BOOL Part)
{
  ASSERT(m_nBitLength != 0);
  ULONG mask = 0xffffffff >> (32 - m_nBitLength) << (32 - m_nBitLength);
  int i;
  if(Part)
    for (i=0;i < m_nSize; i++) {
     m_pData[i] = (~m_pData[i]) & mask;
    } 
  else 
   for (i=0;i < m_nSize; i++) {
     m_pData[i] = (~m_pData[i+m_nMaxSize]) & mask;
   }
  return;
}   

//--------------------------- InvertBitsInPlace -------------------------------
void CsTM::InvertBitsInPlace(BOOL Part,int Row)
{
  ASSERT(m_nBitLength != 0);
  ULONG mask = 0xffffffff >> (32 - m_nBitLength) << (32 - m_nBitLength);
  if(Part)  m_pData[Row] = (~m_pData[Row]) & mask;
  else      m_pData[Row] = (~m_pData[Row+m_nMaxSize]) & mask;
}   

/*
#ifndef _LINUX
///////////////////////////////////////////////////////////////////////////////
// Serialization

//--------------------------------------------------Serialize(CArchive& ar)
void CsTM::Serialize(CArchive& ar)
{ 
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  int i;
  
  if (ar.IsStoring()) {
    ar << m_nSize;
    ar << m_nBitLength;
    for (i=0; i<m_nSize; i++) { 
      ar << m_pData[i];
      ar << m_pData[i+m_nMaxSize];
    }
  }
  else {
    ar >> m_nSize;
    ar >> m_nBitLength;
    SetSize(m_nSize, m_nBitLength);
    for (i=0; i<m_nSize; i++) { 
      ar >> m_pData[i];
      ar >> m_pData[i+m_nMaxSize];
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Diagnostics

//------------------------------------------------Dump(CDumpContext& dc) const
#ifdef _DEBUG
void CsTM::Dump(CDumpContext& dc) const { }

//---------------------------------------------------AssertValid() const
void CsTM::AssertValid() const
{
  if (m_pData == NULL) {
    ASSERT(m_nSize >= 0);
    ASSERT(m_nMaxSize >= 0);
    ASSERT(m_nBitLength >= 0);
  }
  else {
    ASSERT(m_nSize >= 0);
    ASSERT(m_nMaxSize >= 0);
    ASSERT(m_nSize <= m_nMaxSize);
    ASSERT(AfxIsValidAddress(m_pData, m_nMaxSize * sizeof(BYTE*)));
    ASSERT(m_nBitLength >= 0);
    ASSERT(m_nGrowBy >= 0);
  }
}
#endif //_DEBUG
#endif //_LINUX
*/

//--------------------------------------------------Serialize(CArch& ar)
void CsTM::Serialize(CArch& ar)
{ 
#ifdef _DEBUG
  ASSERT_VALID(this);
#endif
  int i;

  if (ar.IsStoring()) {
    ar << m_nSize;
    ar << m_nBitLength;
    for (i=0; i<m_nSize; i++) { 
      ar << m_pData[i];
      ar << m_pData[i+m_nMaxSize];
    }
  }
  else {
    ar >> m_nSize;
    ar >> m_nBitLength;
    SetSize(m_nSize, m_nBitLength);
    for (i=0; i<m_nSize; i++) { 
      ar >> m_pData[i];
      ar >> m_pData[i+m_nMaxSize];
    }
  }
}



///////////////////////////////////////////////////////////////////////////////
//********************* Protected functions ***********************************
//---------------------- AllocMatrix-------------------------------------------
void CsTM::AllocMatrix(int nRow, int nColumn)
{ 
  ASSERT(nRow >= 0 && nColumn >= 0 && nColumn<=32);
  m_pData = (ULONG*) new ULONG[nRow * 2];
  memset((BYTE*)&m_pData[0], 0, nRow * sizeof(ULONG)*2);    // zero fill
  m_nSize = m_nMaxSize = nRow;
  m_nBitLength = nColumn;
}
/*
#ifndef _LINUX
//----------------- CharBit ---------------------------------------------------
void CsTM::CharBit(const CString s)
{
  int j = 0,i = 0,max = 0;
  CString w = s;
  if (w.GetLength() <= 2) { Init(); return; }
  if (w[w.GetLength()-2] != '\r') w = w + "\r\n";
  while (!w.IsEmpty()) {
    j = w.Find('\n');
    if (j > max + 1) max = j - 1;
    w = w.Right(w.GetLength() - j-1);
    i++;
  }
  if (max>32) { Init(); return; }
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
void CsTM::CharBit(const CStringArray& s)   //new 11.02
{
  int k,i,max = 0;
  if (s.GetSize()==0) { Init(); return;}
  for (k=0; k < s.GetSize(); k++) {
    i = s[k].GetLength();
    if (i > max) max = i;    // max - length of row
  }  
  if (max>32) { Init(); return; }
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
#else
*/
//----------------- CharBit ---------------------------------------------------
void CsTM::CharBit(char* s)
{
  int j = 0,i = 0,max = 0, len;
  len = strlen(s);
  char* w, *pdest;
  w =(char*)malloc(len+2);
  strcpy(w,s);
  if (len <= 2) { Init(); return; }
  if (w[len-2] != '\r') strcat(w,"\r\n");
  while (w[0] !=0) {
    pdest = strchr(w,'\n');
    j = (int)(pdest - w + 1);
    if (j > max + 1) max = j - 1;
    strcpy(w, pdest+1);
    i++;
  }

  if (m_pData == NULL) AllocMatrix(i,max);
  else { SetSize(i,max,m_nGrowBy); Clear(); }
  strcpy(w,s);  j = 0;
  if (w[len-2] != '\r') strcat(w,"\r\n");
  while (w[0]!='\0') {
    pdest = strchr(w,'\n');
    max = (int)(pdest - w + 1);
    for(i=0;i < max-1;i++) {
      ASSERT(w[i] == '0' || w[i] == '1' || w[i] == '-' || w[i] == '+');
      if(w[i] != '-') SetBitAt(j,i,w[i]);
    } 
    strcpy(w, pdest+1);
    j++;
  }
}    

//----------------- CharBit ---------------------------------------------------
void CsTM::CharBit(const vector <string>& s)   //new 11.02
{
  int k,i,max = 0;
  if (s.size()==0) { Init(); return;}
  for (k=0; k < s.size(); k++) {
    i = s[k].length();
    if (i > max) max = i;    // max - length of row
  }  
  if (max>32) { Init(); return; }
  if (m_pData == NULL) AllocMatrix(s.size(),max);
  else { SetSize(s.size(),max,m_nGrowBy); Clear(); }
  for (k=0; k < s.size(); k++) { 
    max = s[k].length();
    string w(s[k]);
    for(i=0;i < max;i++) {
      ASSERT(w[i] == '0' || w[i] == '1' || w[i] == '-' || w[i] == '+');
      if (w[i] != '-') SetBitAt(k,i,w[i]);
    }
  }
}
//#endif

//----------------- SafeDelete() ----------------------------------------------
void CsTM::SafeDelete()
{
  if(m_nMaxSize > 0) delete [] (ULONG*)m_pData;
  m_pData=NULL;
}

//__________________________________________________________
// Конвертирование Tm из стандартного  представления 
// в "короткострочное"(TmS)  
//__________________________________________________________

void CsTM::ToShort(CTM &Tm)
{
  ASSERT(Tm.GetCountC()<=32);
  int i;
  CsTV stv;
  RemoveAll();
  for (i=0; i<Tm.GetCountR(); i++) {
    stv.ToShort(Tm.GetRowTv(i)); 
    Add(stv);
  }
  return;
}

//__________________________________________________________
// Конвертирование "короткострочноuго" представления 
// троичной матрицы (TmS) в стандартноe (Tm)  
//__________________________________________________________

void CsTM::FromShort(CTM &Tm)
{
  int i, j;
  BYTE w1[4];
  BYTE w0[4];

  Tm.SetSize(m_nSize,m_nBitLength);
  for (i=0; i<m_nSize; i++)
  {
    for(j=0; j*8<m_nBitLength; j++)
    {  
      w1[j] = (BYTE)((GetLongOne(i)<<(j*8))>>24);
      w0[j] = (BYTE)((GetLongZero(i)<<(j*8))>>24);
    }
    Tm.SetRowOne(i, (BYTE*) &w1[0]);
    Tm.SetRowZero(i, (BYTE*) &w0[0]);
  }
  return;
}


//***********************************************************************
// Дизъюнкция двух ДНФ с использованием интервальных
// операций поглощения и обобщенного склеивания для упрощения
//***********************************************************************

void CsTM::DorD(int Regime, CsTM Tm1)
{
  int a, e, i, k;
  CsTV tva, tvc;           //CString sTm1, sTm2, stva, stvc;//sTm1=Tm1.BitChar();//sTm2=Tm2.BitChar();
  if(Regime<1)
  {
    for(i=0; i<Tm1.GetCountR(); i++) Add(Tm1.GetRowTv(i)); return;
  }
  while ( (k=Tm1.GetCountR()) != 0)
  {
    tva=Tm1.GetRowTv(k-1);
//stva=tva.BitChar();
    Tm1.RemoveRow(k-1); 
    for(i=0, e=1; i<GetCountR(); i++)
    {
      if(IsCover(i,tva)) {e=0; break;}
      if(IsCovered(i,tva))
      { RemoveRow(i); i--; continue;} 
      if(Regime<2) continue;
      if(AreConcens(i,tva))
      {
        a=0; tvc=Concens(i,tva);  //stvc=tvc.BitChar();
        if (tvc.IsCover(tva)) a+=1;
        if(IsCovered(i,tvc)) a+=2;
        if(a%2) {tva=tvc; if(a>1) RemoveRow(i); i=-1; continue;}
        else 
          if(a)//{Tm1.Add(tvc); Tm2.RemoveRow(i); i--;}
            { if(Regime>2) {Tm1.Add(tvc); RemoveRow(i); i--;}
            else SetRowTv(i,tvc); } 
        
      }
    }
    if (e) Add(tva);    //sTm1=Tm1.BitChar();//sTm2=Tm2.BitChar();
  }        //sTm2=Tm2.BitChar();
}

//***********************************************************************
// Дизъюнкция конъюнкции tva и ДНФ Tm с использованием интервальных
// операций поглощения и обобщенного склеивания для упрощения
//***********************************************************************

void CsTM::DorD(int Regime, CsTV tva)
{
  int a, e, i;
  CsTV tvc;                 //CString sTm, stva, stvc;  //sTm=Tm.BitChar();
  if(Regime<1)
  { Add(tva); return;}                      //stva=tva.BitChar();
  for(i=0, e=1; i< GetCountR(); i++)
  {
    if(IsCover(i,tva)) {e=0; break;}
    if(IsCovered(i,tva))
    { RemoveRow(i); i--; continue;} 
    if(Regime<2) continue;
    if(AreConcens(i,tva))
    {
      a=0; tvc= Concens(i,tva);            //stvc=tvc.BitChar();
      if (tvc.IsCover(tva)) a+=1;
      if(IsCovered(i,tvc)) a+=2;
      if(a%2) {tva=tvc; if(a>1) RemoveRow(i); i=-1; continue;}
      else 
        //if(a)//{Tm1.Add(tvc); Tm.RemoveRow(i); i--;}
        //if(Regime>2) {tva=tvc; Tm.RemoveRow(i); i=-1;}
        //else Tm.SetRowTv(i,tvc);
        if(a) 
        {
          if(Regime>2) {Add(tvc); RemoveRow(i); i--;}
          else SetRowTv(i,tvc); 
        }
    }
  }   //stva=tva.BitChar();   //sTm=Tm.BitChar();
  if (e) Add(tva);  //sTm=Tm.BitChar();
}
