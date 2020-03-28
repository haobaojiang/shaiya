#include "stdafx.h"
#include "autoKills.h"




namespace AutoKills
{
	DWORD g_beAddedkills = 0;
	WORD  g_wMap = 0;
	DWORD g_sleepTime = 0;




	void loop(void* p)
	{
		while (1)
		{

			for (DWORD i = 0; i < _countof(g_players); i++)
			{
				Pshaiya50_player player = Pshaiya50_player(g_players[i]);
				if (IsIllagePlayer(player)) {
					continue;
				}

				WORD  Map = player->Map;
				if (Map == g_wMap || g_wMap == 0)
				{
					/////////////这是vs服的特殊
// 					if (Map == 42)
// 					{
// 						addKills(player, 3);
// 						continue;
// 					}
					////////
					addKills(player, g_beAddedkills);
				}
			}

			Sleep(g_sleepTime);
		}
	}



	void Start()
	{
		WCHAR* appName = L"AutoKills";
		g_wMap = GetPrivateProfileInt(appName, L"map", 0, g_szFilePath);
		g_beAddedkills = GetPrivateProfileInt(appName, L"kills", 0, g_szFilePath);
		g_sleepTime = GetPrivateProfileInt(appName, L"sleepTime", 60, g_szFilePath) * 1000;

		_beginthread(loop, 0, 0);
	}
}