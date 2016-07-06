/////////////////////////////////////////////////////////////////
// Данный модуль включается как в систему САПР, так и в программу ESS!!!
//----------------------------------------------------------------------
// objects.cpp : implementation of the CObject class
//------------------------------------------------------------
// 1.      CKnAttr     -          A T T R I B U T E
// 2. CMapAttributes   - Map  of  A T T R I B U T I E S
// 3.      CExpres     -          E X P R E S S I O N
// 4. CArrayExpression - Array of E X P R E S S I O N S
// 5.     CProduction  -          P R O D U C T I O N
// 6.      CProgMod    -          P R O G R A M  M O D U L
// 7.  CMapProgModuls  - Map  of  P R O G R A M  M O D U L S
// 8.      CSlot       -          S L O T  O F  F R A M E
// 9.  CArraySlots     - Array of S L O T S  O F  F R A M E
// 10.      CFrame      -          F R A M E
// 11.   CMapFrames    - Map  of  F R A M E
// 12.CArrayProduction - Array of P R O D U C T I O N S
// 13.     CStrategy   -          S T R A T E G Y
// 14. CMapStrategies  - Map  of  S T R A T E G I E S
//------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#ifdef _MSVC9
#include <afx.h>
#include <afxwin.h>
#pragma warning(disable : 4996)
#endif
#include "../Com_Lib/objects.h"

using namespace std;
void  PrintMap(map<string,string> m_pMapValFrame, string a);
int compare( const void *arg1, const void *arg2 );

int compare( const void *arg1, const void *arg2 )
{  return strcmp( (char*) arg1,  (char*) arg2 ); }

void Seria(vector <string>& ms, CArch& ar)
{
  int Count,i;
  string key, s1="VectorStr";
  if (ar.IsStoring()) {
    Count = ms.size();
    ar << Count << s1;
    for (i=0; i<Count; i++) {
      ar<<ms[i];
    }
  } 
  else {
    ar >> Count >> key;
    if (key != s1) { throw (-4); }

    for (i=0; i<Count; i++) {
      ar >> s1;
      ms.push_back(s1);
    }
  }
}

void Seria(map <string, string>& ms, CArch& ar)
{
  int Count,i;
  string key, s1="MapStrStr";
  if (ar.IsStoring()) {
    Count = ms.size();
    ar << Count << s1;
    map <string, string>::iterator iI=ms.begin();
    for (; iI!=ms.end();)  {
      key =  iI->first;   s1 = iI->second;   iI++; 
      ar << key << s1;
    }
  } 
  else {
    ar >> Count >> key;
    if (key != s1) { throw (-5); }

    for (i=0; i<Count; i++) {
      ar >> key >> s1;
      ms[key]=s1;
    }
  }
}


///////////////////////////////////////////////////////////// CKnAttr
//
// 1.      CKnAttr     - A T T R I B U T E
//
/////////////////////////////////////////////////////////////
void CKnAttr::Serialize(CArch& ar)
{
  if (ar.IsStoring()) {
    ar<< m_ShortName;
    ar<< m_FullName;
    ar<<m_Type; ar<<m_Active; ar<<m_Dyn;
    if (m_Type>1)  { 
      ar<< m_StrValue;
    }
    else           ar<<m_Value;
  }
  else {
    ar>>m_ShortName;
    ar>>m_FullName;
    ar>>m_Type;  ar>>m_Active; ar>>m_Dyn;
    if (m_Type>1) { ar>>m_StrValue;  m_Value = 0; }
    else          { ar>>m_Value; m_StrValue = ""; }
  }
}

//-----------------------------------------------------------
 CKnAttr& CKnAttr::operator=(const CKnAttr& atr) {
   // Attributes
   m_FullName = atr.m_FullName;    // For full name of attributes
   m_ShortName = atr.m_ShortName;  // For short name of attributes
   m_Type = atr.m_Type;            // 0 - logical, 1 - integer, 2 - string
   m_Active = atr.m_Active;            // 1 - active, 0 - passive
   m_Dyn = atr.m_Dyn;                  // 1 - dynamic, 0 - static
   m_StrValue = atr.m_StrValue;        // For string value
   m_Value = atr.m_Value;              // For logical or integer value
   return *this;
 }

