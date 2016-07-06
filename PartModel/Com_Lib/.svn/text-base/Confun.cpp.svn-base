/////////////////////////////////////////////////////////////////
// Ver.1.1.1    15.02.2005   Russian == English
/////////////////////////////////////////////////////////////////

//
//#include "stdafx.h"
#include <stdio.h>
#include <string>
#include <algorithm>
#include "../Com_Lib/confun.h"
#include "../Com_Lib/bufer.h"

#ifdef _LINUX
#include <sys/io.h>
#else
#include <io.h>
#endif
using namespace std;

//------------------------------------------------------------
int SdfInConFun(string FileCon, string FileFun, 
                 CBM *pTrMat_Zero, CBM *pTrMat_One, CBM *pMatFun)  
{ string  Con, Fun, one, two;
  int i, j, i1, i2, i3;
  char b[10];

  i1 = pTrMat_Zero->GetCountR();
  i2 = pTrMat_One->GetCountR();
  i3 = pMatFun->GetCountR();
  if (i1 !=  i2) 
    return 0;
  if (pTrMat_Zero->GetCountC() !=  pTrMat_One->GetCountC()) 
    return 0;
  if (i1 !=  i3) 
    return 0;

  try
  {
// нужно записать в файлы *.con, *.fun значения системы булевых функций   
//    CFile fcon(FileCon, CFile::modeWrite | CFile::modeCreate);
//    CFile ffun(FileFun, CFile::modeWrite | CFile::modeCreate);
    FILE *fcn,*ffn;
    char endl[3];
#ifndef _MSVC9
    endl[0] = '\n'; endl[1] = 0;
    fcn = fopen(FileCon.c_str(), "w");
    ffn = fopen(FileFun.c_str(), "w");
#else
    endl[0] = '\n'; endl[1] = 0;
    int res = fopen_s(&fcn, FileCon.c_str(), "w");
    res = fopen_s(&ffn, FileFun.c_str(), "w");
#endif
    for (i=0; i<pTrMat_Zero->GetCountR(); i++)  {
#ifndef _MSVC9
      sprintf(b,"%4d/", i+1);
#else
      sprintf_s(b,10,"%4d/", i+1);
#endif
      Con = b;
      for  (j=0; j<pTrMat_Zero->GetCountC(); j++)  {
        if (pTrMat_Zero->GetBitAt(i,j))      Con += '0';
        else if (pTrMat_One->GetBitAt(i,j))  Con += '1';
        else                                 Con += '-';
      }  //for  (j=0; j<pTrMat_Zero->GetCountR(); j++)
      Con +="/"; Con += endl;
      fwrite(Con.c_str(),Con.length(),1,fcn);
      //fcon.Write(Con, Con.GetLength());

#ifndef _MSVC9
      sprintf(b,"%4d/", i);
#else
      sprintf_s(b,10,"%4d/", i);
#endif
      Fun = b;
      for  (j=0; j<pMatFun->GetCountC(); j++)  {
        if (pMatFun->GetBitAt(i,j))      Fun += '1';
        else                             Fun += '0';
      }  //for  (j=0; j<pTrMat_Zero->GetCountR(); j++)
      Fun +="/"; Fun += endl;
      fwrite(Fun.c_str(),Fun.length(),1,ffn);
//      ffun.Write(Fun, Fun.GetLength());
    }  // for (i=0; i<pTrMat_Zero->GetCountC(); i++)
  }    
#ifndef _LINUX
  catch( CFileException e )
#else
  catch( ... )
#endif
  { 
#ifdef _LINUX
    printf("Disk error\n");
#else
    AfxMessageBox("Disk error"); 
#endif
    return 0;
  }
//  END_CATCH
  return 1;
}

