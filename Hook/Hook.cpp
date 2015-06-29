#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <commctrl.h>
#include "Tibia.h"
#include "Win.h"
#include "Packet.h"
#include "Constants.h"
#include "Resource1.h"
#include "Toggler.h"
#include "Memory.h"
#include "Hotkeys.h"
#include "Ini.h"
#include "LootList.h"
#include "Hook.h"

#define _DEBUGMODE 0

#pragma comment(lib, "comctl32.lib")

SItemList HealingItems[] = {
	{"Ultimate Health Potion",		Items::UltimateHealthPotion},
	{"Great Health Potion",			Items::GreatHealthPotion},
	{"Strong Health Potion",		Items::StrongHealthPotion},
	{"Health Potion",				Items::HealthPotion},
	{"Great Spirit Potion",			Items::GreatSpiritPotion},
	{"Ultimate Healing Rune",		Items::UltimateHealingRune},
	{"Intense Healing Rune",		Items::IntenseHealingRune}
};

DWORD ping = 0;

_PrintText PrintText = 0;
_PlaceObject PlaceObject = 0;
_PlaceObject2 PlaceObject2 = 0;
_StatusMessage StatusMessage = (_StatusMessage)Address::FUNC_STATUSMESSAGE;
_MessageHandle MessageHandle = 0;
_LookMessage LookMessage = 0;

void GetError(DWORD dwErrorCode, char *lpFunction)
{
	char *error;
	char buffer[512];
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, dwErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&error, 0, NULL );
	sprintf_s(buffer, 512, "Function: %s\nError: %s", lpFunction, error);
	MessageBox(g_hWnd, buffer, PROGRAMNAME, MB_ICONHAND);
	LocalFree(error);
}

void MyPrintName(int nSurface, int nX, int nY, int nFont, int nRed, int nGreen, int nBlue, char* lpText, int nAlign)
{
	PrintText(nSurface, nX, nY, nFont, nRed, nGreen, nBlue, lpText, nAlign);

	SCreature *c = (SCreature*)(lpText - 4);
	if(c->CreatureID < 0x40000000)
	{
		if(AllyNameListCount > 0)
		{
			for(int i = 0; i < AllyNameListCount; i++)
			{
				if(stricmp(lpText, AllyNameList[i].Name) == 0)
				{
					if(bShowTeamMarks == true)
					{
						nX = nX - 12;
						PrintText(nSurface, nX, nY, nFont, 0, 128, 255, "A", nAlign);
					}
					if(bShowTeamColors == true)
					{
						if(c->OutfitType != 0)
						{
							c->OutfitData[0] = 86;
							c->OutfitData[1] = 86;
							c->OutfitData[2] = 86;
							c->OutfitData[3] = 86;
						}
					}
					return;
				}
			}
		}
		if(EnemyNameListCount > 0)
		{
			for(int i = 0; i < EnemyNameListCount; i++)
			{
				if(stricmp(lpText, EnemyNameList[i].Name) == 0)
				{
					if(bShowTeamMarks == true)
					{
						nX = nX - 12;
						PrintText(nSurface, nX, nY, nFont, 255, 0, 0, "E", nAlign);
					}
					if(bShowTeamColors == true)
					{
						if(c->OutfitType != 0)
						{
							c->OutfitData[0] = 94;
							c->OutfitData[1] = 94;
							c->OutfitData[2] = 94;
							c->OutfitData[3] = 94;
						}
					}
					return;
				}
			}
		}
		if(bShowTeamColors == true)
		{
			if(c->OutfitType != 0)
			{
				c->OutfitData[0] = 0;
				c->OutfitData[1] = 0;
				c->OutfitData[2] = 0;
				c->OutfitData[3] = 0;
			}
		}
	}
}

void MyPlaceObject(int nX, int nY, int nZ, int nID, int nCount, int u1)
{
	if(bMagwallTimer == true)
	{
#if _DEBUGMODE
		if(nID == 10181 || nID == 10182 || nID == 2128 || nID == 2130 || nID == 2129 || nID == 3031)
#else
		if(nID == 10181 || nID == 10182 || nID == 2128 || nID == 2130 || nID == 2129)
#endif
		{
			unsigned int *playerX = (unsigned int*)Address::PLAYER_X;
			unsigned int *playerY = (unsigned int*)Address::PLAYER_Y;
			unsigned int *playerZ = (unsigned int*)Address::PLAYER_Z;
			
			int x = -(8 - nX);
			int y = -(6 - nY);

			int z;
			if(*playerZ <= 7)
			{
				z = (*playerZ) - (7 - nZ);
				Magwalls[MagwallCount].Z = (7 - nZ);
			}
			else
			{
				z = -(2 - nZ);
				Magwalls[MagwallCount].Z = *playerZ + (2 - nZ);
			}

			Magwalls[MagwallCount].X = *playerX + x + z;
			Magwalls[MagwallCount].Y = *playerY + y + z;
			if(nID == 10181 || nID == 2128 || nID == 2129)
				Magwalls[MagwallCount].Time = timeGetTime() + (1000 * 20);
			else
				Magwalls[MagwallCount].Time = timeGetTime() + (1000 * 45);
			MagwallCount = (MagwallCount < MagwallArray-1) ? MagwallCount + 1 : 0;
		}
	}
	PlaceObject(nX, nY, nZ, nID, nCount, u1);
}

