#include "Game.h"
#include "GameConf.h"
#include "GameProc.h"
#include "Move.h"
#include "Item.h"
#include "Magic.h"
#include "Talk.h"
#include "Pet.h"
#include "PrintScreen.h"
#include "Button.h"

#include "Driver.h"

#include <shellapi.h>

#include <ShlObj_core.h>
#include <stdio.h>
#include <memory.h>
#include <time.h>
#include <My/Common/func.h>
#include <My/Common/mystring.h>
#include <My/Common/OpenTextFile.h>
#include <My/Common/Explode.h>
#include <My/Driver/KbdMou.h>
#include <My/Db/Sqlite.h>
#include <My/Win32/Peb.h>

#include "../Asm.h"


// ...
void Game::Init(HWND hWnd, const char* conf_path)
{
	// 4:0x071EE0D8 4:0x00 4:0xFFFFFFFF 4:0x100 4:0x071EE248 4:0x99F61480 4:0x9AB140C0 4:0x0000032F
	m_pGame = this;
	m_hUIWnd = hWnd;
	strcpy(m_chPath, conf_path);

	char db_file[255];
	strcpy(db_file, m_chPath);
	strcat(db_file, "\\data\\data.db");
	char* db_file_utf8 = GB23122Utf8(db_file);
	m_pSqlite = new Sqlite(db_file_utf8);
	delete db_file_utf8;

	m_iOpenFBIndex = -1;

	//m_pAccoutCtrl = new WebList;
	//m_pAccoutCtrl->Init(m_pJsCall, "table_1");
	//m_pLogCtrl = new WebList;
	//m_pLogCtrl->Init(m_pJsCall, "log_ul");

	m_pHome = new Home(this);
	m_pGameConf = new GameConf(this);
	m_pEmulator = new Emulator(this);
	m_pMove = new Move(this);
	m_pItem = new Item(this);
	m_pMagic = new Magic(this);
	m_pTalk = new Talk(this);
	m_pPet = new Pet(this);
	m_pButton = new MCButton(this);

	Asm_Nd(GetCurrentThread(), GetNdSysCallIndex()); // ��ֹ������

	m_pDriver = new Driver(this);
	m_pServer = new GameServer(this);
	m_pGameData = new GameData(this);
	m_pGameProc = new GameProc(this);

	m_pServer->SetSelf(m_pServer);
	m_pGameConf->ReadConf(conf_path);

	char pixel_file[255];
	strcpy(pixel_file, m_chPath);
	strcat(pixel_file, "\\data\\pixel.ini");
	m_pPrintScreen = new PrintScreen(this, pixel_file);

	CheckDB();

	ZeroMemory(&m_Setting, sizeof(m_Setting));

	m_nStartTime = time(nullptr);

	// ������ǰ����
	m_pDriver->InstallDriver(m_chPath);
	m_pDriver->SetProtectPid(GetCurrentProcessId());

	m_nVerifyNum = 0;
	m_nVerifyTime = time(nullptr);
}

// ...
Game::~Game()
{
	if (m_pSqlite)
		m_pSqlite->Close();

	delete m_pSqlite;
	m_pSqlite = nullptr;
}

// ����
void Game::Listen(USHORT port)
{
	m_pGame->m_pServer->Listen(port);
}

void Game::Run()
{
	// ������ǰ����
	m_pDriver->SetHidePid(GetCurrentProcessId());

	//DbgPrint("Game::Run!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	if (!m_pGameProc->InitSteps()) {
		DbgPrint("��ʼ������ʧ�ܣ��޷��������У�����\n");
		LOG2(L"��ʼ������ʧ�ܣ��޷��������У�����", "red");
		return;
	}

	for (DWORD dwMs = 0; dwMs < 3000; dwMs += 500) {
		if (m_pBig)
			break;

		Sleep(500);
	}

	if (!m_pBig) {
		Alert(L"������һ�����", 2);
		return;
	}

	//DbgPrint("!m_pGame->m_pHome->IsValid()!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	while (!m_pGame->m_pHome->IsValid()) {
		//DbgPrint("Run:δ����\n");
		Sleep(1000);
	}

	bool try_fs = true;
_try_fs_install_:
	if (m_pDriver->InstallFsFilter(m_chPath, "FsFilter.sys", "370030")) {
		LOG2(L"Install Protect Ok.", "green b");
		if (m_pDriver->StartFsFilter()) {
			LOG2(L"Start Protect Ok.", "green b");
		}
		else {
			if (try_fs) {
				try_fs = false;
				LOG2(L"Start Protect Failed, Try Agin.", "red b");
#if 0
				system("sc stop DriverFs999");
				system("sc delete DriverFs999");
#else
				m_pGame->m_pDriver->Delete(L"DriverFs999");
#endif
				goto _try_fs_install_;
			}
			else {
				LOG2(L"Start Protect Failed, �������������ٳ���.", "red b");
				Alert(L"Start Protect Failed, �������������ٳ���.", 2);
			}
		}
	}
	else {
		LOG2(L"Install Protect Faild.", "red b");
	}

	//printf("!m_pEmulator->List2!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	//m_pEmulator->List2();
	m_pGameData->WatchGame();
	m_pGameData->m_pAccoutBig = m_pBig; // ���
	m_pBig->IsLogin = 1;

#if 0
	::SetForegroundWindow(m_pBig->Mnq->WndTop);
	m_pGameProc->Wait(3 * 1000);
	m_pGameProc->SwitchGameWnd(m_pBig->Mnq->Wnd);
	//m_pItem->SlideBag(1);
	m_pMagic->UseCaiJue(100, 100);
	while (true) Sleep(100);
#endif

	// while (true) Sleep(168);

	while (true) {
		if (m_pBig->Addr.CoorX)
			break;

		Sleep(1000);
	}

	m_pGameProc->SwitchGameWnd(m_pBig->Wnd.Pic);
	m_pGameProc->SwitchGameAccount(m_pBig);
	if (1 && m_pGameData->IsInShenDian(m_pBig)) // ���뿪���
		m_pGameProc->GoLeiMing();

	//m_pGameProc->SellItem();

	if (m_pGameProc->IsInFB()) {
		m_pGameProc->Run(m_pBig);
	}
	else {
		m_pGameProc->GoFBDoor(m_pBig);

		if (GetAccountCount() > 1) {
			while (!m_pBig->IsReady && (!m_pServer->m_iCreateTeam || m_iLoginCount > 0)) Sleep(1000);
			if (!m_pBig->IsReady) {
				m_pServer->CanTeam(m_pBig, nullptr, 0);
				m_pServer->m_iCreateTeam = 0;
			}
		}
		else { // ֻ�����ô��
			m_pServer->CanInFB(m_pGame->m_pBig, nullptr, 0);
		}
		

		while (!IsAllReady()) Sleep(1000);
		m_pGameProc->Run(m_pBig);
	}
}

// ��¼
void Game::Login(Account * p)
{
	if (!IsAutoLogin() || !p)
		return;
}

// ���뵽��¼����
void Game::GoLoginUI(int left, int top)
{
	if (!IsAutoLogin())
		return;
}

// �˳�
void Game::LogOut(Account * account)
{
	if (!account->Mnq)
		return;

	DbgPrint("\n�˳���¼.\n\n");
	LOG2(L"\n�˳���¼.\n", "red b");
	m_pEmulator->Tap(1206, 190, 1230, 215, account->Mnq->Index); // �Ҳ�չ���˵�
	Sleep(2000);
	m_pEmulator->Tap(1208, 645, 1230, 670, account->Mnq->Index); // ����
	Sleep(2500);
	m_pEmulator->Tap(925, 575, 1035, 595, account->Mnq->Index);  // �˳���¼
	Sleep(1500);
	m_pEmulator->Tap(705, 426, 785, 450, account->Mnq->Index);   // ȷ��
}

