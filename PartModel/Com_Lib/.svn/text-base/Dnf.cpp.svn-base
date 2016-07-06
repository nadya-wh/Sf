#include "dnf.h"
//**************** Constructors/Destructor **********************************

CDNF::CDNF() :CTM() {  }
CDNF::CDNF(const CDNF& dnf) : CTM(dnf) { }
CDNF::CDNF(const char* pch) : CTM(pch) { }                       
CDNF::CDNF(CTM tm) : CTM(tm)  { }
CDNF::CDNF(int nRow,int nColumn,char symb/*='-'*/) : CTM(nRow,nColumn,symb) { }

CDNF::~CDNF() { }

//-----------------------------------------------------------------------------
bool CDNF::Sum(CTV& tv)
{
  for (int i=0;i<GetCountR();i++)
  {
    if (IsCover(i,tv))  return false;
    if (IsCovered(i,tv))
    {
      SetRowTv(i,*this,GetCountR()-1);
      SetSize(GetCountR()-1,GetCountC());
      i--;
      continue;
    }
    if (AreNeighb(i,tv))
    {
      tv=tv.MinCover(GetRowTv(i));
      SetRowTv(i,*this,GetCountR()-1);
      SetSize(GetCountR()-1,GetCountC());
      i=-1;
      continue;
    }
  }
  Add(tv);
  return true;
}

//-----------------------------------------------------------------------------
int CDNF::SumCon(CTV tv)
{
  for (int i=0;i<GetCountR();i++)  if (AreEqual(i,tv)) return i;
  return Add(tv);
}

//-----------------------------------------------------------------------------
void CDNF::Sum(CDNF& dnf)
{
  CTV tv;
  for (int i=0;i<dnf.GetCountR();i++) 
  {
    tv=dnf.GetRowTv(i);
    Sum(tv);
  }
}

//-----------------------------------------------------------------------------
int CDNF::Sum(int end,int first,int last)
{
//  ASSERT(end<first);
//  ASSERT(first<=last);
//  ASSERT(last<=GetCountR());

  int Counter=0,i,j,delta=0;

  if (end<0)
  {
    InsertRow(0,first,*this);
    end=0;
    first=first+2;
    last++;
    delta++;
  }

  bool Add;

  for (i=first;i<=last;i++)
  {
    Add=true;
    for (j=0;j<=end+Counter;j++)
    {
      if (IsCover(i,j))
      {
        SetRowTv(j,*this,end+Counter);
        Counter--;
        j--;
        continue;
      }
      if (IsCover(j,i))
      { Add=false;     break; }
      if (AreNeighb(j,i))
      {
        SetRowTv(i,MinCover(j,i));
        SetRowTv(j,*this,end+Counter);
        Counter--;
        j=-1;
        continue;
      }
    }
    if (Add) 
    {
      Counter++;
      SetRowTv(end+Counter,*this,i); 
    }
  }
  return (Counter+delta);
}

//-----------------------------------------------------------------------------
void CDNF::Product(CDNF& dnf)
{
  int i,j,p=-1,q=-1;

  bool One1 = false, One2 = false;
  if ((!GetCountR())&&(GetCountC())) One1 = true;
  if ((!dnf.GetCountR())&&(dnf.GetCountC())) One2 = true;
  if (One1) {
    Sum(dnf);
    return;
  }
  if (One2) return;

  for(j=0;j<dnf.GetCountR();j++)
    for (i=0;i<GetCountR();i++)
    {
      if (AreEqual(i,dnf.GetRowTv(j)))
      {
        dnf.SetRowTv(j,dnf,dnf.GetCountR()-1);
        dnf.SetSize(dnf.GetCountR()-1,dnf.GetCountC());
        j--;
        p++;
        ExchangeRow(p,i);
        break;
      }
      else
      {
        if (IsCover(i,dnf.GetRowTv(j)))
        {
          q++;
          dnf.ExchangeRow(q,j);
          break;
        }
        else
        {
          if (i>p)
          if (IsCovered(i,dnf.GetRowTv(j)))
          {
            p++;
            ExchangeRow(p,i);
          }
        }
      }
    }

  int nRow=GetCountR();
  for (i=q+1;i<dnf.GetCountR();i++)
  {
    for (j=p+1;j<nRow;j++)
    {
      if (!AreOrthog(j,dnf.GetRowTv(i)))
          Add(Intersec(j,dnf.GetRowTv(i)));
    }
  }

  dnf.SetSize(q+1,dnf.GetCountC());
  if (nRow!=GetCountR())
    SetSize(Sum(p,nRow,GetCountR()-1)+p+1,GetCountC());
  else
    SetSize(p+1,GetCountC()); 

  Sum(dnf);
}

