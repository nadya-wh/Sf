using namespace std;

#include "../Com_Lib/objects.h"
#include "../Com_Lib/archive.h"
#include "../Com_Lib/sf.h"

//#ifdef _LINUX
//#undef _DEBUG
typedef unsigned char BYTE; 
typedef unsigned long ULONG; 
typedef int BOOL;

#define ASSERT assert
#include <assert.h>
//#else
//#include <afx.h>
//#include <afxwin.h>
//#endif

//-------------------------------------------------------
CSFVar::CSFVar(){}
//-------------------------------------------------------
CSFVar::CSFVar(string Name, BYTE Typ, int First, int Last)
{
  ASSERT(Name != "");
  m_Name = Name;
  ASSERT(Typ == 'g' || Typ == 'i' || Typ == 'o' || Typ == 't' );
  m_Typ  = Typ;
  ASSERT(First>=0);
  m_First = First;
  ASSERT(Last >=0 && Last>=First);
  m_Last = Last;
  m_NumInSF = -1;
  m_Neg="";
}

//-------------------------------------------------------
CSFVar::CSFVar(CSFVar *pSFVar)
{
  m_Name = pSFVar->m_Name;
  m_Typ  = pSFVar->m_Typ;
  m_First = pSFVar->m_First;
  m_Last = pSFVar->m_Last;
  m_NumInSF = -1;
  m_Neg=pSFVar->m_Neg;
}

//-------------------------------------------------------
bool CSFVar::Compare(CSFVar *pSFVar1)
{
  if (m_Name != pSFVar1->m_Name) return false;
  if (m_Typ  != pSFVar1->m_Typ) return false;
  if (m_First != pSFVar1->m_First) return false;
  if (m_Last != pSFVar1->m_Last) return false;
  return true;
}

//-------------------------------------------------------
bool CSFVar::CompareName(CSFVar *pSFVar1)
{
  if (m_Name != pSFVar1->m_Name) return false;
  return true;
}
//-------------------------------------------------------
CSFVar * CSFVar::operator =(CSFVar *pSFVar1)
{
  m_Name = pSFVar1->m_Name;
  m_Typ  = pSFVar1->m_Typ;
  m_First = pSFVar1->m_First;
  m_Last = pSFVar1->m_Last;
  m_Neg=pSFVar1->m_Neg;
  return this;
}

//-------------------------------------------------------
void CSFVar::Serialize(CArch& ar)
{
  if (ar.IsStoring())
  {
    ar <<  m_Name;
    ar <<  m_Typ;
    ar <<  m_First << m_Last << m_NumInSF;
  }
  else
  {
    ar >>  m_Name;
    ar >>  m_Typ;
    ar >>  m_First >> m_Last >> m_NumInSF;
  }
}


//-------------------------------------------------------
string CSFVar::BuildVectorName()
{
  string Buf;
  if (m_Last==0)
    Buf = m_Name;
  else  {
    char b[10], c[10];
#ifndef _MSVC9
//    itoa(m_First, b, 10);
//    itoa(m_Last, c, 10);
    sprintf(b, "%u", m_First);
    sprintf(c, "%u", m_Last);
#else
    _itoa_s(m_First, b, 10, 10);
    _itoa_s(m_Last, c, 10, 10);
#endif

    Buf = m_Name + "[" + b+ "-" + c + "]";
  }
  return (Buf);
}

/////////////////////////////////////////////////////////////////////////////
// CSFVarArray
//---------------------------------------------------------------
int CSFVarArray::Add(CSFVar* pVar)
{
  int i = IsVarInArray(pVar);
  if (i >=0)  return i;
  Avar.push_back(pVar);
  return Avar.size();
}

//--------------------------------------------------------------- 02.07.2009
int CSFVarArray::Add_M(CSFVar* pVar)
{
  Avar.push_back(pVar);
  return Avar.size();
}

//---------------------------------------------------------------
int CSFVarArray::IsVarInArray(CSFVar* pVar)
{
  CSFVar* pVar1;

  for (int i=0; i<(int)Avar.size(); i++)  {
    pVar1 = Avar[i];
    if (pVar->Compare(pVar1)) return i;
  }
  return -1;
}

//---------------------------------------------------------------
int CSFVarArray::IsNameInArray(CSFVar* pVar)
{
  int j = Avar.size();
  for (int i=0; i<j; i++)  {
    if (pVar->CompareName(Avar[i])) return i;
  }
  return -1;
}

//-------------------------------------------------------
void CSFVarArray::RemoveAt(int i)
{
  ASSERT(i>=0);
  vector<CSFVar*>::iterator it =Avar.begin() + i;
  delete *it;
  Avar.erase(it);
}

//-------------------------------------------------------
void CSFVarArray::RemoveAll()
{
  vector<CSFVar*>::iterator it;
  for (int i=0; i < (int)Avar.size(); i++) {
    delete Avar[i];
  }
  Avar.clear();
}
  
