/////////////////////////////////////////////////////////////////
// Данный модуль включается как в систему САПР, так и в программу ESS!!!
//----------------------------------------------------------------------
// objects_sys.cpp 
//------------------------------------------------------------
// 1. CDoubleArray    - Array of double values
// 2. CBStack         - Stack with BYTES
// 3. CStrStack       - Stack with strategies names
// 4. CMapValFrame    - Map of values attributes for Frame
// 5. CPNodeArraySmall - Special array of nodes CPNode
// 6. CPNode           - node of SF net
// 7. CPNodeArray      - Array of nodes CPNode
// 8. CMapStringToNode - Map of strings for nodes
// 9. CMenuESS         - menu position in ESS
// 10. CArrayMenuESS   - Array of MenuESS positions
//---------------------------------------------------------------------WriteRead

//BOOL ReadDoc(CString FileName, CMapAttributes* pA, CMapProgModuls* pM,
//             CMapStrategies* pS, CMapFrames* pF, CWinApp *pApp)
//------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include <string>
#include <iostream>
#ifdef _MSVC9
#include <afx.h>
#include <afxwin.h>
#pragma warning(disable : 4996)
#endif
#include "../Com_Lib/sf.h"
#include "../Com_Lib/objects.h"
#include "../Com_Lib/objects_sys.h"

using namespace std;
class CSF;
/*
///////////////////////////////////////////////////////////// CDoubleArray
//
// 1. CDoubleArray    - Array of double value
//
/////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CDoubleArray, CObArray)
CDoubleArray::CDoubleArray() { }
CDoubleArray::~CDoubleArray() {RemoveAll(); }
//-----------------------------------------------------------
double* CDoubleArray::GetAt(int nIndex)  const
{ return (double*) CObArray::GetAt(nIndex); }
//-----------------------------------------------------------
void CDoubleArray::SetAt(int nIndex, double newElement)
{ 
  double* dd,*pD; dd = new double;  *dd = newElement;
  if (nIndex<GetSize()) {
    pD = GetAt(nIndex); delete pD;
  }
  CObArray::SetAt(nIndex, (CObject *) dd); 
}
//-----------------------------------------------------------
void CDoubleArray::SetAtGrow(int nIndex, double newElement)
{
  double* dd,*pD; dd = new double;  *dd = newElement;
  if (nIndex<GetSize()) {
    pD = GetAt(nIndex); delete pD;
  }
  CObArray::SetAtGrow(nIndex, (CObject *) dd); 
}
//-----------------------------------------------------------
int CDoubleArray::Add(double newElement)
{
  double* dd; dd = new double;  *dd = newElement;
  return CObArray::Add((CObject *) dd); 
}
//-----------------------------------------------------------
void CDoubleArray::RemoveAll()
{
  int i;
  double* pD;
  for (i=0; i<GetSize(); i++) { pD = GetAt(i); delete pD; }
  CObArray::RemoveAll();
}
*/


