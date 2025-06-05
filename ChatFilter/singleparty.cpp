
#include "stdafx.h"
#include "singleparty.h"




namespace SingleParty
{
	ShaiyaUtility::CMyInlineHook g_objSingleParty;
	DWORD dwCall = 0x0044ed70;
	__declspec(naked) void  Naked_singleparty()
	{
		_asm {
			cmp ecx,edi
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
		ShaiyaUtility::WriteCurrentProcessMemory((PVOID)0x004754e8, temp, sizeof(temp));
		g_objSingleParty.Hook(PVOID(0x0047562b), Naked_singleparty, 5);
	}
}





