#pragma once
#ifndef SHAIYA_COMBINE_HEADER
#define SHAIYA_COMBINE_HEADER

#include "stdafx.h"
#include "config.hpp"
#include "callBack.hpp"
#include <array>

namespace Combine {

	ShaiyaUtility::CMyInlineHook g_obj;

#pragma pack(push,1)
	struct CombineRequestPacket :ShaiyaUtility::Packet::Header {
		BYTE bag[3];
		BYTE slot[3];
	};
#pragma pack(pop)

	std::vector<ShaiyaUtility::Packet::ItemCombines> g_items;


	bool __stdcall combineFunc(void* P, DWORD item1, DWORD item2, DWORD item3) {

		//sort item
		DWORD itemid[3] = { ShaiyaUtility::EP6::PlayerItemToItemId(item1),ShaiyaUtility::EP6::PlayerItemToItemId(item2),ShaiyaUtility::EP6::PlayerItemToItemId(item3) };
		for (DWORD i = 0; i < 3; i++) {
			for (DWORD j = i + 1; j < 3; j++) {
				if (itemid[j] > itemid[i]) {
					DWORD temp = itemid[j];
					itemid[j] = itemid[i];
					itemid[i] = temp;
				}
			}
		}

		for (const auto item : g_items)
		{
			if (item.materials[0].id ==itemid[0] &&
				item.materials[1].id == itemid[1] &&
				item.materials[2].id == itemid[2])
			{
				if (ShaiyaUtility::EP6::IsEnoughInventoryItems(P, item.materials[0].id, item.materials[0].count) &&
					ShaiyaUtility::EP6::IsEnoughInventoryItems(P, item.materials[1].id, item.materials[1].count) &&
					ShaiyaUtility::EP6::IsEnoughInventoryItems(P, item.materials[2].id, item.materials[2].count))
				{

#pragma warning( push )
#pragma warning( disable : 4244 )
					ShaiyaUtility::EP6::DeletePlayerItemid(P, item.materials[0].id, item.materials[0].count);
					ShaiyaUtility::EP6::DeletePlayerItemid(P, item.materials[1].id, item.materials[1].count);
					ShaiyaUtility::EP6::DeletePlayerItemid(P, item.materials[2].id, item.materials[2].count);
#pragma warning( pop )

					ShaiyaUtility::EP6::SendItem(P, item.rewardID, 1);

					char strNotice[MAX_PATH] = { 0 };
					sprintf_s(strNotice, MAX_PATH, "恭喜 [%s] 通过 [%d个%s] [%d个%s] ",
						ShaiyaUtility::EP6::PlayerName(P).c_str(),
						item.materials[0].count, ShaiyaUtility::EP6::Itemid2Name(item.materials[0].id).c_str(),
						item.materials[1].count, ShaiyaUtility::EP6::Itemid2Name(item.materials[1].id).c_str());

					char strNotice2[MAX_PATH]{ 0 };
					sprintf_s(strNotice2, MAX_PATH, "[%d个%s] 兑换 [%s]成功!",
						item.materials[2].count,
						ShaiyaUtility::EP6::Itemid2Name(item.materials[2].id).c_str(),
						ShaiyaUtility::EP6::Itemid2Name(item.rewardID).c_str());

					auto noticeLength = strlen(strNotice) + strlen(strNotice2);
					if (noticeLength > ShaiyaUtility::EP6::MAX_NOTICE_LENGTH) {

						ShaiyaUtility::EP6::SendMsgToAll(strNotice);
						ShaiyaUtility::EP6::SendMsgToAll(strNotice2);

					}
					else {
						std::string notice(strNotice);
						notice.append(strNotice2);
						ShaiyaUtility::EP6::SendMsgToAll(notice.c_str());
					}

					return true;

				}
				else {
					ShaiyaUtility::EP6::SendMsgToPlayer(P, "材料数量不够!");
					return false;
				}
			}
		}

		LOGD << "combination failed " << itemid[0] << "," << itemid[1] << "," << itemid[2];
		return false;
	}


	DWORD addr = 0x0046E105;
	__declspec(naked) void  Naked()
	{
		_asm
		{
			pushad
			MYASMCALL_4(combineFunc, edi, eax, ecx, esi)
			test al, al
			popad
			//		je _Org

			jmp addr

			//		_Org :
			//		jmp g_obj.m_pRet

		}
	}



	void Start() {

		static char* section = "combine";
		if (!GameConfiguration::IsFeatureEnabled(section)) {
			return;
		}

		for (int i = 0; i < 1000; i++) {

			auto key = ShaiyaUtility::itos(i);
			auto data = GameConfiguration::Get(section, key);
			if (data.size() == 0 || data[0] == '\0') {
				continue;
			}

			ShaiyaUtility::Packet::ItemCombines item;
			sscanf_s(data.c_str(), "%d,(%ld,%ld),(%ld,%ld),(%ld,%ld)", &item.rewardID,
				&item.materials[0].id, &item.materials[0].count,
				&item.materials[1].id, &item.materials[1].count,
				&item.materials[2].id, &item.materials[2].count);

			if (item.rewardID == 0 ||
				item.materials[0].id == 0 ||
				item.materials[0].count == 0 ||
				item.materials[1].id == 0 ||
				item.materials[1].count == 0 ||
				item.materials[2].id == 0 ||
				item.materials[2].count == 0) {
				continue;
			}

			// sort data
			for (int i = 0; i < 3; i++) {
				for (int j = i + 1; j < 3; j++) {
					if (item.materials[j].id > item.materials[i].id) {

						auto id = item.materials[j].id;
						auto count = item.materials[j].count;

						item.materials[j].id = item.materials[i].id;
						item.materials[j].count = item.materials[i].count;

						item.materials[i].id = id;
						item.materials[i].count = count;
					}
				}
			}

			char str[MAX_PATH]{ 0 };
			sprintf_s(str, MAX_PATH, "%d, %d:%d, %d:%d, %d:%d", item.rewardID,
				item.materials[0].id, item.materials[0].count,
				item.materials[1].id, item.materials[1].count,
				item.materials[2].id, item.materials[2].count
				);
			LOGD << str;

			g_items.emplace_back(item);
		}

		if (g_items.size()) {
			GameEventCallBack::AddLoginCallBack([](void* p)
				{
					ShaiyaUtility::Packet::ItemCombines pack;
					ShaiyaUtility::EP6::SendToPlayer(p, &pack);
				});

			g_obj.Hook(0x0046da3c, Naked, 6);
		}

	}
}


#endif