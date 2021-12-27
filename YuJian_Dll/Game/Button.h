#pragma once
#include "GameData.h"
#include <Windows.h>

#define BUTTON_ID_ROLE      0x3ED         // ���ﰴťID
#define BUTTON_ID_BAG       0x3EF         // ������ť"��Ʒ"
#define BUTTON_ID_TEAM      0x3F3         // ���鰴ť"���"
#define BUTTON_ID_SHEJIAO   0x402         // �罻��ť"����"
#define BUTTON_ID_SAVE_MNY  0x405         // �ֿ��Ǯ�洢"ȡ"
#define BUTTON_ID_CRTTEAM   0x43B         // ��������"1"
#define BUTTON_ID_CANCEL    0x44E         // ȡ����ť"ȡ��"
#define BUTTON_ID_CLOSESHOP 0x450         // �ر��̵갴ť
#define BUTTON_ID_BAG_SET   0x460         // ������"��Ʒ��-����ť"
#define BUTTON_ID_FIX_ALL   0x454         // ��������װ��"FIX_ALL"
#define BUTTON_ID_BAG_ITEM  0x46B         // ������Ʒ"MPC��Ʒ��"
#define BUTTON_ID_CLOSEBAG  0x471         // �رձ�����ť
#define BUTTON_ID_TIPSURE   0x477         // ��ʾȷ����ť"ȷ��"
#define BUTTON_ID_CUNQIAN   0x482         // �ֿ�ȷ����Ǯ"��"
#define BUTTON_ID_CLOSECKIN 0x485         // �رղֿⰴť
#define BUTTON_ID_CKIN_ITEM 0x486         // �ֿ���Ʒ"�洢��Ʒ��"
#define BUTTON_ID_CKIN_PGUP 0x488         // �ֿ����Ϸ�ҳ��ť"U"
#define BUTTON_ID_CKIN_PGDW 0x489         // �ֿ����·�ҳ��ť"D"
#define BUTTON_ID_CLOSEMENU 0x49C         // ͨ�ò˵��رհ�ť"C"��'x'��'X'��ر�
#define BUTTON_ID_REBORN_AT 0x4A2         // ԭ�ظ���"XP1"
#define BUTTON_ID_REBORN    0x4A3         // ����"XP2"
#define BUTTON_ID_ROLEVIP   0x5AD         // �㿪��������VIP��ťID
#define BUTTON_ID_BAG_PGUP  0x5C5         // �������Ϸ�ҳ��ť"up"
#define BUTTON_ID_BAG_PGDW  0x5C6         // �������·�ҳ��ť"down"
#define BUTTON_ID_FACE_BG   0x5F5         // �����б�"FACE_BG"
#define BUTTON_ID_VIP       0x44F         // VIP��ťID"VIP"
#define BUTTON_ID_LIAOTIANB 0x649         // ������϶���С��ť"�߶�"
#define BUTTON_ID_TEAMFLAG  0x713         // ͬ������Ǹ����İ�ť
#define BUTTON_ID_ITEM      0x751         // ��Ʒ��ݼ�"��Ʒ"
#define BUTTON_ID_MAGIC     0x752         // ���ܿ�ݼ�"����"
#define BUTTON_ID_INFB      0x7B2         // ͬ���������ť"ͬ��"
#define BUTTON_ID_JUJUE     0x7B3         // ͬ���������ť"�ܾ�"
#define BUTTON_ID_INTEAM    0x45A         // ͬ����Ӱ�ť"ͬ��"
//#define BUTTON_ID_INTEAM    0x815         // ͬ����Ӱ�ť
#define BUTTON_ID_SURE      0x8BA         // ȷ����ť"����"(���������Աȷ����ť)
#define BUTTON_ID_CHECKIN   0x960         // ��Ʒ�ֿⰴťID"VIP����---ʹ�òֿ�"
#define BUTTON_ID_LEAVE_LGC 0x9F1         // �뿪��������ť"�뿪������"
#define BUTTON_ID_XXX       0xAC9         // �ر�"X"
#define BUTTON_ID_LOGIN     0xA30         // ���밴ť
#define BUTTON_ID_ROLENO    0xD51         // ѡ���ɫ��[�����ɫӦ����+1����]
#define BUTTON_ID_SHJLIST   0xE2B         // �罻չ���б�"" ������45,62

#define BUTTON_ID_TASK_DUP  0x6B6         // ������Ϣ���ذ�ť"DUP"

#define BUTTON_ID_ACTY      0x00006568    // ��ʾ���ص�ͼ�·���б�ť"BTN_ACTY_SHOW"
#define BUTTON_ID_FULI      0x000F4240    // ����������ť(0x000F4240-0x000F4246)"Worldcup_CloseBtn"

