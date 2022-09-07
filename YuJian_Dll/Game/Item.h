#pragma once
#include "GameData.h"
#include <Windows.h>

enum ComImgIndex;
class Game;
class Item
{
public:
	Item(Game* p);

	// �򿪱���
	bool OpenBag(HWND hWndGame=0);
	// �رձ���
	void CloseBag(HWND hWnd=0);
	// ������
	void SetBag();
	// ��������
	bool SlideBag(int page);
	// �򿪲ֿ�
	void OpenStorage();
	// �����ֿ�
	bool SlideStorge(int page);
	// �رղֿ�
	void CloseStorage(HWND hWnd=NULL);
	// ����Ѫ
	void AddFullLife();
	// ʹ��ҩ
	void UseYao(int num=1, bool use_yaobao=true, int sleep_ms=500);
	// ʹ��ҩ����
	bool UseYaoBao();
	// ȥ�����ſ�
	void GoFBDoor();
	// ȥ�̵�
	void GoShop();
	// �ر��̵�
	void CloseShop();
	// ʹ�ø��������ҩ
	void UseLingYao();
	// �Ƿ���Ҫʹ��ҩ��
	bool IsNeedUseYaoBao();
	// �����Ƿ��
	bool BagIsOpen();
	// �����Ƿ���Ҫ��ҳ
	bool BagNeedPageDown();
	// ��ȡ������Ʒ
	void ReadBagItem(ITEM_TYPE items[], int length, _account_* account=nullptr);
	// �ֿ��Ƿ���Ҫ��ҳ
	bool StorageNeedPageDown();
	// ��ȡ�ֿ���Ʒ
	void ReadStorageItem(ITEM_TYPE items[], int length);
	// ��ȡ������Ʒ���λ��(index=��Ʒ����λ��)
	void GetBagClickPos(int index, int& click_x, int& click_y);
	// �����ť����
	bool PickUpItemByBtn();
	// ����
	int PickUpItem(const char* name, int x, int y, int x2, int y2, int pickup_num=10);
	// �ȴ�������Ʒ
	bool WaitForPickUpItem(DWORD wait_ms=3500, DWORD sleep_ms=500);
	// ��ȡ������Ʒ����
	int GetGroundItemPos(const char* name, int x, int y, int x2, int y2, int& pos_x, int& pos_y, char* pick_name);
	// ����ҩ��
	int  DropItem(DWORD* ms=nullptr);
	// ʹ����Ʒ
	void UseItem(const char* name, int x, int y);
	// ������Ʒ
	void DropItem(const char* name, int x, int y, int index=-1);
	// ������Ʒ
	int  SellItem(ConfItemInfo* items, DWORD length);
	// ������Ʒ
	void SellItem(const char* name, int x, int y);
	// ��ȡ��Ʒ������ťλ��
	void GetItemBtnPos(int& x, int& y, int index);
	// ����ֿ�
	void CheckIn();
	// ����ֿ�
	void CheckInOne(const char* name);
	// ȡ���ֿ�
	int CheckOut(ConfItemInfo* items, DWORD length, bool save_money=true);
	// ȡ��һ���ֿ���Ʒ
	int CheckOutOne(const char* name, bool open=false, bool close=false);
	// ����
	int TransactionItem();
	// ��ȡ������Ʒ����
	int GetBagItemCount(const char* name, int* klybys=nullptr, int* qiu=nullptr);
	// ��ȡ�������Ʒ����
	int GetQuickYaoOrBaoNum(int& yaobao, int& yao, _account_* account=nullptr);
	// �л������ܿ����
	void SwitchMagicQuickBar();
	// ������Ƿ����ǳ�֮��
	bool QuickBarIsXingChen();
	// �л������
	void SwitchQuickBar(int page);
	// ��ȡ��Ʒ��ComImgIndex
	ComImgIndex GetItemComImgIndex(const char* name);
	// ��ȡ����ͼƬ
	HBITMAP PrintBagImg(bool del=false);
	// ɾ������ͼƬ
	void    DeleteBagImg();
	// ��ȡ�ֿ�ͼƬ
	HBITMAP PrintStorageImg(bool del=false);
	// ɾ���ֿ�ͼƬ
	void    DeleteStorageImg();
public:
	// Game��
	Game* m_pGame;
};