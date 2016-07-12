///////////////////////////////////////////////////////////////////////////////
// Ver.1.1.0     05.04.2004   Russian <> English
// Ver.1.1.1     07.04.2004   форматирование
// Ver.1.1.2     19.05.2006 поправка SetPIN
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string>
#include <vector>
#include <utility>
#include <map>
#include <QtWidgets/QMessageBox>
using namespace std;
/*
#ifdef _LINUX
*/
#undef _DEBUG
typedef unsigned char BYTE; 
typedef unsigned long ULONG; 
//typedef int bool;

#define ASSERT assert
#include <assert.h>
/*
#else

#pragma warning(disable : 4996)
#include <afx.h>         // MFC core and standard components
#include <afxwin.h>         // MFC core and standard components
#ifndef FO_
#define FO_
#define FSTD(_x) friend _x AFXAPI 
#define STD(_x) _x AFXAPI 
#endif

#endif
*/
#include "sflog.h"

////////////////////////////////////////////////////////////////////////////
//class CCon : public CObject
//---------------------------------------------------------------
void CCon::Serialize(CArch& ar)
{  m_ConVarArray.Serialize(ar); } 

//////////////////////////////////////////////////////////////////
// class CConArray : public CObArray
//--------------------------------------------------------------- Constructor

void CConArray::Serialize(CArch& ar)
{
  string key, s1="CConArray";
  CCon* pCon;
  int Count,i;
  if (ar.IsStoring()) {
    Count = (int) arCon.size();
    ar<<Count<<s1;

    for (i=0; i<Count;i++)  {
      pCon = arCon[i];   
      pCon->Serialize(ar);
    }
  }
  else
  {
    ar>>Count>>key;
    if (key !=s1) { throw (-9); }

    for(i=0; i<Count; i++) {
      pCon = new CCon;
      pCon->Serialize(ar);
      arCon.push_back(pCon);
    }
  }
}

void CConArray::RemoveAll() 
{  
  for (int i=0; i< (int)arCon.size(); i++)
  {  CCon* pCon = arCon[i]; delete pCon;  }
  arCon.clear(); 
}
/////////////////////////////////////////////////////////////////////
//class CFunc : public CObject

//-------------------------------------------------------
CFunc::CFunc() {}
//-------------------------------------------------------
CFunc::CFunc(CFunc *pFunc)  
{
  CSFVar *pVar,*pVar1; 
  CCon *pCon, *pCon1;
  int i, j, size;
  m_OutVar.m_Name = pFunc->m_OutVar.m_Name;
  m_OutVar.m_Typ  = pFunc->m_OutVar.m_Typ;
  m_OutVar.m_First =pFunc->m_OutVar.m_First;
  m_OutVar.m_Last = pFunc->m_OutVar.m_Last;
  m_OutVar.m_Neg = pFunc->m_OutVar.m_Neg;
   
  for (i=0; i<(int)pFunc->m_InVarArray.Avar.size(); i++) {
    pVar = pFunc->m_InVarArray.Avar[i];
    pVar1 = new CSFVar(pVar);
    size =(int)m_InVarArray.Avar.size();
    if(m_InVarArray.Add(pVar1)<size) delete pVar1;
  }
  for (i=0; i<(int)pFunc->m_ConArray.arCon.size(); i++) {
    pCon=pFunc->m_ConArray.arCon[i];
    pCon1=new CCon();
    for (j=0; j<(int)pCon->m_ConVarArray.Avar.size(); j++) {
      pVar=pCon->m_ConVarArray.Avar[j];
      pVar1 = new CSFVar(pVar);
      size = (int)pCon1->m_ConVarArray.Avar.size(); 
      if (pCon1->m_ConVarArray.Add(pVar1)<size) delete pVar1;
    }
    m_ConArray.Add(pCon1);
  }     
  m_Right =pFunc->m_Right;      
}
//-------------------------------------------------------
CFunc::~CFunc() 
{                                    
  m_InVarArray.RemoveAll();
  m_ConArray.RemoveAll();
} 
//-------------------------------------------------------
bool CFunc::InitFunc(string buf)
{     
  int j,pos,pr,size;
  string  buf1, Buf;
  string bufname;
  string bufur;
  CSFVar *pVar,*pVar2;
  CCon *pCon;
           
  m_Right= buf;
  pos =0;
  pr = GetNextInpName(buf,pos, bufname);
  if (bufname.size()==0)  {
//#ifdef _LINUX
    printf("Input variables are absent\n");
//    QMessageBox *messageBox = new QMessageBox(QMessageBox::Warning, QString("blablabla"), QString("blablabla"), 0, this);
//    messageBox->show();
//#else
//#ifdef ENG
//    AfxMessageBox("Input variables are absent");
//#else
//    AfxMessageBox("Ошибка в записи входных переменных уравнения");
//#endif
//#endif
    return false;
  }  
  pCon = new CCon();

  if ((pr==6)||(pr==7)||(pr==8)) m_OutVar.m_Neg = "^";
  else m_OutVar.m_Neg = ""; 

  m_OutVar.m_Name = bufname;
  m_OutVar.m_Typ = 'o';
  m_OutVar.m_First = 0;
  m_OutVar.m_Last = 0;        
      
  if ((pos = buf.find('=')) < 0)  { 
/*
#ifdef _LINUX
*/
    printf("Syntax error. '=' is absent\n");
//#else
//#ifdef ENG
//    AfxMessageBox("Syntax error. '=' is absent");
//#else
//    AfxMessageBox("Ошибка в записи уравнения. Отсутствует =");
//#endif
//#endif
    delete pCon;
    return false;
  }  
  pos++;
  //bufur=buf.Right(buf.GetLength()-pos);  
  bufur=buf;
  bufur.erase(0,pos);
  j=0; pr=0;
                // read next Conuction;
NextInpName:      
  pr = GetNextInpName(bufur,j, bufname);
  if (bufname.size() ==0)   {
    m_ConArray.Add(pCon);
    goto Prov;
  }
      
  if ((bufname=="1")||(bufname=="0")) {
    m_ConArray.RemoveAll();
    delete pCon;
    goto Prov;
  }
  pVar= new CSFVar(bufname, 'i',0,0);
  pVar->m_Neg = ""; 
  size = (int)m_InVarArray.Avar.size();
  if(m_InVarArray.Add(pVar)<size) delete pVar;
      
  if ((pr==1)||(pr==4)||(pr==5)||(pr==9)) {
    // Скобочная форма, уравнение задается строкой m_right
    pr=6;
    // Освободить память от уже сформированных продукций
    m_ConArray.RemoveAll();
    delete pCon;
// Переписать входные переменные в списки входных
NextInpName1:        
    pr = GetNextInpName(bufur,j, bufname);
    if (bufname.size() ==0) {
      goto Prov;//К чтению следующего уравнения
    }
    pVar = new CSFVar(bufname, 'i',0,0);
    pVar->m_Neg = "";
        
    size = (int)m_InVarArray.Avar.size();
    if(m_InVarArray.Add(pVar)<size) delete pVar;
    goto NextInpName1;
  }  
  if ((pr==2)||(pr==0)||(pr==6)||(pr==7)) {                                 
     // Продолжить формирование продукции
    pVar2=new CSFVar(bufname, 'i',0,0);    
    if ((pr==6)||(pr==7)) pVar2->m_Neg = "^";
    else pVar2->m_Neg = ""; 
                             
    size = (int)pCon->m_ConVarArray.Avar.size();
    if (pCon->m_ConVarArray.Add(pVar2)<size) delete pVar2;
      goto NextInpName;
  }
  if ((pr==3)||(pr==8)) {
    // следует формировать новую конъюнкцию
    m_ConArray.Add(pCon);
    pCon = new CCon();   
    pVar2=new CSFVar(bufname, 'i',0,0);

    if (pr==8) pVar2->m_Neg = "^";
    else pVar2->m_Neg = ""; 

    size = (int)pCon->m_ConVarArray.Avar.size();
    if (pCon->m_ConVarArray.Add(pVar2)<size)delete pVar2;
    pr=0;
    goto NextInpName;
  }
Prov:
  return true;     
}