///////////////////////////////////////////////////////////// CBStack
//
// 2. CBStack    -  stack with BYTES
//
/////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////// CStrStack
//
// 3. CStrStack    -  Stack with strategies names
//
/////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////// CMapValFrame
//
// 4. CMapValFrame - Map of values attributes for Frame
//
/////////////////////////////////////////////////////////////
//-----------------------------------------------------------
int CMapValFrame::GetAt(string key, double& d)
{
  string res=mvf[key];
  if (res.size()==0) return 0;
  d=0;
#ifndef _LINUX
  d = atof((const char *)res.c_str());
#else
  sscanf(res.c_str(), "%lf", &d);
#endif
  return 1;
}
//-----------------------------------------------------------
void CMapValFrame::SetAt(string key, double newValue)
{
  char buf[100];
  string w;
#ifndef _MSVC9
  sprintf(buf, "%.2lf", newValue);
#else
  sprintf_s(buf,100, "%.2lf", newValue);
#endif
  w = buf;
  mvf[key] = w;
}
//-----------------------------------------------------------
// int  CMapValFrame::WriteToIni(string sFile, string Section)
// {
//   string key, value;
//   for (ivf = mvf.begin();ivf !=mvf.end(); )  {
//     GetNextAssoc(key, value);
// //    if ( WritePrivateProfileString(Section, key, value, sFile) == false)  return 0;
//   }
//   return 1;
// }
//-----------------------------------------------------------
// BOOL  CMapValFrame::ReadFromIni(CString sFile, CString Section, CStringArray &whatAttr)
// {
//   CString key, value;
//   char Buf[5000], Buf1[50];
//   int i, j;
//   CStringArray work;
// 
//   for (i=0; i< whatAttr.GetSize(); i++)
//     work.Add(whatAttr.GetAt(i)); 
// 
//   if (work.GetSize()== 0 )  return TRUE;
//   if ( GetPrivateProfileString(Section, NULL, "", Buf, 5000, sFile) == FALSE)
//   { return FALSE; }
//   i=0;
//   for (int num = 0;(int)strlen(Buf+i)>0 && num < work.GetSize(); num++)  {
//     key = Buf+i;
//     GetPrivateProfileString(Section, key, "",Buf1, 5000, sFile);
//     value = Buf1;
//     SetAt(key, value);
//     for (j=0; j<work.GetSize(); j++)  {
//       if (key == work.GetAt(j))  {
//         work.SetAt(j, "");
//         break;
//       }
//     }
//     if (j == work.GetSize())
//     {
// #ifdef _LINUX
//       printf("Procedure has set a attribute %s\nwhich is not required\n", key);
// #else
// #ifdef ENG
//       AfxMessageBox("Procedure has set a attribute " + key +
//         " which is not required");
// #else
//       AfxMessageBox("Проектная операция установила атрибут " + key +
//                     ", который не требуется");
// #endif
// #endif
//     }
//     i += (int)strlen(key)+1;
//   }
//   for (j=0; j<work.GetSize(); j++)
//     if (work.GetAt(j) != "")
//     {
// #ifdef _LINUX
//       printf("Procedure hasn't set the attribute %s\n", work.GetAt(j));
// #else
// #ifdef ENG
//       AfxMessageBox("Procedure hasn't set the attribute " + 
// #else
//       AfxMessageBox("Проектная операция не установила атрибут " + 
// #endif
//         work.GetAt(j) );
// #endif
//     }
//   CMapStringToString  *pSt = (CMapStringToString  *)this;
//   return TRUE;
// }

//-----------------------------------------------------------
int CMapValFrame::GetAt(CMapAttributes* m_pMapAttributes, string key,
  string& res, double& d)
{
  CKnAttr *pKnAttr=NULL;
  m_pMapAttributes->Lookup(key, pKnAttr);
  if (pKnAttr == NULL)  return -1;
  res = mvf[key];
  if (res.empty()) return -1;
  if (pKnAttr->m_Type == 2)  return 2;
#ifndef _LINUX
  d = atof(res.c_str());
#else
  sscanf(res.c_str(), "%lf", &d);
#endif
  return pKnAttr->m_Type;
}

//--------------------------------------------- 29.04.1998 Красильникова
bool CMapValFrame::CheckAttr(string key, string Value)
{
  string sRes;
  sRes = mvf[key];
  int bRes = (!sRes.empty()); 
  if(!bRes)  return 0;
  return( sRes == Value);
}

//--------------------------------------------- 29.04.1998 Красильникова
bool CMapValFrame::CheckAttr(string key, double Value)
{
  string sRes;
  double d;
  sRes = mvf[key];
  int bRes = (!sRes.empty()); 
  if(!bRes)  return 0;
#ifndef _LINUX
  d = atof((const char *)sRes.c_str());
#else
  sscanf(sRes.c_str(), "%lf", &d);
#endif

  return( d == Value);
}

//--------------------------------------------- void Serialize(CArch&);
void CMapValFrame::Serialize(CArch& ar)
{
  string key,val,s1="CMapValFrame";
  int Count;
  if (ar.IsStoring()) {
    Count = mvf.size();
    ar<<Count<<s1;
    ivf=mvf.begin();
    for (; ivf!=mvf.end();)  {
      s1 = ivf->first; val = ivf->second; ivf++;
      ar << s1 << val;
    }
  }
  else
  {
    ar>>Count>>key;
    if (key !=s1) { throw (-31);  }

    for(int i=0; i<Count; i++) {
      ar >> key >> val;
      mvf[key]=val;
    }
  }
}


///////////////////////////////////////////////////////////// CPNodeArraySmall
//
// 5. CPNodeArraySmall     -  Special array of nodes CPNode
//
/////////////////////////////////////////////////////////////

void CPNodeArraySmall::RemoveAll() 
{ 
  CPNode* p;
  for (int i=0; i<(int)apn.size(); i++) 
  { p = apn[i];  
    delete p; 
  }
  apn.clear(); 
}

