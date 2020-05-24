#include "Game.h"
#include "GameConf.h"
#include "Item.h"
#include "Talk.h"
#include "Button.h"
#include "PrintScreen.h"
#include "GameProc.h"
#include <My/Common/C.h>

Item::Item(Game* p)
{
	m_pGame = p;
}

// �򿪱���
bool Item::OpenBag()
{
	DbgPrint("�򿪱���\n");
	LOG2(L"�򿪱���", "c0");
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_BAG);
	for (int i = 0; i < 2000; i += 100) {
		Sleep(100);
		if (m_pGame->m_pButton->CheckButton(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, 
			BUTTON_ID_BAG_ITEM, "MPC��Ʒ��")) {
			Sleep(800);
			return true;
		}
	}
	return false;
}

// �رձ���
void Item::CloseBag()
{
	DbgPrint("�رձ���\n");
	LOG2(L"�رձ���", "orange b");
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_CLOSEBAG);
}

// ������
void Item::SetBag()
{
	DbgPrint("������\n");
	LOG2(L"������", "c0");
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_BAG_SET, "��Ʒ��-����ť");
}

// ��������
bool Item::SlideBag(int page)
{
	if (page <= 0)
		return false;

	LOGVARN2(32, "blue_r b", L"������ҳ(%d)", page);
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_BAG_PGDW, "down");
	return true;
}

// �򿪲ֿ�
void Item::OpenStorage()
{
	DbgPrint("�򿪲ֿ�\n");
	LOG2(L"�򿪲ֿ�", "c0");
	
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_VIP);
	for (int i = 0; i < 5; i++) {
		Sleep(800);
		if (m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_CHECKIN)) {
			Sleep(800);
		}
	}
}

// �����ֿ�
bool Item::SlideStorge(int page)
{
	if (page <= 0)
		return false;

	LOGVARN2(32, "blue_r b", L"�ֿⷭҳ(%d)", page);
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_CKIN_PGDW, "D");
	return true;
}

// �رղֿ�
void Item::CloseStorage()
{
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_CLOSECKIN);
}

// ����Ѫ
void Item::AddFullLife()
{
	for (int i = 0; i < 6; i++) {
		m_pGame->m_pGameData->ReadLife();
		if ((m_pGame->m_pGameData->m_dwLife + 1000) >= m_pGame->m_pGameData->m_dwLifeMax)
			return;

		UseYao(1);
	}
	
}

// ʹ��ҩ
void Item::UseYao(int num, bool use_yaobao, int sleep_ms)
{
	for (int i = 0; i < num; i++) {
		if (use_yaobao && IsNeedUseYaoBao()) {
			if (!UseYaoBao())
				break;
		}

		m_pGame->m_pButton->Key('1'); // 1��
		Sleep(sleep_ms);
	}
}

// ʹ��ҩ����
bool Item::UseYaoBao()
{
	int yaobao = 0, yao = 0;
	GetQuickYaoOrBaoNum(yaobao, yao);
	if (yaobao > 0) {
		m_pGame->m_pButton->Key('2'); // 2��
		Sleep(300);
	}
	
	return yaobao > 0;
}

// ȥ�����ſ�
void Item::GoFBDoor()
{
	m_pGame->m_pButton->Key('8'); // 9��
}

// ȥ�����ſ�
void Item::GoShop()
{
	m_pGame->m_pButton->Key('9'); // 8��
}

// �ر��̵�
void Item::CloseShop()
{
	DbgPrint("�ر��̵�");
	LOG2(L"�ر��̵�", "c0");
	
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_CLOSESHOP);
}

// ʹ�ø��������ҩ
void Item::UseLingYao()
{
	DbgPrint("ʹ����Чʥ����ҩ\n");
	LOG2(L"ʹ����Чʥ����ҩ", "green");

	m_pGame->m_pButton->Key('3'); // 3��
	Sleep(300);
}

// �Ƿ���Ҫʹ��ҩ��
bool Item::IsNeedUseYaoBao()
{
	int yao, yaobao;
	GetQuickYaoOrBaoNum(yaobao, yao);
	if (yao <= 1 && yaobao > 1)
		return true;

	return false;
}

// �����Ƿ��
bool Item::BagIsOpen()
{
	return m_pGame->m_pButton->CheckButton(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_BAG_ITEM, "MPC��Ʒ��");
}

// ��Ʒ������ť�Ƿ��
bool Item::ItemBtnIsOpen(int index)
{
	// ��ȡ��Ʒ������ť 
	if (1 || index == 0)      // ��һ��
		m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 145, 85, 155, 95, 0, true);
	else if (index == 1) // �ڶ���
		m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 145, 145, 155, 175, 0, true);
	else                 // ������
		m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 145, 215, 155, 245, 0, true);

	return m_pGame->m_pPrintScreen->ComparePixel("��Ʒ��ť", nullptr, 1) > 0;
}

// �ȴ���Ʒ������ť����
bool Item::WaitForItemBtnOpen()
{
	for (int i = 0; i < 1000; i += 100) {
		if (ItemBtnIsOpen())
			return true;

		Sleep(100);
	}
	return false;
	//375,260 820,460 ��һ��BOSS����ɨ���Ƿ��б���ҩ
}

