#pragma once

class CWin
{
public:
	CWin(HWND hWnd, HINSTANCE hInstance);
	~CWin(void);
	void SetFont(int Size, char* lpFont);
	HWND CreateButton(char *lpWindowName, int X, int Y, int nWidth, int nHeight, int hMenu);
	HWND CreateCheckBox(char *lpWindowName, int X, int Y, int nWidth, int nHeight, int hMenu);
	HWND CreateEditBox(char* lpWindowName, int X, int Y, int nWidth, int nHeight, int hMenu, bool bNumber);
	HWND CreateListBox(char* lpWindowName, int X, int Y, int nWidth, int nHeight, int hMenu);
	HWND CreateGroupBox(char* lpWindowName, int X, int Y, int nWidth, int nHeight);
	HWND CreateLabel(char *lpWindowName, int X, int Y, int nWidth, int nHeight);
private:
	HWND m_hWnd;
	HINSTANCE m_hInstance;
	HFONT m_hFont;
};
