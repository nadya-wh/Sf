/////////////////////////////////////////////////////////////////
// Ver.2.1.1    02.10.2007
//               поправка для вызова функции Degen с предварительным образованием аргусента-вектора
// Ver.2.1.0    07.07.2006
//               Переход к LINUX.

/////////////////////////////////////////////////////////////////
// Ver.2.0.6     03.04.2006
//             в CrossI - дополнительная проверка на пустоту одного из операндов
// Ver.2.0.3     21.03.2006
//             Устранение ошибок конструктора по размерам
// Ver.2.0.0     17.03.2006   Russian == English

/////////////////////////////////////////////////////////////////
//1  int NoEquiv_D_R(CSOP &Sop_R);
//2  int NoEquiv_Net(CSOP &Sop2);
//3  int NoEquiv_Net(CBV &bvXY1, CSOP &Sop2, CBV & bvXY2);
//4  int NoEquiv(CSOP &Sop2);
//5  int NoEquiv(CSOP &Sop2_1, CSOP &Sop2_0);
//6  int NoEquiv(CBV &bvP, CSOP &Sop2);
//7  int NoEquiv(CBV &bvP, CSOP &Sop2_1, CSOP &Sop2_0);

//8  int NoImpl(CSOP &Sop2);
//9  int NoImpl(CTV &tv, CBV &bv);
//10 int NoImpl(CTV &tv, CBV &bv, CBV &bvR);
//11 int NoImpl(CBV &bvP, CSOP &Sop1);
//12 int CrossI(CSOP &Sop2);
//13 void DivPolD(CBV &bv0, CSOP &Sop1, CSOP &Sop0);

#ifdef _JOINT
#include "Sop.h"
#else
#include "../Common/Sop.h"
#endif

// #ifdef _DEBUG
// #undef THIS_FILE
// static char BASED_CODE THIS_FILE[] = __FILE__;
// #endif

//______________________________________________________
// Проверка реализуемости системы совершенных ДНФ this 
// частичных булевых функций системой РНФ Sop_R 
// (системой полиномов Жегалкина или Рида-Маллера) 
// Возвращается
// z=0,  если отношение выполняется,
// z=-1, если системы ДНФ несоразмерны,
// z>0,  если (z-1)-я функция не реализуется.
//______________________________________________________
 
int CSOP::NoEquiv_D_R (CSOP &Sop_R)
{
  int i, j, k, n = GetnArg(), m = GetnFun(),
               p = GetnCon(), q=Sop_R.GetnCon();
        //CString sSop_D,sSop_R; //sSop_D=Sop_D.ToChar(); //sSop_R=Sop_R.ToChar();  //CString Error; char buf[120];
  if (Sop_R.GetnArg()!=n || Sop_R.GetnFun()!=m) return -1;
  CBM BmRFT(Sop_R.GetFunBM(),0);
  CBM BmDCT(GetConTM().GetOnes(),0); 
  CBM BmDFT(GetFunBM(),0);
  CBV bvE((BYTE)1, p), bvF=bvE, bvG=bvE;
  for(i=0;i<m;i++) {
    j=-1; bvF=BmDFT.GetRow(i);
    while((j=BmRFT.LeftOne(i,j))!=-1) {
      k=-1; bvG=bvE;
      while((k=Sop_R.m_tmCon.LeftDef(j,k))!=-1)
        if(Sop_R.m_tmCon.GetBitAt(j,k)=='1') 
          bvG&=BmDCT.GetRow(k);
        else
          bvG&=~BmDCT.GetRowBv(k);
      bvF^=bvG;
    }
    if(!bvF.IsZero()) return i+1;
  }
  return 0;
}

//***************************************************
// Проверка эквивалентности двух сетей в ДНФ this и Sop.
// Возвращается 
// z=0, если сети эквивалентны,
// z=-1, если this и Sop несоразмерны,  
// z>0 , если (z-1)-е функции не эквивалентны.
//***************************************************

