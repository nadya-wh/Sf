////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string>
#include "archive.h"
#ifdef _MSVC9
#pragma warning(disable : 4996)
#endif

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CArch

void CArch::EndLine() 
{
  char buf[3]="\n";
  Write(&buf[0], 1);
}

CArch& operator<<(CArch& ar, string S)
{ 
  ar.WriteString(S);
  return ar; 
}

CArch& CArch::operator<<(unsigned u) { return CArch::operator<<((int)u); }
CArch& CArch::operator<<(short w)    { return CArch::operator<<((WORD)w); }
CArch& CArch::operator<<(char ch)    { return CArch::operator<<((BYTE)ch); }
CArch& CArch::operator<<(BYTE by)
{ 
  //  if(!IsStoring())
  //    AfxThrowArchiveException(CArchException::readOnly,m_strFileName);
  Write(&by, sizeof(BYTE));
  return *this; 
}

#ifdef _BIT64
CArch& CArch::operator<<(LONGLONG dwdw)
{ 
  //  if(!IsStoring())
  //    AfxThrowArchiveException(CArchException::readOnly,m_strFileName);
  Write(&dwdw, sizeof(LONGLONG));
  return *this; 
}
CArch& CArch::operator<<(ULONGLONG dwdw)
{ 
  //   if(!IsStoring())
  //     AfxThrowArchiveException(CArchException::readOnly,m_strFileName);
  //  *(UNALIGNED ULONGLONG*)m_lpBufCur = dwdw; m_lpBufCur += sizeof(ULONGLONG); return *this; 
  Write(&dwdw, sizeof(ULONGLONG));
  return *this; 
}
#endif

CArch& CArch::operator<<(WORD w)
{ 
  //   if(!IsStoring())
  //     AfxThrowArchiveException(CArchException::readOnly,m_strFileName);
  //   if (m_lpBufCur + sizeof(WORD) > m_lpBufMax) Flush();
  //   *(UNALIGNED WORD*)m_lpBufCur = w; m_lpBufCur += sizeof(WORD); 
  Write(&w, sizeof(WORD));
  return *this; 
}

CArch& CArch::operator<<(long l)
{ 
  //   if(!IsStoring())
  //     AfxThrowArchiveException(CArchException::readOnly,m_strFileName);
  //   if (m_lpBufCur + sizeof(LONG) > m_lpBufMax) Flush();
  //   *(UNALIGNED LONG*)m_lpBufCur = l; m_lpBufCur += sizeof(LONG); 
  Write(&l, sizeof(LONG));
  return *this; 
}
CArch& CArch::operator<<(int i)
{ 
  //   if(!IsStoring())
  //     AfxThrowArchiveException(CArchException::readOnly,m_strFileName);
  //   if (m_lpBufCur + sizeof(LONG) > m_lpBufMax) Flush();
  //   *(UNALIGNED LONG*)m_lpBufCur = l; m_lpBufCur += sizeof(LONG); 
  Write(&i, sizeof(int));
  return *this; 
}

CArch& CArch::operator<<(DWORD dw)
{ 
  //   if(!IsStoring())
  //     AfxThrowArchiveException(CArchException::readOnly,m_strFileName);
  //   if (m_lpBufCur + sizeof(DWORD) > m_lpBufMax) Flush();
  //   *(UNALIGNED DWORD*)m_lpBufCur = dw; m_lpBufCur += sizeof(DWORD); 
  Write(&dw, sizeof(DWORD));
  return *this; 
}
CArch& CArch::operator<<(float f)
{ 
  //   if(!IsStoring())
  //     AfxThrowArchiveException(CArchException::readOnly,m_strFileName);
  //   if (m_lpBufCur + sizeof(float) > m_lpBufMax) Flush();
  //   *(UNALIGNED float*)m_lpBufCur = f; m_lpBufCur += sizeof(float); 
  Write(&f, sizeof(float));
  return *this;
}
CArch& CArch::operator<<(double d)
{
  //   if(!IsStoring())
  //     AfxThrowArchiveException(CArchException::readOnly,m_strFileName);
  //   if (m_lpBufCur + sizeof(double) > m_lpBufMax) Flush();
  //   *(UNALIGNED double*)m_lpBufCur = d; m_lpBufCur += sizeof(double); return *this; 
  Write(&d, sizeof(double));
  return *this;
}

CArch& operator>>(CArch& ar, string& S)
{ 
  ar.ReadString(S);
  return ar; 
}

CArch& CArch::operator>>(unsigned& u) { return CArch::operator>>((int&)u); }
CArch& CArch::operator>>(short& w)    { return CArch::operator>>((WORD&)w); }
CArch& CArch::operator>>(char& ch)    { return CArch::operator>>((BYTE&)ch); }
CArch& CArch::operator>>(BYTE& by)
{ 
  //   if(!IsLoading())
  //     AfxThrowArchiveException(CArchException::writeOnly,m_strFileName);
  //   if (m_lpBufCur + sizeof(BYTE) > m_lpBufMax)
  //     FillBuffer(UINT(sizeof(BYTE) - (m_lpBufMax - m_lpBufCur)));
  //   by = *(UNALIGNED BYTE*)m_lpBufCur; m_lpBufCur += sizeof(BYTE); 

  Read(&by, sizeof(BYTE));
  return *this; 
}

#ifdef _BIT64
CArch& CArch::operator>>(LONGLONG& dwdw)
{ 
  //   if(!IsLoading())
  //     AfxThrowArchiveException(CArchException::writeOnly,m_strFileName);
  //   if (m_lpBufCur + sizeof(LONGLONG) > m_lpBufMax)
  //     FillBuffer(sizeof(LONGLONG) - (UINT)(m_lpBufMax - m_lpBufCur));
  //   dwdw = *(UNALIGNED LONGLONG*)m_lpBufCur; m_lpBufCur += sizeof(LONGLONG); return *this; 
  Read(&dwdw, sizeof(LONGLONG));
  return *this; 
}

