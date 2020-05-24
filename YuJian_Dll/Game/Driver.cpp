#include "Game.h"
#include "Driver.h"
#include <My/Common/func.h>
#include <stdio.h>
#include <shellapi.h>

#define FSFILTER_DRIVER_NAME "DriverFs999"

// ...
Driver::Driver(Game * p)
{
	m_pGame = p;
}

// ����
int Driver::Test()
{
	return 0x12345678;
}

BOOL Driver::InstallFsFilter(const char* path, const char * lpszDriverPath, const char * lpszAltitude)
{
	char    szTempStr[MAX_PATH];
	HKEY    hKey;
	DWORD    dwData;
	char    szDriverImagePath[MAX_PATH];

	if (NULL == lpszDriverPath)
	{
		return FALSE;
	}
	//�õ�����������·��
	//GetFullPathNameA(lpszDriverPath, MAX_PATH, szDriverImagePath, NULL);

	sprintf_s(szDriverImagePath, "%s\\files\\%s", path, lpszDriverPath);
	//printf("szDriverImagePath:%s\n", szDriverImagePath);

	SC_HANDLE hServiceMgr = NULL;// SCM�������ľ��
	SC_HANDLE hService = NULL;// NT��������ķ�����

	//�򿪷�����ƹ�����
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hServiceMgr == NULL)
	{
		// OpenSCManagerʧ��
		printf(" OpenSCManagerʧ��\n");
		CloseServiceHandle(hServiceMgr);
		return FALSE;
	}

	// OpenSCManager�ɹ�  

	//������������Ӧ�ķ���
	hService = CreateServiceA(hServiceMgr,
		FSFILTER_DRIVER_NAME,             // �����������ע����е�����
		FSFILTER_DRIVER_NAME,             // ע������������DisplayName ֵ
		SERVICE_ALL_ACCESS,         // ������������ķ���Ȩ��
		SERVICE_FILE_SYSTEM_DRIVER, // ��ʾ���صķ������ļ�ϵͳ��������
		SERVICE_DEMAND_START,       // ע������������Start ֵ
		SERVICE_ERROR_IGNORE,       // ע������������ErrorControl ֵ
		szDriverImagePath,          // ע������������ImagePath ֵ
		"FSFilter Activity Monitor",// ע������������Group ֵ
		NULL,
		"FltMgr",                   // ע������������DependOnService ֵ
		NULL,
		NULL);

	if (hService == NULL)
	{
		if (GetLastError() == ERROR_SERVICE_EXISTS)
		{
			//���񴴽�ʧ�ܣ������ڷ����Ѿ�������
			printf("���񴴽�ʧ�ܣ������ڷ����Ѿ�������\n");
			CloseServiceHandle(hService);       // ������
			CloseServiceHandle(hServiceMgr);    // SCM���
			return TRUE;
		}
		else
		{
			printf(" ���񴴽�ʧ�� %d\n", GetLastError());
			CloseServiceHandle(hService);       // ������
			CloseServiceHandle(hServiceMgr);    // SCM���
			return FALSE;
		}
	}
	CloseServiceHandle(hService);       // ������
	CloseServiceHandle(hServiceMgr);    // SCM���

	//-------------------------------------------------------------------------------------------------------
	// SYSTEM\\CurrentControlSet\\Services\\DriverName\\Instances�ӽ��µļ�ֵ�� 
	//-------------------------------------------------------------------------------------------------------
	strcpy(szTempStr, "SYSTEM\\CurrentControlSet\\Services\\");
	strcat(szTempStr, FSFILTER_DRIVER_NAME);
	strcat(szTempStr, "\\Instances");
	if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, szTempStr, 0, (LPSTR)"", TRUE, KEY_ALL_ACCESS, NULL, &hKey, (LPDWORD)&dwData) != ERROR_SUCCESS)
	{
		printf("HKEY_LOCAL_MACHINE %s����ʧ��\n", FSFILTER_DRIVER_NAME);
		return FALSE;
	}
	// ע������������DefaultInstance ֵ 
	strcpy(szTempStr, FSFILTER_DRIVER_NAME);
	strcat(szTempStr, " Instance");
	if (RegSetValueExA(hKey, "DefaultInstance", 0, REG_SZ, (CONST BYTE*)szTempStr, (DWORD)strlen(szTempStr)) != ERROR_SUCCESS)
	{
		printf("HKEY_LOCAL_MACHINE Setʧ��\n");
		return FALSE;
	}
	RegFlushKey(hKey);//ˢ��ע���
	RegCloseKey(hKey);
	//-------------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------------
	// SYSTEM\\CurrentControlSet\\Services\\DriverName\\Instances\\DriverName Instance�ӽ��µļ�ֵ�� 
	//-------------------------------------------------------------------------------------------------------
	strcpy(szTempStr, "SYSTEM\\CurrentControlSet\\Services\\");
	strcat(szTempStr, FSFILTER_DRIVER_NAME);
	strcat(szTempStr, "\\Instances\\");
	strcat(szTempStr, FSFILTER_DRIVER_NAME);
	strcat(szTempStr, " Instance");
	if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, szTempStr, 0, (LPSTR)"", TRUE, KEY_ALL_ACCESS, NULL, &hKey, (LPDWORD)&dwData) != ERROR_SUCCESS)
	{
		printf("HKEY_LOCAL_MACHINE %s %s����ʧ��\n", FSFILTER_DRIVER_NAME, FSFILTER_DRIVER_NAME);
		return FALSE;
	}
	// ע������������Altitude ֵ
	strcpy(szTempStr, lpszAltitude);
	if (RegSetValueExA(hKey, "Altitude", 0, REG_SZ, (CONST BYTE*)szTempStr, (DWORD)strlen(szTempStr)) != ERROR_SUCCESS)
	{
		printf("Altitude Setʧ��\n");
		return FALSE;
	}
	// ע������������Flags ֵ
	dwData = 0x0;
	if (RegSetValueExA(hKey, "Flags", 0, REG_DWORD, (CONST BYTE*)&dwData, sizeof(DWORD)) != ERROR_SUCCESS)
	{
		printf("Altitude Setʧ��2\n");
		return FALSE;
	}
	RegFlushKey(hKey);//ˢ��ע���
	RegCloseKey(hKey);
	//-------------------------------------------------------------------------------------------------------

	return TRUE;
}