void MyPlaceObject2(int nX, int nY, int nZ, int u1, int nID, int nCount, int u2)
{
	if(bMagwallTimer == true)
	{
#if _DEBUGMODE
		if(nID == 10181 || nID == 10182 || nID == 2128 || nID == 2130 || nID == 2129 || nID == 3031)
#else
		if(nID == 10181 || nID == 10182 || nID == 2128 || nID == 2130 || nID == 2129)
#endif
		{
			unsigned int *playerX = (unsigned int*)Address::PLAYER_X;
			unsigned int *playerY = (unsigned int*)Address::PLAYER_Y;
			unsigned int *playerZ = (unsigned int*)Address::PLAYER_Z;
			
			int x = -(8 - nX);
			int y = -(6 - nY);

			int z;
			if(*playerZ <= 7)
			{
				z = (*playerZ) - (7 - nZ);
				Magwalls[MagwallCount].Z = (7 - nZ);
			}
			else
			{
				z = -(2 - nZ);
				Magwalls[MagwallCount].Z = *playerZ + (2 - nZ);
			}

			Magwalls[MagwallCount].X = *playerX + x + z;
			Magwalls[MagwallCount].Y = *playerY + y + z;
			if(nID == 10181 || nID == 2128 || nID == 2129)
				Magwalls[MagwallCount].Time = timeGetTime() + (1000 * 20);
			else
				Magwalls[MagwallCount].Time = timeGetTime() + (1000 * 45);
			MagwallCount = (MagwallCount < MagwallArray-1) ? MagwallCount + 1 : 0;
		}
	}
	PlaceObject2(nX, nY, nZ, u1, nID, nCount, u2);
}


void MyPrintFps(int nSurface, int nX, int nY, int nFont, int nRed, int nGreen, int nBlue, char* lpText, int nAlign)
{
	bool *fps = (bool*)Address::SHOW_FPS;
	if(*fps == true)
	{
		PrintText(nSurface, nX, nY, nFont, nRed, nGreen, nBlue, lpText, nAlign);
		nY += 12;
	}
	if(CTibia::IsOnline() == true)
	{
		if(bMagwallTimer == true)
			CTibia::MagwallTimer();
		if(bSpellTimers == true)
			CTibia::SpellTimers();
		if(bShowTarget == true)
		{
			if(PlayerTarget > 0)
			{
				SCreature *c = (SCreature*)CTibia::GetPlayer(PlayerTarget, false);
				if(c > 0)
				{
					char buf[256];
					sprintf_s(buf, 256, "Target: %s", c->Name);
					PrintText(nSurface, nX, nY, nFont, 255, 255, 255, buf, nAlign);
				}
				else
				{
					PrintText(nSurface, nX, nY, nFont, 255, 255, 255, "Target:", nAlign);
					PlayerTarget = 0;
				}
			}
			else
			{
				PrintText(nSurface, nX, nY, nFont, 255, 255, 255, "Target:", nAlign);
			}
			nY += 12;
		}

#if _DEBUGMODE
		char buf[256];
		SCreature *c = (SCreature*)CTibia::GetPlayer();
		if(c != 0)
		{
			sprintf_s(buf, 256, "ID: %.8X", c->CreatureID);
			PrintText(nSurface, nX, nY, nFont, 255, 255, 255, buf, nAlign);
			nY += 12;
			sprintf_s(buf, 256, "Name: %s", c->Name);
			PrintText(nSurface, nX, nY, nFont, 255, 255, 255, buf, nAlign);
			nY += 12;
			sprintf_s(buf, 256, "Health: %d", c->Health);
			PrintText(nSurface, nX, nY, nFont, 255, 255, 255, buf, nAlign);
			nY += 12;
			sprintf_s(buf, 256, "Speed: %d", c->Speed);
			PrintText(nSurface, nX, nY, nFont, 255, 255, 255, buf, nAlign);
			nY += 12;
			sprintf_s(buf, 256, "X: %d", c->X);
			PrintText(nSurface, nX, nY, nFont, 255, 255, 255, buf, nAlign);
			nY += 12;
			sprintf_s(buf, 256, "Y: %d", c->Y);
			PrintText(nSurface, nX, nY, nFont, 255, 255, 255, buf, nAlign);
			nY += 12;
			sprintf_s(buf, 256, "Z: %d", c->Z);
			PrintText(nSurface, nX, nY, nFont, 255, 255, 255, buf, nAlign);
			nY += 12;
			sprintf_s(buf, 256, "MoveX: %d", c->MoveX);
			PrintText(nSurface, nX, nY, nFont, 255, 255, 255, buf, nAlign);
			nY += 12;
			sprintf_s(buf, 256, "MoveY: %d", c->MoveY);
			PrintText(nSurface, nX, nY, nFont, 255, 255, 255, buf, nAlign);
			nY += 12;
			sprintf_s(buf, 256, "IsWalking: %d", c->IsWalking);
			PrintText(nSurface, nX, nY, nFont, 255, 255, 255, buf, nAlign);
			nY += 12;
			sprintf_s(buf, 256, "IsVisible: %d", c->IsVisible);
			PrintText(nSurface, nX, nY, nFont, 255, 255, 255, buf, nAlign);
			nY += 12;
			sprintf_s(buf, 256, "OutfitType: %d", c->OutfitType);
			PrintText(nSurface, nX, nY, nFont, 255, 255, 255, buf, nAlign);
			nY += 12;
		}
		else
		{
			sprintf_s(buf, 256, "Player Not Found");
			PrintText(nSurface, nX, nY, nFont, 255, 0, 0, buf, nAlign);
			nY += 12;
		}
#endif
	}
}

