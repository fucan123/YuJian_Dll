#pragma once

#include "HttpClient.h"
#include <string>

#define HOME_HOST      L"39.100.110.77"
//#define HOME_HOST      L"www.fuzhu.com"
#define HOME_GAME_FLAG "1"
#define JIAOYAN_V      16899

class Game;
class Home;

typedef bool (Home::*func_valid)();
#ifdef x64
typedef union func_addr {
	func_valid f;
	unsigned __int64 v;
} FuncAddr;
#else 
typedef union func_addr {
	func f;
	unsigned __int v;
} FuncAddr;
#endif

class Home
{
public:
	Home(Game* p);
	// �����Ƿ����
	void SetFree(bool v);
	// �Ƿ���Ч
	bool IsValid();
	bool IsValidS();
	// ת��
	bool GetInCard(const char* card);
	// ��ֵ
	bool Recharge(const char* card);
	// ��֤
	bool Verify();
	// ��ô˻�����Чʱ�䣨�룩
	int  GetExpire();
	// �������ؽ��
	void Parse(const char* msg);
	// ��÷��ؽ���е�ֵ
	bool GetValue(char* key, char value[], int length);
	// ��÷��ؽ���е�ֵ
	int  GetValue(char* key, int length, int default_value=0);
	// ��÷��ؽ���е�ֵ
	int  GetValue(const char* key, int length, int default_value = 0);
	// ������֤ʱ��
	int  SetVerifyTime();
	// ���õ���ʱ��
	int  SetEndTime();
	// ���õ���ʱ���ַ���
	void SetExpireTime_S(time_t expire_time);
	// ���ù���ʱ��
	void SetExpire(int t);
	// ���DES��Կ
	void GetDesKey(char save[17]);
	// ��õ���ʱ���ַ���
	std::string& GetExpireTime_S();
	void SetError(int code, const char* str);
	void SetError(int code, wchar_t* str);
	void SetError(int code, CString str);
	void SetError(int code, const char* str, DWORD status);
	void SetError(int code, wchar_t* str, DWORD status);
	void SetErrorCode(int code);
	void SetMsgStr(wchar_t* str);
	void SetMsgStr(CString str);
	void SetMsgStr(wchar_t* str, DWORD status);
	void SetMsgStr(const char* str, DWORD status);
	const char* GetMsgStr();

	// �Ƿ���Ч
	bool IsValid2();
public:
	Game* m_pGame;

	char m_NoUse[128];

	int m_nVerifyNum = 0;
	// ���ؽ��ָ��
	char m_pRepsone[256];
	// ��֤ʱ�䣨�룩
	int   m_iVerifyTime;
	// ����ʱ��
	int m_iExpire;
	// ����ʱ��
	int   m_iEndTime;

	char m_MachineId[33];
	int  m_Status;
	int  m_Error;

	// �Ƿ����
	bool m_bFree = false;

	// ��վ��������
	std::string m_sResult;
	// ��ʾ����
	char m_MsgStr[128];
	// ����ʱ��
	std::string m_sExpireTime;
};