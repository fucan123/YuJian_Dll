#include "Game.h"
#include "GameConf.h"
#include "Item.h"
#include "Talk.h"
#include "Button.h"
#include "PrintScreen.h"
#include "GameProc.h"
#include <My/Common/C.h>

#define ITEM_CLICK_PIC 0

Item::Item(Game* p)
{
	m_pGame = p;
}

// �򿪱���
bool Item::OpenBag()
{
	DbgPrint("�򿪱���\n");
	LOG2(L"�򿪱���", "c0");
	if (m_pGame->m_pButton->CheckButton(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_BAG_ITEM, "MPC��Ʒ��"))
		return true;

	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_BAG);
	for (int i = 0; i < 2000; i += 100) {
		Sleep(100);
		if (m_pGame->m_pButton->CheckButton(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, 
			BUTTON_ID_BAG_ITEM, "MPC��Ʒ��")) {
			Sleep(800);
			DbgPrint("�����Ѿ���\n");
			LOG2(L"�����Ѿ���", "c0");
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
			return;
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
		Sleep(200);
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
	if (yao <= 3 && yaobao > 1)
		return true;

	return false;
}

// �����Ƿ��
bool Item::BagIsOpen()
{
	return m_pGame->m_pButton->CheckButton(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_BAG_ITEM, "MPC��Ʒ��");
}

// �����Ƿ���Ҫ��ҳ
bool Item::BagNeedPageDown()
{
#if IS_READ_MEM == 0
	int x_count, y_count;
	m_pGame->m_pPrintScreen->GetPixelConCount(x_count, y_count, 0xFF151310);
	return x_count < 20 || y_count < 10;
#else
	// ������һ���Ƿ�������
	DWORD data = 0;
	m_pGame->m_pGameData->ReadDwordMemory(m_pGame->m_pGameProc->m_pAccount->Addr.Bag + 0x9C,
		data, m_pGame->m_pGameProc->m_pAccount);
	return data != 0;
#endif
}

// ��ȡ������Ʒ
void Item::ReadBagItem(ITEM_TYPE items[], int length)
{
	m_pGame->m_pGameData->ReadMemory((PVOID)m_pGame->m_pGameProc->m_pAccount->Addr.Bag,
		items, length, m_pGame->m_pGameProc->m_pAccount);
}

// �ֿ��Ƿ���Ҫ��ҳ
bool Item::StorageNeedPageDown()
{
	// ������һ���Ƿ�������
	DWORD data = 0;
	m_pGame->m_pGameData->ReadDwordMemory(m_pGame->m_pGameProc->m_pAccount->Addr.Storage + 0xEC,
		data, m_pGame->m_pGameProc->m_pAccount);
	return data != 0;
}

// ��ȡ�ֿ���Ʒ
void Item::ReadStorageItem(ITEM_TYPE items[], int length)
{
	m_pGame->m_pGameData->ReadMemory((PVOID)m_pGame->m_pGameProc->m_pAccount->Addr.Storage,
		items, length, m_pGame->m_pGameProc->m_pAccount);
}

// ��ȡ������Ʒ���λ��(index=��Ʒ����λ��)
void Item::GetBagClickPos(int index, int& click_x, int& click_y)
{
	int x = 5, x2 = 20;  // ��ʼλ��
	int y = 10, y2 = 20; // ��ʼλ��

	x += (index % 10) * 35; // xÿ������35
	x2 += (index % 10) * 35; // xÿ������35

	y += (index / 10) * 34;  // yÿ������34
	y2 += (index / 10) * 34; // yÿ������34

	click_x = MyRand(x, x2);
	click_y = MyRand(y, y2);
}

// �����ť����
bool Item::PickUpItemByBtn()
{
	LOG2(L"�����ťʰȡ", "c3");
	//m_pGame->m_pEmulator->Tap(MyRand(26, 35), MyRand(656, 675));
	m_pGame->m_pButton->Key('7');
	return WaitForPickUpItem(6000, 1000);
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
		m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game,
			m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, pos_x, pos_y);
		WaitForPickUpItem();
		count++;

		Sleep(260);
	}

	return count;
}

