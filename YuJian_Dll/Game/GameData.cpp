#include "Game.h"
#include "GameData.h"
#include "Emulator.h"
#include "Move.h"
#include "Talk.h"
#include "Item.h"
#include "Button.h"
#include "GameProc.h"
#include "PrintScreen.h"
#include <My/Common/func.h>
#include <My/Win32/Peb.h>
#include <time.h>

GameData::GameData(Game* p)
{
	m_pGame = p;
	m_pReadBuffer = new BYTE[0x10000];

	ZeroMemory(&m_DataAddr, sizeof(m_DataAddr));
}

// ��ȡ��ɫ�ַ�
bool GameData::GetRoleByPid(_account_* account, char* out, int len)
{
	DWORD h3drole = (DWORD)EnumModuleBaseAddr(account->GamePid, L"3drole.dll");
	HANDLE hp = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, account->GamePid);
	if (hp) {
		SIZE_T r;
		ReadProcessMemory(hp, (PVOID)(h3drole + ADDR_ROLE_NAME), out, len, &r);
		CloseHandle(hp);
	}
	else {
		::GetWindowTextA(account->Wnd.Role, out, len);
	}
	return false;
}

// ������Ϸ
int GameData::WatchGame(bool first)
{
	int count = 0;

	wchar_t log[128];
	while (true) {
		DWORD pids[10];
		DWORD len = SGetProcessIds(L"soul.exe", pids, sizeof(pids) / sizeof(DWORD));
		if (first) {
			DbgPrint("��Ϸ��������:%d\n", len);
			LOGVARP2(log, "c0", L"��Ϸ��������:%d", len);
		}
		
		for (int i = 0; i < len; i++) {
			int flag = 0;
			for (int j = 0; j < m_pGame->m_AccountList.size(); j++) {
				Account* pa = m_pGame->m_AccountList[j];
				if (pa->GamePid == pids[i]) {
					if (!m_pGame->CheckStatus(pa, ACCSTA_ONLINE) || !pa->IsReady) { // ��δ�����˻�δ׼����
						flag = -1;
						break;
					}
					if (pa->Addr.CoorX) { // �Ѹ���
						flag = 1;
						break;
					}
				}
			}

			if (flag == -1) {
				continue;
			}
			if (flag == 1) {
				count++;
				continue;
			}

			if (!m_pGame->m_pButton->FindGameWnd(pids[i]))
				continue;

			ZeroMemory(&m_DataAddr, sizeof(m_DataAddr));

			HWND gameWnd = m_pGame->m_pButton->FindGameWnd(pids[i]);
			HWND roleWnd = m_pGame->m_pButton->FindButtonWnd(gameWnd, STATIC_ID_ROLE);
			char role[128];
			DWORD coor[2];

#if IS_READ_MEM == 0
			::GetWindowTextA(roleWnd, role, sizeof(role));
			_account_* account = m_pGame->GetAccountByRole(role);
			if (account) {
				account->Wnd.Game = m_pGame->m_pButton->FindGameWnd(pids[i]);
				account->Wnd.Pic = ::FindWindowEx(account->Wnd.Game, NULL, NULL, NULL);
				account->Wnd.Map = m_pGame->m_pButton->FindButtonWnd(account->Wnd.Game, STATIC_ID_MAP);
				account->Wnd.PosX = m_pGame->m_pButton->FindButtonWnd(account->Wnd.Game, STATIC_ID_POS_X);
				account->Wnd.PosY = m_pGame->m_pButton->FindButtonWnd(account->Wnd.Game, STATIC_ID_POS_Y);

				ReadCoor(&coor[0], &coor[1], account);
				LOGVARP2(log, "c0 b", L"�ʺ�:%hs ��ɫ:%hs ����:%d,%d %08X", account->Name, role, coor[1], coor[0], pids[i]);

				m_pGame->SetStatus(account, ACCSTA_ONLINE, true);
				count++;
			}
#else

			DWORD h3drole = (DWORD)EnumModuleBaseAddr(pids[i], L"3drole.dll");
			m_hGameProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pids[i]);
			if (m_hGameProcess) {
				SIZE_T readlen = 0;
				char user[32] = { 0 };
				ReadProcessMemory(m_hGameProcess, (PVOID)(h3drole + ADDR_ACCOUNT_NAME), user, sizeof(user), &readlen);
				ReadProcessMemory(m_hGameProcess, (PVOID)(h3drole + ADDR_ROLE_NAME), role, sizeof(role), &readlen);
				ReadProcessMemory(m_hGameProcess, (PVOID)(h3drole + ADDR_COOR_Y_OFFSET), coor, sizeof(coor), &readlen);
				::GetWindowTextA(roleWnd, role, sizeof(role));
				
				LOGVARP2(log, "c0", L"\n�ʺ�:%hs ��ɫ:%hs ����:%d,%d %08X", user, role, coor[1], coor[0], pids[i]);

				Account* account = m_pGame->GetAccount(user);
				//Account* account = m_pGame->GetAccountByRole(role);
				if (account && role) {
					account->GamePid = pids[i];
					account->Process = m_hGameProcess;
					m_pAccountTmp = account;

					account->Wnd.Game = m_pGame->m_pButton->FindGameWnd(pids[i]);
					m_pGame->m_pGameProc->SwitchGameAccount(account);
					ReadGameMemory(0x01);

					account->Addr.Account = h3drole + ADDR_ACCOUNT_NAME;
					account->Addr.Role = h3drole + ADDR_ROLE_NAME;
					account->Addr.CoorX = h3drole + ADDR_COOR_X_OFFSET;
					account->Addr.CoorY = h3drole + ADDR_COOR_Y_OFFSET;
					account->Addr.LifeMax = h3drole + ADDR_LIFEMAX_OFFSET;
					account->Addr.Life = m_DataAddr.Life;
					account->Addr.QuickMagicNum = m_DataAddr.QuickMagicNum;
					account->Addr.QuickItemNum = m_DataAddr.QuickItemNum;
					account->Addr.ScreenX = m_DataAddr.ScreenX;
					account->Addr.ScreenY = m_DataAddr.ScreenY;

					account->Wnd.Pic = ::FindWindowEx(account->Wnd.Game, NULL, NULL, NULL);
					account->Wnd.Map = m_pGame->m_pButton->FindButtonWnd(account->Wnd.Game, STATIC_ID_MAP);
					account->Wnd.PosX = m_pGame->m_pButton->FindButtonWnd(account->Wnd.Game, STATIC_ID_POS_X);
					account->Wnd.PosY = m_pGame->m_pButton->FindButtonWnd(account->Wnd.Game, STATIC_ID_POS_Y);
					
					if (!m_DataAddr.Bag) {
						if (first) {
							::SetForegroundWindow(account->Wnd.Game);
						}
						
						m_pGame->m_pItem->OpenBag();
						Sleep(1000);
						m_pGame->m_pButton->Click(account->Wnd.Pic, BUTTON_ID_BAG_ITEM, "MPC��Ʒ��", 80, 10);
						Sleep(1000);
						m_pGame->m_pButton->Click(account->Wnd.Pic, BUTTON_ID_BAG_ITEM, "MPC��Ʒ��", 80, 10);
						Sleep(1000);
						ReadGameMemory(0x01);
						m_pGame->m_pItem->CloseBag();
					}
					account->Addr.Bag = m_DataAddr.Bag;
					
					if (account->IsBig) {
						if (!m_DataAddr.Storage) {
							m_pGame->m_pItem->OpenStorage();
							Sleep(1000);
							m_pGame->m_pButton->Click(account->Wnd.Pic, BUTTON_ID_CKIN_ITEM, "�洢��Ʒ��", 10, 10);
							Sleep(1000);
							m_pGame->m_pButton->Click(account->Wnd.Pic, BUTTON_ID_CKIN_ITEM, "�洢��Ʒ��", 10, 10);
							Sleep(1000);
							ReadGameMemory(0x01);
							m_pGame->m_pItem->CloseStorage();
						}
						account->Addr.Storage = m_DataAddr.Storage;
					}

					m_pGame->SetStatus(account, ACCSTA_ONLINE, true);
					count++;
				}
				else {
					CloseHandle(m_hGameProcess);
				}
			}
			else {
				DbgPrint("��Ȩ�޻�ȡ����[%d]\n", pids[i]);
				LOGVARP2(log, "c0", L"��Ȩ�޻�ȡ����[%d]", pids[i]);
			}
#endif
		}
		break;
		Sleep(8000);
	}
