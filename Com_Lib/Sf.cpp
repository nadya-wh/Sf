/////////////////////////////////////////////////////////////////
// Ver.1.1.1    15.02.2005   Russian / English
// Ver.1.1.2    01.03.2006
//              - добавлено 2 новых метода Кириенко 
// Ver.1.1.3    20.03.2006
//              - Общая поправка формы, введение частичных функций,
//                вынесены функции записи в файл Write_sf.cpp
//                вынесен в Objects класс CMapStringToNode
//                добавлена из SF_elem функция RenameVar
// Ver 1.1.5    06.04.2006
//               включены две функции в CSF 
//  BOOL GetFirstTypeName(CSFType *& pType, POSITION &pos,CString & buf) ;  //22.01.03  Кардаш
//  BOOL GetNextTypeName(CSFType *& pType, POSITION &pos, CString & buf) ;  //22.01.03  Кардаш
// 07.04.2006 Исключены по согласованию в виду повторения другими
// 31.10.2007 Восстановлены, так как их прямые аналоги отсутствуют!
//void ChangeSubstr(string str1,string str2="")    //Кириенко 19 февраля 2015 г.
// Заменить подстроку str1 на подстроку str2 в именах переменных в разделе CSFunction 
/////////////////////////////////////////////////////////////////

//#include <io.h>
#include <vector>
#include <map>
using namespace std;
#include "../Com_Lib/sf.h"
#include "../Com_Lib/objects_sys.h"


////////////////////////////////////////////////////////////////////////////
//class CSF : public CObject
//  IMPLEMENT_SERIAL(CSF, CObject, 1)

//-------------------------------------------------------------------
CSF::CSF() {
  m_Fun = NULL;
  m_NumInVar = 0;
  m_NumOutVar = 0;
  m_NumInterVar = 0;
}

//-------------------------------------------------------------------
CSF::~CSF()
{
    m_SFVarArray.RemoveAll();
    m_InterMap.clear();
    m_TypeMap.RemoveAll();
    m_ElemTypeMap.clear();
    m_ConnectArray.RemoveAll();
    m_NetMap.clear();
    if (m_Fun) delete m_Fun;
    m_Attr.RemoveAll();
}

//-------------------------------------------------------------------08.01.00
CSFVar *CSF::GetInVar(int nIndex)      //nIndex - number in the list of input var.
{
  CSFVar *pVar; //, *pVar1=NULL;

  for (int i=0 ; i< (int)m_SFVarArray.Avar.size(); i++)  {
    pVar = m_SFVarArray.Avar[i];
    if (pVar->GetTyp() != 'i')    continue;
    if (pVar->m_NumInSF == nIndex) return pVar;
  }
  return NULL;
}

//-------------------------------------------------------------------08.01.00
CSFVar *CSF::GetOutVar(int nIndex)    //nIndex - number in the list of output var.
{
  CSFVar *pVar;
  for (int i=0 ; i< (int)m_SFVarArray.Avar.size(); i++)  {
    pVar = (CSFVar*)m_SFVarArray.Avar[i];
    if (pVar->GetTyp() != 'o')  continue;
    if (pVar->m_NumInSF == nIndex)  return pVar;
  }
  return NULL;
}

//-------------------------------------------------------------------08.01.00
CSFVar *CSF::GetInterVar(int nIndex)   //nIndex - number in the list of inter var.
{
  CSFVar *pVar;
  for (int i=0 ; i< (int)m_SFVarArray.Avar.size(); i++)  {
    pVar = (CSFVar*)m_SFVarArray.Avar[i];
    if (pVar->GetTyp() != 't')  continue;
    if (pVar->m_NumInSF == nIndex)  return pVar;
  }
  return NULL;
}

//-------------------------------------------------------------------08.01.00
string CSF::GetNameInVar(int nIndex)      //nIndex - number in the list of input var.
{
  CSFVar *pVar;
  int li_num;
  char b[20];
  string ls_Res;

  pVar = GetInVar(nIndex);
  if (!pVar)  return "";
  if (pVar->m_First == pVar->m_Last)  return pVar->m_Name;
  li_num = pVar->m_First + nIndex - pVar->m_NumInSF;
#ifndef _MSVC9
  //itoa(li_num, b, 10);
  sprintf(b, "%u", li_num);
#else
  _itoa_s(li_num, b, 20, 10);
#endif
  ls_Res =  pVar->m_Name + "[" + b + "]";
  return  ls_Res;
}