// �ȴ�������Ʒ
bool Item::WaitForPickUpItem(DWORD wait_ms, DWORD sleep_ms)
{
	DWORD pos_x = 0, pos_y = 0;
	m_pGame->m_pGameData->ReadCoor(&pos_x, &pos_y, m_pGame->m_pGameProc->m_pAccount);

	DWORD x = pos_x, y = pos_y;
	for (DWORD i = 0; i <= wait_ms; i += 500) {
		Sleep(sleep_ms);

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
		y = 90;
		x2 = 1098;
		y2 = 730;
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
			color = 0xFFBBD533; // ��2��
			diff = 0x000B0B0B;
		}
		else if (strcmp("6��O���", pick_name) == 0) {
			//color = 0xFFBDA392;
			diff = 0x00080808;
		}
		else if (strcmp("12��XO���", pick_name) == 0) {
			color = 0xFFE3E2F1; // ��10��
			diff = 0x00010101;
		}
		else if (strcmp("25��XO���", pick_name) == 0) {
			color = 0xFF667EBE;
			diff = 0x00010101;
		}
		else if (strcmp("30��������Ƭ+1", pick_name) == 0) {
			color = 0xFFF6BB8D; // ��4��
			diff = 0x00020202;
		}
		else if (strcmp("30��������Ƭ+2", pick_name) == 0) {
			color = 0xFF627BA3; // ��3��
			diff = 0x00020202;
		}
		else if (strcmp("30��������Ƭ+3", pick_name) == 0) {
			color = 0xFFBE76D9; // ��2��
			diff = 0x00080808;
		}
		else if (strcmp("��ħ��ʯ", pick_name) == 0) {
			color = 0xFFC52F96; // ��8��
			diff = 0x00080808;
		}
		else if (strcmp("ħ�꾧ʯ", pick_name) == 0) {
			color = 0xFF925B20; // ��5��
			diff = 0x00010101;
		}
		else if (strcmp("��꾧ʯ", pick_name) == 0) {
			color = 0xFF91090B; // ��4��
			diff = 0x00020202;
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
int Item::DropItem(DWORD* ms)
{
	DWORD _tm = GetTickCount();

	OpenBag();
	Sleep(1000);

	bool is_use_item = false;
	int count = 0; // ����
#if IS_READ_MEM == 0
	for (int i = 0; i < 2; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;

		if (SlideBag(i)) // �л�����ҳ��
			Sleep(1000);

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
			if (num <= 8 && strcmp(items[idx].Name, "��Ч���ư�") == 0)
				continue;
			//m_pGame->m_pPrintScreen->SaveBitmapToFile(bitmap, L"C:\\Users\\fucan\\Desktop\\MNQ-9Star\\jt.bmp");
			//DbgPrint("%d.��.%s.������Ʒ����:%d\n", i + 1, drops[idx], num);

			int drop_num = 0;
			drop_num = strcmp(items[idx].Name, "Կ��") != 0 ? num : 1; // Կ��ֻ����ǰ����һ��
			if (strcmp(items[idx].Name, "��Ч���ư�") == 0)
				drop_num = num - 8;

			for (int j = 0; j < drop_num; j++) {
				if (j == 0) {
					DropItem(items[idx].Name, cp[0].x, cp[0].y, 0);
				}
				else {
					ComPoint item[1];
					PrintBagImg(true); // ��ȡ����ͼƬ
					if (!m_pGame->m_pPrintScreen->ComparePixel(items[idx].Name, item, sizeof(item) / sizeof(ComPoint)))
						break;

					DropItem(items[idx].Name, item[0].x, item[0].y, 0);
				}
				Sleep(100);
			}
			count += num;

			m_pGame->m_pGameProc->CloseTipBox();
}

		if (!is_use_item) {
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
				}
				count += num;

				m_pGame->m_pGameProc->CloseTipBox();
			}
		}

		if (!BagNeedPageDown())
			break;
	}
