#include "stdafx.h"
#include "getPlayer.h"







namespace getPlayer
{


	CMyInlineHook g_objGetPlayer;
	DWORD g_index = 0;



	void __stdcall  fun_getPlayer(Pshaiya_player player)
	{
		g_players[g_index++] = player;
	}


	__declspec(naked) void Naked_GetBaseAddr()
	{
		__asm
		{
			mov dword ptr[edi], 0x00571AA8  //Ô­Óï¾ä

			pushad
			MYASMCALL_1(fun_getPlayer,edi)
			popad

			jmp g_objGetPlayer.m_pRet
		}
	}


	void Start()
	{
		DWORD temp = sizeof(shaiya_player);
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00411f74, &temp, sizeof(temp));
		g_objGetPlayer.Hook((PVOID) 0x00454c4c, Naked_GetBaseAddr,6);
	}
}
