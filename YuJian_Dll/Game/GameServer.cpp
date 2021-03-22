#include <time.h>
#include "GameServer.h"
#include "Game.h"
#include "GameProc.h"
#include <My/Common/Explode.h>

#define RECV_NAME(var,data,len)  char var[32]; ZeroMemory(var,sizeof(var)); memcpy(var,data,len); 
#define RECV_ROLE(var,data,len)  char var[16]; ZeroMemory(var,sizeof(var)); memcpy(var,data,len); 

GameServer* GameServer::self = nullptr;
Game* GameServer::m_pGame = nullptr;

// ...
GameServer::GameServer(Game * p)
{
	m_pGame = p;
	m_iOpenFBTime = 0;
}

void GameServer::SetSelf(GameServer * p)
{
	self = p;
	m_iNoYaosi = 0;
}

// ����
void GameServer::Listen(USHORT port)
{
	m_Server.onconect = OnConnect;
	m_Server.onread = OnRead;
	m_Server.onclose = OnClose;

	LOGVARN(64, L"������������, �˿�:%d", port);
	m_Server.Listen(port);
}

// ��Ϸ���ڴ���Ӧ
void GameServer::Open(const char* data, int len)
{
	wchar_t log[64];
	LOG(L"��Ϸ�����Ѵ�, ׼������");
	Account* p = m_pGame->GetReadyAccount(true);
	if (!p) {
		LOG(L"û��Ҫ�������ʺ�");
		return;
	}
	if (m_pGame->GetOnLineCount() > 2) {
		m_pGame->m_iSendCreateTeam = 0;
	}

	m_pGame->SetStatus(p, ACCSTA_LOGIN, true);
	m_pGame->SetSocket(p, m_Socket);
	m_pGame->SetFlag(p, 1);

	// �����Ƿ���ʾ����
	m_Server.ClearSendString();
	m_Server.SetInt(m_pGame->m_Setting.IsDebug);
	Send(0, SCK_DEBUG, false);

	// �����Ƿ��ֶ�ѡ��
	m_Server.ClearSendString();
	m_Server.SetInt(m_pGame->m_Setting.NoAutoSelect);
	Send(0, SCK_NOAUOTOSLT, false);

	// �����Ƿ�������
	m_Server.ClearSendString();
	m_Server.SetInt(m_pGame->m_Setting.TalkOpen);
	Send(0, SCK_TALK_OPEN, false);

	// ����ѡ���ĸ���ɫ
	m_Server.ClearSendString();

	// �����ǰ·��
	m_Server.SetInt(strlen(m_pGame->m_chPath));
	m_Server.SetContent((void*)m_pGame->m_chPath, strlen(m_pGame->m_chPath));

	// �ĸ���Ϸ����
	char* ser_big = strlen(p->SerBig) > 0 ? p->SerBig : m_pGame->m_Setting.SerBig;
	m_Server.SetInt(strlen(ser_big));
	m_Server.SetContent((void*)ser_big, strlen(ser_big));

	// �ĸ���ϷС��
	char* ser_small = strlen(p->SerSmall) > 0 ? p->SerSmall : m_pGame->m_Setting.SerSmall;
	m_Server.SetInt(strlen(ser_small));
	m_Server.SetContent((void*)ser_small, strlen(ser_small));

	m_Server.SetInt(strlen(p->Name));
	m_Server.SetContent(p->Name, strlen(p->Name));

	m_Server.SetInt(strlen(p->Password));
	m_Server.SetContent(p->Password, strlen(p->Password));

	m_Server.SetInt(p->RoleNo);
	m_Server.SetInt(m_pGame->m_Setting.NoGetXL ? 1 : p->IsGetXL);
	m_Server.SetInt(p->IsBig);
	m_Server.SetInt(m_pGame->m_Setting.LogoutByGetXL);
	m_Server.SetInt(m_pGame->m_iSendCreateTeam);
	Send(0, SCK_ACCOUNT, false);

	memcpy(&m_pGame->m_fScale, data, 4);
	memcpy(&p->GameWnd, data + 4, 4);
	memcpy(&p->GameRect, data + 8, sizeof(RECT));

	if (m_pGame->m_iSendCreateTeam) // ��һ�δ�������
		m_pGame->m_iSendCreateTeam = 0;
}

