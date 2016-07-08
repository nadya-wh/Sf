#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "inimap.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef _LINUX
#pragma warning(disable : 4996)
#endif


//--------------------------------------------------------
IniMap::IniMap(string FileName)  // Создавать можно только под конкретный файл инициализации
{
  GetDbMap(FileName);  // Заменить содержание базы информацикй из указанного файла
}

//--------------------------------------------------------
    // Заменить содержание базы информацикй из указанного файла
//--------------------------------------------------------
bool IniMap::GetDbMap(string FileName)
{
  FILE *in;
  char buf [121];
  string Section;
  string Key;
  string Value;
  int i;

  dbMap.clear();  Idb = dbMap.begin();
  in = fopen(FileName.c_str(),"r");
  if (in==NULL)
  {
//    cout << "Error:" << FileName<< endl;
    return false;
  }

  while (!feof(in))
  {
    fgets(buf, 120,in);
    if (buf[0]==';') { continue; } 
    else if (buf[0]=='#') { continue; } 
    else if (buf[0]==' ') { continue; } 
    else if (buf[0]=='\r') { continue; } 
    else if (buf[0]=='\n') { continue; } 
    else if (buf[0]=='[') {   // Переход к новой строке
      Section = buf+1;     
      i = Section.find(']');
      if (i<0) {
        cout << "ini-error: [ without ]"<< endl;
        return false;
      }
      Section = Section.substr(0,i);
      continue;
    }
    else {
      Value = buf;
      i = Value.find('=');
      if (i<=0)
      {
        cout << "ini-error: = is absent"<< endl;
        return false;
      } 
      else
      {
        Key = Value.substr(0,i);
        Value.erase(0,i+1);
        if (Value[Value.length()-1] == '\n') {
          Value = Value.substr(0,Value.length()-1);
        }
        i = Value.find_last_not_of("\r\t ");
        if (i > -1) {
          Value.erase(i+1,Value.length()-i-1);
        }
        dbMap[Section+":"+Key] = Value;
      }
    }
  }
  fclose(in);
  return true;
}

//--------------------------------------------------------
// Построить файл инициализации по текущему содержанию базы
//--------------------------------------------------------

bool IniMap::PutDbMap(string FileName)  
{
  if (dbMap.size()==0)   {
    cout << "ini-error: Map is empty" << endl;
  return false;
  }

  FILE *out;
  string Section;
  string Key;
  string Value;
  int i;

  out = fopen(FileName.c_str(),"w");
  if (out==NULL)
  {
    cout << "ini-error: " << FileName<< endl;
    return false;
  }
  Section ="";
  for (Idb=dbMap.begin(); Idb!=dbMap.end(); Idb++) {
    Value = Idb->first;
    i = Value.find(":");
    Key = Value.substr(i+1,Value.length()-i-1);
    Value = Value.erase(i,Value.length()-i);
    if (Value != Section) {
      if (Section.size()!=0)   fprintf(out,"\n");    
      Section = Value;
      Value = "["+Section+"]";
      fprintf(out, "%s\n",Value.c_str());
    }
    Value = Key + "=" + Idb->second;
    fprintf(out, "%s\n",Value.c_str());
  }
  fclose(out);
  return true;
}


//--------------------------------------------------------
// Выборка данных по сложному ключу: S_Key - имя параметра в форме (секция:ключ)
//--------------------------------------------------------
bool IniMap::GetParam(string S_Key, string& Value, string Def)
{
  Value = Def;
  if (FindKey(S_Key)==false) return false;
  Value = Idb->second;
  return true;
}


//--------------------------------------------------------
bool IniMap::GetParam(string S_Key, double& Value, double Def)
{
  Value = Def;
  if (FindKey(S_Key)==false) return false;
  sscanf(Idb->second.c_str(),"%lE",&Value);
  return true;
}

//--------------------------------------------------------
bool IniMap::GetParam(string S_Key, int& Value, int Def)
{
  Value = Def;
  if (FindKey(S_Key)==false) return false;
  sscanf(Idb->second.c_str(),"%d",&Value);
  return true;
}


//--------------------------------------------------------
// Достать перечень всех определенных в базе секций
//--------------------------------------------------------
void IniMap::GetSectionsName(vector <string>& SectList)
{
  if (dbMap.size()==0)   {
    //    cout << "ini-error: Map is empty" << endl;
    return ;
  }

  string Sect="", w;
  int i;
  map <string, string>::iterator idb;
  for(idb = dbMap.begin(); idb != dbMap.end(); idb++)
  {
    w = idb->first;
    i = w.find(':');
    if (i>0) {
      w = w.substr(0,i);
      if (Sect != w) {
        SectList.push_back(w);
        Sect = w;
      }
    }
  }
}

//--------------------------------------------------------
   //Записать в базу строковый параметр по "сложному ключу"
//--------------------------------------------------------
bool IniMap::SetParam(string S_Key, string Value) 
{
//  if (dbMap.size()==0)   {
//    cout << "ini-error: Map is empty" << endl;
//    return false;
//  }

  int i = S_Key.find(':');
  if (i>0 && i !=S_Key[S_Key.length()-1])  {
    dbMap[S_Key] = Value;
    return true;
  }
  cout << "ini-error: invalid key" << S_Key.c_str() << endl;
  return false;
}

//--------------------------------------------------------
bool IniMap::SetParam(string S_Key, double Value) 
{
//   if (dbMap.size()==0)   {
//     //    cout << "ini-error: Map is empty" << endl;
//     return false;
//   }
  char buf[40];
  sprintf(buf, "%G",Value);
  return SetParam(S_Key,buf);
}

//--------------------------------------------------------
//Записать в базу строковый параметр по "сложному ключу"
bool IniMap::SetParam(string S_Key, int Value) 
{
//   if (dbMap.size()==0)   {
//     //    cout << "ini-error: Map is empty" << endl;
//     return false;
//   }
  char buf[40];
  sprintf(buf, "%d",Value);
  return SetParam(S_Key,buf);
}
  
//--------------------------------------------------------
// Поиск ключа
//--------------------------------------------------------
bool IniMap::FindKey(string S_Key)  // В базе есть указанный ключ?
{
  Idb = dbMap.find(S_Key);
  if (Idb==dbMap.end()) return false;
  else                  return true;
}

