#include "Game.h"
#include "Pet.h"
#include "Item.h"
#include "PrintScreen.h"
#include "GameProc.h"
#include <My/Common/C.h>

Pet::Pet(Game* p)
{
	m_pGame = p;
}

// �������
bool Pet::PetOut(int no)
{
	// no=-1 ����ǰ����
	if (no == -1 || no == 1) {
		m_pGame->m_pGameProc->Click(60, 166, 100, 180); // ��һ������ͷ��
		Sleep(1000);
		m_pGame->m_pGameProc->Click(60, 330, 100, 350); // ����
	}
	
	if (no == -1) Sleep(1000);
	if (no == -1 || no == 2) {
		m_pGame->m_pGameProc->Click(152, 166, 200, 180); // �ڶ�������ͷ��
		Sleep(1000);
		m_pGame->m_pGameProc->Click(152, 330, 200, 350); //����
	}
	
	if (no == -1) Sleep(1000);
	if (no == -1 || no == 3) {
		m_pGame->m_pGameProc->Click(250, 166, 300, 180); // ����������ͷ��
		Sleep(1000);
		m_pGame->m_pGameProc->Click(250, 330, 300, 350); // ����
	}
	

	return true;
}

// �������
bool Pet::PetFuck(DWORD* nums, DWORD length)
{
	return true;
}

// ��������û��Ѫ������
bool Pet::Revive()
{
	bool result = false;
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_hWndBig, 62, 162, 305, 168, 0, true);
	if (!result) { // ��һ������ 62-97
		result = m_pGame->m_pPrintScreen->GetGrayPiexlCount(0, 35, false) > 100;
	}
	if (!result) { // �ڶ������� 152-187
		result = m_pGame->m_pPrintScreen->GetGrayPiexlCount(90, 125, false) > 100;
	}
	if (!result) { // ���������� 252-287
		result = m_pGame->m_pPrintScreen->GetGrayPiexlCount(190, 225, false) > 100;
	}

	if (result) {
		result = m_pGame->m_pPrintScreen->IsOpenglPs() ? true : m_pGame->m_pGameProc->IsForegroundWindow();
		if (result) {
			m_pGame->m_pItem->UseLingYao();
		}
	}
	return result;
}