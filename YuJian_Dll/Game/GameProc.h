#pragma once
#include "GameStep.h"
#include <My/Common/Link.hpp>
#include <Windows.h>
#include <vector>
#include <fstream>

using namespace std;

#define BAG_NUM         40       // 包包容量
#define WUPIN_YAOBAO    0x0B5593 // 药包
#define WUPIN_XIANGLIAN 0x0B5B24 // 爱娜祈祷项链
#define WUPIN_YAO       0x0F6982 // 药
#define WUPIN_LINGYAO   0x0F943E // 速效圣兽灵药

#define RUNRUN 1

#define STEP_INDEX 13

struct white_dlls
{
	wchar_t name[64][MAX_PATH];
};

struct process_dlls
{
	wchar_t name[128][MAX_PATH];
	int length;
};

struct _step_;
struct _account_;

enum ComImgIndex:int;

class Game;
class GameStep;
class GameProc
{
public:
	// !!!
	GameProc(Game* p);

	// 初始化数据
	void InitData(bool v=false);
	// 初始化流程
	bool InitSteps();
	// 切换游戏窗口
	void SwitchGameWnd(HWND);
	// 切换游戏帐号
	void SwitchGameAccount(_account_* account);
	// 鼠标移动到游戏窗口位置
	void SetGameCursorPos(int x, int y);
	// 窗口置前
	void SetForegroundWindow(_account_* account);
	// 执行
	void Exec(_account_* account);
	// 运行
	void Run(_account_* account);
	// 去入队坐标
	void GoInTeamPos(_account_* account);
	// 创建队伍
	void CreateTeam(_account_* account);
	// 邀请入队
	void ViteInTeam(_account_* account);
	// 交易 
	void Transaction(_account_* master, _account_* sma);
	// 入队
	void InTeam(_account_* account);
	// 邀请进入副本
	void ViteInFB(_account_* account);
	// 进副本
	void InFB(_account_* account);
	// 所有人进副本
	void AllInFB(_account_* account_open);
	// 同意系统信息
	void AgreenMsg(const char* name, HWND hwnd = NULL);
	// 同意系统信息
	int AgreenMsg(const char* name, int icon_index, bool click = false, HWND hwnd = NULL);
	// 等待提示框
	void WaitGameMenu(_account_* account);
	// 飞鸽子
	void FeiGeZi(_account_* account);
	// 关闭XXX
	void CloseXXX(_account_* account);
	// 进入无尽虚空之塔
	void InXuKong(Account* account);
	// 离开无尽虚空之塔
	void LeaveXuKong(Account* account);
	// 神殿去雷鸣大陆流程
	void GoLeiMing(_account_* account);
	// 去领取项链
	void GoGetXiangLian(_account_* account);
	// 去树心城
	void GoTreeCity(_account_* account);
	// 离开树心城
	void LeaveTreeCity(_account_* account);
	// 去遗忘神域商店
	void GoShenYuShop(_account_* account);
	// 从遗忘神域商店出来
	void LeaveShenYu(_account_* account);
	// 询问项链数量
	_account_*  AskXiangLian();
	// 去副本门口
	bool GoFBDoor(_account_* account, int num=0, bool must_go=false);
	// 开启副本
	_account_* OpenFB(bool go_leiming=false);
	// 所有出副本big=大号要出去否
	void AllOutFB(bool big = false);
	// 出副本
	void OutFB(_account_* account);
	// 获取开启副本帐号
	_account_* GetOpenFBAccount();
	// 获取时间小时
	int  GetHour();
	// 是否由大号开启副本
	bool IsBigOpenFB();
	// 继续副本
	void ContinueInFB();
	// 是否在副本
	bool IsInFB(_account_* account);
	// 执行副本流程
	void ExecInFB();
	// 执行流程
	bool ExecStep(Link<_step_*>& link, bool isfb = false);
	// 步骤是否已执行完毕
	bool StepIsComplete();
	// 移动
	void Move(bool check_time = false);
	// 移动
	void MoveOne();
	// 移点
	void MoveClick(bool check_time = false);
	// 移至NPC
	bool MoveNPC();
	// 获取要移至位置
	int  GetMoveNPCPos(_npc_coor_* p_npc, DWORD& x, DWORD& y);
	// NCP
	void NPC();
	// NPC
	void NPC(const char* name, int x, int y, int x2 = 0, int y2 = 0);
	// 最后一个对话的NPC
	bool NPCLast(bool check_pos = true, DWORD mov_sleep_ms = 0);
	// 检查是否在可点击NPC范围内 is_move=是否移动到范围
	int  CheckInNPCPos(_npc_coor_* p_npc, DWORD& click_x, DWORD& click_y, bool is_move = true);
	// 获取点击NPC坐标
	bool GetNPCClickPos(_npc_coor_* p_npc, DWORD pos_x, DWORD pos_y, DWORD& click_x, DWORD& click_y);
	// 获取点击NPC坐标
	bool GetNPCClickPos(DWORD cx, DWORD cy, DWORD ccx, DWORD ccy, DWORD ccx2, DWORD ccy2, DWORD pos_x, DWORD pos_y, DWORD& click_x, DWORD& click_y);
	// 选择
	void Select();
	// 技能
	void Magic();
	// 卡位
	void KaWei();
	// 清怪
	void Clear();
	// 凯瑞
	void KaiRui();
	// 捡物
	void PickUp();
	// 存钱
	void SaveMoney();
	// 存物
	DWORD CheckIn(bool in = true);
	// 使用物品
	void UseItem();
	// 扔物品
	void DropItem();
	// 售卖物品
	void SellItem();
	// 按钮
	void Button();
	// 按钮
	bool Button(int button_id, DWORD sleep_ms = 0, const char* name = nullptr);
	// 关闭弹框
	bool CloseTipBox();
	// 关闭组队邀请
	bool CloseVite();
	// 关闭物品使用提示框
	bool CloseItemUseTipBox();
	// 关闭系统邀请提示框
	bool CloseSystemViteBox();
	// 随机点击
	void ClickRand();
	// 狂暴点击
	void ClickCrazy();
	// 等待
	void Wait();
	// 等待
	void Wait(DWORD ms, int no_open = 1);
	// 小号
	void Small();
	// 复活
	void ReBorn();
	// 是否检查此NPC对话完成
	bool IsCheckNPC(const char* name);
	// 是否检查此NPC弹框出来
	bool IsCheckNPCTipBox(const char* name);
	// 鼠标移动[相对于x或y移动rx或ry距离]
	void MouseMove(int x, int y, int rx, int ry, HWND hwnd = NULL);
	// 鼠标移动
	void MouseMove(int x, int y, HWND hwnd = NULL);
	// 鼠标滚轮
	void MouseWheel(int x, int y, int z, HWND hwnd = NULL);
	// 鼠标滚轮
	void MouseWheel(int z, HWND hwnd = NULL);
	// 鼠标左键点击
	void Click(int x, int y, int ex, int ey, int flag = 0xff, HWND hwnd = NULL);
	// 鼠标左键点击
	void Click(int x, int y, int flag = 0xff, HWND hwnd = NULL);
	// 鼠标左键点击
	void Click_Send(int x, int y, int ex, int ey, int flag = 0xff, HWND hwnd = NULL);
	// 鼠标左键点击
	void Click_Send(int x, int y, int flag = 0xff, HWND hwnd = NULL);
	// 鼠标左键点击[不包括此帐号]
	void ClickOther(int x, int y, int ex, int ey, _account_* account_no);
	// 鼠标左键双击
	void DBClick(int x, int y, HWND hwnd = NULL);
	// 按键
	void Keyboard(char key, int flag = 0xff, HWND hwnd = NULL);
	// 读取人物坐标
	bool ReadCoor();
	// 读取人物血量
	bool ReadLife();
	// 读取快捷键上面物品数量
	bool ReadQuickKey2Num();
	// 读取包包物品
	bool ReadBag();
	// 是否需要加血量
	int  IsNeedAddLife(int low_life);
	// 是否最前窗口
	bool IsForegroundWindow();
	// 加血
	void AddLife();
	// 发送信息给服务端
	void SendMsg(const char* v, const char* v2 = nullptr);
	// 停止
	void Stop(bool v = true);
	// 打开日记文件
	void OpenLogFile();
	// 写入日记
	void WriteLog(const char* log, bool flush = true);
	// 检查是否修改了数据
	bool ChNCk();
	// 验证DLL签名
	void CheckDllSign();
public:
	// 白名单DLL
	white_dlls* m_p_white_dlls;
	// 没有通过验证
	bool m_bNoVerify = false;
	// 要操作的游戏窗口
	HWND m_hWndGame;
	// 当前操作的帐号
	_account_* m_pAccount;
	// 游戏指针
	Game*     m_pGame;
	// 游戏步骤指针
	GameStep* m_pGameStep;
	// 正在执行的步骤
	_step_* m_pStep;
	// 正在执行的步骤[副本]
	_step_* m_pStepCopy;
	// 上次执行的步骤
	_step_* m_pStepLast;
	// 上次移动的步骤
	_step_* m_pStepLastMove;
	// 已记录步骤[卡住重回此步骤]
	_step_* m_pStepRecord;

