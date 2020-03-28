
#include "stdafx.h"
#include "purchaseDelay.h"




namespace purchaseDelay
{

	CMyInlineHook g_objNpcPurchase;
	CMyInlineHook g_objitemMall;

	DWORD refuseNpcAddr = 0x004799D2;
	DWORD refuseItemMallAddr = 0x00488D5F;

#define DELAYTIME (1000)
	bool WINAPI IsBuyAble(Pshaiya_player player)
	{
		
// #ifndef _DEBUG
// 		VMProtectBegin("purchaseDelay IsBuyAble");
// #endif 

		bool bRet = true;
		DWORD CurrentTime = GetTickCount();
		DWORD nextMallTime = player->nextMalltime;

		do
		{
			if (!nextMallTime)
				break;

			if (nextMallTime > CurrentTime)
			{
				bRet = false;
				DWORD leftTime = nextMallTime - CurrentTime;
				if (leftTime > DELAYTIME) {
					bRet = true;          //ÌØÊâÇé¿ö
					break;
				}
			}

		} while (0);

		if (bRet) {
			player->nextMalltime = CurrentTime + DELAYTIME;
		}
// #ifndef _DEBUG
// 		VMProtectEnd();
// #endif
		return bRet;
	}



	__declspec(naked) void  Naked_itemMall()
	{
		_asm {
			pushad
			MYASMCALL_1(IsBuyAble, edi)
			cmp al, 0x1
			popad

			je _org
			jmp refuseItemMallAddr

			_org :
			mov eax, 0x2602
				jmp g_objitemMall.m_pRet
		}
	}


	__declspec(naked) void  Naked_NpcPurchase()
	{
		_asm {
			pushad
			MYASMCALL_1(IsBuyAble, edi)
			cmp al, 0x1
			popad
			je _org
			jmp refuseNpcAddr

			_org :
			mov eax, dword ptr ds : [edi + 0xE0]
				jmp g_objNpcPurchase.m_pRet

		}
	}

	void start()
	{
		g_objitemMall.Hook((PVOID)g_hookAddr.delay1, Naked_itemMall, 5);
		g_objNpcPurchase.Hook(PVOID(g_hookAddr.delay2), Naked_NpcPurchase, 6);
	}
}