// �����ʺ������¼
void Game::Input(Account* p)
{
	if (!IsAutoLogin())
		return;
}

// �Զ��Ǻ�
bool Game::AutoLogin(const char* remark)
{
	printf("AutoLogin:%hs\n", remark);
	wchar_t log[64];
	if (!IsAutoLogin())
		return false;

	LOGVARP2(log, "c6", L"AutoLogin:%hs\n", remark);
	if (m_iLoginCount == 0) { // û��Ҫ��¼���ʺ�
		LOG(L"û��Ҫ��¼���ʺ�");
		LoginCompleted("û��Ҫ��¼���ʺ�");
		return false;
	}

	if (m_iLoginFlag >= 0 && m_iLoginIndex != m_iLoginFlag) { // ֻ��һ���˺�
		LOG(L"ֻ��һ���˺�");
		LoginCompleted("ֻ��һ���˺�");
		return false;
	}

	int login_count = GetOnLineCount();
	if (login_count >= MAX_ACCOUNT_LOGIN) {
		::printf("�ѵ�¼��Ϸ�˺�����:%d\n", GetOnLineCount());
		LOGVARP(log, L"�ѵ�¼��Ϸ�˺�����:%d", GetOnLineCount());
		//::MessageBoxA(NULL, "��ȫ��������Ϸ", "��ʾ", MB_OK);
		LoginCompleted("��ȫ��������Ϸ");
		return false;
	}
	if (login_count == 0 || (login_count == 1 && IsOnline(m_pBig))) // û�������߻�ֻ�д������
		m_pGame->m_iSendCreateTeam = 1;

#if 1
	Account* p = GetAccount(m_iLoginIndex);
	while (p && p->LockLogin) { // ����½���ʺ�
		p = GetAccount(++m_iLoginIndex);
	}
	if (!p) // ������
		return false;
	if (IsLogin(p) || (p->IsBig && m_Setting.LogoutByGetXL)) { // ���ڵ�¼���ѵ�¼ ���Ų�������
		m_iLoginIndex++;
		char tmp[128];
		sprintf_s(tmp, "%hs(Status:%08x,Big:%d,LogoutByGetXL:%d)", p->Name, p->Status, p->IsBig, m_Setting.LogoutByGetXL);
		return AutoLogin(tmp); // ��¼��һ��
	}

	p->Mnq = nullptr;
	p->LoginTime = time(nullptr);
	SetStatus(p, ACCSTA_READY);
	UpdateAccountStatus(p);
	m_iLoginIndex++;

	LOGVARN(64, L"������Ϸ, ׼����¼%hs[%d/%d]", p->Name, login_count + 1, m_iLoginCount);
	
	if (0 && p->IsBig) { // ��ŵ�ģ����
		if (1 && !m_pEmulator->Open(p)) {
			LOGVARP(log, L"��ȷ���Ƿ�װ���׵�ģ�������Ƿ��ѱ�ռ��");
			::MessageBoxA(NULL, "��ȷ���Ƿ�װ���׵�ģ�������Ƿ��ѱ�ռ��", "��ʾ", MB_OK);
			return false;
		}
		CreateThread(NULL, NULL, WatchInGame, this, NULL, NULL);
	}
	else { // С�ŵǿͻ���
		ShellExecuteA(NULL, "open", "AutoPatch.exe", NULL, m_Setting.GamePath, SW_SHOWNORMAL);
	}
#else
	for (int i = 0; i < m_iLoginCount; i++,m_iLoginIndex++) {
		Account* p = GetAccount(m_iLoginIndex);
		if (p == nullptr) // ������
			break;
		if (IsLogin(p) || (p->IsBig && m_Setting.LogoutByGetXL)) { // ���ڵ�¼���ѵ�¼ ���Ų�������
			continue;
		}
		if (!m_pEmulator->Open(p))
			continue;

		p->LoginTime = time(nullptr);
		SetStatus(p, ACCSTA_READY);
		UpdateAccountStatus(p);
		LOGVARN(64, "������Ϸ, ׼����¼%hs[%d/%d]", p->Name, login_count + 1, m_iLoginCount);

		m_bLockLogin = true;
		CreateThread(NULL, NULL, WatchInGame, this, NULL, NULL);
		Sleep(100);
		while (m_bLockLogin);
	}
#endif
	return true;
}

// ȫ������
void Game::LoginCompleted(const char* remark)
{
	m_iLoginIndex = 0;
	m_iLoginFlag = -2;
	m_iLoginCount = 0;
	LOGVARN2(64, "blue", L"��ȫ��������Ϸ(%hs)", remark);
}

// ���õǺ�����
void Game::SetLoginFlag(int flag)
{
	if (!IsAutoLogin()) { // �ѵ�¼���
		m_iLoginFlag = flag;
		m_iLoginCount = GetLoginCount();
	}
	m_iLoginIndex = flag >= 0 ? flag : 0;

	if (flag != -2) {
		LOGVARN2(32, "blue_r b", L"��Ҫ��¼�ʺ�����:%d\n", m_iLoginCount);
	}
}

// ��ȡ��Ҫ��¼������
int Game::GetLoginCount()
{
	if (m_iLoginFlag >= 0)
		return !IsLogin(m_AccountList[m_iLoginFlag]) ? 1 : 0;

	int max = 5;
	int count = 0;
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (IsLogin(m_AccountList[i])) {
			max--;
		}
		else {
			if (max == count) // ��¼�����ﵽ���
				break;
			count++;
		}
			
	}
	return count;
}

// ����¼��ʱ���ʺ�
int Game::CheckLoginTimeOut()
{
	int now_time = time(nullptr);

	char time_long_text[64];
	FormatTimeLong(time_long_text, now_time - m_nStartTime);
	//m_pJsCall->SetText("start_time", time_long_text);
		
	// ��ȡģ����
	m_pEmulator->List2();

	// ��ʱ�ػ�
	if (!m_pGameProc->m_bAtFB && ClockShutDown(1))
		return 0;

	// ��ʱ����
	if (IsInTime(m_Setting.OffLine_SH, m_Setting.OffLine_SM, m_Setting.OffLine_EH, m_Setting.OffLine_EM)) {
		if (GetOnLineCount() > 0) {
			m_pEmulator->Close(0);
			m_pServer->SendToOther(0, SCK_CLOSE, true);
			return 0;
		}
	}
	// ��ʱ��¼
	if (IsInTime(m_Setting.AutoLogin_SH, m_Setting.AutoLogin_SM, m_Setting.AutoLogin_EH, m_Setting.AutoLogin_EM)) {
		if (GetOnLineCount() < MAX_ACCOUNT_LOGIN) {
			AutoPlay(-1, false);
			return 0;
		}
	}

	if ((now_time - m_nVerifyTime) > 120) {
		//printf("��֤.\n");
		if (!m_pHome->Verify()) {
			if (++m_nVerifyError >= 6) {
				m_pGame->m_pGameProc->m_bNoVerify = true;
				m_pHome->SetExpire(0);
				m_nVerifyError = 0;

				//Alert(L"��������֤��ʱ������", 2);
			}
		}
		else {
			m_nVerifyError = 0;
		}
		m_nVerifyTime = now_time;
	}

	int count = 0;
	for (int i = 0; i < m_AccountList.size(); i++) {
		Account* p = m_AccountList[i];
		if (p->LoginTime && 
			CheckStatus(p, ACCSTA_READY | ACCSTA_LOGIN)) {
			if ((now_time - p->LoginTime) > m_Setting.LoginTimeOut) {
				LOGVARN(64, L"%hs��¼��ʱ", p->Name);
				m_pServer->Send(p->Socket, SCK_CLOSE, true);
				count++;
			}
		}
		if (p->PlayTime && p->Status != ACCSTA_COMPLETED) {
			UpdateAccountPlayTime(p);
			Sleep(500);
		}
		if (0 && IsOnline(p) && p->LastTime) { // ����
			m_pServer->Send(p->Socket, SCK_PING, true);
			if (m_Setting.TimeOut && (now_time - p->LastTime) > m_Setting.TimeOut) {
				LOGVARN(64, L"%hs�Ѿ�%d��û����Ӧ", p->Name, now_time - p->LastTime);
				m_pServer->Send(p->Socket, SCK_CLOSE);
			}
		}
	}
	return count;
}