// ��¼
void GameServer::Login(_account_ * p)
{
	SetForegroundWindow(p->GameWnd);
	m_pGame->Login(p, 0);
}

// ��Ϣ
void GameServer::Msg(_account_* p, const char* data, int len)
{
	if (len > 0) {
		if (strstr(data, "|") == nullptr) {
			LOGVARN2(128, "c0", L"(MSG)%hs:%hs", p->Name, data);
		}
		else {
			Explode arr("|", data);
			LOGVARN2(128, arr[1], L"(MSG)%hs:%hs", p->Name, arr[0]);
		}	
	}
		
}

// �ѽ�����Ϸ
void GameServer::InGame(_account_* p, const char* data, int len)
{
	wchar_t log[64];	

	char name[32], role[16];
	ZeroMemory(name, sizeof(name));
	ZeroMemory(role, sizeof(role));

	const char* tmp = data;
	int name_len = P2INT(tmp);
	memcpy(name, tmp + 4, name_len);
	tmp += 4 + name_len;

	int role_len = P2INT(tmp);
	memcpy(role, tmp + 4, role_len);

	if (!name_len || !role_len) {
		LOGVARP(log, L"%hs�ʺŲ����б�", name);
		DbgPrint("%hs�ʺŲ����б�\n", name);
		goto end;
	}

	p = m_pGame->GetAccount(name);
	if (!p) {
		LOGVARP(log, L"%hs�ʺŲ����б�2", name);
		DbgPrint("%hs�ʺŲ����б�\n", name);
		goto end;
	}

	m_pGame->SetStatus(p, ACCSTA_ONLINE, true);

	p->IsGetXL = 1;
	p->OfflineLogin = 1;
	p->Socket = m_Socket;
	p->LoginTime = 0;
	p->PlayTime = time(nullptr);
	memset(p->Role, 0, sizeof(p->Role));
	memcpy(p->Role, role, strlen(role));
	LOGVARP(log, L"%hs[%hs]�ѵ�����Ϸ", p->Name, p->Role);
end:
	if (!m_pGame->AutoLogin("GameServer::InGame")) {
		m_pGame->LoginCompleted("�ѽ�����Ϸ");
		//m_pGame->m_pUIWnd->m_pLoginBtn->SetText("�Զ��Ǻ�");
		//m_pGame->m_pUIWnd->m_pStartBtn->SetEnabled(true);
	}
}

// �������
void GameServer::CanTeam(_account_* p, const char* data, int len)
{
	wchar_t log[64];
	if (0 && p->IsBig) {
		LOGVARP(log, L"%hs �Ѵ����ö���", p->Name);
		m_pGame->SetReady(p, 1);
		m_pGame->AddReadyCount(1);
		return;
	}
	
	DbgPrint("%hs(%hs)��׼����,�������\n", p->Name, p->Role);
	LOGVARP(log, L"%hs ��׼����,�������", p->Name);
	// ���͸�������ҿ���������ʺ������[���������Ҫ�ж��Լ��Ƿ�Ϊ�ӳ�������]
	m_Server.ClearSendString();
	m_Server.SetInt(p->Index);
	m_Server.SetContent(&p->Role, sizeof(p->Role));
	SendToOther(p->Socket, SCK_CANTEAM, false);
}

// ���
void GameServer::InTeam(_account_* p, const char* data, int len)
{
	wchar_t log[64];
	int index = P2INT(data);
	int result = P2INT(data + 4);
	Account* act = m_pGame->GetAccount(index);
	if (!act) {
		LOGVARP(log, L"�ʺŲ��ڴ��б�,�޷����[%d]", index);
		return;
	}

	m_iCreateTeam = 1;
	DbgPrint("%hs ���� %hs ���%hs\n", p->Name, act->Name, result ? "�ɹ�" : "ʧ��");
	LOGVARP(log, L"%hs ���� %hs ���%hs", p->Name, act->Name, result?"�ɹ�":"ʧ��");
	if (!result)
		return;

	if (act->IsBig) { // ���ģ����
		Sleep(1500);
		m_pGame->m_pGameProc->InTeam(m_pGame->m_pBig);
		CanInFB(act, nullptr, 0);
	}
	else {
		// ���ߴ˺ſ������
		Send(act->Socket, SCK_INTEAM, true);
	}
}

