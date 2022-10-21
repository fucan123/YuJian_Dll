#include "Game.h"
#include "Button.h"
#include <My/Common/C.h>
#include <stdlib.h>

MCButton::MCButton(Game * p)
{
	m_pGame = p;
	m_bLock = false;
	HWND hWndSure, hWndSureParent;
	return;

	HWND hWnd = (HWND)FindWindow(NULL, L"【魔域】");
	hWnd = (HWND)0x00060196;
	HWND hWndPic = FindWindowEx(hWnd, NULL, NULL, NULL);

	char mapName[32];
	FindButtonWnd(hWnd, STATIC_ID_HP, hWndSure, hWndSureParent);
	::GetWindowTextA(hWndSure, mapName, sizeof(mapName));
	//::GetDlgItemTextA(hWndSureParent, STATIC_ID_MAP, mapName, sizeof(mapName));
	printf("%p(%p) %s\n", hWndSure, hWndSureParent, mapName);
	// 413,341卡督军 290,85大小马

	//Sleep(2600);

	// 975,85 473,63 502,22
	HWND pet = FindPetParentWndAtTop(hWnd);
	printf("宠物操作父窗口:%08X\n", pet);
	HWND petLifeWnds[4];
	FindPetLifeWndAtTop(hWnd, petLifeWnds);
	Click(pet, 0x000006F0, "CALL");
	//Click((HWND)0x00010ABC, 15, 9);
	//while (true) Sleep(169);
	HideActivity(hWnd);

	while (true) Sleep(169);

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

// 隐藏活动列表
bool MCButton::HideActivity(HWND hwnd_own)
{
	if (!IsDisabled(hwnd_own, BUTTON_ID_FULI)) {
		LOG2(L"隐藏右侧活动列表.", "c0 b");
		return Click(hwnd_own, BUTTON_ID_ACTY);
	}
	return false;
}

// 按钮是否禁用
bool MCButton::IsDisabled(HWND hwnd_own, int button_id)
{
	HWND hWnd, hWndParent;
	if (!FindButtonWnd(hwnd_own, button_id, hWnd, hWndParent))
		return true;

	return IsDisabled(hWnd);
}

// 窗口是否禁用
bool MCButton::IsDisabled(HWND hWnd)
{
	return (GetWindowLong(hWnd, GWL_STYLE)&WS_DISABLED) ? true : false;
}

// 按键
void MCButton::Key(BYTE bVk)
{
	KeyDown(bVk);
	KeyUp(bVk);
}

// 按键
void MCButton::KeyDown(BYTE bVk)
{
	keybd_event(bVk, 0, 0, NULL);
}

// 按键
void MCButton::KeyUp(BYTE bVk)
{
	keybd_event(bVk, 0, KEYEVENTF_KEYUP, NULL);
}

// 关闭按钮按钮
bool MCButton::CloseButton(HWND game, int button_id, const char* text)
{
	HWND hWnd = FindButtonWnd(game, button_id, text);
	if (hWnd && !IsDisabled(hWnd)) {
		if (button_id == BUTTON_ID_CLOSEMENU && text && text[0] == 'x') {
			HWND h2 = ::GetNextWindow(hWnd, GW_HWNDNEXT);
			if (h2) {
				int id = ::GetDlgCtrlID(h2);
				if (id == 0x8C3) { // 最大化按钮
					return false;
				}
			}
		}
		return Click(game, button_id, text);
	}

	return false;
}

// 点击屏幕坐标
bool MCButton::ClickScreen(int x, int y, int flag, bool left_click)
{
	return Click(NULL, x, y, flag, left_click);
}

// 点击游戏画面
bool MCButton::ClickPic(HWND game, HWND pic, int x, int y, DWORD sleep_ms, bool left_click)
{
	while (m_bLock);
	m_bLock = true;

	::SetForegroundWindow(game);
	HWND top = GetForegroundWindow();
	if (top != game && top != pic) { // 窗口不在最前
		m_bLock = false;
		return false;
	}
	
	MouseMovePos(pic, x, y);
	Sleep(sleep_ms);

	if (left_click) {
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	}
	else {
		mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
	}
	
	m_bLock = false;
	return true;
}

// 点击对话选项
bool MCButton::ClickTalk(HWND pic, int x, int y, bool moumov)
{
	HWND talkWnd = FindTalkWnd(pic);
	if (IsDisabled(talkWnd))
		return false;

	RECT rect;
	::GetWindowRect(talkWnd, &rect);
	if (moumov) {
		//LOGVARN2(64, "c6", L"鼠标:%d,%d(%d,%d | %d,%d)", rect.left + x, rect.top + y, rect.left, rect.top, x, y);
		MouseMovePos(talkWnd, x, y);
		Sleep(100);
	}

	// while (true) Sleep(168);
	
	Click(talkWnd, x, y);

	//::printf("click talkwnd:%08X %d %d\n", talkWnd, x, y);

	return true;
}

// 点击按钮
bool MCButton::Click(HWND hwnd, int x, int y, int flag, bool left_click)
{
#if 1
	UINT uMsg = left_click ? WM_LBUTTONDOWN : WM_RBUTTONDOWN;
	UINT uMsg2 = left_click ? WM_LBUTTONUP : WM_RBUTTONUP;
	WPARAM wParam = left_click ? MK_LBUTTON : MK_RBUTTON;
	if (flag & 0x01) {
		::PostMessage(hwnd, uMsg, wParam, MAKELPARAM(x, y));
	}
	if (flag & 0x02) {
		::PostMessage(hwnd, uMsg2, 0, MAKELPARAM(x, y));
	}
#else
	UINT uMsg = left_click ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
	UINT uMsg2 = left_click ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTUP;
	if (flag & 0x01) {
		mouse_event(uMsg, 0, 0, 0, 0);
	}
	if (flag & 0x02) {
		mouse_event(uMsg2, 0, 0, 0, 0);
	}
#endif
	return true;
}

// 点击按钮
bool MCButton::Click(HWND hwnd_own, int button_id, const char* text)
{
	HWND hWnd, hWndParent;
	if (FindButtonWnd(hwnd_own, button_id, hWnd, hWndParent, text)) {
#if 1
		SendMessage(hWndParent, WM_COMMAND, MAKEWPARAM(button_id, BN_CLICKED), (LPARAM)hWnd); // 点击按钮
		//printf("Click:%p(%p) %lld(%d)\n", hWnd, hWndParent, r, GetLastError());
#else
		RECT rect;
		GetWindowRect(hWnd, &rect);
		MouseMovePos(NULL, MyRand(rect.left + 2, rect.left - 2), MyRand(rect.top + 2, rect.bottom - 2));
		Sleep(100);
		ClickScreen(0, 0);
#endif
		return true;
	}
	return false;
}

// 点击按钮
bool MCButton::Click(HWND hwnd_own, int button_id, const char* text, int x, int y, int flag, bool left_click)
{
	HWND hWnd, hWndParent;
	if (FindButtonWnd(hwnd_own, button_id, hWnd, hWndParent, text)) {
		Click(hWnd, x, y, flag, left_click);
		// printf("Click:%p(%p)(%d)\n", hWnd, hWndParent, GetLastError());
		return true;
	}
	return false;
}

// 点击按钮(坐标[屏幕的坐标])需要转换(相对于[hwnd_own的坐标])
bool MCButton::ClickRel(HWND hwnd_own, int button_id, const char* text, int x, int y, int flag, bool left_click)
{
	int x2, y2;
	m_pGame->m_pButton->CalcRelGamePos(x2, y2, hwnd_own, button_id, text);
	x -= x2;
	y -= y2;

	return Click(hwnd_own, button_id, text, x, y, flag, left_click);
}

// 鼠标移动到指定位置
void MCButton::MouseMovePos(HWND hWnd, int x, int y)
{
	int screen_x = GetSystemMetrics(SM_CXSCREEN);
	int screen_y = GetSystemMetrics(SM_CYSCREEN);

	if (hWnd) {
		RECT rect;
		::GetWindowRect(hWnd, &rect);

		x += rect.left;
		y += rect.top;
	}

#if 1
	POINT point;
	if (GetCursorPos(&point)) { // 获取了鼠标的当前位置
		int mv_x = x - point.x;
		int mv_y = y - point.y;
		int max = max(abs(mv_x), abs(mv_y));
		int fx = mv_x >= 0 ? 1 : -1;
		int fy = mv_y >= 0 ? 1 : -1;
		for (int i = 1; i < max; ) {
			
			int dist_x = i * fx + point.x;
			int dist_y = i * fy + point.y;

			int flag = 0;
			if (fx == 1 && dist_x > x) { // 超过移动位置x
				dist_x = x;
				flag++;
			}
			if (fx == -1 && dist_x < x) { // 超过移动位置x
				dist_x = x;
				flag++;
			}

			if (fy == 1 && dist_y > y) { // 超过移动位置y
				dist_y = y;
				flag++;
			}
			if (fy == -1 && dist_y < y) { // 超过移动位置Y
				dist_y = y;
				flag++;
			}
			if (flag >= 2)
				break;

			int rand_x = MyRand(1, 3);
			int rand_y = MyRand(1, 3);

			dist_x = dist_x * 65536 / screen_x;
			dist_y = dist_y * 65536 / screen_y;
			mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, dist_x, dist_y, 0, 0);

			i += max(rand_x, rand_y);
		}
	}
#endif

	x = x * 65536 / screen_x;
	y = y * 65536 / screen_y;

#if 0
	POINT point;
	GetCursorPos(&point);
	mouse_event(MOUSEEVENTF_MOVE, x - point.x, y - point.y, 0, 0);

	printf("MouseMovePos:%d,%d(%d,%d) %d,%d %d,%d\n", x, y, ox, oy, point.x, point.y, x - point.x, y - point.y);
#else
	mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, x, y, 0, 0);
