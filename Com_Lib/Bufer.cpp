/////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string>
#include <algorithm>
#include "bufer.h"

using namespace std;

const int  SIZE_BUFER     =   500;
const int  GROW_BY_LARGE  =   100;

//этот файл необходим для работы программы считывания SF-описания из файла
/////////////////////////////////////////////////////////////////////////////
// Bufer
//---------------------------------------------------------------
int GetFileLine(FILE *pFile, string& buf, long& len, string& szComments) 
{
  string buf1, buf2, buf3;
  int nRes;
  char b[5];
  char endl[2];
  endl[0] = '\n'; endl[1] = 0;
  bool bComment;
  bComment = false;

  buf = "";
  buf1 = "";
  bComment = false;
  szComments = "";
nach:
  for (; len>0; len--)  {
      /*
#ifndef _LINUX
    fread_s(b,1,1,1,pFile);
#else
      */
    fread(b,1,1,pFile);
//#endif
    if (b[0]== '\r') {
/*
#ifndef _LINUX
    fread_s(b,1,1,1,pFile);
#else
*/
    fread(b,1,1,pFile);
//#endif
      len-=2;
      break;
    }
    else if (b[0]== '\n') {
      len--;
      break;
    }
    else if (b[0]== '\t') {
      buf += ' ';
    }
    else {
      buf += b[0];
    }
  }
// checkup:
  if ( (buf.size() == 0) && (len>0))  goto nach;
  if (bComment  &&  (nRes = buf.find("*/")) <0 && (len>0)) 
    { szComments +=  buf +endl; buf = ""; goto nach;}
  else if (bComment &&  (nRes =buf.find("*/")) >=0 ) {
    szComments +=  buf.substr(0,nRes);
    goto end_for_comment;
  }

 // Построение строки без комментариев
  nRes =buf.find("/*");
  if (nRes >=0)  {           //строка содержит начало комментариев */
    bComment = true;
    szComments =  buf.substr(nRes+2, buf.length() - nRes);
    buf1 = buf.substr(0,nRes);
    if ((nRes =buf.find("*/")) <0)  {  // строка не содержит конца комментариев
      szComments +=  endl;
      buf = "";
      goto nach;
    }
    else  {
      int nRes3 = szComments.find("*/");
      szComments = szComments.substr(0,nRes3);
    }
end_for_comment:
    if (nRes+2 <  (int) buf.length())
      buf2 = buf.substr(nRes+2);
    buf = buf1+buf2;
    bComment = false;
    //goto checkup;
  }
  return buf.length();
}


//---------------------------------------------------------------
int GetNextName(string& buf, int& pos, string& bufname)
{
//  int i = 0;
  bufname = "";
  while ((pos < (int)buf.length()) && ( (buf[pos]==' ') || (buf[pos]=='\t')
         || (buf[pos]==',') ) ) pos++;
  if (pos >= (int)buf.length())  return 0;
  while ((pos < (int)buf.length()) && (buf[pos]!=' ') && (buf[pos]!='\t') &&
          (buf[pos]!=',') )
    bufname += buf[pos++];
  return (int)bufname.length();
}

//---------------------------------------------------------------
int GetNextNameWithAdd(FILE *in, long& len, string& szComments,
                       string& buf, int& pos, string& bufname)
{
again:
  int i = 0;
  bufname = "";
  while ((pos < (int)buf.length()) && ( (buf[pos]==' ') || (buf[pos]=='\t') ||
         (buf[pos]==',') ) ) pos++;
  if (pos >= (int)buf.length()) goto add;
  while ((pos < (int)buf.length()) && (buf[pos]!=' ') && (buf[pos]!='\t') &&
          (buf[pos]!=',') )
    bufname += buf[pos++];
  if ( bufname.length())  return (int)bufname.length();
add:
  pos = 0;
  i = GetFileLine(in, buf, len, szComments);
  if (i>0)  goto again;
  return 0;
}

