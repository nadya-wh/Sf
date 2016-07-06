/////////////////////////////////////////////////////////////////////
// Ver.2.1.0    07.07.2006
//               Переход к LINUX.

/////////////////////////////////////////////////////////////////
// Ver.2.0.9     06.05.2006
//             Добавлен метод InverSR - "раздельная инверсия"
// Ver.2.0.3     21.03.2006
//             Устранение ошибок конструктора по размерам
// Ver.2.0.0     05.03.2006   Russian == English
/////////////////////////////////////////////////////////////////////
//1  CSOP CSOP::InverS(int simpl)
//2  CSOP CSOP::InverSR (int simpl)
//3  CSOP CSOP::InverS(int simpl, CBV &bvF)
//4  CSOP CSOP::InverS(int simpl,CTV tv,CBV bv,CBV bvR)
//5  int CSOP::Differ(CTV &tv1, CBV &bv1, CBV bvR)
//6  void CSOP::AdExpans(CSOP &Sop0, CTV &tv, CBV &bv, CBV &bvC, CBV &bvW)
//7  int CSOP::DelOrBT(CTV &tv, CBV &bv, CBV &bvR)
//8  void CSOP::DorD(int q, CSOP Sb1)
//9  void CSOP::DorD(int q, CTV tva, CBV bva)


#ifdef _JOINT
#include "Sop.h"
#else
#include "../Common/Sop.h"
#endif

// #ifdef _DEBUG
// #undef THIS_FILE
// static char BASED_CODE THIS_FILE[] = __FILE__;
// #endif

/////////////////////////////////////////////////////////////////////
// CSOP

/////////////////////////////////////////////////////////////////////
// Инверсия системы ДНФ 
//   simpl - параметр упрощения возвращаемой системы ДНФ
// simpl=0 - без упрощения получаемой системы, представленной
//           множеством взаимно-ортогональных интервалов;
// simpl=1 - упрощение путем расширения и поглащения интервалов;
// simpl=2 - упрощение путем обобщенного склеивания интервалов;
// simpl=3 - упрощение с комбинацией режимов simpl=1 и simpl=2.
//-------------------------------------------------------------------

CSOP CSOP::InverS(int simpl)
{
  CBV bv(1, GetnFun()), bvR(1, GetnCon());
  CTV tv(GetnArg(), '-');
  return InverS (simpl,tv,bv,bvR);
}

/////////////////////////////////////////////////////////////////////
// Раздельная инверсия ДНФ в системе Sop1 с параметрическим
// управлением качеством упрощения возвращаемой Sop0.
//   simpl - параметр упрощения возвращаемой системы ДНФ:
// simpl=0 - без упрощения,
// simpl=1 - с приведением подобных интервалов,
// simpl=2 - с обобщенным склеиванием и локальным поглощением,
// simpl=3 - с обобщенным склеиванием и глобальным поглощением.
//-------------------------------------------------------------------

CSOP CSOP::InverSR (int simpl)
{
  int e, i, j, k;
  CSOP Sop0;
  CTM Tm0;
  CTV tv(GetnArg(),'-');
  CBV bvF((BYTE)0, GetnFun());
  for(i=0; i<GetnFun(); i++)
  { 
    Tm0=m_tmCon.InverS(0, GetConFun(i));
    for(j=0; j<Tm0.GetCountR(); j++)
    { 
      tv=Tm0.GetRowTv(j); bvF.SetBitAt(i, 1);
      if(simpl!=1) Sop0.DorD(simpl,tv, bvF); 
      else
      {
        e=1;
        for(k=0,e=1; k<Sop0.GetnCon(); k++)
        {
          if(Sop0.m_tmCon.AreEqual(k,tv)) 
          {Sop0.m_bmFun.SetBitAt(k, i, 1); e=0; break;}
        }
        if(e) Sop0.AddCon(tv, bvF);
      }    
      bvF.SetBitAt(i, 0);
    }
  }
  return Sop0;
}