// ��ֲ�����ť�Ƿ����
bool Item::ChaiFeiBtnIsOpen()
{
	// ��ȡ��ֿ�ȡ����ť
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 466, 505, 476, 515, 0, true);
	return m_pGame->m_pPrintScreen->ComparePixel("��ֿ�", nullptr, 1) > 0;
}

// �رղ�ֿ�
void Item::CloseChaiFeiBox()
{
	m_pGame->m_pGameProc->Click(466, 505, 566, 515);
}

// �����ť����
bool Item::PickUpItemByBtn()
{
	LOG2(L"�����ťʰȡ", "c0 b");
	//m_pGame->m_pEmulator->Tap(MyRand(26, 35), MyRand(656, 675));
	m_pGame->m_pGameProc->Click(26, 656, 35, 675);
	return WaitForPickUpItem();
}

// �ֿ��Ǯ��ť�Ƿ��
bool Item::SaveMoneyBtnIsOpen()
{
	// ��ȡ��Ǯȷ����ť
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 665, 465, 675, 475, 0, true);
	return m_pGame->m_pPrintScreen->ComparePixel("�ֿ��Ǯȷ����ť", nullptr, 1) > 0;
}

// �ֿ������λ��ť�Ƿ��
bool Item::StorageAddBtnIsOpen()
{
	// �ֿ���λ����ȡ����ť
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 500, 380, 510, 390, 0, true);
	return m_pGame->m_pPrintScreen->ComparePixel("�ֿ���λ����ȡ����ť", nullptr, 1) > 0;
}

// ����
int Item::PickUpItem(const char* name, int x, int y, int x2, int y2, int pickup_num)
{
	int count = 0;
	for (int i = 1; i <= pickup_num; i++) {
		char pick_name[64] = { 0 };
		int pos_x = 0, pos_y = 0;
		if (!GetGroundItemPos(name, x, y, x2, y2, pos_x, pos_y, pick_name))
			break;

		DbgPrint("%d.����:%s(%d,%d)\n", i, pick_name, pos_x, pos_y);
		LOGVARN2(64, "c0", L"%d.����:%hs(%d,%d)", i, pick_name, pos_x, pos_y);
		m_pGame->m_pGameProc->Click(pos_x, pos_y);
		WaitForPickUpItem();
		count++;

		Sleep(260);
	}

	return count;
}

// �ȴ�������Ʒ
bool Item::WaitForPickUpItem(DWORD wait_ms)
{
	DWORD pos_x = 0, pos_y = 0;
	m_pGame->m_pGameData->ReadCoor(&pos_x, &pos_y, m_pGame->m_pGameProc->m_pAccount);

	DWORD x = 0, y = 0;
	m_pGame->m_pGameData->ReadCoor(&x, &y, m_pGame->m_pGameProc->m_pAccount);
	for (DWORD i = 0; i <= wait_ms; i += 620) {
		Sleep(620);

		DWORD now_x = 0, now_y = 0;
		m_pGame->m_pGameData->ReadCoor(&now_x, &now_y, m_pGame->m_pGameProc->m_pAccount);
		if (x == now_x && y == now_y)
			return now_x != pos_x && now_y != pos_y;

		x = now_x;
		y = now_y;
	}
	return false;
}

