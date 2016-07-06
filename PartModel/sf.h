/////////////////////////////////////////////////////////////////
#ifndef SFH
#define SFH
#include <string>
#include <vector>
#include <utility>
#include <map>

using namespace std;

enum    CIRCMACH{COMB, MACHINE};
enum    BIBLSF{NOBIBL, BIBL};
enum    FUNCTION{ FLOG, FSDF};
enum    MINSF{NOMIN, MIN};

#include "Com_Lib\objects_sys.h"
#include "Com_Lib\archive.h"

#ifdef _LINUX
#include <sys/io.h>
#else
#include <io.h>
#endif

class CDNF;
class CBrace;
class CMapValFrame;
class CMapStringToNode;
class CPNode;
class CSOP;
class CBM;
class CSBF;

////////////////////////////////////////////////////
class CSFVar 
{
 public:
  CSFVar();
  CSFVar(string Name, BYTE Typ, int First=0, int Last=0);
  CSFVar(CSFVar *pSFVar);
  ~CSFVar() {}

// Attributes

public:
  string m_Name;
  BYTE m_Typ;
  int m_First;
  int m_Last;
  int m_NumInSF;                // 29.12.97  - num of this var in the list of corresp. var.
  string m_Neg; // "^" - если переменная с отрицанием, "" - если без, Кириенко, 24.02.00

  inline string GetName()  const  {return m_Name;}
  inline BYTE GetTyp()  const {return m_Typ;}
  inline int  GetFirst()  const {return m_First;}
  inline int  GetLast()  const {return m_Last;}
  string BuildVectorName();
  bool Compare(CSFVar *pSFVar1);
  bool CompareName(CSFVar *pSFVar1);
  CSFVar * operator =(CSFVar *pSFVar1);
  void Serialize(CArch& ar);
};


////////////////////////////////////////////////////
class CSFVarArray
{
public:
  vector <CSFVar*> Avar;
  CSFVarArray() { }
  ~CSFVarArray() { RemoveAll(); }
// Operations
public:
  int Add(CSFVar* pVar);
  int Add_M(CSFVar* pVar);
  int IsVarInArray(CSFVar* pVar);
  int IsNameInArray(CSFVar* pVar);
  void RemoveAll();
  void RemoveAt(int i);
  //-------------------------------------------------------
  inline int GetSize() { return (int)Avar.size(); }
  inline CSFVar *GetAt(int nI) { return Avar[nI]; }
  void Serialize(CArch& ar);
};

////////////////////////////////////////////////////
class CSFType
{
public:
  CSFType();
  CSFType(string Name, bool Reg, string Lib="")  {  //?????????
                            //It's nessesary to insert TRY-CATCH
    m_Name = Name;
    m_Lib = Lib;
    m_Reg = Reg;
  };
  CSFType(CSFType *pSFType);
  ~CSFType()
  {
    m_SFVarArray.RemoveAll();
    m_ElemArray.clear();
  }

// Attributes
  string              m_Name;
  string              m_Lib;
  int                 m_Reg;
  CSFVarArray         m_SFVarArray;
  vector <string>     m_ElemArray;

  inline string GetName()  const  {return m_Name;}
  inline string GetLib()  const {return m_Lib;}
  inline int GetReg()  const {return m_Reg;}
  inline void SetLib(string lib)  {m_Lib = lib;}

  bool GetNextInVar(int &nIndex, CSFVar*&);
  bool GetNextOutVar(int &nIndex, CSFVar*&);
  inline int AddVar(CSFVar* pVar)   { return m_SFVarArray.Add(pVar);  }

  int AddElem(string Elem);
  int DelElem(string Elem);
  string GetElem(int i);
  int IsNameInArray(string Name);
  void Serialize(CArch& ar);
};