int CSOP::NoEquiv_Net(CSOP &Sop)
{
  int i, y1=GetnInterVar(), y2=Sop.GetnInterVar(), 
      n=(GetnArg()-y1), m=(GetnFun()-y1); 
  if (n!=Sop.GetnArg()-y2 || m!=Sop.GetnFun()-y2) return -1;
  CTM Tm; 
  MTM MTm1, MTm2; 
  MTm1.resize(m+y1, Tm); 
  MTm2.resize(m+y2, Tm); 
  CBM BmA1(m+y1,n+y1), BmA2(m+y2,n+y2), BmK1, BmK2;
  CTM Tm1(10,n), Tm2(10,n);
  CBV bvX((BYTE)1,n), bvXY1((BYTE)1,n+y1), bvXY2((BYTE)1,n+y2),
      bvFY1((BYTE)1,m+y1), bvFY2((BYTE)1,m+y2);
  if(y1) 
  {
    bvXY1=bvXY1<<y1; bvFY1=bvFY1<<y1; 
    BmA1=ForMA_V (bvXY1);
    BmK1=ForMK_V (BmA1, bvXY1);
    ForLDNF_V(bvFY1, BmA1, BmK1, MTm1);
  }
  if(y2) 
  {
    bvXY2=bvXY2<<y2; bvFY2=bvFY2<<y2;
    BmA2=Sop.ForMA_V (bvXY2);
    BmK2=Sop.ForMK_V (BmA2, bvXY2);
    Sop.ForLDNF_V (bvFY2, BmA2, BmK2, MTm2);
  }
  for(i=0; i<m; i++)
  {
    if(y1) Tm1=MapLog_ToFi(MTm1, BmA1, y1, i);
    else Tm1=m_tmCon.Minor(m_bmFun.GetColumnBv(i),bvX);
    if(y2) Tm2=MapLog_ToFi(MTm2, BmA2, y2, i);
    else Tm2=Sop.m_tmCon.Minor(Sop.m_bmFun.GetColumnBv(i),bvX);
    if(Tm1.NoEquiv(Tm2)) return i+1;
  }
  return 0;
}

//__________________________________________________________
// Проверка эквивалентности двух сетей в ДНФ this и 
// Sop2 с векторным заданием распределения переменных
// bvXY1 и bvXY2. (С использованием весов подставляемых ДНФ)
//__________________________________________________________

int CSOP::NoEquiv_Net(CBV &bvXY1, CSOP &Sop2, CBV & bvXY2)

{
  int i, i1, i2, m, n, nn1, y1, yy1, nn2, y2, yy2;
  if ((n=bvXY1.CountBit())!=bvXY2.CountBit() ||
      (m=(GetnFun()-(y1=(~bvXY1).CountBit()))) !=
      (Sop2.GetnFun()-(y2=(~bvXY2).CountBit()))) 
    return -1;
  CTM Tm; 
  MTM MTm1, MTm2; 
  MTm1.resize(m+y1, Tm); 
  MTm2.resize(m+y2, Tm); 
  CBV bvFY1, bvFY2, bvX((BYTE)1,n);
  CBM BmA1, BmA2, BmK1, BmK2;
  CTM Tm1, Tm2; 
  if(y1) 
  {
    nn1=(~bvXY1).LeftOne(-1); yy1=GetnArg()-nn1;
    bvFY1.SetSize(GetnFun()-yy1); bvFY1.One();
    bvFY1.Concat(bvXY1.Extract(nn1,yy1));
    BmA1= ForMA_V (bvXY1);
    BmK1= ForMK_V (BmA1, bvXY1);
    ForLDNF_V(bvFY1, BmA1, BmK1, MTm1);
  }
  if(y2) 
  {
    nn2=(~bvXY2).LeftOne(-1); yy2=Sop2.GetnArg()-nn2;
    bvFY2.SetSize(Sop2.GetnFun()-yy2); bvFY2.One();
    bvFY2.Concat(bvXY2.Extract(nn2, yy2));
    BmA2= Sop2.ForMA_V(bvXY2);
    BmK2= Sop2.ForMK_V (BmA2, bvXY2);
    Sop2.ForLDNF_V(bvFY2, BmA2, BmK2, MTm2);
  }
  for(i=0, i1=i2=-1; i<m; i++)
  {
    if(!y1) Tm1=m_tmCon.Minor(m_bmFun.GetColumnBv(i),bvX);
    else 
    {
      i1=bvFY1.LeftOne(i1);
      Tm1=MapLog_ToFi(MTm1, BmA1, bvXY1, i1);
    }
    if(!y2) Tm2=Sop2.m_tmCon.Minor(Sop2.m_bmFun.GetColumnBv(i),bvX);
    else 
    {
      i2=bvFY2.LeftOne(i2);
      Tm2=MapLog_ToFi(MTm2, BmA2, bvXY2, i2);
    }
    if(Tm1.NoEquiv(Tm2)) return i+1;
  }
  return 0;
}

