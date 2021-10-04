#include "Game.h"
#include "Driver.h"
#include "GameConf.h"
#include "GameData.h"
#include "GameClient.h"
#include "GameProc.h"
#include "Move.h"
#include "Item.h"
#include "Magic.h"
#include "Talk.h"
#include "Pet.h"
#include "Button.h"
#include "PrintScreen.h"

#include <ShlObj_core.h>
#include <My/Common/mystring.h>
#include <My/Common/func.h>
#include <My/Common/C.h>
#include <My/Common/Explode.h>
#include <psapi.h>
#include <stdio.h>
#include <time.h>

#include "../Asm.h"

#if 1
#if 0
#define SMSG_D(v) m_pGame->m_pClient->SendMsg2(v)
#else
#define SMSG_D(v)
#endif
#define SMSG_DP(p,...) { sprintf_s(p,__VA_ARGS__);SMSG_D(p); }
#else
#define SMSG_D(v) 
#define SMSG_DP(v) 
#endif
#define SMSG(v) SendMsg(v)
#define SMSG_P(p,...) { sprintf_s(p,__VA_ARGS__);SMSG(p); }
#define SMSG_N(n,...) {char _s[n]; SMSG_P(_s,__VA_ARGS__); }

// !!!
GameProc::GameProc(Game* p)
{
	m_pGame = p;
	m_pGameStep = new GameStep;
	InitData(1);
}

// ��ʼ������
void GameProc::InitData(bool v)
{
	m_bNoVerify = false;
	m_bStop = false;
	m_bPause = false;
	m_bReStart = false;
	m_bIsCrazy = false;
	m_pStepCopy = nullptr;
	m_pStepLast = nullptr;
	m_pStepLastMove = nullptr;
	m_pStepRecord = nullptr;

	m_bReborn = true;
	m_bIsRecordStep = false;
	m_bClearKaiRui = false;
	m_bIsResetRecordStep = false;
	m_bIsFirstMove = true;
	m_bPlayFB = true;
	m_bAtFB = false;
	m_bToBig = false;
	m_bNeedCloseBag = false;
	m_bIsPickLast = false;

	m_nBossNum = 0;
	m_nFBTimeLongOne = 0;
	m_nReOpenFB = 0;
	m_nReMoveCount = 0;
	m_nReMoveCountLast = 0;
	m_nYaoBao = 0;
	m_nYao = 0;
	m_nLiveYaoBao = 8;
	m_nLiveYao = 6;

	m_nRevivePetTime = 0;

	ZeroMemory(&m_stLast, sizeof(m_stLast));
	ZeroMemory(&m_ClickCrazy, sizeof(m_ClickCrazy));
	if (v) {
		m_p_white_dlls = new white_dlls;
		memset(m_p_white_dlls, 0, sizeof(white_dlls));
		wcscpy(m_p_white_dlls->name[0],  L"c:\\windows\\system32\\appverifui.dll");
		wcscpy(m_p_white_dlls->name[1],  L"c:\\windows\\system32\\containerdevicemanagement.dll");
		wcscpy(m_p_white_dlls->name[2],  L"c:\\windows\\system32\\dwmscene.dll");
		wcscpy(m_p_white_dlls->name[3],  L"c:\\windows\\system32\\fsnvsdevicesource.dll");
		wcscpy(m_p_white_dlls->name[4],  L"c:\\windows\\system32\\heatcore.dll");
		wcscpy(m_p_white_dlls->name[5],  L"c:\\windows\\system32\\hvsocket.dll");
		wcscpy(m_p_white_dlls->name[6],  L"c:\\windows\\system32\\ihds.dll");
		wcscpy(m_p_white_dlls->name[7],  L"c:\\windows\\system32\\libmysql_e.dll");
		wcscpy(m_p_white_dlls->name[8],  L"c:\\windows\\system32\\mfcu140uxd.dll");
		wcscpy(m_p_white_dlls->name[9],  L"c:\\windows\\system32\\rdsxvmaudio.dll");
		wcscpy(m_p_white_dlls->name[10], L"c:\\windows\\system32\\resbparser.dll");
		wcscpy(m_p_white_dlls->name[11], L"c:\\windows\\system32\\scredentialprovider.dll");
		wcscpy(m_p_white_dlls->name[12], L"c:\\windows\\system32\\spectrumsyncclient.dll");
		wcscpy(m_p_white_dlls->name[13], L"c:\\windows\\system32\\ssdm.dll");
		wcscpy(m_p_white_dlls->name[14], L"c:\\windows\\system32\\textinputmethodformatter.dll");
		wcscpy(m_p_white_dlls->name[15], L"c:\\windows\\system32\\umpdc.dll");
		wcscpy(m_p_white_dlls->name[16], L"c:\\windows\\system32\\usocoreps.dll");
		wcscpy(m_p_white_dlls->name[17], L"c:\\windows\\system32\\usbpmapi.dll");
		wcscpy(m_p_white_dlls->name[18], L"c:\\windows\\system32\\virtualmonitormanager.dll");
		wcscpy(m_p_white_dlls->name[19], L"c:\\windows\\system32\\windows.applicationmodel.conversationalagent.internal.proxystub.dll");
		wcscpy(m_p_white_dlls->name[20], L"c:\\windows\\system32\\windows.applicationmodel.conversationalagent.proxystub.dll");
		wcscpy(m_p_white_dlls->name[21], L"c:\\windows\\system32\\xboxgipsynthetic.dll");
		wcscpy(m_p_white_dlls->name[22], L"c:\\windows\\system32\\inputhost.dll");
		wcscpy(m_p_white_dlls->name[23], L"c:\\windows\\system32\\windows.mirage.dll");
		wcscpy(m_p_white_dlls->name[24], L"c:\\windows\\system32\\windowsdefaultheatprocessor.dll");
		wcscpy(m_p_white_dlls->name[25], L"c:\\windows\\system32\\xboxgipsynthetic.dll");
		wcscpy(m_p_white_dlls->name[26], L"c:\\windows\\system32\\php-mysqli.dll");
		wcscpy(m_p_white_dlls->name[26], L"c:\\windows\\system32\\tmp.bak");
		wcscpy(m_p_white_dlls->name[27], L"c:\\users\\fucan\\desktop\\mnq-9star\\vs\\x64\\ld.dll");
		wcscpy(m_p_white_dlls->name[27], L"c:\\users\\fucan\\desktop\\mnq-9star\\vs\\x64\\yujian.dll");
	

		m_p_process_dlls = new process_dlls;
		memset(m_p_process_dlls, 0, sizeof(process_dlls));
	}
}

// ��ʼ������
bool GameProc::InitSteps()
{
	if (!m_pGameStep->ReadNPCCoor(m_pGame->m_chPath)) {
	}

	m_pGameStep->InitGoLeiMingSteps(); // ��ʼ�����ȥ��������

	Asm_Nd(GetCurrentThread(), m_pGame->GetNdSysCallIndex()); // ��ֹ������

	for (int i = 0; i < m_pGame->m_pGameConf->m_Setting.FBFileCount; i++) {
		m_pGameStep->InitSteps(m_pGame->m_chPath, m_pGame->m_pGameConf->m_Setting.FBFile[i]);
	}
	return m_pGame->m_pGameConf->m_Setting.FBFileCount > 0;
}

// �л���Ϸ����
void GameProc::SwitchGameWnd(HWND hwnd)
{
	m_hWndGame = hwnd;
	m_pGame->m_hWndBig = hwnd;
}

// �л���Ϸ�ʺ�
void GameProc::SwitchGameAccount(_account_ * account)
{
	m_pAccount = account;
	SwitchGameWnd(account->Wnd.Game);
}

// ����ƶ�����Ϸ����λ��
void GameProc::SetGameCursorPos(int x, int y)
{
	return;
	HWND top = GetForegroundWindow();
	if (top != m_pAccount->Wnd.Game && top != m_pAccount->Wnd.Pic) // ���ڲ�����ǰ
		return;

	RECT rect;
	::GetWindowRect(m_pAccount->Wnd.Game, &rect);

	x += 0;// rect.left;
	y += 0;// rect.top;

	int result = SetCursorPos(rect.left + x, rect.top + y);
	//LOGVARN2(64, "blue_r b", L"�������(%d,%d) %d %d", x, y, result, GetLastError());
}

// ������ǰ
void GameProc::SetForegroundWindow(_account_* account)
{

	//���붯̬�������������  
	typedef void (WINAPI *PROCSWITCHTOTHISWINDOW)(HWND, BOOL);
	PROCSWITCHTOTHISWINDOW SwitchToThisWindow;
	HMODULE hUser32 = GetModuleHandle(L"user32");
	SwitchToThisWindow = (PROCSWITCHTOTHISWINDOW)GetProcAddress(hUser32, "SwitchToThisWindow");

	//������ֻҪ���κ��ִ洰�ڵľ����������������ɣ�
	//�ڶ���������ʾ������ڴ�����С��״̬���Ƿ�ָ���
	SwitchToThisWindow(account->Wnd.Game, TRUE);
	::SetForegroundWindow(account->Wnd.Game);

	Sleep(100);
	//m_pGame->m_pButton->Click(account->Wnd.Game, 720, 520);
	//m_pGame->m_pButton->ClickPic(account->Wnd.Game, account->Wnd.Pic, 720, 450);
}

// �ȴ���ʾ��
void GameProc::WaitGameMenu(_account_* account)
{
	LOG2(L"�ȴ���Ϸ�˵�...", "c6");
	time_t second = 20;
	time_t s = time(nullptr);
	while (true) {
		Sleep(1000);

		time_t tl = time(nullptr) - s;
		if (tl >= 15) {
			LOG2(L"�ȴ���ʱ", "red");
			return;
		}

		if (m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "x")) {
			LOG2(L"�ر���Ϸ�˵�...", "c0");
			Sleep(1000);
			if (m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_SURE, "ȷ��"))
				Sleep(1000);
			if (m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_CANCEL))
				Sleep(1000);
			if (m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_CLOSEMENU))
				Sleep(1000);

			return;
		}
	}

}

// ���ȥ������½����
void GameProc::GoLeiMing(_account_* account)
{
	if (!account->Wnd.Map) {
		account->Wnd.Map = m_pGame->m_pButton->FindButtonWnd(account->Wnd.Game, STATIC_ID_MAP);
		if (!account->Wnd.Map)
			return;
	}

	wchar_t log[64];
	char map[32];
	::GetWindowTextA(account->Wnd.Map, map, sizeof(map));
	DbgPrint("%hs Say ��ͼ:%hs\n", account->Name, map);
	LOGVARP2(log, "c6", L"%hs Say ��ͼ:%hs", account->Name, map);
	if (!m_pGame->m_pGameData->IsTheMap("���", account))
		return;

	Sleep(2000);
	// 62,59 660,588 666,600 �޵¡���
	// 602,305 810,280 815,300
	// 60,185 ����ȥ����������
	DbgPrint("%hs Say ׼���뿪���\n", account->Name);
	LOGVARP2(log, "c6", L"%hs Say ׼���뿪���", account->Name);
	DWORD pos_x, pos_y, click_x, click_y;
	if (!m_pGame->m_pButton->FindButtonWnd(account->Wnd.Game, BUTTON_ID_LEAVE_LGC, "�뿪������")) {
		Sleep(6000);
		while (true) {
			m_pGame->m_pGameData->ReadCoorByWnd(&pos_x, &pos_y, account);
			GetNPCClickPos(62, 59, 660, 588, 666, 600, pos_x, pos_y, click_x, click_y);
			DbgPrint("%hs Say �Ի�: �޵¡��� %d,%d->%d,%d", account->Name, pos_x, pos_y, click_x, click_y);
			LOGVARP2(log, "c6", L"%hs Say �Ի�: �޵¡��� %d,%d->%d,%d", account->Name, pos_x, pos_y, click_x, click_y);

			m_pGame->m_pButton->ClickPic(account->Wnd.Game, account->Wnd.Pic, click_x, click_y);
			Sleep(1000);
			if (m_pGame->m_pButton->FindButtonWnd(account->Wnd.Game, BUTTON_ID_LEAVE_LGC, "�뿪������"))
				break;
			if (!m_pGame->m_pGameData->IsTheMap("���", account))
				return;
		}
	}

	Sleep(800);
	DbgPrint("%hs Say �뿪������\n", account->Name);
	LOGVARP2(log, "c6", L"%hs Say �뿪������", account->Name);
	while (true) {
		m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_LEAVE_LGC, "�뿪������");
		Sleep(800);
		if (m_pGame->m_pGameData->IsTheMap("��������", account))
			break;
	}

	Sleep(6000);
	DbgPrint("%hs Say ׼���뿪����\n", account->Name);
	LOGVARP2(log, "c6", L"%hs Say ׼���뿪����", account->Name);
	while (true) {
		m_pGame->m_pGameData->ReadCoorByWnd(&pos_x, &pos_y, account);
		GetNPCClickPos(602, 305, 810, 280, 815, 300, pos_x, pos_y, click_x, click_y);
		LOGVARP2(log, "c6", L"%hs Say �Ի�: ������ %d,%d->%d,%d", account->Name, pos_x, pos_y, click_x, click_y);
		LOGVARP2(log, "c6", L"%hs Say �Ի�: ������ %d,%d->%d,%d", account->Name, pos_x, pos_y, click_x, click_y);

		m_pGame->m_pButton->ClickPic(account->Wnd.Game, account->Wnd.Pic, click_x, click_y);
		Sleep(300);
		if (!m_pGame->m_pTalk->WaitTalkOpen(account->Wnd.Pic)) {
			if (!m_pGame->m_pGameData->IsTheMap("��������", account))
				break;

			continue;
		}

		Sleep(500);
		m_pGame->m_pTalk->Select("��Ҫȥ����������", account->Wnd.Pic, true);
		Sleep(1000);
		if (!m_pGame->m_pGameData->IsTheMap("��������", account))
			break;
	}

	DbgPrint("%hs Say �ѵִ�����������\n", account->Name);
	LOGVARP2(log, "c0", L"%hs Say �ѵִ�����������", account->Name);
}

// ȥ��ȡ����
void GameProc::GoGetXiangLian(Account* account)
{
	wchar_t log[64];
	LOGVARP2(log, "c0 b", L"%hsȥ������.", account->Name);
	while (true) {
		if (m_bPause || m_bAtFB)
			break;

		this->SetForegroundWindow(account);
		m_pGame->m_pItem->GoShop();
		Sleep(1000);
		if (m_pGame->m_pGameData->IsInArea(250, 500, 10, account))
			break;
	}

	Sleep(8000);
	_npc_coor_ npcCoor;
	npcCoor.Point[0].MvX = 250;
	npcCoor.Point[0].MvY = 500;
	npcCoor.Point[0].ClkX = 460;
	npcCoor.Point[0].ClkY = 350;
	npcCoor.Point[0].ClkX2 = 460;
	npcCoor.Point[0].ClkY2 = 350;
	npcCoor.Length = 1;

	this->SetForegroundWindow(account);
	CloseTipBox();

	DWORD pos_x, pos_y;
	m_pGame->m_pGameData->ReadCoor(&pos_x, &pos_y, account);
	int click_x, click_y;
	GetNPCClickPos(&npcCoor, pos_x, pos_y, (DWORD&)click_x, (DWORD&)click_y);
	if (m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "C")) {
		Sleep(500);
	}
	if (m_pGame->m_pButton->CloseButton(account->Wnd.Game, BUTTON_ID_XXX, "X")) {
		Sleep(500);
	}

	DbgPrint("(%hs)�������(%d,%d).\n", account->Name, click_x, click_y);
	LOGVARP2(log, "c6", L"(%hs)�������(%d,%d).", account->Name, click_x, click_y);
	m_pGame->m_pButton->ClickPic(account->Wnd.Game, account->Wnd.Pic, click_x, click_y, 300);
	if (m_pGame->m_pTalk->WaitTalkOpen(account->Wnd.Pic)) {
		Sleep(1500);
		m_pGame->m_pTalk->Select("��ȡ����������", account->Wnd.Pic, true);
		Sleep(1000);
		m_pGame->m_pTalk->Select("��ȡ����������", account->Wnd.Pic, true);
		Sleep(1000);
		m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "C");
		Sleep(1000);
	}

	m_pGame->m_pGameData->ReadCoor(&pos_x, &pos_y, account);
	GetNPCClickPos(&npcCoor, pos_x, pos_y, (DWORD&)click_x, (DWORD&)click_y);
	DbgPrint("(%hs)�������(%d,%d).\n", account->Name, click_x, click_y);
	LOGVARP2(log, "c6", L"(%hs)�������(%d,%d).", account->Name, click_x, click_y);
	m_pGame->m_pButton->ClickPic(account->Wnd.Game, account->Wnd.Pic, click_x, click_y, 300);
	if (m_pGame->m_pTalk->WaitTalkOpen(account->Wnd.Pic)) {
		Sleep(1500);
		m_pGame->m_pTalk->Select("��ȡ���ƾ�����", account->Wnd.Pic, true);
		Sleep(1000);
		m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "C");
		Sleep(1000);
	}
}

// ѯ����������
_account_* GameProc::AskXiangLian()
{
	Account* account = nullptr;
	Account* account2 = nullptr;
	int max = 0;
	for (int i = 0; i < m_pGame->m_AccountList.size(); i++) {
		_account_* p = m_pGame->m_AccountList[i];
		if (!m_pGame->IsOnline(p))
			continue;

		if (!p->IsReadXL) { // û�ж�ȡ����
			SwitchGameAccount(p);
			this->SetForegroundWindow(p);
			Sleep(500);

			LOGVARN2(64, "c0 b", L"\n%hs ���ڶ�ȡ������Ŀ...", p->Name);

			int qiu = 0;
			p->XL = m_pGame->m_pItem->GetBagItemCount("����������", &(p->YaoShi), &qiu);
			p->IsReadXL = 1;
			p->IsReadYaoShi = 1;
		}

		if (p->XL > max) {
			max = p->XL;
			account = p;
		}

		if (p->YaoShi && !p->IsBig) {
			account2 = p;
		}

		DbgPrint("%s������%d��, Կ��%d�� %08X\n", p->Name, p->XL, p->YaoShi, p->Wnd.Game);
		LOGVARN2(64, "c0", L"%hs������<b class=\"red\">%d/%d</b>��|��",
			p->Name, p->XL, p->YaoShi);

		//
	}
	if (IsBigOpenFB()) {
		if (!account2 && m_pGame->m_pBig && m_pGame->m_pBig->YaoShi) {
			account2 = m_pGame->m_pBig;
		}
		account = account2;
		if (!account) {
			LOG2(L"���ʱ��û��Կ��, �޷�����", "reb b");
		}
	}

	return account;
}

