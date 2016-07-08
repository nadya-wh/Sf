/////////////////////////////////////////////////////////////////
// Ver.2.1.1     19.06.2007
//               замена для LINUX возврат NULL на имя объекта
// Ver.2.1.0     07.07.2006
/////////////////////////////////////////////////////////////////
//CTM CTM::InverS (int simpl)
//CTM CTM::InverS (int simpl, CBV bvR)
//CTM CTM::InverS (int simpl, CTV tv, CBV bvR)
//CTV CTM::Differ (CTV tv_, CBV &bvR)
//void CTM::AdExpans(CTM &Tm0, CTV &tv, CBV &bvC, CBV &bvW)
/////////////////////////////////////////////////////////////////

#include <stdlib.h>
#ifdef _JOINT
#include "BaseBool.h"
#include "BaseTern.h"
#else
#include "../Common/BaseBool.h"
#include "../Common/BaseTern.h"
#endif

//_____________________________________________________________
// Инверсия ДНФ Tm1
// simpl - параметр упрощения возвращаемой ДНФ
// simpl=0 - без упрощения получаемой ДНФ, представленной
//           множеством взаимно-ортогональных интервалов;
// simpl=1 - упрощение путем расширения и поглощения интервалов;
// simpl=2 - упрощение путем обобщенного склеивания интервалов;
// simpl=3 - упрощение с комбинацией режимов simpl=1 и simpl=2.
//______________________________________________________________

CTM CTM::InverS (int simpl)
{
  CTV tv(GetCountC(), '-');
  CBV bvR(1, GetCountR());
  return InverS (simpl, tv, bvR);
}

//_____________________________________________________________
// Инверсия строчного минора троичной матрицы Tm1 (bvR)
// simpl - параметр упрощения возвращаемой ДНФ
// simpl=0 - без упрощения получаемой ДНФ, представленной
//           множеством взаимно-ортогональных интервалов;
// simpl=1 - упрощение путем расширения и поглощения интервалов;
// simpl=2 - упрощение путем обобщенного склеивания интервалов;
// simpl=3 - упрощение с комбинацией режимов simpl=1 и simpl=2.
//______________________________________________________________

CTM CTM::InverS (int simpl, CBV bvR)
{
  CTV tv(GetCountC(), '-');
  return InverS (simpl, tv, bvR);
}

//______________________________________________________________
// Вычитание из интервала tv строчного минора матрицы Tm1 (bvR) 
// simpl - параметр упрощения возвращаемой ДНФ
// simpl=0 - без упрощения получаемой ДНФ, представленной
//           множеством взаимно-ортогональных интервалов;
// simpl=1 - упрощение путем расширения и поглощения интервалов;
// simpl=2 - упрощение путем обобщенного склеивания интервалов;
// simpl=3 - упрощение с комбинацией режимов simpl=1 и simpl=2.
//______________________________________________________________

