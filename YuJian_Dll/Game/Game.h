#include "Home.h"
#include "GameServer.h"

#include "Emulator.h"
#include "GameData.h"
#include <vector>
#include <fstream>
#include <Windows.h>

#define P2DW(v) (*(DWORD*)(v))       // 转成DWORD数值
#define P2INT(v) (*(int*)(v))        // 转成int数值

#define SET_VAR(var,v) var=v;
#define SET_VAR2(var,v,var2,v2) var=v;var2=v2;

#if ISCMD == 0
#define DbgPrint(...)

#define LOG(v) m_pGame->AddUILog(v,nullptr)
#define LOGVARP(p,...) { wsprintfW(p,__VA_ARGS__);LOG(p); }
#define LOGVARN(n,...) {wchar_t _s[n]; _s[n-1]=0; LOGVARP(_s,__VA_ARGS__); }
#define LOG2(v,cla) m_pGame->AddUILog(v,cla)
#define LOGVARP2(p,cla,...) { wsprintfW(p,__VA_ARGS__);LOG2(p,cla); }
#define LOGVARN2(n,cla,...) {wchar_t _s[n]; _s[n-1]=0; LOGVARP2(_s,cla,__VA_ARGS__); }
#else
#define DbgPrint(...) ::printf(__VA_ARGS__)

#define LOG(v)
#define LOGVARP(p,...)
#define LOGVARN(n,...)
#define LOG2(v,cla)
#define LOGVARP2(p,cla,...)
#define LOGVARN2(n,cla,...)
#endif

#define MAKESPOS(v) (int(v/m_fScale))

#define ACCSTA_INIT       0x1000 // 帐号还未登录
#define ACCSTA_READY      0x0001 // 准备登录
#define ACCSTA_LOGIN      0x0002 // 正在登录
#define ACCSTA_ONLINE     0x0004 // 帐号在线
#define ACCSTA_OPENFB     0x0008 // 正在开启副本
#define ACCSTA_ATFB       0x0010 // 是否在副本
#define ACCSTA_COMPLETED  0x0200 // 已经没有项链刷了
#define ACCSTA_OFFLINE    0x0100 // 帐号下线

#define MAX_ACCOUNT_LOGIN 5      // 最多支持多少账号在线

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
	char    Name[32];       // 帐号
	char    Password[32];   // 密码
	char    Role[32];       // 角色名称
	char    SerBig[32];     // 游戏大区
	char    SerSmall[32];   // 游戏小区
	int     RoleNo;
	int     XL;             // 项链数量 
	int     IsGetXL;        // 是否已经获取了项链
	int     IsReadXL;       // 是否已经读取了项链数量
	int     YaoShi;         // 卡利亚钥匙
	int     IsReadYaoShi;   // 是否获取了它
	int     IsReady;        // 是否已准备
	int     IsBig;          // 是否大号
	int     IsTransaction;  // 是否交易号
	int     IsCanTrans;     // 是否可以交易 交易号标志位
	int     IsLogin;        // 是否已输入帐号密码登录
	int     IsLeadear;      // 是否是队长了
	int     OfflineLogin;   // 是否掉线重连
	int     LockLogin;      // 是否锁定登录
	float   Scale;
	HWND    GameWnd;
	RECT    GameRect;
	DWORD   GamePid;        // 游戏进程ID
	int     Status;
	char    StatusStr[16];
	wchar_t StatusStrW[16];
	SOCKET  Socket;
	int     LoginTime;     // 登录时间
	int     PlayTime;      // 进入时间
	int     AtFBTime;      // 进入副本时间
	int     LastTime;      // 最后通信时间
	int     Flag;          // 
	int     Index;

	MNQ*   Mnq;           // 对应的每个游戏窗口
	GameDataAddr Addr;    // 数据地址
	int    IsGetAddr;     // 是否已获取地址
	DWORD  MvX;           // 要移动到的x
	DWORD  MvY;           // 要移动到的y
	DWORD  LastX;         // 上次的位置x
	DWORD  LastY;         // 上次的位置y
	DWORD  MvTime;        // 上次移动时间
	int    MvFlag;        // 负数为模糊检测(检测到一定范围即可,负多少代表允许范围值)

	HANDLE Process;       // 权限句柄
	struct {
		HWND Game;       // 游戏窗口句柄
		HWND Pic;        // 游戏画面窗口句柄
		HWND Map;        // 地图名称"{1 0x00000000}阿拉玛的哭泣"
		HWND Role;       // 角色名字"笑语嫣然"
		HWND HP;         // 血量"HP:29696/29696神仙HP加成10%"
		HWND PosX;       // 坐标x"{1 0x00000000}905"
		HWND PosY;       // 坐标y"{1 0x00000000}1063"
	} Wnd;
} Account;

