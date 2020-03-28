#include "stdafx.h"
#include "AutoExp.h"

namespace AutoKills
{
	DWORD g_beAddedkills = 0;
	WORD  g_wMap = 0;
	DWORD g_sleepTime = 0;

	DWORD nextTime = 0;

	bool IsTimeAble() {
		static DWORD nextTime = GetTickCount();
		DWORD curTime = GetTickCount();

		if (curTime < nextTime) {
			return false;
		}
		nextTime += (g_sleepTime * 1000);
		return true;
	}


	void Loop()
	{

		if (!IsTimeAble()) {
			return;
		}

		for (DWORD i = 0; i < _countof(g_players); i++) {

			Pshaiya_player player = g_players[i];
			if (!player)
				break;

			if (!player->dwObjMap)
				continue;

			if (g_wMap == 0 ||g_wMap == player->Map){
				AddKill(player, g_beAddedkills);
			}
		}
	}


	void start()
	{
		g_beAddedkills = GetPrivateProfileInt(L"AutoKills", L"kills", 0, g_szFilePath);
		g_wMap = GetPrivateProfileInt(L"AutoKills", L"map", 0, g_szFilePath);
		g_sleepTime = GetPrivateProfileInt(L"AutoKills", L"sleepTime", 0, g_szFilePath);
		if (!g_sleepTime) {
			g_sleepTime = 60;
		}
		nextTime = GetTickCount();
		g_vecLoopFunction.push_back(Loop);
	}
}





