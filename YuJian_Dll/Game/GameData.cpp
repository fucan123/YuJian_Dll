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

// ������Ϸ
void GameData::WatchGame()
{
	wchar_t log[128];
	while (true) {
		DWORD pids[10];
		DWORD len = SGetProcessIds(L"soul.exe", pids, sizeof(pids) / sizeof(DWORD));
		DbgPrint("��Ϸ��������:%d\n", len);
		for (int i = 0; i < len; i++) {
			DWORD h3drole = (DWORD)EnumModuleBaseAddr(pids[i], L"3drole.dll");
			m_hGameProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pids[i]);
			if (m_hGameProcess) {
				SIZE_T readlen = 0;
				char role[32] = { 0 };
				DWORD coor[2];
				ReadProcessMemory(m_hGameProcess, (PVOID)(h3drole + ADDR_ROLE_NAME), role, sizeof(role), &readlen);
				ReadProcessMemory(m_hGameProcess, (PVOID)(h3drole + ADDR_COOR_Y_OFFSET), coor, sizeof(coor), &readlen);
				
				DbgPrint("��ɫ:%s ����:%d,%d\n", role, coor[1], coor[0]);

				Account* account = m_pGame->GetAccountByRole(role);
				if (account && role) {
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

					account->Wnd.Game = m_pGame->m_pButton->FindGameWnd(pids[i]);
					account->Wnd.Pic = ::FindWindowEx(account->Wnd.Game, NULL, NULL, NULL);

					account->Process = m_hGameProcess;

					if (!m_DataAddr.Bag) {
						m_pGame->m_pItem->OpenBag();
						Sleep(2000);
						m_pGame->m_pButton->Click(account->Wnd.Pic, BUTTON_ID_BAG_ITEM, "MPC��Ʒ��", 80, 10);
						Sleep(2000);
						m_pGame->m_pButton->Click(account->Wnd.Pic, BUTTON_ID_BAG_ITEM, "MPC��Ʒ��", 80, 10);
						Sleep(2000);
						ReadGameMemory(0x01);
						m_pGame->m_pItem->CloseBag();
					}

					account->Addr.Bag = m_DataAddr.Bag;

					DbgPrint("%d.�ҵ���ɫ:%s ����:%08X(%08X)\n", pids[i], role, account->Wnd.Game, account->Wnd.Pic);
					goto _end_;
				}
			}
			CloseHandle(m_hGameProcess);
		}
		Sleep(8000);
	}
_end_:
	return;
}

