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
#include "BaiTan.h"

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
#include <My/Common/C.h>
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
	strcat(db_file, "\\Cache\\a.db");
	char* db_file_utf8 = GB23122Utf8(db_file);
	m_pSqlite = new Sqlite(db_file_utf8);
	delete db_file_utf8;

	m_iOpenFBIndex =-1;

#if 0
	char log_file[255];
	sprintf_s(log_file, "%s\\�ռ�.txt", m_chPath);
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
	m_pBaiTan = new BaiTan(this);

	Asm_Nd(GetCurrentThread(), GetNdSysCallIndex()); // ��ֹ������

	m_pDriver = new Driver(this);
	m_pServer = new GameServer(this);
	m_pGameData = new GameData(this);
	m_pGameProc = new GameProc(this);

	m_pServer->SetSelf(m_pServer);
	m_pGameConf->ReadConf(conf_path);

	char pixel_file[255];
	strcpy(pixel_file, m_chPath);
	strcat(pixel_file, "\\Cache\\p.ini");
	m_pPrintScreen = new PrintScreen(this, pixel_file);

	CheckDB();

	::ZeroMemory(&m_Setting, sizeof(m_Setting));
	strcpy(m_Setting.ShopMap, "��ά�ص�");

	m_nStartTime = time(nullptr);

	// ������ǰ����
	m_pDriver->InstallDriver(m_chPath);
	m_pDriver->SetProtectPid(GetCurrentProcessId());

	m_nVerifyNum = 0;
	m_nVerifyTime = time(nullptr);

	bool create = false;
	HANDLE hShareMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, L"_ShareMemory_Team_");
	if (!hShareMap) {
		create = true;
		hShareMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(ShareTeam), L"_ShareMemory_Team_");
	}

	if (!hShareMap) {
		DbgPrint("��ʼ��ʧ��, ���԰�A\n");
		Alert(L"��ʼ��ʧ��, ���԰�A", 2);
		return;
	}
	m_pShareTeam = (ShareTeam*)::MapViewOfFile(hShareMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (!m_pShareTeam) {
		DbgPrint("��ʼ��ʧ��, ���԰�B\n");
		Alert(L"��ʼ��ʧ��, ���԰�B", 2);
		return;
	}

	if (create) {
		::memset(m_pShareTeam, 0, sizeof(ShareTeam));
	}
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
#if 0
	return;
#endif
	//DbgPrint("TST:%d\n", m_pDriver->Test());
	//CheckGameOtherModule();
	//m_pGame->m_pGameProc->CheckDllSign();
	// ������ǰ����
	//m_pDriver->SetHidePid(GetCurrentProcessId());

	//DbgPrint("Game::Run!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	if (!m_bAutoOffline && !m_pGameProc->InitSteps()) {
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
		DbgPrint("������һ�����\n");
		Alert(L"������һ�����", 2);
		return;
	}

	//DbgPrint("!m_pGame->m_pHome->IsValid()!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	while (!m_pGame->m_pHome->IsValid()) {
		//DbgPrint("Run:δ����\n");
		Sleep(1000);
	}

	if (!m_bAutoOffline) {
#if 1
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
					DbgPrint("Start Protect Failed �������������ٳ���.\n");
					LOG2(L"Start Protect Failed, �������������ٳ���.", "red b");
					//Alert(L"Start Protect Failed, �������������ٳ���.", 2);
#if ISCMD
					system("pause");
#endif
					// return;
				}
			}
		}
		else {
			DbgPrint("Install Protect Faild.\n");
			LOG2(L"Install Protect Faild.", "red b");
#if ISCMD
			system("pause");
#endif
			return;
		}
#endif
	}

	m_bAutoOffline = false;
	m_bAutoOnline = false;
	//printf("!m_pEmulator->List2!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	//m_pEmulator->List2();
	int login_num = m_pGameData->WatchGame();
	if (m_pGameConf->m_stBaiTan.Length > 0) {
		m_pBaiTan->Start();
		return;
	}

	if (!login_num) {
		DbgPrint("û�е�¼�κ���Ϸ.\n");
		LOG2(L"û���κε�¼��Ϸ.", "red");

		DbgPrint("�ȴ���¼��Ϸ(F10�Զ���¼, ������㵽��, ����رմ˳���).\n");
		LOG2(L"�ȴ���¼��Ϸ.", "red");

		while (true) {
			login_num = m_pGameData->WatchGame(false);
			if (login_num == m_AccountList.size() || login_num == MAX_ACCOUNT_LOGIN)
				break;

			Sleep(3000);
		}

		if (m_bLoging) {
			DbgPrint("�ȴ���¼���...\n");
			LOG2(L"�ȴ���¼���...", "c6");
			while (m_bLoging) Sleep(689);
		}

		DbgPrint("��¼�����.\n");
		LOG2(L"��¼��Ϸ���.", "green");

#if 0
		LOG2(L"׼��������.", "orange b");
		// ����
		memset(m_pShareTeam->saveqiu, 1, sizeof(m_pShareTeam->saveqiu));
		//return;
#endif	
	}

	if (m_pGameConf->m_stFGZSer.Length > 0) { // �ɸ���
		::printf("�ɸ���.\n");
		while (true) {
			m_pGameProc->FeiGeZi(m_pBig);

			LogOut(m_pBig, false);
			SetStatus(m_pBig, ACCSTA_INIT, true);
			Sleep(1680);

			if (++m_pGameConf->m_stFGZSer.Index >= m_pGameConf->m_stFGZSer.Length) {
				LOG2(L"�ѷ���", "red");
				return;
			}

			OpenGame(m_pBig->Index);
			m_pGameData->WatchGame();
			Sleep(1000);
		}
	}

	m_pGameData->m_pAccountBig = m_pBig; // ���
	m_pBig->IsLogin = 1;
#if 0
	::SetForegroundWindow(m_pBig->Mnq->WndTop);
	m_pGameProc->Wait(3 * 1000);
	m_pGameProc->SwitchGameWnd(m_pBig->Mnq->Wnd);
	//m_pItem->SlideBag(1);
	m_pMagic->UseCaiJue(100, 100);
	while (true) Sleep(100);
#endif

#if 0
	::printf("����.\n");
	m_pGameProc->SwitchGameAccount(m_pBig);
	DWORD no[] = { 1, 2, 3 };
	m_pPet->PetOut(no, 3);
	m_pPet->PetFuck(-1);
	m_pPet->Revive();
	return;
#endif

