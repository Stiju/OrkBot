#include <windows.h>
#include <cstdio>
#include "resource1.h"
#include "Hook.h"
#include "LootList.h"

HWND hLootList;

void AddItem(int ItemID, int Backpack)
{
	char buf[128];
	sprintf_s(buf, 128, "%d %d", ItemID, Backpack);
	SendMessage(hLootList, LB_INSERTSTRING, -1, (LPARAM)buf);
}

void LoadLootList(void)
{
	char FileName[MAX_PATH];
	GetModuleFileName(hMod, FileName, sizeof(FileName));
	if(strrchr(FileName, '\\') != NULL)
		*strrchr(FileName, '\\') = 0;
	strcat_s(FileName, MAX_PATH, "\\Lootlist.txt");

	FILE *file;
	file = fopen(FileName, "r+");
	if(file != 0)
	{
		SendMessage(hLootList, LB_RESETCONTENT, (WPARAM)0, (LPARAM)0);

		char buf[128], *token;
		while(!feof(file))
		{
			*buf = 0;
			fgets(buf, 128, file);
			if((int)strlen(buf) == 0)
				continue;

			token = strtok(buf, "\r\n\0");

			SendMessage(hLootList, LB_INSERTSTRING, -1, (LPARAM)token);
		}
		fclose(file);
	}
	else
	{
		GetError(GetLastError(), "LoadLootList()");
	}
}

void SaveLootList(void)
{
	char FileName[MAX_PATH];
	GetModuleFileName(hMod, FileName, sizeof(FileName));
	if(strrchr(FileName, '\\') != NULL)
		*strrchr(FileName, '\\') = 0;
	strcat_s(FileName, MAX_PATH, "\\Lootlist.txt");

	FILE *file;
	file = fopen(FileName, "r");
	if(file != 0)
	{
		fclose(file);
		int ret = MessageBox(g_hWnd, "Are you sure you want to overwrite Lootlist.txt?", PROGRAMNAME, MB_YESNO|MB_ICONEXCLAMATION);
		if(ret != IDYES)
			return;
	}

	int count = (int)SendMessage(hLootList, LB_GETCOUNT, 0, 0);
	if(count != -1)
	{
		FILE *file;
		file = fopen(FileName, "w+");
		if(!file)
		{
			GetError(GetLastError(), "SaveLootList()");
			return;
		}
		char buf[128];
		for(int i = 0; i < count; i++)
		{
			SendMessage(hLootList, LB_GETTEXT, (WPARAM)i, (LPARAM)&buf);
			fprintf(file, "%s\n", buf);
		}
		fclose(file);
	}
}

void AcceptLootList(void)
{
	int count = (int)SendMessage(hLootList, LB_GETCOUNT, 0, 0);
	if(count != -1)
	{
		LootItemsArray = 0;
		delete [] LootItems;
		LootItems = new SLootItem[count+1];
		char buf[128], *token;
		for(int i = 0; i < count; i++)
		{
			SendMessage(hLootList, LB_GETTEXT, (WPARAM)i, (LPARAM)&buf);
			token = strtok(buf, " ");
			LootItems[i].ItemID = atoi(token);
			token = strtok(0, " ");
			LootItems[i].LootBP = atoi(token);
		}
		LootItemsArray = count;
	}
}

void InitLootList(HWND hWnd)
{
	char buf[128];

	hLootList = GetDlgItem(hWnd, IDC_LOOT_LIST);

	for(int i = 0; i < LootItemsArray; i++)
		AddItem(LootItems[i].ItemID, LootItems[i].LootBP);

	HWND tmp = GetDlgItem(hWnd, IDC_LOOT_BACKPACK);
	for(int i = 0; i < 16; i++)
	{
		sprintf(buf, "Backpack %d", i+1);
		SendMessage(tmp, CB_ADDSTRING, 0, (LPARAM)buf);
	}
	SendMessage(tmp, CB_SETCURSEL, 0, 0);
}

LRESULT CALLBACK ItemBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
				return 0;
			}
		}
		break;
	}
	return CallWindowProc((WNDPROC)lpfnWndProc, hWnd, message, wParam, lParam);
}

int CALLBACK LootListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message)
	{
	case WM_INITDIALOG:
		InitLootList(hWnd);
		SetWindowLong(hLootList, GWLP_WNDPROC, (LONG)ItemBoxProc);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDC_LOOT_ADD:
			{
				char buf[32];
				GetWindowText(GetDlgItem(hWnd, IDC_LOOT_ITEM), buf, 32);
				int bp = (int)SendMessage(GetDlgItem(hWnd, IDC_LOOT_BACKPACK), CB_GETCURSEL, 0, 0);
				if(bp != -1)
				{
					AddItem(atoi(buf), bp);
				}
			}break;
		//case IDC_LOOT_DEL:
		//	{
		//		int i = (int)SendMessage(hLootList, LB_GETCURSEL, 0, 0);
		//		if(i != LB_ERR)
		//		{
		//			SendMessage(hLootList, LB_DELETESTRING, (WPARAM)i, (LPARAM)0);
		//		}
		//	}break;
		case ID_LOOT_LOAD:
			{
				LoadLootList();
			}break;
		case ID_LOOT_SAVE:
			{
				SaveLootList();
			}break;
		case IDOK:
			AcceptLootList();
			EndDialog(hWnd, 0);
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