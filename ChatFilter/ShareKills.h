#pragma once
#ifndef SHAIYA_SHARE_KILLS_HEADER
#define SHAIYA_SHARE_KILLS_HEADER



#include "stdafx.h"
#include "config.hpp"
using std::vector;
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
#include "ShareKills.h"


CRITICAL_SECTION g_csRaids;
CRITICAL_SECTION g_csKillers;


PVOID g_StoreAddrOfRaidCall = NULL;
PVOID g_StoreAddrOfRaidReturnAddr = NULL;


PVOID g_pOrginalCall = (PVOID)0x0044ec70;
vector<PVOID> g_vecpRaid;

PVOID g_pDisMissCall = NULL;
PVOID g_pDisMissReturnAddr = NULL;



PVOID g_ShareKillsReturnAddr = NULL;
PVOID g_ShareKillsCall = NULL;
typedef struct _KILLER
{
	PVOID pKiller;
	PVOID pDeather;
}KILLER, PKILLER;
vector<KILLER> g_vecpKiller;



__declspec(naked) void Naked_GetAddressOfRaid()
{
	_asm
	{
		pushad
		push esi
		call g_StoreAddrOfRaidCall
		popad
		call g_pOrginalCall                    //orginal
		jmp g_StoreAddrOfRaidReturnAddr
	}
}




__declspec(naked) void Naked_DisMissRaid()
{
	_asm
	{
		//check if no more ppl in raid
		cmp dword ptr[ebx + 0x10], 0x1
		jg __orginal
		pushad
		push ebp
		call g_pDisMissCall
		popad

		__orginal :
		cmp dword ptr[ebx + 0x10], edi
			mov byte ptr[esp + 0xF], 0x0
			jmp g_pDisMissReturnAddr
	}
}



PVOID g_ShareKillsTempCall = (PVOID)0x00467EE0;
__declspec(naked) void Naked_ShareKills()
{
	_asm
	{
		pushad
		push edi         //deather
		push ecx         //killer
		call g_ShareKillsCall
		popad
		call g_ShareKillsTempCall
		jmp g_ShareKillsReturnAddr
	}
}



BOOL EnableApplyRange(DWORD dwKiller, DWORD dwDeather)
{
	PVOID pEnableApplyRangeCall = (PVOID)0x0046ad40;
	BOOL bResult = 0;
	_asm
	{
		push 0x64           //range
		mov eax, dwKiller
		mov ecx, dwDeather
		call pEnableApplyRangeCall
		movzx eax, al
		mov bResult, eax
	}
	return bResult;
}


void AddKill(DWORD dwKiller, DWORD dwDeather)
{
	PVOID pTempCall = (PVOID)0x00467ce0;
	_asm
	{
		mov ecx, dwKiller  //杀方
		push dwDeather //被杀方
		call pTempCall
	}

}

void __stdcall  GetAddressOfRaid(PVOID pRaid)
{
	EnterCriticalSection(&g_csRaids);
	const auto item = std::find(g_vecpRaid.begin(), g_vecpRaid.end(), pRaid);
	if (item == g_vecpRaid.end())
	{
		g_vecpRaid.push_back(pRaid);
	}
	LeaveCriticalSection(&g_csRaids);
}


void __stdcall  ShareKills(DWORD dwKiller, DWORD dwDeather)
{


	if (!dwKiller || !dwDeather)
		return;
	if (!*(PDWORD)dwKiller || !*(PDWORD)dwDeather)
		return;
	if (!*PDWORD(dwKiller + 0x17f4))
		return;

	if (*PBYTE(*PDWORD(dwKiller + 0x17f4) + 0x114) != 0x1)  //party or raid
		return;



	EnterCriticalSection(&g_csRaids);
	//enum each raid
	for (vector<PVOID>::iterator iter = g_vecpRaid.begin(); iter != g_vecpRaid.end(); iter++)
	{
		//enum each player of raid
		DWORD dwRaid = (DWORD)*iter;

		//pass own raid
		if (*PDWORD(dwKiller + 0x17f4) == dwRaid)
			continue;

		DWORD dwNumber = *PDWORD(dwRaid + 0x10); //get totol number
		if (!dwNumber)
			continue;

		PDWORD pPlayer = PDWORD(dwRaid + 0x18); //get entry of player

		for (DWORD i = 0; i < dwNumber; i++, pPlayer += 2)
		{
			DWORD dwPlayer = *pPlayer;
			if (!dwPlayer)
				continue;

			//临时设置的,一般不会有这种情况
			if (dwPlayer == dwDeather)
				continue;

			if (!*PDWORD(dwPlayer + 0x128))               //offset of charid
				continue;

			BYTE byOwnCountry = *PBYTE(dwPlayer + 0x12d);
			BYTE byTarteCountry = *PBYTE(dwDeather + 0x12d);
			if (byOwnCountry == 2 || byTarteCountry == 2)
				break;

			if (byOwnCountry == byTarteCountry)
				break;

			if (!EnableApplyRange(dwPlayer, dwDeather)) //range
				continue;

			AddKill(dwPlayer, dwDeather);
		}
	}
	LeaveCriticalSection(&g_csRaids);
}


void __stdcall  DisMissRaid(PVOID pRaid)
{
	EnterCriticalSection(&g_csRaids);
	for (vector<PVOID>::iterator iter = g_vecpRaid.begin(); iter != g_vecpRaid.end();iter++)
	{
		if (*iter == pRaid)
		{
			g_vecpRaid.erase(iter);
			break;
		}
	}
	LeaveCriticalSection(&g_csRaids);
}

void StartShareKillEx(void* p)
{
	Sleep(10 * 1000);
	auto section = "ShareKills";
	if (!GameConfiguration::GetBoolean(section, "enable", false)) {
		return;
	}
	LOGD << "share kills enabled";

	InitializeCriticalSection(&g_csRaids);
	InitializeCriticalSection(&g_csKillers);
	//1.Get Address Of Raid When It Created
	ShaiyaUtility::CMyInlineHook obj1;
	g_StoreAddrOfRaidReturnAddr = (PVOID)obj1.Hook((PVOID)0x004758b8, Naked_GetAddressOfRaid, 5);
	g_StoreAddrOfRaidCall = GetAddressOfRaid;

	//2.Delete The Address of Raid When It Dismiss
	ShaiyaUtility::CMyInlineHook obj2;
	g_pDisMissReturnAddr = (PVOID)obj2.Hook((PVOID)0x0044f659, Naked_DisMissRaid, 8);
	g_pDisMissCall = DisMissRaid;

	//3.share kills for each raid
	ShaiyaUtility::CMyInlineHook obj3;
	g_ShareKillsReturnAddr = (PVOID)obj2.Hook((PVOID)0x00465aad, Naked_ShareKills, 5);
	g_ShareKillsCall = ShareKills;
}

void StartShareKill()
{
	_beginthread(StartShareKillEx, 0, 0);
}



#endif