/////////////////////////////////////////////////////////////////////
// Инверсия выделенных вектором bvF функций в системе ДНФ this
//   simpl - параметр упрощения возвращаемой системы ДНФ
// simpl=0 - без упрощения получаемой системы, представленной
//           множеством взаимно-ортогональных интервалов;
// simpl=1 - упрощение путем расширения и поглащения интервалов;
// simpl=2 - упрощение путем обобщенного склеивания интервалов;
// simpl=3 - упрощение с комбинацией режимов simpl=1 и simpl=2.
//-------------------------------------------------------------------

CSOP CSOP::InverS(int simpl, CBV &bvF)
{
  CTV tv(GetnArg(), '-');
  CBV bv=bvF, bvR(1, GetnCon());
  return InverS (simpl,tv,bv,bvR);
}

/////////////////////////////////////////////////////////////////////
// Вычитание из F-интервала (tv, bv) системы ДНФ this по строковой маске bvR
// (через магазин, с выбором минимальной строки и максимального столбца)
//   simpl - параметр упрощения возвращаемой системы ДНФ:
// simpl=0 - без упрощения получаемой системы, представленной
//           множеством взаимно-ортогональных интервалов;
// simpl=1 - упрощение путем расширения и поглащения интервалов;
// simpl=2 - упрощение путем обобщенного склеивания интервалов;
// simpl=3 - упрощение с комбинацией режимов simpl=1 и simpl=2.
//-------------------------------------------------------------------