//-------------------------------------------------------------------08.01.00
string CSF::GetNameOutVar(int nIndex)      //nIndex - number in the list of output var.
{
  CSFVar *pVar;
  int li_num;
  char b[20];
  string ls_Res;

  pVar = GetOutVar(nIndex);
  if (!pVar)  return "";
  if (pVar->m_First == pVar->m_Last)  return pVar->m_Name;
  li_num = pVar->m_First + nIndex - pVar->m_NumInSF;
#ifndef _MSVC9
//  itoa(li_num, b, 10);
    sprintf(b, "%u", li_num);
#else
  _itoa_s(li_num, b, 20, 10);
#endif
  ls_Res =  pVar->m_Name + "[" + b + "]";
  return  ls_Res;
}

//-------------------------------------------------------------------08.01.00
string CSF::GetNameInterVar(int nIndex)      //nIndex - number in the list of inter var.
{
  CSFVar *pVar;
  int li_num;
  char b[20];
  string ls_Res;

  pVar = GetInterVar(nIndex);
  if (!pVar)  return "";
  if (pVar->m_First == pVar->m_Last)  return pVar->m_Name;
  li_num = pVar->m_First + nIndex - pVar->m_NumInSF;
#ifndef _MSVC9
//  itoa(li_num, b, 10);
  sprintf(b, "%u", li_num);
#else
  _itoa_s(li_num, b, 20, 10);
#endif
  ls_Res =  pVar->m_Name + "[" + b + "]";
  return  ls_Res;
}

//-------------------------------------------------------------------
bool CSF::GetNextGlobalVar(int &nIndex, CSFVar*& pVar)
{
  for ( ; nIndex<(int)m_SFVarArray.Avar.size(); nIndex++)  {
    pVar = (CSFVar*)m_SFVarArray.Avar[nIndex];
    if (pVar->GetTyp() == 'g')  {
      nIndex++;
      return true;
    }
  }
  return false;
}

//-------------------------------------------------------------------
bool CSF::GetNextInVar(int &nIndex, CSFVar*& pVar)
{
  for ( ; nIndex<(int)m_SFVarArray.Avar.size(); nIndex++)  {
    pVar = (CSFVar*)m_SFVarArray.Avar[nIndex];
    if (pVar->GetTyp() == 'i')  {
      nIndex++;
      return true;
    }
  }
  return false;
}

//-------------------------------------------------------------------
bool CSF::GetNextOutVar(int &nIndex, CSFVar*& pVar)
{
  for ( ; nIndex<(int)m_SFVarArray.Avar.size(); nIndex++)  {
    pVar = (CSFVar*)m_SFVarArray.Avar[nIndex];
    if (pVar->GetTyp() == 'o')  {
      nIndex++;
      return true;
    }
  }
  return false;
}

//-------------------------------------------------------------------29.12.97
bool CSF::GetNextInterVar(int &nIndex, CSFVar*& pVar)
{
  for ( ; nIndex<(int)m_SFVarArray.Avar.size(); nIndex++)  {
    pVar = (CSFVar*)m_SFVarArray.Avar[nIndex];
    if (pVar->GetTyp() == 't')  {
      nIndex++;
      return true;
    }
  }
  return false;
}

//-------------------------------------------------------------------24.03.2000
//                                                          Кириенко Н.А.
bool CSF::GetNextVar(int &nIndex, CSFVar*& pVar)
{
  if (nIndex>=(int)m_SFVarArray.Avar.size())return false;
  pVar = (CSFVar*)m_SFVarArray.Avar[nIndex];
  nIndex++;
  return true;
}
//-------------------------------------------------------------------29.12.97
CSFVar *CSF::GetVarAndNumberForName(string as_Name, int &nIndex)     //as_Name can be Vector-name
{
  string ls_Name;
  int li_ind, i;
  CSFVar *pVar;

  i = as_Name.find('[');
  if (i < 1) {
    ls_Name = as_Name;
    li_ind = -1;
  }
  else  {
    ls_Name = as_Name.substr(0,i); // .Left(i);
    as_Name = as_Name.substr(0, as_Name.size()-1);      //.Left( as_Name.GetLength() - 1);
    as_Name = as_Name.substr(i,as_Name.size()-i-1);     //.Right(as_Name.GetLength() - i-1);
//#ifndef _LINUX
//    li_ind =  atoi(as_Name.c_str());                    //.GetBuffer(10));
//                                                        //as_Name.ReleaseBuffer();
//#else
    sscanf(as_Name.c_str(), "%u", &li_ind);
//#endif
  }
  for (i=0 ; i< (int)m_SFVarArray.Avar.size();i++)  {
    pVar = (CSFVar*)m_SFVarArray.Avar[i];
    if (pVar->m_Name != ls_Name ) continue;
    if (li_ind != -1)  {
      nIndex = pVar->m_NumInSF + li_ind -pVar->m_First;
    }
    else  nIndex = pVar->m_NumInSF;
    return pVar;
  }
  return NULL;
}