void CSFVarArray::Serialize(CArch& ar)
{
  CSFVar* pVar;
  int i, Count = Avar.size();
  string key, s1="CSFVarArray";
  if (ar.IsStoring()) {
    ar<<Count<<s1;
    for (i=0;i<Count; i++) {
      pVar =Avar[i];
      pVar->Serialize(ar);
    }
  }
  else {
    ar>>Count>>key;
    if (key !=s1)  { throw (-6);  }

    for(int i=0; i<Count; i++) {
      pVar = new CSFVar;
      pVar->Serialize(ar);
      Avar.push_back(pVar);
    }
  }
}


////////////////////////////////////////////////////////////////////////////
//class CSFType : public CObject
//  IMPLEMENT_SERIAL(CSFType, CObject, 3)

//-------------------------------------------------------
CSFType::CSFType(){}
//-------------------------------------------------------
bool CSFType::GetNextInVar(int &nIndex, CSFVar*& pVar)
{
  for ( ; nIndex< (int) m_SFVarArray.Avar.size(); nIndex++)  {
    pVar = (CSFVar*)m_SFVarArray.GetAt(nIndex);
    if (pVar->GetTyp() == 'i')  {
      nIndex++;
      return true;
    }
  }
  return false;
}

//-------------------------------------------------------
CSFType::CSFType(CSFType *pSFType)
{
  m_Name = pSFType->GetName();
  m_Lib = pSFType->GetLib();
  m_Reg = pSFType->GetReg();

  int i=0;
  string Buf;
  while((Buf = pSFType->GetElem(i))!="")  {
    i++;
    AddElem(Buf);
  }

  i=0;
  CSFVar *pVar, *pVar1;
  while(pSFType->GetNextInVar(i,*&pVar))  {
    pVar1 = new CSFVar(pVar);
    AddVar(pVar1);
  }
  i=0;
  while(pSFType->GetNextOutVar(i,*&pVar))  {
    pVar1 = new CSFVar(pVar);
    AddVar(pVar1);
  }
}

//-------------------------------------------------------
bool CSFType::GetNextOutVar(int &nIndex, CSFVar*& pVar)
{
  for ( ; nIndex< (int)m_SFVarArray.Avar.size(); nIndex++)  {
    pVar = (CSFVar*)m_SFVarArray.GetAt(nIndex);
    if (pVar->GetTyp() == 'o')  {
      nIndex++;
      return true;
    }
  }
  return false;
}

//---------------------------------------------------------------
void CSFType::Serialize(CArch& ar)
{
  if (ar.IsStoring())
  {
    ar <<  m_Name <<  m_Lib <<  m_Reg;
  }
  else
  {
    ar >>  m_Name >>  m_Lib >>  m_Reg;
  }
  m_SFVarArray.Serialize(ar);
  Seria(m_ElemArray, ar);
}

//--------------------------Кириенко 14 дек. 1998
// Подпрограмма проверки наличия имени в массиве имен
int CSFType::IsNameInArray(string Name)
{
  int j = m_ElemArray.size();
  for (int i=0; i<j; i++)  {
    if (Name == m_ElemArray[i]) return i;
  }
  return -1;
}
//------------------------------------------------------------
int CSFType::AddElem(string Elem)
{
  int i;
  ASSERT(Elem != "");
  if ((i=IsNameInArray(Elem))>=0) return(i); //Имя доб. только 1 раз.
  m_ElemArray.push_back(Elem); 
  return m_ElemArray.size();
}
//------------------------------------------
int CSFType::DelElem(string Elem)
{
  int i;
  string  pa;
  ASSERT(Elem != "");
  if ((i=IsNameInArray(Elem))>=0)
  {
    vector <string>::iterator It=m_ElemArray.begin()+i;
	It->clear();
    m_ElemArray.erase(It); // Если имя есть, то удалить его
  }
  return (i);
}

//---------------------------------------------------------------
string CSFType::GetElem(int i)
{
  if (i< (int)m_ElemArray.size())
    return(m_ElemArray[i]);
  else
    return "";
}

/////////////////////////////////////////////////////////////////////////////
// CMapStringToType
//---------------------------------------------------------------
void CMapStringToType::RemoveAll()
{
  string key;
  CSFType *pType;
  for (itp = mtp.begin();  itp!=mtp.end(); )  {
    GetNextAssoc(key, *&pType); delete pType;
  }
  mtp.clear(); 
}

bool CMapStringToType::Lookup(string key, CSFType*& rValue)   
{ 
itp = mtp.find(key);
if (itp==mtp.end()) 
	return false;
rValue = mtp[key]; 
return true;
}

void CMapStringToType::RemoveKey(string Buf)
{
  CSFType *pType;
  if (Lookup(Buf, pType) == true)  {
    mtp.erase(Buf);
    delete pType;
  }
}
void CMapStringToType::Serialize(CArch& ar)
{
  string key, s1="CMapStringToType";
  CSFType *pType;
  int Count;
  if (ar.IsStoring()) {
    Count = mtp.size();
    ar<<Count<<s1;
    itp=mtp.begin();
    for (; itp!=mtp.end();)  {
      GetNextAssoc(s1, *&pType);
      pType->Serialize(ar);
    }
  }
  else
  {
    ar>>Count>>key;
    if (key !=s1)  { throw (-7);  }

    for(int i=0; i<Count; i++) {
      pType = new CSFType;
      pType->Serialize(ar);
      mtp[pType->m_Name]=pType;
    }
  }
}

