#pragma once
#include "GameData.h"
#include <Windows.h>

#define MAX_CONF_ITEMS   32

class Game;
class GameConf
{
public:
	// ... 
	GameConf(Game* p);
	// 读取配置文件
	bool ReadConf(const char* path);
private:
	// 读取出征宠物
	void ReadPetOut(const char* data);
	// 读取捡拾物品
	void ReadPickUp(const char* data);
	// 读取使用物品
	void ReadUse(const char* data);
	// 读取丢弃物品
	void ReadDrop(const char* data);
	// 读取售卖物品
	void ReadSell(const char* data);
	// 读取存入物品
	void ReadCheckIn(const char* data);
	// 读取交易物品
	void ReadTransaction(const char* data);
	// 读取合法宝
	void ReadTrump(const char* data);
	// 读取摆摊
	void ReadBaiTan(const char* data);
	// 读取飞鸽子区服
	void ReadFGZSer(const char* data);
	// 读取其它设置
	void ReadSetting(const char* data);
public:
	// 转成物品类型
	int TransFormItemType(const char* name);
public:
	Game* m_pGame;

	// 出征宠物编号
	struct {
		DWORD No[3];
		DWORD Length;
	} m_stPetOut;

	// 捡拾物品列表
	struct {
		ConfItemInfo PickUps[MAX_CONF_ITEMS];
		DWORD        Length;
	} m_stPickUp;

	// 使用物品列表
	struct {
		ConfItemInfo Uses[MAX_CONF_ITEMS];
		DWORD        Length;
	} m_stUse;

	// 丢弃物品列表
	struct {
		ConfItemInfo Drops[MAX_CONF_ITEMS];
		DWORD        Length;
	} m_stDrop;

	// 售卖物品列表
	struct {
		ConfItemInfo Sells[MAX_CONF_ITEMS];
		DWORD        Length;
	} m_stSell;

	// 存入物品列表
	struct {
		ConfItemInfo CheckIns[MAX_CONF_ITEMS];
		DWORD        Length;
	} m_stCheckIn;

	// 交易物品列表
	struct {
		ConfItemInfo Transcation[MAX_CONF_ITEMS];
		DWORD        Length;
	} m_stTransaction;

	// 合成法宝列表
	struct {
		ConfItemInfo Trumps[MAX_CONF_ITEMS];
		DWORD        Length;
	} m_stTrump;

	// 合成法宝列表
	struct {
		ConfItemInfo BaiTans[MAX_CONF_ITEMS];
		DWORD        Length;
	} m_stBaiTan;

	// 飞鸽子区服列表
	struct {
		char  Ser[500][16];
		DWORD Index; 
		DWORD Length;
	} m_stFGZSer;

	// 其他设置
	struct {
		char FBFile[16][64]; // 副本流程文件
		int  FBFileCount;    // 共有多少文件
	} m_Setting;
};