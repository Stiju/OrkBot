#include <windows.h>
#include "Win.h"

CWin::CWin(HWND hWnd, HINSTANCE hInstance)
{
	m_hWnd = hWnd;
	m_hInstance = hInstance;
	m_hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
}

CWin::~CWin(void)
{
}

void CWin::SetFont(int nSize, char *lpFont)
{
	m_hFont = CreateFont(nSize,0,0,0,FW_DONTCARE,0,0,0,
		DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,DEFAULT_PITCH | FF_DONTCARE, lpFont);
}

HWND CWin::CreateButton(char *lpWindowName, int X, int Y, int nWidth, int nHeight, int hMenu)
{
	HWND hWnd = CreateWindowExA(0, "button", lpWindowName, WS_CHILD | WS_VISIBLE, X, Y, nWidth, nHeight, m_hWnd, (HMENU)hMenu, m_hInstance, 0);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)m_hFont, true);
	return hWnd;
}

HWND CWin::CreateCheckBox(char *lpWindowName, int X, int Y, int nWidth, int nHeight, int hMenu)
{
	HWND hWnd = CreateWindowExA(0, "button", lpWindowName, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, X, Y, nWidth, nHeight, m_hWnd, (HMENU)hMenu, m_hInstance, 0);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)m_hFont, true);
	return hWnd;
}

HWND CWin::CreateEditBox(char *lpWindowName, int X, int Y, int nWidth, int nHeight, int hMenu, bool bNumber)
{
	HWND hWnd = CreateWindowExA(WS_EX_STATICEDGE, "edit", lpWindowName, WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_CENTER | ((bNumber == true)?ES_NUMBER:0), X, Y, nWidth, nHeight, m_hWnd, (HMENU)hMenu, m_hInstance, 0);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)m_hFont, true);
	return hWnd;
}

HWND CWin::CreateListBox(char *lpWindowName, int X, int Y, int nWidth, int nHeight, int hMenu)
{
	HWND hWnd = CreateWindowExA(WS_EX_STATICEDGE, "listbox", lpWindowName, WS_VISIBLE | WS_CHILD | LBS_STANDARD | LBS_DISABLENOSCROLL, X, Y, nWidth, nHeight, m_hWnd, (HMENU)hMenu, m_hInstance, 0);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)m_hFont, true);
	return hWnd;
}

HWND CWin::CreateGroupBox(char *lpWindowName, int X, int Y, int nWidth, int nHeight)
{
	HWND hWnd = CreateWindowExA(0, "button", lpWindowName, WS_CHILD | WS_VISIBLE | BS_GROUPBOX, X, Y, nWidth, nHeight, m_hWnd, 0, m_hInstance, 0);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)m_hFont, true);
	return hWnd;
}

HWND CWin::CreateLabel(char *lpWindowName, int X, int Y, int nWidth, int nHeight)
{
	HWND hWnd = CreateWindowExA(0, "static", lpWindowName, WS_CHILD | WS_VISIBLE, X, Y, nWidth, nHeight, m_hWnd, 0, m_hInstance, 0);
	SendMessage(hWnd, WM_SETFONT, (WPARAM)m_hFont, true);
	return hWnd;
}