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
	strcat(conf_file, "\\�½��ı��ĵ�.txt");
	//printf("��Ϸ�����ļ�:%s\n", conf_file);
	OpenTextFile file;
	if (!file.Open(conf_file)) {
		printf("�Ҳ���'%s'�ļ�������\n", conf_file);
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
		if (strstr(data, "������Ʒ")) {
			trans = true;
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

// ��ȡ��������
void GameConf::ReadPetOut(const char* data)
{
	if (m_stPetOut.Length == 3)
		return;

	DWORD length = m_stPetOut.Length;
	m_stPetOut.No[m_stPetOut.Length++] = atoi(data);
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

// ��ȡ������Ʒ
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
int GameConf::TransFormItemType(const char* name)
{
	if (strcmp(name, "���ƾ��������") == 0)
		return ���ƾ��������;
	if (strcmp(name, "�������°����") == 0)
		return �������°����;
	if (strcmp(name, "�����ǿ������") == 0)
		return �����ǿ������;
	if (strcmp(name, "��ţ���Ƕ����") == 0)
		return ��ţ���Ƕ����;
	if (strcmp(name, "��ţ��³�����") == 0)
		return ��ţ��³�����;
	if (strcmp(name, "˫����������") == 0)
		return ˫����������;
	if (strcmp(name, "˫������ڤ���") == 0)
		return ˫������ڤ���;
	if (strcmp(name, "��Ч���ư�") == 0)
		return ��Ч���ư�;
	if (strcmp(name, "�����ٱ�") == 0)
		return �����ٱ�;
	if (strcmp(name, "��ʯԿ��") == 0)
		return ��ʯԿ��;
	if (strcmp(name, "ɯ�ٵı���Կ��") == 0)
		return ɯ�ٵı���Կ��;
	if (strcmp(name, "ʥ���������") == 0)
		return ʥ���������;
	if (strcmp(name, "������ʥˮ") == 0)
		return ������ʥˮ;
	if (strcmp(name, "�����Ǳ�Կ��") == 0)
		return �����Ǳ�Կ��;
	if (strcmp(name, "��ɫף����Ƭ") == 0)
		return ��ɫף����Ƭ;
	if (strcmp(name, "�������ּ�һ") == 0)
		return �������ּ�һ;
	if (strcmp(name, "�������ּǶ�") == 0)
		return �������ּǶ�;
	if (strcmp(name, "�������ּ���") == 0)
		return �������ּ���;
	if (strcmp(name, "�������ּ���") == 0)
		return �������ּ���;
	if (strcmp(name, "�������ּ���") == 0)
		return �������ּ���;
	if (strcmp(name, "�������ּ���") == 0)
		return �������ּ���;
	if (strcmp(name, "�������ּ���") == 0)
		return �������ּ���;
	if (strcmp(name, "�������ּǰ�") == 0)
		return �������ּǰ�;
	if (strcmp(name, "�������ּǾ�") == 0)
		return �������ּǾ�;
	if (strcmp(name, "�������ּ�ʮ") == 0)
		return �������ּ�ʮ;
	if (strcmp(name, "��ħ����֮��") == 0)
		return ��ħ����֮��;
	if (strcmp(name, "�������緢�Ͱ�") == 0)
		return �������緢�Ͱ�;
	if (strcmp(name, "����������") == 0)
		return ����������;
	if (strcmp(name, "Ů������ͭ��") == 0)
		return Ů������ͭ��;
	if (strcmp(name, "�������ҩˮ") == 0)
		return �������ҩˮ;
	if (strcmp(name, "�İٵ�ͼ����һ") == 0)
		return �İٵ�ͼ����һ;
	if (strcmp(name, "�İٵ�ͼ������") == 0)
		return �İٵ�ͼ������;
	if (strcmp(name, "6��O���") == 0)
		return O���6��;
	if (strcmp(name, "12��XO���") == 0)
		return XO���12��;
	if (strcmp(name, "25��XO���") == 0)
		return XO���25��;
	if (strcmp(name, "��Ч����ҩˮ") == 0)
		return ��Ч����ҩˮ;
	if (strcmp(name, "��Чʥ����ҩ") == 0)
		return ��Чʥ����ҩ;
	if (strcmp(name, "30��������Ƭ+1") == 0)
		return ��ʮ��������Ƭ��1;
	if (strcmp(name, "30��������Ƭ+2") == 0)
		return ��ʮ��������Ƭ��2;
	if (strcmp(name, "30��������Ƭ+3") == 0)
		return ��ʮ��������Ƭ��3;
	if (strcmp(name, "ħ�꾧ʯ") == 0)
		return ħ�꾧ʯ;
	if (strcmp(name, "��꾧ʯ") == 0)
		return ��꾧ʯ;
	if (strcmp(name, "��ħ��ʯ") == 0)
		return ��ħ��ʯ;

	return δ֪��Ʒ;
}