// �����ļ����˱���
BOOL Driver::StartFsFilter()
{
	SC_HANDLE        schManager;
	SC_HANDLE        schService;
	SERVICE_STATUS    svcStatus;

	schManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == schManager)
	{
		printf("schManager=NULL %d\n", GetLastError());
		CloseServiceHandle(schManager);
		return FALSE;
	}
	schService = OpenServiceA(schManager, FSFILTER_DRIVER_NAME, SERVICE_ALL_ACCESS);
	if (NULL == schService)
	{
		printf("schService=NULL %d\n", GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schManager);
		return FALSE;
	}

	if (!StartService(schService, 0, NULL))
	{
		printf("!StartService %d\n", GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schManager);
		if (GetLastError() == ERROR_SERVICE_ALREADY_RUNNING)
		{
			// �����Ѿ�����
			return TRUE;
		}
		return FALSE;
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schManager);

	return TRUE;
}

// ֹͣ�ļ���������
BOOL Driver::StopFsFilter()
{
	SC_HANDLE        schManager;
	SC_HANDLE        schService;
	SERVICE_STATUS    svcStatus;
	bool            bStopped = false;

	schManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == schManager)
	{
		return FALSE;
	}
	schService = OpenServiceA(schManager, FSFILTER_DRIVER_NAME, SERVICE_ALL_ACCESS);
	if (NULL == schService)
	{
		CloseServiceHandle(schManager);
		return FALSE;
	}
	if (!ControlService(schService, SERVICE_CONTROL_STOP, &svcStatus) && (svcStatus.dwCurrentState != SERVICE_STOPPED))
	{
		CloseServiceHandle(schService);
		CloseServiceHandle(schManager);
		return FALSE;
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schManager);

	return TRUE;
}

