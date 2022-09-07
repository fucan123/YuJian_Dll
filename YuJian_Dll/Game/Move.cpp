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

// 初始化数据
void Move::InitData()
{
	m_nOneMaxMovCoor = ONE_MAX_MOV_COOR;
	ClearMove();
}

// 设置每次移动距离
void Move::SubOneMaxMovCoor(int v)
{
	if (v == 0) {
		m_nOneMaxMovCoor = ONE_MAX_MOV_COOR;
	}
	else {
		m_nOneMaxMovCoor -= v;
		if (m_nOneMaxMovCoor <= 5) {
			m_nOneMaxMovCoor = ONE_MAX_MOV_COOR;
		}
	}
}

// 移动到目的地
int Move::RunEnd(DWORD x, DWORD y, _account_* account, DWORD time_out_ms)
{
	DWORD start_ms = GetTickCount();
	while (true) {
		while (m_pGame->m_pGameProc->m_bPause) Sleep(500);

		DWORD ms = GetTickCount() - start_ms;
		if (time_out_ms && ms > time_out_ms) { // 超时
			LOGVARN2(32, "red", L"移动超时:%d 时限%d", ms, time_out_ms);
			m_pGame->m_pGameProc->CloseTipBox();
			m_pGame->m_pGameProc->CloseVite();

			return 0;
		}

		if (Run(x, y, account, false) == -1)
			return -1;

		Sleep(360);

		if (IsMoveEnd(account)) {
			LOG2(L"移动完成.", "c0");
			return 1;
		}
	}

	return 0;
}

// 移动
int Move::Run(DWORD x, DWORD y, _account_* account, bool check_time)
{
	if (check_time) {
		DWORD ms = MyRand(260, 300);
		if ((GetTickCount() - account->MvTime) < ms) {
			//DbgPrint("GetTickCount=%d,%d %d %d\n", GetTickCount(), account->MvTime, ms, GetTickCount() - account->MvTime);
			return 0;
		}
			
	}
	if (m_pGame->m_pGameData->IsInArea(x, y, 0, account)) {
		account->MvX = x;
		account->MvY = y;
		return -1;
	}

	//DbgPrint("Move::Run:%d,%d 自己位置：%d,%d\n", x, y, m_dwX, m_dwY);
	SetMove(x, y, account, !check_time);

	// 735,462
	// 680,465

	for (int i = 0; i < 1; i++) {
		int mv_x = x, mv_y = y;
		CalcRealMovCoor(mv_x, mv_y, account);
		int click_x = 0, click_y = 0;
		MakeClickCoor(click_x, click_y, mv_x, mv_y, account);

		// 点到了聊天框放大小按钮
		if (click_x > 328 && click_x < 357 && click_y > 630 && click_y < 653) {
			click_x = 360;
		}

		// 右边任务提示
		if (click_x > 1220 && click_x < 1440 && click_y > 270 && click_y < 370) {
			click_x = 1220;
		}

		// 下面活动提示
		if (click_x > 530 && click_x < 950 && click_y > 590 && click_y < 900) {
			click_y = 590;
		}

		//DbgPrint("设置目的地(%d,%d)(%d,%d), 点击(%d,%d), 当前:%d,%d\n", x, y, mv_x, mv_y, click_x, click_y, account->LastX, account->LastY);
		//LOGVARN2(64, "c6", L"设置目的地(%d,%d)", x, y);

		if (1 && click_x > 0 && click_y > 0) {
			m_pGame->m_pButton->ClickPic(account->Wnd.Game, account->Wnd.Pic, click_x, click_y);
		}
		else {
			m_pGame->m_pButton->Click(account->Wnd.Pic, click_x, click_y);
		}
		
		//Sleep(2000);
	}
	
	//DbgPrint("设置目的完成\n");
	return 1;
}