_end_:
	//LOGVARP2(log, "c0", L"count[%d]", count);
	return count;
}

// ��ȡ��Ϸ����
void GameData::FindGameWnd()
{
	::EnumWindows(EnumProc, (LPARAM)this);
}

// �����Ϸ��Ϣ
void GameData::FindGameInfo(_account_* account)
{
	::printf("1\n");
	wchar_t log[64];
	char role[128];
	DWORD coor[2];

	DWORD h3drole = (DWORD)EnumModuleBaseAddr(account->GamePid, L"3drole.dll");
	m_hGameProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, account->GamePid);
	if (m_hGameProcess) {
		SIZE_T readlen = 0;
		char user[32] = { 0 };
		ReadProcessMemory(m_hGameProcess, (PVOID)(h3drole + ADDR_ACCOUNT_NAME), user, sizeof(user), &readlen);
		ReadProcessMemory(m_hGameProcess, (PVOID)(h3drole + ADDR_ROLE_NAME), role, sizeof(role), &readlen);
		ReadProcessMemory(m_hGameProcess, (PVOID)(h3drole + ADDR_COOR_Y_OFFSET), coor, sizeof(coor), &readlen);

		LOGVARP2(log, "c0", L"\n�ʺ�[%d]:%hs ��ɫ:%hs ����:%d,%d %d", account->IsBig, user, role, coor[1], coor[0], account->GamePid);


		account->Process = m_hGameProcess;
		m_pAccountTmp = account;

		account->Wnd.Game = m_pGame->m_pButton->FindGameWnd(account->GamePid);
		m_pGame->m_pGameProc->SwitchGameAccount(account);
		ReadGameMemory(0x01);

		account->Addr.Account = h3drole + ADDR_ACCOUNT_NAME;
		account->Addr.Role = h3drole + ADDR_ROLE_NAME;
		account->Addr.CoorX = h3drole + ADDR_COOR_X_OFFSET;
		account->Addr.CoorY = h3drole + ADDR_COOR_Y_OFFSET;
		account->Addr.LifeMax = h3drole + ADDR_LIFEMAX_OFFSET;
		account->Addr.Life = m_DataAddr.Life;
		account->Addr.QuickMagicNum = m_DataAddr.QuickMagicNum;
		account->Addr.QuickItemNum = m_DataAddr.QuickItemNum;
		account->Addr.ScreenX = m_DataAddr.ScreenX;
		account->Addr.ScreenY = m_DataAddr.ScreenY;

		account->Wnd.Pic = ::FindWindowEx(account->Wnd.Game, NULL, NULL, NULL);
		account->Wnd.Map = m_pGame->m_pButton->FindButtonWnd(account->Wnd.Game, STATIC_ID_MAP);
		account->Wnd.PosX = m_pGame->m_pButton->FindButtonWnd(account->Wnd.Game, STATIC_ID_POS_X);
		account->Wnd.PosY = m_pGame->m_pButton->FindButtonWnd(account->Wnd.Game, STATIC_ID_POS_Y);

		::printf("big:%d %08X\n", account->IsBig, account->Addr.QuickItemNum);
		if (!m_DataAddr.Bag) {
			::SetForegroundWindow(account->Wnd.Game);

			m_pGame->m_pItem->OpenBag();
			Sleep(1000);
			m_pGame->m_pButton->Click(account->Wnd.Pic, BUTTON_ID_BAG_ITEM, "MPC��Ʒ��", 80, 10);
			Sleep(1000);
			m_pGame->m_pButton->Click(account->Wnd.Pic, BUTTON_ID_BAG_ITEM, "MPC��Ʒ��", 80, 10);
			Sleep(1000);
			ReadGameMemory(0x01);
			m_pGame->m_pItem->CloseBag();
		}
		account->Addr.Bag = m_DataAddr.Bag;

		::printf("big2:%d %08X\n", account->IsBig, account->Addr.QuickItemNum);
		if (account->IsBig) {
			while (!account->Addr.QuickItemNum) {
				Sleep(500);
				LOGVARP2(log, "c0", L"%hs Say �޷���ȡ��Ʒ����, ����...", role);
				ReadGameMemory(0x01);
				account->Addr.QuickItemNum = m_DataAddr.QuickItemNum;
			}
			if (!m_DataAddr.Storage) {
				m_pGame->m_pItem->OpenStorage();
				Sleep(1000);
				m_pGame->m_pButton->Click(account->Wnd.Pic, BUTTON_ID_CKIN_ITEM, "�洢��Ʒ��", 10, 10);
				Sleep(1000);
				m_pGame->m_pButton->Click(account->Wnd.Pic, BUTTON_ID_CKIN_ITEM, "�洢��Ʒ��", 10, 10);
				Sleep(1000);
				ReadGameMemory(0x01);
				m_pGame->m_pItem->CloseStorage();
			}
			account->Addr.Storage = m_DataAddr.Storage;
		}
	}
	else {
		DbgPrint("��Ȩ�޻�ȡ����[%d]\n", account->GamePid);
		LOGVARP2(log, "c0", L"��Ȩ�޻�ȡ����[%d]", account->GamePid);
	}
}