#else
	for (int i = 0; i < 5; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;

		if (SlideBag(i)) // ��������
			Sleep(1000); 
		
		ITEM_TYPE bag_items[40];
		ConfItemInfo* items = m_pGame->m_pGameConf->m_stDrop.Drops;
		int length = m_pGame->m_pGameConf->m_stDrop.Length;
		int idx = 0, drop_i = 0;
		for (idx = 0; idx < length; idx++) {
			if (drop_i >= 30)
				break;

			ReadBagItem(bag_items, sizeof(bag_items));
			for (int n = 0; n < sizeof(bag_items) / sizeof(ITEM_TYPE); n++) {
				if (bag_items[n] && bag_items[n] == items[idx].Type) {
					int click_x, click_y;
					GetBagClickPos(n, click_x, click_y);
					DropItem(items[idx].Name, click_x, click_y);

					idx = 0;
					drop_i++;
					break;
				}
			}
		}

		items = m_pGame->m_pGameConf->m_stUse.Uses;
		length = m_pGame->m_pGameConf->m_stUse.Length;
		for (idx = 0; idx < length; idx++) {
			ReadBagItem(bag_items, sizeof(bag_items));
			for (int n = 0; n < sizeof(bag_items) / sizeof(ITEM_TYPE); n++) {
				if (bag_items[n] && bag_items[n] == items[idx].Type) {
					int click_x, click_y;
					GetBagClickPos(n, click_x, click_y);
					UseItem(items[idx].Name, click_x, click_y);

					break;
				}
			}
		}

		if (!BagNeedPageDown())
			break;
	}
#endif
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, BUTTON_ID_CLOSEMENU, "C");
	Sleep(200);
	CloseBag();
	Sleep(300);

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
#if IS_READ_MEM == 0
#if ITEM_CLICK_PIC
	m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, x, y, 50, false);
#else
	m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		BUTTON_ID_BAG_ITEM, "MPC��Ʒ��", x, y, 0xff, false);
#endif
#else
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		BUTTON_ID_BAG_ITEM, "MPC��Ʒ��", x, y, 0xff, false);
#endif
#if 1
	if (m_pGame->m_pTalk->WaitTalkOpen(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic)) {
		m_pGame->m_pTalk->Select("�.ȫ��ֱ�Ӻϳ�+1��+2", m_pGame->m_pGameProc->m_pAccount->Wnd.Pic);
		Sleep(500);
		m_pGame->m_pTalk->Select("�.ȫ��ֱ�Ӻϳ�+1��+2.ȷ��", m_pGame->m_pGameProc->m_pAccount->Wnd.Pic);
	}
#endif

	Sleep(500);
}

// ������Ʒ
void Item::DropItem(const char* name, int x, int y, int index)
{
	DbgPrint("����:%s(%d,%d)\n", name, x, y);
	LOGVARN2(64, "c0", L"����:%hs(%d,%d)", name, x, y);

#if IS_READ_MEM == 0
#if ITEM_CLICK_PIC
	m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, x, y);
#else
	m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		BUTTON_ID_BAG_ITEM, "MPC��Ʒ��", x, y);
#endif
#else
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		BUTTON_ID_BAG_ITEM, "MPC��Ʒ��", x, y);
#endif
	Sleep(100);

	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		MyRand(700, 999), MyRand(500, 735));
	Sleep(360);

	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		BUTTON_ID_TIPSURE, "ȷ��");

	Sleep(500);
}

// ������Ʒ
int Item::SellItem(ConfItemInfo* items, DWORD length)
{
	Sleep(1000);

#if IS_READ_MEM == 0
	for (int i = 0; i < 2; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;

		if (SlideBag(i)) // �л�����ҳ��
			Sleep(1000);

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
						SellItem(items[iti].Name, cp[0].x, cp[0].y);
					}
					else {
						ComPoint item[1];
						PrintBagImg(true); // ��ȡ����ͼƬ
						if (!m_pGame->m_pPrintScreen->ComparePixel(items[iti].Name, item, sizeof(item) / sizeof(ComPoint)))
							break;

						SellItem(items[iti].Name, item[0].x, item[0].y);
					}
					Sleep(100);
				}
			}
		}

		if (!BagNeedPageDown())
			break;
	}
