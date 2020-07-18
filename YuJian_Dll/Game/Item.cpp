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

// 打开背包
bool Item::OpenBag()
{
	DbgPrint("打开背包\n");
	LOG2(L"打开背包", "c0");
	if (m_pGame->m_pButton->CheckButton(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_BAG_ITEM, "MPC物品栏"))
		return true;

	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_BAG);
	for (int i = 0; i < 2000; i += 100) {
		Sleep(100);
		if (m_pGame->m_pButton->CheckButton(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, 
			BUTTON_ID_BAG_ITEM, "MPC物品栏")) {
			Sleep(800);
			DbgPrint("背包已经打开\n");
			LOG2(L"背包已经打开", "c0");
			return true;
		}
	}
	return false;
}

// 关闭背包
void Item::CloseBag()
{
	DbgPrint("关闭背包\n");
	LOG2(L"关闭背包", "orange b");
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_CLOSEBAG);
}

// 整理背包
void Item::SetBag()
{
	DbgPrint("整理背包\n");
	LOG2(L"整理背包", "c0");
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_BAG_SET, "物品栏-排序按钮");
}

// 滑动背包
bool Item::SlideBag(int page)
{
	if (page <= 0)
		return false;

	LOGVARN2(32, "blue_r b", L"背包翻页(%d)", page);
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_BAG_PGDW, "down");
	return true;
}

// 打开仓库
void Item::OpenStorage()
{
	DbgPrint("打开仓库\n");
	LOG2(L"打开仓库", "c0");
	
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_VIP);
	for (int i = 0; i < 5; i++) {
		Sleep(800);
		if (m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_CHECKIN)) {
			Sleep(800);
			return;
		}
	}
}

// 滑动仓库
bool Item::SlideStorge(int page)
{
	if (page <= 0)
		return false;

	LOGVARN2(32, "blue_r b", L"仓库翻页(%d)", page);
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_CKIN_PGDW, "D");
	return true;
}

// 关闭仓库
void Item::CloseStorage()
{
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_CLOSECKIN);
}

// 加满血
void Item::AddFullLife()
{
	for (int i = 0; i < 6; i++) {
		m_pGame->m_pGameData->ReadLife();
		if ((m_pGame->m_pGameData->m_dwLife + 1000) >= m_pGame->m_pGameData->m_dwLifeMax)
			return;

		UseYao(1);
	}
	
}

// 使用药
void Item::UseYao(int num, bool use_yaobao, int sleep_ms)
{
	for (int i = 0; i < num; i++) {
		if (use_yaobao && IsNeedUseYaoBao()) {
			if (!UseYaoBao())
				break;
		}

		m_pGame->m_pButton->Key('1'); // 1键
		Sleep(sleep_ms);
	}
}

// 使用药包包
bool Item::UseYaoBao()
{
	int yaobao = 0, yao = 0;
	GetQuickYaoOrBaoNum(yaobao, yao);
	if (yaobao > 0) {
		m_pGame->m_pButton->Key('2'); // 2键
		Sleep(200);
	}
	
	return yaobao > 0;
}

// 去副本门口
void Item::GoFBDoor()
{
	m_pGame->m_pButton->Key('8'); // 9键
}

// 去副本门口
void Item::GoShop()
{
	m_pGame->m_pButton->Key('9'); // 8键
}

// 关闭商店
void Item::CloseShop()
{
	DbgPrint("关闭商店");
	LOG2(L"关闭商店", "c0");
	
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_CLOSESHOP);
}

// 使用复活宠物灵药
void Item::UseLingYao()
{
	DbgPrint("使用速效圣兽灵药\n");
	LOG2(L"使用速效圣兽灵药", "green");

	m_pGame->m_pButton->Key('3'); // 3键
	Sleep(300);
}