void MyMessageHandle(unsigned int u1, char *lpMessage, unsigned int nType, char *lpName, unsigned int nLevel, unsigned int u2, unsigned int u3, unsigned int u4, unsigned int u5, unsigned int u6, unsigned int u7, unsigned int u8, unsigned int u9)
{
	if(bSpellTimers == true)
	{
		SCreature *c = (SCreature*)CTibia::GetPlayer();
		if(strcmp(lpName, c->Name) == 0)
		{
			if(stricmp(lpMessage, "Utani Hur") == 0)
				isHaste = timeGetTime() + (1000 * 33);
			else if(stricmp(lpMessage, "Utani Gran Hur") == 0)
				isHaste = timeGetTime() + (1000 * 22);
			else if(stricmp(lpMessage, "Utamo Vita") == 0)
				isMagicShield = timeGetTime() + (1000 * 200);
			else if(stricmp(lpMessage, "Utana Vid") == 0)
				isInvisible = timeGetTime() + (1000 * 200);
#if _DEBUGMODE
			else if(stricmp(lpMessage, "test") == 0)
				isInvisible = timeGetTime() + (1000 * 200);
#endif
		}
	}
	MessageHandle(u1, lpMessage, nType, lpName, nLevel, u2, u3, u4, u5, u6, u7, u8, u9);
}

void MyLookMessage(int arg1, char *message)
{
	if(bShowItemID == true)
	{
		if(strstr(message, "You see") != 0)
		{
			char buf[512];
			unsigned int *itemID = (unsigned int*)Address::LOOKITEMID;
			sprintf_s(buf, 512, "%s\n(ItemID %d).", message, *itemID);
			LookMessage(arg1, buf);
		}
		else
			LookMessage(arg1, message);
	}
	else
		LookMessage(arg1, message);
}

void MyPrintDmg(int nSurface, int nX, int nY, int nFont, int nRed, int nGreen, int nBlue, char* lpText, int nAlign)
{
	PrintText(nSurface, nX, nY, nFont, nRed, nGreen, nBlue, lpText, nAlign);
}

DWORD HookCall(DWORD dwAddress, DWORD dwFunction)
{
	DWORD dwOldProtect, dwNewProtect, dwOldCall, dwNewCall;
	BYTE callByte[5] = {0xE8, 0x00, 0x00, 0x00, 0x00};

	dwNewCall = dwFunction - dwAddress - 5;
	memcpy(&callByte[1], &dwNewCall, 4);
	
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)(dwAddress), 5, PAGE_READWRITE, &dwOldProtect);
	memcpy(&dwOldCall, (LPVOID)(dwAddress+1), 4);
	memcpy((LPVOID)(dwAddress), &callByte, 5);
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)(dwAddress), 5, dwOldProtect, &dwNewProtect);
	return dwOldCall;
}

PBYTE HookApi(DWORD dwAddress, DWORD dwFunction)
{
	DWORD dwOldProtect, dwNewProtect;
	BYTE *dwOld = new BYTE[10];
	BYTE dwJmp[5] = {0xE9, 0x00, 0x00, 0x00, 0x00};
	*(DWORD*)(dwJmp+1) = dwFunction - dwAddress - 5;
	dwOld[5] = 0xE9;
	*(DWORD*)(dwOld+6) = dwAddress - (DWORD)(dwOld+5);

	VirtualProtectEx(GetCurrentProcess(), (LPVOID)(dwAddress), 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	memcpy(dwOld, (LPVOID)(dwAddress), 5);
	memcpy((LPVOID)(dwAddress), &dwJmp, 5);
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)(dwAddress), 5, dwOldProtect, &dwNewProtect);

	return dwOld;
}

void DetourCall(DWORD dwCallAddress, DWORD dwNewFunction, LPDWORD pOldFunction)
{
	DWORD dwProtect;
	VirtualProtect((LPVOID)(dwCallAddress), 5, PAGE_READWRITE, &dwProtect);
	if(pOldFunction != 0)
		*pOldFunction = dwCallAddress + *(DWORD*)(dwCallAddress+1) + 5;
	*(DWORD*)(dwCallAddress+1) = dwNewFunction - dwCallAddress - 5;
	VirtualProtect((LPVOID)(dwCallAddress), 5, dwProtect, &dwProtect);
}

void DetourFunction(DWORD dwFunctionAddress, DWORD dwNewFunction, LPVOID pOldFunction)
{
	DWORD dwProtect;
	BYTE jumpCode[5] = {0xE9, 0x00, 0x00, 0x00, 0x00};
	*(DWORD*)(jumpCode+1) = dwNewFunction - dwFunctionAddress - 5;

	VirtualProtect((LPVOID)(dwFunctionAddress), 5, PAGE_READWRITE, &dwProtect);
	if(pOldFunction != 0)
	{
		BYTE *oldCode = new BYTE[10];
		oldCode[5] = 0xE9;
		*(DWORD*)(oldCode+6) = dwFunctionAddress - (DWORD)(oldCode+5);
		memcpy(oldCode, (LPVOID)(dwFunctionAddress), 5);
		*(DWORD*)pOldFunction = (DWORD)oldCode;
	}
	memcpy((LPVOID)(dwFunctionAddress), &jumpCode, 5);
	VirtualProtect((LPVOID)(dwFunctionAddress), 5, dwProtect, &dwProtect);
}

