#pragma once
#ifndef SHAIYA_CUSTOMIZED_3_HEADER
#define SHAIYA_CUSTOMIZED_3_HEADER

#include "stdafx.h"
#include "config.hpp"
#include "party_hook.hpp"
#include "fireport.hpp"
#include "get_players.hpp"
#include <mutex>

namespace customized_3 {

	WORD g_map = 0;
	ShaiyaUtility::CMyInlineHook g_objCommonSkill;

	std::mutex g_part_tabl_lock;
	std::map<DWORD,void*> g_parties; // charId as key,party table as value


	int g_homeMap = 42;
	float g_homeX = 50;
	float g_homeY = 15;
	float g_homeZ = 50;
	DWORD g_itemId = 0;
	DWORD g_itemCount = 0;

	// task timer
	std::vector<WORD> g_expiredTimes;

	bool should_be_same_party(void* playerA, void* playerB) {
		std::lock_guard lock(g_part_tabl_lock);
		auto bParty = g_parties.find(ShaiyaUtility::EP6::PlayerCharid(playerB));
		auto aParty = g_parties.find(ShaiyaUtility::EP6::PlayerCharid(playerA));
		if (bParty == g_parties.end() ||
			aParty == g_parties.end() ||
			aParty->second != bParty->second) {
			return false;
		}
		return true;
	}

	bool is_same_group_ppl(void* pplA, void* pplB) {
		auto partyA = ShaiyaUtility::EP6::PlayerParty(pplA);
		auto partyB = ShaiyaUtility::EP6::PlayerParty(pplB);
		if (partyA == 0 || partyB == 0) {
			return false;
		}
		return partyA == partyB;
	}

	bool __stdcall isCommonSkillUsable(void* attack, void* target) {

		bool bRet = (ShaiyaUtility::EP6::PlayerCountry(attack) == ShaiyaUtility::EP6::PlayerCountry(target));
		if (ShaiyaUtility::EP6::PlayerMap(attack) != g_map) {
			return bRet;
		}
		if (ShaiyaUtility::EP6::PlayerMap(target) != g_map) {
			return bRet;
		}

		return is_same_group_ppl(attack, target);
	}

	__declspec(naked) void  Naked_CommonSkill()
	{
		_asm {
			pushad
			MYASMCALL_2(isCommonSkillUsable, esi, edx)
			cmp al, 0x1
			popad
			sete al
			retn
		}
	}

	DWORD g_randomSeek = 1120;
	void __stdcall  SetRandomName(void* player, char* name) {

		if (ShaiyaUtility::EP6::PlayerMap(player) != g_map) {
			return;
		}

		auto n = GetTickCount();
		auto s = std::make_unique<char[]>(50);
		sprintf_s(s.get(), 50, "%lu", n);
		strcpy(name, s.get());
	}

	ShaiyaUtility::CMyInlineHook g_randomNameHook;
	__declspec(naked) void  Naked_RandomName()
	{
		_asm {
			lea ebp, [esp + 0x61]
			pushad
			MYASMCALL_2(SetRandomName, ebx, ebp)
			popad
			lea ebp, dword ptr ds : [ebx + 0x1C0]
			jmp g_randomNameHook.m_pRet
		}
	}

	bool __stdcall is_send_zone_user_shape(void* player) {
		return (ShaiyaUtility::EP6::PlayerMap(player) != g_map);
	}


	void __stdcall process_name(void* player_to_send, void* player_send_to ,BYTE* packet) {

		if (ShaiyaUtility::EP6::PlayerMap(player_to_send) != g_map) {
			return;
		}

		if (is_same_group_ppl(player_to_send, player_send_to)) {
			return;
		}

		// set name
		char* name = (char*)&packet[0x2E];
		auto n = GetTickCount();
		auto s = std::make_unique<char[]>(50);
		sprintf_s(s.get(), 50, "%lu", n);
		strcpy(name, s.get());

		// set guid_name
		char* guid_name = (char*)&packet[0x49];
		guid_name[0] = 0;
	}

	ShaiyaUtility::CMyInlineHook g_process_name;
	__declspec(naked) void  Naked_process_name()
	{
		_asm {
			mov ecx, dword ptr ss : [esp + 0x18]
			pushad
			mov eax, dword ptr[esp + 0x20]
			MYASMCALL_3(process_name, ebx, ecx,eax)
			popad
			jmp g_process_name.m_pRet
		}
	}

	ShaiyaUtility::CMyInlineHook g_process_name_2;
	__declspec(naked) void  Naked_process_name_2()
	{
		_asm {
			push eax
			lea eax, dword ptr ss : [esp + 0x38]
			push eax
			jmp g_process_name_2.m_pRet
		}
	}

	//call 00427470

