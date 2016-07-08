/////////////////////////////////////////////////////////////////
// Ver.1.1.0    20.03.2005   Russian / English 
//              - выделены отдельно методы вывода SF-файлов
// Ver.1.1.1    07.04.2005 
// Ver.1.1.6    16.05.2005 - убрана очистка массива в Write_SF
/////////////////////////////////////////////////////////////////
// Методы класса   CSF:
//      bool CSF::WriteSF(string FileName)
//      bool CSF::WriteSF(vector <string>& Obj)
//      void CSF::WriteVar(CFile& f, BYTE Typ, CSFVarArray *pVA)
//      void CSF::WriteVar(CStringArray& Obj, BYTE Typ, CSFVarArray *pVA)

//#include <io.h>
#include <exception>
#include <vector>
#include <map>

using namespace std;
#include "../Com_Lib/objects.h"
#include "../Com_Lib/archive.h"
#include "../Com_Lib/sf.h"
#include "../Com_Lib/bufer.h"
#include "../Common/BaseBool.h"
#include "../Common/BaseTern.h"


//#ifdef _LINUX
#undef _DEBUG
typedef unsigned char BYTE; 
typedef unsigned long ULONG; 
//typedef bool BOOL;

#define ASSERT assert
#include <assert.h>
/*
#else
#include <afx.h>
#include <afxwin.h>
#pragma warning(disable : 4996)
#endif
*/


