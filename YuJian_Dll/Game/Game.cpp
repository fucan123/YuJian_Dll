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
#include <wincrypt.h>
#include <wintrust.h>
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

#pragma comment(lib, "version.lib")

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

	m_iOpenFBIndex =-1;

#if 0
	char log_file[255];
	sprintf_s(log_file, "%s\\日记.txt", m_chPath);
	m_LogFile.open(log_file, ofstream::out);
#endif;

	m_pHome = new Home(this);
	m_pGameConf = new GameConf(this);
	m_pEmulator = new Emulator(this);
	m_pMove = new Move(this);
	m_pItem = new Item(this);
	m_pMagic = new Magic(this);
	m_pTalk = new Talk(this);
	m_pPet = new Pet(this);
	m_pButton = new MCButton(this);

	Asm_Nd(GetCurrentThread(), GetNdSysCallIndex()); // 禁止反调试

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

	// 保护当前进程
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

// 监听
void Game::Listen(USHORT port)
{
	m_pGame->m_pServer->Listen(port);
}

void Game::Run()
{
#if ISCMD == 0
	return;
#endif
	//DbgPrint("TST:%d\n", m_pDriver->Test());
	//CheckGameOtherModule();
	//m_pGame->m_pGameProc->CheckDllSign();
	// 保护当前进程
	m_pDriver->SetHidePid(GetCurrentProcessId());

	//DbgPrint("Game::Run!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	if (!m_pGameProc->InitSteps()) {
		DbgPrint("初始化步骤失败，无法继续运行！！！\n");
		LOG2(L"初始化步骤失败，无法继续运行！！！", "red");
		return;
	}

	for (DWORD dwMs = 0; dwMs < 3000; dwMs += 500) {
		if (m_pBig)
			break;

		Sleep(500);
	}

	if (!m_pBig) {
		DbgPrint("请配置一个大号\n");
		system("pause");
		Alert(L"请配置一个大号", 2);
		return;
	}

	//DbgPrint("!m_pGame->m_pHome->IsValid()!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	while (!m_pGame->m_pHome->IsValid()) {
		DbgPrint("Run:未激活\n");
		Sleep(1000);
	}

	DbgPrint("try fs...\n");
	bool try_fs = true;
_try_fs_install_:
	if (m_pDriver->InstallFsFilter(m_chPath, "360SafeFsFlt.sys", "370030")) {
		DbgPrint("Install Protect Ok.\n");
		LOG2(L"Install Protect Ok.", "green b");
		if (m_pDriver->StartFsFilter()) {
			DbgPrint("Start Protect Ok.\n");
			LOG2(L"Start Protect Ok.", "green b");
		}
		else {
			if (try_fs) {
				try_fs = false;
				DbgPrint("Start Protect Failed.\n");
				LOG2(L"Start Protect Failed, Try Agin.", "red b");
#if 0
				system("sc stop DriverFs999");
				system("sc delete DriverFs999");
#else
				m_pGame->m_pDriver->Delete(L"360SafeFlt");
#endif
				goto _try_fs_install_;
			}
			else {
				DbgPrint("Start Protect Failed 请重启本程序再尝试.\n");
				LOG2(L"Start Protect Failed, 请重启本程序再尝试.", "red b");
				Alert(L"Start Protect Failed, 请重启本程序再尝试.", 2);
				system("pause");
				return;
			}
		}
	}
	else {
		DbgPrint("Install Protect Faild.\n");
		LOG2(L"Install Protect Faild.", "red b");
		system("pause");
		return;
	}

	//printf("!m_pEmulator->List2!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	//m_pEmulator->List2();
	int login_num = m_pGameData->WatchGame();
	if (!login_num) {
		DbgPrint("没有登录任何游戏.\n");
		Alert(L"没有任何登录游戏.", 2);
		system("pause");
		return;
	}

	m_pGameData->m_pAccountBig = m_pBig; // 大号
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
	m_pGameProc->SwitchGameAccount(m_pBig);
	if (m_pGameData->IsInShenDian(m_pBig) && 168) // 先离开神殿
		m_pGameProc->GoLeiMing();

#if IS_READ_MEM == 0
	m_pGame->m_pGameProc->Wait(5 * 1000);
#endif

	//m_pGame->m_pItem->DropItem();
	//m_pGame->m_pItem->CheckIn();
	//while (true) Sleep(168);

	if (m_pGameProc->IsInFB(m_pBig)) {
		//m_pGameProc->OutFB(m_pBig);
		m_pGameProc->Run(m_pBig);
	}
	else {
		m_pGameProc->GoFBDoor(m_pBig);

		while (true) {
			Account* open = m_pGameProc->OpenFB();
			if (!open) {
				LOG2(L"没有可以开启副本的帐号, 等待其他帐号登录...", "c6");
				m_pGameData->WatchGame();
				m_pGame->m_pGameProc->Wait(60 * 1000);
				break;
			}
			m_pGameProc->ViteInFB(open);
			m_pGameProc->AllInFB(open);

			break;
		}
#if 0
		if (GetAccountCount() > 1) {
			while (!m_pBig->IsReady && (!m_pServer->m_iCreateTeam || m_iLoginCount > 0)) Sleep(1000);
			if (!m_pBig->IsReady) {
				m_pServer->CanTeam(m_pBig, nullptr, 0);
				m_pServer->m_iCreateTeam = 0;
			}
		}
		else { // 只有配置大号
			m_pServer->CanInFB(m_pGame->m_pBig, nullptr, 0);
		}
		while (!IsAllReady()) Sleep(1000);
#endif

		m_pGameProc->Run(m_pBig);
	}
}

// 登录
void Game::Login(Account * p)
{
	if (!IsAutoLogin() || !p)
		return;
}

// 进入到登录界面
void Game::GoLoginUI(int left, int top)
{
	if (!IsAutoLogin())
		return;
}

// 退出
void Game::LogOut(Account * account)
{
	if (!account->Mnq)
		return;

	DbgPrint("\n退出登录.\n\n");
	LOG2(L"\n退出登录.\n", "red b");
	m_pEmulator->Tap(1206, 190, 1230, 215, account->Mnq->Index); // 右侧展开菜单
	Sleep(2000);
	m_pEmulator->Tap(1208, 645, 1230, 670, account->Mnq->Index); // 设置
	Sleep(2500);
	m_pEmulator->Tap(925, 575, 1035, 595, account->Mnq->Index);  // 退出登录
	Sleep(1500);
	m_pEmulator->Tap(705, 426, 785, 450, account->Mnq->Index);   // 确定
}

// 输入帐号密码登录
void Game::Input(Account* p)
{
	if (!IsAutoLogin())
		return;
}