//-------------------------------------------------------
bool CFunc::ReplaceName(CSFVar *pVar, CSFVar *pVar1, int k)
{
// Заменить имя в символьной записи уравнения (в левой и правой частях)
  int last,num; 
  int l;
  string bufname;
  string m_Right1;
  l=0;
  last=0;
  m_Right1=m_Right;
  m_Right.clear();
  GetNextInpName(m_Right1, l,bufname);
  if (bufname.size()==0)  {
//#ifdef _LINUX
    printf("Input variables are absent\n");
//#else
//#ifdef ENG
//    AfxMessageBox("Input variables are absent");
//#else
//    AfxMessageBox("Ошибка в записи входных переменных уравнения");
//#endif
//#endif
    return false;
  }  
  if (bufname==pVar->m_Name) {
    m_Right=pVar1->m_Name;
  }
  else  m_Right=bufname;      
  if ((l= m_Right1.find('=')) < 0)  {
//#ifdef _LINUX
    printf("Syntax error. '=' is absent\n");
//#else
//#ifdef ENG
//    AfxMessageBox("Syntax error. '=' is absent");
//#else
//    AfxMessageBox("Ошибка в записи уравнения. Отсутствует =");
//#endif
//#endif
    return false;
  }  
  m_Right+="=";
  l++;
  last=l;
  while (1) { 
    GetNextInpName(m_Right1, l,bufname);
    if ((num=bufname.size())==0) break;
    if (bufname==pVar->m_Name) {
      m_Right+=m_Right1.substr(last,l-num-last);  //m_Right1.Mid(last,l-num-last);
      if(k==3){
        m_Right+='^';
      } 
      m_Right+=pVar1->m_Name;
      last=l;
    } 
  }
//  m_Right+=m_Right1.Right(m_Right1.GetLength()-last);
  m_Right1.erase(0,last);
  m_Right+=m_Right1;

// Просмотреть m_Right и ликвидировать ситуацию ^^   
    
  while ((l=m_Right.find("^^"))>=0) {
    m_Right[l++]=' ';
    m_Right[l++]=' ';
                        //m_Right.SetAt(l++,' ');
  }
  return true;
}    

//---------------------------------------------------------------
bool CFunc::Set_m_Right()
{
  int k,l;
  CCon *pCon;
  CSFVar *pVar;

  m_Right = m_OutVar.m_Neg + m_OutVar.m_Name + "=";

  for (k=0; k<(int)m_ConArray.arCon.size(); k++){
    pCon= m_ConArray.GetAt(k);
    for (l=0; l<(int)pCon->m_ConVarArray.Avar.size(); l++){
      pVar=pCon->m_ConVarArray.GetAt(l);
      m_Right=m_Right+pVar->m_Neg+pVar->m_Name +"*";
    }
    //m_Right=m_Right.Left(m_Right.GetLength()-1) ; //убрать "*"
    //m_Right+=m_Right+"+";

    m_Right[m_Right.size() - 1]='+';

  }
  //m_Right=m_Right.Left(m_Right.GetLength()-1) ; //убрать "+"
  //m_Right=m_Right+";";
  m_Right[m_Right.size() - 1]=';';

  return true;
}

//---------------------------------------------------------------
bool CFunc::Open_Brace()
{
  CSF *pSFNew;
  CSFVar *pVar, *pVar1;
  CSFunction *pFun;
  string Buf;
  CSOP *pSOP;
  CSOP SOP1; 
  int i;  
  
  pSFNew = new CSF;    
// Построить заголовочную часть описания pSFLogNew   
  pSFNew->SetTitle("~~~temp");  
  pSFNew->SetProject("pr");
  pSFNew->SetAuthor("K");
  pSFNew->SetDate("1.1.2001");
  for(i=0;i<(int)m_InVarArray.Avar.size();i++)  {
    pVar=m_InVarArray.Avar[i];
    pVar1 = new CSFVar(pVar);
    pSFNew->AddVar(pVar1);
  }
  pVar1 = new CSFVar(&m_OutVar);
  pSFNew->AddVar(pVar1);
  pFun= new CSFunction("LOG",(int)m_InVarArray.Avar.size(), 1,0);
  pFun->AddTextLine(m_Right);
  pSFNew->SetFun(pFun);   
  if (pSFNew->LogToSdfWithoutElimination()==false)
  {
//#ifdef _LINUX
    printf("Error in the process 'Open brace'\n");
//#else
//#ifdef ENG
//    AfxMessageBox("Error in the process 'Open brace'");
//#else
//    AfxMessageBox("Ошибка при переходе LogToSdfWithoutElimination.\n"
//      "Раскрытие скобок в уравнении.");
//#endif
//#endif
    m_InVarArray.RemoveAll();
    delete pSFNew;
    return false;
  }
  pFun=pSFNew->GetFun();
  if (!pFun->GetSdf(pSOP)) {
//#ifdef _LINUX
    printf("Error at the reading of part FUNCTION in the process 'Open brace'\n");
//#else
//#ifdef ENG
//    AfxMessageBox("Error at the reading of part FUNCTION in the process 'Open brace'");
//#else
//    AfxMessageBox("Ошибка чтения раздела FUNCTION.\nРаскрытие скобок в уравнении.");
//#endif
//#endif
    delete pSOP;
    m_InVarArray.RemoveAll();
    delete pSFNew;
    return false;
  }
  pSOP->SetnInterVar(pSFNew->GetNumInterVar());
  SOP1 = pSOP->SeqSdnf_F();
  if (!pFun->SetSdf(&SOP1)) {
//#ifdef _LINUX
    printf("Error at the writing of part FUNCTION in the process 'Open brace'\n");
//#else
//#ifdef ENG
//    AfxMessageBox("Error at the writing of part FUNCTION in the process 'Open brace'");
//#else
//    AfxMessageBox("Ошибка записи раздела FUNCTION.\nРаскрытие скобок в уравнении.");
//#endif
//#endif
    delete pSOP;
    m_InVarArray.RemoveAll();
    delete pSFNew;
    return false;
  }
  if (pSFNew->SdfToLog()==false)
  {
//#ifdef _LINUX
    printf("Error in the process 'Open brace'\n");
//#else
//#ifdef ENG
//    AfxMessageBox("Error in the process 'Open brace'");
//#else
//    AfxMessageBox("Ошибка при переходе SdfToLog().\n"
//                  "Раскрытие скобок в уравнении.");
//#endif
//#endif
    delete pSOP;
    m_InVarArray.RemoveAll();
    delete pSFNew;
    return false;
  }
  pFun=pSFNew->GetFun();
  if (pFun->GetTextLine(i=0,m_Right)==false) {
//#ifdef _LINUX
    printf("Error at the getting a row in the process 'Open brace'\n");
//#else
//#ifdef ENG
//    AfxMessageBox("Error at the getting a row in the process 'Open brace'");
//#else
//    AfxMessageBox("Ошибка при получении строки из блока FUNCTION.\n"
//      "Раскрытие скобок в уравнении.");
//#endif
//#endif
    delete pSOP;
    m_InVarArray.RemoveAll();
    delete pSFNew;
    return false;
  }
  delete pSOP;
  m_InVarArray.RemoveAll();
  delete pSFNew;
  return true;  
}

//---------------------------------------------------------------
void CFunc::Serialize(CArch& ar)
{
  if (ar.IsStoring())
  {
    ar <<  m_Right;
  }
  else
  {
    ar >>  m_Right;
  }
  m_OutVar.Serialize(ar);
  m_InVarArray.Serialize(ar);
  m_ConArray.Serialize(ar);
}