#endif
}

// 存入钱
bool MCButton::SaveMoney(HWND hwnd_own)
{
	if (!OpenStorage(hwnd_own))
		return false;

	Sleep(1000);
}

// 打开仓库窗口
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

// 关闭仓库窗口
bool MCButton::CloseStorage(HWND hwnd_own)
{
	return Click(hwnd_own, BUTTON_ID_CLOSECKIN);
}

// 检查按钮
bool MCButton::CheckButton(HWND hwnd_own, int button_id, const char* text)
{
	HWND hWnd, hWndParent;
	return FindButtonWnd(hwnd_own, button_id, hWnd, hWndParent, text);
}

// 获取顶部宠物血量窗口
int MCButton::FindPetLifeWndAtTop(HWND game_wnd, HWND wnds[])
{
	// 975,85 473,63 502,22
	// 1114,85 473,63 641,22
	// 1254,85 473,63 781,22
	// 1394,85 473,63 921,22
	//game_wnd = (HWND)0x00020F68;
	HWND petParentHwnd = FindPetParentWndAtTop(game_wnd);
	EnumWndInfo info;
	info.Left = 502;
	info.Top = -1;
	info.Width = -1;
	info.Height = -1;
	info.WidthFlag = 0;
	info.HeightFlag = 0;
	info.Text = (char*)"/";
	info.TextFlag = -1;
	info.GameWnd = game_wnd;
	info.ReturnV = NULL;

	int length = 0;
	EnumChildWindows(petParentHwnd, EnumProcByWndInfo, (LPARAM)&info);
	wnds[length++] = info.ReturnV;
	printf("宠物血量窗口:%08X(%08X)\n", info.ReturnV, petParentHwnd);

	info.Left = 641;
	info.ReturnV = NULL;
	EnumChildWindows(petParentHwnd, EnumProcByWndInfo, (LPARAM)&info);
	wnds[length++] = info.ReturnV;
	printf("宠物血量窗口:%08X\n", info.ReturnV);

	info.Left = 781;
	info.ReturnV = NULL;
	EnumChildWindows(petParentHwnd, EnumProcByWndInfo, (LPARAM)&info);
	wnds[length++] = info.ReturnV;
	printf("宠物血量窗口:%08X\n", info.ReturnV);

	info.Left = 921;
	info.ReturnV = NULL;
	EnumChildWindows(petParentHwnd, EnumProcByWndInfo, (LPARAM)&info);
	wnds[length++] = info.ReturnV;
	printf("宠物血量窗口:%08X\n", info.ReturnV);

	return length;
}