// ö�ٴ���
BOOL GameData::EnumProc(HWND hWnd, LPARAM lParam)
{
	return TRUE;
}

// �Ƿ���ָ���������� allow=���
bool GameData::IsInArea(int x, int y, int allow, _account_* account)
{
	DWORD pos_x = 0, pos_y = 0;
	ReadCoor(&pos_x, &pos_y, account);
	if (allow == 0)
		return x == pos_x && y == pos_y;

	int cx = (int)pos_x - x;
	int cy = (int)pos_y - y;

	printf("IsInArea:%d,%d %d,%d\n", pos_x, pos_y, cx, cy);

	return abs(cx) <= allow && abs(cy) <= allow;
}

// �Ƿ���ָ���������� allow=���
bool GameData::IsNotInArea(int x, int y, int allow, _account_* account)
{
	ReadCoor(nullptr, nullptr, account);
	int cx = (int)m_dwX - x;
	int cy = (int)m_dwY - y;

	return abs(cx) > allow || abs(cy) > allow;
}

// �Ƿ������
bool GameData::IsInShenDian(_account_* account)
{
	return IsInArea(60, 60, 30, account);
}

// �Ƿ�����������
bool GameData::IsInShenYu(_account_ * account)
{
	return IsInArea(600, 300, 30, account);
}

