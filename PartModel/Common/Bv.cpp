/////////////////////////////////////////////////////////////////
// Ver.2.1.7    21.12.2007
//              Новая реализация метода генерации BV - GenRbv
// Ver.2.1.6    18.12.2007
//              Новый метод генерации чисел в LINUX GetRandN
// Ver.2.1.5    09.11.2007
//              Новый метод генерации векторов GenRbvN
// Ver.2.1.4    08.10.2007
//              Gen вектора - новая реализация (только для LINUX)
// Ver.2.1.3    23.09.2007
//              GenRbvMid - новая реализация
// Ver.2.1.2    21.02.2007
//              AssignCopy - добавил чистку пространства от Len до AllocLen
// Ver.2.1.1     20.07.2006
//               разнообразие генераторов
// Ver.2.1.0    07.07.2006
//              Переход к LINUX

/////////////////////////////////////////////////////////////////
// Ver.2.0.6     19.06.2006
//               Поправки BitChar с ограничением длины строки
// Ver.2.0.2     24.03.2006
//               поправка операции чтения из архива (отказ от WORD)
// Ver.2.0.1     21.03.2006
//               - Генераторы случайных перестроены на возврат *this
//               GetRandV сделан внеклассовым
//               Поправка в SetSize - сразу возврат, если новый равен старому
// Ver.2.0.0     17.03.2006   Russian == English
//               - Добавлены генераторы случайных

// Ver.1.1.1    08.03.2005   Russian == English
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <limits.h>

#ifdef _JOINT
#include "BaseBool.h"
#include "archive.h"
#else
#include "../Common/BaseBool.h"
#include "../Com_Lib/archive.h"
#endif

#ifndef _LINUX
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif //_DEBUG
#endif //_LINUX


static unsigned long Rgrain =1;
static BOOL NewRand = TRUE;

///////////////////////////////////////////////////////////
// Установить новое "зерно" для базового генератора      //
///////////////////////////////////////////////////////////

void SetRgrain(unsigned long NewRgrain ) {Rgrain = NewRgrain;}

///////////////////////////////////////////////////////////
// Взять текущее "зерно" базового генератора             //
///////////////////////////////////////////////////////////

unsigned long GetRgrain() {return (Rgrain);}

///////////////////////////////////////////////////////////
// Установить новый режим генерации                      //
///////////////////////////////////////////////////////////
void SetRandMode(BOOL Fl/* = TRUE*/) {NewRand = Fl; }

///////////////////////////////////////////////////////////
// Взять текущий режим генерации                         //
///////////////////////////////////////////////////////////
BOOL GetRandMode() { return NewRand; }

//////////////////////////////////////////////////////////// GetRandN
// Получение очередного псевдослучайного целого числа     //
// 32 или 16-разрядного(Романов - переход к 32 от 16 - 10.03.2006)
// Корректно работающий вариант Томашева - 27 января 1999 //
////////////////////////////////////////////////////////////
unsigned int GetRandN()

{ unsigned long  f13 = 1220703125;  // f13 = 5**13
  unsigned long   m  = 0x7fffffff;  //  m  = 2**31-1
  unsigned long   w;
//#ifndef _LINUX
//  __asm mov eax,Rgrain   //
//  __asm mul f13          //  Rgrain=(Rgrain*f13)(mod(2**31-1)
//  __asm div m            //   (непосредсвенно на ассемблере)
//  __asm mov Rgrain,edx   //                              V.T.
//  w = (Rgrain<<8)>>16;
//  if (GetRandMode())
//  {
//  __asm mov eax,Rgrain   //
//  __asm mul f13          //  Rgrain=(Rgrain*f13)(mod(2**31-1)
//  __asm div m            //   (непосредсвенно на ассемблере)
//  __asm mov Rgrain,edx   //                              V.T.
//    w = (w<<16)|((Rgrain<<8)>>16);
//  }
//#else

/* asm(
    "movl Rgrain, %eax\n"
    "mull -4(%ebp)\n"
    "divl -8(%ebp)\n"
    "movl %edx, Rgrain"
 );*/
  Rgrain = (Rgrain*f13)%m;
//  printf ("Rgrain%x\n",Rgrain);
  w = (Rgrain<<40)>>48;
//  printf ("w%x\n",w);
  if (GetRandMode())
  {
/* asm(
    "movl Rgrain, %eax\n"
    "mull -4(%ebp)\n"
    "divl -8(%ebp)\n"
    "movl %edx, Rgrain"
 );*/
    Rgrain = (Rgrain*f13)%m;
//    printf ("Rgrain%x\n",Rgrain);

    w = (w<<16)|((Rgrain<<40)>>48);
    }
//#endif
//    printf("%x\n",w);
  return (w);
}

//---------------------------------------------------------GetRandV
//  Получение псевдослучайного 16-разрядного булева вектора
//  Rgrain - глобальная перем.= тек.сост.генератора
//  Rgrain = (Rgrain*(5**13))[mod(2**31-1)]
//    Vect - 16-разрядный базовый булев вектор
//  *** Модификация Томашева - 27 января 1999
//---------------------------------------------------------
CBV GetRandV()
{
  CBV Vect(0,GetRandMode()?32:16,TRUE);

  BYTE Work[4];
  unsigned long   f13 = 1220703125;  // f13 = 5**13
  unsigned long  d, m = 0x7fffffff;  //  m  = 2**31-1
//#ifndef _LINUX
// __asm mov eax,Rgrain         //
// __asm mul f13                //  Rgrain=(Rgrain*f13)[mod(2**31-1)]
// __asm div m                  //  (непосредсвенно на ассемблере)
// __asm mov d,edx              //                              V.T.
//
//  Rgrain = d;     // Rgrain - глобальная перем.= тек.сост.генератора
//  Work[0] = (BYTE)( d >> 16); Work[1] = (BYTE)( d >> 8);
//
//  if (GetRandMode())
//  {
//    __asm mov eax,Rgrain         //
//    __asm mul f13                //  Rgrain=(Rgrain*f13)[mod(2**31-1)]
//    __asm div m                  //  (непосредсвенно на ассемблере)
//    __asm mov d,edx              //                              V.T.
//
//    Rgrain = d;     // Rgrain - глобальная перем.= тек.сост.генератора
//    Work[2] = (BYTE)( d >> 16); Work[3] = (BYTE)( d >> 8);
//  }
//#else
 /*asm(
        "movl Rgrain, %eax\n"
        "mull -32(%ebp)\n"
        "divl -40(%ebp)\n"
        "movl %edx, -36(%ebp)"
 );*/
  d = (Rgrain*f13)%m;

  Rgrain = d;     // Rgrain - глобальная перем.= тек.сост.генератора
  Work[0] = (BYTE)( d >> 16); Work[1] = (BYTE)( d >> 8);

  if (GetRandMode())
  {
/* asm(
        "movl Rgrain, %eax\n"
        "mull -32(%ebp)\n"
        "divl -40(%ebp)\n"
        "movl %edx, -36(%ebp)"
 );*/
    d = (Rgrain*f13)%m;
    Rgrain = d;     // Rgrain - глобальная перем.= тек.сост.генератора
    Work[2] = (BYTE)( d >> 16); Work[3] = (BYTE)( d >> 8);
    }
//#endif
  Vect = (const BYTE*) Work;

  return Vect;
}


//********************************************************   GenRbv
// Генерация псевдослучайного <nCol>-компонентного булева //
// вектора с равновероятным распределением нулей и единиц //
//    Модификация Томашева - 27 января 1999
//---------------------------------------------------------
CBV CBV::GenRbv (int nCol)
{
  Empty();
  int a, b, hh, h, i, j, n;
  a=(GetRandMode())?32:16;
  b=a/8;  hh=a-8;
  n=(nCol/a)+((nCol%a)?1:0);
  unsigned int nn;
  m_nBitLength = n*a;
  m_nByteLength = m_nAllocLength = LEN_BYTE(n*a);
  m_bVect = new unsigned char[m_nByteLength];
  for (i=0; i<n-1; i++)
  {
    nn=GetRandN();
    for(h=hh, j=0; j<b; j++, h-=8)
      m_bVect[b*i+j] =  (nn>>h & 255);
  }

  nn=(GetRandN()>> (a-nCol)) << (a-nCol);
  for(h=hh, j=0; j<b; j++, h-=8)
    m_bVect[b*i+j] =  (nn>>h & 255);

  if (m_nBitLength != nCol) {
    m_nBitLength = nCol;
    m_nByteLength = LEN_BYTE(nCol);
//    if ((i = ADR_BIT(m_nBitLength)) > 0 ) i = S_1 - i;
//    m_bVect[m_nByteLength-1] = (m_bVect[m_nByteLength-1] >>i) <<i;
  }
  return *this;
}