// 自动登号
bool Game::AutoLogin(const char* remark)
{
	printf("AutoLogin:%hs\n", remark);
	wchar_t log[64];
	if (!IsAutoLogin())
		return false;

	LOGVARP2(log, "c6", L"AutoLogin:%hs\n", remark);
	if (m_iLoginCount == 0) { // 没有要登录的帐号
		LOG(L"没有要登录的帐号");
		LoginCompleted("没有要登录的帐号");
		return false;
	}

	if (m_iLoginFlag >= 0 && m_iLoginIndex != m_iLoginFlag) { // 只登一个账号
		LOG(L"只登一个账号");
		LoginCompleted("只登一个账号");
		return false;
	}

	int login_count = GetOnLineCount();
	if (login_count >= MAX_ACCOUNT_LOGIN) {
		::printf("已登录游戏账号数量:%d\n", GetOnLineCount());
		LOGVARP(log, L"已登录游戏账号数量:%d", GetOnLineCount());
		//::MessageBoxA(NULL, "已全部登入游戏", "提示", MB_OK);
		LoginCompleted("已全部登入游戏");
		return false;
	}
	if (login_count == 0 || (login_count == 1 && IsOnline(m_pBig))) // 没有人在线或只有大号在线
		m_pGame->m_iSendCreateTeam = 1;

#if 1
	Account* p = GetAccount(m_iLoginIndex);
	while (p && p->LockLogin) { // 不登陆的帐号
		p = GetAccount(++m_iLoginIndex);
	}
	if (!p) // 不存在
		return false;
	if (IsLogin(p) || (p->IsBig && m_Setting.LogoutByGetXL)) { // 已在登录或已登录 或大号不领项链
		m_iLoginIndex++;
		char tmp[128];
		sprintf_s(tmp, "%hs(Status:%08x,Big:%d,LogoutByGetXL:%d)", p->Name, p->Status, p->IsBig, m_Setting.LogoutByGetXL);
		return AutoLogin(tmp); // 登录下一个
	}

	p->Mnq = nullptr;
	p->LoginTime = time(nullptr);
	SetStatus(p, ACCSTA_READY);
	UpdateAccountStatus(p);
	m_iLoginIndex++;

	LOGVARN(64, L"启动游戏, 准备登录%hs[%d/%d]", p->Name, login_count + 1, m_iLoginCount);
	
	if (0 && p->IsBig) { // 大号登模拟器
		if (1 && !m_pEmulator->Open(p)) {
			LOGVARP(log, L"请确定是否安装了雷电模拟器或是否已被占用");
			::MessageBoxA(NULL, "请确定是否安装了雷电模拟器或是否已被占用", "提示", MB_OK);
			return false;
		}
		CreateThread(NULL, NULL, WatchInGame, this, NULL, NULL);
	}
	else { // 小号登客户端
		ShellExecuteA(NULL, "open", "AutoPatch.exe", NULL, m_Setting.GamePath, SW_SHOWNORMAL);
	}
#else
	for (int i = 0; i < m_iLoginCount; i++,m_iLoginIndex++) {
		Account* p = GetAccount(m_iLoginIndex);
		if (p == nullptr) // 不存在
			break;
		if (IsLogin(p) || (p->IsBig && m_Setting.LogoutByGetXL)) { // 已在登录或已登录 或大号不领项链
			continue;
		}
		if (!m_pEmulator->Open(p))
			continue;

		p->LoginTime = time(nullptr);
		SetStatus(p, ACCSTA_READY);
		UpdateAccountStatus(p);
		LOGVARN(64, "启动游戏, 准备登录%hs[%d/%d]", p->Name, login_count + 1, m_iLoginCount);

		m_bLockLogin = true;
		CreateThread(NULL, NULL, WatchInGame, this, NULL, NULL);
		Sleep(100);
		while (m_bLockLogin);
	}
#endif
	return true;
}

// 全部登完
void Game::LoginCompleted(const char* remark)
{
	m_iLoginIndex = 0;
	m_iLoginFlag = -2;
	m_iLoginCount = 0;
	LOGVARN2(64, "blue", L"已全部登入游戏(%hs)", remark);
}

// 设置登号类型
void Game::SetLoginFlag(int flag)
{
	if (!IsAutoLogin()) { // 已登录完成
		m_iLoginFlag = flag;
		m_iLoginCount = GetLoginCount();
	}
	m_iLoginIndex = flag >= 0 ? flag : 0;

	if (flag != -2) {
		LOGVARN2(32, "blue_r b", L"需要登录帐号数量:%d\n", m_iLoginCount);
	}
}

// 获取需要登录的数量
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
			if (max == count) // 登录数量达到最大
				break;
			count++;
		}
			
	}
	return count;
}

// 检查登录超时的帐号
int Game::CheckLoginTimeOut()
{
	int now_time = time(nullptr);

	char time_long_text[64];
	FormatTimeLong(time_long_text, now_time - m_nStartTime);
	//m_pJsCall->SetText("start_time", time_long_text);
		
	// 获取模拟器
	m_pEmulator->List2();

	// 定时关机
	if (!m_pGameProc->m_bAtFB && ClockShutDown(1))
		return 0;

	// 定时下线
	if (IsInTime(m_Setting.OffLine_SH, m_Setting.OffLine_SM, m_Setting.OffLine_EH, m_Setting.OffLine_EM)) {
		if (GetOnLineCount() > 0) {
			m_pEmulator->Close(0);
			m_pServer->SendToOther(0, SCK_CLOSE, true);
			return 0;
		}
	}
	// 定时登录
	if (IsInTime(m_Setting.AutoLogin_SH, m_Setting.AutoLogin_SM, m_Setting.AutoLogin_EH, m_Setting.AutoLogin_EM)) {
		if (GetOnLineCount() < MAX_ACCOUNT_LOGIN) {
			AutoPlay(-1, false);
			return 0;
		}
	}

	if ((now_time - m_nVerifyTime) > 300) {
		//printf("验证.\n");
		if (!m_pHome->Verify()) {
			if (++m_nVerifyError >= 6) {
				m_pGame->m_pGameProc->m_bNoVerify = true;
				m_pHome->SetExpire(0);
				m_nVerifyError = 0;

				//Alert(L"服务器验证超时！！！", 2);
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
				LOGVARN(64, L"%hs登录超时", p->Name);
				m_pServer->Send(p->Socket, SCK_CLOSE, true);
				count++;
			}
		}
		if (p->PlayTime && p->Status != ACCSTA_COMPLETED) {
			UpdateAccountPlayTime(p);
			Sleep(500);
		}
		if (0 && IsOnline(p) && p->LastTime) { // 在线
			m_pServer->Send(p->Socket, SCK_PING, true);
			if (m_Setting.TimeOut && (now_time - p->LastTime) > m_Setting.TimeOut) {
				LOGVARN(64, L"%hs已经%d秒没有响应", p->Name, now_time - p->LastTime);
				m_pServer->Send(p->Socket, SCK_CLOSE);
			}
		}
	}
	return count;
}


