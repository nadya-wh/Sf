/////////////////////////////////////////////////////////////////
// Ver.1.1.1    15.02.2005   Russian / English
//              - выделены отдельно методы ввода SF-файлов
// Ver.1.1.2    17.03.2006   Russian / English
//              - поправка форматирования, частичные функции
// Ver.1.1.3    07.04.2006
//     - два метода перенесены из файла SFKir и частично изменены
// Ver.1.1.4    04.05.2006
//     - добавлен метод ReadNextSF(CStringArray& Obj,...
// Ver.1.1.5    26.10.2007
//     - поправка расчета длины файла при наличии комментария в строке TITLE
//       (в функции ReadSFByName)
// Ver.1.1.6    25.06.2008
//     - добавлено выбрасывание пробелов при извлечении имени экземпляра в разделе CONNECT
/////////////////////////////////////////////////////////////////
// Методы класса   CSF:
//    bool CSF::ReadNextSF(CFile *fp, DWORD& len)
//    void CSF::InitSFAttrib()
//    int  CSF::ReadSFByName(CFile *fp, DWORD& len,string Name)
//    int  CSF::ReadSFFromLib(string Name, string PrivLib,
//         string LogLib, string Lib_BMK, string LibSTRUCTURE)
//    int  CSF::ReadSFFromFile(string PathToDir)
//    bool CSF::ReadNextSF(CStringArray& Obj, int & index)
/////////////////////////////////////////////////////////////////

#include <stdio.h>
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
#include <io.h>
#undef _DEBUG
typedef unsigned char BYTE; 
typedef unsigned long ULONG; 
typedef int BOOL;

#define ASSERT assert
#include <assert.h>
/*
#else
#include <io.h>
#include <afx.h>
#include <afxwin.h>
#pragma warning(disable : 4996)
#endif
*/

void InvalidMessage(string s1, string s2);

void InvalidMessage(string s1, string s2)
{
  string w = s1+s2;
/*
#ifdef _LINUX
*/
  printf("Error: %s\n", w.c_str());
/*
#else
  AfxMessageBox(w.c_str());
#endif
*/
}