void UnhookCall(DWORD dwAddress, DWORD dwOldCall)
{
	DWORD dwOldProtect, dwNewProtect;
	BYTE callByte[5] = {0xE8, 0x00, 0x00, 0x00, 0x00};

	memcpy(&callByte[1], &dwOldCall, 4);
	
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)(dwAddress), 5, PAGE_READWRITE, &dwOldProtect);
	memcpy((LPVOID)(dwAddress), &callByte, 5);
	VirtualProtectEx(GetCurrentProcess(), (LPVOID)(dwAddress), 5, dwOldProtect, &dwNewProtect);
}

void SetWndProc(HWND hWnd, HINSTANCE hInstance)
{
	FARPROC lpfnSubClassProc;
	lpfnSubClassProc = MakeProcInstance((FARPROC)ListBoxProc, hInstance);
	lpfnWndProc = (FARPROC)SetWindowLongPtr(hWnd, GWL_WNDPROC, (DWORD)lpfnSubClassProc);
}

void InitTray(HWND hWnd)
{
	niData.cbSize = sizeof(NOTIFYICONDATA);
	niData.uID = 1;
	niData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	niData.hIcon = LoadIcon(hMod, MAKEINTRESOURCE(IDI_ICON1));
	strcpy_s(niData.szTip, 64, PROGRAMNAME);
	niData.hWnd = hWnd;
	niData.uCallbackMessage = WM_TRAY;
	Shell_NotifyIcon(NIM_ADD, &niData);
}

void ShowContextMenu(HWND hWnd)
{
	POINT pt;
	GetCursorPos(&pt);
	HMENU hMenu = CreatePopupMenu();
	if(hMenu)
	{
		if(bMinimized == true)
			InsertMenu(hMenu, -1, MF_BYPOSITION, ID_FILE_MINIMIZE, "Maximize");
		else
			InsertMenu(hMenu, -1, MF_BYPOSITION, ID_FILE_MINIMIZE, "Minimize");
		if(!IsWindowVisible(hWnd))
			InsertMenu(hMenu, -1, MF_BYPOSITION, ID_FILE_HIDE, "Show");
		else
			InsertMenu(hMenu, -1, MF_BYPOSITION, ID_FILE_HIDE, "Hide");

		SetForegroundWindow(hWnd);
		TrackPopupMenu(hMenu, TPM_BOTTOMALIGN, pt.x, pt.y, 0, hWnd, NULL );
		DestroyMenu(hMenu);
	}
}

void InitializeWindow(HWND hWnd, HINSTANCE hInstance)
{
	CWin w(hWnd, hInstance);
	w.SetFont(8, "MS Sans Serif");
	w.CreateGroupBox("Auto-Healer", 5, 0, 350, 100);
	w.CreateGroupBox("Aimbot Core", 5, 100, 350, 185);
	w.CreateGroupBox("Extra", 5, 285, 350, 100);

	// Labels
	w.CreateLabel("Spell Hi:", 15, 15, 45, 15);
	w.CreateLabel("Spell Lo:", 15, 35, 45, 15);
	w.CreateLabel("Health:", 170, 15, 35, 20);
	w.CreateLabel("Health:", 170, 35, 35, 20);
	w.CreateLabel("Health:", 170, 55, 35, 20);
	w.CreateLabel("Mana:", 265, 15, 30, 20);
	w.CreateLabel("Mana:", 265, 35, 30, 20);
	w.CreateLabel("Delay:", 265, 55, 30, 20);
	w.CreateLabel("Enemies:", 15, 115, 45, 15);
	w.CreateLabel("Allies:", 185, 115, 45, 15);
	w.CreateLabel("Health %:", 185, 200, 45, 20);
	w.CreateLabel("Track Magwalls:", 185, 300, 100, 20);

	// EditBox
	hSpellHi			= w.CreateEditBox("Exura", 60, 15, 100, 15, 0, false);
	hSpellHiHealth		= w.CreateEditBox("500", 210, 15, 45, 15, 0, true);
	hSpellHiMana		= w.CreateEditBox("25", 300, 15, 45, 15, 0, true);
	hSpellLo			= w.CreateEditBox("Exura Vita", 60, 35, 100, 15, 0, false);
	hSpellLoHealth		= w.CreateEditBox("250", 210, 35, 45, 15, 0, true);
	hSpellLoMana		= w.CreateEditBox("160", 300, 35, 45, 15, 0, true);
	hHealItem			= w.CreateListBox("", 15, 55, 145, 20, 0);
	hHealItemHealth		= w.CreateEditBox("250", 210, 55, 45, 15, 0, true);
	hAutoHealDelay		= w.CreateEditBox("100", 300, 55, 45, 15, 0, true);

	hUseSpellHi			= w.CreateCheckBox("Use Spell Hi", 15, 75, 80, 15, IDC_AUTOHEAL);
	hUseSpellLo			= w.CreateCheckBox("Use Spell Lo", 105, 75, 80, 15, IDC_AUTOHEAL);
	hUseHealItem		= w.CreateCheckBox("Use Item", 195, 75, 65, 15, IDC_AUTOHEAL);
	hAntiParalyze		= w.CreateCheckBox("AntiParalyze", 270, 75, 80, 15, IDC_AUTOHEAL);
	hEnemyList			= w.CreateListBox("", 15, 135, 160, 70, 1200);
	hAllyList			= w.CreateListBox("", 185, 135, 160, 70, 1201);
	hAllyHealHealth		= w.CreateEditBox("80", 300, 200, 45, 15, 0, true);
	hTrackMagwalls		= w.CreateEditBox("128", 300, 300, 45, 15, 0, true);

	SetWndProc(hEnemyList, hInstance);
	SetWndProc(hAllyList, hInstance);

	hUseAllySpell		= w.CreateCheckBox("Use Exura Sio", 185, 220, 160, 15, IDC_AUTOHEALALLY);
	hUseAllyItem		= w.CreateCheckBox("Use Ultimate Healing Rune", 185, 240, 160, 15, IDC_AUTOHEALALLY);
	hTeamMarks			= w.CreateCheckBox("Team Marks", 15, 200, 160, 15, IDC_TEAMMARKS);
	hTeamColors			= w.CreateCheckBox("Team Colors", 15, 220, 160, 15, IDC_TEAMCOLORS);
	hLeastHPP			= w.CreateCheckBox("Choose target with least hp%", 15, 240, 160, 15, IDC_LEASTHPP);
	hShootClosedBP		= w.CreateCheckBox("Shoot from closed bps", 15, 260, 160, 15, IDC_SHOOTFROMBP);
	hDoubleHealing		= w.CreateCheckBox("Double Healing", 185, 260, 160, 15, IDC_DOUBLEHEALING);

	hMagwallTimer		= w.CreateCheckBox("Magwall Timer", 15, 300, 160, 15, IDC_MAGWALLTIMER);
	hSpellTimers		= w.CreateCheckBox("Spell Timers", 15, 320, 160, 15, IDC_SPELLTIMERS);
	hXRay				= w.CreateCheckBox("X-Ray", 185, 320, 160, 15, IDC_XRAY);
	hShowTarget			= w.CreateCheckBox("Show Target", 15, 340, 160, 15, IDC_SHOWTARGET);
	hShowItemID			= w.CreateCheckBox("Show Item ID", 185, 340, 160, 15, IDC_SHOWITEMID);
	hFullLight			= w.CreateCheckBox("ReAttack", 15, 360, 160, 15, IDC_FULLLIGHT);
	
	for(int i = 0; i < (sizeof(HealingItems)/sizeof(HealingItems[0])); i++)
		SendMessage(hHealItem, LB_INSERTSTRING, -1, (LPARAM)HealingItems[i].Name);
}

