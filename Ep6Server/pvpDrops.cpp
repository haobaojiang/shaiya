#include "stdafx.h"
#include "pvpDrops.h"




namespace PvpDrops
{
	CMyInlineHook g_obj;

	WORD g_map = 0;
	DWORD g_Grade[9];
	DWORD g_DropRate[9];

	bool g_isTime[7][24][60] = { 0 };



	void __stdcall fun(Pshaiya_player player)
	{
#ifndef _DEBUG
		VMProtectBegin("PvpDrops fun");
#endif 
		if (player->Map != g_map)
			return;

		SYSTEMTIME stTime;
		GetLocalTime(&stTime);
		if (!g_isTime[stTime.wDayOfWeek][stTime.wHour][stTime.wMinute])
			return;

		for (DWORD i = 0; i < 9; i++)
		{
			if (!g_Grade[i] || !g_Grade[i])
				continue;

			//机率
			if (!IsRandSuccessful(g_DropRate[i], 100))
				continue;

			//获取物品地址
			PITEMINFO pItem = GetRandItem(g_Grade[i]);
			if(!pItem)
				continue;

			//如果组队
			if (player->party) {
				DWORD Index = GetRand(player->party->Count);
				Pshaiya_player pTarget = Pshaiya_player(player->party->member[Index].player);
				if (pTarget) {
					sendItem(pTarget, pItem);
					continue;
				}
			}
			sendItem(player, pItem);
		}
#ifndef _DEBUG
		VMProtectEnd();
#endif
	}

	__declspec(naked) void Naked_PvpDrops()
	{
		_asm
		{	
			pushad
			MYASMCALL_1(fun, esi)
			popad
			mov dword ptr[esp + 0x1C], ebx
			cmp ebx, eax
			jmp g_obj.m_pRet

		}
	}


	void Start()
	{
// #ifndef _DEBUG
// 		VMProtectBegin("PvpDrops Start");
// #endif 

		g_obj.Hook((PVOID)0x004656FF, Naked_PvpDrops, 6);


		//时间
		WCHAR szTime[MAX_PATH] = { 0 };
		GetPrivateProfileString(L"pvpdrop", L"time", L"", szTime, MAX_PATH, g_szFilePath);
		WCHAR *delim = L")";
		WCHAR* p = wcstok(szTime, delim);
		while (p) {
			DWORD openTime = 0;
			DWORD  hours = 0;
			DWORD dayofWeek = 0;
			swscanf(p, L"(%d,%d,%d", &dayofWeek, &openTime, &hours);

			LOGD << "pvpdrop config reading:" << dayofWeek<<','<< openTime <<','<< hours;

			memset(g_isTime[dayofWeek][openTime], 1, 60 * hours);
			p = wcstok(NULL, delim);
		}



		//地图
		g_map = GetPrivateProfileInt(L"pvpdrop", L"map", 0, g_szFilePath);

		LOGD << "pvpdrop config reading map :" << g_map<<"\n";

		//掉落
		for (DWORD i = 0; i < 9; i++) {
			WCHAR szTemp[MAX_PATH] = { 0 };
			WCHAR Key[50] = { 0 };
			swprintf(Key, L"%d", i + 1);
			GetPrivateProfileString(L"pvpdrop", Key, L"", szTemp, MAX_PATH, g_szFilePath);
			swscanf(szTemp, L"%d,%d", &g_Grade[i], &g_DropRate[i]);

			LOGD << "pvpdrop config ,grade :" << g_Grade[i]<<"droprate:"<< g_DropRate[i]<< "\n";

 		}
// #ifndef _DEBUG
// 		VMProtectEnd();
// #endif
	}
}