//-------------------------------------------------------------------29.05.2000 Kirienko
// То же, что и GetVarAndNumberForName(CString as_Name, int &nIndex) 
// только в качестве nIndex возвращает номер в m_SFVarArray, а
// не номер среди однотипных (входных или выходных) pVar->m_NumInSF 
CSFVar *CSF::GetVarAndIndexForName(string as_Name, int &nIndex)     //as_Name can be Vector-name
{
  string ls_Name;
  int li_ind, i;
  CSFVar *pVar;

  i = as_Name.find('[');
  if (i < 1) {
    ls_Name = as_Name;
    li_ind = -1;
  }
  else  {
    ls_Name = as_Name.substr(0,i); // .Left(i);
    as_Name = as_Name.substr(0, as_Name.size()-1);      //.Left( as_Name.GetLength() - 1);
    as_Name = as_Name.substr(i,as_Name.size()-i-1);     //.Right(as_Name.GetLength() - i-1);
//#ifndef _LINUX
//    li_ind =  atoi(as_Name.c_str());                    //.GetBuffer(10));
//    //as_Name.ReleaseBuffer();
//#else
    sscanf(as_Name.c_str(), "%u", &li_ind);
//#endif
  }
  for (i=0 ; i< (int)m_SFVarArray.Avar.size();i++)  {
    pVar = (CSFVar*)m_SFVarArray.Avar[i];
    if (pVar->m_Name != ls_Name ) continue;
    if (li_ind != -1)  {
      nIndex = pVar->m_NumInSF + li_ind -pVar->m_First;
    }
    else  nIndex = i;
    return pVar;
  }
  return NULL;
}


//-------------------------------------------------------------------
int CSF::AddVar(CSFVar* pVar)
{

  if(pVar->m_Typ == 'i')  {
    pVar->m_NumInSF = m_NumInVar;
    m_NumInVar += (pVar->m_Last - pVar->m_First + 1);
  }
  else if(pVar->m_Typ == 'o') {
    pVar->m_NumInSF = m_NumOutVar;
    m_NumOutVar += (pVar->m_Last - pVar->m_First + 1);
  }
  else if(pVar->m_Typ == 't') {
    pVar->m_NumInSF = m_NumInterVar;
    m_NumInterVar += (pVar->m_Last - pVar->m_First + 1);
  }
  return m_SFVarArray.Add(pVar);
}

//-------------------------------------------------------------------
bool CSF::GetFirstType(CSFType *& pType)
{
  string buf;
  m_TypeMap.itp = m_TypeMap.mtp.begin();
  if (m_TypeMap.itp== m_TypeMap.mtp.end())  return false;
  m_TypeMap.GetNextAssoc(buf, pType);
  return true;
}

//---------------------------------------------------------------
bool CSF::GetFirstTypeName(CSFType *& pType, string& buf)
{
  m_TypeMap.itp = m_TypeMap.mtp.begin();
  if (m_TypeMap.itp== m_TypeMap.mtp.end())  return false;
  m_TypeMap.GetNextAssoc(buf, pType);
  return true;
}

//-------------------------------------------------------------------
bool CSF::GetNextType(CSFType *& pType)
{
  string buf;
  if (m_TypeMap.itp== m_TypeMap.mtp.end())  return false;
  m_TypeMap.GetNextAssoc(buf, pType);
  return true;
}

//---------------------------------------------------------------
bool CSF::GetNextTypeName(CSFType *& pType, string& buf)
{
  if (m_TypeMap.itp== m_TypeMap.mtp.end())  return false;
  m_TypeMap.GetNextAssoc(buf, pType);
  return true;
}

//-------------------------------------------------------------------
void CSF::AddElemForType(CSFType *Type, string Elem)
{
  m_ElemTypeMap[Elem] = Type->GetName();
  Type->AddElem(Elem);
}

//-------------------------------------------------------------------
void CSF::AddType(CSFType *Type)
{
  m_TypeMap.mtp[Type->GetName()] = Type;
}

//-------------------------------------------------------------------
bool CSF::GetFirstPareNet(map<string,string>::iterator& pos,string& one, string& two)
{
  pos = m_NetMap.begin();
  if (pos==m_NetMap.end())  return false;
  one =  pos->first;   two = pos->second;   pos++; 
//  m_NetMap.GetNextAssoc(pos, one, two);
  return true;
}

