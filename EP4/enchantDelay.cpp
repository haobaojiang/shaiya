
#include "stdafx.h"
#include "enchantDelay.h"




namespace enchantDelay
{
	CMyInlineHook g_obj;

	CMyInlineHook g_objFixEnchantCrash;

	CRITICAL_SECTION g_cs;
	


	bool __stdcall isEnchantAble(Pshaiya50_player player) {
		
		EnterCriticalSection(&g_cs);
		bool bRet= isLegalTime(&player->nextEnchantTime, 1000);
		LeaveCriticalSection(&g_cs);
		return bRet;
	}


	__declspec(naked) void  Naked()
	{
		_asm {
			pushad
			MYASMCALL_1(isEnchantAble, ecx)
			test al, al
			popad
			jne _Org
			retn 0x4
			_Org:
			sub esp, 0xBF8
				jmp g_obj.m_pRet
		}
	}


	DWORD fallenchant = 0x00460523;
	__declspec(naked) void  Naked_enchantCrash()
	{
		_asm {
			movzx eax, byte ptr[ebp +0x4]
			cmp eax, 0x6
			jnb _failed
			lea edx, [eax + eax * 0x2]
			movzx eax, byte ptr[ebp + 0x5]
			cmp eax, 0x18
			jnb _failed
			jmp g_objFixEnchantCrash.m_pRet

			_failed:
			jmp fallenchant
		}
	}

	void start()
	{
		InitializeCriticalSection(&g_cs);
	//	g_obj.Hook(PVOID(0x0045fb20), Naked, 6);
		g_objFixEnchantCrash.Hook(PVOID(0x0045fbb3), Naked_enchantCrash, 11);
	}
}