CSOP CSOP::InverS(int simpl,CTV tv,CBV bv,CBV bvR)
{
  int i, j, k, p, q, u, v, a, b, c, d, e,
  n=GetnArg(), m=GetnFun(), l=GetnCon();
      //CString ZZsSop1, ZZsSop0, ZZsSopW, ZZsBmR, ZZsBmS, ZZsBmC, ZZsbvW, ZZsbvR, 
      //ZZsbvC, ZZsbv, ZZsbvF, ZZstv, ZZsbvD, ZZsbvSW, ZZsbvRW, ZZsbvS, ZZsbvQ;
      //ZZsSop1=Sop1.ToChar();//ZZstv=tv.BitChar();//ZZsbv=bv.BitChar();//ZZsbvR=bvR.BitChar();
  CSOP Sop0(n, m, 0); 
  char s; 
  if(DelOrBT(tv, bv, bvR)==3) return Sop0;
  CBV bvW=bv, bvX(1, n), bvRW=bvR, bvS=bvR, bvSW=bvR, bvZ=bvR;
  CTV tvC=tv;
  CSOP SopW(n, m, 0);
  if(!l||bvR.IsZero()) {Sop0.AddCon(tv,bv); return Sop0;}  
  CBM BmR(l,l), BmS(l,l); BmR.SetSize(0,l); BmS.SetSize(0,l);
  CTM Sop1_T(GetConTM(), 0);
  CBM Sop1_BT(GetFunBM(), 0);
  SopW.AddCon(tv, bv);
  BmR.Add(bvR); CBM BmC(l,n); BmC.SetSize(0,n); 
  CBV bvC(0,n,1), bvD(0,n,1), bvY(0,n,1);
  if(simpl&1) {BmS.Add(bvS); BmC.Add(bvD);}
  while((i=SopW.GetnCon()-1)>-1) 
  {
    tv=SopW.m_tmCon.GetRowTv(i);
    bv=SopW.m_bmFun.GetRowBv(i); 
    bvR=BmR.GetRowBv(i);
    SopW.RemoveR(i);
    BmR.RemoveRow(i);
    if(simpl&1) 
    {
      bvC=BmC.GetRowBv(i); BmC.RemoveRow(i);  
      bvS=BmS.GetRowBv(i); BmS.RemoveRow(i);
    }        //ZZstv=tv.BitChar();//ZZsbv=bv.BitChar();//ZZsbvR=bvR.BitChar();//ZZsbvC=bvC.BitChar();//ZZsbvS=bvS.BitChar();
    q=0;
    while (!bvR.IsZero()) 
    {
      q=0;j=-1;
      while ((j=bvR.LeftOne(j))!=-1)
        if(m_tmCon.IsCover(j, tv)) 
        {
          if((bv-=m_bmFun.GetRowBv(j)).IsZero()) {q=1; break;}
          bvR.SetBitAt(j,0); if(simpl&1) bvS.SetBitAt(j,0);
          bvZ.Zero(); k=-1;
          while((k=bv.LeftOne(k))!=-1) bvZ|=Sop1_BT[k];
          bvR&=bvZ; if(simpl&1) bvS&=bvZ;               //ZZstv=tv.BitChar();//ZZsbv=bv.BitChar();//ZZsbvR=bvR.BitChar();
        }
      if(q || bvR.IsZero()) break;
      e=n+1; j=-1; bvX=tv.GetUnDefs();
      while((j=bvR.LeftOne(j))!=-1) 
        if((b=m_tmCon.CountDefs(bvX,j))<e) 
        { e=b; d=j; if(e==1) break;}
      if(e==1) 
      {
        v=m_tmCon.LeftDef(d,-1,bvX); 
        s=m_tmCon.GetBitAt(d,v);
      }
      else 
      {
        bvX&=m_tmCon.GetRowDef(d);
        p=-1; k=-1; //q=0;
        while ((k=bvX.LeftOne(k))!=-1)
          if ((a=Sop1_T.CountDefs(bvR,k))>=p) 
          {
            b=Sop1_T.CountOnes(bvR,k); c=a-b;
            if ((a>p)||(b>u)||(c>u)) 
            { v=k; p=a; u=(b<c)? c:b; s=(b<c)? '0':'1'; }
          }
        if(p<1) break;
      }
      bvRW=bvR; bvW=bv; 
      if(simpl&1) {bvD=bvC; bvSW=bvS;}
      tv.SetBitAt(v, s); 
      bvR = Sop1_T.GetRowUnDef(v)|((s=='1')
            ? Sop1_T.GetRowOne(v) : Sop1_T.GetRowZero(v));
      if(simpl&1) bvC.SetBitAt(v,1);
      bvR&=bvRW;
      SopW.AddCon(tv, bv); BmR.Add(bvR);
      if(simpl&1) {BmC.Add(bvC); BmS.Add(bvS);}           //ZZsSopW=SopW.ToChar();//ZZsBmR=BmR.BitChar();//ZZsBmC=BmC.BitChar();//ZZsBmS=BmS.BitChar();
      bv=bvW; 
      tv.SetBitAt(v,((s-'0')^'1')); 
      bvR = Sop1_T.GetRowUnDef(v)|((s=='1')
            ? Sop1_T.GetRowZero(v) : Sop1_T.GetRowOne(v));
      if(simpl&1) {bvS=bvSW & bvR; bvC=bvD;}
      bvR&=bvRW;             //ZZstv=tv.BitChar();//ZZsbv=bv.BitChar();//ZZsbvR=bvR.BitChar();//ZZsbvC=bvC.BitChar();//ZZsbvS=bvS.BitChar();
    }
    if(!q && bvR.IsZero()) 
    {
      if(simpl&1) AdExpans(Sop0,tv,bv,bvC,bvS);
      else Sop0.AddCon(tv, bv); 
    }
  }
  if(simpl &2) { SopW.DorD(simpl, Sop0); return SopW;} //ZZsSop0=Sop0.ToChar();
  return Sop0;
}

/////////////////////////////////////////////////////////////////////
// Урезание F-интервала (tv,bv) из множества this до минимально возможного,
// покрывающего, однако, все его собственные существенные элементы 
// (через вычитание из (tv,bv) this по строковой маске bvR).
// Функция возвращает значение 
// 1, если F-интервал (tv,bv) урезан, 
// 0, если он урезан до пустого и
// 2, если он не изменился. 
//-------------------------------------------------------------------

