/////////////////////////////////////////////////////////////////
// Ver.1.0.0    20.03.2006   Russian / English
//              - Вынесены в этот файл все функции Get/Set SDF CSFunction
//                и CSF::SdfToLog()
// Ver.1.0.1    07.04.2006
//              метод CSF::SdfToLog() перенесен в файл SF_methods.cpp
/////////////////////////////////////////////////////////////////

#include <vector>
#include <map>

using namespace std;
#include "../Com_Lib/objects.h"
#include "../Com_Lib/archive.h"
#include "../Com_Lib/sf.h"
#include "../Com_Lib/bufer.h"
#include "../Common/BaseBool.h"
#include "../Common/BaseTern.h"
#include "../Common/Sop.h"
#include "../Common/Sbf.h"


#ifdef _LINUX
#include <sys/io.h>
#undef _DEBUG
typedef unsigned char BYTE; 
typedef unsigned long ULONG; 
typedef int BOOL;

#define ASSERT assert
#include <assert.h>
#else
#include <io.h>
#include <afx.h>
#include <afxwin.h>
#pragma warning(disable : 4996)
#endif



////////////////////////////////////////////////////////////////////////////
//  BOOL CSFunction::GetSdf(CBM *&pTrMat_Zero, CBM *&pTrMat_One, CBM *&pMatFun)
//  BOOL CSFunction::GetSbf(CBM *&TrMat_Zero, CBM *&TrMat_One, 
//                          CBM *&MatFun_Zero, CBM *&MatFun_One); 
//  BOOL CSFunction::SetSdf(CBM *pTrMat_Zero, CBM *pTrMat_One, CBM *pMatFun)
//  BOOL CSFunction::SetSbf(CBM *TrMat_Zero, CBM *TrMat_One, 
//                          CBM *MatFun_Zero, CBM *MatFun_One); 
//  BOOL CSFunction::GetSdf(CSOP*& SDF)
//  BOOL CSFunction::GetSbf(CSBF *&SBF);
//  BOOL CSFunction::SetSdf(CSOP *SOP)
//  BOOL CSFunction::SetSbf(CSBF *SBF);

//-----------------------------------------------------------------------
bool CSFunction::GetSdf(CBM *&pTrMat_Zero, CBM *&pTrMat_One, CBM *&pMatFun)
{
  int i=0,j=0, k=0,  flag_one_or_two, num_row, num_col1, num_col2 ;
  string Buf;

  pTrMat_Zero = new CBM(m_NumInter,m_NumInt,FALSE);
  pTrMat_One  = new CBM(m_NumInter, m_NumInt,FALSE);
  pMatFun = new CBM(m_NumInter, m_NumOut,FALSE);

  if (m_Typ != "SDF")  return FALSE;
  flag_one_or_two = 0;
  num_row = num_col1 = num_col2 = 0;
  while (GetTextLine(j, Buf))  {
    k++;
    for (i=0; i<(int)Buf.size(); i++)  {
      if ((Buf[i] != '0') && (Buf[i] != '1') && (Buf[i] != '-'))  continue;
      if (flag_one_or_two == 0)  {       // Bool matr.
        if (Buf[i] == '0')     pTrMat_Zero->SetBitAt(num_row,num_col1,TRUE);
        else if (Buf[i] == '1') pTrMat_One->SetBitAt(num_row,num_col1,TRUE);
        num_col1++;
        if (num_col1 == m_NumInt)  {   // will be FUN matr
          num_col1 = 0;
          flag_one_or_two = 1;
        }
      }
      else  {
        if (Buf[i] == '1') pMatFun->SetBitAt(num_row,num_col2,TRUE);
        num_col2++;
        if (num_col2 == m_NumOut)  {   // will be FUN matr
          num_col2 = 0;
          flag_one_or_two = 0;
          num_row++;
        }
      }
    }
  }  //   while (GetTextLine(i, Buf))

  return TRUE;
}

//-----------------------------------------------------------------------
bool CSFunction::GetSbf(CBM *&pTrMat_Zero, CBM *&pTrMat_One, 
  CBM *&pMatFun_Zero, CBM *&pMatFun_One)
{
  int i=0,j=0, k=0,  flag_one_or_two, num_row, num_col1, num_col2 ;
  string Buf;

  pTrMat_Zero = new CBM(m_NumInter,m_NumInt,FALSE);
  pTrMat_One  = new CBM(m_NumInter, m_NumInt,FALSE);
  pMatFun_Zero = new CBM(m_NumInter, m_NumOut,FALSE);
  pMatFun_One = new CBM(m_NumInter, m_NumOut,FALSE);

  if (m_Typ != "SDF")  return FALSE;
  flag_one_or_two = 0;
  num_row = num_col1 = num_col2 = 0;
  while (GetTextLine(j, Buf))  {
    k++;
    for (i=0; i<(int)Buf.size(); i++)  {
      if ((Buf[i] != '0') && (Buf[i] != '1') && (Buf[i] != '-'))  continue;
      if (flag_one_or_two == 0)  {       // Bool matr.
        if (Buf[i] == '0')     pTrMat_Zero->SetBitAt(num_row,num_col1,TRUE);
        else if (Buf[i] == '1') pTrMat_One->SetBitAt(num_row,num_col1,TRUE);
        num_col1++;
        if (num_col1 == m_NumInt)  {   // will be FUN matr
          num_col1 = 0;
          flag_one_or_two = 1;
        }
      }
      else  {
        if (Buf[i] == '0')     pMatFun_Zero->SetBitAt(num_row,num_col2,TRUE);
        else if (Buf[i] == '1') pMatFun_One->SetBitAt(num_row,num_col2,TRUE);
        num_col2++;
        if (num_col2 == m_NumOut)  {   // will be FUN matr
          num_col2 = 0;
          flag_one_or_two = 0;
          num_row++;
        }
      }
    }
  }  //   while (GetTextLine(i, Buf))

  return TRUE;
}

