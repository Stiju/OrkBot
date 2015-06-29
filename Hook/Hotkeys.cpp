#include <windows.h>
#include <commctrl.h>
#include <cstdio>
#include "Tibia.h"
#include "Hook.h"
#include "Toggler.h"
#include "Resource1.h"
#include "Ini.h"

#include "Packet.h"
#include "Hotkeys.h"

//unsigned short Hotkey_LevelSpyUp = 0x006B;
//unsigned short Hotkey_LevelSpyDown = 0x006D;
//unsigned short Hotkey_ShootMagwall = 0x0060;

SHotkey Hotkeys[11];
bool isLooting = false;

void SetHotkey(HWND hWnd, int nIDDlgItem, unsigned short nHotkey)
{
	HWND temp = GetDlgItem(hWnd, nIDDlgItem);
	SendMessage(temp, HKM_SETHOTKEY, nHotkey, 0);
}

unsigned short GetHotkey(HWND hWnd, int nIDDlgItem)
{
	HWND temp = GetDlgItem(hWnd, nIDDlgItem);
	return (unsigned short)SendMessage(temp, HKM_GETHOTKEY, 0, 0);
}

void InitHotkeyDialog(HWND hWnd)
{
	for(int i = 0; i < 11; i++)
	{
		HWND tempcheck = GetDlgItem(hWnd, IDC_HOTKEY_CHECK1+i);
		HWND tempedit = GetDlgItem(hWnd, IDC_HOTKEY_EDIT1+i);
		SetHotkey(hWnd, IDC_HOTKEY_1+i, Hotkeys[i].Key);
		SendMessage(tempcheck, BM_SETCHECK, Hotkeys[i].Enabled, 0);
		SetWindowText(tempedit, Hotkeys[i].Command);
		if(Hotkeys[i].Enabled == true && CheckCommand(&Hotkeys[i]) == false)
		{
			SendMessage(tempcheck, BM_SETCHECK, false, 0);
			Hotkeys[i].Enabled = false;
		}
	}
}

void ApplyNewHotkeys(HWND hWnd)
{
	for(int i = 0; i < 11; i++)
	{
		HWND tempcheck = GetDlgItem(hWnd, IDC_HOTKEY_CHECK1+i);
		HWND tempedit = GetDlgItem(hWnd, IDC_HOTKEY_EDIT1+i);
		Hotkeys[i].Key = GetHotkey(hWnd, IDC_HOTKEY_1+i);
		Hotkeys[i].Enabled = (SendMessage(tempcheck, BM_GETCHECK, 0, 0) == BST_CHECKED)?true:false;
		GetWindowText(tempedit, Hotkeys[i].Command, 128);
		if(Hotkeys[i].Enabled == true && CheckCommand(&Hotkeys[i]) == false)
		{
			SendMessage(tempcheck, BM_SETCHECK, false, 0);
			Hotkeys[i].Enabled = false;
		}
	}
}

void LoadHotkeys(void)
{
	char FileName[MAX_PATH];
	GetModuleFileName(hMod, FileName, sizeof(FileName));
	if(strrchr(FileName, '\\') != NULL)
		*strrchr(FileName, '\\') = 0;
	strcat_s(FileName, MAX_PATH, "\\Hotkeys.ini");
	
	FILE *file;
	file = fopen(FileName, "r");
	if(file)
		fclose(file);
	else
		SaveHotkeys();

	CIni ini(FileName);
	char buf[32];
	for(int i = 0; i < 11; i++)
	{
		sprintf_s(buf, 32, "Hotkey%.2d", i);
		Hotkeys[i].Key = ini.ReadInt("Hotkeys", buf, 0);
		//sprintf_s(buf, 32, "Hotkey%.2d", i);
		Hotkeys[i].Enabled = (bool)ini.ReadInt("HotkeysEnabled", buf, 0);
		//sprintf_s(buf, 32, "Hotkey%.2d", i);
		ini.ReadString("HotkeysCommand", buf, 0, Hotkeys[i].Command, 128);
		if(Hotkeys[i].Enabled == true && CheckCommand(&Hotkeys[i]) == false)
		{
			Hotkeys[i].Enabled = false;
		}
	}
}

void SaveHotkeys(void)
{
	char FileName[MAX_PATH];
	GetModuleFileName(hMod, FileName, sizeof(FileName));
	if(strrchr(FileName, '\\') != NULL)
		*strrchr(FileName, '\\') = 0;
	strcat_s(FileName, MAX_PATH, "\\Hotkeys.ini");
	
	FILE *file;
	file = fopen(FileName, "r");
	if(file != 0)
	{
		fclose(file);
		int ret = MessageBox(g_hWnd, "Are you sure you want to overwrite Hotkeys.ini?", PROGRAMNAME, MB_YESNO|MB_ICONEXCLAMATION);
		if(ret != IDYES)
			return;
	}

	CIni ini(FileName);
	char buf[32];
	for(int i = 0; i < 11; i++)
	{
		sprintf_s(buf, 32, "Hotkey%.2d", i);
		ini.WriteInt("Hotkeys", buf, Hotkeys[i].Key, true);
		//sprintf_s(buf, 32, "Hotkey%.2d", i);
		ini.WriteInt("HotkeysEnabled", buf, (int)Hotkeys[i].Enabled, false);
		//sprintf_s(buf, 32, "Hotkey%.2d", i);
		ini.WriteString("HotkeysCommand", buf, Hotkeys[i].Command);
	}
}

