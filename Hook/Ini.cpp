#include <windows.h>
#include <cstdio>
#include "Ini.h"

CIni::CIni(char *lpFileName)
{
	memset(m_FileName, 0x00, 256);
	memcpy(m_FileName, lpFileName, strlen(lpFileName));
}

CIni::~CIni(void)
{
}

int CIni::ReadInt(char *lpAppName, char *lpKeyName, int nDefault)
{
	return GetPrivateProfileInt(lpAppName, lpKeyName, nDefault, m_FileName);
}

void CIni::ReadString(char *lpAppName, char *lpKeyName, char *lpDefault, char *lpReturnedString, unsigned int nSize)
{
	GetPrivateProfileString(lpAppName, lpKeyName, lpDefault, lpReturnedString, nSize, m_FileName);
}

void CIni::WriteInt(char *lpAppName, char *lpKeyName, int nValue, bool bHex)
{
	char buf[32];
	if(bHex == true)
		sprintf_s(buf, 32, "0x%.2X", nValue);
	else
		sprintf_s(buf, 32, "%d", nValue);
	WritePrivateProfileString(lpAppName, lpKeyName, buf, m_FileName);
}

void CIni::WriteString(char *lpAppName, char *lpKeyName, char *lpString)
{
	WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, m_FileName);
}