// ȥ�����ſ�
void GameProc::GoFBDoor(_account_* account, int num)
{
	DbgPrint("%hsȥ�����ſ�.\n", account->Name);
	LOGVARN2(32, "c0 b", L"%hsȥ�����ſ�.", account->Name);
	this->SetForegroundWindow(account);
	if (m_pGame->m_pGameData->IsInFBDoor(account)) {
		DbgPrint("%hs�Ѿ��ڸ����ſ�\n", account->Name);
		LOG2(L"�Ѿ��ڸ����ſ�", "green b");
		return;
	}

	int i = 0;
	while (true) {
		if ((m_bPause || m_bAtFB) && num == 0) 
			break;

		this->SetForegroundWindow(account);
		if (++i == 10)
			m_pGame->SaveScreen("ȥ�����ſ�");

		Sleep(500);
		DbgPrint("%sʹ��ȥ�����ſ��ǳ�֮��\n", account->Name);
		LOGVARN2(64, "green", L"%hsʹ��ȥ�����ſ��ǳ�֮��(8��)", account->Name);
		if (m_pGame->m_pShareTeam) {
			//memset(m_pGame->m_pShareTeam->gofbdoor, 1, sizeof(m_pGame->m_pShareTeam->gofbdoor));
		}
		m_pGame->m_pItem->GoFBDoor();
		Sleep(3500);
		if (m_pGame->m_pGameData->IsInFBDoor(account)) {
			Sleep(2000);
			DbgPrint("%s�ѵ��︱���ſ�\n", account->Name);
			LOGVARN2(64, "green", L"%hs�ѵ����ſ�", account->Name);
			break;
		}

		if (num > 0 && i == num) {
			DbgPrint("%s Say δ�ܵ��︱���ſڻ�û���ǳ�֮��\n", account->Name);
			LOGVARN2(64, "red", L"%hs Say δ�ܵ��︱���ſڻ�û���ǳ�֮��", account->Name);
			break;
		}
	}
}

// ���븱��
_account_* GameProc::OpenFB(bool go_leiming)
{
	_account_* account = GetOpenFBAccount();
	if (!account) {
		DbgPrint("û����������, �޷����븱��\n");
		LOG2(L"û����������, �޷����븱��", "red");

		LOG2(L"----------------------------------------------------", "c3");
		LOG2(L"׼��Next..........", "blue b");

		bool is_big_open1 = IsBigOpenFB();
		account = m_pGame->GetNextLoginAccount();
		if (!account) {
			DbgPrint("û���˺ſ��Ե�¼\n");
			LOG2(L"û���˺ſ��Ե�¼", "red");
			return nullptr;
		}

		m_pGame->AutoLogout();
		Sleep(1680);
		m_pGame->OpenGame(account->Index);
		m_pGame->m_pGameData->WatchGame();
		Sleep(1000);

		this->SetForegroundWindow(m_pGame->m_pBig);
		this->SwitchGameAccount(m_pGame->m_pBig);
		if (!m_pGame->m_pGameData->IsTheMap("������½", m_pGame->m_pBig)) {
			m_pGame->m_pMagic->UseShenZunTianJiang("������½");
			Sleep(1000);
		}
		
		ViteInTeam(m_pGame->m_pBig);
		InTeam(account);
		GoGetXiangLian(account);

		if (!is_big_open1 && IsBigOpenFB()) {
			go_leiming = true;
		}

		return OpenFB(go_leiming);
	}

	this->SetForegroundWindow(account);
	SwitchGameAccount(account);
	m_pGame->m_pButton->HideActivity(account->Wnd.Game);
	if (m_pGame->m_pButton->CloseButton(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "C")) {
		Sleep(500);
	}
	else if (m_pGame->m_pButton->CloseButton(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "x")) {
		Sleep(500);
	}
	else if (m_pGame->m_pButton->CloseButton(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "X")) {
		Sleep(500);
	}
	if (m_pGame->m_pButton->CloseButton(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "�ر�")) {
		Sleep(500);
	}

	if (go_leiming && account->IsBig) {
		m_pGame->m_pMagic->UseShenZunTianJiang("������½");
	}

	if (IsInFB(account)) {
		DbgPrint("%hs�Ѿ��ڸ���.\n", account->Name);
		LOG2(L"�Ѿ��ڸ���", "green b");
		return m_pGame->m_pBig;
	}

	wchar_t log[64];
	DbgPrint("%sȥ������[%s], ������������:%d\n", account->Name, account->IsBig?"���":"С��", account->XL);
	LOGVARP2(log, "c0 b", L"%hsȥ������[%hs], ������������:%d", account->Name, account->IsBig ? "���" : "С��", account->XL);

	GoFBDoor(account);

	_npc_coor_ npcCoor;
	npcCoor.Point[0].MvX = 865;
	npcCoor.Point[0].MvY = 503;
	npcCoor.Point[0].ClkX = 930;
	npcCoor.Point[0].ClkY = 266;
	npcCoor.Point[0].ClkX2 = 950;
	npcCoor.Point[0].ClkY2 = 273;
	npcCoor.Length = 1;

	for (int loop_i = 0; true; loop_i++) {
		while (m_bPause) Sleep(500);

		// ����
		m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_REBORN, "XP2");
		Sleep(800);
		// ԭ�ظ���
		m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_REBORN_AT, "XP1"); 
		if ((loop_i % 5) == 0) {
			Sleep(1000);
			GoFBDoor(account);
			Sleep(1000);
		}

		this->SetForegroundWindow(account);
		CloseTipBox();

		DWORD pos_x, pos_y;
		m_pGame->m_pGameData->ReadCoor(&pos_x, &pos_y, account);
		int click_x, click_y;
		GetNPCClickPos(&npcCoor, pos_x, pos_y, (DWORD&)click_x, (DWORD&)click_y);

		if (m_pGame->m_pButton->CloseButton(account->Wnd.Game, BUTTON_ID_XXX, "X")) {
			Sleep(500);
		}

		if (click_x > 0 && click_x < 1440 && click_y > 0 && click_y < 600) {
			DbgPrint("(%hs)��������Ǳ�������(%d,%d).\n", account->Name, click_x, click_y);
			LOGVARP2(log, "c6", L"(%hs)��������Ǳ�������(%d,%d).", account->Name, click_x, click_y);
			m_pGame->m_pButton->ClickPic(account->Wnd.Game, account->Wnd.Pic, click_x, click_y);
			if (!m_pGame->m_pTalk->WaitTalkOpen(account->Wnd.Pic)) {
				if (IsInFB(account)) {
					DbgPrint("%s�Ѿ����븱��\n", account->Name);
					LOGVARP2(log, "c0 b", L"(%hs)�Ѿ����븱��\n", account->Name);
					break;
				}

				DbgPrint("(%hs)�Ի�û�д�.", account->Name);
				LOGVARP2(log, "red", L"(%hs)�Ի�û�д�.", account->Name);
				if (m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_CANCEL)) {
					Sleep(500);
				}
				if (m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_XXX, "X")) {
					Sleep(500);
				}
				if (m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "X")) {
					Sleep(500);
				}
				else if (m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "x")) {
					Sleep(500);
				}
				continue;
			}

#if 1
			Sleep(1500);
			m_pGame->m_pTalk->Select("��ս���Ǹ���(������)", account->Wnd.Pic, true);
#endif

			Sleep(1500);
			if (IsBigOpenFB()) {
				m_pGame->m_pTalk->Select("Կ�׿������", account->Wnd.Pic, true);
			}
			else {
				m_pGame->m_pTalk->Select("�����������", account->Wnd.Pic, true);
			}

			Sleep(1500); // 800
			m_pGame->m_pTalk->Select("���뿨���Ǳ�", account->Wnd.Pic, true);
			Sleep(1000); // 1000
			if (m_pGame->m_pTalk->NPCTalkStatus(account->Wnd.Pic)) {
				bool again_open = false;
				for (int xb = 0; xb < 3; xb++) {
					m_pGame->m_pTalk->Select("���뿨���Ǳ�", account->Wnd.Pic, true);
					Sleep(1000);
					again_open = false;
					if (m_pGame->m_pTalk->NPCTalkStatus(account->Wnd.Pic)) {
						again_open = true;
					}
				}
				if (again_open) {
					m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "C");
					continue;
				}
			}

			if (m_pGame->m_pTalk->NPCTalkStatus(account->Wnd.Pic)) {
				m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "C");
				Sleep(500);
				continue;
			}
			if (CloseTipBox()) {
				Sleep(500);
				continue;
			}	
		}
		
		for (int i = 0; i < 6; i++) { // 6
			while (m_bPause) Sleep(500);
			if (IsInFB(account)) {
				DbgPrint("%s�Ѿ����븱��\n", account->Name);
				LOGVARP2(log, "c0 b", L"(%hs)�Ѿ����븱��\n", account->Name);

				if (--account->XL < 0)
					account->XL = 0;

				if (m_pGame->m_pButton->CloseButton(account->Wnd.Game, BUTTON_ID_XXX, "X")) {
					Sleep(500);
				}
				return account;
			}

			DbgPrint("%s�ȴ����븱��(%d,%d)\n", account->Name, pos_x, pos_y);
			LOGVARP2(log, "c6", L"(%hs)�ȴ����븱��(%d,%d)...", account->Name, pos_x, pos_y);
			Sleep(1000); // 1000
		}
	}

	return account;
}

// ���г�����big=���Ҫ��ȥ��
void GameProc::AllOutFB(bool big)
{
	for (int i = 0; i < m_pGame->m_AccountList.size(); i++) {
		_account_* p = m_pGame->m_AccountList[i];
		if (!m_pGame->IsOnline(p))
			continue;
		if (p->IsBig) // ��Ų���ȥ
			continue;

		OutFB(p);
	}
	if (big) {
		for (int i = 0; i < m_pGame->m_AccountList.size(); i++) {
			_account_* p = m_pGame->m_AccountList[i];
			if (!m_pGame->IsOnline(p))
				continue;
			if (!p->IsBig) // ��Ų���ȥ
				continue;

			OutFB(p);
		}
	}
}

// ������
void GameProc::OutFB(_account_* account)
{
	wchar_t log[64];
	this->SetForegroundWindow(account);
	DbgPrint("(%s)׼���뿪����.\n", account);
	LOGVARP2(log, "c0", L"(%hs)׼���뿪����.", account->Name);

	if (m_pGame->m_pButton->CloseButton(account->Wnd.Game, BUTTON_ID_XXX, "X")) {
		Sleep(500);
	}
	if (m_pGame->m_pButton->CloseButton(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "�ر�")) {
		Sleep(500);
	}
	if (m_pGame->m_pButton->CloseButton(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "x")) {
		Sleep(500);
	}
	if (m_pGame->m_pButton->CloseButton(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "X")) {
		Sleep(500);
	}

	_npc_coor_ npcCoor;
	npcCoor.Point[0].MvX = 890;
	npcCoor.Point[0].MvY = 1100;
	npcCoor.Point[0].ClkX = 268;
	npcCoor.Point[0].ClkY = 600;
	npcCoor.Point[0].ClkX2 = 278;
	npcCoor.Point[0].ClkY2 = 606;
	npcCoor.Length = 1;
	
	int reborn_num = 0;
	while (true) {
		while (m_bPause) Sleep(500);

		this->SetForegroundWindow(account);
		CloseTipBox();

		if (reborn_num++ < 300) {
			DbgPrint("(%hs)�ȴ���ɫ����...\n", account->Name);
			LOGVARP2(log, "c6", L"(%hs)�ȴ���ɫ����...", account->Name);
			if ((reborn_num & 0x01) == 0x01) {
				m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_REBORN, "XP2");
			}
			else {
				m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_REBORN_AT, "XP1");
			}
			
			Wait(2 * 1000);
		}
		
		if (000 && m_pGame->m_pMove->RunEnd(890, 1100, account) != -1)
			Sleep(600);

		int click_x = MyRand(268, 278), click_y = MyRand(600, 606);

		if (click_x > 0 && click_x < 800 && click_y > 0 && click_y < 900) {
			DbgPrint("(%s)��������Ǳ������ų�ȥ(%d,%d).\n", account->Name, click_x, click_y);
			LOGVARP2(log, "c6", L"(%hs)��������Ǳ������ų�ȥ(%d,%d).", account->Name, click_x, click_y);
			m_pGame->m_pButton->ClickPic(account->Wnd.Game, account->Wnd.Pic, click_x, click_y);
			if (!m_pGame->m_pTalk->WaitTalkOpen(account->Wnd.Pic)) {
				if (!IsInFB(account)) {
					DbgPrint("%s�Ѿ��ڸ�����\n", account->Name);
					LOGVARP2(log, "c0 b", L"(%hs)�Ѿ��ڸ�����\n", account->Name);
					break;
				}

				DbgPrint("(%s)�Ի�û�д�.", account->Name);
				LOGVARP2(log, "red", L"(%hs)�Ի�û�д�.", account->Name);
				continue;
			}


			m_pGame->m_pTalk->Select("�뿪�����Ǳ�[��]", account->Wnd.Pic, true);
			Sleep(800);
			m_pGame->m_pTalk->Select("��Ҫ�뿪[��]", account->Wnd.Pic, true);
			Sleep(1000);
			m_pGame->m_pTalk->Select("����뿪.ȷ��", account->Wnd.Pic, true);
			Sleep(1000);
		}
		
		for (int i = 0; i < 5; i++) {
			while (m_bPause) Sleep(500);
			if (!IsInFB(account)) {
				DbgPrint("%s�Ѿ��ڸ�����\n", account->Name);
				LOGVARP2(log, "c0 b", L"(%hs)�Ѿ��ڸ�����\n", account->Name);
				Sleep(1000);
				if (!m_pGame->m_Setting.AtFBDoor && !account->IsBig) {
					m_pGame->m_pItem->GoShop();
				}
				return;
			}

			DbgPrint("%s�ȴ���ȥ����...\n", account->Name);
			LOGVARP2(log, "c6", L"(%hs)�ȴ���ȥ����...", account->Name);
			Sleep(1000);
		}
	}
}

// ��ȡ���������ʺ�
_account_ * GameProc::GetOpenFBAccount()
{
	return AskXiangLian();
	//return IsBigOpenFB() ? m_pGame->GetBigAccount() : AskXiangLian();
}

// ��ȡʱ��Сʱ
int GameProc::GetHour()
{
	SYSTEMTIME stLocal;
	::GetLocalTime(&stLocal);

	return stLocal.wHour;;
}

// �Ƿ��ɴ�ſ�������
bool GameProc::IsBigOpenFB()
{
	SYSTEMTIME stLocal;
	::GetLocalTime(&stLocal);

	if (stLocal.wHour < 20) // 20�㵽24�������ѽ�
		return false;

	return stLocal.wHour == 23 ? stLocal.wMinute < 58 : true; // ����Ҫ1����ʱ��׼��
}

// ִ��
void GameProc::Exec(_account_* account)
{
}

// ����
void GameProc::Run(_account_* account)
{
	DbgPrint("ˢ�����ʺ�:%s(%08X)\n", account->Name, m_hWndGame);
	LOGVARN2(64, "green b", L"ˢ�����ʺ�:%hs(%08X)\n", account->Name, m_hWndGame);

	//m_pGame->m_pItem->UseYao();
	//m_pGame->m_pItem->DropItem(CIN_YaoBao, 3);
	//Wait(20 * 1000);
	//m_pGame->m_pPet->Revive();
	//SellItem();
	//m_pGame->m_pItem->CheckIn(m_pGame->m_pGameConf->m_stCheckIn.CheckIns, m_pGame->m_pGameConf->m_stCheckIn.Length);
	//m_pGame->m_pItem->CheckOut(m_pGame->m_pGameConf->m_stUse.Uses, m_pGame->m_pGameConf->m_stUse.Length);
	//return;
	char log[64];

start:
	Exec(account);
	while (true) {
		if (IsInFB(account)) {
			SwitchGameAccount(account);
			ExecInFB();
		}
		if (m_pGame->m_bAutoOffline) {
			break;
		}	

		Sleep(2000);
	}
}

// ȥ�������
void GameProc::GoInTeamPos(_account_* account)
{
}

// ��������
void GameProc::CreateTeam(Account* account)
{
	DbgPrint("��������...\n");
	LOG2(L"��������...", "c0");

	m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_TEAM, "���");
	Sleep(1000);
	m_pGame->m_pButton->Click(account->Wnd.Game, 0x43B);
	Sleep(1000);

	DbgPrint("�����������\n");
	LOG2(L"�����������", "c0");
}

// ������� ������� 
void GameProc::ViteInTeam(Account* account)
{
	DbgPrint("׼���������\n");
	LOG2(L"׼���������", "c0");

	this->SetForegroundWindow(account);

	m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_SHEJIAO, "����");
	Sleep(1000);
	HWND h, hp;
	m_pGame->m_pButton->FindButtonWnd(account->Wnd.Game, 0xE2B, h, hp);
	m_pGame->m_pButton->Click(h, 60, 60);

	Sleep(2000);
	m_pGame->m_pButton->FindButtonWnd(account->Wnd.Game, 0x590, h, hp, "C");

	int ys[4] = { 70, 98, 125, 145 };

	for (int i = 0; i < 4; i++) {
		RECT rect;
		::GetWindowRect(hp, &rect);
		::SetCursorPos(rect.left + 60, rect.top + ys[i]); // 60, 98, 125, 145
		Sleep(50);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		Sleep(3000);
		m_pGame->m_pButton->Click(account->Wnd.Game, 0x506, "�Զ����");
		Sleep(1000);
	}
	
	m_pGame->m_pButton->Click(account->Wnd.Game, 0x590, "C");

	DbgPrint("����������\n");
	LOG2(L"����������", "c0");
}