int CSOP::Differ(CTV &tv1, CBV &bv1, CBV bvR)
{
  int i, j, k, p, q, u, v, a, b, c, d, e, w=0,
  n = GetnArg(), m = GetnFun(), l = GetnCon();
    //CString ZZsSop1, ZZsSop0, ZZsSopW, ZZsBmR, ZZsBmS, ZZsBmC, ZZsbvW, ZZsbvR, 
    //ZZsbvC, ZZsbv, ZZsbvF, ZZstv, ZZsbvD, ZZsbvSW, ZZsbvRW, ZZsbvS, ZZsbvQ;
    //ZZsSop1=Sop1.ToChar();//ZZstv=tv.BitChar();  //ZZsbv=bv.BitChar();//ZZsbvR=bvR.BitChar(); //CSOP Sop0(n, m, 10); 
  char s; 
  if(DelOrBT(tv1, bv1, bvR)==3) return 0;
  if(!l||bvR.IsZero()) return 1;
  CBV bv_=bv1, bv=bv1, bvW, bvX, bvRW=bvR; bv1.Zero();
  CTV tv_=tv1, tv=tv1; 
  CSOP SopW(n, m, 0); SopW.AddCon(tv, bv);
  CBM BmR(l,l); BmR.SetSize(0,l); BmR.Add(bvR); 
  CBV bvSW=bvR, bvZ=bvR;
  CTM Sop1_T(GetConTM(), 0);
  CBM Sop1_BT(GetFunBM(), 0);
  SopW.AddCon(tv, bv);
  BmR.Add(bvR); 
  for (i=0; i<SopW.GetnCon(); i++) //"Половинка" из магазина 
  {                                
    tv=SopW.m_tmCon.GetRowTv(i);
    bv=SopW.m_bmFun.GetRowBv(i); 
    bvR=BmR.GetRowBv(i);
    q=0;
    while (!bvR.IsZero()) 
    {
      if(tv1.IsCover(tv) && !(bv&bv1).IsZero()) 
      {
        bv-=bv1; d=1;
        if(bv.IsZero()) {q=1; break;}
      }
      q=0;j=-1;
      while ((j=bvR.LeftOne(j))!=-1)
        if(m_tmCon.IsCover(j, tv)) 
        {
          if((bv-=m_bmFun.GetRowBv(j)).IsZero()) {q=1; break;}
          bvR.SetBitAt(j,0); 
          bvZ.Zero(); k=-1;
          while((k=bv.LeftOne(k))!=-1) bvZ|=Sop1_BT[k];
            bvR&=bvZ;             //ZZstv=tv.BitChar();//ZZsbv=bv.BitChar();//ZZsbvR=bvR.BitChar();
        }
      if(q || bvR.IsZero()) break;
      e=n+1; j=-1; bvX=tv.GetUnDefs();
      while((j=bvR.LeftOne(j))!=-1) 
        if((b = m_tmCon.CountDefs(bvX,j))<e) 
        { e=b; d=j; if(e==1) break;}
      if(e==1) 
      {
        v = m_tmCon.LeftDef(d,-1,bvX); 
        s = m_tmCon.GetBitAt(d,v);
      }
      else 
      {
        bvX &= m_tmCon.GetRowDef(d);
        p=-1; k=-1; //q=0;
        while ((k=bvX.LeftOne(k))!=-1)
          if ((a=Sop1_T.CountDefs(bvR,k))>=p) 
          {
            b=Sop1_T.CountOnes(bvR,k); c=a-b;
            if ((a>p)||(b>u)||(c>u)) 
            { v=k; p=a; u=(b<c)? c:b; s=(b<c)? '0':'1'; } 
          }
        if(p<1) break;
      }
      bvRW=bvR; bvW=bv; 
      tv.SetBitAt(v, s); 
      bvR = Sop1_T.GetRowUnDef(v)|((s=='1')
            ? Sop1_T.GetRowOne(v) : Sop1_T.GetRowZero(v));
      bvR&=bvRW;
      SopW.AddCon(tv, bv); BmR.Add(bvR);
      bv=bvW; 
      tv.SetBitAt(v,((s-'0')^'1')); 
      bvR = Sop1_T.GetRowUnDef(v)|((s=='1')
            ? Sop1_T.GetRowZero(v) : Sop1_T.GetRowOne(v));
      bvR&=bvRW;
    }
    if(!q) 
    {
      if(!w) {tv1=tv; bv1=bv; w=1;}
      else {tv1=tv.MinCover(tv1); bv1|=bv;} 
      if ((tv1==tv_) && (bv1==bv_)) return 2; //Интервал не урезан 
    }
  }
  return w;    //Sop0;
}

