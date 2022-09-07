#pragma once
#include <Windows.h>

struct _account_;
class Game;
class Talk
{
public:
	Talk(Game* p);

	// 点击第几个好友
	bool ClickFaceBg(int no);
	// 当有多个NPC在一起时, 会有NPC列表选择
	bool IsNeedCheckNPC();
	// 选择NPC
	void SelectNPC(int no);
	// 点击NPC
	void ClickNPC(_account_* account, int mv_x, int mv_y, int clk_x, int clk_y, int clk_x2=0, int clk_y2=0);
	// NPC
	DWORD NPC(const char* name=nullptr);
	// NPC对话选择项
	void Select(DWORD no, bool show_log=true);
	// NPC对话选择项
	void Select(const char* name, HWND pic, bool show_log=true);
	// NPC对话状态[对话框是否打开]
	bool NPCTalkStatus(HWND pic);
	// 等待商店打开
	bool WaitShopOpen(DWORD ms = 0);
	// 等待对话框打开
	bool WaitTalkOpen(HWND pic, DWORD ms=0);
	// 等待对话框关闭
	bool WaitTalkClose(HWND pic, DWORD ms = 0);
	// 对话按钮是否已打开, 靠近NPC会出现对话按钮
	bool TalkBtnIsOpen();
	// 获取对话选项坐标(返回鼠标是否需要移动)
	bool GetSelectClickPos(const char* name, int& click_x, int& click_y, HWND pic);
public:
	// Game类
	Game* m_pGame;
};