//-----------------------------------------------------------------------------
void CDNF::Product(CTV& tv)
{
  for (int i=0;i<GetCountR();i++)
  {
    if (AreOrthog(i,tv))
    {
      RemoveRow(i);
      i--; 
    }
    else
      SetRowTv(i,Intersec(i,tv)); 
  }
}

//-----------------------------------------------------------------------------
void CDNF::ProductD(CTV& ed) 
{
  CTV tv1;
  char cB1,cB2;
  int nRow=GetCountR(),j; //,Counter=0

  for (int i=0;i<nRow;i++) {
    CTV tv=GetRowTv(i);
    RemoveRow(i);
    i--;
    nRow--;
    if (ed.IsCover(tv)) {
      if (tv.CountDefs()==1) ed.SetBitAt(tv.LeftDef(),'-');
      Add(tv);
    } 
    else {
      j=-1;
      while((j=ed.LeftDef(j))>-1) {
        cB1=tv.GetBitAt(j);
        cB2=ed.GetBitAt(j);
        if (cB1==cB2) {
          bool bAdd=true;
          for (int ind=0;ind<GetCountR();ind++) {
            if (IsCover(ind,tv)) {
              bAdd=false;
              break;
            }
            if (IsCovered(ind,tv)) {
              SetRowTv(ind,*this,GetCountR()-1);
              SetSize(GetCountR()-1,GetCountC());
              if (ind<nRow) nRow--;
              ind--;
              continue;
            }
            if (AreNeighb(ind,tv)) {
              tv=tv.MinCover(GetRowTv(ind));
              SetRowTv(ind,*this,GetCountR()-1);
              SetSize(GetCountR()-1,GetCountC());
              if (ind<nRow) nRow--;
              ind=-1;
              continue;
            }
          }
          if (bAdd) Add(tv);
        }
        if (cB1=='-') {
          tv1=tv;
          tv1.SetBitAt(j,cB2);
          bool bAdd=true;
          for (int ind=0;ind<GetCountR();ind++) {
            if (IsCover(ind,tv1)) {
              bAdd=false;
              break;
            }
            if (IsCovered(ind,tv1)) {
              SetRowTv(ind,*this,GetCountR()-1);
              SetSize(GetCountR()-1,GetCountC());
              if (ind<nRow) nRow--;
              ind--;
              continue;
            }
            if (AreNeighb(ind,tv1)) {
              tv1=tv1.MinCover(GetRowTv(ind));
              SetRowTv(ind,*this,GetCountR()-1);
              SetSize(GetCountR()-1,GetCountC());
              if (ind<nRow) nRow--;
              ind=-1;
              continue;
            }
          }
          if (bAdd) Add(tv1);
        }
      }
    }
  }
  return;
}

//-----------------------------------------------------------------------------
CDNF CDNF::Invertion(void)
{
  CTV tv, empt(GetCountC(),'-');
  CDNF dnf;//(1,GetCountC(),'-');
  int j;

  if (!GetCountR()) {
    dnf.Add(empt);
    return dnf;
  }
  if (GetCountR() == 1) {
    bool one = true;
    for (int ind = 0; ind < GetCountC(); ind++)
      if (GetBitAt(0, ind)!='-') {
        one = false; 
        break;
      }
    if (one) {
      dnf.SetSize(0, GetCountC());
      return dnf;
    }
  }

  if (GetCountR()>0) {
    tv=GetRowTv(0);
    tv.InvertDefs();
    j=-1;
    while((j=tv.LeftDef(j))>-1) {
      dnf.Add(empt);
      dnf.SetBitAt(dnf.GetCountR()-1, j, tv.GetBitAt(j));
    }
  }

  for (int i=1;i<GetCountR();i++)
  {
    tv=GetRowTv(i);
    tv.InvertDefs();
    dnf.ProductD(tv);
  }
  return dnf; 
}