//-----------------------------------------------------------
///////////////////////////////////////////////////////////// CMapAttributes
//
// 2. CMapAttributes   - Map of A T T R I B U T I E S
//
/////////////////////////////////////////////////////////////
//-----------------------------------------------------------
bool CMapAttributes::Lookup(string key, CKnAttr*& rValue)   
{ 
  map <string,CKnAttr*> :: iterator ii = mpa.find(key); 
  if (ii==mpa.end()) return false;
  rValue = mpa[key]; 
  return true;
} 

//-----------------------------------------------------------
void CMapAttributes::RemoveAll() 
{
  string key;
  CKnAttr *pAtr=NULL;
  for (iA = mpa.begin();  iA!=mpa.end(); )  {
    GetNextAssoc(key, pAtr); if (pAtr!=NULL) delete pAtr;
  }
  mpa.clear(); 
}

//-----------------------------------------------------------
void CMapAttributes::PrintMapAttribs(string FileName)
{
  string key, s1;
  CKnAttr *pKnAttr=NULL;
  char buf[30];

  try
  {
#ifndef _LINUX
    _unlink(FileName.c_str());
#else
    unlink(FileName.c_str());
#endif
    FILE *out;
    out =fopen(FileName.c_str(),"w");
    //CFile f(FileName, CFile::modeCreate | CFile::modeWrite);
    iA=mpa.begin();
    for (; iA!=mpa.end();)  {
      GetNextAssoc(s1, pKnAttr);
#ifndef _MSVC9
      sprintf(buf,"%10.2lf\n",pKnAttr->m_Value);
#else
      sprintf_s(buf,30, "%10.2lf\n",pKnAttr->m_Value);
#endif

      s1 += (" -> " + pKnAttr->m_StrValue + " " + buf);
      fwrite(s1.c_str(), s1.length(),1,out);
    }
  }
#ifndef _LINUX
  catch(CFileException e)
#else
  catch( ... )
#endif
  {
#ifdef _LINUX
    printf("Error at the record of file\n");
#else
#ifdef ENG
    AfxMessageBox("Error at the record of file ");
#else
    AfxMessageBox("Ошибка при записи файла ");
#endif
#endif
    return;
  }
//  END_CATCH
}

//-----------------------------------------------------------
void CMapAttributes::PrintFullAttribs(string FileName)
{
  string key, s1;
  CKnAttr* pKnAttr=NULL;
  char Buf[256][256];
  int Count=0;
  try
  {
#ifndef _LINUX
    _unlink(FileName.c_str());
#else
    unlink(FileName.c_str());
#endif
    FILE *out;
    out =fopen(FileName.c_str(),"w");
    //CFile f(FileName, CFile::modeCreate | CFile::modeWrite);
    iA=mpa.begin();
    for (; iA!=mpa.end();)  {
      GetNextAssoc(s1, pKnAttr);
      if (pKnAttr->m_Type == 0) 
        s1 +="  (logical)\n   "; 
      else if (pKnAttr->m_Type == 1) 
        s1 +="  (numeric)\n   "; 
      else
        s1 +="  (string)\n   "; 
      s1 += pKnAttr->m_FullName + "\n\n";
#ifndef _MSVC9
      strcpy(Buf[Count++],(const char*)s1.c_str());
#else
      strcpy_s(Buf[Count++],256,(const char*)s1.c_str());
#endif
    }
    qsort(Buf,Count,256,compare);
    for (int i=0; i<Count; i++)
      fwrite(Buf[i],strlen(Buf[i]),1,out);
  }
#ifndef _LINUX
  catch(CFileException e)
#else
  catch( ... )
#endif
  {
#ifdef _LINUX
    printf("Error at the record of file\n");
#else
#ifdef ENG
    AfxMessageBox("Error at the record of file ");
#else
    AfxMessageBox("Ошибка при записи файла ");
#endif
#endif
    return;
  }
//  END_CATCH
}