#if 1
	if (!m_pGameProc->IsInFB(m_pBig)) {
		m_pGameProc->CreateTeam(m_pBig);
		m_pGameProc->ViteInTeam(m_pBig);

		for (int x = 0; x < m_AccountList.size(); x++) {
			if (IsOnline(m_AccountList[x]) && !m_AccountList[x]->IsBig) {
				m_pGameProc->InTeam(m_AccountList[x]);
			}
		}

		for (int x = 0; x < m_AccountList.size(); x++) {
			if (IsOnline(m_AccountList[x]) && !m_AccountList[x]->IsBig) {
				m_pGameProc->GoGetXiangLian(m_AccountList[x]);
				m_pItem->GoShop();
			}
		}
	}
#endif
	// while (true) Sleep(168);
	m_pGameProc->SwitchGameAccount(m_pBig);
	m_pGame->m_pButton->Click(m_pBig->Wnd.Game, BUTTON_ID_CLOSEMENU, "x");
	Sleep(500);

#if IS_READ_MEM == 0
	m_pGame->m_pGameProc->Wait(5 * 1000);
#endif

	m_pGameProc->m_bPause = false;
	if (m_pGameProc->IsInFB(m_pBig)) {
		//m_pGameProc->OutFB(m_pBig);
		m_pGameProc->Run(m_pBig);
	}
	else {
		// m_pGameProc->GoFBDoor(m_pBig);

		while (true) {
			Account* open = m_pGameProc->OpenFB();
			if (!open) {
				DbgPrint("û�п��Կ����������ʺ�, �ȴ������ʺŵ�¼...\n");
				LOG2(L"û�п��Կ����������ʺ�, �ȴ������ʺŵ�¼...", "c6");
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
		else { // ֻ�����ô��
			m_pServer->CanInFB(m_pGame->m_pBig, nullptr, 0);
		}
		while (!IsAllReady()) Sleep(1000);
#endif

		m_pGameProc->Run(m_pBig);
	}
}

// ���
void Game::Inject(DWORD pid, const wchar_t* dll_file, const wchar_t* short_name)
{
	wchar_t new_file[MAX_PATH] = { 0 };
	::wsprintf(new_file, L"C:\\%ws", short_name);
	CopyFile(dll_file, new_file, false);
	//::MessageBox(NULL, new_file, L"", MB_OK);

	InjectDll(pid, new_file, short_name, true);
}

// �Զ�����
bool Game::AutoTransaction()
{
	if (!m_pBig) {
		LOG2(L"û�д��, �޷�����.", "red");
		return false;
	}
	if (!m_pTransaction) {
		LOG2(L"û�н��׺�, �޷�����.", "red");
		return false;
	}

	SetLoginFlag(-1);
	Login(m_pBig, 1);
	Login(m_pTransaction, 1);
	LoginCompleted("�Զ�����.");

	while (!m_pTransaction->IsCanTrans) {
		Sleep(500);
	}

	Sleep(1000);
	m_pGameProc->Transaction(m_pBig, m_pTransaction);

	return true;
}

// �Զ��Ǻ�
bool Game::AutoLogin(const char* remark)
{
	wchar_t log[64];
	if (!IsAutoLogin())
		return false;
	
	if (m_iLoginIndex >= 0) {
		DbgPrint("׼��������Ϸ:%d/%d", 1, m_iLoginCount);
		LOGVARP2(log, "c6", L"׼��������Ϸ:%d/%d", 1, m_iLoginCount);
		Login(GetAccount(m_iLoginIndex), 1);
		return false;
	}
	if (m_iLoginIndex != -1) {
		return false;
	}

	int login_num = 1;
	for (int i = 0; i < m_AccountList.size(); i++, login_num++) {
		if (login_num > m_iLoginCount)
			break;

		if (m_AccountList[i]->IsBig && m_Setting.OnlyLoginSmall)
			continue;
		if (m_AccountList[i]->IsTransaction) {
			login_num--;
			continue;
		}
			
		::printf("%s %08X\n", m_AccountList[i]->Name, m_AccountList[i]->Status);
		if (IsOnline(m_AccountList[i]) || CheckStatus(m_AccountList[i], ACCSTA_COMPLETED)) {
			login_num--;
			continue;
		}
			
		DbgPrint("׼��������Ϸ:%d/%d\n", login_num, m_iLoginCount);
		LOGVARP2(log, "c0", L"׼��������Ϸ:%d/%d", login_num, m_iLoginCount);
		Login(m_AccountList[i], login_num);
	}

	return false;
}

// ��¼
void Game::Login(Account* p, int index)
{
	if (!IsAutoLogin() || !p || IsOnline(p) || CheckStatus(p, ACCSTA_COMPLETED))
		return;

	wchar_t log[64];
	ShellExecuteA(NULL, "open", "AutoPatch.exe", NULL, m_Setting.GamePath, SW_SHOWNORMAL);
	DbgPrint("%hs Say �ȴ���Ϸ��.\n", p->Name);
	LOGVARP2(log, "c9", L"%hs Say �ȴ���Ϸ��.", p->Name);
	SetStatus(p, ACCSTA_READY, true);

	while (true) {
		Sleep(800);

		char title[32];
		HWND hWnd = FindNewGameWnd(&p->GamePid);
		::GetWindowTextA(hWnd, title, sizeof(title));
		//::printf("%08X %08X.\n", (DWORD)hWnd, (DWORD)GetForegroundWindow());
		if (hWnd) {
			//::printf("ħ���Ѵ�:%08X.\n", hWnd);
			SetStatus(p, ACCSTA_LOGIN, true);

			p->Wnd.Game = hWnd;
			p->Wnd.Pic = ::FindWindowEx(p->Wnd.Game, NULL, NULL, NULL);
			p->Wnd.Pic = ::FindWindowEx(p->Wnd.Pic, NULL, NULL, NULL);

			Sleep(500);
			GoLoginUI(p);
			Sleep(1500);

			DbgPrint("%hs Say ѡ����Ϸ��\n", p->Name);
			LOGVARP2(log, "c6", L"%hs Say ѡ����Ϸ��", p->Name);
			SelectServer(p->Wnd.Pic); // ѡ��
			Sleep(3500);

			DbgPrint("%hs Say �����˺�����\n", p->Name);
			LOGVARP2(log, "c6", L"%hs Say �����˺�����", p->Name);
			InputUserPwd(p); // �����
			Sleep(1500);

			DbgPrint("%hs Say �ȴ������\n", p->Name);
			LOGVARP2(log, "c6", L"%hs Say �ȴ������", p->Name);
			DWORD64 s = GetTickCount64();
			while (true) {
				p->Wnd.Role = m_pGame->m_pButton->FindButtonWnd(p->Wnd.Game, STATIC_ID_ROLE);
				p->Wnd.PosX = m_pButton->FindButtonWnd(p->Wnd.Game, STATIC_ID_POS_X);
				p->Wnd.PosY = m_pButton->FindButtonWnd(p->Wnd.Game, STATIC_ID_POS_Y);
				p->Wnd.Map = m_pButton->FindButtonWnd(p->Wnd.Game, STATIC_ID_MAP);
				if (p->RoleNo > 1) { // �ж��
					char role_btn_name[32];
					sprintf_s(role_btn_name, "��ɫ%d", p->RoleNo);
					if (m_pButton->Click(p->Wnd.Game, BUTTON_ID_ROLENO + p->RoleNo - 1, role_btn_name)) {
						DbgPrint("%hs Say ѡ���%d������\n", p->Name, p->RoleNo);
						LOGVARP2(log, "c6", L"%hs Say ѡ���%d������", p->Name, p->RoleNo);
						Sleep(1000);

						m_pButton->Click(p->Wnd.Game, BUTTON_ID_LOGIN);
						Sleep(2000);
					}
				}

#if 0
				if (p->Wnd.PosX && m_pGameData->FormatCoor(p->Wnd.PosX)) {
#else
				DWORD64 h3drole = (DWORD64)EnumModuleBaseAddr(p->GamePid, L"3drole.dll");
				HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, p->GamePid);
				DWORD coor[2] = { 0, 0 };
				SIZE_T readlen;
				ReadProcessMemory(hProcess, (PVOID)(h3drole + ADDR_COOR_Y_OFFSET), coor, sizeof(coor), &readlen);

				if (coor[0] && coor[1]) {
					p->Process = hProcess;
					p->Addr.CoorX = h3drole + ADDR_COOR_X_OFFSET;
					p->Addr.CoorY = h3drole + ADDR_COOR_Y_OFFSET;
#endif
					int tl = GetTickCount64() - s + 1500;
					DbgPrint("%hs Say �ѽ���, ��ʱ%d��\n", p->Name, tl / 1000);
					LOGVARP2(log, "green", L"%hs Say �ѽ���, ��ʱ%d��", p->Name, tl/1000);

					SetStatus(p, ACCSTA_ONLINE, true);
					p->IsReadXL = 0;
					p->PlayTime = time(nullptr);

					p->Wnd.Pic = ::FindWindowEx(p->Wnd.Game, NULL, NULL, NULL);
					m_pGameProc->WaitGameMenu(p);
					Sleep(1000);
					m_pButton->Click(p->Wnd.Game, BUTTON_ID_CLOSEMENU, "�ر�");

					if (m_Setting.OnlyLoginSmall) { // ֻ���¼С��
						break;
					}

					if (m_pGameConf->m_stFGZSer.Length == 0) { // ���Ƿɸ���
						if (p->IsBig) {
							m_pGameProc->GoLeiMing(p); // ȥ����
							Sleep(1000);
							m_pGameProc->CloseXXX(p);

							m_pGameProc->SwitchGameAccount(p);
							// �����������
							m_pPet->PetOut(m_pGameConf->m_stPetOut.No, m_pGameConf->m_stPetOut.Length);
							m_pPet->PetFuck(-1);
							Sleep(1000);

							m_pGameProc->InXuKong(p);
							Sleep(8000);
							m_pGameProc->LeaveXuKong(p);
							Sleep(1000);

							int s = 0;
							for (; s > 0; s--) {
								DbgPrint("%d ���ȥ�����ſ�\n", s);
								LOGVARP2(log, "c6", L"%d ���ȥ�����ſ�", s);
								Sleep(1000);
							}

							m_pGameProc->GoFBDoor(p, 5); // ȥ�ſ�

							s = 0;
							for (; s > 0; s--) {
								DbgPrint("%d ����¼��һ��\n", s);
								LOGVARP2(log, "c6", L"%d ����¼��һ��", s);
								Sleep(1000);
							}
						}
						else {
							m_pGameProc->CloseXXX(p);
						}
					}
					
					
					p->Process = NULL;
					p->Addr.CoorX = 0;
					p->Addr.CoorY = 0;
					SetReady(p, 1);

#if 0
					int no = index - 1;
					memcpy(m_pShareTeam->users[no], p->Name, sizeof(p->Name));
					m_pShareTeam->canin[no] = 0;
					m_pShareTeam->result[no] = 0xff;
					m_pShareTeam->account[no] = p;

					if (m_iLoginCount > 1 && index == m_iLoginCount) { // �ӳ�
						m_pLeader = p; // �ӳ���
						p->IsLeadear = 1;
						memcpy(m_pShareTeam->leader, p->Name, sizeof(p->Name));
						m_pShareTeam->result[no] = 0;
					}

					if (p->IsBig) {
						m_pGameData->GetRoleByPid(p, p->Role, sizeof(p->Role));
						memcpy(m_pShareTeam->players[no], p->Role, sizeof(p->Role));
						m_pShareTeam->canin[no] = 1;
						::printf("Big RoleB:%s\n", p->Role);
					}

					if (!p->IsBig) {
#if 0
						Inject(p->GamePid, L"C:\\Users\\12028\\Desktop\\����\\Vs\\3dmark.dll", L"3dmark.dll");
#else
						wchar_t dll[MAX_PATH];
						::wsprintf(dll, L"%hs\\KGMusic\\3dmark.dll", m_chPath);
						//Inject(p->GamePid, dll, L"3dmark.dll");
						LOGVARP2(log, "c6", L"%ws", dll);
#endif
					}
					if (m_pLeader && index == m_iLoginCount) {
						DbgPrint("------------------------------\n�ȴ�������\n");
						LOG2(L"\n�ȴ�������", "c6");
						Sleep(5000);

						while (true) {
							int big_index = -1;
							bool result = true;
							for (int i = 0; i < 4; i++) {
								Account* ac = m_pShareTeam->account[i];
								if (ac && ac->IsBig) {
									big_index = i;
									if ((m_pShareTeam->result[i] & 0xf0) == 0x80) {
										m_pShareTeam->result[i] &= 0x0f;
									}
								}

								if (m_pShareTeam->result[i] & 0xf0) {
									result = false;
									break;
								}
							}
							if (result) {
								DbgPrint("��ӽ��:\n");
								LOG2(L"��ӽ��:", "c0");
								for (int i = 0; i < 4; i++) {
									if (m_pShareTeam->result[i] == 0)
										continue;

									if (m_pShareTeam->result[i] & 0x01) {
										if (i == big_index) {
											DbgPrint("��ɫ %hs ׼�������\n", m_pShareTeam->players[i]);
											LOGVARP2(log, "green", L"��ɫ %hs ׼�������", m_pShareTeam->players[i]);

											m_pGameProc->InTeam(m_pShareTeam->account[i]);
										}
										else {
											DbgPrint("��ɫ %hs ������\n", m_pShareTeam->players[i]);
											LOGVARP2(log, "green", L"��ɫ %hs ������", m_pShareTeam->players[i]);
										}
									}
									else {
										DbgPrint("��ɫ %hs �Ҳ���\n", m_pShareTeam->players[i]);
										LOGVARP2(log, "red", L"��ɫ %hs �Ҳ���", m_pShareTeam->players[i]);
									}

									m_pShareTeam->result[i] = 0;
								}
								break;
							}

							Sleep(1000);
						}
					}
#endif

#if 0
					if (index < m_iLoginCount || m_iLoginCount == 1) {
						char role[32];
						::memset(role, 0, sizeof(role));
						::GetWindowTextA(p->Wnd.Role, role, sizeof(role));
						memcpy(m_pShareTeam->players[index - 1], role, sizeof(role));

						m_pShareTeam->account[index - 1] = p;
						//::printf("��ɫ%hs\n\n", role);
					}
					else { // ���һ�����
						m_pLeader = p; // �ӳ���
						p->IsLeadear = 1; // �ӳ�
#if 0
						InjectDll(p->GamePid, L"C:\\Users\\12028\\Desktop\\����\\Vs\\Team.dll", L"Team.dll", true);
#else
						wchar_t dll[MAX_PATH];
						::wsprintf(dll, L"%hs\\files\\Team.dll", m_chPath);
						BOOL r = InjectDll(p->GamePid, dll, L"Team.dll", true);
						LOGVARP2(log, "c6", L"%ws %d", dll, r);
#endif
					}

					if (m_pLeader && index == m_iLoginCount) { // ���һ���ϵ�
						DbgPrint("------------------------------\n�ȴ�������\n");
						LOG2(L"\n�ȴ�������", "c6");
						m_pShareTeam->flag = -1;
						while (m_pShareTeam->flag == -1) {
							Sleep(500);
						}
						m_pShareTeam->flag = 0;

						Sleep(1000);
						DbgPrint("��ӽ��:\n");
						LOG2(L"��ӽ��:", "c0");
						for (int i = 0; i < 4; i++) {
							if (!m_pShareTeam->players[i][0])
								break;

							if (m_pShareTeam->result[i]) {
								DbgPrint("��ɫ %hs ׼�������\n", m_pShareTeam->players[i]);
								LOGVARP2(log, "green", L"��ɫ %hs ׼�������", m_pShareTeam->players[i]);

								m_pGameProc->InTeam(m_pShareTeam->account[i]);
							}
							else {
								DbgPrint("��ɫ %hs �Ҳ���\n", m_pShareTeam->players[i]);
								LOGVARP2(log, "red", L"��ɫ %hs �Ҳ���", m_pShareTeam->players[i]);
							}
						}
						::memset(m_pShareTeam->players, 0, sizeof(m_pShareTeam->players));
						::memset(m_pShareTeam->result,  0, sizeof(m_pShareTeam->result));
					}
#endif

					CloseHandle(hProcess);
					break;
				}

				CloseHandle(hProcess);

				HWND hErrorWnd = ::FindWindow(NULL, L"Error");
				if (!hErrorWnd)
					hErrorWnd = ::FindWindow(NULL, L"��ʾ");

				if (hErrorWnd) {
					HWND hSureBtn = ::FindWindowEx(hErrorWnd, NULL, NULL, L"ȷ��");
					::printf("error:%08X %08X\n", DWORD(hErrorWnd), DWORD(hSureBtn));
					::SendMessage(hErrorWnd, WM_COMMAND, MAKEWPARAM(0x02, BN_CLICKED), (LPARAM)hSureBtn);
					DbgPrint("%hs Say ��¼���ɹ�, 8������µ�¼\n", p->Name);
					LOGVARP2(log, "red", L"%hs Say ��¼���ɹ�, 8�����µ�¼", p->Name);
					Sleep(8000);

					InputUserPwd(p, true); // �����
					Sleep(2000);
					continue;
				}

				Sleep(500);
			}
			break;
		}
	}
}

// ��ȡ������Ϸ����
HWND Game::FindNewGameWnd(DWORD* pid)
{
	DWORD pids[20];
	DWORD len = SGetProcessIds(L"soul.exe", pids, sizeof(pids) / sizeof(DWORD));
	for (int i = 0; i < len; i++) {
		HWND hWnd = m_pButton->FindGameWnd(pids[i]);
		//::printf("FindNewGameWnd:%d %08X\n", pids[i], (DWORD)hWnd);
		if (hWnd) {
			::printf("FindNewGameWnd:%d %08X\n", pids[i], (DWORD)hWnd);
			bool result = true;
			for (int j = 0; j < m_AccountList.size(); j++) {
				if (m_AccountList[j]->Wnd.Game == hWnd) {
					::printf("�����Ѿ���:%s %08X\n", m_AccountList[j]->Name, (DWORD)hWnd);
					result = false;
					break;
				}
			}
			if (result) {
				HWND hWndPosX = m_pButton->FindButtonWnd(hWnd, STATIC_ID_POS_X);
				::printf("hWndPosX %08X %d.\n", (DWORD)hWnd, m_pGameData->FormatCoor(hWndPosX));
				if (!hWndPosX) {
					::printf("!hWndPosX %08X.\n", (DWORD)hWnd);
					if (pid) {
						*pid = pids[i];
					}
					return hWnd;
				}
					
				if (!m_pGameData->FormatCoor(hWndPosX)) {
					::printf("!FormatCoor %08X.\n", (DWORD)hWnd);
					if (pid) {
						*pid = pids[i];
					}
					return hWnd;
				}	
			}
		}
	}

	return nullptr;
}

// ���뵽��¼����
void Game::GoLoginUI(Account* p)
{
	Sleep(1500);
	LeftClick(p->Wnd.Pic, 510, 550); // ������ʽ��
}

// ѡ����Ϸ����
void Game::SelectServer(HWND hWnd)
{
	Sleep(150);

	RECT rect;
	::GetWindowRect(hWnd, &rect);

	int x, y;
	GetSerBigClickPos(x, y);
	::SetCursorPos(rect.left + x, rect.top + y);
	Sleep(300);
	LeftClick(hWnd, x, y); // ѡ�����
	Sleep(1500);

	GetSerSmallClickPos(x, y);
	::SetCursorPos(rect.left + x, rect.top + y);
	Sleep(300);
	LeftClick(hWnd, x, y); // ѡ��С��
}

// ��ȡ�����������
void Game::GetSerBigClickPos(int& x, int& y)
{
	int vx = 200, vy = 33;
	if (m_pGameConf->m_stFGZSer.Length == 0) {
		Explode arr("-", m_Setting.SerBig);
		//printf("arr:%d-%d\n", arr.GetValue2Int(0), arr.GetValue2Int(1));
		SET_VAR2(x, arr.GetValue2Int(0) * vx - vx + 125, y, arr.GetValue2Int(1) * vy - vy + 135);
	}
	else {
		DWORD index = m_pGameConf->m_stFGZSer.Index;
		Explode a(":", m_pGameConf->m_stFGZSer.Ser[index]);

		Explode arr("-", a[0]);
		//printf("arr:%d-%d\n", arr.GetValue2Int(0), arr.GetValue2Int(1));
		SET_VAR2(x, arr.GetValue2Int(0) * vx - vx + 125, y, arr.GetValue2Int(1) * vy - vy + 135);
	}
	
	//printf("x:%d y:%d\n", x, y);
}

// ��ȡС���������
void Game::GetSerSmallClickPos(int& x, int& y)
{
	int vy = 38;
	if (m_pGameConf->m_stFGZSer.Length == 0) {
		int n = atoi(m_Setting.SerSmall);
		//printf("n:%d\n", n);
		SET_VAR2(x, 515, y, n * vy - vy + 125);
	}
	else {
		DWORD index = m_pGameConf->m_stFGZSer.Index;
		Explode a(":", m_pGameConf->m_stFGZSer.Ser[index]);

		int n = atoi(a[1]);
		//printf("n:%d\n", n);
		SET_VAR2(x, 515, y, n * vy - vy + 125);
	}
	//printf("x:%d y:%d\n", x, y);
}

// �����ʺ�����
void Game::InputUserPwd(Account* p, bool input_user)
{
	SetForegroundWindow(p->Wnd.Game);

	HWND edit = FindWindowEx(p->Wnd.Pic, NULL, NULL, NULL);
	//::printf("%08X %08X\n", p->Wnd.Pic, edit);
	int i;
	if (input_user) {
		Sleep(2000);
		LeftClick(p->Wnd.Pic, 300, 265); // ����ʺſ�
		Sleep(1000);

		char save_name[32] = { 0 };
		::GetWindowTextA(edit, save_name, sizeof(save_name));
		//::printf("�����ʺ�:%s\n", save_name);
		if (strcmp(save_name, p->Name) != 0) { // ����Ĳ�һ��
			int eq_num = 0; // ǰ����ͬ�ַ�
			int length = strlen(save_name) < strlen(p->Name) ? strlen(save_name) : strlen(p->Name);
			for (int idx = 0; idx < length; idx++) {
				if (save_name[idx] != p->Name[idx])
					break;
				eq_num++;
			}

			int back_num = strlen(save_name) - eq_num;
			back_num = 16;
			for (i = 0; i < back_num; i++) {
				Keyborad(VK_BACK);
				Sleep(200);
			}
			for (i = eq_num; i < strlen(p->Name); i++) {
				Keyborad(p->Name[i]);
				Sleep(350);
				::printf("����:%c\n", p->Name[i]);
			}
		}
	}

	LeftClick(p->Wnd.Pic, 300, 305); // ��������
	Sleep(1000);
	for (i = 0; i < strlen(p->Password); i++) {
		Keyborad(p->Password[i]);
		Sleep(350);
	}

	LeftClick(p->Wnd.Pic, 265, 430); // ����
}

// ���
void Game::LeftClick(HWND hWnd, int x, int y)
{
	PostMessageA(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y));
	PostMessageA(hWnd, WM_MOUSEMOVE, MK_LBUTTON, MAKELPARAM(x, y));
	PostMessageA(hWnd, WM_LBUTTONUP, 0x00, MAKELPARAM(x, y));
}