//---------------------------------------------------------------
bool CSF::WriteSF(string FileName)
{
  string Buf, Buf1, Buf0, endl;
  string one, two;
//  POSITION pos;
  map<string,string>::iterator pos;
  int i, j;
  CSFunction *pFun;
  endl = "\n";
FILE* f;
  try
  {
    f = fopen(FileName.c_str(),"a");
    Buf = "TITLE "+GetTitle();
    if (m_TitleComments != "")  {
      Buf+= "  /*";
      Buf+= m_TitleComments;
      Buf += "*/";
    }
    Buf += endl;
    fwrite(Buf.c_str(), Buf.size(),1,f);

    Buf = "FORMAT  SF"+endl;
    fwrite(Buf.c_str(), Buf.size(),1,f);

    Buf = "AUTHOR "+GetAuthor();
    if (m_AuthorComments != "")  {
      Buf+= " /*";
      Buf+= m_AuthorComments;
      Buf += "*/";
    }
    Buf +=  endl;
    fwrite(Buf.c_str(), Buf.size(),1,f);

    Buf = "DATE "+GetDate();
    if (m_DateComments != "")  {
      Buf+= " /*";
      Buf+= m_DateComments;
      Buf += "*/";
    }
    Buf +=  endl;
    fwrite(Buf.c_str(), Buf.size(),1,f);

    Buf = "PROJECT "+GetProject();
    if (m_ProjectComments != "")  {
      Buf+= " /*";
      Buf+= m_ProjectComments;
      Buf += "*/";
    }
    Buf +=  endl;
    fwrite(Buf.c_str(), Buf.size(),1,f);

    Buf = "DCL_PIN ";
    if (m_SFVarComments != "")  {
      Buf+= " /*";
      Buf+= m_SFVarComments;
      Buf += "*/";
    }
    Buf += endl+ "EXT";
    Buf += endl+ "INP" +endl;
    fwrite(Buf.c_str(), Buf.size(),1,f);

    WriteVar(f,'i', &m_SFVarArray);
    Buf = "OUT" +endl;
    fwrite(Buf.c_str(), Buf.size(),1,f);

    WriteVar(f, 'o', &m_SFVarArray);

    Buf = "INTER"+endl;
    fwrite(Buf.c_str(), Buf.size(),1,f);
    WriteVar(f, 't', &m_SFVarArray);
    Buf = endl;
    if (GetFirstPareInter(pos, one, two))  {
      Buf = one + "=" + two + endl;
      fwrite(Buf.c_str(), Buf.size(),1,f);
      while (GetNextPareInter(pos, one, two))  {
        Buf = one + "=" + two +endl;
        fwrite(Buf.c_str(), Buf.size(),1,f);
      }
    }
    Buf = "END_PIN"+endl;
    fwrite(Buf.c_str(), Buf.size(),1,f);

    CSFType * pType;
    if (GetFirstType(pType))  {
      Buf = "DCL_EL"+endl;

      if (m_TypeComments != "")  {
        Buf+= " /*";
        Buf+= m_TypeComments;
        Buf += "*/"+endl;
      }
      fwrite(Buf.c_str(), Buf.size(),1,f);

      do  {
        if (pType->GetReg())
          Buf = "TYPE_N " + pType->GetName() +endl;
        else
          Buf = "TYPE " + pType->GetName() +endl;
        fwrite(Buf.c_str(), Buf.size(),1,f);
        Buf = "";
        for (int i=0; i<(int)pType->m_ElemArray.size(); i++)  {
          Buf += pType->m_ElemArray.at(i);
          Buf += " ";
        }
        Buf += endl + "INP" + endl;
        fwrite(Buf.c_str(), Buf.size(),1,f);

        WriteVar(f,'i', &(pType->m_SFVarArray));
        Buf = "OUT"+endl;
        fwrite(Buf.c_str(), Buf.size(),1,f);
        WriteVar(f, 'o', &(pType->m_SFVarArray));

        if (pType->GetLib() != "")  {
          Buf = "LIB " + pType->GetLib() + endl;
          fwrite(Buf.c_str(), Buf.size(),1,f);
        }
      }  while (GetNextType(pType));
      Buf = "END_EL" + endl;
      fwrite(Buf.c_str(), Buf.size(),1,f);
    }

    i = 0;
    CSFConnect *pCon = GetConnect(i);
    if (pCon != NULL)  {
      Buf = "CONNECT ";
      if (m_ConnectComments != "")  {
        Buf+= " /*";
        Buf+= m_ConnectComments;
        Buf += "*/";
      }
      Buf += endl;
      fwrite(Buf.c_str(), Buf.size(),1,f);

      for (pCon = GetConnect(i=0); pCon != NULL; pCon = GetConnect(i)) {
        i++;
        Buf = pCon->GetName() + endl;
        fwrite(Buf.c_str(), Buf.size(),1,f);

        Buf = "";
        if (pCon->GetFirstPare(one, two))  {
          while (1)  {
            Buf += one + "=" + two + " ";
            if (Buf.size() >70)  {
              Buf += endl;
              fwrite(Buf.c_str(), Buf.size(),1,f);
              Buf = "";
            }
            //if (!pos)  break;
            if (pCon->GetNextPare(one, two)==false) break;
          }
        }
        else        Buf = " ";
        if (Buf.size() >0)  {
          Buf += endl;
          fwrite(Buf.c_str(), Buf.size(),1,f);
          Buf = "";
        }
      }
      Buf = "END_CONNECT" + endl;
      fwrite(Buf.c_str(), Buf.size(),1,f);
    }  //  if (pCon != NULL)

    if (GetFirstPareNet(pos, one, two))  {
      Buf = "NET";
      if (m_NetComments != "")  {
        Buf+= " /*";
        Buf+= m_NetComments;
        Buf += "*/";
      }
      Buf += endl;
      fwrite(Buf.c_str(), Buf.size(),1,f);
      do  {
        Buf = one + "=" + two + endl;
        fwrite(Buf.c_str(), Buf.size(),1,f);
      }  while (GetNextPareNet(pos, one, two));
      Buf = "END_NET" + endl;
      fwrite(Buf.c_str(), Buf.size(),1,f);
    }

    if ( (pFun = GetFun()) != NULL )  {
      Buf = "FUNCTION ";
      if (m_FunComments != "")  {
        Buf+= " /*";
        Buf+= m_FunComments;
        Buf += "*/";
      }
      Buf += endl;
      Buf += pFun->GetTyp();
      Buf += endl;
      char b[100];

#ifndef _MSVC9
      sprintf(b,"%u  %u  %u",pFun->GetNumInt(),pFun->GetNumOut(),pFun->GetNumInter());
#else
      sprintf_s(b,100, "%u  %u  %u",pFun->GetNumInt(),pFun->GetNumOut(),pFun->GetNumInter());
#endif

/*      _itoa_s(pFun->GetNumInt(), b, 10,10);
      Buf += b;
      Buf += "  ";
      _itoa_s(pFun->GetNumOut(), b, 10, 10);
      Buf += b;
      Buf += "  ";
      _itoa_s(pFun->GetNumInter(), b, 10, 10);
*/

      Buf += b;
      Buf += endl;
      fwrite(Buf.c_str(), Buf.size(),1,f);

      for (i=0; ; )   {
        if (!pFun->GetTextLine(i,Buf))  break;

        while (Buf.size() > 70)  {
          Buf0 = Buf;
          Buf0.erase(0,60);     //.Right(Buf.GetLength() - 60);
          j = Buf0.find_first_of("+*");
          if (j < 0)  break;
          
          Buf1 = Buf.substr(0, 60+j);
          Buf.erase(0,j+60);   //.Right(Buf.GetLength() - 60 - j);
          Buf1 += endl;
          fwrite(Buf1.c_str(), Buf1.size(),1,f);
        }

        Buf += endl;
        fwrite(Buf.c_str(), Buf.size(),1,f);
      }

      Buf = "END_";
      Buf += pFun->GetTyp();
      Buf += endl + "END_FUNCTION" + endl;
      fwrite(Buf.c_str(), Buf.size(),1,f);
    }

    Buf = "END_";
    Buf += GetTitle();
//    Buf += endl+endl;
    Buf += endl;

    fwrite(Buf.c_str(), Buf.size(),1,f);
  }
/*
#ifndef _LINUX
  catch( CFileException e )
#else
*/
  catch( ... )
//#endif
  {
//#ifdef _LINUX
    printf("Error at record of a file %s\n",FileName.c_str());
/*
#else
#ifdef ENG
    Buf="Error at record of a file " + FileName;
#else
    Buf="Ошибка при записи файла " + FileName;
#endif
    AfxMessageBox(Buf.c_str());
#endif
*/
    return false;
  }
//  END_CATCH
fclose(f);
  return true;
}

