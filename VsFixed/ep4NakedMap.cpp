// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <MyInlineHook.h>
#include <vector>


//VS模式下不能建market
PVOID g_pFixVsModeReturnAddr = NULL;
PVOID g_pFixVsModeAddr1 = (PVOID)0x00479155;

PVOID g_pConfrimTradeReturnAddr = NULL;
PVOID g_pConfrimTradeCall = NULL;


typedef struct _legalItem
{
	DWORD dwItemid;
	BOOL  bAlow;
}legalItem, PlegalItem;
legalItem g_ayLegalItem[24] = { 0 };
PVOID g_pNakeMapReturnAddr = NULL;
PVOID g_fnNakeMap = NULL;
DWORD g_dwNakedMap = 0;


PVOID g_pSlotLimitReturnAddr = NULL;
PVOID g_fnSlotLimit = NULL;
PVOID g_pSendPacketCall = (PVOID)0x004ED0E0;//发送封包的,很多地方需要用到

__declspec(naked) void Naked_FixVsMode()
{
	_asm
	{
		cmp dword ptr[edi + 0x558c], 0x0
			jne __exit

			movzx eax, byte ptr[ebp + 0x2]
			mov esi, edi
			jmp g_pFixVsModeReturnAddr

		__exit :
		jmp g_pFixVsModeAddr1

	}
}

__declspec(naked) void Naked_ConfrimTrade()
{
	_asm
	{
		pushad
			push edi    //PackBuffer
			push ecx
			call g_pConfrimTradeCall
			popad
			fstsw ax
			test ah, 0x5
			jmp g_pConfrimTradeReturnAddr
	}
}


PVOID g_pNakedMapFalseAddr = (PVOID)0x00477999;
__declspec(naked) void Naked_NakedMap()
{
	_asm
	{
		pushad
			mov ecx,dword ptr [edi+0x20]
			push ebp
			push ecx
			call g_fnNakeMap
			cmp al,0x1
			popad
			fstsw ax
			je __successed
			jmp g_pNakedMapFalseAddr	
		__successed :
			test ah, 0x5
			jmp g_pNakeMapReturnAddr
	}
}



void __stdcall ConfrimTrade(DWORD dwPlayer, PCONFRIMTRADE packet)
{

	if (packet->cmd != official_packet_const::confrim_trade) {
		return;
	}

		//2.取内容是取消还是确认
		BYTE byConfrim = pPacket[2];

		//3.组织封包:要发送给交易双方
		BYTE bySelf[4] = { 0x0a, 0x0a, 0x01, 0x0 };
		BYTE byTarget[4] = { 0x0a, 0x0a, 0x02, 0x0 };

		bySelf[3] = byConfrim;
		byTarget[3] = byConfrim;

		DWORD dwSelf = (DWORD)bySelf;
		DWORD dwTarget = (DWORD)byTarget;

		//4.获取对方玩家的基址
		DWORD dwTargetPlayer = *PDWORD(dwPlayer + 0x15c4);


		if (!dwTargetPlayer) return;
		if (!(*(PDWORD)dwTargetPlayer)) return;
		if (*PDWORD(dwTargetPlayer + 0x68) == 0xffffffff) return;
		if (*PDWORD(dwPlayer + 0x68) == 0xffffffff) return;

		//4.1发自己

		_asm
		{
			mov ecx, dwPlayer
				push 0x4
				push dwSelf
				call g_pSendPacketCall
		}
		//4.2发对方
		_asm
		{
			mov ecx, dwTargetPlayer
				push 0x4
				push dwTarget
				call g_pSendPacketCall
		}
}

bool SwitchItem (DWORD dwPlayer,DWORD dwSrcBag,DWORD dwSrcSlot, DWORD dwDstBag, DWORD dwDstSlot)
{
	PVOID pSwitchItemSlotCall = (PVOID)0x004685a0;
	bool bRet = false;
	_asm
	{
		push dwDstSlot
			push dwDstBag
			push dwSrcSlot            
			push dwSrcBag
			mov ecx, dwPlayer
			call pSwitchItemSlotCall
			mov bRet,al
	}
}

BOOL __stdcall SlotLImit(DWORD dwPlayer,DWORD dwSlot, DWORD dwType,DWORD dwTypeid)
{
	BOOL bRet = TRUE;
	do
	{
		if (*PBYTE(dwPlayer + 0x160) == g_dwNakedMap)
		{
			if (g_ayLegalItem[dwSlot].bAlow == FALSE)
				return FALSE;
			
			DWORD dwItemid = dwType * 1000 + dwTypeid;
			if (g_ayLegalItem[dwSlot].dwItemid != 0 &&
				g_ayLegalItem[dwSlot].dwItemid != dwItemid)
				bRet = FALSE;
		}

		if (dwSlot == 0xe && dwType != 120)
			bRet = FALSE;

		if (dwSlot == 0xf && dwType != 150)
			bRet = FALSE;

		if (dwSlot == 0x10 && dwType != 121)
			bRet = FALSE;

	} while (0);

	return bRet;
}