//---------------------------------------------------------------
void CFunc::print_CFunc(string FileName)
{ string bufer, endl;
  CCon * pCon;
  CSFVar * pVar;
  char b[10];
  int k,l,i,j;
  endl = "\n";
  
  FILE* f;
  f = fopen( FileName.c_str(), "w");
  bufer = "CFunc: ";
  bufer += m_OutVar.m_Name;
  bufer += endl;       
#ifdef ENG
  bufer += "Input variables: ";
#else
  bufer += "Входные переменные: ";  
#endif
  for (j=0; j<(int)m_InVarArray.Avar.size(); j++) {  
    pVar = m_InVarArray.GetAt(j);        
    bufer += pVar->m_Name + " ";
  }   
  bufer += endl;        
#ifdef ENG
  bufer += "Productions: ";
#else
  bufer += "Продукции: ";
#endif
  if ((l=m_ConArray.arCon.size())==0) goto kon;
  for (i=0; i<l; i++) {
    pCon = m_ConArray.arCon[i];
#ifndef _MSVC9
//    itoa (i,b,10);
    sprintf(b, "%u", i);
#else
    _itoa_s (i,b,10,10);
#endif

    bufer +=endl;
    bufer += b;
    bufer += endl;
    k = pCon->m_ConVarArray.Avar.size();
    for (j=0; j<k; j++) {  
      pVar = pCon->m_ConVarArray.Avar[j];        
      bufer += pVar->m_Name + " ";
    }
  }
kon:
  bufer += endl;
  bufer += m_Right;
  fwrite(bufer.c_str(),bufer.size(),1,f);   //f.Write(bufer,bufer.GetLength()); 
}

//---------------------------------------------------------------
string CFunc::ReplaceAllNames(map <string, string>* ShortNames)
{
  string bufur;  //Формирование нового уравнения
  int last,num; 
  int l; // начало поиска
  string bufname, newname, w;
  map <string, string>::iterator iMss;	

  l=0;
  last=0;
//Определить имя выходной переменной
  if (GetNextInpName(m_Right, l,bufname)== -1)
	  {
//#ifdef _LINUX
      printf("Output variables are absent\n");
//#else
//#ifdef ENG
//		  AfxMessageBox("Output variables are absent");
//#else
//		  AfxMessageBox("Ошибка в записи выходных переменных уравнения");
//#endif
//#endif
		  return "";
	  }  

// if (ShortNames->Lookup( bufname, newname )==0) //май, 27, 2010 Кириенко
//   newname=bufname;                             //май, 27, 2010 Кириенко
  iMss = ShortNames->find(bufname);
  if (iMss == ShortNames->end()) 
    newname=bufname; 
  else newname= iMss->second;
	
	bufur+=newname;  

  if ((l= m_Right.find('=')) < 0)  {  
//#ifdef _LINUX
    printf("Syntax error. '=' is absent\n");
//#else
//#ifdef ENG
//		AfxMessageBox("Syntax error. '=' is absent");
//#else
//		AfxMessageBox("Ошибка в записи уравнения. Отсутствует =");
//#endif
//#endif
		return "";
	}  
  bufur+="=";
  l++;
  last=l;
  while(GetNextInpName(m_Right, l,bufname)!= -1)
  {
//ShortNames->Lookup( bufname, newname );//май, 27, 2010 Кириенко
// Редактирование  май, 27, 2010 Кириенко
// if (ShortNames->Lookup( bufname, newname )==0) //май, 27, 2010 Кириенко
//   newname=bufname;                             //май, 27, 2010 Кириенко

    iMss = ShortNames->find(bufname);
    if (iMss == ShortNames->end()) 
         newname=bufname;
    else newname= iMss->second;
//переписать в bufur все, что до l-символа
    num=bufname.size();
//bufur+=m_Right.Mid(last,l-num-last);
    bufur+=m_Right.substr(last,l-num-last);
//переписать новое имя
    bufur+=newname;
    last=l;
  }
  bufur+=";";
  return bufur;
}


//////////////////////////////////////////////////////////////////
// class CFuncArray : public CObArray
//--------------------------------------------------------------- 
void CFuncArray::RemoveAll()
{ 
  for (int i=0; i< (int) arFun.size(); i++) {
    delete (arFun[i]);
  }
  arFun.clear();
}

//--------------------------------------------------------------- 
void CFuncArray::RemoveAt(int i)
{ 
  delete arFun[i];
  arFun.erase(arFun.begin()+i);
}


//--------------------------------------------------
void CFuncArray::Serialize(CArch& ar)
{
  string key, s1="CFuncArray";
  CFunc* pFun;
  int Count,i;
  if (ar.IsStoring()) {
    Count = (int) arFun.size();
    ar<<Count<<s1;

    for (i=0; i<Count;i++)  {
      pFun = arFun[i];   
      pFun->Serialize(ar);
    }
  }
  else
  {
    ar>>Count>>key;
    if (key !=s1) { throw (-9); }

    for(i=0; i<Count; i++) {
      pFun = new CFunc;
      pFun->Serialize(ar);
      arFun.push_back(pFun);
    }
  }
}

////////////////////////////////////////////////////////////////////////////
//class CFuncSystem : public CObject

//-------------------------------------------------------
CFuncSystem::CFuncSystem(string Typ, int NumInt, int NumOut, int NumInter) 
{
  ASSERT(Typ == "LOG" );
  m_Typ = Typ;
  ASSERT(NumInt>0);
  m_NumInp = NumInt;
  ASSERT(NumOut>0);
  m_NumOut = NumOut; 
  ASSERT(NumOut>=0);
  m_NumInter = NumInter;
}
    
//-------------------------------------------------------
CFuncSystem::CFuncSystem(CFuncSystem *pFuncSystem)  
{
  CSFVar *pVar,*pVar1;
  CFunc *pFunc, *pFunc1;
  int i, size; 
  m_Typ=pFuncSystem->m_Typ;
  m_NumInp=pFuncSystem->m_NumInp;
  m_NumOut=pFuncSystem->m_NumOut;
  m_NumInter=pFuncSystem->m_NumInter;
 
  for (i=0; i< (int)pFuncSystem->m_InVarArray.Avar.size(); i++) {
    pVar=pFuncSystem->m_InVarArray.Avar[i];
    pVar1 = new CSFVar(pVar);
    size =(int)m_InVarArray.Avar.size();
    if(m_InVarArray.Add(pVar1)<size) delete pVar1;
  }
  for (i=0; i<(int) pFuncSystem->m_FuncArray.arFun.size(); i++) {
    pFunc=pFuncSystem->m_FuncArray.arFun[i];
    pFunc1 = new CFunc(pFunc);
    AddFunc(pFunc1);
  }      
}

//-------------------------------------------------------
CFuncSystem::~CFuncSystem() 
{
  m_InVarArray.RemoveAll();
  m_FuncArray.RemoveAll();  
} 

//------------------------------------------------------- 
void CFuncSystem::Clean()
{ 
  m_InVarArray.RemoveAll();
  m_FuncArray.RemoveAll();  
}  

//-------------------------------------------------------  
CFunc * CFuncSystem::GetFuncWithMaxVar(int &k) 
{ 
  int i;
  CFunc * pFunc;
// #ifdef _DEBUG
//   AssertValid();
// #endif     
// 
  k=0;
  pFunc=m_FuncArray.arFun[0];
  int rab2=pFunc->m_InVarArray.Avar.size();
  for (i=1; i< (int) m_FuncArray.arFun.size(); i++) { 
    pFunc=m_FuncArray.arFun[i];
    if ((int)pFunc->m_InVarArray.Avar.size()>rab2) {
      rab2=pFunc->m_InVarArray.Avar.size();
      k=i;
    }
  }
  pFunc=m_FuncArray.arFun[k];
  return (pFunc);
}