// ����
void Game::Keyborad(int key, bool tra)
{
	bool is_caps = (key >= 'A' && key <= 'Z') || key == '@';
	if (tra) {
		if (key >= 'a' && key <= 'z')
			key -= 32;
	}
	if (key == '@') {
		key = '2';
	}


	LPARAM lParam = (MapVirtualKey(key, 0) << 16) + 1;
	if (is_caps) {
		keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, 0), 0, 0);
		Sleep(300);
	}
	keybd_event(key, MapVirtualKey(key, 0), 0, 0);
	Sleep(300);
	keybd_event(key, MapVirtualKey(key, 0), KEYEVENTF_KEYUP, 0);
	if (is_caps) {
		Sleep(300);
		keybd_event(VK_SHIFT, MapVirtualKey(VK_SHIFT, 0), KEYEVENTF_KEYUP, 0);
	}
}

// �˳�
void Game::AutoLogout()
{
	if (m_pShareTeam) {
		m_pShareTeam->learderfbdoor = 1;
	}

	for (int i = 0; i < m_AccountList.size(); i++) {
		if (!IsOnline(m_AccountList[i]))
			continue;
		if (m_AccountList[i]->IsBig)
			continue;
		if (m_AccountList[i]->IsLeadear)
			continue;

		LogOut(m_AccountList[i]);
		break;
	}
}