// ���
void GameProc::InTeam(_account_* account)
{
	::SetForegroundWindow(account->Wnd.Game);
	Sleep(1000);

	wchar_t log[64];
	DbgPrint("%s Say ׼��ͬ�����\n", account->Name);
	LOGVARP2(log, "blue b", L"%hs Say ׼��ͬ�����", account->Name);


	for (int i = 1; i <= 5; i++) {
		DbgPrint("%hs Say ��%d��Ѱ��������İ�ť\n", account->Name, i);
		LOGVARP2(log, "blue b", L"%hs Say ��%d��Ѱ��������İ�ť", account->Name, i);
		if (m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_TEAMFLAG))
			break;

		Sleep(1000);
	}

	Sleep(1000);
	m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_INTEAM, "ͬ��");
	Sleep(1500);
	m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_CLOSEMENU);

	DbgPrint("%s Say ���ͬ�����\n", account->Name);
	LOGVARP2(log, "blue b", L"%hs Say ���ͬ�����", account->Name);
}

// ������븱��
void GameProc::ViteInFB(_account_* account)
{
	wchar_t log[64];
	this->SetForegroundWindow(account);
	// 670,545 675,550
	DbgPrint("(%s)������븱��.\n", account->Name);
	DbgPrint("(%s)�ȴ�ȷ�����밴ť...\n", account->Name);
	LOGVARP2(log, "c0", L"(%hs)������븱��.", account->Name);
	LOGVARP2(log, "c6", L"(%hs)�ȴ�ȷ�����밴ť...", account->Name);
	while (true) {
		if (m_pGame->m_pButton->CheckButton(account->Wnd.Game, BUTTON_ID_SURE))
			break;

		Sleep(1000);
	}
	Wait(2 * 1000);

	DbgPrint("(%s)������ѡ��.\n", account->Name);
	LOGVARP2(log, "c0", L"(%hs)������ѡ��.", account->Name);
	m_pGame->m_pButton->ClickPic(account->Wnd.Game, account->Wnd.Pic, MyRand(670, 675), MyRand(545, 550));
	Wait(1 * 1000);
	
	for (int i = 0; i < 5; i++) {
		if (m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_SURE)) {
			DbgPrint("(%hs)������ѽ��븱�����.\n", account->Name);
			LOGVARP2(log, "c0 b", L"(%hs)������ѽ��븱�����.", account->Name);
			return;
		}
	}

	DbgPrint("(%hs)�޷�������ѽ��븱��.\n", account->Name);
	LOGVARP2(log, "reb", L"(%hs)�޷�������ѽ��븱��.", account->Name);
}

void GameProc::InFB(_account_* account)
{
	wchar_t log[64];
	DbgPrint("%s׼��ͬ�����\n", account->Name);
	LOGVARP2(log, "c0 b", L"%hs׼��ͬ�����", account->Name);

	if (m_pGame->m_pButton->CloseButton(account->Wnd.Game, BUTTON_ID_XXX, "X")) {
		Sleep(500);
	}
	if (m_pGame->m_pButton->CloseButton(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "�ر�")) {
		Sleep(500);
	}
	if (m_pGame->m_pButton->CloseButton(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "x")) {
		Sleep(500);
	}
	if (m_pGame->m_pButton->CloseButton(account->Wnd.Game, BUTTON_ID_CLOSEMENU, "X")) {
		Sleep(500);
	}

	// ����
	m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_REBORN, "XP2");
	Sleep(500);
	m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_REBORN, "XP1");
	Sleep(1000);

	// ԭ�ظ���
	m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_REBORN_AT, "XP1");
	Sleep(500);
	m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_REBORN_AT, "XP2");
	Sleep(2000);

	for (int i = 1; i <= 5; i++) {
		if (m_pGame->m_pButton->Click(account->Wnd.Game, BUTTON_ID_INFB, "ͬ��")) {
			DbgPrint("%s���ͬ�������\n", account->Name);
			LOGVARP2(log, "c0 b", L"%hs���ͬ�������", account->Name);
			return;
		}

		DbgPrint("%s�ȴ���������(%d)...\n", account->Name, i);
		LOGVARN2(32, "c6", L"%hs�ȴ���������(%d)...", account->Name, i);
		Sleep(1000);
	}
	
	DbgPrint("%s������ʧ��.\n", account->Name);
	LOGVARP2(log, "red b", L"%hs������ʧ��.", account->Name);

}

// �����˽�����
void GameProc::AllInFB(_account_* account_open)
{
	for (int i = 0; i < m_pGame->m_AccountList.size(); i++) {
		_account_* p = m_pGame->m_AccountList[i];
		if (!m_pGame->IsOnline(p))
			continue;

		if (account_open == p) // �Լ����ĸ���
			continue;

		InFB(p);
	}
}

// ͬ��ϵͳ��Ϣ
void GameProc::AgreenMsg(const char* name, HWND hwnd)
{
	for (int i = 0; i < 3; i++) { // �������濪ʼ��
		if (m_bPause || m_bAtFB)
			return;

		SetForegroundWindow(m_pAccount);
		int result = AgreenMsg(name, i, false, hwnd);
		if (result == 1)
			return;
		if (result == -1)
			break;

		Sleep(1000);
	}
	AgreenMsg(name, 0, true, hwnd); // �Ҳ���ǿ�Ƶ�һ�µ�һ��
}

// ͬ��ϵͳ��Ϣ
int GameProc::AgreenMsg(const char* name, int icon_index, bool click, HWND hwnd)
{
	SetGameCursorPos(325, 62);

	return 1;
}

// �Ƿ��ڸ���
bool GameProc::IsInFB(_account_* account)
{
	if (account && account->Wnd.Map) {
		char text[128] = { 0 };
		::GetWindowTextA(account->Wnd.Map, text, sizeof(text));
		LOGVARN2(32, "red", L"��ͼ:%hs", text);
		if (strstr(text, "������Ŀ���"))
			return true;
	}

	DWORD x = 0, y = 0;
	m_pGame->m_pGameData->ReadCoor(&x, &y, account);
	if (x >= 882 && x <= 930 && y >= 1055 && y < 1115) // �ս���������
		return true;

	return false;
}

// ִ�и�������
void GameProc::ExecInFB()
{
	if (!m_pAccount) {
		DbgPrint("��ѡ��ִ�и����ʺ�\n");
		LOG2(L"��ѡ��ִ�и����ʺ�", "red");
		return;
	}
	m_bAtFB = true;

#if 0
	//m_pGame->m_pMove->Run(MyRand(903, 913), MyRand(1058, 1065), m_pAccount);
	//m_pGame->m_pMagic->UseMagic("����þ�", 890, 1093, 10000);
	//m_pGame->m_pMove->RunEnd(836, 520, m_pAccount, 20 * 1000);
	mouse_event(MOUSEEVENTF_MOVE, -10, 0, 0, 0);
	while (1) Sleep(680);
#endif

	wchar_t log[128];

	DbgPrint("ִ�и�������:��ʼ������ CST:%d\n", m_pGame->m_pDriver->Test());
	LOG2(L"ִ�и�������:��ʼ������.", "green b");

	m_nPlayFBCount_QB = m_pGame->GetfbCount();

	// ������ǰ����
	m_pGame->m_pDriver->SetProtectPid(GetCurrentProcessId());

	InitData();
	m_pGame->SetStatus(m_pGame->m_pBig, ACCSTA_ATFB, true); // ��״̬�ڸ���

	this->SetForegroundWindow(m_pAccount);
	Sleep(100);
	m_pGame->m_pButton->Click(m_pAccount->Wnd.Game, 720, 520);
	m_pGame->m_pButton->ClickPic(m_pAccount->Wnd.Game, m_pAccount->Wnd.Pic, 720, 450);
	Sleep(500);
	// 1886,320 465,62
	// 1420,260
	if (m_pGame->m_pButton->HideActivity(m_pAccount->Wnd.Game)) // ���ػ�б�
		Sleep(500);

#if 0
	// �����Ҳ�������ʾ
	m_pGame->m_pButton->ClickPic(m_pAccount->Wnd.Game, m_pAccount->Wnd.Pic, MyRand(1420, 1425), MyRand(260, 262));
	Sleep(500);
#endif

	// �����������
	m_pGame->m_pPet->PetOut(m_pGame->m_pGameConf->m_stPetOut.No, m_pGame->m_pGameConf->m_stPetOut.Length);
	m_pGame->m_pPet->PetFuck(-1);

	char* step_file = m_pGameStep->SelectRandStep();
	wchar_t file[128];
	wsprintfW(file, L"�ļ�:%hs", step_file);
	m_pGame->UpdateText("step_file", file);
		
	//m_pGameStep->ResetStep();
	//SendMsg("��ʼˢ����");
	int start_time = time(nullptr);
	m_nStartFBTime = start_time;
	m_nUpdateFBTimeLongTime = start_time;

	//m_pGame->m_pItem->DropItem(CIN_YaoBao);
	//while (168) Sleep(600);

	//while (true) Sleep(168);
	DbgPrint("ִ�и�������:��ʼ.\n");
	LOG2(L"ִ�и�������:��ʼ.", "green b");
	m_pGameStep->ResetStep(m_pGameStep->m_iStepInitIndex, 0x01);
	while (1 && ExecStep(m_pGameStep->m_Step, true)); // ���ˢ����
	m_bLockGoFB = false;

	if (m_pGame->m_bAutoOffline) {
		return;
	}

	//while (true) Sleep(900);

	int end_time = time(nullptr);
	int second = end_time - start_time;
	DbgPrint("��ͨ�أ�����ʱ:%02d��%02d��\n", second / 60, second % 60);
	LOGVARP2(log, "green b", L"��ͨ�أ�����ʱ:%02d��%02d��\n", second / 60, second % 60);

	m_pGame->SetStatus(m_pGame->m_pBig, ACCSTA_ONLINE, true);

	m_pGameStep->ResetStep();
	int game_flag = 0;

	if (game_flag == 0) { // ��Ϸ��������
		if (m_nReOpenFB == 0) {
			m_pGame->UpdateFBCountText(++m_nPlayFBCount, true);
		}
		if (m_nReOpenFB == 1) { // ��ȥ���¿�ʼ
			AllOutFB(true);
		}
	}
	else { // ��Ϸ���˳�����¼����
	}
		
	m_pGame->UpdateFBTimeLongText(end_time - m_nStartFBTime + m_nFBTimeLong, end_time - m_nUpdateFBTimeLongTime); // ����ʱ��
	m_pGame->InsertFBRecord(m_nStartFBTime, end_time, m_nReOpenFB);
	m_nUpdateFBTimeLongTime = end_time;
	m_nFBTimeLong = end_time - m_nStartFBTime + m_nFBTimeLong;

	m_nReOpenFB = 0; // ���ò��ؿ�����
	//GoFBDoor();
	Sleep(1000);
	while (!m_pGame->m_pGameData->IsInFBDoor(m_pGame->GetBigAccount())) { // �ȴ���������
		Sleep(1000);
	}

	m_bAtFB = false;

	int wait_s = 60;
	int out_time = time(nullptr);

	// ������
	SellItem();

	//if (!GetOpenFBAccount()) // û���ʺ���
	//	return;
	while (true) {
		int c = (time(nullptr) - out_time);
		if (c > wait_s)
			break;

		DbgPrint("�ȴ�%d���ٴο�������, ��ʣ%d��\n", wait_s, wait_s - c);
		LOGVARP2(log, "c6", L"�ȴ�%d���ٴο�������, ��ʣ%d��", wait_s, wait_s - c);
		Sleep(1000);
	}

	Account* open = OpenFB();
	if (open) {
		ViteInFB(open);
		AllInFB(open);
	}
	else {
		DbgPrint("û�п��Կ����������ʺ�.\n");
		LOG2(L"û�п��Կ����������ʺ�.", "red");
	}
	
	//Wait((wait_s - (time(nullptr) - out_time)) * 1000);
}

