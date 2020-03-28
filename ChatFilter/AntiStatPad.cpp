#include "stdafx.h"
#include "AntiStatPad.h"
#include <map>




namespace AntiStatPad
{


	//ɱ���ӳ�
	
	DWORD g_Killdelay = 0;


	//�ж�ˢս����
	CMyInlineHook g_objfightBack;
	DWORD g_Delay = 0;
	float g_deadRange = 0.0;
	DWORD g_deadRow = 0;
	DWORD g_punishmentTime = 0;
	bool  g_fightBack = false;

	//�ȼ���
	WORD g_levelDifference = 0;


	//ָ�����û��ս��
	std::map<DWORD, DWORD>  g_mapBannedPlayerUids;
	std::map<DWORD, DWORD>  g_mapBannedPlayerCharids;
	CRITICAL_SECTION g_mapCs;



	void resetstatPad(Pshaiya50_player player) {
		player->statpad.deadRow = 0;
		player->statpad.hasFightBack = false;
		player->statpad.nextTime = GetTickCount() + g_Delay;
		player->statpad.punishTime = 0;
	}



	void enablePunishment(Pshaiya50_player player) {
		resetstatPad(player);
		LOGD << player->charname << "�ܵ��˳ͷ�,���ɵõ�ս��!\n";
		player->statpad.punishTime = GetTickCount() + g_punishmentTime;
	}


	bool antistatpad_IsgiveKills(Pshaiya50_player deather, Pshaiya50_player killer) {

		WORD levelLeft = abs(deather->Level-killer->Level);

		if (levelLeft > g_levelDifference) {
			return false;
		}




		//��Ҫͬ�ȼ�
// 		if (deather->Level != killer->Level) {
// 			return false;
// 		}

#ifdef _DEBUG
		char strTemp[MAX_PATH] = { 0 };
#endif

		//ɱ�˸���ͷ�����ӳٵ�
		if (!isLegalTime(&deather->nextKillTime, g_Killdelay)) {
#ifdef _DEBUG
			sprintf(strTemp, "%s ��ɱ��̫����,û����ͷ!", deather->charname);
			sendNotice(strTemp);
#endif
			return false;
		}



		//�ͷ���ʱ��
		if (deather->statpad.punishTime&&deather->statpad.punishTime > GetTickCount()) {
#ifdef _DEBUG	
			sprintf(strTemp, "%s Ŀǰ�ڳͷ��׶���!û����ͷ!", deather->charname);
			sendNotice(strTemp);
#endif
			return false;
		}




		// ��¼��һ�ε���������
		float x = deather->pos.x - deather->statpad.lastDeadPos.x;
		float z = deather->pos.z - deather->statpad.lastDeadPos.z;
		float fleft = sqrt(x*x + z*z);
		memcpy(&deather->statpad.lastDeadPos, &deather->pos, sizeof(deather->statpad.lastDeadPos)); //������һ������

		// ����� ���з���/��������/��ⳬʱ
		if (g_fightBack&&deather->statpad.hasFightBack) {

#ifdef _DEBUG	
			sprintf(strTemp, "%s�з���,����һ���������%d��,�����÷�ˢս����¼!", deather->charname, DWORD(fleft));
			sendNotice(strTemp);
#endif
			resetstatPad(deather);
			return true;
		}
			
			

		if (fleft > g_deadRange) {

#ifdef _DEBUG	
			sprintf(strTemp, "%s->������ˢս����Χ!,����һ���������%d��,�����÷�ˢս����¼!", deather->charname, DWORD(fleft));
			sendNotice(strTemp);
#endif
			resetstatPad(deather);
			return true;
		}
				
		if(isLegalTime(&deather->statpad.nextTime, g_Delay)) {

#ifdef _DEBUG	
			sprintf(strTemp, "%s �����˼������ ,����һ���������%d��,�����÷�ˢս����¼!", deather->charname, DWORD(fleft));
			sendNotice(strTemp);
#endif
			resetstatPad(deather);
			return true;
		}


		if (++deather->statpad.deadRow == g_deadRow) {

#ifdef _DEBUG
			sprintf(strTemp, "%s ������ɱ%d ��,�ܵ��ͷ�!", deather->charname, deather->statpad.deadRow);
			sendNotice(strTemp);
#endif
			enablePunishment(deather);
			return false;
		}

#ifdef _DEBUG
		sprintf(strTemp, "%s ĿǰΪֹ������ɱɱ���� %d��!", deather->charname, deather->statpad.deadRow);
		sendNotice(strTemp);
#endif
		return true;


	}

	


	bool isBannedUser(Pshaiya50_player player) {

		bool bRet = false;
		EnterCriticalSection(&g_mapCs);


		//uid����Ƿ񱻷�
		{
			auto iter = g_mapBannedPlayerUids.find(player->uid);
			if (iter != g_mapBannedPlayerUids.end()) {
				DWORD currentTime = GetTickCount();
				if (iter->second > currentTime) {
					bRet = true;
				}
				else {
					g_mapBannedPlayerUids.erase(iter);
				}
			}
		}
		//charid����Ƿ񱻷�
		{
			auto iter = g_mapBannedPlayerCharids.find(player->Charid);
			if (iter != g_mapBannedPlayerCharids.end()) {
				DWORD currentTime = GetTickCount();
				if (iter->second > currentTime) {
					bRet = true;
				}
				else {
					g_mapBannedPlayerCharids.erase(iter);
				}
			}
		}

		LeaveCriticalSection(&g_mapCs);
		return bRet;
	}