// ��ȡ��Ϸ����
void GameData::FindGameWnd()
{
	::EnumWindows(EnumProc, (LPARAM)this);
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

	//printf("IsInArea:%d,%d %d,%d\n", pos_x, pos_y, cx, cy);

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

// ��ȡ�����׵�ַ
bool GameData::FindQuickAddr()
{
	// 4:0xCA000000 4:0xCA000000 4:0xCA000000 4:0x01 4:0x00 4:0x00 4:0x136 4:0x20
	DWORD codes[] = {
		0xCA000000, 0xCA000000, 0xCA000000, 0x00000001,
		0x00000000, 0x00000000, 0x00000136, 0x00000020,
	};
	DWORD address = 0;
	if (SearchCode(codes, sizeof(codes) / sizeof(DWORD), &address)) {
		DWORD type_addr = address + 0x20 + 0x200;
		DWORD type = 0;
		SIZE_T readlen = 0;
		ReadProcessMemory(m_hGameProcess, (PVOID)type_addr, &type, sizeof(type), &readlen);
		if (type == 0x02020202) {
			m_DataAddr.QuickMagicNum = address + 0x20;
			LOGVARN2(32, "blue", L"���ܿ��������:%08X", m_DataAddr.QuickMagicNum);
		}
		else {
			m_DataAddr.QuickItemNum = address + 0x20;
			LOGVARN2(32, "blue", L"��Ʒ���������:%08X", m_DataAddr.QuickItemNum);
		}
	}

	return address != 0;
}

// ��ȡĿ�ĵ������ַ
bool GameData::FindLifeAddr()
{
	// 4:0x00 4:0x00 4:* 4:0x03 4:0x0A 4:0x18 4:0x29 4:0x00
	DWORD codes[] = {
		0x00000000, 0x00000000, 0x00000011, 0x00000003,
		0x0000000A, 0x00000018, 0x00000029, 0x00000000,
	};
	DWORD address = 0;
	if (SearchCode(codes, sizeof(codes) / sizeof(DWORD), &address, 1, 1)) {
		m_DataAddr.Life = address + 0x24;
		LOGVARN2(32, "blue", L"Ѫ����ַ:%08X", m_DataAddr.Life);
	}
	return address > 0;
}

// ��ȡ������Ʒ��ַ
bool GameData::FindBagAddr()
{
	// 4:0x0002080E 4:0x00077B1B 4:0x00000000 4:0x00000000 4:0x00000000 4:0x00000000 4:0x00095E98 4:0x00000000
	DWORD codes[] = {
		0x00000022, 0x00000022, 0x00000A04, 0x00000005,
		0x00000005, 0x00001E1E, 0x00000022, 0x00000022,
	};
	DWORD address = 0;
	if (SearchCode(codes, sizeof(codes) / sizeof(DWORD), &address)) {
		m_DataAddr.Bag = address + 0x40;

		DbgPrint("������Ʒ��ַ:%08X\n", m_DataAddr.Bag);
		LOGVARN2(32, "blue", L"������Ʒ��ַ:%08X", m_DataAddr.Bag);
	}

	return address != 0;
}

// ��ȡ������Ļ����
bool GameData::FindScreenPos()
{
	// 4:0x0002080E 4:0x00077B1B 4:0x00000000 4:0x00000000 4:0x00000000 4:0x00000000 4:0x00095E98 4:0x00000000
	DWORD codes[] = {
		0x0002080E, 0x00077B1B, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00095E98, 0x00000000,
	};
	DWORD address = 0;
	if (SearchCode(codes, sizeof(codes) / sizeof(DWORD), &address)) {
		SIZE_T readlen = 0;
		DWORD data[8] = { 0 };
		bool result = ReadProcessMemory(m_hGameProcess, LPVOID(address), &data, sizeof(data), &readlen);
		if (1) {
			m_DataAddr.ScreenX = address - 0x60;
			m_DataAddr.ScreenY = m_DataAddr.ScreenX + 4;

			DbgPrint("������Ļ��ַ:%08X\n", m_DataAddr.ScreenX);
			LOGVARN2(32, "blue", L"������Ļ��ַ:%08X", m_DataAddr.ScreenX);
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

			LOGVARN2(32, "blue", L"�������ŵ�ַ:%08X", m_DataAddr.PicScale);
		}
	}

	return address != 0;
}

// ��ȡ��ɫ
bool GameData::ReadName(char* name, _account_* account)
{
	account = account ? account : m_pAccoutBig;
	if (!ReadMemory((PVOID)account->Addr.Role, name, 16, account)) {
		DbgPrint("�޷���ȡ��ɫ����(%d)\n", GetLastError());
		LOGVARN2(32, "red", L"�޷���ȡ��ɫ����(%d)", GetLastError());
		return false;
	}
	return true;
}

// ��ȡ����
bool GameData::ReadCoor(DWORD * x, DWORD * y, _account_* account)
{
	account = account ? account : m_pAccoutBig;
	if (!account->Addr.CoorX || !account->Addr.CoorY)
		return false;

	DWORD pos_x = 0, pos_y = 0;
	if (!ReadDwordMemory(account->Addr.CoorX, pos_x, account)) {
		::printf("�޷���ȡ����X(%d) %08X\n", GetLastError(), account->Addr.CoorX);
		return false;
	}
	if (!ReadDwordMemory(account->Addr.CoorY, pos_y, account)) {
		::printf("�޷���ȡ����Y(%d) %08X\n", GetLastError(), account->Addr.CoorY);
		return false;
	}

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

// ��ȡ��Ļ����
bool GameData::ReadScreenPos(int& x, int& y, _account_ * account)
{
	account = account ? account : m_pAccoutBig;
	int data[2];
	ReadMemory((PVOID)account->Addr.ScreenX, data, sizeof(data), account);
	x = data[0];
	y = data[1];

	return true;
}

// ��ȡ����ֵ
DWORD GameData::ReadLife(DWORD* life, DWORD* life_max, _account_* account)
{
	account = account ? account : m_pAccoutBig;
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

// ���������ڴ�
void GameData::CreateShareMemory()
{
	m_hShareMap = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024, L"Share_Write_XY");
	if (!m_hShareMap) {
		m_pShareBuffer = nullptr;
		DbgPrint("CreateFileMappingʧ��\n");
		LOGVARN2(32, "red", L"CreateFileMappingʧ��(%d)", GetLastError());
		return;
	}
	// ӳ������һ����ͼ���õ�ָ�����ڴ��ָ�룬�������������
	m_pShareBuffer = (ShareWriteXYData*)::MapViewOfFile(m_hShareMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	// ��ʼ��
	ZeroMemory(m_pShareBuffer, 1024);
}

// д��Ŀ�ĵ�
void GameData::WriteMoveCoor(DWORD x, DWORD y, _account_* account)
{
	account = account ? account : m_pAccoutBig;
	if (!account || !account->Mnq)
		return;

	DWORD old[2] = { 0, 0 };
	if (ReadMemory((PVOID)account->Addr.MoveX, old, sizeof(old))) {
		//printf("old:%d,%d\n", old[0], old[1]);
		if (old[0] == x && old[1] == y)
			return;
	}

	SIZE_T write_len = 0;
	DWORD data[] = { x, y };

	DWORD old_p;
	//VirtualProtect((PVOID)m_DataAddr.MoveX, sizeof(data), PAGE_READWRITE, &old_p);
	
	if (0 && !m_bInDll && WriteProcessMemory(account->Mnq->Process, (PVOID)account->Addr.MoveX, data, sizeof(data), &write_len)) {
		//printf("WriteMoveCoor����:%d\n", write_len);
	}
	else {
		wchar_t log[64];
		//::printf("�޷�д��Ŀ������(%d) %08X\n", GetLastError(), account->Addr.MoveX);
		if (!m_pShareBuffer) {
			DbgPrint("�޷���������\n");
			LOGVARP2(log, "red", L"�޷���������(!m_pShareBuffer.%d)", GetLastError());
			return;
		}

		if (!m_bInDll) {
#if 0
			char cmd[128];
			sprintf_s(cmd, "%s\\win7\\\WriteXY.exe %d", m_pGame->m_chPath, account->Mnq->VBoxPid);
			printf("����WriteXY����ע��DLL:%hs\n", cmd);
			system(cmd);
#else
			wchar_t dll[256];
			wsprintfW(dll, L"%hs\\files\\wxy.dll", m_pGame->m_chPath);
			InjectDll(account->Mnq->VBoxPid, dll, NULL, FALSE);
#endif
			m_bInDll = true;
			Sleep(100);
		}

		// ӳ������һ����ͼ���õ�ָ�����ڴ��ָ�룬�������������
		m_pShareBuffer = (ShareWriteXYData*)::MapViewOfFile(m_hShareMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		m_pShareBuffer->AddrX = account->Addr.MoveX;
		m_pShareBuffer->AddrY = account->Addr.MoveY;
		m_pShareBuffer->X = x;
		m_pShareBuffer->Y = y;
		m_pShareBuffer->Flag = 1; // Dllд��

		DbgPrint("�ȴ�д�����...\n");
		//LOGVARP2(log, "c6", L"�ȴ�д�����...");
		while (m_pShareBuffer->Flag == 1) { // �ȴ�Dllд�����
			//printf("m_pShareBuffer->Flag:%d\n", m_pShareBuffer->Flag);
			Sleep(10);
		}
		DbgPrint("�Ѿ����д��!!!\n");
		//LOGVARP2(log, "c6", L"�Ѿ����д��!!!");
	}
}

// д�뻭����ֵ
bool GameData::WritePicScale(DWORD v, _account_* account)
{
	account = account ? account : m_pAccoutBig;
	if (!account || !account->Mnq || !account->Addr.PicScale)
		return false;

	if (!m_bInDll && WriteProcessMemory(account->Mnq->Process, (PVOID)account->Addr.PicScale, &v, sizeof(v), NULL)) {
		//printf("WriteMoveCoor����:%d\n", write_len);
	}
	else {
		wchar_t log[64];
		//::printf("�޷�д��Ŀ������(%d) %08X\n", GetLastError(), account->Addr.MoveX);
		if (!m_pShareBuffer) {
			DbgPrint("�޷���������\n");
			LOGVARP2(log, "red", L"�޷���������(!WritePicScale.%d)", GetLastError());
			return false;
		}

		if (!m_bInDll) {
			wchar_t dll[256];
			wsprintfW(dll, L"%hs\\win7\\wxy.dll", m_pGame->m_chPath);
			InjectDll(account->Mnq->VBoxPid, dll, NULL, FALSE);
			m_bInDll = true;
		}

		// ӳ������һ����ͼ���õ�ָ�����ڴ��ָ�룬�������������
		m_pShareBuffer = (ShareWriteXYData*)::MapViewOfFile(m_hShareMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		m_pShareBuffer->AddrPic = account->Addr.PicScale;
		m_pShareBuffer->PicScale = v;
		m_pShareBuffer->Flag = 2; // Dllд��
	}

	return true;
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

		DWORD addr = m_dwReadBase + i;
		if (addr == (DWORD)codes) { // �����Լ�
			//::printf("���������Լ�:%08X %08X\n", codes, codes[0]);
			//return 0;
		}

		DWORD* dw = (DWORD*)(m_pReadBuffer + i);
		bool result = true;
		for (DWORD j = 0; j < length; j++) {
			if (codes[j] == 0x11) { // �����
				result = true;
			}
			else if (codes[j] == 0x22) { // ��Ҫ��ֵ��Ϊ0
				if (dw[j] == 0) {
					result = false;
					break;
				}
			}
			else if (((codes[j] & 0xffff0000) == 0x12340000)) { // ��2�ֽ����
				if ((dw[j] & 0x0000ffff) != (codes[j] & 0x0000ffff)) {
					result = false;
					break;
				}
				else {
					//::printf("%08X\n", dw[j]);
				}
			}
			else if (((codes[j] & 0x0000ffff) == 0x00001234)) { // ��2�ֽ����
				if ((dw[j] & 0xffff0000) != (codes[j] & 0xffff0000)) {
					result = false;
					break;
				}
			}
			else {
				if ((codes[j] & 0xffffff00) == 0x00001100) { // �Ƚ�������ַ��ֵ��� ���8λΪ�Ƚ�����
					//::printf("%X:%X %08X:%08X\n", j, codes[j] & 0xff, dw[j], dw[codes[j] & 0xff]);
					if (dw[j] != dw[codes[j] & 0xff]) {
						result = false;
						break;
					}
				}
				else if (dw[j] != codes[j]) { // ������ֵ�����
					if (0 && m_dwReadBase >= 0x247DD000 && m_dwReadBase < 0x247DE000 && i == 0) {
						printf("%d.m_dwReadBase!=0x4F67C000 %08X=%08X\n", j, dw[j], codes[j]);
					}
					result = false;
					break;
				}
			}
		}

		if (result) {
			save[count++] = addr;
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
			DWORD dwOneReadSize = 0x1000; // ÿ�ζ�ȡ����
			DWORD dwReadSize = 0x00;      // �Ѷ�ȡ����
			while (dwReadSize < mbi.RegionSize) {
				m_dwReadBase = pTmpReadAddress;
				m_dwReadSize = (dwReadSize + dwOneReadSize) <= mbi.RegionSize
					? dwOneReadSize : mbi.RegionSize - dwReadSize;

				SIZE_T write_len = 0;
				if (ReadProcessMemory(m_hGameProcess, (LPVOID)pTmpReadAddress, m_pReadBuffer, m_dwReadSize, &write_len)) {
					//::printf("flag:%08X %p-%p\n", flag, ReadAddress, ReadAddress + mbi.RegionSize);

					if (flag & 0x01) {
						DWORD find_num = 4;
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
						if (find_num == 4)
							flag = 0;
					}
					if (!flag) {
						DbgPrint("������ȫ���ҵ�\n");
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
	DbgPrint("��ȡ���ڴ���ʱ:%d����\n", ms2 - ms);
	LOGVARN2(64, "c0", L"��ȡ���ڴ���ʱ:%d����", ms2 - ms);
	m_bIsReadEnd = true;
	return true;
}