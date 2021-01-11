#pragma once

#ifndef SHAIYA_PARTY_HOOK_HEADER
#define SHAIYA_PARTY_HOOK_HEADER
#include "stdafx.h"


namespace part_hook {

	ShaiyaUtility::CMyInlineHook g_objPartyMenu;
	ShaiyaUtility::CMyInlineHook g_objPartyWords;
	ShaiyaUtility::CMyInlineHook g_objPartyJoin;

	using party_callBack = std::function<bool(void*, void*)>;
	std::vector<party_callBack> g_callbacks;

	bool __stdcall is_party_able(void* request_player, void* target_player)
	{
		for (auto callback : g_callbacks) {
			if (!callback(request_player,target_player)) {
				return false;
			}
		}
		return true;
	}

	DWORD partyWords_addr = 0x00475505;
	__declspec(naked) void  Naked_partyWords()
	{
		_asm {
			pushad
			MYASMCALL_2(is_party_able, ebp, edi)
			cmp al, 0x1
			popad
			je _org
			jmp partyWords_addr

			_org :
			mov cl, byte ptr ds : [edi + 0x12D]
				jmp g_objPartyWords.m_pRet

		}
	}

	DWORD partyJoin_addr = 0x004757c8;
	__declspec(naked) void  Naked_PartyJoin()
	{
		_asm {
			pushad
			MYASMCALL_2(is_party_able, ebp, edi)
			cmp al, 0x1
			popad
			je _org
			jmp partyJoin_addr

			_org :
			mov dl, byte ptr ss : [ebp + 0x12D]
				jmp g_objPartyJoin.m_pRet

		}
	}

	DWORD partyMenu_addr = 0x004753a7;
	__declspec(naked) void  Naked_partyMenu()
	{
		_asm {
			pushad
			MYASMCALL_2(is_party_able, esi, ebp)
			cmp al, 0x1
			popad
			je __org
			jmp partyMenu_addr

			__org :
			mov dl, byte ptr ds : [esi + 0x12D]
				jmp g_objPartyMenu.m_pRet
		}
	}


	void register_callback(party_callBack callback) {
		g_callbacks.push_back(callback);
	}


	void start() {

		g_objPartyJoin.Hook((PVOID)0x004757ba, Naked_PartyJoin, 6);
		g_objPartyMenu.Hook(PVOID(0x00475396), Naked_partyMenu, 6);
		g_objPartyWords.Hook(PVOID(0x004754f7), Naked_partyWords, 6);

	}
}

#endif