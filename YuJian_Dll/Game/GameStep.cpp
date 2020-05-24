#include "Game.h"
#include "GameStep.h"
#include <ShlObj_core.h>
#include <stdio.h>
#include <memory.h>
#include <My/Common/mystring.h>
#include <My/Common/OpenTextFile.h>
#include <My/Common/Explode.h>
#include <My/Common/C.h>

#define ZeroStep(p) memset(p, 0, sizeof(_step_))
#define InitStep(n) _step_ n; ZeroStep(&n);

#define MAX_STEP 512

// ...
GameStep::GameStep()
{
	m_nStepRandIndex = -1;
	m_nStepCount = 0;
	ZeroMemory(m_NpcCoor, sizeof(m_NpcCoor));
	//InitSteps();
}

#if USE_MY_LINK
// ��ȡ��ǰ��ִ�еĲ���
_step_* GameStep::Current(Link<_step_*>& link)
{
	NODE<_step_*>* p = link.Current();

	if (p) {
		printf("Current:%s\n", p->value->Cmd);
	}
	
	return p ? p->value : nullptr;
}

// ��һ������ָ��
_step_ * GameStep::Prev(Link<_step_*>& link)
{
	NODE<_step_*>* p =  link.Prev();
	return p ? p->value : nullptr;
}

// �������ִ�в��� ������һ��
_step_* GameStep::CompleteExec(Link<_step_*>& link)
{
	link.Next();
	//INLOGVARN(32, "Index:%d", m_iStepIndex);
	return Current(link);
}

// ��ȡ��һ����������
_step_* GameStep::GetNext(Link<_step_*>& link)
{
	link.Store();
	NODE<_step_*>* p = link.Next();
	link.ReStore();
	return p ? p->value : nullptr;
}

// ��õ�ǰ���������
STEP_CODE GameStep::CurrentCode(Link<_step_*>& link)
{
	_step_* pStep = Current(link);
	return pStep ? pStep->OpCode : OP_UNKNOW;
}

// �����һ���������
STEP_CODE GameStep::NextCode(Link<_step_*>& link)
{
	return OP_UNKNOW;
}
#else
// ��ȡ��ǰ��ִ�еĲ���
_step_* GameStep::Current(vector<_step_*>& link)
{
	if (m_iStepIndex >= link.size())
		return nullptr;

	return link[m_iStepIndex];
}

// �������ִ�в��� ������һ��
_step_* GameStep::CompleteExec(vector<_step_*>& link)
{
	m_iStepIndex++;
	//INLOGVARN(32, "Index:%d", m_iStepIndex);
	return Current(link);
}

// ��õ�ǰ���������
STEP_CODE GameStep::CurrentCode(vector<_step_*>& link)
{
	_step_* pStep = Current(link);
	return pStep ? pStep->OpCode : OP_UNKNOW;
}

// �����һ���������
STEP_CODE GameStep::NextCode(vector<_step_*>& link)
{
	return OP_UNKNOW;
}
#endif