// ��ȡ������Ʒ����
int Item::GetGroundItemPos(const char* name, int x, int y, int x2, int y2, int& pos_x, int& pos_y, char* pick_name)
{
	if (!x || !y || !x2 || !y2) {
		x = 360;
		y = 150;
		x2 = 880;
		y2 = 566;
	}

	wchar_t log[128];
	//LOGVARP2(log, "orange b", L"��������:%d (%d,%d)-(%d,%d)", 0, x, y, x2, y2);
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, x, y, x2, y2, 0, true);

	ConfItemInfo* items = m_pGame->m_pGameConf->m_stPickUp.PickUps; // ���ü�����Ʒ
	int length = m_pGame->m_pGameConf->m_stPickUp.Length; // ������Ʒ����
	
	for (int i = 0; i < length; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			return 0;

		if (!strstr(name, ".") && strcmp(name, items[i].Name) != 0)
			continue;
		if (strcmp(name, "...") == 0 && strcmp("30��������Ƭ+3", items[i].Name) == 0)
			continue;
		if (strcmp(name, ".") == 0 || strcmp(name, ".nowait") == 0) { // ���һ��
			if (strcmp("30��������Ƭ+2", items[i].Name) != 0 && strcmp("30��������Ƭ+3", items[i].Name) != 0)
				continue;
		}
		if (strcmp(name, "��Ч���ư�") == 0) {
#if 0
			int yaobao = 0, yao = 0;
			GetQuickYaoOrBaoNum(yaobao, yao);
			if (yaobao >= 7) {
				LOGVARP2(log, "blue_r b", L"�����ټ�ҩ��, ����:%d\n", yaobao);
				return 0;
			}	
#endif
		}

		strcpy(pick_name, items[i].Name);
		if (0 && strcmp(name, "��Ч���ư�") == 0 && !strstr(name, "."))
			LOGVARP2(log, "orange b", L"ʶ����Ʒ:%hs(%hs)", pick_name, items[i].Name);

		DWORD color = 0x00000000, diff = 0x00000000;
		DWORD color2 = 0x00000000, diff2 = 0x00000000;
		int tmp_x = 0, tmp_y = 0, tmp_x2 = 0, tmp_y2 = 0;
		if (strcmp("��Чʥ����ҩ", pick_name) == 0) {
			color = 0xFF72AA23;
			diff = 0x000B0B0B;
		}
		else if (strcmp("6��O���", pick_name) == 0) {
			//color = 0xFFBDA392;
			diff = 0x00080808;
		}
		else if (strcmp("12��XO���", pick_name) == 0) {
			//color = 0xFF5AB6DF;
			diff = 0x00080808;
		}
		else if (strcmp("25��XO���", pick_name) == 0) {
			color = 0xFF667EBE;
			diff = 0x00010101;
		}
		else if (strcmp("30��������Ƭ+1", pick_name) == 0) {
			color = 0xFFB6936B;
			diff = 0x00010101;
		}
		else if (strcmp("30��������Ƭ+2", pick_name) == 0) {
			color = 0xFF6278B2;
			diff = 0x00030303;
		}
		else if (strcmp("30��������Ƭ+3", pick_name) == 0) {
			color = 0xFFB885CA;
			diff = 0x00080808;
		}
		else if (strcmp("��ħ��ʯ", pick_name) == 0) {
			color = 0xFFE352ED;
			diff = 0x00020202;
		}
		else if (strcmp("ħ�꾧ʯ", pick_name) == 0) {
			color = 0xFFF5E880;
			diff = 0x00020202;
		}
		else if (strcmp("��꾧ʯ", pick_name) == 0) {
			color = 0xFFF7C68E;
			diff = 0x00030303;
		}
		else if (strcmp("��Ч���ư�", pick_name) == 0) {
			if (!strstr(name, ".")) {
				color = 0xFFF66A6D;
				diff = 0x00151515;

				//color2 = 0xFFFA8ADE;
				//diff2 = 0x00202020;
			}
			else {
				color = 0xFFF66A6D;
				diff = 0x00030303;
			}
		}
		if (color == 0x00000000)
			continue;

		m_pGame->m_pPrintScreen->GetPixelPos(color, tmp_x, tmp_y, diff);
		if (color2) {
			//m_pGame->m_pPrintScreen->GetPixelPos(color2, tmp_x2, tmp_y2, diff2);
		}
		if ((tmp_x && tmp_y) || (tmp_x2 && tmp_y2)) {
			//LOGVARP2(log, "c0", L"������Ʒ:%hs, ����:%d,%d ����2:%d,%d", name, tmp_x, tmp_y, tmp_x2, tmp_y2);
			if (tmp_x) {
				pos_x = tmp_x;
				pos_y = tmp_y;
			}
			else {
				pos_x = tmp_x2;
				pos_y = tmp_y2;
			}

			return 1;
		}
	}
	//DbgPrint("�����ͼ:%s(%d,%d %d,%d)\n", name, x, y, x2, y2);
	
	return 0;
}

