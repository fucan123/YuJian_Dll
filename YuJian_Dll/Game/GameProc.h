#pragma once
#include "GameStep.h"
#include <My/Common/Link.hpp>
#include <Windows.h>
#include <vector>
#include <fstream>

using namespace std;

#define BAG_NUM         40       // ��������
#define WUPIN_YAOBAO    0x0B5593 // ҩ��
#define WUPIN_XIANGLIAN 0x0B5B24 // ����������
#define WUPIN_YAO       0x0F6982 // ҩ
#define WUPIN_LINGYAO   0x0F943E // ��Чʥ����ҩ

#define RUNRUN 1

#define STEP_INDEX 13

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

	// ��ʼ������
	void InitData();
	// ��ʼ������
	bool InitSteps();
	// �л���Ϸ����
	void SwitchGameWnd(HWND);
	// �л���Ϸ�ʺ�
	void SwitchGameAccount(_account_* account);
	// ����ƶ�����Ϸ����λ��
	void SetGameCursorPos(int x, int y);
	// ������ǰ
	void SetForegroundWindow(HWND hwnd);
	// ִ��
	void Exec(_account_* account);
	// ����
	void Run(_account_* account);
	// ȥ�������
	void GoInTeamPos(_account_* account);
	// ��������
	void CreateTeam();
	// �������
	void ViteInTeam();
	// ���
	void InTeam(_account_* account);
	// ������븱��
	void ViteInFB(_account_* account);
	// ������
	void InFB(_account_* account);
	// �����˽�����
	void AllInFB(_account_* account_open);
	// ͬ��ϵͳ��Ϣ
	void AgreenMsg(const char* name, HWND hwnd=NULL);
	// ͬ��ϵͳ��Ϣ
	int AgreenMsg(const char* name, int icon_index, bool click=false, HWND hwnd = NULL);
	// ���ȥ������½����
	void GoLeiMing();
	// ȥ��ȡ����
	void GoGetXiangLian();
	// ѯ����������
	_account_*  AskXiangLian();
	// ȥ�����ſ�
	void GoFBDoor(_account_* account);
	// ��������
	_account_* OpenFB();
	// ���г�����big=���Ҫ��ȥ��
	void AllOutFB(bool big=false);
	// ������
	void OutFB(_account_* account);
	// ��ȡ���������ʺ�
	_account_* GetOpenFBAccount();
	// ��ȡʱ��Сʱ
	int  GetHour();
	// �Ƿ��ɴ�ſ�������
	bool IsBigOpenFB();
	// ��������
	void ContinueInFB();
	// �Ƿ��ڸ���
	bool IsInFB(_account_* account);
	// ִ�и�������
	void ExecInFB();
	// ִ������
	bool ExecStep(Link<_step_*>& link, bool isfb=false);
	// �����Ƿ���ִ�����
	bool StepIsComplete();
	// �ƶ�
	void Move(bool check_time=false);
	// �ƶ�
	void MoveOne();
	// �Ƶ�
	void MoveClick(bool check_time=false);
	// ����NPC
	bool MoveNPC();
	// ��ȡҪ����λ��
	int  GetMoveNPCPos(_npc_coor_* p_npc, DWORD& x, DWORD& y);
	// NCP
	void NPC();
	// NPC
	void NPC(const char* name, int x, int y, int x2 = 0, int y2 = 0);
	// ���һ���Ի���NPC
	bool NPCLast(bool check_pos=true, DWORD mov_sleep_ms=0);
	// ����Ƿ��ڿɵ��NPC��Χ�� is_move=�Ƿ��ƶ�����Χ
	int  CheckInNPCPos(_npc_coor_* p_npc, DWORD& click_x, DWORD& click_y, bool is_move=true);
	// ��ȡ���NPC����
	bool GetNPCClickPos(_npc_coor_* p_npc, DWORD pos_x, DWORD pos_y, DWORD& click_x, DWORD& click_y);
	// ѡ��
	void Select();
	// ����
	void Magic();
	// ��λ
	void KaWei();
	// ���
	void Clear();
	// ����
	void KaiRui();
	// ����
	void PickUp();
	// ��Ǯ
	void SaveMoney();
	// ����
	DWORD CheckIn(bool in=true);
	// ʹ����Ʒ
	void UseItem();
	// ����Ʒ
	void DropItem();
	// ������Ʒ
	void SellItem();
	// ��ť
	void Button();
	// ��ť
	bool Button(int button_id, DWORD sleep_ms=0, const char* name=nullptr);
	// �رյ���
	bool CloseTipBox();
	// �ر��������
	bool CloseVite();
	// �ر���Ʒʹ����ʾ��
	bool CloseItemUseTipBox();
	// �ر�ϵͳ������ʾ��
	bool CloseSystemViteBox();
	// ����
	void Find(Link<_step_*>& link);
	// ������
	void ClickRand();
	// �񱩵��
	void ClickCrazy();
	// �ȴ�
	void Wait();
	// �ȴ�
	void Wait(DWORD ms, int no_open=1);
	// С��
	void Small();
	// ����
	void ReBorn();
	// �Ƿ����NPC�Ի����
	bool IsCheckNPC(const char* name);
	// �Ƿ����NPC�������
	bool IsCheckNPCTipBox(const char* name);
	// ����ƶ�[�����x��y�ƶ�rx��ry����]
	void MouseMove(int x, int y, int rx, int ry, HWND hwnd=NULL);
	// ����ƶ�
	void MouseMove(int x, int y, HWND hwnd=NULL);
	// ������
	void MouseWheel(int x, int y, int z, HWND hwnd=NULL);
	// ������
	void MouseWheel(int z, HWND hwnd=NULL);
	// ���������
	void Click(int x, int y, int ex, int ey, int flag = 0xff, HWND hwnd=NULL);
	// ���������
	void Click(int x, int y, int flag = 0xff, HWND hwnd=NULL);
	// ���������
	void Click_Send(int x, int y, int ex, int ey, int flag = 0xff, HWND hwnd = NULL);
	// ���������
	void Click_Send(int x, int y, int flag = 0xff, HWND hwnd = NULL);
	// ���������[���������ʺ�]
	void ClickOther(int x, int y, int ex, int ey, _account_* account_no);
	// ������˫��
	void DBClick(int x, int y, HWND hwnd=NULL);
	// ����
	void Keyboard(char key, int flag=0xff, HWND hwnd=NULL);
	// ��ȡ��������
	bool ReadCoor();
	// ��ȡ����Ѫ��
	bool ReadLife();
	// ��ȡ��ݼ�������Ʒ����
	bool ReadQuickKey2Num();
	// ��ȡ������Ʒ
	bool ReadBag();
	// �Ƿ���Ҫ��Ѫ��
	int  IsNeedAddLife(int low_life);
	// �Ƿ���ǰ����
	bool IsForegroundWindow();
	// ��Ѫ
	void AddLife();
	// ������Ϣ�������
	void SendMsg(const char* v, const char* v2=nullptr);
	// ֹͣ
	void Stop(bool v=true);
	// ���ռ��ļ�
	void OpenLogFile();
	// д���ռ�
	void WriteLog(const char* log, bool flush=true);
	// ����Ƿ��޸�������
	bool ChNCk();