// ɾ���ļ���������
BOOL Driver::DeleteFsFilter()
{
	SC_HANDLE        schManager;
	SC_HANDLE        schService;
	SERVICE_STATUS    svcStatus;

	schManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == schManager)
	{
		return FALSE;
	}
	schService = OpenServiceA(schManager, FSFILTER_DRIVER_NAME, SERVICE_ALL_ACCESS);
	if (NULL == schService)
	{
		CloseServiceHandle(schManager);
		return FALSE;
	}
	ControlService(schService, SERVICE_CONTROL_STOP, &svcStatus);
	if (!DeleteService(schService))
	{
		CloseServiceHandle(schService);
		CloseServiceHandle(schManager);
		return FALSE;
	}
	CloseServiceHandle(schService);
	CloseServiceHandle(schManager);

	return TRUE;
}

// ��װ����
bool Driver::InstallDriver(const char* path)
{
	CString file;
	file = path;
	file += L"\\files\\firenet.sys";

	if (!IsFileExist(file)) {
		//m_pJsCall->ShowMsg("ȱ�ٱ����ļ�:firenet.sys", "�ļ�������", 2);
		LOG2(L"firenet.sys������", "red");
		m_bIsInstallDll = false;
		char kill[32];
		sprintf_s(kill, "taskkill /f /t /pid %d", GetCurrentProcessId());
		system(kill);
		TerminateProcess(GetCurrentProcess(), 4);
		return false;
	}

	bool is_try = false;
_try_install_:
	if (m_SysDll.Install(L"firenet_safe", L"safe fire", file)) {
		LOG2(L"Install Driver Ok.", "green b");
		return true;
	}
	else {
		if (!is_try) {
			is_try = true;
			LOG2(L"Install Driver Failed, Try Agin.", "red");
#if 1
			Delete(L"firenet_safe");
#else
			ShellExecuteA(NULL, "open", "cmd", "/C sc stop firenet_safe", NULL, SW_HIDE);
			ShellExecuteA(NULL, "open", "cmd", "/C sc delete firenet_safe", NULL, SW_HIDE);
#endif
			goto _try_install_;
		}
		else {
			m_SysDll.UnInstall();
			char kill[32];
			sprintf_s(kill, "taskkill /f /t /pid %d", GetCurrentProcessId());
			system(kill);
			TerminateProcess(GetCurrentProcess(), 4);
			//LOG2(L"��װ����ʧ��, �������������ٳ���.", "red");
		}

		//MessageBox(NULL, "��װ����ʧ��", "��ʾ", MB_OK);
		return false;
	}
}

// ��װDll����
bool Driver::InstallDll(const char* path)
{
	if (!m_bIsInstallDll) {
		SetDll(path);
		m_bIsInstallDll = true;
	}
	else {
		if (DelDll()) {
			LOG2(L"ɾ��dll�ɹ�", "green");
			m_bIsInstallDll = false;
			return true;
		}
		else {
#if 0
			system("sc stop firenet_safe");
			system("sc delete firenet_safe");
#else
			Delete(L"firenet_safe");
#endif
			InstallDriver(path);
			if (!DelDll()) {
				LOGVARN2(32, "red", L"ɾ��dllʧ��:%d", GetLastError());
				return false;
			}
			//MessageBox(NULL, "ж������ʧ��", "��ʾ", MB_OK);
			return true;
		}
	}

}

// ж��
bool Driver::UnStall()
{
	//StopFsFilter();
	//DeleteFsFilter();

	return m_SysDll.UnInstall();
}