// �Ƿ��ڸ����ſ�
bool GameData::IsInFBDoor(_account_* account)
{
	return IsInArea(865, 500, 50, account);
}

bool GameData::IsTheMap(const char* map, _account_* account)
{
	account = account ? account : m_pGame->m_pBig;
	char string[32];
	::GetWindowTextA(account->Wnd.Map, string, sizeof(string));

	return strstr(string, map) != nullptr;
}

// ��ȡ�����׵�ַ
bool GameData::FindQuickAddr(int flag)
{
	if (0 && m_dwReadBase > 0x12600000 && m_dwReadBase < 0x12656910)
		::printf("FindQuickAddr %08X.\n", m_dwReadBase);
	// 4:0xCA000000 4:0xCA000000 4:0xCA000000 4:0x01 4:0x00 4:0x00 4:0x136 4:0x20
	DWORD codes[] = {
		0xCA000000, 0xCA000000, 0xCA000000, 0x00000001,
		0x00000000, 0x00000000, 0x00000136, 0x00000020,
	};
	DWORD address[2] = { 0, 0 };
	int length = SearchCode(codes, sizeof(codes) / sizeof(DWORD), address, sizeof(address) / sizeof(DWORD));
	if (length > 0) {
		printf("FindQuickAddr Success %d.\n", length);
		for (int i = 0; i < length; i++) {
			DWORD type_addr = address[i] + 0x20 + 0x200;
			DWORD type = 0;
			SIZE_T readlen = 0;
			ReadProcessMemory(m_hGameProcess, (PVOID)type_addr, &type, sizeof(type), &readlen);
			int v02 = 0;
			for (int i = 0; i <= 24; i += 8) {
				if (((type >> i) & 0xff) == 0x02) {
					v02++;
				}
			}
			LOGVARN2(64, "blue", L"(%hs)type:%08X", m_pAccountTmp->Name, type);
			if (v02 >= 2) {
				m_DataAddr.QuickMagicNum = address[i] + 0x20;
				DbgPrint("(%s)���ܿ��������:%08X\n", m_pAccountTmp->Name, m_DataAddr.QuickMagicNum);
				LOGVARN2(64, "blue", L"(%hs)���ܿ��������:%08X", m_pAccountTmp->Name, m_DataAddr.QuickMagicNum);
			}
			else {
				m_DataAddr.QuickItemNum = address[i] + 0x20;
				DbgPrint("(%s)��Ʒ���������:%08X\n", m_pAccountTmp->Name, m_DataAddr.QuickItemNum);
				LOGVARN2(64, "blue", L"(%hs)��Ʒ���������:%08X", m_pAccountTmp->Name, m_DataAddr.QuickItemNum);

				int yao[2] = { 0, 0 };
				m_pGame->m_pGameData->ReadMemory((PVOID)m_DataAddr.QuickItemNum, yao, sizeof(yao), m_pAccountTmp);
				LOGVARN2(64, "blue", L"(%hs)ҩ/��:{%d/%d}", m_pAccountTmp->Name, yao[0], yao[1]);
			}
			
		}
		m_pGame->WriteLog("FindQuickAddr Success p.\n");
	}

	return address != 0;
}

// ��ȡĿ�ĵ������ַ
bool GameData::FindLifeAddr()
{
	m_pGame->WriteLog("FindLifeAddr.\n");
	//DbgPrint("FindLifeAddr.\n");
	// 4:0x00 4:0x00 4:* 4:0x03 4:0x0A 4:0x18 4:0x29 4:0x00
	DWORD codes[] = {
		0x00000003, 0x0000000A, 0x00000018, 0x00000029, 0x00000000,
	};
	DWORD address = 0;
	if (SearchCode(codes, sizeof(codes) / sizeof(DWORD), &address, 1, 1)) {
		m_DataAddr.Life = address + 0x18;
		DbgPrint("(%s)Ѫ����ַ:%08X\n", m_pAccountTmp->Name, m_DataAddr.Life);
		LOGVARN2(64, "blue", L"(%hs)Ѫ����ַ:%08X", m_pAccountTmp->Name, m_DataAddr.Life);
	}
	return address > 0;
}

