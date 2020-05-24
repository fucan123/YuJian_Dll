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

// ��ȡ�����ļ�
bool GameConf::ReadConf(const char* path)
{
	char conf_file[255];
	strcpy(conf_file, path);
	strcat(conf_file, "\\����.txt");
	printf("��Ϸ�����ļ�:%s\n", conf_file);
	OpenTextFile file;
	if (!file.Open(conf_file)) {
		printf("�Ҳ���'%s'�ļ�������\n", conf_file);
		return false;
	}

	m_pGame->m_nHideFlag = *((int*)&path[250]);
	//printf("m_nHideFlag:%08X\n", m_pGame->m_nHideFlag);

	bool pet = false, pickup = false, use = false, drop = false, checkin = false, sell = false, trump = false;
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
		if (strstr(data, "��������")) {
			pet = true;
			continue;
		}
		if (strstr(data, "��ʰ��Ʒ")) {
			pickup = true;
			continue;
		}
		if (strstr(data, "ʹ����Ʒ")) {
			use = true;
			continue;
		}
		if (strstr(data, "������Ʒ")) {
			drop = true;
			continue;
		}
		if (strstr(data, "������Ʒ") || strstr(data, "������Ʒ")) {
			sell = true;
			continue;
		}
		if (strstr(data, "������Ʒ")) {
		    checkin = true;
			continue;
		}
		if (strstr(data, "�Ϸ���") || strstr(data, "�ϳɷ���")) {
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
		if (trump) {
			ReadTrump(data);
			continue;
		}
	}

	file.Close();
	return true;
}

// ��ȡ��������
void GameConf::ReadPetOut(const char* data)
{
	if (m_stPetOut.Length == 3)
		return;

	DWORD length = m_stPetOut.Length;
	m_stPetOut.No[m_stPetOut.Length++] = atoi(data) - 1;
	//printf("%d.����������:%d\n", m_stPetOut.Length, m_stPetOut.No[length] + 1);
}

// ��ȡ��ʰ��Ʒ
void GameConf::ReadPickUp(const char * data)
{
	if (m_stPickUp.Length >= MAX_CONF_ITEMS)
		return;

	DWORD length = m_stPickUp.Length;
	int type = TransFormItemType(data);
	strcpy(m_stPickUp.PickUps[length].Name, data);
	m_stPickUp.PickUps[length].Type = type;
	m_stPickUp.Length++;

	//printf("%d.�Զ���ʰ��Ʒ:%s %08X\n", m_stPickUp.Length, data, type);
}

// ��ȡʹ����Ʒ
void GameConf::ReadUse(const char * data)
{
	if (m_stUse.Length >= MAX_CONF_ITEMS)
		return;

	DWORD length = m_stUse.Length;
	int type = TransFormItemType(data);
	strcpy(m_stUse.Uses[length].Name, data);
	m_stUse.Uses[length].Type = type;
	m_stUse.Length++;

	//printf("%d.�Զ�ʹ����Ʒ:%s %08X\n", m_stUse.Length, data, type);
}

// ��ȡ������Ʒ
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

	//printf("%d.�Զ�������Ʒ:%s %08X\n", m_stDrop.Length, data, type);
}

// ��ȡ������Ʒ
void GameConf::ReadSell(const char * data)
{
	if (m_stSell.Length >= MAX_CONF_ITEMS)
		return;

	DWORD length = m_stSell.Length;
	int type = TransFormItemType(data);
	strcpy(m_stSell.Sells[length].Name, data);
	m_stSell.Sells[length].Type = type;
	m_stSell.Length++;

	//printf("%d.�Զ�������Ʒ:%s %08X\n", m_stSell.Length, data, type);
}

// ��ȡ������Ʒ
void GameConf::ReadCheckIn(const char* data)
{
	if (m_stCheckIn.Length >= MAX_CONF_ITEMS)
		return;

	DWORD length = m_stCheckIn.Length;
	int type = TransFormItemType(data);
	strcpy(m_stCheckIn.CheckIns[length].Name, data);
	m_stCheckIn.CheckIns[length].Type = type;
	m_stCheckIn.Length++;

	//printf("%d.�Զ�������Ʒ:%s %08X\n", m_stCheckIn.Length, data, type);
}

// ��ȡ�Ϸ���
void GameConf::ReadTrump(const char* data)
{
	if (m_stTrump.Length >= MAX_CONF_ITEMS)
		return;

	Explode arr("-", data);
	DWORD length = m_stTrump.Length;
	strcpy(m_stTrump.Trumps[length].Name, arr[0]);
	m_stTrump.Trumps[length].Extra[0] = arr.GetValue2Int(1);
	m_stTrump.Length++;

	//printf("%d.�Զ��Ϸ���:%s %d\n", m_stCheckIn.Length, arr[0], arr.GetValue2Int(1));
}

// ��ȡ��������
void GameConf::ReadSetting(const char * data)
{
	Explode explode("=", data);
	if (strcmp(explode[0], "�����ļ�") == 0) {
		if (m_Setting.FBFileCount == 16) {
			printf("��ʾ:ֻ֧��16���̸��ļ�\n");
			::MessageBoxA(NULL, "ֻ֧��16���̸��ļ�", "��ʾ", MB_OK);
			return;
		}
		strcpy(m_Setting.FBFile[m_Setting.FBFileCount], explode[1]);
		m_Setting.FBFileCount++;
	}
	//printf("%s:%s\n", explode[0], explode[1]);
}

// ת����Ʒ����
int GameConf::TransFormItemType(const char * item_name)
{
	return 168;
}