//___________________________________________________
// Проверка отношения this == Sop2 (через Differ). 
// Возвращается 
// z=0, если this == Sop2,
// z=-1, если this и Sop2 несоразмерны,  
// z<-1, если (|z|-2)-я строка this не имплицирует Sop2,
// z>0 , если (z-1)-я строка Sop2 не имплицирует this.
//___________________________________________________

int CSOP::NoEquiv(CSOP &Sop2)
{
  int z;
  if (z= NoImpl(Sop2)) return ((z==-1)?z:z=-z-1);
  return Sop2.NoImpl(*this);
}

//______________________________________________________
// Проверка отношения реализуемости системой ДНФ Sop1
// системы частичных булевых функций (Sop2_1, Sop2_0)
// по правилу (Sop2_1 -> this) && (Sop2_0 & this = 0).
// Возвращается
// z=0,  если отношение выполняется,
// z=-1, если системы ДНФ несоразмерны,
// z<-1, если (|z|-2)-я строка Sop2_1 не имплицирует this,
// z>0 , если (z-1)-я строка Sop2_0 пересекается c this.
//______________________________________________________
int CSOP::NoEquiv(CSOP &Sop2_1, CSOP &Sop2_0)
{
  int z;
  if (z= Sop2_1.NoImpl(*this)) return ((z==-1)?z:z=-z-1);
  return (Sop2_0.CrossI(*this));
}

//____________________________________________________________
// Проверка отношения реализуемости системы частичных булевых
// функций (Sop1, Sop0) системой ДНФ this поляризованных 
// (вектором bvP) булевых функций.
// Возвращается
// z=0,  если отношение выполняется,
// z=-1, если системы ДНФ несоразмерны,
// z>0 , если (z-1)-я строка Sop1 или Sop0 не имплицирует this(~bv0),
// z<-1, если (|z|-2)-я строка Sop1 или Sop0 пересекается с this(bv0).
//____________________________________________________________

int CSOP::NoEquiv(CBV &bvP, CSOP &Sop1, CSOP &Sop0)
{
  int z;
  CSOP SopR1, SopR0, Sop1_1, Sop1_0, Sop0_1, Sop0_0;  
  DivPolD(bvP, SopR1, SopR0);
  Sop1.DivPolD(bvP, Sop1_1, Sop1_0);
  if(z= SopR1.NoEquiv (Sop1_1, Sop1_0)) return ((z==-1)?z:z=-z-1);
  Sop0.DivPolD(bvP, Sop0_1, Sop0_0);
  return (SopR0.NoEquiv(Sop0_0, Sop0_1)); 
}

//__________________________________________________________
// Проверка отношения эквивалентности между системами ДНФ 
// this и Sop2, поляризованной векторами bvP. 
// Возвращается
// z=0,  если отношение выполняется,
// z=-1, если системы ДНФ несоразмерны,
// z>0 , если (z-1)-я строка нарушает отношение 
//       для равнополярных функций и 
// z<-1, если (|z|-2)-я строка  нарушает отношение 
//       для разнополярных функций.
//__________________________________________________________

int CSOP::NoEquiv(CBV &bvP, CSOP &Sop2)
{
  int z=0, m= GetnFun(); 
  if(GetnArg()!=Sop2.GetnArg() || Sop2.GetnFun()!=m ||
      bvP.GetBitLength()!=m) return -1; 
  CSOP Sop1_1, Sop1_0, Sop2_1, Sop2_0; 
  if(bvP.IsZero()) return NoEquiv(Sop2);
  if(bvP.IsOne()) return CrossI(Sop2);
  DivPolD(bvP, Sop1_1, Sop1_0);
  Sop2.DivPolD(bvP, Sop2_1, Sop2_0);
  if(z= Sop1_1.NoEquiv(Sop2_1)) return ((z==-1)?z:z=-z-1);
  return Sop1_0.CrossI(Sop2_0);
}

/////////////////////////////////////////// Implication
//_____________________________________________________________
// Проверка отношения импликации this -> Sop2.
// Возвращается
// z=0,  если отношение выполняется,
// z=-1, если системы ДНФ несоразмерны или |Sop2|=0,
// z>0 , если (z-1)-я строка this не имплицирует Sop2.
//_____________________________________________________________