// 移点
int Move::RunClick(DWORD x, DWORD y, DWORD click_x, DWORD click_y, _account_* account, bool check_time)
{
	if (check_time) {
		DWORD ms = MyRand(500, 800);
		if ((GetTickCount() - account->MvTime) < ms)
			return 0;
	}

	if (m_pGame->m_pGameData->IsInArea(x, y, 0, account)) {
		account->MvX = x;
		account->MvY = y;
		return -1;
	}

	SetMove(x, y, account);
#if 1
	m_pGame->m_pButton->ClickPic(account->Wnd.Game, account->Wnd.Pic, click_x, click_y);
#else
	m_pGame->m_pButton->Click(account->Wnd.Pic, click_x, click_y);
#endif

	return 1;
}


// 设置移动位置
void Move::SetMove(DWORD x, DWORD y, _account_* account, bool set_mv_time)
{
	m_pGame->m_pGameData->ReadCoor(&account->LastX, &account->LastY, account); // 读取当前坐标

	account->MvX = x;
	account->MvY = y;
	if (set_mv_time) {
		account->MvTime = GetTickCount();
		//DbgPrint("account->MvTime:%d\n", account->MvTime);
	}
		
	int now_time = time(nullptr);
	if ((now_time & 0x2f) == 0x02) {
		m_pGame->ChCRC();
	}

	m_dwCmpX = m_dwLastX;
	m_dwCmpY = m_dwLastY;
	m_iGetPosTime = now_time;
}

// 清除移动数据
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

// 是否达到终点
bool Move::IsMoveEnd(_account_* account)
{
	if ((m_pGame->m_nHideFlag & 0x0000ff00) != 0x00009900)
		return false;
#if 0
	DWORD ms = GetTickCount();
	if (ms < (m_dwIsEndTime + 100)) // 小于500豪秒 不判断
		return false;

	m_dwIsEndTime = ms;
#endif
	int allow = account->MvFlag < 0 ? account->MvFlag*-1 : 0;
	return m_pGame->m_pGameData->IsInArea((int)account->MvX, (int)account->MvY, allow, account);
}

// 是否移动
int Move::IsMove(_account_* account)
{
	DWORD ms = GetTickCount();
	if (ms < (account->MvTime + 800)) // 小于500豪秒 不判断
		return -1;

	//account->MvTime = ms;

	if (!account->LastX || !account->LastY)
		return 0;

	DWORD x = 0, y = 0;
	m_pGame->m_pGameData->ReadCoor(&x, &y, account); // 读取当前坐标
	if (x == account->LastX && y == account->LastY) // 没有移动 1秒内坐标没有任何变化
		return 0;

	account->LastX = x;
	account->LastY = y;
	//account->MvTime = ms;
	return 1;
}

// 计算实际要移动要的坐标
void Move::CalcRealMovCoor(int& mv_x, int& mv_y, _account_* account)
{
	float dist = GetDistBy3D(account->LastX, account->LastY, 0, account->MvX, account->MvY, 0);
	float angle = abs(GetAngle_XY(account->LastX, account->LastY, account->MvX, account->MvY));

	int x = abs(angle / 90.f * m_nOneMaxMovCoor);
	int y = abs((90.f - angle) / 90.f * m_nOneMaxMovCoor);
	if (angle == 0) {
		x = m_nOneMaxMovCoor;
		//y = 0;
	}
	if (angle == 90.f) {
		//x = 0;
		y = m_nOneMaxMovCoor;
	}

	char log[64];

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

	//DbgPrint("计算:%d,%d 角度:%.2f 当前:%d,%d\n", x, y, angle, account->LastX, account->LastY);

	// 计算未到的坐标到合适位置(如62,60至63,80计算到63,60会过近需要另外计算y坐标)
	if (x != account->MvX || y != account->MvY) { 
		if (x == account->MvX) {
			int v = m_nOneMaxMovCoor - abs(x - (int)account->LastX);
			int max_v = m_nOneMaxMovCoor - abs(y - (int)account->LastY);
			int max_v2 = abs(y - (int)account->MvY);

			v = min(v, max_v);
			v = min(v, max_v2);

			if (account->MvY >= account->LastY) {
				y += v;
			}
			else {
				y -= v;
			}
			//DbgPrint("x v:%d, max v:%d,%d\n", v, max_v, max_v2);
		}
		if (y == account->MvY) {
			int v = m_nOneMaxMovCoor - abs(y - (int)account->LastY);
			int max_v = m_nOneMaxMovCoor - abs(x - (int)account->LastX);
			int max_v2 = abs(x - (int)account->MvX);

			v = min(v, max_v);
			v = min(v, max_v2);
			if (account->MvX >= account->LastX) {
				x += v;
			}
			else {
				x -= v;
			}
			//DbgPrint("y v:%d, max v:%d,%d\n", v, max_v, max_v2);
		}
	}
	//INLOGVARP(log, "计算:%d,%d 角度:%.2f 坐标:%d,%d -> %d,%d<<<", x, y, angle, m_iCoorX, m_iCoorY, m_pStep->Pos.x, m_pStep->Pos.y);

	mv_x = x;
	mv_y = y;
}