//-----------------------------------------------------------
void CMapAttributes::Serialize(CArch& ar)
{
  string key, s1="CMapAttributes";
  CKnAttr* pKnAttr=NULL;
  int Count;
  if (ar.IsStoring()) {
    Count = mpa.size();
    ar<<Count<<s1;
    iA=mpa.begin();
    for (; iA!=mpa.end();)  {
      GetNextAssoc(s1, pKnAttr);
      ar<<s1;
      pKnAttr->Serialize(ar);
    }
  }
  else
  {
    ar>>Count>>key;
    if (key !=s1) { throw (-24);  }
    for(int i=0; i<Count; i++) {
      ar>>key;
      pKnAttr = new CKnAttr;
      pKnAttr->Serialize(ar);
      mpa[key]=pKnAttr;
    }
  }
}

///////////////////////////////////////////////////////////// CExpres
//
// 3.      CExpres     - E X P R E S S I O N
//
/////////////////////////////////////////////////////////////
//---------------------------------------------
void CExpres::Serialize(CArch& ar)
{
  if (ar.IsStoring()) 
  { ar<<m_Type <<m_Formula <<m_Value; }
  else
  { ar>>m_Type>> m_Formula>> m_Value; }
}

///////////////////////////////// /////////////////////////// CArrayExpression
//
// 4. CArrayExpression - Array of E X P R E S S I O N
//
/////////////////////////////////////////////////////////////
void CArrayExpression::Serialize(CArch& ar)
{
  string key, s1="CArrayExpression";
  CExpres* pExpres;
  int Count,i;
  if (ar.IsStoring()) {
    Count = ae.size();
    ar<<Count<<s1;
    
    for (i=0; i<Count;i++)  {
    pExpres = ae[i];   
      pExpres->Serialize(ar);
    }
  }
  else
  {
    ar>>Count>>key;
    if (key !=s1) { throw (-25);  }

    for(i=0; i<Count; i++) {
      pExpres = new CExpres;
      pExpres->Serialize(ar);
      ae.push_back(pExpres);
    }
  }
}

//-----------------------------------------------------------
void CArrayExpression::RemoveAll() 
{  
  for (int i=0; i < (int)ae.size(); i++)
  { delete ae[i];  }
  ae.clear(); 
}

///////////////////////////////////////////////////////////// CProduction
//
// 5.      CProduction - P R O D U C T I O N
//
/////////////////////////////////////////////////////////////
CProduction::~CProduction() { m_ArrayExpression.RemoveAll(); }
//-----------------------------------------------------------
void CProduction::Serialize(CArch& ar)
{
  m_ArrayExpression.Serialize(ar);
  if (ar.IsStoring()) {
    ar<<m_Formula <<m_Type <<m_NameCon;
  }
  else {
    ar>>m_Formula >>m_Type >>m_NameCon;
  }
}

//----------------------------------------------------------- CreateCondition
string CProduction::CreateCondition()
{
  int k,num,p0;
  string Formula;
  string W,W1;
  CExpres* pExpres;

  num = m_ArrayExpression.ae.size();
  Formula="";
  W = m_Formula;
  while (!W.empty()) {
    if (W[0]!= '{') { 
      Formula+=W[0]; 
      W.erase(0);   //      W=W.Right(W.GetLength()-1); 
      continue; 
    }
    Formula += "{ "; 

    W.erase(0); //    W=W.Right(W.GetLength()-1);
    p0 = W.find('}');
    if (p0 <0) return "Error!!!";
    W1 = W.substr(0, p0);           //W.Left(p0);

    W.erase(0, p0+1); // W =  W.Right(W.GetLength()-p0-1);


#ifndef _LINUX
    k = atoi(W1.c_str());
#else
    sscanf(W1.c_str(), "%u", &k);;
#endif

    if (k<1 || k>num) return "Error!!!";
    k--;
    pExpres = m_ArrayExpression.GetAt(k);
    Formula +=pExpres->m_Formula + " }";
  }
  return Formula;
}