// 获取顶部操作宠物父窗口
HWND MCButton::FindPetParentWndAtTop(HWND game)
{
	EnumWndInfo info;
	info.Left = -1;
	info.Top = -1;
	info.Width = 580;
	info.Height = 85;
	info.WidthFlag = 0;
	info.HeightFlag = 0;
	info.Text = NULL;
	info.TextFlag = 0;
	info.GameWnd = game;
	info.ReturnV = NULL;
	EnumChildWindows(game, EnumProcByWndInfo, (LPARAM)&info);
	return info.ReturnV;
}

// 获取对话框窗口
HWND MCButton::FindTalkWnd(HWND pic)
{
	HWND hWnd = NULL;
	EnumChildWindows(pic, EnumProcTalkWnd, (LPARAM)&hWnd);
	return hWnd;
}

// 获取窗口句柄
HWND MCButton::FindButtonWnd(HWND hwnd_own, int button_id, const char* text)
{
	HWND hWnd, hWndParent;
	if (FindButtonWnd(hwnd_own, button_id, hWnd, hWndParent, text))
		return hWnd;

	return NULL;
}

// 获得窗口句柄
bool MCButton::FindButtonWnd(HWND hwnd_own, int button_id, HWND& hwnd, HWND& parent, const char* text)
{
	HWND wnds[] = { (HWND)button_id, NULL, (HWND)text };
	::EnumChildWindows(hwnd_own, EnumChildProc, (LPARAM)wnds);
	hwnd = wnds[0];
	parent = wnds[1];
	return parent != NULL;
}

// 获得游戏窗口
HWND MCButton::FindGameWnd(DWORD pid)
{
	DWORD pid_t = pid;
	::EnumChildWindows(NULL, EnumProc, (LPARAM)&pid_t);
	if (pid != pid_t)
		return (HWND)pid_t;

	return NULL;
}