//---------------------------------------------------------------
bool CSF::ReadNextSF(FILE* fp, long& len)
{
  string buf, buf1, buf2, bufname, comments, InvalidSect;
  int i, nRes, pos, first, last,NumInSF;
  CSFConnect *pConnect;
  bool bRes;                                     

#ifdef ENG
  InvalidSect = "Wrong section ";
#else
  InvalidSect = "Неверный раздел ";
#endif

  if (GetFileLine(fp, buf,len, comments) !=0 )  {           //TITLE
    if ((i = buf.find("TITLE")) >=0)  {
      for (i+=5; buf[i] == ' '; i++);
      m_Title = buf.erase(0, i); //Right(buf.size() - i);
      m_Title = BeginEndTrim(m_Title);
      //m_Title.TrimLeft();
      //m_Title.TrimRight();
    }
    else  {
      InvalidMessage(InvalidSect, "TITLE");
      return false;
    }
  }
  else  {
    InvalidMessage(InvalidSect, "TITLE");
    return false;
  }

  if (GetFileLine(fp, buf,len, comments) !=0 )  {           // FORMAT
    if (((int)buf.find("FORMAT")) >=0)  {
      if ((int)buf.find("SF")<0)  {
        InvalidMessage(InvalidSect, "FORMAT");
        return false;
      }
    }
  }
  else  {
    InvalidMessage(InvalidSect, "FORMAT");
    return false;
  }

  if (GetFileLine(fp, buf,len, comments) !=0 )  {           //AUTHOR
    if ((i=buf.find("AUTHOR")) >=0)  {
      m_TitleComments = comments;
      comments = "";
      for ( i+=6; i<(int)buf.size() && buf[i] == ' '; i++);
      m_Author = buf.erase(0,i); // Right(buf.size() - i);
    }
    else  {
      InvalidMessage(InvalidSect, "AUTHOR");
      return false;
    }
  }
  else  {
    InvalidMessage(InvalidSect, "AUTHOR");
    return false;
  }

  if (GetFileLine(fp, buf,len, comments) !=0 )  {           //DATE
    if ((i=buf.find("DATE")) >=0)  {
      m_AuthorComments = comments;
      comments = "";
      for (i+=4; i<(int)buf.size() && buf[i] == ' '; i++);
      m_Date = buf.erase(0,i); // Right(buf.size() - i);
    }
    else  {
      InvalidMessage(InvalidSect, "DATE");
      return false;
    }
  }
  else  {
    InvalidMessage(InvalidSect, "DATE");
    return false;
  }

  if (GetFileLine(fp, buf,len, comments) !=0 )  {           //PROJECT
    if ((i=buf.find("PROJECT")) >=0)  {
      m_DateComments = comments;
      comments = "";
      if (buf.size()>8) {
        for (i+=8; i<(int)buf.size() && buf[i] == ' '; i++);
        m_Project = buf.erase(0,i); // Right(buf.size() - i);
      }
    }
    else  {
      InvalidMessage(InvalidSect, "PROJECT");
      return false;
    }
  }
  else {
    InvalidMessage(InvalidSect, "PROJECT");
    return false;
  }
  
  if (GetFileLine(fp, buf,len, comments) !=0 )  {           //DCL_PIN
    if ((i = buf.find("DCL_PIN")) <0) {
      InvalidMessage(InvalidSect, "DCL_PIN");
      return false;
    }
    m_ProjectComments = comments;
    comments = "";
  }
  else  {
    InvalidMessage(InvalidSect, "DCL_PIN");
    return false;
  }

  if (GetFileLine(fp, buf,len, comments) !=0 )  {           //EXT
    if ((i = buf.find("EXT")) <0)  {
      InvalidMessage(InvalidSect, "EXT");
      return false;
    }
  }
  else  {
    InvalidMessage(InvalidSect, "EXT");
    return false;
  }

  if (GetFileLine(fp, buf,len, comments) !=0 )  {           //INP
    if ((i = buf.find("INP")) <0)  {
      InvalidMessage(InvalidSect, "INP");
      return false;
    }
  }
  else  {
    InvalidMessage(InvalidSect, "INP");
    return false;
  }
  NumInSF=0;  // 25.02.00 правила Кириенко, чтобы формировался номер m_NumInSF;
  inp_var:
  if (GetFileLine(fp, buf,len, comments) !=0 )  {           //INP
    pos =0;
    while((i = buf.find("OUT")) < 0)  {              // read all input variables
      nRes = GetNextName(buf,pos, bufname);
      if (nRes ==0)  goto inp_var;
      first = last = 0;
      if (pos<(int)buf.size() && buf[pos] == '[')  {    // this variable has interval
        nRes = GetNextName(buf,pos, buf1);
        if (nRes ==0)  {
          InvalidMessage(InvalidSect, "OUT");
          return false;
        }
        first = atoi(buf1.c_str());
        nRes = GetNextName(buf,pos, buf2);
        if (nRes ==0)  {
          InvalidMessage(InvalidSect, "OUT");
          return false;
        }
        last = atoi(buf2.c_str());
      }            //  if (buf[pos] == '[')  
      CSFVar *pVar = new CSFVar(bufname, 'i',first,last);
      pVar->m_NumInSF=NumInSF;
      NumInSF++;
      AddVar(pVar);
    }              //  while(i = buf.find("OUT") <0)
  }                //  if (GetFileLine(fp, buf,len) !=0 )
  else  {
    InvalidMessage(InvalidSect, "OUT");
    return false;
  }

out_var:
  if (GetFileLine(fp, buf,len, comments) !=0 )  {           //OUT
    pos =0;
    while((i = buf.find("INTER")) < 0 )    { // read all output variables
      nRes = GetNextName(buf,pos, bufname);
      if (nRes ==0)  goto out_var;
      first = last = 0;
      if (pos<(int)buf.size() && buf[pos] == '[')  {    // this variable has interval
        nRes = GetNextName(buf,pos, buf1);
        if (nRes ==0)  {
          InvalidMessage(InvalidSect, "INTER");
          return false;
        }  
        first = atoi(buf1.c_str());
        nRes = GetNextName(buf,pos, buf2);
        if (nRes ==0)  {
          InvalidMessage(InvalidSect, "INTER");
          return false;
        }  
        last = atoi(buf2.c_str());
      }            //  if (buf[pos] == '[')  
      CSFVar *pVar = new CSFVar(bufname, 'o',first,last);
      pVar->m_NumInSF=NumInSF;
      NumInSF++;
      AddVar(pVar);
    }              //  while(i = buf.find("INTER") <0)
  }
  else  {
    InvalidMessage(InvalidSect, "INTER");
    return false;
  }

inter:
  if (GetFileLine(fp, buf,len, comments) !=0 )  {           //INTER
    pos =0;
    while((i = buf.find("END_PIN")) < 0)  {     // read all output variables
      if ((i = buf.find("=")) < 0)  {
        nRes = GetNextName(buf,pos, bufname);
        if (nRes ==0)  goto inter;
        first = last = 0;
        if (pos<(int)buf.size() && buf[pos] == '[')  {  // this variable has interval
          nRes = GetNextName(buf,pos, buf1);
          if (nRes ==0)  {
            InvalidMessage(InvalidSect, "INTER");
            return false;
          }
          first = atoi(buf1.c_str());
          nRes = GetNextName(buf,pos, buf2);
          if (nRes ==0)  {
            InvalidMessage(InvalidSect, "INTER");
            return false;
          }
          last = atoi(buf2.c_str());
        }            //  if (buf[pos] == '[')  
        CSFVar *pVar = new CSFVar(bufname, 't',first,last);
        pVar->m_NumInSF=NumInSF;
        NumInSF++;
        AddVar(pVar);
      }              //if ((i = buf.find("=")) < 0) 
      else {
				buf = BeginEndTrim(buf);
				i = buf.find("=");	
				buf1 = buf.substr(0,i);
        buf2 = buf.erase(0,i+1); //Right(buf.size()-i-1);
        AddPareInter(buf1, buf2);
        goto inter;
      }
    }              //  while(i = buf.find("END_PIN") <0)
  }
  else  {
    InvalidMessage(InvalidSect, "END_PIN");
    return false;
  }
  m_SFVarComments = comments;
  comments = "";

  while (GetFileLine(fp, buf,len, comments) !=0 )  {
    if ((i = buf.find("DCL_EL")) >= 0)  {             //DCL_EL 
      CSFType *pSFType;
      if (GetFileLine(fp, buf,len, comments) !=0 )  {          
        while((i = buf.find("END_EL")) < 0)  {      // read all output variables
          pos = 0;
          nRes = GetNextName(buf,pos, bufname);
          if (bufname == "TYPE_N")
            bRes = true;
          else if (bufname == "TYPE") 
            bRes = false;
          else  {
            InvalidMessage(InvalidSect, "TYPE");
            return false;
          }
          nRes = GetNextName(buf,pos, buf1);
          if (buf1 == "") {
            InvalidMessage(InvalidSect, "TYPE");
            return false;
          }
          pSFType = new CSFType(buf1,bRes);
          AddType(pSFType);
          NumInSF=0;
          if (GetFileLine(fp, buf,len, comments) ==0 )  {
            InvalidMessage(InvalidSect, "TYPE");
            return false;
          }
          pos=0;
          while (1)  {
            i = GetNextNameWithAdd(fp, len,comments, buf, pos, bufname);
            if (i==0)  {
              InvalidMessage(InvalidSect, "TYPE");
              return false;
            }
            if (bufname == "INP")  break;
            AddElemForType(pSFType, bufname);
          }  //   while (1)
          while (1)  {
            i = GetNextNameWithAdd(fp, len,comments, buf, pos, bufname);
            if (i==0)  {
              InvalidMessage(InvalidSect, "TYPE");
              return false;
            }
            if (bufname == "OUT")  break;
            CSFVar *pVar = new CSFVar(bufname, 'i',0,0);
            pVar->m_NumInSF=NumInSF;
            NumInSF++;
            pSFType->AddVar(pVar);
          }  //   while (1)
          NumInSF=0;
          while (1)  {
            i = GetNextNameWithAdd(fp, len,comments, buf, pos, bufname);
            if (i==0)  {
              InvalidMessage(InvalidSect, "TYPE");
              return false;
            }
            if (bufname == "LIB" ||bufname == "TYPE" ||bufname == "TYPE_N" ||
                bufname == "END_EL" )  break;
            CSFVar *pVar = new CSFVar(bufname, 'o',0,0);
            pVar->m_NumInSF=NumInSF;
            NumInSF++;
            pSFType->AddVar(pVar);
          }  //   while (1)
          if (bufname == "LIB")  {
            i = GetNextNameWithAdd(fp, len,comments, buf, pos, bufname);
            if (i==0)  {
              InvalidMessage(InvalidSect, "TYPE");
              return false;
            }
            pSFType->SetLib(bufname);
            if (GetFileLine(fp, buf,len, comments) ==0 ) {
              InvalidMessage(InvalidSect, "TYPE");
              return false;
            }
          }
        }    //  while((i = buf.find("END_EL")) < 0)
      }
      else  {
        InvalidMessage(InvalidSect, "TYPE");
        return false;
      }
      m_TypeComments = comments;
      comments = "";
      if (GetFileLine(fp, buf,len, comments) ==0 )  {
        InvalidMessage(InvalidSect, "TYPE");
        return false;
      }
    }  // if ((i = buf.find("DCL_EL")) >= 0)

    if ((i = buf.find("CONNECT")) >= 0)  {             //CONNECT 
      if (GetFileLine(fp, buf,len, comments) ==0 )  {
        InvalidMessage(InvalidSect, "CONNECT");
        return false;
      }
      buf = BeginEndTrim(buf);
      //buf.TrimLeft();  // 25.06.2008
      //buf.TrimRight();
      bufname = buf;
new_connect:
      pConnect = new CSFConnect(bufname);
      AddConnect(pConnect);

      if (GetFileLine(fp, buf,len, comments) ==0 )  {
        InvalidMessage(InvalidSect, "CONNECT");
		return false;
	  }
        pos = 0;
        while (1)  {
          buf2 = "";
          GetNextPartString(fp, len,comments, buf, pos, bufname);
          if ((int)buf.find("END_CONNECT")>=0)  break;
          if (pos>=(int)buf.size() ||  buf[pos] != '=') 
            goto new_connect;
new_connect_right:
          GetNextPartString(fp, len,comments, buf, pos, buf1);
          if (buf2 == "")  buf2 += buf1;
          else buf2 = (buf2 + "=" + buf1);
          if (pos < (int)buf.size() && buf[pos] == '=')  goto new_connect_right;
          if (bufname != "" || buf2 != "")
          pConnect->AddPare(bufname, buf2);
          bufname = "";
          buf2 = "";
       // if (pos >= buf.size()) goto new_connect;
        }   //  while (1)
        m_ConnectComments = comments;
        comments = "";
        if (GetFileLine(fp, buf,len, comments) ==0 )  {
          InvalidMessage(InvalidSect, "CONNECT");
          return false;
        }
      
    }   // if ((i = buf.find("CONNECT")) >= 0)

    if ((i = buf.find("NET")) >= 0)  {             //NET 
      pos =0;
      if (GetFileLine(fp, buf,len, comments) ==0 )  {
        InvalidMessage(InvalidSect, "NET");
        return false;
      }
      while((i = buf.find("END_NET")) < 0)  {
        if ((i = buf.find("=")) < 0)  {
          InvalidMessage(InvalidSect, "NET");
          return false;
        }
				buf = BeginEndTrim(buf);
				i = buf.find("=");	
        buf1 = buf.substr(0,i);
        buf2 = buf.erase(0,i+1); //Right(buf.size()-i-1);
        AddPareNet(buf1, buf2);
        if (GetFileLine(fp, buf,len, comments) ==0 ) {
          InvalidMessage(InvalidSect, "NET");
          return false;
        }
      }              //  while(i = buf.find("END_PIN") <0)
      m_NetComments = comments;
      comments = "";
      if (GetFileLine(fp, buf,len, comments) ==0 )  {
        InvalidMessage(InvalidSect, "NET");
        return false;
      }
    }         //  if ((i = buf.find("NET")) >= 0)

    if ((i = buf.find("FUNCTION")) >= 0)  {             //FUNCTION 
      int num_in;
      int num_out;
      int num_inter;
      if (GetFileLine(fp, buf,len, comments) ==0 )  goto err_func;
      if ((i = buf.find("LOG")) >= 0)  buf1= "LOG";           //LOG
      else                             buf1= "SDF";           //SDF
      if (GetFileLine(fp, buf,len, comments) ==0 ) goto err_func;
      pos = 0;
      i = GetNextNameWithAdd(fp, len,comments, buf, pos, bufname);
      if (i==0)  goto err_func;
      num_in = atoi(bufname.c_str());
      i = GetNextNameWithAdd(fp, len,comments, buf, pos, bufname);
      if (i==0)  goto err_func;
      num_out = atoi(bufname.c_str());
      i = GetNextNameWithAdd(fp, len,comments, buf, pos, bufname);
      if (i==0)  goto err_func;
      num_inter = atoi(bufname.c_str());
      if (/* num_in < 1 ||*/  num_out < 1) {
err_func: InvalidMessage(InvalidSect, "FUNCTION");
        return false;
      }
      CSFunction *pFun = new CSFunction(buf1, num_in, num_out, num_inter);
      while(1)  {
        if (GetFileLine(fp, buf,len, comments) ==0 ) goto err_func;
        i = buf.find("END_");  
        if (i >= 0 && (int)buf.find("=")<0)  break;
        pFun->AddTextLine(buf);
      }
      SetFun(pFun);
      m_FunComments = comments;
      comments = "";
      if ((i = buf.find("END_")) < 0) {
        InvalidMessage(InvalidSect, buf);
        return false;
      }         //  if ((i = buf.find("END")) >= 0)

      if (GetFileLine(fp, buf,len, comments) ==0 )  {
        InvalidMessage(InvalidSect, buf);
        return false;
      }
      continue;
    }         //  if ((i = buf.find("FUNCTION")) >= 0)

    if ((i = buf.find("END_")) >= 0)  break;
//#ifdef _LINUX
    printf("Error in SF-description: %s\n", buf.c_str());
/*
#else
#ifdef ENG
    buf = "Error in SF-description:" + buf;
#else
    buf = "Ошибка в SF-описании: " + buf;
#endif
    AfxMessageBox(buf.c_str());
#endif
*/
    return false;
  }   // while (GetFileLine(fp, buf,len, comments) !=0 )

  InitSFAttrib();
  return true;
}

