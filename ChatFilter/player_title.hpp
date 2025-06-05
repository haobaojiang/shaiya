#ifndef SHAIYA_PLAYER_TITLE_HEADER
#define SHAIYA_PLAYER_TITLE_HEADER

#include "stdafx.h"
#include "config.hpp"
#include "callBack.hpp"


namespace PlayerTitle {

	 ShaiyaUtility::Packet::TitleInfo g_title_info_first_part;
	 ShaiyaUtility::Packet::TitleInfo g_title_info_sec_part;
	


	std::vector<ShaiyaUtility::Packet::PlayerTitle > read_from_config()
	{
		auto path = ShaiyaUtility::GetCurrentExePathA();
		path += "\\player_title.ini";
		auto ini = std::make_unique<INIReader>(path.c_str());


		std::vector<ShaiyaUtility::Packet::PlayerTitle > result;
		for (auto i = 0; i < 100; i++)
		{
			char key[MAX_PATH] = { 0 };
			sprintf_s(key, MAX_PATH, "%d", i);
			auto ret = ini->Get("player_title", key,"");
			if (ret == "") {
				break;
			}

			ShaiyaUtility::Packet::PlayerTitle titleItem;
			sscanf(ret.c_str(), "%d,%d,%s", 
				&titleItem.charId,
				&titleItem.Color,
				titleItem.text );

			result.push_back(titleItem);
		}
		return result;
	}

	void Loop(void*)
	{


		while (true)
		{
			Sleep(30 * 1000);


			auto i = 0;

			// cfg colors
			auto items = read_from_config();

			// split it to 2 parts
			ShaiyaUtility::Packet::PlayerTitle first_part[50]{};
			ShaiyaUtility::Packet::PlayerTitle sec_part[50]{};

			for (auto i = 0;i < items.size() && i<50;i++) {
				first_part[i] = items[i];
			}
			for (auto i = 50;i < items.size() && i < 100;i++) {
				sec_part[i-50] = items[i];
			}



			// no changed
			if (memcmp(first_part, g_title_info_first_part.titles, sizeof(first_part)) == 0 &&
				memcmp(sec_part,g_title_info_sec_part.titles, sizeof(sec_part))==0) {
				continue;
			}

			// 
			memcpy(g_title_info_first_part.titles, first_part, sizeof(first_part));
			memcpy(g_title_info_sec_part.titles, sec_part, sizeof(sec_part));

			ShaiyaUtility::EP6::SendPacketAll(&g_title_info_first_part, sizeof(g_title_info_first_part));
			ShaiyaUtility::EP6::SendPacketAll(&g_title_info_sec_part, sizeof(g_title_info_sec_part));
		}
	}

	void Start()
	{

		auto path = ShaiyaUtility::GetCurrentExePathA();
		path += "\\player_title.ini";
		auto ini = new INIReader(path.c_str());


		if (!ini->GetBoolean("player_title","enable",false)){
			return;
		}

		g_title_info_first_part.isFirstPart = true;
		g_title_info_sec_part.isFirstPart = false;

		GameEventCallBack::AddLoginCallBack([&](void* player){
			auto uid = ShaiyaUtility::EP6::PlayerUid(player);
			auto first_part = std::make_unique<ShaiyaUtility::Packet::TitleInfo>();
			auto sec_part = std::make_unique<ShaiyaUtility::Packet::TitleInfo>();

			memcpy(first_part.get(), &g_title_info_first_part, sizeof(ShaiyaUtility::Packet::TitleInfo));
			memcpy(sec_part.get(), &g_title_info_sec_part, sizeof(ShaiyaUtility::Packet::TitleInfo));

			first_part->addr1 ^= uid;
			first_part->addr2 ^= uid;

			ShaiyaUtility::EP6::SendToPlayer(player, first_part.get(), sizeof(ShaiyaUtility::Packet::TitleInfo));
			ShaiyaUtility::EP6::SendToPlayer(player, sec_part.get(), sizeof(ShaiyaUtility::Packet::TitleInfo));
		});

		_beginthread(Loop, 0, 0);
	}
}

#endif