#include "Game.h"
#include "GameData.h"
#include "Move.h"
#include "Button.h"
#include "PrintScreen.h"
#include "GameProc.h"

#include <time.h>
#include <My/Common/func.h>
#include <My/Common/C.h>

Move::Move(Game* p)
{
	m_pGame = p;
	InitData();
}

// ��ʼ������
void Move::InitData()
{
	ClearMove();
}

// �ƶ�
int Move::Run(DWORD x, DWORD y, _account_* account, bool check_time)
{
	if (check_time) {
		DWORD ms = MyRand(350, 500);
		if ((GetTickCount() - account->MvTime) < ms)
			return 0;
	}
	if (m_pGame->m_pGameData->IsInArea(x, y, 0, account)) {
		account->MvX = x;
		account->MvY = y;
		return -1;
	}

	//DbgPrint("Move::Run:%d,%d �Լ�λ�ã�%d,%d\n", x, y, m_dwX, m_dwY);
	SetMove(x, y, account);

	// 735,462
	// 680,465

	for (int i = 0; i < 1; i++) {
		int mv_x = x, mv_y = y;
		CalcRealMovCoor(mv_x, mv_y, account);
		int click_x = 0, click_y = 0;
		MakeClickCoor(click_x, click_y, x, y, account);

		DbgPrint("����Ŀ�ĵ�(%d,%d)(%d,%d), ���(%d,%d), ��ǰ:%d,%d\n", x, y, mv_x, mv_y, click_x, click_y, account->LastX, account->LastY);
		LOGVARN2(64, "c6", L"����Ŀ�ĵ�(%d,%d)", x, y);

		if (0 && click_x > 0 && click_y > 0) {
			m_pGame->m_pButton->ClickPic(account->Wnd.Game, account->Wnd.Pic, click_x, click_y);
		}
		else {
			m_pGame->m_pButton->Click(account->Wnd.Pic, click_x, click_y);
		}
		
		//Sleep(2000);
	}
	

	return 1;
}


// �����ƶ�λ��
void Move::SetMove(DWORD x, DWORD y, _account_* account)
{
	m_pGame->m_pGameData->ReadCoor(&account->LastX, &account->LastY, account); // ��ȡ��ǰ����

	account->MvX = x;
	account->MvY = y;
	account->MvTime = GetTickCount();

	int now_time = time(nullptr);
	if ((now_time & 0x2f) == 0x02) {
		m_pGame->ChCRC();
	}

	m_dwCmpX = m_dwLastX;
	m_dwCmpY = m_dwLastY;
	m_iGetPosTime = now_time;
}

// ����ƶ�����
void Move::ClearMove()
{
	m_dwX = 0;
	m_dwY = 0;
	m_dwLastX = 0;
	m_dwLastY = 0;
	m_dwLastMvX = 0;
	m_dwLastMvY = 0;
	m_dwMvX = 0;
	m_dwMvY = 0;

	m_dwIsEndTime = 0;
	m_dwIsMvTime = 0;
	m_dwMvTime = 0;

	m_dwCmpX = 0;
	m_dwCmpY = 0;
	m_iGetPosTime = 0;
	m_iFlagPosTime = 0;
}

// �Ƿ�ﵽ�յ�
bool Move::IsMoveEnd(_account_* account)
{
#if 0
	DWORD ms = GetTickCount();
	if (ms < (m_dwIsEndTime + 100)) // С��500���� ���ж�
		return false;

	m_dwIsEndTime = ms;
#endif
	DWORD x, y;
	m_pGame->m_pGameData->ReadCoor(&x, &y, account); // ��ȡ��ǰ����
	//DbgPrint("IsMoveEnd %d,%d %d,%d\n", m_dwX, m_dwY, m_dwMvX, m_dwMvY);
	// 0x168999CB
	return x == account->MvX && y == account->MvY && (m_pGame->m_nHideFlag & 0x0000ff00) == 0x00009900;
}

// �Ƿ��ƶ�
bool Move::IsMove(_account_* account)
{
	DWORD ms = GetTickCount();
	if (ms < (account->MvTime + 680)) // С��500���� ���ж�
		return true;

	account->MvTime = ms;

	if (!account->LastX || !account->LastY)
		return false;

	DWORD x = 0, y = 0;
	m_pGame->m_pGameData->ReadCoor(&x, &y, account); // ��ȡ��ǰ����
	if (x == account->LastX && y == account->LastY) // û���ƶ� 1��������û���κα仯
		return false;

	account->LastX = x;
	account->LastY = y;
	account->MvTime = ms;
	return true;
}

