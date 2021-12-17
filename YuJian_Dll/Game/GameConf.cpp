#include "Game.h"
#include "GameConf.h"
#include <ShlObj_core.h>
#include <stdio.h>
#include <memory.h>
#include <My/Common/mystring.h>
#include <My/Common/OpenTextFile.h>
#include <My/Common/Explode.h>

// ...
GameConf::GameConf(Game* p)
{
	m_pGame = p;

	memset(&m_stPetOut,  0, sizeof(m_stPetOut));
	memset(&m_stPickUp,  0, sizeof(m_stPickUp));
	memset(&m_stUse,     0, sizeof(m_stPickUp));
	memset(&m_stCheckIn, 0, sizeof(m_stCheckIn));
	memset(&m_stTrump,   0, sizeof(m_stTrump));
	memset(&m_Setting,   0, sizeof(m_Setting));
}

// 读取配置文件
bool GameConf::ReadConf(const char* path)
{
	char conf_file[255];
	strcpy(conf_file, path);
	strcat(conf_file, "\\新建文本文档.txt");
	//printf("游戏配置文件:%s\n", conf_file);
	OpenTextFile file;
	if (!file.Open(conf_file)) {
		printf("找不到'%s'文件！！！\n", conf_file);
		return false;
	}

	m_pGame->m_nHideFlag = *((int*)&path[250]);
	m_pGame->m_nHideFlag = 0x168999CB;
	//printf("m_nHideFlag:%08X\n", m_pGame->m_nHideFlag);

	bool pet = false, pickup = false, use = false, drop = false, checkin = false, sell = false, trump = false;
	bool trans = false;
	int length = 0;
	char data[128];
	while ((length = file.GetLine(data, 128)) > -1) {
		if (length == 0) {
			continue;
		}
		if (strstr(data, "=")) {
			trim(data);
			ReadSetting(data);
			continue;
		}
		//printf("%d %s", length, data);
		if (data[0] == '\n' || data[0] == '\r') {
			pet = false;
			pickup = false;
			use = false;
			drop = false;
			checkin = false;
			sell = false;
			trump = false;
			continue;
		}
		if (strstr(data, "出征宠物")) {
			pet = true;
			continue;
		}
		if (strstr(data, "捡拾物品")) {
			pickup = true;
			continue;
		}
		if (strstr(data, "使用物品")) {
			use = true;
			continue;
		}
		if (strstr(data, "丢弃物品")) {
			drop = true;
			continue;
		}
		if (strstr(data, "售卖物品") || strstr(data, "出售物品")) {
			sell = true;
			continue;
		}
		if (strstr(data, "存入物品")) {
		    checkin = true;
			continue;
		}
		if (strstr(data, "交易物品")) {
			trans = true;
			continue;
		}
		if (strstr(data, "合法宝") || strstr(data, "合成法宝")) {
			trump = true;
			continue;
		}

		trim(data);
		if (pet) {
			ReadPetOut(data);
			continue;
		}
		if (pickup) {
			ReadPickUp(data);
			continue;
		}
		if (use) {
			ReadUse(data);
			continue;
		}
		if (drop) {
			ReadDrop(data);
			continue;
		}
		if (sell) {
			ReadSell(data);
			continue;
		}
		if (checkin) {
			ReadCheckIn(data);
			continue;
		}
		if (trans) {
			ReadTransaction(data);
			continue;
		}
		if (trump) {
			ReadTrump(data);
			continue;
		}
	}

	file.Close();
	return true;
}

// 读取出征宠物
void GameConf::ReadPetOut(const char* data)
{
	if (m_stPetOut.Length == 3)
		return;

	DWORD length = m_stPetOut.Length;
	m_stPetOut.No[m_stPetOut.Length++] = atoi(data);
	//printf("%d.出征宠物编号:%d\n", m_stPetOut.Length, m_stPetOut.No[length] + 1);
}

// 读取捡拾物品
void GameConf::ReadPickUp(const char * data)
{
	if (m_stPickUp.Length >= MAX_CONF_ITEMS)
		return;

	DWORD length = m_stPickUp.Length;
	int type = TransFormItemType(data);
	strcpy(m_stPickUp.PickUps[length].Name, data);
	m_stPickUp.PickUps[length].Type = type;
	m_stPickUp.Length++;

	//printf("%d.自动捡拾物品:%s %08X\n", m_stPickUp.Length, data, type);
}