// ��ȡNPC�����ļ���Ϣ
bool GameStep::ReadNPCCoor(const char * path)
{
	char file[255];
	sprintf_s(file, "%s\\data\\pos.ini", path);

	printf("NPC�����ļ�:%s\n", file);
	OpenTextFile hfile;
	if (!hfile.Open(file)) {
		printf("�Ҳ���'%s'�ļ�������", file);
		return false;
	}

	m_nNpcCoorCount = 0;
	char key[64] = { 0 };
	int npc_index = 0, coor_index = 0;
	int length = 0;
	char data[128];
	while ((length = hfile.GetLine(data, 128)) > -1) {
		//printf("length:%d\n", length);
		if (length == 0) {
			continue;
		}

		Explode line(":", data);
		//printf("line:%d %s\n", line.GetCount(), line[0]);
		if (line.GetCount() > 1) {
			if (strstr(line[1], "��ʼ")) { // ��ʼ
				strcpy(key, line[0]);
				strcpy(m_NpcCoor[m_nNpcCoorCount].Name, line[0]);
				//printf("��ȡ%s��ʼ\n", key);
				continue;
	
			}
			else if (strstr(line[1], "����")) { // ����
				key[0] = 0;
				m_nNpcCoorCount++;
				//printf("��ȡ%s����\n", key);
				//printf("\n");
				continue;
			}
			
		}
		if (key[0]) {
			Explode arr(" ", data);
			if (arr.GetCount() > 1 && strstr(arr[0], "λ��") && strstr(arr[1], "���")) {
				Explode pos(":", arr[0]);
				Explode click(":", arr[1]);
				if (pos.GetCount() > 1 && click.GetCount() > 1) {
					_npc_coor_* pnc = &m_NpcCoor[m_nNpcCoorCount];

					if (strstr(pos[1], "-")) { // ������λ��
						Explode pos2("-", pos[1]);
						Explode pos_data(",", pos2[0]);
						Explode pos_data2(",", pos2[1]);

						pnc->Point[pnc->Length].MvX = pos_data.GetValue2Int(0);
						pnc->Point[pnc->Length].MvY = pos_data.GetValue2Int(1);
						pnc->Point[pnc->Length].MvX2 = pos_data2.GetValue2Int(0);
						pnc->Point[pnc->Length].MvY2 = pos_data2.GetValue2Int(1);
					}
					else {
						Explode pos_data(",", pos[1]);
						pnc->Point[pnc->Length].MvX = pos_data.GetValue2Int(0);
						pnc->Point[pnc->Length].MvY = pos_data.GetValue2Int(1);
					}

					if (strstr(click[1], "-")) { // ���������
						Explode click2("-", click[1]);
						Explode click_data(",", click2[0]);
						Explode click_data2(",", click2[1]);

						pnc->Point[pnc->Length].ClkX = click_data.GetValue2Int(0);
						pnc->Point[pnc->Length].ClkY = click_data.GetValue2Int(1);
						pnc->Point[pnc->Length].ClkX2 = click_data2.GetValue2Int(0);
						pnc->Point[pnc->Length].ClkY2 = click_data2.GetValue2Int(1);
					}
					else {
						if (strstr(click[1], "�Զ�����")) {
							pnc->Point[pnc->Length].ClkX = 0;
							pnc->Point[pnc->Length].ClkY = 0;
						}
						else {
							Explode click_data(",", click[1]);
							pnc->Point[pnc->Length].ClkX = click_data.GetValue2Int(0);
							pnc->Point[pnc->Length].ClkY = click_data.GetValue2Int(1);
						}
					}

					int rand = 1;
					if (arr.GetCount() > 2 && strstr(arr[2], "���")) {
						if (strstr(arr[2], "��"))
							rand = 0;
					}

					if (0 && m_nNpcCoorCount < 50) {
						printf("%d.%s %d.λ��:%d,%d-%d,%d ���:%d,%d-%d,%d ���:%d\n", m_nNpcCoorCount + 1,
							pnc->Name,
							pnc->Length + 1,
							pnc->Point[pnc->Length].MvX, pnc->Point[pnc->Length].MvY,
							pnc->Point[pnc->Length].MvX2, pnc->Point[pnc->Length].MvY2,
							pnc->Point[pnc->Length].ClkX, pnc->Point[pnc->Length].ClkY,
							pnc->Point[pnc->Length].ClkX2, pnc->Point[pnc->Length].ClkY2,
							rand);
					}

					pnc->Point[pnc->Length].Flag = rand;
					pnc->MvLength += rand;
					pnc->Length++;
				}
			}
		}
	}

	hfile.Close();
	return true;
}

// ��ȡNPC������Ϣ
_npc_coor_* GameStep::GetNpcCoor(const char * name)
{
	for (int i = 0; i < m_nNpcCoorCount; i++) {
		if (strcmp(name, m_NpcCoor[i].Name) == 0)
			return &m_NpcCoor[i];
	}

	return nullptr;
}

// ��ȡ����ִ�е������ļ�����
char* GameStep::GetStepFileName()
{
	int index = m_nStepRandListIndex[m_nStepRandIndex];
	return m_chStepFiles[index];
}

