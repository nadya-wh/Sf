/////////////////////////////////////////////////////////////////
// Ver.2.1.0     07.07.2006
/////////////////////////////////////////////////////////////////
//CTV CTM::Degen()
//CTV CTM::Degen(CBV &bvX, CBV &bvY)
//int CTM::Degen(CTM &TmT, CTV &tvW, CBV &bvY)
//int CTM::UnAltRow(CTM &mtT,CBV &bvX,CBV &bvY,CTV &tv)
//int CTM::UnAltCol(CBV &bvX,CBV &bvY,CTV &tv)
/////////////////////////////////////////////////////////////////

#include <stdlib.h>
        
#ifdef _JOINT
#include "BaseBool.h"
#include "BaseTern.h"
#else
#include "../Common/BaseBool.h"
#include "../Common/BaseTern.h"
#endif

//____________________________________________________________
// Проверка троичной матрицы на вырожденность 
//____________________________________________________________

CTV CTM::Degen()
{
  CBV bvX((BYTE)1, GetCountC()), bvY((BYTE)1, GetCountR());  
  return Degen(bvX, bvY);
}

//____________________________________________________________
// Проверка минора троичной матрицы на вырожденность 
// (bvX - столбцы, bvY - строки)
//____________________________________________________________

CTV CTM::Degen(CBV &bvX, CBV &bvY)
{
  int a,b,c,i,k=0,w,u,d,e=0,g=1,q,m=GetCountR(),n=GetCountC();
  CTV tv(n,'-');
  if(!m || bvY.IsZero()) return(tv);
  if(bvX.IsZero()) return(0);
  CBM BmY(n,m), BmX(n,n); CTM TmV(n,n); 
  CTM mtt = Trans(); //CTM mtt(mt, 0); 
  CBV bvy; 
  while  (g) 
  {
    switch (d=UnAltRow (mtt, bvX, bvY, tv))
    {
      case -3: return (tv);
      case -2: 
      {
        if(!k--) return(0); // Решения нет
        bvX=BmX[k]; tv=TmV.GetRowTv(k); bvY=BmY[k];
        continue;
      } 
      default:
      {
        switch (mtt.UnAltCol(bvX, bvY, tv))
       {
          case -3: return (tv);
          case -2:
          {
            if(!k--) return(0); // Решения нет
            bvX=BmX[k]; tv=TmV.GetRowTv(k); bvY=BmY[k];
            continue;
          }
          case -1: continue;
          default:
          {
            if(d<0) d=bvY.LeftOne(-1); 
            w=0; i=-1; // Максимальный столбец, пересекающий d-ю строку
            while((i=LeftDef(d, i, bvX))!=-1)
              if((a=mtt.CountDefs(bvY, i))>=w)
              {
                b=mtt.CountOnes(bvY, i); c=a-b;
                if ((a>w)||(b>u)||(c>u))
                { e=i; w=a; u=(b<c)? c:b; q=(b<c)? 0 : 1;} 
              }
              tv.SetBitAt(e, q+'0'); TmV.SetRowTv(k,tv);
              bvX.SetBitAt(e, 0); BmX.SetRow(k,bvX); bvy=bvY;
              bvY-=((q) ? mtt.GetRowZero(e) : mtt.GetRowOne(e));
              BmY.SetRow(k++, bvY); tv.SetBitAt(e,q^'1'); 
              bvY=bvy -((q) ? mtt.GetRowOne(e) : mtt.GetRowZero(e));
          }
        }
      }
    }
  }
  return (tv);
}

//____________________________________________________________
// Редукция и расщепление минора троичной матрицы Tm по выбранной
// переменной (с использованием транспонированной матрицы TmT) 
//____________________________________________________________