////////////////////////////////////////////////////////////////////////////
class CMapStringToType
{
public:
  CMapStringToType()
  { itp = mtp.begin(); }
  ~CMapStringToType() { RemoveAll(); }
  map <string, CSFType*> mtp;
  map <string, CSFType*> :: iterator itp;

// Operations
public:
  inline void GetNextAssoc(string& rKey, CSFType*& rValue)
  {   rKey =  itp->first;   rValue = itp->second;   itp++;   }
  bool Lookup(string key, CSFType*& rValue);

  inline void SetAt(string key, CSFType* newValue)   { mtp[key] = newValue; }

  void RemoveAll();
  void RemoveKey(string Buf);
  void Serialize(CArch&);
};


////////////////////////////////////////////////////
class CSFConnect
{
  CSFConnect();
public:
  CSFConnect(string Name)  {  //?????????
                            //It's nesesary to insert TRY-CATCH
    m_Name = Name;
    iCon = m_Connect.begin(); 
  }
  ~CSFConnect()  {    m_Connect.clear();  }

// Attributes
public:
  string                m_Name;
  map <string, string>  m_Connect;
  map <string, string> :: iterator iCon;
  
  inline int GetCount()  { return m_Connect.size(); }
  inline string GetName()  const  {return m_Name;}
  inline void GetNextAssoc(string& rKey, string& rValue)
  { rValue = iCon->second; rKey = iCon->first;  iCon++; }

  bool GetFirstPare(string& one, string& two);
  bool GetNextPare(string& one, string& two);
  bool GetPareForString(string one, string& two);
  void AddPare(string one, string two);
  void Serialize(CArch& ar);
};

////////////////////////////////////////////////////
class CConnectArray
{
public:
  vector <CSFConnect*> Acon;
  CConnectArray() { }
  ~CConnectArray() { this->RemoveAll(); }
// Operations
public:
  inline int Add(CSFConnect* pCon) { Acon.push_back(pCon);  return Acon.size(); }
  inline int GetSize() { return (int)Acon.size(); }
  inline CSFConnect* GetAt(int nIndex) { return Acon[nIndex]; }
  void RemoveAll();
  void RemoveAt(int i);
  void Serialize(CArch& ar);
};


////////////////////////////////////////////////////
class CSFunction
{
public:
  CSFunction();
  CSFunction(string Typ, int NumInt, int NumOut, int NumInter);
  CSFunction(CSFunction *pFun);

  ~CSFunction()
  {
    m_Text.clear();
  }

// Attributes
  vector <string>      m_Text;
public:
  string              m_Typ;
  int                  m_NumInt;
  int                  m_NumOut;
  int                  m_NumInter;

public:
  inline string GetTyp()    const  {return m_Typ;}
  inline int GetNumInt()     const  {return m_NumInt;}
  inline int GetNumOut()     const  {return m_NumOut;}
  inline int GetNumInter()   const  {return m_NumInter;}
  void SetNumInt(int i);
  void SetNumOut(int i) ;
  void SetNumInter(int i);

  bool GetTextLine(int& index, string &szRes);
  bool ReplaceTextLine(int index, string szRes);
  int AddTextLine(string szRes);

  bool GetSdf(CBM *&TrMat_Zero, CBM *&TrMat_One, CBM *&MatFun);
  bool SetSdf(CBM *TrMat_Zero, CBM *TrMat_One, CBM *MatFun); 
  bool GetSdf(CSOP *&SOP);
  bool SetSdf(CSOP *SOP);

//---------------------------------------- 20.03.2006
//  Набор функций для работы с частичными функциями (матрица функций - 
//  частичная). Признаком служит указание комментария /*T*/ в строке FUNCTION:
//  если указан, то вторая матрица - троичная и может содержать '-'
  bool GetSbf(CBM *&TrMat_Zero, CBM *&TrMat_One, 
              CBM *&MatFun_Zero, CBM *&MatFun_One); 
  bool SetSbf(CBM *TrMat_Zero, CBM *TrMat_One, 
              CBM *MatFun_Zero, CBM *MatFun_One); 
  bool GetSbf(CSBF *&SBF);
  bool SetSbf(CSBF *SBF);

  virtual void Serialize(CArch& ar);

};

