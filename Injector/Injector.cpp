#include <windows.h>
#include <conio.h>
#include <cstdio>
#include <commctrl.h>
#include "..\\Hook\\Constants.h"
#include "..\\Hook\\Tibia.h"
#include "Win.h"
#include "resource.h"

#pragma comment(lib, "comctl32.lib")

#define PROGRAMNAME			"OrkBot Injector\0"
#define PROGRAMCLASSNAME	"ORKBOTINJECTOR\0"
#define WIN_WIDTH			285
#define WIN_HEIGHT			230
#define IDC_LIST			1001
#define IDC_REFRESH			1002
#define IDC_ACCEPT			1003

// Structures
struct SClient
{
	char name[32];
	DWORD dwProcessId;
};

// Global variables
static HINSTANCE g_hInstance;
char DLL[MAX_PATH] = "hook.dll";
static HWND hWndList;
SClient clients[32];

bool InjectLibrary(DWORD dwProcessId, char* strDll)
{
	bool bInject = false;
	bool bSuccess; 
	int ret;
	FILE *file;
	file = fopen("Logfile.txt", "w");
	fprintf(file, "== InjectLibrary ==\n");
	fprintf(file, "Call OpenProcess() ");
	HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, false, dwProcessId);
	fprintf(file, "returned [%d]\n", hProcess);
	HANDLE hThread;
	if(hProcess != 0)
	{
		fprintf(file, "Call VirtualAllocEx() ");
		LPVOID lpRemoteAddress = VirtualAllocEx(hProcess, 0, strlen(DLL), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		fprintf(file, "returned [%d]\n", lpRemoteAddress);
		if(lpRemoteAddress != 0)
		{
			fprintf(file, "Call WriteProcessMemory() ");
			ret = WriteProcessMemory(hProcess, lpRemoteAddress, (LPVOID)strDll, strlen(DLL), 0);
			fprintf(file, "returned [%d]\n", ret);
			fprintf(file, "Call CreateRemoteThread() ");
			hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle("Kernel32"), "LoadLibraryA"), lpRemoteAddress, 0, 0);
			fprintf(file, "returned [%d]\n", hThread);
			if(hThread != 0)
			{
				fprintf(file, "Call WaitForSignleObject() ");
				ret = WaitForSingleObject(hThread, INFINITE);
				fprintf(file, "retruned [%d]\n", ret);
				bInject = true;
			}
			else
			{
				fprintf(file, "[Error code: %d]\n", GetLastError());
			}
			fprintf(file, "Call VirtualFreeEx() ");
			bSuccess = VirtualFreeEx(hProcess, lpRemoteAddress, 0, MEM_RELEASE);
			fprintf(file, "returned [%s]\n", ((bSuccess == true)?"TRUE":"FALSE"));
			if(bSuccess == false)
				fprintf(file, "[Error code: %d]\n", GetLastError());
		}
		else
		{
			fprintf(file, "[Error code: %d]\n", GetLastError());
		}
	}
	else
	{
		fprintf(file, "[Error code: %d]\n", GetLastError());
	}
	fclose(file);
	return bInject;
}

bool CheckDll(void)
{
	if(GetModuleFileName(0, DLL, sizeof(DLL) - 32) != 0)
	{
		if(strrchr(DLL, '\\') != NULL)
			*strrchr(DLL, '\\') = 0;
		else 
			strcpy(DLL, ".");
		strcat_s(DLL, MAX_PATH, "\\Hook.dll");

		FILE *file;
		file = fopen(DLL, "r");
		if(file)
		{
			fclose(file);
			return true;
		}
	}
	return false;
}

void GetPlayerName(HANDLE hProcess, char *lpName)
{
	char name[32];
	int playerID;
	ReadProcessMemory(hProcess, (LPVOID)Address::PLAYER_ID, &playerID, 4, 0);
	for(int i = Address::BATTLELIST_BEGIN; i < Address::BATTLELIST_END; i += sizeof(SCreature))
	{
		int cID;
		ReadProcessMemory(hProcess, (LPVOID)i, &cID, 4, 0);
		if(playerID == cID)
		{
			ReadProcessMemory(hProcess, (LPVOID)(i+4), lpName, 32, 0);
			return;
		}
	}
}

