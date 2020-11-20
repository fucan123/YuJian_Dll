#include "Home.h"
#include "Game.h"
#include "Driver.h"
#include <time.h>
#include <My/Common/MachineID.h>
#include <My/Common/Des.h>
#include <My/Common/Explode.h>

Home::Home(Game* p)
{
	m_pGame = p;

	m_bFree = false;

	m_iVerifyTime = 0;
	m_iExpire = 0;
	m_iEndTime = 0;
	m_Status = 0;
	m_Error = 0;

	m_nVerifyNum = 0;

	MachineID mac;
	mac.GetMachineID(m_MachineId);
	m_MachineId[32] = 0;
	printf("机器码:%s\n", m_MachineId);
}

// 设置是否免费
void Home::SetFree(bool v)
{
	m_bFree = v;
}

bool Home::IsValid()
{
#if 0
	return true;
#endif
#if 1
	if (m_bFree)
		return true;
#endif
	if (!m_iVerifyTime || !m_iExpire || !m_iEndTime)
		return false;

	int now_time = time(NULL);
	if (now_time < m_iVerifyTime) { // 当前时间比验证时间还早 可能修改了系统时间
		return false;
	}
	//printf("%d --- %d(%d)\n", now_time, m_iEndTime, m_iEndTime - now_time);
	return now_time <= m_iEndTime;
}

// ...
bool Home::IsValidS()
{
	return false;
}

// 转移
bool Home::GetInCard(const char* card)
{
	char key[17], param[128], encryptParam[256];
	GetDesKey(key);
	sprintf_s(param, "game=%s&machine_id=%s&card=%s&tm=%d", HOME_GAME_FLAG, m_MachineId, card, time(nullptr));
	DesEncrypt(encryptParam, key, param, strlen(param));
	//printf("key:%s %d\n", key, strlen(key));
	//printf("param:%s %d\n", param, strlen(param));
	//printf("encryptParam:%s %d\n", encryptParam, strlen(encryptParam));

	std::string result;
	wchar_t path[32];
	wsprintfW(path, L"/getincard?t=%d", time(nullptr));
	HttpClient http;
	http.m_GB2312 = false;
	http.AddParam("p", encryptParam);
	HTTP_STATUS status = http.Request(HOME_HOST, path, result, HTTP_POST);
	if (status != HTTP_STATUS_OK) {
		SetError(status, "移机失败！", status);
		return false;
	}

	Parse(result.c_str());
	time_t a;

	//MessageBox(NULL, m_MsgStr, L"XXX", MB_OK);
	//printf("%d, %s 内容->%s %s %d\n", m_Error, m_MsgStr, m_pRepsone, result.c_str(), result.length());

	return m_Error == 0;
}

// 充值
bool Home::Recharge(const char* card, const char* remark)
{
	//printf("Home::Recharge\n");
	std::string result;
	wchar_t path[32];
	char key[17], param[128], encryptParam[256];
	GetDesKey(key);

	HttpClient http;
	http.m_GB2312 = false;

#define NEW_SER 1
#if NEW_SER == 0
	sprintf_s(param, "game=%s&machine_id=%s&card=%s&tm=%d", HOME_GAME_FLAG, m_MachineId, card, time(nullptr));
	DesEncrypt(encryptParam, key, param, strlen(param));
	//printf("key:%s %d\n", key, strlen(key));
	//printf("param:%s %d\n", param, strlen(param));
	//printf("encryptParam:%s %d\n", encryptParam, strlen(encryptParam));
	wsprintfW(path, L"/recharge_des?t=%d", time(nullptr));
#else
	sprintf_s(param, "machine_id=%s&tm=%d", m_MachineId, time(nullptr));
	DesEncrypt(encryptParam, key, param, strlen(param));
	wsprintfW(path, L"/r?t=%d", time(nullptr));
	http.AddParam("card", card);
	http.AddParam("remark", remark);
#endif

	//printf("encryptParam:%s %d\n", encryptParam, strlen(encryptParam));
	
	http.AddParam("p", encryptParam);
	HTTP_STATUS status = http.Request(HOME_HOST, path, result, HTTP_POST);
	if (status != HTTP_STATUS_OK) {
		SetError(status, "激活失败了啦！！！", status);
		return false;
	}

	Parse(result.c_str());
	time_t a;

	//MessageBox(NULL, m_MsgStr, L"XXX", MB_OK);
	//printf("%d, %s 内容->%s %s %d\n", m_Error, m_MsgStr, m_pRepsone, result.c_str(), result.length());

	return m_Error == 0;
}