//-------------------------------------------------------------------
bool CSF::GetNextPareNet(map<string,string>::iterator& pos, string& one, string& two)
{
  if (pos==m_NetMap.end()) return false;
  one =  pos->first;   two = pos->second;   pos++; 
//  m_NetMap.GetNextAssoc(pos, one, two);
  return true;
}

//-------------------------------------------------------------------
string CSF::GetPareNetForString(string one)
{ 
  map <string,string> :: iterator ii = m_NetMap.find(one); 
  if (ii==m_NetMap.end()) return "";
  return m_NetMap[one];
}

//-------------------------------------------------------------------
void CSF::AddPareNet(string one, string two)
{
  m_NetMap[one] = two;
}

//-------------------------------------------------------------------
bool CSF::GetFirstPareInter(map<string,string>::iterator &pos, string& one, string& two)
{
  pos = m_InterMap.begin();
  if (pos == m_InterMap.end())  return false;
  one =  pos->first;   two = pos->second;   pos++;   
  return true;
}

//-------------------------------------------------------------------
bool CSF::GetNextPareInter(map<string,string>::iterator &pos, string& one, string& two)
{
  if (pos == m_InterMap.end())  return false;
  one =  pos->first;   two = pos->second;   pos++;   
  return true;
}

//-------------------------------------------------------------------
string CSF::GetPareInterForString(string one)
{ 
  string buf;
  map <string,string> :: iterator ii = m_InterMap.find(one); 
  if (ii==m_InterMap.end()) return "";
  return m_InterMap[one];
}

//-------------------------------------------------------------------
void CSF::AddPareInter(string one, string two)
{
  m_InterMap[one] = two;
}

//-------------------------------------------------------------------
void CSF::SetFun(CSFunction *pFun)
{
  if (m_Fun)  delete m_Fun;
  m_Fun = pFun;
}

//-------------------------------------------------------------------
int CSF::AddConnect(CSFConnect *pConnect)
{
  return m_ConnectArray.Add(pConnect);
}

//------------------------------------------------------------04.05.2000 Кириенко
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 17.11.2005 Кириенко - правка 26.04.2011
bool CSF::DeleteConnect(int nIndex)
{
  CSFConnect * pCon;

//  ASSERT((nIndex>=0)&&(nIndex<m_ConnectArray.GetSize()));
  if ((pCon=GetConnect(nIndex))!=NULL){
    m_ConnectArray.RemoveAt(nIndex);
    return true;
  }
  else return false;
}

//-------------------------------------------------------------------
CSFConnect *CSF::GetConnect(int nIndex)
{
  if (nIndex < (int)m_ConnectArray.Acon.size())  return m_ConnectArray.Acon[nIndex];
  return NULL;
}

//-------------------------------------------------------------------
CSFConnect *CSF::GetConnectForType(string Type)
{
  CSFConnect *pConnect;

  for (int i=0; i< (int)m_ConnectArray.Acon.size(); i++)  {
    pConnect = (CSFConnect *)m_ConnectArray.Acon[i];
    if (pConnect->GetName() == Type)  return pConnect;
  }
  return NULL;
}

//-------------------------------------------------------------------26.04.11 Кириенко
// Вернуть для имени элемента указатель раздел CONNECT  и его индекс
int CSF::GetConnectForElem(string Name, CSFConnect *& pConnect)
{
  //CSFConnect *pConnect;

  for (int i=0; i< (int)m_ConnectArray.Acon.size(); i++)  {
    pConnect = (CSFConnect *)m_ConnectArray.Acon[i];
    if (pConnect->GetName() == Name)  return i;
  }
  return 0;
}

//-------------------------------------------------------------------31.03.00 Кириенко Н.А. 
bool CSF::GetTypeForElem(string name,CSFType *& pSFType)
{
  string TypeName, Buf;
  map <string,string> :: iterator ii = m_ElemTypeMap.find(name); 
  if (ii==m_ElemTypeMap.end()) {
  // #ifdef ENG
  //     Buf="Absent TYPE for ";
  // #else
  //     Buf="Нет TYPE для ";
  // #endif
  //     Buf+=name;
  // #ifdef _LINUX
  //     printf("%s\n",Buf.c_str());
  // #else
  //     AfxMessageBox(Buf.c_str());
  // #endif
    return false;
  }
  TypeName = m_ElemTypeMap[name];

  if (m_TypeMap.Lookup(TypeName,pSFType)==0){
  // #ifdef ENG
  //     Buf="Absent TYPE ";
  // #else
  //     Buf="Нет TYPE ";
  // #endif
  //     Buf+=TypeName;
  // #ifdef _LINUX
  //     printf("%s\n",Buf.c_str());
  // #else
  //     AfxMessageBox(Buf.c_str());
  // #endif
    return false;
  }
  return true;
}

