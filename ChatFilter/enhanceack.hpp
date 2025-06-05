
#ifndef SHAIYA_ENHANCE_ATTACK_HEADER
#define SHAIYA_ENHANCE_ATTACK_HEADER

#include "stdafx.h"
#include "config.hpp"
#include "onLogin.hpp"
#include <INIReader.h>


namespace enhanceAck {

	ShaiyaUtility::Packet::EnhanceAttack g_pack;

	void CallBack(void* Player) {
		ShaiyaUtility::EP6::SendToPlayer(Player, &g_pack, sizeof(g_pack));
	}

	void Start() {

		auto dir = ShaiyaUtility::GetCurrentExePathA();
		auto reader = std::make_unique<INIReader>(dir+ "\\data\\ItemEnchant.ini");

		LOGD << dir;

		if (reader->ParseError()) {
			ExitProcess(0);
		}
		for (auto i = 0; i < 51; i++) {
			char key[50]{};
			sprintf_s(key, 50, "WeaponStep%02d", i);
			auto value = reader->GetInteger("LapisianEnchantAddValue", key, 0);
			g_pack.values[i] = value;
		}

		onLogin::AddCallBack(CallBack);

	}
}

#endif
