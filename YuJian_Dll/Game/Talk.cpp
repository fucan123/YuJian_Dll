#include "Game.h"
#include "Home.h"
#include "Talk.h"
#include "Item.h"
#include "Button.h"
#include "PrintScreen.h"
#include "GameProc.h"
#include <My/Common/C.h>

Talk::Talk(Game* p)
{
	m_pGame = p;
}

// 点击第几个好友
bool Talk::ClickFaceBg(int no)
{
	int x = MyRand(50, 66);
	int y = 0;
	if (no == 1) {
		y = MyRand(45, 50);
	}
	if (no == 2) {
		y = MyRand(45, 50);
	}
	if (no == 3) {
		y = MyRand(45, 50);
	}
	if (no == 4) {
		y = MyRand(45, 50);
	}
	return false;
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
	int x = 0, y = 0;
	bool moumov = GetSelectClickPos(name, x, y, pic);

	if (show_log) {
		DbgPrint("选择:%hs(%d,%d) %d\n", name, x, y, moumov);
		LOGVARN2(32, "c6", L"选择:%hs(%d,%d)", name, x, y);
	}
		
	m_pGame->m_pButton->ClickTalk(pic, x, y, true);
}

// NPC对话状态[对话框是否打开]
bool Talk::NPCTalkStatus(HWND pic)
{
	HWND talkWnd = m_pGame->m_pButton->FindTalkWnd(pic);
	::printf("NPCTalkStatus->talkWnd:%08X\n", talkWnd);
	return !m_pGame->m_pButton->IsDisabled(talkWnd);
}

// 等待商店打开
bool Talk::WaitShopOpen(DWORD ms)
{
	if (ms == 0)
		ms = 1350;

	for (int i = 0; i < ms; i += 100) {
		if (m_pGame->m_pItem->BagIsOpen())
			return true;

		Sleep(100);
	}
	return false;
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

	if (m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_hWndGame, BUTTON_ID_CANCEL))
		Sleep(500);
	if (m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_hWndGame, BUTTON_ID_CLOSEMENU, "x"))
		Sleep(500);
	if (m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_hWndGame, BUTTON_ID_CLOSEMENU, "X"))
		Sleep(500);

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
	int x2 = 150, y2 = 0;

	if (strcmp(name, "我要去雷鸣交易行") == 0) {
		y = 185, y2 = 187;
	}
	else if (strcmp(name, "领取爱娜祈祷项链") == 0) {
		y = 285, y2 = 288;
	}
	else if (strcmp(name, "领取特制经验球") == 0) {
		y = 320, y2 = 323;
	}
	else if (strcmp(name, "挑战九星副本(阿拉玛)") == 0) {
		y = 179, y2 = 180;
	}
	else if (strcmp(name, "钥匙开启入口") == 0) {
		y = 127, y2 = 132;
	}
	else if (strcmp(name, "项链开启入口") == 0) {
		y = 162, y2 = 165;
	}
	else if (strcmp(name, "进入卡利亚堡") == 0) {
		y = 127, y2 = 132;
	}
	else if (strcmp(name, "继续进入副本") == 0) {
		y = 127, y2 = 132;
	}
	else if (strcmp(name, "离开卡利亚堡[外]") == 0) {
		y = 162, y2 = 163;
	}
	else if (strcmp(name, "我要离开[外]") == 0) {
		y = 172, y2 = 178;
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
	else if (strcmp(name, "关闭魔法障壁") == 0) {
		y = 170, y2 = 180;
	}
	else if (strcmp(name, "关闭爆雷障壁") == 0) {
		y = 158, y2 = 163;
	}
	else if (strcmp(name, "解开四骑士祭坛") == 0) {
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
		y = 172, y2 = 178;
	}
	else if (strcmp(name, "献祭2000点生命(第二关)") == 0) {
		y = 207, y2 = 212;
	}
	else if (strcmp(name, "献祭500点魂力(第二关)") == 0) {
		y = 240, y2 = 246;
	}
	else if (strcmp(name, "献祭1000点生命(第三关)") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "献祭3000点生命(第三关)") == 0) {
		y = 220, y2 = 225;
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
	else if (strcmp(name, "卖东西.装备商") == 0) {
	    y = 160, y2 = 163;
	}
	else if (strcmp(name, "卖东西.武器商") == 0) {
		y = 130, y2 = 133;
	}
	else if (strcmp(name, "卖东西.杂货商") == 0) {
		y = 160, y2 = 163;
	}
	else if (strcmp(name, "卖东西.首饰商") == 0) {
		y = 130, y2 = 135;
	}
	else if (strcmp(name, "卖东西.武器商[亚维特岛]") == 0) {
		y = 130, y2 = 133;
	}
	else if (strcmp(name, "卖东西.炼金师[亚维特岛]") == 0) {
		y = 160, y2 = 163;
	}
	else if (strcmp(name, "卖东西.首饰匠[亚维特岛]") == 0) {
		y = 160, y2 = 163;
	}
	else if (strcmp(name, "卖东西.装备商[亚维特岛]") == 0) {
		y = 130, y2 = 133;
	}
	else if (strcmp(name, "卖东西.购买庆典礼花") == 0) {
		y = 172, y2 = 175;
	}
	else if (strcmp(name, "活动.全部直接合成+1或+2") == 0) {
		y = 230, y2 = 233;
	}
	else if (strcmp(name, "活动.全部直接合成+1或+2.确定") == 0) {
		y = 130, y2 = 133;
	}

	click_x = MyRand(x, x2);
	click_y = MyRand(y, y2);

	HWND talkWnd = m_pGame->m_pButton->FindTalkWnd(pic);
	RECT rect;
	::GetWindowRect(talkWnd, &rect);
	POINT point;
	::GetCursorPos(&point);

	if ((point.x < (rect.left + x)) || (point.x > (rect.left + x2)))
		return true;
	if ((point.y < (rect.top + y)) || (point.y > (rect.top + y2)))
		return true;

	if ((point.x > (rect.left + x)) && (point.x < (rect.left + x2))) {
		if (point.x < (rect.left + x + 100)) {
			//LOGVARN2(64, "green", L"point.x < (rect.left + x + 100):%d-%d", point.x + 5, point.x + 15);
			click_x = MyRand(point.x - rect.left + 2, point.x - rect.left + 5);
		}	
		else {
			//LOGVARN2(64, "green", L"point.x >= (rect.left + x + 100):%d-%d", point.x - 5, point.x - 15);
			click_x = MyRand(point.x - rect.left - 2, point.x - rect.left - 5);
		}	
	}

	return false;
}