// �����Ϸ����ID�Ƿ����
bool Game::CheckGamePid(DWORD pid)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (m_AccountList[i]->GamePid)
			return true;
	}
	return false;
}

// ������Ϸ����
bool Game::SetGameWnd(HWND hwnd, bool* isbig)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		Account* p = m_AccountList[i];
		if (p->GameWnd == (HWND)0x01) {
			::printf("%hs(%08X)\n", p->Name, hwnd);
			p->GameWnd = hwnd;
			if (p->IsBig) {
				m_hWndBig = hwnd;
				if (isbig)
					*isbig = true;
			}
			return true;
		}
	}
	return false;
}

// ������Ϸ����ID
bool Game::SetGameAddr(Account* p, GameDataAddr* addr)
{
	memcpy(&p->Addr, addr, sizeof(GameDataAddr));
	p->IsGetAddr = 1;

	SetStatus(p, ACCSTA_ONLINE, true);
	return true;
}

// ����״̬-ȫ��
void Game::SetAllStatus(int status)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		Account* p = m_AccountList[i];
		p->Status = status;
		GetStatusStr(p);
		UpdateAccountStatus(p);
	}
}

// ����׼��
void Game::SetReady(Account * p, int v)
{
	p->IsReady = v;
}

// ����ģ��������
void Game::SetMNQName(Account* p, char* name)
{
	wchar_t str[128];
	if (name) {
		if (p->IsBig) {
			wsprintfW(str, L"<b class = 'c3'>%hs[���]</b> (<span class=\"cb fz12\">%hs</span>)", p->Name, name);
		}
		else {
			wsprintfW(str, L"<b class = 'c3'>%hs</b> (<span class=\"cb fz12\">%hs</span>)", p->Name, name);
		}
	}
	else {
		if (p->IsBig) {
			wsprintfW(str, L"<b class = 'c3'>%hs[���]</b>", p->Name);
		}
		else {
			wsprintfW(str, L"<b class = 'c3'>%hs</b>", p->Name);
		}
	}

	UpdateTableText(nullptr, p->Index, 1, str);
}

// ����״̬
void Game::SetStatus(Account* p, int status, bool update_text)
{
	if (p == nullptr)
		return;

	p->Status = status;
	GetStatusStr(p);
	if (update_text) {
		UpdateAccountStatus(p);
	}
}

// ����SOKCET
void Game::SetSocket(Account * p, SOCKET s)
{
	p->Socket = s;
}

// ����Flag
void Game::SetFlag(Account * p, int flag)
{
	p->Flag = flag;
}

// ���������
void Game::SetCompleted(Account* p)
{
	LOGVARN2(64, "green", L"%hs[%hs]�Ѿ����", p->Name, p->Role);

	SetStatus(p, ACCSTA_COMPLETED);
	UpdateAccountStatus(p);
}

// ��ȡ�ʺ�����
int Game::GetAccountCount()
{
	return m_AccountList.size();
}

// ��ȡ����ˢ�����ʺ�����
int Game::GetAtFBCount()
{
	int count = 0;
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (m_AccountList[i]->Status == ACCSTA_ATFB)
			count++;
	}
	return count;
}

// ��ȡ���������˺�����
int Game::GetOnLineCount()
{
	int count = 0;
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (IsOnline(m_AccountList[i]))
			count++;
	}
	return count;
}

// �ʺ��Ƿ������б�
Account* Game::GetAccount(const char* name)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (strcmp(name, m_AccountList[i]->Name) == 0)
			return m_AccountList[i];
	}
	return nullptr;
}

// ��ȡ�ʺ�
Account * Game::GetAccount(int index)
{
	if (index < 0)
		return nullptr;

	return index < m_AccountList.size() ? m_AccountList[index] : nullptr;
}

// ��ȡ�ʺ�
Account * Game::GetAccountByRole(const char* role)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (strcmp(role, m_AccountList[i]->Role) == 0)
			return m_AccountList[i];
	}
	return nullptr;
}

// ��ȡ�ʺ�[����״̬]
Account * Game::GetAccountByStatus(int status)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (m_AccountList[i]->Status & status) {
			LOGVARN(64, L"��ȡ�ʺ�:%hs[%08X|%08X]", m_AccountList[i]->Name, m_AccountList[i]->Status, status);
			return m_AccountList[i];
		}
	}
	return nullptr;
}

// ��ȡ��������ʺ� last=���Ƚϵ��ʺ�
Account* Game::GetMaxXLAccount(Account** last)
{
	if (last) *last = nullptr;

	Account* p = nullptr;
	int value = 0;
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (m_AccountList[i]->IsBig || !IsLogin(m_AccountList[i])) // û������
			continue;

		if (i != m_iOpenFBIndex && m_AccountList[i]->XL > value) { // �ϴο��������ȼ����ں���
			p = m_AccountList[i];
			value = p->XL;
			DbgPrint("%hs������%d\n", p->Name, p->XL);
		}
		if (last && !m_AccountList[i]->IsBig) // �������Ƚ��ʺ�
			*last = m_AccountList[i];
	}
	if (!p && m_iOpenFBIndex > -1) { // û���ҵ�, ������һ����
		p = m_AccountList[m_iOpenFBIndex];
		if (p && p->XL == 0) // ��һ����û��������
			p = nullptr;
	}
	if (p) {
		m_iOpenFBIndex = p->Index; // ���濪���������ʺ�����
	}
	return p;
}

// ��ȡ���ڵ�¼���ʺ�
Account* Game::GetReadyAccount(bool nobig)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (nobig && m_AccountList[i]->IsBig) // ���������
			continue;
		if (m_AccountList[i]->Status == ACCSTA_READY)
			return m_AccountList[i];
	}
	return nullptr;
}

// ��ȡ��һ��Ҫ��¼���ʺ�
Account* Game::GetNextLoginAccount()
{
	if (0 && !m_Setting.AutoLoginNext)
		return nullptr;

	DbgPrint("GetAccountByStatus(ACCSTA_INIT | ACCSTA_OFFLINE)\n");
	Account* p = GetAccountByStatus(ACCSTA_INIT | ACCSTA_OFFLINE);
	DbgPrint("GetAccountByStatus(ACCSTA_INIT | ACCSTA_OFFLINE) ���\n");
	if (!p) {
		::printf("for (int i = 0; i < m_AccountList.size(); i++) \n");
		for (int i = 0; i < m_AccountList.size(); i++) {
			if (CheckStatus(m_AccountList[i], ACCSTA_COMPLETED)) { // ��ɵ�
				tm t;
				time_t play_time = m_AccountList[i]->PlayTime;
				gmtime_s(&t, &play_time);

				tm t2;
				time_t now_time = time(nullptr);
				gmtime_s(&t2, &now_time);

				if (t.tm_mday != t2.tm_mday) { // �ʺ������������¼��, �����ٴε�¼
					DbgPrint(" �����ʺ�״̬%d %d %d,%d\n", t.tm_mday, t2.tm_mday, (int)play_time, (int)now_time);
					p = m_AccountList[i];
					SetStatus(p, ACCSTA_INIT); // �����ʺ�״̬
					break;
				}
			}
		}
		DbgPrint("for (int i = 0; i < m_AccountList.size(); i++)  ���\n");
	}

	return p;
}

// ��ȡ�ʺ�
Account* Game::GetAccountBySocket(SOCKET s)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (m_AccountList[i]->Socket == s)
			return m_AccountList[i];
	}
	return nullptr;
}