public:
	// û��ͨ����֤
	bool m_bNoVerify = false;
	// Ҫ��������Ϸ����
	HWND m_hWndGame;
	// ��ǰ�������ʺ�
	_account_* m_pAccount;
	// ��Ϸָ��
	Game*     m_pGame;
	// ��Ϸ����ָ��
	GameStep* m_pGameStep;
	// ����ִ�еĲ���
	_step_* m_pStep;
	// ����ִ�еĲ���[����]
	_step_* m_pStepCopy;
	// �ϴ�ִ�еĲ���
	_step_* m_pStepLast;
	// �ϴ��ƶ��Ĳ���
	_step_* m_pStepLastMove;
	// �Ѽ�¼����[��ס�ػش˲���]
	_step_* m_pStepRecord;
	// �Ƿ��¼����
	bool m_bIsRecordStep;
	// �Ƿ�ֹͣ
	bool  m_bStop = false;
	// �Ƿ���ͣ
	bool  m_bPause = false;
	// �Ƿ����¿�ʼ
	bool  m_bReStart = false;
	// �Ƿ��˦
	bool m_bIsCrazy = false;
	// ��˦����
	char m_CrazyMagic[32];

	// �Ƿ��������븱��
	bool m_bLockGoFB = false;
	// �Ƿ��ڸ�������
	bool m_bAtFB = false;
	// �Ƿ���Ҫ����(�����ص�������ﲻ��Ҫ)
	bool m_bReborn = true;
	// �Ƿ����忭��
	bool m_bClearKaiRui = false;
	// �Ƿ����õ���¼����
	bool m_bIsResetRecordStep = false;
	// �Ƿ��һ���ƶ�, ��Ҫ��һ�µ�ͼ
	bool m_bIsFirstMove = true;
	// �Ƿ��渱��
	bool m_bPlayFB = true;
	// 
	bool m_bToBig = false;
	//
	bool m_bNeedCloseBag = false;
	// �Ƿ����������Ʒ
	bool m_bIsPickLast = false;

	// ���˵ڼ���BOSS
	int m_nBossNum = 0;
	// �Ƿ��ؿ�����0-���ؿ� 1-ֱ�ӳ�ȥ�ؿ� 2-�˳���¼���ؿ� 
	int m_nReOpenFB = 0;
	// �����ƶ�����
	int m_nReMoveCount = 0;
	// �����ƶ������ϴ�
	int m_nReMoveCountLast = 0;
	// ҩ������
	int m_nYaoBao = 0;
	// ҩ����
	int m_nYao = 0;
	// ҩ����������
	int m_nLiveYaoBao = 6;
	// ҩ��������
	int m_nLiveYao = 3;
	// �ؿ���������
	int m_nReOpenFBCount = 0;
	// ˢ��������
	int m_nPlayFBCount = 0;
	// �Ѿ�����ʱ��
	int m_nFBTimeLongOne = 0;
	// �Ѿ�����ʱ��
	int m_nFBTimeLong = 0;
	// ��ʼ����ʱ��
	int m_nStartFBTime = 0;
	// ���¸���ʱ��
	int m_nUpdateFBTimeLongTime = 0;
	// �������ʱ��
	int m_nRevivePetTime = 0;

	// ��һ�ζԻ�֮ѡ��
	char m_chLastSelectName[128];

	// �ϴε�����
	struct {
		DWORD OpCode;
		DWORD MvX;
		DWORD MvY;
		DWORD NPCOpCode; // �ǶԻ����ǵ��
		char  NPCName[32];  // ����
		DWORD ClickX;
		DWORD ClickY;
		DWORD ClickX2;
		DWORD ClickY2;
	} m_stLast;

	// ��������
	struct {
		int X;
		int Y;
		int X2;
		int Y2;
		int Count;
	} m_ClickCrazy;
};