//-------------------------------------------------------
int CFuncSystem::GetVarArrayFuncSystem(CSFVarArray &InpArray,
                                       CSFVarArray &OutArray,
                                       CSFVarArray &InterArray) 
{return 1;}
//-------------------------------------------------------
int CFuncSystem::GetNumInpVarInNewFuncSystem(CFunc * pFunc)
  // Опр. число входных и промежуточных переменных уравнения и            
  // формируемой системы.                    
{     
// #ifdef _DEBUG
//   AssertValid(); pFunc->AssertValid();
// #endif
  int nRes;
  int rab1 = m_InVarArray.Avar.size();
  for (int i=0; i< (int)pFunc->m_InVarArray.Avar.size(); i++) {
    if((nRes=m_InVarArray.IsVarInArray(pFunc->m_InVarArray.Avar[i])==(-1)))  ///????????? Надо разбираться
      rab1++; 
  }
  return rab1;
} 
//-------------------------------------------------------
int CFuncSystem::AddFunc (CFunc * pFunc)
// Добавить уравнение pFunc в CFuncSystem
{
  CSFVar * pVar1, *pVar2;
  int size;
// #ifdef _DEBUG
//   AssertValid(); pFunc->AssertValid();
// #endif
  for (int i=0; i<(int)pFunc->m_InVarArray.Avar.size(); i++) {
    pVar1=pFunc->m_InVarArray.Avar[i];
    pVar2= new CSFVar(pVar1);
    size=m_InVarArray.Avar.size();
    if (m_InVarArray.Add(pVar2)<size)delete pVar2;
  }
  return (m_FuncArray.Add(pFunc));
}
//-------------------------------------------------------
bool CFuncSystem::DeleteFunc(int i)
// Удалить уравнение с номером i из системы   
{ 
  CSFVar * pVar;
  CFunc *pFunc,*pFunc1;
  int k, j;
// #ifdef _DEBUG
//   m_FuncArray.AssertValid();
//   m_InVarArray.AssertValid();
// #endif  
//  ASSERT(i>=0);
  pFunc=m_FuncArray.arFun[i];

  for (j=0; j<(int)m_InVarArray.Avar.size();j++) {
    pVar=m_InVarArray.Avar[j];
    if (pFunc->m_InVarArray.IsNameInArray(pVar)>=0) {
// Надо посмотреть есть ли эта переменная в других уравнениях?
      for (k=0; k< (int) m_FuncArray.arFun.size();k++) {
        if (k==i) continue;
        pFunc1=m_FuncArray.arFun[k];    
        if (pFunc1->m_InVarArray.IsNameInArray(pVar)>=0) goto Yes;
      }
      m_InVarArray.RemoveAt(j);j--;
    }
Yes: continue;
  }
  m_FuncArray.RemoveAt(i);
  return true;
}
//-------------------------------------------------------
bool CFuncSystem::DelFunc (CFunc * pFunc)
// Удалить функцию pFunc из CFuncSystem
{       
  int i, j, k, m;
  CFunc * pFunc1, *pFunc2;
  CSFVar *pVar;

// #ifdef _DEBUG
//   AssertValid();  pFunc->AssertValid();
// #endif  
  for (i=0; i<(int) m_FuncArray.arFun.size(); i++) { 
    pFunc1=m_FuncArray.arFun[i];
    if (pFunc1->m_OutVar.Compare(&pFunc->m_OutVar)==false) continue ;
    for (j=0; j<(int)pFunc1->m_InVarArray.Avar.size(); j++) {
      if (pFunc1->m_InVarArray.Avar[j]->Compare(pFunc->m_InVarArray.Avar[j]) == false)
        goto next; 
    }
    for (j=0; j<(int)pFunc1->m_ConArray.arCon.size(); j++) {
      for (k=0; k<(int)((pFunc1->m_ConArray.arCon[j])->m_ConVarArray.Avar.size()); k++) {
    
        if (pFunc1->m_ConArray.arCon[j]->m_ConVarArray.Avar[k]->Compare
            (pFunc->m_ConArray.arCon[j]->m_ConVarArray.Avar[k])==false) 
          goto next ;
      }  
    }
    if (pFunc1->m_Right!=pFunc->m_Right) { 
next:   
      continue ;                            
    }
    for (k=0; k<(int)m_InVarArray.Avar.size(); k++)  {
      pVar=m_InVarArray.Avar[k];
      if (pFunc1->m_InVarArray.IsNameInArray(pVar)>=0) {  
// Надо посмотреть есть ли эта переменная в других уравнениях?
        for (m=0; m<(int)m_FuncArray.arFun.size();m++) {
          if (m==i) continue;
          pFunc2=m_FuncArray.arFun[m];    
          if (pFunc2->m_InVarArray.IsNameInArray(pVar)>=0)goto Yes;
        }
        m_InVarArray.RemoveAt(k);k--; 
      }
Yes: continue;   
    } 
    m_FuncArray.RemoveAt(i);
    return true; 
  }  
  return false ;
}                                                       

//---------------------------------------------------------------
bool CFuncSystem::Replace(CSFVar *pVar,CSFVar *pVar1,int k)
{       
// Заменить имена переменных в системе уравнений в соответствии
// с удалением уравнения A=B или A=^B. В pVar - А, в pVar1 - В 

  int i, j, l;
  CFunc * pFunc;
  CSFVar *pVar2;
  CCon *pCon;
// #ifdef _DEBUG
//   AssertValid();
//   pVar->AssertValid();
//   pVar1->AssertValid();
// #endif   

  if (k==2){
// Для ситуации 2 найти уравнение с выходной переменной
// pVar1  и заменить ее на pVar в m_OutVar и в m_Right  
    for (i=0; i<(int)m_FuncArray.arFun.size();i++) {
      pFunc=m_FuncArray.arFun[i];
      if (pFunc->m_OutVar.m_Name==pVar1->m_Name){
        pFunc->m_OutVar=pVar;
// далее заменить pVar1 на pVar в m_Right
        pFunc->ReplaceName(pVar1, pVar,0);      
      }
    }
  }
  if (k==4){
// Для ситуации 4 найти уравнение с выходной переменной
// pVar  и заменить ее на pVar1 в m_OutVar и в m_Right  
    for (i=0; i<(int)m_FuncArray.arFun.size();i++) {
      pFunc=m_FuncArray.arFun[i];
      if (pFunc->m_OutVar.m_Name==pVar->m_Name){
        pFunc->m_OutVar=pVar1;
// далее заменить pVar на pVar1 в m_Right
        pFunc->ReplaceName(pVar, pVar1,0);    
      }
    }
  }  
  
  if (k==1||k==3||k==4) {
// Для ситуации 1 в каждом уравнении с входными переменными
// pVar заменить их на pVar1 в m_InVarArray, pFunc->m_InVarArray,
// pFunc->m_ConArray, и в m_Right 
// Для ситуации 3 заменять на переменную с отрицанием а затем
// обработать на предмет двойного отрицания
    if ((i=m_InVarArray.IsNameInArray(pVar))>=0){  
      if (m_InVarArray.IsNameInArray(pVar1)==-1){
        m_InVarArray.Avar[i]->m_Name=pVar1->m_Name;
        m_InVarArray.Avar[i]->m_Typ=pVar1->m_Typ;
      }
      else {
        m_InVarArray.RemoveAt(i);
      } 
    }

    for (i=0; i<(int)m_FuncArray.arFun.size();i++) {
      pFunc=m_FuncArray.arFun[i];
      for (j=0; j<(int)pFunc->m_InVarArray.Avar.size();j++) {
        pVar2=pFunc->m_InVarArray.GetAt(j);
        if (pVar2->m_Name==pVar->m_Name) {
          if (pFunc->m_InVarArray.IsNameInArray(pVar1)==-1){
            pVar2->m_Name=pVar1->m_Name;
            pVar2->m_Typ=pVar1->m_Typ;
            break;
          }
          else {
            pFunc->m_InVarArray.RemoveAt(j);
            break;
          }
        }
      }
      for (j=0; j<(int)pFunc->m_ConArray.arCon.size();j++) {
        pCon=pFunc->m_ConArray.GetAt(j);
        for (l=0; l<(int)pCon->m_ConVarArray.Avar.size();l++) {
          pVar2=pCon->m_ConVarArray.GetAt(l);
          if (pVar2->m_Name==pVar->m_Name) {
            pVar2->m_Name=pVar1->m_Name;
            pVar2->m_Typ=pVar1->m_Typ;
          }
        }
      }
      pFunc->ReplaceName(pVar, pVar1,k);     
    }
  }  
  return true;
}  

//---------------------------------------------------------------
void CFuncSystem::Serialize(CArch& ar)
{
  if (ar.IsStoring())
  {
    ar <<  m_Typ;
    ar <<  m_NumInp;
    ar <<  m_NumOut;
    ar <<  m_NumInter;
  }
  else
  { 
    ar >>  m_Typ;
    ar >> m_NumInp;
    ar >> m_NumOut;
    ar >> m_NumInter;  
  }
  m_InVarArray.Serialize(ar);
  m_FuncArray.Serialize(ar);
}