// ��ȡ����ʺ�
Account* Game::GetBigAccount()
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (m_AccountList[i]->IsBig)
			return m_AccountList[i];
	}
	return nullptr;
}

// ��ȡ���SOCKET
SOCKET Game::GetBigSocket()
{
	Account* p = GetBigAccount();
	return p ? p->Socket : 0;
}

// �����Ѿ����
void Game::SetInTeam(int index)
{
	Account* p = GetAccount(index);
	if (p) {
		m_pServer->CanInFB(p, nullptr, 0);
	}
}

// �ر���Ϸ
void Game::CloseGame(int index)
{
	Account* p = GetAccount(index);
	if (p) {
		if (p->IsBig) {
			if (p->Mnq) {
				DbgPrint("�ر�ģ����:%hs\n", p->Mnq->Name);
				LOGVARN2(64, "red", L"�ر�ģ����:%hs\n", p->Mnq->Name);
				m_pEmulator->Close(p->Mnq->Index);
			}
		}
		else {
			p->OfflineLogin = 0;
			m_pServer->Send(p->Socket, SCK_CLOSE, true);
		}
	}
}

// ������׼�����ʺ�����
void Game::SetReadyCount(int v)
{
	m_iReadyCount = v;
}

// ������׼�����ʺ�����
int Game::AddReadyCount(int add)
{
	m_iReadyCount += add;
	return m_iReadyCount;
}

// �Ա��ʺ�״̬
bool Game::CheckStatus(Account* p, int status)
{
	return p ? (p->Status & status) : false;
}

// �Ƿ��Զ���¼
bool Game::IsAutoLogin()
{
	return m_iLoginFlag != -2;
}

// �Ƿ���ȫ��׼����
bool Game::IsAllReady()
{
	int count = 0;
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (m_AccountList[i]->IsReady)
			count++;
	}

	return count >= 5 || count >= m_AccountList.size();
}

// �ʺ��Ƿ��ڵ�¼
bool Game::IsLogin(Account * p)
{
	return CheckStatus(p, 0xff);
}

// �ʺ��Ƿ�����
bool Game::IsOnline(Account * p)
{
	return CheckStatus(p, 0xff);
}

// ��ȡ״̬�ַ�
char* Game::GetStatusStr(Account* p)
{
	if (p->Status == ACCSTA_INIT) {
		wcscpy(p->StatusStrW, L"δ��¼");
		return strcpy(p->StatusStr, "δ��¼");
	}
		
	if (p->Status == ACCSTA_READY) {
		wcscpy(p->StatusStrW, L"���ڴ���Ϸ...");
		return strcpy(p->StatusStr, "���ڴ���Ϸ...");
	}
		
	if (p->Status == ACCSTA_LOGIN) {
		wcscpy(p->StatusStrW, L"���ڵ�¼...");
		return strcpy(p->StatusStr, "���ڵ�¼...");
	}
		
	if (p->Status == ACCSTA_ONLINE) {
		wcscpy(p->StatusStrW, L"����");
		return strcpy(p->StatusStr, "����");
	}
		
	if (p->Status == ACCSTA_OPENFB) {
		wcscpy(p->StatusStrW, L"���ڿ�������...");
		return strcpy(p->StatusStr, "���ڿ�������...");
	}
		
	if (p->Status == ACCSTA_ATFB) {
		wcscpy(p->StatusStrW, L"����ˢ����...");
		return strcpy(p->StatusStr, "����ˢ����...");
	}
		
	if (p->Status == ACCSTA_COMPLETED) {
		wcscpy(p->StatusStrW, L"�����");
		return strcpy(p->StatusStr, "�����");
	}
		
	if (p->Status == ACCSTA_OFFLINE) {
		wcscpy(p->StatusStrW, L"����");
		return strcpy(p->StatusStr, "����");
	}
		
	return p->Name;
}

// ���Ϳ�ס����ʱ��
int Game::SendQiaZhuS(int second)
{
	if (second > 0) {
		return SendToBig(SCK_QIAZHUS, second, true);
	}
	return 0;
}

// ���͸����
int Game::SendToBig(SOCKET_OPCODE opcode, bool clear)
{
	return m_pServer->Send(GetBigSocket(), opcode, clear);
}

// ���͸����
int Game::SendToBig(SOCKET_OPCODE opcode, int v, bool clear)
{
	if (clear)
		m_pServer->m_Server.ClearSendString();

	m_pServer->m_Server.SetInt(v);
	return SendToBig(opcode, false);
}

// ������ݿ�
void Game::CheckDB()
{
	if (!m_pSqlite->TableIsExists("items")) {
		m_pSqlite->Exec("create table items("\
			"id integer primary key autoincrement, "\
			"account varchar(32),"\
			"name varchar(32),"\
			"created_at integer"\
			")");
		m_pSqlite->Exec("create index IF NOT EXISTS idx_name on items(name);");
	}
	if (!m_pSqlite->TableIsExists("item_count")) {
		m_pSqlite->Exec("create table item_count("\
			"id integer primary key autoincrement, "\
			"name varchar(32),"\
			"num integer,"\
			"created_at integer,"\
			"updated_at integer)");
	}
	if (!m_pSqlite->TableIsExists("fb_count")) {
		m_pSqlite->Exec("create table fb_count("\
			"id integer primary key autoincrement, "\
			"num integer)");
	}
	if (!m_pSqlite->TableIsExists("fb_time_long")) {
		m_pSqlite->Exec("create table fb_time_long("\
			"id integer primary key autoincrement, "\
			"time_long integer)");
	}
	if (!m_pSqlite->TableIsExists("fb_record")) {
		m_pSqlite->Exec("create table fb_record("\
			"id integer primary key autoincrement, "\
			"start_time integer,"\
			"end_time integer,"\
			"time_long integer,"\
			"status integer)"
		);
	}
}

// ������Ʒ��Ϣ
void Game::UpdateDBItem(const char* account, const char* item_name)
{
	int now_time = time(nullptr);

	char sql[255];
	sprintf_s(sql, "INSERT INTO items(account,name,created_at) VALUES ('%hs','%hs',%d)",
		account, item_name, now_time);
	m_pSqlite->Exec(sql);

	sprintf_s(sql, "SELECT num FROM item_count WHERE name='%hs'", item_name);
	if (m_pSqlite->GetRowCount(sql) == 0) { // û��
		::printf("����\n");
		sprintf_s(sql, "INSERT INTO item_count(name,num,created_at,updated_at) VALUES ('%hs',%d,%d,%d)", 
			item_name, 1, now_time, now_time);
		m_pSqlite->Exec(sql);
	}
	else { // ����
		::printf("����\n");
		sprintf_s(sql, "UPDATE item_count SET num=num+1,updated_at=%d WHERE name='%hs'",
			now_time, item_name);
		m_pSqlite->Exec(sql);
	}
}

// ����ˢ��������
void Game::UpdateDBFB(int count)
{
	if (m_pSqlite->GetRowCount("SELECT num FROM fb_count WHERE id=1") == 0) { // û��
		::printf("����\n");
		m_pSqlite->Exec("INSERT INTO fb_count(id,num) VALUES (1,1)");
	}
	else { // ����
		::printf("����\n");
		m_pSqlite->Exec("UPDATE fb_count SET num=num+1 WHERE id=1");
	}
}

// ����ˢ����ʱ��
void Game::UpdateDBFBTimeLong(int time_long)
{
	char sql[128];
	if (m_pSqlite->GetRowCount("SELECT time_long FROM fb_time_long WHERE id=1") == 0) { // û��
		sprintf_s(sql, "INSERT INTO fb_time_long(id,time_long) VALUES (1,%d)", time_long);
		m_pSqlite->Exec(sql);
	}
	else { // ����
		sprintf_s(sql, "UPDATE fb_time_long SET time_long=time_long+%d WHERE id=1", time_long);
		m_pSqlite->Exec(sql);
	}
}

