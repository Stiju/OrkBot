#pragma once

extern unsigned short Hotkey_LevelSpyUp;
extern unsigned short Hotkey_LevelSpyDown;
extern unsigned short Hotkey_ShootMagwall;

struct SHotkey {
	bool Enabled;
	unsigned short Key;
	char Command[128];
	int Type;
	int Arg1;
	int Arg2;
	char Say[128];
};

extern SHotkey Hotkeys[];
extern bool isLooting;

void SetHotkey(HWND hWnd, int nIDDlgItem, unsigned short nHotkey);
unsigned short GetHotkey(HWND hWnd, int nIDDlgItem);
void InitHotkeyDialog(HWND hWnd);
void ApplyNewHotkeys(HWND hWnd);
void LoadHotkeys(void);
void SaveHotkeys(void);
bool CheckCommand(SHotkey *hk);
void ExecuteCommand(SHotkey *hk);
int CALLBACK HotkeysProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
DWORD WINAPI LootThread(LPVOID lpParam);

enum HotkeyCommand {
	LevelSpyUp,
	LevelSpyDown,
	Magwall,
	Mana,
	Heal,
	Say,
	Loot,
	Equip,
	Shoot,
	Attack,
};

enum HotkeyCreature {
	Self,
	Target,
	Friend,
	Enemy,
};