//---------------------------------------------------------------
void CFuncSystem::print_CFuncSystem(string FileName)
{ string bufer, endl;
  CCon * pCon;
  CSFVar * pVar;
  CFunc * pFunc;
  char b[100];
  int k,l,j,m,r;
  endl = "\n";
  
// #ifdef _DEBUG
//   AssertValid();
// #endif     
//   ASSERT(FileName!="");
  //CFile f( FileName, CFile::modeCreate | CFile::modeWrite );
  FILE *f;
  f = fopen( FileName.c_str(),"w");
  bufer = "CFuncSystem: ";
#ifndef _MSVC9
    sprintf(b,"%u  %u  %u",m_NumInp, m_NumOut, m_NumInter);
#else
    sprintf_s(b, 100, "%u  %u  %u",m_NumInp, m_NumOut, m_NumInter);
#endif
/*
  _itoa_s (m_NumInp,b,10,10);
  bufer +=" ";
  bufer += b;
  _itoa_s (m_NumOut,b,10,10);
  bufer +=" ";
  bufer += b;
  _itoa_s (m_NumInter,b,10,10);
  bufer +=" ";
*/
  bufer += b;
  bufer += " " + endl;
#ifdef ENG
  bufer += "Input variables: " + endl;
#else
  bufer += "Входные переменные: " + endl;
#endif
  l=(int)m_InVarArray.Avar.size();
  j=0;
  for (int i=0; i<l; i++) {
    pVar = m_InVarArray.Avar[i];
    bufer += pVar->m_Name + " ";
    j++;
    if (j>7){
      j=0; bufer += endl;        
    }
  }
#ifdef ENG
  bufer += endl + "Equations: ";
#else
  bufer += endl + "Уравнения: ";
#endif
  m=(int)m_FuncArray.arFun.size();
  for (j=0; j<m; j++) {
    pFunc= m_FuncArray.GetAt(j);
            
    bufer += endl + "CFunc: ";
    bufer += pFunc->m_OutVar.m_Name;
    bufer += endl;         
#ifdef ENG
    bufer += "Productions: ";
#else
    bufer += "Продукции: ";
#endif
    l=pFunc->m_ConArray.arCon.size();
    for (int i=0; i<l; i++) {
      pCon = pFunc->m_ConArray.GetAt(i);
#ifndef _MSVC9
//      itoa (i,b,10);
      sprintf(b, "%u", i);
#else
      _itoa_s (i,b,10,10);
#endif
      bufer +=endl;
      bufer += b;
      bufer += endl;
      k=pCon->m_ConVarArray.Avar.size();
      for (r=0; r<k; r++) {  
        pVar = pCon->m_ConVarArray.GetAt(r);        
        bufer += pVar->m_Name + " ";
      }
    }
  }
  fwrite(bufer.c_str(),bufer.size(),1,f); 
  fclose(f);
}                     


#ifdef _DEBUG
// -------------------------------------------------------
// void CFuncSystem::AssertValid() const
// {   
//   m_InVarArray.AssertValid();
//   m_FuncArray.AssertValid();
// }
// -------------------------------------------------------
// void CFuncSystem::Dump(CDumpContext& dc) const
// {  
//  dc<<"Список входных переменных ";
//   dc<<"List of inputs: ";  
//   dc <<"\r\n";
//   m_InVarArray.Dump(dc);
//   dc <<"\r\n";
//  dc<<"Список уравнений: ";
//   dc<<"List of equations: ";
//   dc <<"\r\n";
//   m_FuncArray.Dump(dc);
//   dc<<"\r\n";                      
// }
#endif     

////////////////////////////////////////////////////////////////////////////
//class CSFLog : public CSF

//IMPLEMENT_SERIAL(CSFLog, CSF, 1)

//-------------------------------------------------------
CSFLog::CSFLog() {}
//-------------------------------------------------------
CSFLog::~CSFLog() {}  

//---------------------------------------------------------------
CFuncSystem *CSFLog::GetFuncSystem()
{
  CFuncSystem * pFuncSystem;
// #ifdef _DEBUG
//   AssertValid();
// #endif     
  pFuncSystem= & m_FuncSystem;
  return pFuncSystem;
}  
//---------------------------------------------------------------
void CSFLog::SetFuncSystem(CFuncSystem *pFuncSystem)
{ 
  CSFVar *pVar,*pVar1;
  CFunc *pFunc, *pFunc1;
  int i, size; 
// #ifdef _DEBUG
//   AssertValid();
//   pFuncSystem->AssertValid();
// #endif      

  m_FuncSystem.m_Typ=pFuncSystem->m_Typ;
  m_FuncSystem.m_NumInp=pFuncSystem->m_NumInp;
  m_FuncSystem.m_NumOut=pFuncSystem->m_NumOut;
  m_FuncSystem.m_NumInter=pFuncSystem->m_NumInter;
 
  for (i=0; i<(int)pFuncSystem->m_InVarArray.Avar.size(); i++) {
    pVar=pFuncSystem->m_InVarArray.Avar[i];
    pVar1 = new CSFVar(pVar);
    size =m_FuncSystem.m_InVarArray.Avar.size();
    if(m_FuncSystem.m_InVarArray.Add(pVar1)<size)delete pVar1;
  }
  for (i=0; i< (int)pFuncSystem->m_FuncArray.arFun.size(); i++) {
    pFunc=pFuncSystem->m_FuncArray.arFun[i];
    pFunc1 = new CFunc(pFunc);
    m_FuncSystem.AddFunc(pFunc1);
  }      
}

//-------------------------------------------------------
bool  CSFLog::CFuncSystemToNewFunction() 
// Получить из CFuncSystem CFunction
{                            
  CSFunction *pFun;
// #ifdef _DEBUG
//   AssertValid();
// #endif     
  pFun= new CSFunction("LOG",m_FuncSystem.m_NumInp,
            m_FuncSystem.m_NumOut,m_FuncSystem.m_NumInter);
  for (int i=0; i<(int)m_FuncSystem.m_FuncArray.arFun.size();i++) {
    pFun->AddTextLine(m_FuncSystem.m_FuncArray.arFun[i]->m_Right);
  }
  if (m_Fun != NULL) delete m_Fun;
  m_Fun=pFun;   
  return true;
}

//---------------------------------------------------------------
bool CSFLog::ReplaceConnectFunc()
// Подставить в исходную систему все уравнения связей
// Для каждого уравнения связи 1) Удалить его из системы 
// 2) Для оставшейся подсистемы произвести переименование 
{
  CFunc *pFunc, *pFunc1;
  CCon *pCon;    
  CSFVar * pVar, * pVar1;
//  int rab2, rab3;
  int i,j,k; 
// #ifdef _DEBUG
//   AssertValid();
// #endif     

  for (i=0; i<(int)m_FuncSystem.m_FuncArray.arFun.size(); i++) {
    pFunc=m_FuncSystem.m_FuncArray.arFun[i];
//    if ((int)m_FuncSystem.m_FuncArray.arFun.size()==i+2){
//      rab2=0;
//    }
    if (pFunc->m_ConArray.arCon.size()==1){
      pCon=pFunc->m_ConArray.GetAt(0);
      if (pCon->m_ConVarArray.Avar.size()==1) {
// Это уравнение связи
//        rab2=0;
//        rab3=0;
        pVar=&pFunc->m_OutVar;  // Левая переменная             
        pVar1=pCon->m_ConVarArray.GetAt(0); // Правая переменная
        if (m_FuncSystem.m_InVarArray.IsNameInArray(pVar)>=0){
// Левая переменная является промежуточной             
//Ситуация 1 - вместо левой переменной везде подставляется
//правая (j=1), если надо, то с отрицанием (j=3)              
          j=1;
          if ((int)pFunc->m_Right.find('^')>=0) j=3; 
          m_FuncSystem.Replace(pVar,pVar1,j);
          m_FuncSystem.DeleteFunc(i);
          i--;
          SetPIN();
        }                
        else {  
// Левая переменная не является промежуточной                 
//          rab2=0;
//          rab3=0;
          if (m_FuncSystem.m_InVarArray.IsNameInArray(pVar1)>=0){
// Правая переменная является промежуточной
            if ((int)pFunc->m_Right.find('^')>=0) continue;
// Если есть отрицание правой переменной, то подстановку не осуществлять
// Если нет отрицания правой переменной, то Ситуация 2 -
// (j=2) подстановить в pFunc->m_OutVar.m_Name вместо 
// правой переменной левую, если правая больше нигде не встречается
// в правых частях системы
// Чтобы проверить, встречается ли в правых частях pVar1, надо 
// просмотреть все уравнения, кроме i-того и посмотреть pVar1  
// в  m_InVarArray
            j=2;
            for (k=0; k<(int)m_FuncSystem.m_FuncArray.arFun.size(); k++) {
              if (i==k) continue;
              pFunc1=m_FuncSystem.m_FuncArray.GetAt(k);
              if (pFunc1->m_InVarArray.IsNameInArray(pVar1)>=0) {
                j=5; break;   
              }
            }
            if (j==2) {  
              m_FuncSystem.Replace(pVar,pVar1,j);    
              m_FuncSystem.DeleteFunc(i);
              i--;
              SetPIN();     
            }   
          }                 
        } 
        CFuncSystemToNewFunction();
      }
    }
    continue;
  }     
  return true;
}                    
  

