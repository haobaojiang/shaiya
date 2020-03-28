#include "stdafx.h"
#include "CFFA.h"



bool CFFA::process(Pshaiya50_player player, WORD Map, Pshaiya_pos pos) {
	bool bRet = false;
	do
	{
		if (!player->party) {
			sendNoticeToplayer(player, "������ٽ���,ok?");
			break;
		}

		if (!isgateTime()) {
			sendNoticeToplayer(player, "û��ʱ��������Ѿ��ؿ�!");
			break;
		}


		if (m_country != 2 && player->Country != m_country) {
			sendNoticeToplayer(player, "�����ڵ���Ӫ������Ҫ��!");
			break;
		}



		if (player->party->Count < m_minmembers) {
			sendNoticeToplayer(player, "������������!");
			break;
		}
 			

		if (GetLeader(player->party) != player) {
			sendNoticeToplayer(player, "���ŵ���ֻ���Ƕӳ�!");
			break;
		}

		Pshaiya_party party = player->party;
		if (!addParty(player)) {
			break;
		}
			
		//�Ѷ������г�Ա�ƶ�����ͼ��
		for (DWORD i = 0; i < party->Count; i++) {
			Pshaiya50_player pTarget = Pshaiya50_player(party->member[i].player);
			if (pTarget&&pTarget != player) {
				LOGD << "moving player to ffp map,playerName:" << pTarget->charname << "belongs to leader->" << player->charname;
				MovePlayer((DWORD)pTarget, Map, pos->x, pos->z);
			}
		}
		bRet = true;
		break;

	} while (0);
	return bRet;
}
void CFFA::loadConfig() {

	WCHAR szTime[MAX_PATH] = { 0 };
	ZeroMemory(m_IsgateTime,   sizeof(m_IsgateTime));
	ZeroMemory(m_isAttackTime, sizeof(m_isAttackTime));
	m_Map = GetPrivateProfileInt(m_appName, L"map", 0, g_szFilePath);
	m_minmembers = GetPrivateProfileInt(m_appName, L"minmembers", 0, g_szFilePath);
	m_maxPartys = GetPrivateProfileInt(m_appName, L"maxpartys", 0, g_szFilePath);
	m_minlevel = GetPrivateProfileInt(m_appName, L"minlevel", 1, g_szFilePath);
	m_kills = GetPrivateProfileInt(m_appName, L"kills", 1, g_szFilePath);
	m_maxDeadCount = GetPrivateProfileInt(m_appName, L"deadcount", 100, g_szFilePath);
	m_country= GetPrivateProfileInt(m_appName, L"country", 2, g_szFilePath);
	DWORD gateTime= GetPrivateProfileInt(m_appName, L"gatetime", 0, g_szFilePath);
	DWORD penddingtime = GetPrivateProfileInt(m_appName, L"penddingtime", 0, g_szFilePath);



	GetPrivateProfileString(m_appName, L"time", L"", szTime, MAX_PATH, g_szFilePath);

	if (!m_Map || !m_minmembers || !m_maxPartys||szTime[0]==L'\0') {
		return;
	}



	WCHAR *delim = L")";
	WCHAR* p = wcstok(szTime, delim);
	while (p){
		FFPTASK st;
		st.minute = 0;
		st.beginDay = 0;
		st.endDay = 0;
		swscanf(p,L"(%hd,%hd,%hd", &st.dayOfweek,&st.hour,&st.duration);

		memset(m_isAttackTime[st.dayOfweek][st.hour], 1, 60 * st.duration);


		memset(m_IsgateTime   [st.dayOfweek][st.hour], 1, gateTime);
		memset(m_isAttackTime[st.dayOfweek][st.hour],  0, penddingtime);

		m_vectasks.push_back(st);
		p = wcstok(NULL, delim);
	}
}


bool CFFA::isAttackTime() {
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	return m_isAttackTime[stTime.wDayOfWeek][stTime.wHour][stTime.wMinute];
}


bool CFFA::isgateTime() {
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	return m_IsgateTime[stTime.wDayOfWeek][stTime.wHour][stTime.wMinute];
}




PPARTYKILLRECORD CFFA::getsignedParty(Pshaiya_party party) {

	PPARTYKILLRECORD p = NULL;
	lock();
	for (auto iter = m_Party.begin(); iter != m_Party.end(); iter++) {

		if (iter->pParty == party) {
			p= &(*iter);
			break;
		}
	}
	unlock();
	return p;
}

bool CFFA::isExistedMember(Pshaiya50_player player) {
	lock();
	for (auto iter = m_Party.begin(); iter != m_Party.end(); iter++) {
		for (DWORD i = 0; i < iter->copyParty.Count; i++) {
			if (player == iter->copyParty.member[i].player) {
				unlock();
				return true;
			}
		}
	}
	unlock();
	return false;
}

