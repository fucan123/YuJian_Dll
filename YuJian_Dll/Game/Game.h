#include "Home.h"
#include "GameServer.h"

#include "Emulator.h"
#include "GameData.h"
#include <vector>
#include <fstream>
#include <Windows.h>

#define P2DW(v) (*(DWORD*)(v))       // ת��DWORD��ֵ
#define P2INT(v) (*(int*)(v))        // ת��int��ֵ

#if 1
#define DbgPrint(...) ::printf(__VA_ARGS__)
#else
#define DbgPrint(...)
#endif

#define LOG(v) m_pGame->AddUILog(v,nullptr)
#define LOGVARP(p,...) { wsprintfW(p,__VA_ARGS__);LOG(p); }
#define LOGVARN(n,...) {wchar_t _s[n]; _s[n-1]=0; LOGVARP(_s,__VA_ARGS__); }
#define LOG2(v,cla) m_pGame->AddUILog(v,cla)
#define LOGVARP2(p,cla,...) { wsprintfW(p,__VA_ARGS__);LOG2(p,cla); }
#define LOGVARN2(n,cla,...) {wchar_t _s[n]; _s[n-1]=0; LOGVARP2(_s,cla,__VA_ARGS__); }

#define MAKESPOS(v) (int(v/m_fScale))

#define ACCSTA_INIT       0x1000 // �ʺŻ�δ��¼
#define ACCSTA_READY      0x0001 // ׼����¼
#define ACCSTA_LOGIN      0x0002 // ���ڵ�¼
#define ACCSTA_ONLINE     0x0004 // �ʺ�����
#define ACCSTA_OPENFB     0x0008 // ���ڿ�������
#define ACCSTA_ATFB       0x0010 // �Ƿ��ڸ���
#define ACCSTA_COMPLETED  0x0200 // �Ѿ�û������ˢ��
#define ACCSTA_OFFLINE    0x0100 // �ʺ�����

#define MAX_ACCOUNT_LOGIN 5      // ���֧�ֶ����˺�����

#define MSG_CALLJS       (WM_USER+100)
#define MSG_ADDLOG        1
#define MSG_SETTEXT       2
#define MSG_ADDTABLEROW   3
#define MSG_FILLTABLE     4
#define MSG_SETSETTING    5
#define MSG_ALERT         6
#define MSG_UPSTATUSTEXT  100
#define MSG_UPVER_OK      101
#define MSG_UPSTEP_OK     102
#define MSG_VERIFY_OK     200

struct my_msg {
	int  op;
	char id[32];
	char text[64];
	wchar_t text_w[64];
	char cla[32];
	int  value[10];
	int  table_text;
	int  status_text;
};

typedef struct _account_
{
	char    Name[32];       // �ʺ�
	char    Password[32];   // ����
	char    Role[16];       // ��ɫ����
	char    SerBig[32];     // ��Ϸ����
	char    SerSmall[32];   // ��ϷС��
	int     RoleNo;
	int     XL;             // �������� 
	int     IsGetXL;        // �Ƿ��Ѿ���ȡ������
	int     IsReadXL;       // �Ƿ��Ѿ���ȡ����������
	int     IsReady;        // �Ƿ���׼��
	int     IsBig;          // �Ƿ���
	int     IsLogin;        // �Ƿ��������ʺ������¼
	int     OfflineLogin;   // �Ƿ��������
	int     LockLogin;      // �Ƿ�������¼
	float   Scale;
	HWND    GameWnd;
	RECT    GameRect;
	DWORD   GamePid;        // ��Ϸ����ID
	int     Status;
	char    StatusStr[16];
	wchar_t StatusStrW[16];
	SOCKET  Socket;
	int     LoginTime;     // ��¼ʱ��
	int     PlayTime;      // ����ʱ��
	int     AtFBTime;      // ���븱��ʱ��
	int     LastTime;      // ���ͨ��ʱ��
	int     Flag;          // 
	int     Index;

	MNQ*   Mnq;           // ��Ӧ��ÿ����Ϸ����
	GameDataAddr Addr;    // ���ݵ�ַ
	int    IsGetAddr;     // �Ƿ��ѻ�ȡ��ַ
	DWORD  MvX;           // Ҫ�ƶ�����x
	DWORD  MvY;           // Ҫ�ƶ�����y
	DWORD  LastX;         // �ϴε�λ��x
	DWORD  LastY;         // �ϴε�λ��y
	DWORD  MvTime;        // �ϴ��ƶ�ʱ��

	HANDLE Process;       // Ȩ�޾��
	struct {
		HWND Game;       // ��Ϸ���ھ��
		HWND Pic;        // ��Ϸ���洰�ھ��
		HWND Map;        // ��ͼ����"{1 0x00000000}������Ŀ���"
		HWND Role;       // ��ɫ����"Ц����Ȼ"
		HWND HP;         // Ѫ��"HP:29696/29696����HP�ӳ�10%"
		HWND PosX;       // ����x"{1 0x00000000}905"
		HWND PosY;       // ����y"{1 0x00000000}1063"
	} Wnd;
} Account;