// ����ҩ��
int Item::DropItem(ComImgIndex index, int live_count, DWORD* ms)
{
	DWORD _tm = GetTickCount();

	if (!OpenBag())
		Sleep(500);

	Sleep(300);

	bool is_use_item = false;
	int count = 0; // ����
	for (int i = 0; i <= 3; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;

		if (!BagIsOpen()) {
			LOGVARN2(64, "red", L"����û�д�, Wait Bag Open!");
			Sleep(100);
			if (!BagIsOpen()) {
				LOGVARN2(32, "red", L"����û�д�");
				if (i > 1) {
					break;
				}
				else {
					Sleep(100);
					continue;
				}
			}
		}

		if (SlideBag(i)) { // ��������
			Sleep(200);
		}

		ConfItemInfo* items = m_pGame->m_pGameConf->m_stDrop.Drops;
		int length = m_pGame->m_pGameConf->m_stDrop.Length;
		int idx = 0;
		for (idx = 0; idx < length; idx++) {
			if (i > 0 && strcmp("��ɫף����Ƭ", items[idx].Name) == 0)
				continue;

			ComPoint cp[32];
			PrintBagImg(true); // ��ȡ����ͼƬ
			int num = m_pGame->m_pPrintScreen->ComparePixel(items[idx].Name, cp, sizeof(cp) / sizeof(ComPoint));
			if (num == 0)
				continue;
			if (num < 3 && strcmp(items[idx].Name, "Կ��") == 0) // Կ��ֻ��һ�Ѳ��ܶ�
				continue;
			if (num <= 6 && strcmp(items[idx].Name, "��Ч���ư�") == 0)
				continue;
			//m_pGame->m_pPrintScreen->SaveBitmapToFile(bitmap, L"C:\\Users\\fucan\\Desktop\\MNQ-9Star\\jt.bmp");
			//DbgPrint("%d.��.%s.������Ʒ����:%d\n", i + 1, drops[idx], num);

			int drop_num = 0;

			if (count >= live_count) { // ��Ҫȫ����
				drop_num = num;
			}
			else {
				drop_num = count + num - live_count;
			}
			drop_num = strcmp(items[idx].Name, "Կ��") != 0 ? num : 1; // Կ��ֻ����ǰ����һ��
			if (strcmp(items[idx].Name, "��Ч���ư�") == 0)
				drop_num = num - 6;

			int btn_index = 1;
			if (strcmp("Կ��", items[idx].Name) == 0 || strcmp("������ʥˮ", items[idx].Name) == 0) // ֻ��һ��������ť
				btn_index = 0;

			for (int j = 0; j < drop_num; j++) {
				if (j == 0) {
					DropItem(items[idx].Name, cp[0].x, cp[0].y, btn_index);
				}
				else {
					ComPoint item[1];
					PrintBagImg(true); // ��ȡ����ͼƬ
					if (!m_pGame->m_pPrintScreen->ComparePixel(items[idx].Name, item, sizeof(item) / sizeof(ComPoint)))
						break;

						DropItem(items[idx].Name, item[0].x, item[0].y, btn_index);
				}
				Sleep(600);

				if (ChaiFeiBtnIsOpen()) {
					CloseChaiFeiBox();
					Sleep(300);
				}
			}
			count += num;

			m_pGame->m_pGameProc->CloseTipBox();
		}

		if (1 || !is_use_item) {
			items = m_pGame->m_pGameConf->m_stUse.Uses;
			length = m_pGame->m_pGameConf->m_stUse.Length;
			for (idx = 0; idx < length; idx++) {
				ComPoint cp[32];
				PrintBagImg(true); // ��ȡ����ͼƬ
				int num = m_pGame->m_pPrintScreen->ComparePixel(items[idx].Name, cp, sizeof(cp) / sizeof(ComPoint));
				if (num == 0)
					continue;

				is_use_item = true;
				for (int j = 0; j < num; j++) {
					if (j == 0) {
						UseItem(items[idx].Name, cp[0].x, cp[0].y);
					}
					else {
						ComPoint item[1];
						PrintBagImg(true); // ��ȡ����ͼƬ
						if (!m_pGame->m_pPrintScreen->ComparePixel(items[idx].Name, item, sizeof(item) / sizeof(ComPoint)))
							break;

						UseItem(items[idx].Name, item[0].x, item[0].y);
					}
					Sleep(600);

					m_pGame->m_pGameProc->CloseTipBox();
					if (ChaiFeiBtnIsOpen()) {
						CloseChaiFeiBox();
						Sleep(300);
					}
				}
				count += num;

				m_pGame->m_pGameProc->CloseTipBox();
			}
		}
		
		if (i > 1) {
			if (m_pGame->m_pPrintScreen->ComparePixel("û����Ʒ", nullptr, 1))
				break;
			if (m_pGame->m_pPrintScreen->ComparePixel("��������", nullptr, 1))
				break;
		}
	}

	if (ChaiFeiBtnIsOpen()) {
		CloseChaiFeiBox();
		Sleep(300);
	}

	Sleep(200);
	CloseBag();
	Sleep(100);

	_tm = GetTickCount() - _tm;
	if (ms) {
		*ms = _tm;
	}
	
	DbgPrint("������Ʒ��ʱ:%.2f��, %d����\n", (float)_tm/1000.0f, _tm);
	LOGVARN2(64, "c0", L"������Ʒ��ʱ:%d��, %d����", _tm / 1000, _tm);
	return count;
}

// ʹ����Ʒ
void Item::UseItem(const char* name, int x, int y)
{
	DbgPrint("ʹ����Ʒ:%s(%d,%d)\n", name, x, y);
	LOGVARN2(64, "c0", L"ʹ����Ʒ:%hs(%d,%d)", name, x, y);
	m_pGame->m_pGameProc->Click(x, y);
	if (WaitForItemBtnOpen()) {
		Sleep(150);
		// �����Ʒ����������ť�Աߵ�ͼ��
		m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 320, 82, 375, 145, 0, true);
		if (m_pGame->m_pPrintScreen->ComparePixel("30��������Ƭ+3", nullptr, 1) > 0) // ��Ҫȥʹ��30��������Ƭ+3
			return;

		if (!ItemBtnIsOpen()) {
			LOGVARN2(64, "red", L"��Ʒ������ťδ��", name, x, y);
			Sleep(300);
		}

		GetItemBtnPos(x, y, 0);
		m_pGame->m_pGameProc->Click(x, y); // ���ʹ�ð�ť
		LOGVARN2(64, "c0", L"ʹ����Ʒ���", name, x, y);
	}
	else {
		LOGVARN2(64, "c0", L"ʹ����Ʒ:%hs(%d,%d)ʧ��", name, x, y);
	}
}

