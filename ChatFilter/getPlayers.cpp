#include "stdafx.h"
#include "getPlayers.h"



namespace GetPlayers
{
	CMyInlineHook g_objGetPlayers;
	DWORD g_index = 0;



	void __stdcall fun_getPlayers(DWORD player)
	{
		g_players[g_index++] = player;
	}


	__declspec(naked) void Naked_getPlayers()
	{
		_asm {
			add esp, 0x4
			mov dword ptr ss : [esp + 0x8], eax
			pushad
			MYASMCALL_1(fun_getPlayers, eax)
			popad
			jmp g_objGetPlayers.m_pRet
		}
	}

	void Start()
	{
		g_objGetPlayers.Hook((PVOID)0x0040aa72, Naked_getPlayers, 7);
	}
}