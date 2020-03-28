#ifndef SHAIYA_NAMECOLOR_HEADER
#define SHAIYA_NAMECOLOR_HEADER

#include "stdafx.h"
#include "config.hpp"
#include "callBack.hpp"


namespace NameColor
{

	static ShaiyaUtility::Packet::NameColorContent lastNameColorContent;

	void Loop(void*)
	{


		while (true)
		{
			Sleep(60 * 1000);
			ShaiyaUtility::Packet::NameColorContent nameColorContent ;

			int i = 0;
			for (i = 0; i < ARRAYSIZE(nameColorContent.players); i++)
			{
				char key[MAX_PATH] = { 0 };
				sprintf_s(key, MAX_PATH, "%d", i + 1);
				auto ret = GameConfiguration::Get("namecolor", key);
				if (ret == "") {
					break;
				}

				sscanf_s(ret.c_str(), "%d,%d", 
					&nameColorContent.players[i].charid, 
					&nameColorContent.players[i].rgb);

				nameColorContent.players[i].rgb |= 0xff000000;
			}

			if (i==0) {
				continue;
			}

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