bool CheckCommand(SHotkey *hk)
{
	char *token;
	bool ret = false;
	char buf[128];
	sprintf_s(buf, 128, "%s", hk->Command);
	token = strtok(buf, " ");
	while(token != 0)
	{
		if(stricmp(token, "levelspyup") == 0)
		{
			hk->Type = HotkeyCommand::LevelSpyUp;
			ret = true;
			break;
		}
		else if(stricmp(token, "levelspydown") == 0)
		{
			hk->Type = HotkeyCommand::LevelSpyDown;
			ret = true;
			break;
		}
		else if(stricmp(token, "magwall") == 0)
		{
			hk->Type = HotkeyCommand::Magwall;
			token = strtok(0, " ");
			if(token == 0)
				break;
			if(stricmp(token, "self") == 0)
				hk->Arg1 = HotkeyCreature::Self;
			else if(stricmp(token, "target") == 0)
				hk->Arg1 = HotkeyCreature::Target;
			else if(stricmp(token, "friend") == 0)
				hk->Arg1 = HotkeyCreature::Friend;
			else if(stricmp(token, "enemy") == 0)
				hk->Arg1 = HotkeyCreature::Enemy;
			else
				break;

			token = strtok(0, " ");
			if(token != 0)
			{
				try{
					hk->Arg2 = atoi(token);
				}catch(char *ex){
					break;
				}
				if(hk->Arg2 == 0)
					break;
			}
			else
				hk->Arg2 = 1;
			ret = true;
			break;
		}
		else if(stricmp(token, "mana") == 0)
		{
			hk->Type = HotkeyCommand::Mana;
			token = strtok(0, " ");
			if(token == 0)
				break;
			if(stricmp(token, "normal") == 0)
				hk->Arg1 = Items::ManaPotion;
			else if(stricmp(token, "strong") == 0)
				hk->Arg1 = Items::StrongManaPotion;
			else if(stricmp(token, "great") == 0)
				hk->Arg1 = Items::GreatManaPotion;
			else if(stricmp(token, "spirit") == 0)
				hk->Arg1 = Items::GreatSpiritPotion;
			else
				break;
			ret = true;
			break;
		}
		else if(stricmp(token, "heal") == 0)
		{
			hk->Type = HotkeyCommand::Heal;
			token = strtok(0, " ");
			if(token == 0)
				break;
			if(stricmp(token, "uh") == 0)
				hk->Arg1 = Items::UltimateHealingRune;
			else if(stricmp(token, "ih") == 0)
				hk->Arg1 = Items::HealthPotion;
			else if(stricmp(token, "normal") == 0)
				hk->Arg1 = Items::HealthPotion;
			else if(stricmp(token, "strong") == 0)
				hk->Arg1 = Items::StrongHealthPotion;
			else if(stricmp(token, "great") == 0)
				hk->Arg1 = Items::GreatHealthPotion;
			else if(stricmp(token, "ultimate") == 0)
				hk->Arg1 = Items::UltimateHealthPotion;
			else if(stricmp(token, "spirit") == 0)
				hk->Arg1 = Items::GreatSpiritPotion;
			else
				break;
			token = strtok(0, " ");
			if(token == 0)
				break;
			if(stricmp(token, "self") == 0)
				hk->Arg2 = HotkeyCreature::Self;
			else if(stricmp(token, "target") == 0)
				hk->Arg2 = HotkeyCreature::Target;
			else if(stricmp(token, "friend") == 0)
				hk->Arg2 = HotkeyCreature::Friend;
			else if(stricmp(token, "enemy") == 0)
				hk->Arg2 = HotkeyCreature::Enemy;
			else
				break;
			ret = true;
			break;
		}
		else if(stricmp(token, "say") == 0)
		{
			hk->Type = HotkeyCommand::Say;
			memcpy(&hk->Say, &buf[4], 128);
			ret = true;
			break;
		}
		else if(stricmp(token, "loot") == 0)
		{
			hk->Type = HotkeyCommand::Loot;
			ret = true;
			break;
		}
		else if(stricmp(token, "equip") == 0)
		{
			hk->Type = HotkeyCommand::Equip;
			token = strtok(0, " ");
			if(token == 0)
				break;
			if(stricmp(token, "head") == 0)
				hk->Arg1 = EquipSlot::Head;
			else if(stricmp(token, "amulet") == 0)
				hk->Arg1 = EquipSlot::Neck;
			else if(stricmp(token, "container") == 0)
				hk->Arg1 = EquipSlot::Backpack;
			else if(stricmp(token, "body") == 0)
				hk->Arg1 = EquipSlot::Body;
			else if(stricmp(token, "lefthand") == 0)
				hk->Arg1 = EquipSlot::LeftHand;
			else if(stricmp(token, "righthand") == 0)
				hk->Arg1 = EquipSlot::RightHand;
			else if(stricmp(token, "legs") == 0)
				hk->Arg1 = EquipSlot::Legs;
			else if(stricmp(token, "feet") == 0)
				hk->Arg1 = EquipSlot::Feet;
			else if(stricmp(token, "ring") == 0)
				hk->Arg1 = EquipSlot::Ring;
			else if(stricmp(token, "ammo") == 0)
				hk->Arg1 = EquipSlot::Ammo;
			else
				break;
			token = strtok(0, " ");
			if(token == 0)
				break;
			try{
				hk->Arg2 = atoi(token);
			}catch(char *ex){
				break;
			}
			if(hk->Arg2 == 0)
				break;
			ret = true;
			break;
		}
		else if(stricmp(token, "shoot") == 0)
		{
			hk->Type = HotkeyCommand::Shoot;
			token = strtok(0, " ");
			if(token == 0)
				break;
			if(stricmp(token, "sd") == 0)
				hk->Arg1 = Items::SuddenDeathRune;
			else if(stricmp(token, "hmm") == 0)
				hk->Arg1 = Items::HeavyMagicMissleRune;
			else if(stricmp(token, "explo") == 0)
				hk->Arg1 = Items::ExplosionRune;
			else
			{
				try{
					hk->Arg1 = atoi(token);
				}catch(char *ex){
					break;
				}
				if(hk->Arg1 == 0)
					break;
			}
			token = strtok(0, " ");
			if(token == 0)
				break;
			if(stricmp(token, "self") == 0)
				hk->Arg2 = HotkeyCreature::Self;
			else if(stricmp(token, "target") == 0)
				hk->Arg2 = HotkeyCreature::Target;
			else if(stricmp(token, "friend") == 0)
				hk->Arg2 = HotkeyCreature::Friend;
			else if(stricmp(token, "enemy") == 0)
				hk->Arg2 = HotkeyCreature::Enemy;
			else
				break;
			ret = true;
			break;
		}
		else if(stricmp(token, "attack") == 0)
		{
			hk->Type = HotkeyCommand::Attack;
			token = strtok(0, " ");
			if(token == 0)
				break;
			if(stricmp(token, "self") == 0)
				hk->Arg1 = HotkeyCreature::Self;
			else if(stricmp(token, "target") == 0)
				hk->Arg1 = HotkeyCreature::Target;
			else if(stricmp(token, "friend") == 0)
				hk->Arg1 = HotkeyCreature::Friend;
			else if(stricmp(token, "enemy") == 0)
				hk->Arg1 = HotkeyCreature::Enemy;
			else
				break;
			ret = true;
			break;
		}
		token = strtok(0, " ");
	}

	return ret;
}

