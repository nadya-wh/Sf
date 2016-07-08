#ifndef INIMAP
#define INIMAP
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

//-------------------------------------------------------------------------
// Класс ведения базы данных Map, отражающих содержание файла инициализации
//-------------------------------------------------------------------------
// версия 1.0.   20.01.2014
//-------------------------------------------------------------------------

class IniMap
{

public:
  IniMap() {}
  IniMap(string FileName);  // Создавать можно только под конкретный файл инициализации

// Attributes
  string FileName; // Имя ini-файла
  map <string, string> dbMap;    // Коллекция для хранения информации - база
                                 // ключ состоит из двух частей, разделенных символом ":"
                                 // <имя секции>:<имя параметра>  
  map <string, string>::iterator Idb; // Итератор по базе

// Methods
  bool GetDbMap(string FileName);  // Заменить содержание базы по указанному файлу
  bool PutDbMap(string FileName);  // Сохранить содержание базы в указанном файле

// Выборка данных по ключу: Sect - имя секции файла
//                          Key - имя параметра 
//                          Value - выбранное значение параметра,
//                          Def - значение параметра по умолчанию
// Возврат: 0 - использовано значение по умолчанию
//          1 - значение выбрано из базы

  //Взять из базы строковый параметр по имени секции и ключу
inline bool GetParam(string Sect, string Key, string& Value, string Def) 
            { return GetParam(Sect+":"+Key,Value, Def); }
  //Взять из базы параметр (вещественое число) по имени секции и ключу
  bool GetParam(string Sect, string Key, double& Value, double Def) 
            { return GetParam(Sect+":"+Key, Value, Def); }
  //Взять из базы параметр (целое число) по имени секции и ключу
  bool GetParam(string Sect, string Key, int& Value, int Def)    
            { return GetParam(Sect+":"+Key, Value, Def); }

// Выборка данных по сложному ключу: S_Key - имя параметра в форме (секция:ключ)

  //Взять из базы строковый параметр по "сложному ключу" (секция:ключ)
  bool GetParam(string S_Key, string& Value, string Def);  
  //Взять из базы параметр (вещественое число) по "сложному ключу" (секция:ключ)
  bool GetParam(string S_Key, double& Value, double Def);  
  //Взять из базы параметр (целоеое число) по "сложному ключу" (секция:ключ)
  bool GetParam(string S_Key, int& Value, int Def);     
  
  // Достать перечень всех определенных в базе секций
  void GetSectionsName(vector <string>& SectList); 

  // Запись данных по ключу: Sect - имя секции файла
  //                          Key - имя параметра 
  //                          Value - выбранное значение параметра

  //Записать в базу строковый параметр по имени секции и ключу
inline  bool SetParam(string Sect, string Key, string Value) 
            { return SetParam(Sect+":"+Key,Value); }
  //Записать в базу параметр (вещественое число) по имени секции и ключу
inline  bool SetParam(string Sect, string Key, double Value) 
            { return SetParam(Sect+":"+Key,Value); }
  //Записать в базу параметр (целое число) по имени секции и ключу
inline  bool SetParam(string Sect, string Key, int Value)    
            { return SetParam(Sect+":"+Key,Value); }

  // Выборка данных по сложному ключу: S_Key - имя параметра в форме (секция:ключ)

  bool SetParam(string S_Key, string Value);  
  //Записать в базу строковый параметр по "сложному ключу" (секция:ключ)
  bool SetParam(string S_Key, double Value);  
  //Записать в базу параметр (вещественое число) по "сложному ключу" (секция:ключ)
  bool SetParam(string S_Key, int Value);     
  //Записать в базу параметр (целоеое число) по "сложному ключу" (секция:ключ)


  // Поиски
inline  bool FindKey(string Sect, string Key) { return FindKey(Sect+":"+Key); }

  bool FindKey(string S_Key);
};

#endif  //INIMAP