//-----------------------------------------------------------------------------
void CDNF::Check(int nRow)
{
//  ASSERT(0<=nRow);
//  ASSERT(nRow<GetCountR());

  int CountRow=GetCountR();
  for (int i=0; i<CountRow; i++) {
    if (i==nRow) continue;
    if (IsCover(i,nRow)) {
      RemoveRow(nRow);
      return;
    }
    if (IsCovered(i,nRow)) {
      if (nRow>i) nRow--;
      RemoveRow(i);
      i--;
      CountRow--;
      continue;
    }
    if (AreNeighb(i,nRow)) {
      SetRowTv(nRow,MinCover(i,nRow));
      RemoveRow(i);
      if (nRow>i) nRow--;
      i=-1;
      CountRow--;
      continue;
    }
  }
  return;
}

//-----------------------------------------------------------------------------
void CDNF::Conc(void)
{
  int i,j,k;

  for (i=0;i<GetCountR();i++)
    for (j=i+1;j<GetCountR();j++)
    {
      if (IsCover(i,j))
      {
        RemoveRow(j);
        j--;
        continue;
      }
      if (IsCover(j,i))
      {
        RemoveRow(i);
        i--;
        break;
      }
    }
  for (i=1;i<GetCountR();i++)
    for (j=0;j<i;j++)
    {
      if (AreConcens(i,j))
      {
        Add(Concens(i,j));
        for (k=0;k<GetCountR()-1;k++)
        {
          if (IsCover(k,GetCountR()-1))
          {
            if (AreEqual(k,GetCountR()-1))
            {
              RemoveRow(k);
              break;
            }
            else 
            {
              RemoveRow(GetCountR()-1);
              break;
            }
          }
          if (IsCover(GetCountR()-1,k))
          {
            RemoveRow(k);
            if (k<=i) i--;
            if (k<=j) j--;
            k--;
          }
        }
      }
    }
}

//-----------------------------------------------------------------------------
bool CDNF::EqualTo(CDNF* dnf)
{
  if (!GetCountR())
    {
    if (!dnf->GetCountR()) return true;
    else return false;
    }
  if ((GetCountR()==1)&&(!CountDefs(0)))
    {
    if ((dnf->GetCountR()==1)&&(!dnf->CountDefs(0))) return true;
    else return false;
    }
  if (!dnf->GetCountR())
    {
    if (!GetCountR()) return true;
    else return false;
    }
  if ((dnf->GetCountR()==1)&&(!dnf->CountDefs(0)))
    {
    if ((GetCountR()==1)&&(!CountDefs(0))) return true;
    else return false;
    }
  CDNF d1,d2;
  d1=*this;
  d2=dnf->Invertion();
  d1.Product(d2);
  if (d1.GetCountR()) return false;
  d2=this->Invertion();
  d2.Product(*dnf);
  if (d2.GetCountR()) return false;
  return true; 
}

//-----------------------------------------------------------------------------
CDNF CDNF::Cofactor(int nVar, bool bBranch)
{
  int k, j;
  if (!GetCountR()) {
    CDNF dnf;
    dnf.SetSize(0,GetCountC());
    return dnf;
  }
//  ASSERT (nVar<GetCountC());
  CDNF co;
  char symb=bBranch?'1':'0';
  for (j=0;j<GetCountR();j++) {
    if (GetBitAt(j,nVar)==symb) {
      co.Add(GetRowTv(j));
      co.SetBitAt(co.GetCountR()-1,nVar,'-');
    }
    if (GetBitAt(j,nVar)=='-') co.Add(GetRowTv(j));
  }
  for (j=0;j<co.GetCountR();j++) {
    if (!co.CountDefs(j)) {
      co.SetRowTv(0,co.GetRowTv(j));
      co.SetSize(1,co.GetCountC());
      break;
    }
    for (k=co.GetCountR()-1;k>j;k--) {
      if (co.IsCover(j,k)) co.RemoveRow(k);
      if (co.IsCovered(k,j)) {
        co.RemoveRow(j);
        continue;
      }
      if (co.AreNeighb(j,k))
      {
        co.SetRowTv(j,co.MinCover(j,k));
        co.RemoveRow(k);
      }
    }
  }
  return co;
}