	ShaiyaUtility::CMyInlineHook g_process_name_3;
	DWORD dwCall3 = 0x00427470;
	__declspec(naked) void  Naked_process_name_3()
	{
		_asm {
			pushad
			MYASMCALL_1(is_send_zone_user_shape, ebp)
			cmp al, 0x1
			popad
			je _org
			add esp, 0xc
			jmp g_process_name_3.m_pRet

			_org :
			call dwCall3
				jmp g_process_name_3.m_pRet
		}
	}


	bool is_party_able(void* request_player, void* target_player) {

		auto player_map = ShaiyaUtility::EP6::PlayerMap(request_player);
		auto target_map = ShaiyaUtility::EP6::PlayerMap(target_player);
		if (player_map != g_map && target_map != g_map) {
			return true;
		}

		return  should_be_same_party(request_player, target_player);
	}

	//get faction from B
	BYTE __stdcall GetFaction(void* playerA, void* playerB) {

		// check map
		auto mapA = ShaiyaUtility::EP6::PlayerMap(playerA);
		auto mapB = ShaiyaUtility::EP6::PlayerMap(playerB);
		auto result = ShaiyaUtility::EP6::PlayerCountry(playerB);
		if (mapA != g_map || mapB != g_map) {
			return result;
		}

		auto country = ShaiyaUtility::EP6::PlayerCountry(playerA);
		if (!is_same_group_ppl(playerA, playerB)) {
			return country == 0 ? 1 : 0;
		}
		return country;
	}


	ShaiyaUtility::CMyInlineHook g_objGetFaction;
	__declspec(naked) void  Naked_Faction()
	{
		_asm
		{
			push eax
			push edx
			push ebx
			push esp
			push ebp
			push esi
			push edi

			MYASMCALL_2(GetFaction, ebp, ebx)
			movzx ecx, al

			pop edi
			pop esi
			pop ebp
			pop esp
			pop ebx
			pop edx
			pop eax
			cmp dword ptr[ebx + 00001444], 01
			jmp g_objGetFaction.m_pRet
		}
	}


	bool __stdcall AttackAble(void* Attacker, void* Target)
	{
		if (ShaiyaUtility::EP6::PlayerMap(Attacker) != g_map)
		{
			return  ShaiyaUtility::EP6::PlayerCountry(Attacker) != 
				ShaiyaUtility::EP6::PlayerCountry(Target);
		}

		return (!is_same_group_ppl(Attacker, Target));
	}

	ShaiyaUtility::CMyInlineHook g_objAttack;
	__declspec(naked) void  Naked_Attack()
	{
		_asm {
			pushad
			MYASMCALL_2(AttackAble, eax, edx)
			cmp al, 0x0             //把结果存放在zf标志位里先
			popad
			sete al
			pop esi
			retn
		}
	}

	bool g_time_bits[24][60]{};
	bool fire_port_check(void* player, WORD Map, ShaiyaUtility::Pos* pos) {

		if (Map != g_map) {
			return true;
		}

		// check time
		SYSTEMTIME st{};
		GetLocalTime(&st);
		if (!g_time_bits[st.wHour][st.wMinute]) {
			LOGD << "!g_time_bits[st.wHour][st.wMinute]: " << st.wHour << " :" << st.wMinute;
			return false;
		}

		// check skill
		auto job = ShaiyaUtility::EP6::GetPlayerJob(player);
		auto total_skills = ShaiyaUtility::EP6::GetPlayerSkillNTotal(player);
		for (auto i = 0; i < total_skills; i++) {
			auto skillObj = ShaiyaUtility::EP6::GetPlayerSkillObj(player, i);
			auto skillUid = ShaiyaUtility::EP6::GetSkillObjUid(skillObj);
			auto skilInfo = ShaiyaUtility::EP6::GetSkillInfo(skillObj);
			if (!ShaiyaUtility::EP6::CheckSkillUsableForJob(skilInfo, job)) {
				ShaiyaUtility::EP6::RemoveSkill(player, skillUid);
			}
		}

		// must has a party
		auto party = ShaiyaUtility::EP6::PlayerParty(player);
		if (party == 0) {
			return false;
		}

		// process table
		std::lock_guard lock(g_part_tabl_lock);
		auto charId = ShaiyaUtility::EP6::PlayerCharid(player);
		auto part_table = g_parties.find(charId);
		if (part_table == g_parties.end()) {
			//if not found
			g_parties[charId] = (void*)party;
			return true;
		}else {
			// the party table must same with the previous one
			return (part_table->second == (void*)party);
		}
	}