// ִ������
bool GameProc::ExecStep(Link<_step_*>& link, bool isfb)
{
	if (m_pGame->m_bAutoOffline) {
		return false;
	}

	m_pStep = m_pGameStep->Current(link);
	if (!m_pStep) {
		DbgPrint("����ȫ��ִ�����.\n");
		LOG2(L"����ȫ��ִ�����.", "green");
		return false;
	}

	wchar_t log[128];
	_step_* m_pTmpStep = m_pStep; // ��ʱ��

	m_pGame->m_pMove->SubOneMaxMovCoor(0); // ����ÿ���ƶ�����

	int now_time = time(nullptr);
	//printf("CMD:%s\n", m_pStep->Cmd);
	//while (true) Sleep(168);

	if (m_pStep->OpCode == OP_MOVE) {
		_step_* next = m_pGameStep->GetNext(link);
		if (next && next->OpCode == OP_PICKUP && strcmp(next->Name, "��Ч���ư�") == 0) {
			int yaobao = 0, yao = 0;
			m_pGame->m_pItem->GetQuickYaoOrBaoNum(yaobao, yao);
			if (yaobao >= 6) { // �����ټ�ҩ��
				LOGVARN2(32, "blue_r b", L"�����ټ�ҩ��, ����:%d\n", yaobao);
				return m_pGameStep->CompleteExec(link) != nullptr;
			}
		}
	}

	if (m_pStep->OpCode != OP_NPC && m_pStep->OpCode != OP_SELECT) {
		m_pGame->m_pPet->Revive();
		if ((now_time & 0xff) == 0x09) {
			CheckDllSign();
		}
	}

	m_pStep->ExecTime = now_time;
	if (isfb) {
		m_pGame->UpdateFBTimeLongText(now_time - m_nStartFBTime + m_nFBTimeLong, now_time - m_nUpdateFBTimeLongTime); // ����ʱ��
		m_nUpdateFBTimeLongTime = now_time;

		m_nFBTimeLongOne += now_time - m_nStartFBTime;

		if (m_pStepCopy) { // �Ѿ�ִ�е��Ĳ���
			if (m_pStep == m_pStepCopy) {
				m_pStepCopy = nullptr;
			}
			else {
				if (m_pStep->OpCode != OP_MOVE && m_pStep->OpCode != OP_MAGIC) { // �����ƶ���ż���
					return m_pGameStep->CompleteExec(link) != nullptr;
				}
			}
		}
	}

	if ((now_time & 0xff) == 0x00) {
		ChNCk();
	}

	CloseTipBox();
	CloseVite();

	bool bk = false;
	char msg[128];
	switch (m_pStep->OpCode)
	{
	case OP_MOVE:
	case OP_MOVEPICKUP:
		DbgPrint("����->�ƶ�:%d.%d��%d.%d\n", m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2);
		LOGVARP2(log, "c0 b", L"����->�ƶ�:%d.%d��%d.%d ���:%d", m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2, m_pStep->OpCount);
		if (m_pStep->OpCode == OP_MOVEPICKUP) {
			m_pGame->m_pItem->GetQuickYaoOrBaoNum(m_nYaoBao, m_nYao);
			if (m_nYaoBao >= 6) {
				LOG2(L"�����ټ�ҩ��.", "blue_r b");
				bk = true;
				break;
			}
		}

		if (m_pStep->X2 && m_pStep->Y2) {
			DWORD pos_x, pos_y;
			m_pGame->m_pGameData->ReadCoor(&pos_x, &pos_y, m_pAccount); // ��ȡ��ǰ����
			DWORD min_x = min(m_pStep->X, m_pStep->X2);
			DWORD max_x = max(m_pStep->X, m_pStep->X2);
			DWORD min_y = min(m_pStep->Y, m_pStep->Y2);
			DWORD max_y = max(m_pStep->Y, m_pStep->Y2);

			bool x_in_pos = pos_x >= min_x && pos_x <= max_x; // λ��x��ָ����Χ��
			bool y_in_pos = pos_y >= min_y && pos_y <= max_y; // λ��y��ָ����Χ��
			if (x_in_pos && y_in_pos) {
				LOG2(L"���ڴ�λ��, �����ƶ���.", "c0");
				bk = true;
				break;
			}

			m_pStep->Extra[0] = x_in_pos ? pos_x : MyRand(m_pStep->X, m_pStep->X2, m_nFBTimeLongOne);
			m_pStep->Extra[1] = y_in_pos ? pos_y : MyRand(m_pStep->Y, m_pStep->Y2, m_nFBTimeLongOne);

			DbgPrint("ʵ���ƶ�λ��:%d,%d\n", m_pStep->Extra[0], m_pStep->Extra[1]);
			LOGVARP2(log, "c0", L"ʵ���ƶ�λ��:%d,%d", m_pStep->Extra[0], m_pStep->Extra[1]);
		}
		else {
			m_pStep->Extra[0] = m_pStep->X;
			m_pStep->Extra[1] = m_pStep->Y;
		}
		Move();
		break;
	case OP_MOVEFAR:
		DbgPrint("����->����:%d.%d\n", m_pStep->X, m_pStep->Y);
		LOGVARP2(log, "c0 b", L"����->����:%d.%d", m_pStep->X, m_pStep->Y);
		m_pStep->Extra[0] = m_pStep->X;
		m_pStep->Extra[1] = m_pStep->Y;
		Move();
		break;
	case OP_MOVERAND:
		DbgPrint("����->����ƶ�:%d.%d��%d.%d\n", m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2);
		LOGVARP2(log, "c0 b", L"����->����ƶ�:%d.%d��%d.%d", m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2);
		if (MyRand(1, 2, MyRand(m_pStep->X, m_pStep->Y)) == 1) {
			if (m_pStep->X2 && m_pStep->Y2) {
				m_pStep->Extra[0] = MyRand(m_pStep->X, m_pStep->X2, m_nFBTimeLongOne);
				m_pStep->Extra[1] = MyRand(m_pStep->Y, m_pStep->Y2, m_nFBTimeLongOne);

				DbgPrint("ʵ���ƶ�λ��:%d,%d\n", m_pStep->Extra[0], m_pStep->Extra[1]);
				LOGVARP2(log, "c0", L"ʵ���ƶ�λ��:%d,%d", m_pStep->Extra[0], m_pStep->Extra[1]);
				Move();
			}
		}
		else {
			DbgPrint("����ƶ�����\n\n");
			LOG2(L"����ƶ�����\n", "c0 b");
			bk = true;
		}
		break;
	case OP_MOVEONE:
		LOGVARP2(log, "c0 b", L"����->�ƶ�:%d.%d��%d.%d", m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2);
		if (m_pStep->X2 && m_pStep->Y2) {
			m_pStep->Extra[0] = MyRand(m_pStep->X, m_pStep->X2, m_nFBTimeLongOne);
			m_pStep->Extra[1] = MyRand(m_pStep->Y, m_pStep->Y2, m_nFBTimeLongOne);

			DbgPrint("ʵ���ƶ�λ��:%d,%d\n", m_pStep->Extra[0], m_pStep->Extra[1]);
			LOGVARP2(log, "c0", L"ʵ���ƶ�λ��:%d,%d", m_pStep->Extra[0], m_pStep->Extra[1]);
		}
		else {
			m_pStep->Extra[0] = m_pStep->X;
			m_pStep->Extra[1] = m_pStep->Y;
		}
		MoveOne();
		break;
	case OP_MOVECLICK:
		DbgPrint("����->�ƶ�:%d.%d ���:%d.%d\n", m_pStep->X, m_pStep->Y, m_pStep->Clx, m_pStep->Cly);
		LOGVARP2(log, "c0 b", L"����->�ƶ�:%d.%d ���:%d.%d", m_pStep->X, m_pStep->Y, m_pStep->Clx, m_pStep->Cly);
		MoveClick();
		break;
	case OP_MOVENPC:
		if (m_pStep->p_npc) {
			DbgPrint("����->����NPC:%hs\n", m_pStep->p_npc->Name);
			LOGVARP2(log, "c0 b", L"����->����NPC:%hs", m_pStep->p_npc->Name);
			if (MoveNPC()) {
				m_pStepLastMove = m_pStep;
				DbgPrint("�������ƶ�\n");
				LOG2(L"�������ƶ�", "c0 b");
				bk = true;
			}
		}
		break;
	case OP_NPC:
		DbgPrint("����->NPC:%s(���:%d,%d �� %d,%d)\n", m_pStep->NPCName, m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2);
		LOGVARP2(log, "c0 b", L"����->NPC:%hs(���:%d,%d �� %d,%d)", m_pStep->NPCName, m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2);
		m_stLast.NPCOpCode = OP_NPC;
		NPC();
		break;
	case OP_SELECT:
		if (m_pStep->SelectNo != 0xff) {
			DbgPrint("����->ѡ��:%d\n", m_pStep->SelectNo);
			LOGVARP2(log, "c0 b", L"����->ѡ��:%d", m_pStep->SelectNo);
		}
		else {
			DbgPrint("����->ѡ��:%s\n", m_pStep->Name);
			LOGVARP2(log, "c0 b", L"����->ѡ��:%hs", m_pStep->Name);
		}
		Select();
		break;
	case OP_MAGIC:
		DbgPrint("����->����.%s(%d,%d) ʹ�ô���:%d\n", m_pStep->Magic, m_pStep->X, m_pStep->Y, m_pStep->OpCount);
		LOGVARP2(log, "c0 b", L"����->����.%hs(%d,%d) ʹ�ô���:%d", m_pStep->Magic, m_pStep->X, m_pStep->Y, m_pStep->OpCount);
		Magic();
		break;
	case OP_MAGIC_PET:
		DbgPrint("����->���＼��:%s\n", m_pStep->Magic);
		LOGVARP2(log, "c0 b", L"����->���＼��:%hs", m_pStep->Magic);
		//MagicPet();
		break;
	case OP_KAWEI:
		DbgPrint("��λ->ʹ�ü���:%s �ȴ�:%d��\n", m_pStep->Magic, m_pStep->WaitMs / 1000);
		LOGVARP2(log, "c0 b", L"��λ->ʹ�ü���:%hs �ȴ�:%d��", m_pStep->Magic, m_pStep->WaitMs / 1000);
		KaWei();
		break;
	case OP_CRAZY:
		SMSG_D("����->���");
		//Crazy();
		break;
	case OP_CLEAR:
		SMSG_D("����->����");
		//Clear();
		break;
	case OP_KAIRUI:
		DbgPrint("����->���� �ƶ�:(%d,%d) ����:(%d,%d)\n", m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2);
		LOGVARP2(log, "c0 b", L"����->���� �ƶ�:(%d,%d) ����:(%d,%d)", m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2);

		if (!m_bClearKaiRui) { // ��û�иɵ���
			m_pStep->Extra[0] = m_pStep->X;
			m_pStep->Extra[1] = m_pStep->Y;
			Move();
		}
		else {
			DbgPrint("�����ѱ��ɵ�, ����.\n");
			LOG2(L"�����ѱ��ɵ�, ����.", "green");
			bk = true;
		}
		break;
	case OP_PICKUP:
		DbgPrint("����->��ʰ��Ʒ:%s, ����:(%d,%d)-(%d,%d)\n", m_pStep->Name, m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2);
		LOGVARP2(log, "c0 b", L"����->��ʰ��Ʒ:%hs (%d,%d)-(%d,%d)", m_pStep->Name, m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2);
		PickUp();
		break;
	case OP_CHECKIN:
		DbgPrint("����->������Ʒ\n");
		LOG2(L"����->������Ʒ", "c0 b");
		CheckIn();
		break;
	case OP_USEITEM:
		DbgPrint("����->ʹ����Ʒ\n");
		LOG2(L"����->ʹ����Ʒ", "c0 b");
		//UseItem();
		break;
	case OP_DROPITEM:
		DbgPrint("����->������Ʒ\n");
		LOG2(L"����->������Ʒ", "c0 b");
		DropItem();
		break;
	case OP_SELL:
		DbgPrint("����->������Ʒ\n");
		LOG2(L"����->������Ʒ", "c0 b");
		//SellItem();
		break;
	case OP_BUTTON:
		DbgPrint("����->ȷ��\n");
		LOG2(L"����->ȷ��", "c0 b");
		Button();
		break;
	case OP_CLICK:
		m_stLast.NPCOpCode = OP_CLICK;
		m_stLast.ClickX = m_pStep->X;
		m_stLast.ClickY = m_pStep->Y;
		m_stLast.ClickX2 = 0;
		m_stLast.ClickY2 = 0;
		DbgPrint("����->���:%d,%d %d,%d\n", m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2);
		LOGVARP2(log, "c0 b", L"����->���:%d,%d %d,%d", m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2);
		if (m_pStep->X2 && m_pStep->Y2) {
			m_stLast.ClickX2 = m_pStep->X2;
			m_stLast.ClickY2 = m_pStep->Y2;
			Click(m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2);
		}
		else {
			Click(m_pStep->X, m_pStep->Y);
		}
		break;
	case OP_CLICKRAND:
		DbgPrint("����->������:%d,%d %d,%d ����:%d\n", m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2, m_pStep->OpCount);
		LOGVARP2(log, "c0 b", L"����->������:%d,%d %d,%d ����:%d", m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2, m_pStep->OpCount);
		ClickRand();
		break;
	case OP_CLICKCRAZ:
		DbgPrint("����->�������:%d,%d %d,%d ����:%d\n", m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2, m_pStep->OpCount);
		LOGVARP2(log, "c0 b", L"����->�������:%d,%d %d,%d ����:%d", m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2, m_pStep->OpCount);
		m_ClickCrazy.X = m_pStep->X;
		m_ClickCrazy.Y = m_pStep->Y;
		m_ClickCrazy.X2 = m_pStep->X2;
		m_ClickCrazy.Y2 = m_pStep->Y2;
		m_ClickCrazy.Count = m_pStep->OpCount;
		bk = true;
		break;
	case OP_WAIT:
		DbgPrint("����->�ȴ�:%d %d\n", m_pStep->WaitMs / 1000, m_pStep->Extra[0]);
		LOGVARP2(log, "c0 b", L"����->�ȴ�:%d %d", m_pStep->WaitMs / 1000, m_pStep->Extra[0]);
		Wait();
		break;
	case OP_SMALL:
		if (m_pStep->SmallV == -1) {
			DbgPrint("����->С��:δ֪����\n");
			LOG2(L"����->С��:δ֪����", "red b");
		}
		else {
			DbgPrint("����->С��:%d %s\n", m_pStep->SmallV, m_pStep->SmallV == 0 ? "������" : "������");
			LOGVARP2(log, "c0 b", L"����->С��:%d %hs", m_pStep->SmallV, m_pStep->SmallV == 0 ? "������" : "������");
			Small();
		}
		break;
	case OP_RECORD:
		DbgPrint("\n--------׼����¼��һ����--------\n\n");
		LOG2(L"\n--------׼����¼��һ����--------\n", "blue_r b");
		m_bIsRecordStep = true;
		bk = true;
		break;
	default:
		SMSG_D("����->δ֪");
		LOG2(L"����->δ֪", "red b");
		break;
	}

	SMSG_D("����->ִ�����");
	if (bk) {
		if (!m_bIsRecordStep) {
			DbgPrint("����->����ɴ˲���\n------------------------------------------------------\n");
			LOG2(L"����->����ɴ˲���\n", "c0 b");
		}
			
		return m_pGameStep->CompleteExec(link) != nullptr;
	}
	if (m_bIsResetRecordStep) {
		DbgPrint("\n--------���õ��Ѽ�¼����------\n------------------------------------------------------\n");
		LOG2(L"\n--------���õ��Ѽ�¼����--------\n", "blue_r b");
		m_pGameStep->ResetStep(m_pStepRecord->Index, 0x01);
		m_bIsResetRecordStep = false;
		return true;
	}

	if ((now_time & 0xff) == 0x02) {
		m_pGame->ChCRC();
	}

	m_nYaoBao = 0;
	m_nYao = 0;

	
	bool no_mov_screen = false; // ��ס�Ƿ��ѽ�ͼ��
	int  try_count = 0;         // �����ס�������
	int  use_second = 0;        // ������ʱ��
	int  pause_second = 0;      // ��ͣ��ʱ��
	int  pause_time = 0;        // ��ͣ����ʱ��
	int  mov_i = 0;
	int  drop_i = 0;
	int  move_far_i = 0;
	do {
		while (m_bNoVerify); // û��ͨ����֤

		while (true) {
			if (m_bStop || m_bReStart)
				return false;

			if (m_bPause) {
				if (pause_time == 0) {
					pause_time = time(nullptr);
				}
				Sleep(500);
			}
			else {
				if (pause_time > 0) {
					pause_second = time(nullptr) - pause_time;
				}
				break;
			}
		}

		if (0 && isfb && m_pGame->m_Setting.FBTimeOutErvry > 0) { // �����������ʱ��
			int use_second = time(nullptr) - m_nStartFBTime;
			if (use_second >= m_pGame->m_Setting.FBTimeOutErvry) {
				DbgPrint("\n����ʱ��ﵽ%d��, ����%d��, �ؿ�����\n\n", use_second, m_pGame->m_Setting.FBTimeOutErvry);
				LOGVARP2(log, "red b", L"\n����ʱ���Ѵﵽ�������ʱ��(%d)��, ����(%d)��, ���¿�������\n", use_second, m_pGame->m_Setting.FBTimeOutErvry);
				m_nReOpenFB = 2;
				return false;
			}
		}

		Sleep(10);

		//DbgPrint("use_yao_bao\n");
		bool use_yao_bao = false;
		if (m_pStep->OpCode == OP_MOVE || m_pStep->OpCode == OP_MOVERAND || m_pStep->OpCode == OP_MOVENPC
			|| m_pStep->OpCode == OP_MOVEFAR || m_pStep->OpCode == OP_KAIRUI) {
			mov_i++;
			//DbgPrint("m_pStep->OpCount:%d\n", m_pStep->OpCount);
			if (m_pStep->OpCount <= 0 && (mov_i % 10) == 0) {
				//DbgPrint("m_pStep->OpCount222:%d\n", m_pStep->OpCount);
				Move(true);
			}

			if (m_bIsFirstMove && mov_i == 50) {
				// �����Ҳ�������ʾ
				m_pGame->m_pButton->ClickPic(m_pAccount->Wnd.Game, m_pAccount->Wnd.Pic, MyRand(1423, 1425), MyRand(260, 262), 150);
				Sleep(500);

				// �����������
				m_pGame->m_pPet->PetOut(m_pGame->m_pGameConf->m_stPetOut.No, m_pGame->m_pGameConf->m_stPetOut.Length);
				m_pGame->m_pPet->PetFuck(-1);
			}

			if (mov_i == 80 && mov_i <= 100) {
				CloseTipBox(); // �رյ�����
			}
			if (mov_i > 0 && (mov_i % 40) == 0) {
				m_pGame->m_pItem->GetQuickYaoOrBaoNum(m_nYaoBao, m_nYao);
			}
			if (mov_i > 60 && (mov_i % 40) == 0) {
				// ����·����ҩ
				if (m_nYaoBao > m_nLiveYaoBao) { // ҩ������6
					if (m_nYao <= 3) { // ҩС��2
						m_pGame->m_pItem->UseYaoBao(); // ��ҩ��
						m_nYaoBao--;
						m_nYao += 6;
					}
					else {
						m_pGame->m_pItem->UseYao(1, false, 300); // ��ҩ
						m_nYao--;
					}

					use_yao_bao = true;
				}
				else {
					if (m_nYao > m_nLiveYao) { // ҩ����6
						m_pGame->m_pItem->UseYao(1, false, 300);
						m_nYao--;

						use_yao_bao = true;
					}
				}
			}

			// ���������
			if (m_ClickCrazy.Count) {
				ClickCrazy();
			}
		}

		if (m_pStep->OpCode != OP_DROPITEM) {
			SMSG_D("�жϼ�Ѫ");
			int life_flag = IsNeedAddLife(12600);
			if (!m_bReborn && life_flag == -1) { // ������ֻ��Ҫ��Ѫ
				life_flag = 1;
			}

			if (life_flag == 1) { // ��Ҫ��Ѫ
				SMSG_D("��Ѫ");
				m_pGame->m_pItem->UseYao();
				SMSG_D("��Ѫ->���");
			}
			else if (life_flag == -1) { // ��Ҫ����
				m_pGame->SaveScreen("��Ѫ");

				m_pGame->m_pGameData->ReadCoor(&m_pAccount->LastX, &m_pAccount->LastY, m_pAccount);
				if (!m_pAccount->LastX || !m_pAccount->LastY) {
					DbgPrint("\n~~~~~~~~~~~~~~~~~~��Ϸ�ѵ��߻����쳣����~~~~~~~~~~~~~~~~~~\n\n");
					LOG2(L"\n~~~~~~~~~~~~~~~~~~��Ϸ�ѵ��߻����쳣����~~~~~~~~~~~~~~~~~~\n", "red");
					return false;
				}

				ReBorn(); // ����
				m_pStepCopy = m_pStep; // ���浱ǰִ�и���
				m_pGameStep->ResetStep(0); // ���õ���һ��
				DbgPrint("���õ���һ��\n");
				LOG2(L"���õ���һ��", "red");
				if (m_nBossNum > 0) {
					LOG2(L"���¿�������", "red");
					m_nReOpenFB = 1;
				}
				return m_nBossNum <= 0;
			}
			SMSG_D("�жϼ�Ѫ->���");
		}

		//DbgPrint("�ж�����\n");
		bool complete = StepIsComplete();
		//DbgPrint("�ж�����->���\n");
		if (m_pStep->OpCode == OP_MOVEFAR) {
			if (++move_far_i == 300) {
				DbgPrint("���ͳ�ʱ\n");
				LOG2(L"���ͳ�ʱ", "red");
				complete = true;
			}
		}

		if ((m_pGame->m_nHideFlag & 0x000000ff) != 0x000000CB) { // ��������������0x168999CB
			while (true);
		}

		//DbgPrint("use_yao_bao end.\n");

		if (complete) { // ����ɴ˲���
			DbgPrint("����->����ɴ˲��� ����:%d/%d\n------------------------------------------------------\n", m_nPlayFBCount, m_nPlayFBCount_QB);
			LOG2(L"����->����ɴ˲���\n", "c0 b");
			if (m_bIsRecordStep) { // ��¼�˲���
				m_pStepRecord = m_pStep;
				m_bIsRecordStep = false;
				DbgPrint("--------�Ѽ�¼�˲���--------\n------------------------------------------------------\n");
				LOG2(L"--------�Ѽ�¼�˲���--------\n", "blue_r b");
			}

			m_stLast.OpCode = m_pStep->OpCode;
			m_pStepLast = m_pStep;
			
			if (m_pStep->OpCode != OP_NPC && m_pStep->OpCode != OP_SELECT && m_pStep->OpCode != OP_WAIT) {
				//if (m_pGame->m_pTalk->NPCTalkStatus()) // �Ի����Ǵ򿪵�
				//	m_pGame->m_pTalk->NPCTalk(0xff);   // �ص���
			}

			_step_* next = m_pGameStep->CompleteExec(link);
			if (m_pStep->OpCode != OP_WAIT && next) {
				Sleep(use_yao_bao ? 100 : 5);
			}

			if ((now_time & 0xff) == 0x06) {
				if (!m_pGame->m_pDriver->Test()) {
					m_nNoConnectDriver++;
					//::MessageBoxA(NULL, "Ӧ�ò�ͨ����", "����", MB_OK);
				}
			}

			return next != nullptr;
		}
		else {
			use_second = time(nullptr) - m_pStep->ExecTime - pause_second; // ����ִ�в�����ʱ, ��ȥ��ͣ��ʱ
			if (use_second >= 60) {
				DbgPrint("--------�����ѿ�ס, �����ﵽ60��.--------\n");
				LOG2(L"--------�����ѿ�ס, �����ﵽ60��.--------", "red");
				if (!no_mov_screen) {
					Sleep(800);
					m_pGame->SaveScreen("��ס");
					no_mov_screen = true;
				}

				if (0 && m_pGame->m_Setting.FBTimeOut > 0) {
					int use_second = time(nullptr) - m_nStartFBTime;
					if (use_second >= m_pGame->m_Setting.FBTimeOut) {
						DbgPrint("\n����ʱ��ﵽ%d��, ����%d��, �ؿ�����\n\n", use_second, m_pGame->m_Setting.FBTimeOut);
						LOGVARP2(log, "red b", L"\n����ʱ��ﵽ%d��, ����%d��, �ؿ�����\n", use_second, m_pGame->m_Setting.FBTimeOut);
						m_nReOpenFB = 2;
						return false;
					}
				}

				if (try_count++ < 2) {
					DbgPrint("--------�����ٴζԻ�NPC--------\n");
					LOG2(L"--------�����ٴζԻ�NPC--------", "blue");
					Sleep(800);
					NPCLast(1);
					m_pGame->m_pTalk->WaitTalkOpen(m_pAccount->Wnd.Game);
					Sleep(1000);
					m_pGame->m_pTalk->Select(m_chLastSelectName, m_pAccount->Wnd.Game);
					Sleep(500);
					DbgPrint("--------���ԶԻ�NPC��ɣ���Ҫ�ƶ���:(%d,%d)-------\n", m_pStep->X, m_pStep->Y);
					LOGVARP2(log, "blue", L"--------���ԶԻ�NPC��ɣ���Ҫ�ƶ���:(%d,%d)-------", m_pStep->X, m_pStep->Y);
					m_nReMoveCount++;
				}
				else {
					if (use_second > 90) {
						DbgPrint("--------���õ��Ѽ�¼����------\n");
						LOG2(L"\n--------���õ��Ѽ�¼����--------\n", "blue_r b");
						m_pGameStep->ResetStep(m_pStepRecord->Index, 0x01);
						m_nReMoveCount = 0;
						m_nReMoveCountLast = 0;
						return true;
					}
					else {
						DbgPrint("--------%d������õ���¼����--------\n", 90 - use_second);
						LOGVARP2(log, "red", L"--------%d������õ���¼����--------", 80 - use_second);
					}
				}

				m_nReMoveCountLast = m_nReMoveCount;
			}
		}
	} while (true);

	return false;
}

