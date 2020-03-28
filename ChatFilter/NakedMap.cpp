#include "stdafx.h"
#include "NakedMap.h"


namespace NakedMap {

	legalItem g_ayLegalItem[24] = { 0 };
	PVOID g_pNakeMapReturnAddr = NULL;
	PVOID g_fnNakeMap = NULL;
	DWORD g_dwNakedMap = 0;

	PVOID g_pSlotLimitReturnAddr = NULL;
	PVOID g_fnSlotLimit = NULL;





	PVOID g_pNakedMapFalseAddr = (PVOID)0x00469c37;
	__declspec(naked) void Naked_NakedMap()
	{
		_asm
		{
			pushad
			mov ecx, dword ptr[edi + 0x20]
			push ecx
			push ebp
			call g_fnNakeMap
			cmp al, 0x1
			popad
			fstsw ax
			je __successed
			jmp g_pNakedMapFalseAddr
			__successed :
			test ah, 0x5
				jmp g_pNakeMapReturnAddr
		}
	}


	bool SwitchItem(DWORD dwPlayer, DWORD dwSrcBag, DWORD dwSrcSlot, DWORD dwDstBag, DWORD dwDstSlot)
	{
		PVOID pSwitchItemSlotCall = (PVOID)0x0045b690;
		bool bRet = false;
		_asm
		{
			push dwDstSlot
			push dwDstBag
			push dwSrcSlot
			push dwSrcBag
			push dwPlayer
			call pSwitchItemSlotCall
			mov bRet, al
		}
	}




	bool __stdcall NakedMap(DWORD dwPlayer, DWORD dwMap)
	{
		do
		{
			if (dwMap != g_dwNakedMap)
				break;

			if (!dwPlayer)
				break;

			if (!*PDWORD(dwPlayer))
				break;



			PDWORD pItemAddr = PDWORD(dwPlayer + 0x1c4); //ep4 的偏移是1c4
			DWORD dwMaxBag = *PBYTE(dwPlayer + 0x1304);//ep4 的偏移

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
			movzx ebx, byte ptr[ebx + 0x41]  //typeid
			push ebx
			push edx
			push eax                         //slot
			push edi                         //player
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

	BOOL __stdcall SlotLImit(DWORD dwPlayer, DWORD dwSlot, DWORD dwType, DWORD dwTypeid)
	{
		BOOL bRet = TRUE;
		do
		{
			if (*PBYTE(dwPlayer + 0x164) == g_dwNakedMap)
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

	void StartNakedMap()
	{

		CMyInlineHook Hookobj_6;
		g_pNakeMapReturnAddr = (PVOID)Hookobj_6.Hook((PVOID)0x00468abe, Naked_NakedMap, 5);
		g_fnNakeMap = NakedMap;

		CMyInlineHook HookObj_7;
		g_pSlotLimitReturnAddr = (PVOID)HookObj_7.Hook((PVOID)0x0045b450, Naked_SlotLimit, 7);
		g_fnSlotLimit = SlotLImit;
	}

}