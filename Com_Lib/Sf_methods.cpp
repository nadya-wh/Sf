/////////////////////////////////////////////////////////////////
// Ver.1.0.0    07.04.2006   Russian / English
//              включены некоторые методы класса CSF 

/////////////////////////////////////////////////////////////////
//    BOOL CSF::LogToSdfWithoutElimination(void)
//    BOOL CSF::SdfToLog()
//    int  CSF::Cascade (CMapStringToString& NumOfCascade)
//    int  CSF::GetLevel()
//    BOOL CSF::RenameVar(CMapStringToString * MultiPare)

//#include <io.h>
#include <vector>
#include <map>

using namespace std;
#include "../Com_Lib/objects.h"
#include "../Com_Lib/archive.h"
#include "../Com_Lib/sf.h"
#include "../Com_Lib/bufer.h"
#include "../Common/BaseBool.h"
#include "../Common/BaseTern.h"
#include "../Common/Sop.h"


//#ifdef _LINUX
#undef _DEBUG
typedef unsigned char BYTE; 
typedef unsigned long ULONG; 
typedef int BOOL;

#define ASSERT assert
#include <assert.h>
/*#else
#include <afx.h>
#include <afxwin.h>
#pragma warning(disable : 4996)
#endif
*/


//-------------------------------------------------------------------
bool CSF::LogToSdfWithoutElimination(void)
{ int i, j, k, ii, iii, num_first_inter_var, num_row, num_cur_row, 
      num_col, num_fun;
  string ls_ur, ls_ur1, ls1, ls2, ls3, ls4, ls5, lserr, name_var, name_var1;
  char cbuf[32];
  CSFVar *pVar;
  CSFunction *pNewFun;
  CBM *TrMat_Zero, *TrMat_One, *MatFun;
  BOOL bVal;
  int jj = 0;
  i= iii = 0;

  if (m_Fun== NULL) {
//#ifdef _LINUX
    printf("The LOG-section in SF-description is absent\n");
/*#else
#ifdef ENG
    AfxMessageBox("The LOG-section in SF-description is absent");
#else
    AfxMessageBox("В SF-описании отсутствует LOG-секция");
#endif
#endif
*/
    return false;
  }

  while (m_Fun->GetTextLine(i, ls_ur))  {
    ls_ur = BeginEndTrim(ls_ur);
//    while ( ls_ur.ReverseFind(';') != ls_ur.GetLength() - 1) {
    while ( ls_ur.rfind(';') != ls_ur.size() - 1) {
      ii = i;
      if (!m_Fun->GetTextLine(i, ls_ur1)) {
//#ifdef _LINUX
        printf("Error in section of LOG SF-description: %s\n", ls_ur.c_str());
/*#else
#ifdef ENG
        ls_ur ="Error in section of LOG SF-description: " + ls_ur;
#else
        ls_ur = "Ошибка в разделе LOG SF-описания: " + ls_ur;
#endif
        AfxMessageBox(ls_ur.c_str());
#endif
*/
        return false;
      }
      ls_ur1 = BeginEndTrim(ls_ur1);
      m_Fun->m_Text[iii] = ls_ur + ls_ur1;
      m_Fun->m_Text[ii] = "";
      ls_ur = m_Fun->m_Text.at(iii);
    }

    ls_ur = m_Fun->m_Text.at(iii);
    ls_ur = BeginEndTrim(ls_ur);
    if (ls_ur.find(';') != ls_ur.size() - 1)  {
      j = ls_ur.find(';');
      //ls1 = ls_ur.Right(ls_ur.GetLength() - j -1);
      ls1 = ls_ur;
      ls1.erase(0,j+1);
      ls_ur = ls_ur.substr(0,j+1);    //ls_ur.Left(j + 1);
      m_Fun->m_Text[iii] = ls_ur;
      m_Fun->m_Text.push_back(ls1);
    }
    iii = i;
  }
  num_first_inter_var = -1;
  name_var = "__inter_";
  if (!m_Fun)  return false;
  if (m_Fun->GetTyp() != "LOG")  return false;
  i= 0; 
  while (m_Fun->GetTextLine(i, ls_ur))  {
    ls_ur = BeginEndTrim(ls_ur);
    while ((int)ls_ur.find('(') >= 0)  {
      j = ls_ur.find(')');
      if (j < 0)  {
        lserr = ls_ur;
        goto bad;
      }  
      ls1 = ls_ur.substr(0,j);  //ls_ur.Left(j);
//      ls2 = ls_ur.Right(ls_ur.GetLength() - j - 1);
      ls2 = ls_ur;
      ls2.erase(0,j+1);
      j = ls1.rfind('(');
      ls3 = ls1.substr(0,j);  // Left(j);
//      ls5 = ls1.Right(ls1.GetLength() - j - 1);
      ls5 = ls1;
      ls5.erase(0,j+1);



//      ls4.Format("%s%d=%s;", name_var, ++num_first_inter_var, ls5);
//      itoa(++num_first_inter_var, cbuf, 10);
      sprintf(cbuf, "%u", ++num_first_inter_var);
      ls4 = name_var + cbuf;
      ls4 +='=' + ls5; 
	  ls4 +=';';//добавить ; 

//      name_var1.Format("%s%d", name_var, num_first_inter_var);
      name_var1 = name_var + cbuf;  

      m_Fun->AddTextLine(ls4);
      ls_ur = ls3 + name_var1 + ls2;
      m_Fun->ReplaceTextLine(i-1,ls_ur); 
      pVar = new  CSFVar(name_var1, 't', 0, 0);
      AddVar(pVar);
    }   // while (ls_ur.Find('(') >= 0)
  }   //  while (GetTextLine(i, ls_ur))

  i = 0;
  num_row = 0;
  while (m_Fun->GetTextLine(i, ls_ur))  {
    if ((int)ls_ur.find('=') >0)
      num_row++;
    while ((k = ls_ur.find('+')) >=0)  {
      num_row++;
      //ls_ur = ls_ur.Right(ls_ur.GetLength() - k -1);
      ls_ur.erase(0, k+1);
    }
  }

  int inv, intv, outv;
  inv = GetNumInVar();
  intv = GetNumInterVar();
  outv = GetNumOutVar();
  TrMat_Zero = new CBM(num_row, inv+ intv,false);
  TrMat_One  = new CBM(num_row, inv + intv,false);
  MatFun = new CBM(num_row, outv + intv,false);

  i = 0;
  num_cur_row = 0;
  while (m_Fun->GetTextLine(i, ls_ur))  {
    while ((int)ls_ur.find(';') < 0) {

      jj = m_Fun->GetTextLine(i, ls_ur1);
      if (!jj && ls_ur != "") {

//#ifdef _LINUX
        printf("Error in section of LOG SF-description: %s\n",ls_ur.c_str());
/*#else
#ifdef ENG
        ls_ur = "Error in section of LOG SF-description: " + ls_ur;
#else
        ls_ur = "Ошибка в разделе LOG SF-описания: " + ls_ur;
#endif
        AfxMessageBox(ls_ur.c_str());
#endif
*/
        goto badbad;
      }
      else if (!jj) goto good_kon;
      ls_ur += ls_ur1;
    } 

    ls_ur = BeginEndTrim(ls_ur);
    while ((ii=ls_ur.find("^^")) >=0)   
//      ls_ur = ls_ur.Left(ii) + ls_ur.Right(ls_ur.GetLength() - ii -2);
      ls_ur = ls_ur.substr(0,ii) + ls_ur.erase(0,ii+2);

    j = 0;
    if (GetNameVar(ls_ur, j, ls1)<1)  {
      lserr = ls_ur;
bad:
//#ifdef _LINUX
      printf("Error in the equation %s\n", lserr.c_str());
/*#else
#ifdef ENG
      lserr="Error in the equation " + lserr;
#else
      lserr="Ошибка в уравнении " + lserr;
#endif
      AfxMessageBox(lserr.c_str());
#endif
*/
badbad:
      delete TrMat_Zero;
      delete TrMat_One;
      delete MatFun;
      return false;
    }
    ls1 = BeginEndTrim(ls1);
    if (ls1.find("^") == 0)  ls1.erase(0,1);  //ls1 = ls1.Right(ls1.GetLength()-1);
    ls1 = BeginEndTrim(ls1);
    pVar = GetVarAndNumberForName(ls1, k);
    if (pVar == NULL) { 
      pVar = new CSFVar(ls1, 't', 0,0);
      ASSERT(pVar);
      AddVar(pVar);
      k = pVar->m_NumInSF;
      TrMat_Zero->SetSize(num_row, GetNumInVar() + GetNumInterVar()); 
      TrMat_One->SetSize(num_row, GetNumInVar() + GetNumInterVar());  
      MatFun->SetSize(num_row, GetNumOutVar() + GetNumInterVar()); 
    }
    if (pVar->m_Typ == 'o')  num_fun = k;
    else                     num_fun = GetNumOutVar() + k;
    //ls_ur = ls_ur.Right(ls_ur.GetLength() - j-1);
    ls_ur.erase(0,j+1);

    ls_ur = BeginEndTrim(ls_ur);
    while (ls_ur != "")  {
beg_kon:
      ls_ur = BeginEndTrim(ls_ur);
      k = ls_ur.find('+');
      if (k < 0)  {
        ls1 = ls_ur;
        ls_ur = "";
      }
      else  {
        //ls1 = ls_ur.Left(k);
        ls1 =ls_ur.substr(0,k);
        
        ls1 = BeginEndTrim(ls1);
        //ls_ur = ls_ur.Right(ls_ur.GetLength() - k-1);
        ls_ur.erase(0,k+1);
      }
      MatFun->SetBitAt(num_cur_row,num_fun,true);
//-------------- now we nave one conjunction   - ls1
      while (ls1 != "")  {
        if (ls1[0] == '^')  { bVal = false; ls1.erase(0,1); }  //ls1 = ls1.Right(ls1.GetLength()-1);
        else               bVal = true;
        j = 0;
        ls1 = BeginEndTrim(ls1);
        if (GetNameVar(ls1, j, ls2)<1)  {
          lserr = ls1;
          goto bad;
        }
        if ( (int)ls1.size() == j )  ls1.erase(0,j); // ls1 = ls1.Right(ls1.GetLength()-j);
        else                  ls1.erase(0,j+1); //ls1 = ls1.Right(ls1.GetLength()-j-1);
        ls2 = BeginEndTrim(ls2);
        if (ls2.find('^') == 0)    ls2.erase(0,1); //ls2 = ls2.Right(ls2.GetLength()-1);
        ls2 = BeginEndTrim(ls2);

        if (ls2 == "1")  {
          MatFun->SetBitAt(num_cur_row,num_fun,true);
          ls_ur = "";
          break;
        }
        if (ls2 == "0")  {
          MatFun->SetBitAt(num_cur_row,num_fun,false);
          ls_ur = "";
          break;
        }

        pVar = GetVarAndNumberForName(ls2, k);
        if (pVar == NULL) { 
          pVar = new CSFVar(ls2, 't', 0,0);
          AddVar(pVar);
          k = pVar->m_NumInSF;
          TrMat_Zero->SetSize(num_row, GetNumInVar() + GetNumInterVar());
          TrMat_One->SetSize(num_row, GetNumInVar() + GetNumInterVar());
          MatFun->SetSize(num_row, GetNumOutVar() + GetNumInterVar()); 
        }
        if (pVar->m_Typ == 'i')       num_col = k;
        else if (pVar->m_Typ == 't')  num_col = GetNumInVar() + k;
        else  {
//#ifdef _LINUX
            printf("In the given version of system the output variables"
            " should not be included into the right parts of the equations (variable %s)\n",pVar->m_Name.c_str());
/*#else
            string st0;
#ifdef ENG
            st0 = "In the given version of system the output variables"
            " should not be included into the right parts of the equations (variable " + pVar->m_Name;
#else
            st0 = "В данной версии системы выходные переменные"
            " не должны входить в правые части уравнений (переменная " + pVar->m_Name;
#endif
            st0 += ')';
            AfxMessageBox(st0.c_str());
#endif
*/
          goto badbad;
        }
        if (bVal)   TrMat_One->SetBitAt(num_cur_row,num_col,true);
        else        TrMat_Zero->SetBitAt(num_cur_row,num_col,true);
        ls1 = BeginEndTrim(ls1);
      }  // while (ls1 != "")
      if (ls_ur != "") {
        num_cur_row ++;
        goto beg_kon;
      }
      ls_ur = BeginEndTrim(ls_ur);
    }  // while (ls_ur != "")
    num_cur_row++;
  }  // while (m_Fun->GetTextLine(i, ls_ur))
good_kon:
  pNewFun = new CSFunction("SDF",GetNumInVar() + GetNumInterVar(),
                GetNumOutVar()+ GetNumInterVar(), num_row );
  pNewFun->SetSdf(TrMat_Zero, TrMat_One, MatFun);
  SetFun(pNewFun);
  delete TrMat_Zero;
  delete TrMat_One;
  delete MatFun;
  return true;
}

