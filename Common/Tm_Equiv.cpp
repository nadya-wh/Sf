/////////////////////////////////////////////////////////////////
// Ver.2.1.0     07.07.2006
/////////////////////////////////////////////////////////////////
//int CTM::NoEquiv(CTM &Tm)
//int CTM::NoImpl (CTM &Tm)
//int CTM::NoImpl(CTV &tv)
//int CTM::NoImpl(CTV &tv, CBV &bvR)
//int CTM::CrossI(CTM &Tm)
//CTM CTM::DistAL (int e, CBV &bv, CBV &bvR)
//void CTM::DisConDis (CTV &tv, CTM &TmEE)
//void CTM::DorD(int Regime, CTM Tm1)
//void CTM::DorD(int Regime, CTV tva)
/////////////////////////////////////////////////////////////////

#include <stdlib.h>
#ifdef _JOINT
#include "BaseBool.h"
#include "BaseTern.h"
#else
#include "../Common/BaseBool.h"
#include "../Common/BaseTern.h"
#endif

//________________________________________________________________
// Проверка отношения эквивалентности двух ДНФ (this <-> Tm).
// Возвращается
// z=0,  если отношение выполняется,
// z=-1, если системы ДНФ несоразмерны,
// z>0 , если (z-1)-я строка Tm не имплицирует this.
// z<-1, если (|z|-2)-я строка this не имплицирует Tm.
//________________________________________________________________

int CTM::NoEquiv(CTM &Tm)
{
  int z;
  if (z = NoImpl(Tm)) return ((z==-1)?z:z=-z-1);
  return Tm.NoImpl(*this);
}

//__________________________________________________________
// Проверка отношения импликации this -> Tm.
// Возвращается
// z=0,  если отношение выполняется,
// z=-1, если ДНФ несоразмерны,
// z>0 , если (z-1)-я строка this не имплицирует Tm.
//__________________________________________________________

int CTM::NoImpl (CTM &Tm)
{
  int a, b, c, d, g, i, j, k, p, u, v, q, w=0,
  n=Tm.GetCountC(), p1=GetCountR(), p2=Tm.GetCountR();
  char s;
  if(n!=Tm.GetCountC() || p1&&!p2) return -1; 
  CTM TmW, Tm_T(Tm, 0);
  CBM BmR;
  CTV tv; 
  CBV bvX, bvR((BYTE)1,p2), bvRR=bvR, bvRW;
  for(g=0; g<p1; g++) {
    tv = GetRowTv(g);
    bvR = bvRR; i=-1;
    while((i = bvR.LeftOne(i))!=-1) 
      if(Tm.AreOrthog(i,tv)) bvR.SetBitAt(i,0);
    if(bvR.IsZero()) return g+1; 
    TmW.Add(tv);
    BmR.Add(bvR); 
    while((i=TmW.GetCountR()-1)>-1) {
      tv=TmW.GetRowTv(i);
      bvR=BmR.GetRowBv(i);
      TmW.RemoveRow(i);
      BmR.RemoveRow(i); 
      q=0;
      while (!bvR.IsZero()) {
        c=n+1; q=0;j=-1;
        while ((j=bvR.LeftOne(j))!=-1) {
          if(Tm.IsCover(j,tv)) {q=1; break;}
          if((b=Tm.CountDefs(tv.GetUnDefs(),j))<c)
          {c=b; d=j;}
        }
        if(q || (bvX=Tm.GetRowDef(d,tv.GetUnDefs())).IsZero()) break;
        p=-1; k=-1; 
        while((k=bvX.LeftOne(k))!=-1)
          if ((a=Tm_T.CountDefs(bvR,k))>=p) {
            b=Tm_T.CountOnes(bvR,k); c=a-b;
            if ((a>p)||(b>u)||(c>u)) {
              v=k; p=a; u=(b<c)? c:b; s=(b<c)? '0':'1';
            } 
          }
        tv.SetBitAt(v, s); 
        bvRW = Tm_T.GetRowUnDef(v)|((s=='1')
               ? Tm_T.GetRowOne(v) : Tm_T.GetRowZero(v));
        bvRW &= bvR;
        TmW.Add(tv);
        BmR.Add(bvRW);
        tv.SetBitAt(v,((s-'0')^'1')); 
        bvR &= Tm_T.GetRowUnDef(v)|((s=='1')
               ? Tm_T.GetRowZero(v) : Tm_T.GetRowOne(v));
      }
      if(!q) return g+1;
    }
  }
  return 0;
}


//____________________________________________________________
// Проверка отношения импликации tv -> this(bvR), 
// где bvR выделяет строчный минор в this.
// Возвращается
// z=0, если отношение выполняется и z=1 в противном случае.
//____________________________________________________________

int CTM::NoImpl(CTV &tv)
{
  CBV bvR((BYTE)1, GetCountR()); 
  return NoImpl(tv, bvR);
}

//____________________________________________________________
// Проверка отношения импликации tv -> this(bvR), 
// где bvR выделяет строчный минор в this.
// Возвращается
// z=0, если отношение выполняется и z=1 в противном случае.
//____________________________________________________________

int CTM::NoImpl(CTV &tv, CBV &bvR)
{
  int i=-1;
  CBV bvX=tv.GetUnDefs(), bvY=bvR; 
  while((i=bvY.LeftOne(i))!=-1) {
    if (!AreOrthog(i, tv)) {
      if (IsCover(i,tv)) return 0;
    }
    else bvY.SetBitAt(i,0);
  }
  if(!Degen(bvX, bvY).IsEmpty()) return 1;
  return 0;
}

