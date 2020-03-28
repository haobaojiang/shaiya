#include "stdafx.h"
#include "firePort.h"







#define _SHAIYA_YING 1

#ifdef _SHAIYA_YING
vector<PARTYKILLRECORD> g_vecYingParty;
CRITICAL_SECTION g_csParty;
#endif

namespace firePort
{

	DWORD g_item = 0;
	WORD  g_map = 0;
	bool  g_partyNeeded = 0;
	bool  g_isOpenedTime[7][24][60] = { 0 };
	CMyInlineHook g_objFirePoart;





	bool __stdcall fireportCheck(Pshaiya_player player, WORD Map, Pshaiya_pos pos)
	{
		bool bRet = false;
		do
		{
			//comapre map
			if (Map != g_map) {
				bRet = true;
				break;
			}

			//check is with a party
			if (g_partyNeeded && !player->party)
				break;

			//check time
			SYSTEMTIME stTime;
			GetLocalTime(&stTime);
			if (!g_isOpenedTime[stTime.wDayOfWeek][stTime.wHour][stTime.wMinute])
				break;

			//影神泣的特殊处理
#ifdef _SHAIYA_YING
// 			if (player->party->Count < 2)
// 				break;


			if (GetLeader(player->party) != player) {
				break;
			}


			Pshaiya_party party = player->party;

			//存进表里
			EnterCriticalSection(&g_csParty);

			bool IsFind = false;
			for (auto iter = g_vecYingParty.begin(); iter != g_vecYingParty.end(); iter++) {
				if (iter->pParty == party) {
					IsFind = true;
					break;
				}
			}
			if (!IsFind) {
				PARTYKILLRECORD record;
				record.pParty = party;
				record.kills = 0;
				memcpy(&(record.copyParty), party, sizeof(shaiya_party));
				strcpy(record.strLeaderName, player->charname);
				g_vecYingParty.push_back(record);
			}


			LeaveCriticalSection(&g_csParty);

			//把队内所有成员移动到动图里
			for (DWORD i = 0; i < party->Count; i++) {
				Pshaiya_player pTarget = Pshaiya_player(party->member[i].player);
				if (pTarget&&
					pTarget != player&&
					pTarget->Map != Map) {
					MovePlayer((DWORD)pTarget, Map, pos->x, pos->z);
				}
			}
			bRet = true;
			break;
#else

			//delete requiredItem
			if (g_item && !DeleteItemByItemID(player, g_item))
				break;

			bRet = true;
#endif
		} while (0);
		return bRet;
	}



	PVOID  g_pFailedAddr = (PVOID)0x0047679A;
	__declspec(naked) void Naked()
	{
		_asm
		{
			pushad
			add edi, 0x24
			MYASMCALL_3(fireportCheck, ebp, ecx, edi)
			cmp al, 0x0
			popad
			je _wrong

			cmp ecx, 0xc8
			jmp g_objFirePoart.m_pRet
			_wrong :
			jmp g_pFailedAddr
		}
	}






	void start()
	{

#ifdef _SHAIYA_YING
		InitializeCriticalSection(&g_csParty);
#endif

		WCHAR* appname = L"fireport";
		WCHAR szTime[MAX_PATH] = { 0 };

		g_item = GetPrivateProfileInt(appname, L"item", 0, g_szFilePath);
		g_map = GetPrivateProfileInt(appname, L"map", 0, g_szFilePath);
		g_partyNeeded = GetPrivateProfileInt(appname, L"partyRequire", 0, g_szFilePath);
		GetPrivateProfileString(appname, L"time", L"", szTime, MAX_PATH, g_szFilePath);

		memset(g_isOpenedTime, 1, sizeof(g_isOpenedTime));

		if (szTime[0] != L'0') {
			ZeroMemory(g_isOpenedTime, sizeof(g_isOpenedTime));


			WCHAR *delim = L")";
			WCHAR* p = wcstok(szTime, delim);
			while (p) {
				DWORD dayofWeek;
				DWORD hour;
				DWORD duration;
				swscanf(p, L"(%d,%d,%d", &dayofWeek, &hour, &duration);
				if (dayofWeek >= 7 || hour >= 24 || !duration)
					continue;

				memset(g_isOpenedTime[dayofWeek][hour], 1, 60 * duration);
				p = wcstok(NULL, delim);
			}
		}
		g_objFirePoart.Hook((PVOID)g_hookAddr.fireport, Naked, 6);
	}





}