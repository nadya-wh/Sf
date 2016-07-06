#ifndef OBJECTS_SYS
#define OBJECTS_SYS

#include <map>
#include <vector>
#include <string>
#include <utility>
#include <stack>
#include "../Com_Lib/archive.h"
//#include "../com_lib/sf.h"

using namespace std;
class CMapAttributes;
class CPNode;
class CSF;
extern void Seria(vector <string>& ms, CArch& ar);
extern void Seria(map <string, string>& ms, CArch& ar);
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
//------------------------------------------------------------


//..................................................................................

// Полная замена vector <double>

///////////////////////////////////////////////////////////// CDoubleArray
//
// 1. CDoubleArray - Array of double value
//
/////////////////////////////////////////////////////////////
// class CDoubleArray : public CObArray
// {
// public:
//   CDoubleArray();
//   ~CDoubleArray();
//   DECLARE_DYNCREATE(CDoubleArray)
// // Accessing elements
//   double* GetAt(int nIndex) const;
//   void SetAt(int nIndex, double newElement);
//   void SetAtGrow(int nIndex, double newElement);
//   int Add(double newElement);
//   void RemoveAll();
// };
//..................................................................................


///////////////////////////////////////////////////////////// CBStack
//
// 2. CBStack     -  Stack with very short integers
//
/////////////////////////////////////////////////////////////
class CBStack
{
public:
  stack <BYTE> sb;
  CBStack() {}
  ~CBStack() { Clear(); }
  // Operations
public:
  // Accessing elements
  inline void Push(BYTE El) { sb.push(El); }

  void Clear() 
  { while(!sb.empty()) sb.pop();  }

  bool Pop(BYTE& El) 
  { 
    if (sb.empty()) return 0;
    El = sb.top();
    sb.pop();
  }
};



///////////////////////////////////////////////////////////// CStrStack
//
// 3. CStrStack     -  Stack with strategies names
//
/////////////////////////////////////////////////////////////
class CStrStack
{
public:
  stack <string> ss;
  CStrStack() {}
  ~CStrStack() { Clear(); }
  // Operations
public:
  // Accessing elements
  inline void Push(string El) { ss.push(El); }

  void Clear() 
  { while(!ss.empty()) ss.pop();  }

  bool Pop(string& El) 
  { 
    if (ss.empty()) return 0;
    El = ss.top();
    ss.pop();
  }
};



///////////////////////////////////////////////////////////// CMapValFrame
//
// 4. CMapValFrame - Map of values attributes for Frame
//
/////////////////////////////////////////////////////////////
class CMapValFrame
{
public:
  map <string, string> mvf;
  map <string, string> :: iterator ivf;
  CMapValFrame() { ivf = mvf.begin(); }
  ~CMapValFrame() { mvf.clear(); }

// Operations
public:
  inline void RemoveAll() { mvf.clear(); }
  inline void GetAt(string key, string& res) { res = mvf[key]; }
  inline void SetAt(string key, string newValue) { mvf[key]= newValue; }
  int GetAt(string key, double& d);
  void SetAt(string key, double newValue);
  inline void GetNextAssoc(string& rKey, string& rValue)
  { rValue = ivf->second; rKey = ivf->first;  ivf++; }

//   int  WriteToIni(string File, string Section);
//   int  ReadFromIni(string sFile, string Section, vector <string> &whatAttr);
  int GetAt(CMapAttributes* m_pMapAttributes, string key, string& res, double& d);  
                                   // -1 - bad, 0 - int, 1 - double, 2 - string
  bool CheckAttr(string key, string Value);
  bool CheckAttr(string key, double Value);
  void Serialize(CArch&);
};


///////////////////////////////////////////////////////////// CPNodeArraySmall
//
// 5. CPNodeArraySmall     -  Special array of nodes CPNode
//
/////////////////////////////////////////////////////////////
class CPNode;
class CPNodeArraySmall
{
public:
  vector <CPNode*> apn;
  CPNodeArraySmall() {} 
  ~CPNodeArraySmall() { RemoveAll(); } 
// Operations
public:
  void RemoveAll();
  inline int Add(CPNode* pNode) { apn.push_back(pNode); return apn.size(); }
  inline CPNode* GetAt(int nIndex) { return apn[nIndex];}
  inline int GetSize() { return (int)apn.size(); }