/////////////////////////////////////////////////////////////////////
// Расширение F-интервала перед включением его в систему ДНФ,
// инверсную заданной, с удалением поглощаемых F-интервалов.
//-------------------------------------------------------------------

void CSOP::AdExpans(CSOP &Sop0, CTV &tv, CBV &bv, CBV &bvC, CBV &bvW)
{
  int k, a;
  CTV tvC=tv; tvC.SetUnDefs(bvC); CBV bvF;
      //CString ZZtv, ZZtvC, ZZbv, ZZbvC, ZZbvW, ZZSop1, ZZtvZ, ZZbvY;
      //ZZSop1=Sop1.ToChar();//ZZtv=tv.BitChar();//ZZbv=bv.BitChar();//ZZbvC=bvC.BitChar();//ZZbvW=bvW.BitChar();
  tvC=tv; tvC.SetUnDefs(bvC); k=-1;            //ZZtvC=tvC.BitChar();
  while (!bvC.IsZero() && ((k=bvW.LeftOne(k))!=-1))
    if(!m_tmCon.AreOrthog(k,tvC)
        && !m_bmFun.GetRowBv(k,bv).IsZero())
    { 
      a = m_tmCon.Intersec(bvC,k,tvC).Orthogon(tv).LeftOne(-1);
      tvC.SetBitAt(a, tv.GetBitAt(a));
      bvC.SetBitAt(a,0);
    }
        //ZZtv=tv.BitChar();//ZZbv=bv.BitChar();//ZZbvC=bvC.BitChar();
  for (k=0, a=1; k<Sop0.GetnCon(); k++)
  {
    if (Sop0.m_tmCon.IsCover(k, tvC))
    {
      if ((bv-=Sop0.m_bmFun.GetRowBv(k)).IsZero())   {a=0; break;}
      continue;
    }
    if (Sop0.m_tmCon.IsCovered(k, tvC))
    {
      if (!(bvF=Sop0.m_bmFun.GetRowBv(k)-bv).IsZero())
      {Sop0.m_bmFun.SetRow(k,bvF); continue;}
      if(a) 
      {
        Sop0.m_tmCon.SetRowTv(k,tvC); 
        Sop0.m_bmFun.SetRow(k,bv); a=0;
      }
      else {Sop0.RemoveR(k); k--;}
    }
  }
  if (a) Sop0.AddCon(tvC, bv);
}

/////////////////////////////////////////////////////////////////////
// В заданной системе ДНФ this выделно некоторое  подмножество конъюнкций bvR.
// Если среди них есть конъюнкции (tv1,bv1), покрывающие конъюнкцию (tv),
// то bv-=bv1, и если при этом bv станет пустым, то возвращается значение q=3 
// (значение bvR при этом не определено), в противном случае из bvR удаляются
// конъюнкции, ортогональные (tv,bv) и возвращается значение 
// q=0, если ни bvR, ни bv не изменились,
// q=1, если изменился только bvR, 
// q=2, если изменились и bvR и bv.
//-------------------------------------------------------------------