//-------------------------------------------------------------
int ConFunInSdf(string FileCon, string FileFun, 
                 CBM *&pTrMat_Zero, CBM *&pTrMat_One, CBM *&pMatFun)  
{ string  Con, Fun, one, two;
  int FlagFirst= 1;
  int i;
  string buf, szComments;
  try
  {
// нужно прочесть файлы *.con, *.fun и записать их значения в матрицы   
//    CFile fcon(FileCon,CFile::modeRead);
//    DWORD dwFileSizeCon = (DWORD)fcon.GetLength();
//    CFile ffun(FileFun,CFile::modeRead);
//    DWORD dwFileSizeFun = (DWORD)ffun.GetLength();

    FILE *fcn, *ffn;

#ifndef _MSVC9
    fcn = fopen(FileCon.c_str(), "w");
    ffn = fopen(FileFun.c_str(), "w");
#else
    int res = fopen_s(&fcn, FileCon.c_str(), "r");
    res = fopen_s(&ffn, FileFun.c_str(), "r");
#endif

    long lenCon, lenFun;
//    int a = _fileno(fcn);
//    lenCon = _filelength(a);
    fseek(fcn, 0, SEEK_END);
    lenCon = ftell(fcn);
    fseek(fcn, 0L, SEEK_SET);
//    a = _fileno(ffn);
//    lenFun = _filelength(a);
    fseek(ffn, 0, SEEK_END);
    lenFun = ftell(ffn);
    fseek(ffn, 0L, SEEK_SET);

    while (GetSimpleFileLine(fcn, Con, lenCon)) {
      GetSimpleFileLine(ffn, Fun, lenFun);
      i = Con.find("/");
      if (i<1)  {
#ifdef _LINUX
        printf("Read error file temp.con\n");
#else
        AfxMessageBox("Ошибка при чтении файла temp.con",  MB_ICONSTOP);
#endif
        return 0;
      }
//      one = Con.Right(Con.GetLength()-i-1);
      one = Con.substr(i+1, Con.length()-i-1);

      i = one.find("/");
      if (i<1)  { 
#ifdef _LINUX
        printf("Read error file temp.con\n");
#else
        AfxMessageBox("Ошибка при чтении файла temp.con",  MB_ICONSTOP);
#endif
        return 0;
      }
      one = one.substr(i);
      i = Fun.find("/");
      if (i<1)  {
#ifdef _LINUX
        printf("Read error file temp.con\n");
#else
        AfxMessageBox("Ошибка при чтении файла temp.con",  MB_ICONSTOP);
#endif
        return 0;
      }
//      two = Fun.Right(Fun.GetLength()-i-1);
      two = Fun.substr(i+1, Fun.length()-i-1);

      i = two.find("/");
      if (i<1)  {
#ifdef _LINUX
        printf("Read error file temp.con\n");
#else
        AfxMessageBox("Ошибка при чтении файла temp.con",  MB_ICONSTOP);
#endif
        return 0; 
      }
      two = two.substr(i);
      if (FlagFirst)  {
        pTrMat_Zero = new CBM(1, one.length(), 0);
        pTrMat_One  = new CBM(1, one.length(), 0);
        pMatFun     = new CBM(1, two.length(), 0);
        FlagFirst = 0;
      }
      else  {  // add new row
        pTrMat_Zero->Add(*pTrMat_Zero, 0);
        pTrMat_One->Add(*pTrMat_One, 0);
        pMatFun->Add(*pMatFun, 0);
      }
      for (i=0; i<(int)one.length(); i++)  {
        if (one[i] == '0')  {
          pTrMat_Zero->SetBitAt(pTrMat_Zero->GetCountR()-1, i, 1);
          pTrMat_One->SetBitAt(pTrMat_One->GetCountR()-1, i, 0);
        }
        else if (one[i] == '1')  {
          pTrMat_Zero->SetBitAt(pTrMat_Zero->GetCountR()-1, i, 0);
          pTrMat_One->SetBitAt(pTrMat_One->GetCountR()-1, i, 1);
        }
        else  {
          pTrMat_Zero->SetBitAt(pTrMat_Zero->GetCountR()-1, i, 0);
          pTrMat_One->SetBitAt(pTrMat_One->GetCountR()-1, i, 0);
        }
      }  // for (i=0, i<one.GetLength(); i++)
      for (i=0; i<(int)two.length(); i++) 
        if (two[i] == '1') pMatFun->SetBitAt(pMatFun->GetCountR()-1, i, 1);
        else               pMatFun->SetBitAt(pMatFun->GetCountR()-1, i, 0);
    }   //while (GetSimpleFileLine(&fcon, Con, dwFileSizeCon))
  }    
#ifndef _LINUX
  catch( CFileException e )
#else
  catch( ... )
#endif
  { 
#ifdef _LINUX
    printf("Disk error\n");
#else
    AfxMessageBox("Ошибка при обращении к диску"); 
#endif
    return 0;
  }
//  END_CATCH
  return 1;
}
