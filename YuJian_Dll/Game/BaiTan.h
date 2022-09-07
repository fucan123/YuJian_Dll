#pragma once

#include "GameData.h"
#include <Windows.h>

class Game;
class BaiTan
{
public:
	// ...
	BaiTan(Game* p);
	// ‘À––
	void Start();
	// …œº‹
	void UpItem(_account_* account);
public:
	Game* m_pGame;
};