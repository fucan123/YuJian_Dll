#pragma once
#include <Windows.h>

class Game;
class Magic
{
public:
	Magic(Game* p);

	// 使用技能
	int  UseMagic(const char* name, int mv_x=0, int mv_y=0, int ms=0, int count=1);
	// 使用诸神裁决[mv_x=鼠标移动x, mv_y鼠标移动y]
	int  UseCaiJue(int mv_x, int mv_y, int ms);
	// 使用最终审判
	int  UseShenPan(int mv_x, int mv_y);
	// 使用攻击符
	void UseGongJiFu();
	// 使用神尊天降(选择区域的xy)
	bool UseShenZunTianJiang(const char* map);
	// 获取技能按键码
	BYTE GetMagicKey(const char* name);
	// 技能是否放出
	int MagicIsOut(const char* name);
public:
	// Game类
	Game* m_pGame;
	// 放出技能颜色数量
	int m_nPixelCount = 0;
};