DWORD WINAPI AutoHealerThread(LPVOID lpParam)
{
	while(true)
	{
		if(CTibia::IsOnline() == true)
		{
			if(bAutoHealer == true)
			{
				CTibia::AutoSelfHeal();
			}
			if(bAutoAllyHealer == true)
			{
				CTibia::AutoAllyHeal();
			}
			if(bAntiParalyze == true)
			{
				CTibia::AntiParalyze();
			}

			if(*(unsigned int*)Address::PLAYER_ATTACK_ID != 0)
			{
				PlayerTarget = *(unsigned int*)Address::PLAYER_ATTACK_ID;
			}
			else
			{
				if(bReAttack == true && PlayerTarget != 0)
				{
					CTibia::AttackTarget(HotkeyCreature::Target);
				}
			}
		}
		else
			PlayerTarget = 0;
		if(bSpy1 == true)
		{
			int *playerZ = (int*)Address::PLAYER_Z;
			if(levelSpyPZ != *playerZ || CTibia::IsOnline() == false)
				CTibia::LevelSpy();
		}
		Sleep(10);
	}
	return 0;
}

DWORD WINAPI MainThread(LPVOID lpParam)
{
	InitCommonControls();
	g_hInstance = hMod;
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInstance;
	wcex.hIcon			= 0;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(16);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDR_MENU1);
	wcex.lpszClassName	= PROGRAMCLASSNAME;
	wcex.hIconSm		= 0;

	RegisterClassEx(&wcex);

	g_hWnd = CreateWindowExA(WS_EX_TOOLWINDOW|WS_EX_TOPMOST, PROGRAMCLASSNAME, PROGRAMNAME, WS_SYSMENU, CW_USEDEFAULT, 0, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, g_hInstance, NULL);

	InitTray(g_hWnd);
	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);

	MSG msg;

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK ListBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_DELETE:
			{
				int i;
				i = (int)SendMessage(hWnd, LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
				if(i != LB_ERR)
					SendMessage(hWnd, LB_DELETESTRING, (WPARAM)i, (LPARAM)0);
				UpdateTeamList();
				return 0;
			}
		}
		break;
	}
	return CallWindowProc((WNDPROC)lpfnWndProc, hWnd, message, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	
	switch (message)
	{
	case WM_CREATE:
		InitializeWindow(hWnd, g_hInstance);
		LoadHotkeys();
		hMainThread[1] = CreateThread(NULL, 0, AutoHealerThread, 0, 0, &dwMainThreadId[1]);
		LoadSettings();
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDC_AUTOHEAL:
			{
				int ret = SendMessage(hHealItem, LB_GETTOPINDEX, 0, 0);
				SendMessage(hHealItem, LB_SETCURSEL, ret, 0);
				ToggleHealer();
			}break;
		case IDC_AUTOHEALALLY:
			{
				ToggleAllyHealer();
			}break;
		case IDC_TEAMMARKS:
			{
				ToggleTeamMarks();
			}break;
		case IDC_TEAMCOLORS:
			{
				ToggleTeamColors();
			}break;
		case IDC_SHOOTFROMBP:
			{
				ToggleShootFromClosedBp();
			}break;
		case IDC_DOUBLEHEALING:
			{
				ToggleDoubleHealing();
			}break;
		case IDC_MAGWALLTIMER:
			{
				ToggleMagwallTimer();
			}break;
		case IDC_SPELLTIMERS:
			{
				ToggleSpellTimers();
			}break;
		case IDC_XRAY:
			{
				ToggleXRay();
			}break;
		case IDC_LEASTHPP:
			{
				ToggleLeastHealth();
			}break;
		case IDC_SHOWTARGET:
			ToggleShowTarget();
			break;
		case IDC_FULLLIGHT:
			ToggleFullLight();
			break;
		case IDC_SHOWITEMID:
			ToggleShowItemID();
			break;
		case ID_FILE_ADDTEAM:
			{
				DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, AddTeamProc);
			}break;
		case ID_FILE_SAVESETTINGS:
			{
				SaveSettings();
			}break;
		case ID_FILE_LOADSETTINGS:
			{
				LoadSettings();
			}break;
		case ID_FILE_OPENTEAMLIST:
			{
				OnOpenFriendList(hWnd, g_hInstance);
			}break;
		case ID_FILE_SAVETEAMLIST:
			{
				OnSaveFriendList(hWnd, g_hInstance);
			}break;
		case ID_FILE_HIDE:
			{
				ShowHide(g_hWnd);
			}break;
		case ID_FILE_MINIMIZE:
			{
				Minimize();
			}break;
		case ID_FILE_HOTKEYS:
			{
				DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_HOTKEYS), hWnd, HotkeysProc);
			}break;
		case ID_FILE_SAVEHOTKEYS:
			{
				SaveHotkeys();
			}break;
		case ID_FILE_LOADHOTKEYS:
			{
				LoadHotkeys();
			}break;
		case ID_HELP_ABOUT:
			{
				DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutProc);
			}break;
		case ID_TOOLS_LOOTER:
			{
				DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_LOOTLIST), hWnd, LootListProc);
			}break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}break;
	case WM_TRAY:
		switch(lParam)
		{
		case WM_LBUTTONDBLCLK:
			ShowHide(g_hWnd);
			break;
		case WM_RBUTTONDOWN:
		case WM_CONTEXTMENU:
			ShowContextMenu(g_hWnd);
			break;
		}break;
	case WM_DESTROY:
	case WM_CLOSE:
		ShowHide(g_hWnd);
		return 0;
	case WM_KEYDOWN:
		{
			int mod = KeyMod();
			if(LOBYTE(LOWORD(wParam)) == VK_F12 && (mod & 2) == 2)
				ShowHide(g_hWnd);
		}break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int CALLBACK AddTeamProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	
	switch (message)
	{
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hWnd, IDC_RADIO1), BM_SETCHECK, BST_CHECKED, 0);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDC_ADD:
			{
				char name[32];
				GetWindowText(GetDlgItem(hWnd, IDC_ADDNAME), name, 30);
				if(strlen(name) <= 0)
					return 0;
				if(SendMessage(GetDlgItem(hWnd, IDC_RADIO1), BM_GETCHECK, 0, 0) == BST_CHECKED)
					SendMessage(hAllyList, LB_INSERTSTRING, -1, (LPARAM)name);
				else
					SendMessage(hEnemyList, LB_INSERTSTRING, -1, (LPARAM)name);
				SetWindowText(GetDlgItem(hWnd, IDC_ADDNAME), "");
				UpdateTeamList();
			}break;
		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;
		default:
			return 0;
		}break;
	case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hWnd, 0);
		break;
	default:
		return 0;
	}
	return 0;
}