// 是否需要使用药包
bool Item::IsNeedUseYaoBao()
{
	int yao, yaobao;
	GetQuickYaoOrBaoNum(yaobao, yao);
	if (yao <= 3 && yaobao > 1)
		return true;

	return false;
}

// 背包是否打开
bool Item::BagIsOpen()
{
	return m_pGame->m_pButton->CheckButton(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_BAG_ITEM, "MPC物品栏");
}

// 背包是否需要翻页
bool Item::BagNeedPageDown()
{
#if IS_READ_MEM == 0
	int x_count, y_count;
	m_pGame->m_pPrintScreen->GetPixelConCount(x_count, y_count, 0xFF151310);
	return x_count < 20 || y_count < 10;
#else
	// 检查最后一个是否还有数据
	DWORD data = 0;
	m_pGame->m_pGameData->ReadDwordMemory(m_pGame->m_pGameProc->m_pAccount->Addr.Bag + 0x9C,
		data, m_pGame->m_pGameProc->m_pAccount);
	return data != 0;
#endif
}

// 获取背包物品
void Item::ReadBagItem(ITEM_TYPE items[], int length)
{
	m_pGame->m_pGameData->ReadMemory((PVOID)m_pGame->m_pGameProc->m_pAccount->Addr.Bag,
		items, length, m_pGame->m_pGameProc->m_pAccount);
}

// 仓库是否需要翻页
bool Item::StorageNeedPageDown()
{
	// 检查最后一个是否还有数据
	DWORD data = 0;
	m_pGame->m_pGameData->ReadDwordMemory(m_pGame->m_pGameProc->m_pAccount->Addr.Storage + 0xEC,
		data, m_pGame->m_pGameProc->m_pAccount);
	return data != 0;
}

// 获取仓库物品
void Item::ReadStorageItem(ITEM_TYPE items[], int length)
{
	m_pGame->m_pGameData->ReadMemory((PVOID)m_pGame->m_pGameProc->m_pAccount->Addr.Storage,
		items, length, m_pGame->m_pGameProc->m_pAccount);
}

// 获取背包物品点击位置(index=物品所在位置)
void Item::GetBagClickPos(int index, int& click_x, int& click_y)
{
	int x = 5, x2 = 20;  // 初始位置
	int y = 10, y2 = 20; // 初始位置

	x += (index % 10) * 35; // x每次增加35
	x2 += (index % 10) * 35; // x每次增加35

	y += (index / 10) * 34;  // y每次增加34
	y2 += (index / 10) * 34; // y每次增加34

	click_x = MyRand(x, x2);
	click_y = MyRand(y, y2);
}

// 点击按钮捡东西
bool Item::PickUpItemByBtn()
{
	LOG2(L"点击按钮拾取", "c3");
	//m_pGame->m_pEmulator->Tap(MyRand(26, 35), MyRand(656, 675));
	m_pGame->m_pButton->Key('7');
	return WaitForPickUpItem(6000, 1000);
}

// 捡物
int Item::PickUpItem(const char* name, int x, int y, int x2, int y2, int pickup_num)
{
	int count = 0;
	for (int i = 1; i <= pickup_num; i++) {
		char pick_name[64] = { 0 };
		int pos_x = 0, pos_y = 0;
		if (!GetGroundItemPos(name, x, y, x2, y2, pos_x, pos_y, pick_name))
			break;

		DbgPrint("%d.捡东西:%s(%d,%d)\n", i, pick_name, pos_x, pos_y);
		LOGVARN2(64, "c0", L"%d.捡东西:%hs(%d,%d)", i, pick_name, pos_x, pos_y);
		m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game,
			m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, pos_x, pos_y);
		WaitForPickUpItem();
		count++;

		Sleep(260);
	}

	return count;
}

// 等待捡起物品
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

