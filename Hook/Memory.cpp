#include <windows.h>
#include "Memory.h"

CMemory::CMemory(void)
{
}

CMemory::~CMemory(void)
{
}

void CMemory::WriteMemory(DWORD dwAddress, LPVOID lpBuffer, int nSize)
{
	DWORD dwOldProtect, dwNewProtect;
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)(dwAddress), nSize, PAGE_READWRITE, &dwOldProtect);
	memcpy((LPVOID)(dwAddress), lpBuffer, nSize);
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)(dwAddress), nSize, dwOldProtect, &dwNewProtect);
}

void CMemory::Nop(DWORD dwAddress, int size)
{
	DWORD dwOldProtect, dwNewProtect;
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)(dwAddress), size, PAGE_READWRITE, &dwOldProtect);
	memset((LPVOID)(dwAddress), 0x90, size);
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)(dwAddress), size, dwOldProtect, &dwNewProtect);
}