//-------------------------------------------------------------------
// Перевод FUNCTION из SDF в LOG           Кириенко 02.05.2000
bool  CSF::SdfToLog()
{
  string Buf, one, two, BufUr, Buf_copy;
  CSFunction  *pFunNew;
  CSFVar *pVar;
  int stoltro, stolbool, str, chert, FunEqualZero;
  int i, j, k, j1;
  int sdvigvh, sdvigvyh, sdviginter_vy;
  int sdviginter_vh;

  if(m_Fun->GetTyp()!="SDF"){
#ifdef ENG
    Buf="SDF is absent in section FUNCTION";
#else
    Buf="В блоке FUNCTION нет SDF";
#endif
//#ifdef _LINUX
    printf("%s\n",Buf.c_str());
/*#else
    AfxMessageBox(Buf.c_str());
#endif
*/
    return false;
  }
  if(m_FunComments=="T"){     // 20.03.2006 Новости для частичных функций
#ifdef ENG
    Buf="Partial functions are defined in section FUNCTION";
#else
    Buf="В блоке FUNCTION определены частичные функции";
#endif
//#ifdef _LINUX
    printf("%s\n",Buf.c_str());
/*#else
    AfxMessageBox(Buf.c_str());
#endif
*/
    return false;
  }

  pFunNew = new CSFunction("LOG",GetNumInVar(),
              GetNumOutVar(),GetNumInterVar());

  stoltro = m_Fun->GetNumInt();
  stolbool = m_Fun->GetNumOut();
  str = m_Fun->GetNumInter();

  chert=i=0;
  sdvigvyh=0;
  sdviginter_vy=0;

  for (j=0; j<stolbool; j++) {
////////////////////////////////////////////////////////////
// Построение j-ого уравнения,

    FunEqualZero=0;
    if (!GetNextOutVar(sdvigvyh,pVar)) {
      if (!GetNextInterVar(sdviginter_vy,pVar)) {
//#ifdef _LINUX
        printf("Syntax error: Number of outputs and number of columns do not coincide\n");
/*#else
#ifdef ENG
         AfxMessageBox(
         "Syntax error: Number of outputs and number of columns do not coincide");
#else
         AfxMessageBox(
         "Синтаксическая ошибка: не совпадают число выходов и столбцов");
#endif
#endif
*/
         delete pFunNew;
         return false;
      }
    }
    BufUr = pVar->GetName();
    BufUr += " = ";
    Buf_copy=BufUr;
// цикл по числу конъюнкций k.
    for (k=0; k<str;) {
      if (!m_Fun->GetTextLine(k,Buf)) break;
      //Buf.TrimRight();
      while(Buf.at(Buf.size()-1)==' ') Buf.erase(Buf.size()-1,1);
      j1 = Buf.find(' ');
      one = Buf.substr(0,j1);                //Left(j1);
      j1 = Buf.rfind(' ');
      //two = Buf.Right(Buf.GetLength() -j1-1);
      two = Buf;
      two.erase(0,j1+1);
      if (one.size()==0||two==""){
//#ifdef _LINUX
          printf("Syntax error: the rows of SDF-matrixes  are given incorrectly\n");
/*#else
          AfxMessageBox(
#ifdef ENG
         "Syntax error: the rows of SDF-matrixes  are given incorrectly."
#else
         "Синтаксическая ошибка: неправильно заданы строки матриц в SDF"
#endif
         ,MB_ICONSTOP);
#endif
*/

        delete pFunNew;
        return false;
      }
      if (two.at(j)=='1') {
        sdvigvh=0;
        sdviginter_vh=0;

        chert=0;
        for (i=0; i<stoltro; i++) {
          if (!GetNextInVar(sdvigvh,pVar)) {
            if (!GetNextInterVar(sdviginter_vh,pVar)) {
//#ifdef _LINUX
              printf("Syntax error: Number of inputs and number of columns do not coincide\n");
/*#else
#ifdef ENG
              AfxMessageBox(
              "Syntax error: Number of inputs and number of columns do not coincide");
#else
              AfxMessageBox(
              "Синтаксическая ошибка: не совпадают число входов и столбцов");
#endif
#endif
*/
              delete pFunNew;
              return false;
            }
          }
          if  (one.at(i)=='-') {
            chert++;
// Пропустить одну входную переменную
            continue;
          }
          if  (one.at(i)=='0') BufUr += "^";
          BufUr += pVar->GetName();
          BufUr += "*";
        }

        if (chert==i) {
          BufUr = Buf_copy + "1;";
          pFunNew->AddTextLine(BufUr);
          break;
        }

        BufUr[BufUr.size()-1] = '+';
      }
      else FunEqualZero++;
    }
    if (FunEqualZero==str) {
      sdvigvh=0;
      if (GetNextInVar(sdvigvh,pVar)==false)
      {
//#ifdef _LINUX
        printf("Error in the enumeration of a input variables, SdfToLog\n");
/*#else
#ifdef ENG
        AfxMessageBox("Error in the enumeration of a input variables, SdfToLog");
#else
        AfxMessageBox("Ошибка в перечне входных переменных, SdfToLog");
#endif
#endif
*/
        delete pFunNew;
        return false;
      }
      BufUr = Buf_copy + "0;";
      pFunNew->AddTextLine(BufUr); chert=i;
    }
    if (chert!=i) {
      BufUr[BufUr.size()-1] = ';';
      pFunNew->AddTextLine(BufUr);
    }
    BufUr.clear();
  }
  SetFun(pFunNew);
  return true;
}