//-----------------------------------------------------------
void CProduction::operator =( CProduction* src )
{
  m_ArrayExpression.RemoveAll();
  int i,k;
  CExpres *pExpres;
  CExpres *pExpresOld;
  k = src->m_ArrayExpression.ae.size();
  for (i = 0; i<k; i++) {
    pExpres = new CExpres ();
    pExpresOld = src->m_ArrayExpression.GetAt(i);
    pExpres->m_Formula = pExpresOld->m_Formula;
    pExpres->m_Type = pExpresOld->m_Type;
    pExpres->m_Value = pExpresOld->m_Value;
    m_ArrayExpression.Add(pExpres);
  }
  m_Formula = src->m_Formula;
  m_Type = src->m_Type;
  m_NameCon = src->m_NameCon;
}

///////////////////////////////////////////////////////////// CProgMod
//
// 6.      CProgMod    - P R O G R A M  M O D U L
//
/////////////////////////////////////////////////////////////
//-----------------------------------------------------------
void CProgMod::Serialize(CArch& ar)
{
  if (ar.IsStoring()) {
    ar<<m_Name; ar<<m_Comment; ar<<m_Menu; ar<<m_Type;  ar<<m_Undo;
   }
   else { 
     ar>>m_Name; ar>>m_Comment; ar>>m_Menu; ar>>m_Type; ar>>m_Undo;
   }
   m_Prod.Serialize(ar);
   Seria(m_AttrNames, ar);
   Seria(m_ParamNames, ar);
}

///////////////////////////////////////////////////////////// CMapProgModuls
//
// 7.  CMapProgModuls  - Map of P R O G R A M  M O D U L S
//
/////////////////////////////////////////////////////////////
//-----------------------------------------------------------
void CMapProgModuls::RemoveAll()
{
  string key;
  CProgMod *pProgMod=NULL;
  
  for (iPm = mpr.begin();  iPm!=mpr.end(); )  {
    GetNextAssoc(key, pProgMod); if (pProgMod!=NULL)  delete pProgMod;
  }
  mpr.clear();
}

//-----------------------------------------------------------
bool  CMapProgModuls::Lookup(string key, CProgMod*& rValue)  
{ 
  map <string,CProgMod*> :: iterator ii = mpr.find(key); 
  if (ii==mpr.end()) return false;
  rValue = mpr[key]; 
  return true;
}