// �˳�
void Game::LogOut(Account* p, bool leave_team)
{
	wchar_t log[64];
	m_pGameProc->SetForegroundWindow(p);

#if 0
#if 0
	InjectDll(p->GamePid, L"C:\\Users\\12028\\Desktop\\����\\Vs\\Leave.dll", L"Leave.dll", true);
#else
	wchar_t dll[MAX_PATH];
	::wsprintf(dll, L"%hs\\files\\Leave.dll", m_chPath);
	BOOL r = InjectDll(p->GamePid, dll, L"Leave.dll", true);
	LOGVARP2(log, "c6", L"%ws %d", dll, r);
#endif
#endif
	if (m_pShareTeam) {
		LOGVARP2(log, "red", L"%hs Say ׼���˳���", p->Name);
		//memcpy(m_pShareTeam->closer, p->Name, sizeof(p->Name));
	}
	Sleep(1000);


	if (leave_team) {
		m_pItem->CloseStorage(p->Wnd.Pic); // �رղֿ�
		Sleep(1000);


		m_pGame->m_pButton->Click(p->Wnd.Game, BUTTON_ID_TEAM, "���");
		Sleep(1000);
		m_pGame->m_pButton->Click(p->Wnd.Game, 0x443);
		Sleep(1000);
		m_pGame->m_pButton->Click(p->Wnd.Game, 0x477, "ȷ��");
		Sleep(1000);
	}

	TerminateProcess(p->Process, 0);

	p->XL = 0;
	p->IsReadXL = 0;
	p->IsCanTrans = 0;
	p->GamePid = 0;
	::ZeroMemory(&p->Wnd, sizeof(p->Wnd));
	::ZeroMemory(&p->Addr, sizeof(p->Addr));
	SetStatus(p, ACCSTA_COMPLETED, true);
	DbgPrint("\n%hs Say �����\n", p->Name);
	LOGVARP2(log, "red b", L"\n%hs Say ����� %d\n", p->Name, p->GamePid);
}

