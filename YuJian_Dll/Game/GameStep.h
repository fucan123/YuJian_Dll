#pragma once
#include <Windows.h>
#include <My/Common/Link.hpp>
#include <vector>
#include <list>

using namespace std;

typedef unsigned char      u_char;
typedef unsigned int       u_int;
typedef unsigned long long qword;

#define STEP_IS_MOVE(step) (step.OpCode==OP_MOVE)
#define STEP_IS_CLICK(step) (step.OpCode==OP_CLICK)
#define STEP_IS_SELECT(step) (step.OpCode==OP_SELECT)
#define STEP_IS_MOUMOVE(step) (step.OpCode==OP_MOUMOVE)
#define STEP_IS_WAIT(step) (step.OpCode==OP_KEY)
#define STEP_IS_WAIT(step) (step.OpCode==OP_WAIT)

#define USE_MY_LINK 1
#define MAX_STEP_FILES 16

enum STEP_CODE
{
	OP_UNKNOW  = 0x00,  // δ֪
	OP_MOVE,            // �ƶ�
	OP_MOVEFAR,         // ����
	OP_MOVERAND,        // ����
	OP_MOVEPICKUP,      // �ƶ�ȥ����
	OP_MOVENPC,         // ������һ��NPC
	OP_NPC,             // NPC�Ի�
	OP_SELECT,          // ѡ��Ի�ѡ��
	OP_MAGIC,           // ����
	OP_MAGIC_PET,       // ����-����
	OP_KAWEI,           // ��λ��BOSS
	OP_CRAZY,           // ��˦
	OP_CLEAR,           // ���
	OP_KAIRUI,          // ����
	OP_PICKUP,          // ����
	OP_CHECKIN,         // ����
	OP_USEITEM,         // ʹ����Ʒ
	OP_DROPITEM,        // ����
	OP_SELL,            // ������
	OP_BUTTON,          // �����ť
	OP_CLICK,           // ���
	OP_CLICKRAND,       // ���
	OP_CLICKCRAZ,       // ���
	OP_FIND,            // ����
	OP_WAIT,            // �ȴ�
	OP_WAIT_GW,         // �ȹ�
	OP_SMALL,           // С�Ų���
	OP_RECORD,          // ��¼
};

struct Point
{
	int x;
	int y;
	int flag; // 0-��Ļ���겻��Ҫת�� 1-������Ϸ������Ҫת��
};

// NPC��������Ϣ
struct _npc_coor_
{
	CHAR  Name[64];     // NPC����
	struct {
		DWORD MvX;      // ��Ҫ�ƶ�λ��X
		DWORD MvY;      // ��Ҫ�ƶ�λ��Y
		DWORD MvX2;     // ��Ҫ�ƶ�λ��X2, ������X-X2�����
		DWORD MvY2;     // ��Ҫ�ƶ�λ��Y2, ������Y-Y2�����
		DWORD ClkX;     // ���X
		DWORD ClkY;     // ���Y
		DWORD ClkX2;    // ���X, ������X-X2�����
		DWORD ClkY2;    // ���Y, ������Y-Y2�����
		DWORD Flag;     // 0-�������ƶ����, 1-�����ƶ�λ�����
	} Point[64];        // ���֧��64��λ��
	DWORD MvLength;     // �����ƶ�λ���������
	DWORD Length;       // ����λ������
};

// ��Ϸִ�в���
struct _step_
{
	char      Cmd[256];    // ԭʼ�ַ���
	STEP_CODE OpCode;      // ������

	DWORD     X;           // Ҫ������λ��X
	DWORD     Y;           // Ҫ������λ��Y
	DWORD     X2;
	DWORD     Y2;
	DWORD     NPCId;        // Ҫ�Ի���NPCID 
	CHAR      NPCName[32];  // Ҫ�Ի���NPC����
	DWORD     SelectNo;     // �Ի�ѡ������ 0��ʼ
	CHAR      Name[128];    // ���� ���ݲ�����������
	CHAR      Magic[128];   // ����
	DWORD     WaitMs;       // �ȴ����ٺ�����Ƿ�ȴ�������ȴ���ܿ����ж�������ȴ
	DWORD     OpCount;      // ��������
	DWORD     ButtonId;     // ��ťID
	int       SmallV;       // С�Ų���ֵ
	DWORD     Extra[8];     // ��չ
	__int64   ExecTime;     // ִ��ʱ��
	bool      Exec;         // ����ִ��
	int       Index;        // ����

	_npc_coor_* p_npc;      // ��Ӧ��һ��NPC��Ϣ
};

class Explode;
class GameStep
{
public:
	// ...
	GameStep();

	// ��ȡ����ִ�еĲ���
	_step_* Current(Link<_step_*>& link);
	// ��һ������ָ��
	_step_* Prev(Link<_step_*>& link);
	// �������ִ�в��� ������һ��
	_step_* CompleteExec(Link<_step_*>& link);
	// ��ȡ��һ����������
	_step_* GetNext(Link<_step_*>& link);
	// ��õ�ǰ���������
	STEP_CODE CurrentCode(Link<_step_*>& link);
	// �����һ���������
	STEP_CODE NextCode(Link<_step_*>& link);

	// ��ȡNPC�����ļ���Ϣ
	bool ReadNPCCoor(const char* path);
	// ��ȡNPC������Ϣ
	_npc_coor_* GetNpcCoor(const char* name);
	// ��ȡ����ִ�е������ļ�����
	char* GetStepFileName();
	// ����
	void SetConsoleTle(const char* cmd);
	// ���ѡ�����ļ�
	char* SelectRandStep();
	// ��ʼ������
	bool InitSteps(const char* path, const char* file);
	// ��ʼ��ȥ��������
	int InitGoLeiMingSteps();
	// ��������
	int ParseStep(const char* data, Link<_step_*>& link);
	// ����ִ�в�������
	void ResetStep(int index = 0, int flag = 0xff);
private:
	// ת��ʵ��ָ��
	STEP_CODE TransFormOP(const char* data);
	// ת��ʵ������
	bool TransFormPos(const char* str, _step_& step);
	// ת��ʵ������
	bool TransFormPos(const char* str, DWORD& x, DWORD& y);
	// ת��ʵ�ʼ���
	bool TransFormMagic(Explode& line, _step_& step);
	// ת��Wait����
	bool    TransFormWait(Explode& line, _step_& step);
	// С��
	void TransFormSmall(Explode& line, _step_& step);
	// ��Ӳ���
	void AddStep(_step_& step);
public:
	// NPC��Ϣ����
	int m_nNpcCoorCount = 0;
	// NPC��Ϣ�б�
	_npc_coor_ m_NpcCoor[50];
	// ����ִ�в����ļ�����
	int m_nStepCount = 0;
	// �����ļ��б�
	char m_chStepFiles[MAX_STEP_FILES][64];
	// m_nStepRandListIndex������ ����ѭ���� �ٴδ���
	int m_nStepRandIndex = -1;
	// ��������б�, ���ȴ��Ұ��������ڴ���������
	int  m_nStepRandListIndex[MAX_STEP_FILES];
	// ����ִ�в����ļ�
	Link<_step_*> m_StepList[MAX_STEP_FILES];
	// ��Ϸ��������(��ǰ����ִ��)
	Link<_step_*> m_Step;
	// ���ȥ������������
	Link<_step_*> m_GoLeiMingStep;

	// ��Ϸ��������
	int m_iStepCount = 0;
	// ��ǰִ�в�������
	int m_iStepIndex = 0;
	// ��ǰִ�в�������[m_GoLeiMingStep]
	int m_iStepGoLMIndex = 0;
	// ��Ϸ���˲������
	_step_* m_Steps;
	// ���迪ʼ����
	int m_iStepStartIndex = 0;
};