int CTM::Degen(CTM &TmT, CTV &tvW, CBV &bvY)
{
  int a,b,c,i,k=0,w,u,d,e=0,g=1,q,m=GetCountR(),n=GetCountC();
  CBV bvX=tvW.GetUnDefs();
  while (g) 
  {
    if((d=UnAltRow(TmT, bvX, bvY, tvW))<0) return (d+1);
    if((e=TmT.UnAltCol(bvX, bvY, tvW))<-1) return (e+1);
    if(e>=0)
    {
      if(d<0) d=bvY.LeftOne(-1); 
      w=0; i=-1; // Максимальный столбец, пересекающий d-ю строку
      while((i=LeftDef(d, i, bvX))!=-1)
        if((a=TmT.CountDefs(bvY, i))>=w)
        {
          b=TmT.CountOnes(bvY, i); c=a-b;
          if ((a>w)||(b>u)||(c>u))
          { e=i; w=a; u=(b<c)? c:b; q=(b<c)? 1 : 0;} 
        }
      tvW.SetBitAt(e, q+'0'); 
      return e;
    }
  }
  return (e);
}

//____________________________________________________________
// Поиск безальтернативных и пустых строк  
//____________________________________________________________

int CTM::UnAltRow(CTM &mtT,CBV &bvX,CBV &bvY,CTV &tv)
{
  int a,n=GetCountC(),w=n+1,i=-1,j,k,d=-1,e,f,g; 
  char s, ss;
  while ((i = bvY.LeftOne( i )) != -1)//Перебор по"маске" 
  {
    if((k = LeftDef(i, -1, bvX)) == -1) return (-2); // Найдена пустая строка (тупик)
    if((j = LeftDef(i, k, bvX)) == -1) // Найдена безальтернативная строка.                  
    {
      g=i;// Фиксирование соотв.значения комп.иск.вектора nd.m_tvW.
      while(1) 
      {
        if((s=GetBitAt(g, k))=='1')
        { tv.SetBitAt(k,'0'); bvY-=mtT.GetRowOne(k); j=mtT.LeftZero(k,-1,bvY);}
        else
        {tv.SetBitAt(k,'1'); bvY-=mtT.GetRowZero(k); j=mtT.LeftOne(k,-1,bvY);}
        if(j!=-1 && j<i) i=j-1;
        if(bvY.IsZero()) return (-3); // Строк в миноре нет (решение найдено)
        bvX.SetBitAt(k, 0);
        if(bvX.IsZero()) return (-2); // Столбцов в миноре нет (тупик)
        if(d!=-1) 
        {
          if(((ss=GetBitAt(d,k))^'1')==s) 
          { 
            if(w==2) {g=d; k=(e==k)?f:e; d=-1; w=n+1; continue;} 
            w--;
            if(w==2) {e=LeftDef(d,-1,bvX); f=LeftDef(d,e,bvX);}
            if(i<d) i=d;
          }
          else if(ss!='-') {d=-1; w=n+1;}
        }
        break;
      }
    }
    else 
    {
      if((w>2)&&((a=CountDefs(bvX,i))<w)) 
      {
        d=i; w=a; if(w==2) {e=k; f=j;}//Найдена строка с весом 2.  
      }
    } 
  }
  return (d);
}
   
//____________________________________________________________
// Поиск безальтернативных столбцов 
//____________________________________________________________

int CTM::UnAltCol(CBV &bvX,CBV &bvY,CTV &tv)
{
  int i = -1, k, r=0, m=bvY.GetBitLength(), n=bvX.GetBitLength(); char s;
  while ((i = bvX.LeftOne(i)) != -1) 
  {
    k = 0; 
    if(ExistZero((BYTE*) bvY, i)) {k += 2; s = '1';} 
    if(ExistOne((BYTE*) bvY,i)) {k += 1; s = '0';}
    if(!k)
    {
      bvX.SetBitAt(i, 0); //"Пустой"столбец.
      if(bvX.IsZero()) return (-2); // Столбцов в миноре нет (тупик)
      continue;
    }
    if (k!=3)        // Альтернативный столбец.
    {
      tv.SetBitAt(i, s); 
      bvY -= (s=='1') ? GetRowZero(i) : GetRowOne(i); 
      if(bvY.IsZero()) return (-3); // Строк в миноре нет (решение найдено)
      bvX.SetBitAt(i, 0);
      if(bvX.IsZero()) return (-2); // Столбцов в миноре нет (тупик)
      i=r=-1; 
    }
  }
  return (r);
}
