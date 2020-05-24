#pragma once
#include <Windows.h>

struct _account_;
// ģ������Ϣ
typedef struct mnq {
	int    Index;     // ����
	char   Name[64];  // ����
	HWND   WndTop;    // ���㴰�ھ��
	HWND   Wnd;       // ������Ϣ���ھ��
	int    UiPid;     // UI����ID
	int    UiWidth;   // UI���
	int    UiHeight;  // UI�߶�
	int    VBoxPid;   // ��ʵ���ݽ���ID
	int    Init;      // ģ�����Ƿ���������
	HANDLE Process;   // Ȩ�޾��
	int    StartTime; // ����ʱ��

	_account_* Account; // ��Ӧ�ĸ��ʺ�, һ��ģ�����Ͱ��ʺ�
} MNQ;

class Game;
class Emulator
{
public:
	// ...
	Emulator(Game* p);
	// ����·��
	void SetPath(char* path);
	// ִ��CMD�����ȡ�����
	void ExecCmdLd(const char* cmd, int index=0);
	// ִ��CMD�����ȡ�����
	bool ExecCmd(const char* cmd, char* result, int size);
	// ��ģ����
	MNQ* Open(_account_* p);
	// ��ģ����
	MNQ* Open(int index);
	// �ر�ģ����
	MNQ* Close(int index);
	// �ر�����ģ����
	void CloseAll();
	// ������Ϸ
	void StartGame(int index=0);
	// �ر���Ϸ
	void CloseGame(int index=0);
	// �ȴ�������Ϸ
	void WatchInGame(_account_* account);
	// list2����
	int List2();
	// list2����
	int List2(MNQ* mnq, int length);
	// ��������
	void Setprop(const char* key, int value, int index = 0);
	// ��������
	void Setprop(const char* key, const char* value, int index=0);
	// ��ȡ����
	bool Getprop(const char* key, char* result, int size, int index=0);
	// ��ȡ����
	int  Getprop(const char* key, int index=0, int d_v=0);
	// ��ȡ������Ե�ģ��������
	int  GetpropCount(const char* key, int v);
	// ���÷ֱ���
	void SetRate(int index, int width, int height, int dpi=240);
	// ������
	void ReName(const char* name, int index=0);
	// ���ô��ڴ�С
	void ReSize(HWND hwnd, int cx, int cy);
	// ������Ϸ����
	void ReGameWndSize(int index=0);
	// ���
	void Tap(int x, int y, int index=0);
	// ���
	void Tap(int x, int y, int x2, int y2, int index = 0);
	// ����
	void Swipe(int x, int y, int x2, int y2, int index=0, int ms=0);
	// ����
	void Key(int key, int index=0);
	// ����
	void Text(char* text, int index = 0);
	// ����ģ��������ʱ��
	int SetStartTime(int index, int v=-1);
	// ��ȡģ��������
	int GetCount() { return m_nCount; }
	// ��ȡģ������������
	int GetFreeCount();
	// ��ȡģ������Ϣ
	MNQ* GetMNQ(int index) { return index < m_nCount ? &m_List[index] : nullptr;  }
public:
	// Game��
	Game* m_pGame;
	// ģ����·��
	char  m_chPath[255];
	// dnconsole����
	char m_chDnconsole[255];
	// ld����
	char m_chLd[255];
public:
	// ģ��������
	int  m_nCount;
	// ģ�����б�
	MNQ  m_List[10];
	// �Ƿ��ȡ�б���
	bool m_bIsGetList = false;
};