#include "Game.h"
#include "Home.h"
#include "Talk.h"
#include "Button.h"
#include "PrintScreen.h"
#include "GameProc.h"
#include <My/Common/C.h>

Talk::Talk(Game* p)
{
	m_pGame = p;
}

// 当有多个NPC在一起时, 会有NPC列表选择
bool Talk::IsNeedCheckNPC()
{
	// 多个NPC
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 503, 40, 513, 50, 0, true);
	return m_pGame->m_pPrintScreen->ComparePixel("多个NPC", nullptr, 1) > 0;
}

// 选择NPC
void Talk::SelectNPC(int no)
{
	int x = 450, x2 = 600, y = 0, y2 = 0;
	if (no == 0)
		y = 90, y2 = 125;
	if (no == 1)
		y = 175, y2 = 205;
	if (no == 2)
		y = 260, y2 = 286;

	m_pGame->m_pGameProc->Click(x, y, x2, y2);
}

// NPC
DWORD Talk::NPC(const char* name)
{
	// 对话按钮坐标[875,340 912,388]
	//m_pGame->m_pEmulator->Tap(MyRand(875, 912), MyRand(350, 376));
	m_pGame->m_pGameProc->Click(875, 350, 912, 376);
	return 0;
}

// NPC对话选择项
void Talk::Select(DWORD no, bool show_log)
{
	if (show_log)
		LOGVARN2(32, "c6", L"选项:0x%02d", no);

	int x = 50, y = 0;
	int x2 = 286, y2 = 0;
	if (no == 0) {
		y = 170, y2 = 180;
	}
	else if (no == 1) {
		y = 205, y2 = 212;
	}
	else if (no == 2) {
		y = 237, y2 = 245;
	}

	//m_pGame->m_pButton->ClickTalk(pic, x, y, moumov);
}

// NPC对话选择项
void Talk::Select(const char* name, HWND pic, bool show_log)
{
	int x, y;
	bool moumov = GetSelectClickPos(name, x, y, pic);

	DbgPrint("选择:%hs(%d,%d)\n", name, x, y);
	if (show_log)
		LOGVARN2(32, "c6", L"选择:%hs(%d,%d)", name, x, y);

	m_pGame->m_pButton->ClickTalk(pic, x, y, moumov);
}

// NPC对话状态[对话框是否打开]
bool Talk::NPCTalkStatus(HWND pic)
{
	return !m_pGame->m_pButton->IsDisabled(pic);
}

// 等待对话框打开
bool Talk::WaitTalkOpen(HWND pic, DWORD ms)
{
	if (ms == 0)
		ms = 1350;

	for (int i = 0; i < ms; i+=100) {
		if (NPCTalkStatus(pic))
			return true;

		Sleep(100);
	}
	return false;
}

// 等待对话框打开
bool Talk::WaitTalkClose(HWND pic, DWORD ms)
{
	if (ms == 0)
		ms = 1250;

	for (int i = 0; i < ms; i += 50) {
		if (!NPCTalkStatus(pic))
			return true;

		Sleep(50);
	}
	return false;
}

// 对话按钮是否已打开, 靠近NPC会出现对话按钮
bool Talk::TalkBtnIsOpen()
{
	// 截取的是对话按钮上面像数
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 886, 355, 896, 365, 0, true);
	return m_pGame->m_pPrintScreen->ComparePixel("对话按钮", nullptr, 1) > 0;
}

// 获取对话选项坐标(返回鼠标是否需要移动)
bool Talk::GetSelectClickPos(const char* name, int& click_x, int& click_y, HWND pic)
{
	int x = 50, y = 0;
	int x2 = 286, y2 = 0;

	if (strcmp(name, "关闭魔法障壁") == 0) {
		y = 170, y2 = 180;
	}
	if (strcmp(name, "解开四骑士祭坛") == 0) {
		y = 170, y2 = 180;
	}
	else if (strcmp(name, "解除仇之缚灵柱禁制") == 0) {
		y = 186, y2 = 196;
	}
	else if (strcmp(name, "解除怨之缚灵柱禁制") == 0) {
		y = 186, y2 = 196;
	}
	else if (strcmp(name, "解开女伯爵祭坛封印") == 0) {
		y = 186, y2 = 196;
	}
	else if (strcmp(name, "献祭1000点生命(第一关)") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "献祭1000点生命(第二关)") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "献祭2000点生命(第二关)") == 0) {
		y = 222, y2 = 230;
	}
	else if (strcmp(name, "献祭500点魂力(第二关)") == 0) {
		y = 255, y2 = 260;
	}
	else if (strcmp(name, "献祭1000点生命(第三关)") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "献祭3000点生命(第三关)") == 0) {
		y = 222, y2 = 230;
	}
	else if (strcmp(name, "献祭500点魂力(第三关)") == 0) {
		y = 255, y2 = 260;
	}
	else if (strcmp(name, "确定释放贪求之魂") == 0) {
		y = 158, y2 = 163;
	}
	else if (strcmp(name, "确定释放怨恨之魂") == 0) {
		y = 158, y2 = 163;
	}
	else if (strcmp(name, "确定释放迷幻之魂") == 0) {
		y = 158, y2 = 163;
	}
	else if (strcmp(name, "确定释放离爱之魂") == 0) {
		y = 158, y2 = 163;
	}
	else if (strcmp(name, "解开督军祭坛封印") == 0) {
		y = 170, y2 = 180;
	}
	else if (strcmp(name, "关闭火元素障壁") == 0) {
		y = 170, y2 = 180;
	}
	else if (strcmp(name, "火元素障壁.确定") == 0) {
		y = 126, y2 = 135;
	}
	else if (strcmp(name, "关闭冰元素障壁") == 0) {
		y = 170, y2 = 180;
	}
	else if (strcmp(name, "冰元素障壁.确定") == 0) {
		y = 126, y2 = 135;
	}
	else if (strcmp(name, "关闭雷元素障壁") == 0) {
		y = 158, y2 = 165;
	}
	else if (strcmp(name, "雷元素障壁.确定") == 0) {
		y = 126, y2 = 135;
	}
	else if (strcmp(name, "启动血祭仪式") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "解除爱娜的封印") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "解除复仇大厅障壁") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "我立即开始处理") == 0) {
		y = 170, y2 = 180;
	}
	else if (strcmp(name, "领取爱娜之泪") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "朗读咒文") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "荣誉即吾命") == 0) {
		y = 170, y2 = 180;
	}
	else if (strcmp(name, "离开卡利亚堡") == 0) {
		y = 126, y2 = 135;
	}
	else if (strcmp(name, "我要离开") == 0) {
		y = 126, y2 = 135;
	}
	else if (strcmp(name, "最后离开.确定") == 0) {
		y = 145, y2 = 150;
	}

	click_x = MyRand(x, x2);
	click_y = MyRand(y, y2);

	HWND talkWnd = ::FindWindowEx(pic, NULL, NULL, NULL);
	RECT rect;
	::GetWindowRect(talkWnd, &rect);
	POINT point;
	::GetCursorPos(&point);

	if ((point.x < (rect.left + x)) || (point.x > (rect.left + x2)))
		return true;
	if ((point.y < (rect.top + y)) || (point.y > (rect.top + y2)))
		return true;

	return false;
}

