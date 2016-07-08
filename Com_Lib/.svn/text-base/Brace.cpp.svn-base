/////////////////////////////////////////////////////////////////
// Ver.1.1.1    15.02.2005   Russian == English
// Ver.1.1.2    07.04.2006  мелочи
/////////////////////////////////////////////////////////////////

#include <stdio.h>
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
#include "../Com_Lib/brace.h"


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

           
CBrace::CBrace(void)
{
 X_amount=Y_amount=Z_amount=0;
}

CBrace::CBrace(CBrace& Br)
{                        
 X_amount=Br.X_amount;
 Y_amount=Br.Y_amount;
 Z_amount=Br.Z_amount;

 Vars=Br.Vars;
 m_mask=Br.m_mask;
 for (int i=0;i<Y_amount+Z_amount;i++)
  {                                                  
   //m_DnfArray.SetAtGrow(i,Br.m_DnfArray.GetAt(i));
   m_DnfArray.Add(Br.m_DnfArray.GetAt(i));
   n_Fun.push_back(Br.n_Fun[i]); 
  } 
}

CBrace::~CBrace(void)
{
 n_Fun.clear();
 m_DnfArray.RemoveAll();
}

CBV CBrace::GetVars(int nFun)
{                         
 ASSERT (nFun<Vars.GetCountR());
 return Vars.GetRowBv(nFun);
}  

int CBrace::CountZ(int nFun)
{           
 ASSERT (nFun<Y_amount+Z_amount);
 int z=0,No=X_amount-1;
 while (((No=Vars.LeftOne(nFun,No))>0)&&(No<Vars.GetCountC()))
  if (GetMaskBit(No))
   z++;
 return z;   
}

bool CBrace::IsUsed(int nVar)
{
 ASSERT (nVar<Vars.GetCountC());
 for (int i=0;i<Vars.GetCountR();i++)
  if (Vars.GetBitAt(i,nVar)==1)
   return TRUE;
 return FALSE;
}

bool CBrace::IsUsed(int nVar,int nFun)
{             
 ASSERT (nVar<Vars.GetCountC());
 ASSERT (nFun<Vars.GetCountR());
 if (Vars.GetBitAt(nFun,nVar)==1)
   return TRUE;
 return FALSE;
}

void CBrace::SetMask(CBV mask)
{                               
 ASSERT(mask.GetBitLength()==X_amount+Z_amount);
 m_mask=mask;
}

void CBrace::SetMaskBit(int i,bool b_mask)
{                                             
 ASSERT(i<m_mask.GetBitLength());
 m_mask.SetBitAt(i,b_mask);
}

CBV CBrace::GetMask()
{
 return m_mask;
}                    

bool CBrace::GetMaskBit(int nVar)
{
 ASSERT(nVar<m_mask.GetBitLength());                              
 return (m_mask.GetBitAt(nVar)!=0);
}
               
void CBrace::SetDNF(int nDNF,CDNF DNF)
{                                    
  ASSERT(nDNF<Y_amount+Z_amount);
  CBV bv(0xFF,(X_amount+Z_amount));
 
 
  //SetAtGrow

  if ( nDNF == (int)m_DnfArray.Adnf.size() )
  {
    m_DnfArray.Adnf.push_back(&DNF);    
  }
  else 
    if (nDNF < (int)m_DnfArray.Adnf.size())
      m_DnfArray.Adnf[nDNF] = &DNF;
    else {
      m_DnfArray.SetSize(nDNF+10);
      m_DnfArray.Adnf.resize(nDNF);
      m_DnfArray.Adnf[nDNF] = &DNF;
    }
     //SetAtGrow  - конец
 
 
  bv.Zero();                                 
  for (int j=0;j<(m_DnfArray.GetAt(nDNF))->GetCountR();j++)
  {                                       
    bv|=(m_DnfArray.GetAt(nDNF))->GetRowDef(j);
  }                 
  Vars.SetRow(nDNF,bv); 
}
                
void CBrace::SetNumFun(int nFun, int Number)
{                             
 ASSERT(nFun<Y_amount+Z_amount);
 n_Fun[nFun]=Number;
}
              
