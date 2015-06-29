#include <windows.h>
#include "Hook.h"

NOTIFYICONDATA niData;
WNDPROC wndProc;
HWND g_hWnd;
HMODULE hMod;
FARPROC lpfnWndProc;
HINSTANCE g_hInstance;
DWORD dwMainThreadId[2];
HANDLE hMainThread[2];
HWND hSpellHi;
HWND hSpellLo;
HWND hSpellHiHealth;
HWND hSpellLoHealth;
HWND hSpellHiMana;
HWND hSpellLoMana;
HWND hHealItem;
HWND hHealItemHealth;
HWND hAutoHealDelay;
HWND hUseSpellHi;
HWND hUseSpellLo;
HWND hUseHealItem;
HWND hAntiParalyze;
HWND hEnemyList;
HWND hAllyList;
HWND hAllyHealHealth;
HWND hUseAllySpell;
HWND hUseAllyItem;
HWND hTeamMarks;
HWND hTeamColors;
HWND hShootClosedBP;
HWND hAllyAntiParalyze;
HWND hDoubleHealing;
HWND hSpellTimers;
HWND hXRay;
HWND hLeastHPP;
HWND hShowTarget;
HWND hFullLight;
HWND hShowItemID;

bool bAutoHealer = false;
bool bAntiParalyze = false;
bool bAutoAllyHealer = false;
bool bShowTeamMarks = false;
bool bShowTeamColors = false;
bool bShootFromClosedBp = false;
bool bMinimized = false;
bool bDoubleHeal = false;
bool bSpellTimers = false;
bool bHotkeys = true;
bool bLeastHealth = false;
bool bShowTarget = false;
bool bShowItemID = false;

unsigned int dwSpellTime = 0;
unsigned int dwItemTime = 0;
unsigned int isHaste = 0;
unsigned int isMagicShield = 0;
unsigned int isInvisible = 0;
unsigned int PlayerTarget = 0;


SNameList *AllyNameList;
unsigned int AllyNameListCount = 0;
SNameList *EnemyNameList;
unsigned int EnemyNameListCount = 0;

SAutoHeal SpellHi;
SAutoHeal SpellLo;
SAutoHeal HealItem;
SAutoHeal AllySpell;
SAutoHeal AllyHealItem;

SMagWall *Magwalls;
int MagwallCount = 0;
int MagwallArray = 128;
bool bMagwallTimer = false;
HWND hMagwallTimer;
HWND hTrackMagwalls;


// LEVELSPY
int levelSpyPZ = 0;
int nSpy = 0;
int nSpy2 = 0;
bool bSpy1 = false;
bool bSpy2 = false;
bool bXRay = false;

SLootItem *LootItems;
int LootItemsArray = 0;


bool bVIP = false;
bool bReAttack = false;