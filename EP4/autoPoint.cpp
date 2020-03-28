#include "stdafx.h"
#include "DataBase.h"


namespace AutoPoint
{

	CDataBase g_objdb;
	DWORD g_dwExp[80] = { 0 };
	WORD g_wMap = 0;
	DWORD g_sleepTime = 0;
	DWORD g_points = 0;
	BYTE g_Requiredlevel = 0;



	void AddPoints(DWORD player, DWORD points)
	{
		DWORD uid = *PDWORD(player + 0x578c);
		if (uid) {
			WCHAR sql[MAX_PATH] = { 0 };;
			swprintf_s(sql, L"update users_master set point=point+%ld where useruid=%ld", points, uid);
			g_objdb.ExeSqlByCon(sql);
		}
	}



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
				if ((Map == g_wMap || g_wMap == 0) && level >= g_Requiredlevel)
				{
					AddPoints(player, g_points);
				}
			}

			Sleep(g_sleepTime);
		}
	}


	void Start()
	{
		g_objdb.LinkDataBase(L"ps_userdata");
		WCHAR* appName = L"autopoint";
	    g_wMap = GetPrivateProfileInt(appName, L"map", 0, g_szFilePath);
		g_sleepTime = GetPrivateProfileInt(appName, L"sleepTime", 0, g_szFilePath) * 1000;
		g_points = GetPrivateProfileInt(appName, L"points", 0, g_szFilePath);
		g_Requiredlevel = GetPrivateProfileInt(appName, L"level", 0, g_szFilePath);
		_beginthread(loop, 0, 0);
	}
}