//------------------------------------------------------------------- 7.10.05 Кириенко Н.А. 
bool CSF::IsTypeInSF(string TypeName)
{
  CSFType * pSFType;
  if (m_TypeMap.Lookup(TypeName, *&pSFType)==0)     return false;
  return true;
}


//-------------------------------------------------------------------
void CSF::Serialize(CArch& ar)
{
  BYTE FlagFun;
  if (ar.IsStoring())
  {
    ar << m_Title;
    ar << m_Project;
    ar << m_Author;
    ar << m_Date;
    ar << m_NumInVar;
    ar << m_NumOutVar;
    ar << m_NumInterVar;
    ar << m_TitleComments;
    ar << m_ProjectComments;
    ar << m_AuthorComments;
    ar << m_DateComments;
    ar << m_SFVarComments;
    ar << m_TypeComments;
    ar << m_ConnectComments;
    ar << m_NetComments;
    ar << m_FunComments;
    if(m_Fun==NULL) FlagFun=0;
    else            FlagFun=1;
    ar << FlagFun;
  }
  else
  {
    ar >> m_Title;
    ar >> m_Project;
    ar >> m_Author;
    ar >> m_Date;
    ar >> m_NumInVar;
    ar >> m_NumOutVar;
    ar >> m_NumInterVar;
    ar >> m_TitleComments;
    ar >> m_ProjectComments;
    ar >> m_AuthorComments;
    ar >> m_DateComments;
    ar >> m_SFVarComments;
    ar >> m_TypeComments;
    ar >> m_ConnectComments;
    ar >> m_NetComments;
    ar >> m_FunComments;
    ar >> FlagFun;
    if (FlagFun==1) m_Fun = new CSFunction;
  }
  m_SFVarArray.Serialize(ar);
  m_TypeMap.Serialize(ar);
  Seria(m_ElemTypeMap,ar);   //m_ElemTypeMap.Serialize(ar);
  m_ConnectArray.Serialize(ar);
  Seria(m_NetMap,ar);        //m_NetMap.Serialize(ar);
  Seria(m_InterMap,ar);        //m_InterMap.Serialize(ar);
  if (FlagFun) m_Fun->Serialize(ar);
  m_Attr.Serialize(ar);
}

//-------------------------------------------------------------------29.04.1998  Krasilnikova
int CSF::GetHierarSF(CMapStringToNode&    m_MapNode, int li_Hierar)
{
  CPNode    *pNode;
//  POSITION   pos;
  CSFType   *pType;
  int li_Hierar1, li_Hierar2;

  if (GetFirstType(pType))  {
    li_Hierar = li_Hierar2 = li_Hierar+1;
    do  {
      if (GetTitle()  == pType->GetName())  continue;
      if ( m_MapNode.Lookup(pType->GetName(),pNode)== false)  continue;
      li_Hierar1 = pNode->m_pSF->GetHierarSF(m_MapNode, li_Hierar2);
      if (li_Hierar1 >  li_Hierar) li_Hierar = li_Hierar1;
    }  while (GetNextType(pType));
  }
  return li_Hierar;
}

//-------------------------------------------------------------------24.04.98 Кириенко
bool CSF::GetStartPositionForType()  //POSITION &pos
{
  m_TypeMap.itp = m_TypeMap.mtp.begin();
  if (m_TypeMap.itp==m_TypeMap.mtp.end())  return false;
  return true;
}

//-------------------------------------------------------------------
string CSF::GetListInVar()
{
  string ls = "", ls_name;
  int  i = 0;

  while ((ls_name = GetNameInVar(i++)) != "")  {
    ls += ls_name;
    ls += " ";
  }

  return ls;
}

//-------------------------------------------------------------------
string CSF::GetListOutVar()
{
  string ls = "", ls_name;
  int  i = 0;

  while ((ls_name = GetNameOutVar(i++)) != "")  {
    ls += ls_name;
    ls += " ";
  }

  return ls;
}

//-------------------------------------------------------
void CSF::ChangeSubstr(string str1,string str2)    //Кириенко 19 февраля 2015 г.
	// Заменить подстроку str1 на подстроку str2  
	// в именах переменных в разделе CSFunction
{
	int i=0, j;
	string buf;

	while(m_Fun->GetTextLine(i, buf)){
		j=0;
		while ((j=buf.find_first_of(str1))>=0)
		{buf=buf.substr(0,j)+ buf.substr(j+str1.length());
		}
		m_Fun->ReplaceTextLine(i-1, buf);
//		i++;
	}
	return;
}