//-------------------------------------------------------------------
//Определение числа каскадов в системе уравнений (возвр. значение)
//и CMapStringToPtr - соответствие "Имя переменной - номер каскада"
//Число каскадов = число промежуточных переменных на самом длинном 
//пути от выхода к входу.
//Нулевая строка не содержит информации
//первая строка - внутренние переменные, от которых зависят 
//выходные (1 каскад)и т.д.
//n-ая строка - нулевая, соответствует входным переменным.
//Число каскадов = число строк BmK -1.

int CSF::Cascade (map <string,string>& NumOfCascade)
{
  int i,y,m,n,k,j;
  string Buf;
  CSFunction *pF;
  CSFVar *pVar1;
  CSOP *pSop;
  CBV bvE,bvC, bv1;
  char str[10];

  // Перейти к SDF
  if (LogToSdfWithoutElimination()==false)
  {
#ifdef ENG
    Buf="Transformation error from LOG to SDF";
#else
    Buf="Ошибка при переходе LogToSdfWithoutElimination";
#endif
//#ifdef _LINUX
    printf("%s\n", Buf.c_str());
/*#else
    AfxMessageBox(Buf.c_str());
#endif*/
    return -1;
  }
  pF = GetFun();
  if (!pF->GetSdf(pSop)) {
#ifdef ENG
    Buf="Error in section FUNCTION (SDF)";
#else
    Buf="Ошибка чтения раздела FUNCTION (SDF)";
#endif
//#ifdef _LINUX
    printf("%s\n", Buf.c_str());
/*#else
    AfxMessageBox(Buf.c_str());
#endif
*/
    return -1;
  }

  y = GetNumInterVar();
  m = (pSop->GetnFun() - y);
  n = (pSop->GetnArg()-y);
  CBV bvA((BYTE)0,n+y);
  CBV bvK((BYTE)0,y), bv=bvK;
  CBV bvY((BYTE)1,m+y);
  CBM BmA(m+y,n+y), BmK, BmFT(pSop->GetFunBM(),0);

 // Формирование (m+y)*(n+y) матрицы аргументов BmA
  i=-1; bvE=bvA; bvE.SetBitAt(0,1);
  while((i=bvY.LeftOne(i))!=-1)
  { 
    bvC=BmFT.GetRowBv(i); bvA.Zero(); j=-1;
    while((j=bvC.LeftOne(j))!=-1)
    {
      if(!(bv1=pSop->m_tmCon.GetRowDef(j)).IsZero())
        bvA |= bv1;
      else 
      {
        k=-1;
        bvY &= ~(bv1=pSop->m_bmFun.GetRowBv(j,bvY));
        while((k=bv1.LeftOne(k))!=-1)
          BmA.SetRow(k,bvE);
        break;
      }
      BmA.SetRow(i,bvA);
    }
  }
 // Формирование k*y матрицы каскадов BmK
  for(i=0; i<m; i++) 
    bvK |= BmA.GetRowBv(i).Extract(n,y);
  while(!bvK.IsZero())
  {
    BmK.Add(bvK);bv=bvK;
    i=-1; bvK.Zero();
    while((i=bv.LeftOne(i))!=-1)
      bvK |= BmA.GetRowBv(m+i).Extract(n,y);
    if(!bvK.IsZero())
      for(k=0; k<BmK.GetCountR(); k++)
      {
        BmK.SetRow(k,BmK.GetRowBv(k)-bvK);
        if(BmK.GetRowBv(k).IsZero())
        {
          SdfToLog();
          delete pSop;
//#ifdef _LINUX
          printf("The loop is in the net\n");
/*#else
#ifdef ENG
          AfxMessageBox("The loop is in the net");
#else
          AfxMessageBox("В схеме есть цикл");
#endif
#endif
*/
          return -1;    
        }
      }
  }
//Составить список CMapStringToPtr
// - соответствия "Имя переменной - номер каскада"
  k = BmK.GetCountR();
  for(j=0; j<k; j++)
  {
    bvK = BmK.GetRowBv(j);
    i=-1;  
    while((i=bvK.LeftOne(i))!=-1)  
    {
      pVar1 = GetInterVar(i);
#ifndef _MSVC9
//      itoa(j+1,str,10);
      sprintf(str, "%u", j+1);
#else
      _itoa_s(j+1,str,10,10);
#endif
      Buf = str;
      NumOfCascade[pVar1->m_Name] = Buf;   
    }
  }  
  SdfToLog();
  delete pSop;
  return (BmK.GetCountR());  
}