void GameStep::SetConsoleTle(const char* cmd)
{
	int index = m_nStepRandListIndex[m_nStepRandIndex];
	char console_title[256];
	if (cmd == nullptr) {
		sprintf_s(console_title, "����ʹ�������ļ�:%s\n", m_chStepFiles[index]);
	}
	else {
		sprintf_s(console_title, "����ʹ�������ļ�:%s ִ�в���:%s\n", m_chStepFiles[index], cmd);
	}
	SetConsoleTitleA(console_title);
}

// ���ѡ�����ļ�
char* GameStep::SelectRandStep()
{
	if (m_nStepCount == 1) { // ֻ��һ���ļ�
		m_nStepRandIndex = 0;
	}
	else {
		if (m_nStepRandIndex != -1) {
			if (++m_nStepRandIndex >= m_nStepCount)
				m_nStepRandIndex = -1;
		}
		if (m_nStepRandIndex == -1) { // δ��ʼ����������
			int i;
			for (i = 0; i < MAX_STEP_FILES; i++) {
				m_nStepRandListIndex[i] = i;
			}
			for (i = 0; i < m_nStepCount - 1; i++) { // ����
				int index = MyRand(0, m_nStepCount - 1);
				int tmp = m_nStepRandListIndex[i];
				m_nStepRandListIndex[i] = m_nStepRandListIndex[index];
				m_nStepRandListIndex[index] = tmp;
			}
			m_nStepRandIndex = 0;
		}
	}

	int index = m_nStepRandListIndex[m_nStepRandIndex];
	//index = 3;
	m_Step = m_StepList[index];
	SetConsoleTle(nullptr);

	printf("SelectRandStep:%s\n", m_Step.Current()->value->Cmd);

	return GetStepFileName();
}

// ��ʼ������
bool GameStep::InitSteps(const char* path, const char* file)
{
	char fb_file[255];
	sprintf_s(fb_file, "%s\\%s", path, file);
	
	printf("ˢ���������ļ�:%s\n", fb_file);
	OpenTextFile hfile;
	if (!hfile.Open(fb_file)) {
		printf("�Ҳ���'%s'�ļ�������", fb_file);
		//::MessageBoxA(NULL, fb_file, "msg", MB_OK);
		return false;
	}

	strcpy(m_chStepFiles[m_nStepCount], file);

	int i = 0, index = 0;
	int length = 0;
	char data[128];
	while ((length = hfile.GetLine(data, 128)) > -1) {
		//printf("length:%d\n", length);
		if (length == 0) {
			continue;
		}
		char log[64];

		int ret = ParseStep(trim(data), m_StepList[m_nStepCount]);
		if (ret == 0)
			break;
		if (ret > 0) {
			if (ret == 2) {
				index = i;
			}
			i++;
		}
	}
#if USE_MY_LINK
	//char box[256];
	//sprintf_s(box, "ִ������������%d\n", m_StepList[m_nStepCount].Count());
	//::MessageBoxA(NULL, box, "msg", MB_OK);
	printf("ִ������������%d\n", m_StepList[m_nStepCount].Count());
#else
	printf("ִ������������%d\n", m_Step.size());
#endif
	m_Step = m_StepList[m_nStepCount];
	ResetStep(index, 0x01);
	m_nStepCount++;

	_step_* c = Current(m_Step);

	printf("�״�ִ�в���:%d(%s)\n", index, c->Cmd);

	hfile.Close();
	return true;
}

// ��ʼ��ȥ��������
int GameStep::InitGoLeiMingSteps()
{
	ParseStep("�ƶ� 62,59", m_GoLeiMingStep); // �����ʦ
	ParseStep("��� 580,535 586,539", m_GoLeiMingStep); // �����ʦ
	ParseStep("�ȴ� 2 1", m_GoLeiMingStep);
	ParseStep("��� 708,598 720,606", m_GoLeiMingStep);
	ParseStep("�ȴ� 8 1", m_GoLeiMingStep);
	ParseStep("��� 710,330 720,335", m_GoLeiMingStep); // ��NPC
	ParseStep("�ȴ� 2 1", m_GoLeiMingStep);
	ParseStep("��� 67,360 260,393", m_GoLeiMingStep);  // ѡ���һ��
	ParseStep("�ȴ� 8 ", m_GoLeiMingStep);
	printf("��ʼ�������͵�������½�������:%d\n", m_GoLeiMingStep.Count());
	return m_GoLeiMingStep.Count();
}