CTM CTM::InverS (int simpl, CTV tv, CBV bvR)
{
  int i=0, j, k, p, q, u, v, a, b, c, d, e,
  n=GetCountC(), l=GetCountR();
    //CString ZZsTm1, ZZsTm, ZZsTm0, ZZsTmW, ZZsBmR, ZZsBmS, ZZsBmC, ZZsTm1_T, 
    //ZZstv, ZZstvC, ZZsbvC, ZZsbvD, ZZsbvY, ZZsbvR, ZZsbvX, ZZsbvRW, ZZsbvS, ZZsbvSW;
    //ZZsTm1=Tm1.BitChar(); //ZZsbvR=bvR.BitChar();
  char s;
  CTM Tm0, TmW, Tm1_T = Trans(); //(Tm1, 0);
  Tm0.SetSize(0,n);
  CBM BmR, BmS, BmC;
  i=-1;
  while ((i=bvR.LeftOne(i))!=-1) 
  {
    if(AreOrthog(i,tv)) bvR.SetBitAt(i,0);
    else if(IsCover(i, tv)) return Tm0;
  }
  if(!l||bvR.IsZero()) { Tm0.Add(tv); return Tm0;}
  CTV tvC=tv;
  CBV bvX(1, n), bvRW=bvR, bvS=bvR, bvSW=bvR,
  bvC(0,n,1), bvD(0,n,1), bvY(0,n,1);
  TmW.Add(tv); if(!l||bvR.IsZero()) return TmW;
  BmR.Add(bvR); 
  if(simpl&1) {BmS.Add(bvS); BmC.Add(bvD);}
  while((i=TmW.GetCountR()-1)>-1) 
  {
    tv=TmW.GetRowTv(i);
    bvR=BmR.GetRowBv(i);
    TmW.RemoveRow(i);
    BmR.RemoveRow(i);
    if(simpl&1) 
    {
      bvC=BmC.GetRowBv(i); BmC.RemoveRow(i);  
      bvS=BmS.GetRowBv(i); BmS.RemoveRow(i);
    }         //ZZstv=tv.BitChar();//ZZsbvC=bvC.BitChar();//ZZsbvR=bvR.BitChar();//ZZsbvS=bvS.BitChar();
    q=1; //if(!i) goto A;
    while (q) 
    {
      e=n+1; j=-1; bvX=tv.GetUnDefs();
      while((j=bvR.LeftOne(j))!=-1)  
        if((b=CountDefs(bvX,j))<e) 
        { e=b; d=j; if(e<2) break;}
        if(e==1) 
        {
          v=LeftDef(d,-1,bvX); 
          s=GetBitAt(d,v);
        }
        else 
        {
          bvX&=GetRowDef(d);
          p=-1; k=-1; 
          while ((k=bvX.LeftOne(k))!=-1)
            if ((a=Tm1_T.CountDefs(bvR,k))>=p) 
            {
              b=Tm1_T.CountOnes(bvR,k); c=a-b;
              if ((a>p)||(b>u)||(c>u)) 
              {
                v=k; p=a; u=(b<c)? c:b; s=(b<c)? '0':'1';
              } 
            }
        }
        bvRW=bvR; if(simpl&1) {bvD=bvC; bvSW=bvS;}
        tv.SetBitAt(v, s); 
        bvR = Tm1_T.GetRowUnDef(v)|((s=='1')
              ? Tm1_T.GetRowOne(v) : Tm1_T.GetRowZero(v));
        if(simpl&1) bvC.SetBitAt(v,1);
        bvR&=bvRW;             //ZZstv=tv.BitChar();//ZZsbvC=bvC.BitChar();//ZZsbvR=bvR.BitChar();//ZZsbvS=bvS.BitChar();
        j=-1; q=1;
        while ((j=bvR.LeftOne(j))!=-1)
          if (IsCover(j, tv)) {q=0; break;}
        if (q) 
        {
          TmW.Add(tv); BmR.Add(bvR);
          if(simpl&1) {BmC.Add(bvC); BmS.Add(bvS);}
             //ZZsTmW=TmW.BitChar();//ZZsBmC=BmC.BitChar();//ZZsBmR=BmR.BitChar();//ZZsBmS=BmS.BitChar();
        }
        tv.SetBitAt(v,((s-'0')^'1')); 
        bvR = Tm1_T.GetRowUnDef(v)|((s=='1')
              ? Tm1_T.GetRowZero(v) : Tm1_T.GetRowOne(v));
        if(simpl&1) {bvS=bvSW&bvR; bvC=bvD;}
        bvR&=bvRW;           //ZZstv=tv.BitChar();//ZZsbvC=bvC.BitChar();//ZZsbvR=bvR.BitChar();//ZZsbvS=bvS.BitChar();
        q=1; j=-1;
        while ((j=bvR.LeftOne(j))!=-1) 
        {
          q=2; if (IsCover(j, tv)) {q=0; break;}
        }
        if(q==1) 
        { 
          if(simpl&1) AdExpans(Tm0,tv,bvC,bvS);
          else Tm0.Add(tv);              //ZZsTm0=Tm0.BitChar();
        }
        if(q<2) break;
    }
  }
  if(simpl &2) 
  {
    CTM Tm;
    Tm.SetSize(0, Tm0.GetCountC(), 20); 
    Tm.DorD(simpl, Tm0);                   //ZZsTm=Tm.BitChar();
    return Tm;
   }                                     //ZZsTm0=Tm0.BitChar();
  return Tm0;
}

//______________________________________________________
// Урезание интервала tv_ из множества Tm до минимально
// возможного, покрывающего, однако, все его собственные 
// существенные элементы (чрез вычитание tv_ из Tm 
// по строковой маске bvR)
//______________________________________________________

