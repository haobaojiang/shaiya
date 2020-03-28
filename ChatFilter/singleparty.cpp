#include "stdafx.h"

#include "singleparty.h"



namespace SingleParty
{
	CMyInlineHook g_objSingleParty;
	DWORD dwCall = 0x00445a10;
	__declspec(naked) void  Naked_singleparty()
	{
		_asm {
			cmp eax,edi
			jne _org
			mov al,0x0
			jmp g_objSingleParty.m_pRet
			_org:
			call dwCall
			jmp g_objSingleParty.m_pRet
		}
	}

	void start()
	{
		BYTE temp[] = { 0x90,0x90 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004678b0, temp, sizeof(temp));
		g_objSingleParty.Hook(PVOID(g_hookAddr.g_objSingleParty), Naked_singleparty, 5);
	}
}





