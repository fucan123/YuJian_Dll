#pragma once
#include <Windows.h>

class Game;
class Pet
{
public:
	Pet(Game* p);

	// 宠物出征
	bool  PetOut(DWORD nos[], DWORD length);
	// 宠物合体
	bool  PetFuck(int no);
	// 复活所有没有血量宠物
	bool Revive();
	// 解析宠物血量 flag=0当前 flag=1最大
	int  ParsetLife(const char* text, int flag=0);
public:
	// Game类
	Game* m_pGame;
};