//-------------------------------------------------------

#ifdef _DEBUG
//-------------------------------------------------------
// void CSFLog::AssertValid() const
// {   CSF::AssertValid();
//     m_FuncSystem.AssertValid();
// }
// //-------------------------------------------------------
// void CSFLog::Dump(CDumpContext& dc) const
// {  
// CSF::Dump(dc);                      
// }
#endif 

//-------------------------------------------------------
bool CSFLog::CSFunctionToCFuncSystem()
// Получить из CSFunction CFuncSystem   
{
  int i,j,pos,pos1,pr,k,size;
  string buf, buf1;
  string bufname;
  string bufur;
  CSFVar *pVar,*pVar1,*pVar2;
  CFunc *pFunc ;
  CCon *pCon;
  CSFunction *pFun;

// #ifdef _DEBUG
//   AssertValid();
// #endif     
  pFun = GetFun();
  if (pFun==NULL)  {
//#ifdef _LINUX
    printf("Section FUNCTION is not LOG\n");
//#else
//#ifdef ENG
//    AfxMessageBox("Section FUNCTION is not LOG");
//#else
//    AfxMessageBox("B SF-описании нет раздела FUNCTION");
//#endif
//#endif
    return false;                    
  }
	if (pFun->m_Typ!="LOG")  {
//#ifdef _LINUX
		printf("Section FUNCTION is absent in SF-description\n");
//#else
//#ifdef ENG
//		AfxMessageBox("Section FUNCTION is absent in SF-description");
//#else
//		AfxMessageBox("Раздел FUNCTION не является LOG");
//#endif
//#endif
		return false;                    
	}


  m_FuncSystem.Clean();
  m_FuncSystem.m_Typ=pFun->GetTyp(); 
  m_FuncSystem.m_NumInp=pFun->GetNumInt();
  m_FuncSystem.m_NumOut=pFun->GetNumOut();
  m_FuncSystem.m_NumInter=pFun->GetNumInter();

  i=0;
  while (pFun->GetTextLine(i, buf))  { 
NextFunc:     
    pFunc = new CFunc();                
    while ((pos1 = buf.find(';'))<0) {
      if (!pFun->GetTextLine(i, buf1)) {
//#ifdef _LINUX
        printf("Error equation. No ';'\n");
//#else
//#ifdef ENG
//        AfxMessageBox("Error equation. No ';'");
//#else
//        AfxMessageBox("Ошибка в записи уравнения. Нет ';'.");
//#endif
//#endif
        delete pFunc;
        return false;                    
      }
      buf += buf1;
    }
    bufur=buf.substr(0,pos1+1);   //Left(pos1+1);  
    pFunc->m_Right= bufur;
AfterOpen:
    pos =0;
    pr = GetNextInpName(bufur,pos, bufname);
    if (bufname.size()==0)  {
//#ifdef _LINUX
      printf("Error in equation, output\n");
//#else
//#ifdef ENG
//      AfxMessageBox("Error in equation, output");
//#else
//      AfxMessageBox("Ошибка в записи Вых. перем.уравнения");
//#endif
//#endif
      delete pFunc;
      return false;   
    }  
    pCon = new CCon();      
    pFunc->m_OutVar.m_Name = bufname;
    pFunc->m_OutVar.m_Typ = 'o';
    pFunc->m_OutVar.m_First = 0;
    pFunc->m_OutVar.m_Last = 0;
    
    if ((pr==6)||(pr==7)||(pr==8)) pFunc->m_OutVar.m_Neg = "^";
    else pFunc->m_OutVar.m_Neg = "";
        
    if ((pos = bufur.find('=')) < 0)  {  
//#ifdef _LINUX
      printf("Error in equation. No =\n");
//#else
//#ifdef ENG
//      AfxMessageBox("Error in equation. No =");
//#else
//      AfxMessageBox("Ошибка в записи уравнения. Отсутствует =");
//#endif
//#endif
      delete pFunc;
      delete pCon;
      return false;
    }  
    pos++;
    bufur=bufur.substr(pos,bufur.size()-pos);   //Mid(pos,bufur.GetLength()-pos);  
    j=0; pr=0;
NextInpName:    // read next Conuction;  
    pr = GetNextInpName(bufur,j, bufname);
    if (bufname.size() ==0)   {
      pFunc->m_ConArray.Add(pCon);
      m_FuncSystem.m_FuncArray.Add(pFunc);
      goto Prov;
    }      
    if ((bufname=="1")||(bufname=="0")) {
      if (pFunc->m_InVarArray.Avar.size()>0){
//#ifdef _LINUX
        printf("Error: incorrect name of variable\n%s\n",pFunc->m_Right.c_str());
//#else
//#ifdef ENG
//        pFunc->m_Right = "Error: incorrect name of variable\n" + pFunc->m_Right;
//#else
//        pFunc->m_Right = "Ошибка! Имя переменной = 1 \n" + pFunc->m_Right;
//#endif
//         AfxMessageBox(pFunc->m_Right.c_str());
//#endif
        delete pFunc;
        delete pCon;
        return false;
      }
      pFunc->m_ConArray.RemoveAll();
      delete pCon;
      m_FuncSystem.m_FuncArray.Add(pFunc);
      goto Prov;
    }
    pVar= new CSFVar(bufname, 'i',0,0);
    pVar->m_Neg = ""; 
    pVar1=new CSFVar(pVar); 
    size = pFunc->m_InVarArray.Avar.size();
    if(pFunc->m_InVarArray.Add(pVar)<size)delete pVar;
      
    size = m_FuncSystem.m_InVarArray.Avar.size();
    if (m_FuncSystem.m_InVarArray.Add(pVar1)<size)delete pVar1;  
        
    if ((pr==1)||(pr==4)||(pr==5)||(pr==9)) {
// Скобочная форма, по строке m_right построить ДНФ функции 
// и продолжить построение m_ConArray 
         
/*                
// Скобочная форма, уравнение задается строкой m_right
   pr=6;
*/
   // Освободить память от уже сформированных продукций
      pFunc->m_ConArray.RemoveAll();
      delete pCon;
// Переписать входные переменные в списки входных
NextInpName1:        
      pr = GetNextInpName(bufur,j, bufname);
      if (bufname.size() ==0) {
// В строке m_right раскрыть скобки
        if (pFunc->Open_Brace()==false)return false;
        bufur=pFunc->m_Right;                    
        goto  AfterOpen;       
      }
      pVar = new CSFVar(bufname, 'i',0,0);
      pVar->m_Neg = ""; 
      size = pFunc->m_InVarArray.Avar.size();
      if(pFunc->m_InVarArray.Add(pVar)<size)delete pVar;
      goto NextInpName1;
    }  
    if ((pr==2)||(pr==0)||(pr==6)||(pr==7)) {                                 
        // Продолжить формирование продукции
      pVar2=new CSFVar(bufname, 'i',0,0);
      if ((pr==6)||(pr==7)) pVar2->m_Neg = "^";
      else pVar2->m_Neg = ""; 
      
      size =pCon->m_ConVarArray.Avar.size();
      if (pCon->m_ConVarArray.Add(pVar2)<size)delete pVar2;
      goto NextInpName;
    }
    if ((pr==3)||(pr==8)) {
      // следует формировать новую конъюнкцию
      pFunc->m_ConArray.Add(pCon);
      pCon = new CCon();   
      pVar2=new CSFVar(bufname, 'i',0,0);

      if (pr==8) pVar2->m_Neg = "^";
      else pVar2->m_Neg = ""; 

      size = pCon->m_ConVarArray.Avar.size();
      if (pCon->m_ConVarArray.Add(pVar2)<size)delete pVar2;
      pr=0;
      goto NextInpName;
    }
Prov:
// Есть ли в строке еще уравнения?
    pos1++;
    k=buf.size();
    buf=buf.substr(pos1,k-pos1);        //Mid(pos1,k-pos1);        
    if (buf.size()==0) continue;

    for (j=0; j<(int)buf.size(); j++) {
      if (buf.at(j)!=' ') break;
    }
    if (j>=(int)buf.size()) continue;    
    if (buf.at(j)=='\r') continue;  

    k=buf.size();
    buf=buf.substr(j,k-j);   //Mid(j,k-j);  
    goto NextFunc;
  }          
  return (true); 
}