////////////////////////////////////////////////////
class CSF
{
public:
  CSF();
  ~CSF();

// Attributes
//private:
  string               m_Title;
  string               m_Project;
  string               m_Author;
  string               m_Date;
  CSFVarArray          m_SFVarArray;
  CMapStringToType     m_TypeMap;
  map <string, string>   m_ElemTypeMap;
  CConnectArray        m_ConnectArray;
  map <string, string>   m_NetMap;
  map <string, string>   m_InterMap;
  CSFunction          *m_Fun;
 //---------------------------- 29.04.1998     Krasilnikova; delete m_Bibl and m_IHaveError
//................................................................................................................
// Надо разбираться
  CMapValFrame         m_Attr;          
//................................................................................................................

  int GetHierarSF(CMapStringToNode&  m_MapSF, int li_Hierar);
//------------------------------
public:
  int                  m_NumInVar;
  int                  m_NumOutVar;
  int                  m_NumInterVar;

  string              m_TitleComments;
  string              m_ProjectComments;
  string              m_AuthorComments;
  string              m_DateComments;
  string              m_SFVarComments;
  string              m_TypeComments;
  string              m_ConnectComments;
  string              m_NetComments;
  string              m_FunComments; // /*T*/ - частичные функции! 20.03.2006
// 20.03.2006     - внесены изменения для реализации формата SDF
//                с частичными функциями. Признаком служит указание
//                комментария /*T*/ в строке FUNCTION: если указан,
//                то вторая матрица - троичная и может содержать '-'

public:
  string GetListOutVar();
  string GetListInVar();
  inline string GetTitle()  {return m_Title;}
  inline void SetTitle(string Title)  { m_Title = Title;}
  inline string GetProject()  {return m_Project;}
  inline void SetProject(string Project)  { m_Project = Project;}
  inline string GetAuthor()  {return m_Author;}
  inline void SetAuthor(string Author)  { m_Author = Author;}
  inline string GetDate()  {return m_Date;}
  inline void SetDate(string Date)  { m_Date = Date;}

//-------  29.12.97
  inline int GetNumInVar(){return m_NumInVar;}
  inline int GetNumOutVar(){return m_NumOutVar;}
  inline int GetNumInterVar(){return m_NumInterVar;}
  bool GetNextInterVar(int &nIndex, CSFVar*&);

  CSFVar *GetInVar(int nIndex);      //nIndex - number in the list of input var.
  CSFVar *GetOutVar(int nIndex);     //nIndex - number in the list of output var.
  CSFVar *GetInterVar(int nIndex);   //nIndex - number in the list of inter var.
  string GetNameInVar(int nIndex);    //nIndex - number in the list of input var.
  string GetNameOutVar(int nIndex);   //nIndex - number in the list of output var.
  string GetNameInterVar(int nIndex); //nIndex - number in the list of inter var.

  CSFVar *GetVarAndNumberForName(string as_Name, int &nIndex);  
                                          // as_Name can be Vector-name
//-------------------------------------------------29.05.2000 Kirienko
// То же, что и GetVarAndNumberForName(string as_Name, int &nIndex) 
// только в качестве nIndex возвращает номер в m_SFVarArray, а
// не номер среди однотипных (входных или выходных) pVar->m_NumInSF 
CSFVar *GetVarAndIndexForName(string as_Name, int &nIndex);  
//--------

  bool GetNextGlobalVar(int &nIndex, CSFVar*&);
  bool GetNextInVar(int &nIndex, CSFVar*&);
  bool GetNextOutVar(int &nIndex, CSFVar*&);
  bool GetNextVar(int &nIndex, CSFVar*&);
  int AddVar(CSFVar* pVar);

  bool GetStartPositionForType(/*POSITION &pos*/);    //24.04.98  Кириенко
  bool GetFirstType(CSFType *& pType/*, POSITION &pos*/) ;
  bool GetNextType(CSFType *& pType/*, POSITION &pos*/) ;
  void AddElemForType(CSFType *Type, string Elem);
  void AddType(CSFType *Type);
  bool IsTypeInSF(string TypeName);