// ���븱����¼ status=0����ͨ�� 1-�����ؿ� 2-��ס��ʱ
void Game::InsertFBRecord(int start_time, int end_time, int status)
{
	char sql[128];
	sprintf_s(sql, "INSERT INTO fb_record(start_time,end_time,time_long,status) VALUES (%d,%d,%d,%d)", 
		start_time, end_time, end_time-start_time, status);
	m_pSqlite->Exec(sql);
}

// ��ѯ������¼
int Game::SelectFBRecord(char*** result, int* col)
{
	return m_pSqlite->SelectAll("SELECT * FROM fb_record order by id desc", result, col);
}

// �����ؿ���������
void Game::UpdateReOpenFBCount(int count)
{
	char text[16];
	sprintf_s(text, "%d",count);

	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	strcpy(msg->id, "fb_reopen_count");
	strcpy(msg->text, text);

	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// ����ˢ���������ı�
void Game::UpdateFBCountText(int lx, bool add)
{
	if (add) {
		UpdateDBFB();
	}
	
	char num[16] = { '0', 0 };
	m_pSqlite->GetOneCol("SELECT num FROM fb_count WHERE id=1", num);

	char text[32];
	sprintf_s(text, "%d/%hs", lx, num);

	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	strcpy(msg->id, "fb_count");
	strcpy(msg->text, text);
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// ����ˢ����ʱ���ı�
void Game::UpdateFBTimeLongText(int time_long, int add_time_long)
{
	wchar_t text[64];
	FormatTimeLong(text, time_long);

	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	strcpy(msg->id, "fb_time_long");
	wcscpy(msg->text_w, text);
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);

	if (add_time_long > 0)
		UpdateDBFBTimeLong(add_time_long);

	char time_long_all[16] = { '0', 0 };
	m_pSqlite->GetOneCol("SELECT time_long FROM fb_time_long WHERE id=1", time_long_all);


	wchar_t text_all[64];
	FormatTimeLong(text_all, atoi(time_long_all));

	msg = GetMyMsg(MSG_SETTEXT);
	strcpy(msg->id, "fb_time_long_all");
	wcscpy(msg->text_w, text_all);
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
	//printf("fb_time_long_all:%hs\n", text_all);
}

// ���µ��߸�������ı�
void Game::UpdateOffLineAllText(int offline, int reborn)
{
	char text[32];
	sprintf_s(text, "%d/%d", offline, reborn);
	
	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	strcpy(msg->id, "offline_count");
	strcpy(msg->text, text);

	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// д���ռ�
void Game::WriteLog(const char* log)
{
	//m_LogFile << log << endl;
}

// ��ȡ�����ļ�
DWORD Game::ReadConf()
{
	char path[255], logfile[255];

	strcpy(path, m_chPath);
	strcpy(logfile, m_chPath);
	strcat(path, "\\�ʺ�.txt");
	strcat(logfile, "\\�ռ�.txt");

	Asm_Nd(GetCurrentThread(), GetNdSysCallIndex()); // ��ֹ������

	::printf("�ʺ��ļ�:%hs\n", path);
	OpenTextFile file;
	if (!file.Open(path)) {
		::printf("�Ҳ���'�ʺ�.txt'�ļ�������");
		return false;
	}


	int i = 0, index = 0;
	int length = 0;
	char data[128];
	while ((length = file.GetLine(data, 128)) > -1) {
		//::printf("length:%d\n", length);
		if (length == 0) {
			continue;
		}

		trim(data);
		if (strstr(data, "::") == nullptr && strstr(data, "=")) { // ��������
			if (!ReadAccountSetting(data)) {
				ReadSetting(data);
			}
			
			continue;
		}

		// �ʺ��б�
		Explode explode("::", data);
		if (explode.GetCount() < 2)
			continue;

		Account* p = GetAccount(explode[0]);
		if (!p) { // ������
			p = new Account;
			ZeroMemory(p, sizeof(Account));

			strcpy(p->Name, explode[0]);
			strcpy(p->Password, explode[1]);
			strcpy(p->Role,    "--");
			p->RoleNo = explode.GetValue2Int(2);
			p->IsBig = explode.GetCount() > 3 && strcmp(explode[3], "���") == 0;
			p->OfflineLogin = 1;
			p->Status = ACCSTA_INIT;
			GetStatusStr(p);
			p->Index = m_AccountList.size();
			m_AccountList.push_back(p);

			//::printf("explode[2]:%hs\n", explode[2]);
			AddAccount(p);

			if (!m_pBig && p->IsBig)
				m_pBig = p;
		}
	}
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)GetMyMsg(MSG_FILLTABLE), 0);

	// �޸�ģ�����ֱ���
	m_pEmulator->SetRate(0, 1280, 720, 240);

	return 0;
}

// ��ȡ�ʺ�����
bool Game::ReadAccountSetting(const char* data)
{
	Explode explode("=", data);
	Account* p = GetAccount(explode[0]);
	if (!p)
		return false;

	Explode arr("|", explode[1]);
	if (arr.GetCount() == 2) {
		if (strcmp(arr[1], "��ɫ") == 0) {
			strcpy(p->Role, arr[0]);
			LOGVARN2(64, "c0", L"��ɫ.%hs: %hs", explode[0], p->Role);
		}
		else {
			strcpy(p->SerBig, arr[0]);
			strcpy(p->SerSmall, arr[1]);
			LOGVARN2(64, "cb", L"����.%hs: %hs|%hs", explode[0], arr[0], arr[1]);
		}
	}

	return true;
}