//-------------------------------------------------------
bool CSFLog::SetPIN()
// Сформировать списки входных, выходных, промежуточных
// переменных в разделе DCL_PIN
{
  CSFVar * pVar;
  CSFVar * pVar1, *pVar2;
  CFunc* pFunc;
  int rab1, j;
  string Buf;

// #ifdef _DEBUG
//   AssertValid();
// #endif     
  m_SFVarArray.RemoveAll();
  m_NumInVar=0;
  m_NumOutVar=0;
  m_NumInterVar=0;

  m_FuncSystem.m_NumOut=0;
  m_FuncSystem.m_NumInter=0;
  m_FuncSystem.m_NumInp=0; 
  for (j=0; j<(int)m_FuncSystem.m_FuncArray.arFun.size(); j++) {
    pFunc=m_FuncSystem.m_FuncArray.GetAt(j);
    pVar2 = &pFunc->m_OutVar;
    if (m_SFVarArray.IsNameInArray(pVar2)==-1) {
     // Переменной еще нет в списке внешних
      pVar= new CSFVar(pVar2);  
      if ((rab1=m_FuncSystem.m_InVarArray.IsNameInArray(pVar))==-1) {
       // выходная
        pVar->m_Typ='o';
        m_FuncSystem.m_NumOut++;                 
      }
      else {              
       // промежуточная
        pVar->m_Typ='t';
        m_FuncSystem.m_NumInter++;
        pVar1=m_FuncSystem.m_InVarArray.GetAt(rab1);
        pVar1->m_Typ='t';
      }
      AddVar(pVar);
    }
  } 
    
for (j=0; j<(int)m_FuncSystem.m_InVarArray.Avar.size(); j++) {     
  pVar=m_FuncSystem.m_InVarArray.GetAt(j);
  if (m_SFVarArray.IsNameInArray(pVar)==-1) {
//  if ((pVar->m_Typ=='i')&&(pVar->m_Name!="Gnd")&&
//    (pVar->m_Name!="Vcc"))  Замена 19.05.2006

if ((pVar->m_Name!="Gnd")&&(pVar->m_Name!="Vcc")) 
{// Входная или промежуточная, которую нужно сделать входной

// if ((pVar->m_Typ=='t')){	}
 m_FuncSystem.m_NumInp++;
 pVar->m_Typ='i';
 pVar1= new CSFVar(pVar);
 AddVar(pVar1);
 }
 }  
}  
//      m_FuncSystem.m_NumInter += m_FuncSystem.m_NumOut;

  if (m_Fun==0){
//#ifdef _LINUX
    printf("There is no section FUNCTION\n");
//#else
//#ifdef ENG
//    Buf="There is no section FUNCTION";
//#else
//    Buf="Нет раздела FUNCTION";
//#endif
//    AfxMessageBox(Buf.c_str());
//#endif
    return false;
  }
  m_Fun->SetNumInt(m_NumInVar);
  m_Fun->SetNumInter(m_NumInterVar);
  m_Fun->SetNumOut(m_NumOutVar);   
  return true;
}  

//-------------------------------------------------------
int CSFLog::Limitation(int inp, int term, int&j)      
// Проверка ограничений в каждом уравнении на число
// входов, конъюнкций. Номер уравнения не отв. огр. - в number
{
  CFunc* pFunc;
// #ifdef _DEBUG
//   AssertValid();
// #endif     
  for (j=0; j<(int)m_FuncSystem.m_FuncArray.arFun.size(); j++) {
    pFunc=m_FuncSystem.m_FuncArray.GetAt(j);
    if ((int)pFunc->m_InVarArray.Avar.size()>inp) return 1;    
    if ((int)pFunc->m_ConArray.arCon.size()>term) return 2;
  }
  return 0;
}

//-------------------------------------------------------
int CSFLog::GetNumInpVarInNewSF(CFunc * pFunc)
  // Опр. число входных переменных уравнения и            
  // формируемой системы, без учета промежуточных                    
{     
// #ifdef _DEBUG
//   AssertValid(); pFunc->AssertValid();
// #endif      
  int nRes;
  int rab1 = GetNumInVar();
  for (int i=0; i<(int)pFunc->m_InVarArray.Avar.size(); i++) {
    if((nRes=m_SFVarArray.IsNameInArray(pFunc->m_InVarArray.Avar[i])==(-1)))   // Надо разобраться!!!!
      rab1++; 
  }
  return rab1;
} 

//-------------------------------------------------------
int CSFLog::ReplaceNames(map <string,string>* ShortNames)      
// Заменить имена в разделе CSFunction 
// согласно таблице ShortNames
{
	
int i;
string buf, buf1;
string bufname;
string bufur, bufur_new;
CSFunction *pFun;
CFunc* pFunc;

// #ifdef _DEBUG
// 	AssertValid();
// #endif     
// Читать строки раздела  m_fun, заменять имена
// и переписывать в pfun

pFun= new CSFunction("LOG",m_FuncSystem.m_NumInp,
	 m_FuncSystem.m_NumOut,m_FuncSystem.m_NumInter);
for (i=0; i<(int)m_FuncSystem.m_FuncArray.arFun.size();i++) {
// получить очередное уравнение
pFunc=m_FuncSystem.m_FuncArray.arFun[i];
// заменить в нем имена согласно ShortNames
bufur_new=pFunc->ReplaceAllNames(ShortNames);
// поместить в новый pFun
pFun->AddTextLine(bufur_new);
}
if (m_Fun != NULL) delete m_Fun;
m_Fun=pFun;   
  return true;
}
//---------------------------------------------------------------
int CSFLog::GetNumTerms()
{ 
//Получить число термов в системе
//Version  21 июля 2008
CSFunction pF;
CSOP *pSOP;
int rab1;
string Buf;	

CFuncSystemToNewFunction();	
if (LogToSdfWithoutElimination()==false)
{
//#ifdef _LINUX
  printf("Error in LogToSdfWithoutElimination\n");
//#else
//#ifdef ENG
//	Buf="Error in LogToSdfWithoutElimination";
//#else
//	Buf="Ошибка при переходе LogToSdfWithoutElimination";
//#endif
//	AfxMessageBox(Buf.c_str());
//#endif
	return (-1);
}

if (!m_Fun->GetSdf(pSOP)) {
//#ifdef _LINUX
  printf("Error when reading section FUNCTION\n");
//#else
//#ifdef ENG
//	Buf="Error when reading section FUNCTION";
//#else
//	Buf="Ошибка чтения раздела FUNCTION";
//#endif
//	AfxMessageBox(Buf.c_str());
//#endif
	delete pSOP;
	return (-1);
	}
rab1=pSOP->GetnCon();
delete pSOP;
return rab1;
}