void ExecuteCommand(SHotkey *hk)
{
	switch(hk->Type)
	{
	case HotkeyCommand::LevelSpyUp:
		CTibia::LevelSpy(true);
		break;
	case HotkeyCommand::LevelSpyDown:
		CTibia::LevelSpy(false);
		break;
	case HotkeyCommand::Magwall:
		CTibia::ShootMagwall(hk->Arg1, hk->Arg2);
		break;
	case HotkeyCommand::Mana:
		CTibia::DrinkManaPotion(hk->Arg1);
		break;
	case HotkeyCommand::Heal:
		CTibia::HotkeyShoot(hk->Arg1, hk->Arg2, 95);
		break;
	case HotkeyCommand::Say:
		CPacket::SayMessage(hk->Say);
		break;
	case HotkeyCommand::Loot:
		HANDLE hLootThread;
		DWORD dwLootThreadId;
		hLootThread = CreateThread(0, 0, LootThread, 0, 0, &dwLootThreadId);
		break;
	case HotkeyCommand::Equip:
		CTibia::EquipItem(hk->Arg2, hk->Arg1);
		break;
	case HotkeyCommand::Shoot:
		CTibia::HotkeyShoot(hk->Arg1, hk->Arg2, 100);
		break;
	case HotkeyCommand::Attack:
		CTibia::AttackTarget(hk->Arg1);
		//CPacket::Move(0);
		break;
	}
}

int CALLBACK HotkeysProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_INITDIALOG:
		InitHotkeyDialog(hWnd);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDOK:
			ApplyNewHotkeys(hWnd);
			//EndDialog(hWnd, 0);
			break;
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


DWORD WINAPI LootThread(LPVOID lpParam)
{
	if(isLooting == false)
	{
		isLooting = true;
		CTibia::Loot();
		isLooting = false;
	}
	return 0;
}