typedef struct _share_team_
{
	char leader[32];     // 队长
	char closer[32];     // 需要关闭的
	char users[5][32];   // 账号
	char players[5][32]; // 名称
	char canin[5];       // 可以入队了
	char result[5];      // 邀请的结果 与(&0x0f)表示有未处理
	char gofbdoor[5];    // 到达副本门口
	char leavefbdoor[5]; // 离开副本门口
	char learderfbdoor;  // 队长去副本门口
	char saveqiu[5];     // 存球
	int  flag;           // 为1 请求完成
	Account* account[5];
} ShareTeam;

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
class BaiTan;

class Game
{
public:
	// ...
	~Game();

	// Init
	void Init(HWND hWnd, const char* conf_path);
	// 监听
	void Listen(USHORT port);
	// 执行
	void Run();
	// 搞进
	void Inject(DWORD pid, const wchar_t* dll_file, const wchar_t* short_name);
	// 自动交易
	bool AutoTransaction();
	// 自动登号
	bool AutoLogin(const char* remark);
	// 登录
	void Login(Account* p, int index);
	// 获取最新游戏窗体
	HWND FindNewGameWnd(DWORD* pid=NULL);
	// 进入到登录界面
	void GoLoginUI(Account* p);
	// 选择游戏服务
	void SelectServer(HWND hWnd);
	// 获取大区点击坐标
	void GetSerBigClickPos(int& x, int& y);
	// 获取小区点击坐标
	void GetSerSmallClickPos(int& x, int& y);
	// 输入
	void InputUserPwd(Account* p, bool input_user=true);
	// 左击
	void LeftClick(HWND hWnd, int x, int y);
	// 按键
	void Keyborad(int key, bool tra=true);
	// 退出
	void AutoLogout();
	// 退出
	void LogOut(Account* account, bool leave_team=true);
	// 输入帐号密码登录
	void Input(Account* p);
	// 全部登完
	void LoginCompleted(const char* remark);
	// 设置登号类型
	void SetLoginFlag(int flag);
	// 获取需要登录的数量
	int GetLoginCount();
	// 检查登录超时的帐号
	int CheckLoginTimeOut();
	// 检查游戏进程ID是否存在
	bool CheckGamePid(DWORD pid);
	// 设置游戏窗口
	bool SetGameWnd(HWND hwnd, bool* isbig=nullptr);
	// 设置游戏进程ID
	bool SetGameAddr(Account* p, GameDataAddr* addr);
	// 设置状态-全部
	void SetAllStatus(int status);
	// 设置准备
	void SetReady(Account* p, int v);
	// 设置模拟器名称
	void SetMNQName(Account* p, char* name);
	// 设置状态
	void SetStatus(Account* p, int status, bool update_text=false);
	// 设置SOKCET
	void SetSocket(Account* p, SOCKET s);
	// 设置Flag
	void SetFlag(Account* p, int flag);
	// 设置已完成
	void SetCompleted(Account* p);
	// 获取帐号总数
	int GetAccountCount();
	// 获取正在刷副本帐号数量
	int GetAtFBCount();
	// 获取所有在线账号数量
	int GetOnLineCount();
	// 帐号获取
	Account* GetAccount(const char* name);
	// 获取帐号
	Account* GetAccount(int index);
	// 获取帐号
	Account* GetAccountByRole(const char* role);
	// 获取帐号[根据状态]
	Account* GetAccountByStatus(int status);
	// 获取最多项链帐号
	Account* GetMaxXLAccount(Account** last=nullptr);
	// 获取准备登录的帐号[是否排除大号]
	Account* GetReadyAccount(bool nobig=true);
	// 获取下一个要登录的帐号
	Account* GetNextLoginAccount();
	// 获取帐号
	Account* GetAccountBySocket(SOCKET s);
	// 获取大号帐号
	Account* GetBigAccount();
	// 获取大号SOCKET
	SOCKET   GetBigSocket();
	// 设置已经入队
	void SetInTeam(int index);
	// 关闭游戏
	void CloseGame(int index);
	// 设置已准备好帐号数量
	void SetReadyCount(int v);
	// 设置已准备好帐号数量
	int AddReadyCount(int add = 1);
	// 对比帐号状态
	bool CheckStatus(Account* p, int status);
	// 是否自动登录
	bool IsAutoLogin();
	// 是否已全部准备好
	bool IsAllReady();
	// 帐号是否在登录
	bool IsLogin(Account* p);
	// 帐号是否在线
	bool IsOnline(Account* p);
	// 获取状态字符
	char* GetStatusStr(Account* p);

	int   GetShareTeamIndex(const char* user);

	// 发送卡住重启时间
	int SendQiaZhuS(int second);
	// 发送给大号
	int SendToBig(SOCKET_OPCODE opcode, bool clear=false);
	// 发送给大号
	int SendToBig(SOCKET_OPCODE opcode, int v, bool clear = true);