#else
	for (int i = 0; i < 5; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;

		if (SlideBag(i)) // ��������
			Sleep(1000);

		ITEM_TYPE bag_items[40];
		ConfItemInfo* items = m_pGame->m_pGameConf->m_stSell.Sells;
		int length = m_pGame->m_pGameConf->m_stSell.Length;
		int idx = 0;
		for (idx = 0; idx < length; idx++) {
			ReadBagItem(bag_items, sizeof(bag_items));
			for (int n = 0; n < sizeof(bag_items) / sizeof(ITEM_TYPE); n++) {
				if (bag_items[n] && bag_items[n] == items[idx].Type) {
					int click_x, click_y;
					GetBagClickPos(n, click_x, click_y);
					SellItem(items[idx].Name, click_x, click_y);

					idx = 0;
					break;
				}
			}
		}

		if (!BagNeedPageDown())
			break;
	}
#endif

	return 0;
}

// ������Ʒ
void Item::SellItem(const char* name, int x, int y)
{
	DbgPrint("����Ʒ:%s %d,%d\n", name, x, y);
	LOGVARN2(64, "c0", L"����Ʒ:%hs(%d,%d)", name, x, y);

#if IS_READ_MEM == 0
#if ITEM_CLICK_PIC
	m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, x, y, 50, false);
#else
	m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		BUTTON_ID_BAG_ITEM, "MPC��Ʒ��", x, y, 0xff, false);
#endif
#else
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		BUTTON_ID_BAG_ITEM, "MPC��Ʒ��", x, y, 0xff, false);
#endif
	Sleep(360);

	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_TIPSURE, "ȷ��");
	Sleep(500);
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
void Item::CheckIn()
{
	DbgPrint("����\n");
	LOG2(L"����", "c0");
	DWORD _tm = GetTickCount();

	OpenStorage();
	Sleep(1000);

#if IS_READ_MEM == 0
	for (int i = 0; i < 2; i++) {
		m_pGame->m_pGameProc->CloseTipBox();

		if (m_pGame->m_pGameProc->m_bPause)
			break;
		if (SlideBag(i)) // �л�����ҳ��
			Sleep(1000);

		bool is_save = false; // �Ƿ���˶���
		ConfItemInfo* items = m_pGame->m_pGameConf->m_stCheckIn.CheckIns;
		int length = m_pGame->m_pGameConf->m_stCheckIn.Length;
		for (DWORD iti = 0; iti < length; iti++) {
			ComPoint cp[32];
			PrintBagImg(true); // ��ȡ����ͼƬ
			int num = m_pGame->m_pPrintScreen->ComparePixel(items[iti].Name, cp, sizeof(cp) / sizeof(ComPoint));
			//DbgPrint("%d.����.������Ʒ����:%d\n", i + 1, num);
			if (num > 0) {
				for (int j = 0; j < num; j++) {
					if (j == 0) {
#if ITEM_CLICK_PIC
						m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, 
							cp[j].x, cp[j].y, 50, false);
#else
						m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
							BUTTON_ID_BAG_ITEM, "MPC��Ʒ��", cp[j].x, cp[j].y, 0xff, false);
#endif
						
						LOGVARN2(64, "c0", L"%d.����:%hs(%d,%d)", j + 1, items[iti].Name, cp[j].x, cp[j].y);
					}
					else {
						ComPoint item[1];
						PrintBagImg(true); // ��ȡ����ͼƬ
						if (!m_pGame->m_pPrintScreen->ComparePixel(items[iti].Name, item, sizeof(item) / sizeof(ComPoint)))
							break;

#if ITEM_CLICK_PIC
						m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
							item[0].x, item[0].y, 50, false);
#else
						m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
							BUTTON_ID_BAG_ITEM, "MPC��Ʒ��", item[0].x, item[0].y, 0xff, false);
#endif
						DbgPrint("%d.����.%s:(%d,%d)\n", j + 1, items[iti].Name, item[0].x, item[0].y);
						LOGVARN2(64, "c0", L"%d.����:%hs(%d,%d)", j + 1, items[iti].Name, item[0].x, item[0].y);
					}
					Sleep(600);
				}
				is_save = true;
			}
		}

		if (!BagNeedPageDown())
			break;
	}
