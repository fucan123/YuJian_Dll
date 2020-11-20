#pragma once
#include <Windows.h>

#define ONE_COOR_PIX     30      // 一坐标占用像数
#define ONE_MAX_MOV_COOR 0x0E    // 一次最大可移动8距离坐标
#define ONE_MOV_MAX_MS   1000    // 一次移动最大允许时间(毫秒)

struct _account_;
class Game;
class Move
{
public:
	// ...
	Move(Game* p);

	// 初始化数据
	void InitData();
	// 设置每次移动距离
	void SubOneMaxMovCoor(int v=0);
	// 移动到目的地
	int  RunEnd(DWORD x, DWORD y, _account_* account, DWORD time_out_ms=0);
	// 移动
	int  Run(DWORD x, DWORD y, _account_* account, bool check_time=false);
	// 移点
	int  RunClick(DWORD x, DWORD y, DWORD click_x, DWORD click_y, _account_* account, bool check_time);
	// 设置移动位置
	void SetMove(DWORD x, DWORD y, _account_* account, bool set_mv_time=true);
	// 清除移动数据
	void ClearMove();
	// 是否达到终点
	bool IsMoveEnd(_account_* account);
	// 是否移动
	int  IsMove(_account_* account);
	// 计算实际要移动要的坐标
	void CalcRealMovCoor(int& mv_x, int& mv_y, _account_* account);
	// 制作点击坐标x
	int MakeClickX(int& x, int& y, int pos_x,  int dist_x);
	// 制作点击坐标y
	int MakeClickY(int& x, int& y, int pos_y,  int dist_y);
	// 制作点击坐标(pos_x=当前坐标x,pos_y=当前坐标y;dist_x=目标坐标x,dist_x=目标坐标y)
	int MakeClickCoor(int& x, int& y, int pos_x, int pos_y, int dist_x, int dist_y, int scrren_x, int screen_y);
	// 游戏坐标转屏幕坐标
	int MakeClickCoor(int& x, int& y, int dist_x, int dist_y, _account_* account);
public:
	// 游戏类
	Game* m_pGame;
	// 一次移动最大坐标
	int   m_nOneMaxMovCoor;
	// 当前位置X
	DWORD m_dwX = 0;
	// 当前位置Y
	DWORD m_dwY = 0;
	// 上次位置X
	DWORD m_dwLastX = 0;
	// 上次位置Y
	DWORD m_dwLastY = 0;
	// 移动位置X
	DWORD m_dwMvX;
	// 移动位置Y
	DWORD m_dwMvY;
	// 上次移动位置X
	DWORD m_dwLastMvX = 0;
	// 上次移动位置Y
	DWORD m_dwLastMvY = 0;
	// 判断移动结束时间
	DWORD m_dwIsEndTime;
	// 判断是否移动时间
	DWORD m_dwIsMvTime;
	// 用于比较的X
	DWORD m_dwCmpX;
	// 用于比较的Y
	DWORD m_dwCmpY;
	// 移动时间
	DWORD m_dwMvTime;
	// 获取坐标时间
	int   m_iGetPosTime;
	// 获取坐标标记时间
	int   m_iFlagPosTime;
};