	// 是否记录步骤
	bool m_bIsRecordStep;
	// 是否停止
	bool  m_bStop = false;
	// 是否暂停
	bool  m_bPause = false;
	// 是否重新开始
	bool  m_bReStart = false;
	// 是否狂甩
	bool m_bIsCrazy = false;
	// 狂甩技能
	char m_CrazyMagic[32];

	// 是否锁定进入副本
	bool m_bLockGoFB = false;
	// 是否在副本里面
	bool m_bAtFB = false;
	// 是否需要复活(第三关点机关那里不需要)
	bool m_bReborn = true;
	// 是否已清凯瑞
	bool m_bClearKaiRui = false;
	// 是否重置到记录步骤
	bool m_bIsResetRecordStep = false;
	// 是否第一次移动, 需要点一下地图
	bool m_bIsFirstMove = true;
	// 是否玩副本
	bool m_bPlayFB = true;
	// 
	bool m_bToBig = false;
	//
	bool m_bNeedCloseBag = false;
	// 是否捡了最后关物品
	bool m_bIsPickLast = false;

	// 是否能够连接驱动
	int m_nNoConnectDriver = 0;

	// 打了第几个BOSS
	int m_nBossNum = 0;
	// 是否重开副本0-不重开 1-直接出去重开 2-退出登录再重开 
	int m_nReOpenFB = 0;
	// 重新移动次数
	int m_nReMoveCount = 0;
	// 重新移动次数上次
	int m_nReMoveCountLast = 0;
	// 药包数量
	int m_nYaoBao = 0;
	// 药数量
	int m_nYao = 0;
	// 药包留存数量
	int m_nLiveYaoBao = 6;
	// 药留存数量
	int m_nLiveYao = 3;
	// 重开副本次数
	int m_nReOpenFBCount = 0;
	// 刷副本次数
	int m_nPlayFBCount = 0;
	int m_nPlayFBCount_QB = 0;
	// 已经计数时间
	int m_nFBTimeLongOne = 0;
	// 已经计数时间
	int m_nFBTimeLong = 0;
	// 开始副本时间
	int m_nStartFBTime = 0;
	// 更新副本时间
	int m_nUpdateFBTimeLongTime = 0;
	// 复活宠物时间
	int m_nRevivePetTime = 0;

	// 进程dll
	process_dlls* m_p_process_dlls;

	// 上一次对话之选项
	char m_chLastSelectName[128];

	// 上次的数据
	struct {
		DWORD OpCode;
		DWORD MvX;
		DWORD MvY;
		DWORD NPCOpCode; // 是对话还是点击
		char  NPCName[32];  // 名称
		DWORD ClickX;
		DWORD ClickY;
		DWORD ClickX2;
		DWORD ClickY2;
	} m_stLast;

	// 狂点的数据
	struct {
		int X;
		int Y;
		int X2;
		int Y2;
		int Count;
	} m_ClickCrazy;
};