//---------------------------------------------------------------
void CSF::InitSFAttrib()
{
  CSFType * pType;
  CSFunction *pFun = GetFun();

  m_Attr.SetAt("PORT_INP", (double) GetNumInVar()); 
  m_Attr.SetAt("PORT_OUT", (double) GetNumOutVar()); 
  m_Attr.SetAt("PORT_INTER", (double) GetNumInterVar()); 
  m_Attr.SetAt("TYPE", (double) m_TypeMap.mtp.size()); 
  m_Attr.SetAt("NET", (double) m_ElemTypeMap.size()); 
  m_Attr.SetAt("CIRC_MACH", (double)COMB);
  m_Attr.SetAt("ERROR", 1);

  if ( GetFirstType(pType))  {
    do  {
      if (pType->GetReg()) {
        m_Attr.SetAt("CIRC_MACH", MACHINE);
        break;
      }
    }  while ( GetNextType(pType));
  }
  m_Attr.SetAt("BIBL",  (double)NOBIBL); 
  if (pFun != NULL && pFun->GetTyp()=="LOG")       m_Attr.SetAt("FUNCTION",FLOG);
  else if (pFun != NULL && pFun->GetTyp()=="SDF")  m_Attr.SetAt("FUNCTION", FSDF);
}

//---------------------------------------------------------------
int CSF::ReadSFByName(FILE* fp, string Name)
{
  bool bRes; 
  string Buf, buf, buf1, buf2, bufname, comments;
  int i;
  long dwPosition, len, len1; 
 
  fseek(fp, 0, SEEK_END);
  len1 = ftell(fp);
  len = len1;
  fseek(fp, 0L, SEEK_SET);
//  int fn =_fileno( fp );
//  int fn = fileno( fp );
//  len = _filelength( fn );
//  len = filelength( fn );
  
  dwPosition = ftell(fp);
 
  while (GetFileLine(fp, buf,len, comments) !=0 )  {//TITLE
    if ((i = buf.find("TITLE")) >=0)  {
      for (i+=5; buf[i] == ' '; i++);
      m_Title = buf.erase(0,i); //Right(buf.size() - i);
      m_Title = BeginEndTrim(m_Title);
      //m_Title.TrimLeft();
      //m_Title.TrimRight();
      if(m_Title==Name)  break;
    }
    dwPosition = ftell(fp); //    dwPosition = fp->GetPosition();
  }
  if (m_Title!=Name) { return 1; }  //SF-описание не найдено в файле
  
  len = len1;
//  len = _filelength( fn );
//  len=(DWORD)fp->size();  // 26.10.2007
  //fp->Seek( dwPosition, CFile::begin );
  fseek(fp,dwPosition, SEEK_SET);
  bRes = ReadNextSF(fp, len);
  if (bRes == false)  {
//#ifdef _LINUX
    printf("Error when SF-description reading\n");
/*
#else
#ifdef ENG
    Buf="Error when SF-description reading";
#else
    Buf="Ошибка при чтении SF-описания";
#endif
    AfxMessageBox(Buf.c_str());
#endif
*/
    return -1;
  }
  return 0;
}