using namespace std;

class Sqlite;
class Home;
class Driver;
class Emulator;
class GameServer;
class GameConf;
class GameData;
class GameProc;
class Move;
class Item;
class Magic;
class Talk;
class Pet;
class PrintScreen;
class MCButton;

class Game
{
public:
	// ...
	~Game();

	// Init
	void Init(HWND hWnd, const char* conf_path);
	// ����
	void Listen(USHORT port);
	// ִ��
	void Run();
	// ��¼
	void Login(Account* p);
	// ���뵽��¼����
	void GoLoginUI(int left, int top);
	// �˳�
	void LogOut(Account* account);
	// �����ʺ������¼
	void Input(Account* p);
	// �Զ��Ǻ�
	bool AutoLogin(const char* remark);
	// ȫ������
	void LoginCompleted(const char* remark);
	// ���õǺ�����
	void SetLoginFlag(int flag);
	// ��ȡ��Ҫ��¼������
	int GetLoginCount();
	// ����¼��ʱ���ʺ�
	int CheckLoginTimeOut();
	// �����Ϸ����ID�Ƿ����
	bool CheckGamePid(DWORD pid);
	// ������Ϸ����
	bool SetGameWnd(HWND hwnd, bool* isbig=nullptr);
	// ������Ϸ����ID
	bool SetGameAddr(Account* p, GameDataAddr* addr);
	// ����״̬-ȫ��
	void SetAllStatus(int status);
	// ����׼��
	void SetReady(Account* p, int v);
	// ����ģ��������
	void SetMNQName(Account* p, char* name);
	// ����״̬
	void SetStatus(Account* p, int status, bool update_text=false);
	// ����SOKCET
	void SetSocket(Account* p, SOCKET s);
	// ����Flag
	void SetFlag(Account* p, int flag);
	// ���������
	void SetCompleted(Account* p);
	// ��ȡ�ʺ�����
	int GetAccountCount();
	// ��ȡ����ˢ�����ʺ�����
	int GetAtFBCount();
	// ��ȡ���������˺�����
	int GetOnLineCount();
	// �ʺŻ�ȡ
	Account* GetAccount(const char* name);
	// ��ȡ�ʺ�
	Account* GetAccount(int index);
	// ��ȡ�ʺ�
	Account* GetAccountByRole(const char* role);
	// ��ȡ�ʺ�[����״̬]
	Account* GetAccountByStatus(int status);
	// ��ȡ��������ʺ�
	Account* GetMaxXLAccount(Account** last=nullptr);
	// ��ȡ׼����¼���ʺ�[�Ƿ��ų����]
	Account* GetReadyAccount(bool nobig=true);
	// ��ȡ��һ��Ҫ��¼���ʺ�
	Account* GetNextLoginAccount();
	// ��ȡ�ʺ�
	Account* GetAccountBySocket(SOCKET s);
	// ��ȡ����ʺ�
	Account* GetBigAccount();
	// ��ȡ���SOCKET
	SOCKET   GetBigSocket();
	// �����Ѿ����
	void SetInTeam(int index);
	// �ر���Ϸ
	void CloseGame(int index);
	// ������׼�����ʺ�����
	void SetReadyCount(int v);
	// ������׼�����ʺ�����
	int AddReadyCount(int add = 1);
	// �Ա��ʺ�״̬
	bool CheckStatus(Account* p, int status);
	// �Ƿ��Զ���¼
	bool IsAutoLogin();
	// �Ƿ���ȫ��׼����
	bool IsAllReady();
	// �ʺ��Ƿ��ڵ�¼
	bool IsLogin(Account* p);
	// �ʺ��Ƿ�����
	bool IsOnline(Account* p);
	// ��ȡ״̬�ַ�
	char* GetStatusStr(Account* p);

	// ���Ϳ�ס����ʱ��
	int SendQiaZhuS(int second);
	// ���͸����
	int SendToBig(SOCKET_OPCODE opcode, bool clear=false);
	// ���͸����
	int SendToBig(SOCKET_OPCODE opcode, int v, bool clear = true);

