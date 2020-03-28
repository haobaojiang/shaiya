#include "stdafx.h"
#include "antidupe.h"

namespace antiDupe {

	CMyInlineHook g_obj;


	void cancelTrade(Pshaiya50_player player) {
		BYTE cmd[] = { 0x05 ,0x0A ,0x02 ,0x00 ,0x00 ,0x00 };
		DWORD temp = (DWORD)cmd;

		DWORD dwCall = 0x0046ec90;
		_asm {
			mov ecx,player
			mov edx, temp
			call dwCall
		}
	}


	void __stdcall fun_AntiDupe(Pshaiya50_player player){
		cancelTrade(player);
	}



	DWORD dwConsolTemp = 0x004D5ECC;
	__declspec(naked) void  Naked()
	{
		_asm
		{
			pushad
			MYASMCALL_1(fun_AntiDupe,esi)
			popad
			 lea ecx,[edi+0x130]
			 jmp g_obj.m_pRet
		}
	}


	void Start()
	{
		g_obj.Hook((PVOID)0x004d8fcb, Naked, 6);
		
		//取消PK游戏币
		{
			BYTE temp[] = { 0x0e9,0x85,0x00,0x00,0x00,0x90,0x90 };
			MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0047d98f, temp, sizeof(temp));
		}


		//取消PK物品
		{
			BYTE temp[] = { 0xe9,0x34,0x03,0x00,0x00,0x90,0x90 };
			MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0047d6e0, temp, sizeof(temp));
		}
	}
}