//---------------------------------------------------------------
int CSF::ReadSFFromLib(string Name, string PrivLib, string LogLib,
                       string Lib_BMK, string LibSTRUCTURE)
{
  string Path;
  int Res;

  Path = LogLib + "\\"+Name+".sf"; 
  Res = ReadSFFromFile(Path);
  if (Res >= 0) return Res; // Найден или сбой
  
  Path = PrivLib + "\\"+Name+".sf"; 
  Res = ReadSFFromFile(Path);
  if (Res >= 0) return Res; // Найден или сбой

  Path = Lib_BMK+"\\"+Name+".sf"; 
  Res = ReadSFFromFile(Path);
  if (Res >= 0) return Res; // Найден или сбой

  Path = LibSTRUCTURE + "\\"+Name+".sf"; 
  Res = ReadSFFromFile(Path);
  if (Res >= 0) return Res; // Найден или сбой
  return -1; // SF-описание не найдено в библиотеках
}

//---------------------------------------------------------------
int CSF::ReadSFFromFile(string Path)
  // Return: 0 - normal, 1 - error at reading, -1 - file not found
{
  string Buf;
  bool bRes;
#ifdef _LINUX
  if (access(Path.c_str(),0) != -1 ){ // Файл существует?
#else
  if (_access(Path.c_str(),0) != -1 ){ // Файл существует?
#endif
    try
    {     // чтение исходного SF- описания из файла
      FILE* f;
      f = fopen(Path.c_str(), "r");
//      int fn =_fileno( f );
      long dwFileSize ;//= _filelength( fn );
      fseek(f, 0, SEEK_END);
      dwFileSize = ftell(f);
      fseek(f, 0L, SEEK_SET);

       
      bRes = ReadNextSF(f, dwFileSize);
      if (bRes == 0)  {
//#ifdef _LINUX
        printf("Error at reading SF-description %s\n", Path.c_str());
/*
#else
#ifdef ENG
        Buf = "Error at reading SF-description " + Path;
#else
        Buf = "Ошибка при чтении SF-опиcания " + Path;
#endif
        AfxMessageBox(Buf.c_str());
#endif
*/
        return 1; // Сбой при чтении SF-опиcания 
      }
    }    
/*
#ifndef _LINUX
    catch( CFileException  e )
#else
*/
    catch( ... )
//#endif
    {
//#ifdef _LINUX
      printf("Error at reading %s\n", Path.c_str());
/*
#else
#ifdef ENG
      Buf = ("Error at reading " + Path);
#else
      Buf = ("Ошибка при чтении файла " + Path);
#endif
      AfxMessageBox(Buf.c_str());
#endif
*/
      return 1; // Сбой при чтении файла 
    }
//    END_CATCH
    return 0;
  }
  return -1; // SF-описание не найдено
}
//---------------------------------------------------------------
bool CSF::ReadNextSF(vector <string>& Obj, int & index) //Кириенко 11.05.06
{
  string buf, buf1, buf2, bufname, comments, InvalidSect;
  int i, nRes, pos, first, last,NumInSF, ii, size;
  CSFConnect *pConnect;
  bool bRes;                                     

#ifdef ENG
  InvalidSect = "Wrong section ";
#else
  InvalidSect = "Неверный раздел ";
#endif
size=Obj.size();
for (ii=0; ii<size; ii++) {
if (GetLineFromStringArray(Obj, buf, index, comments)!=0 )  { //TITLE
    if ((i = buf.find("TITLE")) >=0)  {
      for (i+=5; buf[i] == ' '; i++);

      m_Title = buf;
      m_Title.erase(0,i); //.Right(buf.size() - i);
      m_Title = BeginEndTrim(m_Title);
//      m_Title.TrimLeft();
//      m_Title.TrimRight();
	  break;
    }
 }
  else  {
    InvalidMessage(InvalidSect, "TITLE");
    return false;
  }
}
if (i<0)return false;

if (GetLineFromStringArray(Obj, buf, index, comments) !=0 )  {           // FORMAT
    if (((int)buf.find("FORMAT")) >=0)  {
      if ((int)buf.find("SF")<0)  {
        InvalidMessage(InvalidSect, "FORMAT");
        return false;
      }
    }
  }
  else  {
    InvalidMessage(InvalidSect, "FORMAT");
    return false;
  }

  if (GetLineFromStringArray(Obj, buf, index, comments) !=0 )  {           //AUTHOR
    if ((i=buf.find("AUTHOR")) >=0)  {
      m_TitleComments = comments;
      comments = "";
      for ( i+=6; i<(int)buf.size() && buf[i] == ' '; i++);
      m_Author = buf.erase(0,i); //Right(buf.size() - i);
    }
    else  {
      InvalidMessage(InvalidSect, "AUTHOR");
      return false;
    }
  }
  else  {
    InvalidMessage(InvalidSect, "AUTHOR");
    return false;
  }

  if (GetLineFromStringArray(Obj, buf, index, comments) !=0 )  {           //DATE
    if ((i=buf.find("DATE")) >=0)  {
      m_AuthorComments = comments;
      comments = "";
      for (i+=4; i<(int)buf.size() && buf[i] == ' '; i++);
      m_Date = buf.erase(0,i); //Right(buf.size() - i);
    }
    else  {
      InvalidMessage(InvalidSect, "DATE");
      return false;
    }
  }
  else  {
    InvalidMessage(InvalidSect, "DATE");
    return false;
  }

  if (GetLineFromStringArray(Obj, buf, index, comments)!=0 )  { //PROJECT
    if ((i=buf.find("PROJECT")) >=0)  {
      m_DateComments = comments;
      comments = "";
      if (buf.size()>8) {
        for (i+=8; i<(int) buf.size() && buf[i] == ' '; i++);
        m_Project = buf.erase(0,i);
      }
    }
    else  {
      InvalidMessage(InvalidSect, "PROJECT");
      return false;
    }
  }

  if (GetLineFromStringArray(Obj, buf, index, comments)!=0 ) { //DCL_PIN
    if ((i = buf.find("DCL_PIN")) <0) {
      InvalidMessage(InvalidSect, "DCL_PIN");
      return false;
    }
    m_ProjectComments = comments;
    comments = "";
  }
  else  {
    InvalidMessage(InvalidSect, "DCL_PIN");
    return false;
  }

  if (GetLineFromStringArray(Obj, buf, index, comments)!=0 ) {//EXT
    if ((i = buf.find("EXT")) <0)  {
      InvalidMessage(InvalidSect, "EXT");
      return false;
    }
  }
  else  {
    InvalidMessage(InvalidSect, "EXT");
    return false;
  }

  if (GetLineFromStringArray(Obj, buf, index, comments)!=0 ) {//INP
    if ((i = buf.find("INP")) <0)  {
      InvalidMessage(InvalidSect, "INP");
      return false;
    }
  }
  else  {
    InvalidMessage(InvalidSect, "INP");
    return false;
  }
  NumInSF=0;  // 25.02.00 правила Кириенко, чтобы формировался номер m_NumInSF;
  inp_var:
  if (GetLineFromStringArray(Obj, buf, index, comments)!=0 ) {//INP
    pos =0;
    while((i = buf.find("OUT")) < 0)  {    // read all input variables
      nRes = GetNextName(buf,pos, bufname);
      if (nRes ==0)  goto inp_var;
      first = last = 0;
      if (pos<(int)buf.size() && buf[pos] == '[')  { // this variable has interval
        nRes = GetNextName(buf,pos, buf1);
        if (nRes ==0)  {
          InvalidMessage(InvalidSect, "OUT");
          return false;
        }
        first = atoi(buf1.c_str());
        nRes = GetNextName(buf,pos, buf2);
        if (nRes ==0)  {
          InvalidMessage(InvalidSect, "OUT");
          return false;
        }
        last = atoi(buf2.c_str());
      }            //  if (buf[pos] == '[')  
      CSFVar *pVar = new CSFVar(bufname, 'i',first,last);
      pVar->m_NumInSF=NumInSF;
      NumInSF++;
      AddVar(pVar);
    }              //  while(i = buf.find("OUT") <0)
  }                //  if (GetFileLine(fp, buf,len) !=0 )
  else  {
    InvalidMessage(InvalidSect, "OUT");
    return false;
  }

out_var:
  if (GetLineFromStringArray(Obj, buf, index, comments)!=0) {//OUT
    pos =0;
    while((i = buf.find("INTER")) < 0 )    { // read all output variables
      nRes = GetNextName(buf,pos, bufname);
      if (nRes ==0)  goto out_var;
      first = last = 0;
      if (pos<(int)buf.size() && buf[pos] == '[')  {    // this variable has interval
        nRes = GetNextName(buf,pos, buf1);
        if (nRes ==0)  {
          InvalidMessage(InvalidSect, "INTER");
          return false;
        }  
        first = atoi(buf1.c_str());
        nRes = GetNextName(buf,pos, buf2);
        if (nRes ==0)  {
          InvalidMessage(InvalidSect, "INTER");
          return false;
        }  
        last = atoi(buf2.c_str());
      }            //  if (buf[pos] == '[')  
      CSFVar *pVar = new CSFVar(bufname, 'o',first,last);
      pVar->m_NumInSF=NumInSF;
      NumInSF++;
      AddVar(pVar);
    }              //  while(i = buf.find("INTER") <0)
  }
  else  {
    InvalidMessage(InvalidSect, "INTER");
    return false;
  }

inter:
  if (GetLineFromStringArray(Obj, buf, index, comments)!=0) { //INTER
    pos =0;
    while((i = buf.find("END_PIN")) < 0)  {     // read all output variables
      if ((i = buf.find("=")) < 0)  {
        nRes = GetNextName(buf,pos, bufname);
        if (nRes ==0)  goto inter;
        first = last = 0;
        if (pos<(int)buf.size() && buf[pos] == '[')  {  // this variable has interval
          nRes = GetNextName(buf,pos, buf1);
          if (nRes ==0)  {
          InvalidMessage(InvalidSect, "INTER");
            return false;
          }
          first = atoi(buf1.c_str());
          nRes = GetNextName(buf,pos, buf2);
          if (nRes ==0)  {
            InvalidMessage(InvalidSect, "INTER");
            return false;
          }
          last = atoi(buf2.c_str());
        }            //  if (buf[pos] == '[')  
        CSFVar *pVar = new CSFVar(bufname, 't',first,last);
        pVar->m_NumInSF=NumInSF;
        NumInSF++;
        AddVar(pVar);
      }              //if ((i = buf.find("=")) < 0) 
      else {
				buf = BeginEndTrim(buf);
				i = buf.find("=");	
        buf1 = buf.substr(0,i);
        buf2 = buf.erase(0, i+1); //Right(buf.size()-i-1);
        AddPareInter(buf1, buf2);
        goto inter;
      }              //  while(i = buf.find("END_PIN") <0)
    }
  }
  else  {
    InvalidMessage(InvalidSect, "END_PIN");
    return false;
  }
  m_SFVarComments = comments;
  comments = "";

  while (GetLineFromStringArray(Obj, buf, index, comments) !=0 ) {
    if ((i = buf.find("DCL_EL")) >= 0)  {    //DCL_EL 
      CSFType *pSFType;
      if (GetLineFromStringArray(Obj, buf, index, comments)!=0 ) {        
        while((i = buf.find("END_EL")) < 0)  {      // read all output variables
          pos = 0;
          nRes = GetNextName(buf,pos, bufname);
          if (bufname == "TYPE_N")
            bRes = true;
          else if (bufname == "TYPE") 
            bRes = false;
          else  {
            InvalidMessage(InvalidSect, "TYPE");
            return false;
          }
          nRes = GetNextName(buf,pos, buf1);
          if (buf1 == "") {
            InvalidMessage(InvalidSect, "TYPE");
            return false;
          }
          pSFType = new CSFType(buf1,bRes);
          AddType(pSFType);
          NumInSF=0;
          if (GetLineFromStringArray(Obj, buf, index, comments)==0 ){
            InvalidMessage(InvalidSect, "TYPE");
            return false;
          }
          pos=0;
          while (1)  {
            i=GetNextNameWithAdd(Obj,index,comments,buf,pos,bufname);
            if (i==0)  {
              InvalidMessage(InvalidSect, "TYPE");
              return false;
            }
            if (bufname == "INP")  break;
            AddElemForType(pSFType, bufname);
          }  //   while (1)
          while (1)  {
            i=GetNextNameWithAdd(Obj,index,comments,buf,pos,bufname);
            if (i==0)  {
              InvalidMessage(InvalidSect, "TYPE");
              return false;
            }
            if (bufname == "OUT")  break;
            CSFVar *pVar = new CSFVar(bufname, 'i',0,0);
            pVar->m_NumInSF=NumInSF;
            NumInSF++;
            pSFType->AddVar(pVar);
          }  //   while (1)
          NumInSF=0;
          while (1)  {
            i=GetNextNameWithAdd(Obj,index,comments,buf,pos,bufname);
            if (i==0)  {
              InvalidMessage(InvalidSect, "TYPE");
              return false;
            }
            if (bufname == "LIB" ||bufname == "TYPE" ||bufname == "TYPE_N" ||
                bufname == "END_EL" )  break;
            CSFVar *pVar = new CSFVar(bufname, 'o',0,0);
            pVar->m_NumInSF=NumInSF;
            NumInSF++;
            pSFType->AddVar(pVar);
          }  //   while (1)
          if (bufname == "LIB")  {
            i=GetNextNameWithAdd(Obj,index,comments,buf,pos,bufname);
            if (i==0)  {
              InvalidMessage(InvalidSect, "TYPE");
              return false;
            }
            pSFType->SetLib(bufname);
            if (GetLineFromStringArray(Obj, buf, index, comments)==0 ) {
              InvalidMessage(InvalidSect, "TYPE");
              return false;
            }
          }
        }    //  while((i = buf.find("END_EL")) < 0)
      }
      else  {
        InvalidMessage(InvalidSect, "TYPE");
        return false;
      }
      m_TypeComments = comments;
      comments = "";
      if (GetLineFromStringArray(Obj, buf, index, comments)==0 )  {
        InvalidMessage(InvalidSect, "TYPE");
        return false;
      }
    }  // if ((i = buf.find("DCL_EL")) >= 0)

    if ((i = buf.find("CONNECT")) >= 0)  {             //CONNECT 
      if (GetLineFromStringArray(Obj, buf, index, comments)==0 )  {
        InvalidMessage(InvalidSect, "TYPE");
        return false;
      }
      bufname = buf;
new_connect:
      pConnect = new CSFConnect(bufname);
      AddConnect(pConnect);

      if (GetLineFromStringArray(Obj, buf, index, comments)==0 )  {
        InvalidMessage(InvalidSect, "CONNECT");
        return false;
      }
      pos = 0;
      while (1)  {
        buf2 = "";
        GetNextPartString(Obj,index,comments,buf,pos,bufname);
        if ((int)buf.find("END_CONNECT")>=0)  break;
        if (pos>=(int)buf.size() ||  buf[pos] != '=') 
          goto new_connect;
new_connect_right:
        GetNextPartString(Obj,index,comments,buf,pos,buf1);
        if (buf2 == "")  buf2 += buf1;
        else buf2 = (buf2 + "=" + buf1);
        if (pos < (int)buf.size() && buf[pos] == '=')  goto new_connect_right;
        if (bufname != "" || buf2 != "")
        pConnect->AddPare(bufname, buf2);
        bufname = "";
        buf2 = "";
       // if (pos >= buf.size()) goto new_connect;
      }   //  while (1)
      m_ConnectComments = comments;
      comments = "";
      if (GetLineFromStringArray(Obj, buf, index, comments)==0 )  {
        InvalidMessage(InvalidSect, "CONNECT");
        return false;
      }
    }   // if ((i = buf.find("CONNECT")) >= 0)

    if ((i = buf.find("NET")) >= 0)  {             //NET 
      pos =0;
      if (GetLineFromStringArray(Obj, buf, index, comments)==0 )  {
        InvalidMessage(InvalidSect, "NET");
        return false;
      }
      while((i = buf.find("END_NET")) < 0)  {
        if ((i = buf.find("=")) < 0)  {
          InvalidMessage(InvalidSect, "NET");
          return false;
        }
				buf = BeginEndTrim(buf);
				i = buf.find("=");	
        buf1 = buf.substr(0,i);
        buf2 = buf.erase(0,i+1); //Right(buf.size()-i-1);
        AddPareNet(buf1, buf2);
        if (GetLineFromStringArray(Obj, buf, index, comments)==0 ) {
          InvalidMessage(InvalidSect, "NET");
          return false;
        }
      }              //  while(i = buf.find("END_PIN") <0)
      m_NetComments = comments;
      comments = "";
      if (GetLineFromStringArray(Obj, buf, index, comments)==0 )  {
        InvalidMessage(InvalidSect, "NET");
        return false;
      }
    }         //  if ((i = buf.find("NET")) >= 0)

    if ((i = buf.find("FUNCTION")) >= 0)  {             //FUNCTION 
      if (GetLineFromStringArray(Obj, buf, index, comments)==0 )  {
        InvalidMessage(InvalidSect, "FUNCTION");
        return false;
      }
      if ((i = buf.find("LOG")) >= 0)  buf1= "LOG";           //LOG
      else                             buf1= "SDF";           //SDF
      if (GetLineFromStringArray(Obj, buf, index, comments)==0 )  {
        InvalidMessage(InvalidSect, "FUNCTION");
        return false;
      }
      pos = 0;
      i=GetNextNameWithAdd(Obj,index,comments,buf,pos,bufname);
      if (i==0)  {
        InvalidMessage(InvalidSect, "FUNCTION");
        return false;
      }
      int num_in = atoi(bufname.c_str());
      i=GetNextNameWithAdd(Obj,index,comments,buf,pos,bufname);
      if (i==0) {
        InvalidMessage(InvalidSect, "FUNCTION");
        return false;
      }
      int num_out = atoi(bufname.c_str());
      i=GetNextNameWithAdd(Obj,index,comments,buf,pos,bufname);
      if (i==0)  {
        InvalidMessage(InvalidSect, "FUNCTION");
        return false;
      }
      int num_inter = atoi(bufname.c_str());
      if ( /*num_in < 1 ||*/  num_out < 1) {
err_func:
        InvalidMessage(InvalidSect, "FUNCTION");
        return false;
      }
      CSFunction *pFun = new CSFunction(buf1, num_in, num_out, num_inter);
      while(1)  {
        if (GetLineFromStringArray(Obj, buf, index, comments)==0 ) goto err_func;
        i = buf.find("END_");  
        if (i >= 0 && (int)buf.find("=")<0)  break;
        buf.erase(0,buf.find_last_not_of(" "));
        pFun->AddTextLine(buf);
      }
      SetFun(pFun);
      m_FunComments = comments;
      comments = "";
      if ((i = buf.find("END_")) < 0) {
        InvalidMessage(InvalidSect, buf);
        return false;
      }         //  if ((i = buf.find("NET")) >= 0)

      if (GetLineFromStringArray(Obj, buf, index, comments)==0 )  {
        InvalidMessage(InvalidSect, buf);
        return false;
      }
      continue;
    }         //  if ((i = buf.find("NET")) >= 0)

    if ((i = buf.find("END_")) >= 0)  break;
//#ifdef _LINUX
    printf("Error in SF-description: %s\n", buf.c_str());
/*
#else
#ifdef ENG
    buf = "Error in SF-description:" + buf;
#else
    buf = "Ошибка в SF-описании:" + buf;
#endif
    AfxMessageBox(buf.c_str());
#endif
*/
    return false;

  }   // while (GetFileLine(fp, buf,len, comments) !=0 )

  InitSFAttrib();
  return true;
}
