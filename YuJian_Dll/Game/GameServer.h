#pragma once
#include "Home.h"
#include <My/Socket/MServer.h>

enum SOCKET_OPCODE
{
	SCK_PING = 0x00,
	SCK_MSG,         // ��Ϣ
	SCK_MSG2,        // ��Ϣ
	SCK_OPEN,        // ��Ϸ�����Ѵ�
	SCK_ACCOUNT,     // �ʺ���Ϣ 0-3λ=ѡ���ĸ���ɫ, 4-7=�Ƿ��Ǵ��
	SCK_LOGIN,       // ���Ե�¼�ʺ���
	SCK_INGAME,      // �ѽ�����Ϸ
	SCK_CANTEAM,     // ���������
	SCK_INTEAM,      // ���
	SCK_CANINFB,     // ���Խ�������
	SCK_OPENFB,      // ��������
	SCK_INFB,        // ���븱��
	SCK_OUTFB,       // ��ȥ����
	SCK_ATFB,        // �Ƿ����ڸ���
	SCK_SMALLOUTFB,  // С�ų�����
	SCK_SMALLINFB,   // С�Ž�����
	SCK_GETXL,       // ��ȡ��������������
	SCK_NOYAOSI,     // ���û��Կ��
	SCK_PICKUPITEM,  // ����
	SCK_REBORN,      // ����
	SCK_QIAZHUS,     // ��ס�������
	SCK_OFFLINE,     // ��Ϸ������
	SCK_CLOSE,       // �ر���Ϸ

	SCK_DEBUG,       // �Ƿ����
	SCK_NOAUOTOSLT,  // ���Զ�ѡ��
	SCK_TALK,        // ����
	SCK_TALK_OPEN,   // ��������

	SCK_EXPIRE_OUT,  // ����
};

struct _account_;

class Game;
class GameServer
{
public:
	// ...
	GameServer(Game* p);

    // 
	void SetSelf(GameServer* p);
	// ����
	void Listen(USHORT port);

	// ��Ϸ���ڴ���Ӧ
	void Open(const char* data, int len);
	// ��¼
	void Login(_account_* p);
	// ��Ϣ
	void Msg(_account_* p, const char* data, int len);
	// �ѽ�����Ϸ
	void InGame(_account_* p, const char* data, int len);
	// �������
	void CanTeam(_account_* p, const char* data, int len);
	// ���
	void InTeam(_account_* p, const char* data, int len);
	// ���Խ�������
	void CanInFB(_account_* p, const char* data, int len);
	// ��������
	void OpenFB(_account_* p, const char* data, int len);
	// ���븱��
	void InFB(_account_* p, const char* data, int len);
	// ��ȥ����
	void OutFB(_account_* p, const char* data, int len);
	// ���ڸ���
	void AtFB(_account_* p, const char* data, int len);
	// С�Ž��븱��
	void SmallInFB(_account_* p, const char* data, int len);
	// С�ų�ȥ����
	void SmallOutFB(_account_* p, const char* data, int len);
	// ��ȡ�������������� 
	void GetXL(_account_* p, const char* data, int len);
	// ����
	void PickUpItem(_account_* p, const char* data, int len);
	// ����
	void ReBorn(_account_*p, const char* data, int lne);
	// ��Ϸ������
	void OffLine(_account_* p, const char* data, int len);

	// ѯ����������
	void AskXLCount(const char* msg);
	// ��������
	int Send(SOCKET s=0);
	// ��������
	int Send(SOCKET s, SOCKET_OPCODE opcode, bool clear = false);
	// ���͸������ʺ�
	int SendToOther(SOCKET s, SOCKET_OPCODE opcode, bool clear = false);

	static void OnConnect(SOCKET client, SOCKADDR_IN& info, int index);
	static void OnRead(SOCKET client, int index, int opcode, const char* data, int len);
	static void OnClose(SOCKET client, int index);
public:
	static GameServer* self;

	static Game*   m_pGame;
	MServer m_Server;

	// ���ڲ�����SOCKET
	SOCKET m_Socket;

	// �Ƿ��Ѿ������˶���
	int m_iCreateTeam = 0;
	// ����Ƿ�û��Կ��
	int m_iNoYaosi = 0;
	// ѯ���˶��ٸ��ʺ���������
	int m_iSendXL = 0;
	// Ӧ����������
	int m_iRecvXL = 0;
	// ����������
	int m_iOutFBCount = 0;
	// ����ˢ�˶����˸���
	int m_iPlayFB = 0;
	// ���˶��ٴ���
	int m_iOffLine = 0;
	// �����˶��ٴ�
	int m_iReBorn = 0;

	// ������ʱ��
	__int64 m_iOpenFBTime = 0;
};