int CSOP::DelOrBT(CTV &tv, CBV &bv, CBV &bvR)
{
  int i=-1, j=-1, q=0;
   CBV bv1;
  while((i=bvR.LeftOne(i))!=-1)  // Удаление ортогональных конЪюнкций
  {
    if(m_tmCon.AreOrthog(i,tv) || 
       (bv1 = m_bmFun.GetRowBv(i,bv)).IsZero())
    { bvR.SetBitAt(i,0); q=1;}
    else  // Ортогонализация bv
    {
      if(m_tmCon.IsCover(i, tv)) 
      {
        bv-=bv1;
        if(bv.IsZero()) {q=3; break;}
        bvR.SetBitAt(i,0); q=2;
      }
    }
  }
  if(q==2)  // Коррекция bvR
  {
    while ((j=bvR.LeftOne(j))!=-1)
      if(m_bmFun.GetRowBv(j,bv).IsZero())  bvR.SetBitAt(j, 0); 
  } 
  return q;
}

/////////////////////////////////////////////////////////////////////
// Дизъюнкция двух систем ДНФ с параметрическим управлением
// качеством упрощения 
// q=0 - без упрощения,
// q=1 - с поглощением интервалов,
// q=2 - с обобщенным склеиванием и локальным поглощением,
// q=3 - с обобщенным склеиванием и глобальным поглощением.
//-------------------------------------------------------------------
void CSOP::DorD(int q, CSOP Sb1)
{
  int a, d, e, i, j, k;
  if(q<1)
  {
    for(i=0; i<Sb1.GetnCon(); i++)
      if(!Sb1.m_bmFun.GetRowBv(i).IsZero())
        AddCon(Sb1.m_tmCon.GetRowTv(i),Sb1.m_bmFun.GetRowBv(i));
    return;
  }
      //CString sSb1, sSb2, stva, stvc, sbva, sbvc, sbvd, sbv, sbv1;//sSb1=Sb1.ToChar();//sSb2=Sb2.ToChar();
  CTV tva, tvc; CBV bva, bvc, bvd;
  while (k=Sb1.GetnCon())
  {
    tva=Sb1.m_tmCon.GetRowTv(k-1);
    bva=Sb1.m_bmFun.GetRowBv(k-1);        //sSb1=Sb1.ToChar();//sSb2=Sb2.ToChar();
    Sb1.RemoveR(k-1);                   //stva=tva.BitChar();//sbva=bva.BitChar();
    if(bva.IsZero()) continue;
    for(i=0, e=1; i<GetnCon(); i++)
    {
      d=0; 
      if(m_tmCon.IsCover(i,tva)) d+=1;
      if(m_tmCon.IsCovered(i,tva)) d+=2;
      if (d>1)
      {
        if (d==3) bva|=m_bmFun.GetRowBv(i); 
        else 
        {
          if(!(bvd=(m_bmFun.GetRowBv(i)-bva)).IsZero())
          {
            m_bmFun.SetRow(i, bvd);       //sbva=bva.BitChar();//sSb2=Sb2.ToChar();
            continue;
          }
        }
        RemoveR(i); i--;
      }
      else
      {
        if (d) 
        {
          if((bva-=m_bmFun.GetRowBv(i)).IsZero())
          {e=0; break;} continue;
        }
        if(q<2) continue;
        a=0;
        if(!(bvc=m_bmFun.GetRowBv(i, bva)). IsZero()
            && m_tmCon.AreConcens(i,tva))
        {
          tvc=m_tmCon.Concens(i,tva); 
          if (tvc.IsCover(tva)&&(bvc==bva)) a+=1;
          if(m_tmCon.IsCovered(i,tvc)&&(m_bmFun.GetRowBv(i)==bvc)) 
            a+=2;
          if(a%2) {tva=tvc; if(a>1) RemoveR(i); i=-1;}
          else if (a) {Sb1.AddCon(tvc, bvc); RemoveR(i); i--;}
          else  
          {
            if(q<3) continue;
            for(j=0; j<GetnCon()&&j!=i; j++)
            {
              if(m_tmCon.IsCover(j,tvc))
              {
                if(m_bmFun.GetRowBv(j)>bvc) break;
              }
              if(m_tmCon.IsCovered(j,tvc)) 
              {
                if(!(bvd=(m_bmFun.GetRowBv(j)-bvc)).IsZero())
                  m_bmFun.SetRow(j, bvd);
                else {RemoveR(j); if(j<i) i--; j--;}
              }
            }
          }
        }
      }                        //sSb1=Sb1.ToChar();  //sSb2=Sb2.ToChar();
    }
    if(e) AddCon(tva, bva);                          //sSb2=Sb2.ToChar();
  }
}