// ������Ʒ
void Item::DropItem(const char* name, int x, int y, int index)
{
	DbgPrint("����:%s(%d,%d)\n", name, x, y);
	LOGVARN2(64, "c0", L"����:%hs(%d,%d)", name, x, y);
	m_pGame->m_pGameProc->Click(x, y);
	if (WaitForItemBtnOpen()) {
		Sleep(100);
		if (strcmp("��ɫף����Ƭ", name) == 0 && ItemBtnIsOpen(2)) // �е�������ť, ��ô��30��������Ƭ+3
			return;

		if (!ItemBtnIsOpen()) {
			LOGVARN2(64, "red", L"��Ʒ������ťδ��", name, x, y);
			Sleep(300);
		}

#if 1
		if (strcmp("Կ��", name) != 0) {
			// �����Ʒ����������ť�Աߵ�ͼ��
			for (int n = 1; n <= 2; n++) { // һ���������
				m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 320, 82, 375, 145, 0, true);
				if (m_pGame->m_pPrintScreen->ComparePixel("Կ��", nullptr, 1) > 0) // ������Կ��
					return;

				Sleep(n < 2 ? 100 : 50);
			}
		}
		else {
			// �����Ʒ����������ť�Աߵ�ͼ��
			for (int n = 1; n <= 2; n++) { // һ���������
				// �����Ʒ������Ʒ����(ȡԿ��������)
				m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 500, 76, 510, 86, 0, true);
				if (m_pGame->m_pPrintScreen->ComparePixel("�����Ǳ�Կ��(��������)", nullptr, 1) > 0) // ������Կ��
					return;

				Sleep(n < 2 ? 100 : 50);
			}
		}

		// �����Ʒ����������ť�Աߵ�ͼ��
		m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 320, 82, 375, 145, 0, true);
		if (m_pGame->m_pPrintScreen->ComparePixel("������ʯ", nullptr, 1) > 0) // ������ʯ
			return;
		
#else
		// �����Ʒ������Ʒ����(ȡԿ��������)
		m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 500, 76, 510, 86, 0, true);
		if (strcmp("Կ��", name) != 0) {
			if (m_pGame->m_pPrintScreen->ComparePixel("�����Ǳ�Կ��(��������)", nullptr, 1) > 0) // ������Կ��
				return;
		}

		// �����Ʒ������Ʒ����
		m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 415, 76, 425, 86, 0, true);
		if (m_pGame->m_pPrintScreen->ComparePixel("������ʯ(��������)", nullptr, 1) > 0) // ������ʯ
			return;
#endif

		GetItemBtnPos(x, y, index > -1 ? index : 1);
		m_pGame->m_pGameProc->Click(x, y); // ���������ť
	}
	else {
		LOGVARN2(64, "c0", L"����:%hs(%d,%d)ʧ��", name, x, y);
	}
}

// ������Ʒ
int Item::SellItem(ConfItemInfo* items, DWORD length)
{
	for (int i = 0; i <= 3; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;

		if (!BagIsOpen()) {
			LOGVARN2(64, "red", L"����û�д�, Wait Bag Open!");
			Sleep(100);
			if (!BagIsOpen()) {
				LOGVARN2(32, "red", L"����û�д�");
				if (i > 1) {
					break;
				}
				else {
					Sleep(100);
					continue;
				}
			}
		}

		if (SlideBag(i)) { // ��������
			Sleep(100);
		}

		for (DWORD iti = 0; iti < length; iti++) {
			for (int sell_i = 0; sell_i < 10; sell_i++) {
				ComPoint cp[32];
				PrintBagImg(true); // ��ȡ����ͼƬ
				int num = m_pGame->m_pPrintScreen->ComparePixel(items[iti].Name, cp, sizeof(cp) / sizeof(ComPoint));
				//DbgPrint("%d.����.������Ʒ����:%d\n", i + 1, num);
				if (num == 0)
					break;

				for (int j = 0; j < num; j++) {
					if (j == 0) {
						SellItem(cp[0].x, cp[0].y);
						DbgPrint("%d.����:%hs(%d,%d)\n", j + 1, items[iti].Name, cp[j].x, cp[j].y);
						LOGVARN2(64, "c0", L"%d.����:%hs(%d,%d)", j + 1, items[iti].Name, cp[j].x, cp[j].y);
					}
					else {
						ComPoint item[1];
						PrintBagImg(true); // ��ȡ����ͼƬ
						if (!m_pGame->m_pPrintScreen->ComparePixel(items[iti].Name, item, sizeof(item) / sizeof(ComPoint)))
							break;

						SellItem(item[0].x, item[0].y);
						DbgPrint("%d.����:%hs(%d,%d)\n", j + 1, items[iti].Name, item[0].x, item[0].y);
						LOGVARN2(64, "c0", L"%d.����:%hs(%d,%d)", j + 1, items[iti].Name, item[0].x, item[0].y);
					}
					Sleep(600);
				}
			}
		}

		if (i > 1) {
			if (m_pGame->m_pPrintScreen->ComparePixel("û����Ʒ", nullptr, 1))
				break;
			if (m_pGame->m_pPrintScreen->ComparePixel("��������", nullptr, 1))
				break;
		}
	}

	return 0;
}

