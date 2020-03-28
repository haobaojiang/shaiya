
#include "stdafx.h"
#include "vsFixed.h"




namespace vsFixed
{

	CMyInlineHook g_obj;



	DWORD addr1 = 0x00479155;
	__declspec(naked) void Naked()
	{
		_asm
		{
			cmp dword ptr[edi + 0x558c], 0x0
			jne __exit

			movzx eax, byte ptr[ebp + 0x2]
			mov esi, edi
			jmp g_obj.m_pRet


			__exit :
			jmp addr1
		}
	}

	void start()
	{
		g_obj.Hook((PVOID)0x00478932, Naked, 6);
	}
}