	void move_out_players() {


		getPlayer::enumPlayers([](void* player)->bool {
			if (ShaiyaUtility::EP6::PlayerMap(player) != g_map) {
				return true;
			}
			ShaiyaUtility::EP6::MovePlayer(player, g_homeMap, g_homeX, g_homeY, g_homeZ);
			return true;
			});

		Sleep(20 * 1000);

		getPlayer::enumPlayers([](void* player)->bool {
			if (ShaiyaUtility::EP6::PlayerMap(player) != g_map) {
				return true;
			}
			ShaiyaUtility::EP6::KickPlayer(player);
			return true;
			});
	}

	void clean_parties() {
		std::lock_guard lock(g_part_tabl_lock);
		g_parties.clear();
	}

	void map_expired_monitor(void*) {

		while (true)
		{
			SYSTEMTIME localTime{};
			GetLocalTime(&localTime);

			for (auto expiredTime : g_expiredTimes) {
				if (localTime.wHour == expiredTime &&
					localTime.wMinute == 1) {
					move_out_players();
					clean_parties();
					Sleep(20 * 1000);
					break;
				}
			}



			Sleep(1000);
		}
	}

	void start() {

		auto section = "customized_3";

		if (!GameConfiguration::GetBoolean(section, "enable", false)) {
			return;
		}

		// add party limitation
		auto map = GameConfiguration::GetInteger(section, "map", -1);
		if (map == -1) {
			LOGD << "invalid map value";
			ExitProcess(0);
		}

		g_itemId = GameConfiguration::GetInteger(section, "item_id", 0);
		g_itemCount = GameConfiguration::GetInteger(section, "item_count", 0);


		LOGD << "itemid:" << g_itemId << " item_count:" << g_itemCount;


		g_map = static_cast<WORD>(map);

		LOGD << "custmozed_1,map:" << g_map;
		part_hook::register_callback(is_party_able);

		// get faction event
		g_objGetFaction.Hook((PVOID)0x00426af7, Naked_Faction, 7);


		// attack 
		g_objAttack.Hook((PVOID)0x00457e27, Naked_Attack, 6);

		// fireport
		fire_port::register_callBack(fire_port_check);

		for (auto i = 0; i < 20; i++) {
			char timeKey[MAX_PATH]{};
			sprintf_s(timeKey, MAX_PATH, "time_%02d", i);
			auto s = GameConfiguration::Get(section, timeKey, "");
			if (s == "") {
				break;
			}

			int openHour{};
			int hours{};
			sscanf(s.c_str(), "%d,%d", &openHour, &hours);

			if (hours == 0) {
				LOGD << "hours =0:" << s;
				ExitProcess(0);
			}

			LOGD << "openHours:" << openHour << " , hours:" << hours;

			for (auto j = 0; j < hours; j++) {
				for (auto k = 0; k < 60; k++) {
					g_time_bits[j + openHour][k] = true;
				}
			}

			g_expiredTimes.push_back(openHour + hours);
		}

		// expired map
		auto expired_home = GameConfiguration::Get(section, "expired_home");
		sscanf(expired_home.c_str(), "%d,%f,%f,%f",
			&g_homeMap,
			&g_homeX,
			&g_homeY,
			&g_homeZ);


		// random name
		//00426BB4  |.  8DAB C0010000             lea ebp,dword ptr ds:[ebx+0x1C0]
	//	g_randomNameHook.Hook(0x00426BB4, Naked_RandomName, 6);


		//00457F33  |.  8A86 2D010000 mov al,byte ptr ds:[esi+0x12D]
		g_objCommonSkill.Hook(0x00457F33, Naked_CommonSkill, 6);

		// guid name
		// 00426C34  |.  8B83 DC010000 mov eax,dword ptr ds:[ebx+0x1DC]
		// 00491EB9 | E8 B255F9FF              | call <ps_game.sub_427470>                                  |
		g_process_name.Hook(0x00426c96, Naked_process_name, 5);
		g_process_name_2.Hook(0x00426c91, Naked_process_name_2, 5);
		g_process_name_3.Hook(0x00491eb9, Naked_process_name_3, 5);


		// 00426C72     /EB 22                jmp Xps_game.00426C96
		{
			BYTE data[] = { 0xeb,0x22 };
			ShaiyaUtility::WriteCurrentProcessMemory((void*)0x00426C72, data, 2);
		}

		{
			BYTE data[] = { 0xeb,0x22 };
			ShaiyaUtility::WriteCurrentProcessMemory((void*)0x00426C72, data, 2);
		}
		{
			//00426AE4     /E9 AD010000          jmp ps_game.00426C96


			BYTE data[] = { 0xE9 ,0xAD ,0x01 ,0x00 ,0x00 };
			ShaiyaUtility::WriteCurrentProcessMemory((void*)0x00426AE4, data, 5);
		}

		//
		_beginthread(map_expired_monitor, 0, 0);
	}

}

#endif