int GameStep::ParseStep(const char* data, Link<_step_*>& link)
{
	Explode explode(" ", data);
	if (explode.GetCount() < 2)
		return -1;
	if (*data == '=')
		return 0;

	int result = 1;
	char* cmd = explode.GetValue(0);
	if (*cmd == '-') {
		printf("%s (%s)\n", data, cmd);
		result = 2;
		cmd++;
	}

	//printf("%s (%s)\n", data, cmd);
	InitStep(step);
	try {
		step.OpCode = TransFormOP(cmd);
		switch (step.OpCode)
		{
		case OP_MOVE:
		case OP_MOVEFAR:
		case OP_MOVERAND:
		case OP_MOVEPICKUP:
			if (!TransFormPos(explode[1], step)) {
				printf("GameStep::InitSteps.TransFormPosʧ��:%s\n", explode[1]);
				result = -1;
			}
			TransFormPos(explode[2], step.X2, step.Y2);
			//printf("����->�ƶ�:%d.%d��%d.%d\n", step.X, step.Y, step.X2, step.Y2);
			break;
		case OP_MOVENPC:
			step.p_npc = GetNpcCoor(explode[1]);
			if (!step.p_npc) {
				printf("NPC (%s) ��Ϣδ�ҵ�.\n", explode[1]);
				result = -1;
			}
			else {
				//printf("NPC (%s) ��Ϣ���ҵ�.\n", step.p_npc->Name);
			}
			break;
		case OP_NPC:
			strcpy(step.NPCName, explode[1]);
			if (explode.GetCount() > 2)
				TransFormPos(explode[2], step);
			if (explode.GetCount() > 3)
				TransFormPos(explode[3], step.X2, step.Y2);

			//printf("NPC:%s %d,%d\n", step.Name, step.X, step.Y);
			break;
		case OP_SELECT:
			if (is_number(explode[1])) {
				step.SelectNo = explode.GetValue2Int(1);
			}
			else {
				strcpy(step.Name, explode[1]);
				step.SelectNo = 0xff;
			}

			step.OpCount = explode.GetValue2Int(2);
			if (step.OpCount == 0)
				step.OpCount = 1;
			
			step.Extra[0] = explode.GetValue2Int(3);
			break;
		case OP_MAGIC:
		case OP_MAGIC_PET:
		case OP_KAWEI:
			TransFormMagic(explode, step);
			break;
		case OP_CRAZY:
		case OP_CLEAR:
			strcpy(step.Name, explode[1]);
			if (explode.GetCount() > 2) {
				strcpy(step.Magic, explode[2]);
			}
			if (explode.GetCount() > 3) {
				step.Extra[0] = explode.GetValue2Int(3);
			}
			break;
		case OP_KAIRUI:
			if (!TransFormPos(explode[1], step)) {
				printf("GameStep::InitSteps.TransFormPosʧ��:%s\n", explode[1]);
				result = -1;
			}
			if (explode.GetCount() > 2) {
				if (strstr(explode[2], ",")) { // ���½�ͼλ��
					TransFormPos(explode[2], step.Extra[0], step.Extra[1]);
				}
				if (explode.GetCount() > 3) {
					TransFormPos(explode[3], step.Extra[2], step.Extra[3]);
				}
				if (explode.GetCount() > 4) {
					step.Extra[4] = explode.GetValue2Int(4);
				}
			}
			break;
		case OP_PICKUP:
			strcpy(step.Name, explode[1]);
			if (explode.GetCount() > 2)
				TransFormPos(explode[2], step);
			if (explode.GetCount() > 3)
				TransFormPos(explode[3], step.X2, step.Y2);
			break;
		case OP_CHECKIN:
			break;
		case OP_USEITEM:
			strcpy(step.Name, explode[1]);     // ��Ʒ����
			if (explode.GetCount() > 2) {
				if (strstr(explode[2], ",")) { // �Ƿ���֤��������
					TransFormPos(explode[2], step);
				}
				if (explode.GetCount() > 3) {
					step.Extra[0] = explode.GetValue2Int(3);
				}
			}
			break;
		case OP_DROPITEM:
			strcpy(step.Name, explode[1]);     // ��Ʒ����
			if (explode.GetCount() > 2) {
				step.Extra[0] = explode.GetValue2Int(2);
			}
			break;
		case OP_SELL:
			break;
		case OP_BUTTON:
			break;
		case OP_CLICK:
		case OP_CLICKRAND:
		case OP_CLICKCRAZ:
			TransFormPos(explode[1], step);
			TransFormPos(explode[2], step.X2, step.Y2);
			step.OpCount = explode.GetValue2Int(3);
			if (step.OpCount == 0)
				step.OpCount = 1;
			break;
		case OP_FIND:
			strcpy(step.Name, explode[1]);     // ��������
			if (explode.GetCount() > 2) {
				step.Extra[0] = explode.GetValue2Int(2);
			}
			if (explode.GetCount() > 3) {
				step.Extra[1] = explode.GetValue2Int(3);
			}
			break;
		case OP_WAIT:
			TransFormWait(explode, step);
			break;
		case OP_WAIT_GW:
			strcpy(step.Name, explode[1]);     // ���������
			if (explode.GetCount() > 2) {
				step.Extra[0] = explode.GetValue2Int(2);
			}
			break;
		case OP_SMALL:
			TransFormSmall(explode, step);
			break;
		case OP_RECORD:
			break;
		default:
			result = -1;
			break;
		}
	}
	catch (...) {
		result = -1;
		printf("��ȡ���̴���:%s\n", data);
	}
	if (result) {
		_step_* pStep = new _step_;
		memcpy(pStep, &step, sizeof(_step_));
		strcpy(pStep->Cmd, data);
		char* tmp = pStep->Cmd;
		while (*tmp) {
			if (*tmp == '#') { // �Ѻ����ע��Ūȥ
				*tmp = 0;
				break;
			}
			tmp++;
		}

		NODE<_step_*>* node = link.Add(pStep);
		pStep->Index = node->index;
	}
	return result;
}