// �����Ƿ���ִ�����
bool GameProc::StepIsComplete()
{
	int move_flag = 0;
	bool result = false;
	switch (m_pStep->OpCode)
	{
	case OP_MOVE:
	case OP_MOVERAND:
	case OP_MOVENPC:
	case OP_KAIRUI:
		if (m_pGame->m_pMove->IsMoveEnd(m_pAccount)) { // �ѵ�ָ��λ��
			m_pStepLastMove = m_pStep;
			m_stLast.MvX = m_pStep->Extra[0];
			m_stLast.MvY = m_pStep->Extra[1];
			m_nReMoveCount = 0;
			m_nReMoveCountLast = 0;
			result = true;

			if (m_bIsFirstMove) {
				// �л������ܿ����
				//m_pGame->m_pItem->SwitchMagicQuickBar();
				m_bIsFirstMove = false;
			}
			if (m_pStep->OpCode == OP_KAIRUI) {
				KaiRui(); // �㿭��
			}
			goto end;
		}

		move_flag = m_pGame->m_pMove->IsMove(m_pAccount);
		if (move_flag == 0) {   // δ�ƶ�
			CloseTipBox();
			CloseVite();

			if (m_pGame->m_pTalk->NPCTalkStatus(m_pAccount->Wnd.Game)) {
				m_pGame->m_pMove->SubOneMaxMovCoor(1); // �㵽��NPC���ƶ�λ�ü�һ��
				DbgPrint("�㵽��NPC���ƶ�λ�ü�һ��\n");
			}

			m_pGame->m_pButton->Click(m_pAccount->Wnd.Game, BUTTON_ID_CLOSEMENU);

			Move(false);
			
			m_nReMoveCountLast = m_nReMoveCount;
			m_nReMoveCount++;
		}
		if (move_flag == 1) { // �ƶ���
			m_pGame->m_pMove->SubOneMaxMovCoor(0); // �ָ�֮ǰ��

			int tmp = m_nReMoveCount;
			m_nReMoveCountLast = m_nReMoveCount;
			if (m_nReMoveCount > 0)
				m_nReMoveCount--;
		}
		break;
	case OP_MOVEONE:
		MoveOne();
		break;
	case OP_MOVECLICK:
		DWORD pos_x, pos_y;
		m_pGame->m_pGameData->ReadCoor(&pos_x, &pos_y, m_pAccount); // ��ȡ��ǰ����
		if (!m_pStep->X2 || !m_pStep->Y2) {
			if (m_pStep->X == pos_x && m_pStep->Y == pos_y)
				result = true;
		}
		else {
			DWORD min_x = min(m_pStep->X, m_pStep->X2);
			DWORD max_x = max(m_pStep->X, m_pStep->X2);
			DWORD min_y = min(m_pStep->Y, m_pStep->Y2);
			DWORD max_y = max(m_pStep->Y, m_pStep->Y2);

			if ((pos_x >= min_x && pos_x <= max_x) && (pos_y >= min_y && pos_y <= max_y)) {
				LOGVARN2(64, "c0", L"(%d>=%d&&%d<=%d) && (%d>=%d&&%d<=%d)", 
					pos_x, min_x, pos_x, max_x, pos_y, min_y, pos_y, max_y);
				result = true;
			}	
		}

		if (result) {
			LOGVARN2(64, "c0", L"��ǰ����:%d,%d", pos_x, pos_y);
			if (m_pStep->Extra[0] == 0x01) {
				m_pGame->m_pButton->Click(m_pAccount->Wnd.Pic, SCREEN_X/2, SCREEN_Y/2);
			}
			goto end;
		}
		else {
			if (m_pStep->Extra[0] & 0x01) {
				MoveClick(true);
			}
		}
		break;
	case OP_MOVEFAR:
		if (m_pGame->m_pGameData->IsNotInArea(m_pStep->X, m_pStep->Y, 50, m_pAccount)) { // �Ѳ��ڴ�����
			result = true;
			m_nReMoveCount = 0;
			m_nReMoveCountLast = 0;
			goto end;
		}
		if (m_pGame->m_pMove->IsMoveEnd(m_pAccount)) { // �ѵ�ָ��λ��
			Move(false);
			m_nReMoveCount = 0;
			m_nReMoveCountLast = 0;
		}
		if (!m_pGame->m_pMove->IsMove(m_pAccount)) {   // �Ѿ�ֹͣ�ƶ�
			Move(false);
			m_nReMoveCountLast = m_nReMoveCount;
			m_nReMoveCount++;
		}
		break;
	default:
		result = true;
		break;
	}
end:

	return result;
}

// �ƶ�
void GameProc::Move(bool check_time)
{
	m_pAccount->MvFlag = m_pStep->OpCount;
	m_pGame->m_pMove->Run(m_pStep->Extra[0], m_pStep->Extra[1], m_pAccount, check_time);
}

// �ƶ�
void GameProc::MoveOne()
{
	m_pAccount->MvFlag = m_pStep->OpCount;
	int x, y;
	m_pGame->m_pMove->MakeClickCoor(x, y, m_pStep->Extra[0], m_pStep->Extra[1], m_pGame->m_pGameProc->m_pAccount);
	m_pGame->m_pMove->RunClick(m_pStep->Extra[0], m_pStep->Extra[1], x, y, m_pAccount, false);
}

// �Ƶ�
void GameProc::MoveClick(bool check_time)
{
	m_pAccount->MvFlag = m_pStep->OpCount;

	DWORD click_x = m_pStep->Clx, click_y = m_pStep->Cly;
	if (m_pStep->Clx2)
		click_x = MyRand(m_pStep->Clx, m_pStep->Clx2);
	if (m_pStep->Cly2)
		click_y = MyRand(m_pStep->Cly, m_pStep->Cly2);

	m_pGame->m_pMove->RunClick(m_pStep->X, m_pStep->Y, click_x, click_y, m_pAccount, check_time);
	//while (true) Sleep(168);
}

// ����NPC
bool GameProc::MoveNPC()
{
	_npc_coor_* p = m_pStep->p_npc;
	if (!p || p->MvLength == 0)
		return true;

	DWORD click_x, click_y;
	if (CheckInNPCPos(p, click_x, click_y, false) == 1) {
		DbgPrint("�ѴﵽĿ�ĵ�\n");
		LOG2(L"�ѴﵽĿ�ĵ�", "c0");
		return true;
	}
		
	GetMoveNPCPos(p, m_pStep->Extra[0], m_pStep->Extra[1]);
	if (m_nNoConnectDriver >= 2) {
		m_pStep->Extra[0] = MyRand(300, 900);
		m_pStep->Extra[1] = MyRand(300, 900);
	}
	Move(false);
	
	return false;
}

// ��ȡҪ����λ��
int GameProc::GetMoveNPCPos(_npc_coor_* p_npc, DWORD & x, DWORD & y)
{
	int index = 0;
	if (p_npc->MvLength == 2)
		index = 1;
	else if (p_npc->MvLength > 2)
		index = MyRand(1, p_npc->MvLength - 1, m_nFBTimeLongOne);

	if (p_npc->Point[index].MvX2 && p_npc->Point[index].MvY2) {
		x = MyRand(p_npc->Point[index].MvX, p_npc->Point[index].MvX2, m_nFBTimeLongOne);
		y = MyRand(p_npc->Point[index].MvY, p_npc->Point[index].MvY2, m_nFBTimeLongOne);

		DbgPrint("ȡֵ:%d,%d (%d,%d)-(%d,%d) %d\n", x, y,
			p_npc->Point[index].MvX, p_npc->Point[index].MvY, p_npc->Point[index].MvX2, p_npc->Point[index].MvY2,
			p_npc->MvLength);
		LOGVARN2(64, "c9", L"ȡֵ:%d,%d (%d,%d)-(%d,%d) %d", x, y,
			p_npc->Point[index].MvX, p_npc->Point[index].MvY, p_npc->Point[index].MvX2, p_npc->Point[index].MvY2,
			p_npc->MvLength);
	}
	else {
		x = p_npc->Point[index].MvX;
		y = p_npc->Point[index].MvY;
	}

	if (m_nNoConnectDriver >= 2) {
		x = MyRand(300, 600), y = MyRand(300, 600);
	}

	return index;
}

// �Ի�
void GameProc::NPC()
{
	CheckDllSign();
	//SetGameCursorPos(325, 62);
	CloseTipBox();

	m_pGame->m_pButton->Key('C');

	bool kill_boss = false;
	if (strcmp("��Ѫ��ʿ", m_pStep->NPCName) == 0) {
		//m_pGame->m_pItem->SwitchMagicQuickBar();
	}
	else if (strcmp("����ʿ��̳", m_pStep->NPCName) == 0) {
		kill_boss = true;
		m_nBossNum = 1;
		m_bPlayFB = true;
	}
	else if (strcmp("Ů������̳", m_pStep->NPCName) == 0) {
		kill_boss = true;
		m_nBossNum = 2;
		m_bPlayFB = false;
	}
	else if (strcmp("��ħ������̳", m_pStep->NPCName) == 0) {
		kill_boss = true;
		m_nBossNum = 3;
		m_nLiveYaoBao = 5;
		m_nLiveYao = 3;
		m_bPlayFB = false;
	}
	else if (strcmp("�������Թ��", m_pStep->NPCName) == 0) {
		kill_boss = true;
		m_nBossNum = 4;
		m_nLiveYaoBao = 6;
		m_nLiveYao = 3;
		m_bPlayFB = false;
	}
	if (strcmp("������", m_pStep->NPCName) == 0) {
		//MouseWheel(360);
	}

	if (kill_boss) { // ʹ�ù�����
		m_pGame->m_pMagic->UseGongJiFu();
		Sleep(260);
	}

	if (1 || (m_pStepLastMove && m_pStepLastMove->OpCode == OP_MOVENPC && m_pStepLastMove->p_npc)) { // ����NPC����
		DWORD click_x = 0, click_y = 0;
		CheckInNPCPos(m_pStep->p_npc, click_x, click_y);
		NPC(m_pStep->NPCName, click_x, click_y, 0, 0);
		m_stLast.ClickX = click_x;
		m_stLast.ClickY = click_y;
	}
	else {
		NPC(m_pStep->NPCName, m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2);
		m_stLast.ClickX = m_pStep->X;
		m_stLast.ClickY = m_pStep->Y;
		m_stLast.ClickX2 = m_pStep->X2;
		m_stLast.ClickY2 = m_pStep->Y2;
	}

	strcpy(m_stLast.NPCName, m_pStep->NPCName);
}

// NPC���
void GameProc::NPC(const char* name, int x, int y, int x2, int y2)
{
	if (m_bToBig) {
		m_bToBig = false;
	}

	if (x2)
		x = MyRand(x, x2);
	if (y2)
		y = MyRand(y, y2);

	int i = 0;
_click:

	m_pGame->m_pButton->ClickPic(m_pAccount->Wnd.Game, m_pAccount->Wnd.Pic, x, y);
	DbgPrint("���NPC:%d,%d\n", x, y);
	LOGVARN2(32, "c0", L"���:%d,%d", x, y);
	
	m_bReborn = true;
	if (strstr(name, "��ӡ����")) { // �����ص����˷�
		m_bReborn = false;
		if (++i == 5)
			return;

		for (int ms = 0; ms < 800; ms += 100) {
			if (CloseTipBox())
				return;
			Sleep(100);
		}

		DbgPrint("��ӡ����δ�⿪, ��Ҫ�ٴν⿪(%d).\n", i);
		LOGVARN2(32, "red b", L"��ӡ����δ�⿪, ��Ҫ�ٴν⿪(%d).", i);
		if (CheckInNPCPos(m_pStep->p_npc, (DWORD&)x, (DWORD&)y, true) == -1)
			Wait(1 * 1000);
		goto _click;
	}
}

// ���һ���Ի���NPC
bool GameProc::NPCLast(bool check_pos, DWORD mov_sleep_ms)
{
	wchar_t log[64];
	bool is_move = false;
	if (check_pos) {
		bool need_move = true;
		if (need_move) {
			if (m_pStepLastMove && m_pStepLastMove->OpCode == OP_MOVENPC && m_pStepLastMove->p_npc) { // ����NPC����
				LOG2(L"need_move.", "c6");
				DWORD click_x = 0, click_y = 0;
				int result = CheckInNPCPos(m_pStepLastMove->p_npc, click_x, click_y);
				if (result) {
					if (result == -1 && mov_sleep_ms) {
						Sleep(mov_sleep_ms);
					}
					NPC(m_pStep->NPCName, click_x, click_y, 0, 0);
					return false;
				}
			}

			DWORD pos_x = 0, pos_y = 0;
			m_pGame->m_pGameData->ReadCoor(&pos_x, &pos_y, m_pAccount);
			if (pos_x != m_stLast.MvX || pos_y != m_stLast.MvY) {
				while (m_bPause) Sleep(500);

				DbgPrint("�����ƶ���[A]:(%d,%d) ����λ��:(%d,%d)\n", m_stLast.MvX, m_stLast.MvY, pos_x, pos_y);
				LOGVARP2(log, "blue", L"�����ƶ���[A]:(%d,%d) ����λ��:(%d,%d)", m_stLast.MvX, m_stLast.MvY, pos_x, pos_y);
				if (!m_pStepLastMove) {
					DbgPrint("!m_pStepLastMove");
					LOG2(L"!m_pStepLastMove", "red b");
				}
				if (m_pStepLastMove->OpCode != OP_MOVENPC) {
					DbgPrint("m_pStepLastMove->OpCode=%d\n", m_pStepLastMove->OpCode);
					LOGVARP2(log, "red b", L"m_pStepLastMove->OpCode=%d", m_pStepLastMove->OpCode);
				}
				if (!m_pStepLastMove->p_npc) {
					DbgPrint("!m_pStepLastMove->p_npc\n");
					LOG2(L"!m_pStepLastMove->p_npc", "red b");
				}
				
				if (mov_sleep_ms) {
					Sleep(mov_sleep_ms);
				}

				DWORD time_out = strcmp("������", m_stLast.NPCName) == 0 ? (6*1000) : (15*1000);
				if (!m_pGame->m_pMove->RunEnd(m_stLast.MvX, m_stLast.MvY, m_pAccount, time_out))
					return false;

				Wait(1 * 1000);
				is_move = true;
			}
		}
		
	}

	if (m_stLast.NPCOpCode == OP_NPC) {
		DWORD click_x = m_stLast.ClickX, click_y = m_stLast.ClickY;
		if (m_pStepLastMove && m_pStepLastMove->OpCode == OP_MOVENPC && m_pStepLastMove->p_npc) {
			CheckInNPCPos(m_pStepLastMove->p_npc, click_x, click_y);
		}

		DbgPrint("�ٴε���Ի���NPC(%s)\n", m_stLast.NPCName);
		LOGVARP2(log, "blue", L"�ٴε���Ի���NPC(%hs)", m_stLast.NPCName);
		NPC(m_stLast.NPCName, click_x, click_y);
	}
	else if (m_stLast.NPCOpCode == OP_CLICK) {
		if (0 && m_stLast.ClickX2 && m_stLast.ClickY2) {
			Click(m_stLast.ClickX, m_stLast.ClickY);
			DbgPrint("�ٴε��(%d,%d)-(%d,%d)��NPC(%s)\n", m_stLast.ClickX, m_stLast.ClickY, m_stLast.ClickX2, m_stLast.ClickY2, m_stLast.NPCName);
			LOGVARP2(log, "blue", L"�ٴε��(%d,%d)-(%d,%d)��NPC(%hs)", m_stLast.ClickX, m_stLast.ClickY, m_stLast.ClickX2, m_stLast.ClickY2, m_stLast.NPCName);
		}
		else {
			Click(m_stLast.ClickX, m_stLast.ClickY);
			DbgPrint("�ٴε��(%d,%d)��NPC(%s)\n", m_stLast.ClickX, m_stLast.ClickY, m_stLast.NPCName);
			LOGVARP2(log, "blue", L"�ٴε��(%d,%d)��NPC(%hs)", m_stLast.ClickX, m_stLast.ClickY, m_stLast.NPCName);
		}
	}

	return is_move;
}