// ��ȡ������Ʒ��ַ
bool GameData::FindBagAddr()
{
	m_pGame->WriteLog("FindBagAddr.\n");
	DWORD codes[] = {
		0x00000022, 0x00000022, 0x00000A04, 0x00000005,
		0x00000005, 0x00001E1E, 0x00000022, 0x00000022,
	};
	DWORD address = 0;
	if (SearchCode(codes, sizeof(codes) / sizeof(DWORD), &address)) {
		m_DataAddr.Bag = address + 0x40;
		DbgPrint("(%s)������Ʒ��ַ:%08X\n", m_pAccountTmp->Name, m_DataAddr.Bag);
		LOGVARN2(64, "blue", L"(%hs)������Ʒ��ַ:%08X", m_pAccountTmp->Name, m_DataAddr.Bag);
	}

	return address != 0;
}

// ��ȡ�ֿ���Ʒ��ַ
bool GameData::FindStorageAddr()
{
	m_pGame->WriteLog("FindStorageAddr.\n");
	DWORD codes[] = {
		0x00000022, 0x00000022, 0x00000A06, 0x00000004,
		0x00000004, 0x00001F1F, 0x00000022, 0x00000022,
	};
	DWORD address = 0;
	if (SearchCode(codes, sizeof(codes) / sizeof(DWORD), &address)) {
		m_DataAddr.Storage = address + 0x40;

		DbgPrint("(%s)�ֿ���Ʒ��ַ:%08X\n", m_pAccountTmp->Name, m_DataAddr.Storage);
		LOGVARN2(64, "blue", L"(%hs)�ֿ���Ʒ��ַ:%08X", m_pAccountTmp->Name, m_DataAddr.Storage);
	}

	return address != 0;
}

// ��ȡ������Ļ����
bool GameData::FindScreenPos()
{
	m_pGame->WriteLog("FindScreenPos.\n");
	// 4:0x0002080E 4:0x00077B1B 4:0x00000000 4:0x00000000 4:0x00000000 4:0x00000000 4:0x00095E98 4:0x00000000
	// 4:0x00020804 4:0x00077B0E 4:0x00000000 4:0x00000000 4:0x00000000 4:0x00000000 4:0x00000000 4:0x00000000
	DWORD codes[] = { // WIN10
		0x0002080E, 0x00077B1B, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00095E98, 0x00000000,
	};
	DWORD address = 0;
	bool result = SearchCode(codes, sizeof(codes) / sizeof(DWORD), &address);
	if (!result) { // WIN7
		ZeroMemory(codes, sizeof(codes)); 
		codes[0] = 0x00020804;
		codes[1] = 0x00077B0E;
		result = SearchCode(codes, sizeof(codes) / sizeof(DWORD), &address);
	}
	if (result) {
		SIZE_T readlen = 0;
		DWORD data[8] = { 0 };
		bool result = ReadProcessMemory(m_hGameProcess, LPVOID(address), &data, sizeof(data), &readlen);
		if (1) {
			m_DataAddr.ScreenX = address - 0x60;
			m_DataAddr.ScreenY = m_DataAddr.ScreenX + 4;

			DbgPrint("(%s)������Ļ��ַ:%08X\n", m_pAccountTmp->Name, m_DataAddr.ScreenX);
			LOGVARN2(64, "blue", L"(%hs)������Ļ��ַ:%08X", m_pAccountTmp->Name, m_DataAddr.ScreenX);
		}
	}

	return address != 0;
}

// ��ȡ����������ֵ��ַ
bool GameData::FindPicScale()
{
	// 0x00000022, 0x00000600, 0x00000360, 0x12340000,
	DWORD codes[] = {
		0x00000000, 0x00000000, 0x00000000, 0x3F800000,
		0x00000022, 0x00001234, 0x00001234, 0x12340000,
	};
	DWORD address = 0;
	if (SearchCode(codes, sizeof(codes) / sizeof(DWORD), &address)) {
		DWORD data[4] = { 0 };
		bool result = ReadProcessMemory(m_hGameProcess, LPVOID(address + 0x10), &data, sizeof(data), NULL);
		//LOGVARN2(32, "blue", L"�������ŵ�ַf:%08X", address + 0x10);
		if (0 && address > 0x4DEC9500 && address < 0x4DEC9600) {
			printf("�������ŵ�ַf:%08X %d %d %d\n", address + 0x10, data[0], data[1], data[2]);
		}
		
		if (data[0] >= 128 && data[0] <= 256 
			&& data[1] >= 0x300 && data[1] <= 0x600 
			&& data[2] >= 0x1B0 && data[2] <= 0x360) {
			m_DataAddr.PicScale = address + 0x10;

			LOGVARN2(64, "blue", L"�������ŵ�ַ:%08X", m_DataAddr.PicScale);
		}
	}

	return address != 0;
}