//-----------------------------------------------------------
void CMapProgModuls::PrintFullProgs(string FileName)
{
  string key, s1, endl;
  CProgMod *pProgMod=NULL;
  char Buf[64][512];
  int Count=0;
  
///////////////////////////////////////////////////////
//   CString m_Name;         // Name of program modul
//   CString m_Menu;         // String to menu of main program
//   CString m_Comment;      // Comment for program modul
//   BOOL m_Undo;            // Possibility of Undo operation
//   BYTE m_Type;            // 0 - internal Windows, 1 - external Windows,
                            // 2 - DOS  3 - special DOS  4... - reserved
//   CStringArray m_AttrNames;   // Map of modul setting attributes
//   CStringArray m_ParamNames;   // Map of modul setting parameters
//   CProduction m_Prod;     // Condition of execution

  try
  {
    s1 = "------------------------------\n";
    endl = "\n";
#ifndef _LINUX
    _unlink(FileName.c_str());
#else
    unlink(FileName.c_str());
#endif
    //CFile f(FileName, CFile::modeCreate | CFile::modeWrite);
    FILE *out;
    string w;
    int j;
    out = fopen(FileName.c_str(), "w");
    fwrite(s1.c_str(), s1.length(), 1, out);
    for (iPm = mpr.begin(); iPm!=mpr.end(); )  {
      GetNextAssoc(s1, pProgMod);
      w = pProgMod->m_Menu + endl;
      s1 = w + s1;
      s1 += endl + "   ";
      w = pProgMod->m_Comment;
      while ((j=w.find_first_of(endl.c_str()))>=0) 
        w = w.substr(0,j) + w.substr(j+1); //w = w.Left(j) + w.Mid(j+1);
      s1 += w + endl + "   ";
      w = pProgMod->m_Prod.CreateCondition();
      if (!w.empty())
#ifdef ENG
        s1 += w + "\t\t- start condition";
        s1 += endl +"   ";
      if (!pProgMod->m_Undo) {
        s1 += "Backoff is impossible!";
        s1 += endl +"   ";
      }
#else
        s1 += w + "\t\t- условие запуска";
        s1 += endl + "   ";
      if (!pProgMod->m_Undo) {
        s1 += "Нет возможности отката!";
        s1 += endl + "   ";
      }
#endif

      if (pProgMod->m_ParamNames.size()>0) 
      {
#ifndef ENG
        s1 += "Параметры запуска: ";
#else
        s1 += "Start parameters: ";
#endif
        for (int yy=0; yy < (int)pProgMod->m_ParamNames.size(); yy++)
        {
          s1 += pProgMod->m_ParamNames[yy].c_str();
          s1 += "  ";
        }
        s1 += endl +"   ";
      }

      if (pProgMod->m_AttrNames.size()>0) 
      {
#ifndef ENG
        s1 += "Устанавливаемые атрибуты: ";
#else
        s1 += "Established attributes: ";
#endif
        for (int yy=0; yy < (int)pProgMod->m_AttrNames.size(); yy++)
        {
          s1 += pProgMod->m_AttrNames[yy].c_str();
          s1 += "  ";
        }
        s1 += endl +"   ";
      }
#ifndef ENG
      s1 += "Тип модуля: ";
#else
      s1 += "Type of modul: ";
#endif
      switch(pProgMod->m_Type)
      {
#ifdef ENG
      case 0:s1 += "Windows application"; break; 
      case 1:s1 += "DOS application"; break;
      case 2:s1 += "Special DOS program"; break;
      case 3:s1 += "Internal for Windows"; break;
#else
      case 0:s1 += "приложение Windows"; break; 
      case 1:s1 += "приложение DOS"; break;
      case 2:s1 += "специальный DOS"; break;
      case 3:s1 += "внутренний Windows"; break;
#endif
      }
      s1 += endl +endl+"------------------------------";
      s1 += endl;
#ifndef _MSVC9
      strcpy(Buf[Count++],(const char*)s1.c_str());
#else
      strcpy_s(Buf[Count++],512,(const char*)s1.c_str());
#endif
    }
    qsort(Buf,Count,512,compare);
    for (int i=0; i<Count; i++)
      fwrite(Buf[i], strlen(Buf[i]), 1, out); //f.Write(Buf[i], strlen(Buf[i]));
  }
#ifndef _LINUX
  catch(CFileException e)
#else
  catch( ... )
#endif
  { 
#ifdef _LINUX
    printf("Error at the record of file\n");
#else
#ifdef ENG
    AfxMessageBox("Error at the record of file ");
#else
    AfxMessageBox("Ошибка при записи файла ");
#endif
#endif
    return;
  }
//  END_CATCH
}

//-----------------------------------------------------------
void CMapProgModuls::Serialize(CArch& ar)
{
  string key, s1="CMapProgModuls";
  CProgMod* pProgMod=NULL;
  int Count;
  if (ar.IsStoring()) {
    Count = mpr.size();
    ar<<Count<<s1;
    iPm=mpr.begin();
    for (; iPm!=mpr.end();)  {
      GetNextAssoc(s1, pProgMod);
      ar<<s1;
      pProgMod->Serialize(ar);
    }
  }
  else
  {
    ar>>Count>>key;
    if (key !=s1) { throw (-26);  }

    for(int i=0; i<Count; i++) {
      ar>>key;
      pProgMod = new CProgMod;
      pProgMod->Serialize(ar);
      mpr[key]=pProgMod;
    }
  }
}


