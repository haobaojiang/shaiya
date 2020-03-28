#include "stdafx.h"
#include "AutoExp.h"



 

namespace AutoExp
{

	DWORD g_dwExp[255] = { 0 };
	WORD g_wMap = 0;
	DWORD g_sleepTime = 0;

	DWORD nextTime = 0;
	bool IsTimeAble(){
	
		DWORD curTime = GetTickCount();

		if (curTime < nextTime) {
			return false;
		}
		nextTime += (g_sleepTime * 1000);
		return true;
	}



	void Loop()
	{
		do 
		{
			if (!IsTimeAble()) {
				break;
			}

			for (DWORD i = 0; i < _countof(g_players); i++) {

				Pshaiya_player player = g_players[i];

				if (!player)
					break;

				if (!player->dwObjMap)
					continue;


				if (g_wMap == 0 || g_wMap == player->Map)
				{
					DWORD Exp = g_dwExp[player->Level - 1];
					if (Exp) {
						addExpFromUser(player, Exp);
					}

				}
			}

		} while (0);


	}


	void start()
	{
		g_wMap = GetPrivateProfileInt(L"AutoExp", L"map", 0, g_szFilePath);
		g_sleepTime = GetPrivateProfileInt(L"AutoExp", L"sleepTime", 0, g_szFilePath);
		if (!g_sleepTime) {
			g_sleepTime = 60;
		}
		nextTime = GetTickCount();

		for (DWORD i = 0; i < _countof(g_dwExp); ++i) {

			WCHAR Key[20] = { 0 };
			swprintf_s(Key, L"level_%d", i +1);

			g_dwExp[i] = GetPrivateProfileInt(L"AutoExp", Key, 0, g_szFilePath);
		}
		g_vecLoopFunction.push_back(Loop);
	}
}





