#include "stdafx.h"
#include "Customconsole.h"

namespace Customconsole {
	CMyInlineHook g_obj;

	void __stdcall fun_console(const char* pInput)
	{
		for (auto iter = g_consoleCallBack.begin(); iter != g_consoleCallBack.end(); iter++) {
			(*iter)(pInput);
		}
	}



	DWORD dwConsolTemp = 0x004D5ECC;
	__declspec(naked) void  Naked_Console()
	{
		_asm
		{
			pushad
			MYASMCALL_1(fun_console,ebx)
			popad
			cmp eax, 0x2
			jnz __tag
			jmp g_obj.m_pRet
			__tag :
			jmp dwConsolTemp
		}
	}


	void Start()
	{
		g_obj.Hook((PVOID)0x004D5EC0, Naked_Console, 5);//004D5EC0  |.  83F8 02       cmp eax,0x2
	}
}

