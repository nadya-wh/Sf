/////////////////////////////////////////////////////////////////////
// Ver.1.1.0     05.04.2004   Russian == English
// Ver.1.1.1     21.07.2008 Добавлен метод GetNumberOfTerms() 
//                          в класс CSFLog

/////////////////////////////////////////////////////////////////////
// Определение классов CCon, CConArray
//                    CFunc, CFuncArray
//                    CFuncSystem
//                    CSFLog
//                    CTypeConnect, CTypeConnectArray
//                    CSFLog

#ifndef SFLOGH
#define SFLOGH

#include <string>
#include <vector>
#include <utility>
#include <map>

using namespace std;

//#ifdef _LINUX
#undef _DEBUG
typedef unsigned char BYTE; 
typedef unsigned long ULONG; 

#define TRUE 1
#define FALSE 0

#define ASSERT assert
#include <assert.h>
/*
#else

//typedef int bool;
#include <afxwin.h>         // MFC core and standard components
#ifndef FO_
#define FO_
#define FSTD(_x) friend _x AFXAPI 
#define STD(_x) _x AFXAPI 
#endif

#endif
*/
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

#include "../ComBool/ShortBool.h"
#include "../ComBool/ShortTern.h"
#include "../Com_Lib/archive.h"

#include "../Com_Lib/objects.h"
#include "../Com_Lib/objects_sys.h"

////////////////////////////////////////////////////
class CCon
{
public:
  CSFVarArray  m_ConVarArray;
  CCon() { }
  ~CCon() { m_ConVarArray.RemoveAll(); } 
  virtual void Serialize(CArch& ar);

}; 

/////////////////////////////////////////////////////////////////////
class CConArray
{
public:
  CConArray() {  } 
  vector <CCon*> arCon;
  ~CConArray() { RemoveAll(); } 

  // Operations
  //---------------------------------------------------------------
  inline int Add(CCon* pCon) { arCon.push_back(pCon); return arCon.size(); }
  //--------------------------------------------------------------- 
  inline CCon* GetAt(int nIndex) { return arCon[nIndex]; }

  //--------------------------------------------------------------- 
  inline int GetSize() { return (int)arCon.size(); }

  //--------------------------------------------------------------- 
  void RemoveAll();
  //--------------------------------------------------------------- 
  virtual void Serialize(CArch& ar);
};


/////////////////////////////////////////////////////////////////////
class CFunc
{
public:
  CFunc();       
  CFunc(CFunc *pFunc);                     
  ~CFunc();

  CSFVar              m_OutVar;
  CSFVarArray         m_InVarArray;
  CConArray           m_ConArray;
  string             m_Right;
  
// Заменить имя pVar на pVar1 в символьной записи уравнения
  bool ReplaceName(CSFVar *pVar, CSFVar *pVar1, int k);
// Построить CFunc по строке buf 
  bool InitFunc(string buf);
// Построить строку m_Right по уравнению 
  bool Set_m_Right();
// Раскрыть скобки в строке m_Right
  bool Open_Brace();
//Заменить все имена в уравнении согласно таблице
  string ReplaceAllNames(map <string, string>* ShortNames);                          

  void print_CFunc(string FileName);   
  virtual void Serialize(CArch& ar);
}; 

/////////////////////////////////////////////////////////////////
class CFuncArray
{
public:
  CFuncArray() { } 
  vector <CFunc*> arFun;
  ~CFuncArray() { RemoveAll(); } 

// Operations
  inline int Add(CFunc* pFunc) { arFun.push_back(pFunc); return arFun.size(); }
  inline CFunc* GetAt(int nIndex) { return arFun[nIndex]; }
  inline int GetSize() { return (int)arFun.size(); }
  void RemoveAt(int i);
  void RemoveAll(); 
  void Serialize(CArch& ar);
};

//////////////////////////////////////////////////////////////////
class CFuncSystem
{
public:                                                          
  CFuncSystem() { }                    
  CFuncSystem(string Typ, int NumInt, int NumOut, int NumInter);
  CFuncSystem(CFuncSystem *pFuncSystem);
  ~CFuncSystem();

