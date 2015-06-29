#pragma once

class CMemory
{
public:
	CMemory(void);
	~CMemory(void);
	static void WriteMemory(DWORD dwAddress, LPVOID lpBuffer, int nSize);
	static void Nop(DWORD dwAddress, int size);
};