// ������Ʒ
void Item::SellItem(int x, int y)
{
	//DbgPrint("����Ʒ:%d,%d\n", x, y);
	m_pGame->m_pGameProc->Click(x, y);
	if (WaitForItemBtnOpen()) {
		Sleep(100);
		GetItemBtnPos(x, y, 0);
		m_pGame->m_pGameProc->Click(x, y); // ���������ť
	}
}

// ��ȡ��Ʒ������ťλ��
void Item::GetItemBtnPos(int& x, int& y, int index)
{
	if (index == 0) {
		x = MyRand(180, 200);
		y = MyRand(86, 100);
		return;
	}
	if (index == 1) {
		x = MyRand(180, 200);
		y = MyRand(155, 168);
		return;
	}
}

// ����ֿ�
void Item::CheckIn(ConfItemInfo* items, DWORD length)
{
	DbgPrint("����\n");
	LOG2(L"����", "c0");
	DWORD _tm = GetTickCount();

	OpenStorage();
	Sleep(1000);

	int pet_yao_1 = 0, pet_yao_2 = 1;
	for (int i = 0; i <= 3; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;

		if (!BagIsOpen()) {
			LOGVARN2(64, "red", L"����û�д�, Wait Bag Open!");
			Sleep(100);
			if (!BagIsOpen()) {
				LOGVARN2(32, "red", L"����û�д�");
				if (i > 1) {
					break;
				}
				else {
					Sleep(100);
					continue;
				}
			}
		}

		m_pGame->m_pGameProc->CloseTipBox();
		if (SlideBag(i)) { // ��������
			Sleep(300);
		}

		int pet_yao = 0;
		if (!pet_yao_1) {
			PrintBagImg(true); // ��ȡ����ͼƬ
			pet_yao_1 = m_pGame->m_pPrintScreen->ComparePixel("��Чʥ����ҩ", nullptr, 1) > 0;
			pet_yao = pet_yao_1;
		}
		else {
			pet_yao = m_pGame->m_pPrintScreen->ComparePixel("��Чʥ����ҩ", nullptr, 1) > 0;
		}

		bool is_save = false; // �Ƿ���˶���
		for (DWORD iti = 0; iti < length; iti++) {
			ComPoint cp[32];
			PrintBagImg(true); // ��ȡ����ͼƬ
			int num = m_pGame->m_pPrintScreen->ComparePixel(items[iti].Name, cp, sizeof(cp) / sizeof(ComPoint));
			//DbgPrint("%d.����.������Ʒ����:%d\n", i + 1, num);
			if (num > 0) {
				for (int j = 0; j < num; j++) {
					if (j == 0) {
						m_pGame->m_pGameProc->Click(cp[0].x, cp[0].y);
						DbgPrint("%d.����.%s:(%d,%d)\n", j + 1, items[iti].Name, cp[j].x, cp[j].y);
						LOGVARN2(64, "c0", L"%d.����:%hs(%d,%d)", j + 1, items[iti].Name, cp[j].x, cp[j].y);
					}
					else {
						ComPoint item[1];
						PrintBagImg(true); // ��ȡ����ͼƬ
						if (!m_pGame->m_pPrintScreen->ComparePixel(items[iti].Name, item, sizeof(item) / sizeof(ComPoint)))
							break;

						m_pGame->m_pGameProc->Click(item[0].x, item[0].y);
						DbgPrint("%d.����.%s:(%d,%d)\n", j + 1, items[iti].Name, item[0].x, item[0].y);
						LOGVARN2(64, "c0", L"%d.����:%hs(%d,%d)", j + 1, items[iti].Name, item[0].x, item[0].y);
					}
					Sleep(800);
				}
				is_save = true;
			}
		}

		if (is_save && pet_yao_1 && pet_yao) {
			PrintBagImg(true); // ��ȡ����ͼƬ
			pet_yao_2 = m_pGame->m_pPrintScreen->ComparePixel("��Чʥ����ҩ", nullptr, 1);
		}

		if (i > 2) {
			if (m_pGame->m_pPrintScreen->ComparePixel("û����Ʒ", nullptr, 1))
				break;
			if (m_pGame->m_pPrintScreen->ComparePixel("��������", nullptr, 1))
				break;
		}
	}
	if (pet_yao_1 && !pet_yao_2) {
		DbgPrint("\n======��Чʥ����ҩ������, ��Ҫȡ����======\n\n");
		LOG2(L"\n======��Чʥ����ҩ������, ��Ҫȡ����======", "blue_r");
		CheckOutOne("��Чʥ����ҩ", false, false);
	}

	Sleep(150);
	CloseStorage();
	Sleep(100);

	_tm = GetTickCount() - _tm;
	DbgPrint("������ʱ:%.2f��, %d����\n", (float)_tm / 1000.0f, _tm);
	LOGVARN2(64, "c0", L"������ʱ:%d��, %d����", _tm / 1000, _tm);
}