///////////////////////////////////////////////////////////// CSlot
//
// 8.      CSlot       - S L O T  O F  F R A M E
//
/////////////////////////////////////////////////////////////
void CSlot::Serialize(CArch& ar)
{
  Seria(m_AttrNames, ar);
  if (ar.IsStoring())  ar<<m_Name;   else  ar>>m_Name;
}

///////////////////////////////////////////////////////////// CArraySlots
//
// 9.  CArraySlots     - Array of S L O T S  O F  F R A M E
//
/////////////////////////////////////////////////////////////
void CArraySlots::RemoveAll() 
{ 
  for (int i=0; i < (int)as.size(); i++) delete as[i];
  as.clear(); 
}

//-----------------------------------------------------------
CSlot* CArraySlots::GetForName(string Name)
{
  int i;
  CSlot* pSlot;
  for (i=0; i<(int)as.size(); i++)
  {  pSlot = as[i];
    if (pSlot->m_Name == Name) return pSlot;  }
  return NULL;
}

//-----------------------------------------------------------
void CArraySlots::Serialize(CArch& ar)
{
  string key, s1="CArraySlots";
  CSlot* pSlot;
  int Count,i;
  if (ar.IsStoring()) {
    Count = as.size();
    ar<<Count<<s1;

    for (i=0; i<Count;i++)  {
      pSlot =as[i];   
      pSlot->Serialize(ar);
    }
  }
  else
  {
    ar>>Count>>key;
    if (key !=s1) { throw (-27);  }

    for(int i=0; i<Count; i++) {
      pSlot = new CSlot;
      pSlot->Serialize(ar);
      as.push_back(pSlot);
    }
  }
}

///////////////////////////////////////////////////////////// CFrame
//
// 10.      CFrame      - F R A M E
//
/////////////////////////////////////////////////////////////
//-----------------------------------------------------------
void CFrame::Serialize(CArch& ar)
{
  if (ar.IsStoring()) { ar<<m_Name; ar<<m_Comment; ar<<m_Specimen; }
  else                { ar>>m_Name; ar>>m_Comment; ar>>m_Specimen; }
  m_ArraySlots.Serialize(ar);
}

///////////////////////////////////////////////////////////// CMapFrames
//
// 11.   CMapFrames    - Map  of  F R A M E
//
/////////////////////////////////////////////////////////////
//-----------------------------------------------------------
bool CMapFrames::Lookup(string key, CFrame*& rValue)
{ 
  map <string,CFrame*> :: iterator ii = mfr.find(key); 
  if (ii==mfr.end()) return false;
  rValue = mfr[key]; 
  return true;
}

//-----------------------------------------------------------
void CMapFrames::RemoveAll()
{
  string key;
  CFrame *pFrame=NULL;

  for (iFr = mfr.begin();  iFr!=mfr.end(); )  {
    GetNextAssoc(key, pFrame); if (pFrame!=NULL)  delete pFrame;  
  }
  mfr.clear();
}
//------------------------------------------------------------
CFrame* CMapFrames::GetFrame(int Num)
{
  string key;
  CFrame *pFrame=NULL;
  int n=0;
  for (iFr = mfr.begin();  iFr!=mfr.end(); n++)  {
    GetNextAssoc(key, pFrame); 
    if (Num == n) return pFrame;
  }
  return NULL;
}