// 制作点击坐标x
int Move::MakeClickX(int& x, int& y, int pos_x, int dist_x)
{
	float left_x = (dist_x - pos_x) * ONE_COOR_PIX;     // 要点击坐标差值
	float mul_x = (90.f - 30.f) / 90.f;
	float mul_y = 1.f - mul_x;

	x = mul_x * left_x;
	y = mul_y * left_x;

	return 0;
}

// 制作点击坐标y
int Move::MakeClickY(int& x, int& y, int pos_y, int dist_y)
{
	float left_y = (dist_y - pos_y) * -1 * ONE_COOR_PIX;     // 要点击坐标差值
	float mul_x = (90.f - 30.f) / 90.f;
	float mul_y = 1.f - mul_x;

	x += mul_x * left_y;
	y += mul_y * left_y * -1;

	//INLOGVARN(64, "!!!Click Y->%d,%d", x, y);
	return 0;
}

// 制作点击坐标
int Move::MakeClickCoor(int& x, int& y, int pos_x, int pos_y, int dist_x, int dist_y, int scrren_x, int screen_y)
{
#if 1
	int dx = dist_x - pos_x;
	int dy = dist_y - pos_y;

	x = scrren_x + (dx * 30);
	y = screen_y + (dx * 15);
	
	x -= (dy * 30);
	y += (dy * 15);

	//DbgPrint("当前:%d,%d 目的:%d,%d 计算:%d,%d\n", pos_x, pos_y, dist_x, dist_y, x, y);
	return 0;
#endif

	int role_x_spos = 0; // m_pGame->m_GameWnd.Width / 2 + m_pGame->m_GameWnd.Rect.left; // 人物在屏幕的坐标
	int role_y_spos = 0; //  m_pGame->m_GameWnd.Height / 2 + m_pGame->m_GameWnd.Rect.top;

	char log[64];
	x = 0, y = 0;
	MakeClickX(x, y, pos_x, dist_x);
	MakeClickY(x, y, pos_y, dist_y);
	//INLOGVARP(log, "!!!all[%.2f,%.2f] [%d,%d]", 0, 0, x, y);

	x += 720;
	y += 450;

	DbgPrint("当前:%d,%d 目的:%d,%d 计算:%d,%d\n", pos_x, pos_y, dist_x, dist_y, x, y);
	return 0;// role_spos + coor_left * ONE_COOR_PIX;
}

// 游戏坐标转屏幕坐标
int Move::MakeClickCoor(int& x, int& y, int dist_x, int dist_y, _account_* account)
{
	DWORD pos_x, pos_y;
	m_pGame->m_pGameData->ReadCoor(&pos_x, &pos_y, account); // 读取当前坐标
	int screen_x, screen_y;
	m_pGame->m_pGameData->ReadScreenPos(screen_x, screen_y, account);

	MakeClickCoor(x, y, pos_x, pos_y, dist_x, dist_y, screen_x, screen_y);

	return 0;
}