//---------------------------------------------------------------
int GetNextPartString(FILE *in, long& len, string& szComments,
                      string& buf, int& pos, string& bufname)
{
again:
  int i = 0;
  bufname = "";
  while ((pos < (int)buf.length()) && ( (buf[pos]==' ') || (buf[pos]=='=') ||
         (buf[pos]=='\n') ) ) pos++;
  if (pos >= (int)buf.length()) goto add;
  while ((pos < (int)buf.length()) && (buf[pos]!=' ') && (buf[pos]!='=') &&
          (buf[pos]!='\n') ) 
    bufname += buf[pos++];

  if ( bufname.length())  {
    while ((pos < (int)buf.length()) && (buf[pos]==' ') ) pos++;
    return (int)bufname.length();
  }
add:
  pos = 0;
  i = GetFileLine(in, buf, len, szComments);
  if (i>0)  goto again;

  return 0; 
}

//---------------------------------------------------------------
int GetSimpleFileLine(FILE *in, string& buf, long& len) 
{
  char b[5];
  int i;
  bool FlagSpace;

  buf = "";
nach:
  for (; len>0; len--)  {
/*
#ifndef _LINUX
    fread_s(b,1,1,1,in);
#else
*/
    fread(b,1,1,in);
//#endif
    if (b[0]== '\r') {
/*
#ifndef _LINUX
    fread_s(b,1,1,1,in);
#else
*/
    fread(b,1,1,in);
//#endif
      len-=2;
      break;
    }
    else if (b[0]== '\n') {
      len--;
      break;
    }
    else if (b[0]== '\t') {
      buf += ' ';
    }
    else {
      buf += b[0];
    }
  }
//#ifdef _MSVC9
//    fread_s(b,1,1,1,in);
//#else
//    fread(b,1,1,in);
//#endif
//    if (b[0]!= '\r')
//      if (b[0] == '\t') 
//        buf += ' ';
//      else
//        buf += b[0];
//    else {
//#ifdef _MSVC9
//      fread_s(b,1,1,1,in);
//#else
//      fread(b,1,1,in);
//#endif
//      len-=2;
//      break;
//    }
//  }
  if ( ((int)buf.length() == 0) && (len>0))  goto nach;
  FlagSpace = true;
  for (i=0; i<(int)buf.length(); i++)
    if (buf[i] != ' ')
      FlagSpace = false;
  
  if (FlagSpace == true)  {
    buf = "";
    if (len>0)  goto nach;
  }  
  return (int)buf.length();
}

//------------------------
string Trim(string Formula)
{
  if (Formula[0]!=' '&& Formula[0]!='\t' && Formula[0]!='\r' && Formula[0]!='\n') return Formula;
  return Formula.erase(0,Formula.find_first_not_of(" \t\r\n"));
}

//------------------------
string BeginEndTrim(string Formula)
{ string Res;
if (Formula.size()==0) return "";
  Res = Trim(Formula);
  if (Res.size()==0) return "";
  return (Res.substr(0,Res.find_last_not_of(" \t\r\n")+1));
}

//---------------  var name from logur
int GetNameVar(string& buf, int &pos, string& bufname)
{
//  int i = 0;
  bufname = "";
  while ((pos < (int)buf.length()) && (buf[pos]=='\t')  ) pos++;
  if (pos >= (int)buf.length())  return 0;
  while ((pos < (int)buf.length()) && (buf[pos]!='+') && 
          (buf[pos]!=';') && (buf[pos]!='\t') &&
          (buf[pos]!='*')  && (buf[pos]!='=') )
    bufname += buf[pos++];
  return (int)bufname.length();
}

//---------------  var name previos var from logur
int GetNamePrevVar(string& buf, int &pos, string& bufname)
{
//  int i = 0;
  bufname = "";
  while ((pos >=0) && (buf[pos]=='\t')  ) 
    pos--;
  if (pos < 0)  return 0;
  while ((--pos >=0) && (buf[pos]!='+') && (buf[pos]!='\t') && (buf[pos]!='*')
                     && (buf[pos]!='=')) 
    bufname += buf[pos];
  reverse(bufname.begin(), bufname.end());
  return bufname.length();
}

