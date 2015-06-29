#pragma once

class CIni
{
public:
	CIni(char *lpFileName);
	~CIni(void);
	int ReadInt(char* lpAppName, char* lpKeyName, int nDefault);
	void ReadString(char *lpAppName, char *lpKeyName, char *lpDefault, char *lpReturnedString, unsigned int nSize);
	void WriteInt(char *lpAppName, char *lpKeyName, int nValue, bool bHex);
	void WriteString(char *lpAppName, char *lpKeyName, char *lpString);
private:
	char m_FileName[256];
};
