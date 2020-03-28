
#include "stdafx.h"
#include "ffp.h"







namespace FFP
{
	void addSummontask(WORD Map, BYTE dayOfweek, BYTE hour, BYTE minute=0, WORD dstMap=42, float x=50, float z=50, bool isLoop=false);

	void addSummontask(WORD Map,BYTE dayOfweek,BYTE hour,BYTE minute,WORD dstMap,float x,float z,bool isLoop) {
		SUMMONTASK task;
		task.dayOfWeek = dayOfweek;
		task.dstMap = dstMap;
		task.dstX = x;
		task.dstZ = z;
		task.hour = hour;
		task.minute = minute;
		task.srcMap = Map;
		task.isLoop = isLoop;
		LOGD << "taskasummon added:" << task.dayOfWeek << ',' << task.hour << ':' << task.minute;
		g_vecSummontasks.push_back(task);
	}



	void loop() {
		SYSTEMTIME stcTime;
		GetLocalTime(&stcTime);

		//ffp
		if (g_ffp1) {
			for (auto iter = g_ffp1->m_vectasks.begin(); iter != g_ffp1->m_vectasks.end(); iter++) {

				//地图开始
				if (stcTime.wHour == iter->hour&&stcTime.wDayOfWeek == iter->dayOfweek&&iter->minute == stcTime.wMinute&&iter->beginDay != stcTime.wDay) {
					sendNotice("《血战到底》活动已经开启!请各队长带领大家进入战斗");
					LOGD << "《血战到底》活动已经开启!请各队长带领大家进入战斗";
					addSummontask(g_ffp1->m_Map, iter->dayOfweek, iter->hour + iter->duration,1);  //先把召唤出地图的任务放进队列
					iter->beginDay = stcTime.wDay;
				}

				//地图结束
				if (stcTime.wHour == (iter->hour+iter->duration)&&stcTime.wDayOfWeek == iter->dayOfweek&&iter->minute == stcTime.wMinute&&iter->endDay != stcTime.wDay) {
					g_ffp1->outPut();
					iter->endDay = stcTime.wDay;
				}
			}
		}
	}


	//打架的
	void __stdcall isSameFaction(Pshaiya50_player Attacker, Pshaiya50_player Target, bool* pRet)
	{
		if (g_ffp1&&g_ffp1->isFFAMap(Attacker->Map)) {
			*pRet = g_ffp1->isSameFaction(Attacker, Target);
		}
	}


	void __stdcall addKills(Pshaiya50_player killer, Pshaiya50_player deather)
	{
		if (g_ffp1&&g_ffp1->isFFAMap(killer->Map)) {
			g_ffp1->addKills(killer);
		}
	}


	void __stdcall killLog(Pshaiya50_player killer, Pshaiya50_player deather)
	{
		if (g_ffp1&&g_ffp1->isFFAMap(killer->Map)) {
			g_ffp1->killLog(killer,deather);
		}
	}


	//从b里面拿阵营
	void __stdcall GetFaction(Pshaiya50_player playerA, Pshaiya50_player playerB,BYTE* pCountry)
	{
		if (g_ffp1&&g_ffp1->isFFAMap(playerB->Map)) {
			*pCountry = g_ffp1->getFaction(playerA,playerB);
		}
	}


	void __stdcall IsPartyAble(Pshaiya50_player srcPlayer, Pshaiya50_player dstPlayer,bool* pRet)
	{
		if (g_ffp1->isFFAMap(srcPlayer->Map)) {
			*pRet = g_ffp1->ispartyAble(srcPlayer, dstPlayer);
		}
	}

	void __stdcall fireport(Pshaiya50_player player, WORD Map, Pshaiya_pos pos, bool* pRet)
	{
		if (g_ffp1&&g_ffp1->isFFAMap(Map)) {
			*pRet = g_ffp1->process(player,Map,pos);
		}
	}


	void moveAble(PVOID p1,PVOID p2,bool* pRet) {
		Pshaiya50_player playerA = Pshaiya50_player(p1);
		Pshaiya50_player playerB = Pshaiya50_player(p2);

		if (g_ffp1&&g_ffp1->isFFAMap(playerA->Map)) {
			*pRet = false;
		}

		if (g_ffp1&&g_ffp1->isFFAMap(playerB->Map)) {
			*pRet = false;
		}
	}


	void isCommonSkillUsable(void* p1, void* p2, bool* pRet) {
		Pshaiya50_player player = (Pshaiya50_player)p1;
		Pshaiya50_player target = (Pshaiya50_player)p2;

		if (g_ffp1&&g_ffp1->isFFAMap(player->Map)&&player->party!=target->party) {
			*pRet = false;
		}


	}


	void Start() {

		if (GetPrivateProfileInt(L"ffp1", L"enable", 0, g_szFilePath) == 1) {
			g_ffp1 = new CFFA(L"ffp1");
			g_factionCallBack.push_back(isSameFaction);
			g_killedCallBack.push_back(killLog);
			g_getfactionCallBack.push_back(GetFaction);
			g_partyAbleCallBack.push_back(IsPartyAble);
			g_fireportCallBack.push_back(fireport);
			g_addkillsCallBack.push_back(addKills);
			g_vecLoopFunction.push_back(loop);
			g_moveCallBack.push_back(moveAble);
			g_commonSkillUsableCallBack.push_back(isCommonSkillUsable);
		}


	}
}