	// 检查数据库
	void CheckDB();
	// 更新物品信息
	void UpdateDBItem(const char* account, const char* item_name);
	// 更新刷副本次数
	void UpdateDBFB(int count=-1);
	// 更新刷副本时长
	void UpdateDBFBTimeLong(int time_long);
	// 插入副本记录
	void InsertFBRecord(int start_time, int end_time, int status);
	// 查询副本记录
	int SelectFBRecord(char*** result, int* col);

	// 获取教学数量
	int GetfbCount();
	// 更新重开副本次数
	void UpdateReOpenFBCount(int count);
	// 更新刷副本次数文本
	void UpdateFBCountText(int lx, bool add=true);
	// 更新刷副本时长文本
	void UpdateFBTimeLongText(int time_long, int add_time_long);
	// 更新掉线复活次数文本
	void UpdateOffLineAllText(int offline, int reborn);
	// 重新启动程序和全部下号
	void RestartApp();

	// 写入日记
	void WriteLog(const char* log);

	// 读取配置文件
	DWORD ReadConf();
	// 读取帐号设置
	bool  ReadAccountSetting(const char* data);
	// 读取其它设置
	void  ReadSetting(const char* data);

	// 自动关机
	void AutoShutDown();
	// 定时关机
	bool ClockShutDown(int flag=0);
	// 关机
	void ShutDown();
	// 当前时间是否在此时间
	bool IsInTime(int s_hour, int s_minute, int e_hour, int e_minute);

	// 推送到html界面
	void SetSetting(const char* name, int v);
	// 设置
	void PutSetting(const wchar_t* name, int v);
	// 设置
	void PutSetting(wchar_t* name, wchar_t* v);
	// 打开游戏
	int OpenGame(int index, int close_all=true);
	// 喊话
	void CallTalk(const char* text, int type);
	// 注入DLL
	int InstallDll();
	// 自动登号
	int AutoPlay(int index, bool stop, bool trans=false);
	// 添加帐号
	void AddAccount(Account* account);
	// 转移卡号本机
	void GetInCard(const wchar_t* card);
	// 验证卡号
	void VerifyCard(const wchar_t* card, const wchar_t* remark);
	// 更新程序版本
	void UpdateVer();

	// mmm
	bool CheckGameOtherModule();

	// 更新帐号状态
	void UpdateAccountStatus(Account * account);
	// 更新帐号在线时长
	void UpdateAccountPlayTime(Account * account);
	// 输入日记到UI界面
	void AddUILog(const wchar_t* text, const char* cla);
	// 更新
	void UpdateTableText(const char* id, int row, int col, const char* text);
	// 更新
	void UpdateTableText(const char* id, int row, int col, const wchar_t* text);
	// 更新
	void UpdateText(const char* id, const char* text);
	// 更新
	void UpdateText(const char* id, const wchar_t* text);
	// 更新状态栏文字
	void UpdateStatusText(const wchar_t* text, int icon);
	// Alert
	void Alert(const wchar_t* text, int icon);
	// 获取msg指针
	my_msg* GetMyMsg(int op);

	int    GetNdSysCallIndex();

	// 时长转成文字
	void FormatTimeLong(char* text, int time_long);
	// 时长转成文字
	void FormatTimeLong(wchar_t* text, int time_long);

	// 截图
	void SaveScreen(const char* name);
	// CRC校验
	bool ChCRC(bool loop=true);
public:
	// 观察是否进入游戏
	static DWORD WINAPI WatchInGame(LPVOID);
public:
	// 设置
	struct
	{
		char MnqPath[32];    // 模拟器路径
		char GamePath[32];   // 游戏客户端路径
		char GameFile[32];   // 游戏文件
		char SerBig[32];     // 游戏大区
		char SerSmall[32];   // 游戏小区
		char ShopMap[32];    // 商店地图
		char GeZiText[256];  // 鸽子内容
		int  CloseMnq;       // 启动前是否关闭模拟器
		int  OnlyLoginSmall; // 只会登录小号
		int  InitTimeOut;    // 启动超时时间
		int  LoginTimeOut;   // 登录超时允许时间
		int  TimeOut;        // 游戏超时时间
		int  FBTimeOut;      // 副本超时时间[卡住检测]
		int  FBTimeOutErvry; // 副本超时时间[随时检测]
		int  TransactionNum; // 交易次数
		int  XuKongSite;     // 无尽虚空塔位置