//---------------------------------------------------------------
bool CSF::WriteSF(vector <string>& Obj)
{
  string Buf, Buf1, Buf0;
  string one, two;
  map<string,string>::iterator pos;
//  POSITION pos;
  int i, j;
  CSFunction *pFun;

//  Obj.RemoveAll();            // 16.05.2006
  Buf = "TITLE "+GetTitle();
  if (m_TitleComments != "")  {
    Buf+= "  /*";
    Buf+= m_TitleComments;
    Buf += "*/";
  }
  Obj.push_back(Buf);

  Buf = "FORMAT  SF";
  Obj.push_back(Buf);
  Buf = "AUTHOR "+GetAuthor();
  if (m_AuthorComments != "")  {
    Buf+= " /*";
    Buf+= m_AuthorComments;
    Buf += "*/";
  }
  Obj.push_back(Buf);
  Buf = "DATE "+GetDate();
  if (m_DateComments != "")  {
    Buf+= " /*";
    Buf+= m_DateComments;
    Buf += "*/";
  }
  Obj.push_back(Buf);
  Buf = "PROJECT "+GetProject();
  if (m_ProjectComments != "")  {
    Buf+= " /*";
    Buf+= m_ProjectComments;
    Buf += "*/";
  }
  Obj.push_back(Buf);
  Buf = "DCL_PIN ";
  if (m_SFVarComments != "")  {
    Buf+= " /*";
    Buf+= m_SFVarComments;
    Buf += "*/";
  }
  Obj.push_back(Buf);
  Obj.push_back("EXT");
  Obj.push_back("INP");
  WriteVar(Obj,'i', &m_SFVarArray);
  Obj.push_back("OUT");
  WriteVar(Obj, 'o', &m_SFVarArray);
  Obj.push_back("INTER");
  WriteVar(Obj, 't', &m_SFVarArray);
  if (GetFirstPareInter(pos, one, two))  {
    Buf = one + "=" + two;
    Obj.push_back(Buf);
    while (GetNextPareInter(pos, one, two))  {
      Buf = one + "=" + two;
      Obj.push_back(Buf);
    }
  }
  Obj.push_back("END_PIN");

  CSFType * pType;
  if (GetFirstType(pType))  {
    Buf="DCL_EL";
    if (m_TypeComments != "")  {
       Buf+= " /*";
       Buf+= m_TypeComments;
       Buf += "*/";
    }
    Obj.push_back(Buf);
    do  {
      if (pType->GetReg())
        Buf = "TYPE_N " + pType->GetName();
      else
        Buf = "TYPE " + pType->GetName();
      Obj.push_back(Buf);
      Buf = "";
      for (int i=0; i<(int)pType->m_ElemArray.size(); i++)  {
        Buf += pType->m_ElemArray.at(i);
        Buf += " ";
      }
      Obj.push_back(Buf);
      Obj.push_back("INP");
      WriteVar(Obj,'i', &(pType->m_SFVarArray));
      Obj.push_back("OUT");
      WriteVar(Obj, 'o', &(pType->m_SFVarArray));
      if (pType->GetLib() != "")  {
        Buf = "LIB " + pType->GetLib();
        Obj.push_back(Buf);
      }
    }  while (GetNextType(pType));
    Obj.push_back("END_EL");
  }

  i = 0;
  CSFConnect *pCon = GetConnect(i);
  if (pCon != NULL)  {
    Buf = "CONNECT ";
    if (m_ConnectComments != "")  {
      Buf+= " /*";
      Buf+= m_ConnectComments;
      Buf += "*/";
    }
    Obj.push_back(Buf);
    for (pCon = GetConnect(i=0); pCon != NULL; pCon = GetConnect(i)) {
      i++;
      Buf = pCon->GetName();
      Obj.push_back(Buf);
      Buf = "";
      if (pCon->GetFirstPare(one, two))  {
        while (1)  {
          Buf += one + "=" + two + " ";
          if (Buf.size() >70)  {
            Obj.push_back(Buf);
            Buf = "";
          }
//          if (!pos)  break;
          if (pCon->GetNextPare(one, two)==false) break;
        }
      }
      else        Buf = " ";
      if (Buf.size() >0)  {
        Obj.push_back(Buf);
        Buf = "";
      }
    }
    Obj.push_back("END_CONNECT");
  }  //  if (pCon != NULL)

  if (GetFirstPareNet(pos, one, two))  {
    Buf = "NET";
    if (m_NetComments != "")  {
      Buf+= " /*";
      Buf+= m_NetComments;
      Buf += "*/";
    }
    Obj.push_back(Buf);
    do  {
      Buf = one + "=" + two;
      Obj.push_back(Buf);
    }  while (GetNextPareNet(pos, one, two));
    Obj.push_back("END_NET");
  }

  if ( (pFun = GetFun()) != NULL )  {
    Buf = "FUNCTION ";
    if (m_FunComments != "")  {
      Buf+= " /*";
      Buf+= m_FunComments;
      Buf += "*/";
    }
    Obj.push_back(Buf);
    Buf="";
    Buf += pFun->GetTyp();
    Obj.push_back(Buf);
    Buf="";
    char b[100];

#ifndef _MSVC9
      sprintf(b,"%u  %u  %u",pFun->GetNumInt(),pFun->GetNumOut(),pFun->GetNumInter());
#else
      sprintf_s(b,100, "%u  %u  %u",pFun->GetNumInt(),pFun->GetNumOut(),pFun->GetNumInter());
#endif

/*    _itoa_s(pFun->GetNumInt(), b, 10, 10);
    Buf += b;
    Buf += "  ";
    _itoa_s(pFun->GetNumOut(), b, 10, 10);
    Buf += b;
    Buf += "  ";
    _itoa_s(pFun->GetNumInter(), b, 10, 10);
*/
    Buf += b;
    Obj.push_back(Buf);

    for (i=0; ; )   {
      if (!pFun->GetTextLine(i,Buf))  break;

      while (Buf.size() > 70)  {
        Buf0 = Buf;
        Buf0.erase(0,60);       //Right(Buf.GetLength() - 60);
        j = Buf0.find_first_of("+*");
        if (j < 0)  break;

        Buf1 = Buf.substr(0,60+j);
        Buf.erase(0,j+60); // = Buf.Right(Buf.GetLength() - 60 - j);
        Obj.push_back(Buf1);
      }
      Obj.push_back(Buf);
    }

    Buf = "END_";
    Buf += pFun->GetTyp();
    Obj.push_back(Buf);
    Obj.push_back("END_FUNCTION");
  }

  Buf = "END_";
  Buf += GetTitle();
  Obj.push_back(Buf);
  Obj.push_back("");
  return TRUE;
}

