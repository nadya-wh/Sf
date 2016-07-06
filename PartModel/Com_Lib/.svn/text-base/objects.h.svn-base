#ifndef OBJECTS
#define OBJECTS

#include <map>
#include <vector>
#include <string>
#include <utility>
#include <stack>
#include "../Com_Lib/archive.h"

using namespace std;

void Seria(vector <string>& ms, CArch& ar);
void Seria(map <string, string>& ms, CArch& ar);
//------------------------------------------------------------
// 1.      CKnAttr     -          A T T R I B U T E
// 2. CMapAttributes   - Map  of  A T T R I B U T I E S
// 3.      CExpres     -          E X P R E S S I O N
// 4. CArrayExpression - Array of E X P R E S S I O N S
// 5.      CProgMod    -          P R O G R A M  M O D U L
// 6.  CMapProgModuls  - Map  of  P R O G R A M  M O D U L S
// 7.      CSlot       -          S L O T  O F  F R A M E
// 8.  CArraySlots     - Array of S L O T S  O F  F R A M E
// 9.      CFrame      -          F R A M E
// 10.   CMapFrames    - Map  of  F R A M E
// 11.     CProduction -          P R O D U C T I O N
// 12.CArrayProduction - Array of P R O D U C T I O N S
// 13.     CStrategy   -          S T R A T E G Y
// 14. CMapStrategies  - Map  of  S T R A T E G I E S
//------------------------------------------------------------

///////////////////////////////////////////////////////////// CKnAttr
//
// 1.      CKnAttr     - A T T R I B U T E
//
/////////////////////////////////////////////////////////////
class CKnAttr
{  
public:
  CKnAttr() { } // Construction
  CKnAttr& operator=(const CKnAttr& atr);
// Attributes
  string m_FullName;   // For full name of attributes
  string m_ShortName;  // For short name of attributes
  BYTE m_Type;          // 0 - logical, 1 - integer, 2 - string
  BYTE m_Active;        // 1 - active, 0 - passive
  BYTE m_Dyn;           // 1 - dynamic, 0 - static
  string m_StrValue;   // For string value
  double m_Value;       // For logical or integer value
  void Serialize(CArch&);
  ~CKnAttr() { }
};

///////////////////////////////////////////////////////////// CMapAttributes
//
// 2. CMapAttributes   - Map of A T T R I B U T I E S
//
/////////////////////////////////////////////////////////////
  class CMapAttributes
  {
public:
  CMapAttributes() 
   { iA = mpa.begin(); }
  ~CMapAttributes() { RemoveAll(); }
  map<string,CKnAttr*> mpa;
  map <string, CKnAttr*> :: iterator iA;

// Operations
  bool Lookup(string key, CKnAttr*& rValue);
  void RemoveAll();
  inline void GetNextAssoc(string& rKey, CKnAttr*& rValue)
  {   rKey =  iA->first;   rValue = iA->second;   iA++;   }
  inline void SetAt(string key, CKnAttr* newValue)   { mpa[key] = newValue; }
  
  void PrintMapAttribs(string FileName);
  void PrintFullAttribs(string FileName);
  void Serialize(CArch&);
};

///////////////////////////////////////////////////////////// CExpres
//
// 3.      CExpres     - E X P R E S S I O N
//
/////////////////////////////////////////////////////////////
class CExpres
{
public:
  CExpres(){ }  // Construction
// Attributes
  BYTE m_Type;        // 0 - logical, 1 - integer, 2 - string
  string m_Formula;  // For string value
  WORD m_Value;       // For  value (TRUE / FALSE)
  ~CExpres() {}
  void Serialize(CArch&);
};

///////////////////////////////////////////////////////////// CArrayExpression
//
// 4. CArrayExpression - Array of E X P R E S S I O N
//
/////////////////////////////////////////////////////////////
class CArrayExpression
{
public:
  vector <CExpres*> ae;
  CArrayExpression() {}
  ~CArrayExpression() { RemoveAll(); }
// Accessing elements
  inline CExpres* GetAt(int nIndex) { return ae[nIndex]; }
  inline int GetSize() { return (int)ae.size(); }
  inline void SetAt(int nIndex, CExpres* newElement) { ae[nIndex]=newElement; }
  inline int Add(CExpres* newElement) { ae.push_back(newElement); return ae.size(); }
  void RemoveAll();
  void Serialize(CArch&);
};

///////////////////////////////////////////////////////////// CProduction
//
// 11.      CProduction - P R O D U C T I O N
//
/////////////////////////////////////////////////////////////
class CProduction
{
public:
  CProduction() { }  // Construction
// Attributes
  CArrayExpression m_ArrayExpression;     // Left part of production
  string m_Formula;
  BYTE m_Type;    // Right part of production:  0 - strategy, 1 - modul
  string m_NameCon;  // Name of strategy or modul
  ~CProduction();
  void Serialize(CArch&);
  string CreateCondition();
  void operator =( CProduction* src );
};

///////////////////////////////////////////////////////////// CProgMod
//
// 5.      CProgMod    - P R O G R A M  M O D U L
//
/////////////////////////////////////////////////////////////
class CProgMod 
{
public:
  CProgMod() { }  // Construction
// Attributes
  string m_Name;         // Name of program modul
  string m_Menu;         // String to menu of main program
  string m_Comment;      // Comment for program modul
  int m_Undo;            // Possibility of Undo operation
  BYTE m_Type;            // 0 - internal Windows, 1 - external Windows,
                          // 2 - DOS  3 - special DOS  4... - reserved
  vector <string> m_AttrNames;   // Map of modul setting attributes
  vector <string> m_ParamNames;  // Map of modul using parameters
  CProduction m_Prod;     // Condition of execution
  ~CProgMod() { m_AttrNames.clear(); m_ParamNames.clear(); }
  void Serialize(CArch&);
};

