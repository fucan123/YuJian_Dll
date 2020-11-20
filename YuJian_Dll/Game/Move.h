#pragma once
#include <Windows.h>

#define ONE_COOR_PIX     30      // һ����ռ������
#define ONE_MAX_MOV_COOR 0x0E    // һ�������ƶ�8��������
#define ONE_MOV_MAX_MS   1000    // һ���ƶ��������ʱ��(����)

struct _account_;
class Game;
class Move
{
public:
	// ...
	Move(Game* p);

	// ��ʼ������
	void InitData();
	// ����ÿ���ƶ�����
	void SubOneMaxMovCoor(int v=0);
	// �ƶ���Ŀ�ĵ�
	int  RunEnd(DWORD x, DWORD y, _account_* account, DWORD time_out_ms=0);
	// �ƶ�
	int  Run(DWORD x, DWORD y, _account_* account, bool check_time=false);
	// �Ƶ�
	int  RunClick(DWORD x, DWORD y, DWORD click_x, DWORD click_y, _account_* account, bool check_time);
	// �����ƶ�λ��
	void SetMove(DWORD x, DWORD y, _account_* account, bool set_mv_time=true);
	// ����ƶ�����
	void ClearMove();
	// �Ƿ�ﵽ�յ�
	bool IsMoveEnd(_account_* account);
	// �Ƿ��ƶ�
	int  IsMove(_account_* account);
	// ����ʵ��Ҫ�ƶ�Ҫ������
	void CalcRealMovCoor(int& mv_x, int& mv_y, _account_* account);
	// �����������x
	int MakeClickX(int& x, int& y, int pos_x,  int dist_x);
	// �����������y
	int MakeClickY(int& x, int& y, int pos_y,  int dist_y);
	// �����������(pos_x=��ǰ����x,pos_y=��ǰ����y;dist_x=Ŀ������x,dist_x=Ŀ������y)
	int MakeClickCoor(int& x, int& y, int pos_x, int pos_y, int dist_x, int dist_y, int scrren_x, int screen_y);
	// ��Ϸ����ת��Ļ����
	int MakeClickCoor(int& x, int& y, int dist_x, int dist_y, _account_* account);
public:
	// ��Ϸ��
	Game* m_pGame;
	// һ���ƶ��������
	int   m_nOneMaxMovCoor;
	// ��ǰλ��X
	DWORD m_dwX = 0;
	// ��ǰλ��Y
	DWORD m_dwY = 0;
	// �ϴ�λ��X
	DWORD m_dwLastX = 0;
	// �ϴ�λ��Y
	DWORD m_dwLastY = 0;
	// �ƶ�λ��X
	DWORD m_dwMvX;
	// �ƶ�λ��Y
	DWORD m_dwMvY;
	// �ϴ��ƶ�λ��X
	DWORD m_dwLastMvX = 0;
	// �ϴ��ƶ�λ��Y
	DWORD m_dwLastMvY = 0;
	// �ж��ƶ�����ʱ��
	DWORD m_dwIsEndTime;
	// �ж��Ƿ��ƶ�ʱ��
	DWORD m_dwIsMvTime;
	// ���ڱȽϵ�X
	DWORD m_dwCmpX;
	// ���ڱȽϵ�Y
	DWORD m_dwCmpY;
	// �ƶ�ʱ��
	DWORD m_dwMvTime;
	// ��ȡ����ʱ��
	int   m_iGetPosTime;
	// ��ȡ������ʱ��
	int   m_iFlagPosTime;
};