// 检查游戏进程ID是否存在
bool Game::CheckGamePid(DWORD pid)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (m_AccountList[i]->GamePid)
			return true;
	}
	return false;
}

// 设置游戏窗口
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

// 设置游戏进程ID
bool Game::SetGameAddr(Account* p, GameDataAddr* addr)
{
	memcpy(&p->Addr, addr, sizeof(GameDataAddr));
	p->IsGetAddr = 1;

	SetStatus(p, ACCSTA_ONLINE, true);
	return true;
}

// 设置状态-全部
void Game::SetAllStatus(int status)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		Account* p = m_AccountList[i];
		p->Status = status;
		GetStatusStr(p);
		UpdateAccountStatus(p);
	}
}

// 设置准备
void Game::SetReady(Account * p, int v)
{
	p->IsReady = v;
}

// 设置模拟器名称
void Game::SetMNQName(Account* p, char* name)
{
	wchar_t str[128];
	if (name) {
		if (p->IsBig) {
			wsprintfW(str, L"<b class = 'c3'>%hs[大号]</b> (<span class=\"cb fz12\">%hs</span>)", p->Name, name);
		}
		else {
			wsprintfW(str, L"<b class = 'c3'>%hs</b> (<span class=\"cb fz12\">%hs</span>)", p->Name, name);
		}
	}
	else {
		if (p->IsBig) {
			wsprintfW(str, L"<b class = 'c3'>%hs[大号]</b>", p->Name);
		}
		else {
			wsprintfW(str, L"<b class = 'c3'>%hs</b>", p->Name);
		}
	}

	UpdateTableText(nullptr, p->Index, 1, str);
}

// 设置状态
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

// 设置SOKCET
void Game::SetSocket(Account * p, SOCKET s)
{
	p->Socket = s;
}

// 设置Flag
void Game::SetFlag(Account * p, int flag)
{
	p->Flag = flag;
}

// 设置已完成
void Game::SetCompleted(Account* p)
{
	LOGVARN2(64, "green", L"%hs[%hs]已经完成", p->Name, p->Role);

	SetStatus(p, ACCSTA_COMPLETED);
	UpdateAccountStatus(p);
}

// 获取帐号总数
int Game::GetAccountCount()
{
	return m_AccountList.size();
}

// 获取正在刷副本帐号数量
int Game::GetAtFBCount()
{
	int count = 0;
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (m_AccountList[i]->Status == ACCSTA_ATFB)
			count++;
	}
	return count;
}

// 获取所有在线账号数量
int Game::GetOnLineCount()
{
	int count = 0;
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (IsOnline(m_AccountList[i]))
			count++;
	}
	return count;
}

// 帐号是否已在列表
Account* Game::GetAccount(const char* name)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (strcmp(name, m_AccountList[i]->Name) == 0)
			return m_AccountList[i];
	}
	return nullptr;
}

// 获取帐号
Account * Game::GetAccount(int index)
{
	if (index < 0)
		return nullptr;

	return index < m_AccountList.size() ? m_AccountList[index] : nullptr;
}

// 获取帐号
Account * Game::GetAccountByRole(const char* role)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (strcmp(role, m_AccountList[i]->Role) == 0)
			return m_AccountList[i];
	}
	return nullptr;
}

// 获取帐号[根据状态]
Account * Game::GetAccountByStatus(int status)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (m_AccountList[i]->Status & status) {
			LOGVARN(64, L"获取帐号:%hs[%08X|%08X]", m_AccountList[i]->Name, m_AccountList[i]->Status, status);
			return m_AccountList[i];
		}
	}
	return nullptr;
}

// 获取最多项链帐号 last=最后比较的帐号
Account* Game::GetMaxXLAccount(Account** last)
{
	if (last) *last = nullptr;

	Account* p = nullptr;
	int value = 0;
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (m_AccountList[i]->IsBig || !IsLogin(m_AccountList[i])) // 没有在线
			continue;

		if (i != m_iOpenFBIndex && m_AccountList[i]->XL > value) { // 上次开启的优先级排在后面
			p = m_AccountList[i];
			value = p->XL;
			DbgPrint("%hs有项链%d\n", p->Name, p->XL);
		}
		if (last && !m_AccountList[i]->IsBig) // 保存最后比较帐号
			*last = m_AccountList[i];
	}
	if (!p && m_iOpenFBIndex > -1) { // 没有找到, 尝试上一个的
		p = m_AccountList[m_iOpenFBIndex];
		if (p && p->XL == 0) // 上一个的没有项链了
			p = nullptr;
	}
	if (p) {
		m_iOpenFBIndex = p->Index; // 保存开启副本的帐号索引
	}
	return p;
}

// 获取正在登录的帐号
Account* Game::GetReadyAccount(bool nobig)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (nobig && m_AccountList[i]->IsBig) // 不包括大号
			continue;
		if (m_AccountList[i]->Status == ACCSTA_READY)
			return m_AccountList[i];
	}
	return nullptr;
}

// 获取下一个要登录的帐号
Account* Game::GetNextLoginAccount()
{
	if (0 && !m_Setting.AutoLoginNext)
		return nullptr;

	DbgPrint("GetAccountByStatus(ACCSTA_INIT | ACCSTA_OFFLINE)\n");
	Account* p = GetAccountByStatus(ACCSTA_INIT | ACCSTA_OFFLINE);
	DbgPrint("GetAccountByStatus(ACCSTA_INIT | ACCSTA_OFFLINE) 完成\n");
	if (!p) {
		::printf("for (int i = 0; i < m_AccountList.size(); i++) \n");
		for (int i = 0; i < m_AccountList.size(); i++) {
			if (CheckStatus(m_AccountList[i], ACCSTA_COMPLETED)) { // 完成的
				tm t;
				time_t play_time = m_AccountList[i]->PlayTime;
				gmtime_s(&t, &play_time);

				tm t2;
				time_t now_time = time(nullptr);
				gmtime_s(&t2, &now_time);

				if (t.tm_mday != t2.tm_mday) { // 帐号是昨天或更早登录的, 可以再次登录
					DbgPrint(" 更新帐号状态%d %d %d,%d\n", t.tm_mday, t2.tm_mday, (int)play_time, (int)now_time);
					p = m_AccountList[i];
					SetStatus(p, ACCSTA_INIT); // 更新帐号状态
					break;
				}
			}
		}
		DbgPrint("for (int i = 0; i < m_AccountList.size(); i++)  完成\n");
	}

	return p;
}