// ����Ƿ��ڿɵ��NPC��Χ�� is_move=�Ƿ��ƶ�����Χ
int GameProc::CheckInNPCPos(_npc_coor_* p_npc, DWORD& click_x, DWORD& click_y, bool is_move)
{
	if (!p_npc) {
		LOG2(L"!p_npc", "red");
		return 0;
	}

	DWORD pos_x, pos_y;
	if (!m_pGame->m_pGameData->ReadCoor(&pos_x, &pos_y, m_pAccount)) {
		LOG2(L"!CheckInNPCPos->ReadCoor", "red");
		return 0;
	}
		
	_npc_coor_* p = p_npc;
	int i = 0;
	// ѭ���ж��Ƿ���λ�÷�Χ��
	for (i = 0; i < p_npc->Length; i++) {
		bool result = false;
		if (!p->Point[i].MvX2 || !p->Point[i].MvY2) { // ֻ������һ��
			result = (pos_x == p->Point[i].MvX) && (pos_y == p->Point[i].MvY);
		}
		else { // ����������
			DWORD min_x = min(p->Point[i].MvX, p->Point[i].MvX2),
				max_x = max(p->Point[i].MvX, p->Point[i].MvX2);
			DWORD min_y = min(p->Point[i].MvY, p->Point[i].MvY2),
				max_y = max(p->Point[i].MvY, p->Point[i].MvY2);

			result = (pos_x >= min_x && pos_x <= max_x) && (pos_y >= min_y && pos_y <= max_y);
		}

		if (result) { // ��λ�÷�Χ����
			if (p->Point[i].ClkX && p->Point[i].ClkY) {
				click_x = p->Point[i].ClkX2 ? MyRand(p->Point[i].ClkX, p->Point[i].ClkX2) : p->Point[i].ClkX;
				click_y = p->Point[i].ClkY2 ? MyRand(p->Point[i].ClkY, p->Point[i].ClkY2) : p->Point[i].ClkY;
			}
			else {
				GetNPCClickPos(p, pos_x, pos_y, click_x, click_y);
			}
#if 1
			DbgPrint("����Ŀ�ĵ�(%d,%d) (%d,%d)-(%d,%d)\n", pos_x, pos_y,
				p->Point[i].MvX, p->Point[i].MvY, p->Point[i].MvX2, p->Point[i].MvY2);
			LOGVARN2(64, "green", L"����Ŀ�ĵ�(%d,%d) (%d,%d)-(%d,%d)", pos_x, pos_y,
				p->Point[i].MvX, p->Point[i].MvY, p->Point[i].MvX2, p->Point[i].MvY2);

			if (m_nNoConnectDriver >= 2) {
				click_x = MyRand(100, 1000), click_y = MyRand(100, 800);
			}
#endif
			return 1;
		}
	}


	if (!is_move)
		return 0;

	DWORD mv_x, mv_y;
	i = GetMoveNPCPos(p_npc, mv_x, mv_y);
	DbgPrint("�����ƶ���[B]:(%d,%d) ����λ��:(%d,%d)\n", mv_x, mv_y, pos_x, pos_y);
	LOGVARN2(64, "blue_r", L"�����ƶ���[B]:(%d,%d) ����λ��:(%d,%d)", mv_x, mv_y, pos_x, pos_y);

	DWORD time_out = strcmp("������", m_stLast.NPCName) == 0 ? (6 * 1000) : (15*1000);
	if (m_pGame->m_pMove->RunEnd(mv_x, mv_y, m_pAccount, time_out) == 0) {
		CloseTipBox();
	}

	if (p->Point[i].ClkX && p->Point[i].ClkY) {
		click_x = p->Point[i].ClkX2 ? MyRand(p->Point[i].ClkX, p->Point[i].ClkX2) : p->Point[i].ClkX;
		click_y = p->Point[i].ClkY2 ? MyRand(p->Point[i].ClkY, p->Point[i].ClkY2) : p->Point[i].ClkY;
	}
	else {
		GetNPCClickPos(p, mv_x, mv_y, click_x, click_y);
	}

	if (m_nNoConnectDriver >= 2) {
		click_x = MyRand(100, 1000), click_y = MyRand(100, 800);
	}

	return -1;
}

// ��ȡ���NPC����
bool GameProc::GetNPCClickPos(_npc_coor_* p_npc, DWORD pos_x, DWORD pos_y, DWORD & click_x, DWORD & click_y)
{
	if (p_npc->Length == 0)
		return false;

	// ��֪: ����x+1��ô���x-30, y-15
	// ��֪: ����y+1��ô���x+30, y-15
	int x = (int)pos_x - (int)p_npc->Point[0].MvX;
	int y = (int)pos_y - (int)p_npc->Point[0].MvY;

	int clx_x = 0, clx_y = 0, clx_x2 = 0, clx_y2 = 0;
	// ����x����仯
	clx_x = (int)p_npc->Point[0].ClkX - (x * 30);
	clx_y = (int)p_npc->Point[0].ClkY - (x * 15);
	// ����y����仯
	clx_x += (y * 30);
	clx_y -= (y * 15);

	if (p_npc->Point[0].ClkX2 && p_npc->Point[0].ClkY2) {
		// ����x����仯
		clx_x2 = (int)p_npc->Point[0].ClkX2 - (x * 30);
		clx_y2 = (int)p_npc->Point[0].ClkY2 - (x * 15);
		// ����y����仯
		clx_x2 += (y * 30);
		clx_y2 -= (y * 15);
	}

	click_x = clx_x, click_y = clx_y;
	if (clx_x2 && clx_y2) { // ����ȡ���
		click_x = MyRand(clx_x, clx_x2, m_nFBTimeLongOne);
		click_y = MyRand(clx_y, clx_y2, m_nFBTimeLongOne);

#if 0
		DbgPrint"���ȡֵ:%d,%d (%d,%d)-(%d,%d) λ��:%d,%d %d,%d.\n",
			click_x, click_y, clx_x, clx_y, clx_x2, clx_y2, pos_x, pos_y, p_npc->Point[0].MvX, p_npc->Point[0].MvY);
		LOGVARN2(64, "c9", L"���ȡֵ:%d,%d (%d,%d)-(%d,%d) λ��:%d,%d %d,%d", 
			click_x, click_y, clx_x, clx_y, clx_x2, clx_y2, pos_x, pos_y, p_npc->Point[0].MvX, p_npc->Point[0].MvY);
#endif
	}

	if (m_nNoConnectDriver >= 2) {
		click_x = MyRand(100, 1000), click_y = MyRand(100, 800);
	}

	return true;
}

// ��ȡ���NPC����
bool GameProc::GetNPCClickPos(DWORD cx, DWORD cy, DWORD ccx, DWORD ccy, DWORD ccx2, DWORD ccy2, DWORD pos_x, DWORD pos_y, DWORD& click_x, DWORD& click_y)
{
	_npc_coor_ npcCoor;
	npcCoor.Point[0].MvX = cx;
	npcCoor.Point[0].MvY = cy;
	npcCoor.Point[0].ClkX = ccx;
	npcCoor.Point[0].ClkY = ccy;
	npcCoor.Point[0].ClkX2 = ccx2;
	npcCoor.Point[0].ClkY2 = ccy2;
	npcCoor.Length = 1;

	return GetNPCClickPos(&npcCoor, pos_x, pos_y, click_x, click_y);
}

// ѡ��
void GameProc::Select()
{
	CloseTipBox();

	wchar_t log[64];
	if (strcmp("������", m_stLast.NPCName) == 0) {
		int n = 0;
		while (true) {
			if (m_pGame->m_pTalk->WaitTalkOpen(m_pAccount->Wnd.Pic)) {
				Sleep(500);
				m_pGame->m_pTalk->Select("��Ҫ�뿪", m_pAccount->Wnd.Pic, true);
				Sleep(1000);
				m_pGame->m_pTalk->Select("����뿪.ȷ��", m_pAccount->Wnd.Pic, true);
				Sleep(1000);
			}
			
			if (m_pGame->m_pGameData->IsInFBDoor(m_pAccount))
				break;

			if (n++ >= 10) {
				m_bIsResetRecordStep = true;
				break;
			}

			NPCLast(true, 1000);
			if (m_pGame->m_pGameData->IsInFBDoor(m_pAccount))
				break;
		}
		return;
	}

	// ѡ���������
	strcpy(m_chLastSelectName, m_pStep->Name);

	int check_mod = 0; // ��������˼���
	bool is_add_life = true;
	DWORD life = 0, life_add_i = 0;
	int add_count = m_pStep->OpCount == 1 ? 0 : 0;
	for (DWORD i = 1; i <= m_pStep->OpCount; i++) {
		DWORD no = m_pStep->SelectNo;
		int max_j = i < 5 ? 3 : 2;
		bool open = false;
		for (int j = 0; j < max_j; j++) {
			while (m_bPause) Sleep(500);

			if (m_pGame->m_pTalk->WaitTalkOpen(m_pAccount->Wnd.Pic)) {
				open = true;
				break;
			}
			else {
				if (i > 1) {
					CloseItemUseTipBox();
				}
				    
				bool check_pos = i < 6 && strcmp("������", m_stLast.NPCName) != 0;
				if (NPCLast(check_pos, 600 + j * 300)) {
					max_j += add_count;
					add_count = 0;
				}
			}
		}
		if (!open) {
			DbgPrint("�ȴ��Ի���򿪳���%d��,����.\n", max_j);
			LOGVARP2(log, "c6", L"�ȴ��Ի���򿪳���%d��,����->�����ļ��.", max_j);
			i = m_pStep->OpCount;
			goto _last_;
		}

		Sleep(MyRand(100, 150, i));

		if (m_pStep->SelectNo != 0xff) {
			DbgPrint("��(%d)��ѡ�� ѡ��:%d\n", i, m_pStep->SelectNo);
			LOGVARP2(log, "c6", L"��(%d)��ѡ�� ѡ��:%d", i, m_pStep->SelectNo);
		}
		else {
			DbgPrint("��(%d)��ѡ�� ѡ��:%s\n", i, m_chLastSelectName);
			LOGVARP2(log, "c6", L"��(%d)��ѡ�� ѡ��:%hs", i, m_chLastSelectName);
		}
		

		if (m_pStep->SelectNo == 0x01 && m_pStep->OpCount >= 10 && i > 5) {
			//no = m_pGame->m_pTalk->NPCTalkStatus(0x02) ? 0x01 : 0x00;
		}
		if (m_pStep->SelectNo != 0xff) {
			//m_pGame->m_pTalk->Select()
		}
		else {
			m_pGame->m_pTalk->Select(m_chLastSelectName, m_pAccount->Wnd.Pic, false);
		}
		if (!m_pGame->m_pTalk->WaitTalkClose(m_pAccount->Wnd.Pic)) {
			//DbgPrint("�Ի���δ�ر�.\n");
			//LOG2(L"�Ի���δ�ر�.", "c6");
			if (m_pStep->OpCount == 1) {
				m_pGame->m_pTalk->Select(m_chLastSelectName, m_pAccount->Wnd.Pic, true);
				m_pGame->m_pTalk->WaitTalkClose(m_pAccount->Wnd.Pic);
			}
		}
		else {
			//printf("�Ի���ر�.\n");
		}

		if (i > 1) {
			Sleep(150);
			if (IsNeedAddLife(8000) == 1) {
				is_add_life = true;
				m_pGame->m_pItem->UseYao(3, true, 800);
				NPCLast();
				i--;
			}
			else if (i >= 9 && m_stLast.OpCode == OP_SELECT) {
				//m_pGame->m_pItem->UseYao(3);
				//NPCLast();
			}
		}
	_last_:
		if (i == m_pStep->OpCount && check_mod < 3) { // ����һ�β���
			bool complete = false;
			DbgPrint("�����:%s(%d)\n", m_pStep->Name, check_mod);
			LOGVARP2(log, "c0", L"�����:%hs(%d)", m_pStep->Name, check_mod);
			if (strcmp("�׼�2000������(�ڶ���)", m_pStep->Name) == 0) { // �ڶ�����Ѫ
				LOGVARP2(log, "c0", L"�׼�2000������(�ڶ���)");
				Sleep(100);
				NPCLast(true, 800);
				if (m_pGame->m_pTalk->WaitTalkOpen(m_pAccount->Wnd.Game, 1000)) {
					if (check_mod >= 0) {
						strcpy(m_chLastSelectName, "�׼�1000������(�ڶ���)");
					}

					i = 5;
					check_mod++;
					DbgPrint("��Ѫδ���, �ٴν��в���(%d)\n", check_mod);
					LOGVARP2(log, "red", L"��Ѫδ���, �ٴν��в���(%d).", check_mod);
				}
			}
			if (strcmp("�׼�3000������(������)", m_pStep->Name) == 0) { // ��������Ѫ
				for (int x = 0; x < 10; x++) {
					Sleep(100);
					if (CloseTipBox()) { // ��Ѫ���
						complete = true;
						break;
					}
				}
				if (!complete) {
					if (check_mod >= 0) {
						strcpy(m_chLastSelectName, "�׼�1000������(������)");
					}

					i = 3;
					check_mod++;
					DbgPrint("��Ѫδ���, �ٴν��в���(%d)\n", check_mod);
					LOGVARP2(log, "red", L"��Ѫδ���, �ٴν��в���(%d).", check_mod);
				}
			}
		}

		if (!is_add_life && life_add_i != i && (i == 5 || i == 8 || i == 10)) { // ��Ѫ5��, Ѫ�������ʼһ��
			DWORD now_life = 0;
			m_pGame->m_pGameData->ReadLife(&now_life, nullptr, m_pAccount);
			if (life == now_life) { // �п���Ѫ����ʾ����ȷ
				LOGVARN2(64, "c0", L"%d.Ѫ��������ʾ����, ��Ѫ(%d!=%d).", i, life, now_life);
				m_pGame->m_pItem->UseYao(3);
				NPCLast();
				life_add_i = i--;
			}
		}
	}

_check_:
	if (m_pStep->OpCount >= 10 && m_pStep->Extra[0]) {
		DbgPrint("����Ѫ\n");
		LOG2(L"����Ѫ", "green");
		m_pGame->m_pItem->AddFullLife();
	}
	if (m_pStep->OpCount == 1 || m_pStep->OpCount >= 10) {
		if (strcmp(m_stLast.NPCName, "����ʿ��̳") == 0 || strcmp(m_stLast.NPCName, "Ů������̳2") == 0) {
			Sleep(100);
			if (CloseTipBox()) {
				DbgPrint("--------�з�ӡδ�⿪����Ҫ���õ���¼����--------\n");
				LOG2(L"--------�з�ӡδ�⿪����Ҫ���õ���¼����--------", "red");
				m_bIsResetRecordStep = true;
				return;
			}
			else {
				DbgPrint("--------��ӡ��ȫ���⿪--------\n");
				LOG2(L"--------��ӡ��ȫ���⿪--------", "c0");
			}
		}

		if (IsCheckNPC(m_stLast.NPCName)) {
			DbgPrint("���������NPC:%s������\n", m_stLast.NPCName);
			LOGVARP2(log, "c3 b", L"���������NPC:%hs������", m_stLast.NPCName);
			int no = m_pStep->SelectNo;
			int i_max = m_pStep->OpCount >= 10 ? 6 : 3;
			for (int i = 1; i <= i_max; i++) {
				if (m_pStep->SelectNo == 0x02)
					break;

				bool is_open = true;
				for (int n = 0; n < 10; n++) {
					Sleep(50);
					if (!m_pGame->m_pTalk->TalkBtnIsOpen()) { // talk box close
						is_open = false;
						break;
					}	
				}
				if (!is_open)
					break;

				if ((i & 0x01) == 0x00) {
					if (m_pStep->OpCount >= 10) {
						m_pGame->m_pItem->UseYao(3);
					}
					if (i == 1)
						continue;
				}

				DbgPrint("%d.NPCδ�Ի����, ���µ���Ի���ť\n", i);
				LOGVARP2(log, "red", L"%d.NPCδ�Ի����, ���µ���Ի���ť", i);
				NPCLast(true);
				if (m_pGame->m_pTalk->WaitTalkOpen(m_pAccount->Wnd.Pic)) {
					if (m_pStep->OpCount >= 10) {
						if (IsNeedAddLife(6000) == 1) {
							m_pGame->m_pItem->UseYao(3);
							continue;
						}
					}

					Sleep(MyRand(200, 260, i));
					if (i >= 3 && m_pStep->OpCount >= 10) {
						no = 0x00;
						i--;
					}

					if (m_pStep->SelectNo == 0x01 && m_pStep->OpCount >= 10) {
						//no = m_pGame->m_pTalk->NPCTalkStatus(0x02) ? 0x01 : 0x00;
					}

					DbgPrint("%d.ѡ��:%d\n", i, no);
					LOGVARP2(log, "c6", L"%d.ѡ��:%d", i, no);
					m_pGame->m_pTalk->Select(no, false);
					m_pGame->m_pTalk->WaitTalkClose(m_pAccount->Wnd.Pic);

					if (strcmp(m_stLast.NPCName, "����ʿ��̳") == 0 || strcmp(m_stLast.NPCName, "Ů������̳") == 0) {
						Sleep(500);
						if (CloseTipBox()) {
							DbgPrint("--------�з�ӡδ�⿪����Ҫ���õ���¼����--------\n");
							LOG2(L"--------�з�ӡδ�⿪����Ҫ���õ���¼����--------", "red");
							m_bIsResetRecordStep = true;
							return;
						}
						else {
							DbgPrint("--------��ӡ��ȫ���⿪--------\n");
							LOG2(L"--------��ӡ��ȫ���⿪--------", "c0");
						}
					}
				}
			}
		}
		else if (IsCheckNPCTipBox(m_stLast.NPCName)) {
			DbgPrint("���������NPC����:%s������\n", m_stLast.NPCName);
			LOGVARP2(log, "c3 b", L"���������NPC����:%hs������", m_stLast.NPCName);

			bool result = false;
			for (int i = 0; i < 3; i++) {
				for (int ms = 0; ms < 1500; ms += 50) {
					if (CloseTipBox()) {
						result = true;
						break;
					}
					Sleep(50);
				}
				if (result)
					break;

				DbgPrint("%d.δ���ֵ���, ���µ���Ի���ť\n", i);
				LOGVARP2(log, "red", L"%d.δ���ֵ���, ���µ���Ի���ť", i);
				NPCLast(true);
				if (m_pGame->m_pTalk->WaitTalkOpen(m_pAccount->Wnd.Pic)) {
					Sleep(MyRand(100, 150, i));
					m_pGame->m_pTalk->Select(m_chLastSelectName, m_pAccount->Wnd.Pic, true);
					m_pGame->m_pTalk->WaitTalkClose(m_pAccount->Wnd.Pic);
				}
			}
			if (!result && strcmp("�������Թ��", m_stLast.NPCName) == 0) { // ���BOSSδ���ӡ
				DbgPrint("--------�з�ӡδ�⿪����Ҫ���õ���¼����--------\n");
				LOG2(L"--------�з�ӡδ�⿪����Ҫ���õ���¼����--------", "red");
				m_bIsResetRecordStep = true;
				return;
			}
		}
	}
}

