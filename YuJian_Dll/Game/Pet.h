#pragma once
#include <Windows.h>

class Game;
class Pet
{
public:
	Pet(Game* p);

	// �������
	bool  PetOut(int no);
	// �������
	bool  PetFuck(DWORD* nums, DWORD length);
	// ��������û��Ѫ������
	bool Revive();
public:
	// Game��
	Game* m_pGame;
};