void RefreshList(void)
{
	HWND temp = 0;
	HANDLE hProcess;
	int x = 0;

	char buffer[128];
	SendMessage(hWndList, LB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	do{
		temp = FindWindowEx(0, temp, "TibiaClient", 0);
		if(!temp)
			break;
		else
		{
			GetWindowThreadProcessId(temp, &clients[x].dwProcessId);
			hProcess = OpenProcess(PROCESS_VM_READ, false, clients[x].dwProcessId);
			int isOnline;
			ReadProcessMemory(hProcess, (LPVOID)Address::ISONLINE, &isOnline, 4, 0);
			if(isOnline == 8)
			{
				GetPlayerName(hProcess, clients[x].name);
				sprintf_s(buffer, 256, "[0x%.4X] %s", clients[x].dwProcessId, clients[x].name);
			}
			else
			{
				sprintf_s(clients[x].name, 32, "Unknown");
				sprintf_s(buffer, 256, "[0x%.4X] Unknown", clients[x].dwProcessId);
			}
			SendMessage(hWndList, LB_INSERTSTRING, (WPARAM)(-1), (LPARAM)buffer);
			CloseHandle(hProcess);
			x++;
		}
	}while(true);
	SendMessage(hWndList, LB_SETCURSEL, 0, 0);
}

void InitializeWindow(HWND hWnd, HINSTANCE hInstance)
{
	CWin w(hWnd, hInstance);
	w.SetFont(8, "MS Sans Serif");
	hWndList = w.CreateListBox(0, 10, 10, 260, 155, IDC_LIST);
	w.CreateButton("Refresh List", 10, 165, 75, 23, IDC_REFRESH);
	w.CreateButton("Ok", 195, 165, 75, 23, IDC_ACCEPT);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	
	switch (message)
	{
	case WM_CREATE:
		{
			HFONT hFont = CreateFont(8,0,0,0,FW_DONTCARE,0,0,0,
				DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");
			HWND hTemp;
			hWndList = CreateWindowEx(WS_EX_STATICEDGE, "listbox", "", WS_VISIBLE | WS_CHILD | LBS_STANDARD | LBS_DISABLENOSCROLL, 10, 10, 260, 155, hWnd, (HMENU)IDC_LIST, g_hInstance, 0);
			SendMessage(hWndList, WM_SETFONT, (WPARAM)hFont, true);
			hTemp = CreateWindowEx(0, "button", "Refresh List", WS_CHILD | WS_VISIBLE, 10, 165, 75, 23, hWnd, (HMENU)IDC_REFRESH, g_hInstance, 0);
			SendMessage(hTemp, WM_SETFONT, (WPARAM)hFont, true);
			hTemp = CreateWindowEx(0, "button", "Ok", WS_CHILD | WS_VISIBLE, 195, 165, 75, 23, hWnd, (HMENU)IDC_ACCEPT, g_hInstance, 0);
			SendMessage(hTemp, WM_SETFONT, (WPARAM)hFont, true);
			RefreshList();
			if(CheckDll() == false)
			{
				char buffer[256];
				sprintf_s(buffer, 256, "Could not find file: %s", DLL);
				MessageBox(hWnd, buffer, PROGRAMNAME, MB_OK+MB_ICONHAND);
				DestroyWindow(hWnd);
				return 0;
			}
		}break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDC_LIST:
			{
				if(wmEvent != LBN_DBLCLK)
					break;
			}
		case IDC_ACCEPT:
			{
				int i = (int)SendMessage(hWndList, LB_GETCURSEL, 0, 0);
				if(i != LB_ERR)
				{
					if(InjectLibrary(clients[i].dwProcessId, DLL) == false)
					{
						char buffer[256];
						sprintf_s(buffer, 256, "Could not inject: %s", DLL);
						MessageBox(hWnd, buffer, PROGRAMNAME, MB_OK+MB_ICONHAND);
					}
					else
					{
						DestroyWindow(hWnd);
					}
				}
				else
				{
					MessageBox(hWnd, "Select a client first", PROGRAMNAME, MB_OK+MB_ICONEXCLAMATION);
				}
			}break;
		case IDC_REFRESH:
			{
				RefreshList();
			}break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	InitCommonControls();
	WNDCLASSEX wcex;
	g_hInstance = hInstance;

	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(16);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= PROGRAMCLASSNAME;
	wcex.hIconSm		= 0;

	RegisterClassEx(&wcex);

	int x = (GetSystemMetrics(SM_CXSCREEN) - WIN_WIDTH) / 2;
	int y = (GetSystemMetrics(SM_CYSCREEN) - WIN_HEIGHT) / 2;
	HWND hWnd = CreateWindowExA(0, PROGRAMCLASSNAME, PROGRAMNAME, WS_OVERLAPPED|
		WS_CAPTION|WS_SYSMENU, x, y, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, g_hInstance, NULL);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MSG msg;

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}