  // Attributes
  string              m_Typ;
  int                 m_NumInp;
  int                 m_NumOut;
  int                 m_NumInter;
  CSFVarArray         m_InVarArray;
  CFuncArray          m_FuncArray;

// удалить элементы массивов m_InVarArray, m_FuncArray;   
  void Clean();
// Добавить уравнение в систему
  int AddFunc(CFunc* pFunc);
// Удалить уравнение из системы
  bool DelFunc(CFunc* pFunc);
// Удалить уравнение из системы
  bool DeleteFunc(int i);

// Есть ли переменная в правых частях уравнений формируемой системы 
  CFunc * GetFuncWithMaxVar(int &);
// Опр. число входных и промежуточных переменных уравнения и формируемой системы.                    
  int GetNumInpVarInNewFuncSystem(CFunc * pFunc);
// Определить  списки входных, промежуточных и выходных перменных  
  int GetVarArrayFuncSystem(CSFVarArray &InpArray,CSFVarArray &OutArray,
                  CSFVarArray &InterArray);
// Заменить имена переменных в системе уравнений в соответствии
// с удалением уравнения A=B или A=^B. В pVar - А, в pVar1 - В   
  bool Replace(CSFVar *pVar,CSFVar *pVar1,int k);
// Получить уравнение связи из большой системы уравнений для переменной,
// которая слева  
  void print_CFuncSystem(string FileName);  
    
  inline string GetTyp()    const  {return m_Typ;}
  inline int GetNumInp()     const  {return m_NumInp;}
  inline int GetNumOut()     const  {return m_NumOut;}
  inline int GetNumInter()   const  {return m_NumInter;}
  inline void SetNumInp(int i)     { m_NumInp=i;}
  inline void SetNumOut(int i)     { m_NumOut=i;}
  inline void SetNumInter(int i)   { m_NumInter=i;}
  inline void SetTyp(string name) {m_Typ=name;}                                                                 

  virtual void Serialize(CArch& ar);
}; 

///////////////////////////////////////////////////
class CSFLog : public CSF

{
public:
  CSFLog();
//  DECLARE_SERIAL(CSFLog)
  ~CSFLog();

// Attributes
CFuncSystem  m_FuncSystem;

CFuncSystem *GetFuncSystem();  
void SetFuncSystem(CFuncSystem *pFuncSystem);
bool CSFunctionToCFuncSystem();
// Получить из CSFunction CFuncSystem   

bool CFuncSystemToNewFunction();
// Получить из CFuncSystem CSFunction
bool SetPIN();
//  Сформировать по системе уравнений числа входов, выходов,
//  пром. и CSF::CSFVarArray
bool ReplaceConnectFunc();
// Подставить уравнения связей вида А=В,  А=^B
int Limitation(int inp, int term, int&j);      
// Проверка ограничений в каждом уравнении на число
// входов, конъюнкций. Номер уравнения не отв. огр. - в number
int GetNumInpVarInNewSF(CFunc * pFunc);
  // Опр. число входных переменных уравнения и            
  // формируемой системы, без учета промежуточных 
int ReplaceNames(map <string,string>* ShortNames);
// Заменить имена в разделе CSFunction 
// согласно таблице ShortNames
int GetNumTerms();
//Получить число термов в системе
//version  21 июля 2008

//int GetNumTerms(CFunc *pFunc, CPtrArray *pAB_Rab);
int GetNumTerms(CFunc *pFunc, vector <int *> *pAB_Rab);
//Получить число термов в системе + уравнение + куст
//version  28 июля 2008
// Если не добавляется уравнение pFunc=0; 
// если не добавляется куст, pAB_Rab=0


// #ifdef _DEBUG
//   virtual void AssertValid() const;
//   virtual void Dump(CDumpContext& dc) const;
// #endif
};

//////////////////////////////////////////////////////////////////
class CTypeConnect //: public CObject 
{
public:                                                          
//  DECLARE_SERIAL(CTypeConnect)
  CTypeConnect();                    
  ~CTypeConnect() ;
  CTypeConnect(CSFType * pType,CTypeConnect * pTypeConnect);
  // Attributes
  CSFType  m_Type; // Нераскрытый TYPE
  vector <string>  m_prefix; // Массив префиксов, где перечислены все внешние  
// по уровню вложенности имена, где встречен данный TYPE 
  virtual void Serialize(CArch& ar);

// #ifdef _DEBUG
//   virtual void AssertValid() const;
//   virtual void Dump(CDumpContext& dc) const;
// #endif
}; 

///////////////////////////////////////////////////

class CTypeConnectArray //  : public CObArray 
{
public:
  vector <CTypeConnect*> atcp;
  CTypeConnectArray() { } 
  ~CTypeConnectArray() { RemoveAll(); } 
//  DECLARE_DYNCREATE(CTypeConnectArray)
// Operations
  inline int Add(CTypeConnect* pTypeConnect) 
  { atcp.push_back(pTypeConnect);  return atcp.size(); }
  inline CTypeConnect* GetAt(int nIndex) { return atcp[nIndex]; };
  inline int GetSize() { return (int)atcp.size(); }
  void RemoveAt(int i);
  void RemoveAll(); 
//  virtual void Serialize(CArch& ar);

// #ifdef _DEBUG
//   virtual void AssertValid() const;
//   virtual void Dump(CDumpContext& dc) const;
// #endif
};
//////////////////////////////////////////////

#endif