// 获取帐号
Account* Game::GetAccountBySocket(SOCKET s)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (m_AccountList[i]->Socket == s)
			return m_AccountList[i];
	}
	return nullptr;
}

// 获取大号帐号
Account* Game::GetBigAccount()
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (m_AccountList[i]->IsBig)
			return m_AccountList[i];
	}
	return nullptr;
}

// 获取大号SOCKET
SOCKET Game::GetBigSocket()
{
	Account* p = GetBigAccount();
	return p ? p->Socket : 0;
}

// 设置已经入队
void Game::SetInTeam(int index)
{
	Account* p = GetAccount(index);
	if (p) {
		m_pServer->CanInFB(p, nullptr, 0);
	}
}

// 关闭游戏
void Game::CloseGame(int index)
{
	Account* p = GetAccount(index);
	if (p) {
		if (p->IsBig) {
			if (p->Mnq) {
				DbgPrint("关闭模拟器:%hs\n", p->Mnq->Name);
				LOGVARN2(64, "red", L"关闭模拟器:%hs\n", p->Mnq->Name);
				m_pEmulator->Close(p->Mnq->Index);
			}
		}
		else {
			p->OfflineLogin = 0;
			m_pServer->Send(p->Socket, SCK_CLOSE, true);
		}
	}
}

// 设置已准备好帐号数量
void Game::SetReadyCount(int v)
{
	m_iReadyCount = v;
}

// 设置已准备好帐号数量
int Game::AddReadyCount(int add)
{
	m_iReadyCount += add;
	return m_iReadyCount;
}

// 对比帐号状态
bool Game::CheckStatus(Account* p, int status)
{
	return p ? (p->Status & status) : false;
}

// 是否自动登录
bool Game::IsAutoLogin()
{
	return m_iLoginFlag != -2;
}

// 是否已全部准备好
bool Game::IsAllReady()
{
	int count = 0;
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (m_AccountList[i]->IsReady)
			count++;
	}

	return count >= 5 || count >= m_AccountList.size();
}

// 帐号是否在登录
bool Game::IsLogin(Account * p)
{
	return CheckStatus(p, 0xff);
}

// 帐号是否在线
bool Game::IsOnline(Account * p)
{
	return CheckStatus(p, 0xff);
}

// 获取状态字符
char* Game::GetStatusStr(Account* p)
{
	if (p->Status == ACCSTA_INIT) {
		wcscpy(p->StatusStrW, L"未登录");
		return strcpy(p->StatusStr, "未登录");
	}
		
	if (p->Status == ACCSTA_READY) {
		wcscpy(p->StatusStrW, L"正在打开游戏...");
		return strcpy(p->StatusStr, "正在打开游戏...");
	}
		
	if (p->Status == ACCSTA_LOGIN) {
		wcscpy(p->StatusStrW, L"正在登录...");
		return strcpy(p->StatusStr, "正在登录...");
	}
		
	if (p->Status == ACCSTA_ONLINE) {
		wcscpy(p->StatusStrW, L"在线");
		return strcpy(p->StatusStr, "在线");
	}
		
	if (p->Status == ACCSTA_OPENFB) {
		wcscpy(p->StatusStrW, L"正在开启副本...");
		return strcpy(p->StatusStr, "正在开启副本...");
	}
		
	if (p->Status == ACCSTA_ATFB) {
		wcscpy(p->StatusStrW, L"正在刷副本...");
		return strcpy(p->StatusStr, "正在刷副本...");
	}
		
	if (p->Status == ACCSTA_COMPLETED) {
		wcscpy(p->StatusStrW, L"已完成");
		return strcpy(p->StatusStr, "已完成");
	}
		
	if (p->Status == ACCSTA_OFFLINE) {
		wcscpy(p->StatusStrW, L"离线");
		return strcpy(p->StatusStr, "离线");
	}
		
	return p->Name;
}

// 发送卡住重启时间
int Game::SendQiaZhuS(int second)
{
	if (second > 0) {
		return SendToBig(SCK_QIAZHUS, second, true);
	}
	return 0;
}

// 发送给大号
int Game::SendToBig(SOCKET_OPCODE opcode, bool clear)
{
	return m_pServer->Send(GetBigSocket(), opcode, clear);
}

// 发送给大号
int Game::SendToBig(SOCKET_OPCODE opcode, int v, bool clear)
{
	if (clear)
		m_pServer->m_Server.ClearSendString();

	m_pServer->m_Server.SetInt(v);
	return SendToBig(opcode, false);
}

// 检查数据库
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

// 更新物品信息
void Game::UpdateDBItem(const char* account, const char* item_name)
{
	int now_time = time(nullptr);

	char sql[255];
	sprintf_s(sql, "INSERT INTO items(account,name,created_at) VALUES ('%hs','%hs',%d)",
		account, item_name, now_time);
	m_pSqlite->Exec(sql);

	sprintf_s(sql, "SELECT num FROM item_count WHERE name='%hs'", item_name);
	if (m_pSqlite->GetRowCount(sql) == 0) { // 没有
		::printf("增加\n");
		sprintf_s(sql, "INSERT INTO item_count(name,num,created_at,updated_at) VALUES ('%hs',%d,%d,%d)", 
			item_name, 1, now_time, now_time);
		m_pSqlite->Exec(sql);
	}
	else { // 更新
		::printf("更新\n");
		sprintf_s(sql, "UPDATE item_count SET num=num+1,updated_at=%d WHERE name='%hs'",
			now_time, item_name);
		m_pSqlite->Exec(sql);
	}
}

// 更新刷副本次数
void Game::UpdateDBFB(int count)
{
	if (m_pSqlite->GetRowCount("SELECT num FROM fb_count WHERE id=1") == 0) { // 没有
		::printf("增加\n");
		m_pSqlite->Exec("INSERT INTO fb_count(id,num) VALUES (1,1)");
	}
	else { // 更新
		::printf("更新\n");
		m_pSqlite->Exec("UPDATE fb_count SET num=num+1 WHERE id=1");
	}
}

// 更新刷副本时长
void Game::UpdateDBFBTimeLong(int time_long)
{
	char sql[128];
	if (m_pSqlite->GetRowCount("SELECT time_long FROM fb_time_long WHERE id=1") == 0) { // 没有
		sprintf_s(sql, "INSERT INTO fb_time_long(id,time_long) VALUES (1,%d)", time_long);
		m_pSqlite->Exec(sql);
	}
	else { // 更新
		sprintf_s(sql, "UPDATE fb_time_long SET time_long=time_long+%d WHERE id=1", time_long);
		m_pSqlite->Exec(sql);
	}
}

