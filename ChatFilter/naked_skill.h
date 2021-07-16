#pragma once
#ifndef SHAIYA_NAKED_SKILL_HEADER
#define SHAIYA_NAKED_SKILL_HEADER

#include "stdafx.h"
#include "config.hpp"
#include "callBack.hpp"
#include "fireport.hpp"

namespace NakedSkill
{
	std::unique_ptr<INIReader> g_conf;
	std::vector<BYTE> g_maps;


	/*
	std::map<DWORD, bool> g_enabledPlayers;
	std::mutex g_mutex;
	bool is_player_enabled(void* player)
	{
		auto uid = ShaiyaUtility::EP6::PlayerUid(player);
		std::lock_guard lock(g_mutex);
		auto iter = g_enabledPlayers.find(uid);
		if(iter ==g_enabledPlayers.end())
		{
			return false;
		}
		return iter->second;
	}
	*/

	void start()
	{
		auto path = ShaiyaUtility::GetCurrentExePathA();
		path += "\\naked_skill.ini";

		g_conf =  std::make_unique<INIReader>(path);
		
		auto section = "base_setting";
		if (!g_conf->GetBoolean(section, "enable", false)) {
			return;
		}

		for(auto i=0;i<20;i++)
		{
			auto key = std::string("ignore_map_") + std::to_string(i);
			auto m = g_conf->Get(section, key, "");
			if(m.empty())
			{
				continue;
			}
			LOGD << "naked skill map:" << m;
			g_maps.push_back(static_cast<BYTE>(std::atoi(m.c_str()))+1);
		}

		

		
		GameEventCallBack::AddLoginCallBack([&](void* player)
			{
				ShaiyaUtility::Packet::NakedSkillEffect p;
			//	p.enabled = is_player_enabled(player);
			    for(auto i=0;i<g_maps.size();i++)
			    {
					p.maps[i] =g_maps[i];
			    }
				ShaiyaUtility::EP6::SendToPlayer(player, &p, sizeof(p));
			});
		
	}
}


#endif