	// ������ݿ�
	void CheckDB();
	// ������Ʒ��Ϣ
	void UpdateDBItem(const char* account, const char* item_name);
	// ����ˢ��������
	void UpdateDBFB(int count=-1);
	// ����ˢ����ʱ��
	void UpdateDBFBTimeLong(int time_long);
	// ���븱����¼
	void InsertFBRecord(int start_time, int end_time, int status);
	// ��ѯ������¼
	int SelectFBRecord(char*** result, int* col);

	// �����ؿ���������
	void UpdateReOpenFBCount(int count);
	// ����ˢ���������ı�
	void UpdateFBCountText(int lx, bool add=true);
	// ����ˢ����ʱ���ı�
	void UpdateFBTimeLongText(int time_long, int add_time_long);
	// ���µ��߸�������ı�
	void UpdateOffLineAllText(int offline, int reborn);

	// д���ռ�
	void WriteLog(const char* log);

	// ��ȡ�����ļ�
	DWORD ReadConf();
	// ��ȡ�ʺ�����
	bool  ReadAccountSetting(const char* data);
	// ��ȡ��������
	void  ReadSetting(const char* data);

	// �Զ��ػ�
	void AutoShutDown();
	// ��ʱ�ػ�
	bool ClockShutDown(int flag=0);
	// �ػ�
	void ShutDown();
	// ��ǰʱ���Ƿ��ڴ�ʱ��
	bool IsInTime(int s_hour, int s_minute, int e_hour, int e_minute);

	// ���͵�html����
	void SetSetting(const char* name, int v);
	// ����
	void PutSetting(const wchar_t* name, int v);
	// ����
	void PutSetting(wchar_t* name, wchar_t* v);
	// ����Ϸ
	int OpenGame(int index, int close_all=true);
	// ����
	void CallTalk(const char* text, int type);
	// ע��DLL
	int InstallDll();
	// �Զ��Ǻ�
	int AutoPlay(int index, bool stop);
	// ����ʺ�
	void AddAccount(Account* account);
	// ת�ƿ��ű���
	void GetInCard(const wchar_t* card);
	// ��֤����
	void VerifyCard(const wchar_t* card);
	// ���³���汾
	void UpdateVer();

	// �����ʺ�״̬
	void UpdateAccountStatus(Account * account);
	// �����ʺ�����ʱ��
	void UpdateAccountPlayTime(Account * account);
	// �����ռǵ�UI����
	void AddUILog(const wchar_t* text, const char* cla);
	// ����
	void UpdateTableText(const char* id, int row, int col, const char* text);
	// ����
	void UpdateTableText(const char* id, int row, int col, const wchar_t* text);
	// ����
	void UpdateText(const char* id, const char* text);
	// ����
	void UpdateText(const char* id, const wchar_t* text);
	// ����״̬������
	void UpdateStatusText(const wchar_t* text, int icon);
	// Alert
	void Alert(const wchar_t* text, int icon);
	// ��ȡmsgָ��
	my_msg* GetMyMsg(int op);

	int    GetNdSysCallIndex();

	// ʱ��ת������
	void FormatTimeLong(char* text, int time_long);
	// ʱ��ת������
	void FormatTimeLong(wchar_t* text, int time_long);

	// ��ͼ
	void SaveScreen(const char* name);
	// CRCУ��
	bool ChCRC(bool loop=true);
public:
	// �۲��Ƿ������Ϸ
	static DWORD WINAPI WatchInGame(LPVOID);
public:
	// ����
	struct
	{
		char MnqPath[32];    // ģ����·��
		char GamePath[32];   // ��Ϸ�ͻ���·��
		char GameFile[32];   // ��Ϸ�ļ�
		char SerBig[32];     // ��Ϸ����
		char SerSmall[32];   // ��ϷС��
		int  CloseMnq;       // ����ǰ�Ƿ�ر�ģ����
		int  InitTimeOut;    // ������ʱʱ��
		int  LoginTimeOut;   // ��¼��ʱ����ʱ��
		int  TimeOut;        // ��Ϸ��ʱʱ��
		int  FBTimeOut;      // ������ʱʱ��[��ס���]
		int  FBTimeOutErvry; // ������ʱʱ��[��ʱ���]