//-----------------------------------------------------------------------
bool CSFunction::SetSdf(CBM *pTrMat_Zero, CBM *pTrMat_One, CBM *pMatFun)
{
  if (m_Typ != "SDF")  m_Typ = "SDF";
  // Контроль соответствия размеров матриц
  if (pTrMat_Zero->GetCountR() !=  pTrMat_One->GetCountR()) return FALSE;
  if (pTrMat_Zero->GetCountR() !=  pMatFun->GetCountR()) return FALSE;
  if (pTrMat_Zero->GetCountC() !=  pTrMat_One->GetCountC()) return FALSE;

  m_Text.clear();
  m_NumInt = pTrMat_Zero->GetCountC();
  m_NumOut = pMatFun->GetCountC();
  m_NumInter = pTrMat_Zero->GetCountR();

  string Buf;
  int i,j;

  for (i=0; i<pTrMat_Zero->GetCountR(); i++)  {
    Buf = "";
    for (j=0; j< pTrMat_Zero->GetCountC(); j++)  {
      if (pTrMat_Zero->GetBitAt(i,j))      Buf += '0';
      else if (pTrMat_One->GetBitAt(i,j))  Buf += '1';
      else                                 Buf += '-';
    }  // for (j=0; j< m_NumInt; j++)
    Buf += ' ';
    for (j=0; j< pMatFun->GetCountC(); j++)  {
      if (pMatFun->GetBitAt(i,j))      Buf += '1';
      else                             Buf += '0';
    }  // for (j=0; j< m_NumInt; j++)
    m_Text.push_back(Buf);
//    if (m_Text.size() % 100 == 0)
//      int lll = 0;
  }  // for (int i=0; i<m_NumInt; i++)

  return TRUE;
}

//-----------------------------------------------------------------------
bool CSFunction::SetSbf(CBM *pTrMat_Zero, CBM *pTrMat_One, 
  CBM *pMatFun_Zero, CBM *pMatFun_One)
{
  if (m_Typ != "SDF")  m_Typ = "SDF";
  // Контроль соответствия размеров матриц
  if (pTrMat_Zero->GetCountR() !=  pTrMat_One->GetCountR()) return FALSE;
  if (pTrMat_Zero->GetCountR() !=  pMatFun_One->GetCountR()) return FALSE;
  if (pTrMat_Zero->GetCountR() !=  pMatFun_Zero->GetCountR()) return FALSE;
  if (pTrMat_Zero->GetCountC() !=  pTrMat_One->GetCountC()) return FALSE;
  if (pMatFun_One->GetCountC() !=  pMatFun_Zero->GetCountC()) return FALSE;

  m_Text.clear();
  m_NumInt = pTrMat_Zero->GetCountC();
  m_NumOut = pMatFun_Zero->GetCountC();
  m_NumInter = pTrMat_Zero->GetCountR();

  string Buf;
  int i,j;

  for (i=0; i < m_NumInter; i++)  {
    Buf = "";
    for (j=0; j < m_NumInt; j++)  {
      if (pTrMat_Zero->GetBitAt(i,j))      Buf += '0';
      else if (pTrMat_One->GetBitAt(i,j))  Buf += '1';
      else                                 Buf += '-';
    }
    Buf += ' ';
    for (j=0; j< m_NumOut; j++)  {
      if (pMatFun_One->GetBitAt(i,j))       Buf += '1';
      else if (pMatFun_Zero->GetBitAt(i,j)) Buf += '0';
      else                                  Buf += '-';
    }
    m_Text.push_back(Buf);
  }
  return TRUE;
}