// 获取地面物品坐标
int Item::GetGroundItemPos(const char* name, int x, int y, int x2, int y2, int& pos_x, int& pos_y, char* pick_name)
{
	if (!x || !y || !x2 || !y2) {
		x = 360;
		y = 90;
		x2 = 1098;
		y2 = 730;
	}

	wchar_t log[128];
	//LOGVARP2(log, "orange b", L"捡东西数量:%d (%d,%d)-(%d,%d)", 0, x, y, x2, y2);
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, x, y, x2, y2, 0, true);

	ConfItemInfo* items = m_pGame->m_pGameConf->m_stPickUp.PickUps; // 配置捡东西物品
	int length = m_pGame->m_pGameConf->m_stPickUp.Length; // 捡东西物品数量
	
	for (int i = 0; i < length; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			return 0;

		if (!strstr(name, ".") && strcmp(name, items[i].Name) != 0)
			continue;
		if (strcmp(name, "...") == 0 && strcmp("30星神兽碎片+3", items[i].Name) == 0)
			continue;
		if (strcmp(name, ".") == 0 || strcmp(name, ".nowait") == 0) { // 最后一关
			if (strcmp("30星神兽碎片+2", items[i].Name) != 0 && strcmp("30星神兽碎片+3", items[i].Name) != 0)
				continue;
		}
		if (strcmp(name, "速效治疗包") == 0) {
#if 0
			int yaobao = 0, yao = 0;
			GetQuickYaoOrBaoNum(yaobao, yao);
			if (yaobao >= 7) {
				LOGVARP2(log, "blue_r b", L"无须再俭药包, 数量:%d\n", yaobao);
				return 0;
			}	
#endif
		}

		strcpy(pick_name, items[i].Name);
		if (0 && strcmp(name, "速效治疗包") == 0 && !strstr(name, "."))
			LOGVARP2(log, "orange b", L"识别物品:%hs(%hs)", pick_name, items[i].Name);

		DWORD color = 0x00000000, diff = 0x00000000;
		DWORD color2 = 0x00000000, diff2 = 0x00000000;
		int tmp_x = 0, tmp_y = 0, tmp_x2 = 0, tmp_y2 = 0;
		if (strcmp("速效圣兽灵药", pick_name) == 0) {
			color = 0xFFBBD533; // 有2个
			diff = 0x000B0B0B;
		}
		else if (strcmp("6星O礼包", pick_name) == 0) {
			//color = 0xFFBDA392;
			diff = 0x00080808;
		}
		else if (strcmp("12星XO礼包", pick_name) == 0) {
			color = 0xFFE3E2F1; // 有10个
			diff = 0x00010101;
		}
		else if (strcmp("25星XO礼包", pick_name) == 0) {
			color = 0xFF667EBE;
			diff = 0x00010101;
		}
		else if (strcmp("30星神兽碎片+1", pick_name) == 0) {
			color = 0xFFF6BB8D; // 有4个
			diff = 0x00020202;
		}
		else if (strcmp("30星神兽碎片+2", pick_name) == 0) {
			color = 0xFF627BA3; // 有3个
			diff = 0x00020202;
		}
		else if (strcmp("30星神兽碎片+3", pick_name) == 0) {
			color = 0xFFBE76D9; // 有2个
			diff = 0x00080808;
		}
		else if (strcmp("幻魔晶石", pick_name) == 0) {
			color = 0xFFC52F96; // 有8个
			diff = 0x00080808;
		}
		else if (strcmp("魔魂晶石", pick_name) == 0) {
			color = 0xFF925B20; // 有5个
			diff = 0x00010101;
		}
		else if (strcmp("灵魂晶石", pick_name) == 0) {
			color = 0xFF91090B; // 有4个
			diff = 0x00020202;
		}
		else if (strcmp("速效治疗包", pick_name) == 0) {
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
			//LOGVARP2(log, "c0", L"发现物品:%hs, 坐标:%d,%d 坐标2:%d,%d", name, tmp_x, tmp_y, tmp_x2, tmp_y2);
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
	//DbgPrint("捡物截图:%s(%d,%d %d,%d)\n", name, x, y, x2, y2);
	
	return 0;
}

// 丢弃药包
int Item::DropItem(DWORD* ms)
{
	DWORD _tm = GetTickCount();

	OpenBag();
	Sleep(1000);

	bool is_use_item = false;
	int count = 0; // 数量
#if IS_READ_MEM == 0
	for (int i = 0; i < 2; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;

		if (SlideBag(i)) // 切换背包页面
			Sleep(1000);

		ConfItemInfo* items = m_pGame->m_pGameConf->m_stDrop.Drops;
		int length = m_pGame->m_pGameConf->m_stDrop.Length;
		int idx = 0;
		for (idx = 0; idx < length; idx++) {
			if (i > 0 && strcmp("紫色祝福碎片", items[idx].Name) == 0)
				continue;

			ComPoint cp[32];
			PrintBagImg(true); // 截取背包图片
			int num = m_pGame->m_pPrintScreen->ComparePixel(items[idx].Name, cp, sizeof(cp) / sizeof(ComPoint));
			if (num == 0)
				continue;
			if (num < 3 && strcmp(items[idx].Name, "钥匙") == 0) // 钥匙只有一把不能丢
				continue;
			if (num <= 8 && strcmp(items[idx].Name, "速效治疗包") == 0)
				continue;
			//m_pGame->m_pPrintScreen->SaveBitmapToFile(bitmap, L"C:\\Users\\fucan\\Desktop\\MNQ-9Star\\jt.bmp");
			//DbgPrint("%d.丢.%s.背包物品数量:%d\n", i + 1, drops[idx], num);

			int drop_num = 0;
			drop_num = strcmp(items[idx].Name, "钥匙") != 0 ? num : 1; // 钥匙只丢最前面那一把
			if (strcmp(items[idx].Name, "速效治疗包") == 0)
				drop_num = num - 8;

			for (int j = 0; j < drop_num; j++) {
				if (j == 0) {
					DropItem(items[idx].Name, cp[0].x, cp[0].y, 0);
				}
				else {
					ComPoint item[1];
					PrintBagImg(true); // 截取背包图片
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
				PrintBagImg(true); // 截取背包图片
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
						PrintBagImg(true); // 截取背包图片
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

		if (SlideBag(i)) // 滑动背包
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
	
	DbgPrint("丢弃物品用时:%.2f秒, %d毫秒\n", (float)_tm/1000.0f, _tm);
	LOGVARN2(64, "c0", L"丢弃物品用时:%d秒, %d毫秒", _tm / 1000, _tm);
	return count;
}

// 使用物品
void Item::UseItem(const char* name, int x, int y)
{
	DbgPrint("使用物品:%s(%d,%d)\n", name, x, y);
	LOGVARN2(64, "c0", L"使用物品:%hs(%d,%d)", name, x, y);
#if IS_READ_MEM == 0
#if ITEM_CLICK_PIC
	m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, x, y, 50, false);
#else
	m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		BUTTON_ID_BAG_ITEM, "MPC物品栏", x, y, 0xff, false);
#endif
#else
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		BUTTON_ID_BAG_ITEM, "MPC物品栏", x, y, 0xff, false);
#endif
#if 1
	if (m_pGame->m_pTalk->WaitTalkOpen(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic)) {
		m_pGame->m_pTalk->Select("活动.全部直接合成+1或+2", m_pGame->m_pGameProc->m_pAccount->Wnd.Pic);
		Sleep(500);
		m_pGame->m_pTalk->Select("活动.全部直接合成+1或+2.确定", m_pGame->m_pGameProc->m_pAccount->Wnd.Pic);
	}
#endif

	Sleep(500);
}

// 丢弃物品
void Item::DropItem(const char* name, int x, int y, int index)
{
	DbgPrint("丢物:%s(%d,%d)\n", name, x, y);
	LOGVARN2(64, "c0", L"丢物:%hs(%d,%d)", name, x, y);

#if IS_READ_MEM == 0
#if ITEM_CLICK_PIC
	m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, x, y);
#else
	m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		BUTTON_ID_BAG_ITEM, "MPC物品栏", x, y);
#endif
#else
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		BUTTON_ID_BAG_ITEM, "MPC物品栏", x, y);
#endif
	Sleep(100);

	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		MyRand(700, 999), MyRand(500, 735));
	Sleep(360);

	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		BUTTON_ID_TIPSURE, "确定");

	Sleep(500);
}