// ����Ҫע���DLL
bool Driver::SetDll(const char* path)
{
	BOOL	result;
	DWORD	returnLen;
	char	output;

	HANDLE	hDevice = NULL;

	PVOID	dllx64Ptr = NULL;
	PVOID	dllx86Ptr = NULL;

	ULONG	dllx64Size = 0;
	ULONG	dllx86Size = 0;

	hDevice = CreateFileA("\\\\.\\CrashDumpUpload",
		NULL,
		NULL,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE) {
		//m_pJsCall->ShowMsg("��������ʧ��.", "��ʾ", 2);
		return false;
	}

	DWORD pid = GetCurrentProcessId();
	DWORD   in_buffer[2] = { pid, GetParentProcessID() };
	result = DeviceIoControl(
		hDevice,
		IOCTL_SET_PROTECT_PID,
		in_buffer,
		sizeof(in_buffer),
		&output,
		sizeof(output),
		&returnLen,
		NULL);
	//printf("��������ID:%d %d\n", pid, result);

	// �������ؽ���
	SetHidePid(pid);

	char file[255];
	sprintf_s(file, "%s\\files\\9Star-e", path);
#if 0
	strcpy(file, "C:\\Users\\fucan\\Desktop\\MNQ-9Star\\vs\\9Star.dll");
	printf("%s\n", file);
#endif
	dllx86Ptr = MyReadFile(file, &dllx86Size);
	if (dllx86Ptr == NULL) {
		LOG2(L"�Ҳ����ļ�9Star.dll", "red");
		return false;
	}
	// 1234
	// 1 2 3 4 5 6 7 8 9 10
	// 11 12 13 14 15

	result = DeviceIoControl(
		hDevice,
		IOCTL_SET_INJECT_X86DLL,
		dllx86Ptr,
		dllx86Size,
		&output,
		sizeof(char),
		&returnLen,
		NULL);

	if (result) {
		LOGVARN2(64, "green", L"����dll�ɹ�(%d).", dllx86Size);
	}
	else {
		//LOGVARN2(32, "red", L"GetLastError:%d.\n", GetLastError());
		LOG2(L"����dllʧ�ܣ��Զ�����.", "red");
		if (DelDll()) {
			LOG2(L"ɾ��dll�ɹ�", "green");
			
			result = DeviceIoControl(
				hDevice,
				IOCTL_SET_INJECT_X86DLL,
				dllx86Ptr,
				dllx86Size,
				&output,
				sizeof(char),
				&returnLen,
				NULL);

			if (result) {
				LOG2(L"����dll�ɹ�.", "green");
			}
			else {
				LOG2(L"����dllʧ��, ���ֶ�����.", "red");
			}
		}
	}

	if (dllx86Ptr) {
		free(dllx86Ptr);
	}

	if (hDevice != NULL) {
		CloseHandle(hDevice);
	}
	return true;
}

// ɾ��Ҫע��dll
bool Driver::DelDll()
{
	HANDLE hDevice = CreateFileA("\\\\.\\CrashDumpUpload",
		NULL,
		NULL,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE) {
		return false;
	}

	int     inv = 0;
	char	output;
	DWORD	returnLen;
	BOOL result = DeviceIoControl(
		hDevice,
		IOCTL_DEL_INJECT_X86DLL,
		&inv,
		sizeof(int),
		&output,
		sizeof(char),
		&returnLen,
		NULL);

	CloseHandle(hDevice);
	return result;
}