//-------------------------------------------------------------------28.03.2006 Кардаш С.Н.
//   Подсчет числа ярусов
int CSF::GetLevel()
{
  int i, j=0, point, err, bin = 0, bin1 = 0, bin2 = 0, max_level = 0;
  string Beg, End, Inter, str_inp, str_out, str_inter;
  vector <string> L, R;
  vector <string> LL, RR;
  vector <int> U,UU;
  string left, right, NamEl, str, Name_inter;
  CSFType *pType;
  CSFConnect *pConnect;
//  POSITION pos=0, pos1=0;
  BOOL bRes, bRes1;

///////////////// разбор раздела CONNECT  
  L.clear();  R.clear(); 
  LL.clear(); RR.clear();
  m_TypeMap.itp= m_TypeMap.mtp.begin();    //pos = 0; 
  for(bRes =  GetFirstType(pType); bRes;
      bRes =  GetNextType(pType)) {
    for(i = 0; (NamEl = pType->GetElem(i)) != ""; i++) {
        pConnect = GetConnectForType(NamEl);
      pConnect->iCon = pConnect->m_Connect.begin();  //pos1 = 0;
      for(bRes1 = (pConnect->GetFirstPare(left,right)); bRes1;
          bRes1 = (pConnect->GetNextPare(left,right)))  {
        str = NamEl; str += "."; str += left;
        if((point = right.find('.'))== -1)   {  
          Beg = "$$$"; 
          if((point = str.find('.'))== -1)     End = "$$$"; 
          else
            End = str.substr(0,point);   //Left(point); 
          for(j = 0; j < bin; j++)
            { if(End == R.at(j))  break;}
          if(j != bin)     continue; 
          L.push_back(Beg); R.push_back(End);
          U.push_back(1); bin++;
        }
        else {
          Beg = right.substr(0,point);   //Left(point); 
          if((point = str.find('.'))== -1) 
            End = "$$$"; 
          else
            End = str.substr(0,point);   //Left(point); 
          LL.push_back(Beg); RR.push_back(End);
          bin1++;  
        }  
      }
    } 
  }
///////////////// разбор раздела OUT
  pConnect = GetConnectForType("OUT");       
   pConnect->iCon = pConnect->m_Connect.begin();  //pos1 = 0;
  for(bRes1 = (pConnect->GetFirstPare(left,right)); bRes1;  
      bRes1 = (pConnect->GetNextPare(left,right)))    {    
    if((point = right.find('.'))== -1) 
      Beg = "$$$"; 
    else
      Beg = right.substr(0,point);   //Left(point); 
    if((point = left.find('.'))== -1) 
      End = "$$$"; 
    else
      End = left.substr(0,point);   //Left(point); 
    LL.push_back(Beg); RR.push_back(End);
    bin1++;
  }
///// перебор пар
  for(i = 0; i < bin; i++) { 
    str = L.at(i);         
    if(str != "$$$")    continue;
    Beg = R.at(i);
    if(Beg == "$$$")     continue;
    for(j = 0; j < bin1; j++) {  // поиск по внутренним
      left = LL.at(j);
      if(left != Beg)    continue;
      End = RR.at(j);
      if(Beg == End)    {err = 999994; goto ERR;}
      if(End == "$$$")   { 
        UU.push_back( err = U[i] + 1);     
        bin2++; continue;
      }
      L.push_back("$$$");  R.push_back(End); 
      U.push_back(U[i] + 1);
      bin++;
    }
    L.erase(L.begin()+i);  R.erase(R.begin()+i); 
    U.erase(U.begin()+i);
    i--; bin--;
  }
  for(j = 0; j < bin2; j++) {
    if(UU[j] <= max_level)   continue;
    max_level = UU[j];
  }
  err = --max_level;
ERR:   return err;
}