bool CFFA::addParty(Pshaiya50_player player) {

	Pshaiya_party party = player->party;
	PPARTYKILLRECORD pParty = getsignedParty(party);

	//������Ѿ�ע��Ķ���
	if (pParty) {
		if (pParty->deadCount >= m_maxDeadCount) {
			sendNoticeToplayer(player, "�������Ĵ���̫����,��Ҫ�ٽ���!");
			return false;
		}
		else {
			return true;
		}
	}

	
	//
	if (isExistedMember(player)) {
		sendNoticeToplayer(player, "��ǰ�治���Ѿ����������㲻����������!");
		return false;
	}




	Pshaiya50_player leader = GetLeader(party);

	if (m_Party.size() >= m_maxPartys) {
		sendNoticeToplayer(player, "�õ�ͼ���鼺��!!");
		return false;
	}

	if (party->Count > 7) {
		return false;
	}


	BYTE Defensecount = 0;
	for (DWORD i = 0; i < party->Count; i++) {
		Pshaiya50_player player = (Pshaiya50_player)party->member[i].player;
		if (player) {

			if (player->Level < m_minlevel) {
				sendNoticeToplayer(player, "�ж�Ա�����ŵȼ�Ҫ��!");
				return false;
			}

			if ( (player->family==3&&player->job==1)||
				(player->family==0&&player->job==1)) {
				Defensecount++;
			}
		}
	}

	if (Defensecount > 2) {
		sendNoticeToplayer(player, "���ܳ���2������������ʿ!");
		return false;
	}


	PARTYKILLRECORD record;
	ZeroMemory(&record, sizeof(record));
	record.pParty = party;
	memcpy(&(record.copyParty), party, sizeof(shaiya_party));
	strcpy(record.strLeaderName, player->charname);

	lock();
	m_Party.push_back(record);
	unlock();
	return true;
}


void CFFA::addKills(Pshaiya50_player player) {

	if (player->party&&m_kills)
	{
		lock();
		for (auto iter = m_Party.begin(); iter != m_Party.end(); iter++) {
			if (iter->pParty == player->party) {
				EP4CALL::addKills(player,m_kills);
				break;
			}
		}
		unlock();
	}
}

void CFFA::killLog(Pshaiya50_player player,  Pshaiya50_player deather) {

	if (player->party)
	{
		lock();
		for (auto iter = m_Party.begin(); iter != m_Party.end(); iter++) {
			if (iter->pParty == player->party) {
				PLAYERMESSSAGE st;
				sprintf(st.strMessage, "%s,���ڶ��鵱ǰɱ����:%d", iter->strLeaderName, ++iter->kills);
				send_packet_specific_area(player, &st, sizeof(st));
				break;
			}
		}

		if (deather->party) {
			for (auto iter = m_Party.begin(); iter != m_Party.end(); iter++) {
				//ƥ�����
				if (iter->pParty == deather->party) {

					//�Ƿ񳬳���������
					if (++iter->deadCount >= m_maxDeadCount) {

						//�ٻ���ȥ
						for (DWORD i = 0; i < deather->party->Count; i++) {
							Pshaiya50_player player = Pshaiya50_player(deather->party->member[i].player);
							if (player&&player->Map==m_Map) {
								MovePlayer((DWORD)player, 42, 50, 50);
							}	
						}
					}
					break;
				}
			}
		}
		unlock();
	}



}

void CFFA::lock() {
	EnterCriticalSection(&m_cs);
}
void CFFA::unlock() {
	LeaveCriticalSection(&m_cs);
}

bool CFFA::isFFAMap(WORD Map)
{
	return Map == m_Map&&Map ? true : false;
}

bool CFFA::ispartyAble(Pshaiya50_player srcPlayer, Pshaiya50_player dstPlayer) {
	bool bRet = true;
	if (isFFAMap(srcPlayer->Map)) {
		bRet = false;
		Pshaiya_party party = dstPlayer->party;
		if (party) {
			for (auto iter = m_Party.begin(); iter != m_Party.end(); iter++) {
				if (iter->pParty == party) {
					for (DWORD i = 0; i < iter->copyParty.Count; i++) {
						if (srcPlayer == iter->copyParty.member[i].player) {
							bRet = true;
							break;
						}
					}
					break;
				}
			}
		}
	}
	return bRet;
}





void CFFA::outPut() {

	lock();
	for (auto iter = m_Party.begin(); iter != m_Party.end(); iter++) {
		char temp[MAX_PATH] = { 0 };
		sprintf(temp, "�����,<%s>���ڵĶ�����ͷ��:%d", iter->strLeaderName, iter->kills);
		sendNotice(temp);
		LOGD << temp;
	}
	m_Party.clear();
	unlock();
}

CFFA::CFFA(WCHAR* appname)
{
	InitializeCriticalSection(&m_cs);
	wcscpy(m_appName, appname);
	loadConfig();
}

CFFA::~CFFA()
{
	DeleteCriticalSection(&m_cs);
}


bool CFFA::isSameFaction(Pshaiya50_player Attacker, Pshaiya50_player Target) {
	if (!isAttackTime()) {
#ifdef _DEBUG
		sendNoticeToplayer(Attacker, "���ǹ���ʱ��!");
#endif
		return true;
	}

	if (!Target->party||!Attacker->party) {
		return true;
	}

	return Attacker->party == Target->party;
}


//AҪȡb����Ӫ
BYTE CFFA::getFaction(Pshaiya50_player playerA, Pshaiya50_player playerB) {
	if (playerA->party&&playerA->party == playerB->party) {
#ifdef _DEBUG
		LOGD << "return playerA->Country------>"<<playerA->charname << " wants to get " << playerB->charname << " country,and it is" << playerA->Country;
#endif
		return playerA->Country;

	}else {
#ifdef _DEBUG
		LOGD << "playerA->Country ? 0 : 1------>"<<playerA->charname << " wants to get " << playerB->charname << " country,and it is" << (playerA->Country ? 0 : 1);
#endif
		return playerA->Country ? 0 : 1;
	}
}