#define STATIC_ID_MAP       0x56D         // ��ͼ����"{1 0x00000000}������Ŀ���"
#define STATIC_ID_POS_X     0x56E         // ����X"{1 0x00000000}905"
#define STATIC_ID_POS_Y     0x56F         // ����Y"{1 0x00000000}1063"
#define STATIC_ID_HP        0x5A1         // Ѫ��"HP:29696/29696����HP�ӳ�10%"
#define STATIC_ID_ROLE      0x5B1         // ��ɫ����"Ц����Ȼ"

#define STATIC_ID_PETLIFE_1 0xD45         // ����1Ѫ��"12587/12587"
#define STATIC_ID_PETLIFE_2 0xD49         // ����2Ѫ��"12587/12587"
#define STATIC_ID_PETLIFE_3 0xD4D         // ����3Ѫ��"12587/12587"
#define STATIC_ID_PETLIFE_4 0xD51         // ����4Ѫ��"12587/12587"

struct EnumWndInfo {
	int   Left;       // �������λ��, -1����
	int   Top;        // ���붥��λ��, -1����
	int   Width;      // ���, -1����
	int   Height;     // �߶�, -1����
	int   WidthFlag;  // 0Ϊ=Width, -1Ϊ<Width, -2Ϊ>Width, ����Ϊ����Χ
	int   HeightFlag; // 0Ϊ=Height, -1Ϊ<Height, -2Ϊ>Height, ����Ϊ����Χ
	char* Text;       // �ı�
	int   TextFlag;   // 0=Text, -1Ϊƥ��Text����
	HWND GameWnd;    // ��Ϸ���ھ��
	HWND ReturnV;    // ����ֵ
}; // Ҫö�ٵĴ�����Ϣ

class Game;
class MCButton
{
public:
	MCButton(Game* p);
	// ���ػ�б�
	bool HideActivity(HWND hwnd_own);
	// ��ť�Ƿ����
	bool IsDisabled(HWND hwnd_own, int button_id);
	// �����Ƿ����
	bool IsDisabled(HWND hWnd);
	// ����
	void Key(BYTE bVk);
	// ����
	void KeyDown(BYTE bVk);
	// ����
	void KeyUp(BYTE bVk);
	// �رհ�ť��ť
	bool CloseButton(HWND game, int button_id, const char* text=nullptr);
	// �����Ļ����
	bool ClickScreen(int x, int y, int flag = 0xff, bool left_click = true);
	// �����Ϸ����
	bool ClickPic(HWND game, HWND pic, int x, int y, DWORD sleep_ms=50, bool left_click=true);
	// ����Ի�ѡ��
	bool ClickTalk(HWND pic, int x, int y, bool moumov);
	// �����ť
	bool Click(HWND hwnd, int x, int y, int flag = 0xff, bool left_click = true);
	// �����ť
	bool Click(HWND hwnd_own, int button_id, const char* text=nullptr);
	// �����ť
	bool Click(HWND hwnd_own, int button_id, const char* text, int x, int y, int flag=0xff, bool left_click=true);
	// �����ť(����[��Ļ������])��Ҫת��(�����[hwnd_own������])
	bool ClickRel(HWND hwnd_own, int button_id, const char* text, int x, int y, int flag = 0xff, bool left_click = true);
	// ����ƶ���ָ��λ��
	void MouseMovePos(HWND hWnd, int x, int y);
	// ����Ǯ
	bool SaveMoney(HWND hwnd_own);
	// �򿪲ֿⴰ��
	bool OpenStorage(HWND hwnd_own);
	// �رղֿⴰ��
	bool CloseStorage(HWND hwnd_own);
	// ��鰴ť
	bool CheckButton(HWND hwnd_own, int button_id, const char* text = nullptr);
	// ��ȡ��������Ѫ������
	int  FindPetLifeWndAtTop(HWND game_wnd, HWND wnds[]);
	// ��ȡ�����������︸����
	HWND FindPetParentWndAtTop(HWND game_wnd);
	// ��ȡ�Ի��򴰿�
	HWND FindTalkWnd(HWND pic);
	// ��ȡ���ھ��
	HWND FindButtonWnd(HWND hwnd_own, int button_id, const char* text = nullptr);
	// ��ô��ھ��
	bool FindButtonWnd(HWND hwnd_own, int button_id, HWND& hwnd, HWND& parent, const char* text = nullptr);
	// �����Ϸ����
	HWND FindGameWnd(DWORD pid);
	// �����������Ϸ����λ��
	bool CalcRelGamePos(int& x, int& y, HWND game, int button_id, const char* text=nullptr);
	// ö�ٶԻ��򴰿�
	static BOOL CALLBACK EnumProcByWndInfo(HWND hWnd, LPARAM lParam);
	// ö�ٶԻ��򴰿�
	static BOOL CALLBACK EnumProcTalkWnd(HWND hWnd, LPARAM lParam);
	// ö�ٴ���
	static BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lParam);
	// ö���Ӵ���
	static BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam);
public:
	Game* m_pGame;
};