// 登录窗口
HWND MCButton::FindLoginWnd(HWND hWndGame)
{
	HWND hWndChild = ::FindWindowEx(hWndGame, NULL, NULL, NULL);
	HWND hWndChild2 = ::FindWindowEx(hWndChild, NULL, NULL, NULL);
	if (!hWndChild2)
		return NULL;

	while (true) {
		RECT rect;
		::GetWindowRect(hWndChild2, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		if (width == 1024 && height == 768)
			return hWndChild2;

		hWndChild2 = ::GetNextWindow(hWndChild2, GW_HWNDNEXT);
		if (!hWndChild2)
			return NULL;
	}

	return NULL;
}

// 计算相对于游戏窗口位置
bool MCButton::CalcRelGamePos(int& x, int& y, HWND game, int button_id, const char* text)
{
	x = 0;
	y = 0;

	HWND hWnd = FindButtonWnd(game, button_id, text);
	if (!hWnd)
		return false;

	RECT r, r2;
	::GetWindowRect(game, &r);
	::GetWindowRect(hWnd, &r2);

	x = r2.left - r.left;
	y = r2.top - r.top;

	return true;
}

// 枚举对话框窗口
BOOL MCButton::EnumProcByWndInfo(HWND hWnd, LPARAM lParam)
{
	EnumWndInfo* p = (EnumWndInfo*)lParam;
	RECT gameRect, rect;
	::GetWindowRect(hWnd, &rect);
	if (p->GameWnd != NULL) { // 游戏窗口位置
		::GetWindowRect(p->GameWnd, &gameRect);
	}

	if (0 && p->Text) {
		printf("%08X(%d,%d) %d,%d %d,%d\n", hWnd, rect.left, rect.top, p->Left,p->Top, rect.left - gameRect.left, rect.top - gameRect.top);
	}

	bool result = true;
	if (p->Left != -1)
		result = result && (rect.left-gameRect.left) == p->Left;
	if (p->Top != -1)
		result = result && (rect.top-gameRect.top) == p->Top;
	if (p->Width != -1) {
		int width = rect.right - rect.left;
		if (p->WidthFlag == 0)
			result = result && width == p->Width;
		if (p->WidthFlag == -1)
			result = result && width < p->Width;
		if (p->WidthFlag == -2)
			result = result && width > p->Width;
	}
	if (p->Height != -1) {
		int height = rect.bottom - rect.top;
		if (p->WidthFlag == 0)
			result = result && height == p->Height;
		if (p->WidthFlag == -1)
			result = result && height < p->Height;
		if (p->WidthFlag == -2)
			result = result && height > p->Height;
	}
	if (p->Text) {
		if (0 && result) {
			printf("result:%08X\n----------------\n", hWnd);
		}
		char name[64];
		GetWindowTextA(hWnd, name, sizeof(name));
		if (p->TextFlag == 0)
			result = result && strcmp(name, p->Text) == 0;
		if (p->TextFlag == -1)
			result = result && strstr(name, p->Text);
	}
	if (result) { // 已找到
		p->ReturnV = hWnd;
		return FALSE;
	}

	return TRUE;
}

// 枚举对话框窗口
BOOL MCButton::EnumProcTalkWnd(HWND hWnd, LPARAM lParam)
{
	RECT rect;
	::GetWindowRect(hWnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	if (width == 520 && height > 200) {
		*(HWND*)(lParam) = hWnd;
		return FALSE;
	}

	return TRUE;
}

// 枚举窗口
BOOL MCButton::EnumProc(HWND hWnd, LPARAM lParam)
{
	DWORD thepid = *(DWORD*)lParam;
	DWORD pid;
	GetWindowThreadProcessId(hWnd, &pid);
	//::printf("HWND:%08X pid:%d thepid:%d\n", (DWORD)hWnd, pid, thepid);
	if (pid == thepid) {
		char name[64];
		GetWindowTextA(hWnd, name, sizeof(name));
		if (strcmp(name, "【魔域】") == 0) {
			*(HWND*)lParam = hWnd;
			return FALSE;
		}

	}
	return TRUE;
}

// 枚举子窗口
BOOL MCButton::EnumChildProc(HWND hWnd, LPARAM lParam)
{
	HWND* param = (HWND*)lParam;
	HWND hWnd_Child = ::GetDlgItem(hWnd, (int)param[0]);

	if (hWnd_Child) { // 找到了子窗口
		//printf("hWnd_Child:%08X\n", hWnd_Child);
		if (param[2]) { // 如果搜索按钮文字
			char text[32];
			GetWindowTextA(hWnd_Child, text, sizeof(text)); // 获取按钮文字
			if (strcmp(text, (const char*)param[2]) != 0)
				return TRUE;
		}

		param[0] = hWnd_Child; // 子窗口句柄 
		param[1] = hWnd;       // 父窗口句柄

		return FALSE;
	}
	return TRUE;
}