// ��ȡ��ɫ
bool GameData::ReadName(char* name, _account_* account)
{
	account = account ? account : m_pGame->m_pBig;
	if (!ReadMemory((PVOID)account->Addr.Role, name, 16, account)) {
		DbgPrint("�޷���ȡ��ɫ����(%d)\n", GetLastError());
		LOGVARN2(64, "red", L"�޷���ȡ��ɫ����(%d)", GetLastError());
		return false;
	}
	return true;
}

// ��ȡ����
bool GameData::ReadCoor(DWORD * x, DWORD * y, _account_* account)
{
	account = account ? account : m_pGame->m_pBig;

	DWORD pos_x = 0, pos_y = 0;

#if IS_READ_MEM == 0
	pos_x = FormatCoor(account->Wnd.PosX);
	pos_y = FormatCoor(account->Wnd.PosY);
#else
	if (!account->Addr.CoorX || !account->Addr.CoorY) {
		pos_x = FormatCoor(account->Wnd.PosX);
		pos_y = FormatCoor(account->Wnd.PosY);
	}
	else {
		if (!ReadDwordMemory(account->Addr.CoorX, pos_x, account)) {
			::printf("�޷���ȡ����X(%d) %08X\n", GetLastError(), account->Addr.CoorX);
			return false;
		}
		if (!ReadDwordMemory(account->Addr.CoorY, pos_y, account)) {
			::printf("�޷���ȡ����Y(%d) %08X\n", GetLastError(), account->Addr.CoorY);
			return false;
		}
	}
#endif

	m_dwX = pos_x;
	m_dwY = pos_y;

	if (x) {
		*x = pos_x;
	}
	if (y) {
		*y = pos_y;
	}

	return true;
}

// ��ȡ����
bool GameData::ReadCoorByWnd(DWORD* x, DWORD* y, _account_* account)
{
	account = account ? account : m_pGame->m_pBig;

	DWORD pos_x = FormatCoor(account->Wnd.PosX);
	DWORD pos_y = FormatCoor(account->Wnd.PosY);

	m_dwX = pos_x;
	m_dwY = pos_y;

	if (x) {
		*x = pos_x;
	}
	if (y) {
		*y = pos_y;
	}

	return true;
}

// ������������
int GameData::FormatCoor(HWND hWnd)
{
	int val = 0;
	char text[128] = { 0 };
	::GetWindowTextA(hWnd, text, sizeof(text));
	//::printf("FormatCoor:%s.\n", text);
	char* str = strstr(text, "}");
	if (str) {
		val = atoi(str + 1);
	}

	return val;
}

// ��ȡ��Ļ����
bool GameData::ReadScreenPos(int& x, int& y, _account_* account)
{
	x = 720;
	y = 450;
	return true;
#if IS_READ_MEM == 0
	x = 720;
	y = 450;
#else
	account = account ? account : m_pGame->m_pBig;
	int data[2];
	ReadMemory((PVOID)account->Addr.ScreenX, data, sizeof(data), account);
	x = data[0];
	y = data[1];
#endif

	return false;
}

// ��ȡ����ֵ
DWORD GameData::ReadLife(DWORD* life, DWORD* life_max, _account_* account)
{
	account = account ? account : m_pGame->m_pBig;
	ReadDwordMemory(account->Addr.Life, m_dwLife, account);
	ReadDwordMemory(account->Addr.LifeMax, m_dwLifeMax, account);

	if (life) {
		*life = m_dwLife;
	}
	if (life_max) {
		*life_max = m_dwLifeMax;
	}
	return m_DataAddr.Life;
}

