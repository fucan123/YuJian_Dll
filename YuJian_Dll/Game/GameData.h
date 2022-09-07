#pragma once
#include <Windows.h>

#define ISCMD 0

#define IS_READ_MEM         0x0000001     // �Ƿ���MEMģʽ

#define ADDR_ACCOUNT_NAME   0x1DFC0A0     // ��¼�ʺ�����
#define ADDR_ROLE_NAME      0x1E39760     // ��Ϸ��ɫ���� 
#define ADDR_SERVER_NAME    0x1DCC668     // ��Ϸ��������
#define ADDR_COOR_X_OFFSET  0x1E7E274     // X�����ַ��ģ�������ƫ��[MOD_3drole]
#define ADDR_COOR_Y_OFFSET  0x1E7E270     // Y�����ַ��ģ�������ƫ��[MOD_3drole]
#define ADDR_LIFE_OFFSET    0x1E39688     // Ѫ����ַ��ģ�������ƫ��[MOD_3drole]
#define ADDR_LIFEMAX_OFFSET 0x1E39690     // Ѫ�����޵�ַ��ģ�������ƫ��[MOD_3drole]

#define SCREEN_X 1440 // ��Ļ�Ŀ��
#define SCREEN_Y 900  // ��Ļ�ĸ߶�

/// ��Ϸ��Ʒ����
enum ITEM_TYPE
{
	δ֪��Ʒ = 0x00,       // ��֪��ʲô��Ʒ
	���ƾ�������� = 0x000B1FC8,
	�������°���� = 0x000B21B0,
	�����ǿ������ = 0x000B21B1,
	��ţ���Ƕ���� = 0x000B21BB,
	��ţ��³����� = 0x000B21BC,
	��Ч���ư� = 0x000B5593, // ���Կ�����ƿ��Ч����ҩˮ
	�����ٱ� = 0x000B84E9,
	��ʯԿ�� = 0x000B55FA,
	ɯ�ٵı���Կ�� = 0x000B5545,
	ʥ��������� = 0x000B55F8,
	������ʥˮ = 0x000B55F9,
	�����Ǳ�Կ�� = 0x000B55FB,
	��ɫף����Ƭ = 0x000B561E,
	Ů������ͭ�� = 0x000B5797,
	�������ּ�һ = 0x000B578C,
	�������ּǶ� = 0x000B578D,
	�������ּ��� = 0x000B578E,
	�������ּ��� = 0x000B578F,
	�������ּ��� = 0x000B5790,
	�������ּ��� = 0x000B5791,
	�������ּ��� = 0x000B5792,
	�������ּǰ� = 0x000B5793,
	�������ּǾ� = 0x000B5794,
	�������ּ�ʮ = 0x000B5795,
	��ħ����֮�� = 0x000B5798,
	�������緢�Ͱ� = 0x000B579D,
	���������� = 0x000B5B24,
	�������ҩˮ = 0x000B783C,
	�İٵ�ͼ����һ = 0x000C5D77,
	�İٵ�ͼ������ = 0x000C614D,
	O���6��   = 0x000C67C9,
	XO���12�� = 0x000C67C8,
	XO���25�� = 0x000C6FA4,
	˫���������� = 0x000C6FD7,
	˫������ڤ��� = 0x000C6FD8,
	��Ч����ҩˮ = 0x000F6982, // +2500����ֵ
	��Чʥ����ҩ = 0x000F943E, // �������
	��ʮ��������Ƭ��1 = 0x000F90E4,
	��ʮ��������Ƭ��2 = 0x000F90E5,
	��ʮ��������Ƭ��3 = 0x000F90E6,
	ħ�꾧ʯ = 0x000FD35E,
	��꾧ʯ = 0x000FD368,
	��ħ��ʯ = 0x000FD372,
};

// �Զ���ʰ��Ʒ��Ϣ
typedef struct conf_item_info
{
	CHAR      Name[32];
	DWORD     Type;
	DWORD     Extra[2];   
} ConfItemInfo;

typedef struct game_data_addr
{
	DWORD Player;        // �����׵�ַ
	DWORD Account;       // �ʺ�
	DWORD Role;          // ��ɫ������
	DWORD CoorX;         // X����
	DWORD CoorY;         // Y����
	DWORD MoveX;         // X����(Ŀ�ĵ�)
	DWORD MoveY;         // Y����(Ŀ�ĵ�)
	DWORD ScreenX;       // ��������Ļ����X
	DWORD ScreenY;       // ��������Ļ����Y
	DWORD Life;          // Ѫ��
	DWORD LifeMax;       // Ѫ�����ֵ
	DWORD PicScale;      // ����������ֵ
	DWORD QuickMagicNum; // ���ܿ����������Ʒ��ʾ����
	DWORD QuickItemNum;  // ��Ʒ��������Ʒ��ʾ����
	DWORD Bag;           // ������Ʒ��ַ
	DWORD Storage;       // �ֿ���Ʒ��ַ
} GameDataAddr;