// ��ȡ��������
void Game::ReadSetting(const char* data)
{
	char tmp[16] = { 0 };
	Explode explode("=", data);
	if (strcmp(explode[0], "����.") == 0) {
		if (strcmp(explode[1], "������+����") == 0)
			m_pHome->SetFree(true);
	}
	else if (strcmp(explode[0], "ģ����·��") == 0) {
		strcpy(m_Setting.MnqPath, explode[1]);
		m_pEmulator->SetPath(m_Setting.MnqPath);
	}
	else if (strcmp(explode[0], "��Ϸ·��") == 0) {
		strcpy(m_Setting.GamePath, explode[1]);
	}
	else if (strcmp(explode[0], "��Ϸ�ļ�") == 0) {
		strcpy(m_Setting.GameFile, explode[1]);
	}
	else if (strcmp(explode[0], "��Ϸ����") == 0) {
		strcpy(m_Setting.SerBig, explode[1]);
	}
	else if (strcmp(explode[0], "��ϷС��") == 0) {
		strcpy(m_Setting.SerSmall, explode[1]);
	}
	else if (strcmp(explode[0], "������ʱ") == 0) {
		m_Setting.InitTimeOut = explode.GetValue2Int(1);
		strcpy(tmp, "��");
		SetSetting("init_timeout", m_Setting.InitTimeOut);
	}
	else if (strcmp(explode[0], "��¼��ʱ") == 0) {
		m_Setting.LoginTimeOut = explode.GetValue2Int(1);
		strcpy(tmp, "��");
		SetSetting("login_timeout", m_Setting.LoginTimeOut);
	}
	else if (strcmp(explode[0], "��Ϸ��ʱ") == 0) {
		m_Setting.TimeOut = explode.GetValue2Int(1);
		strcpy(tmp, "��");
	}
	else if (strcmp(explode[0], "������ʱ") == 0) {
		m_Setting.FBTimeOut = explode.GetValue2Int(1);
		SetSetting("fb_timeout", m_Setting.FBTimeOut);
		m_Setting.FBTimeOut *= 60;
		strcpy(tmp, "����");
	}
	else if (strcmp(explode[0], "����ʱ��") == 0) {
		m_Setting.FBTimeOutErvry = explode.GetValue2Int(1);
		SetSetting("fb_timeout_ervry", m_Setting.FBTimeOutErvry);
		m_Setting.FBTimeOutErvry *= 60;
		strcpy(tmp, "����");
	}
	else if (strcmp(explode[0], "��������") == 0) {
		m_Setting.ReConnect = strcmp("��", explode[1]) == 0;
	}
	else if (strcmp(explode[0], "�Զ��л��ʺ�") == 0) {
		m_Setting.AutoLoginNext = strcmp("��", explode[1]) == 0;
	}
	else if (strcmp(explode[0], "ˢ��ػ�") == 0) {
		m_Setting.ShutDownNoXL = strcmp("��", explode[1]) == 0;
		SetSetting("shuawan_shutdown", m_Setting.ShutDownNoXL);
	}
	else if (strcmp(explode[0], "��ʱ�ػ�") == 0) {
		Explode t("-", explode[1]);
		if (t[0]) {
			Explode s(":", t[0]);
			m_Setting.ShutDown_SH = s.GetValue2Int(0);
			m_Setting.ShutDown_SM = s.GetValue2Int(1);
		}
		if (t[1]) {
			Explode e(":", t[1]);
			m_Setting.ShutDown_EH = e.GetValue2Int(0);
			m_Setting.ShutDown_EM = e.GetValue2Int(1);
		}

		SetSetting("shutdown_sh", m_Setting.ShutDown_SH);
		SetSetting("shutdown_sm", m_Setting.ShutDown_SM);
		SetSetting("shutdown_eh", m_Setting.ShutDown_EH);
		SetSetting("shutdown_em", m_Setting.ShutDown_EM);
	}
	else if (strcmp(explode[0], "��ʱ����") == 0) {
		Explode t("-", explode[1]);
		if (t[0]) {
			Explode s(":", t[0]);
			m_Setting.OffLine_SH = s.GetValue2Int(0);
			m_Setting.OffLine_SM = s.GetValue2Int(1);
		}
		if (t[1]) {
			Explode e(":", t[1]);
			m_Setting.OffLine_EH = e.GetValue2Int(0);
			m_Setting.OffLine_EM = e.GetValue2Int(1);
		}

		SetSetting("offline_sh", m_Setting.OffLine_SH);
		SetSetting("offline_sm", m_Setting.OffLine_SM);
		SetSetting("offline_eh", m_Setting.OffLine_EH);
		SetSetting("offline_em", m_Setting.OffLine_EM);
	}
	else if (strcmp(explode[0], "��ʱ��¼") == 0) {
		Explode t("-", explode[1]);
		if (t[0]) {
			Explode s(":", t[0]);
			m_Setting.AutoLogin_SH = s.GetValue2Int(0);
			m_Setting.AutoLogin_SM = s.GetValue2Int(1);
		}
		if (t[1]) {
			Explode e(":", t[1]);
			m_Setting.AutoLogin_EH = e.GetValue2Int(0);
			m_Setting.AutoLogin_EM = e.GetValue2Int(1);
		}

		SetSetting("autologin_sh", m_Setting.AutoLogin_SH);
		SetSetting("autologin_sm", m_Setting.AutoLogin_SM);
		SetSetting("autologin_eh", m_Setting.AutoLogin_EH);
		SetSetting("autologin_em", m_Setting.AutoLogin_EM);
	}

	//printf("����.%s: %s%s\n", explode[0], explode[1], tmp);
	//LOGVARN2(64, "cb", L"����.%hs: %hs%hs", explode[0], explode[1], tmp);
	LOGVARN2(64, "cb", L"����.%hs: %hs%hs", explode[0], explode[1], tmp);
}

// �Զ��ػ�
void Game::AutoShutDown()
{
	if (m_Setting.ShutDownNoXL)
		ShutDown();
}

// ��ʱ�ػ�
bool Game::ClockShutDown(int flag)
{
	if (!m_Setting.ShutDown_SH && !m_Setting.ShutDown_SM && !m_Setting.ShutDown_EH && !m_Setting.ShutDown_EM)
		return false;

	if (IsInTime(m_Setting.ShutDown_SH, m_Setting.ShutDown_SM, m_Setting.ShutDown_EH, m_Setting.ShutDown_EM)) {
		ShutDown();
		return true;
	}
	return false;
}

// �ػ�
void Game::ShutDown()
{
	SaveScreen("�ػ�");
	system("shutdown -s -t 10");
}

// ��ǰʱ���Ƿ��ڴ�ʱ��
bool Game::IsInTime(int s_hour, int s_minute, int e_hour, int e_minute)
{
	SYSTEMTIME stLocal;
	::GetLocalTime(&stLocal); // ��õ�ǰʱ��
	if (s_hour == e_hour) { // ʱ����ͬһСʱ
		return stLocal.wHour == s_hour
			&& stLocal.wMinute >= s_minute && stLocal.wMinute <= e_minute;
	}
	if (s_hour < e_hour) { // ����ͬһСʱ
		if (stLocal.wHour == s_hour) // �ڿ�ʼСʱʱ��
			return stLocal.wMinute >= s_minute;
		if (stLocal.wHour == e_hour) // �ڽ���Сʱʱ��
			return stLocal.wMinute <= e_minute;
		if (stLocal.wHour > s_hour && stLocal.wHour < e_hour) // ���м�
			return true;
	}
	return false;
}