// ���Խ�������
void GameServer::CanInFB(_account_ * p, const char * data, int len)
{
	m_iCreateTeam = 1;
	wchar_t log[64];
	if (p->IsReady == 0) {
		m_pGame->SetReady(p, 1);
		m_pGame->AddReadyCount();
	}
	LOGVARP(log, L"%hs �Ѿ������[%d/%d]", p->Name, m_pGame->m_iReadyCount, m_pGame->m_iLoginCount);

	if (m_pGame->IsAllReady()) {
		m_iCreateTeam = 0;
		printf("CanInFB->ѯ������\n");
		AskXLCount("���Խ�����");
	}
}

// ��������
void GameServer::OpenFB(_account_ * p, const char * data, int len)
{
}

// ���븱��
void GameServer::InFB(_account_ * p, const char * data, int len)
{
	LOGVARN(64, L"%hs������븱��[%d]", p->Name, p->Socket);
	printf("%hs������븱�� [%d]\n", p->Name, p->Socket);
	SendToOther(p->Socket, SCK_INFB, true);

	if (!p->IsBig) {
		Sleep(1500);
		m_pGame->m_pGameProc->InFB(m_pGame->m_pBig);
	}
}

// ��ȥ����
void GameServer::OutFB(_account_ * p, const char * data, int len)
{
	m_iOutFBCount++;
	m_pGame->SetStatus(p, ACCSTA_ONLINE);
	m_pGame->UpdateAccountStatus(p);
	m_pGame->UpdateFBCountText(m_iOutFBCount, true);

	wchar_t log[64];
	LOGVARP2(log, "blue", L"%hs������", p->Name);

	if (!m_pGame->m_pHome->IsValid()) {
		SendToOther(0, SCK_EXPIRE_OUT, true);
		LOGVARP2(log, "red", L"�����ѹ���, �뼤��.");
		m_pGame->Alert(L"�����ѹ���, �뼤��.", 2);
	}
}

// ���ڸ���
void GameServer::AtFB(_account_* p, const char * data, int len)
{
	p->AtFBTime = time(nullptr);
	m_pGame->SetStatus(p, ACCSTA_ATFB);
	m_pGame->UpdateAccountStatus(p);

	LOGVARN2(64, "blue", L"%hs��ʼˢ����", p->Name);
}

// С�Ž��븱��
void GameServer::SmallInFB(_account_ * p, const char * data, int len)
{
	if (p->IsBig) {
		printf("����SCK_SMALLINFB\n");
		SendToOther(p->Socket, SCK_SMALLINFB, true);
	}
}

// С�ų�ȥ����
void GameServer::SmallOutFB(_account_ * p, const char * data, int len)
{
	if (p->IsBig) {
		printf("����SCK_SMALLOUTFB\n");
		SendToOther(p->Socket, SCK_SMALLOUTFB, true);
	}
}