// 获取卡号
void Home::GetCardNo(std::string& result)
{
	HttpClient http;
	http.m_GB2312 = false;
	http.AddParam("mac", m_MachineId);
	//printf("mac:%s\n", m_MachineId);
	HTTP_STATUS status = http.Request(HOME_HOST, L"/getcardno", result, HTTP_POST);
}

bool Home::Verify()
{
	char key[17], param[128], encryptParam[256], encryptParam2[512], tmStr[16] = "123";
	memset(key, 0, sizeof(key));
	memset(param, 0, sizeof(param));
	memset(encryptParam, 0, sizeof(encryptParam));
	memset(encryptParam2, 0, sizeof(encryptParam2));

	GetDesKey(key);
	memcpy(key, "phpcpp999888666b", 16);
	sprintf_s(param, "game=%s&machine_id=%s&tm=%d", HOME_GAME_FLAG, m_MachineId, time(nullptr));
	DesEncrypt(encryptParam, key, param, strlen(param));
	int encryptParam_Length = strlen(encryptParam);
	//printf("key:%s %d\n", key, strlen(key));
	//printf("param:%s %d\n", param, strlen(param));
#if 1
	//printf("encryptParam:%s %d\n", encryptParam, strlen(encryptParam));
	m_pGame->m_pDriver->EncodeStr((BYTE*)encryptParam, (BYTE*)encryptParam, strlen(encryptParam));
	DWORD tm = m_pGame->m_pDriver->GetEnDeStrTickCount();
	CharToHext(encryptParam2, encryptParam, encryptParam_Length);
	//printf("EncodeStr:%s %d\n", encryptParam2, strlen(encryptParam2));
	//printf("tm:%d %08X\n", tm, tm);
	sprintf_s(tmStr, "%d", tm);
#endif
	
	std::string result;
	wchar_t path[32];
#if NEW_SER == 0
	wsprintfW(path, L"/verify_n?t=%d", time(nullptr));
#else
	wsprintfW(path, L"/v?t=%d", time(nullptr));
#endif
	HttpClient http;
	http.m_GB2312 = false;
	http.AddParam("p", encryptParam2);
	http.AddParam("tm", tmStr);
	HTTP_STATUS status = http.Request(HOME_HOST, path, result, HTTP_POST);
	if (status != HTTP_STATUS_OK && status != 222) {
		m_pGame->m_nVerifyNum++;
		SetError(status, "验证失败！", status);
		return false;
	}

	//m_pGame->m_pDriver->Delete(L"net2020");

	Parse_N(result.c_str());
	m_pGame->m_nVerifyNum++;
	time_t a;

	//MessageBox(NULL, m_MsgStr, L"XXX", MB_OK);
	//printf("%d, %s 内容->%s %s %d\n", m_Error, m_MsgStr, m_pRepsone, result.c_str(), result.length());

	return m_Error == 0;
}

int Home::GetExpire()
{
	return m_iExpire = GetValue("expire:", 10);
}

void Home::Parse(const char* msg)
{
	// 格式为(MSG||DES加密字符)
	// DES加密字符解密为(状态||机器码||过期日期[时间戳]||还剩时间[秒])

	char a = 'a', e = 'E', r = 'r', o = 'o', dian='.';
	char unknow_str[] = { 'U', 'n', 'k', 'n', o, 'w', dian, 0 };
	strcpy(m_MsgStr, unknow_str);

	char error_str[] = {e, r, r, o, r, dian, 0};
	char* msgStr = (char*)msg;
	char* desStr = strstr(msgStr, "||");
	if (!desStr) {
		printf("!desStr\n");
		SetError(1, error_str);
		return;
	}

	int i = 0;
	for (;  msgStr < desStr; i++, msgStr++) {
		m_MsgStr[i] = *msgStr;
	}
	m_MsgStr[i] = 0;

	//printf("m_MsgStr:%s\n desStr:%s\n", m_MsgStr, desStr+2);
	char key[17];
	GetDesKey(key);
	DesDecrypt(m_pRepsone, key, desStr + 2, strlen(desStr + 2), true);

	Explode arr("||", m_pRepsone);
	if (arr.GetCount() != 5) {
		SetError(1, error_str);
		return;
	}

	if (arr.GetValue2Int(0) == 0) {
		SetError(1, error_str);
		return;
	}

	//printf("MsgPtr:%p\n", m_pRepsone);
	if (strcmp(arr[1], m_MachineId) != 0) {
		SetError(1, error_str);
		return;
	}

	//printf("msgPtr:%s\n", msgPtr);
	SetExpire(arr.GetValue2Int(3));

	m_pGame->m_nEndTime = (m_iEndTime - JIAOYAN_V);
	SetErrorCode(0);
	//printf("有效时间（秒）:%d\n", m_iExpire);
}