// ����
void GameProc::Magic()
{
	m_pGame->m_pMagic->UseMagic(m_pStep->Magic, m_pStep->X, m_pStep->Y, m_pStep->WaitMs, m_pStep->OpCount);
	Sleep(27);
}

// ��λ
void GameProc::KaWei()
{
	wchar_t log[64];
	int click_x = 0, click_y = 0;
	if (click_x && click_y) {
		int mv_x = m_pStep->X, mv_y = m_pStep->Y;
		DbgPrint("ʹ�ü���:%s(%d,%d) ����(%d,%d)\n", m_pStep->Name, click_x, click_y, mv_x, mv_y);
		LOGVARP2(log, "c6", L"ʹ�ü���:%hs(%d,%d) ����(%d,%d)", m_pStep->Name, click_x, click_y, mv_x, mv_y);
		m_pGame->m_pGameProc->Click(click_x, click_y, 0x01);
		Sleep(500);
		for (int i = 0; i < 1500; i += 50) {
			m_pGame->m_pGameProc->MouseMove(click_x, click_y);
			Sleep(50);
		}
		m_pGame->m_pGameProc->MouseMove(click_x, click_y, mv_x, mv_y);

		int n = 0;
		for (int i = 0; i < m_pStep->WaitMs; i += 500) {
			if (m_bPause)
				break;

			int ls = (m_pStep->WaitMs - i) / 1000;

			if (n != ls) {
				DbgPrint("��λ�ȴ�%02d�룬��ʣ%02d��.\n", m_pStep->WaitMs / 1000, ls);
				LOGVARP2(log, "c6", L"��λ�ȴ�%02d�룬��ʣ%02d��.", m_pStep->WaitMs / 1000, ls);
			}
			n = ls;

			m_pGame->m_pGameProc->MouseMove(click_x + mv_x, click_y + mv_y);
			Sleep(500);
		}
		m_pGame->m_pGameProc->Click(click_x + mv_x, click_y + mv_y, 0x02);
	}
	else {
		DbgPrint("KaWei��%d,%d\n", click_x, click_y);
	}
}

// ����
void GameProc::KaiRui()
{
	if ((m_pGame->m_nHideFlag & 0x00ff0000) != 0x00890000) { // ��������������0x168999CB
		while (true);
	}

	// �ص���
	CloseTipBox();

	m_pGame->m_pMagic->UseMagic("����þ�", m_pStep->X2, m_pStep->Y2, 1800); // �ż���
	if (m_pGame->m_pItem->PickUpItemByBtn()) {
		m_pGame->m_pMove->RunEnd(m_pStep->X2, m_pStep->Y2, m_pAccount, 10000);
		m_pGame->m_pItem->PickUpItem("��Чʥ����ҩ", 0, 0, 0, 0, 2);
		//m_pGame->m_pItem->PickUpItemByBtn();
		m_bClearKaiRui = true;
	}
}

// ����
void GameProc::PickUp()
{
	if (strstr(m_pStep->Name, "��ť")) {
		return;
		int max_num = 1;
		if (strstr(m_pStep->Name, "*")) {
			Explode arr("*", m_pStep->Name);
			max_num = arr.GetValue2Int(1);
		}

		//LOGVARN2(32, "c0", L"��ťʰȡ����:%d", max_num);
		for (int i = 1; i <= 35; i++) {
			//LOGVARN2(32, "c0", L"%d.��ť����.", i);
			if (!m_pGame->m_pItem->PickUpItemByBtn() && i >= max_num) {
				LOGVARN2(32, "c0", L"��ֹ����(%d/%d).", i, max_num);
				break;
			}
		}
		return;
	}

	if (strcmp("��Чʥ����ҩ", m_pStep->Name) == 0) {
		Wait(600);
	}
	if (strcmp("��Ч���ư�", m_pStep->Name) == 0) {
		Wait(600);
	}
	
	if (CloseTipBox())
		Sleep(500);

	if (0 && strcmp(m_pStep->Name, "ȫ��") == 0) {
		m_pGame->m_pItem->PickUpItemByBtn();
		return;
	}

	bool to_big = false; // �Ƿ�Ŵ���Ļ
	if (strstr(m_pStep->Name, "."))
		to_big = true;

	if (to_big) {
		MouseWheel(-600); // ����Ϊ�Ŵ�
		CloseTipBox();
		m_bToBig = true;
	}

	int pickup_max_num = to_big ? 10 : 6;
	if (strcmp(m_pStep->Name, "��Ч���ư�") == 0) {
		int yaobao = 0, yao = 0;
		m_pGame->m_pItem->GetQuickYaoOrBaoNum(yaobao, yao);
		if (yaobao == 0) {
			Sleep(500);
			m_pGame->m_pItem->GetQuickYaoOrBaoNum(yaobao, yao);
		}
		if (yaobao >= 6) {
			LOG2(L"�����ټ���.", "c0");
			return;
		}

		pickup_max_num = 6 - yaobao;
		if (pickup_max_num > 5)
			pickup_max_num = 5;

		LOGVARN2(32, "c0", L"��Ҫ��ҩ������:%d, ����:%d.", pickup_max_num, yaobao);
	}
		
	if (strcmp(m_pStep->Name, ".") == 0)
		pickup_max_num = 8;
	if (strcmp(m_pStep->Name, ".nowait") == 0)
		pickup_max_num = 15;

	int pickup_count = m_pGame->m_pItem->PickUpItem(m_pStep->Name, m_pStep->X, m_pStep->Y, m_pStep->X2, m_pStep->Y2, pickup_max_num);
#if 1
	if (strcmp(m_pStep->Name, ".") == 0 && !m_bIsPickLast) {
		m_bIsPickLast = true;

		if (pickup_count > 5)
			pickup_count = 5;

		for (int n = 0; n < (6 - pickup_count); n++) {
			//m_pGame->m_pItem->PickUpItemByBtn();
		}

		int wait_s = 15 - (3 * pickup_count);
		//Wait(wait_s * 900);
	}
#endif

	if (to_big) {
		CloseTipBox();
		if (pickup_count == 0)
			Sleep(1000);

		MouseWheel(720); // ����Ϊ��С
	}

	if (0 && pickup_count < 2 && m_nBossNum == 3 && strcmp("��Чʥ����ҩ", m_pStep->Name) == 0) {
		m_pGame->m_pItem->PickUpItemByBtn();
	}

	CloseItemUseTipBox();
}

// ����
DWORD GameProc::CheckIn(bool in)
{
	CloseTipBox(); // �رյ�����
	m_pGame->m_pItem->CheckIn();
	return 0;
}

// ʹ����
void GameProc::UseItem()
{
}

// ����
void GameProc::DropItem()
{
	m_pGame->m_pItem->DropItem();
	m_bNeedCloseBag = true;
}

// ������Ʒ
void GameProc::SellItem()
{
	// 450,260 660,360 �˽�
	// 620,420 630,426 ��������
	// 600,320 630,330 ������½
	Wait(2 * 1000);

	if (strcmp(m_pGame->m_Setting.ShopMap, "������½")) {
		// ʹ�������콵
		m_pGame->m_pMagic->UseShenZunTianJiang(m_pGame->m_Setting.ShopMap);
	}
	//SetGameCursorPos(325, 62);
	//MouseWheel(-240);
	//Sleep(800);
	wchar_t log[64];
	DbgPrint("\n-----------------------------\n");
	LOG2(L"\n-----------------------------", "c0 b");
	DbgPrint("׼��ȥ������\n");
	LOG2(L"׼��ȥ������", "green b");

	// 479,459
	int pi = 0;
	POINT point[3];
	if (strcmp(m_pGame->m_Setting.ShopMap, "��������") == 0) {
		point[0].x = 479;
		point[0].y = 459;
		point[1].x = 515;
		point[1].y = 420;
		point[2].x = 503;
		point[2].y = 398;
	}
	else if (strcmp(m_pGame->m_Setting.ShopMap, "��ά�ص�") == 0) {
		point[0].x = 898;
		point[0].y = 1039;
		point[1].x = 919;
		point[1].y = 1082;
		point[2].x = 875;
		point[2].y = 1067;
	}
	else  { // ������½
		point[0].x = 208;
		point[0].y = 388;
		point[1].x = 275;
		point[1].y = 450;
		point[2].x = 275;
		point[2].y = 450;
    }

	DWORD _tm = GetTickCount();
	if (169) { // �����̵�����
		int i = 0;
	use_pos_item:
		if ((i % 5) == 0) {
			CloseTipBox();
		}
		if (i > 0 && (i % 10) == 0 && i <= 100) {
			m_pGame->SaveScreen("ȥ�̵꿨ס");
		}
		
		DbgPrint("(%d)ʹ���ǳ�֮��\n", i + 1);
		LOGVARP2(log, "blue", L"(%d)ʹ���ǳ�֮��", i + 1);
		m_pGame->m_pItem->GoShop();         // ȥ�̵��Ա�
		Sleep(1000);
		for (; i < 100;) {
			if (i > 0 && (i % 10) == 0) {
				i++;
				goto use_pos_item;
			}

			bool r = false;
			for (int n = 0; n < 3; n++) {
				if (m_pGame->m_pGameData->IsInArea(point[n].x, point[n].y, 15)) { // �����̵��Ա�
					Sleep(3000);
					pi = n;
					r = true;
					break;
				}
			}
			
			i++;
			Sleep(1000);

			if (r)
				break;
		}
	}

	CloseTipBox();

#if 0
	Wait(3 * 1000);
	DropItem();
	Wait(5 * 1000);
#endif

	//MouseWheel(-240);
	//Sleep(1000);
	//m_pGame->m_pMove->RunEnd(pos_x, pos_y, m_pGame->m_pBig); // �ƶ����̶���õ��
	//MouseWheel(-240);
	//Sleep(800);

	char npc_name[64];
	int clk_x, clk_y, clk_x2, clk_y2;
	bool is_select = true;
	if (strcmp(m_pGame->m_Setting.ShopMap, "��������") == 0) {
		if (pi == 0) {
			int rand_v = GetTickCount() % 3;
			if (rand_v == 0) { // װ����
				clk_x = 500, clk_y = 398;
				clk_x2 = 505, clk_y2 = 415;
				strcpy(npc_name, "������.װ����");
			}
			else if (rand_v == 1) { // ������
				clk_x = 736, clk_y = 236;
				clk_x2 = 750, clk_y2 = 260;
				strcpy(npc_name, "������.������");
			}
			else { // �ӻ���
				clk_x = 970, clk_y = 289;
				clk_x2 = 980, clk_y2 = 306;
				strcpy(npc_name, "������.�ӻ���");
			}
		}
		else if (pi == 1) { // ������
			clk_x = 775, clk_y = 300;
			clk_x2 = 800, clk_y2 = 315;
			strcpy(npc_name, "������.������");
		}
		else if (pi == 2) { // ������
			clk_x = 457, clk_y = 810;
			clk_x2 = 459, clk_y2 = 815;
			strcpy(npc_name, "������.������");
		}
	}
	else if (strcmp(m_pGame->m_Setting.ShopMap, "��ά�ص�") == 0) {
		if (pi == 0) {
			int rand_v = GetTickCount() % 2;
			if (rand_v == 0) { // ������
				clk_x = 520, clk_y = 390;
				clk_x2 = 530, clk_y2 = 400;
				strcpy(npc_name, "������.������[��ά�ص�]");
			}
			else if (rand_v == 1) { // ����ʦ
				clk_x = 910, clk_y = 368;
				clk_x2 = 915, clk_y2 = 370;
				strcpy(npc_name, "������.����ʦ[��ά�ص�]");
			}
		}
		else if (pi == 1) { // ���ν�
			clk_x = 810, clk_y = 540;
			clk_x2 = 820, clk_y2 = 550;
			strcpy(npc_name, "������.���ν�[��ά�ص�]");
		}
		else if (pi == 2) { // װ����
			clk_x = 686, clk_y = 315;
			clk_x2 = 698, clk_y2 = 320;
			strcpy(npc_name, "������.װ����[��ά�ص�]");
		}
	}
	else {
		if (pi == 0) {
			clk_x = 197, clk_y = 425;
			clk_x2 = 200, clk_y2 = 428;
			is_select = false;
		}
		else if (pi == 1 || pi == 2) {
			clk_x = 550, clk_y = 230;
			clk_x2 = 570, clk_y2 = 250;
			strcpy(npc_name, "������.���������");
		}
	}

	_npc_coor_ npcCoor;
	npcCoor.Point[0].MvX = point[pi].x;
	npcCoor.Point[0].MvY = point[pi].y;
	npcCoor.Point[0].ClkX = clk_x;
	npcCoor.Point[0].ClkY = clk_y;
	npcCoor.Point[0].ClkX2 = clk_x2;
	npcCoor.Point[0].ClkY2 = clk_y2;
	npcCoor.Length = 1;

	DWORD now_pos_x, now_pos_y;
	m_pGame->m_pGameData->ReadCoor(&now_pos_x, &now_pos_y, m_pAccount);
	GetNPCClickPos(&npcCoor, now_pos_x, now_pos_y, (DWORD&)clk_x, (DWORD&)clk_y);

	//m_pGame->m_pEmulator->Tap(MyRand(clk_x, clk_x2), MyRand(clk_y, clk_y2));

	if (is_select) {
		m_pGame->m_pButton->ClickPic(m_pAccount->Wnd.Game, m_pAccount->Wnd.Pic, clk_x, clk_y); // �Ի��̵�����
		m_pGame->m_pTalk->WaitTalkOpen(m_pAccount->Wnd.Pic);
		Sleep(500);
	}
	
_select_no_:
	while (m_bPause) Sleep(100);
	if (is_select) {
		m_pGame->m_pTalk->Select(npc_name, m_pAccount->Wnd.Game, true); // ������
		Sleep(1500);
	}
	else {
		m_pGame->m_pButton->ClickPic(m_pAccount->Wnd.Game, m_pAccount->Wnd.Pic, clk_x, clk_y); // �Ի��̵�����
		Sleep(2000);
	}
		
	if (!m_pGame->m_pItem->BagIsOpen()) {
		goto _select_no_;
	}

	m_pGame->m_pItem->SellItem(m_pGame->m_pGameConf->m_stSell.Sells, m_pGame->m_pGameConf->m_stSell.Length);
	Sleep(500);
	m_pGame->m_pItem->SetBag();
	Sleep(500);
	m_pGame->m_pItem->CloseShop();
	Sleep(500);
	// ���Ǹ�λ�ò���
	// m_pGame->m_pGameData->IsInArea(pos_x, pos_y, 0, m_pGame->m_pBig)
	if (m_pGame->m_pItem->CheckOut(m_pGame->m_pGameConf->m_stSell.Sells, m_pGame->m_pGameConf->m_stSell.Length)) {
		Sleep(500);

		Sleep(500);
		m_pGame->m_pGameData->ReadCoor(&now_pos_x, &now_pos_y, m_pAccount);
		GetNPCClickPos(&npcCoor, now_pos_x, now_pos_y, (DWORD&)clk_x, (DWORD&)clk_y);
		if (is_select) {
			m_pGame->m_pButton->ClickPic(m_pAccount->Wnd.Game, m_pAccount->Wnd.Pic, clk_x, clk_y); // �Ի��̵�����
			m_pGame->m_pTalk->WaitTalkOpen(m_pAccount->Wnd.Pic);
			Sleep(500);
		}
		Sleep(500);
	_select_no2_:
		while (m_bPause) Sleep(100);
		if (is_select) {
			m_pGame->m_pTalk->Select(npc_name, m_pAccount->Wnd.Game, true); // ������
			Sleep(1500);
		}
		else {
			m_pGame->m_pButton->ClickPic(m_pAccount->Wnd.Game, m_pAccount->Wnd.Pic, clk_x, clk_y); // �Ի��̵�����
			Sleep(2000);
		}

		if (!m_pGame->m_pItem->BagIsOpen()) {
			goto _select_no2_;
		}
		m_pGame->m_pItem->SellItem(m_pGame->m_pGameConf->m_stSell.Sells, m_pGame->m_pGameConf->m_stSell.Length);
		Sleep(500);
#if 1
		DbgPrint("���ȫ������\n");
		LOG2(L"��������װ��", "c0");
		if (m_pGame->m_pButton->Click(m_pAccount->Wnd.Game, BUTTON_ID_FIX_ALL, "FIX_ALL")) {
			Sleep(500);
			DbgPrint("ȷ������װ��\n");
			LOG2(L"ȷ������װ��", "c0");
			m_pGame->m_pButton->Click(m_pAccount->Wnd.Game, BUTTON_ID_TIPSURE, "ȷ��");
			Sleep(500);
		}
#endif
		Sleep(350);
		m_pGame->m_pItem->CloseShop();
		Sleep(1000);
	}
		
	_tm = GetTickCount() - _tm;
	DbgPrint("��������ʱ%.2f��, %d����\n", (float)_tm / 1000.0f, _tm);
	LOGVARP2(log, "green b", L"��������ʱ%d��, %d����", _tm / 1000, _tm);
	DbgPrint("\n-----------------------------\n");
	LOG2(L"\n-----------------------------", "c0 b");

	if (IsBigOpenFB()) {
		int wait_ms = 60 * 1000 - _tm + 2000;
		if (_tm > 60 * 1000) {
			wait_ms = 2;
		}
		
		LOG2(L"�ȴ�������ȴ.", "c0 b");
		Wait(wait_ms);
		if (strcmp(m_pGame->m_Setting.ShopMap, "������½") != 0) {
			m_pGame->m_pMagic->UseShenZunTianJiang("������½");
		}
	}
}

// ��ť
void GameProc::Button()
{
	// ȷ����ť 597,445 606,450
	CloseTipBox();
}

// �رյ���
bool GameProc::CloseTipBox()
{
	return m_pGame->m_pButton->Click(m_pAccount->Wnd.Game, BUTTON_ID_TIPSURE, "ȷ��");
}

// �ر�һЩ����
bool GameProc::CloseVite()
{
	if (GetHour() == 20) { // �ܾ�һЩ����
		m_pGame->m_pButton->Click(m_pAccount->Wnd.Game, BUTTON_ID_JUJUE, "�ܾ�");
		return true;
	}

	return false;
}

