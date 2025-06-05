#pragma once
#ifndef SHAIYA_BANNED_ITEM_HEADER
#define SHAIYA_BANNED_ITEM_HEADER

#include "stdafx.h"
#include "config.hpp"

namespace banned_item {


	struct BanItem  {
		BYTE mapId = 0;
		DWORD itemId = 0;
	};

	std::vector<BanItem> g_items;
	ShaiyaUtility::CMyInlineHook g_obj;


	bool __stdcall item_usable(void* player,DWORD ItemObj) {
		auto playerMap = ShaiyaUtility::EP6::PlayerMap(player);
		auto itemid = ShaiyaUtility::EP6::ItemObjToItemid(ItemObj);
		for (auto item : g_items) {
			if (item.mapId == playerMap && item.itemId == itemid) {
				return false;
			}
		}
		return true;
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




		// read items
		for (auto i = 0; i < 100; i++) {
			char key[50]{};
			sprintf(key, "%d", i);
			LOGD << "banned item key:" << key;
			auto val = GameConfiguration::Get(section, key);
			LOGD << "banned item val:" << val;
			if (val == "") {
				break;
			}
			DWORD mapId = 0;
			DWORD itemId = 0;
			sscanf(val.c_str(), "%d,%d", &mapId,&itemId);
			LOGD << "banned item:" << itemId;
			g_items.push_back(BanItem{static_cast<BYTE>(mapId),itemId});
		}

		// 00472E23  |.  83B9 A0000000 1A    cmp dword ptr ds:[ecx+0xA0],0x1A
		g_obj.Hook(0x00472E23, Naked, 7);
	}
}

#endif