// ��ȡ�ļ�
PVOID Driver::MyReadFile(const CHAR* fileName, PULONG fileSize)
{
	HANDLE fileHandle = NULL;
	DWORD readd = 0;
	PVOID fileBufPtr = NULL;

	fileHandle = CreateFileA(
		fileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (fileHandle == INVALID_HANDLE_VALUE)
	{
		*fileSize = 0;
		return NULL;
	}

	*fileSize = GetFileSize(fileHandle, NULL);

	fileBufPtr = calloc(1, *fileSize);

	if (!ReadFile(fileHandle, fileBufPtr, *fileSize, &readd, NULL))
	{
		free(fileBufPtr);
		fileBufPtr = NULL;
		*fileSize = 0;
	}

	CloseHandle(fileHandle);
	return fileBufPtr;
}

// ���ñ�������ID
void Driver::SetProtectPid(DWORD pid)
{
	pid = pid ? pid : GetCurrentProcessId();
	HANDLE hDevice = CreateFileA("\\\\.\\CrashDumpUpload",
		NULL,
		NULL,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE) {
		LOGVARN2(32, "red b", L"��������ʧ��Protect:%d", pid);
		return;
	}

	char    out;
	DWORD   in_buffer[2] = { pid, GetParentProcessID() };
	DWORD	returnLen;
	BOOL result = DeviceIoControl(
		hDevice,
		IOCTL_SET_PROTECT_PID,
		in_buffer,
		sizeof(in_buffer),
		&out,
		sizeof(out),
		&returnLen,
		NULL);
	//printf("��������ID:%d %d(Game-e)\n", pid, result);
	//LOGVARN2(32, "green b", L"Protect Process:%d", pid);

	CloseHandle(hDevice);
}


// ����ģ��������ID
void Driver::SetProtectVBoxPid(DWORD pid)
{
	HANDLE hDevice = CreateFileA("\\\\.\\CrashDumpUpload",
		NULL,
		NULL,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE) {
		LOGVARN2(32, "red b", L"��������ʧ��VBox:%d", pid);
		return;
	}

	char	output;
	DWORD	returnLen;
	BOOL result = DeviceIoControl(
		hDevice,
		IOCTL_SET_PROTECT_VBOX_PID,
		&pid,
		4,
		&output,
		sizeof(char),
		&returnLen,
		NULL);
	//printf("����ģ��������ID:%d %d\n", pid, result);
	LOGVARN2(32, "green b", L"Protect VBox:%d", pid);

	CloseHandle(hDevice);
}

// �������ؽ���ID
void Driver::SetHidePid(DWORD pid)
{
	return;
	DWORD dwWinLogonId = SGetProcessId(L"winlogon.exe");
	if (!dwWinLogonId) {
		dwWinLogonId = SGetProcessId(L"WINLOGON.exe");
	}
	if (!dwWinLogonId)
		return;

	if (m_pGame->m_Setting.NoHideProc)
		return;
	if (0 && pid == GetCurrentProcessId())
		return;

	wchar_t log[64];
	HANDLE hDevice = CreateFileA("\\\\.\\CrashDumpUpload",
		NULL,
		NULL,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE) {
		LOGVARP2(log, "red b", L"��������ʧ��Hide:%d", pid);
		return;
	}

	DWORD pids[2] = { pid, dwWinLogonId };
	char	output;
	DWORD	returnLen;
	BOOL result = DeviceIoControl(
		hDevice,
		IOCTL_SET_HIDE_PID,
		pids,
		sizeof(pids),
		&output,
		sizeof(char),
		&returnLen,
		NULL);

	//printf("���ؽ���ID:%d-%d(%d)\n", pid, dwWinLogonId, result);
	//LOGVARP2(log, "green b", L"���ؽ���:%d", pid);

	CloseHandle(hDevice);
}

// ����
void Driver::BB()
{
	HANDLE hDevice = CreateFileA("\\\\.\\CrashDumpUpload",
		NULL,
		NULL,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE) {
		return;
	}

	DWORD v = 0;
	char	output;
	DWORD	returnLen;
	BOOL result = DeviceIoControl(
		hDevice,
		IOCTL_BSOD,
		&v,
		4,
		&output,
		sizeof(char),
		&returnLen,
		NULL);

	CloseHandle(hDevice);
}

// ɾ����������
bool Driver::Delete(const wchar_t* name)
{
	if (wcsstr(name, L"firenet_safe")) {
		HANDLE hDevice = CreateFileA("\\\\.\\CrashDumpUpload", NULL, NULL, NULL, OPEN_EXISTING, NULL, NULL);

		if (hDevice == INVALID_HANDLE_VALUE) {
			goto _unstall_;
		}

		DWORD v = 0;
		char	output;
		DWORD	returnLen;
		BOOL result = DeviceIoControl(hDevice, IOCTL_SAFE_UNSTALL, &v, 4, &output, sizeof(char), &returnLen, NULL);

		CloseHandle(hDevice);
	}

_unstall_:
	SC_HANDLE        schManager;
	SC_HANDLE        schService;
	SERVICE_STATUS    svcStatus;

	schManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (NULL == schManager)
		return false;

	schService = OpenService(schManager, name, SERVICE_ALL_ACCESS);
	if (NULL == schService) {
		LOG2(L"NULL == schService", "red");
		CloseServiceHandle(schManager);
		return false;
	}
	if (!ControlService(schService, SERVICE_CONTROL_STOP, &svcStatus)) {
		//LOG2(L"!ControlService", "red");
		//return false;
	}	

	if (!DeleteService(schService)) {
		LOG2(L"!DeleteServic", "red");
		CloseServiceHandle(schService);
		CloseServiceHandle(schManager);
		return false;
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schManager);
	return true;
}
