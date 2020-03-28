#include "stdafx.h"

#include "purchaseDelay.h"





namespace purchaseDelay
{
	CMyInlineHook g_objitemMall, g_objNpcPurchase;
	DWORD refuseItemMallAddr = 0x00479882;
	DWORD refuseNpcAddr = 0x0046b6d6;


#define DELAYTIME (2000)
	bool WINAPI IsBuyAble(Pshaiya50_player player)
	{
		//检测时间间隔
		bool bRet = true;
		DWORD CurrentTime = GetTickCount();
		DWORD nextMallTime = player->nextMallTime;

		//有时，这个时间会不对

		do
		{
			if (!nextMallTime) {
				break;
			}

			if (nextMallTime > CurrentTime)
			{
				bRet = false;
				DWORD leftTime = nextMallTime - CurrentTime;


				if (leftTime > DELAYTIME) {
					bRet = true;          //特殊情况
					break;
				}
			}

		} while (0);

		if (bRet) {
			player->nextMallTime = CurrentTime + DELAYTIME;
		}
		else {
		//	DisConnectPlayer(player);
		}
		return bRet;
	}

	__declspec(naked) void  Naked_itemMall()
	{
		_asm {
			pushad
			MYASMCALL_1(IsBuyAble, ebx)
			cmp al, 0x1
			popad

			je _org
			jmp refuseItemMallAddr

			_org :
			mov eax, dword ptr[0x546f6c]
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
			mov eax, dword ptr[ebx + 0x2]
				push eax
				mov eax, dword ptr[edi + 0xe0]
				jmp g_objNpcPurchase.m_pRet

		}
	}


	void Start()
	{
		g_objitemMall.Hook((PVOID)g_hookAddr.g_objitemMall, Naked_itemMall, 5);
		g_objNpcPurchase.Hook(PVOID(g_hookAddr.g_objNpcPurchase), Naked_NpcPurchase, 10);
	}

}