// ����д��Ŀ�ĵ�xy��ַ����
typedef struct share_write_xy_data
{
	DWORD InDll;    // �Ƿ���ע��DLL
	DWORD AddrX;    // X��ַ
	DWORD AddrY;    // Y��ַ
	DWORD AddrPic;  // �������ŵ�ַ
	DWORD X;        // X��ֵ
	DWORD Y;        // Y��ֵ
	DWORD PicScale; // ����������ֵ
	DWORD Flag;     // 0-������д������ 1-Dll����д������ 2-д�뻭��������ֵ
} ShareWriteXYData;

class Game;
class GameData
{
public:
	GameData(Game* p);

	// ��ȡ��ɫ�ַ�
	bool GetRoleByPid(_account_* account, char* out, int len);
	// ������Ϸ
	int  WatchGame(bool first=true);
	// ��ȡ��Ϸ����
	void FindGameWnd();
	// ��ȡ��Ϸ��Ϣ
	void FindGameInfo(_account_* account);
	// ö�ٴ���
	static BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lParam);

	// �Ƿ���ָ���������� allow=���
	bool IsInArea(int x, int y, int allow, _account_* account=nullptr);
	// �Ƿ���ָ���������� allow=���
	bool IsNotInArea(int x, int y, int allow, _account_* account=nullptr);

	// �Ƿ������
	bool IsInShenDian(_account_* account=nullptr);
	// �Ƿ�����������
	bool IsInShenYu(_account_* account = nullptr);
	// �Ƿ��ڸ����ſ�
	bool IsInFBDoor(_account_* account=nullptr);

	// �Ƿ����ǵ�ͼ
	bool IsTheMap(const char* map, _account_* account = nullptr);

	// ��ȡ�������������
	bool FindQuickAddr(int flag=0);
	// ��ȡѪ����ַ
	bool FindLifeAddr();
	// ��ȡ������Ʒ��ַ
	bool FindBagAddr();
	// ��ȡ�ֿ���Ʒ��ַ
	bool FindStorageAddr();
	// ��ȡ������Ļ����
	bool FindScreenPos();
	// ��ȡ����������ֵ��ַ
	bool FindPicScale();

	// ��ȡ��ɫ
	bool ReadName(char* name, _account_* account=nullptr);
	// ��ȡ����
	bool ReadCoor(DWORD* x=nullptr, DWORD* y=nullptr, _account_* account=nullptr);
	// ��ȡ����
	bool ReadCoorByWnd(DWORD* x=nullptr, DWORD* y=nullptr, _account_* account=nullptr);
	// ������������
	int  FormatCoor(HWND hWnd);
	// ��ȡ��Ļ����
	bool ReadScreenPos(int& x, int& y, _account_* account = nullptr);
	// ��ȡ����ֵ
	DWORD ReadLife(DWORD* life=nullptr, DWORD* life_max=nullptr, _account_* account=nullptr);

	// ����������
	DWORD SearchCode(DWORD* codes, DWORD length, DWORD* save, DWORD save_length = 1, DWORD step = 4);
	// ����������
	DWORD SearchByteCode(BYTE* codes, DWORD length);
	// ��ȡ���ֽ�����
	bool ReadDwordMemory(DWORD addr, DWORD& v, _account_* account=nullptr);
	// ��ȡ�ڴ�
	bool ReadMemory(PVOID addr, PVOID save, DWORD length, _account_* account=nullptr);
	// ��ȡ��Ϸ�ڴ�
	bool ReadGameMemory(DWORD flag = 0x01);

public:
	Game* m_pGame;

	// ��ǰX����
	DWORD m_dwX = 0;
	// ��ǰY����
	DWORD m_dwY = 0;
	// ��ǰѪ��
	DWORD m_dwLife = 0;
	// Ѫ�����ֵ
	DWORD m_dwLifeMax = 0;

	// ����
	GameDataAddr m_DataAddr;
	// ���ݴ��
	GameDataAddr m_DataAddrBig;
public:
	// ��Ϸ��Ŵ���
	HWND m_hWndBig = NULL;
	// ��Ϸ���
	HANDLE m_hProcessBig = NULL;
	// ���
	_account_* m_pAccountBig = NULL;
	// ��ʱ
	_account_* m_pAccountTmp = NULL;

	// ��ϷȨ�޾��
	HANDLE   m_hGameProcess = NULL;
	// �Ƿ��ȡ���
	bool  m_bIsReadEnd;
	// ��ȡ�ڴ��Ĵ�С
	DWORD m_dwReadSize;
	// ��ȡ����ַ
	DWORD m_dwReadBase;
	// ��ȡ������ʱ�ڴ�
	BYTE* m_pReadBuffer;

	HANDLE m_hShareMap;    // �����ڴ�
	ShareWriteXYData* m_pShareBuffer; // �����ڴ�
	bool m_bInDll = false;
};