CArch& CArch::operator>>(ULONGLONG& dwdw)
{ 
  //   if(!IsLoading())
  //     AfxThrowArchiveException(CArchException::writeOnly,m_strFileName);
  //   if (m_lpBufCur + sizeof(ULONGLONG) > m_lpBufMax)
  //     FillBuffer(sizeof(ULONGLONG) - (UINT)(m_lpBufMax - m_lpBufCur));
  //   dwdw = *(UNALIGNED ULONGLONG*)m_lpBufCur; m_lpBufCur += sizeof(ULONGLONG); return *this; 
  Read(&dwdw, sizeof(ULONGLONG));
  return *this; 
}
#endif

CArch& CArch::operator>>(WORD& w)
{ 
  //   if(!IsLoading())
  //     AfxThrowArchiveException(CArchException::writeOnly,m_strFileName);
  //   if (m_lpBufCur + sizeof(WORD) > m_lpBufMax)
  //     FillBuffer(UINT(sizeof(WORD) - (m_lpBufMax - m_lpBufCur)));
  //   w = *(UNALIGNED WORD*)m_lpBufCur; m_lpBufCur += sizeof(WORD); return *this; 
  Read(&w, sizeof(WORD));
  return *this; 
}
CArch& CArch::operator>>(DWORD& dw)
{ 
  //   if(!IsLoading())
  //     AfxThrowArchiveException(CArchException::writeOnly,m_strFileName);
  //   if (m_lpBufCur + sizeof(DWORD) > m_lpBufMax)
  //     FillBuffer(UINT(sizeof(DWORD) - (m_lpBufMax - m_lpBufCur)));
  //   dw = *(UNALIGNED DWORD*)m_lpBufCur; m_lpBufCur += sizeof(DWORD); return *this; 
  Read(&dw, sizeof(DWORD));
  return *this; 
}
CArch& CArch::operator>>(float& f)
{ 
  //   if(!IsLoading())
  //     AfxThrowArchiveException(CArchException::writeOnly,m_strFileName);
  //   if (m_lpBufCur + sizeof(float) > m_lpBufMax)
  //     FillBuffer(UINT(sizeof(float) - (m_lpBufMax - m_lpBufCur)));
  //   f = *(UNALIGNED float*)m_lpBufCur; m_lpBufCur += sizeof(float); return *this; 
  Read(&f, sizeof(float));
  return *this; 
}
CArch& CArch::operator>>(double& d)
{ 
  //   if(!IsLoading())
  //     AfxThrowArchiveException(CArchException::writeOnly,m_strFileName);
  //   if (m_lpBufCur + sizeof(double) > m_lpBufMax)
  //     FillBuffer(UINT(sizeof(double) - (m_lpBufMax - m_lpBufCur)));
  //   d = *(UNALIGNED double*)m_lpBufCur; m_lpBufCur += sizeof(double); return *this; 
  Read(&d, sizeof(double));
  return *this; 
}

CArch& CArch::operator>>(int& i)
{ 
  Read(&i, sizeof(int));
  return *this; 
}

CArch& CArch::operator>>(long& l)
{ 
  //   if(!IsLoading())
  //     AfxThrowArchiveException(CArchException::writeOnly,m_strFileName);
  //   if (m_lpBufCur + sizeof(LONG) > m_lpBufMax)
  //     FillBuffer(UINT(sizeof(LONG) - (m_lpBufMax - m_lpBufCur)));
  //   l = *(UNALIGNED LONG*)m_lpBufCur; m_lpBufCur += sizeof(LONG); return *this; 
  Read(&l, sizeof(LONG));
  return *this; 
}

inline CArch::CArch(const CArch& /* arSrc */)
{ }
inline void CArch::operator=(const CArch& /* arSrc */)
{ }


////////////////////////////////////////////////////////////////////////////

CArch::CArch(char * fname, int nMode) 
{
  char Mode[3] = "rb";   //Load

  m_strFileName = fname;
  m_nMode = nMode;
  if (nMode==0)  
    Mode[0]='w';    // Store 

  m_pFile = fopen(fname, Mode);
  if (m_pFile == NULL)
  {
    printf("Open %s error", fname);
    throw(-21);
  } 
}

CArch::~CArch()
{
  if (m_pFile != NULL)
    Close();
  //exit(-22);    // abort completely shuts down the archive
}


void CArch::Close()
{
  fclose(m_pFile);
  m_pFile = NULL;
}

UINT CArch::Read(void* lpBuf, UINT nMax)
{
  if (nMax == 0)     return 0;
  if(lpBuf == NULL)  return 0;
  return fread(lpBuf, 1,nMax, m_pFile);
  }

void CArch::Write(const void* lpBuf, UINT nMax)
{
  if (nMax == 0)      return;
  if(lpBuf == NULL)   return;

  fwrite(lpBuf, 1, nMax, m_pFile);
}


// special functions for text file input and output
void CArch::WriteString(string S)
{
  int len= S.size();
  Write(&len,sizeof(int));
  Write(S.c_str(), len);
}


void CArch::ReadString(string& S)
{
  S = "";    // empty string without deallocating
  int sz;
  Read(&sz, sizeof(int));
  char * bf = new char[sz+1];
  Read(&bf[0], sz);
  bf[sz]='\0';
  S.reserve(sz+1);
  S=bf;
  delete [] bf;
}