//-------------------------------------------------------------------25.05.2000 Кириенко
// Замена имени переменной в уравнениях, в списке внешних переменных
// m_SFVarArray. В таблица отображений:
// Слева - что менять, справа - на что менять
bool  CSF::RenameVar(map <string, string>* MultiPare)
{
  map <string,string> :: iterator ii;
  CSFunction  *pFunNew;
  string buf,buf1,buf2, bufname,two;
  int k, index,index1,index2;
  CSFVar * pVar;
  pFunNew = new CSFunction("LOG",GetNumInVar(),
                GetNumOutVar(),GetNumInterVar());
  k=0;
  index=0;
  while (m_Fun->GetTextLine(k,buf)) {
    index=0;
    GetNextInpName(buf,index, bufname);
    while (buf[index]!='=') index++;
    index++;
    buf1=buf.substr(0,index);
    index1=index;
    while(GetNextInpName(buf,index,bufname)!=-1){
// Переписать все, что до bufname
      buf2=buf.substr(index1,index-index1);
      index2=buf2.find(bufname)+index1;
      buf1+=buf2.substr(0,index2-index1);
// Есть ли имя в списке соответствий имен?
      //if (MultiPare->Lookup(bufname,two)==0){
      ii = MultiPare->find(bufname); 
      if (ii == MultiPare->end()) {   // Если нет, то переписать имя в уравнение
        buf1+=bufname;
      }
      else { // Если есть, то заменить на  Two
        two = MultiPare->at(bufname); 
        buf1+=two;
      }
      index1=index;
    }
    buf1+=buf.erase(0,index);    //.Right(buf.GetLength()-index);
    pFunNew->AddTextLine(buf1);
  }
  SetFun(pFunNew);
//Просмотреть переменные и удалить те, что заменены
  for (k=0;k< (int)m_SFVarArray.Avar.size();k++ ){
    pVar=m_SFVarArray.GetAt(k);
    //if (MultiPare->Lookup(pVar->m_Name,two)!=0){
    ii = MultiPare->find(pVar->m_Name); 
    if (ii != MultiPare->end()) {  
      m_SFVarArray.RemoveAt(k);
      m_Fun->m_NumInt--;
      m_NumInVar--;
    }
  }
  return true;
}