// ��ȡ�������������� 
void GameServer::GetXL(_account_* p, const char * data, int len)
{
	if (m_pGame->ClockShutDown(0))
		return;

	wchar_t log[64];
	p->XL = data ? P2INT(data) : 0;
	printf("%hs ӵ������:%d (%d/%d)\n", p->Name, p->XL, m_iRecvXL+1, m_iSendXL);
	LOGVARP(log, L"%hsӵ������:%d (%d/%d)", p->Name, p->XL, m_iRecvXL + 1, m_iSendXL);

	if (!m_iSendXL) {
		LOGVARP2(log, "red b", L"ѯ����������Ϊ��, ����������");
		++m_iRecvXL;
		return;
	}

	if (++m_iRecvXL >= m_iSendXL) {
		__int64 now_time = time(nullptr);
		if (m_iOpenFBTime) {
			if ((m_iOpenFBTime + 30) > now_time) {
				LOGVARP2(log, "red b", L"30�����Ѿ����˿����˸���");
				return;
			}
		}
		m_iOpenFBTime = now_time;
		bool big_in = false; // �Ƿ��Ž�
		SYSTEMTIME stLocal;
		::GetLocalTime(&stLocal);
		if (m_iNoYaosi == 0 && stLocal.wHour >= 20 && stLocal.wHour <= 23) { // 20�㵽24�������ѽ�
			big_in = stLocal.wHour == 23 ? stLocal.wMinute < 59 : true; // ������Ҫ1����
		}
		if (big_in) {
			Account* last = nullptr;
			Account* act = m_pGame->GetMaxXLAccount(&last);
			if (!act) {
				LOGVARP2(log, "blue b", L"С��û��Կ��, ��[���]��������(%d)\n", m_iRecvXL);

				m_pGame->SetStatus(m_pGame->GetBigAccount(), ACCSTA_OPENFB, true);
				m_pGame->m_pGameProc->OpenFB();
			}
			else {
				LOGVARP(log, L"��%hs[%hs]��Կ�׿�������(����)", act->Name, act->Role);

				m_pGame->SetStatus(act, ACCSTA_OPENFB, true);
				Send(act->Socket, SCK_OPENFB, true);
			}
			
			//InFB(m_pGame->m_pBig, nullptr, 0);
			return;
		}

		Account* last = nullptr;
		Account* act = m_pGame->GetMaxXLAccount(&last);
		if (!act) {
			if (last && !last->IsBig) { // ���Ǵ��
				Account* next = m_pGame->GetNextLoginAccount();
				if (next) {
					LOGVARP2(log, "red b", L"%hsû������, %hs׼����¼", last->Name, next->Name);

					m_pGame->m_iSendCreateTeam = 0;

					last->Flag = 1;
					m_pGame->SetCompleted(last);         // ���������
					m_Server.ClearSendString();
					m_Server.SetInt(1);
					Send(last->Socket, SCK_CLOSE, false); // ֪ͨ��Ϸ�ر�
				}
				else {
					LOG2(L"�Ѿ�û����������,�޷�����\n", "red b");
					m_pGame->Alert(L"�Ѿ�û����������,�޷�����", 2);
					m_pGame->AutoShutDown();
				}
			}
			else {
				LOG2(L"û����������,�޷�����\n", "red b");
				m_pGame->AutoShutDown();
			}
		}
		else {
			m_pGame->SetStatus(act, ACCSTA_OPENFB, true);
			int ret = Send(act->Socket, SCK_OPENFB, true);

			printf("��%hs[%hs]��������(����)(%d)\n", act->Name, act->Role, ret);
			LOGVARP(log, L"��%hs[%hs]��������(����)(%d)", act->Name, act->Role, ret);
		}
	}
}

// ����
void GameServer::PickUpItem(_account_ * p, const char * data, int len)
{
	//LOGVARN(64, "%hs ʰȡ.%hs", p->Name, data);
	m_pGame->UpdateDBItem(p->Name, data);
}

// ����
void GameServer::ReBorn(_account_ * p, const char * data, int lne)
{
	return;
	LOGVARN(64, L"%hs[%hs]����", p->Name, p->Role);
	if (p->IsBig) {
		m_iReBorn++;
		m_pGame->UpdateOffLineAllText(m_iOffLine, m_iReBorn);
	}	
}

// ��Ϸ������
void GameServer::OffLine(_account_ * p, const char * data, int len)
{
	return;
	m_pGame->SetStatus(p, ACCSTA_OFFLINE);
	m_pGame->UpdateAccountStatus(p);
	return;
	LOGVARN(64, L"%hs������", p->Name);
	
	if (m_pGame->m_Setting.ReConnect && m_pGame->IsAutoLogin()) { // ֪ͨ��Ϸ�ر�
		m_Server.ClearSendString();
		m_Server.SetInt(1);
		Send(p->Socket, SCK_CLOSE, false);
	}
}

// ѯ����������
void GameServer::AskXLCount(const char* msg)
{
	if (m_pGame->ClockShutDown(0))
		return;

	wchar_t log[64];
	if (!m_pGame->m_pHome->IsValid()) {
		LOGVARP2(log, "red", L"�����ѹ���, �뼤��");
		m_pGame->Alert(L"�����ѹ���, �뼤��.", 2);
		return;
	}
	m_iOutFBCount = 0;
	m_iRecvXL = 0;
	m_iSendXL = SendToOther(0, SCK_GETXL, true);
	if (m_iSendXL > 5)
		m_iSendXL = 5;

	if (m_pGame->GetAccountCount() == 1 && m_pGame->m_pBig) { // ֻ�����ô��
		GetXL(m_pGame->m_pBig, nullptr, 0);
	}

	printf("ѯ�ʽ�������������, ѯ������:%d\n", m_iSendXL);
	LOGVARP(log, L"ѯ�ʽ�������������,ѯ������:%d(%hs)", m_iSendXL, msg);
}