void CBrace::SetFun(int Fun)
{
 ASSERT(Fun<Y_amount+Z_amount);
 for (int i=0;i<Y_amount+Z_amount;i++)
  if (Fun==-1) 
   n_Fun[i]=i;
  else
   n_Fun[i]=Fun;
}    
   
int CBrace::GetX(void)
{
 return X_amount;
}
 
int CBrace::GetY(void)
{
 return Y_amount;
} 
 
int CBrace::GetZ(void)
{
 return Z_amount;
} 

CDNF CBrace::GetDNF(int No)
{
 ASSERT(No<Y_amount+Z_amount);
 return (*m_DnfArray.GetAt(No));
}

int CBrace::GetNumFun(int nFun)
{
 ASSERT(nFun<Y_amount+Z_amount);
 return n_Fun[nFun];
} 
   
void CBrace::RemoveDNF(int No)
{            
 ASSERT(No<Y_amount+Z_amount);
 m_DnfArray.RemoveAt(No);
 for (int i=No;i<Y_amount+Z_amount-1;i++)
  {
   n_Fun[i]=n_Fun[i+1];
  }                                                     
 if (No<Y_amount)
  Y_amount--;
 else
  Z_amount--;
 Vars.RemoveRow(No);  
}

bool CBrace::Elemenation(int nDnf,int nVar) 
{                                              
 ASSERT (nVar-Y_amount<Z_amount);
 
 int No=X_amount+(n_Fun[nVar])-Y_amount;
 if (!Vars.GetBitAt(nDnf,No))
  return FALSE;
   
 CDNF dnf;                            
 CTV tv;
 char cB;
 
 for (int i=0,nRow=m_DnfArray.GetAt(nDnf)->GetCountR();i<nRow;i++)
  {      
   cB=m_DnfArray.GetAt(nDnf)->GetBitAt(i,No);
   if (cB!='-') 
    {                     
     if (cB=='0')
      dnf=m_DnfArray.GetAt(nVar)->Invertion();
     else
      dnf=*m_DnfArray.GetAt(nVar);
     m_DnfArray.GetAt(nDnf)->SetBitAt(i,No,'-');
     tv=m_DnfArray.GetAt(nDnf)->GetRowTv(i);
     dnf.Product(tv);
     m_DnfArray.GetAt(nDnf)->RemoveRow(i);                   
     m_DnfArray.GetAt(nDnf)->InsertRow(m_DnfArray.GetAt(nDnf)->GetCountR(),0,dnf,dnf.GetCountR());
     nRow--;
     i--;
    } 
  }
  
 CBV bv(0xFF,m_DnfArray.GetAt(nDnf)->GetCountC());
 bv.Zero();
 for (int j=0;j<m_DnfArray.GetAt(nDnf)->GetCountR();j++)
 {
  bv|=m_DnfArray.GetAt(nDnf)->GetRowDef(j);
 }                 
 Vars.SetRow(nDnf,bv); 
 
 return TRUE;
}        

void CBrace::BraceOpening(void)
{      
 int i,j,k,No=-1,Cnt=0;
 while ((No=m_mask.LeftOne(No))>=0)
 {
  i=No-X_amount+Y_amount;
  for (k=Y_amount;k<Y_amount+Z_amount;k++)
   if (n_Fun[k]==(WORD)i)
    break;
  i=k;  
  if (!CountZ(i))
  {               
   for (j=0;j<Y_amount+Z_amount;j++)
    if (i!=j)
     Elemenation(j,i);
   SetMaskBit(n_Fun[i]-Y_amount+X_amount,FALSE);
   RemoveDNF(i);
   Cnt++;
   No=-1;  
  } 
 }
                          
 for (i=0;i<Y_amount+Z_amount;i++)
  m_DnfArray.GetAt(i)->Conc();
}
 