///////////////////////////////////////////////////////////// CMapProgModuls
//
// 6.  CMapProgModuls  - Map of P R O G R A M  M O D U L S
//
/////////////////////////////////////////////////////////////
class CMapProgModuls
{
public:
  map <string, CProgMod* > mpr;
  map <string, CProgMod*> :: iterator iPm;
  CMapProgModuls() { iPm = mpr.begin(); }
// Operations
  inline void GetNextAssoc(string& rKey, CProgMod*& rValue) 
  { rValue = iPm->second; rKey = iPm->first;  iPm++; }
  bool  Lookup(string key, CProgMod*& rValue);
  inline void SetAt(string key, CProgMod* newValue) { mpr[key] = newValue; }
  void RemoveAll();
  void PrintFullProgs(string FileName);
  ~CMapProgModuls() { RemoveAll(); }
  void Serialize(CArch&);
};

///////////////////////////////////////////////////////////// CSlot
//
// 7.      CSlot       - S L O T  O F  F R A M E
//
/////////////////////////////////////////////////////////////
class CSlot 
{
public:
 CSlot() { }
 ~CSlot() { m_AttrNames.clear(); }

// Attributes
  string m_Name;         // Name of Slot
  vector <string> m_AttrNames;   // Map of slot attributes
  void Serialize(CArch&);
};

///////////////////////////////////////////////////////////// CArraySlots
//
// 8.  CArraySlots     - Array of S L O T S  O F  F R A M E
//
/////////////////////////////////////////////////////////////
class CArraySlots
{
public:
  vector <CSlot*> as;
  CArraySlots() { }
  ~CArraySlots() { RemoveAll(); }

  // Accessing elements
  inline CSlot* GetAt(int nIndex) { return as[nIndex]; }
  inline int GetSize() { return (int)as.size(); }
  inline void SetAt(int nIndex, CSlot* newElement) { as[nIndex]=newElement; }
  inline int Add(CSlot* newElement)  { as.push_back(newElement); return as.size(); }
  CSlot* GetForName(string Name);
  void RemoveAll();
 void Serialize(CArch&);
};

///////////////////////////////////////////////////////////// CFrame
//
// 9.      CFrame      - F R A M E
//
/////////////////////////////////////////////////////////////
class CFrame
{
public:
  CFrame() { }
  ~CFrame() { m_ArraySlots.RemoveAll(); }// Attributes
  string m_Name;            // Name of frame
  WORD m_Specimen;           // Number of frame type
  string m_Comment;         // Comment of frame
  CArraySlots m_ArraySlots;  // Array of slots
  void Serialize(CArch&);
};

///////////////////////////////////////////////////////////// CMapFrames
//
// 10.   CMapFrames    - Map  of  F R A M E
//
/////////////////////////////////////////////////////////////
class CMapFrames
{
public:
  map <string, CFrame* > mfr;
  map <string, CFrame*> :: iterator iFr;
  CMapFrames() { iFr = mfr.begin(); }
  ~CMapFrames() { RemoveAll(); }
// Operations
public:
  CFrame* GetFrame(int Num);
  inline void GetNextAssoc(string& rKey, CFrame*& rValue)
                           { rValue = iFr->second; rKey = iFr->first;  iFr++; }
  bool Lookup(string key, CFrame*& rValue);
  inline void SetAt(string key, CFrame* newValue) { mfr[key] = newValue; } 
  void RemoveAll();
  void Serialize(CArch&);
};

///////////////////////////////////////////////////////////// CArrayProduction
//
// 12.CArrayProduction - Array of P R O D U C T I O N S
//
/////////////////////////////////////////////////////////////
class CArrayProduction
{
public:
  vector <CProduction*> apr;
  CArrayProduction() {}
  ~CArrayProduction() { RemoveAll(); }

  // Accessing elements
  inline CProduction* GetAt(int nIndex)  { return apr[nIndex]; }
  inline int GetSize() { return (int)apr.size(); }
  inline void SetAt(int nIndex, CProduction* newElement){ apr[nIndex]=newElement; }
  inline int Add(CProduction* newElement) { apr.push_back(newElement); return apr.size(); }
  void RemoveAll();
  void Serialize(CArch&);
};

///////////////////////////////////////////////////////////// CStrategy
//
// 13.     CStrategy   - S T R A T E G Y
//
/////////////////////////////////////////////////////////////
class CStrategy
{
public:
  CStrategy() { }  // Construction
// Attributes
  string m_Name;                      // Name of strategy
  string m_Comment;                   // Comment of strategy
  CArrayProduction m_ArrayProduction;  // Array of productions
  ~CStrategy() { m_ArrayProduction.RemoveAll(); }
  void Serialize(CArch&);
};

/////////////////////////////////////////////////////////////
//
// 14.  CMapStrategies  - Map  of  S T R A T E G I E S
//
/////////////////////////////////////////////////////////////
class CMapStrategies
{
public:
    map <string, CStrategy* > mst;
    map <string, CStrategy*> :: iterator iSt;
    CMapStrategies() { iSt = mst.begin(); }
    ~CMapStrategies() { RemoveAll(); }

// Operations
  CStrategy* GetStrategy(int Num);
  inline void GetNextAssoc(string& rKey, CStrategy*& rValue)
      { rValue = iSt->second; rKey = iSt->first;  iSt++; }
  bool Lookup(string key, CStrategy*& rValue);
  inline void SetAt(string key, CStrategy* newValue) { mst[key] = newValue; } 
  void RemoveAll();
  void Serialize(CArch&);
};

#endif //__OBJECTS.H__
