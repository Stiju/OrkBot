#pragma once

extern HWND hLootList;

void AddItem(int ItemID, int Backpack);
void LoadLootList(void);
void SaveLootList(void);
void AcceptLootList(void);
void InitLootList(HWND hWnd);

int CALLBACK LootListProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