// ����ʵ��Ҫ�ƶ�Ҫ������
void Move::CalcRealMovCoor(int& mv_x, int& mv_y, _account_* account)
{
	float dist = GetDistBy3D(account->LastX, account->LastY, 0, account->MvX, account->MvY, 0);
	float angle = abs(GetAngle_XY(account->LastX, account->LastY, account->MvX, account->MvY));

	int x = angle / 90.f * ONE_MAX_MOV_COOR;
	int y = (90.f - angle) / 90.f * ONE_MAX_MOV_COOR;
	if (angle == 0) {
		x = ONE_MAX_MOV_COOR;
		//y = 0;
	}
	if (angle == 90.f) {
		//x = 0;
		y = ONE_MAX_MOV_COOR;
	}

	char log[64];
	printf("����:%d,%d �Ƕ�:%.2f", x, y, angle);

	if (account->MvX >= account->LastX) {
		x += account->LastX;
		if (x > account->MvX)
			x = account->MvX;
	}
	else {
		x = account->LastX - x;
		if (x < account->MvX)
			x = account->MvX;
	}
	if (account->MvY >= account->LastY) {
		y += account->LastY;
		if (y > account->MvY)
			y = account->MvY;
	}
	else {
		y = account->LastY - y;
		if (y < account->MvY)
			y = account->MvY;
	}
	//INLOGVARP(log, "����:%d,%d �Ƕ�:%.2f ����:%d,%d -> %d,%d<<<", x, y, angle, m_iCoorX, m_iCoorY, m_pStep->Pos.x, m_pStep->Pos.y);

	mv_x = x;
	mv_y = y;
}

// �����������x
int Move::MakeClickX(int& x, int& y, int pos_x, int dist_x)
{
	float left_x = (dist_x - pos_x) * ONE_COOR_PIX;     // Ҫ��������ֵ
	float mul_x = (90.f - 30.f) / 90.f;
	float mul_y = 1.f - mul_x;

	x = mul_x * left_x;
	y = mul_y * left_x;

	return 0;
}

// �����������y
int Move::MakeClickY(int& x, int& y, int pos_y, int dist_y)
{
	float left_y = (dist_y - pos_y) * -1 * ONE_COOR_PIX;     // Ҫ��������ֵ
	float mul_x = (90.f - 30.f) / 90.f;
	float mul_y = 1.f - mul_x;

	x += mul_x * left_y;
	y += mul_y * left_y * -1;

	//INLOGVARN(64, "!!!Click Y->%d,%d", x, y);
	return 0;
}

// �����������
int Move::MakeClickCoor(int& x, int& y, int pos_x, int pos_y, int dist_x, int dist_y, int scrren_x, int screen_y)
{
#if 1
	int dx = dist_x - pos_x;
	int dy = dist_y - pos_y;

	x = scrren_x + (dx * 30);
	y = screen_y + (dx * 15);
	
	x -= (dy * 30);
	y += (dy * 15);

	printf("��ǰ:%d,%d Ŀ��:%d,%d ����:%d,%d\n", pos_x, pos_y, dist_x, dist_y, x, y);
	return 0;
#endif

	int role_x_spos = 0; // m_pGame->m_GameWnd.Width / 2 + m_pGame->m_GameWnd.Rect.left; // ��������Ļ������
	int role_y_spos = 0; //  m_pGame->m_GameWnd.Height / 2 + m_pGame->m_GameWnd.Rect.top;

	char log[64];
	x = 0, y = 0;
	MakeClickX(x, y, pos_x, dist_x);
	MakeClickY(x, y, pos_y, dist_y);
	//INLOGVARP(log, "!!!all[%.2f,%.2f] [%d,%d]", 0, 0, x, y);

	x += 720;
	y += 450;

	printf("��ǰ:%d,%d Ŀ��:%d,%d ����:%d,%d\n", pos_x, pos_y, dist_x, dist_y, x, y);
	return 0;// role_spos + coor_left * ONE_COOR_PIX;
}

// ��Ϸ����ת��Ļ����
int Move::MakeClickCoor(int& x, int& y, int dist_x, int dist_y, _account_* account)
{
	DWORD pos_x, pos_y;
	m_pGame->m_pGameData->ReadCoor(&pos_x, &pos_y, account); // ��ȡ��ǰ����
	int screen_x, screen_y;
	m_pGame->m_pGameData->ReadScreenPos(screen_x, screen_y, account);

	MakeClickCoor(x, y, pos_x, pos_y, dist_x, dist_y, screen_x, screen_y);

	return 0;
}