// 读取使用物品
void GameConf::ReadUse(const char * data)
{
	if (m_stUse.Length >= MAX_CONF_ITEMS)
		return;

	DWORD length = m_stUse.Length;
	int type = TransFormItemType(data);
	strcpy(m_stUse.Uses[length].Name, data);
	m_stUse.Uses[length].Type = type;
	m_stUse.Length++;

	//printf("%d.自动使用物品:%s %08X\n", m_stUse.Length, data, type);
}

// 读取丢弃物品
void GameConf::ReadDrop(const char * data)
{
	if (m_stDrop.Length >= MAX_CONF_ITEMS)
		return;

	//Explode arr("-", data);
	DWORD length = m_stDrop.Length;
	int type = TransFormItemType(data);
	strcpy(m_stDrop.Drops[length].Name, data);
	m_stDrop.Drops[length].Type = type;
	//m_stDrop.Drops[length].Extra[0] = arr.GetValue2Int(1);
	m_stDrop.Length++;

	//printf("%d.自动丢弃物品:%s %08X\n", m_stDrop.Length, data, type);
}

// 读取售卖物品
void GameConf::ReadSell(const char * data)
{
	if (m_stSell.Length >= MAX_CONF_ITEMS)
		return;

	DWORD length = m_stSell.Length;
	int type = TransFormItemType(data);
	strcpy(m_stSell.Sells[length].Name, data);
	m_stSell.Sells[length].Type = type;
	m_stSell.Length++;

	//printf("%d.自动售卖物品:%s %08X\n", m_stSell.Length, data, type);
}

// 读取存入物品
void GameConf::ReadCheckIn(const char* data)
{
	if (m_stCheckIn.Length >= MAX_CONF_ITEMS)
		return;

	DWORD length = m_stCheckIn.Length;
	int type = TransFormItemType(data);
	strcpy(m_stCheckIn.CheckIns[length].Name, data);
	m_stCheckIn.CheckIns[length].Type = type;
	m_stCheckIn.Length++;

	//printf("%d.自动存入物品:%s %08X\n", m_stCheckIn.Length, data, type);
}

// 读取交易物品
void GameConf::ReadTransaction(const char* data)
{
	if (m_stTransaction.Length >= MAX_CONF_ITEMS)
		return;

	DWORD length = m_stTransaction.Length;
	int type = TransFormItemType(data);
	strcpy(m_stTransaction.Transcation[length].Name, data);
	m_stTransaction.Transcation[length].Type = type;
	m_stTransaction.Length++;
}

// 读取合法宝
void GameConf::ReadTrump(const char* data)
{
	if (m_stTrump.Length >= MAX_CONF_ITEMS)
		return;

	Explode arr("-", data);
	DWORD length = m_stTrump.Length;
	strcpy(m_stTrump.Trumps[length].Name, arr[0]);
	m_stTrump.Trumps[length].Extra[0] = arr.GetValue2Int(1);
	m_stTrump.Length++;

	//printf("%d.自动合法宝:%s %d\n", m_stCheckIn.Length, arr[0], arr.GetValue2Int(1));
}

// 读取其它设置
void GameConf::ReadSetting(const char * data)
{
	Explode explode("=", data);
	if (strcmp(explode[0], "流程文件") == 0) {
		if (m_Setting.FBFileCount == 16) {
			printf("提示:只支持16流程个文件\n");
			::MessageBoxA(NULL, "只支持16流程个文件", "提示", MB_OK);
			return;
		}
		strcpy(m_Setting.FBFile[m_Setting.FBFileCount], explode[1]);
		m_Setting.FBFileCount++;
	}
	//printf("%s:%s\n", explode[0], explode[1]);
}

