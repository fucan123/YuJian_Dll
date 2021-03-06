#pragma once
#include <devioctl.h>
#include <My/Driver/Sys.h>

#define IOCTL_TEST \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x000, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_SET_INJECT_X86DLL \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x900, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_SET_INJECT_X64DLL \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x901, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DEL_INJECT_X86DLL \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x902, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DEL_INJECT_X64DLL \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x903, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_SET_PROTECT_PID \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x100, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_SET_PROTECT_VBOX_PID \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x101, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_SET_HIDE_PID \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x102, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_DECODE_DLL \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x200, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENUM_DLL \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x201, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_ENCODE_STR \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x301, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_DECODE_STR \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x302, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ENDESTR_TICKCOUNT \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x303, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_SAFE_UNSTALL \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0xf00, METHOD_IN_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_BSOD \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0xfff, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

class JsCall;
class Game;
class Driver
{
public:
	// ...
	Driver(Game* p);

	// 测试
	bool  Test();
	// 安装文件过滤驱动
	BOOL InstallFsFilter(const char* path, const char* lpszDriverPath, const char* lpszAltitude);
	// 启动文件过滤保护
	BOOL StartFsFilter();
	// 停止文件保护驱动
	BOOL StopFsFilter();
	// 删除文件保护驱动
	BOOL DeleteFsFilter();
	// 安装驱动
	bool InstallDriver(const char* path);
	// 安装Dll驱动
	bool InstallDll(const char* path);
	// 卸载
	bool UnStall();
	// 设置要注入的DLL
	bool SetDll(const char* path);
	// 删除要注入dll
	bool DelDll();
	// 读取文件
	PVOID MyReadFile(const CHAR* fileName, PULONG fileSize);
	// 设置保护进程ID
	void SetProtectPid(DWORD pid);
	// 设置模拟器进程ID
	void SetProtectVBoxPid(DWORD pid);
	// 设置隐藏进程ID
	void SetHidePid(DWORD pid);
	// 解密Dll
	bool DecodeDll(BYTE* in, BYTE* out, DWORD size);
	// 获取进程Dll
	bool EnumDll(BYTE* in, BYTE* out, DWORD size);
	// 加密字符串
	bool EncodeStr(BYTE* in, BYTE* out, DWORD size);
	// 解密字符串
	bool DecodeStr(BYTE* in, BYTE* out, DWORD size);
	// 获取加密解密滴答数
	DWORD GetEnDeStrTickCount();
	// 连接到驱动式
	HANDLE ConnectDriver();
	// 蓝屏
	void BB();
	// 删除驱动服务
	bool Delete(const wchar_t* name);
public:
	JsCall* m_pJsCall;
	Game* m_pGame;

	bool  m_bIsInstallDll = false;
	Sys m_SysDll;
};