//---------------------------------------------------------------
void CMapFrames::Serialize(CArch& ar)
{
  string key, s1="CMapFrames";
  CFrame* pFrame=NULL;
  int Count;
  if (ar.IsStoring()) {
    Count = mfr.size();
    ar<<Count<<s1;
    iFr=mfr.begin();
    for (; iFr!=mfr.end();)  {
      GetNextAssoc(s1, pFrame);
      ar<<s1;
      pFrame->Serialize(ar);
    }
  }
  else
  {
    ar>>Count>>key;
    if (key !=s1) { throw (-28);  }

    for(int i=0; i<Count; i++) {
      ar>>key;
      pFrame = new CFrame;
      pFrame->Serialize(ar);
      mfr[key]=pFrame;
    }
  }
}



///////////////////////////////////////////////////////////// CArrayProduction
//
// 12.CArrayProduction - Array of P R O D U C T I O N S
//
/////////////////////////////////////////////////////////////

void CArrayProduction::RemoveAll()
{ 
  for(int i=0; i <(int)apr.size(); i++ ) delete apr[i];
  apr.clear(); 
}
//-----------------------------------------------------------
void CArrayProduction::Serialize(CArch& ar)
{
  string key, s1="CArrayProduction";
  CProduction* pProduction;
  int Count,i;
  if (ar.IsStoring()) {
    Count = apr.size();
    ar<<Count<<s1;

    for (i=0; i<Count;i++)  {
      pProduction =apr[i];   
      pProduction->Serialize(ar);
    }
  }
  else
  {
    ar>>Count>>key;
    if (key !=s1)  { throw (-29);  }

    for(i=0; i<Count; i++) {
      pProduction = new CProduction;
      pProduction->Serialize(ar);
      apr.push_back(pProduction);
    }
  }
}

///////////////////////////////////////////////////////////// CStrategy
//
// 13.     CStrategy   - S T R A T E G Y
//
/////////////////////////////////////////////////////////////
//-----------------------------------------------------------
void CStrategy::Serialize(CArch& ar)
{
  if (ar.IsStoring()) { ar<<m_Name; ar<<m_Comment; }
  else                { ar>>m_Name; ar>>m_Comment; }
  m_ArrayProduction.Serialize(ar);
}

/////////////////////////////////////////////////////////////
//
// 14.  CMapStrategies  - Map  of  S T R A T E G I E S
//
/////////////////////////////////////////////////////////////
//-----------------------------------------------------------
bool CMapStrategies::Lookup(string key, CStrategy*& rValue) 
{
  map <string,CStrategy*> :: iterator ii = mst.find(key); 
  if (ii==mst.end()) return false;
  rValue = mst[key]; 
  return true;
}

//-----------------------------------------------------------
void CMapStrategies::RemoveAll()
{
  string key;
  CStrategy *pS=NULL;
  
  for (iSt=mst.begin(); iSt!=mst.end(); ) 
  { GetNextAssoc(key,pS); if (pS!=NULL) delete pS; }
  mst.clear();
}
//------------------------------------------------------------
CStrategy* CMapStrategies::GetStrategy(int Num)
{ 
  string key;
  CStrategy *pStrategy=NULL;
  int n=0;
  for (iSt = mst.begin();  iSt!=mst.end(); n++)  {
    GetNextAssoc(key, pStrategy); 
    if (Num == n) return pStrategy;
  }
  return NULL;
}

//---------------------------------------------------------------
void CMapStrategies::Serialize(CArch& ar)
{
  string key, s1="CMapFrames";
  CStrategy* pStrategy=NULL;
  int Count;
  if (ar.IsStoring()) {
    Count = mst.size();
    ar<<Count<<s1;
    iSt=mst.begin();
    for (; iSt!=mst.end();)  {
      GetNextAssoc(s1, pStrategy);
      ar<<s1;
      pStrategy->Serialize(ar);
    }
  }
  else
  {
    ar>>Count>>key;
    if (key !=s1) { throw (-30);  }

    for(int i=0; i<Count; i++) {
      ar>>key;
      pStrategy = new CStrategy;
      pStrategy->Serialize(ar);
      mst[key]=pStrategy;
    }
  }
}