//*********************************** GenRbv ****************
// Ускоренное генерирование псевдослучайного <n>-компонентного
// булева вектора с равновероятным распределением нулей и единиц
//---------------------------------------------------------
CBV CBV::GenRbvN(int n)
{ int i, k,m;
  unsigned long *Syn;
  Empty();
  m_nBitLength = n;
  m_nByteLength = m_nAllocLength = LEN_BYTE(n);
  m_bVect = new unsigned char[m_nByteLength];
  k = LEN_LONG(n);
  Syn = (unsigned long *) (m_bVect);
  for(i=0; i<k-1; i++)
    Syn[i] = GetRandN();
  m = S_4 - ADR_BITLONG(n);
  Syn[i] = GetRandN() << m >> m;
  return *this;
}


//____________________________________________________________
// Генерирование псевдослучайного <nCol>-компонентного
// булева вектора со средним числом <nRang> единиц.
// (при nRang>=nCol все компоненты вектора равны 1,
// а при nRang=0 все компоненты вектора равны 0).
// Изменен с целью минимизации дисперсии (1/32) // Исправлен 02.10.08
// Изменен для случая nRang>=nCol // Исправлен 08.11.08
//////////////////////////////////////////////////////////////
CBV CBV::GenRbvMid(int nCol, int nRang)
{
  //int i, j, rr=(32*nRang)/nCol;
  int i, j, rr=(nRang<nCol) ? (32*nRang)/nCol : 31;
  CBV bvM((BYTE)0,nCol), bv;
  if(rr)
  {
/*    bvM.One();
    if(rr<32)
      for(i=1; i<32; i=i<<1)
      {
        bv.GenRbv(nCol);
        if(i & rr) bvM|= bv;
        else bvM&= bv;
      }
  }
*/
    CBV bvP(bvM);
    for(i=1; i<6; i++)
    {
      if(rr & (1<<(5-i)))
      {
        for(j=0, bvP.One(); j<i; j++)
        {
          bv.GenRbv(nCol);
          bvP&= bv;
        }
      }
      bvM|= bvP;
    }
  }
  return (*this=bvM);
}

////////////////////////////////////////////////////////////
// Генерация псевдослучайного <nCol>-компонентного булева //
// вектора с фиксированным числом <nRang> единиц
// (при nRang>=nCol все компоненты вектора равны 1)
////////////////////////////////////////////////////////////

CBV CBV::GenRbvFix (int nCol, int nRang)
{
  SetSize(nCol);
  if(nRang>=nCol) { One(); return *this; }
  Zero();
  int j; unsigned int k;
  for ( j = 0; j <nRang; j++)
  {
    k = GetRandN() % nCol;
    if (GetBitAt( k )) j--;
    SetBitAt(k, 1);
  }
  return *this;
}


/////////////////////////////////////////////////////////////////////////////////////////
//***************************************************************************************
//////////////////////// Construction/Destruction ///////////////////////////////////////

//****************************** Protected function ***********************************//

//------------- AllocCopy(CBV& dest, int nCopyLen, int nCopyIndex,int nExtraLen) const
void CBV::AllocCopy(CBV& dest, int nCopyLen, int nCopyIndex,int nExtraLen) const
{int nNewLen = nCopyLen + nExtraLen;
 if (nNewLen == 0) { dest.Init(); }
 else {
   dest.AllocBuffer(nNewLen);
   memcpy(dest.m_bVect, &m_bVect[nCopyIndex], nCopyLen);
 }
}

//*************************************************************************************//

//------------------------------------------------------------------------------ CBV()
CBV::CBV()   { Init();  }