////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------
CSFConnect::CSFConnect(){}
//-------------------------------------------------------
bool CSFConnect::GetFirstPare(string& one, string& two)
{
  if (m_Connect.size()==0)  return false;
  iCon = m_Connect.begin();
  GetNextAssoc(one, two);
  return true;
}

//-------------------------------------------------------
bool CSFConnect::GetNextPare(string& one, string& two)
{
  if (iCon==m_Connect.end())  return false;
  GetNextAssoc(one, two);
  return true;
}

//-------------------------------------------------------
bool CSFConnect::GetPareForString(string one, string& two)
{
  ASSERT(one != "");
  map <string, string> :: iterator iC=m_Connect.find(one);
  if (iC==m_Connect.end())  return false;
  two = m_Connect[one];
  if (two!="")  return true;
  return false;
}

//-------------------------------------------------------
void CSFConnect::AddPare(string one, string two)
{
  m_Connect[one] = two;
}

//---------------------------------------------------------------
void CSFConnect::Serialize(CArch& ar)
{
  if (ar.IsStoring()) { ar <<  m_Name; }
  else                { ar >>  m_Name; }
  Seria(m_Connect, ar);
}

/////////////////////////////////////////////////////////////////////////////
// CConnectArray

void CConnectArray::RemoveAll() 
{ 
  for (int i=0; i < (int)Acon.size(); i++) delete Acon[i];

  Acon.clear(); 
}
//-------------------------------------------------------
void CConnectArray::RemoveAt(int i)  // 04.05.2000, Кириенко
{
  ASSERT(i>=0);
  vector <CSFConnect*>::iterator ii=Acon.begin()+i;
  delete *ii;
  Acon.erase(ii);
}

//-------------------------------------------------------
void CConnectArray::Serialize(CArch& ar)
{
  string key, s1="CConnectArray";
  CSFConnect* pCon;
  int Count,i;
  if (ar.IsStoring()) {
    Count = Acon.size();
    ar<<Count<<s1;

    for (i=0; i<Count;i++)  {
      pCon =Acon[i];   
      pCon->Serialize(ar);
    }
  }
  else
  {
    ar>>Count>>key;
    if (key !=s1) { throw (-8); }

    for(i=0; i<Count; i++) {
      pCon = new CSFConnect("Con");
      pCon->Serialize(ar);
      Acon.push_back(pCon);
    }
  }
}



////////////////////////////////////////////////////////////////////////////
//class CSFunction : public CObject
//---------------------------------------------------------------
CSFunction::CSFunction(){}
//---------------------------------------------------------------
CSFunction::CSFunction(string Typ, int NumInt, int NumOut, int NumInter)  //??
{                            //It's nesesary to insert TRY-CATCH
  ASSERT(Typ == "LOG" || Typ == "SDF");
  m_Typ = Typ;
  ASSERT(NumInt>=0);    // Кириенко, 6.12.00
  m_NumInt = NumInt;
  ASSERT(NumOut>=0);
  m_NumOut = NumOut;
  ASSERT(NumInter>=0);   // Кириенко, 6.12.00
  m_NumInter = NumInter;
}

//---------------------------------------------------------------
CSFunction::CSFunction(CSFunction *pFun)
{
  int i;
  string buf;
  m_Typ = pFun->GetTyp();
  m_NumInt = pFun->GetNumInt();
  m_NumOut = pFun->GetNumOut();
  m_NumInter = pFun->GetNumInter();
  i = 0;
  while(pFun->GetTextLine(i, buf))
    AddTextLine(buf);
}

//---------------------------------------------------------------
bool CSFunction::GetTextLine(int& index, string &szRes)
{
  if (index < (int) m_Text.size())  {
    szRes = m_Text[index++];
    return true;
  }
  return false;
}

//-------------------------------------------
int CSFunction::AddTextLine(string szRes)
{ m_Text.push_back(szRes);
  return(m_Text.size());
}

//----------------------------------------------
bool CSFunction::ReplaceTextLine(int index, string szRes)
{
  if (index >= (int)m_Text.size())  return false;
  m_Text[index] = szRes;
  return true;
}

//---------------------------------------------------------------
void CSFunction::Serialize(CArch& ar)
{
  if (ar.IsStoring())
  {
    ar <<  m_Typ;
    ar << m_NumInt;
    ar << m_NumOut;
    ar << m_NumInter;
  }
  else
  {
    ar >>  m_Typ;
    ar >> m_NumInt;
    ar >> m_NumOut;
    ar >> m_NumInter;
  }
  Seria(m_Text, ar);
}

//--------------------------------
void CSFunction::SetNumInt(int i)
{
  ASSERT (i>=0);
  m_NumInt=i;
}

//--------------------------------
void CSFunction::SetNumOut(int i)
{
  ASSERT (i>=0);
  m_NumOut=i;
}

//--------------------------------
void CSFunction::SetNumInter(int i)
{
  ASSERT (i>=0);
  m_NumInter=i;
}

