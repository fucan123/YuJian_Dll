#include "Game.h"
#include "GameConf.h"
#include "Pet.h"
#include "Item.h"
#include "Button.h"
#include "PrintScreen.h"
#include "GameProc.h"
#include <My/Common/C.h>

Pet::Pet(Game* p)
{
	m_pGame = p;
}

// 宠物出征
bool Pet::PetOut(DWORD nos[], DWORD length)
{
	// ::printf("出征宝宝\n.");
	COLORREF color = 0xFF310000;
	int count = 2;
	HWND hWndGame = m_pGame->m_pGameProc->m_pAccount->Wnd.Game;
	HWND hWndPic = m_pGame->m_pGameProc->m_pAccount->Wnd.Pic;
	// m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWndGame, 890, 70, 900, 80, 0, true);
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWndGame, 820, 70, 830, 80, 0, true);
	int c = m_pGame->m_pPrintScreen->GetPixelCount(color);
	// ::printf("颜色: %d.\n", c);
	if (c < count) // 如果第三个没有出征按钮, 证明全部出征
		return true;

	// 70
	// 541,70  680,70 820,70 960,15 出征/召回
	// 585,70  725,70 855,70 0,0    合体
	// no=-1 出征前三个
	for (int i = 0; i < length; i++) {
		if (nos[i] == 1) {
			// m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWndGame, 610, 70, 620, 80, 0, true);
			m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWndGame, 540, 70, 550, 80, 0, true);
			if (m_pGame->m_pPrintScreen->GetPixelCount(color) >= count) { // 有出征按钮
				m_pGame->m_pButton->ClickPic(hWndGame, hWndPic, MyRand(540, 550), MyRand(70, 80));
				Sleep(1000);
			}
		}
		else  if (nos[i] == 2) {
			// m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWndGame, 750, 70, 760, 80, 0, true);
			m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWndGame, 680, 70, 690, 80, 0, true);
			if (m_pGame->m_pPrintScreen->GetPixelCount(color) >= count) { // 有出征按钮
				m_pGame->m_pButton->ClickPic(hWndGame, hWndPic, MyRand(680, 690), MyRand(70, 75));
				Sleep(1000);
			}
		}
		else if (nos[i] == 3) {
			m_pGame->m_pButton->ClickPic(hWndGame, hWndPic, MyRand(820, 830), MyRand(70, 75));
			Sleep(1000);
		}
		else if (nos[i] == 4) {
			m_pGame->m_pButton->ClickPic(hWndGame, hWndPic, MyRand(960, 970), MyRand(70, 75));
			Sleep(1000);
		}
	}
	
	return true;
}

// 宠物合体
bool Pet::PetFuck(int no)
{
	// 541,70  680,70 820,70 960,15 出征/召回
	// 585,70  725,70 855,70 0,0    合体
	// no=-1 出征前三个

	COLORREF color = 0xFF4A499C;
	int count = 6;
	HWND hWndGame = m_pGame->m_pGameProc->m_pAccount->Wnd.Game;
	HWND hWndPic = m_pGame->m_pGameProc->m_pAccount->Wnd.Pic;

	if (no == -1 || no == 1) {
		// m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWndGame, 610, 70, 620, 80, 0, true);
		m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWndGame, 540, 70, 550, 80, 0, true);
		if (m_pGame->m_pPrintScreen->GetPixelCount(color) >= count) { // 有召回按钮
			m_pGame->m_pButton->ClickPic(hWndGame, hWndPic, MyRand(580, 590), MyRand(70, 75));
			Sleep(1000);
		}
	}
	if (no == -1 || no == 2) {
		// m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWndGame, 750, 70, 760, 80, 0, true);
		m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWndGame, 680, 70, 690, 80, 0, true);
		if (m_pGame->m_pPrintScreen->GetPixelCount(color) >= count) { // 有召回按钮
			m_pGame->m_pButton->ClickPic(hWndGame, hWndPic, MyRand(720, 730), MyRand(70, 75));
			Sleep(1000);
		}
	}
	if (no == -1 || no == 3) {
		// m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWndGame, 885, 70, 900, 80, 0, true);
		m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWndGame, 815, 70, 830, 80, 0, true);
		if (m_pGame->m_pPrintScreen->GetPixelCount(color) >= count) { // 有召回按钮
			m_pGame->m_pButton->ClickPic(hWndGame, hWndPic, MyRand(860, 870), MyRand(70, 75));
			Sleep(1000);
		}
	}

	return true;
}

// 复活所有没有血量宠物
bool Pet::Revive()
{
	// 464,74
	// 935,90  1075,90 1215,90
	// 955,105 1095,90 1235,90
	// 470,15  610,15 750,15
	// 490,30  630,30 770,30
	bool result = false;
	HWND hWnd = m_pGame->m_pGameProc->m_pAccount->Wnd.Game;
	// m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWnd, 540, 20, 560, 35, 0, true);
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWnd, 470, 20, 490, 35, 0, true);
	int count = m_pGame->m_pPrintScreen->GetGrayPiexlCount();
	::printf("宝宝: %d.\n", count);
	result = count > 100;
	if (result)
		goto _end_;

	// m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWnd, 675, 20, 695, 35, 0, true);
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWnd, 610, 20, 630, 35, 0, true);
	count = m_pGame->m_pPrintScreen->GetGrayPiexlCount();
	::printf("宝宝: %d.\n", count);
	result = count > 100;
	if (result)
		goto _end_;

	// m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWnd, 815, 20, 835, 35, 0, true);
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(hWnd, 750, 20, 770, 35, 0, true);
	count = m_pGame->m_pPrintScreen->GetGrayPiexlCount();
	::printf("宝宝: %d.\n", count);
	result = count > 100;

_end_:
	::printf("结果: %d.\n", result);
	if (result) {
		Sleep(600);
		m_pGame->m_pItem->UseLingYao();
		Sleep(1600);
		// m_pGame->m_pPet->PetOut(m_pGame->m_pGameConf->m_stPetOut.No, m_pGame->m_pGameConf->m_stPetOut.Length);
		// Sleep(1000);
		PetFuck(-1);
	}
	return result;
}

// 解析宠物血量 flag=0当前 flag=1最大
int Pet::ParsetLife(const char* text, int flag)
{
	if (flag == 1) {
		while (*text) {
			if (*text == '/') {
				text++;
				break;
			}

			text++;
		}
		if (*text == 0)
			return 0;
	}

	int v = 0;
	while (*text) {
		if (*text < '0' || *text > '9')
			break;

		v = v * 10 + (*text - '0');
		text++;
	}

	return v;
}