//----------------------------------------------------------- CBV(const CBV& bvSrc)
CBV::CBV(const CBV& bvSrc)
{bvSrc.AllocCopy(*this, bvSrc.m_nByteLength, 0, 0);
 m_nBitLength = bvSrc.m_nBitLength;
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////// More sophisticated construction /////////////////////////////

//------------------------------------------------- CBV(BYTE ch, int nLength, BOOL Fl)
CBV::CBV(BYTE mask, int nLength, BOOL Fl)
{ int nLenByte,w;
  if (nLength < 1) Init();      // return empty vector if invalid repeat count
  else {
    if (Fl) {   // Bits
      nLenByte = LEN_BYTE(nLength);
      AllocBuffer(nLenByte);                   // Not BYTE independente !!!!!!!
      if (mask) {
        memset(m_bVect, 0xff, nLenByte);
        w = ADR_BIT(nLength);
        if (w) m_bVect[nLenByte-1] = (m_bVect[nLenByte-1]>>(S_1-w))<<(S_1-w);
      }
      else       memset(m_bVect, 0, nLenByte);
      m_nBitLength = nLength;
    }
    else {   // Bytes (mask)
      AllocBuffer(nLength);
      memset(m_bVect, mask, nLength);
      m_nBitLength = nLength * S_1;
    }
  }
}

//--------------------------------------------------- CBV(const BYTE* pbt,int nLenBit)
CBV::CBV(const BYTE* pbt,int nLenBit)
{ int nLenByte;
  if (nLenBit==0) Init();
  else   {
    nLenByte = LEN_BYTE(nLenBit);
    AllocBuffer(nLenByte);
    memcpy(m_bVect, pbt, nLenByte);
    m_nBitLength = nLenBit;
  }
}

//--------------------------------------------------------------- CBV(const char* pch)
CBV::CBV(const char* pch)
{ int nLenByte,nLenBit;
  if ((nLenBit = SafeStrlen(pch)) == 0) Init();
  else   {
    nLenByte = LEN_BYTE(nLenBit);        // Translate from symbol to bit
    AllocBuffer(nLenByte);
    CharBit(nLenByte,pch);
    m_nBitLength = nLenBit;
  }
}

//----------------------------------------------------------------------------- ~CBV()
CBV::~CBV()       //  free any attached data
{ SafeDelete(m_bVect); }

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Reading and writing //////////////////////////////////
//#ifndef _LINUX
//----------------------------------------------------------- BitChar(char One,char Zero)
//CString CBV::BitChar(char One,char Zero,int Max/*=0*/)
//{ int i,j=0;
//  CString res('\0',m_nBitLength);
//  for (i=0; i<m_nBitLength; i++, j++)
//  {
//    if (GetBitAt(i)) res.SetAt(j,One);
//    else             res.SetAt(j,Zero);
//    if (Max!=0)
//      if ((Max==1)||((i+1)%Max == 0))
//      { res.SetAt(++j,'\n'); }  // res.SetAt(++j,'\r');
//  }
//  return res;
//}
//#else
char* CBV::BitChar(char One,char Zero,int Max/*=0*/)
{ int i,j=0;
  if (Max!=0) j = m_nBitLength/Max +1;
  char* res = (char*)malloc(m_nBitLength + 1 + 2*j);
  memset(res,0,m_nBitLength + 1 + 2*j);
  j = 0;
  for (i=0; i<m_nBitLength; i++, j++)
  {
    if (GetBitAt(i)) res[j] = One;
    else             res[j] = Zero;
    if (Max!=0)
      if ((Max==1)||((i+1)%Max == 0))
      { res[++j] = '\n'; }   // res[++j] = '\r';
  }
  return res;
}
//#endif

/////////////////////////////////////////////////////////////////////////////////////////
////////////////////// Assignment operator and functions ////////////////////////////////

//****************************** Protected function ***********************************//
//----------------------------------------------------------------- AllocBuffer(int nLen)
void CBV::AllocBuffer(int nLen)
{ASSERT(nLen >= 0);
 ASSERT(nLen <= INT_MAX - 1);    // max size (enough room for 1 extra)
 if (nLen == 0)     {   Init();     }
 else   {
   m_bVect = new BYTE[nLen];       // may throw an exception
   m_nByteLength = nLen;
   m_nAllocLength = nLen;
   memset(m_bVect,0,nLen);
 }
}

//-------------------------- AssignCopy(int nLenBit,int nLenByte, const BYTE* pbtSrcData)
void CBV::AssignCopy(int nLenBit,int nLenByte, const BYTE* pbtSrcData)
{ //--- check if it will fit
  if (nLenByte > m_nAllocLength) { // it won't fit, allocate another one
    Empty();
    AllocBuffer(nLenByte);
  }
  else
   memset(m_bVect+nLenByte,0,m_nAllocLength-nLenByte);   // 21.02.2007

  if (nLenByte != 0) memcpy(m_bVect, pbtSrcData, nLenByte);
  m_nByteLength = nLenByte;
  m_nBitLength = nLenBit;
}

//------------------------------------------------- CharBit(int nLenByte,const char* pch)
void CBV::CharBit(int nLenByte,const char* pch)
{ int i,j;
  for (j=0; j<nLenByte; j++) {
    for (i=0; i<S_1 && *pch; i++) {
      if (*pch=='1') m_bVect[j] |= OB[i];
      else { ASSERT(*pch=='0' || *pch=='-'|| *pch=='.'); }  //new 03.06.98
      ++pch;
    }
  }
}

//---------------------------------------------- AssignChar(int nLenBit, const char* pch)
void CBV::AssignChar(int nLenBit, const char* pch)
{ //--- check if it will fit
  int nLenByte = LEN_BYTE(nLenBit);
  if (nLenByte > m_nAllocLength) { // it won't fit, allocate another one
    Empty();
    AllocBuffer(nLenByte);
    m_nBitLength = nLenBit;
  }
  else memset(m_bVect,0,nLenByte);
  CharBit(nLenByte,pch);
  m_nByteLength = nLenByte;
  m_nBitLength = nLenBit;
}

//------------------------ Extr(const BYTE* SrcVect, int SrcBitLen,int nFirst,int nCount)
void CBV::Extr(const BYTE* SrcVect, int SrcBitLen,int nFirst,int nCount)
{ int i,j,nLenByte,l_bit,r_bit,AdrBeg;

  ASSERT(nCount >= 0);  ASSERT(nFirst >= 0);
  ASSERT((nCount+nFirst) <= SrcBitLen);
  nLenByte = LEN_BYTE(nCount);
  AllocBuffer(nLenByte);
  m_nBitLength = nCount;
  if (SrcBitLen == nCount) { memcpy((BYTE*)m_bVect,SrcVect,nLenByte); return; }
  AdrBeg = BIT_BYTE(nFirst);
  memcpy((BYTE*)m_bVect,SrcVect+AdrBeg,nLenByte);
  l_bit = ADR_BIT(nFirst);
  if (l_bit)    {
    r_bit = S_1 - l_bit;
    if (nLenByte == LEN_BYTE(SrcBitLen)) j=nLenByte-1;   //new, 4.12.97
    else                                 j=nLenByte;     //new, 4.12.97
    for (i=0; i<j; i++)
      m_bVect[i] = (SrcVect[AdrBeg+i] << l_bit) | (SrcVect[AdrBeg+i+1] >> r_bit);
    if (j == nLenByte-1) m_bVect[j] = (SrcVect[AdrBeg+j] << l_bit);  //new, 4.12.97
  }
  else memcpy((BYTE*)m_bVect,SrcVect+AdrBeg,nLenByte);
  if (ADR_BIT(m_nBitLength)) {
    i = S_1 - ADR_BIT(m_nBitLength);
    m_bVect[m_nByteLength-1] = (m_bVect[m_nByteLength-1] >> i) << i;
  }
}
//*************************************************************************************//

//--------------------------------------------------------------------------------- One()
void CBV::One()
{ int i;
  if (m_nBitLength==0) return;          //22.01.2002
  ASSERT(m_nByteLength>0);
  memset(m_bVect,0xff,m_nByteLength);
  if ((i = ADR_BIT(m_nBitLength)) > 0 ) i = S_1 - i;
  m_bVect[m_nByteLength-1] = (m_bVect[m_nByteLength-1] >> i) << i;
}

//-------------------------------------------------------- Extract(int nFirst,int nCount)
CBV CBV::Extract(int nFirst,int nCount)
{ CBV s;
  s.Extr(m_bVect,m_nBitLength,nFirst,nCount);  return s;
}


/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// Concatination //////////////////////////////////////

//****************************** Protected function ***********************************//
//---------------------------------------------- Conc(const BYTE* SrcVect, int SrcBitLen)

 void CBV::Conc(const BYTE* SrcVect, int SrcBitLen)
{ int NewBitLen, NewByteLen, SrcByteLen, OldByte, r_bit,l_bit,i,j;
  BYTE *work;
  NewBitLen = m_nBitLength + SrcBitLen;
  if (NewBitLen == 0) { Init(); return;}
  NewByteLen = LEN_BYTE(NewBitLen);
  OldByte=m_nByteLength;
  r_bit = ADR_BIT(m_nBitLength);
  if (m_nAllocLength < NewByteLen)  {
    if (m_nByteLength) {
      work = new BYTE[m_nByteLength];
      memcpy(work, m_bVect, m_nByteLength);
      Empty();
      AllocBuffer(NewByteLen);
      memcpy(m_bVect, work, OldByte);
      delete [] work;
    }
    else { AllocBuffer(NewByteLen); }
  }
  else                                   // new 16.11.99
    if(m_nByteLength < NewByteLen) {      // new 16.11.99
      m_nByteLength = NewByteLen;
      m_bVect[m_nByteLength-1]=0;        // new 16.11.99
    }
  m_nBitLength = NewBitLen;
  SrcByteLen = LEN_BYTE(SrcBitLen);
  if (r_bit==0)  {
    memcpy(m_bVect+OldByte, SrcVect, SrcByteLen);
    r_bit = S_1 - ADR_BIT(m_nBitLength);                       //new 02.02.99
    if (r_bit==8) r_bit=0;                                     //new 02.02.99
    m_bVect[m_nByteLength-1] = (m_bVect[m_nByteLength-1]>>r_bit)<<r_bit; //new 02.02.99
    return;
  }
  l_bit = S_1-r_bit; i=OldByte;
  m_bVect[i-1] |= SrcVect[0]>>r_bit;
  for (j=0; j<SrcByteLen-1; j++)
    { m_bVect[i++] = (SrcVect[j] << l_bit) | (SrcVect[j+1] >> r_bit); }
  if (j*S_1+l_bit < SrcBitLen)
    m_bVect[m_nByteLength-1] |= SrcVect[SrcByteLen-1] << l_bit;
  if (j=ADR_BIT(m_nBitLength)) {
    i = S_1 - j;
    m_bVect[m_nByteLength-1] = (m_bVect[m_nByteLength-1] >> i) << i;
  }
}


//------------ Conc2(const BYTE* Vect1, int SrcBitLen1,const BYTE* Vect2, int SrcBitLen2)
void CBV::Conc2(const BYTE* Vect1, int SrcBitLen1,const BYTE* Vect2, int SrcBitLen2)
{ int NewBitLen, NewByteLen, SrcByteLen, OldByte, r_bit,l_bit,i,j;

  NewBitLen = SrcBitLen1 + SrcBitLen2;
  if (NewBitLen == 0) { Init(); return;}
  NewByteLen = LEN_BYTE(NewBitLen);
  if (m_nAllocLength < NewByteLen)  {
    Empty(); AllocBuffer(NewByteLen);
  }
  else       {                            // new 16.11.99
//  if(m_nByteLength < NewByteLen)       // new 24.01.00
    m_nByteLength = NewByteLen;
    m_bVect[m_nByteLength-1]=0;          // new 16.11.99
  }
  m_nBitLength = NewBitLen;
  OldByte = LEN_BYTE(SrcBitLen1);
  memcpy(m_bVect, Vect1, OldByte);
  r_bit = ADR_BIT(SrcBitLen1);
  SrcByteLen = LEN_BYTE(SrcBitLen2);
  if (r_bit==0)  { memcpy(m_bVect+OldByte, Vect2, SrcByteLen); return;}
  l_bit = S_1-r_bit; i=OldByte;
  m_bVect[OldByte-1] |= Vect2[0]>>r_bit;
  for (j=0; j<SrcByteLen-1; j++)
    { m_bVect[i++] = (Vect2[j] << l_bit) | (Vect2[j+1] >> r_bit); }
  if (j*S_1+l_bit < SrcBitLen2)
    m_bVect[m_nByteLength-1] |= Vect2[SrcByteLen-1] << l_bit;
  if (j=ADR_BIT(m_nBitLength)) {
    i = S_1 - j;
    m_bVect[m_nByteLength-1] = (m_bVect[m_nByteLength-1] >> i) << i;
  }
}
//*************************************************************************************//

//-------------------------------------------------- Concat(const BYTE* pbt, int nLength)
void CBV::Concat(const BYTE* pbt, int nLength)
{  Conc(pbt,nLength); return; }

//-------------------------------------------------------------- Concat(const CBV& bv)
void CBV::Concat(const CBV& bv)
{  Conc(bv.m_bVect,bv.m_nBitLength); return; }

//------------------------------------------- Concat(const CBV& bv1,const CBV& bv2)
void CBV::Concat(const CBV& bv1,const CBV& bv2)
{  Conc2(bv1.m_bVect,bv1.m_nBitLength,bv2.m_bVect,bv2.m_nBitLength); return; }

/////////////////////////////////////////////////////////////////////////////////////////
///////////////////////// Advanced assignment operators /////////////////////////////////

//****************************** Protected function ***********************************//
//---------------------------------------------------------- LeftShiftInPlace(int nShift)
void CBV::LeftShiftInPlace(int nShift)
{  // LeftShift bool vector  -- assume that 'this' is a new CBV object
 int i,j,l_bit,r_bit,nNewLenByte,nNewLenBit,AdrBeg;

 ASSERT(nShift >= 0);
 nNewLenBit = m_nBitLength - nShift;
 ASSERT(nNewLenBit >= 0);
 if (nNewLenBit==0)  { memset(m_bVect,0,m_nByteLength); return; }
 if (nShift==0)       return;
 nNewLenByte = LEN_BYTE(nNewLenBit);
 AdrBeg = BIT_BYTE(nShift);
 l_bit = ADR_BIT(nShift);
 if (l_bit)    {
   r_bit = S_1 - l_bit;  j=0;
   for (i=AdrBeg; i<m_nByteLength-1; i++)
     m_bVect[j++] = (m_bVect[i] << l_bit) | (m_bVect[i+1] >> r_bit);
   if ((j*S_1 + nShift) < m_nBitLength)                             //***new 9.04
     m_bVect[nNewLenByte-1] = m_bVect[m_nByteLength-1] << l_bit;
 }
 else   memcpy(m_bVect, m_bVect+AdrBeg, nNewLenByte);
 i= m_nByteLength - nNewLenByte;
 memset(m_bVect+nNewLenByte, 0, i);
}

//--------------------------------------------------------- RightShiftInPlace(int nShift)
void CBV::RightShiftInPlace(int nShift)
{  // RightShift bool vector  -- assume that 'this' is a new CBV object
 int i,j,l_bit,r_bit,AdrBeg;
 BYTE *work;
 ASSERT(nShift >= 0);
 ASSERT((m_nBitLength - nShift) >= 0);
 if (nShift == 0) return;
 if ((m_nBitLength - nShift)==0)  { memset(m_bVect,0,m_nByteLength); return; }
 work = new BYTE[m_nByteLength];       // may throw an exception
 memset(work,0,m_nByteLength);
 AdrBeg = BIT_BYTE(nShift);
 r_bit = ADR_BIT(nShift);
 if (r_bit)    {
   l_bit = S_1 - r_bit;  j=0;
   work[AdrBeg] = (m_bVect[0] >> r_bit);
   for (i=AdrBeg+1; i<m_nByteLength; i++)
     { work[i] = (m_bVect[j] << l_bit) | (m_bVect[j+1] >> r_bit); j++; }
 }
 else memcpy(work+AdrBeg, m_bVect, m_nByteLength-AdrBeg);
 if (i=ADR_BIT(m_nBitLength))
   work[m_nByteLength-1] = (work[m_nByteLength-1] >> (S_1-i)) << (S_1-i);
 memcpy(m_bVect,work, m_nByteLength);
 delete [] work;
}

//--------------------------------------------- DizInPlace(const BYTE* Vect1,int BitLen1)
void CBV::DizInPlace(const BYTE* Vect1,int BitLen1)
{int i;
 ASSERT(BitLen1 >= 0);
 ASSERT(BitLen1 == m_nBitLength);
 for (i=0; i<m_nByteLength; i++) m_bVect[i] = m_bVect[i] | Vect1[i];
}

//--------------------------------------------- ConInPlace(const BYTE* Vect1,int BitLen1)
void CBV::ConInPlace(const BYTE* Vect1,int BitLen1)
{int i;
 ASSERT(BitLen1 >= 0);
 ASSERT(BitLen1 == m_nBitLength);
 for (i=0; i<m_nByteLength; i++) m_bVect[i] = m_bVect[i] & Vect1[i];
}

//-------------------------------------------- Add2InPlace(const BYTE* Vect1,int BitLen1)
void CBV::Add2InPlace(const BYTE* Vect1,int BitLen1)
{int i;
 ASSERT(BitLen1 >= 0);
 ASSERT(BitLen1 == m_nBitLength);
 for (i=0; i<m_nByteLength; i++) m_bVect[i] = m_bVect[i] ^ Vect1[i];
}

//------------------------------------------ ConNotInPlace(const BYTE* Vect1,int BitLen1)
void CBV::ConNotInPlace(const BYTE* Vect1,int BitLen1)
{int i;
 ASSERT(BitLen1 >= 0);
 ASSERT(BitLen1 == m_nBitLength);
 for (i=0; i<m_nByteLength; i++) m_bVect[i] = m_bVect[i] & ~Vect1[i];
 i = ADR_BIT(BitLen1);
 if (i)  m_bVect[m_nByteLength-1] = (m_bVect[m_nByteLength-1] >> (S_1-i)) << (S_1-i);
}
//*************************************************************************************//

//-------------------------------------------------------------- operator <<=(int nShift)
const CBV&  CBV::operator <<=(int nShift)
{ LeftShiftInPlace(nShift);  return *this; }

//-------------------------------------------------------------- operator >>=(int nShift)
const CBV& CBV::operator >>=(int nShift)
{ RightShiftInPlace(nShift); return *this;}

//-------------------------------------------------------- operator |=(const CBV& bv1)
const CBV& CBV::operator |=(const CBV& bv1)
{ DizInPlace(bv1.m_bVect,bv1.m_nBitLength);    return *this;}

//-------------------------------------------------------- operator |=(const BYTE* pbt)
const CBV& CBV::operator |=(const BYTE* pbt)
{ ASSERT(m_nBitLength>0); DizInPlace(pbt,m_nBitLength);    return *this;}

//-------------------------------------------------------- operator &=(const CBV& bv1)
const CBV& CBV::operator &=(const CBV& bv1)
{ ConInPlace(bv1.m_bVect,bv1.m_nBitLength);    return *this;}

//-------------------------------------------------------- operator &=(const BYTE* pbt)
const CBV& CBV::operator &=(const BYTE* pbt)
{ ASSERT(m_nBitLength>0); ConInPlace(pbt,m_nBitLength);    return *this;}

//-------------------------------------------------------- operator ^=(const CBV& bv1)
const CBV& CBV::operator ^=(const CBV& bv1)
 { Add2InPlace(bv1.m_bVect,bv1.m_nBitLength);    return *this;}

//-------------------------------------------------------- operator ^=(const BYTE* pbt)
const CBV& CBV::operator ^=(const BYTE* pbt)
 { ASSERT(m_nBitLength>0); Add2InPlace(pbt,m_nBitLength);    return *this;}

//-------------------------------------------------------- operator -=(const CBV& bv1)
const CBV& CBV::operator -=(const CBV& bv1)
 { ConNotInPlace(bv1.m_bVect,bv1.m_nBitLength);    return *this;}

//-------------------------------------------------------- operator -=(const BYTE* pbt)
const CBV& CBV::operator -=(const BYTE* pbt)
 { ASSERT(m_nBitLength>0); ConNotInPlace(pbt,m_nBitLength);    return *this;}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Logic operations ////////////////////////////////////

//****************************** Protected function ***********************************//
//--------------------------- LeftShiftVect(const BYTE* SrcVect, int nSrcLen, int nShift)
void CBV::LeftShiftVect(const BYTE* SrcVect, int nSrcLen, int nShift)
{int j,i,l_bit,r_bit,nNewLenByte,nNewLenBit,nSrcByte,AdrBeg;
 ASSERT(nShift >= 0);
 nNewLenBit = nSrcLen - nShift;
 ASSERT(nNewLenBit >= 0);
 nNewLenByte = LEN_BYTE(nNewLenBit);
 nSrcByte = LEN_BYTE(nSrcLen);
 AllocBuffer(nSrcByte);
 m_nBitLength = nSrcLen;
 if (nNewLenBit==0)  return;
 if (nShift==0)      { memcpy(m_bVect,SrcVect,nSrcByte); return; }
 AdrBeg = BIT_BYTE(nShift);
 l_bit = ADR_BIT(nShift);
 if (l_bit)    {
   r_bit = S_1 - l_bit;  j=0;
   for (i=AdrBeg; i<nSrcByte-1; i++)
     m_bVect[j++] = (SrcVect[i] << l_bit) | (SrcVect[i+1] >> r_bit);
   if ((j*S_1 + nShift) < m_nBitLength)                        //***new 9.04
     m_bVect[nNewLenByte-1] = SrcVect[nSrcByte-1] << l_bit;
 }
 else   memcpy(m_bVect, SrcVect+AdrBeg, nNewLenByte);
 i= nSrcByte - nNewLenByte;
 memset(m_bVect+nNewLenByte, 0, i);
}

//-------------------------- RightShiftVect(const BYTE* SrcVect, int nSrcLen, int nShift)
void CBV::RightShiftVect(const BYTE* SrcVect, int nSrcLen, int nShift)
{int i,j,l_bit,r_bit,nSrcByte,AdrBeg;
 ASSERT(nShift >= 0);
 ASSERT((nSrcLen - nShift) >= 0);
 nSrcByte = LEN_BYTE(nSrcLen);
 AllocBuffer(nSrcByte);
 m_nBitLength = nSrcLen;
 if ((nSrcLen - nShift)==0)  return;
 if (nShift==0)      { memcpy(m_bVect,SrcVect,nSrcByte); return; }
 AdrBeg = BIT_BYTE(nShift);
 r_bit = ADR_BIT(nShift);
 if (r_bit)    {
   l_bit = S_1 - r_bit;  j=0;
   m_bVect[AdrBeg] = (SrcVect[0] >> r_bit);
   for (i=AdrBeg+1; i<nSrcByte; i++)
     { m_bVect[i] = (SrcVect[j] << l_bit) | (SrcVect[j+1] >> r_bit); j++; }
 }
 else memcpy(m_bVect+AdrBeg, SrcVect, nSrcByte-AdrBeg);
 if (i=ADR_BIT(m_nBitLength))
   m_bVect[nSrcByte-1] = (m_bVect[nSrcByte-1] >> (S_1 - i)) << (S_1 - i);
}

//---------------------- Diz(const BYTE* Vect1,int BitLen1,const BYTE* Vect2,int BitLen2)
void CBV::Diz(const BYTE* Vect1,int BitLen1,const BYTE* Vect2,int BitLen2)
{int i,LenByte;
 ASSERT(BitLen1 == BitLen2);
 ASSERT(BitLen1 >= 0);
 LenByte = LEN_BYTE(BitLen1);
 AllocBuffer(LenByte);
 m_nBitLength = BitLen1;
 for (i=0; i<LenByte; i++) m_bVect[i] = Vect1[i] | Vect2[i];
}

//---------------------- Con(const BYTE* Vect1,int BitLen1,const BYTE* Vect2,int BitLen2)
void CBV::Con(const BYTE* Vect1,int BitLen1,const BYTE* Vect2,int BitLen2)
{int i,LenByte;
 ASSERT(BitLen1 == BitLen2);
 ASSERT(BitLen1 >= 0);
 LenByte = LEN_BYTE(BitLen1);
 AllocBuffer(LenByte);
 m_nBitLength = BitLen1;
 for (i=0; i<LenByte; i++) m_bVect[i] = Vect1[i] & Vect2[i];
}

//--------------------- Add2(const BYTE* Vect1,int BitLen1,const BYTE* Vect2,int BitLen2)
void CBV::Add2(const BYTE* Vect1,int BitLen1,const BYTE* Vect2,int BitLen2)
{int i,LenByte;
 ASSERT(BitLen1 == BitLen2);
 ASSERT(BitLen1 >= 0);
 LenByte = LEN_BYTE(BitLen1);
 AllocBuffer(LenByte);
 m_nBitLength = BitLen1;
 for (i=0; i<LenByte; i++) m_bVect[i] = Vect1[i] ^ Vect2[i];
}

//------------------- ConNot(const BYTE* Vect1,int BitLen1,const BYTE* Vect2,int BitLen2)
void CBV::ConNot(const BYTE* Vect1,int BitLen1,const BYTE* Vect2,int BitLen2)
{int i,LenByte;
 ASSERT(BitLen1 == BitLen2);
 ASSERT(BitLen1 >= 0);
 LenByte = LEN_BYTE(BitLen1);
 AllocBuffer(LenByte);
 m_nBitLength = BitLen1;
 for (i=0; i<LenByte; i++) m_bVect[i] = Vect1[i] & ~Vect2[i];
}

//---------------------------------------------------- Not(const BYTE* Vect1,int BitLen1)
void CBV::Not(const BYTE* Vect1,int BitLen1)
{int i,LenByte;
 ASSERT(BitLen1 >= 0);
 LenByte = LEN_BYTE(BitLen1);
 AllocBuffer(LenByte);
 m_nBitLength = BitLen1;
 for (i=0; i<LenByte; i++) m_bVect[i] = ~Vect1[i];
 i = ADR_BIT(BitLen1);
 if (i) m_bVect[LenByte-1] = (m_bVect[LenByte-1] >> (S_1-i)) << (S_1-i);
}
//*************************************************************************************//

//------------------------------------- operator <<(const CBV& bv1, int nShift)
STD(CBV) operator <<(const CBV& bv1, int nShift)
{ CBV s;
 s.LeftShiftVect(bv1.m_bVect,bv1.m_nBitLength,nShift);  return s;
}

//------------------------------------- operator >>(const CBV& bv1, int nShift)
STD(CBV) operator >>(const CBV& bv1, int nShift)
{ CBV s;
 s.RightShiftVect(bv1.m_bVect,bv1.m_nBitLength,nShift);  return s;
}

//-------------------------------- operator |(const CBV& bv1,const CBV& bv2)
STD(CBV) operator |(const CBV& bv1,const CBV& bv2)
{ CBV s;
 s.Diz(bv1.m_bVect,bv1.m_nBitLength,bv2.m_bVect,bv2.m_nBitLength);
return s;
}

//-------------------------------- operator |(const CBV& bv1,const BYTE* pbt)
STD(CBV) operator |(const CBV& bv1,const BYTE* pbt)
{ CBV s;
 s.Diz(bv1.m_bVect,bv1.m_nBitLength,pbt,bv1.m_nBitLength);
return s;
}

//-------------------------------- operator |(const CBV& bv1,const BYTE* pbt)
STD(CBV) operator |(const BYTE* pbt,const CBV& bv2)
{ CBV s;
 s.Diz(pbt,bv2.m_nBitLength,bv2.m_bVect,bv2.m_nBitLength);
return s;
}

//-------------------------------- operator &(const CBV& bv1,const CBV& bv2)
STD(CBV) operator &(const CBV& bv1,const CBV& bv2)
{ CBV s;
 s.Con(bv1.m_bVect,bv1.m_nBitLength,bv2.m_bVect,bv2.m_nBitLength);
return s;
}

//-------------------------------- operator &(const BYTE* pbt,const CBV& bv2)
STD(CBV) operator &(const BYTE* pbt,const CBV& bv2)
{ CBV s;
 s.Con(pbt,bv2.m_nBitLength,bv2.m_bVect,bv2.m_nBitLength);
return s;
}

//-------------------------------- operator &(const CBV& bv1,const BYTE* pbt)
STD(CBV) operator &(const CBV& bv1,const BYTE* pbt)
{ CBV s;
 s.Con(bv1.m_bVect,bv1.m_nBitLength,pbt,bv1.m_nBitLength);
return s;
}

//-------------------------------- operator ^(const CBV& bv1,const CBV& bv2)
STD(CBV) operator ^(const CBV& bv1,const CBV& bv2)
{ CBV s;
  s.Add2(bv1.m_bVect,bv1.m_nBitLength,bv2.m_bVect,bv2.m_nBitLength);
return s;
}

//-------------------------------- operator ^(const BYTE* pbt,const CBV& bv2)
STD(CBV) operator ^(const BYTE* pbt,const CBV& bv2)
{ CBV s;
 s.Add2(pbt,bv2.m_nBitLength,bv2.m_bVect,bv2.m_nBitLength);
return s;
}

//-------------------------------- operator ^(const CBV& bv1,const BYTE* pbt)
STD(CBV) operator ^(const CBV& bv1,const BYTE* pbt)
{ CBV s;
 s.Add2(bv1.m_bVect,bv1.m_nBitLength,pbt,bv1.m_nBitLength);
return s;
}

//-------------------------------- operator -(const CBV& bv1,const CBV& bv2)
STD(CBV) operator -(const CBV& bv1,const CBV& bv2)
{ CBV s;
 s.ConNot(bv1.m_bVect,bv1.m_nBitLength,bv2.m_bVect,bv2.m_nBitLength);
return s;
}

//-------------------------------- operator -(const BYTE* pbt,const CBV& bv2)
STD(CBV) operator -(const BYTE* pbt,const CBV& bv2)
{ CBV s;
 s.ConNot(pbt,bv2.m_nBitLength,bv2.m_bVect,bv2.m_nBitLength);
return s;
}

//-------------------------------- operator -(const CBV& bv1,const BYTE* pbt)
STD(CBV) operator -(const CBV& bv1,const BYTE* pbt)
{ CBV s;
 s.ConNot(bv1.m_bVect,bv1.m_nBitLength,pbt,bv1.m_nBitLength);
return s;
}

//-------------------------------------------------- operator ~(const CBV& bv1)
STD(CBV) operator ~(const CBV& bv1)
{ CBV s;
 s.Not(bv1.m_bVect,bv1.m_nBitLength);
return s;
}

//-------------------------------------------------- Invert(const BYTE* pbt)
void CBV:: Invert(const BYTE* pbt)
{ int i;
 for (i=0; i<m_nByteLength; i++) m_bVect[i] = ~pbt[i];
 i = ADR_BIT(m_nBitLength);
 if (i)  m_bVect[m_nByteLength-1] = (m_bVect[m_nByteLength-1] >> (S_1-i)) << (S_1-i);
}

//------------------------------------------------------------- LoopLeftShift(int nShift)
void CBV::LoopLeftShift(int nShift)
{  // LeftShift bool vector  -- assume that 'this' is a new CBV object
 int i,j,l_bit,r_bit,nNewLenByte,nNewLenBit,nEndLenByte;
 BYTE *work;
 ASSERT(nShift >= 0);
 nNewLenBit = m_nBitLength - nShift;
 ASSERT(nNewLenBit >= 0);
 if (nNewLenBit==0 || nShift==0) return;
 nNewLenByte = LEN_BYTE(nNewLenBit);
 work = new BYTE[m_nByteLength];       // may throw an exception
 memset(work,0,m_nByteLength);
 i = BIT_BYTE(nShift);
 l_bit = ADR_BIT(nShift);
 if (l_bit)    {
   r_bit = S_1 - l_bit;
   memcpy(work, m_bVect+i, nNewLenByte+1);
   for (i=0; i<nNewLenByte; i++)
     work[i] = (work[i] << l_bit) | (work[i+1] >> r_bit);
   work[i] <<= l_bit;
 }
 else   memcpy(work, m_bVect+i, nNewLenByte);
 nEndLenByte = m_nByteLength - nNewLenByte;
 j = BIT_BYTE(nNewLenBit);
 r_bit = ADR_BIT(nNewLenBit);
 if (r_bit == 0 )    { memcpy(work+j, m_bVect, nEndLenByte); }
 else {
   l_bit = S_1 - r_bit;
   work[j] = (work[j] >> l_bit) << l_bit;
   work[j++] |= (m_bVect[0] >> r_bit);
   for (i=0; i<nEndLenByte; i++)
     work[j++] = (m_bVect[i] << l_bit) | (m_bVect[i+1] >> r_bit);
   j = S_1 - ADR_BIT(m_nBitLength);
   work[m_nByteLength-1] = (work[m_nByteLength-1] >> j)<<j;
 }
 memcpy(m_bVect,work, m_nByteLength);
 delete [] work;
}

//------------------------------------------------------------ LoopRightShift(int nShift)
void CBV::LoopRightShift(int nShift)
{  // LeftShift bool vector  -- assume that 'this' is a new CBV object
 int i,j,l_bit,r_bit,AdrBeg,AdrEnd,BitEnd;
 BYTE *work;
 ASSERT(nShift >= 0);
 ASSERT((m_nBitLength - nShift) >= 0);
 if (nShift == 0) return;
 if ((m_nBitLength - nShift)==0)  { memset(m_bVect,0,m_nByteLength); return; }
 work = new BYTE[m_nByteLength];       // may throw an exception
 memset(work,0,m_nByteLength);
 AdrBeg = BIT_BYTE(nShift);
 AdrEnd = BIT_BYTE(m_nBitLength - nShift);
 BitEnd = ADR_BIT(m_nBitLength);
 if (BitEnd < nShift) {
   l_bit = ADR_BIT(m_nBitLength - nShift);
   r_bit = S_1 - l_bit;     j = AdrEnd;
   for (i=0; i<AdrBeg+1; i++)
     { work[i] = (m_bVect[j] << l_bit) | (m_bVect[j+1] >> r_bit); j++; }
 }
 else { l_bit = BitEnd - nShift; work[0] = m_bVect[AdrEnd] << l_bit; }
 r_bit = ADR_BIT(nShift); l_bit = S_1 - r_bit;
 work[AdrBeg] = (work[AdrBeg] >> l_bit) << l_bit;
 if (r_bit)    {
   j=0;
   work[AdrBeg] |= (m_bVect[0] >> r_bit);
   for (i=AdrBeg+1; i<m_nByteLength; i++)
     { work[i] = (m_bVect[j] << l_bit) | (m_bVect[j+1] >> r_bit); j++; }
 }
 else memcpy(work+AdrBeg, m_bVect, m_nByteLength-AdrBeg);
 if (i=ADR_BIT(m_nBitLength))
   work[m_nByteLength-1] = (work[m_nByteLength-1] >> (S_1-i)) << (S_1-i);
 memcpy(m_bVect,work, m_nByteLength);
 delete [] work;
}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////// Operation of weighting,finding and casing ///////////////////////
//---------------------------------------------------------------------------- CountBit()
int CBV::CountBit() const
{ int j,one=0;
 for (j=0; j<m_nByteLength; j++) one+=TabC[m_bVect[j]];
 return one;
}

//-------------------------------------------------------------------- LeftOne(int nNext)
int CBV::LeftOne(int nNext) const
{int i,j,pos = 0;
 int k;                                //new 24.01.00
 BYTE ch;
// ASSERT((m_nBitLength - nNext) >= 0);
 ASSERT((m_nBitLength - nNext) > 0);   //new 24.01.00
 if (nNext == -1) {i=0;j=0;}
 else {i=BIT_BYTE(nNext);  j=ADR_BIT(nNext); j++;}
 if (j==S_1)  {j=0;  i++; }
 if (i==m_nByteLength)  return (-1);
 ch = m_bVect[i] << j;
 if (ch)  {
   while (!(ch & OB[pos++]));
   return (nNext + pos);
 }
 for (j=i+1;j<m_nByteLength;j++)
   if (m_bVect[j])  {
     while (!(m_bVect[j] & OB[pos++]));
     k = (j*S_1)+pos-1;               //new 24.01.00
     if (k>=m_nBitLength) return -1;  //new 24.01.00
     else                 return k;   //new 24.01.00
//     return ((j*S_1)+pos-1);
   }
 return (-1);
}

//--------------------------------------------------------------------- LeftOne(BYTE& bt)
int CBV::LeftOne(BYTE& bt) const
{int i,pos;
 for (i=0;i<m_nByteLength;i++)
   if (m_bVect[i]!=0) {
     pos=0;
     while (!(bt=(m_bVect[i] & OB[pos++])));
     return (i);
   }
 return(-1);
}

//------------------------------------------------------------------- RightOne(int nPrev)
int CBV::RightOne(int nPrev) const
{int i,j,pos = 7;
 BYTE ch;
 ASSERT((m_nBitLength - nPrev) >= 0);
 if (nPrev == -1) {i=m_nByteLength-1;j=0;}
 else {i=BIT_BYTE(nPrev);  j=S_1 - ADR_BIT(nPrev);}
 if (j==8)  {j=0;  i--; }
 ch = (m_bVect[i] >> j) << j;
 if (ch)  {
   while (!(ch & OB[pos--]));
   return ((i*S_1)+pos+1);
 }
 for (j=i-1;j>=0;j--)
   if(m_bVect[j])  {
     while (!(m_bVect[j] & OB[pos--]));
     return ((j*S_1)+pos+1);
   }
 return (-1);
}

//-------------------------------------------------------------------- RightOne(BYTE& bt)
int CBV::RightOne(BYTE& bt) const
{int i,pos;
 for (i=m_nByteLength-1;i>=0;i--)
   if (m_bVect[i]!=0)  {
     pos=7;
     while (!(bt=(m_bVect[i] & OB[pos--])));
     return (i);
   }
 return (-1);
}


/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Comparing operations ///////////////////////////////////////

//****************************** Protected function ***********************************//
//----------------------------------------- Equality(const BYTE* Vect2,int BitLen2) const
BOOL CBV::Equality(const BYTE* Vect2,int BitLen2) const
{int i;
 if (Vect2==NULL && m_bVect) return FALSE;
 if (m_nBitLength!=BitLen2)  return FALSE;
 for (i=0;i<m_nByteLength;i++)
   if (m_bVect[i]!=Vect2[i]) return FALSE;
 return TRUE;
}

//----------------------------------- Pogl(const BYTE* Vect2,int BitLen2,BOOL Dist) const
BOOL CBV::Pogl(const BYTE* Vect2,int BitLen2,BOOL Dist) const
{ int i,j=0;
 ASSERT(m_nBitLength==BitLen2);
 for (i=0;i<m_nByteLength;i++) {
   if (m_bVect[i] == Vect2[i]) j++;
   if (Dist) { if ((m_bVect[i] & Vect2[i]) != Vect2[i])   return FALSE; }
   else      { if ((m_bVect[i] & Vect2[i]) != m_bVect[i]) return FALSE; }
 }
 if (j==m_nByteLength) return FALSE;
 return TRUE;
}

//--------------------------------- PoglEq(const BYTE* Vect2,int BitLen2,BOOL Dist) const
BOOL CBV::PoglEq(const BYTE* Vect2,int BitLen2,BOOL Dist) const
{ int i;
 ASSERT(m_nBitLength==BitLen2);
 for (i=0;i<m_nByteLength;i++) {
   if (Dist) { if ((m_bVect[i] & Vect2[i]) != Vect2[i])   return FALSE; }
   else      { if ((m_bVect[i] & Vect2[i]) != m_bVect[i]) return FALSE; }
 }
 return TRUE;
}
//*************************************************************************************//
//--------------------------------- Concat(BOOL bBit)
void CBV::Concat(BOOL Bit/*=FALSE*/)
{ int nByteLength, nLen=m_nBitLength+1;
  if((nByteLength = LEN_BYTE(nLen)) > m_nAllocLength)
    GetBuffer(nLen);
  m_nByteLength = nByteLength;
  m_nBitLength = nLen; SetBitAt(nLen-1,Bit);
}

//------------------------------------------------------------------------------ IsZero()
BOOL CBV::IsZero() const
{ int i;
 if (m_nByteLength == 0) return FALSE;  // 27.05.02
 for (i=0; i<m_nByteLength; i++)  if (m_bVect[i]) return FALSE;
 return TRUE;
}

//------------------------------------------------------------------------------- IsOne()
BOOL CBV::IsOne() const
{ int i; char LastB;
 if (m_nByteLength == 0) return FALSE;    // 27.05.02
 for (i=0; i<m_nByteLength-1; i++)
   { LastB = ~m_bVect[i]; if (LastB) return FALSE; }
 i = S_1 - ADR_BIT(m_nBitLength);
 if (i !=S_1) LastB = (~m_bVect[m_nByteLength-1] >> i) << i;
 else         LastB = ~m_bVect[m_nByteLength-1];
 if (LastB) return FALSE;
 return TRUE;
}

//-------------------------------------- operator ==(const CBV& bv1,const CBV& bv2)

STD(BOOL) operator ==(const CBV& bv1,const CBV& bv2)
{ return bv1.Equality(bv2.m_bVect,bv2.m_nBitLength);}

//----------------------------------------- operator ==(const CBV& bv1,const BYTE* p2)
STD(BOOL) operator ==(const CBV& bv1,const BYTE* p2)
{  return bv1.Equality(p2,bv1.m_nBitLength);}

//----------------------------------------- operator ==(const BYTE* p1,const CBV& bv2)
STD(BOOL) operator ==(const BYTE* p1,const CBV& bv2)
{ return bv2.Equality(p1,bv2.m_nBitLength);}

//-------------------------------------- operator !=(const CBV& bv1,const CBV& bv2)

STD(BOOL) operator !=(const CBV& bv1,const CBV& bv2)
{ return !(bv1.Equality(bv2.m_bVect,bv2.m_nBitLength));}

//----------------------------------------- operator !=(const CBV& bv1,const BYTE* p2)
STD(BOOL) operator !=(const CBV& bv1,const BYTE* p2)
{ return !(bv1.Equality(p2,bv1.m_nBitLength));}

//----------------------------------------- operator !=(const BYTE* p1,const CBV& bv2)
STD(BOOL) operator !=(const BYTE* p1,const CBV& bv2)
{ return !(bv2.Equality(p1,bv2.m_nBitLength));}

//--------------------------------------- operator >(const CBV& bv1,const CBV& bv2)
STD(BOOL) operator >(const CBV& bv1,const CBV& bv2)
{ return (bv1.Pogl(bv2.m_bVect,bv2.m_nBitLength,TRUE)) ; }

//------------------------------------------ operator >(const CBV& bv1,const BYTE* p2)
STD(BOOL) operator >(const CBV& bv1,const BYTE* p2)
{ return (bv1.Pogl(p2,bv1.m_nBitLength,TRUE)); }

//------------------------------------------ operator >(const BYTE* p1,const CBV& bv2)
STD(BOOL) operator >(const BYTE* p1,const CBV& bv2)
{ return (bv2.Pogl(p1,bv2.m_nBitLength,FALSE));}

//--------------------------------------- operator <(const CBV& bv1,const CBV& bv2)
STD(BOOL) operator <(const CBV& bv1,const CBV& bv2)
{ return (bv2.Pogl(bv1.m_bVect,bv1.m_nBitLength,TRUE));}

//------------------------------------------ operator <(const CBV& bv1,const BYTE* p2)
STD(BOOL) operator <(const CBV& bv1,const BYTE* p2)
{ return (bv1.Pogl(p2,bv1.m_nBitLength,FALSE)); }

//------------------------------------------ operator <(const BYTE* p1,const CBV& bv2)
STD(BOOL) operator <(const BYTE* p1,const CBV& bv2)
{ return (bv2.Pogl(p1,bv2.m_nBitLength,TRUE)); }

//-------------------------------------- operator >=(const CBV& bv1,const CBV& bv2)
STD(BOOL) operator>=(const CBV& bv1, const CBV& bv2)
{ return (bv1.PoglEq(bv2.m_bVect,bv2.m_nBitLength,TRUE)) ; }

//----------------------------------------- operator >=(const CBV& bv1,const BYTE* p2)
STD(BOOL) operator>=(const CBV& bv1, const BYTE* p2)
{ return (bv1.PoglEq(p2,bv1.m_nBitLength,TRUE)); }

//----------------------------------------- operator >=(const BYTE* p1,const CBV& bv2)
STD(BOOL) operator>=(const BYTE* p1, const CBV& bv2)
{ return (bv2.PoglEq(p1,bv2.m_nBitLength,FALSE));}

//-------------------------------------- operator <=(const CBV& bv1,const CBV& bv2)
STD(BOOL) operator <=(const CBV& bv1, const CBV& bv2)
{ return (bv2.PoglEq(bv1.m_bVect,bv1.m_nBitLength,TRUE)); }

//----------------------------------------- operator <=(const CBV& bv1,const BYTE* p2)
STD(BOOL) operator <=(const CBV& bv1, const BYTE* p2)
{ return (bv1.PoglEq(p2,bv1.m_nBitLength,FALSE)); }

//----------------------------------------- operator <=(const BYTE* p1,const CBV& bv2)
STD(BOOL) operator<=(const BYTE* p1, const CBV& bv2)
{ return (bv2.PoglEq(p1,bv2.m_nBitLength,TRUE)); }


/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////// Input/output operations //////////////////////////////////////
//#ifndef _LINUX
//-------------------------------- operator <<(CDumpContext& dc, const CBV& bv)
#ifdef _DEBUG
CDumpContext& AFXAPI operator <<(CDumpContext& dc, const CBV& bv)
{int i;
 CString res('\0',bv.m_nBitLength);
 for (i=0; i<bv.m_nBitLength; i++)
   if (bv.GetBitAt(i)) res.SetAt(i,'1');
   else             res.SetAt(i,'0');
 dc << res; return dc;
}
#endif //_DEBUG

//------------------------------------- operator<<(CArchive& ar, const CBV& bv)
//CArchive& AFXAPI operator<<(CArchive& ar, const CBV& bv)
//{
//  ar << bv.m_nByteLength;
//  ar << bv.m_nBitLength;
//  ar.Write(bv.m_bVect, bv.m_nByteLength);
//  return ar;
//}

//------------------------------------- operator>>(CArchive& ar, const CBV& bv)
//CArchive& AFXAPI operator>>(CArchive& ar, CBV& bv)
//{
//  bv.Empty();
//  ar >> bv.m_nByteLength;
//  ar >> bv.m_nBitLength;
//  if (bv.m_nByteLength != 0) {
//    bv.AllocBuffer(bv.m_nByteLength);
//    if (ar.Read(bv.m_bVect, bv.m_nByteLength) != (unsigned int)bv.m_nByteLength)
//      AfxThrowArchiveException(CArchiveException::endOfFile);
//  }
//  return ar;
//}

//#endif //_LINUX

//------------------------------------- operator<<(CArchive& ar, const CBV& bv)

STD(CArch&) operator<<(CArch& ar, const CBV& bv)
{
  ar << bv.m_nByteLength;
  ar << bv.m_nBitLength;
  ar.Write(bv.m_bVect, bv.m_nByteLength);
  return ar;
}

//------------------------------------- operator>>(CArchive& ar, const CBV& bv)
STD(CArch&) operator>>(CArch& ar, CBV& bv)
{
  bv.Empty();
  ar >> bv.m_nByteLength;
  ar >> bv.m_nBitLength;
  if (bv.m_nByteLength != 0) {
    bv.AllocBuffer(bv.m_nByteLength);
    ar.Read(bv.m_bVect, bv.m_nByteLength);
  }
  return ar;
}




/////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////// Advanced dump of memmory /////////////////////////////////
//---------------------------------------------------------- GetBuffer(int nMinBufLength)

BYTE* CBV::GetBuffer(int nMinBufLength)
{ ASSERT(nMinBufLength >= 0);
 if (nMinBufLength > m_nBitLength) {
   BYTE* pbtOldData = m_bVect;
   int nOldLen = m_nByteLength;
   AllocBuffer(LEN_BYTE(nMinBufLength));
   memcpy(m_bVect, pbtOldData, nOldLen);
   m_nByteLength = LEN_BYTE(nMinBufLength);
   m_nBitLength = nMinBufLength;
   SafeDelete(pbtOldData);
 }
// ASSERT(m_bVect != NULL);
 return m_bVect;
}

//--------------------------------------------------------- ReleaseBuffer(int nNewLength)
void CBV::ReleaseBuffer(int nNewLength)
{ int nNewByte;
 if (nNewLength == -1) nNewLength = m_nBitLength;
 nNewByte = LEN_BYTE(nNewLength);
 ASSERT(nNewByte <= m_nAllocLength);
 // memset(m_bVect+LEN_BYTE(nNewLength),i+1);      ??????????
 //HVect[i]=(HVect[i]>>(S_1-j))<<(S_1-j);
 //for (i=m_nBitLength-1; i>=nNewLength; i--)
 m_nByteLength = nNewByte;
 m_nBitLength = nNewLength;
}

//--------------------------------------------------------- GetBufferSetLength(int nNewLength)
BYTE* CBV::SetSize(int nNewLength,int nNewAllocLength/*-1*/)
{ int i;
  ASSERT(nNewLength >= 0);
  int nLen=(nNewAllocLength > 0)?nNewAllocLength:nNewLength;
  ASSERT(nNewLength <=nLen);
  int nOldByte = m_nByteLength;    // 03.01.2002
  GetBuffer(nLen);                      //nLen - in bits
  m_nByteLength = LEN_BYTE(nNewLength);
  m_nBitLength = nNewLength;
//  new 03.04.2000
  if (i=ADR_BIT(m_nBitLength)) {
     m_bVect[m_nByteLength-1]=(m_bVect[m_nByteLength-1]>>(S_1-i))<<(S_1-i);
   }
  if (nOldByte > m_nByteLength)
    memset(m_bVect+m_nByteLength,0,nOldByte-m_nByteLength);

  return m_bVect;
}


//***************************************************************************************
//-------------------------------------------------------------------
void CBV::AssignDiz(int nBitLength, const BYTE* v1, const BYTE* v2)
{
  int nLen = LEN_BYTE(nBitLength);
  if (m_nByteLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new BYTE[nLen];       // may throw an exception
    m_nByteLength = nLen;
    m_nAllocLength = nLen;
    m_nBitLength = nBitLength;
  }
  else
    if (nLen > m_nAllocLength) {
      Empty();
      goto NewMem;
    }
    else {
      m_nByteLength = nLen;
      m_nBitLength = nBitLength;
    }
  for (int i=0; i<m_nByteLength; i++) m_bVect[i] = v1[i] | v2[i];
}

//-------------------------------------------------------------------
void CBV::AssignDiz(int nBitLength, int Num, BYTE* v1, ...)
{
  int nLen = LEN_BYTE(nBitLength);
  if (m_nByteLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new BYTE[nLen];       // may throw an exception
    m_nByteLength = nLen;
    m_nAllocLength = nLen;
    m_nBitLength = nBitLength;
  }
  else
    if (nLen > m_nAllocLength) {
      Empty();
      goto NewMem;
    }
    else {
      m_nByteLength = nLen;
      m_nBitLength = nBitLength;
    }

  BYTE * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, BYTE* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nByteLength; i++) {
    m_bVect[i] = v[0][i];
    for (int j=1; j< Num; j++) {
      m_bVect[i] |= v[j][i];
    }
  }
}