#else

	int pet_yao_1 = 0, pet_yao_2 = 1;
	for (int i = 0; i <= 5; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;

		if (SlideBag(i)) // ��������
			Sleep(1000);

		ITEM_TYPE bag_items[40];
		ConfItemInfo* items = m_pGame->m_pGameConf->m_stCheckIn.CheckIns;
		int length = m_pGame->m_pGameConf->m_stCheckIn.Length;
		int idx = 0, save_i = 0;
		for (idx = 0; idx < length; idx++) {
			if (save_i >= 30)
				break;

			ReadBagItem(bag_items, sizeof(bag_items));
			for (int n = 0; n < sizeof(bag_items) / sizeof(ITEM_TYPE); n++) {
				if (bag_items[n] && bag_items[n] == items[idx].Type) {
					int click_x, click_y;
					GetBagClickPos(n, click_x, click_y);

					LOGVARN2(64, "c0", L"����:%hs %d,%d(%d)", items[idx].Name, click_x, click_y, n);
					m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
						BUTTON_ID_BAG_ITEM, "MPC��Ʒ��", click_x, click_y, 0xff, false);
					Sleep(600);

					idx = 0;
					save_i++;
					break;
				}
			}
		}

		if (!BagNeedPageDown())
			break;
	}
#endif

	Sleep(200);
	CloseStorage();
	Sleep(300);

	_tm = GetTickCount() - _tm;
	DbgPrint("������ʱ:%.2f��, %d����\n", (float)_tm / 1000.0f, _tm);
	LOGVARN2(64, "c0", L"������ʱ:%d��, %d����", _tm / 1000, _tm);
}

// ȡ���ֿ�
int Item::CheckOut(ConfItemInfo* items, DWORD length)
{
	int count = 0;
	DbgPrint("ȡ��\n");
	LOG2(L"ȡ��", "c0");
	DWORD _tm = GetTickCount();

	OpenStorage();
	Sleep(1000);


	HWND game = m_pGame->m_pGameProc->m_pAccount->Wnd.Game;
	HWND pic = m_pGame->m_pGameProc->m_pAccount->Wnd.Pic;

#if IS_READ_MEM == 0
	for (int i = 0; i < 2; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;
		if (SlideStorge(i)) { // �����ֿⰡ
			Sleep(1000);
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
						LOGVARN2(64, "c0", L"ȡ��:%hs %d,%d", items[iti].Name, cp[0].x, cp[0].y);
#if ITEM_CLICK_PIC
						m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
							cp[0].x, cp[0].y, 50, false);
#else
						m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
							BUTTON_ID_CKIN_ITEM, "�洢��Ʒ��", cp[0].x, cp[0].y, 0xff, false);
#endif
					}
					else {
						ComPoint item[1];
						PrintStorageImg(true); // ��ȡ�ֿ�ͼƬ
						if (!m_pGame->m_pPrintScreen->ComparePixel(items[iti].Name, item, sizeof(item) / sizeof(ComPoint)))
							break;

						LOGVARN2(64, "c0", L"ȡ��:%hs %d,%d", items[iti].Name, item[0].x, item[0].y);
#if ITEM_CLICK_PIC
						m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
							item[0].x, item[0].y, 50, false);