// ��������
int GameServer::Send(SOCKET s)
{
	int ret = send(s ? s : m_Socket, m_Server.GetSendString(), m_Server.GetSendLength(), 0);
	//printf("����:%d %d\n", m_Server.GetSendLength(), ret);
	return ret;
}

// ��������
int GameServer::Send(SOCKET s, SOCKET_OPCODE opcode, bool clear)
{
	if (clear) {
		m_Server.ClearSendString();
	}
	m_Server.MakeSendString(opcode);
	//printf("����:%d %08X\n", opcode, s);
	return Send(s);
}

// ���͸������ʺ�
int GameServer::SendToOther(SOCKET s, SOCKET_OPCODE opcode, bool clear)
{
	if (clear) {
		m_Server.ClearSendString();
	}
	m_Server.MakeSendString(opcode);
	int ret = 0;
	int count = m_pGame->GetAccountCount();
	for (int i = 0; i < count; i++) {
		Account* p = m_pGame->GetAccount(i);
		if (p && m_pGame->IsOnline(p) && p->Socket != s) {
			if (opcode == SCK_SMALLINFB) {
				m_pGame->SetStatus(p, ACCSTA_ATFB, true);
			}
			else if (opcode == SCK_SMALLOUTFB) {
				m_pGame->SetStatus(p, ACCSTA_ONLINE, true);
			}

			Send(p->Socket);
			ret++;
		}
	}
	return ret;
}

void GameServer::OnConnect(SOCKET client, SOCKADDR_IN& info, int index)
{
	self->m_Socket = client;
	self->m_Server.ClearSendString();
	//self->m_Server.MakeSendString(SCK_FIRST);
	//self->Send(client);
}

void GameServer::OnRead(SOCKET client, int index, int opcode, const char* data, int len)
{
	self->m_Socket = client;

	Account* p = self->m_pGame->GetAccountBySocket(client);
	if (!p && opcode != SCK_OPEN && opcode != SCK_INGAME) {
		printf("�Ҳ�����Ӧ�ʺ�\n");
		return;
	}
	//printf("%hs OpCode:%d %X\n", p ? p->Name : "null", opcode, client);
	if (p) {
		p->LastTime = time(nullptr);
		self->m_pGame->UpdateAccountStatus(p);
	}
	switch (opcode)
	{
	case SCK_OPEN:   // ��Ϸ�����Ѵ�
		self->Open(data, len);
		break;
	case SCK_LOGIN:  // ���Ե�¼��
		self->Login(p);
		break;
	case SCK_MSG:
		self->Msg(p, data, len);
		break;
	case SCK_MSG2:
		self->m_pGame->WriteLog(data);
		break;
	case SCK_INGAME: // �ѽ�����Ϸ
		self->InGame(p, data, len);
		break;
	case SCK_CANTEAM: // �������
		self->CanTeam(p, data, len);
		break;
	case SCK_INTEAM:  // ���Խ�����
		self->InTeam(p, data, len);
		break;
	case SCK_CANINFB:  // ���Խ�����
		self->CanInFB(p, data, len);
		break;
	case SCK_OPENFB: // ��������
		self->OpenFB(p, data, len);
		break;
	case SCK_INFB:   // ���븱��
		self->InFB(p, data, len);
		break;
	case SCK_OUTFB:  // ��ȥ����
		self->OutFB(p, data, len);
		break;
	case SCK_ATFB:  // ���ڸ���
		self->AtFB(p, data, len);
		break;
	case SCK_SMALLINFB:   // С�Ž��븱��
		self->SmallInFB(p, data, len);
		break;
	case SCK_SMALLOUTFB:  // С�ų�ȥ����
		self->SmallOutFB(p, data, len);
		break;
	case SCK_GETXL:  // ��ȡ����
		self->GetXL(p, data, len);
		break;
	case SCK_NOYAOSI: // ���û��Կ��
		LOG2(L"С��û��Կ��, ��{���}��������", "blue b");
		self->m_pGame->SetStatus(self->m_pGame->GetBigAccount(), ACCSTA_OPENFB, true);
		self->m_pGame->m_pGameProc->OpenFB();
		break;
	case SCK_PICKUPITEM: // ����Ʒ
		self->PickUpItem(p, data, len);
		break;
	case SCK_REBORN: // ����
		self->ReBorn(p, data, len);
		break;
	case SCK_OFFLINE: // ��Ϸ�ѵ���
		self->OffLine(p, data, len);
		break;
	case SCK_CLOSE:
		self->OnClose(client, index);
		break;
	default:
		break;
	}
}

