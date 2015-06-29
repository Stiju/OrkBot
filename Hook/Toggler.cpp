#include <windows.h>
#include <cstdio>
#include "Constants.h"
#include "Hook.h"
#include "Resource1.h"
#include "Tibia.h"
#include "Toggler.h"


void ShowHide(HWND hWnd)
{
	if(!IsWindowVisible(hWnd))
	{
		ShowWindow(hWnd, SW_SHOW);
	}
	else
	{
		ShowWindow(hWnd, SW_HIDE);
	}
}

void Minimize(void)
{
	if(bMinimized == false)
	{
		SetWindowPos(g_hWnd, 0, 0, 0, 200, 43, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER);
		HMENU hMenu = GetMenu(g_hWnd);
		ModifyMenu(hMenu, ID_FILE_MINIMIZE, MF_STRING, ID_FILE_MINIMIZE, "Maximize");
		bMinimized = true;
	}
	else
	{
		SetWindowPos(g_hWnd, 0, 0, 0, WIN_WIDTH, WIN_HEIGHT, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOZORDER);
		HMENU hMenu = GetMenu(g_hWnd);
		ModifyMenu(hMenu, ID_FILE_MINIMIZE, MF_STRING, ID_FILE_MINIMIZE, "Minimize");
		bMinimized = false;
	}
}