// 解析返回结果
void Home::Parse_N(const char * msg)
{
	// 格式为(MSG||DES加密字符)
	// DES加密字符解密为(状态||机器码||过期日期[时间戳]||还剩时间[秒])

	char a = 'a', e = 'E', r = 'r', o = 'o', dian = '.';
	char unknow_str[] = { 'U', 'n', 'k', 'n', o, 'w', dian, 0 };
	strcpy(m_MsgStr, unknow_str);

	char error_str[] = { e, r, r, o, r, dian, 0 };
	char* msgStr = (char*)msg;
	char* desStr = strstr(msgStr, "||");
	if (!desStr) {
		//printf("!desStr\n");
		SetError(1, error_str);
		return;
	}

	int i = 0;
	for (; msgStr < desStr; i++, msgStr++) {
		m_MsgStr[i] = *msgStr;
	}
	m_MsgStr[i] = 0;

	//printf("m_MsgStr:%s\n desStr:%s\n", m_MsgStr, desStr+2);
	int n = 0;
	char yhStr[256];
	for (i = 0; i < strlen(desStr + 2); i += 2, n++) {
		yhStr[n] = HexToInt(desStr + 2 + i, 2) & 0xff;
	}
	yhStr[n] = 0;

	m_pGame->m_pDriver->DecodeStr((BYTE*)yhStr, (BYTE*)yhStr, n);
	DesDecrypt(m_pRepsone, (char*)"phpcpp999888666b", yhStr, n, true);

	Explode arr("||", m_pRepsone);
	if (arr.GetCount() != 5) {
		SetError(1, error_str);
		return;
	}

	if (arr.GetValue2Int(1) == 0) {
		//printf("arr.GetValue2Int(1) == 0\n");
		m_Error = 1;
		return;
	}

	//printf("MsgPtr:%p --- %c\n", msgPtr, *statusPtr);
	if (strcmp(arr[2], m_MachineId) != 0) {
		//printf("strcmp(arr[2], m_MachineId) != 0\n");
		m_Error = 1;
		return;
	}

	//printf("msgPtr:%s\n", msgPtr);
	SetExpire(arr.GetValue2Int(3));

	m_pGame->m_nEndTime = (m_iEndTime - JIAOYAN_V);
	SetErrorCode(0);
}

bool Home::GetValue(char* key, char value[], int length)
{
	char* ptr = strstr(m_pRepsone, key);
	if (!ptr) {
		value[0] = 0;
		return false;
	}

	ptr += strlen(key);
	for (int i = 0; i < length; i++) {
		if (ptr[i] == ' ') {
			length = i;
			break;
		}
		value[i] = ptr[i];
	}
	value[length] = 0;

	return true;
}

int Home::GetValue(char* key, int length, int default_value)
{
	return GetValue((const char*)key, length, default_value);
}

int Home::GetValue(const char* key, int length, int default_value)
{
	char* ptr = strstr(m_pRepsone, key);
	if (!ptr) {
		return default_value;
	}
	//printf("%s\n", key);
	int value = 0;
	ptr += strlen(key);
	for (int i = 0; i < length; i++) {
		if (ptr[i] == '-') {
			return 0;
		}
		if (ptr[i] == ' ') {
			break;
		}
		if (ptr[i] >= '0' && ptr[i] <= '9') {
			//printf("%c\n", ptr[i]);
			value = value * 10 + (ptr[i] - '0');
		}
	}

	return value;
}

