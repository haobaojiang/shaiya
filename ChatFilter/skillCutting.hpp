#pragma once
#ifndef SHAIYA_SKILLCUTTING_HEADER
#define SHAIYA_SKILLCUTTING_HEADER


#include "stdafx.h"
#include "config.hpp"
#include "callBack.hpp"


namespace SkillCutting {


	int g_mode = 0;

	void callBack(void* Player) {
		ShaiyaUtility::Packet::SkillCuttingContent packet;
		packet.mode = g_mode;
		ShaiyaUtility::EP6::SendToPlayer(Player, &packet, sizeof(packet));
	}

	void Start() {

		if (!GameConfiguration::IsFeatureEnabled("skillcutting")) {
			return;
		}

		auto g_mode = GameConfiguration::GetInteger("skillcutting", "mode");

		GameEventCallBack::AddLoginCallBack(callBack);
	}
}

#endif