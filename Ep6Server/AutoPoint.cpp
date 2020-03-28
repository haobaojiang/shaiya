#include "stdafx.h"
#include "AutoPoint.h"








namespace AutoPoint
{
	WORD g_Map = 0;
	DWORD g_point = 0;
	DWORD g_sleepTime = 0;


	DWORD nextTime = 0;

	bool IsTimeAble() {

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

			if(!player->dwObjMap)
				continue;

	
			if (g_Map == 0 || g_Map == player->Map)
			{
				WCHAR sql[MAX_PATH] = { 0 };
				swprintf(sql, L"update ps_userdata.dbo.users_master set point=point+%ld where useruid=%d", g_point, player->uid);
				g_db.ExeSqlByCon(sql);
			}

		}
	}


	void start()
	{
		g_Map = GetPrivateProfileInt(L"autoPoint", L"map", 0, g_szFilePath);
		g_point = GetPrivateProfileInt(L"autoPoint", L"point", 0, g_szFilePath);
		g_sleepTime = GetPrivateProfileInt(L"autoPoint", L"sleepTime", 0, g_szFilePath);
		if (!g_sleepTime) {
			g_sleepTime = 60;
		}
		nextTime = GetTickCount();
		g_vecLoopFunction.push_back(Loop);
	}
}