// ת��ʵ��ָ��
STEP_CODE GameStep::TransFormOP(const char* data)
{
	if (data == nullptr)
		return OP_UNKNOW;

	if (strcmp(data, "�ƶ�") == 0)
		return OP_MOVE;
	if (strcmp(data, "����") == 0)
		return OP_MOVEFAR;
	if (strcmp(data, "����") == 0)
		return OP_MOVERAND;
	if (strcmp(data, "�Ƽ�") == 0)
		return OP_MOVEPICKUP;
	if (strcmp(data, "����") == 0)
		return OP_MOVENPC;
	if (strcmp(data, "NPC") == 0 || strcmp(data, "�Ի�") == 0)
		return OP_NPC;
	if (strcmp(data, "ѡ��") == 0 || strcmp(data, "ѡ��") == 0)
		return OP_SELECT;
	if (strcmp(data, "����") == 0)
		return OP_MAGIC;
	if (strcmp(data, "�輼") == 0)
		return OP_MAGIC_PET;
	if (strcmp(data, "��λ") == 0)
		return OP_KAWEI;
	if (strcmp(data, "��˦") == 0 || strcmp(data, "ȫ��") == 0)
		return OP_CRAZY;
	if (strcmp(data, "���") == 0)
		return OP_CLEAR;
	if (strcmp(data, "����") == 0)
		return OP_KAIRUI;
	if (strcmp(data, "����") == 0)
		return OP_PICKUP;
	if (strcmp(data, "����") == 0)
		return OP_CHECKIN;
	if (strcmp(data, "ʹ��") == 0)
		return OP_USEITEM;
	if (strcmp(data, "����") == 0)
		return OP_DROPITEM;
	if (strcmp(data, "����") == 0)
		return OP_SELL;
	if (strcmp(data, "����") == 0)
		return OP_SELL;
	if (strcmp(data, "��ť") == 0 || strcmp(data, "ȷ��") == 0)
		return OP_BUTTON;
	if (strcmp(data, "���") == 0)
		return OP_CLICK;
	if (strcmp(data, "���") == 0)
		return OP_CLICKRAND;
	if (strcmp(data, "���") == 0)
		return OP_CLICKCRAZ;
	if (strcmp(data, "����") == 0)
		return OP_FIND;
	if (strcmp(data, "�ȴ�") == 0)
		return OP_WAIT;
	if (strcmp(data, "�ȹ�") == 0)
		return OP_WAIT_GW;
	if (strcmp(data, "С��") == 0)
		return OP_SMALL;
	if (strcmp(data, "��¼") == 0)
		return OP_RECORD;

	return OP_UNKNOW;
}