bool __stdcall NakedMap(DWORD dwPlayer,DWORD dwMap)
{
	do 
	{
		if (dwMap != g_dwNakedMap)
			break;

		if (!dwPlayer)
			break;

		if (!*PDWORD(dwPlayer))
			break;

		

		PDWORD pItemAddr = PDWORD(dwPlayer + 0x1c0);
		DWORD dwMaxBag = *PBYTE(dwPlayer + 0x130c);

		//1.寻找身上的物品
		for (DWORD i = 0; i < 24; i++)
		{
			//2.确定非法
			if (pItemAddr[i] &&
				g_ayLegalItem[i].bAlow == FALSE)
			{
				//3.转移物品
				for (DWORD j = 1; j <= dwMaxBag; ++j)
				{
					for (DWORD k = 0; k < 24; ++k)
					{
						//4.如果是空位
						if (!pItemAddr[j * 24 + k])
						{
							SwitchItem(dwPlayer, 0, i, j, k);
							//说明没有转成功
							if (pItemAddr[i])
							{
								return false;
							}
							goto __exit;
						}
					}
				}
			__exit:
				;

			}
		}
	} while (0);

	return true;
}


__declspec(naked) void Naked_SlotLimit()
{
	_asm
	{
		pushad
			movzx edx, byte ptr[ebx + 0x40] //type
			movzx ebx, byte ptr [ebx+0x41]  //typeid
			push ebx
			push edx
			push eax
			push edi
			call g_fnSlotLimit
			cmp al, 0x0
			je __failed
			popad
			mov cx, word ptr[edi + 0x136]
			jmp g_pSlotLimitReturnAddr
		__failed :
		popad
			xor eax, eax
			ret

	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
// 		LoadLibraryA("ShareKills.dll");
// 		WCHAR szTemp[MAX_PATH] = { 0 };
// 		WCHAR szFilePath[MAX_PATH] = { 0 };
// 		GetModuleFileName(GetModuleHandle(0), szTemp, MAX_PATH);
// 		PathRemoveFileSpec(szTemp);
// 		swprintf_s(szFilePath, MAX_PATH, L"%s\\config.ini", szTemp);
// 	
// 		g_dwNakedMap = GetPrivateProfileInt(L"Naked", L"map", 0, szFilePath);
// 
// 		for (DWORD i = 0; i < 24; ++i)
// 		{
// 			WCHAR szKey[MAX_PATH] = {0};                  
// 			WCHAR   szReturn[MAX_PATH] = { 0 };
// 			swprintf_s(szKey, MAX_PATH,L"%d", i);
// 			if (GetPrivateProfileString(L"Naked", szKey, L"", szReturn, MAX_PATH, szFilePath) <= 0)
// 				continue;
// 
// 			//切割出字符串出来
// 			CString szTemp[2];
// 			for (DWORD j = 0; j < _countof(szTemp); j++)
// 			{
// 				AfxExtractSubString(szTemp[j], szReturn, j, L',');
// 			}
// 			g_ayLegalItem[i].dwItemid = _tcstoul(szTemp[0], 0, 10);
// 			g_ayLegalItem[i].bAlow    = _tcstoul(szTemp[1], 0, 10);
// 		}


		CMyInlineHook HookObj_22;
		g_pFixVsModeReturnAddr = (PVOID)HookObj_22.Hook((PVOID)0x00478932, Naked_FixVsMode, 6);

		CMyInlineHook Hookobj_5;
		g_pConfrimTradeReturnAddr = (PVOID)Hookobj_5.Hook((PVOID)0x0047e0d1, Naked_ConfrimTrade, 9);
		g_pConfrimTradeCall = ConfrimTrade;

// 		CMyInlineHook Hookobj_6;
// 		g_pNakeMapReturnAddr = (PVOID)Hookobj_6.Hook((PVOID)0x00476784, Naked_NakedMap, 5);
// 		g_fnNakeMap = NakedMap;


// 		CMyInlineHook HookObj_7;
// 		g_pSlotLimitReturnAddr= (PVOID)HookObj_7.Hook((PVOID)0x00468370, Naked_SlotLimit, 7);
// 		g_fnSlotLimit = SlotLImit;
	}

	break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