// 售卖物品
int Item::SellItem(ConfItemInfo* items, DWORD length)
{
	Sleep(1000);

#if IS_READ_MEM == 0
	for (int i = 0; i < 2; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;

		if (SlideBag(i)) // 切换背包页面
			Sleep(1000);

		for (DWORD iti = 0; iti < length; iti++) {
			for (int sell_i = 0; sell_i < 10; sell_i++) {
				ComPoint cp[32];
				PrintBagImg(true); // 截取背包图片
				int num = m_pGame->m_pPrintScreen->ComparePixel(items[iti].Name, cp, sizeof(cp) / sizeof(ComPoint));
				//DbgPrint("%d.售卖.背包物品数量:%d\n", i + 1, num);
				if (num == 0)
					break;

				for (int j = 0; j < num; j++) {
					if (j == 0) {
						SellItem(items[iti].Name, cp[0].x, cp[0].y);
					}
					else {
						ComPoint item[1];
						PrintBagImg(true); // 截取背包图片
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

		if (SlideBag(i)) // 滑动背包
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

// 售卖物品
void Item::SellItem(const char* name, int x, int y)
{
	DbgPrint("卖物品:%s %d,%d\n", name, x, y);
	LOGVARN2(64, "c0", L"卖物品:%hs(%d,%d)", name, x, y);

#if IS_READ_MEM == 0
#if ITEM_CLICK_PIC
	m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, x, y, 50, false);
#else
	m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		BUTTON_ID_BAG_ITEM, "MPC物品栏", x, y, 0xff, false);
#endif
#else
	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
		BUTTON_ID_BAG_ITEM, "MPC物品栏", x, y, 0xff, false);
#endif
	Sleep(360);

	m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, BUTTON_ID_TIPSURE, "确定");
	Sleep(500);
}

// 获取物品操作按钮位置
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

// 存入仓库
void Item::CheckIn()
{
	DbgPrint("存物\n");
	LOG2(L"存物", "c0");
	DWORD _tm = GetTickCount();

	OpenStorage();
	Sleep(1000);

#if IS_READ_MEM == 0
	for (int i = 0; i < 2; i++) {
		m_pGame->m_pGameProc->CloseTipBox();

		if (m_pGame->m_pGameProc->m_bPause)
			break;
		if (SlideBag(i)) // 切换背包页数
			Sleep(1000);

		bool is_save = false; // 是否存了东西
		ConfItemInfo* items = m_pGame->m_pGameConf->m_stCheckIn.CheckIns;
		int length = m_pGame->m_pGameConf->m_stCheckIn.Length;
		for (DWORD iti = 0; iti < length; iti++) {
			ComPoint cp[32];
			PrintBagImg(true); // 截取背包图片
			int num = m_pGame->m_pPrintScreen->ComparePixel(items[iti].Name, cp, sizeof(cp) / sizeof(ComPoint));
			//DbgPrint("%d.存物.背包物品数量:%d\n", i + 1, num);
			if (num > 0) {
				for (int j = 0; j < num; j++) {
					if (j == 0) {
#if ITEM_CLICK_PIC
						m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic, 
							cp[j].x, cp[j].y, 50, false);
#else
						m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
							BUTTON_ID_BAG_ITEM, "MPC物品栏", cp[j].x, cp[j].y, 0xff, false);
#endif
						
						LOGVARN2(64, "c0", L"%d.存物:%hs(%d,%d)", j + 1, items[iti].Name, cp[j].x, cp[j].y);
					}
					else {
						ComPoint item[1];
						PrintBagImg(true); // 截取背包图片
						if (!m_pGame->m_pPrintScreen->ComparePixel(items[iti].Name, item, sizeof(item) / sizeof(ComPoint)))
							break;

#if ITEM_CLICK_PIC
						m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
							item[0].x, item[0].y, 50, false);
#else
						m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
							BUTTON_ID_BAG_ITEM, "MPC物品栏", item[0].x, item[0].y, 0xff, false);
#endif
						DbgPrint("%d.存物.%s:(%d,%d)\n", j + 1, items[iti].Name, item[0].x, item[0].y);
						LOGVARN2(64, "c0", L"%d.存物:%hs(%d,%d)", j + 1, items[iti].Name, item[0].x, item[0].y);
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

		if (SlideBag(i)) // 滑动背包
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

					LOGVARN2(64, "c0", L"存物:%hs %d,%d(%d)", items[idx].Name, click_x, click_y, n);
					m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
						BUTTON_ID_BAG_ITEM, "MPC物品栏", click_x, click_y, 0xff, false);
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
	DbgPrint("存物用时:%.2f秒, %d毫秒\n", (float)_tm / 1000.0f, _tm);
	LOGVARN2(64, "c0", L"存物用时:%d秒, %d毫秒", _tm / 1000, _tm);
}

// 取出仓库
int Item::CheckOut(ConfItemInfo* items, DWORD length)
{
	int count = 0;
	DbgPrint("取物\n");
	LOG2(L"取物", "c0");
	DWORD _tm = GetTickCount();

	OpenStorage();
	Sleep(1000);


	HWND game = m_pGame->m_pGameProc->m_pAccount->Wnd.Game;
	HWND pic = m_pGame->m_pGameProc->m_pAccount->Wnd.Pic;

#if IS_READ_MEM == 0
	for (int i = 0; i < 2; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;
		if (SlideStorge(i)) { // 滑动仓库啊
			Sleep(1000);
		}

		for (DWORD iti = 0; iti < length; iti++) {
			ComPoint cp[32];
			PrintStorageImg(true); // 截取仓库图片
			int num = m_pGame->m_pPrintScreen->ComparePixel(items[iti].Name, cp, sizeof(cp) / sizeof(ComPoint));
			count += num;
			//DbgPrint("%d.取物.仓库物品数量:%d\n", i + 1, num);
			if (num > 0) {
				for (int j = 0; j < num; j++) {
					if (j == 0) {
						LOGVARN2(64, "c0", L"取物:%hs %d,%d", items[iti].Name, cp[0].x, cp[0].y);
#if ITEM_CLICK_PIC
						m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
							cp[0].x, cp[0].y, 50, false);
#else
						m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
							BUTTON_ID_CKIN_ITEM, "存储物品栏", cp[0].x, cp[0].y, 0xff, false);
#endif
					}
					else {
						ComPoint item[1];
						PrintStorageImg(true); // 截取仓库图片
						if (!m_pGame->m_pPrintScreen->ComparePixel(items[iti].Name, item, sizeof(item) / sizeof(ComPoint)))
							break;

						LOGVARN2(64, "c0", L"取物:%hs %d,%d", items[iti].Name, item[0].x, item[0].y);
#if ITEM_CLICK_PIC
						m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
							item[0].x, item[0].y, 50, false);
#else
						m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
							BUTTON_ID_CKIN_ITEM, "存储物品栏", item[0].x, item[0].y, 0xff, false);
#endif
					}
					Sleep(600);
				}
			}
		}
		if (m_pGame->m_pPrintScreen->ComparePixel("没有物品", nullptr, 1))
			break;
	}