//-------------------------------------------------------
void CPNodeArraySmall::Serialize(CArch& ar)
{
  string key, s1="CPNodeArray";
  CPNode *pNode;
  int Count,i;
  if (ar.IsStoring()) {
    Count = apn.size();
    ar<<Count<<s1;

    for (i=0; i<Count;i++)  {
      apn[i]->Serialize(ar);
    }
  }
  else
  {
    ar>>Count>>key;
    if (key !=s1)  { throw (-32);  }

    for(i=0; i<Count; i++) {
      pNode = new CPNode;
      pNode->Serialize(ar);
      apn.push_back(pNode);
    }
  }
}


///////////////////////////////////////////////////////////// CPNode
//
// 6. CPNode     -  node of SF net
//
/////////////////////////////////////////////////////////////
//Element  EK[i] contain reference to CPNode (it's index in array ???), 
//which is shown in row number i in view
 CPNode::~CPNode() 
 { 
   m_Attr.RemoveAll(); 
   m_Children.RemoveAll();  
   if (m_pSF != NULL)  delete m_pSF; 
 }

void CPNode::Serialize(CArch& ar)
{
  if (ar.IsStoring())    {   // storing code here
    ar<<m_PldName; ar<<m_level; ar<<m_NumStringInView;
  }
  else    {                 // loading code here
    ar>>m_PldName; ar>>m_level; ar>>m_NumStringInView;
    m_pSF = new CSF;
  }
  m_pSF->Serialize(ar);
  m_Attr.Serialize(ar);
  m_Children.Serialize(ar); 
}

/*
///////////////////////////////////////////////////// CPNodeArray

// 21. CPNodeArray     -  Array of nodes CPNode

/////////////////////////////////////////////////////
//---------------------------------------------------------------
int CPNodeArray::Add(CPNode* pNode) 
{ 
  apn.push_back(pNode);
  return (int)apn.size(); 
}

//----------------------------------- 
CPNode* CPNodeArray::GetAt(int nIndex)  
{
  if (nIndex>=(int)apn.size()) return NULL;
  CPNode *pNode = apn[nIndex];
  return pNode;
}

//-------------------------------------------------------
void CPNodeArray::RemoveAll() 
{
  for (int i=0; i< (int)apn.size(); i++)
    delete apn[i];
  apn.clear();
}

//-------------------------------------------------------
CPNode* CPNodeArray::GetChildWithAttrib(int ActNode, CKnAttr *pKnAttr)
{
  string ls, key;
  double d; 
  CPNode *pNode1;
  int i;

  CPNode *pNode = GetAt(ActNode);
  if (pNode==NULL) return NULL; 
  if ((int)pNode->m_Children.apn.size() == 0)  return NULL; 
  for (i=0; i< (int)pNode->m_Children.apn.size(); i++)  {
    pNode1 = pNode->m_Children.GetAt(i);
    if (pKnAttr->m_Type == 2)  {
      pNode1->m_Attr.GetAt(key, ls);
      if (ls == pKnAttr->m_StrValue)  return pNode1;
    }
    else  {
      pNode1->m_Attr.GetAt(key, d);
      if (d == pKnAttr->m_Value)  return pNode1;
    }
  }
  return NULL;
}

//-------------------------------------------------------
void CPNodeArray::Serialize(CArch& ar)
{
  string key, s1="CPNodeArray";
  CPNode *pNode;
  int Count,i;
  if (ar.IsStoring()) {
    Count = apn.size();
    ar<<Count<<s1;

    for (i=0; i<Count;i++)  {
      apn[i]->Serialize(ar);
    }
  }
  else
  {
    ar>>Count>>key;
    if (key !=s1)  { throw (-32);  }

    for(i=0; i<Count; i++) {
      pNode = new CPNode;
      pNode->Serialize(ar);
      apn.push_back(pNode);
    }
  }
}
*/

//////////////////////////////////////////////////////  CMapStringToNode
//
// 22. CMapStringToNode  - Map of strings for nodes
//
/////////////////////////////////////////////////////////////
//---------------------------------------------------------------
void CMapStringToNode::RemoveAll()
{
  string key;
  CPNode *pNode=NULL;
  for (ipn = mpn.begin();  ipn!=mpn.end(); )  {
    GetNextAssoc(key, pNode); 
    if (pNode!=NULL) delete pNode;
  }
  mpn.clear();
}

