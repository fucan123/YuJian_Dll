#include "Game.h"
#include "Magic.h"
#include "Move.h"
#include "Button.h"
#include "GameProc.h"
#include "PrintScreen.h"
#include <My/Common/C.h>

Magic::Magic(Game* p)
{
	m_pGame = p;
}

// 使用技能
int Magic::UseMagic(const char* name, int mv_x, int mv_y, int ms, int count)
{
	wchar_t log[128];
	int click_x = 0, click_y = 0;
	BYTE key = GetMagicKey(name);
	if (strcmp(name, "诸神裁决") == 0) {
		UseCaiJue(mv_x, mv_y, ms);
	}
	else if (strcmp(name, "最终审判") == 0) {
		UseShenPan(mv_x, mv_y);
	}
	else {
		if (count == 1)
			count = 10;

		DWORD use_ms_start = GetTickCount();
		for (int i = 1; i <= count; i++) {
			if (mv_x && mv_y && strcmp(name, "虚无空间") == 0) {
				int x, y;
				m_pGame->m_pMove->MakeClickCoor(x, y, mv_x, mv_y, m_pGame->m_pGameProc->m_pAccount);
				m_pGame->m_pButton->MouseMovePos(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, x, y);
				Sleep(150);
			}
			m_pGame->m_pButton->Key(key);

			DWORD use_ms = GetTickCount();
			DbgPrint("使用技能:%s\n", name);
			LOGVARP2(log, "c6", L"%d.使用技能:%hs", i, name);

			for (int j = 0; j < 350; j += 50) {
				Sleep(50);

				if (m_pGame->m_pGameProc->m_bPause)
					break;

				int result = MagicIsOut(name);
				if (result == -1) {
					return -1;
				}
				if (result > 0) {
					DWORD ms = GetTickCount() - use_ms;
					if ((ms & 0x0f) == 0x08) {
						m_pGame->m_pGameProc->ChNCk();
					}
					if (ms >= 100) {
						DbgPrint("技能:%s(%d)已完成释放, 用时:%d毫秒\n", name, m_nPixelCount, ms);
						LOGVARP2(log, "c6", L"技能:%hs(%d)已释放, 用时:(%d)毫秒",name, m_nPixelCount, GetTickCount() - use_ms_start);
						return 1;
					}
				}
			}
		}
	}

	return 0;
}

// 使用诸神裁决[mv_x=鼠标移动x, mv_y鼠标移动y]
int Magic::UseCaiJue(int mv_x, int mv_y, int ms)
{
	int x, y;
	m_pGame->m_pMove->MakeClickCoor(x, y, mv_x, mv_y, m_pGame->m_pGameProc->m_pAccount);
	m_pGame->m_pButton->MouseMovePos(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, x, y);
	Sleep(150);

	BYTE key = GetMagicKey("诸神裁决");
	m_pGame->m_pButton->KeyDown(key);
	for (int i = 0; i < ms; i += 100) {
		Sleep(100);
	}	
	m_pGame->m_pButton->KeyUp(key);

	return 0;
}

// 使用最终审判
int Magic::UseShenPan(int mv_x, int mv_y)
{
	m_pGame->m_pGameProc->CloseTipBox();

	if (mv_x && mv_y) { // 移动
		int x, y;
		m_pGame->m_pMove->MakeClickCoor(x, y, mv_x, mv_y, m_pGame->m_pGameProc->m_pAccount);
		m_pGame->m_pButton->MouseMovePos(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, x, y);
		Sleep(150);
	}

	BYTE key = GetMagicKey("最终审判");

	int n = 0, flag = 0;
	DWORD start_ms = GetTickCount(), use_ms, now_ms;
	while (n++ < 10) {
		use_ms = GetTickCount();
		m_pGame->m_pButton->Key(key);
		now_ms = GetTickCount();
		if ((now_ms - start_ms) >= 300) {
			flag = 2;
			break;
		}
		Sleep(180);
	}

	DWORD ms = GetTickCount() - start_ms;
	LOGVARN2(64, "c6", L"技能:最终审判已完成(%d), 次数:(%d), 用时:(%d)毫秒",
		flag, n, ms);
	return 0;
}

// 使用攻击符
void Magic::UseGongJiFu()
{
	LOG2(L"使用攻击符.", "green");
	m_pGame->m_pButton->Key('4');
}