		int  ReConnect;      // 是否断线重连
		int  AutoLoginNext;  // 是否自动登录帐号
		int  LogoutByGetXL;  // 领完项链是否自动退出
		int  NoGetXL;        // 不领项链
		int  NoPlayFB;       // 不刷副本
		int  ShutDownNoXL;   // 刷完没有项链自动关机
		int  AtFBDoor;       // 出去是否呆在副本口口
		int  FBMode;         // 副本模式1=边领边刷,2=不领只刷,3=先领后刷,4=只领不刷

		int  ShutDown_SH;    // 定时关机[开始小时]
		int  ShutDown_SM;    // 定时关机[开始分钟]
		int  ShutDown_EH;    // 定时关机[结束小时]
		int  ShutDown_EM;    // 定时关机[结束分钟]

		int  OffLine_SH;     // 定时关机[开始小时]
		int  OffLine_SM;     // 定时关机[开始分钟]
		int  OffLine_EH;     // 定时关机[结束小时]
		int  OffLine_EM;     // 定时关机[结束分钟]

		int  AutoLogin_SH;   // 定时登录[开始小时]
		int  AutoLogin_SM;   // 定时登录[开始分钟]
		int  AutoLogin_EH;   // 定时登录[结束小时]
		int  AutoLogin_EM;   // 定时登录[结束分钟]

		int  SwitchLogin_SH; // 定时换号[开始小时]
		int  SwitchLogin_SM; // 定时换号[开始分钟]
		int  SwitchLogin_EH; // 定时换号[结束小时]
		int  SwitchLogin_EM; // 定时换号[结束分钟]

		int  NoAutoSelect;   // 不自动选择游戏区
		int  TalkOpen;       // 是否自动喊话    
		int  IsDebug;        // 是否显示调试信息

		int  NoHideProc;     // 是否不隐藏进程
		
	} m_Setting;

public:
	// 自身
	Game* m_pGame;
	// 模拟器
	Emulator* m_pEmulator;
	// 配置
	GameConf* m_pGameConf;
	// 游戏数据类
	GameData* m_pGameData;
	// 游戏操作类
	GameProc* m_pGameProc;
	// 移动
	Move*     m_pMove;
	// 物品
	Item*     m_pItem;
	// 技能
	Magic*    m_pMagic;
	// 对话
	Talk*     m_pTalk;
	// 宠物
	Pet*      m_pPet;
	// 截图
	PrintScreen* m_pPrintScreen;
	// 按钮
	MCButton * m_pButton;
	// 摆摊
	BaiTan* m_pBaiTan;
	// 数据库
	Sqlite*   m_pSqlite;


	// Home
	Home* m_pHome;
	// Driver
	Driver* m_pDriver;

	ShareTeam* m_pShareTeam;
public:
	// UI窗口
	HWND m_hUIWnd;
	// 更新版本线程
	LPTHREAD_START_ROUTINE m_funcUpdateVer;

	int m_nMsgIndex = 0;
	// 消息
	my_msg m_Msg[100];

	// 游戏大号窗口
	HWND m_hWndBig;
	// 游戏句柄
	HANDLE m_hProcessBig;

	float m_fScale;

	// 监听游戏信息
	GameServer* m_pServer;

	// 帐号列表
	std::vector<Account*> m_AccountList;

	// 大号
	Account* m_pBig = nullptr;
	// 交易
	Account* m_pTransaction = nullptr;
	// 队长的
	Account* m_pLeader = nullptr;

	// 登号类型 -2停止登 -1全部 其他账号列表索引
	int m_iLoginFlag = -2;

	// 当前登号索引
	int m_iLoginIndex = 0;
	// 需要登录帐号数量
	int m_iLoginCount = 0;
	// 已经准备好帐号数量
	int m_iReadyCount = 0;
	// 上次开启副本帐号索引
	int m_iOpenFBIndex = -1;
	// 是否发送创建队伍
	int m_iSendCreateTeam = 1;
	// 是否锁定登录
	bool m_bLockLogin = false;
	// 是否正在登录
	bool m_bLoging = false;
	// 是否自动下线了
	bool m_bAutoOffline = false;
	// 是否自动上线了
	bool m_bAutoOnline = false;
	// 正在重启
	bool m_bRestarting = false;

	char m_chTitle[32];
	char m_chPath[255];
	ofstream m_LogFile;

	bool m_bLock = false;

	// 启动时间
	int m_nStartTime = 0;
	// 更新启动时间
	int m_nUpdateTimeLongTime = 0;
	// 验证总次数
	int m_nVerifyNum = 0;
	// 验证时间
	int m_nVerifyTime = 0;
	// 验证错误次数
	int m_nVerifyError = 0;
	// 游戏无效时间
	int m_nEndTime = 0;
	// 校准错误次数
	int m_nEndTimeError = 0;
	// 检查CRC验证失败
	int m_nCheckCRCError = 0;

	// 隐藏标志应该为0x168999CB
	int m_nHideFlag = 0;
};