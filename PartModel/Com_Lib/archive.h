#ifndef ARCH_ROM
#define ARCH_ROM
#include <stdio.h>
#include <string>

using namespace std;
typedef unsigned UINT;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef long LONG;
typedef unsigned long DWORD;
#ifdef _BIT64
typedef __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;
#endif

class CArch
{
public:
  // Flag values   enum Mode { store = 0, load = 1};
  CArch(char* fname, int nMode);
  ~CArch();
  // Attributes
  inline int IsLoading() { return (m_nMode==1);}
  inline int IsStoring() { return (m_nMode==0);}
  inline string GetFileName() { return m_strFileName; }

  // Operations
  UINT Read(void* lpBuf, UINT nMax);
  void Write(const void* lpBuf, UINT nMax);
  void Close();
  void EndLine();

  // reading and writing strings
  void WriteString(string S);
  void ReadString(string& S);

  // insertion operations
  friend CArch& operator<<(CArch& ar, string S);
  friend CArch& operator>>(CArch& ar, string& S);

//  CArch& operator<<(string S);
  CArch& operator<<(BYTE by);
  CArch& operator<<(WORD w);
  CArch& operator<<(long l);
  CArch& operator<<(DWORD dw);
  CArch& operator<<(float f);
  CArch& operator<<(double d);
  CArch& operator<<(int i);
  CArch& operator<<(unsigned int u);
  CArch& operator<<(short w);
  CArch& operator<<(char ch);
#ifdef _BIT64
  CArch& operator<<(LONGLONG dwdw);
  CArch& operator<<(ULONGLONG dwdw);
#endif

  // extraction operations
//  CArch& operator>>(string& S);
  CArch& operator>>(BYTE& by);
  CArch& operator>>(WORD& w);
  CArch& operator>>(DWORD& dw);
  CArch& operator>>(long& l);
  CArch& operator>>(float& f);
  CArch& operator>>(double& d);
  CArch& operator>>(int& i);
  CArch& operator>>(unsigned int& u);
  CArch& operator>>(short& w);
  CArch& operator>>(char& ch);
#ifdef _BIT64
  CArch& operator>>(LONGLONG& dwdw);
  CArch& operator>>(ULONGLONG& dwdw);
#endif

  // Implementation
  
protected:
  // archive objects cannot be copied or assigned
  CArch(const CArch& arSrc);
  void operator=(const CArch& arSrc);
  int m_nMode;
  FILE* m_pFile;
  string m_strFileName;
};
#endif  // ARCH_ROM