//-------------------------------------------------------------------
void CBV::AssignCon(int nBitLength, const BYTE* v1, const BYTE* v2)
{
  int nLen = LEN_BYTE(nBitLength);
  if (m_nByteLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new BYTE[nLen];       // may throw an exception
    m_nByteLength = nLen;
    m_nAllocLength = nLen;
    m_nBitLength = nBitLength;
  }
  else
    if (nLen > m_nAllocLength) {
      Empty();
      goto NewMem;
    }
    else {
      m_nByteLength = nLen;
      m_nBitLength = nBitLength;
    }
  for (int i=0; i<m_nByteLength; i++) m_bVect[i] = v1[i] & v2[i];
}

//-------------------------------------------------------------------
void CBV::AssignCon(int nBitLength, int Num, BYTE* v1, ...)
{
  int nLen = LEN_BYTE(nBitLength);
  if (m_nByteLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new BYTE[nLen];       // may throw an exception
    m_nByteLength = nLen;
    m_nAllocLength = nLen;
    m_nBitLength = nBitLength;
  }
  else
    if (nLen > m_nAllocLength) {
      Empty();
      goto NewMem;
    }
    else {
      m_nByteLength = nLen;
      m_nBitLength = nBitLength;
    }

  BYTE * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, BYTE* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nByteLength; i++) {
    m_bVect[i] = v[0][i];
    for (int j=1; j< Num; j++) {
      m_bVect[i] &= v[j][i];
    }
  }
}