#else
	for (int i = 0; i <= 5; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;

		if (SlideStorge(i)) // 滑动仓库页码
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

					LOGVARN2(64, "c0", L"取物:%hs %d,%d(%d)", items[idx].Name, click_x, click_y, n);
					m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
						BUTTON_ID_CKIN_ITEM, "存储物品栏", click_x, click_y, 0xff, false);
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
	LOG2(L"存钱.", "orange");
	m_pGame->m_pButton->Click(game, BUTTON_ID_SAVE_MNY, "取");
	Sleep(800);
	m_pGame->m_pButton->ClickPic(game, pic, MyRand(1216, 1286), MyRand(300, 303), 300);
	char money[] = {'5', '0', '0', '0', '0', '0', '0', '0', 0};
	for (int i = 0; money[i]; i++) {
		m_pGame->m_pButton->Key(money[i]);
		Sleep(260);
	}
	m_pGame->m_pButton->Click(game, BUTTON_ID_CUNQIAN, "存");
	Sleep(300);
	LOG2(L"存钱完成.", "orange");
#endif

	CloseStorage();
	Sleep(300);

	_tm = GetTickCount() - _tm;
	DbgPrint("取物用时:%.2f秒, %d毫秒\n", (float)_tm / 1000.0f, _tm);
	LOGVARN2(64, "c0", L"取物用时:%d秒, %d毫秒", _tm / 1000, _tm);

	return count;
}

