
#ifndef SHAIYA_ITEM_MAP_HEADER
#define SHAIYA_ITEM_MAP_HEADER

#include "stdafx.h"
#include "config.hpp"
#include "callBack.hpp"
#include <windows.h>
#include <process.h>
#include "../utility/utility.h"

namespace itemForMap
{
	ShaiyaUtility::CMyInlineHook g_obj;

	struct ItemInfo {
		WORD map;
		DWORD itemId;
		BYTE count;
	};

	std::vector<ItemInfo> g_items;

	std::vector<std::function<void(ShaiyaUtility::EP4::CUser*, WORD, ShaiyaUtility::EP4::SVector*, bool*)>> g_funs;

	void __stdcall fun(ShaiyaUtility::EP4::CUser* Player, WORD Map, ShaiyaUtility::EP4::SVector* Pos, bool* pRet)
	{
		for (const auto& item : g_items) {
			if (item.map == Map) {

				if (!ShaiyaUtility::EP4::IsEnoughInventoryItems(Player, item.itemId, item.count)) {
					*pRet = false;
					return;
				}

				if (!ShaiyaUtility::EP4::DeletePlayerItemid(Player, item.itemId, item.count)) {
					*pRet = false;
					return;
				}

				return;
			}
		}
	}

	bool __stdcall fireportCheck(ShaiyaUtility::EP4::CUser* player, WORD Map, float x, float y, float z)
	{
		bool bRet = true;
		ShaiyaUtility::EP4::SVector pos;
		pos.x = x;
		pos.y = y;
		pos.z = z;

		for (auto fun: g_funs) {
			fun(player, Map, &pos, &bRet);
		}
		return bRet;
	}


	PVOID  g_pFailedAddr = (PVOID)0x00469C37;
	__declspec(naked) void Naked()
	{
		_asm
		{
			pushad
			MYASMCALL_5(fireportCheck, ebp, edx, ebx, edi, esi)
			test al,al
			popad
			je _wrong

			mov dword ptr ss : [ebp + 0x5818] , 0x1
			jmp g_obj.m_pRet
			_wrong :
			jmp g_pFailedAddr
		}
	}



	void start()
	{
		if (!GameConfiguration::IsFeatureEnabled("itemformap")) {
			return;
		}

		

		for (auto i = 0; i < 100; i++) {
			auto s =GameConfiguration::Get("itemformap", std::to_string(i));
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

		g_funs.push_back(fun);

		auto hookThread = [](void* p)->void {
			Sleep(20 * 1000);
			g_obj.Hook(0x00468f25, Naked, 10);
		};
		_beginthread(hookThread, 0, 0);
		
	}
}


#endif


