#pragma once
#include "Constants.h"

#define PROGRAMNAME			"OrkBot\0"
#define PROGRAMCLASSNAME	"ORKBOT\0"

#define WIN_WIDTH			365
#define WIN_HEIGHT			435

#define WM_TRAY				1999
#define IDC_AUTOHEAL		2000
#define IDC_AUTOHEALALLY	2001
#define IDC_TEAMMARKS		2002
#define IDC_TEAMCOLORS		2003
#define IDC_SHOOTFROMBP		2004
#define IDC_DOUBLEHEALING	2005
#define IDC_MAGWALLTIMER	2006
#define IDC_SPELLTIMERS		2007
#define IDC_XRAY			2008
#define IDC_LEASTHPP		2009
#define IDC_SHOWTARGET		2010
#define IDC_FULLLIGHT		2011
#define IDC_SHOWITEMID		2012

extern NOTIFYICONDATA niData;
extern WNDPROC wndProc;

typedef void (__cdecl *_PrintText)(int nSurface, int nX, int nY, int nFont, int nRed, int nGreen, int nBlue, char* lpText, int nAlign);
typedef void (__cdecl *_PlaceObject)(int nX, int nY, int nZ, int nID, int nCount, int u1);
typedef void (__cdecl *_PlaceObject2)(int nX, int nY, int nZ, int u1, int nID, int nCount, int u2);
typedef void (__cdecl *_StatusMessage)(char *message, int arg);
typedef void (__cdecl *_MessageHandle)(unsigned int u1, char *lpMessage, unsigned int nType, char *lpName, unsigned int nLevel, unsigned int u2, unsigned int u3, unsigned int u4, unsigned int u5, unsigned int u6, unsigned int u7, unsigned int u8, unsigned int u9);
//typedef void (__cdecl *_MessageHandle)(int nType, char *lpName, int u1, char *lpMessage, int u2);
typedef void (__cdecl *_LookMessage)(int arg1, char *message);
//typedef int (WINAPI *_Recv)(SOCKET s, char* buf, int len, int flags);
//typedef int (WINAPI *_Send)(SOCKET s, char* buf, int len, int flags);

extern _PrintText PrintText;//(_PrintText)Address::CALL_PRINTTEXT;
extern _PlaceObject PlaceObject;//(_PlaceObject)Address::CALL_PLACEOBJECT;
extern _PlaceObject2 PlaceObject2;//(_PlaceObject2)Address::CALL_PLACEOBJECT2;
extern _StatusMessage StatusMessage;
extern _MessageHandle MessageHandle;//(_MessageHandle)Address::CALL_MESSAGEHANDLE;
extern _LookMessage LookMessage;//(_LookMessage)Address::CALL_LOOKMESSAGE;
//static _Recv Recv = 0;
//static _Recv Send = 0;
//
//extern DWORD ping;
//
//int WINAPI MyRecv(SOCKET s, char* buf, int len, int flags);
//int WINAPI MySend(SOCKET s, char* buf, int len, int flags);
void MyLookMessage(int arg1, char *message);
void MyMessageHandle(unsigned int u1, char *lpMessage, unsigned int nType, char *lpName, unsigned int nLevel, unsigned int u2, unsigned int u3, unsigned int u4, unsigned int u5, unsigned int u6, unsigned int u7, unsigned int u8, unsigned int u9);
//void MyMessageHandle(int nType, char *lpName, int u1, char *lpMessage, int u2);
void MyPlaceObject(int nX, int nY, int nZ, int nID, int nCount, int u1);
void MyPlaceObject2(int nX, int nY, int nZ, int u1, int nID, int nCount, int u2);
void MyPrintName(int nSurface, int nX, int nY, int nFont, int nRed, int nGreen, int nBlue, char* lpText, int nAlign);
void MyPrintFps(int nSurface, int nX, int nY, int nFont, int nRed, int nGreen, int nBlue, char* lpText, int nAlign);
void MyPrintDmg(int nSurface, int nX, int nY, int nFont, int nRed, int nGreen, int nBlue, char* lpText, int nAlign);
void MyPrintTextEx(int nSurface, int nX, int nY, int nFont, int nRed, int nGreen, int nBlue, char* lpText, int nAlign, int arg10, int arg11, int arg12, int arg13);

