#include "stdafx.h"
#include "autoSummon.h"




#define _SHAIYA_YING 1
#ifdef _SHAIYA_YING
extern	vector<PARTYKILLRECORD> g_vecYingParty;
extern	CRITICAL_SECTION g_csParty;
#endif


namespace autoSummon
{


	WORD g_srcMap = 0;
	//pos1
	WORD g_DstMap1 = 0;
	shaiya_pos g_dstPos1 = { 0 };
	//pos2
	WORD g_DstMap2 = 0;
	shaiya_pos g_dstPos2 = { 0 };




	bool  g_IsTime[7][24][60] = { 0 };


	void summon()
	{
		SYSTEMTIME stcTime;
		GetLocalTime(&stcTime);
		if (g_IsTime[stcTime.wDayOfWeek][stcTime.wHour][stcTime.wMinute]&&g_srcMap)
		{
			for (DWORD i = 0; i < _countof(g_players); i++)
			{
				Pshaiya_player player = g_players[i];
				if (!player)
					break;

				if (!player->dwObjMap)
					continue;


				if (player->Map == g_srcMap)
				{
					if (player->Country == 0) {
						MovePlayer((DWORD)player, g_DstMap1, g_dstPos1.x, g_dstPos1.z);
					}
					else if (player->Country == 1) {
						MovePlayer((DWORD)player, g_DstMap2, g_dstPos2.x, g_dstPos2.z);
					}
				}

			}


#ifdef _SHAIYA_YING
			EnterCriticalSection(&g_csParty);
			for (auto iter = g_vecYingParty.begin(); iter != g_vecYingParty.end(); iter++) {
				MyLog("%s,kills:%d\n", iter->strLeaderName, iter->kills);
			}
			g_vecYingParty.clear();
			LeaveCriticalSection(&g_csParty);
#endif
			Sleep(60 * 1000);
		}
	}








	void start()
	{
		WCHAR szPos1[MAX_PATH] = { 0 };
		WCHAR szPos2[MAX_PATH] = { 0 };
		WCHAR szTime[MAX_PATH] = { 0 };
		WCHAR* appname = L"autoSummon";
		GetPrivateProfileString(appname, L"dstPos1", L"", szPos1, MAX_PATH, g_szFilePath);
		GetPrivateProfileString(appname, L"dstPos2", L"", szPos2, MAX_PATH, g_szFilePath);
		GetPrivateProfileString(appname, L"time", L"", szTime, MAX_PATH, g_szFilePath);

		if (szPos1[0] != L'\0') {
			swscanf(szPos1, L"%hd,%f,%f", &g_DstMap1, &g_dstPos1.x, &g_dstPos1.z);
		}
		if (szPos2[0] != L'\0') {
			swscanf(szPos2, L"%hd,%f,%f", &g_DstMap2, &g_dstPos2.x, &g_dstPos2.z);
		}

		ZeroMemory(g_IsTime, sizeof(g_IsTime));


		if (szTime[0] != L'0') {
			WCHAR *delim = L")";
			WCHAR* p = wcstok(szTime, delim);
			while (p) {
				DWORD dayofWeek;
				DWORD hour;
				DWORD miniute;
				swscanf(p, L"(%d,%d,%d", &dayofWeek, &hour, &miniute);
				if (dayofWeek >= 7 || hour >= 24 || miniute>=60)
					continue;

				g_IsTime[dayofWeek][hour][miniute] = true;
				p = wcstok(NULL, delim);
			}
		}


		g_srcMap = GetPrivateProfileInt(appname, L"srcMap", 0, g_szFilePath);
		g_vecLoopFunction.push_back(summon);



	}
}





