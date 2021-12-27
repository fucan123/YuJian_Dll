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
	// ��ȡ�����ļ�
	bool ReadConf(const char* path);
private:
	// ��ȡ��������
	void ReadPetOut(const char* data);
	// ��ȡ��ʰ��Ʒ
	void ReadPickUp(const char* data);
	// ��ȡʹ����Ʒ
	void ReadUse(const char* data);
	// ��ȡ������Ʒ
	void ReadDrop(const char* data);
	// ��ȡ������Ʒ
	void ReadSell(const char* data);
	// ��ȡ������Ʒ
	void ReadCheckIn(const char* data);
	// ��ȡ������Ʒ
	void ReadTransaction(const char* data);
	// ��ȡ�Ϸ���
	void ReadTrump(const char* data);
	// ��ȡ��������
	void ReadSetting(const char* data);
public:
	// ת����Ʒ����
	int TransFormItemType(const char* name);
public:
	Game* m_pGame;

	// ����������
	struct {
		DWORD No[3];
		DWORD Length;
	} m_stPetOut;

	// ��ʰ��Ʒ�б�
	struct {
		ConfItemInfo PickUps[MAX_CONF_ITEMS];
		DWORD        Length;
	} m_stPickUp;

	// ʹ����Ʒ�б�
	struct {
		ConfItemInfo Uses[MAX_CONF_ITEMS];
		DWORD        Length;
	} m_stUse;

	// ������Ʒ�б�
	struct {
		ConfItemInfo Drops[MAX_CONF_ITEMS];
		DWORD        Length;
	} m_stDrop;

	// ������Ʒ�б�
	struct {
		ConfItemInfo Sells[MAX_CONF_ITEMS];
		DWORD        Length;
	} m_stSell;

	// ������Ʒ�б�
	struct {
		ConfItemInfo CheckIns[MAX_CONF_ITEMS];
		DWORD        Length;
	} m_stCheckIn;

	// ������Ʒ�б�
	struct {
		ConfItemInfo Transcation[MAX_CONF_ITEMS];
		DWORD        Length;
	} m_stTransaction;

	// �ϳɷ����б�
	struct {
		ConfItemInfo Trumps[MAX_CONF_ITEMS];
		DWORD        Length;
	} m_stTrump;

	// ��������
	struct {
		char FBFile[16][64]; // ���������ļ�
		int  FBFileCount;    // ���ж����ļ�
	} m_Setting;
};