//______________________________________________________
// Проверка отношения пересечения двух ДНФ (this и Tm)  
// Возвращается
// z=0,  если пересечения не обнаружено,
// z=-1, если системы ДНФ несоразмерны,
// z>0 , если (z-1)-я строка this пересекается с Tm.
//______________________________________________________

int CTM::CrossI(CTM &Tm)
{
  if (GetCountC()!=Tm.GetCountC()) return -1;
  int i,j; CTV tv;
  for (i=0; i<GetCountR(); i++) {
    tv=GetRowTv(i);
    for (j=0; j<Tm.GetCountR(); j++)
      if (!Tm.AreOrthog(j,tv)) return i+1;
  }
  return 0;
}

//***************************************************************
// Расширение множества аргументов локальной ДНФ Tm по исходному
// (bv) и результирующему (bvR) векторам ее аргументов 
// При e=-1 все столбцы из исходной матрицы переписываются 
// в результирующую, а при e>-1 перепись e-го столбца 
// блокируется.
//*************************************************************** 
 
CTM CTM::DistAL (int e, CBV &bv, CBV &bvR)
{
  int a=GetCountR(), b, c, d, h=bvR.CountBit();
          //CString sTm, sTmR, sbv, sbvR;//sTm=Tm.BitChar();//sbv=bv.BitChar();//sbvR=bvR.BitChar();
  CTM TmR(a,(h)?h:GetCountC());
  if(a)
  {
    TmR.Clear('-');
    a=-1; c=0; d=(!e)?1:0; 
    while((a=bvR.LeftOne(a))!=-1)
    {
      if(bv.GetBitAt(a))
      {
        for(b=0; b<GetCountR(); b++)
          TmR.SetBitAt(b,c,GetBitAt(b,d));
        d++; if(d==e) d++;
      }
      c++;
    }
  }       //sTmR=TmR.BitChar();
  return TmR;
}

//******************************************
// Перемножение конъюнкции tv на ДНФ TMEE
// и добавление результата в Tm
//****************************************** 
 
void CTM::DisConDis (CTV &tv, CTM &TmEE)
{
  int a, b=GetCountR(), e=0;
  for(a=0; a<TmEE.GetCountR(); a++)
    if(!TmEE.AreOrthog(a,tv))
    {
      Add(TmEE.Intersec(a,tv));
      if(AreEqual((b+e), tv)) 
      { RemoveRow(b,e); break;}
      e++;
    }
}

//***********************************************************************
// Дизъюнкция двух ДНФ с использованием интервальных
// операций поглощения и обобщенного склеивания для упрощения
//***********************************************************************

void CTM::DorD(int Regime, CTM Tm1)
{
  int a, e, i, k;
  CTV tva, tvc;           //CString sTm1, sTm2, stva, stvc;//sTm1=Tm1.BitChar();//sTm2=Tm2.BitChar();
  if(Regime<1)
  {
    for(i=0; i<Tm1.GetCountR(); i++) Add(Tm1.GetRowTv(i)); return;
  }
  while (k=Tm1.GetCountR())
  {
    tva=Tm1.GetRowTv(k-1);
//stva=tva.BitChar();
    Tm1.RemoveRow(k-1); 
    for(i=0, e=1; i<GetCountR(); i++)
    {
      if(IsCover(i,tva)) {e=0; break;}
      if(IsCovered(i,tva))
      { RemoveRow(i); i--; continue;} 
      if(Regime<2) continue;
      if(AreConcens(i,tva))
      {
        a=0; tvc=Concens(i,tva);  //stvc=tvc.BitChar();
        if (tvc.IsCover(tva)) a+=1;
        if(IsCovered(i,tvc)) a+=2;
        if(a%2) {tva=tvc; if(a>1) RemoveRow(i); i=-1; continue;}
        else 
          if(a)//{Tm1.Add(tvc); Tm2.RemoveRow(i); i--;}
            if(Regime>2) {Tm1.Add(tvc); RemoveRow(i); i--;}
            else SetRowTv(i,tvc);
        
      }
    }
    if (e) Add(tva);    //sTm1=Tm1.BitChar();//sTm2=Tm2.BitChar();
  }        //sTm2=Tm2.BitChar();
}

//***********************************************************************
// Дизъюнкция конъюнкции tva и ДНФ Tm с использованием интервальных
// операций поглощения и обобщенного склеивания для упрощения
//***********************************************************************

void CTM::DorD(int Regime, CTV tva)
{
  int a, e, i;
  CTV tvc;                 //CString sTm, stva, stvc;  //sTm=Tm.BitChar();
  if(Regime<1)
  { Add(tva); return;}                      //stva=tva.BitChar();
  for(i=0, e=1; i< GetCountR(); i++)
  {
    if(IsCover(i,tva)) {e=0; break;}
    if(IsCovered(i,tva))
    { RemoveRow(i); i--; continue;} 
    if(Regime<2) continue;
    if(AreConcens(i,tva))
    {
      a=0; tvc= Concens(i,tva);            //stvc=tvc.BitChar();
      if (tvc.IsCover(tva)) a+=1;
      if(IsCovered(i,tvc)) a+=2;
      if(a%2) {tva=tvc; if(a>1) RemoveRow(i); i=-1; continue;}
      else 
        //if(a)//{Tm1.Add(tvc); Tm.RemoveRow(i); i--;}
        //if(Regime>2) {tva=tvc; Tm.RemoveRow(i); i=-1;}
        //else Tm.SetRowTv(i,tvc);
        if(a) 
        {
          if(Regime>2) {Add(tvc); RemoveRow(i); i--;}
          else SetRowTv(i,tvc); 
        }
    }
  }   //stva=tva.BitChar();   //sTm=Tm.BitChar();
  if (e) Add(tva);  //sTm=Tm.BitChar();
}