void ToggleHealer(void)
{
	if(SendMessage(hUseSpellHi, BM_GETCHECK, 0, 0) == BST_CHECKED ||
		SendMessage(hUseSpellLo, BM_GETCHECK, 0, 0) == BST_CHECKED ||
		SendMessage(hUseHealItem, BM_GETCHECK, 0, 0) == BST_CHECKED ||
		SendMessage(hAntiParalyze, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		char buf[200];
		GetWindowText(hSpellHi, SpellHi.Spell, 200);
		GetWindowText(hSpellHiHealth, buf, 200);
		SpellHi.Health = atoi(buf);
		GetWindowText(hSpellHiMana, buf, 200);
		SpellHi.Mana = atoi(buf);

		GetWindowText(hSpellLo, SpellLo.Spell, 200);
		GetWindowText(hSpellLoHealth, buf, 200);
		SpellLo.Health = atoi(buf);
		GetWindowText(hSpellLoMana, buf, 200);
		SpellLo.Mana = atoi(buf);

		GetWindowText(hHealItemHealth, buf, 200);
		HealItem.Health = atoi(buf);

		int ret = SendMessage(hHealItem, LB_GETCURSEL, 0, 0);
		HealItem.ItemID = HealingItems[ret].ID;

		SpellHi.Use = (SendMessage(hUseSpellHi, BM_GETCHECK, 0, 0) == BST_CHECKED)?true:false;
		SpellLo.Use = (SendMessage(hUseSpellLo, BM_GETCHECK, 0, 0) == BST_CHECKED)?true:false;
		HealItem.Use = (SendMessage(hUseHealItem, BM_GETCHECK, 0, 0) == BST_CHECKED)?true:false;
		bAntiParalyze = (SendMessage(hAntiParalyze, BM_GETCHECK, 0, 0) == BST_CHECKED)?true:false;

		EnableWindow(hSpellHi,			false);
		EnableWindow(hSpellHiHealth,	false);
		EnableWindow(hSpellHiMana,		false);
		EnableWindow(hSpellLo,			false);
		EnableWindow(hSpellLoHealth,	false);
		EnableWindow(hSpellLoMana,		false);
		EnableWindow(hHealItem,			false);
		EnableWindow(hHealItemHealth,	false);
		bAutoHealer = true;
		/* char buf2[512];
		sprintf(buf2, "SpellHi.Spell = %s; SpellHi.Health = %d; SpellHi.Mana = %d; SpellHi.Use = %s;\n",
			SpellHi.Spell, SpellHi.Health, SpellHi.Mana, SpellHi.Use?"True":"False");
		MessageBox(g_hWnd, buf2, "AutoHealer", 0);
		sprintf(buf2, "SpellLo.Spell = %s; SpellLo.Health = %d; SpellLo.Mana = %d; SpellLo.Use = %s;\n",
			SpellLo.Spell, SpellLo.Health, SpellLo.Mana, SpellLo.Use?"True":"False");
		MessageBox(g_hWnd, buf2, "AutoHealer", 0);
		sprintf(buf2, "SpellHi.ItemID = %d; SpellHi.Health = %d; SpellHi.Mana = %d; SpellHi.Use = %s;\n",
			HealItem.ItemID, HealItem.Health, HealItem.Mana, HealItem.Use?"True":"False");
		MessageBox(g_hWnd, buf2, "AutoHealer", 0); */
	}
	else
	{
		EnableWindow(hSpellHi,			true);
		EnableWindow(hSpellHiHealth,	true);
		EnableWindow(hSpellHiMana,		true);
		EnableWindow(hSpellLo,			true);
		EnableWindow(hSpellLoHealth,	true);
		EnableWindow(hSpellLoMana,		true);
		EnableWindow(hHealItem,			true);
		EnableWindow(hHealItemHealth,	true);
		bAntiParalyze = false;
		bAutoHealer = false;
	}
}

void ToggleAllyHealer(void)
{
	if(SendMessage(hUseAllySpell, BM_GETCHECK, 0, 0) == BST_CHECKED ||
		SendMessage(hUseAllyItem, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		char buf[200];
		GetWindowText(hAllyHealHealth, buf, 200);
		AllySpell.Health = atoi(buf);
		AllySpell.Mana = 160;
		int ret = atoi(buf);
		if(ret >= 100)
		{
			AllyHealItem.Health = 100;
			SetWindowText(hAllyHealHealth, "100");
		}
		else
		{
			AllyHealItem.Health = ret;
		}
		AllyHealItem.ItemID = Items::UltimateHealingRune;

		AllySpell.Use = (SendMessage(hUseAllySpell, BM_GETCHECK, 0, 0) == BST_CHECKED)?true:false;
		AllyHealItem.Use = (SendMessage(hUseAllyItem, BM_GETCHECK, 0, 0) == BST_CHECKED)?true:false;
		EnableWindow(hAllyHealHealth,	false);
		bAutoAllyHealer = true;
	}
	else
	{
		EnableWindow(hAllyHealHealth,	true);
		bAutoAllyHealer = false;
	}
}

void ToggleTeamMarks(void)
{
	if(SendMessage(hTeamMarks, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		bShowTeamMarks = true;
	}
	else
	{
		bShowTeamMarks = false;
	}
}

void ToggleTeamColors(void)
{
	if(SendMessage(hTeamColors, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		bShowTeamColors = true;
	}
	else
	{
		bShowTeamColors = false;
	}
}

void ToggleShootFromClosedBp(void)
{
	if(SendMessage(hShootClosedBP, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		bShootFromClosedBp = true;
	}
	else
	{
		bShootFromClosedBp = false;
	}
}

void ToggleDoubleHealing(void)
{
	if(SendMessage(hDoubleHealing, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		bDoubleHeal = true;
	}
	else
	{
		bDoubleHeal = false;
	}
}

void ToggleMagwallTimer(void)
{
	if(SendMessage(hMagwallTimer, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		delete [] Magwalls;
		char buf[8];
		GetWindowText(hTrackMagwalls, buf, 8);
		MagwallArray = atoi(buf);
		Magwalls = new SMagWall[MagwallArray];
		ZeroMemory(Magwalls, sizeof(SMagWall)*MagwallArray);
		EnableWindow(hTrackMagwalls,	false);
		bMagwallTimer = true;
	}
	else
	{
		EnableWindow(hTrackMagwalls,	true);
		bMagwallTimer = false;
	}
}

void ToggleSpellTimers(void)
{
	if(SendMessage(hSpellTimers, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		bSpellTimers = true;
	}
	else
	{
		bSpellTimers = false;
	}
}

void ToggleXRay(void)
{
	if(SendMessage(hXRay, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		CTibia::XRay(true);
		bXRay = true;
	}
	else
	{
		CTibia::XRay(false);
		bXRay = false;
	}
}

void ToggleLeastHealth(void)
{
	if(SendMessage(hLeastHPP, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		bLeastHealth = true;
	}
	else
	{
		bLeastHealth = false;
	}
}

void ToggleShowTarget(void)
{
	if(SendMessage(hShowTarget, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		bShowTarget = true;
	}
	else
	{
		bShowTarget = false;
	}
}

void ToggleFullLight(void)
{
	if(SendMessage(hFullLight, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		bReAttack = true;
	}
	else
	{
		bReAttack = false;
	}
}

void ToggleShowItemID(void)
{
	if(SendMessage(hShowItemID, BM_GETCHECK, 0, 0) == BST_CHECKED)
	{
		bShowItemID = true;
	}
	else
	{
		bShowItemID = false;
	}
}

void UpdateTeamList(void)
{

	AllyNameListCount = 0;
	delete [] AllyNameList;
	int count = (int)SendMessage(hAllyList, LB_GETCOUNT, 0, 0);
	if(count != -1)
	{
		AllyNameList = new SNameList[count+1];
		for(int i = 0; i < count; i++)
		{
			SendMessage(hAllyList, LB_GETTEXT, (WPARAM)i, (LPARAM)&AllyNameList[i].Name);
		}
	}
	AllyNameListCount = count;
	EnemyNameListCount = 0;
	delete [] EnemyNameList;
	count = (int)SendMessage(hEnemyList, LB_GETCOUNT, 0, 0);
	if(count != -1)
	{
		EnemyNameList = new SNameList[count+1];
		for(int i = 0; i < count; i++)
		{
			SendMessage(hEnemyList, LB_GETTEXT, (WPARAM)i, (LPARAM)&EnemyNameList[i].Name);
		}
	}
	EnemyNameListCount = count;
}