// ת��ʵ������
bool GameStep::TransFormPos(const char* str, _step_& step)
{
	Explode explode(",", str);
	if (explode.GetCount() == 2) {
		step.X = explode.GetValue2Int(0);
		step.Y = explode.GetValue2Int(1);
	}

	//printf("TransFormPos:%s(%d)\n", str, explode.GetCount());
	return explode.GetCount() == 2;
}

// ת��ʵ������
bool GameStep::TransFormPos(const char * str, DWORD & x, DWORD & y)
{
	if (str == nullptr)
		return false;

	Explode explode(",", str);
	if (explode.GetCount() == 2) {
		x = explode.GetValue2Int(0);
		y = explode.GetValue2Int(1);
	}

	return explode.GetCount() == 2;
}

// ת��ʵ�ʼ���
bool GameStep::TransFormMagic(Explode& line, _step_ & step)
{
	//printf("TransFormMagic\n");
	if (strstr(line[1], "*") == nullptr) {
		strcpy(step.Magic, line[1]);
		step.OpCount = 1;
	}
	else {
		Explode m("*", line[1]);
		strcpy(step.Magic, m[0]);
		step.OpCount = m.GetValue2Int(1);
	}
	if (line.GetCount() > 2) {
		int index = 2;
		if (strstr(line[2], ",")) { // ����������
			TransFormPos(line[2], step);
			index++;
		}

		int s = line.GetValue2Int(index);
		if (s < 100)
			s *= 1000;

		step.WaitMs = s;
	}
	//printf("TransFormMagic End\n");

	return true;
}

// ת��Wait����
bool GameStep::TransFormWait(Explode& line, _step_& step)
{
	int index = 1;
	int test = line.GetValue2Int(1);
	if (test == 0) { // �ȴ�������ȴ
		strcpy(step.Magic, line[1]);
		index++;
	}
	int v = line.GetValue2Int(index);
	step.WaitMs = v < 90 ? v * 1000 : v;

	step.Extra[0] = line.GetValue2Int(2);

	return 0;
}

// С��
void GameStep::TransFormSmall(Explode & line, _step_ & step)
{
	int v = -1;
	if (line.GetCount() > 1) {
		if (strcmp(line[1], "������") == 0)
			v = 0;
		if (strcmp(line[1], "������") == 0)
			v = 1;
	}
	step.SmallV = v;
}

// ��Ӳ���
void GameStep::AddStep(_step_& step)
{
	//memset(&m_Steps[m_iStepIndex], 0, sizeof(_step_));
	memcpy(&m_Steps[m_iStepIndex], &step, sizeof(_step_));
	//INLOGVARN(32, "��Ӳ���:%d Exec:%d", step.OpCode, step);
	m_iStepCount = ++m_iStepIndex;
}

// ����ִ�в�������
void GameStep::ResetStep(int index, int flag)
{
	printf("ResetStep:%d\n", index);
	if (index > 0) {
		m_iStepStartIndex = index;
	}
	m_iStepIndex = index;

	if ((flag & 0x01) && m_iStepIndex > 0) {
		m_Step.Reset(m_iStepIndex);
		
	}	
	else if (flag & 0x02)
		m_GoLeiMingStep.Reset(m_iStepIndex);
}