#else
						m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
							BUTTON_ID_CKIN_ITEM, "�洢��Ʒ��", item[0].x, item[0].y, 0xff, false);
#endif
					}
					Sleep(600);
				}
			}
		}
		if (m_pGame->m_pPrintScreen->ComparePixel("û����Ʒ", nullptr, 1))
			break;
	}
#else
	for (int i = 0; i <= 5; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;

		if (SlideStorge(i)) // �����ֿ�ҳ��
			Sleep(1000);

		ITEM_TYPE storage_items[60];
		int idx = 0, out_i = 0;
		for (idx = 0; idx < length; idx++) {
			if (out_i >= 30)
				break;

			ReadStorageItem(storage_items, sizeof(storage_items));
			for (int n = 0; n < sizeof(storage_items) / sizeof(ITEM_TYPE); n++) {
				if (storage_items[n] && storage_items[n] == items[idx].Type) {
					int click_x, click_y;
					GetBagClickPos(n, click_x, click_y);

					LOGVARN2(64, "c0", L"ȡ��:%hs %d,%d(%d)", items[idx].Name, click_x, click_y, n);
					m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
						BUTTON_ID_CKIN_ITEM, "�洢��Ʒ��", click_x, click_y, 0xff, false);
					Sleep(600);

					count++;
					idx = 0;
					out_i++;
					break;
				}
			}
		}

		if (!StorageNeedPageDown())
			break;
	}
#endif

	Sleep(200);

#if 1
	LOG2(L"��Ǯ.", "orange");
	m_pGame->m_pButton->Click(game, BUTTON_ID_SAVE_MNY, "ȡ");
	Sleep(800);
	m_pGame->m_pButton->ClickPic(game, pic, MyRand(1216, 1286), MyRand(300, 303), 300);
	char money[] = {'5', '0', '0', '0', '0', '0', '0', '0', 0};
	for (int i = 0; money[i]; i++) {
		m_pGame->m_pButton->Key(money[i]);
		Sleep(260);
	}
	m_pGame->m_pButton->Click(game, BUTTON_ID_CUNQIAN, "��");
	Sleep(300);
	LOG2(L"��Ǯ���.", "orange");
#endif

	CloseStorage();
	Sleep(300);

	_tm = GetTickCount() - _tm;
	DbgPrint("ȡ����ʱ:%.2f��, %d����\n", (float)_tm / 1000.0f, _tm);
	LOGVARN2(64, "c0", L"ȡ����ʱ:%d��, %d����", _tm / 1000, _tm);

	return count;
}

// ȡ��һ���ֿ���Ʒ
int Item::CheckOutOne(const char * name, bool open, bool close)
{
	DbgPrint("ȡ��\n");
	LOG2(L"ȡ��", "c0");

	ITEM_TYPE type = (ITEM_TYPE)m_pGame->m_pGameConf->TransFormItemType(name);
	if (type == δ֪��Ʒ) {
		DbgPrint("δ֪��Ʒ.\n");
		LOG2(L"δ֪��Ʒ.", "c0");
		return 0;
	}
		
	if (open) {
		OpenStorage();
		Sleep(1000);
	}

	int count = 0;

#if IS_READ_MEM == 0
	for (int i = 0; i < 2; i++) {
		if (SlideStorge(i)) { // �����ֿ�
			Sleep(1000);
		}

		ComPoint cp[1];
		PrintStorageImg(true); // ��ȡ�ֿ�ͼƬ
		if (m_pGame->m_pPrintScreen->ComparePixel(name, cp, 1)) {
			LOGVARN2(64, "c0", L"ȡ��:%hs %d,%d", name, cp[0].x, cp[0].y);
#if 0
			m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
				cp[0].x, cp[0].y, 50, false);
#else
			m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
				BUTTON_ID_CKIN_ITEM, "�洢��Ʒ��", cp[0].x, cp[0].y, 0xff, false);
#endif
			Sleep(600);
			count = 1;
		}

		if (m_pGame->m_pPrintScreen->ComparePixel("û����Ʒ", nullptr, 1))
			break;
	}
