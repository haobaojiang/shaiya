// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <afxwin.h>
#include "stdafx.h"
#include <MyClass.h>
#include "MyInlineHook.h"

//pvp drops
BOOL g_bEnablePvpDrops = FALSE;
PVOID g_pPvpDropsReturnAddr = NULL;
PVOID g_pPvpDropsCall = NULL;

typedef struct _DropDetail
{
	DWORD dwGrade;
	DWORD dwDropRate;
}DropDetail, *PDropDetail;

typedef struct _PVPDROPS
{
	DropDetail Detail[9];
}PVPDROPS, *PPVPDROPS;

PVPDROPS g_AyPvpDrops[200] = { 0 };

DWORD __stdcall GetItemAddress(DWORD dwItemid)
{
	BYTE byType = dwItemid / 1000;
	BYTE byTypeid = dwItemid % 1000;
	return  (byType * 0x100 + byTypeid) * 0xa8 + 0x00589674; //计算物品地址
}

DWORD GetRandItem(DWORD dwGrade)
{
	PVOID pCall = (PVOID)0x004D5E90;
	DWORD dwResult = 0;
	_asm
	{
		mov eax, dwGrade
			mov ecx, 0x01091878
			call pCall
			mov dwResult, eax
	}
	return dwResult;
}
bool SendItemToPlayer(DWORD dwPlayer, PVOID pItemAddr, DWORD dwCount = 1)
{
	PVOID pSendItem = (PVOID)0x0046BD10;
	bool bResult = FALSE;
	_asm
	{
		push dwCount
			push pItemAddr
			mov ecx, dwPlayer
			call pSendItem
			mov bResult, al
	}
	return bResult;
}


void __stdcall PvpDrops(DWORD dwPlayer)
{
	do
	{
		if (!dwPlayer)
			break;

		BYTE byMap = *PBYTE(dwPlayer + 0x160);     //Map
		DWORD dwParty = *PDWORD(dwPlayer + 0x17f4);//Party
		DWORD dwTarget = dwPlayer;



// 		if (!byMap)
// 			break;

		PDropDetail stcTemp = g_AyPvpDrops[byMap].Detail;

		for (DWORD i = 0; i < 9; i++)
		{
			if (!stcTemp[i].dwGrade ||
				!stcTemp[i].dwDropRate)
				continue;

			//机率
			if (!IsRandSuccessful(stcTemp[i].dwDropRate, 100))
				continue;

			//获取物品地址
			DWORD dwItemAddr = GetRandItem(stcTemp[i].dwGrade);

			//如果组队
			if (dwParty)
			{

				DWORD dwPlayerCount = *PDWORD(dwParty + 0x10);//找出队伍里的最大人数
				DWORD dwRand = GetRand(dwPlayerCount);
				dwTarget = *PDWORD(dwParty + 0x18 + dwRand * 0x8);  //因为一个人是占了8个字节
				if (!dwTarget)
					dwTarget = dwPlayer;
			}
			SendItemToPlayer(dwTarget, (PVOID)dwItemAddr);
		}
	} while (0);
}


__declspec(naked) void Naked_PvpDrops()
{
	_asm
	{
		mov dword ptr [esp + 0x1C], ebx
			pushad
			push esi            //pPlayer
			call g_pPvpDropsCall
			popad
			cmp ebx, eax
			jmp g_pPvpDropsReturnAddr

	}
}


DWORD WINAPI ThreadProc1(_In_  LPVOID lpParameter)
{
	//1.先修改一处地方改为nop
	BYTE byTemp[6] = { 0x90 };
	memset(byTemp, 0x90, _countof(byTemp)*sizeof(BYTE));
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00468636, byTemp, _countof(byTemp)*sizeof(BYTE));

	//2.加载config
	WCHAR szTemp[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(0), szTemp, MAX_PATH);
	PathRemoveFileSpec(szTemp);
	CString szFilePath = szTemp;
	szFilePath = szFilePath + L"\\config.ini";
	g_bEnablePvpDrops = GetPrivateProfileInt(L"PvpDrops", L"Enable", 0, szFilePath);
	if (g_bEnablePvpDrops)
	{
		for (DWORD i = 0; i < _countof(g_AyPvpDrops); i++)
		{
			CString szAppName;
			szAppName.Format(L"map%d", i);
			for (DWORD j = 0; j < 9; j++)
			{
				WCHAR   szReturn[MAX_PATH] = { 0 };
				CString szKeyName;
				CString szTemp[2];
				szKeyName.Format(L"%d", j + 1);
				if (GetPrivateProfileString(szAppName, szKeyName, L"", szReturn, MAX_PATH, szFilePath) <= 0) break;

				for (DWORD k = 0; k < _countof(szTemp); k++)
					AfxExtractSubString(szTemp[k], szReturn, k, L',');

				g_AyPvpDrops[i].Detail[j].dwGrade = _tcstoul(szTemp[0], 0, 10);
				g_AyPvpDrops[i].Detail[j].dwDropRate = _tcstoul(szTemp[1], 0, 10);
			}

		}
	}



	//3.激活功能

	if (g_bEnablePvpDrops)
	{
		CMyInlineHook HookObj_28((PVOID)0x004656FF, Naked_PvpDrops, (PDWORD)&g_pPvpDropsReturnAddr, 6);
		g_pPvpDropsCall = PvpDrops;
	}

	return 0;
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		::CreateThread(0, 0, ThreadProc1, 0, 0, 0);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