void GameServer::OnClose(SOCKET client, int index)
{
	wchar_t log[64];
	Account* p = self->m_pGame->GetAccountBySocket(client);
	printf("��Ϸ�ر�:%hs %d\n", p ? p->Name : nullptr, client);
	LOGVARP2(log, "red", L"��Ϸ�ر�:%hs %d", p ? p->Name : nullptr, client);
	if (!p)
		p = self->m_pGame->GetReadyAccount();
	if (p) {
		if (!self->m_pGame->CheckStatus(p, ACCSTA_COMPLETED)) {
			p->PlayTime = 0;
			self->m_pGame->SetStatus(p, ACCSTA_OFFLINE);
			self->m_pGame->UpdateAccountStatus(p);
		}
		
		LOGVARP2(log, "red", L"%hs[%hs]���˳���Ϸ", p->Name, p->Role);
	}
	if (p && p->Flag == 1) {
		if (p->IsBig && p->OfflineLogin) {
			self->m_iOffLine++;
			self->m_pGame->UpdateOffLineAllText(self->m_iOffLine, self->m_iReBorn);
			self->m_pGame->WriteLog("���˳���Ϸ\n-----------------------------------\n\n\n");
#if 0
			p->PlayTime = 0;
			self->m_pGame->SetStatus(p, ACCSTA_OFFLINE);
			self->m_pGame->m_pUIWnd->UpdateAccountStatus(p);
			LOGVARN(64, "%hs[%hs] ���˳���Ϸ", p->Name, p->Role);
			return;
#endif
		}
		p->XL = 0;
		self->m_pGame->SetReady(p, 0);
		self->m_pGame->AddReadyCount(-1); // ��׼�����ʺ�
		self->m_pGame->SetFlag(p, 0); // ��־�Ѵ����˳� 

		if (self->m_pGame->m_Setting.LogoutByGetXL) { // �����������˳�
			self->m_pGame->SetCompleted(p);
		}

		if (self->m_pGame->CheckStatus(p, ACCSTA_COMPLETED)) {  // �����
			//printf("self->m_pGame->CheckStatus(p, ACCSTA_COMPLETED)\n");
			Account* next = self->m_pGame->GetNextLoginAccount();
			if (next) { // �Զ���¼��һ����
				printf("��һ��Ҫ��¼���ʺ�:%hs:%d %08X\n", next->Name, next->Index, next->Status);
				LOGVARP2(log, "blue", L"��һ��Ҫ��¼���ʺ�:%hs(%d) %08X\n", next->Name, next->Index, next->Status);
				self->m_pGame->SetLoginFlag(next->Index);
				self->m_pGame->AutoLogin("GameServer::OnClose:��һ��");
			}
			else if (self->m_pGame->m_Setting.LogoutByGetXL && !self->m_pGame->m_Setting.NoPlayFB) { // ������
				self->m_pGame->m_Setting.LogoutByGetXL = 0;
				self->m_pGame->SetAllStatus(ACCSTA_INIT);
				self->m_pGame->SetLoginFlag(-1);
				self->m_pGame->AutoLogin("GameServer::OnClose::������");
			}
			return;
		}

		if (self->m_pGame->IsAutoLogin() && p->OfflineLogin && self->m_pGame->m_Setting.ReConnect) {
			self->m_pGame->SetLoginFlag(p->Index);
			self->m_pGame->AutoLogin("GameServer::OnClose::�رպ����µ�¼");
		}
	}
}
