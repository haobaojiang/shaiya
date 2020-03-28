
#include "stdafx.h"
#include "timeoutLogs.h"







namespace timeoutLogger
{
	CMyInlineHook g_objW, g_objS, g_objD;


	void __stdcall fun_sTimeover() {
		bool static s_isLoged = false;
		if (!s_isLoged) {
			LOGD << "fun_sTimeover:" << GetCurrentThreadId();
			s_isLoged = true;
		}
	}

	void __stdcall fun_dTimeover() {
		bool static s_isLoged = false;
		if (!s_isLoged) {
			LOGD << "fun_dTimeover:" << GetCurrentThreadId();
			s_isLoged = true;
		}
	}



	void __stdcall fun_wTimeover() {
		bool static s_isLoged = false;
		if (!s_isLoged) {
			LOGD << "fun_wTimeover:" << GetCurrentThreadId();
			s_isLoged = true;
		}
	}

	__declspec(naked) void  naked_wTimeover()
	{
		_asm {
			pushad
			MYASMCALL(fun_wTimeover)
			popad
			push 0x54719C
			jmp g_objW.m_pRet
		}
	}

	
	__declspec(naked) void  naked_dTimeover()
	{
		_asm {
			pushad
			MYASMCALL(fun_dTimeover)
			popad
			push 0x547198
			jmp g_objD.m_pRet
		}
	}

	__declspec(naked) void  naked_sTimeover()
	{
		_asm {
			pushad
			MYASMCALL(fun_sTimeover)
			popad
			push 0x547194
			jmp g_objS.m_pRet
		}
	}


	void Start()
	{
		g_objW.Hook(PVOID(0x00407375), naked_wTimeover, 5);
		g_objD.Hook(PVOID(0x00406e85), naked_dTimeover, 5);
		g_objS.Hook(PVOID(0x004070da), naked_sTimeover, 5);
	}
}





