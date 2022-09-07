#pragma once
#include <Windows.h>

struct _account_;
class Game;
class Talk
{
public:
	Talk(Game* p);

	// ����ڼ�������
	bool ClickFaceBg(int no);
	// ���ж��NPC��һ��ʱ, ����NPC�б�ѡ��
	bool IsNeedCheckNPC();
	// ѡ��NPC
	void SelectNPC(int no);
	// ���NPC
	void ClickNPC(_account_* account, int mv_x, int mv_y, int clk_x, int clk_y, int clk_x2=0, int clk_y2=0);
	// NPC
	DWORD NPC(const char* name=nullptr);
	// NPC�Ի�ѡ����
	void Select(DWORD no, bool show_log=true);
	// NPC�Ի�ѡ����
	void Select(const char* name, HWND pic, bool show_log=true);
	// NPC�Ի�״̬[�Ի����Ƿ��]
	bool NPCTalkStatus(HWND pic);
	// �ȴ��̵��
	bool WaitShopOpen(DWORD ms = 0);
	// �ȴ��Ի����
	bool WaitTalkOpen(HWND pic, DWORD ms=0);
	// �ȴ��Ի���ر�
	bool WaitTalkClose(HWND pic, DWORD ms = 0);
	// �Ի���ť�Ƿ��Ѵ�, ����NPC����ֶԻ���ť
	bool TalkBtnIsOpen();
	// ��ȡ�Ի�ѡ������(��������Ƿ���Ҫ�ƶ�)
	bool GetSelectClickPos(const char* name, int& click_x, int& click_y, HWND pic);
public:
	// Game��
	Game* m_pGame;
};