int CSOP::NoImpl(CSOP &Sop2)
{
  int i, j, k, p, q, u, v, a, b, c, d, e, g, n= GetnArg(),
      m= GetnFun(), p1= GetnCon(), p2=Sop2.GetnCon();
  char s;
      //CString ZZsSop1, ZZsSop2, ZZsSopW, ZZsBmR, ZZsbvR, //ZZsbv, ZZsbvF, ZZstv, ZZsbvRW; 
      //ZZsSop1=Sop1.ToChar();  //ZZsSop2=Sop2.ToChar(); 
  if(!p1) return 0;
  if(GetnArg()!=Sop2.GetnArg() || 
     GetnFun()!=Sop2.GetnFun() || !p2) return -1; 
  CBV bv, bvX(1, n), bvRR(1,p2), bvR, bvRW, bvW=bvRR; 
  CTV tv;
  CSOP SopW(n, m, 0);
  CBM BmR(p2,p2);
  BmR.SetSize(0,p2);
  CTM Tm_T(Sop2.GetConTM(),0);
  CBM Bm_T(Sop2.GetFunBM(),0);
  for(g=0; g<p1; g++) {
    tv= m_tmCon.GetRowTv(g);
    bv= m_bmFun.GetRowBv(g);
    bvR=bvRR;       //ZZstv=tv.BitChar();//ZZsbv=bv.BitChar();//ZZsbvR=bvR.BitChar();
    if(bv.IsZero() || Sop2.DelOrBT(tv, bv, bvR)==3) continue;
    if(bvR.IsZero()) return g+1; //ZZsbv=bv.BitChar();//ZZsbvR=bvR.BitChar();
    SopW.AddCon(tv, bv);
    BmR.Add(bvR); i=-1;
    while((i=SopW.GetnCon()-1)>-1) {
      tv=SopW.m_tmCon.GetRowTv(i);
      bv=SopW.m_bmFun.GetRowBv(i); 
      bvR=BmR.GetRowBv(i);  //ZZstv=tv.BitChar();//ZZsbv=bv.BitChar();//ZZsbvR=bvR.BitChar();
      SopW.RemoveR(i);
      BmR.RemoveRow(i); q=0;
      while (!bvR.IsZero()) {
        q=0;j=-1;
        while ((j=bvR.LeftOne(j))!=-1)
          if(Sop2.m_tmCon.IsCover(j, tv)) {
            if((bv-=Sop2.m_bmFun.GetRowBv(j)).IsZero()) {q=1; break;}
            bvR.SetBitAt(j, 0); 
                  //bvW.Zero(); j=-1;
                  //while((j=bv.LeftOne(j))!=-1) bvW|=Bm_T[j];
                  //bvR&=bvW;
                  //ZZstv=tv.BitChar();//ZZsbv=bv.BitChar();//ZZsbvR=bvR.BitChar();
          }
        if(q) break;
        e=n+1; j=-1; bvX=tv.GetUnDefs();
        while((j=bvR.LeftOne(j))!=-1) 
          if(Sop2.m_bmFun.GetRowBv(j,bv).IsZero()) bvR.SetBitAt(j,0); 
          else 
            if((b=Sop2.m_tmCon.CountDefs(bvX,j))<e) 
            { 
              e=b; d=j; 
              if(e==1) break;
            }
        if(e==1) {
          v=Sop2.m_tmCon.LeftDef(d,-1,bvX); 
          s=Sop2.m_tmCon.GetBitAt(d,v);
        }
        else {
          bvX&=Sop2.m_tmCon.GetRowDef(d);
          p=-1; k=-1; q=0;
          while ((k=bvX.LeftOne(k))!=-1)
            if ((a=Tm_T.CountDefs(bvR,k))>=p) {
              b=Tm_T.CountOnes(bvR,k); c=a-b;
              if ((a>p)||(b>u)||(c>u)) {
                v=k; p=a; u=(b<c)? c:b; s=(b<c)? '0':'1';
              } 
            }
          if(p<1) return g+1; 
        }
        bvRW=bvR;
        tv.SetBitAt(v, s); 
        bvR = Tm_T.GetRowUnDef(v)|((s=='1')
              ? Tm_T.GetRowOne(v) : Tm_T.GetRowZero(v));
        bvR &= bvRW;
        SopW.AddCon(tv, bv); BmR.Add(bvR);    //ZZstv=tv.BitChar();//ZZsbv=bv.BitChar();//ZZsbvR=bvR.BitChar();
        tv.SetBitAt(v,((s-'0')^'1')); 
        bvR = Tm_T.GetRowUnDef(v)|((s=='1')
              ? Tm_T.GetRowZero(v) : Tm_T.GetRowOne(v));
        bvR&=bvRW;      //ZZstv=tv.BitChar();//ZZsbv=bv.BitChar();//ZZsbvR=bvR.BitChar();
      }
      if(!q) return g+1; 
    }
  }
  return 0;
}

