#pragma once
#include <Windows.h>

class Game;
class Pet
{
public:
	Pet(Game* p);

	// �������
	bool  PetOut(DWORD nos[], DWORD length);
	// �������
	bool  PetFuck(int no);
	// ��������û��Ѫ������
	bool Revive();
	// ��������Ѫ�� flag=0��ǰ flag=1���
	int  ParsetLife(const char* text, int flag=0);
public:
	// Game��
	Game* m_pGame;
};