int Home::SetVerifyTime()
{
	return m_iVerifyTime = time(NULL);
}

int Home::SetEndTime()
{
	return m_iEndTime = m_iVerifyTime + m_iExpire;
}

void Home::SetExpireTime_S(time_t expire_time)
{
	if (!expire_time) {
		expire_time = m_iEndTime;
	}

	struct tm t;
	localtime_s(&t, &expire_time);
	char str[128];
	sprintf_s(str, "%d-%02d-%02d %02d:%02d", t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min);
	m_sExpireTime = str;
	//printf("expire_time:%d %s\n", expire_time, str);
}

// 设置过期时间
void Home::SetExpire(int t)
{
	m_iExpire = t;
	SetVerifyTime();
	SetEndTime();
	SetExpireTime_S(time(nullptr) + t);
	m_nVerifyNum++;
}

// 获得DES密钥
void Home::GetDesKey(char save[17])
{
	save[0] = 'b';
	save[1] = save[0] + 1, save[2] = save[1] + 1;
	save[3] = save[0] + 1, save[4] = 'b', save[5] = 'b';
	save[6] = '9', save[7] = 'x', save[8] = 'f', save[9] = 'o', save[10] = 'r', save[11] = 'e';
	save[12] = 'v', save[13] = 'e', save[14] = 'r', save[15] = 'y';
	save[16] = 0;
}

std::string& Home::GetExpireTime_S()
{
	return m_sExpireTime;
}

void Home::SetError(int code, const char * str)
{
	SetErrorCode(code);
	SetMsgStr(str);
}

void Home::SetError(int code, wchar_t* str)
{
	SetErrorCode(code);
	SetMsgStr(str);
}

void Home::SetError(int code, CString str)
{
	SetErrorCode(code);
	SetMsgStr(str);
}

void Home::SetError(int code, const char * str, DWORD status)
{
	SetErrorCode(code);
	SetMsgStr(str, status);
}

void Home::SetError(int code, wchar_t * str, DWORD status)
{
	SetErrorCode(code);
	SetMsgStr(str, status);
}

void Home::SetErrorCode(int code)
{
	m_Error = code;
}

void Home::SetMsgStr(wchar_t* str)
{
	//m_MsgStr = str;
}

void Home::SetMsgStr(CString str)
{
	//m_MsgStr = str;
}

void Home::SetMsgStr(wchar_t* str, DWORD status)
{
	//m_MsgStr.Format(L"%ws，错误码:(%d)", str, status);
}

void Home::SetMsgStr(const char * str, DWORD status)
{
	sprintf_s(m_MsgStr, "%s，错误码:(%d)", str, status);
}

const char* Home::GetMsgStr()
{
	return m_MsgStr;
}

bool Home::IsValid2()
{
	if (!m_iVerifyTime || !m_iExpire || !m_iEndTime)
		return false;

	int now_time = time(NULL);
	if (now_time < m_iVerifyTime) { // 当前时间比验证时间还早 可能修改了系统时间
		return false;
	}
	//printf("%d --- %d(%d)\n", now_time, m_iEndTime, m_iEndTime - now_time);
	return now_time <= m_iEndTime;
}

int Home::HexToInt(const char* str, int length)
{
	if (str == nullptr)
		return 0;

	if (str[0] == '0') {
		if (str[1] == 'x' || str[1] == 'X')
			str += 2;
	}

	int i = 0;
	int num = 0;
	while (*str) {
		char ch = *str;
		if (ch >= '0' && ch <= '9') {
			ch = ch - '0';
		}
		else if (ch >= 'A' && ch <= 'F') {
			ch = ch - 'A' + 0x0A;
		}
		else if (ch >= 'a' && ch <= 'f') {
			ch = ch - 'a' + 0x0a;
		}
		else {
			ch = 0;
			break;
		}

		num = num * 0x10 + ch;
		if (length > 0 && (++i >= length))
			break;

		str++;
	}
	return num;
}

void Home::CharToHext(char* save, char* str, int length)
{
	int i = 0, j = 0;
	for (; i < length; i++) {
		char tmp[3];
		sprintf_s(tmp, "%02x", str[i] & 0xff);
		save[j] = tmp[0];
		save[j + 1] = tmp[1];
		j += 2;
	}
}
