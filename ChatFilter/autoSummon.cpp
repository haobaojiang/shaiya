#include "stdafx.h"
#include "autoSummon.h"




namespace autoSummon
{
	void summon()
	{
		SYSTEMTIME stcTime;
		GetLocalTime(&stcTime);

		//summon
		for (auto iter = g_vecSummontasks.begin(); iter != g_vecSummontasks.end(); iter++) {
			if (stcTime.wHour == iter->hour&&stcTime.wDayOfWeek == iter->dayOfWeek&&iter->minute == stcTime.wMinute&&iter->lastDay!=stcTime.wDay) {
				for (DWORD i = 0; i < _countof(g_players); i++) {
					Pshaiya50_player player = (Pshaiya50_player)g_players[i];
					if (!player)
						break;

					if (!player->dwObjMap) {
						continue;
					}
					WORD  Map = player->Map;
		
					if (Map == iter->srcMap ) {
						LOGD << "autoSummon:" << player->charname << ",Dstmap:" << iter->dstMap;
						MovePlayer((DWORD)player, iter->dstMap, iter->dstX, iter->dstZ);
					}
				}
				
				if (!iter->isLoop) {
					g_vecSummontasks.erase(iter);
					break;
				}
				iter->lastDay = stcTime.wDay;
			}
		}

		//notice
		for (auto iter = g_vecNoticeTasks.begin(); iter != g_vecNoticeTasks.end(); iter++) {
			if (stcTime.wHour == iter->hour&&
				stcTime.wDayOfWeek == iter->dayOfWeek&&
				iter->minute == stcTime.wMinute&&
				iter->lastDay!=stcTime.wDay) {
				sendNotice(iter->strNotice);
				LOGD << "notice sent:" << iter->strNotice;

				if (!iter->isLoop) {
					g_vecNoticeTasks.erase(iter);
					break;
				}
				iter->lastDay = stcTime.wDay;
				
			}
		}

	}







	void Start()
	{
		g_vecLoopFunction.push_back(summon);
	}
}