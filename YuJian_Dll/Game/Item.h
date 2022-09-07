#pragma once
#include "GameData.h"
#include <Windows.h>

enum ComImgIndex;
class Game;
class Item
{
public:
	Item(Game* p);

	// 打开背包
	bool OpenBag(HWND hWndGame=0);
	// 关闭背包
	void CloseBag(HWND hWnd=0);
	// 整理背包
	void SetBag();
	// 滑动背包
	bool SlideBag(int page);
	// 打开仓库
	void OpenStorage();
	// 滑动仓库
	bool SlideStorge(int page);
	// 关闭仓库
	void CloseStorage(HWND hWnd=NULL);
	// 加满血
	void AddFullLife();
	// 使用药
	void UseYao(int num=1, bool use_yaobao=true, int sleep_ms=500);
	// 使用药包包
	bool UseYaoBao();
	// 去副本门口
	void GoFBDoor();
	// 去商店
	void GoShop();
	// 关闭商店
	void CloseShop();
	// 使用复活宠物灵药
	void UseLingYao();
	// 是否需要使用药包
	bool IsNeedUseYaoBao();
	// 背包是否打开
	bool BagIsOpen();
	// 背包是否需要翻页
	bool BagNeedPageDown();
	// 获取背包物品
	void ReadBagItem(ITEM_TYPE items[], int length, _account_* account=nullptr);
	// 仓库是否需要翻页
	bool StorageNeedPageDown();
	// 获取仓库物品
	void ReadStorageItem(ITEM_TYPE items[], int length);
	// 获取背包物品点击位置(index=物品所在位置)
	void GetBagClickPos(int index, int& click_x, int& click_y);
	// 点击按钮捡东西
	bool PickUpItemByBtn();
	// 捡物
	int PickUpItem(const char* name, int x, int y, int x2, int y2, int pickup_num=10);
	// 等待捡起物品
	bool WaitForPickUpItem(DWORD wait_ms=3500, DWORD sleep_ms=500);
	// 获取地面物品坐标
	int GetGroundItemPos(const char* name, int x, int y, int x2, int y2, int& pos_x, int& pos_y, char* pick_name);
	// 丢弃药包
	int  DropItem(DWORD* ms=nullptr);
	// 使用物品
	void UseItem(const char* name, int x, int y);
	// 丢弃物品
	void DropItem(const char* name, int x, int y, int index=-1);
	// 售卖物品
	int  SellItem(ConfItemInfo* items, DWORD length);
	// 售卖物品
	void SellItem(const char* name, int x, int y);
	// 获取物品操作按钮位置
	void GetItemBtnPos(int& x, int& y, int index);
	// 存入仓库
	void CheckIn();
	// 存入仓库
	void CheckInOne(const char* name);
	// 取出仓库
	int CheckOut(ConfItemInfo* items, DWORD length, bool save_money=true);
	// 取出一个仓库物品
	int CheckOutOne(const char* name, bool open=false, bool close=false);
	// 交易
	int TransactionItem();
	// 获取背包物品数量
	int GetBagItemCount(const char* name, int* klybys=nullptr, int* qiu=nullptr);
	// 获取快捷栏物品数量
	int GetQuickYaoOrBaoNum(int& yaobao, int& yao, _account_* account=nullptr);
	// 切换到技能快捷栏
	void SwitchMagicQuickBar();
	// 快捷栏是否有星辰之眼
	bool QuickBarIsXingChen();
	// 切换快捷栏
	void SwitchQuickBar(int page);
	// 获取物品的ComImgIndex
	ComImgIndex GetItemComImgIndex(const char* name);
	// 截取背包图片
	HBITMAP PrintBagImg(bool del=false);
	// 删除背包图片
	void    DeleteBagImg();
	// 截取仓库图片
	HBITMAP PrintStorageImg(bool del=false);
	// 删除仓库图片
	void    DeleteStorageImg();
public:
	// Game类
	Game* m_pGame;
};