DWORD HookCall(DWORD dwAddress, DWORD dwFunction);
PBYTE HookApi(DWORD dwAddress, DWORD dwFunction);
void UnhookCall(DWORD dwAddress, DWORD dwOldCall);
void SetWndProc(HWND hWnd, HINSTANCE hInstance);
void GetError(DWORD dwErrorCode, char *lpFunction);
int KeyMod(void);
void LoadSettings(void);
void SaveSettings(void);


DWORD WINAPI MainThread(LPVOID lpParam);
LRESULT CALLBACK ListBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int CALLBACK AddTeamProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
int CALLBACK AboutProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK HookProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void OnOpenFriendList(HWND hWnd, HINSTANCE hInstance);
void OnSaveFriendList(HWND hWnd, HINSTANCE hInstance);
void ShowHide(HWND hWnd);

extern HWND g_hWnd;
extern HMODULE hMod;
extern FARPROC lpfnWndProc;
extern HINSTANCE g_hInstance;
extern DWORD dwMainThreadId[];
extern HANDLE hMainThread[];

extern HWND hSpellHi;
extern HWND hSpellLo;
extern HWND hSpellHiHealth;
extern HWND hSpellLoHealth;
extern HWND hSpellHiMana;
extern HWND hSpellLoMana;
extern HWND hHealItem;
extern HWND hHealItemHealth;
extern HWND hAutoHealDelay;
extern HWND hUseSpellHi;
extern HWND hUseSpellLo;
extern HWND hUseHealItem;
extern HWND hAntiParalyze;
extern HWND hEnemyList;
extern HWND hAllyList;
extern HWND hAllyHealHealth;
extern HWND hUseAllySpell;
extern HWND hUseAllyItem;
extern HWND hTeamMarks;
extern HWND hTeamColors;
extern HWND hShootClosedBP;
extern HWND hAllyAntiParalyze;
extern HWND hDoubleHealing;
extern HWND hSpellTimers;
extern HWND hXRay;
extern HWND hLeastHPP;
extern HWND hShowTarget;
extern HWND hFullLight;
extern HWND hShowItemID;

extern bool bAutoHealer;
extern bool bAntiParalyze;
extern bool bAutoAllyHealer;
extern bool bShowTeamMarks;
extern bool bShowTeamColors;
extern bool bShootFromClosedBp;
extern bool bMinimized;
extern bool bDoubleHeal;
extern bool bSpellTimers;
extern bool bHotkeys;
extern bool bLeastHealth;
extern bool bShowTarget;
extern bool bShowItemID;

extern unsigned int dwSpellTime;
extern unsigned int dwItemTime;
extern unsigned int isHaste;
extern unsigned int isMagicShield;
extern unsigned int isInvisible;
extern unsigned int PlayerTarget;

struct SNameList {
	char Name[32];
};
extern SNameList *AllyNameList;
extern unsigned int AllyNameListCount;
extern SNameList *EnemyNameList;
extern unsigned int EnemyNameListCount;

struct SItemList {
	char *Name;
	int ID;
};

struct SAutoHeal {
	char Spell[256];
	int ItemID;
	int Health;
	int Mana;
	bool Use;
};

extern SAutoHeal SpellHi;
extern SAutoHeal SpellLo;
extern SAutoHeal HealItem;
extern SAutoHeal AllySpell;
extern SAutoHeal AllyHealItem;
extern SItemList HealingItems[];

struct SMagWall {
	int X;
	int Y;
	int Z;
	unsigned int Time;
};

extern SMagWall *Magwalls;
extern int MagwallCount;
extern int MagwallArray;
extern bool bMagwallTimer;
extern HWND hMagwallTimer;
extern HWND hTrackMagwalls;

// LEVELSPY
extern int levelSpyPZ;
extern int nSpy;
extern int nSpy2;
extern bool bSpy1;
extern bool bSpy2;
extern bool bXRay;

struct SLootItem {
	int ItemID;
	int LootBP;
};

extern SLootItem *LootItems;
extern int LootItemsArray;

extern bool bVIP;
extern bool bReAttack;