//__________________________________________________________________
// Проверка отношения импликации (tv,bv) -> Sop1.
// Возвращается
// z=0, если отношение выполняется и z=1 в противном случае.
//__________________________________________________________________

int CSOP::NoImpl (CTV &tv, CBV &bv)
{
  CBV bvR((BYTE)1, GetnCon());
  return NoImpl(tv, bv, bvR);
}

//__________________________________________________________________
// Проверка отношения импликации (tv,bv) -> Sop1(bvR), 
// где bvR выделяет строчный минор в Sop1.
// Возвращается
// z=0, если отношение выполняется z=1 в противном случае.
//__________________________________________________________________

int CSOP::NoImpl (CTV &tv, CBV &bv, CBV &bvR)
{
  int i=-1, h, j;
  CBV bvY((BYTE)0, GetnCon()), bv1;
  while ((i=bv.LeftOne(i))!=-1) {
    bvY= m_bmFun.GetColumnBv(i,bvR); 
    h=1; j=-1;
    while((j=bvY.LeftOne(j))!=-1) {
      if(!m_tmCon.AreOrthog(j,tv)) {
        if(m_tmCon.IsCover(j,tv)) {
          if(m_bmFun.GetRowBv(j)>=bv) return 0;
          h=0; break;
        }
      }
      else bvY.SetBitAt(j,0); 
    }
    if(h) {
      bv1= tv.GetUnDefs(); 
      if(!m_tmCon.Degen(bv1, bvY).IsEmpty()) return 1;
    }
  }
  return 0;
}

//__________________________________________________________
// Проверка отношения импликации (this(bv0) -> Sop1) 
// между системой ДНФ this поляризованных (вектором bvP) 
// булевых функцийи и системой Sop1 
// Возвращается
// z=0,  если отношение выполняется,
// z=-1, если системы ДНФ несоразмерны,
// z>0 , если (z-1)-я строка this(~b0) не имплицирует Sop1,
// z<-1, если (|z|-2)-я строка this(b0) пересекается с Sop1.
//__________________________________________________________

int CSOP::NoImpl(CBV &bvP, CSOP &Sop1)
{
  int z;
  CSOP SopR1, SopR0;  
  DivPolD (bvP, SopR1, SopR0);
  if(z=SopR1.NoImpl(Sop1)) return ((z==-1)?z:z=-z-1);
  return Sop1.CrossI(SopR0);
}


//_________________________________________________
// Проверка отношения пересечения двух систем ДНФ.
// Возвращается 
// z=0, если this & Sop2 = 0,
// z>0, если (z-1)-я строка this пересекается с Sop2.
//_________________________________________________

int CSOP::CrossI(CSOP &Sop2)
{
  if(!GetnCon() || !Sop2.GetnCon()) return 0;
  if(GetnArg()!=Sop2.GetnArg() || 
     GetnFun()!=Sop2.GetnFun()) return -1; 
  int i, j;  
  CTV tv; CBV bv;
  for (i=0; i<GetnCon(); i++)
    if (!(bv= m_bmFun.GetRowBv(i)).IsZero()) {
      tv= m_tmCon.GetRowTv(i);
      for (j=0; j<Sop2.GetnCon(); j++)
        if (!Sop2.m_tmCon.AreOrthog(j, tv)&&
            !Sop2.m_bmFun.GetRowBv(j, bv).IsZero())
          return (i+1);
    }
  return 0;                       
}
 
//__________________________________________________________
// Разделение системы ДНФ Sop поляризованных (вектором bv0)
// булевых функций на две системы ДНФ Sop1 и Sop0.
//__________________________________________________________

void CSOP::DivPolD(CBV &bv0, CSOP &Sop1, CSOP &Sop0)
{
  int i;
  CBV bv1=~bv0, bv, bvF;
  CTV tv;
  Sop1.m_tmCon.SetSize(0, GetnArg());
  Sop1.m_bmFun.SetSize(0, GetnFun());
  Sop0.m_tmCon.SetSize(0, GetnArg());
  Sop0.m_bmFun.SetSize(0, GetnFun());
  for(i=0; i<GetnCon(); i++) {
    tv= m_tmCon.GetRowTv(i);
    bv= m_bmFun.GetRowBv(i);
    if(!(bvF=bv1&bv).IsZero()) Sop1.AddCon(tv, bvF);
    if(!(bvF=bv0&bv).IsZero()) Sop0.AddCon(tv, bvF);
  }
  return;
}