		int  ReConnect;      // �Ƿ��������
		int  AutoLoginNext;  // �Ƿ��Զ���¼�ʺ�
		int  LogoutByGetXL;  // ���������Ƿ��Զ��˳�
		int  NoGetXL;        // ��������
		int  NoPlayFB;       // ��ˢ����
		int  ShutDownNoXL;   // ˢ��û�������Զ��ػ�
		int  FBMode;         // ����ģʽ1=�����ˢ,2=����ֻˢ,3=�����ˢ,4=ֻ�첻ˢ

		int  ShutDown_SH;    // ��ʱ�ػ�[��ʼСʱ]
		int  ShutDown_SM;    // ��ʱ�ػ�[��ʼ����]
		int  ShutDown_EH;    // ��ʱ�ػ�[����Сʱ]
		int  ShutDown_EM;    // ��ʱ�ػ�[��������]

		int  OffLine_SH;     // ��ʱ�ػ�[��ʼСʱ]
		int  OffLine_SM;     // ��ʱ�ػ�[��ʼ����]
		int  OffLine_EH;     // ��ʱ�ػ�[����Сʱ]
		int  OffLine_EM;     // ��ʱ�ػ�[��������]

		int  AutoLogin_SH;   // ��ʱ��¼[��ʼСʱ]
		int  AutoLogin_SM;   // ��ʱ��¼[��ʼ����]
		int  AutoLogin_EH;   // ��ʱ��¼[����Сʱ]
		int  AutoLogin_EM;   // ��ʱ��¼[��������]

		int  NoAutoSelect;   // ���Զ�ѡ����Ϸ��
		int  TalkOpen;       // �Ƿ��Զ�����    
		int  IsDebug;        // �Ƿ���ʾ������Ϣ

		int  NoHideProc;     // �Ƿ����ؽ���
		
	} m_Setting;

public:
	// ����
	Game* m_pGame;
	// ģ����
	Emulator* m_pEmulator;
	// ����
	GameConf* m_pGameConf;
	// ��Ϸ������
	GameData* m_pGameData;
	// ��Ϸ������
	GameProc* m_pGameProc;
	// �ƶ�
	Move*     m_pMove;
	// ��Ʒ
	Item*     m_pItem;
	// ����
	Magic*    m_pMagic;
	// �Ի�
	Talk*     m_pTalk;
	// ����
	Pet*      m_pPet;
	// ��ͼ
	PrintScreen* m_pPrintScreen;
	// ��ť
	MCButton * m_pButton;
	// ���ݿ�
	Sqlite*   m_pSqlite;


	// Home
	Home* m_pHome;
	// Driver
	Driver* m_pDriver;

public:
	// UI����
	HWND m_hUIWnd;
	// ���°汾�߳�
	LPTHREAD_START_ROUTINE m_funcUpdateVer;

	int m_nMsgIndex = 0;
	// ��Ϣ
	my_msg m_Msg[100];

	// ��Ϸ��Ŵ���
	HWND m_hWndBig;
	// ��Ϸ���
	HANDLE m_hProcessBig;

	float m_fScale;

	// ������Ϸ��Ϣ
	GameServer* m_pServer;

	// �ʺ��б�
	std::vector<Account*> m_AccountList;

	// ���
	Account* m_pBig = nullptr;

	// �Ǻ����� -2ֹͣ�� -1ȫ�� �����˺��б�����
	int m_iLoginFlag = -2;

	// ��ǰ�Ǻ�����
	int m_iLoginIndex = 0;
	// ��Ҫ��¼�ʺ�����
	int m_iLoginCount = 0;
	// �Ѿ�׼�����ʺ�����
	int m_iReadyCount = 0;
	// �ϴο��������ʺ�����
	int m_iOpenFBIndex = -1;
	// �Ƿ��ʹ�������
	int m_iSendCreateTeam = 1;
	// �Ƿ�������¼
	bool m_bLockLogin = false;

	char m_chTitle[32];
	char m_chPath[255];
	ofstream m_LogFile;

	bool m_bLock = false;

	// ����ʱ��
	int m_nStartTime = 0;
	// ��������ʱ��
	int m_nUpdateTimeLongTime = 0;
	// ��֤�ܴ���
	int m_nVerifyNum = 0;
	// ��֤ʱ��
	int m_nVerifyTime = 0;
	// ��֤�������
	int m_nVerifyError = 0;
	// ��Ϸ��Чʱ��
	int m_nEndTime = 0;
	// У׼�������
	int m_nEndTimeError = 0;
	// ���CRC��֤ʧ��
	int m_nCheckCRCError = 0;

	// ���ر�־Ӧ��Ϊ0x168999CB
	int m_nHideFlag = 0;


};