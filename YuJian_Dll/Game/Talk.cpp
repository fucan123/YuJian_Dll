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

// ���ж��NPC��һ��ʱ, ����NPC�б�ѡ��
bool Talk::IsNeedCheckNPC()
{
	// ���NPC
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 503, 40, 513, 50, 0, true);
	return m_pGame->m_pPrintScreen->ComparePixel("���NPC", nullptr, 1) > 0;
}

// ѡ��NPC
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
	// �Ի���ť����[875,340 912,388]
	//m_pGame->m_pEmulator->Tap(MyRand(875, 912), MyRand(350, 376));
	m_pGame->m_pGameProc->Click(875, 350, 912, 376);
	return 0;
}

// NPC�Ի�ѡ����
void Talk::Select(DWORD no, bool show_log)
{
	if (show_log)
		LOGVARN2(32, "c6", L"ѡ��:0x%02d", no);

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

// NPC�Ի�ѡ����
void Talk::Select(const char* name, HWND pic, bool show_log)
{
	int x, y;
	bool moumov = GetSelectClickPos(name, x, y, pic);

	DbgPrint("ѡ��:%hs(%d,%d)\n", name, x, y);
	if (show_log)
		LOGVARN2(32, "c6", L"ѡ��:%hs(%d,%d)", name, x, y);

	m_pGame->m_pButton->ClickTalk(pic, x, y, moumov);
}

// NPC�Ի�״̬[�Ի����Ƿ��]
bool Talk::NPCTalkStatus(HWND pic)
{
	return !m_pGame->m_pButton->IsDisabled(pic);
}

// �ȴ��Ի����
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

// �ȴ��Ի����
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

// �Ի���ť�Ƿ��Ѵ�, ����NPC����ֶԻ���ť
bool Talk::TalkBtnIsOpen()
{
	// ��ȡ���ǶԻ���ť��������
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 886, 355, 896, 365, 0, true);
	return m_pGame->m_pPrintScreen->ComparePixel("�Ի���ť", nullptr, 1) > 0;
}

// ��ȡ�Ի�ѡ������(��������Ƿ���Ҫ�ƶ�)
bool Talk::GetSelectClickPos(const char* name, int& click_x, int& click_y, HWND pic)
{
	int x = 50, y = 0;
	int x2 = 286, y2 = 0;

	if (strcmp(name, "�ر�ħ���ϱ�") == 0) {
		y = 170, y2 = 180;
	}
	if (strcmp(name, "�⿪����ʿ��̳") == 0) {
		y = 170, y2 = 180;
	}
	else if (strcmp(name, "�����֮����������") == 0) {
		y = 186, y2 = 196;
	}
	else if (strcmp(name, "���Թ֮����������") == 0) {
		y = 186, y2 = 196;
	}
	else if (strcmp(name, "�⿪Ů������̳��ӡ") == 0) {
		y = 186, y2 = 196;
	}
	else if (strcmp(name, "�׼�1000������(��һ��)") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "�׼�1000������(�ڶ���)") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "�׼�2000������(�ڶ���)") == 0) {
		y = 222, y2 = 230;
	}
	else if (strcmp(name, "�׼�500�����(�ڶ���)") == 0) {
		y = 255, y2 = 260;
	}
	else if (strcmp(name, "�׼�1000������(������)") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "�׼�3000������(������)") == 0) {
		y = 222, y2 = 230;
	}
	else if (strcmp(name, "�׼�500�����(������)") == 0) {
		y = 255, y2 = 260;
	}
	else if (strcmp(name, "ȷ���ͷ�̰��֮��") == 0) {
		y = 158, y2 = 163;
	}
	else if (strcmp(name, "ȷ���ͷ�Թ��֮��") == 0) {
		y = 158, y2 = 163;
	}
	else if (strcmp(name, "ȷ���ͷ��Ի�֮��") == 0) {
		y = 158, y2 = 163;
	}
	else if (strcmp(name, "ȷ���ͷ��밮֮��") == 0) {
		y = 158, y2 = 163;
	}
	else if (strcmp(name, "�⿪������̳��ӡ") == 0) {
		y = 170, y2 = 180;
	}
	else if (strcmp(name, "�رջ�Ԫ���ϱ�") == 0) {
		y = 170, y2 = 180;
	}
	else if (strcmp(name, "��Ԫ���ϱ�.ȷ��") == 0) {
		y = 126, y2 = 135;
	}
	else if (strcmp(name, "�رձ�Ԫ���ϱ�") == 0) {
		y = 170, y2 = 180;
	}
	else if (strcmp(name, "��Ԫ���ϱ�.ȷ��") == 0) {
		y = 126, y2 = 135;
	}
	else if (strcmp(name, "�ر���Ԫ���ϱ�") == 0) {
		y = 158, y2 = 165;
	}
	else if (strcmp(name, "��Ԫ���ϱ�.ȷ��") == 0) {
		y = 126, y2 = 135;
	}
	else if (strcmp(name, "����Ѫ����ʽ") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "������ȵķ�ӡ") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "�����������ϱ�") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "��������ʼ����") == 0) {
		y = 170, y2 = 180;
	}
	else if (strcmp(name, "��ȡ����֮��") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "�ʶ�����") == 0) {
		y = 186, y2 = 195;
	}
	else if (strcmp(name, "����������") == 0) {
		y = 170, y2 = 180;
	}
	else if (strcmp(name, "�뿪�����Ǳ�") == 0) {
		y = 126, y2 = 135;
	}
	else if (strcmp(name, "��Ҫ�뿪") == 0) {
		y = 126, y2 = 135;
	}
	else if (strcmp(name, "����뿪.ȷ��") == 0) {
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