// 使用神尊天降(选择区域的xy)
bool Magic::UseShenZunTianJiang(const char* map)
{
	wchar_t log[64];
	LOGVARP2(log, "c0 b", L"传送到:%hs", map);

	HWND game = m_pGame->m_pGameProc->m_pAccount->Wnd.Game;
	HWND pic = m_pGame->m_pGameProc->m_pAccount->Wnd.Pic;

	DWORD pos_x, pos_y;
	m_pGame->m_pGameData->ReadCoor(&pos_x, &pos_y, m_pGame->m_pGameProc->m_pAccount);

	while (true) {
		while (m_pGame->m_pGameProc->m_bPause) Sleep(100);

		int x, y, qx, qy;
	_start_:
		if (strcmp(map, "迷梦沼泽") == 0) {
			x = MyRand(620, 630);
			y = MyRand(420, 426);
			qx = MyRand(350, 500);
			qy = MyRand(210, 230);
		}
		else if (strcmp(map, "雷鸣大陆") == 0) {
			x = MyRand(609, 630);
			y = MyRand(350, 360);
			qx = MyRand(763, 863);
			qy = MyRand(350, 360);
		}
		else {
			LOGVARP2(log, "red", L"没有此区域:%hs", map);
			return false;
		}

		LOG2(L"使用神尊天降.", "c3");
		m_pGame->m_pButton->Key(VK_F8);
		while (true) {
			while (m_pGame->m_pGameProc->m_bPause) Sleep(100);
			Sleep(800);
			if (m_pGame->m_pButton->CheckButton(game, BUTTON_ID_CLOSEMENU, "C"))
				break;
		}

		Sleep(1000);
		LOG2(L"传送到人界.", "c3");
		m_pGame->m_pButton->ClickPic(game, pic, MyRand(450, 600), MyRand(260, 360), 100); // 450,260 660,360 人界
		Sleep(1000);
		LOGVARP2(log, "c3", L"点击区域位置:%d,%d", x, y);
		m_pGame->m_pButton->ClickPic(game, pic, x, y, 100);
		Sleep(1000);
		LOG2(L"点击传送图标.", "c3");
		m_pGame->m_pButton->ClickPic(game, pic, MyRand(1170, 1172), MyRand(625, 630), 500); // 1170,616 1180,630 神尊天降图标
		Sleep(500);
		LOGVARP2(log, "c3", L"确定传送位置:%d,%d", qx, qy);
		m_pGame->m_pButton->ClickPic(game, pic, qx, qy, 100); // 地图下面
		Sleep(500);

		for (int i = 0; i < 5; i++) {
			if (m_pGame->m_pButton->CheckButton(game, BUTTON_ID_CANCEL, "取消")) {
				if (m_pGame->m_pGameProc->CloseTipBox()) {
					LOG2(L"确定此传送.", "c0 b");
					m_pGame->m_pGameProc->Wait(10 * 1000);
					
					DWORD now_x, now_y;
					m_pGame->m_pGameData->ReadCoor(&now_x, &now_y, m_pGame->m_pGameProc->m_pAccount);
					if (now_x == pos_x && now_y == pos_y) {
						LOG2(L"传送失败, 重新传送.", "red b");
						goto _start_;
					}

					return true;
				}
			}
			Sleep(500);
		}
	}
	
	return false;
}

// 获取技能按键码
BYTE Magic::GetMagicKey(const char* name)
{
	if (strcmp(name, "诸神裁决") == 0)
		return VK_F1;
	if (strcmp(name, "虚无空间") == 0)
		return VK_F2;
	if (strcmp(name, "星陨") == 0)       
		return VK_F3;
	if (strcmp(name, "影魂契约") == 0)
		return VK_F4;
	if (strcmp(name, "最终审判") == 0)
		return VK_F5;
}

// 技能是否放出
int Magic::MagicIsOut(const char* name)
{
	DWORD color = 0xFFFFFFFF, diff = 0x003A3A3A;
	int need_count = 26;
	int max_count = 300;

	// 596, 753
	int x = 0, y = 8, x2 = 0, y2 = 26;
	if (strcmp(name, "虚无空间") == 0) {     // F2
		x = 40;
		x2 = 60;
	}
	else if (strcmp(name, "星陨") == 0) {    // F3
		x = 75;
		x2 = 98;
	}
	else if (strcmp(name, "影魂契约") == 0) { // F4
		x = 110;
		x2 = 133;
	}
	else if (strcmp(name, "最终审判") == 0) { // F5
		x = 145;
		x2 = 168;

		color = 0xFFFF0000;
		diff = 0x00205050;

		need_count = 20;
	}
	else {
		return -1;
	}

	// 技能窗口离游戏主窗口位置 596,753
	x += 596;
	y += 753;
	x2 += 596;
	y2 += 753;

#if 0
	HWND hWndParent, hWndMagic;
	m_pGame->m_pButton->FindButtonWnd(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, BUTTON_ID_MAGIC,
		hWndMagic, hWndParent, "技能");
#endif

	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, x, y, x2, y2, 0, true);
	m_nPixelCount = m_pGame->m_pPrintScreen->GetPixelCount(color, diff);
	int result = m_nPixelCount >= need_count && m_nPixelCount <= max_count ? 1 : 0;
	if (result && strcmp(name, "星陨") == 0)
		result = m_nPixelCount != 73;

	return result;
}