  CSFConnect *GetConnect(int nIndex);
  CSFConnect *GetConnectForType(string Type);
  bool GetTypeForElem(string name, CSFType *& pSFType);
  int AddConnect(CSFConnect *pConnect);
  bool DeleteConnect(int nIndex);
  // Кириенко 26.04.11 
  // Вернуть для имени элемента указатель на раздел CONNECT  и его индекс
  int GetConnectForElem(string Name, CSFConnect *& pConnect);

// Кардаш 06.04.2006
  bool GetFirstTypeName(CSFType *& pType, string & buf) ;  //22.01.03  Кардаш
  bool GetNextTypeName(CSFType *& pType, string & buf) ;  //22.01.03  Кардаш
//////

  bool GetFirstPareNet(map<string,string>::iterator& pos, string& one, string& two);
  bool GetNextPareNet(map<string,string>::iterator &pos, string& one, string& two);
  string GetPareNetForString(string one);
  void AddPareNet(string one, string two);

  bool GetFirstPareInter(map<string,string>::iterator &pos, string& one, string& two);
  bool GetNextPareInter(map<string,string>::iterator &pos, string& one, string& two);
  string GetPareInterForString(string one);
  void AddPareInter(string one, string two);

  void SetFun(CSFunction *pFun); 
  inline CSFunction *GetFun() {return m_Fun;}
	void ChangeSubstr(string str1,string str2="");    //Кириенко 19 февраля 2015 г.
// Заменить подстроку str1 на подстроку str2 в именах переменных в разделе CSFunction


//................................................................................................................
// Надо разбираться!!!!!!!!!!!!!
//  inline bool GetError()  {return (bool)m_Attr.CheckAttr("ERROR", 1); }
//  inline void SetExistError(bool bErr)   { m_Attr.SetAt("ERROR", (double)bErr);}
//................................................................................................................

/////////////////////////////////////////////////////////////////////
//-------------------- Файл SF_brace.cpp-----------------
//*** Litskevich 11.03.98 *******************************************
  bool GetBrace(CBrace *pBrace);
  bool SetBrace(CBrace *pBrace);
  bool OpenBraces(void);
//*******************************************************************                                                                     

/////////////////////////////////////////////////////////////////////
//-------------------- Файл SF_Write.cpp-----------------
  bool WriteSF(string FileName);
  bool WriteSF(vector <string>& Obj);
  void WriteVar(FILE* f, BYTE Typ, CSFVarArray *pVA);
  void WriteVar(vector <string>& Obj, BYTE Typ, CSFVarArray *pVA);

/////////////////////////////////////////////////////////////////////
//-------------------- Файл SF_Read.cpp-----------------
  bool ReadNextSF(FILE *fp, long& len);
  void InitSFAttrib();
  int ReadSFByName(FILE *fp, string Name); // Кириенко 21.02.00
  int ReadSFFromFile(string PathToDir);
  int ReadSFFromLib(string Name,string PrivLib,string LogLib,
                    string Lib_BMK,string LibSTRUCTURE); // Кириенко 21.01.02
  bool ReadNextSF(vector <string>& Obj, int & index); //Кириенко 11.05.06

/////////////////////////////////////////////////////////////////////
//-------------------- Файл SF_methods.cpp-----------------
  bool LogToSdfWithoutElimination(void);
  bool SdfToLog();
  //Определение числа каскадов в системе уравнений (возвр. значение)
  //и CMapStringToPtr - соответствие "Имя переменной - номер каскада"
  int Cascade (map <string, string>& NumOfCascade);  // Кириенко 21.01.02
  
  //Определение числа уровней схемы
  int GetLevel();                         //03.04.2006 Кардаш С.Н. 

  // Замена имени переменной в уравнениях,           Кириенко 25.05.2000 
  bool RenameVar(map <string, string>* MultiPare);
/////////////////////////////////////////////////////////////////////

  virtual void Serialize(CArch& ar);
};

#endif
