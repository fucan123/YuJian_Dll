#include "Game.h"
#include "Button.h"
#include <stdlib.h>

MCButton::MCButton(Game * p)
{
	m_pGame = p;

	return;

	HWND hWnd = (HWND)FindWindow(NULL, L"��ħ��");
	hWnd = (HWND)0x00030660;
	HWND hWndPic = FindWindowEx(hWnd, NULL, NULL, NULL);

	char mapName[32];
	HWND hWndSure, hWndSureParent;
	FindButtonWnd(hWnd, STATIC_ID_HP, hWndSure, hWndSureParent);
	::GetWindowTextA(hWndSure, mapName, sizeof(mapName));
	//::GetDlgItemTextA(hWndSureParent, STATIC_ID_MAP, mapName, sizeof(mapName));
	printf("%p(%p) %s\n", hWndSure, hWndSureParent, mapName);
	// 413,341������ 290,85��С��

	//Sleep(2600);

	HideActivity(hWnd);

	return;

	Click((HWND)0x00010BF8, 95, 72);
	//Click(hWnd, 0x6F0, "CALL2");
	//Click(hWnd, 0x6F3, "CALL3");
	//Click(hWnd, 0x6F1);
	//Click(hWnd, 0x8A1);
	Sleep(100);
	while (true) Sleep(900);
#if 0
	int x = 500, y = 500;
	::SendMessage(hWndPic, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y));
	::SendMessage(hWndPic, WM_LBUTTONUP, 0, MAKELPARAM(x, y));
#endif
	return;
	if (SaveMoney(hWnd)) {
		Sleep(1680);
		CloseStorage(hWnd);
	}
}

// ���ػ�б�
bool MCButton::HideActivity(HWND hwnd_own)
{
	if (!IsDisabled(hwnd_own, BUTTON_ID_FULI)) {
		return Click(hwnd_own, BUTTON_ID_ACTY);
	}
	return true;
}

// ��ť�Ƿ����
bool MCButton::IsDisabled(HWND hwnd_own, int button_id)
{
	HWND hWnd, hWndParent;
	if (!FindButtonWnd(hwnd_own, BUTTON_ID_FULI, hWnd, hWndParent))
		return true;

	return IsDisabled(hWnd);
}

// �����Ƿ����
bool MCButton::IsDisabled(HWND hWnd)
{
	return (GetWindowLong(hWnd, GWL_STYLE)&WS_DISABLED) ? true : false;
}

// ����
void MCButton::Key(BYTE bVk)
{
	KeyDown(bVk);
	KeyUp(bVk);
}

// ����
void MCButton::KeyDown(BYTE bVk)
{
	keybd_event(bVk, 0, 0, NULL);
}

// ����
void MCButton::KeyUp(BYTE bVk)
{
	keybd_event(bVk, 0, KEYEVENTF_KEYUP, NULL);
}

// �����Ϸ����
bool MCButton::ClickPic(HWND game, HWND pic, int x, int y)
{
	::SetForegroundWindow(game);
	HWND top = GetForegroundWindow();
	if (top != game && top != pic) // ���ڲ�����ǰ
		return false;

	RECT rect;
	::GetWindowRect(pic, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	if (x <= 0 || x >= width)
		return false;
	if (y <= 0 || y >= height)
		return false;

	SetCursorPos(rect.left + x, rect.top + y);
	Sleep(100);

	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	return true;
}

// ����Ի�ѡ��
bool MCButton::ClickTalk(HWND pic, int x, int y, bool moumov)
{
	HWND talkWnd = ::FindWindowEx(pic, NULL, NULL, NULL);
	if (!talkWnd)
		return false;

	RECT rect;
	::GetWindowRect(talkWnd, &rect);
	if (moumov) {
		
		
		SetCursorPos(rect.left + x, rect.top + y);
		Sleep(500);
	}

	DbgPrint("ClickTalk:%d,%d %d,%d(%08X)\n", rect.left, rect.top, x, y, talkWnd);

	// while (true) Sleep(168);
	
	Click(talkWnd, x, y);

	return true;
}

// �����ť
bool MCButton::Click(HWND hwnd, int x, int y, int flag)
{
	if (flag & 0x01) {
		::PostMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y));
	}
	if (flag & 0x02) {
		::PostMessage(hwnd, WM_LBUTTONUP, 0, MAKELPARAM(x, y));
	}
	return true;
}

