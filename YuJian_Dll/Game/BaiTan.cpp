#include "Game.h"
#include "BaiTan.h"
#include "GameConf.h"
#include "Item.h"
#include "Button.h"
#include <stdlib.h>

// ...
BaiTan::BaiTan(Game* p)
{
	m_pGame = p;
}

// 运行
void BaiTan::Start()
{
	LOG2(L"摆摊...", "green b");
	wchar_t log[64];
	char text[255];
	while (true) {
		for (int i = 0; i < m_pGame->m_AccountList.size(); i++) {
			Account* p = m_pGame->m_AccountList[i];
			if (!p->Wnd.Game)
				continue;

			char name[255];
			HWND hWndPage = m_pGame->m_pButton->FindButtonWnd(p->Wnd.Game, 0x7EC);
			if (hWndPage) {
				::ZeroMemory(text, sizeof(text));
				::GetWindowTextA(hWndPage, text, sizeof(text));
				::printf("%s\n", text);
				if (::strcmp(text, "1/1") == 0) {
					HWND hWndRole = m_pGame->m_pButton->FindButtonWnd(p->Wnd.Game, STATIC_ID_ROLE);
					::GetWindowTextA(hWndRole, text, sizeof(text));
					UpItem(p);
				}
			}
		}

		Sleep(9999);
	}
}

// 上架
void BaiTan::UpItem(_account_* account)
{
	::SetForegroundWindow(account->Wnd.Game);
	Sleep(100);

	m_pGame->m_pItem->OpenBag(account->Wnd.Game);
	Sleep(1500);

	int count = 0;
	for (int i = 0; i < 1; i++) {
		if (m_pGame->m_pItem->SlideBag(i)) // 滑动背包
			Sleep(1000);

		ITEM_TYPE bag_items[40];
		ConfItemInfo* items = m_pGame->m_pGameConf->m_stBaiTan.BaiTans;
		int length = m_pGame->m_pGameConf->m_stBaiTan.Length;
		int idx = 0;
		for (idx = 0; idx < length; idx++) {
			if (!items[idx].Extra[0])
				continue;

			while (true) {
				::SetForegroundWindow(account->Wnd.Game);
				Sleep(100);

				bool has = false;
				m_pGame->m_pItem->ReadBagItem(bag_items, sizeof(bag_items), account);

				for (int n = 0; n < sizeof(bag_items) / sizeof(ITEM_TYPE); n++) {
					if (bag_items[n] && bag_items[n] == items[idx].Type) {
						int click_x, click_y;
						m_pGame->m_pItem->GetBagClickPos(n, click_x, click_y);
						m_pGame->m_pButton->Click(account->Wnd.Game,
							BUTTON_ID_BAG_ITEM, "MPC物品栏", click_x, click_y, 0xff, false);

						Sleep(1000);
						m_pGame->m_pButton->ClickPic(account->Wnd.Game, account->Wnd.Pic,
							675, 503, 800);
						char money[16] = { 0 };
						sprintf_s(money, "%d", items[idx].Extra[0]);
						for (int i = 0; money[i]; i++) {
							m_pGame->m_pButton->Key(money[i]);
							Sleep(260);
						}
						Sleep(800);
						m_pGame->m_pButton->Click(account->Wnd.Game, 0x88D, "确定");
						Sleep(1000);

						LOGVARN2(64, "red b", L"%hs: 上架[%hs] 价格[%hs].", account->Name, items[idx].Name, money);

						count++;
						has = true;
						break;
					}
				}

				if (count >= 18)
					break;
				if (!has)
					break;
			}

			if (count >= 18)
				break;
		}
	}
}
