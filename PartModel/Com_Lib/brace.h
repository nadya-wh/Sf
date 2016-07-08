/////////////////////////////////////////////////////////////////
// Ver.1.1.0    21.11.2004   Russian == English
// Ver.1.1.1    17.03.2006
//              - поправки форматирования
// Ver.1.1.2    07.04.2006  мелочи
/////////////////////////////////////////////////////////////////
//
//Содержит определение классов CDNF, CDNFArray и CBrace
//
/////////////////////////////////////////////////////////////////

#ifndef _BRACE
#define _BRACE

//#define AMOUNT 1000

class CDNF   : public CTM
{
public:
 //****************** Constructors/Destructor ***********************************
  CDNF();
  CDNF(const CDNF& dnf);
  CDNF(CTM tm);
  CDNF(const char* pch);
  CDNF(int nRow,int nColumn,char symb='-');
  ~CDNF();

 //**************** Operations with DNFs ****************************************
  bool Sum(CTV& tv);
  int  SumCon(CTV tv);
  bool Sum_check(CTV& tv);
  void Sum(CDNF& dnf);
  int Sum(int end,int first,int last);
  bool Sum_cons(CTV& tv, int* curr);

  void Product(CTV& tv);
  void Product(CDNF& dnf);

  void ProductD(CTV& ed);
  CDNF Invertion(void);
  void ProductD_ext(CTV& ed);
  CDNF Invertion_ext(void);

  void Conc(void);
  void Check(int nRow);
  bool EqualTo(CDNF* dnf);
  CDNF Cofactor(int nVar, bool bBranch);

// #ifdef _DEBUG
//   virtual void Dump(CDumpContext& dc) const;
// #endif

};


class CDNFArray    //:CObArray
{
 public:
  CDNFArray();
//  DECLARE_DYNCREATE(CDNFArray)
  vector <CDNF*> Adnf;
// Operations
 public:
  inline int Add(CDNF* pDNF) { Adnf.push_back(pDNF); return Adnf.size(); }
  inline CDNF* GetAt(int nIndex) { return(Adnf[nIndex]); }
  inline int GetSize() { return (int)Adnf.size(); }
// void SetAtGrow( int nIndex, CDNF* newElement );
  inline void RemoveAt(int i) { delete Adnf[i]; Adnf.erase(Adnf.begin()+i);}
  void RemoveAll(); 
  inline void SetSize( int nNewSize) { Adnf.reserve(nNewSize+10); }

// #ifdef _DEBUG
//   virtual void AssertValid() const;
//   virtual void Dump(CDumpContext& dc) const;
// #endif
};

class CBrace
{
public:
  int Y_amount,X_amount,Z_amount;
  vector <int> n_Fun;
  CBM  Vars;
  CBV m_mask;
  CDNFArray m_DnfArray;

//********************** Constructors ************************************
  CBrace(void);
  CBrace(CBrace& Br); 
  ~CBrace();
//********************** Functions with data *****************************

  void SetDNF(int nDNF,CDNF DNF);        // Sets DNF #nDNF
  void SetNumFun(int nFun, int Number);  // Sets nFun[nFun] to be equal to Number
  void SetFun(int Fun=-1);               // Sets every element of nFun to be equal Fun
  void SetMask(CBV mask);                // Sets every element of mask to 1
  void SetMaskBit(int i,bool b_mask);    // Sets element #i of mask to 1 or to 0

  int GetX(void);                        // Returrns X_amount
  int GetY(void);
  int GetZ(void);
  CDNF GetDNF(int No);                   // Returns DNF #No
  int GetNumFun(int nFun);               // Returns element of nFun #nFun
  CBV GetMask();
  bool GetMaskBit(int nVar);
  CBV GetVars(int nFun);

  int CountZ(int nFun);
  int MaxCountTerms(void);
  bool IsUsed(int nVar);                 // Returns TRUE if variable #nVar is used in some DNF
  bool IsUsed(int nVar,int nFun);        // Returns TRUE if variable #nVar is used in DNF #nFun

  int EstimateLenDnf(int nDnf1, int nDnf2);

  void RemoveDNF(int No);

  void BraceOpening(void);               // Opens all braces    
  bool Elemenation(int nDnf,int nVar);   // Substitutes DNF #nDnf1 into DNF #nDnf2
  void UpdateVars(void);                 // Utdates array Vars

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif
};

#endif   //_BRACE
