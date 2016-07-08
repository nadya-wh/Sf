/////////////////////////////////////////////////////////////////
// Ver.1.1.1    15.02.2005   Russian == English
/////////////////////////////////////////////////////////////////
#include <string>
#include <vector>
#include <utility>
#include <map>

using namespace std;

#ifdef _LINUX
#include <sys/io.h>
#undef _DEBUG
typedef unsigned char BYTE; 
typedef unsigned long ULONG; 
typedef int BOOL;

#define TRUE 1
#define FALSE 0

#define ASSERT assert
#include <assert.h>
#else
#include <io.h>
#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
//#include "stdafx.h"
#ifndef FO_
#define FO_
#define FSTD(_x) friend _x AFXAPI 
#define STD(_x) _x AFXAPI 
#endif
#endif

#include "../Common/BaseBool.h"
#include "../Common/BaseTern.h"

typedef std::vector<CTM> MTM;
typedef std::pair <CTV, int> PairTv_I;
typedef std::vector <PairTv_I> MTv_I;
typedef std::vector <MTv_I> MMTv_I;

#include "../Common/Sop.h"
#include "../Common/Sbf.h"

#include "../Com_Lib/bufer.h"
#include "../Com_Lib/sf.h"
#include "../Com_Lib/sflog.h"

#include "../ComBool/ShortBool.h"
#include "../ComBool/ShortTern.h"


#include "../Com_Lib/objects.h"
#include "../Com_Lib/archive.h"


#include "../Com_Lib/brace.h"

bool CSF::GetBrace(CBrace *pBrace)
{                 
  int i=0, j=0, No=-1,k;
  string Buf;
  vector <unsigned short> Nm;
//  CWordArray Nm;                           
  
  CSFunction *pF=GetFun();
  if (pF->GetTyp()=="LOG")      
   {        
    LogToSdfWithoutElimination();
    pF=GetFun();
   } 
  if (pF->GetTyp()!= "SDF")  return FALSE;
  int Out=m_NumOutVar,Int=m_NumInVar,Inter=m_Fun->GetNumInt()-m_NumInVar;
  CTV tv(Int+Inter,'0');
  Nm.resize(Out+Inter);   // SetSize
  pBrace->m_DnfArray.SetSize(Out+Inter);
  CDNF *dnf;
  for (i=0;i<Out+Inter;i++)
   {    
    dnf=new CDNF;   
    pBrace->m_DnfArray.Add(dnf); //SetAtGrow(i,dnf);
    pBrace->m_DnfArray.GetAt(i)->SetSize(0,Int+Inter,100);
    Nm[i]=0;
   } 
  while (pF->GetTextLine(j, Buf))  
   {                  
    No=-1;                     
    k=0;
    while (Buf[k]==' ') k++;
    while (Buf[k]!=' ') k++;
    while (Buf[k]==' ') k++;
    for (i=k;i<(int)Buf.size();i++)
     if (Buf[i]=='1')
      {
       No=i-k;
       break;
      }   
    if (No>=0)                  
     {
      Nm[No]=Nm[No]+1;  
      k=0;
      while (Buf[k]==' ') k++;
      pBrace->m_DnfArray.GetAt(No)->Add(tv);                      
      for (i=k;i<k+Int+Inter;i++)
       pBrace->m_DnfArray.GetAt(No)->SetBitAt(Nm[No]-1,i,Buf[i]);
     }  
   }            
                   
  pBrace->X_amount=Int;
  pBrace->Y_amount=Out;
  pBrace->Z_amount=Inter;
  
  pBrace->n_Fun.resize(Out+Inter);
  pBrace->Vars.SetSize(Out+Inter,Int+Inter);
  for (i=0;i<Out+Inter;i++) pBrace->n_Fun[i]=i;
  for (i=0;i<Int;i++) pBrace->m_mask.Concat(FALSE);
  for (i=0;i<Inter;i++) pBrace->m_mask.Concat(TRUE);

  pBrace->UpdateVars();
 
  return TRUE;
                                                       
}
//---------------------------------------------------------------
bool CSF::SetBrace(CBrace *pBrace)
{                              
 CSFunction *pF;              
 pF=new CSFunction("SDF",pBrace->X_amount+pBrace->Z_amount,pBrace->Y_amount+pBrace->Z_amount,0);
 m_NumInterVar=0;                           
 
 CSFVar* pVar;                                    
 int Cnt=pBrace->X_amount;
 for (int ind=0 ; ind< (int)m_SFVarArray.Avar.size(); ind++)  
 {   
  pVar = (CSFVar*)m_SFVarArray.Avar[ind];
// #ifdef _DEBUG
//   pVar->AssertValid();
// #endif     
  if (pVar->GetTyp() == 't')  
  {        
   if (!pBrace->IsUsed(Cnt))
   { 
    m_SFVarArray.RemoveAt(ind);
    ind--; 
   } 
   else       
    m_NumInterVar++;
   Cnt++;
  } 
 }  
 
 int i,j,No; //,No1=0;
 CBV bv=pBrace->GetMask();
 bv.Zero();
 for (i=0;i<pBrace->X_amount;i++)
  bv.SetBitAt(i,TRUE);
 for (i=pBrace->X_amount;i<pBrace->Vars.GetCountC();i++)
  if (pBrace->IsUsed(i))
   bv.SetBitAt(i,TRUE);
  else
   bv.SetBitAt(i,FALSE); 

 int nDNF=pBrace->m_DnfArray.GetSize();
 string s;
 string w('0',nDNF);
 if (nDNF==0) return FALSE;

 for (i=0;i<nDNF;i++)
  {
   w[i] = '1';
   for (j=0;j<pBrace->m_DnfArray.GetAt(i)->GetCountR();j++)
    {      
     s="";    
     No=-1;
     while ((No=bv.LeftOne(No))>=0)
      s=s+pBrace->m_DnfArray.GetAt(i)->GetBitAt(j,No);
     s=s+' '+w;         
     pF->AddTextLine(s); 
     pF->SetNumInter(pF->GetNumInter()+1);
    }
   w[i] ='0'; 
  }  
 SetFun(pF);
  
 return TRUE;
}

bool CSF::OpenBraces(void)
{ 
 CSFunction *pF=GetFun();
 
 if (pF==NULL) return FALSE;
 CBrace *pBr;
 pBr = new CBrace;
 if (!GetBrace(pBr)) return FALSE;
 pBr->BraceOpening();         
 if (!SetBrace(pBr)) return FALSE;   
 delete pBr;                         
 return TRUE;
}