#else	
	for (int i = 0; i <= 5; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;

		if (SlideStorge(i)) // �����ֿ�ҳ��
			Sleep(1000);

		ITEM_TYPE storage_items[60];
		ReadStorageItem(storage_items, sizeof(storage_items));
		for (int n = 0; n < sizeof(storage_items) / sizeof(ITEM_TYPE); n++) {
			if (storage_items[n] && storage_items[n] == type) {
				int click_x, click_y;
				GetBagClickPos(n, click_x, click_y);

				LOGVARN2(64, "c0", L"ȡ��:%hs %d,%d(%d)", name, click_x, click_y, n);
				m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
					BUTTON_ID_CKIN_ITEM, "�洢��Ʒ��", click_x, click_y, 0xff, false);
				Sleep(600);

				count++;
				ReadStorageItem(storage_items, sizeof(storage_items));
				n = 0;
				continue;
			}
		}

		if (!StorageNeedPageDown())
			break;
	}
#endif
	if (close) {
		Sleep(200);
		CloseStorage();
		Sleep(300);
	}

	DbgPrint("ȡ�����.\n");
	LOG2(L"ȡ�����.", "c0");
	return count;
}

// ��ȡ������Ʒ����
int Item::GetBagItemCount(const char* name)
{
	int count = 0;
	OpenBag();
	Sleep(1000);

	DbgPrint("��ȡ������Ʒ����:%s\n", name);
#if IS_READ_MEM == 0
	for (int i = 0; i < 2; i++) {
		if (SlideBag(i)) { // ��������
			Sleep(300);
		}

		PrintBagImg(true); // ��ȡ����ͼƬ
		int num = m_pGame->m_pPrintScreen->ComparePixel(name);
		//DbgPrint("%d.������Ʒ����:%d\n", i + 1, num);
		//LOGVARN2(64, "c0", L"%d.������Ʒ����:%d", i + 1, num);

		count += num;
		if (count > 0 && num == 0) // ������û��Ʒ
			break;

		if (m_pGame->m_pPrintScreen->ComparePixel("û����Ʒ", nullptr, 1))
			break;
	}
#else
	ITEM_TYPE type = (ITEM_TYPE)m_pGame->m_pGameConf->TransFormItemType(name);
	for (int i = 0; i < 5; i++) {
		if (SlideBag(i)) // ��������
			Sleep(1000);

		ITEM_TYPE bag_items[40];
		ReadBagItem(bag_items, sizeof(bag_items));
		for (int n = 0; n < sizeof(bag_items) / sizeof(ITEM_TYPE); n++) {
			if (bag_items[n] && bag_items[n] == type)
				count++;
		}

		if (!BagNeedPageDown())
			break;
	}
#endif

	Sleep(200);
	CloseBag();

	return count;
}

// ��ȡ�������Ʒ����
int Item::GetQuickYaoOrBaoNum(int& yaobao, int& yao, _account_* account)
{
	// 1060,858 596,793 611,808
	// 15,15 - 30,25
	account = account ? account : m_pGame->m_pGameProc->m_pAccount;
#if IS_READ_MEM == 0
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(account->Wnd.Game, 612, 810, 660, 823, 0, true);
	yaobao = m_pGame->m_pPrintScreen->LookNum(30, 0, 0, 0, 0xffffffff, 0x00000000);
	yao = m_pGame->m_pPrintScreen->LookNum(0, 15, 0, 0, 0xffffffff, 0x00000000);
#else
	int data[2] = {0, 0};
	m_pGame->m_pGameData->ReadMemory((PVOID)account->Addr.QuickItemNum, data, sizeof(data), account);
	yao = data[0];
	yaobao = data[1];
#endif
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
	// 1057,552 8,8 1065,560
	HBITMAP bitmap = m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 1057, 552, 1410, 690);
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
	// 1055,226 
	HBITMAP bitmap = m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_hWndBig, 1055, 226, 1410, 435);
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
