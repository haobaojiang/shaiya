#include "stdafx.h"
#include "CrossFactionTrade.h"

namespace CrossFactionTrade {
	CMyInlineHook g_obj;




	DWORD addr = 0x0046ed04;
	__declspec(naked) void  Naked()
	{
		_asm
		{
			cmp word ptr [eax+0x164],0x2a
			je __successAddr

			mov cl,byte ptr [eax+0x12d]
			jmp g_obj.m_pRet

			__successAddr:
			jmp addr
		}
	}


	void Start()
	{
		g_obj.Hook((PVOID)0x0046ecf6, Naked, 6);
	}
}