// ����������
DWORD GameData::SearchCode(DWORD* codes, DWORD length, DWORD* save, DWORD save_length, DWORD step)
{
	if (length == 0 || save_length == 0)
		return 0;

	DWORD count = 0;
	for (DWORD i = 0; i < m_dwReadSize; i += step) {
		if ((i + length) > m_dwReadSize)
			break;

		DWORD* dw = (DWORD*)(m_pReadBuffer + i);
		DWORD addr = m_dwReadBase + i;
		if (addr == (DWORD)codes) { // �����Լ�
			//::printf("���������Լ�:%08X %08X\n", codes, codes[0]);
			//return 0;
		}

		if (0 && addr == (DWORD)0x12656910) {
			::printf("0x12656910:%08X\n", dw[0]);
			for (int xxx = 0; xxx < 8; xxx++) {
				::printf("%08X ", dw[xxx]);
			}
			::printf("\n");
		}
		bool result = true;
		for (DWORD j = 0; j < length; j++) {
			if (0 && addr == (DWORD)0x12656910) {
				::printf("0x12656910:%08X == %08X %d\n", dw[j], codes[j], j);
			}
			if (codes[j] == 0x11) { // �����
				result = true;
			}
			else if (codes[j] == 0x22) { // ��Ҫ��ֵ��Ϊ0
				if (dw[j] == 0) {
					if (0 && addr == (DWORD)0x12656910) {
						::printf("0x12656910:%08X != %08X %d 1\n", dw[j], codes[j], j);
					}
					result = false;
					break;
				}
			}
			else if (((codes[j] & 0xffff0000) == 0x12340000)) { // ��2�ֽ����
				if ((dw[j] & 0x0000ffff) != (codes[j] & 0x0000ffff)) {
					if (0 && addr == (DWORD)0x12656910) {
						::printf("0x12656910:%08X != %08X %d 2\n", dw[j], codes[j], j);
					}
					result = false;
					break;
				}
				else {
					//::printf("%08X\n", dw[j]);
				}
			}
			else if (((codes[j] & 0x0000ffff) == 0x00001234)) { // ��2�ֽ����
				if ((dw[j] & 0xffff0000) != (codes[j] & 0xffff0000)) {
					if (0 && addr == (DWORD)0x12656910) {
						::printf("0x12656910:%08X != %08X %d 3\n", dw[j], codes[j], j);
					}
					result = false;
					break;
				}
			}
			else {
				if ((codes[j] & 0xffffff00) == 0x00001100) { // �Ƚ�������ַ��ֵ��� ���8λΪ�Ƚ�����
					//::printf("%X:%X %08X:%08X\n", j, codes[j] & 0xff, dw[j], dw[codes[j] & 0xff]);
					if (dw[j] != dw[codes[j] & 0xff]) {
						if (0 && addr == (DWORD)0x12656910) {
							::printf("0x12656910:%08X != %08X %d 4\n", dw[j], codes[j], j);
						}
						result = false;
						break;
					}
				}
				else if (dw[j] != codes[j]) { // ������ֵ�����
					if (0 && m_dwReadBase >= 0x247DD000 && m_dwReadBase < 0x247DE000 && i == 0) {
						printf("%d.m_dwReadBase!=0x4F67C000 %08X=%08X 5\n", j, dw[j], codes[j]);
					}
					if (0 && addr == (DWORD)0x12656910) {
						::printf("0x12656910:%08X != %08X %d \n", dw[j], codes[j], j);
					}
					result = false;
					break;
				}
			}
		}
		if (0 && addr == (DWORD)0x12656910) {
			::printf("0x12656910:%08X %d\n", dw[0], result);
		}
		if (result) {
			m_pGame->WriteLog("result.\n");
			save[count++] = addr;
			m_pGame->WriteLog("result end.\n");
			//::printf("��ַ:%08X   %08X\n", addr, codes);
			if (count == save_length)
				break;
		}
	}

	return count;
}

// ����������
DWORD GameData::SearchByteCode(BYTE * codes, DWORD length)
{
	if (length == 0)
		return 0;

	for (DWORD i = 0; i < m_dwReadSize; i++) {
		if ((i + length) > m_dwReadSize)
			break;

		DWORD addr = m_dwReadBase + i;
		if (addr == (DWORD)codes) { // �����Լ�
			//::printf("���������Լ�:%08X\n", codes);
			return 0;
		}

		BYTE* data = (m_pReadBuffer + i);
		bool result = true;
		for (DWORD j = 0; j < length; j++) {
			if (codes[j] == 0x11) { // �����
				result = true;
			}
			else if (codes[j] == 0x22) { // ��Ҫ��ֵ��Ϊ0
				if (data[j] == 0) {
					result = false;
					break;
				}
			}
			else if (codes[j] != data[j]) {
				result = false;
				break;
			}
		}

		if (result)
			return addr;
	}

	return 0;
}

// ��ȡ���ֽ�����
bool GameData::ReadDwordMemory(DWORD addr, DWORD& v, _account_* account)
{
	return ReadMemory((PVOID)addr, &v, 4, account);
}