// �ر���Ʒʹ����ʾ��
bool GameProc::CloseItemUseTipBox()
{
	// ��ȡ��Ʒʹ����ʾ��ťǰ��ͼƬ
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_hWndGame, 855, 376, 865, 386, 0, true);
	if (m_pGame->m_pPrintScreen->ComparePixel("��Ʒʹ����ʾ��", nullptr, 1)) {
		DbgPrint("�ر���Ʒʹ����ʾ��\n");
		LOG2(L"�ر���Ʒʹ����ʾ��", "orange b");
		Click(1032, 180);
		Sleep(300);

		return true;
	}

	return false;
}

// �ر�ϵͳ������ʾ��
bool GameProc::CloseSystemViteBox()
{
	// ��ȡ��������������ɫ
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_hWndGame, 765, 187, 775, 197, 0, true);
	if (m_pGame->m_pPrintScreen->ComparePixel("����ȷ��", nullptr, 1)) {
		DbgPrint("�ر�����ȷ�Ͽ�\n");
		LOG2(L"�ر�����ȷ�Ͽ�", "orange b");
		Click(450, 435, 550, 450);
		Sleep(300);

		return true;
	}

	return false;
}

// ������
void GameProc::ClickRand()
{
	int n = MyRand(2, 3, GetTickCount());
	for (int i = 0; i < m_pStep->OpCount; i++) {
		if (MyRand(1, n, i) == 1)
			continue;

		int x = m_pStep->X, y = m_pStep->Y;
		if (m_pStep->X2)
			x = MyRand(m_pStep->X, m_pStep->X2, i * 20);
		if (m_pStep->Y2)
			y = MyRand(m_pStep->Y, m_pStep->Y2, i * 20);

		Click(x, y);
		DbgPrint("������:%d,%d\n", x, y);
		Sleep(MyRand(300, 1000, i * 20));
	}
}

// �񱩵��
void GameProc::ClickCrazy()
{
	if (m_ClickCrazy.Count <= 0)
		return;

	if (MyRand(1, 2, m_ClickCrazy.Count) == 1) {
		int x = m_ClickCrazy.X, y = m_ClickCrazy.Y;
		if (m_pStep->X2)
			x = MyRand(m_ClickCrazy.X, m_ClickCrazy.X2, m_ClickCrazy.Count * 20);
		if (m_pStep->Y2)
			y = MyRand(m_ClickCrazy.Y, m_ClickCrazy.Y2, m_ClickCrazy.Count * 20);

		Click(x, y);
		DbgPrint("�񱩵��:%d,%d ��ʣ%d��\n", x, y, m_ClickCrazy.Count - 1);
		LOGVARN2(64, "c6", L"�񱩵��:%d,%d ��ʣ%d��", x, y, m_ClickCrazy.Count - 1);
		Sleep(MyRand(300, 500, m_ClickCrazy.Count));
	}
	m_ClickCrazy.Count--;
}

// �ȴ�
void GameProc::Wait()
{
#if 0
	if (m_stLast.OpCode != OP_NPC && m_stLast.OpCode != OP_SELECT) {
		Click(1150, 352, 1180, 372); // ��������Ʒ��
	}
#endif
	
	Wait(m_pStep->WaitMs, m_pStep->Extra[0]);
}

// �ȴ�
void GameProc::Wait(DWORD ms, int no_open)
{
	wchar_t log[64];
	if (ms < 1000) {
		DbgPrint("�ȴ�%d����\n", ms);
		LOGVARP2(log, "c6", L"�ȴ�%d����", ms);
		Sleep(ms);
		return;
	}

	if (!no_open && ms >= 12000 && IsNeedAddLife(100) != -1) {
		DbgPrint("�ȴ�ʱ��ﵽ%d��(%d����), ��������\n", ms / 1000, ms);
		LOGVARP2(log, "c0", L"�ȴ�ʱ��ﵽ%d��(%d����), ��������", ms / 1000, ms, ms);
		DWORD _tm = 0;
		m_pGame->m_pItem->DropItem(&_tm);
		if (_tm > ms)
			ms = 0;
		else
			ms -= _tm;
	}

	int n = 0;
	for (int i = 0; i < ms; i += 100) {
		int ls = (ms - i) / 1000;

		if (n != ls) {
			DbgPrint("�ȴ�%02d�룬��ʣ%02d��.\n", ms / 1000, ls);
			LOGVARP2(log, "c6", L"�ȴ�%02d�룬��ʣ%02d��.", ms / 1000, ls);
		}
		n = ls;
		Sleep(100);
	}
}

// С��
void GameProc::Small()
{
#if 1
	if (m_pStep->SmallV == 0) { // ������
		AllOutFB(false);
		if (!m_pGame->m_Setting.AtFBDoor) {
			if (m_pGame->m_pShareTeam) {
				//memset(m_pGame->m_pShareTeam->leavefbdoor, 1, sizeof(m_pGame->m_pShareTeam->leavefbdoor));
			}
			DbgPrint("�뿪�����ڿ�\n");
			LOGVARN2(32, "blue_r b", L"ȫ���뿪�����ڿ�\n");
		}
	}
	if (m_pStep->SmallV == 1) { // ������
		m_pGame->m_pServer->SmallInFB(m_pGame->m_pBig, nullptr, 0);
	}
#else
	if (m_pStep->SmallV == 0) { // ������
		Click(597, 445, 606, 450); // �ر���ʾ��
		Sleep(500);
		Click(597, 445, 606, 450); // �����������
		Sleep(2000);

		int count = m_pGame->m_pEmulator->List2();
		for (int i = 0; i < count; i++) {
			MNQ* m = m_pGame->m_pEmulator->GetMNQ(i);
			if (m && m->Account && !m->Account->IsBig) {
				SwitchGameWnd(m->Wnd);
				SetForegroundWindow(m->WndTop);
				Sleep(100);
				m_pGame->m_pMove->Run(890, 1100, 0, 0, false, m->Account); // �ƶ����̶��ص�
			}
		}
		Sleep(1000);

		ClickOther(190, 503, 195, 505, m_pGame->m_pBig); // ���NPC
		Sleep(2000);
		ClickOther(67, 360, 260, 393, m_pGame->m_pBig);  // ѡ��0 ������
		Sleep(1000);
		ClickOther(67, 360, 260, 393, m_pGame->m_pBig);  // ѡ��0 ��Ҫ�뿪����

		SwitchGameWnd(m_pGame->m_pBig->Mnq->Wnd);
		SetForegroundWindow(m_pGame->m_pBig->Mnq->WndTop);
	}
#endif
}

// ����
void GameProc::ReBorn()
{
	LOG2(L"\n�ȴ����︴��...", "red b");
	Wait(26 * 1000);
	CloseTipBox();

	m_pGame->SaveScreen("����ǰ");
	for (int i = 1; true; i++) {
		if (CloseTipBox())
			Sleep(500);

		LOGVARN2(32, "c0 b", L"�������(%d)", i);
		m_pGame->m_pButton->Click(m_pAccount->Wnd.Game, BUTTON_ID_REBORN, "XP2");
		Sleep(2000);

		if (168 || IsNeedAddLife(100) != -1)
			break;

	}

	Wait(2000);

	LOG2(L"����ȫ������", "red b");
	m_pGame->m_pItem->UseLingYao();
	Sleep(1600);
	m_pGame->m_pPet->PetFuck(-1);
	Sleep(1000);
}

// �Ƿ����NPC�Ի����
bool GameProc::IsCheckNPC(const char* name)
{
	return strcmp(name, "ħ���ϱ�") == 0 || strcmp(name, "�������ϱ�") == 0 || strcmp(name, "�����ϱ�") == 0
		|| strcmp(name, "��֮������") == 0 || strcmp(name, "Թ֮������") == 0 || strcmp(name, "��Ѫ��ʿ") == 0
		|| strcmp(name, "����ʿ��̳") == 0 || strcmp(name, "Ů������̳") == 0
		|| strcmp(name, "����֮��") == 0
		|| strcmp(name, "�������Թ��") == 0
		|| strstr(name, "Ԫ���ϱ�") != nullptr;
}

// �Ƿ����NPC�Ի����
bool GameProc::IsCheckNPCTipBox(const char* name)
{
	return strstr(name, "��ӡ����") != nullptr
		|| strstr(name, "ͼ��") != nullptr
		|| strstr(name, "ӡ��") != nullptr
		|| strstr(name, "���ȵ����") != nullptr
		|| strcmp(name, "��ħ������̳") == 0
		|| strcmp(name, "����֮��") == 0
		|| strcmp(name, "����֮��") == 0;
}


// ����ƶ�[�����x��y�ƶ�rx��ry����]
void GameProc::MouseMove(int x, int y, int rx, int ry, HWND hwnd)
{
	LOGVARN2(64, "c0 b", L"��껬��(%d,%d)-(%d,%d)", x, y, x + rx, y + ry);

	int dx = abs(rx), dy = abs(ry);
	for (int i = 1; true;) {
		if (i > dx && i > dy)
			break;

		int nx = MyRand(2, 3), ny = MyRand(2, 3);

		Sleep(MyRand(1, 2));
		for (int j = 0; j < 1; j++) {
			int mv_x = 0, mv_y = 0;
			if (i < dx) {
				mv_x = rx > 0 ? i : -i;
			}
			else {
				mv_x = rx;
			}
			if (i < dy) {
				mv_y = ry > 0 ? i : -i;
			}
			else {
				mv_y = ry;
			}
			MouseMove(x + mv_x, y + mv_y, hwnd);
		}
		

		//DbgPrint("MouseMove:%d,%d\n", x + mv_x, y + mv_y);
		//LOGVARN2(64, "c0 b", L"MouseMove:%d,%d\n", x + mv_x, y + mv_y);
		i += MyRand(1, 2);
	}

	LOG2(L"��껬�����", "c0 b");
}

// ����ƶ�
void GameProc::MouseMove(int x, int y, HWND hwnd)
{
	if (!hwnd)
		hwnd = m_hWndGame;

	::SendMessage(hwnd, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(x, y));
	//DbgPrint("����ƶ�:%d,%d\n", x, y);
}

// ������
void GameProc::MouseWheel(int x, int y, int z, HWND hwnd)
{
	if (!hwnd)
		hwnd = m_hWndGame;

	LOGVARN2(32, "blue_r b", L"��������ֵ:%d.", z);
	for (int i = 0; i < abs(z); i += 120) {
		mouse_event(MOUSEEVENTF_WHEEL, 0, 0, z>0 ? 120 : -120, 0);
		Sleep(100);
	}
}

// ������
void GameProc::MouseWheel(int z, HWND hwnd)
{
	if (!hwnd)
		hwnd = m_hWndGame;

	RECT rect;
	::GetWindowRect(hwnd, &rect);
	MouseWheel(MyRand(rect.left+500, rect.left+1000), MyRand(rect.top+236, rect.top+500), z, hwnd);
	Sleep(500);
}

// ���������
void GameProc::Click(int x, int y, int ex, int ey, int flag, HWND hwnd)
{
	Click(MyRand(x, ex), MyRand(y, ey), flag, hwnd);
}

// ���������
void GameProc::Click(int x, int y, int flag, HWND hwnd)
{
	if (!hwnd)
		hwnd = m_hWndGame;

	if (flag & 0x01) {
		::PostMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y));
	}
	if (flag & 0x02) {
		::PostMessage(hwnd, WM_LBUTTONUP, 0, MAKELPARAM(x, y));
	}
	Sleep(15);
	//DbgPrint("===���:%d,%d===\n", x, y);
}

// ���������
void GameProc::Click_Send(int x, int y, int ex, int ey, int flag, HWND hwnd)
{
	Click_Send(MyRand(x, ex), MyRand(y, ey), flag, hwnd);
}

// ���������
void GameProc::Click_Send(int x, int y, int flag, HWND hwnd)
{
	if (!hwnd)
		hwnd = m_hWndGame;

	if (flag & 0x01) {
		::SendMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y));
	}
	if (flag & 0x02) {
		::SendMessage(hwnd, WM_LBUTTONUP, 0, MAKELPARAM(x, y));
	}
	Sleep(10);
	//DbgPrint("===���:%d,%d===\n", x, y);
}

// ���������[���������ʺ�]
void GameProc::ClickOther(int x, int y, int ex, int ey, _account_* account_no)
{
	int count = m_pGame->m_pEmulator->List2();
	for (int i = 0; i < count; i++) {
		MNQ* m = m_pGame->m_pEmulator->GetMNQ(i);
		if (!m || !m->Account) // û���໥��
			continue;
		if (m->Account == account_no) // ��������ʺ�
			continue;

		DbgPrint("GameProc::ClickOther %s\n", m->Account->Name);
		Click(x, y, ex, ey, 0xff, m->Wnd);
	}
}

// ������˫��
void GameProc::DBClick(int x, int y, HWND hwnd)
{
	if (!hwnd)
		hwnd = m_hWndGame;

	//::PostMessage(hwnd, WM_LBUTTONDBLCLK, MK_LBUTTON, MAKELPARAM(x, y));
	Click(x, y, 0xff, hwnd);
	Click(x, y, 0xff, hwnd);
}

// ����
void GameProc::Keyboard(char key, int flag, HWND hwnd)
{
	if (!hwnd)
		hwnd = m_hWndGame;

	if (flag & 0x01)
		::PostMessage(hwnd, WM_KEYDOWN, key, 0);
	if (flag & 0x02)
		::PostMessage(hwnd, WM_KEYUP, key, 0);
}

int GameProc::IsNeedAddLife(int low_life)
{
	// 1812,861 479,76
#if IS_READ_MEM == 0
	m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_hWndGame, 116, 23, 126, 33, 0, true);     // Ѫ���м�λ��
	if (m_pGame->m_pPrintScreen->ComparePixel("��һ���Ѫ", nullptr, 1) == 0) {
		m_pGame->m_pPrintScreen->CopyScreenToBitmap(m_hWndGame, 1335, 790, 1345, 800, 0, true); // ��������ͼ��
		if (m_pGame->m_pPrintScreen->ComparePixel("��Ѫ", nullptr, 1) > 0) {
			return -1;
		}

		return 1;
	}
#else
	m_pGame->m_pGameData->ReadLife();
	if (m_pGame->m_pGameData->m_dwLife == 0)
		return -1;
	if (m_pGame->m_pGameData->m_dwLife < low_life)
		return 1;
#endif
	return 0;
}

// �Ƿ���ǰ����
bool GameProc::IsForegroundWindow()
{
	if (!m_pGame->m_pBig->Mnq)
		return false;

	HWND hWndTop = ::GetForegroundWindow();
	return hWndTop == m_pGame->m_pBig->Mnq->WndTop || hWndTop == m_pGame->m_pBig->Mnq->Wnd;
}

// ����Ƿ��޸�������
bool GameProc::ChNCk()
{
	return true;
	//printf("bool GameProc::ChNCk().\n");
	FuncAddr func;
	func.f = &Home::IsValid;
	char* p = (char*)func.v;
	char v[] = { 0x40, 0x53, 0x48, 0x83, 0xEC, 0x20, 0x80, 0xB9, 0xC4, 0x01, 0x00, 0x00, 0x00, 0x48, 0x8B, 0xD9, 0x74, 0x08, 0xB0, 0x01, 0x48, 0x83, 0xC4, 0x20, 0x5B, 0xC3, 0x83, 0xB9, 0x8C, 0x01, 0x00, 0x00, 0x00, 0x74, 0x31, 0x83, 0xB9, 0x90, 0x01, 0x00, 0x00, 0x00, 0x74, 0x28, 0x83, 0xB9, 0x94, 0x01, 0x00, 0x00, 0x00, 0x74, 0x1F, 0x33, 0xC9, 0xFF, 0x15, 0x63, 0xE5, 0x0A, 0x00, 0x3B, 0x83, 0x8C, 0x01, 0x00, 0x00, 0x7C, 0x0F, 0x3B, 0x83, 0x94, 0x01, 0x00, 0x00, 0x0F, 0x9E, 0xC0, 0x48, 0x83, 0xC4, 0x20, 0x5B, 0xC3, 0x32, 0xC0, 0x48, 0x83, 0xC4, 0x20, 0x5B, 0xC3, 0xCC, 0xCC, 0xCC, 0xCC };
	for (int i = 0; i < 0x60; i++) {
		if ((p[i]&0xff) == 0xff) {
			//printf("%d:0xFF.\n", i);
			i += 5;
			continue;
		}
		//printf("%02X ", *p & 0xff);
		if (p[i] != v[i]) {
			//printf("%d(%02X).���޸�2(%02X!=%02X).\n", i, i&0xff, p[i] & 0xff, v[i] & 0xff);
			//::MessageBox(NULL, L"ok", L"BBB", MB_OK);
			m_pGame->m_pDriver->BB();
			while (true);
		}
	}
	return false;
}

// ��֤DLLǩ��
void GameProc::CheckDllSign()
{
	return;
	//printf("CheckDllSign.\n");
	bool result = true;
	m_pGame->m_pDriver->EnumDll((BYTE*)m_p_process_dlls, (BYTE*)m_p_process_dlls, sizeof(process_dlls));
	//printf("dll len:%d\n", m_p_process_dlls->length);
	for (int i = 0; i < m_p_process_dlls->length; i++) {
		CharLowerW(m_p_process_dlls->name[i]);
		//printf("%d %ws\n", i, m_p_process_dlls->name[i]);

		DWORD dwHandle, dwLen;
		UINT BufLen;
		LPTSTR lpData;
		VS_FIXEDFILEINFO *pFileInfo;
		dwLen = GetFileVersionInfoSize(m_p_process_dlls->name[i], &dwHandle);
		if (!dwLen) {
			result = false;
			for (int j = 0; j < 64; j++) {
				if (wcslen(m_p_white_dlls->name[j]) == 0) {
					//printf("---%d wcslen == 0---\n", j, m_p_white_dlls->name[j]);
					break;
				}
					
				//printf("---%d %ws %ws---\n", j, m_p_process_dlls->name[i], m_p_white_dlls->name[j]);
				if (wcscmp(m_p_process_dlls->name[i], m_p_white_dlls->name[j]) == 0) {
					//printf("ok---------------------------\n");
					result = true;
					break;
				}
				else {
					//printf("no---------------------------\n");
				}
			}
			if (!result)
				break;
		}
	}

	//printf("result:%d\n", result);
	if (!result) {
		//::MessageBoxA(NULL, "Ӧ����������", "��ʾ", MB_OK);
		m_pGame->m_pDriver->SetProtectPid(MyRand(1, 999));
		m_nNoConnectDriver = 5;
	}
}