// ���͵�html����
void Game::SetSetting(const char* name, int v)
{
	my_msg* msg = GetMyMsg(MSG_SETSETTING);
	strcpy(msg->id, name);
	msg->value[0] = v;
	msg->value[1] = 0;
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// ����
void Game::PutSetting(const wchar_t* name, int v)
{
	if (wcscmp(name, L"fb_mode") == 0) {
		m_Setting.FBMode = v;
		m_Setting.LogoutByGetXL = 0;
		m_Setting.NoGetXL = 0;
		m_Setting.NoPlayFB = 0;

		char str[16];
	    if (v == 1) strcpy(str, "�����ˢ");
		else if (v == 2) {
			strcpy(str, "����ֻˢ");
			m_Setting.NoGetXL = 1;
		}
		else if (v == 3) {
			strcpy(str, "�����ˢ");
			m_Setting.LogoutByGetXL = 1;
			m_Setting.NoPlayFB = 0;
		}
		else if (v == 4) {
			strcpy(str, "ֻ�첻ˢ");
			m_Setting.LogoutByGetXL = 1;
			m_Setting.NoPlayFB = 1;
		}
		else strcpy(str, "δ֪");

		LOGVARN2(64, "cb", L"�޸�.����ģʽ:%hs.", str);
	}
	else if (wcscmp(name, L"close_mnq") == 0) {
		m_Setting.CloseMnq = v;
		LOGVARN2(64, "cb", L"�޸�.�ر�ģ����:%hs.", v ? "��" : "��");
	}
	else if (wcscmp(name, L"login_timeout") == 0) {
		m_Setting.LoginTimeOut = v;
		LOGVARN2(64, "cb", L"�޸�.��¼��ʱ:%d��.", v);
	}
	else if (wcscmp(name, L"fb_timeout") == 0) {
		m_Setting.FBTimeOut = v * 60;
		LOGVARN2(64, "cb", L"�޸�.������ʱ:%d����.", v);
	}
	else if (wcscmp(name, L"qiazhu") == 0) {
		SendQiaZhuS(v);
		LOGVARN2(64, "cb", L"�޸�.��ס����:%d��.", v);
	}
	else if (wcscmp(name, L"shuawan_shutdown") == 0) {
		m_Setting.ShutDownNoXL = v;
		LOGVARN2(64, "cb", L"�޸�.ˢ��ػ�:%hs.", v ? "��" : "��");
	}
	/******** ��ʱ�ػ� ********/
	else if (wcscmp(name, L"shutdown_sh") == 0) {
		m_Setting.ShutDown_SH = v;
		LOGVARN2(64, "cb", L"�޸�.��ʱ�ػ�(��ʼ):%d��.", v);
	}
	else if (wcscmp(name, L"shutdown_sm") == 0) {
		m_Setting.ShutDown_SM = v;
		LOGVARN2(64, "cb", L"�޸�.��ʱ�ػ�(��ʼ):%d��.", v);
	}
	else if (wcscmp(name, L"shutdown_eh") == 0) {
		m_Setting.ShutDown_EH = v;
		LOGVARN2(64, "cb", L"�޸�.��ʱ�ػ�(����):%d��.", v);
	}
	else if (wcscmp(name, L"shutdown_em") == 0) {
		m_Setting.ShutDown_EM = v;
		LOGVARN2(64, "cb", L"�޸�.��ʱ�ػ�(����):%d��.", v);
	}
	/******** ��ʱ���� ********/
	else if (wcscmp(name, L"offline_sh") == 0) {
		m_Setting.OffLine_SH = v;
		LOGVARN2(64, "cb", L"�޸�.��ʱ����(��ʼ):%d��.", v);
	}
	else if (wcscmp(name, L"offline_sm") == 0) {
		m_Setting.OffLine_SM = v;
		LOGVARN2(64, "cb", L"�޸�.��ʱ����(��ʼ):%d��.", v);
	}
	else if (wcscmp(name, L"offline_eh") == 0) {
		m_Setting.OffLine_EH = v;
		LOGVARN2(64, "cb", L"�޸�.��ʱ����(����):%d��.", v);
	}
	else if (wcscmp(name, L"offline_em") == 0) {
		m_Setting.OffLine_EM = v;
		LOGVARN2(64, "cb", L"�޸�.��ʱ����(����):%d��.", v);
	}
	/******** ��ʱ��¼ ********/
	else if (wcscmp(name, L"autologin_sh") == 0) {
		m_Setting.AutoLogin_SH = v;
		LOGVARN2(64, "cb", L"�޸�.��ʱ��¼(��ʼ):%d��.", v);
	}
	else if (wcscmp(name, L"autologin_sm") == 0) {
		m_Setting.AutoLogin_SM = v;
		LOGVARN2(64, "cb", L"�޸�.��ʱ��¼(��ʼ):%d��.", v);
	}
	else if (wcscmp(name, L"autologin_eh") == 0) {
		m_Setting.AutoLogin_EH = v;
		LOGVARN2(64, "cb", L"�޸�.��ʱ��¼(����):%d��.", v);
	}
	else if (wcscmp(name, L"autologin_em") == 0) {
		m_Setting.AutoLogin_EM = v;
		LOGVARN2(64, "cb", L"�޸�.��ʱ��¼(����):%d��.", v);
	}

	else if (wcscmp(name, L"no_auto_select") == 0) {
		m_Setting.NoAutoSelect = v;
		LOGVARN2(64, "cb", L"�޸�.�ֶ�ѡ��:%hs.", v ? "��" : "��");
	}
	else if (wcscmp(name, L"is_debug") == 0) {
		m_Setting.IsDebug = v;
		LOGVARN2(64, "cb", L"�޸�.������Ϣ:%hs.", v ? "��ʾ" : "����ʾ");
	}
	else if (wcscmp(name, L"talk_open") == 0) {
		m_Setting.TalkOpen = v;
		LOGVARN2(64, "cb", L"�޸�.��������:%hs.", v ? "��" : "��");
	}
	else if (wcsstr(name, L"account_locklogin")) {
		Account* account = GetAccount(_wtoi(name + 18));
		if (account && !account->IsBig) {
			account->LockLogin = v;
			LOGVARN2(64, "cb", L"�޸�.�ʺ�(%hs)%hs.", account->Name, v ? "����½" : "��½");
		}
	}
}

// ����
void Game::PutSetting(wchar_t* name, wchar_t* v)
{
	if (!v || wcslen(v) == 0)
		return;

	char* value = wchar2char(v);
	::printf("����:%hs\n", value);
	delete value;
}

// ����Ϸ
int Game::OpenGame(int index, int close_all)
{
	if (close_all && m_Setting.CloseMnq)
		m_pEmulator->CloseAll();
	if (m_Setting.FBMode < 1 || m_Setting.FBMode > 4)
		m_Setting.FBMode = 1;

	printf("OpenGame\n");
	return AutoPlay(index, false);
}

// ����
// ����Ϸ
void Game::CallTalk(const char* text, int type)
{
	m_pServer->m_Server.ClearSendString();
	m_pServer->m_Server.SetInt(type);
	m_pServer->m_Server.SetInt(strlen(text));
	m_pServer->m_Server.SetContent((void*)text, strlen(text));
	m_pServer->SendToOther(0, SCK_TALK, false);
}

// ע��DLL
int Game::InstallDll()
{
	if (!m_pHome->IsValid() && !m_pDriver->m_bIsInstallDll) {
		//m_pJsCall->ShowMsg("�޷�����, ���ȼ���.", "��ʾ", 2);
		return -1;
	}
	//m_pJsCall->SetBtnDisabled("start_btn", 1);
	return m_pDriver->InstallDll(m_chPath) ? 1 : 0;
}

// �Զ��Ǻ�
int Game::AutoPlay(int index, bool stop)
{
	printf("AutoPlay\n");
	//m_pJsCall->SetBtnDisabled("start_btn", 1);
	if (stop) {
		SetLoginFlag(-2);
		//m_pJsCall->SetText("auto_play_btn", "�Զ��Ǻ�");
		//m_pJsCall->SetBtnDisabled("start_btn", 0);
		return 1;
	}
	if (!m_pDriver->m_bIsInstallDll || IsAutoLogin())
		return 0;

	SetLoginFlag(index);
	if (!AutoLogin("AutoPlay")) {
		LoginCompleted("�Զ��Ǻ�");
	}

	return 1;
} 

// ����ʺ�
void Game::AddAccount(Account * account)
{
	CString  name = L"<b class='c3'>";
	name += account->Name;
	if (account->IsBig) {
		name += L"[���]";
	}
	name += L"</b>";

	//wsprintfW(text, L"%hs", name.GetBuffer());
	my_msg* msg = GetMyMsg(MSG_ADDTABLEROW);
	strcpy(msg->id, "table_1");
	strcpy(msg->text, "--");
	msg->value[0] = account->Index + 1;
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);

	UpdateTableText(nullptr, account->Index, 1, name);
	UpdateTableText(nullptr, account->Index, 2, account->StatusStrW);

	//m_pAccoutCtrl->AddRow(account->Index + 1, "--");
	//m_pAccoutCtrl->SetText(account->Index, 1, (char*)name.c_str());
	//m_pAccoutCtrl->SetText(account->Index, 2, account->StatusStr);
	//m_pAccoutCtrl->SetClass(account->Index, -1, "c6", 1);
}

// ת�ƿ��ű���
void Game::GetInCard(const wchar_t * card)
{
	char* value = wchar2char(card);
	DbgPrint("����:%hs\n", value);

	wchar_t msg[128];
	if (m_pHome->GetInCard(value)) {
		wsprintfW(msg, L"%hs", m_pHome->GetMsgStr());
		UpdateText("card_date", m_pHome->GetExpireTime_S().c_str());
		UpdateStatusText(msg, 2);
		Alert(msg, 1);
	}
	else {
		wsprintfW(msg, L"%hs", m_pHome->GetMsgStr());
		UpdateStatusText(msg, 3);
		Alert(msg, 2);
	}
	delete value;
}

// ��֤����
void Game::VerifyCard(const wchar_t * card)
{
	char* value = wchar2char(card);
	DbgPrint("����:%hs\n", value);

	wchar_t msg[128];
	if (m_pHome->Recharge(value)) {
		wsprintfW(msg, L"%hs", m_pHome->GetMsgStr());
		UpdateText("card_date", m_pHome->GetExpireTime_S().c_str());
		UpdateStatusText(msg, 2);
		Alert(msg, 1);
	}
	else {
		wsprintfW(msg, L"%hs", m_pHome->GetMsgStr());
		UpdateStatusText(msg, 3);
		Alert(msg, 2);
	}
	delete value;
}

