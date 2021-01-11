#pragma once
#ifndef SHAIYA_BANNED_ITEM_HEADER
#define SHAIYA_BANNED_ITEM_HEADER

#include "stdafx.h"
#include "config.hpp"

namespace banned_item {


	WORD g_map = 0;
	std::vector<DWORD> g_items;
	ShaiyaUtility::CMyInlineHook g_obj;


	bool __stdcall item_usable(void* player,DWORD ItemObj) {

		if (ShaiyaUtility::EP6::PlayerMap(player) != g_map) {
			return true;
		}

		auto itemid = ShaiyaUtility::EP6::ItemObjToItemid(ItemObj);
		auto iter = std::find(g_items.begin(),g_items.end(), itemid);
		if (iter == g_items.end()) {
			return true;
		}
		return false;
	}

	DWORD failedAddr = 0x0047469F;
	__declspec(naked) void  Naked()
	{
		_asm {
			pushad
			MYASMCALL_2(item_usable,ebp,ecx);
			test al,al
			popad
			je  _banned

			cmp dword ptr ds : [ecx + 0xA0] , 0x1A
			jmp g_obj.m_pRet
		_banned:
			jmp failedAddr
		}
	}

	void start() {

		auto section = "banned_item";
		if (!GameConfiguration::GetBoolean(section, "enable", false)) {
			return;
		}

		// read map
		g_map = static_cast<WORD>(GameConfiguration::GetInteger(section, "map", -1));


		// read items
		for (auto i = 0; i < 100; i++) {
			char key[50]{};
			sprintf(key, "%d", i);

			auto item = GameConfiguration::GetInteger(section, key, 0);
			if (item == 0) {
				continue;
			}

			LOGD << "banned item:" << item;
			g_items.push_back(item);
		}

		// 00472E23  |.  83B9 A0000000 1A    cmp dword ptr ds:[ecx+0xA0],0x1A
		g_obj.Hook(0x00472E23, Naked, 7);
	}
}

#endif