//---------------------------------------------------------------
// int CSFLog::GetNumTerms(CFunc *pFunc1, CPtrArray *pAB_Rab)
// { 
// //Получить число термов в системе
// //Version  28 июля 2008
// // Если не добавляется уравнение pFunc=0; 
// // если не добавляется куст, pAB_Rab=0
// CSFunction pF;
// CSOP *pSOP;
// int rab1, i;
// CString Buf;
// CFuncSystem *Temp, *Temp1;
// CFunc *pFunc, *pFunc2;	
// 
// #ifdef _DEBUG
// AssertValid();
// #endif
// Temp1 = &m_FuncSystem;
// Temp = new CFuncSystem(Temp1);
// if (pFunc1!=0) 
// {pFunc2= new CFunc(pFunc1);
// Temp->AddFunc(pFunc2);
// }
// if (pAB_Rab!=0) 
// {for (i=0; i<pAB_Rab->GetSize(); i++) 
// { 
// pFunc=(CFunc*)(pAB_Rab->GetAt(i));
// pFunc2= new CFunc(pFunc);
// Temp->AddFunc(pFunc2);
// } 
// }
// CFuncSystemToNewFunction();
// if (LogToSdfWithoutElimination()==false)
// {
// #ifdef ENG
// 		Buf="Error in LogToSdfWithoutElimination";
// #else
// 		Buf="Ошибка при переходе LogToSdfWithoutElimination";
// #endif
// 		AfxMessageBox(Buf);
// 		delete Temp;
// 		return (-1);
// 	}
// 	
// 	if (!m_Fun->GetSdf(pSOP)) {
// #ifdef ENG
// 		Buf="Error when reading section FUNCTION";
// #else
// 		Buf="Ошибка чтения раздела FUNCTION";
// #endif
// 		AfxMessageBox(Buf);
// 		delete pSOP; delete Temp;
// 		return (-1);
// 	}
// 	rab1=pSOP->GetnCon();
// 	delete pSOP;
// //	delete Temp;
// 	return rab1;
// }

//---------------------------------------------------------------
//int CSFLog::GetNumTerms(CFunc *pFunc1, CPtrArray *pAB_Rab)
int CSFLog::GetNumTerms(CFunc *pFunc1, vector <int *> *pAB_Rab)
{ 
	//Получить число термов в системе
	//Version  28 июля 2008
	// Если не добавляется уравнение pFunc=0; 
	// если не добавляется куст, pAB_Rab=0
	CSFunction pF;
	CSOP *pSOP;
	int rab1, i;
	string Buf;
//	CFuncSystem *Temp, *Temp1;
	CFunc *pFunc, *pFunc2;	
	
// #ifdef _DEBUG
// 	AssertValid();
// #endif
 
	if (pFunc1!=0) 
	{pFunc2= new CFunc(pFunc1);
	m_FuncSystem.AddFunc(pFunc2);
	}
	if ( pAB_Rab != 0 ) 
	{for (i=0; i<(int)pAB_Rab->size(); i++) 
	{ 
		pFunc=(CFunc*)(pAB_Rab->at(i));
		pFunc2= new CFunc(pFunc);
		m_FuncSystem.AddFunc(pFunc2);
	} 
	}
SetPIN();
CFuncSystemToNewFunction();
#if defined(_LINUX) || defined(__APPLE__)
unlink ("~Term");
#else
_unlink ("~Term");
#endif
WriteSF("~Term"); 
	if (LogToSdfWithoutElimination()==false)
	{
#ifdef ENG
		Buf="Error in LogToSdfWithoutElimination";
#else
		Buf="Ошибка при переходе LogToSdfWithoutElimination";
#endif
//#ifdef _LINUX
    printf("%s\n",Buf.c_str());
//#else
//		AfxMessageBox(Buf.c_str());
//#endif
//		return (-1);
//	}
	
//	if (!m_Fun->GetSdf(pSOP)) {
//#ifdef ENG
//		Buf="Error when reading section FUNCTION";
//#else
//		Buf="Ошибка чтения раздела FUNCTION";
//#endif
//#ifdef _LINUX
//    printf("%s\n",Buf.c_str());
//#else
//    AfxMessageBox(Buf.c_str());
//#endif
		delete pSOP;
		return (-1);
	}
	rab1=pSOP->GetnCon();
if (pFunc1!=0) 
{
m_FuncSystem.DelFunc(pFunc2);
}
if (pAB_Rab!=0) 
{for (i=0; i<(int)pAB_Rab->size(); i++) 
{ 
pFunc=(CFunc*)(pAB_Rab->at(i));
m_FuncSystem.DelFunc(pFunc);
} 
}
CFuncSystemToNewFunction();
	delete pSOP;
	return rab1;
}

//-------------------------------------------------------
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//class CTypeConnect : public CObject
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//IMPLEMENT_SERIAL(CTypeConnect, CObject, 1)

//-------------------------------------------------------
CTypeConnect::CTypeConnect() {}
//-------------------------------------------------------
CTypeConnect::~CTypeConnect() { m_prefix.clear(); }
//-------------------------------------------------------
CTypeConnect::CTypeConnect(CSFType * pSFType,CTypeConnect * pTypeConnect)
{
  int i,j;
  string Buf,one,name;
// #ifdef _DEBUG
//   pSFType->AssertValid();
//   pTypeConnect->AssertValid();
// #endif     
  m_Type.m_Name = pSFType->GetName();
  m_Type.m_Lib = pSFType->GetLib();
  m_Type.m_Reg = pSFType->GetReg();
  i=0;
  while((Buf = pSFType->GetElem(i))!="")  {
    i++;
    m_Type.AddElem(Buf);
  }

  i=0;
  CSFVar *pVar, *pVar1;
  while(pSFType->GetNextInVar(i,pVar))  {
    i++;
    pVar1 = new CSFVar(pVar);
    m_Type.AddVar(pVar1);
  }
  i=0;
  while(pSFType->GetNextOutVar(i,pVar))  {
    i++;
    pVar1 = new CSFVar(pVar);
    m_Type.AddVar(pVar1);
  }

  for (i=0;i< (int)pTypeConnect->m_prefix.size();i++) {
    Buf=pTypeConnect->m_prefix[i];
    j=0;
    while((one = pSFType->GetElem(j))!="")  {
      j++;
      name=Buf+one+"_";
      m_prefix.push_back(name);
    }
  }
}

//-------------------------------------------------------
void CTypeConnect::Serialize(CArch& ar)
{
  m_Type.Serialize(ar);
  Seria(m_prefix, ar);
}
//-------------------------------------------------------
//#ifdef _DEBUG
// void CTypeConnect::AssertValid() const
// {
//     m_prefix.AssertValid();
//     m_Type.AssertValid();
// }
// //-------------------------------------------------------
// void CTypeConnect::Dump(CDumpContext& dc) const
// {                        
//   m_Type.Dump(dc);
//   m_prefix.Dump(dc);
// }
//#endif 

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//class CTypeConnectArray : public CObArray 
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//IMPLEMENT_DYNCREATE(CTypeConnectArray, CObArray) 

// int CTypeConnectArray::Add(CTypeConnect* pTypeConnect)
// // Добавить TypeConnect в CTypeConnectArray
// {
// #ifdef _DEBUG
//   AssertValid(); pTypeConnect->AssertValid();
// #endif     
//  return CObArray::Add((CObject *) pTypeConnect);
//}

//---------------------------------------------------------------
// CTypeConnect* CTypeConnectArray::GetAt(int nIndex)
// {                
// #ifdef _DEBUG
//   AssertValid();
// #endif     
//   CTypeConnect* pTypeConnect = (CTypeConnect *)CObArray::GetAt(nIndex);
// #ifdef _DEBUG
//   pTypeConnect->AssertValid();
// #endif     
//   return pTypeConnect;
// }

//--------------------------------------------------------------- 
void CTypeConnectArray::RemoveAll()
{ 
// #ifdef _DEBUG
//   AssertValid();
// #endif     
  for (int i=0; i< (int)atcp.size(); i++){
    delete atcp[i];
  }
  atcp.clear();
}

//--------------------------------------------------------------- 
void CTypeConnectArray::RemoveAt(int i)
{ 
  delete  atcp[i];
  atcp.erase(atcp.begin()+i);
}


// #ifdef _DEBUG
// //-------------------------------------------------------
// void CTypeConnectArray::AssertValid() const { }
// //-------------------------------------------------------
// void CTypeConnectArray::Dump(CDumpContext& dc) const { }
// #endif 

