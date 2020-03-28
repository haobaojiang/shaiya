#include "stdafx.h"
#include "enchantDelay.h"




namespace enchantDelay
{

	CMyInlineHook g_obj;


	bool __stdcall fun(Pshaiya_player player) {
#ifndef _DEBUG
		VMProtectBegin("enchantDelay fun");
#endif 
		return isLegalTime(&player->nextEnchantTime, 1000);
#ifndef _DEBUG
		VMProtectEnd();
#endif
	}

	__declspec(naked) void  Naked()
	{
		_asm {
			pushad
			mov eax,dword ptr [esp+0x24]
			MYASMCALL_1(fun,eax)
			test al,al
			popad
			jne _Org
			retn 0x4

			_Org:
			sub esp,0xc18
				jmp g_obj.m_pRet
		}
	}

	void start()
	{
		g_obj.Hook(PVOID(0x0046cba0), Naked, 6);
	}
}





