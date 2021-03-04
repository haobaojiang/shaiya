
#ifndef SHAIYA_ITEM_MAP_HEADER
#define SHAIYA_ITEM_MAP_HEADER

#include "stdafx.h"
#include "fireport.hpp"
#include "../utility/utility.h"

namespace itemForMap
{

	struct ItemInfo {
		WORD map;
		DWORD itemId;
		BYTE count;
	};

	std::vector<ItemInfo> g_items;
	
	bool  fun(void* Player, WORD Map)
	{
		for (const auto& item : g_items) {
			if (item.map == Map) {

				if (!ShaiyaUtility::EP6::IsEnoughInventoryItems(Player, item.itemId, item.count)) {
					return false;
				}

				if (!ShaiyaUtility::EP6::DeletePlayerItemid(Player, item.itemId, item.count)) {
					return false;
				}
				return true;
			}
		}
		return true;
	}
	
	void start()
	{
		if (!GameConfiguration::IsFeatureEnabled("ItemMap")) {
			return;
		}
		
		for (auto i = 0; i < 100; i++) {
			
			auto s = GameConfiguration::Get("ItemMap", std::to_string(i));
			if (s.empty()) {
				continue;
			}

			auto temp = ShaiyaUtility::split(s, ',');
			ItemInfo item;
			item.map = std::stoi(temp[0]);
			item.itemId = std::stoi(temp[1]);
			item.count = std::stoi(temp[2]);

			LOGD << "id:" << item.itemId << " map:" << item.map << " count:" << item.count;
			g_items.push_back(item);
		}

		fire_port::register_callBack_2(fun);
	}
}

#endif


