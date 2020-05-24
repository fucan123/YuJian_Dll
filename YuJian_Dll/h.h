#pragma once
#include <Windows.h>

#define DLLEXPORT __declspec(dllexport)

#ifdef __cplusplus
extern "C" {
#endif

	// ��ʼ�����п�
	DLLEXPORT void WINAPI EntryIn(HWND hWnd, const char* conf_path);
	// ��Ϸ�Ƿ񼤻�
	bool WINAPI Game_IsValid();
	// ��Ϸ�ͷ�
	void WINAPI Game_Relase();
	// ��Ϸ��ͣ
	void WINAPI Game_Pause(bool v);
	// �ʺ��Ƿ��¼
	int WINAPI Game_IsLogin(int index);
	// �Ƿ�ɹ���װdll 0δ 1�� -1�뼤��
	int WINAPI Game_InstallDll();
	// ����Ϸ index[-2=ֹͣ -1=�Զ���¼ ����-1=�˺�����]
	int WINAPI Game_OpenGame(int index, int close_all);
	// �ر���Ϸ��
	int WINAPI Game_CloseGame(int index);
	// �Ѿ������
	int WINAPI Game_InTeam(int index);
	// �޸�һЩ����
	int WINAPI Game_PutSetting(const wchar_t* name, int v);
	// ת�ƿ��ű���
	int WINAPI Game_GetInCard(const wchar_t* card);
	// ��֤����
	int WINAPI Game_VerifyCard(const wchar_t* card);
	// ��ѯ������¼
	int WINAPI Game_SelectFBRecord(char*** result, int* col);
	// ����
	DLLEXPORT void WINAPI Talk(const char* text, int type);
#ifdef __cplusplus
}
#endif

// ����Ϸ
DWORD WINAPI PlayGame(LPVOID param);
// game.run
DWORD WINAPI RunGame(LPVOID param);
// game.listen
DWORD WINAPI Listen(LPVOID param);
// ����
DWORD WINAPI Verify(LPVOID param);