//--------------- convert string from DOS - coding to Windows - coding
string TrDosToWin(string buf)
{
  int c;
  for (int i=0; i< (int)buf.length(); i++)  {
    c = buf[i];
    switch(c)  {
      case -127: buf[i] ='А'; break;
      case -126: buf[i] ='В'; break;
      case -125: buf[i] ='Г'; break;
      case -124: buf[i] ='Д'; break;
      case -123: buf[i] ='Е'; break;
      case -122: buf[i] ='Ж'; break;
      case -121: buf[i] ='З'; break;
      case -120: buf[i] ='И'; break;
      case -119: buf[i] ='Й'; break;
      case -118: buf[i] ='К'; break;
      case -117: buf[i] ='Л'; break;
      case -116: buf[i] ='М'; break;
      case -115: buf[i] ='Н'; break;
      case -114: buf[i] ='О'; break;
      case -113: buf[i] ='П'; break;
      case -112: buf[i] ='Р'; break;
      case -111: buf[i] ='С'; break;
      case -110: buf[i] ='Т'; break;
      case -109: buf[i] ='У'; break;
      case -108: buf[i] ='Ф'; break;
      case -107: buf[i] ='Х'; break;
      case -106: buf[i] ='Ц'; break;
      case -105: buf[i] ='Ч'; break;
      case -104: buf[i] ='Ш'; break;
      case -103: buf[i] ='Щ'; break;
      case -102: buf[i] ='Ъ'; break;
      case -101: buf[i] ='Ы'; break;
      case -100: buf[i] ='Ь'; break;
      case -99: buf[i] ='Э'; break;
      case -98: buf[i] ='Ю'; break;
      case -97: buf[i] ='Я'; break;

      case -96:  buf[i] ='а'; break;
      case -95:  buf[i] ='б'; break;
      case -94:  buf[i] ='в'; break;
      case -93:  buf[i] ='г'; break;
      case -92:  buf[i] ='д'; break;
      case -91: buf[i] ='е'; break;
      case -90: buf[i] ='ж'; break;
      case -89: buf[i] ='з'; break;
      case -88: buf[i] ='и'; break;
      case -87: buf[i] ='й'; break;
      case -86: buf[i] ='к'; break;
      case -85: buf[i] ='л'; break;
      case -84: buf[i] ='м'; break;
      case -83: buf[i] ='н'; break;
      case -82: buf[i] ='о'; break;
      case -81: buf[i] ='п'; break;

      case -32: buf[i] ='р'; break;
      case -31: buf[i] ='с'; break;
      case -30: buf[i] ='т'; break;
      case -29: buf[i] ='у'; break;
      case -28: buf[i] ='ф'; break;
      case -27: buf[i] ='х'; break;
      case -26: buf[i] ='ц'; break;
      case -25: buf[i] ='ч'; break;
      case -24: buf[i] ='ш'; break;
      case -23: buf[i] ='щ'; break;
      case -22: buf[i] ='ъ'; break;
      case -21: buf[i] ='ы'; break;
      case -20: buf[i] ='ь'; break;
      case -19: buf[i] ='э'; break;
      case -18: buf[i] ='ю'; break;
      case -17: buf[i] ='я'; break;

      default:  if (c == 32)
                  i = i;
                break;
    }
  }
  return buf;
}

/*

//-----------------------------
void PrintMap(CMapStringToString *pMap, CString as_file)
{
  POSITION pos;
  CString s1,s2;
  TRY
  {
    CFile f(as_file, CFile::modeCreate | CFile::modeWrite);
    for (pos = pMap->GetStartPosition(); pos; )  {
      pMap->GetNextAssoc(pos, s1, s2);
      s1 += (" -> " + s2 + "\n");
      f.Write(s1.GetBuffer(100), s1.GetLength());
    }
  }
  CATCH(CFileException,e)
  {
#ifdef _LINUX
    printf("Error at record of the file \n");
#else
    AfxMessageBox("Error at record of the file ");  
#endif
    return;
  }
  END_CATCH
}
*/