//-------------------------------------------------------------------
void CBV::AssignXor(int nBitLength, const BYTE* v1, const BYTE* v2)
{
  int nLen = LEN_BYTE(nBitLength);
  if (m_nByteLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new BYTE[nLen];       // may throw an exception
    m_nByteLength = nLen;
    m_nAllocLength = nLen;
    m_nBitLength = nBitLength;
  }
  else
    if (nLen > m_nAllocLength) {
      Empty();
      goto NewMem;
    }
    else {
      m_nByteLength = nLen;
      m_nBitLength = nBitLength;
    }
  for (int i=0; i<m_nByteLength; i++) m_bVect[i] = v1[i] ^ v2[i];
}

//-------------------------------------------------------------------
void CBV::AssignXor(int nBitLength, int Num, BYTE* v1, ...)
{
  int nLen = LEN_BYTE(nBitLength);
  if (m_nByteLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new BYTE[nLen];       // may throw an exception
    m_nByteLength = nLen;
    m_nAllocLength = nLen;
    m_nBitLength = nBitLength;
  }
  else
    if (nLen > m_nAllocLength) {
      Empty();
      goto NewMem;
    }
    else {
      m_nByteLength = nLen;
      m_nBitLength = nBitLength;
    }

  BYTE * v[10];
  va_list marker;
  v[0] = v1;
  va_start( marker, v1);     /* Initialize variable arguments. */
  for (int j=1; j< Num; j++) {
    v[j] = va_arg( marker, BYTE* );
  }
  va_end( marker );              /* Reset variable arguments.      */

  for (int i=0; i<m_nByteLength; i++) {
    m_bVect[i] = v[0][i];
    for (int j=1; j< Num; j++) {
      m_bVect[i] ^= v[j][i];
    }
  }
}

//-------------------------------------------------------------------
void CBV::AssignDif(int nBitLength, const BYTE* v1, const BYTE* v2)
{
  int nLen = LEN_BYTE(nBitLength);
  if (m_nByteLength == 0) { // new memory
    if (nBitLength==0)  return;     // Result vector is empty
NewMem:
    m_bVect = new BYTE[nLen];       // may throw an exception
    m_nByteLength = nLen;
    m_nAllocLength = nLen;
    m_nBitLength = nBitLength;
  }
  else
    if (nLen > m_nAllocLength) {
      Empty();
      goto NewMem;
    }
    else {
      m_nByteLength = nLen;
      m_nBitLength = nBitLength;
    }
  for (int i=0; i<m_nByteLength; i++) m_bVect[i] = v1[i] & ~v2[i];
}
