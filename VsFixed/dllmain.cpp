// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
#include "stdafx.h"
#include <windows.h>
#include <MyInlineHook.h>
#include <vector>




//VSģʽ�²��ܽ�market
PVOID g_pFixVsModeReturnAddr = NULL;
PVOID g_pFixVsModeAddr1 = (PVOID)0x00479155;

PVOID g_pConfrimTradeReturnAddr = NULL;
PVOID g_pConfrimTradeCall = NULL;

//������ɫ
PVOID g_pConfrimCharNameReturnAddr = NULL;
PVOID g_pConfrimCharNameCall = NULL;


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
PVOID g_pSendPacketCall = (PVOID)0x004ED0E0;//���ͷ����,�ܶ�ط���Ҫ�õ�


__declspec(naked) void Naked_ConfrimCharName()
{
	_asm
	{
		pushad
			push edi    //PackBuffer
			push esi
			call g_pConfrimCharNameCall
			popad
			jmp g_pConfrimCharNameReturnAddr
	}
}
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
			push ecx
			push ebp
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

void __stdcall ConfrimTrade(DWORD dwPlayer, PBYTE pPacket)
{

		//1.�жϰ�ͷ
		WORD wCmd = *(PWORD)pPacket;
		if (wCmd != 0xa0a) return;  //a0a��ȷ�ϰ�ť�İ�

		//2.ȡ������ȡ������ȷ��
		BYTE byConfrim = pPacket[2];

		//3.��֯���:Ҫ���͸�����˫��
		BYTE bySelf[4] = { 0x0a, 0x0a, 0x01, 0x0 };
		BYTE byTarget[4] = { 0x0a, 0x0a, 0x02, 0x0 };

		bySelf[3] = byConfrim;
		byTarget[3] = byConfrim;

		DWORD dwSelf = (DWORD)bySelf;
		DWORD dwTarget = (DWORD)byTarget;

		//4.��ȡ�Է���ҵĻ�ַ
		DWORD dwTargetPlayer = *PDWORD(dwPlayer + 0x15c4);


		if (!dwTargetPlayer) return;
		if (!(*(PDWORD)dwTargetPlayer)) return;
		if (*PDWORD(dwTargetPlayer + 0x68) == 0xffffffff) return;
		if (*PDWORD(dwPlayer + 0x68) == 0xffffffff) return;

		//4.1���Լ�

		_asm
		{
			mov ecx, dwPlayer
				push 0x4
				push dwSelf
				call g_pSendPacketCall
		}
		//4.2���Է�
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

		//1.Ѱ�����ϵ���Ʒ
		for (DWORD i = 0; i < 24; i++)
		{
			//2.ȷ���Ƿ�
			if (pItemAddr[i] &&
				g_ayLegalItem[i].bAlow == FALSE)
			{
				//3.ת����Ʒ
				for (DWORD j = 1; j <= dwMaxBag; ++j)
				{
					for (DWORD k = 0; k < 24; ++k)
					{
						//4.����ǿ�λ
						if (!pItemAddr[j * 24 + k])
						{
							SwitchItem(dwPlayer, 0, i, j, k);
							//˵��û��ת�ɹ�
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



void __stdcall SendPacketToPlayer(DWORD dwPlayer, DWORD dwBuff, DWORD dwSize)
{
	PVOID g_pSendPacketCall = (PVOID)0x004ED0E0;//���ͷ����,�ܶ�ط���Ҫ�õ�
	_asm
	{
		mov ecx, dwPlayer
			push dwSize
			push dwBuff
			call g_pSendPacketCall
	}
}

/*
PVOID g_pFixShowStatsCall = NULL;
PVOID g_pFixShowStatsReturnAddr = NULL;
CMyInlineHook Hookobj_2;
void __stdcall ShowStatsFix(PBYTE pPlayer)
{
	//1.�����°�
	BYTE   byPacket[0x32] = { 0 };
	ZeroMemory(byPacket, sizeof(BYTE)*_countof(byPacket));
	*(PWORD)byPacket = 0x526;                             //������26 05
	PDWORD pPacket = (PDWORD)&byPacket[2];                //����������ĵط������޸�

	//2.������������ݸ��ƹ�ȥ
	PDWORD pTotal = PDWORD(pPlayer + 0x121c);//�����Դ�
	PWORD  pExtend = PWORD(pPlayer + 0x16c);  //�������Դ�



	//3.��������
	//3.1 ������
	pPacket[0] = *pTotal - *pExtend;
	pTotal++;
	pExtend++;

	//3.2 ��������
	pPacket[4] = *pTotal - *pExtend;
	pTotal++;
	pExtend++;

	//3.3 ��������
	pPacket[2] = *pTotal - *pExtend;
	pTotal++;
	pExtend++;

	//3.4 ���Ӿ���
	pPacket[3] = *pTotal - *pExtend;
	pTotal++;
	pExtend++;

	//3.5 ��������
	pPacket[1] = *pTotal - *pExtend;
	pTotal++;
	pExtend++;

	//3.6 ������
	pPacket[5] = *pTotal - *pExtend;
	pTotal++;
	pExtend++;


	DWORD dwPhAttackLow = *PDWORD(pPlayer + 0x13cc);                   //�﹥����:�ϵ���13cc
	DWORD dwPhAttackHig = dwPhAttackLow + *PDWORD(pPlayer + 0x12e4);   //�﹥����
	pPacket[6] = dwPhAttackLow;
	pPacket[7] = dwPhAttackHig;

	DWORD dwMagicAttackLow = *PDWORD(pPlayer + 0x1424);                   //ħ������
	DWORD dwMagicAttackHig = dwMagicAttackLow + *PDWORD(pPlayer + 0x12e4);//ħ������
	pPacket[8] = dwMagicAttackLow;
	pPacket[9] = dwMagicAttackHig;

	pPacket[10] = *PDWORD(pPlayer + 0x13d4);                   //����
	pPacket[11] = *PDWORD(pPlayer + 0x142c);                   //�ֿ�

	//4. ���ͷ��--��ʾ����
	SendPacketToPlayer((DWORD)pPlayer, (DWORD)byPacket, 0x32);

}

__declspec(naked) void Naked_ShowStatsFix()
{
	_asm
	{
		mov[esi + 0x1434], ecx //orginal code
			pushad
			push esi             //pPlayer
			call g_pFixShowStatsCall
			popad
			jmp g_pFixShowStatsReturnAddr
	}
}

*/



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{



// 		g_pFixShowStatsReturnAddr = (PVOID)Hookobj_2.Hook((PVOID)0x00460FFF, Naked_ShowStatsFix, 6);
// 		g_pFixShowStatsCall = ShowStatsFix;
	}

	break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