void CBrace::UpdateVars(void)
{            
 CBV bv;//(0xFF,(X_amount+Z_amount));
 int i,j;
 for (i=0;i<X_amount+Z_amount;i++)
  bv.Concat(TRUE);
 Vars.SetSize(Y_amount+Z_amount,X_amount+Z_amount);           
 for (i=0;i<Y_amount+Z_amount;i++)
 {     
  bv.Zero();
  for (j=0;j<m_DnfArray.GetAt(i)->GetCountR();j++)
   {
    bv|=m_DnfArray.GetAt(i)->GetRowDef(j);
   }                 
  Vars.SetRow(i,bv); 
 }
 n_Fun.reserve(Y_amount+Z_amount);
}

int CBrace::EstimateLenDnf(int nDnf1, int nDnf2)
{                            
 ASSERT (nDnf2>=Y_amount);                   
 int nVar=nDnf2-Y_amount+X_amount,max=1, i;
 if (IsUsed(nVar,nDnf1))        
 {       
  for (i=0;i<m_DnfArray.GetAt(nDnf2)->GetCountR();i++)
   max=max*m_DnfArray.GetAt(nDnf2)->CountDefs(i);
  int k=m_DnfArray.GetAt(nDnf1)->GetCountR(); 

  for (i=0;i<m_DnfArray.GetAt(nDnf1)->GetCountR();i++)
   if (m_DnfArray.GetAt(nDnf1)->GetBitAt(i,nVar)!='-')
   {
    if (m_DnfArray.GetAt(nDnf1)->GetBitAt(i,nVar)=='1')
     k=k+m_DnfArray.GetAt(nDnf1)->GetCountR()-1;
    else   
     k=k+max-1;
   } 
  return k;
 } 
 else
  return m_DnfArray.GetAt(nDnf1)->GetCountR();
}

int CBrace::MaxCountTerms(void)
{
 int max=0; 
 for (int i=0;i<Y_amount+Z_amount;i++)
  if (m_DnfArray.GetAt(i)->GetCountR()>max)
   max=m_DnfArray.GetAt(i)->GetCountR();
 return max;
}

// #ifdef _DEBUG
// //-------------------------------------------------------
// void CBrace::AssertValid() const
// {
// }
// //-------------------------------------------------------
// void CBrace::Dump(CDumpContext& dc) const
// {                        
//  dc<<"CBrace";
// }
// #endif  

//IMPLEMENT_DYNCREATE(CDNFArray, CObArray)
//--------------------------------------------------------------- Constructor
CDNFArray::CDNFArray() {  }
//---------------------------------------------------------------
// int CDNFArray::Add(CDNF* pDNF)
// { 
// #ifdef _DEBUG
//   AssertValid();
//   pDNF->AssertValid();
// #endif     
//   
//   return CObArray::Add((CObject *) pDNF);
// }
//--------------------------------------------------------------- 

//--------------------------------------------------------------- 
// void CDNFArray::SetAtGrow( int nIndex, CDNF* newElement )
// {               
// #ifdef _DEBUG
//   AssertValid();
//   newElement->AssertValid();
// #endif     
//  CObArray::SetAtGrow(nIndex,(CObject *) newElement);
//}

//---------------------------------------------------------------  
void CDNFArray::RemoveAll()
{ CDNF * pDNF;
// #ifdef _DEBUG
//   AssertValid();
// #endif     
  for (int i=0; i< (int)Adnf.size(); i++){
    pDNF=(CDNF *)GetAt(i);
    delete (pDNF);
    }
  Adnf.clear();
}

//--------------------------------------------------------------- 
// void CDNFArray::RemoveAt(int i)
// { CDNF * pDNF;
// #ifdef _DEBUG
//   AssertValid();
// #endif     
//   ASSERT(i>=0);
//   pDNF=(CDNF *)GetAt(i);
//     delete (pDNF);
//   CObArray::RemoveAt(i);
// } 

// int CDNFArray::GetSize()
// {              
//  return CObArray::GetSize();
// } 

// void CDNFArray::SetSize( int nNewSize, int nGrowBy )
// {
//  CObArray::SetSize(nNewSize,nGrowBy);
// }

// #ifdef _DEBUG
// //-------------------------------------------------------
// void CDNFArray::AssertValid() const
// {
// }
// //-------------------------------------------------------
// void CDNFArray::Dump(CDumpContext& dc) const
// {                        
//  dc<<"CDNFArray";
// }
// #endif  
  