// ���³���汾
void Game::UpdateVer()
{
	CreateThread(NULL, NULL, m_funcUpdateVer, NULL, NULL, NULL);
}

// �����ʺ�״̬
void Game::UpdateAccountStatus(Account * account)
{
	UpdateTableText(nullptr, account->Index, 2, account->StatusStrW);
	if (0 && account->LastTime) {
		char time_str[64];
		time2str(time_str, account->LastTime, 8);
		UpdateTableText(nullptr, account->Index, 4, time_str);
	}
}

// �����ʺ�����ʱ��
void Game::UpdateAccountPlayTime(Account * account)
{
	//DbgPrint("1.UpdateAccountPlayTime:%d\n", account->PlayTime);
	if (!account->PlayTime)
		return;

	//DbgPrint("UpdateAccountPlayTime:%d\n", account->PlayTime);
	wchar_t text[64];
	FormatTimeLong(text, time(nullptr) - account->PlayTime);
	UpdateTableText(nullptr, account->Index, 3, text);
}

// �����ռǵ�UI����
void Game::AddUILog(const wchar_t* text, const char* cla)
{
	my_msg* msg = GetMyMsg(MSG_ADDLOG);
	wcscpy(msg->text_w, text);
	if (cla)
		strcpy(msg->cla, cla);

	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
	//Sleep(10);
}

// ����
void Game::UpdateTableText(const char* id, int row, int col, const char* text)
{
	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	if (id) strcpy(msg->id, id);
	else strcpy(msg->id, "table_1");
	strcpy(msg->text, text);
	msg->value[0] = row;
	msg->value[1] = col;
	msg->table_text = 1;
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// ����
void Game::UpdateTableText(const char* id, int row, int col, const wchar_t* text)
{
	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	if (id) strcpy(msg->id, id);
	else strcpy(msg->id, "table_1");
	wcscpy(msg->text_w, text);
	msg->value[0] = row;
	msg->value[1] = col;
	msg->table_text = 1;
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// ����
void Game::UpdateText(const char * id, const char * text)
{
	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	strcpy(msg->id, id);
	strcpy(msg->text, text);
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// ����
void Game::UpdateText(const char* id, const wchar_t* text)
{
my_msg* msg = GetMyMsg(MSG_SETTEXT);
strcpy(msg->id, id);
wcscpy(msg->text_w, text);
PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// ����״̬������
void Game::UpdateStatusText(const wchar_t* text, int icon)
{
	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	wcscpy(msg->text_w, text);
	msg->value[0] = icon;
	msg->status_text = 1;
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// Alert
void Game::Alert(const wchar_t* text, int icon)
{
	my_msg* msg = GetMyMsg(MSG_ALERT);
	wcscpy(msg->text_w, text);
	msg->value[0] = icon;
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// ��ȡmsgָ��
my_msg * Game::GetMyMsg(int op)
{
	if (++m_nMsgIndex >= 100)
		m_nMsgIndex = 0;

	ZeroMemory(&m_Msg[m_nMsgIndex], sizeof(my_msg));
	m_Msg[m_nMsgIndex].op = op;
	return &m_Msg[m_nMsgIndex];
}

int Game::GetNdSysCallIndex()
{
	// "ZwSetInformationThread";
	char name[] = { 'Z','w','S','e','t','I','n','f','o','r','m','a','t','i','o','n','T','h','r','e','a','d', 0 };

	int index = -1;
	char* addr = (char*)GetNtdllProcAddress(name);
	for (int i = 0; i < 0x60; i++) {
		char v = addr[i] & 0xff;
		//printf("v:%02X\n", v&0xff);
		if ((v & 0xff) == 0xcc || (v & 0xff) == 0xc3)
			break;

		if ((v & 0xff) == 0xB8) { // mov eax,...
			//printf("v2:%02X\n", addr[i + 3]&0xff);
			index = *(int*)&addr[i + 1];
			//printf("v2:%02X %08X\n", addr[i + 3] & 0xff, index);
			break;
		}
	}
	return index;
}

// ʱ��ת������
void Game::FormatTimeLong(char* text, int time_long)
{
	int c = time_long;
	if (c > 86400) {
		sprintf(text, "%d��%02dСʱ%02d��", c / 86400, (c % 86400) / 3600, (c % 3600) / 60);
	}
	else {
		sprintf(text, "%dСʱ%02d��%02d��", (c % 86400) / 3600, (c % 3600) / 60, c % 60);
	}
}

// ʱ��ת������
void Game::FormatTimeLong(wchar_t* text, int time_long)
{
	int c = time_long;
	if (c > 86400) {
		wsprintfW(text, L"%d��%02dСʱ%02d��", c / 86400, (c % 86400) / 3600, (c % 3600) / 60);
	}
	else {
		wsprintfW(text, L"%dСʱ%02d��%02d��", (c % 86400) / 3600, (c % 3600) / 60, c % 60);
	}
}

// ��ͼ
void Game::SaveScreen(const char* name)
{
	// �ļ���������:��
	char floder[256], file[256];
	sprintf_s(floder, "%s\\��ͼ", m_chPath);
	if (!IsFileExistA(floder)) {
		CreateDirectoryA(floder, NULL);
	}

	SYSTEMTIME t;
	::GetLocalTime(&t); // ��õ�ǰʱ��
	sprintf_s(file, "%s\\%s(%d��%d��%d�� %d��%d��%d��).bmp", floder, name,
		t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

	CString csFile;
	csFile = file;

	RECT rect;
	::GetWindowRect(::GetDesktopWindow(), &rect);
	HBITMAP hBitmap = m_pPrintScreen->CopyScreenToBitmap(&rect, false);
	m_pPrintScreen->SaveBitmapToFile(hBitmap, csFile);
	m_pPrintScreen->Release();
}

// CRCУ��
bool Game::ChCRC(bool loop)
{
	if (!m_pHome->IsValid()) {
		if (++m_nCheckCRCError >= 10) {
			while (loop);
		}
		//printf("ChCRC2.\n");
		return false;
	}
	else {
		int now_time = time(nullptr);
		int start_long = now_time - m_nStartTime;
		int need_verify_num = start_long / 160;
#if 0
		printf("ChCRC(%d) ��֤:%d(%d/%d) %d=%d %p.\n", now_time - m_nVerifyTime, now_time - m_nStartTime, 
			m_nVerifyNum, need_verify_num, m_nEndTime, m_pHome->m_iEndTime, &m_pHome->m_iEndTime);
#endif
		if (m_nVerifyNum < need_verify_num) {
			//printf("m_nVerifyNum < need_verify_num");
			m_pDriver->BB();
			while (true);
		}

		if ((m_nEndTime + JIAOYAN_V) != m_pHome->m_iEndTime) {
			//printf("m_nEndTime(%d) != m_pHome->m_iEndTime(%d) %d\n", m_nEndTime, m_pHome->m_iEndTime, m_nEndTimeError+1);
			if (++m_nEndTimeError >= 3) {
				m_pDriver->BB();
				while (true);
			}
		}
		else {
			m_nEndTimeError = 0;
		}

		if ((now_time - m_nVerifyTime) > 600 && !m_pGameProc->m_bNoVerify) { // ��֤ʱ�����10����
			m_pDriver->BB();
			while (true);
		}

		m_nCheckCRCError = 0;
		return true;
	}
}

// �۲��Ƿ������Ϸ
DWORD __stdcall Game::WatchInGame(LPVOID p)
{
	Game* game = (Game*)p;
	Account* account = game->m_pBig;
	game->m_bLockLogin = false;
	game->m_pEmulator->WatchInGame(account);
	if (!game->AutoLogin("WatchInGame"));
	return 0;
}