//---------------------------------------------------------------
void CSF::WriteVar(FILE* f, BYTE Typ, CSFVarArray *pVA)
{
  string  Buf = "", endl;
  CSFVar *pVar;
  char b[10];
  endl = "\n";

  for (int i=0; i< (int) pVA->Avar.size(); i++)  {
    pVar = (CSFVar *)pVA->Avar[i];
    if (pVar->GetTyp() != Typ)  continue;
    Buf +=pVar->GetName();
    if (pVar->GetLast() >0)  {
      Buf +="[";
#ifndef _MSVC9
      sprintf(b, "%u", pVar->GetFirst());
#else
      _itoa_s(pVar->GetFirst(),b,10,10);
#endif

      Buf += b;
      Buf += "-";
#ifndef _MSVC9
      sprintf(b, "%u", pVar->GetLast());
#else
      _itoa_s(pVar->GetLast(),b,10,10);
#endif
      Buf += b;
      Buf += "]";
    }  // if (pVar->GetLast() >0)
    Buf += " ";
    if (Buf.size() >70)  {
      Buf += endl;
      fwrite(Buf.c_str(), Buf.size(),1,f);
      Buf = "";
    }
  }
  Buf += endl;
  fwrite(Buf.c_str(), Buf.size(),1,f);
}

//-----------------------------------------------------------------
void CSF::WriteVar(vector <string>& Obj, BYTE Typ, CSFVarArray *pVA)
{
  string  Buf = "";
  CSFVar *pVar;
  char b[10];

  for (int i=0; i< (int)pVA->Avar.size(); i++)  {
    pVar = (CSFVar *)pVA->GetAt(i);
    if (pVar->GetTyp() != Typ)  continue;
    Buf +=pVar->GetName();
    if (pVar->GetLast() >0)  {
      Buf +="[";
#ifndef _MSVC9
      sprintf(b, "%u", pVar->GetFirst());
#else
      _itoa_s(pVar->GetFirst(),b,10,10);
#endif

      Buf += b;
      Buf += "-";
#ifndef _MSVC9
      sprintf(b, "%u", pVar->GetLast());
#else
      _itoa_s(pVar->GetLast(),b,10,10);
#endif
      Buf += b;
      Buf += "]";
    }  // if (pVar->GetLast() >0)
    Buf += " ";
    if (Buf.length() >70)  {
      Obj.push_back(Buf);
      Buf = "";
    }
  }
  Obj.push_back(Buf);
}
