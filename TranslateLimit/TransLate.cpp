// Ep4VipColor.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "MyInlineHook.h"




DWORD g_dwReturnAddr = 0;
PVOID g_pfun = NULL;
DWORD g_dwMap = 0;
DWORD g_dwItemid = 0;
DWORD g_dwDeleteItemCall = 0x00465120;
DWORD g_dwFailedAddr = 0x00468ad2;

__declspec(naked) void Naked()
{
	_asm
	{
		pushad
			mov ecx,dword ptr [edi+0x20]
			push ecx        //map
			push ebp        //pPlayer
			call g_pfun
			cmp al, 0
			popad
			je _Wrong
			jmp _Right
		_Right :
		cmp dword ptr  [edi + 0x20], 0xC8
		jmp g_dwReturnAddr
		_Wrong :
		jmp g_dwFailedAddr
	}
}




bool __stdcall fun(PVOID pPlayer, DWORD dwMap)
{
	bool bRet = true;

	do
	{
		if (g_dwMap != dwMap) break;
		if (!g_dwItemid) break;
		if (!pPlayer) break;
		if (!*(PDWORD)pPlayer) break;

		bRet = false;

		BYTE byMaxBag = *(PBYTE((DWORD)pPlayer + 0x1304));//get max bags
		if (!byMaxBag)  break;                            //must greater than 0
		PDWORD pBagBase = PDWORD((DWORD)pPlayer + 0x1c4); //get bagbase at bag1

		for (DWORD i = 1; i <byMaxBag; i++)
		{
			for (DWORD j = 0; j < 24; j++)
			{
				DWORD pItem = pBagBase[i*24+j];
				if (pItem ==0) continue;             //if no item in this slot

				BYTE vbyType = *PBYTE(pItem +0x40);//get itemid
				BYTE vbyTypeid = *PBYTE(pItem +0x41);
				DWORD dwTempItemid = vbyType * 1000 + vbyTypeid;

				if (dwTempItemid == g_dwItemid)
				{
					_asm
					{
						push 0
						mov ebx,i
						push j
						mov ecx, pPlayer
						call g_dwDeleteItemCall
					}
					return TRUE;
				}
			}
		}
	} while (0);
	return bRet;
}



void StartTranslateLimit()
{

	CMyInlineHook obj1;
	g_dwReturnAddr =obj1.Hook((PVOID)0x00468ac9, Naked, 7);
	g_pfun = fun;
}