//-----------------------------------------------------------------
bool CSFunction::GetSdf(CSOP*& SDF)
{
  int i=0,j=0, k=0,  flag_one_or_two, num_row, num_col1, num_col2 ;
  string Buf;

  SDF = new CSOP();
  SDF->m_tmCon.SetSize(m_NumInter, m_NumInt, 10);
  SDF->m_bmFun.SetSize(m_NumInter, m_NumOut, 10);
  if (m_Typ != "SDF")  return FALSE;

  flag_one_or_two = 0;
  num_row = num_col1 = num_col2 = 0;
  while (GetTextLine(j, Buf))  {
    k++;
    for (i=0; i<(int)Buf.size(); i++)  {
      if ((Buf[i] != '0') && (Buf[i] != '1') && (Buf[i] != '-'))  continue;
      if (flag_one_or_two == 0)  {       // Bool matr.
        if (Buf[i] == '0')
          SDF->m_tmCon.SetBitAt(num_row,num_col1,'0');
        else if (Buf[i] == '1')
          SDF->m_tmCon.SetBitAt(num_row,num_col1,'1');
        num_col1++;
        if (num_col1 == m_NumInt)  {   // will be FUN matr
          num_col1 = 0;
          flag_one_or_two = 1;
        }
      }
      else  {
        if (Buf[i] == '1')
          SDF->m_bmFun.SetBitAt(num_row,num_col2,TRUE);
        num_col2++;
        if (num_col2 == m_NumOut)  {   // will be FUN matr
          num_col2 = 0;
          flag_one_or_two = 0;
          num_row++;
        }
      }
    }
  }  //   while (GetTextLine(i, Buf))

  return TRUE;
}

//-----------------------------------------------------------------
bool CSFunction::GetSbf(CSBF*& SBF)
{
  int i=0,j=0, k=0,  flag_one_or_two, num_row, num_col1, num_col2 ;
  string Buf;

  SBF = new CSBF();
  SBF->m_tmCon.SetSize(m_NumInter, m_NumInt, 10);
  SBF->m_tmFun.SetSize(m_NumInter, m_NumOut, 10);
  if (m_Typ != "SDF")  return FALSE;

  flag_one_or_two = 0;
  num_row = num_col1 = num_col2 = 0;
  while (GetTextLine(j, Buf))  {
    k++;
    for (i=0; i<(int)Buf.size(); i++)  {
      if ((Buf[i] != '0') && (Buf[i] != '1') && (Buf[i] != '-'))  continue;
      if (flag_one_or_two == 0)  {    // Matrix of conjunction
        if (Buf[i] == '0')
          SBF->m_tmCon.SetBitAt(num_row,num_col1,'0');
        else if (Buf[i] == '1')
          SBF->m_tmCon.SetBitAt(num_row,num_col1,'1');
        num_col1++;
        if (num_col1 == m_NumInt)  {  // will be FUN matr
          num_col1 = 0;
          flag_one_or_two = 1;
        }
      }
      else  {                         // Matrix of function
        if (Buf[i] == '0')
          SBF->m_tmFun.SetBitAt(num_row,num_col2,'0');
        else if (Buf[i] == '1')
          SBF->m_tmFun.SetBitAt(num_row,num_col2,'1');
        num_col2++;
        if (num_col2 == m_NumOut)  {   // will be FUN matr
          num_col2 = 0;
          flag_one_or_two = 0;
          num_row++;
        }
      }
    }
  }
  return TRUE;
}

//------------------------------------------------------------------
bool CSFunction::SetSdf(CSOP *SOP)
{
  if (m_Typ != "SDF")  m_Typ = "SDF";
  m_Text.clear();
  m_NumInt = SOP->GetnArg();
  m_NumOut = SOP->GetnFun();
  m_NumInter = SOP->GetnCon();

  string Buf;
  int i,j;

  for (i=0; i<SOP->GetnCon(); i++)  {
    Buf = "";
    for (j=0; j< m_NumInt; j++)  {
      if (SOP->m_tmCon.GetBitAt(i,j) == '0')       Buf += '0';
      else if (SOP->m_tmCon.GetBitAt(i,j) == '1')  Buf += '1';
      else                                         Buf += '-';
    }
    Buf += ' ';
    for (j=0; j< m_NumOut; j++)  {
      if (SOP->m_bmFun.GetBitAt(i,j) == 0)      Buf += '0';
      else                                      Buf += '1';
    }
    m_Text.push_back(Buf);
  }
  return TRUE;
}

//------------------------------------------------------------------
bool CSFunction::SetSbf(CSBF *SBF)
{
  if (m_Typ != "SDF")  m_Typ = "SDF";
  m_Text.clear();
  m_NumInt = SBF->GetnArg();
  m_NumOut = SBF->GetnFun();
  m_NumInter = SBF->GetnCon();

  string Buf;
  int i,j;

  for (i=0; i < m_NumInter; i++)  {
    Buf = "";
    for (j=0; j< m_NumInt; j++)  {
      if (SBF->m_tmCon.GetBitAt(i,j) == '0')       Buf += '0';
      else if (SBF->m_tmCon.GetBitAt(i,j) == '1')  Buf += '1';
      else                                         Buf += '-';
    }
    Buf += ' ';
    for (j=0; j< m_NumOut; j++)  {
      if (SBF->m_tmFun.GetBitAt(i,j) == '0')       Buf += '0';
      else if (SBF->m_tmFun.GetBitAt(i,j) == '1')  Buf += '1';
      else                                         Buf += '-';
    }
    m_Text.push_back(Buf);
  }
  return TRUE;
}


