#pragma once
#include <Windows.h>

class Game;
class Magic
{
public:
	Magic(Game* p);

	// ʹ�ü���
	int  UseMagic(const char* name, int mv_x=0, int mv_y=0, int ms=0, int count=1);
	// ʹ������þ�[mv_x=����ƶ�x, mv_y����ƶ�y]
	int  UseCaiJue(int mv_x, int mv_y, int ms);
	// ʹ����������
	int  UseShenPan(int mv_x, int mv_y);
	// ʹ�ù�����
	void UseGongJiFu();
	// ʹ�������콵(ѡ�������xy)
	bool UseShenZunTianJiang(const char* map);
	// ��ȡ���ܰ�����
	BYTE GetMagicKey(const char* name);
	// �����Ƿ�ų�
	int MagicIsOut(const char* name);
public:
	// Game��
	Game* m_pGame;
	// �ų�������ɫ����
	int m_nPixelCount = 0;
};