// ȡ���ֿ�
int Item::CheckOut(ConfItemInfo* items, DWORD length)
{
	DbgPrint("ȡ��\n");
	LOG2(L"ȡ��", "c0");
	DWORD _tm = GetTickCount();

	OpenStorage();
	Sleep(1000);

	int count = 0;

	for (int i = 0; i <= 5; i++) {
		if (SlideStorge(i)) { // �����ֿ�
			Sleep(300);
		}

		for (DWORD iti = 0; iti < length; iti++) {
			ComPoint cp[32];
			PrintStorageImg(true); // ��ȡ�ֿ�ͼƬ
			int num = m_pGame->m_pPrintScreen->ComparePixel(items[iti].Name, cp, sizeof(cp) / sizeof(ComPoint));
			count += num;
			//DbgPrint("%d.ȡ��.�ֿ���Ʒ����:%d\n", i + 1, num);
			if (num > 0) {
				for (int j = 0; j < num; j++) {
					if (j == 0) {
						m_pGame->m_pGameProc->Click(cp[0].x, cp[0].y);
						DbgPrint("%d.ȡ��.(%s:%d,%d)\n", j + 1, items[iti].Name, cp[j].x, cp[j].y);
						LOGVARN2(64, "c0", L"%d.ȡ��:%hs(%d,%d)", j + 1, items[iti].Name, cp[j].x, cp[j].y);
					}
					else {
						ComPoint item[1];
						PrintStorageImg(true); // ��ȡ�ֿ�ͼƬ
						if (!m_pGame->m_pPrintScreen->ComparePixel(items[iti].Name, item, sizeof(item) / sizeof(ComPoint)))
							break;

						m_pGame->m_pGameProc->Click(item[0].x, item[0].y);
						DbgPrint("%d.ȡ��.%s:(%d,%d)\n", j + 1, items[iti].Name, item[0].x, item[0].y);
						LOGVARN2(64, "c0", L"%d.ȡ��:%hs(%d,%d)", j + 1, items[iti].Name, item[0].x, item[0].y);
					}
					Sleep(800);
				}
			}
		}

		if (0) {
			if (m_pGame->m_pPrintScreen->ComparePixel("û����Ʒ", nullptr, 1))
				break;
			if (m_pGame->m_pPrintScreen->ComparePixel("��������", nullptr, 1))
				break;
		}
	}

	// �ֿ���ӽ����Ƿ��
	while (StorageAddBtnIsOpen()) {
		m_pGame->m_pGameProc->Click(510, 380, 602, 400); // ȡ��
		Sleep(500);
	}

#if 1
	LOG2(L"��Ǯ.", "orange");
	m_pGame->m_pGameProc->Click(150, 666, 235, 682); // �ֿ��Ǯ��ť
	for (int n = 0; n < 2000; n += 100) {
		Sleep(100);
		if (SaveMoneyBtnIsOpen()) {
			Sleep(500);
			m_pGame->m_pGameProc->Click(770, 382, 776, 385); // �������ұ�
			Sleep(500);
			m_pGame->m_pGameProc->Click(750, 300, 760, 320); // +��, ����0�治��
			Sleep(500);
			m_pGame->m_pGameProc->Click(680, 468, 750, 490); // ȷ��
			Sleep(500);
			break;
		}
	}

	while (SaveMoneyBtnIsOpen()) { // ����Ƿ��а�ť
		m_pGame->m_pGameProc->Click(460, 468, 550, 490); // ȡ��
		Sleep(300);
	}
	LOG2(L"��Ǯ���.", "orange");
#endif

	CloseStorage();

	for (int j = 0; j < 5; j++) {
		Sleep(500);
		if (!BagIsOpen())
			break;

		CloseStorage();
	}

	_tm = GetTickCount() - _tm;
	DbgPrint("ȡ����ʱ:%.2f��, %d����\n", (float)_tm / 1000.0f, _tm);
	LOGVARN2(64, "c0", L"ȡ����ʱ:%d��, %d����", _tm / 1000, _tm);

	return count;
}

// ȡ��һ���ֿ���Ʒ
int Item::CheckOutOne(const char * name, bool open, bool close)
{
	if (open) {
		OpenStorage();
		Sleep(1000);
	}
	
	int count = 0;
	for (int i = 0; i <= 5; i++) {
		if (SlideStorge(i)) { // �����ֿ�
			Sleep(300);
		}

		ComPoint cp[1];
		PrintStorageImg(true); // ��ȡ�ֿ�ͼƬ
		if (m_pGame->m_pPrintScreen->ComparePixel(name, cp, 1)) {
			DbgPrint("ȡ��.%s:(%d,%d)\n", name, cp[0].x, cp[0].y);
			LOGVARN2(64, "c0", L"ȡ��.%hs:(%d,%d)\n", name, cp[0].x, cp[0].y);
			m_pGame->m_pGameProc->Click(cp[0].x, cp[0].y);
			count = 1;
		}

		if (m_pGame->m_pPrintScreen->CompareImage(CIN_NoItem, nullptr, 1))
			break;
		if (m_pGame->m_pPrintScreen->CompareImage(CIN_LockItem, nullptr, 1))
			break;
	}

	if (close) {
		Sleep(150);
		CloseStorage();
	}

	return count;
}