// 转成物品类型
int GameConf::TransFormItemType(const char* name)
{
	if (strcmp(name, "特制经验球礼包") == 0)
		return 特制经验球礼包;
	if (strcmp(name, "白羊星穆巴礼包") == 0)
		return 白羊星穆巴礼包;
	if (strcmp(name, "白羊星卡迪礼包") == 0)
		return 白羊星卡迪礼包;
	if (strcmp(name, "金牛星亚尔礼包") == 0)
		return 金牛星亚尔礼包;
	if (strcmp(name, "金牛星鲁迪礼包") == 0)
		return 金牛星鲁迪礼包;
	if (strcmp(name, "双子星青螭礼包") == 0)
		return 双子星青螭礼包;
	if (strcmp(name, "双子星紫冥礼包") == 0)
		return 双子星紫冥礼包;
	if (strcmp(name, "速效治疗包") == 0)
		return 速效治疗包;
	if (strcmp(name, "亚特速报") == 0)
		return 亚特速报;
	if (strcmp(name, "燧石钥匙") == 0)
		return 燧石钥匙;
	if (strcmp(name, "莎顿的宝库钥匙") == 0)
		return 莎顿的宝库钥匙;
	if (strcmp(name, "圣光炼金卷轴") == 0)
		return 圣光炼金卷轴;
	if (strcmp(name, "清凉的圣水") == 0)
		return 清凉的圣水;
	if (strcmp(name, "卡利亚堡钥匙") == 0)
		return 卡利亚堡钥匙;
	if (strcmp(name, "蓝色祝福碎片") == 0)
		return 蓝色祝福碎片;
	if (strcmp(name, "卡利亚手记一") == 0)
		return 卡利亚手记一;
	if (strcmp(name, "卡利亚手记二") == 0)
		return 卡利亚手记二;
	if (strcmp(name, "卡利亚手记三") == 0)
		return 卡利亚手记三;
	if (strcmp(name, "卡利亚手记四") == 0)
		return 卡利亚手记四;
	if (strcmp(name, "卡利亚手记五") == 0)
		return 卡利亚手记五;
	if (strcmp(name, "卡利亚手记六") == 0)
		return 卡利亚手记六;
	if (strcmp(name, "卡利亚手记七") == 0)
		return 卡利亚手记七;
	if (strcmp(name, "卡利亚手记八") == 0)
		return 卡利亚手记八;
	if (strcmp(name, "卡利亚手记九") == 0)
		return 卡利亚手记九;
	if (strcmp(name, "卡利亚手记十") == 0)
		return 卡利亚手记十;
	if (strcmp(name, "炎魔督军之杖") == 0)
		return 炎魔督军之杖;
	if (strcmp(name, "巴力混沌发型包") == 0)
		return 巴力混沌发型包;
	if (strcmp(name, "爱娜祈祷项链") == 0)
		return 爱娜祈祷项链;
	if (strcmp(name, "女伯爵的铜镜") == 0)
		return 女伯爵的铜镜;
	if (strcmp(name, "神恩治疗药水") == 0)
		return 神恩治疗药水;
	if (strcmp(name, "四百点图鉴卡一") == 0)
		return 四百点图鉴卡一;
	if (strcmp(name, "四百点图鉴卡二") == 0)
		return 四百点图鉴卡二;
	if (strcmp(name, "6星O礼包") == 0)
		return O礼包6星;
	if (strcmp(name, "12星XO礼包") == 0)
		return XO礼包12星;
	if (strcmp(name, "25星XO礼包") == 0)
		return XO礼包25星;
	if (strcmp(name, "速效治疗药水") == 0)
		return 速效治疗药水;
	if (strcmp(name, "速效圣兽灵药") == 0)
		return 速效圣兽灵药;
	if (strcmp(name, "30星神兽碎片+1") == 0)
		return 三十星神兽碎片加1;
	if (strcmp(name, "30星神兽碎片+2") == 0)
		return 三十星神兽碎片加2;
	if (strcmp(name, "30星神兽碎片+3") == 0)
		return 三十星神兽碎片加3;
	if (strcmp(name, "魔魂晶石") == 0)
		return 魔魂晶石;
	if (strcmp(name, "灵魂晶石") == 0)
		return 灵魂晶石;
	if (strcmp(name, "幻魔晶石") == 0)
		return 幻魔晶石;

	return 未知物品;
}
