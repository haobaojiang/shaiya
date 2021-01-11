#pragma once
#ifndef SHAIYA_GET_PLAYERS_HEADER
#define SHAIYA_GET_PLAYERS_HEADER


#include "stdafx.h"


namespace getPlayer
{


	ShaiyaUtility::CMyInlineHook g_objGetPlayer;

	DWORD g_index = 0;
	void* g_players[2000]{};

	void __stdcall  fun_getPlayer(void* player)
	{
		g_players[g_index++] = player;
	}

	void enumPlayers(std::function<bool(void*)> callBack) {
		for (auto i = 0; i < ARRAYSIZE(g_players); i++) {
			if (g_players[i] == nullptr) {
				break;
			}
			if(!ShaiyaUtility::EP6::IsLegalPlayer(g_players[i]))
			{
				continue;
			}
			if (!callBack(g_players[i])) {
				break;
			}
		}
	}

	__declspec(naked) void Naked_GetBaseAddr()
	{
		__asm
		{
			mov byte ptr ss : [esp + 0x1C] , 0x1
			pushad
			MYASMCALL_1(fun_getPlayer, edi)
			popad

			jmp g_objGetPlayer.m_pRet
		}
	}


	void Start()
	{	
		//00454C46  |.  C64424 1C 01        mov byte ptr ss:[esp+0x1C],0x1
		g_objGetPlayer.Hook((PVOID)0x00454C46, Naked_GetBaseAddr, 5);
	}
}

#endif