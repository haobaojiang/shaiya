#include "stdafx.h"
#include "autoExp.h"



namespace AutoExp
{
	DWORD g_dwExp[81] = { 0 };
	WORD g_wMap = 0;
	DWORD g_sleepTime = 0;



	void loop(void* p)
	{
		while (1)
		{

			for (DWORD i = 0; i < _countof(g_players); i++)
			{
				DWORD player = g_players[i];
				if (!player) {
					continue;
				}
				if (!*PDWORD(player + 0xe0)) {
					continue;
				}

				WORD  Map = *PWORD(player + 0x164);
				BYTE level = *PBYTE(player + 0x136);
				if (Map == g_wMap || g_wMap == 0)
				{
					BYTE level = *PBYTE(player + 0x136);
					DWORD Exp = g_dwExp[level ];
					if (Exp)
						AddExp(player, Exp);
				}
			}

			Sleep(g_sleepTime);
		}
	}



	void Start()
	{
		WCHAR* appName = L"autoExp";
		g_wMap      = GetPrivateProfileInt(appName, L"map", 0, g_szFilePath);
		g_sleepTime = GetPrivateProfileInt(appName, L"sleepTime", 0, g_szFilePath) * 1000;


		for (auto i = 0; i < _countof(g_dwExp); ++i) {
			WCHAR Key[50] = { 0 };
			swprintf_s(Key, L"level_%d", i);
			g_dwExp[i] = GetPrivateProfileInt(appName, Key, 0, g_szFilePath);
		}
		_beginthread(loop, 0, 0);
	}
}