#ifndef SHAIYA_NAMECOLOR_HEADER
#define SHAIYA_NAMECOLOR_HEADER

#include "stdafx.h"
#include "config.hpp"
#include "callBack.hpp"


namespace NameColor
{

	static ShaiyaUtility::Packet::NameColorContent lastNameColorContent;


	struct ColorItem
	{
		DWORD charid = 0;
		DWORD rgb = 0;
	};

	std::map<DWORD, DWORD> g_colors;
	
	CRITICAL_SECTION g_cs;

	
	void applyPlayerColor(void* player, DWORD rgb)
	{
		auto charId = ShaiyaUtility::EP6::PlayerCharid(player);
		EnterCriticalSection(&g_cs);
		g_colors[charId] = rgb;
		LeaveCriticalSection(&g_cs);
	}

	void removePlayerColor(void* player)
	{
		auto charId = ShaiyaUtility::EP6::PlayerCharid(player);
		EnterCriticalSection(&g_cs);
		auto iter = g_colors.find(charId);
		if(iter!=g_colors.end())
		{
			g_colors.erase(iter);
		}
		LeaveCriticalSection(&g_cs);
	}




	std::vector<ColorItem> read_config_colors()
	{
		std::vector<ColorItem> result;
		for (auto i = 0; i < 200; i++)
		{
			char key[MAX_PATH] = { 0 };
			sprintf_s(key, MAX_PATH, "%d", i + 1);
			auto ret = GameConfiguration::Get("namecolor", key);
			if (ret == "") {
				break;
			}
			
			ColorItem item;
			sscanf_s(ret.c_str(), "%d,%d",	&item.charid,	&item.rgb);
			result.push_back(item);
		}
		return result;
	}

	void Loop(void*)
	{


		while (true)
		{
			Sleep(10 * 1000);
			ShaiyaUtility::Packet::NameColorContent nameColorContent ;


			auto i = 0;

			// cfg colors
			auto cfg_colors = read_config_colors();
			for(auto color :cfg_colors)
			{
				nameColorContent.players[i].charid = color.charid;
				nameColorContent.players[i].rgb = 0xff000000|color.rgb;
				i++;
			}
			

			// other colors
			EnterCriticalSection(&g_cs);
			for(const auto& [charId,rgb] : g_colors)
			{
				nameColorContent.players[i].charid = charId;
				nameColorContent.players[i].rgb = 0xff000000 | rgb;
				i++;
			}
			LeaveCriticalSection(&g_cs);
			
			// no changed
			if (memcmp(&nameColorContent, &lastNameColorContent,sizeof(ShaiyaUtility::Packet::NameColorContent)) == 0) {
				continue;
			}
			
			// 
			memcpy(&lastNameColorContent, &nameColorContent, sizeof(ShaiyaUtility::Packet::NameColorContent));

			ShaiyaUtility::EP6::SendPacketAll(&nameColorContent, sizeof(nameColorContent));
			
		}
	}

	void Start()
	{
		InitializeCriticalSection(&g_cs);
		if (!GameConfiguration::IsFeatureEnabled("namecolor")) {
			return;
		}



		GameEventCallBack::AddLoginCallBack([&](void* player)
			{
				ShaiyaUtility::EP6::SendToPlayer(player, &lastNameColorContent,sizeof(lastNameColorContent));
			});

		_beginthread(Loop, 0, 0);
	}

};

#endif