CTV CTM::Differ (CTV tv_, CBV &bvR)
{
  int i, j, k, l, n, p, u, v, a, b, c, d, q, w=0;
    //CString sTm, sTmW, sBmR, sTm_T, stv, stv1, sbvY, sbvR, sbvX, sbvRW;
    //sTm=Tm.BitChar(); //stv=tv_.BitChar();  //sbvR=bvR.BitChar();
  n=GetCountC(), l=GetCountR();
  char s;
  CTM TmW, Tm_T=Trans(); // (Tm, 0);
  CBM BmR;
  CTV tv=tv_, tv1; i=-1;
  while((i=bvR.LeftOne(i))!=-1) 
  {
    if(AreOrthog(i,tv)) bvR.SetBitAt(i,0);
    else if(IsCover(i, tv)) return tv;
  }
  if(!l||bvR.IsZero()) return tv;
  CBV bvX,  bvRW=bvR;     //bvR((BYTE)1,l);
  TmW.Add(tv); 
  BmR.Add(bvR); 
  for (i=0; i<TmW.GetCountR(); i++)
  {
    tv=TmW.GetRowTv(i);
    bvR=BmR.GetRowBv(i);    //stv=tv.BitChar();   //sbvR=bvR.BitChar();
    q=0;
    while (!bvR.IsZero()) 
    {
      if(w) 
      {
        if(tv1.IsCover(tv)) { q=1; break;}
      }
      c=n+1; q=0;j=-1;
      while ((j=bvR.LeftOne(j))!=-1) 
      {
        if(IsCover(j,tv)) {q=1; break;}
        if((b=CountDefs(tv.GetUnDefs(),j))<c) {c=b; d=j;}
      }
      if(q || (bvX=GetRowDef(d,tv.GetUnDefs())).IsZero()) break;
      p=-1; k=-1; 
      while((k=bvX.LeftOne(k))!=-1)
        if ((a=Tm_T.CountDefs(bvR,k))>=p) 
        {
          b=Tm_T.CountOnes(bvR,k); c=a-b;
          if ((a>p)||(b>u)||(c>u)) 
          { v=k; p=a; u=(b<c)? c:b; s=(b<c)? '0':'1'; } 
        }
      tv.SetBitAt(v, s); 
      bvRW = Tm_T.GetRowUnDef(v)|((s=='1') 
             ? Tm_T.GetRowOne(v) : Tm_T.GetRowZero(v));
      bvRW&=bvR;
      TmW.Add(tv);
      BmR.Add(bvRW);          //stv=tv.BitChar(); //sbvRW=bvRW.BitChar();
      tv.SetBitAt(v,((s-'0')^'1')); 
      bvR &= Tm_T.GetRowUnDef(v)|((s=='1')
             ? Tm_T.GetRowZero(v) : Tm_T.GetRowOne(v));          //stv=tv.BitChar(); //sbvR=bvR.BitChar();
    }
    if(!q) 
    {
      if(!w) {tv1=tv; w=1;}
      else tv1=tv.MinCover(tv1);         //stv1=tv1.BitChar();
      if (tv1==tv_) return tv1;
    }
  }      //stv1=tv1.BitChar();
  return (tv1);
}

//_____________________________________________________________
// Расширение интервала перед включением его в ДНФ,
// инверсную заданной, с удалением поглощаемых интервалов.
//_____________________________________________________________

void CTM::AdExpans(CTM &Tm0, CTV &tv, CBV &bvC, CBV &bvW)
{
  int k, a;
  CTV tvC=tv; tvC.SetUnDefs(bvC);  k=-1;
    //CString sTm1, sTm0, sTmW, sBmR, sBmS, sBmC, sTm1_T, 
    //stv, stvC, sbvC, sbvW, sbvD, sbvY, sbvR, sbvX, sbvRW, sbvS, sbvSW;
    //sTm1=Tm1.BitChar();    //sTm0=Tm0.BitChar();
    //stv=tv.BitChar();//sbvC=bvC.BitChar();//sbvW=bvW.BitChar();
  while (!bvC.IsZero() && ((k=bvW.LeftOne(k))!=-1))
    if(!AreOrthog(k,tvC))
    {
      a=Intersec(bvC,k,tvC).Orthogon(tv).LeftOne(-1);
      tvC.SetBitAt(a, tv.GetBitAt(a));
    }          //stv=tv.BitChar();//sbvC=bvC.BitChar();
  for (k=0, a=1; k<Tm0.GetCountR(); k++)
  {
    if (Tm0.IsCover(k, tvC)) {a=0; break;}
    else if (Tm0.IsCovered(k, tvC))
    {
      if (a) {Tm0.SetRowTv(k,tvC); a=0;}
      else {Tm0.RemoveRow(k); k--;}
    }
  }
  if (a) Tm0.Add(tvC);
}