  void Serialize(CArch& ar);  
};

///////////////////////////////////////////////////////////// CPNode
//
// 6. CPNode     -  node of SF net
//
/////////////////////////////////////////////////////////////
//Element  EK[i] contain reference to CPNode (it's index in array ???), 
//which is shown in row number i in view
class CPNode //: public CObject
{
public:
  CPNode() {  m_NumStringInView = 0;   m_pSF = NULL;  }

  ~CPNode();// { m_Attr.RemoveAll(); m_Children.RemoveAll();  if (m_pSF != NULL)  delete m_pSF; }

// Attributes
  CSF              *m_pSF;         // Element   -SF
  string          m_PldName;       // File with PLD  
  CMapValFrame     m_Attr;
  BYTE             m_level;        // Number of elements level
  CPNodeArraySmall m_Children;
  int              m_NumStringInView;

// Operations
public:
  virtual void Serialize(CArch& ar);   // overridden for document i/o
};

/*
///////////////////////////////////////////////////// CPNodeArray

// 7. CPNodeArray     -  Array of nodes CPNode

/////////////////////////////////////////////////////
class CPNodeArray  // : public CObArray 
{
public:
  vector <CPNode*> apn;
  CPNodeArray() { } 
  ~CPNodeArray() { RemoveAll(); } 
public:
  int Add(CPNode* pNode);
  CPNode* GetAt(int nIndex);  
  void RemoveAll(); 
  CPNode *GetChildWithAttrib(int ActNode, CKnAttr *pKnAttr);
  void Serialize(CArch& ar);  
};
*/

///////////////////////////////////////////////////////////// CMapStringToNode
//
// 8.    CMapStringToNode   - Map of strings for nodes
//
/////////////////////////////////////////////////////////////
class CMapStringToNode      // : public CMapStringToOb
{
public:
  map <string, CPNode*> mpn;
  map <string, CPNode*> :: iterator ipn;
  CMapStringToNode() { ipn = mpn.begin(); }
  ~CMapStringToNode() { RemoveAll(); }

// Operations
public:
  inline void SetAt(string key, CPNode* newValue) { mpn[key]= newValue; }
  inline void GetNextAssoc(string& rKey, CPNode*& rValue)
    { rValue = ipn->second; rKey = ipn->first;  ipn++; }
  bool Lookup(string key, CPNode*& rValue);
  void RemoveAll();
  void RemoveKey(string Buf);
  void Serialize(CArch& ar);  
};



#if 0
///////////////////////////////////////////////////////////// CMenuESS
//
// 9.      CMenuESS       - menu position in ESS
//
/////////////////////////////////////////////////////////////
class CMenuESS : public CObject
{  DECLARE_SERIAL(CMenuESS)
public:
  CMenuESS();  // Construction
// Attributes
  CString m_Name;        // Name of program modul (key for Map)
  CString m_Label;       // Text of menu position
  HTREEITEM m_hParent;     // Handle of parent menu
  HTREEITEM m_hOwn;        // Own handle
  ~CMenuESS();
};

///////////////////////////////////////////////////////////// CArrayMenuESS
//
// 10.  CArrayMenuESS     - Array of MenuESS positions
//
/////////////////////////////////////////////////////////////
class CArrayMenuESS : public CObArray
{
public:
  CArrayMenuESS();
  DECLARE_DYNCREATE(CArrayMenuESS)
// Accessing elements
  CMenuESS* GetAt(int nIndex) const;
  int Add(CMenuESS* newElement);
  void RemoveAll();
};

#endif

//........................................................................................................
// Надо разбираться

//bool ReadDoc(CString FileName, CMapAttributes* pA, CMapProgModuls* pM,
//             CMapStrategies* pS, CMapFrames* pF, CWinApp *pApp);
//........................................................................................................



#endif //__OBJECTS_SYS__
