#include "stdafx.h"

#include "onLogin.h"



namespace onLogin
{
	CMyInlineHook g_obj;
	CMyInlineHook g_objToonselection;
	DWORD dwCall = 0x00445a10;

	void __stdcall  fun(Pshaiya_player player) {
		ZeroMemory(&player->nextMalltime, sizeof(shaiya_player) - offsetof(shaiya_player, nextMalltime));
		for (auto iter = g_vecOnloginCallBack.begin(); iter != g_vecOnloginCallBack.end(); iter++) {
			(*iter)(player);
		}
	}



	__declspec(naked) void  Naked_toonSelection()
	{
		_asm {
			
			pushad
			MYASMCALL_1(fun, ebp)
			popad
			mov dword ptr[esp + 0x1c], ebx
			cmp byte ptr ds : [edi + 0x7], bl
			jmp g_objToonselection.m_pRet
		}
	}

	void start()
	{
		g_objToonselection.Hook(PVOID(0x0047b522), Naked_toonSelection, 7);
	}
}