//---------------------------------------------------------------
int GetNextInpName(string& bufur, int& j, string& bufname)
// Прочитать очередное имя переменной в уравнении Кириенко 29.05.200
// j - начало поиска
{      
  int pr = 0;// i = 0, 
  bufname = "";
  while ((j < (int)bufur.length()) && ( (bufur[j]==' ') || (bufur[j]=='\t')
         ||(bufur[j]=='^')||(bufur[j]=='(')|| (bufur[j]==')')
         ||(bufur[j]=='*')||(bufur[j]=='+'))) {
    if (bufur[j]=='(') {
      if (pr==0) pr=1;  // ( перед именем
      if (pr==2) pr=4;  // *( перед именем
      if (pr==3) pr=5;  // +( перед именем
      if (pr==6) pr=9;  // ^( перед именем
      j++;  continue;
    }
    if (bufur[j]=='*') {pr=2;j++;  continue;} // * перед именем
    if (bufur[j]=='+') {pr=3;j++;  continue;} // + перед именем
    if ((bufur[j]=='^')&&(pr==0)) {pr=6;j++;continue;}// ^ перед именем
    if ((bufur[j]=='^')&&(pr==2)) {pr=7;j++;continue;}// *^ перед именем
    if ((bufur[j]=='^')&&(pr==3)) {pr=8;j++;continue;}// +^ перед именем
    j++;
  }
  if (j >= (int)bufur.length())  return 0;
  while ((j < (int)bufur.length()) && (bufur[j]!=' ') && (bufur[j]!='\t') &&
         (bufur[j]!=')') && (bufur[j]!='*') && (bufur[j]!='+') &&
         (bufur[j]!='=') && (bufur[j]!=';')) 
    bufname += bufur[j++];
  if (bufname.empty())  pr=-1;
  return pr;
}


//---------------------------------------------------------------
int GetLineFromStringArray(stringarray &Obj, string& buf, int& index, string& szComments) 
{
  string buf1, buf2;
  int nRes, len;
  char endl[3];
  bool bComment = false;
  endl[0] = '\n'; endl[1] = 0;

  buf = "";
  buf1 = "";
  bComment = false;
  len = Obj.size();
nach:
  buf=Obj.at(index);
  index++;

//checkup:
  if ( (buf.length() == 0) && (index<len))  goto nach;
  if (bComment  &&  (nRes =buf.find("*/")) <0 && (index<len))
    { szComments +=  buf +endl; buf = ""; goto nach;}

  else if (bComment &&  (nRes =buf.find("*/")) >=0 ) {
    szComments +=  buf.substr(0,nRes);
    goto end_for_comment;
  }

 // Построение строки без комментариев
  nRes =buf.find("/*");
  if (nRes >=0)  {           //строка содержит начало комментариев
    bComment = true;
    szComments =  buf.substr(nRes+2, buf.length() - nRes);
    buf1 = buf.substr(0,nRes);
    if ((nRes =buf.find("*/")) <0)  {  // строка не содержит конца комментариев
      szComments +=  endl;
      buf = "";
      goto nach;
    }
    else  {
      int nRes3 = szComments.find("*/");
      szComments = szComments.substr(0,nRes3);
    }

end_for_comment:
    if (nRes+2 <  (int) buf.length())
      buf2 = buf.substr(nRes+2);
    buf = buf1+buf2;
    bComment = false;
    //goto checkup;
  }
  return buf.length();
}
//---------------------------------------------------------------
int GetNextNameWithAdd(stringarray &Obj, int& index, string& szComments,
                       string& buf, int& pos, string& bufname)
{
again:
  int i = 0;
  bufname = "";
  while ((pos < (int)buf.length()) && ( (buf[pos]==' ') || (buf[pos]=='\t') ||
         (buf[pos]==',') ) ) pos++;
  if (pos >= (int)buf.length()) goto add;
  while ((pos < (int)buf.length()) && (buf[pos]!=' ') && (buf[pos]!='\t') &&
          (buf[pos]!=',') )
    bufname += buf[pos++];
  if ( (int)bufname.length())  return (int)bufname.length();
add:
  pos = 0;
  i = GetLineFromStringArray(Obj, buf, index, szComments);
  if (i>0)  goto again;
  return 0;
}


//---------------------------------------------------------------
int GetNextPartString(stringarray &Obj, int& index, string& szComments,
                      string& buf, int& pos, string& bufname)
{
again:
  int i = 0;
  bufname = "";
  while ((pos < (int)buf.length()) && ( (buf[pos]==' ') || (buf[pos]=='=') ||
         (buf[pos]=='\n') ) ) pos++;
  if (pos >= (int)buf.length()) goto add;
  while ((pos < (int)buf.length()) && (buf[pos]!=' ') && (buf[pos]!='=') &&
          (buf[pos]!='\n') ) 
    bufname += buf[pos++];

  if ( bufname.length())  {
    while ((pos < (int)buf.length()) && (buf[pos]==' ') ) pos++;
    return (int)bufname.length();
  }
add:
  pos = 0;
  i = GetLineFromStringArray(Obj, buf, index, szComments);
  if (i>0)  goto again;

  return 0; 
}