	void enableKill(Pshaiya50_player deather, Pshaiya50_player killer, bool* pRet) {

		if (g_ffp1&&g_ffp1->isFFAMap(deather->Map)) {
			*pRet = true;
			return;
		}

		//�Ƿ�ָ�������UID
		if (isBannedUser(deather)) {
#ifdef _DEBUG
			sendNoticeToplayer(killer, "��Ϊdeather�Ǳ������,���Բ��ɻ�ȡս��!");
#endif
			*pRet = false;
			return;
		}

		//ԭ�еķ�ˢս��
		if (!antistatpad_IsgiveKills(deather, killer)) {
			*pRet = false;
			return;
		}
	}





	void __stdcall fun_fightBack(Pshaiya50_player player)
	{
		player->statpad.hasFightBack = true;
	}

	__declspec(naked) void  Naked_fightBack()
	{
		_asm {
			pushad
			MYASMCALL_1(fun_fightBack, edi)
			popad
			mov eax, 0x7530
			jmp g_objfightBack.m_pRet
		}
	}









	void addBanuser(const char* playerChat) {

		char cmd[MAX_PATH] = { 0 };
		DWORD id = 0;
		DWORD bantime = 0;

		sscanf(playerChat, "%s %d %d", cmd, &id, &bantime);


		if (stricmp(cmd, "enablekillbanuid") == 0 && id&&bantime) {
			DWORD releaseTime = GetTickCount() + bantime * 60 * 1000;
			EnterCriticalSection(&g_mapCs);
			g_mapBannedPlayerUids.insert(std::pair<DWORD, DWORD>(id, releaseTime));
			LeaveCriticalSection(&g_mapCs);
			LOGD << "killban--->uid:" << id;
		}

		if (stricmp(cmd, "releasekillbanuid") == 0 && id) {

			EnterCriticalSection(&g_mapCs);
			auto iter = g_mapBannedPlayerUids.find(id);
			if (iter != g_mapBannedPlayerUids.end()) {
				g_mapBannedPlayerUids.erase(iter);
				LOGD << "releasekillban--->uid:" << id;
			}
			LeaveCriticalSection(&g_mapCs);
		}


		if (stricmp(cmd, "enablekillbancid") == 0 && id&&bantime) {
			DWORD releaseTime = GetTickCount() + bantime * 60 * 1000;
			EnterCriticalSection(&g_mapCs);
			g_mapBannedPlayerCharids.insert(std::pair<DWORD, DWORD>(id, releaseTime));
			LeaveCriticalSection(&g_mapCs);
			LOGD << "killban--->uid:" << id;
		}

		if (stricmp(cmd, "releasekillbancid") == 0 && id) {

			EnterCriticalSection(&g_mapCs);
			auto iter = g_mapBannedPlayerCharids.find(id);
			if (iter != g_mapBannedPlayerCharids.end()) {
				g_mapBannedPlayerCharids.erase(iter);
				LOGD << "releasekillban--->uid:" << id;
			}
			LeaveCriticalSection(&g_mapCs);
		}


	}


	bool isTime() {
		static DWORD nextTime = GetTickCount() + 15 *1000;

		DWORD curTime = GetTickCount();

		if (nextTime > curTime) {
			return false;
		}
		nextTime = curTime + 15 * 1000;;
		return true;
	}


	void loop() {

		if (!isTime()) {
			return;
		}


		// anti statpad
		{
			WCHAR* appName = L"AntiStatPad";
			g_Killdelay = GetPrivateProfileInt(appName, L"killDelay", 0, g_szFilePath) * 1000;
			g_Delay = GetPrivateProfileInt(appName, L"antipadDelay", 0, g_szFilePath) * 1000;
			g_deadRange = float(GetPrivateProfileInt(appName, L"range", 0, g_szFilePath));
			g_deadRow = GetPrivateProfileInt(appName, L"deadrow", 0, g_szFilePath);
			g_punishmentTime = GetPrivateProfileInt(appName, L"punishmentTime", 0, g_szFilePath) * 1000;
			g_fightBack = GetPrivateProfileInt(appName, L"fightback", 0, g_szFilePath);
			g_levelDifference= GetPrivateProfileInt(appName, L"levelDifference", 0, g_szFilePath);
		}




	}


	void Start()
	{
		InitializeCriticalSection(&g_mapCs);


		g_objfightBack.Hook(PVOID(0x004579f7), Naked_fightBack, 5);
		g_enablekillsCallBack.push_back(enableKill);
		g_vecLoopFunction.push_back(loop);
		g_consoleCallBack.push_back(addBanuser);
	}
}