// 插入副本记录 status=0正常通关 1-复活重开 2-卡住超时
void Game::InsertFBRecord(int start_time, int end_time, int status)
{
	char sql[128];
	sprintf_s(sql, "INSERT INTO fb_record(start_time,end_time,time_long,status) VALUES (%d,%d,%d,%d)", 
		start_time, end_time, end_time-start_time, status);
	m_pSqlite->Exec(sql);
}

// 查询副本记录
int Game::SelectFBRecord(char*** result, int* col)
{
	return m_pSqlite->SelectAll("SELECT * FROM fb_record order by id desc", result, col);
}

// 获取教学数量
int Game::GetfbCount()
{
	char num[16] = { '0', 0 };
	m_pSqlite->GetOneCol("SELECT num FROM fb_count WHERE id=1", num);
	return atoi(num);
}

// 更新重开副本次数
void Game::UpdateReOpenFBCount(int count)
{
#if ISCMD
	return;
#endif

	char text[16];
	sprintf_s(text, "%d",count);

	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	strcpy(msg->id, "fb_reopen_count");
	strcpy(msg->text, text);

	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// 更新刷副本次数文本
void Game::UpdateFBCountText(int lx, bool add)
{
	if (add) {
		UpdateDBFB();
	}

#if ISCMD
	return;
#endif
	
	char num[16] = { '0', 0 };
	m_pSqlite->GetOneCol("SELECT num FROM fb_count WHERE id=1", num);

	char text[32];
	sprintf_s(text, "%d/%hs", lx, num);

	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	strcpy(msg->id, "fb_count");
	strcpy(msg->text, text);
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// 更新刷副本时长文本
void Game::UpdateFBTimeLongText(int time_long, int add_time_long)
{
#if ISCMD
	if (add_time_long > 0)
		UpdateDBFBTimeLong(add_time_long);
	return;
#endif

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

// 更新掉线复活次数文本
void Game::UpdateOffLineAllText(int offline, int reborn)
{
#if ISCMD
	return;
#endif

	char text[32];
	sprintf_s(text, "%d/%d", offline, reborn);
	
	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	strcpy(msg->id, "offline_count");
	strcpy(msg->text, text);

	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// 写入日记
void Game::WriteLog(const char* log)
{
	return;
	m_LogFile << log << endl;
}

// 读取配置文件
DWORD Game::ReadConf()
{
	char path[255], logfile[255];

	strcpy(path, m_chPath);
	strcpy(logfile, m_chPath);
	strcat(path, "\\帐号.txt");
	strcat(logfile, "\\日记.txt");

	Asm_Nd(GetCurrentThread(), GetNdSysCallIndex()); // 禁止反调试

	::printf("帐号文件:%hs\n", path);
	OpenTextFile file;
	if (!file.Open(path)) {
		::printf("找不到'帐号.txt'文件！！！");
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
		if (strstr(data, "::") == nullptr && strstr(data, "=")) { // 其它配置
			if (!ReadAccountSetting(data)) {
				ReadSetting(data);
			}
			
			continue;
		}

		// 帐号列表
		Explode explode("::", data);
		if (explode.GetCount() < 2)
			continue;

		Account* p = GetAccount(explode[0]);
		if (!p) { // 不存在
			p = new Account;
			ZeroMemory(p, sizeof(Account));

			strcpy(p->Name, explode[0]);
			strcpy(p->Password, explode[1]);
			strcpy(p->Role,    "--");
			p->RoleNo = explode.GetValue2Int(2);
			p->IsBig = explode.GetCount() > 3 && strcmp(explode[3], "大号") == 0;
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

#if ISCMD == 0
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)GetMyMsg(MSG_FILLTABLE), 0);
#endif

	return 0;
}

// 读取帐号设置
bool Game::ReadAccountSetting(const char* data)
{
	Explode explode("=", data);
	Account* p = GetAccount(explode[0]);
	if (!p)
		return false;

	Explode arr("|", explode[1]);
	if (arr.GetCount() == 2) {
		if (strcmp(arr[1], "角色") == 0) {
			strcpy(p->Role, arr[0]);
			LOGVARN2(64, "c0", L"角色.%hs: %hs", explode[0], p->Role);
		}
		else {
			strcpy(p->SerBig, arr[0]);
			strcpy(p->SerSmall, arr[1]);
			LOGVARN2(64, "cb", L"区服.%hs: %hs|%hs", explode[0], arr[0], arr[1]);
		}
	}

	return true;
}

// 读取其它设置
void Game::ReadSetting(const char* data)
{
	char tmp[16] = { 0 };
	Explode explode("=", data);
	if (strcmp(explode[0], "无限.") == 0) {
		if (strcmp(explode[1], "无限期+永久") == 0)
			m_pHome->SetFree(true);
	}
	else if (strcmp(explode[0], "模拟器路径") == 0) {
		strcpy(m_Setting.MnqPath, explode[1]);
		m_pEmulator->SetPath(m_Setting.MnqPath);
	}
	else if (strcmp(explode[0], "游戏路径") == 0) {
		strcpy(m_Setting.GamePath, explode[1]);
	}
	else if (strcmp(explode[0], "游戏文件") == 0) {
		strcpy(m_Setting.GameFile, explode[1]);
	}
	else if (strcmp(explode[0], "游戏大区") == 0) {
		strcpy(m_Setting.SerBig, explode[1]);
	}
	else if (strcmp(explode[0], "游戏小区") == 0) {
		strcpy(m_Setting.SerSmall, explode[1]);
	}
	else if (strcmp(explode[0], "启动超时") == 0) {
		m_Setting.InitTimeOut = explode.GetValue2Int(1);
		strcpy(tmp, "秒");
		SetSetting("init_timeout", m_Setting.InitTimeOut);
	}
	else if (strcmp(explode[0], "登录超时") == 0) {
		m_Setting.LoginTimeOut = explode.GetValue2Int(1);
		strcpy(tmp, "秒");
		SetSetting("login_timeout", m_Setting.LoginTimeOut);
	}
	else if (strcmp(explode[0], "游戏超时") == 0) {
		m_Setting.TimeOut = explode.GetValue2Int(1);
		strcpy(tmp, "秒");
	}
	else if (strcmp(explode[0], "副本超时") == 0) {
		m_Setting.FBTimeOut = explode.GetValue2Int(1);
		SetSetting("fb_timeout", m_Setting.FBTimeOut);
		m_Setting.FBTimeOut *= 60;
		strcpy(tmp, "分钟");
	}
	else if (strcmp(explode[0], "副本时限") == 0) {
		m_Setting.FBTimeOutErvry = explode.GetValue2Int(1);
		SetSetting("fb_timeout_ervry", m_Setting.FBTimeOutErvry);
		m_Setting.FBTimeOutErvry *= 60;
		strcpy(tmp, "分钟");
	}
	else if (strcmp(explode[0], "断线重连") == 0) {
		m_Setting.ReConnect = strcmp("是", explode[1]) == 0;
	}
	else if (strcmp(explode[0], "自动切换帐号") == 0) {
		m_Setting.AutoLoginNext = strcmp("是", explode[1]) == 0;
	}
	else if (strcmp(explode[0], "刷完关机") == 0) {
		m_Setting.ShutDownNoXL = strcmp("是", explode[1]) == 0;
		SetSetting("shuawan_shutdown", m_Setting.ShutDownNoXL);
	}
	else if (strcmp(explode[0], "定时关机") == 0) {
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
	else if (strcmp(explode[0], "定时下线") == 0) {
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
	else if (strcmp(explode[0], "定时登录") == 0) {
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

	//printf("配置.%s: %s%s\n", explode[0], explode[1], tmp);
	//LOGVARN2(64, "cb", L"配置.%hs: %hs%hs", explode[0], explode[1], tmp);
	LOGVARN2(64, "cb", L"配置.%hs: %hs%hs", explode[0], explode[1], tmp);
}

// 自动关机
void Game::AutoShutDown()
{
	if (m_Setting.ShutDownNoXL)
		ShutDown();
}

// 定时关机
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

// 关机
void Game::ShutDown()
{
	SaveScreen("关机");
	system("shutdown -s -t 10");
}

// 当前时间是否在此时间
bool Game::IsInTime(int s_hour, int s_minute, int e_hour, int e_minute)
{
	SYSTEMTIME stLocal;
	::GetLocalTime(&stLocal); // 获得当前时间
	if (s_hour == e_hour) { // 时间是同一小时
		return stLocal.wHour == s_hour
			&& stLocal.wMinute >= s_minute && stLocal.wMinute <= e_minute;
	}
	if (s_hour < e_hour) { // 不是同一小时
		if (stLocal.wHour == s_hour) // 在开始小时时间
			return stLocal.wMinute >= s_minute;
		if (stLocal.wHour == e_hour) // 在结束小时时间
			return stLocal.wMinute <= e_minute;
		if (stLocal.wHour > s_hour && stLocal.wHour < e_hour) // 在中间
			return true;
	}
	return false;
}

// 推送到html界面
void Game::SetSetting(const char* name, int v)
{
#if ISCMD
	return;
#endif

	my_msg* msg = GetMyMsg(MSG_SETSETTING);
	strcpy(msg->id, name);
	msg->value[0] = v;
	msg->value[1] = 0;
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// 设置
void Game::PutSetting(const wchar_t* name, int v)
{
	if (wcscmp(name, L"fb_mode") == 0) {
		m_Setting.FBMode = v;
		m_Setting.LogoutByGetXL = 0;
		m_Setting.NoGetXL = 0;
		m_Setting.NoPlayFB = 0;

		char str[16];
	    if (v == 1) strcpy(str, "边领边刷");
		else if (v == 2) {
			strcpy(str, "不领只刷");
			m_Setting.NoGetXL = 1;
		}
		else if (v == 3) {
			strcpy(str, "先领后刷");
			m_Setting.LogoutByGetXL = 1;
			m_Setting.NoPlayFB = 0;
		}
		else if (v == 4) {
			strcpy(str, "只领不刷");
			m_Setting.LogoutByGetXL = 1;
			m_Setting.NoPlayFB = 1;
		}
		else strcpy(str, "未知");

		LOGVARN2(64, "cb", L"修改.副本模式:%hs.", str);
	}
	else if (wcscmp(name, L"close_mnq") == 0) {
		m_Setting.CloseMnq = v;
		LOGVARN2(64, "cb", L"修改.关闭模拟器:%hs.", v ? "是" : "否");
	}
	else if (wcscmp(name, L"login_timeout") == 0) {
		m_Setting.LoginTimeOut = v;
		LOGVARN2(64, "cb", L"修改.登录超时:%d秒.", v);
	}
	else if (wcscmp(name, L"fb_timeout") == 0) {
		m_Setting.FBTimeOut = v * 60;
		LOGVARN2(64, "cb", L"修改.副本超时:%d分钟.", v);
	}
	else if (wcscmp(name, L"qiazhu") == 0) {
		SendQiaZhuS(v);
		LOGVARN2(64, "cb", L"修改.卡住重启:%d秒.", v);
	}
	else if (wcscmp(name, L"shuawan_shutdown") == 0) {
		m_Setting.ShutDownNoXL = v;
		LOGVARN2(64, "cb", L"修改.刷完关机:%hs.", v ? "是" : "否");
	}
	/******** 定时关机 ********/
	else if (wcscmp(name, L"shutdown_sh") == 0) {
		m_Setting.ShutDown_SH = v;
		LOGVARN2(64, "cb", L"修改.定时关机(开始):%d点.", v);
	}
	else if (wcscmp(name, L"shutdown_sm") == 0) {
		m_Setting.ShutDown_SM = v;
		LOGVARN2(64, "cb", L"修改.定时关机(开始):%d分.", v);
	}
	else if (wcscmp(name, L"shutdown_eh") == 0) {
		m_Setting.ShutDown_EH = v;
		LOGVARN2(64, "cb", L"修改.定时关机(结束):%d点.", v);
	}
	else if (wcscmp(name, L"shutdown_em") == 0) {
		m_Setting.ShutDown_EM = v;
		LOGVARN2(64, "cb", L"修改.定时关机(结束):%d分.", v);
	}
	/******** 定时下线 ********/
	else if (wcscmp(name, L"offline_sh") == 0) {
		m_Setting.OffLine_SH = v;
		LOGVARN2(64, "cb", L"修改.定时下线(开始):%d点.", v);
	}
	else if (wcscmp(name, L"offline_sm") == 0) {
		m_Setting.OffLine_SM = v;
		LOGVARN2(64, "cb", L"修改.定时下线(开始):%d分.", v);
	}
	else if (wcscmp(name, L"offline_eh") == 0) {
		m_Setting.OffLine_EH = v;
		LOGVARN2(64, "cb", L"修改.定时下线(结束):%d点.", v);
	}
	else if (wcscmp(name, L"offline_em") == 0) {
		m_Setting.OffLine_EM = v;
		LOGVARN2(64, "cb", L"修改.定时下线(结束):%d分.", v);
	}
	/******** 定时登录 ********/
	else if (wcscmp(name, L"autologin_sh") == 0) {
		m_Setting.AutoLogin_SH = v;
		LOGVARN2(64, "cb", L"修改.定时登录(开始):%d点.", v);
	}
	else if (wcscmp(name, L"autologin_sm") == 0) {
		m_Setting.AutoLogin_SM = v;
		LOGVARN2(64, "cb", L"修改.定时登录(开始):%d分.", v);
	}
	else if (wcscmp(name, L"autologin_eh") == 0) {
		m_Setting.AutoLogin_EH = v;
		LOGVARN2(64, "cb", L"修改.定时登录(结束):%d点.", v);
	}
	else if (wcscmp(name, L"autologin_em") == 0) {
		m_Setting.AutoLogin_EM = v;
		LOGVARN2(64, "cb", L"修改.定时登录(结束):%d分.", v);
	}

	else if (wcscmp(name, L"no_auto_select") == 0) {
		m_Setting.NoAutoSelect = v;
		LOGVARN2(64, "cb", L"修改.手动选区:%hs.", v ? "是" : "否");
	}
	else if (wcscmp(name, L"is_debug") == 0) {
		m_Setting.IsDebug = v;
		LOGVARN2(64, "cb", L"修改.调试信息:%hs.", v ? "显示" : "不显示");
	}
	else if (wcscmp(name, L"talk_open") == 0) {
		m_Setting.TalkOpen = v;
		LOGVARN2(64, "cb", L"修改.喊话开启:%hs.", v ? "是" : "否");
	}
	else if (wcsstr(name, L"account_locklogin")) {
		Account* account = GetAccount(_wtoi(name + 18));
		if (account && !account->IsBig) {
			account->LockLogin = v;
			LOGVARN2(64, "cb", L"修改.帐号(%hs)%hs.", account->Name, v ? "不登陆" : "登陆");
		}
	}
}

// 设置
void Game::PutSetting(wchar_t* name, wchar_t* v)
{
	if (!v || wcslen(v) == 0)
		return;

	char* value = wchar2char(v);
	delete value;
}

// 打开游戏
int Game::OpenGame(int index, int close_all)
{
	if (close_all && m_Setting.CloseMnq)
		m_pEmulator->CloseAll();
	if (m_Setting.FBMode < 1 || m_Setting.FBMode > 4)
		m_Setting.FBMode = 1;

	return AutoPlay(index, false);
}

// 喊话
// 打开游戏
void Game::CallTalk(const char* text, int type)
{
	m_pServer->m_Server.ClearSendString();
	m_pServer->m_Server.SetInt(type);
	m_pServer->m_Server.SetInt(strlen(text));
	m_pServer->m_Server.SetContent((void*)text, strlen(text));
	m_pServer->SendToOther(0, SCK_TALK, false);
}

// 注入DLL
int Game::InstallDll()
{
	if (!m_pHome->IsValid() && !m_pDriver->m_bIsInstallDll) {
		//m_pJsCall->ShowMsg("无法启动, 请先激活.", "提示", 2);
		return -1;
	}
	//m_pJsCall->SetBtnDisabled("start_btn", 1);
	return m_pDriver->InstallDll(m_chPath) ? 1 : 0;
}

// 自动登号
int Game::AutoPlay(int index, bool stop)
{
	//printf("AutoPlay\n");
	//m_pJsCall->SetBtnDisabled("start_btn", 1);
	if (stop) {
		SetLoginFlag(-2);
		//m_pJsCall->SetText("auto_play_btn", "自动登号");
		//m_pJsCall->SetBtnDisabled("start_btn", 0);
		return 1;
	}
	if (IsAutoLogin())
		return 0;

	SetLoginFlag(index);
	if (!AutoLogin("AutoPlay")) {
		LoginCompleted("自动登号");
	}

	return 1;
} 

// 添加帐号
void Game::AddAccount(Account * account)
{
#if ISCMD
	return;
#endif

	CString  name = L"<b class='c3'>";
	name += account->Name;
	if (account->IsBig) {
		name += L"[大号]";
	}
	name += L"</b>";;

	//wsprintfW(text, L"%hs", name.GetBuffer());
	my_msg* msg = GetMyMsg(MSG_ADDTABLEROW);
	strcpy(msg->id, "table_1");
	strcpy(msg->text, "--");
	msg->value[0] = account->Index + 1;
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);

	UpdateTableText(nullptr, account->Index, 1, name);
	UpdateTableText(nullptr, account->Index, 2, account->StatusStrW);
}

// 转移卡号本机
void Game::GetInCard(const wchar_t * card)
{
#if ISCMD
	return;
#endif

	char* value = wchar2char(card);
	DbgPrint("卡号:%hs\n", value);

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

// 验证卡号
void Game::VerifyCard(const wchar_t* card, const wchar_t* remark)
{
#if ISCMD
	return;
#endif

	char* value = wchar2char(card);
	char* value2 = wchar2char(remark);
	//printf("卡号:%hs : %hs\n", value, value2);

	wchar_t msg[128];
	if (m_pHome->Recharge(value, value2)) {
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

// 更新程序版本
void Game::UpdateVer()
{
	CreateThread(NULL, NULL, m_funcUpdateVer, NULL, NULL, NULL);
}

// 检查是否有其他模块
bool Game::CheckGameOtherModule()
{
	HANDLE hProcess = GetCurrentProcess();
	if (!hProcess) { // 打开目标进程，获得句柄
		return false;
	}

	DWORD dwLen = 0;
	HMODULE hMods[256];
	EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &dwLen, LIST_MODULES_ALL); //  LIST_MODULES_ALL
	dwLen /= sizeof(HMODULE);
	//printf("len:%d %d\n", dwLen, GetLastError());
	if (dwLen > 256)
		dwLen = 256;

	bool result = true;
	for (DWORD i = 0; i < dwLen; i++) {
		CHAR name[128] = { 0 };
		wchar_t name_w[128] = { 0 };
		GetModuleFileNameA(hMods[i], name, sizeof(name));
		GetModuleFileNameW(hMods[i], name_w, sizeof(name_w));

		DWORD dwHandle, dwLen;
		UINT BufLen;
		LPTSTR lpData;
		VS_FIXEDFILEINFO *pFileInfo;
		dwLen = GetFileVersionInfoSize(name_w, &dwHandle);
		if (!dwLen) {
			DbgPrint("%s dwLen=%d\n", name, dwLen);
			continue;
		}

		lpData = (LPTSTR)malloc(dwLen);
		if (!GetFileVersionInfoA(name, dwHandle, dwLen, lpData))
		{
			free(lpData);
			DbgPrint("%s GetFileVersionInfoA No\n", name);
		}
		if (VerQueryValueA(lpData, "\\", (LPVOID *)&pFileInfo, (PUINT)&BufLen))
		{
			DbgPrint("%s:%08X %d\n", name, pFileInfo->dwSignature, pFileInfo->dwProductVersionMS);
			free(lpData);
		}

		CharLowerA(name);
		if (strstr(name, "9星")) {
			//printf("-----------------------\n");
			CHAR name_short[128] = { 0 };
			GetModuleBaseNameA(hProcess, hMods[i], name_short, sizeof(name_short));
			if (strcmp(name_short, "点我启动.exe")
				&& strcmp(name_short, "miniblink_x64.dll")
				&& strcmp(name_short, "web.dll")) {
				//::MessageBoxA(NULL, name, "t", MB_OK);
				result = false;
				//break;
			}
		}
		//printf("%s\n", name);
	}
	//printf("验证结果:%d\n", result);

	CloseHandle(hProcess);

	return result;
}

// 更新帐号状态
void Game::UpdateAccountStatus(Account * account)
{
#if ISCMD
	return;
#endif

	UpdateTableText(nullptr, account->Index, 2, account->StatusStrW);
	if (0 && account->LastTime) {
		char time_str[64];
		time2str(time_str, account->LastTime, 8);
		UpdateTableText(nullptr, account->Index, 4, time_str);
	}
}

// 更新帐号在线时长
void Game::UpdateAccountPlayTime(Account * account)
{
#if ISCMD
	return;
#endif

	//DbgPrint("1.UpdateAccountPlayTime:%d\n", account->PlayTime);
	if (!account->PlayTime)
		return;

	//DbgPrint("UpdateAccountPlayTime:%d\n", account->PlayTime);
	wchar_t text[64];
	FormatTimeLong(text, time(nullptr) - account->PlayTime);
	UpdateTableText(nullptr, account->Index, 3, text);
}

// 输入日记到UI界面
void Game::AddUILog(const wchar_t* text, const char* cla)
{
#if ISCMD
	return;
#endif

	my_msg* msg = GetMyMsg(MSG_ADDLOG);
	wcscpy(msg->text_w, text);
	if (cla)
		strcpy(msg->cla, cla);

	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
	//Sleep(10);
}

// 更新
void Game::UpdateTableText(const char* id, int row, int col, const char* text)
{
#if ISCMD
	return;
#endif

	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	if (id) strcpy(msg->id, id);
	else strcpy(msg->id, "table_1");
	strcpy(msg->text, text);
	msg->value[0] = row;
	msg->value[1] = col;
	msg->table_text = 1;
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// 更新
void Game::UpdateTableText(const char* id, int row, int col, const wchar_t* text)
{
#if ISCMD
	return;
#endif

	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	if (id) strcpy(msg->id, id);
	else strcpy(msg->id, "table_1");
	wcscpy(msg->text_w, text);
	msg->value[0] = row;
	msg->value[1] = col;
	msg->table_text = 1;
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// 更新
void Game::UpdateText(const char * id, const char * text)
{
#if ISCMD
	return;
#endif

	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	strcpy(msg->id, id);
	strcpy(msg->text, text);
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// 更新
void Game::UpdateText(const char* id, const wchar_t* text)
{
#if ISCMD
	return;
#endif

	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	strcpy(msg->id, id);
	wcscpy(msg->text_w, text);
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// 更新状态栏文字
void Game::UpdateStatusText(const wchar_t* text, int icon)
{
#if ISCMD
	return;
#endif

	my_msg* msg = GetMyMsg(MSG_SETTEXT);
	wcscpy(msg->text_w, text);
	msg->value[0] = icon;
	msg->status_text = 1;
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// Alert
void Game::Alert(const wchar_t* text, int icon)
{
#if ISCMD
	return;
#endif

	my_msg* msg = GetMyMsg(MSG_ALERT);
	wcscpy(msg->text_w, text);
	msg->value[0] = icon;
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// 获取msg指针
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

// 时长转成文字
void Game::FormatTimeLong(char* text, int time_long)
{
	int c = time_long;
	if (c > 86400) {
		sprintf(text, "%d天%02d小时%02d分", c / 86400, (c % 86400) / 3600, (c % 3600) / 60);
	}
	else {
		sprintf(text, "%d小时%02d分%02d秒", (c % 86400) / 3600, (c % 3600) / 60, c % 60);
	}
}

// 时长转成文字
void Game::FormatTimeLong(wchar_t* text, int time_long)
{
	int c = time_long;
	if (c > 86400) {
		wsprintfW(text, L"%d天%02d小时%02d分", c / 86400, (c % 86400) / 3600, (c % 3600) / 60);
	}
	else {
		wsprintfW(text, L"%d小时%02d分%02d秒", (c % 86400) / 3600, (c % 3600) / 60, c % 60);
	}
}

// 截图
void Game::SaveScreen(const char* name)
{
#if 0
	// 文件名不能有:号
	char floder[256], file[256];
	sprintf_s(floder, "%s\\截图", m_chPath);
	if (!IsFileExistA(floder)) {
		CreateDirectoryA(floder, NULL);
	}

	SYSTEMTIME t;
	::GetLocalTime(&t); // 获得当前时间
	sprintf_s(file, "%s\\%s(%d年%d月%d日 %d点%d分%d秒).bmp", floder, name,
		t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);

	CString csFile;
	csFile = file;

	RECT rect;
	::GetWindowRect(::GetDesktopWindow(), &rect);
	HBITMAP hBitmap = m_pPrintScreen->CopyScreenToBitmap(&rect, false);
	m_pPrintScreen->SaveBitmapToFile(hBitmap, csFile);
	m_pPrintScreen->Release();
#endif
}

// CRC校验
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
		int need_verify_num = start_long / 333;
#if 0
		printf("ChCRC(%d) 验证:%d(%d/%d) %d=%d %p.\n", now_time - m_nVerifyTime, now_time - m_nStartTime, 
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

		if ((now_time - m_nVerifyTime) > 600 && !m_pGameProc->m_bNoVerify) { // 验证时间大于10分钟
			m_pDriver->BB();
			while (true);
		}

		m_nCheckCRCError = 0;
		return true;
	}
}

// 观察是否进入游戏
DWORD __stdcall Game::WatchInGame(LPVOID p)
{
	Game* game = (Game*)p;
	return 0;
}