int CALLBACK AboutProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;
		default:
			return 0;
		}break;
	case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hWnd, 0);
		break;
	default:
		return 0;
	}
	return 0;
}

int KeyMod(void)
{
	int mod = 0;
	BYTE keystate[256];
	GetKeyboardState(keystate);
	if(keystate[VK_SHIFT] & 0x80)
		mod |= 1;
	if(keystate[VK_CONTROL] & 0x80)
		mod |= 2;
	if(keystate[VK_MENU] & 0x80)
		mod |= 4;
	if(keystate[VK_PRIOR] & 0x80 ||
		keystate[VK_NEXT] & 0x80 ||
		keystate[VK_END] & 0x80 ||
		keystate[VK_HOME] & 0x80 ||
		keystate[VK_INSERT] & 0x80 ||
		keystate[VK_NUMLOCK] & 0x80)
		mod |= 8;
	return mod;
}

LRESULT CALLBACK HookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int vkKey;
	if(bHotkeys == true && CTibia::IsOnline() == true)
	{
		switch(uMsg)
		{
		case WM_CHAR:
			{
				vkKey = LOBYTE(LOWORD(wParam));
				if(vkKey >= 'a' && vkKey <= 'z')
					vkKey += 'A' - 'a';
				for(int i = 0; i < 11; i++)
					if(Hotkeys[i].Enabled == true && MapVirtualKey(Hotkeys[i].Key, 2) == vkKey)
						return 0;
			}break;
		case WM_KEYDOWN:
			{
				vkKey = LOBYTE(LOWORD(wParam));
				if(vkKey == VK_ESCAPE)
				{
					PlayerTarget = 0;
				}
				int mod = KeyMod();
				for(int i = 0; i < 11; i++)
				{
					if(Hotkeys[i].Enabled == true && LOBYTE(Hotkeys[i].Key) == vkKey)
					{
						if(HIBYTE(Hotkeys[i].Key) != mod)
							break;
						ExecuteCommand(&Hotkeys[i]);
						return 0;
					}
				}
			}break;
		}
	}
	if(uMsg == WM_KEYDOWN)
	{
		int mod = KeyMod();
		if(LOBYTE(LOWORD(wParam)) == VK_F12 && (mod & 2) == 2)
			ShowHide(g_hWnd);
	}
    return CallWindowProc(wndProc, hWnd, uMsg, wParam, lParam ); 
} 

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		{
			DisableThreadLibraryCalls(hModule);
			hMod = hModule;
			HWND temp = 0;
			do{
				temp = FindWindowEx(0, temp, "TibiaClient", 0);
				if(!temp)
					break;
				DWORD pidtemp;
				GetWindowThreadProcessId(temp, &pidtemp);
				if(pidtemp == GetCurrentProcessId())
				{
					wndProc = (WNDPROC)SetWindowLongPtr(temp, GWLP_WNDPROC, (LONG)HookProc);
					if(!wndProc)
						GetError(GetLastError(), "SetWindowLongPtr()");
					break;
				}
			}while(true);

			hMainThread[0] = CreateThread(NULL, 0, MainThread, 0, 0, &dwMainThreadId[0]);

			DetourCall(Address::CALL_PRINTFPS, (DWORD)&MyPrintFps, (LPDWORD)&PrintText);
			DetourCall(Address::CALL_PRINTNAME, (DWORD)&MyPrintName, 0);
			DetourCall(Address::CALL_PLACEOBJECT, (DWORD)&MyPlaceObject, (LPDWORD)&PlaceObject);
			DetourCall(Address::CALL_PLACEOBJECT2, (DWORD)&MyPlaceObject2, (LPDWORD)&PlaceObject2);
			DetourCall(Address::CALL_MESSAGEHANDLE, (DWORD)&MyMessageHandle, (LPDWORD)&MessageHandle);
			DetourCall(Address::CALL_LOOKMESSAGE, (DWORD)&MyLookMessage, (LPDWORD)&LookMessage);

			CMemory::Nop(Address::ADDR_ALWAYSFPS, 6);
		}break;
	case DLL_PROCESS_DETACH:
		{
			Shell_NotifyIcon(NIM_DELETE, &niData);
		}break;
	}
    return 1;
}

