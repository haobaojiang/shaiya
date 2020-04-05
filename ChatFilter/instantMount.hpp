#pragma once
#ifndef SHAIYA_INSTANT_MOUNT_HEADER
#define SHAIYA_INSTANT_MOUNT_HEADER

#include "stdafx.h"
#include "config.hpp"

namespace InstantMount {

	ShaiyaUtility::CMyInlineHook g_obj, g_obj1;
	std::vector<DWORD> g_items;

	bool __stdcall fun(void* Player) {

		auto mountItem = ShaiyaUtility::EP6::GetInventoryItem(Player, 0, 13);
		auto itemid = ShaiyaUtility::EP6::PlayerItemToItemId(mountItem);

		for (auto id : g_items) {
			if (itemid == id) {
				return true;
			}
		}
		return false;
	}


	__declspec(naked) void  Naked_1()
	{
		_asm
		{
			pushad
			MYASMCALL_1(fun, edi)
			test al, al
			popad
			jz _org

			jmp g_obj1.m_pRet

			_org :
	      	mov dword ptr ds:[edi+0x1480],ebx
		    mov dword ptr ds : [edi + 0x147C] , ebx
			jmp g_obj1.m_pRet
		}
	}


	__declspec(naked) void  Naked()
	{
		_asm
		{

			pushad
			MYASMCALL_1(fun, ebp)
			test al,al
			popad
			jz _org
			jmp g_obj.m_pRet
			_org:
			mov dword ptr ss : [ebp + 0x1480] , esi
			jmp g_obj.m_pRet
		}
	}


	void Start() {

		if (!GameConfiguration::IsFeatureEnabled("instantMount")) {
			return;
		}

		for (auto i = 0; i < 100; i++) {
			char key[100]{};
			sprintf_s(key, 100, "item_%d", i);
			auto itemid = GameConfiguration::GetInteger("instantMount", key);
			if (itemid == 0) {
				continue;
			}
			g_items.push_back(itemid);
		}
		g_obj.Hook((void*)0x00477175, Naked, 6);
		g_obj1.Hook((void*)0x004787bb, Naked_1, 12);	
	}
}


#endif