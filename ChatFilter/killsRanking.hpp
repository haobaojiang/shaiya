#pragma once
#ifndef SHAIYA_KILLS_RANKING_HEADER
#define SHAIYA_KILLS_RANKING_HEADER


#include "stdafx.h"
#include "config.hpp"
#include "callBack.hpp"


namespace KillsRanking {

	static ShaiyaUtility::Packet::KillsRankingContent g_packet;

	void callBack(void* Player) {
		ShaiyaUtility::EP6::SendToPlayer(Player, &g_packet, sizeof(g_packet));
	}

	void Start() {

#pragma pack(push,1)
		struct  
		{
			DWORD level{ 0 };
			DWORD kills{ 0 };
			DWORD normalModePoints{ 0 };
			DWORD hardModePoints{ 0 };
			DWORD ultimateModePoints{ 0 };
		}rank[31];
#pragma pack(pop)

		memcpy(rank,reinterpret_cast<void*>(0x00581fc8),sizeof(rank));

		for (int i = 0; i < 31; i++) {
			g_packet.killsNeeded[i] = rank[i].kills;
			g_packet.normalModePoints[i] = rank[i].normalModePoints;
			g_packet.hardModePoints[i] = rank[i].hardModePoints;
			g_packet.ultimateModePoints[i] = rank[i].ultimateModePoints;
		}

		GameEventCallBack::AddLoginCallBack(callBack);
	}
}

#endif