/////////////////////////////////////////////////////////////////////
// Дизъюнкция F-интервала (tv,bv) и системы ДНФ this 
// с параметрическим управлением качеством упрощения 
// q=0 - без упрощения,
// q=1 - с поглощением интервалов,
// q=2 - с обобщенным склеиванием и локальным поглощением,
// q=3 - с обобщенным склеиванием и глобальным поглощением.
//-------------------------------------------------------------------

void CSOP::DorD(int q, CTV tva, CBV bva)
{
  int a, d, e, i, j;
  if(q<1) { AddCon(tva,bva); return;}
          //CString sSop, stva, stvc, sbva, sbvc, sbvd, sbv, sbv1;    //sSop=Sop.ToChar();
  CTV tvc; CBV bvc, bvd;    //sSop=Sop.ToChar();//stva=tva.BitChar();//sbva=bva.BitChar();
  if(bva.IsZero()) return;
  for(i=0, e=1; i<GetnCon(); i++)
  {
    d=0; 
    if(m_tmCon.IsCover(i,tva)) d+=1;
    if(m_tmCon.IsCovered(i,tva)) d+=2;
    if (d>1)
    {
      if (d==3) bva|=m_bmFun.GetRowBv(i); 
      else 
      {
        if(!(bvd=(m_bmFun.GetRowBv(i)-bva)).IsZero())
        {
          m_bmFun.SetRow(i, bvd); //sbva=bva.BitChar();//sSop=Sop.ToChar();
          continue;
        }
      }
      RemoveR(i); i--;
    }
    else
    {
      if (d) 
      {
        if((bva-=m_bmFun.GetRowBv(i)).IsZero())
        {e=0; break;} 
        continue;
      }
      if(q<2) continue;
      a=0;
      if(!(bvc=m_bmFun.GetRowBv(i, bva)). IsZero()
           && m_tmCon.AreConcens(i,tva))
      {
        tvc=m_tmCon.Concens(i,tva); 
        if (tvc.IsCover(tva)&&(bvc==bva)) a+=1;
        if(m_tmCon.IsCovered(i,tvc)&&(m_bmFun.GetRowBv(i)==bvc)) 
          a+=2;
        if(a%2) {tva=tvc; if(a>1) RemoveR(i); i=-1;}
        else if (a) {tva=tvc; bva=bvc; RemoveR(i); i--;}
        else  
        {
          if(q<3) continue;
          for(j=0; j<GetnCon()&&j!=i; j++)
          {
            if(m_tmCon.IsCover(j,tvc))
            {
              if(m_bmFun.GetRowBv(j)>bvc) break;
            }
            if(m_tmCon.IsCovered(j,tvc)) 
            {
              if(!(bvd=(m_bmFun.GetRowBv(j)-bvc)).IsZero())
                m_bmFun.SetRow(j, bvd);
              else {RemoveR(j); if(j<i) i--; j--;}
            }
          }
        }
      }
    }                 //sSb1=Sb1.ToChar();       //sSop=Sop.ToChar();
  }
  if(e) AddCon(tva, bva);                        //sSop=Sop.ToChar();
}