//---------------------------------------------------------------
void CMapStringToNode::RemoveKey(string Buf)
{
  CPNode *pNode=NULL;
  if (!Lookup(Buf,*&pNode)) return;
  if (pNode!=NULL) delete pNode;
  mpn.erase(Buf);
}

//---------------------------------------------------------------
bool CMapStringToNode::Lookup(string key, CPNode*& rValue)
{
  map <string,CPNode*> :: iterator ii = mpn.find(key); 
  if (ii==mpn.end()) return false;
  rValue = mpn[key]; 
  return true;
}

void CMapStringToNode::Serialize(CArch& ar)
{
  string key,val,s1="CMapStringToNode";
  int Count;
  CPNode *pNode=NULL;
  if (ar.IsStoring()) {
    Count = mpn.size();
    ar<<Count<<s1;
    for (ipn=mpn.begin(); ipn!=mpn.end();)  {
      GetNextAssoc(s1, pNode);
      ar<<s1;
      pNode->Serialize(ar);
    }
  }
  else
  {
    ar>>Count>>key;
    if (key !=s1) { throw (-33);  }

    for(int i=0; i<Count; i++) {
      pNode = new CPNode;
      ar>>key;
      pNode->Serialize(ar);
      mpn[key]=pNode;
    }
  }
}


#if 0
///////////////////////////////////////////////////////////// CMenuESS
//
// 20. CMenuESS       - menu position in ESS
//
/////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CMenuESS,CObject,0)
CMenuESS::CMenuESS() { }
CMenuESS::~CMenuESS() { }
//-----------------------------------------------------------
// Attributes

// void CMenuESS::Serialize(CArchive& ar)
// {
//   if (ar.IsStoring())
//   {
//     ar<<m_Name;  ar<<m_Label;  ar<<(int)m_hParent;  ar<<(int)m_hOwn;
//   }
//   else
//   {
//     ar>>m_Name;  ar>>m_Label; int v;
//     ar>>v; m_hParent = (struct _TREEITEM *)v;
//     ar>>v; m_hOwn = (struct _TREEITEM *)v;
//   }
// }

///////////////////////////////////////////////////////////// CArrayMenuESS
//
// 21.  CArrayMenuESS     - Array of MenuESS positions
//
/////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CArrayMenuESS, CObArray)
CArrayMenuESS::CArrayMenuESS() { }
//-----------------------------------------------------------
CMenuESS* CArrayMenuESS::GetAt(int nIndex)  const
{ return (CMenuESS*) CObArray::GetAt(nIndex); }
//-----------------------------------------------------------
int CArrayMenuESS::Add(CMenuESS* newElement)
{ return CObArray::Add((CObject *) newElement); }
//-----------------------------------------------------------
void CArrayMenuESS::RemoveAll()
{ CMenuESS* pMenuESS;
  for (int i=0; i<GetSize(); i++) { pMenuESS = GetAt(i); delete pMenuESS; }
  CObArray::RemoveAll();
}


//---------------------------------------------------------------------WriteRead
BOOL ReadDoc(CString FileName,
             CMapAttributes* pA, CMapProgModuls* pM,
             CMapStrategies* pS, CMapFrames* pF, CWinApp *pApp)
{
  CFile f;
  int li;
  CFrame *pFr;
  CSlot *pSl;
  CKnAttr *pKnAttr;
  CString ls, lsw1, lsw2;

  if( !f.Open( (const char *)FileName, CFile::modeRead ) ) return FALSE;
  CArchive ar( &f,CArchive::load);
  pA->Serialize(ar);
  pM->Serialize(ar);
  pS->Serialize(ar);
  pF->Serialize(ar);

#ifdef ENG
  lsw1 = "Target";
  lsw2 = "Final target";
#else
  lsw1 = "Цель";
  lsw2 = "Конечная цель";
#endif
  if (pF->Lookup(lsw1, pFr)) {
    for (li= 0; li< pFr->m_ArraySlots.GetSize(); li++)  {
      pSl =  pFr->m_ArraySlots.GetAt(li);
      if (pSl->m_Name != lsw2) continue;
      for (li=0; li< pSl->m_AttrNames.GetSize(); li++)  {
        ls = pSl->m_AttrNames.GetAt(li);
        if (pA->Lookup(ls, pKnAttr))  
        pKnAttr->m_Value = pApp->GetProfileInt("AIM", pKnAttr->m_ShortName, 0);
      }
      break;
    }
  }
  return TRUE;
}
#endif