// ��ȡ�ڴ�
bool GameData::ReadMemory(PVOID addr, PVOID save, DWORD length, _account_* account)
{
	if (!account)
		account = m_pGame->m_pBig;
	if (!account)
		return false;

	SIZE_T dwRead = 0;
	bool result = ReadProcessMemory(account->Process, addr, save, length, &dwRead);
	//::printf("ReadProcessMemory:%d %08X %d Read:%d ��ֵ:%d(%08X)\n", GetLastError(), addr, result, dwRead, *(DWORD*)save, *(DWORD*)save);

	if (!result || dwRead != length) {
		::printf("ReadProcessMemory����:%d %08X %d\n", GetLastError(), addr, result);
		if (GetLastError() == 6) {
			m_hProcessBig = GetCurrentProcess();
			return ReadProcessMemory(m_hProcessBig, addr, save, length, NULL);
		}
	}

	return result;
}


// ��ȡ��Ϸ�ڴ�
bool GameData::ReadGameMemory(DWORD flag)
{
	m_bIsReadEnd = false;

	MEMORY_BASIC_INFORMATION mbi;
	memset(&mbi, 0, sizeof(MEMORY_BASIC_INFORMATION));
	DWORD_PTR MaxPtr = 0x70000000; // ����ȡ�ڴ��ַ
	DWORD_PTR max = 0;


	DWORD ms = GetTickCount();
	DWORD_PTR ReadAddress = 0x00000000;
	ULONG count = 0, failed = 0;
	//::printf("fuck\n");
	while (ReadAddress < MaxPtr)
	{
		SIZE_T r = VirtualQueryEx(m_hGameProcess, (LPCVOID)ReadAddress, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
		SIZE_T rSize = 0;

		//::printf("r:%d\n", r);
		//::printf("ReadAddress:%08X - %08X-----%X\n", ReadAddress, ReadAddress + mbi.RegionSize, mbi.RegionSize);
		if (r == 0) {
			::printf("r:%d -- %p\n", (int)r, ReadAddress);
			break;
		}

		if (mbi.Type != MEM_PRIVATE && mbi.Protect != PAGE_READWRITE) {
			//::printf("%p-%p ===����, ��С:%d %08X %08X\n", ReadAddress, ReadAddress + mbi.RegionSize, mbi.RegionSize, mbi.Type, mbi.Protect);
			goto _c;
		}
		else {
			DWORD pTmpReadAddress = ReadAddress;
			DWORD dwOneReadSize = 0x10000; // ÿ�ζ�ȡ����
			DWORD dwReadSize = 0x00;      // �Ѷ�ȡ����
			while (dwReadSize < mbi.RegionSize) {
				m_dwReadBase = pTmpReadAddress;
				m_dwReadSize = (dwReadSize + dwOneReadSize) <= mbi.RegionSize
					? dwOneReadSize : mbi.RegionSize - dwReadSize;

				SIZE_T write_len = 0;
				if (ReadProcessMemory(m_hGameProcess, (LPVOID)pTmpReadAddress, m_pReadBuffer, m_dwReadSize, &write_len)) {
					//::printf("flag:%08X %p-%p\n", flag, ReadAddress, ReadAddress + mbi.RegionSize);

					if (flag & 0x01) {
						DWORD find_num = 5;
						if (!m_DataAddr.Player) {
							find_num -= 2;
							if (FindQuickAddr())
								find_num++;
							if (FindLifeAddr())
								find_num++;
						}
						if (!m_DataAddr.ScreenX) {
							find_num -= 1;
							if (FindScreenPos())
								find_num++;
						}
						if (!m_DataAddr.Bag) {
							find_num -= 1;
							if (FindBagAddr())
								find_num++;
						}
						if (!m_DataAddr.Storage) {
							find_num -= 1;
							if (FindStorageAddr())
								find_num++;
						}
						if (find_num == 5)
							flag = 0;
					}
					if (!flag) {
						::printf("������ȫ���ҵ�\n");
						goto end;
					}
				}
				else {
					//::printf("%p-%p === ��ȡʧ��, ��С:%d, ������:%d\n", pTmpReadAddress, pTmpReadAddress + m_dwReadSize, (int)m_dwReadSize, GetLastError());
				}

				dwReadSize += m_dwReadSize;
				pTmpReadAddress += m_dwReadSize;
			}

			count++;
		}
	_c:
		// ��ȡ��ַ����
		ReadAddress += mbi.RegionSize;
		//Sleep(8);
		//::printf("%016X---�ڴ��С2:%08X\n", ReadAddress, mbi.RegionSize);
		// ɨ0x10000000�ֽ��ڴ� ����100����
	}
end:
	DWORD ms2 = GetTickCount();
	DbgPrint("---��ʱ:%d����---\n", ms2 - ms);
	LOGVARN2(64, "c0", L"---��ʱ:%d����---", ms2 - ms);
	m_bIsReadEnd = true;
	return true;
}