void OnOpenFriendList(HWND hWnd, HINSTANCE hInstance)
{
	char FileName[MAX_PATH];
	GetModuleFileName(hMod, FileName, sizeof(FileName));
	if(strrchr(FileName, '\\') != NULL)
		*strrchr(FileName, '\\') = 0;
	strcat_s(FileName, MAX_PATH, "\\Teamlist.txt");

	FILE *file;
	file = fopen(FileName, "r+");
	if(file != 0)
	{
		char buf[256], *name;
		int state = 0;
		SendMessage(hEnemyList, LB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
		SendMessage(hAllyList, LB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
		while(!feof(file))
		{
			*buf = 0;
			fgets(buf, 256, file);
			if(strlen(buf) <= 1)
				continue;
			if(buf[0] == ';' || buf[0] == '\r' || buf[0] == '\n')
				continue;

			name = strtok(buf, ";\r\n");
			if(strlen(name) > 0)
			{
				if(stricmp(name, "[Ally]") == 0)
					state = 1;
				else if(stricmp(name, "[Enemy]") == 0)
					state = 2;
				else if(state == 1)
					SendMessage(hAllyList, LB_INSERTSTRING, -1, (LPARAM)name);
				else if(state == 2)
					SendMessage(hEnemyList, LB_INSERTSTRING, -1, (LPARAM)name);
			}
		}
		fclose(file);
		UpdateTeamList();
	}
	else
	{
		GetError(GetLastError(), "OpenTeamList()");
	}
}

void OnSaveFriendList(HWND hWnd, HINSTANCE hInstance)
{
	char FileName[MAX_PATH];
	GetModuleFileName(hMod, FileName, sizeof(FileName));
	if(strrchr(FileName, '\\') != NULL)
		*strrchr(FileName, '\\') = 0;
	strcat_s(FileName, MAX_PATH, "\\Teamlist.txt");

	FILE *file;
	file = fopen(FileName, "r");
	if(file != 0)
	{
		fclose(file);
		int ret = MessageBox(hWnd, "Are you sure you want to overwrite Teamlist.txt?", PROGRAMNAME, MB_YESNO|MB_ICONEXCLAMATION);
		if(ret != IDYES)
			return;
	}
	file = fopen(FileName, "w+");
	if(file != 0)
	{
		char buf[128];
		int ret;
		fprintf(file, "[Ally]\r\n", buf);
		ret = SendMessage(hAllyList, LB_GETCOUNT, (WPARAM)0, (LPARAM)0);
		for(int i = 0; i < ret; i++)
		{
			SendMessage(hAllyList, LB_GETTEXT, (WPARAM)i, (LPARAM)&buf);
			fprintf(file, "%s;\r\n", buf);
		}
		fprintf(file, "[Enemy]\r\n", buf);
		ret = SendMessage(hEnemyList, LB_GETCOUNT, (WPARAM)0, (LPARAM)0);
		for(int i = 0; i < ret; i++)
		{
			SendMessage(hEnemyList, LB_GETTEXT, (WPARAM)i, (LPARAM)&buf);
			fprintf(file, "%s;\r\n", buf);
		}
		fclose(file);
	}
	else
	{
		GetError(GetLastError(), "SaveTeamList()");
	}
}

void LoadSettings(void)
{
	char FileName[MAX_PATH];
	GetModuleFileName(hMod, FileName, sizeof(FileName));
	if(strrchr(FileName, '\\') != NULL)
		*strrchr(FileName, '\\') = 0;
	strcat_s(FileName, MAX_PATH, "\\Settings.ini");

	char buf[256];
	int temp;
	CIni ini(FileName);
	ini.ReadString("AutoHealer", "SpellHi", "Exura", buf, 256);
	SetWindowText(hSpellHi, buf);
	temp = ini.ReadInt("AutoHealer", "SpellHiHealth", 500);
	sprintf_s(buf, 256, "%d", temp);
	SetWindowText(hSpellHiHealth, buf);
	temp = ini.ReadInt("AutoHealer", "SpellHiMana", 25);
	sprintf_s(buf, 256, "%d", temp);
	SetWindowText(hSpellHiMana, buf);

	ini.ReadString("AutoHealer", "SpellLo", "Exura Vita", buf, 256);
	SetWindowText(hSpellLo, buf);
	temp = ini.ReadInt("AutoHealer", "SpellLoHealth", 250);
	sprintf_s(buf, 256, "%d", temp);
	SetWindowText(hSpellLoHealth, buf);
	temp = ini.ReadInt("AutoHealer", "SpellLoMana", 160);
	sprintf_s(buf, 256, "%d", temp);
	SetWindowText(hSpellLoMana, buf);

	temp = ini.ReadInt("AutoHealer", "HealItem", 0);
	SendMessage(hHealItem, LB_SETCURSEL, temp, 0);
	temp = ini.ReadInt("AutoHealer", "HealItemHealth", 250);
	sprintf_s(buf, 256, "%d", temp);
	SetWindowText(hHealItemHealth, buf);
	temp = ini.ReadInt("AutoHealer", "Delay", 100);
	sprintf_s(buf, 256, "%d", temp);
	SetWindowText(hAutoHealDelay, buf);


	temp = ini.ReadInt("AimbotCore", "AllyHealth", 80);
	sprintf_s(buf, 256, "%d", temp);
	SetWindowText(hAllyHealHealth, buf);

	temp = ini.ReadInt("Extra", "TrackMagicwalls", 128);
	sprintf_s(buf, 256, "%d", temp);
	SetWindowText(hTrackMagwalls, buf);
}

void SaveSettings(void)
{
	char FileName[MAX_PATH];
	GetModuleFileName(hMod, FileName, sizeof(FileName));
	if(strrchr(FileName, '\\') != NULL)
		*strrchr(FileName, '\\') = 0;
	strcat_s(FileName, MAX_PATH, "\\Settings.ini");

	FILE *file;
	file = fopen(FileName, "r");
	if(file != 0)
	{
		fclose(file);
		int ret = MessageBox(g_hWnd, "Are you sure you want to overwrite Settings.ini?", PROGRAMNAME, MB_YESNO|MB_ICONEXCLAMATION);
		if(ret != IDYES)
			return;
	}

	char buf[256];
	CIni ini(FileName);
	GetWindowText(hSpellHi, buf, 256);
	ini.WriteString("AutoHealer", "SpellHi", buf);
	GetWindowText(hSpellHiHealth, buf, 256);
	ini.WriteInt("AutoHealer", "SpellHiHealth", atoi(buf), false);
	GetWindowText(hSpellHiMana, buf, 256);
	ini.WriteInt("AutoHealer", "SpellHiMana", atoi(buf), false);

	GetWindowText(hSpellLo, buf, 256);
	ini.WriteString("AutoHealer", "SpellLo", buf);
	GetWindowText(hSpellLoHealth, buf, 256);
	ini.WriteInt("AutoHealer", "SpellLoHealth", atoi(buf), false);
	GetWindowText(hSpellLoMana, buf, 256);
	ini.WriteInt("AutoHealer", "SpellLoMana", atoi(buf), false);

	int ret = SendMessage(hHealItem, LB_GETCURSEL, 0, 0);
	if(ret < 0)
		ret = 0;
	ini.WriteInt("AutoHealer", "HealItem", ret, false);
	GetWindowText(hHealItemHealth, buf, 256);
	ini.WriteInt("AutoHealer", "HealItemHealth", atoi(buf), false);
	GetWindowText(hAutoHealDelay, buf, 256);
	ini.WriteInt("AutoHealer", "Delay", atoi(buf), false);


	// AimbotCore
	GetWindowText(hAllyHealHealth, buf, 256);
	ini.WriteInt("AimbotCore", "AllyHealth", atoi(buf), false);

	// Extra
	GetWindowText(hTrackMagwalls, buf, 256);
	ini.WriteInt("Extra", "TrackMagicwalls", atoi(buf), false);
}