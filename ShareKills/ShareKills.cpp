// ShareKills.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "MyInlineHook.h"
#include <windows.h>

#include <vector>
using std::vector;
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

CRITICAL_SECTION g_csRaids;
CRITICAL_SECTION g_csKillers;



//#define _EP4

BOOL g_bEnable = FALSE;

PVOID g_StoreAddrOfRaidCall = NULL;
PVOID g_StoreAddrOfRaidReturnAddr = NULL;

#ifdef _EP4
PVOID g_pOrginalCall = (PVOID)0x004458F0;
#else
PVOID g_pOrginalCall = (PVOID)0x0044ec70;
#endif


vector<PVOID> g_vecpRaid;

PVOID g_pDisMissCall = NULL;
PVOID g_pDisMissReturnAddr = NULL;



PVOID g_ShareKillsReturnAddr = NULL;
PVOID g_ShareKillsCall = NULL;



typedef struct _KILLER
{
	PVOID pKiller;
	PVOID pDeather;
}KILLER,PKILLER;
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



#ifdef _EP4
__declspec(naked) void Naked_DisMissRaid()
{
	_asm
	{
		//check if no more ppl in raid
		cmp dword ptr[ebp + 0x10], 0x1
			jg __orginal
			pushad
			push ebp
			call g_pDisMissCall
			popad

		__orginal :
		cmp dword ptr[ebp + 0x10], ebx
			mov byte ptr[esp + 0x13], 0
			jmp g_pDisMissReturnAddr
	}
}
#else
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
		mov byte ptr [esp + 0xF], 0x0
			jmp g_pDisMissReturnAddr
	}
}

#endif

#ifdef _EP4
PVOID g_ShareKillsTempCall = (PVOID)0x0045B050;
#else
PVOID g_ShareKillsTempCall = (PVOID)0x00467EE0;
#endif

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
#ifdef _EP4
	PVOID pEnableApplyRangeCall = (PVOID)0x0045DE60;
#else
	PVOID pEnableApplyRangeCall = (PVOID)0x0046ad40;
#endif

	BOOL bResult = 0;

#ifdef _EP4
	_asm
	{
		push 0x64           //range
		mov eax, dwDeather 
		mov ecx, dwKiller
		call pEnableApplyRangeCall
		movzx eax,al
		mov bResult,eax
	}
	return bResult;
#else
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
#endif
}
void AddKill(DWORD dwKiller, DWORD dwDeather)
{
#ifdef _EP4
	PVOID pTempCall = (PVOID)0x0045AED0;
	_asm
	{
		mov ecx, dwKiller  //杀方
		mov edx, dwDeather //被杀方
		call pTempCall
	}
#else
	PVOID pTempCall = (PVOID)0x00467ce0;
	_asm
	{
		mov ecx, dwKiller  //杀方
			push dwDeather //被杀方
			call pTempCall
	}
#endif
}

void __stdcall  GetAddressOfRaid(PVOID pRaid)
{
	EnterCriticalSection(&g_csRaids);
	g_vecpRaid.push_back(pRaid);
	LeaveCriticalSection(&g_csRaids);
}

#ifdef _EP4
void __stdcall  ShareKills(DWORD dwKiller, DWORD dwDeather)
{
	if (!dwKiller || !dwDeather)
		return;
	if (!*(PDWORD)dwKiller || !*(PDWORD)dwDeather)
		return;
	if (!*PDWORD(dwKiller + 0x173c))
		return;

	if (*PBYTE(*PDWORD(dwKiller + 0x173c)+0x114)!=0x1)  //party or raid
		return;


	EnterCriticalSection(&g_csRaids);
	//enum each raid
	for (vector<PVOID>::iterator iter = g_vecpRaid.begin(); iter != g_vecpRaid.end();iter++)
	{
		//enum each player of raid
		DWORD dwRaid = (DWORD)*iter;

		//pass own raid
		if (*PDWORD(dwKiller + 0x173c) == dwRaid)
			continue;

		DWORD dwNumber = *PDWORD(dwRaid + 0x10); //get totol number
		if (!dwNumber)
			continue;

		PDWORD pPlayer = PDWORD(dwRaid + 0x138); //get entry of player 菲律宾那家伙的是138特殊点

		for (DWORD i = 0; i < dwNumber; i++,pPlayer+=2)  
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
			if (byOwnCountry==2||byTarteCountry==2)
				break;

			if (byOwnCountry ==byTarteCountry)
				break;

			if (!EnableApplyRange(dwPlayer, dwDeather)) //range
				continue;

			AddKill(dwPlayer,dwDeather);
		}
	}
	LeaveCriticalSection(&g_csRaids);
}
#else
void __stdcall  ShareKills(DWORD dwKiller, DWORD dwDeather)
{
	if (!g_bEnable)
	{
		return;
	}

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
#endif


void __stdcall  DisMissRaid(PVOID pRaid)
{
	EnterCriticalSection(&g_csRaids);
	for (vector<PVOID>::iterator iter = g_vecpRaid.begin(); iter != g_vecpRaid.end();iter++)
	{
		if(*iter==pRaid)
		{
			g_vecpRaid.erase(iter);
			break;
		}
	}
	LeaveCriticalSection(&g_csRaids);
}



DWORD WINAPI ThreadProc1(_In_  LPVOID lpParameter)
{
	InitializeCriticalSection(&g_csRaids);
	InitializeCriticalSection(&g_csKillers);

	//1.Get Address Of Raid When It Created
	CMyInlineHook obj1;
#ifdef _EP4
	g_StoreAddrOfRaidReturnAddr=(PVOID)obj1.Hook((PVOID)0x00467c84, Naked_GetAddressOfRaid, 5);
	g_StoreAddrOfRaidCall = GetAddressOfRaid;
#else
	g_StoreAddrOfRaidReturnAddr = (PVOID)obj1.Hook((PVOID)0x004758b8, Naked_GetAddressOfRaid, 5);
	g_StoreAddrOfRaidCall = GetAddressOfRaid;
#endif


	//2.Delete The Address of Raid When It Dismiss
	CMyInlineHook obj2;
#ifdef _EP4
	g_pDisMissReturnAddr=(PVOID)obj2.Hook((PVOID)0x00446545, Naked_DisMissRaid, 8);
	g_pDisMissCall = DisMissRaid;
#else
	g_pDisMissReturnAddr = (PVOID)obj2.Hook((PVOID)0x0044f659, Naked_DisMissRaid, 8);
	g_pDisMissCall = DisMissRaid;
#endif

	//3.share kills for each raid
	CMyInlineHook obj3;
#ifdef _EP4
	g_ShareKillsReturnAddr = (PVOID)obj2.Hook((PVOID)0x00458c78, Naked_ShareKills, 5);
	g_ShareKillsCall = ShareKills;
#else
	g_ShareKillsReturnAddr = (PVOID)obj2.Hook((PVOID)0x00465aad, Naked_ShareKills, 5);
	g_ShareKillsCall = ShareKills;
#endif


	LoadLibraryA("Trade.dll");
	LoadLibraryA("VsFixed.dll");
	LoadLibraryA("ChatFilter.dll");
	//设置路径
	WCHAR szFilePath[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(0), szFilePath, MAX_PATH);
	PathRemoveFileSpec(szFilePath);
	wcscat_s(szFilePath, MAX_PATH, L"\\config.ini");
	while (1)
	{
		Sleep(3000);
		g_bEnable = GetPrivateProfileInt(L"ShareKills", L"enable", 0, szFilePath);
	}



	return 0;
}