// ��ȡ������Ʒ����
int Item::GetBagCount(ComImgIndex index)
{
	DWORD _tm = GetTickCount();

	OpenBag();
	Sleep(1000);

	int count = 0; // ����
	for (int i = 0; i <= 3; i++) {
		if (SlideBag(i)) { // ��������
			Sleep(300);
		}

		PrintBagImg(true); // ��ȡ����ͼƬ
		int num = m_pGame->m_pPrintScreen->CompareImage(index);
		DbgPrint("%d.������Ʒ����:%d\n", i + 1, num);
		LOGVARN2(64, "c0", L"%d.������Ʒ����:%d", i + 1, num);

		count += num;
		if (count > 0 && num == 0) // ������û��Ʒ
			break;

		if (m_pGame->m_pPrintScreen->CompareImage(CIN_NoItem, nullptr, 1))
			break;
		if (m_pGame->m_pPrintScreen->CompareImage(CIN_LockItem, nullptr, 1))
			break;
	}

	Sleep(500);
	CloseBag();

	_tm = GetTickCount() - _tm;
	DbgPrint("��ȡ��Ʒ��ʱ:%.2f��, %d����\n", (float)_tm / 1000.0f, _tm);
	LOGVARN2(64, "c0", L"��ȡ��Ʒ��ʱ:%d��, %d����", _tm / 1000, _tm);
	return count;
}

// ��ȡ�������Ʒ����
int Item::GetQuickYaoOrBaoNum(int& yaobao, int& yao, _account_* account)
{
	account = account ? account : m_pGame->m_pGameProc->m_pAccount;
	int data[2] = {0, 0};
	m_pGame->m_pGameData->ReadMemory((PVOID)account->Addr.QuickItemNum, data, sizeof(data), account);
	yao = data[0];
	yaobao = data[1];
	//DbgPrint("ҩ������:%d ҩ����:%d\n", yaobao, yao);

	return 0;
}

// �л������ܿ����
void Item::SwitchMagicQuickBar()
{
#if 0
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_hWndBig, 1235, 350, 1245, 360, 0, true);
	if (m_pGame->m_pPrintScreen->ComparePixel("������л���ť", nullptr, 1) == 0) { // ������XP������
		DbgPrint("������л���ť\n");
		SwitchQuickBar(1);
		Sleep(500);
	}
#endif

	if (QuickBarIsXingChen()) { // �л�
		SwitchQuickBar(1);
		Sleep(500);
	}
}

// ������Ƿ����ǳ�֮��
bool Item::QuickBarIsXingChen()
{
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 926, 510, 936, 520, 0, true);
	return m_pGame->m_pPrintScreen->ComparePixel("�ǳ�֮�ۿ����", nullptr, 1) > 0;
}

// �л������
void Item::SwitchQuickBar(int page)
{
	DbgPrint("�л��������:%d\n", page);
	m_pGame->m_pGameProc->Click(1235, 360, 1250, 370);
}

// ��ȡ��Ʒ��ComImgIndex
ComImgIndex Item::GetItemComImgIndex(const char * name)
{
	if (strcmp(name, "˫����������") == 0)
		return CIN_QingChi;
	if (strcmp(name, "˫������ڤ���") == 0)
		return CIN_ZiMing;
	if (strcmp(name, "�����ǿ������") == 0)
		return CIN_KaDi;
	if (strcmp(name, "�������°����") == 0)
		return CIN_MuBa;
	if (strcmp(name, "��ţ���Ƕ����") == 0)
		return CIN_YaEr;
	if (strcmp(name, "��ţ��³�����") == 0)
		return CIN_LuDi;
	if (strcmp(name, "��ħ��ʯ") == 0)
		return CIN_HuanMo;
	if (strcmp(name, "ħ�꾧ʯ") == 0)
		return CIN_MoHun;
	if (strcmp(name, "��꾧ʯ") == 0)
		return CIN_LingHun;
	if (strcmp(name, "���ȵ�����") == 0)
		return CIN_XiangLian;

	return CIN_NoItem;
}

// ��ȡ����ͼƬ
HBITMAP Item::PrintBagImg(bool del)
{
	HBITMAP bitmap = m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 672, 158, 1170, 620);
	if (del) {
		m_pGame->m_pPrintScreen->GetPixel(0, 0);
		DeleteBagImg();
	}
	return bitmap;
}

// ɾ������ͼƬ
void Item::DeleteBagImg()
{
	m_pGame->m_pPrintScreen->Release();
}

// ��ȡ�ֿ�ͼƬ
HBITMAP Item::PrintStorageImg(bool del)
{
	HBITMAP bitmap = m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_hWndBig, 113, 158, 620, 620);
	if (del) {
		m_pGame->m_pPrintScreen->GetPixel(0, 0);
		DeleteBagImg();
	}
	return bitmap;
}

// ɾ���ֿ�ͼƬ
void Item::DeleteStorageImg()
{
	m_pGame->m_pPrintScreen->Release();
}