// 取出一个仓库物品
int Item::CheckOutOne(const char * name, bool open, bool close)
{
	DbgPrint("取物\n");
	LOG2(L"取物", "c0");

	ITEM_TYPE type = (ITEM_TYPE)m_pGame->m_pGameConf->TransFormItemType(name);
	if (type == 未知物品) {
		DbgPrint("未知物品.\n");
		LOG2(L"未知物品.", "c0");
		return 0;
	}
		
	if (open) {
		OpenStorage();
		Sleep(1000);
	}

	int count = 0;

#if IS_READ_MEM == 0
	for (int i = 0; i < 2; i++) {
		if (SlideStorge(i)) { // 滑动仓库
			Sleep(1000);
		}

		ComPoint cp[1];
		PrintStorageImg(true); // 截取仓库图片
		if (m_pGame->m_pPrintScreen->ComparePixel(name, cp, 1)) {
			LOGVARN2(64, "c0", L"取物:%hs %d,%d", name, cp[0].x, cp[0].y);
#if 0
			m_pGame->m_pButton->ClickPic(m_pGame->m_pGameProc->m_pAccount->Wnd.Game, m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
				cp[0].x, cp[0].y, 50, false);
#else
			m_pGame->m_pButton->ClickRel(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
				BUTTON_ID_CKIN_ITEM, "存储物品栏", cp[0].x, cp[0].y, 0xff, false);
#endif
			Sleep(600);
			count = 1;
		}

		if (m_pGame->m_pPrintScreen->ComparePixel("没有物品", nullptr, 1))
			break;
	}
#else	
	for (int i = 0; i <= 5; i++) {
		if (m_pGame->m_pGameProc->m_bPause)
			break;

		if (SlideStorge(i)) // 滑动仓库页码
			Sleep(1000);

		ITEM_TYPE storage_items[60];
		ReadStorageItem(storage_items, sizeof(storage_items));
		for (int n = 0; n < sizeof(storage_items) / sizeof(ITEM_TYPE); n++) {
			if (storage_items[n] && storage_items[n] == type) {
				int click_x, click_y;
				GetBagClickPos(n, click_x, click_y);

				LOGVARN2(64, "c0", L"取物:%hs %d,%d(%d)", name, click_x, click_y, n);
				m_pGame->m_pButton->Click(m_pGame->m_pGameProc->m_pAccount->Wnd.Pic,
					BUTTON_ID_CKIN_ITEM, "存储物品栏", click_x, click_y, 0xff, false);
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

	DbgPrint("取物完成.\n");
	LOG2(L"取物完成.", "c0");
	return count;
}

// 获取背包物品数量
int Item::GetBagItemCount(const char* name)
{
	int count = 0;
	OpenBag();
	Sleep(1000);

	DbgPrint("获取背包物品数量:%s\n", name);
#if IS_READ_MEM == 0
	for (int i = 0; i < 2; i++) {
		if (SlideBag(i)) { // 滑动背包
			Sleep(300);
		}

		PrintBagImg(true); // 截取背包图片
		int num = m_pGame->m_pPrintScreen->ComparePixel(name);
		//DbgPrint("%d.背包物品数量:%d\n", i + 1, num);
		//LOGVARN2(64, "c0", L"%d.背包物品数量:%d", i + 1, num);

		count += num;
		if (count > 0 && num == 0) // 后面已没物品
			break;

		if (m_pGame->m_pPrintScreen->ComparePixel("没有物品", nullptr, 1))
			break;
	}
#else
	ITEM_TYPE type = (ITEM_TYPE)m_pGame->m_pGameConf->TransFormItemType(name);
	for (int i = 0; i < 5; i++) {
		if (SlideBag(i)) // 滑动背包
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

// 获取快捷栏物品数量
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
	//DbgPrint("药包数量:%d 药数量:%d\n", yaobao, yao);

	return 0;
}

// 切换到技能快捷栏
void Item::SwitchMagicQuickBar()
{
#if 0
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_hWndBig, 1235, 350, 1245, 360, 0, true);
	if (m_pGame->m_pPrintScreen->ComparePixel("快捷栏切换按钮", nullptr, 1) == 0) { // 可能是XP技能栏
		DbgPrint("快捷栏切换按钮\n");
		SwitchQuickBar(1);
		Sleep(500);
	}
#endif

	if (QuickBarIsXingChen()) { // 切换
		SwitchQuickBar(1);
		Sleep(500);
	}
}

// 快捷栏是否有星辰之眼
bool Item::QuickBarIsXingChen()
{
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_pGame->m_pGameProc->m_hWndGame, 926, 510, 936, 520, 0, true);
	return m_pGame->m_pPrintScreen->ComparePixel("星辰之眼快捷栏", nullptr, 1) > 0;
}

// 切换快捷栏
void Item::SwitchQuickBar(int page)
{
	DbgPrint("切换到快捷栏:%d\n", page);
	m_pGame->m_pGameProc->Click(1235, 360, 1250, 370);
}

// 获取物品的ComImgIndex
ComImgIndex Item::GetItemComImgIndex(const char * name)
{
	if (strcmp(name, "双子星青螭礼包") == 0)
		return CIN_QingChi;
	if (strcmp(name, "双子星紫冥礼包") == 0)
		return CIN_ZiMing;
	if (strcmp(name, "白羊星卡迪礼包") == 0)
		return CIN_KaDi;
	if (strcmp(name, "白羊星穆巴礼包") == 0)
		return CIN_MuBa;
	if (strcmp(name, "金牛星亚尔礼包") == 0)
		return CIN_YaEr;
	if (strcmp(name, "金牛星鲁迪礼包") == 0)
		return CIN_LuDi;
	if (strcmp(name, "幻魔晶石") == 0)
		return CIN_HuanMo;
	if (strcmp(name, "魔魂晶石") == 0)
		return CIN_MoHun;
	if (strcmp(name, "灵魂晶石") == 0)
		return CIN_LingHun;
	if (strcmp(name, "爱娜的项链") == 0)
		return CIN_XiangLian;

	return CIN_NoItem;
}

// 截取背包图片
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

// 删除背包图片
void Item::DeleteBagImg()
{
	m_pGame->m_pPrintScreen->Release();
}

// 截取仓库图片
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

// 删除仓库图片
void Item::DeleteStorageImg()
{
	m_pGame->m_pPrintScreen->Release();
}