// �����ť
bool MCButton::Click(HWND hwnd_own, int button_id, const char* text)
{
	HWND hWnd, hWndParent;
	if (FindButtonWnd(hwnd_own, button_id, hWnd, hWndParent, text)) {
		LRESULT r = ::SendMessage(hWndParent, WM_COMMAND, MAKEWPARAM(button_id, BN_CLICKED), (LPARAM)hWnd); // �����ť
		printf("Click:%p(%p) %lld(%d)\n", hWnd, hWndParent, r, GetLastError());
		return true;
	}
	return false;
}

// �����ť
bool MCButton::Click(HWND hwnd_own, int button_id, const char* text, int x, int y, int flag)
{
	HWND hWnd, hWndParent;
	if (FindButtonWnd(hwnd_own, button_id, hWnd, hWndParent, text)) {
		Click(hWnd, x, y, flag);
		printf("Click:%p(%p)(%d)\n", hWnd, hWndParent, GetLastError());
		return true;
	}
	return false;
}

// ����ƶ���ָ��λ��
void MCButton::MouseMovePos(HWND hWnd, int x, int y)
{
	RECT rect;
	::GetWindowRect(hWnd, &rect);
	SetCursorPos(rect.left + x, rect.top + y);
}

// ����Ǯ
bool MCButton::SaveMoney(HWND hwnd_own)
{
	if (!OpenStorage(hwnd_own))
		return false;

	Sleep(1000);
}

// �򿪲ֿⴰ��
bool MCButton::OpenStorage(HWND hwnd_own)
{
	if (Click(hwnd_own, BUTTON_ID_VIP)) {
		for (int i = 0; i < 10; i++) {
			Sleep(800);
			if (Click(hwnd_own, BUTTON_ID_CHECKIN))
				return true;
		}
	}
	return false;
}

// �رղֿⴰ��
bool MCButton::CloseStorage(HWND hwnd_own)
{
	return Click(hwnd_own, BUTTON_ID_CLOSECKIN);
}

// ��鰴ť
bool MCButton::CheckButton(HWND hwnd_own, int button_id, const char* text)
{
	HWND hWnd, hWndParent;
	return FindButtonWnd(hwnd_own, button_id, hWnd, hWndParent, text);
}

// ��ô��ھ��
bool MCButton::FindButtonWnd(HWND hwnd_own, int button_id, HWND& hwnd, HWND& parent, const char* text)
{
	HWND wnds[] = { (HWND)button_id, NULL, (HWND)text };
	::EnumChildWindows(hwnd_own, EnumChildProc, (LPARAM)wnds);
	hwnd = wnds[0];
	parent = wnds[1];
	return parent != NULL;
}

// �����Ϸ����
HWND MCButton::FindGameWnd(DWORD pid)
{
	DWORD pid_t = pid;
	::EnumChildWindows(NULL, EnumProc, (LPARAM)&pid_t);
	if (pid != pid_t)
		return (HWND)pid_t;

	return NULL;
}

// ö�ٴ���
BOOL MCButton::EnumProc(HWND hWnd, LPARAM lParam)
{
	DWORD thepid = *(DWORD*)lParam;
	DWORD pid;
	GetWindowThreadProcessId(hWnd, &pid);
	//::printf("HWND:%08X pid:%d thepid:%d\n", (DWORD)hWnd, pid, thepid);
	if (pid == thepid) {
		char name[64];
		GetWindowTextA(hWnd, name, sizeof(name));
		if (strcmp(name, "��ħ��") == 0) {
			*(HWND*)lParam = hWnd;
			return FALSE;
		}

	}
	return TRUE;
}

// ö���Ӵ���
BOOL MCButton::EnumChildProc(HWND hWnd, LPARAM lParam)
{
	HWND* param = (HWND*)lParam;
	HWND hWnd_Child = ::GetDlgItem(hWnd, (int)param[0]);
	if (hWnd == (HWND)0x000F1CE6)
		printf("hWnd:%08X\n", hWnd);

	if (hWnd_Child) { // �ҵ����Ӵ���
		//printf("hWnd_Child:%08X\n", hWnd_Child);
		if (param[2]) { // ���������ť����
			char text[32];
			GetWindowTextA(hWnd_Child, text, sizeof(text)); // ��ȡ��ť����
			if (strcmp(text, (const char*)param[2]) != 0)
				return TRUE;
		}

		param[0] = hWnd_Child; // �Ӵ��ھ�� 
		param[1] = hWnd;       // �����ھ��

		return FALSE;
	}
	return TRUE;
}