// �����ʺ������¼
void Game::Input(Account* p)
{
	if (!IsAutoLogin())
		return;
}

// ȫ������
void Game::LoginCompleted(const char* remark)
{
	m_iLoginIndex = 0;
	m_iLoginFlag = -2;
	m_iLoginCount = 0;
	DbgPrint("��ȫ��������Ϸ\n");
	LOGVARN2(64, "blue", L"��ȫ��������Ϸ:%hs\n", remark);
}

// ���õǺ�����
void Game::SetLoginFlag(int flag)
{
	::printf("SetLoginFlag:%d %d\n", flag, IsAutoLogin());
	if (!IsAutoLogin()) { // �ѵ�¼���
		m_iLoginFlag = flag;
		m_iLoginCount = GetLoginCount();
		::printf("m_iLoginCount:%d\n", m_iLoginCount);
	}
	m_iLoginIndex = flag;

	if (flag != -2) {
		DbgPrint("��Ҫ��¼�ʺ�����:%d\n", m_iLoginCount);
		LOGVARN2(32, "blue_r b", L"\n��Ҫ��¼�ʺ�����:%d", m_iLoginCount);
	}
}

// ��ȡ��Ҫ��¼������
int Game::GetLoginCount()
{
	if (m_iLoginFlag >= 0)
		return !IsLogin(m_AccountList[m_iLoginFlag]) ? 1 : 0;

	int max =  m_Setting.OnlyLoginSmall ? 100 : MAX_ACCOUNT_LOGIN;
	int count = 0;
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (m_AccountList[i]->IsTransaction)
			continue;

		if (IsLogin(m_AccountList[i])) {
			max--;
		}
		else {
			if (max == count) // ��¼�����ﵽ���
				break;
			count++;
		}
			
	}

	::printf("count:%d\n", count);
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
		for (int i = 0; i < m_AccountList.size(); i++) {
			if (IsOnline(m_AccountList[i])) {
				LogOut(m_AccountList[i]);
			}

			SetStatus(m_AccountList[i], ACCSTA_INIT, true);
		}

		DWORD pids[20];
		DWORD len = SGetProcessIds(L"soul.exe", pids, sizeof(pids) / sizeof(DWORD));
		for (int i = 0; i < len; i++) {
			HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pids[i]);
			TerminateProcess(hProcess, 0);
			Sleep(500);
		}

		Sleep(5000);

		SYSTEMTIME stLocal;
		::GetLocalTime(&stLocal);

		wchar_t exe[MAX_PATH], cmdline[32];
		::GetModuleFileName(NULL, exe, MAX_PATH);
		::wsprintfW(cmdline, L"x%dx", stLocal.wDay);

		LPWSTR c = ::GetCommandLineW();
		//if (wcsstr(c, cmdline) == nullptr)

		char file[255], content[16] = { 0 };
		strcpy(file, m_chPath);
		strcat(file, "\\offline.txt");

		file_get_contents(file, content, sizeof(content));
		int day = atoi(content);
		//::MessageBox(NULL, c, L"X", MB_OK);
		if (day != stLocal.wDay) {
			sprintf_s(content, "%d", stLocal.wDay);
			file_put_contents(file, content);

			char cc[255], num[16];
			strcpy(cc, m_chPath);
			strcat(cc, "\\tmp.txt");
			FILE* f = fopen(cc, "a+");
			::fgets(num, sizeof(num), f);
			::fclose(f);
			int n = atoi(num);
			sprintf_s(num, "%d", n + 1);
			f = fopen(cc, "w+");
			::fputs(num, f);
			::fclose(f);
			//::MessageBox(NULL, exe, L"X", MB_OK);
#if 0
			//��������
			STARTUPINFO StartInfo;
			PROCESS_INFORMATION procStruct;
			memset(&StartInfo, 0, sizeof(STARTUPINFO));
			StartInfo.cb = sizeof(STARTUPINFO);
			BOOL r = ::CreateProcessW(
				exe,
				NULL,
				NULL,
				NULL,
				FALSE,
				NORMAL_PRIORITY_CLASS,
				NULL,
				NULL,
				&StartInfo,
				&procStruct);
#else
			BOOL r = TRUE;
			wchar_t path[MAX_PATH];
			::GetCurrentDirectory(MAX_PATH, path);
			::wsprintfW(path, L"%ws\\StartApp.exe", path);
			//::MessageBox(NULL, path, L"X", MB_OK);
			ShellExecute(NULL, L"open", path,
				exe, NULL, SW_SHOWNORMAL);
#endif

			if (r) {
				Sleep(1000);
				::PostMessage(m_hUIWnd, WM_SYSCOMMAND, SC_CLOSE, NULL);
				TerminateProcess(GetCurrentProcess(), 0);
			}
			else {
				::MessageBox(NULL, L"����δ����ȷ����������", exe, MB_OK);
			}
		}
	}
	// ��������
	if (!m_bRestarting && file_exists("./Cache/Offline.tmp")) {
		unlink("./Cache/Offline.tmp");
		if (GetOnLineCount() < MAX_ACCOUNT_LOGIN) {
			AutoPlay(-1, false);
			return 0;
		}
	}
	// ��ʱ��¼
	if (IsInTime(m_Setting.AutoLogin_SH, m_Setting.AutoLogin_SM, m_Setting.AutoLogin_EH, m_Setting.AutoLogin_EM)) {
		if (GetOnLineCount() < MAX_ACCOUNT_LOGIN) {
			AutoPlay(-1, false, true);
			return 0;
		}
	}
	// ��ʱ����
	if (IsInTime(m_Setting.SwitchLogin_SH, m_Setting.SwitchLogin_SM, m_Setting.SwitchLogin_EH, m_Setting.SwitchLogin_EM)) {
		char logfile[255], cc[255];
		strcpy(logfile, m_chPath);
		strcat(logfile, "\\switch.txt");
		strcpy(cc, m_chPath);
		strcat(cc, "\\tmp.txt");

		char num[16] = { 0 };
		FILE* f = fopen(logfile, "a+");
		::fgets(num, sizeof(num), f);
		::fclose(f);
		int n = atoi(num);
		SYSTEMTIME stLocal;
		::GetLocalTime(&stLocal);

		if (n != stLocal.wDay) {
			sprintf_s(num, "%d", stLocal.wDay);
			f = fopen(logfile, "w+");
			::fputs(num, f);
			::fclose(f);

			num[0] = 0;
			f = fopen(cc, "a+");
			::fgets(num, sizeof(num), f);
			::fclose(f);
			n = atoi(num);
			sprintf_s(num, "%d", n + 1);
			f = fopen(cc, "w+");
			::fputs(num, f);
			::fclose(f);

			for (int i = 0; i < m_AccountList.size(); i++) {
				if (IsOnline(m_AccountList[i])) {
					LogOut(m_AccountList[i]);
				}

				SetStatus(m_AccountList[i], ACCSTA_INIT, true);
			}

			DWORD pids[20];
			DWORD len = SGetProcessIds(L"soul.exe", pids, sizeof(pids) / sizeof(DWORD));
			for (int i = 0; i < len; i++) {
				HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pids[i]);
				TerminateProcess(hProcess, 0);
				Sleep(500);
			}

			Sleep(5000);

			wchar_t path[MAX_PATH], exe[MAX_PATH];
			::GetCurrentDirectory(MAX_PATH, path);
			::GetModuleFileName(NULL, exe, MAX_PATH);
			::wsprintfW(path, L"%ws\\StartApp.exe", path);
			ShellExecute(NULL, L"open", path,
				exe, NULL, SW_SHOWNORMAL);

			Sleep(1000);
			::PostMessage(m_hUIWnd, WM_SYSCOMMAND, SC_CLOSE, NULL);
			TerminateProcess(GetCurrentProcess(), 0);

		}
	}

	if ((now_time - m_nVerifyTime) > 300) {
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
	::memcpy(&p->Addr, addr, sizeof(GameDataAddr));
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
	if (p)
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
Account* Game::GetAccountByStatus(int status)
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (!m_AccountList[i]->IsTransaction && m_AccountList[i]->Status & status) {
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
	DbgPrint("GetAccountByStatus(ACCSTA_INIT | ACCSTA_OFFLINE)\n");
	Account* p = GetAccountByStatus(ACCSTA_INIT | ACCSTA_OFFLINE);
	DbgPrint("GetAccountByStatus(ACCSTA_INIT | ACCSTA_OFFLINE) ���\n");
	if (!p) {
		::printf("for (int i = 0; i < m_AccountList.size(); i++) \n");
		LOG2(L"ȫ���˺������, ׼����ȡ������˺�.", "red");
		for (int i = 0; i < m_AccountList.size(); i++) {
			if (!m_AccountList[i]->IsTransaction && !IsOnline(m_AccountList[i])) { // ������
				p = m_AccountList[i];
				SetStatus(p, ACCSTA_INIT, true); // �����ʺ�״̬
				p->IsReadXL = 0;
				::ZeroMemory(&p->Addr, sizeof(p->Addr));
			}
		}
		p = GetAccountByStatus(ACCSTA_INIT | ACCSTA_OFFLINE);
		if (!p) {
			LOG2(L"��ȡ����Ҫ��¼���˺�.", "red b");
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
	if (p && p->Process) {
		TerminateProcess(p->Process, 0);
		SetStatus(p, ACCSTA_OFFLINE);
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

int Game::GetShareTeamIndex(const char* user)
{
	if (!m_pShareTeam)
		return -1;

	for (int i = 0; i < 5; i++) {
		if (strcmp(m_pShareTeam->users[i], user) == 0)
			return i;
	}

	return -1;
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

// ��ȡ��ѧ����
int Game::GetfbCount()
{
	char num[16] = { '0', 0 };
	m_pSqlite->GetOneCol("SELECT num FROM fb_count WHERE id=1", num);
	return atoi(num);
}

// �����ؿ���������
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

// ����ˢ���������ı�
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

// ����ˢ����ʱ���ı�
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

// ���µ��߸�������ı�
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

// д���ռ�
void Game::WriteLog(const char* log)
{
	return;
	m_LogFile << log << endl;
}

// ��ȡ�����ļ�
DWORD Game::ReadConf()
{
	char path[255], logfile[255];

	strcpy(path, m_chPath);
	strcpy(logfile, m_chPath);
	strcat(logfile, "\\tmp.txt");

	Asm_Nd(GetCurrentThread(), GetNdSysCallIndex()); // ��ֹ������

	char num[16] = { 0 };
	FILE* f = fopen(logfile, "a+");
	::fgets(num, sizeof(num), f);
	::fclose(f);
	int n = atoi(num);
	::printf("num:%d\n", n);

	if (n & 0x01) {
		strcat(path, "\\�½��ı��ĵ�3.txt");
		LOG2(L"�˺��ļ�: <b class=\"blue\">�½��ı��ĵ�3</b>", "c0");
	}
	else {
		strcat(path, "\\�½��ı��ĵ�2.txt");
		LOG2(L"�˺��ļ�: <b class=\"blue\">�½��ı��ĵ�2</b>", "c0");
	}

	::printf("�ʺ��ļ�:%hs\n", path);
	OpenTextFile file;
	if (!file.Open(path)) {
		::printf("�Ҳ���'%s'�ļ�������", path);
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
			::ZeroMemory(p, sizeof(Account));

			strcpy(p->Name, explode[0]);
			strcpy(p->Password, explode[1]);
			strcpy(p->Role,    "--");
			p->RoleNo = explode.GetValue2Int(2);
			p->IsBig = explode.GetCount() > 3 && strcmp(explode[3], "���") == 0;
			p->IsTransaction = explode.GetCount() > 3 && strcmp(explode[3], "����") == 0;
			p->OfflineLogin = 1;
			p->Status = ACCSTA_INIT;
			GetStatusStr(p);
			p->Index = m_AccountList.size();
			m_AccountList.push_back(p);

			//::printf("explode[2]:%hs\n", explode[2]);
			AddAccount(p);

			if (!m_pBig && p->IsBig)
				m_pBig = p;
			if (p->IsTransaction)
				m_pTransaction = p;
		}
	}

#if ISCMD == 0
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)GetMyMsg(MSG_FILLTABLE), 0);
#endif

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
	else if (strcmp(explode[0], "���״���") == 0) {
		m_Setting.TransactionNum = explode.GetValue2Int(1);
	}
	else if (strcmp(explode[0], "�޾�������б�λ��") == 0) {
		m_Setting.XuKongSite = explode.GetValue2Int(1);
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
	else if (strcmp(explode[0], "�̵��ͼ") == 0) {
		strcpy(m_Setting.ShopMap, explode[1]);
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
	else if (strcmp(explode[0], "���ڸ����ſ�") == 0) {
		m_Setting.AtFBDoor = strcmp("��", explode[1]) == 0;
	}
	else if (strcmp(explode[0], "��������") == 0) {
		strcpy(m_Setting.GeZiText, explode[1]);
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
	else if (strcmp(explode[0], "��ʱ����") == 0) {
		Explode t("-", explode[1]);
		if (t[0]) {
			Explode* s = new Explode(":", t[0]);
			m_Setting.SwitchLogin_SH = s->GetValue2Int(0);
			m_Setting.SwitchLogin_SM = s->GetValue2Int(1);
			delete s;
		}
		if (t[1]) {
			Explode* e = new Explode(":", t[1]);
			m_Setting.SwitchLogin_EH = e->GetValue2Int(0);
			m_Setting.SwitchLogin_EM = e->GetValue2Int(1);
			delete e;
		}

		::printf("��ʱ����:%d:%d-%d:%d\n", m_Setting.SwitchLogin_SH, m_Setting.SwitchLogin_SM,
			m_Setting.SwitchLogin_EH, m_Setting.SwitchLogin_EM);
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
	::system("shutdown -s -t 10");
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
#if ISCMD
	return;
#endif

	my_msg* msg = GetMyMsg(MSG_SETSETTING);
	strcpy(msg->id, name);
	msg->value[0] = v;
	msg->value[1] = 0;
	PostMessage(m_hUIWnd, MSG_CALLJS, (WPARAM)msg, 0);
}

// ��������������º�
void Game::RestartApp()
{
	for (int i = 0; i < m_AccountList.size(); i++) {
		if (IsOnline(m_AccountList[i])) {
			LogOut(m_AccountList[i]);
		}

		SetStatus(m_AccountList[i], ACCSTA_INIT, true);
	}

	DWORD pids[20];
	DWORD len = SGetProcessIds(L"soul.exe", pids, sizeof(pids) / sizeof(DWORD));
	for (int i = 0; i < len; i++) {
		HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pids[i]);
		TerminateProcess(hProcess, 0);
		Sleep(500);
	}

	Sleep(5000);

	wchar_t path[MAX_PATH], exe[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, path);
	::GetModuleFileName(NULL, exe, MAX_PATH);
	::wsprintfW(path, L"%ws\\StartApp.exe", path);
	ShellExecute(NULL, L"open", path,
		exe, NULL, SW_SHOWNORMAL);

	Sleep(1000);
	::PostMessage(m_hUIWnd, WM_SYSCOMMAND, SC_CLOSE, NULL);
	TerminateProcess(GetCurrentProcess(), 0);
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
	else if (wcscmp(name, L"login_only_small") == 0) {
		m_Setting.OnlyLoginSmall = v;
		LOGVARN2(64, "cb", L"�޸�.ֻ��С��:%hs.", v ? "��" : "��");
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
	delete value;
}

// ����Ϸ
int Game::OpenGame(int index, int close_all)
{
#if 0
	Inject(61736, L"C:\\Users\\12028\\Desktop\\����\\Vs\\Team.dll", L"Team.dll");
	return 0;
#endif

	if (m_Setting.FBMode < 1 || m_Setting.FBMode > 4)
		m_Setting.FBMode = 1;

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
int Game::AutoPlay(int index, bool stop, bool trans)
{
	//m_pJsCall->SetBtnDisabled("start_btn", 1);
	if (stop) {
		SetLoginFlag(-2);
		//m_pJsCall->SetText("auto_play_btn", "�Զ��Ǻ�");
		//m_pJsCall->SetBtnDisabled("start_btn", 0);
		return 1;
	}

	if (IsAutoLogin())
		return 0;

	if (m_bLoging)
		return 0;

	if (trans && m_pTransaction) {
		AutoTransaction();
	}

	m_bLoging = true;
	SetLoginFlag(index);
	if (!AutoLogin("AutoPlay")) {
		LoginCompleted("�Զ��Ǻ�");
	}
	m_bLoging = false;

	return 1;
} 

// ����ʺ�
void Game::AddAccount(Account * account)
{
#if ISCMD
	return;
#endif

	CString  name = L"<b class='c3'>";
	name += account->Name;
	if (account->IsBig) {
		name += L"[���]";
	}
	if (account->IsTransaction) {
		name += L"[����]";
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

// ת�ƿ��ű���
void Game::GetInCard(const wchar_t * card)
{
#if ISCMD
	return;
#endif

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
void Game::VerifyCard(const wchar_t* card, const wchar_t* remark)
{
#if ISCMD
	return;
#endif

	char* value = wchar2char(card);
	char* value2 = wchar2char(remark);
	//printf("����:%hs : %hs\n", value, value2);

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

// ���³���汾
void Game::UpdateVer()
{
	CreateThread(NULL, NULL, m_funcUpdateVer, NULL, NULL, NULL);
}

// ����Ƿ�������ģ��
bool Game::CheckGameOtherModule()
{
	HANDLE hProcess = GetCurrentProcess();
	if (!hProcess) { // ��Ŀ����̣���þ��
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
		if (strstr(name, "9��")) {
			//printf("-----------------------\n");
			CHAR name_short[128] = { 0 };
			GetModuleBaseNameA(hProcess, hMods[i], name_short, sizeof(name_short));
			if (strcmp(name_short, "��������.exe")
				&& strcmp(name_short, "miniblink_x64.dll")
				&& strcmp(name_short, "web.dll")) {
				//::MessageBoxA(NULL, name, "t", MB_OK);
				result = false;
				//break;
			}
		}
		//printf("%s\n", name);
	}

	CloseHandle(hProcess);

	return result;
}

// �����ʺ�״̬
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

// �����ʺ�����ʱ��
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

// �����ռǵ�UI����
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

// ����
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

// ����
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

// ����
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

// ����
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

// ����״̬������
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

// ��ȡmsgָ��
my_msg * Game::GetMyMsg(int op)
{
	if (++m_nMsgIndex >= 100)
		m_nMsgIndex = 0;

	::ZeroMemory(&m_Msg[m_nMsgIndex], sizeof(my_msg));
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
#if 0
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
#endif
}

// CRCУ��
bool Game::ChCRC(bool loop)
{
	return true;
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
	return 0;
}
