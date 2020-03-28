#include "stdafx.h"
#include "expRemoving.h"




namespace expRemoving
{

	CMyInlineHook g_obj;



	DWORD passExpaddr = 0x0046518a;
	__declspec(naked) void  Naked()
	{
		_asm {
			cmp word ptr[esi + 0x160], 0x12
			je _passexpAddr
			cmp word ptr[esi + 0x160], 0x1e
			je _passexpAddr

			mov dword ptr [esi + 0x0000013C], edi
			jmp g_obj.m_pRet

			_passexpAddr:
			jmp passExpaddr

		}
	}

	void start()
	{
		g_obj.Hook(PVOID(0x0046517f), Naked, 6);
	}
}





