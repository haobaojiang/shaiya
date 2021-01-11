#pragma once
#ifndef SHAIYA_CUSTOMIZED_1_HEADER
#define SHAIYA_CUSTOMIZED_1_HEADER

#include "stdafx.h"
#include "config.hpp"
#include "party_hook.hpp"
#include "fireport.hpp"
#include "get_players.hpp"


namespace customized_1 {

	WORD g_map = 0;
	ShaiyaUtility::CMyInlineHook g_objCommonSkill;


	int g_homeMap = 42;
	float g_homeX = 50;
	float g_homeY = 15;
	float g_homeZ = 50;

	// task timer
	std::vector<WORD> g_expiredTimes;

	bool __stdcall isCommonSkillUsable(void* attack, void* target) {

		bool bRet = (ShaiyaUtility::EP6::PlayerCountry(attack) == ShaiyaUtility::EP6::PlayerCountry(target));

		if (ShaiyaUtility::EP6::PlayerMap(attack) != g_map) {
			return bRet;
		}
		if (ShaiyaUtility::EP6::PlayerMap(target) != g_map) {
			return bRet;
		}

		return false;
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
	void __stdcall  SetRandomName(void* player,char* name) {

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
			lea ebp,[esp+0x61]
			pushad
			MYASMCALL_2(SetRandomName,ebx,ebp)
			popad
			lea ebp, dword ptr ds : [ebx + 0x1C0]
			jmp g_randomNameHook.m_pRet
		}
	}


	void __stdcall process_name(void* player,BYTE* packet) {

		if (ShaiyaUtility::EP6::PlayerMap(player) != g_map) {
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
			MYASMCALL_2(process_name, ebx, eax)
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




	bool is_party_able(void* request_player,void* target_player) {

		auto player_map = ShaiyaUtility::EP6::PlayerMap(request_player);
		if (g_map == player_map) {
			return false;
		}

		auto target_map = ShaiyaUtility::EP6::PlayerMap(target_player);
		if (g_map == target_map) {
			return false;
		}

		return true;
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

		// reverse country
		auto country = ShaiyaUtility::EP6::PlayerCountry(playerA);
		return country == 0 ? 1 : 0;
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
		if (ShaiyaUtility::EP6::PlayerMap(Attacker) == g_map &&
			ShaiyaUtility::EP6::PlayerMap(Target) == g_map)
		{
			return false;
		}

		auto attack_country = ShaiyaUtility::EP6::PlayerCountry(Attacker);
		auto target_country = ShaiyaUtility::EP6::PlayerCountry(Target);
		return attack_country == target_country;
	}

	ShaiyaUtility::CMyInlineHook g_objAttack;
	__declspec(naked) void  Naked_Attack()
	{
		_asm {
			pushad
			MYASMCALL_2(AttackAble,eax,edx)
			cmp al, 0x1             //把结果存放在zf标志位里先
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

		if (ShaiyaUtility::EP6::PlayerParty(player)!=0) {
			LOGD << "ShaiyaUtility::EP6::PlayerParty(player)!=0";
			return false;
		}

		// check time
		SYSTEMTIME st{};
		GetLocalTime(&st);
		if(!g_time_bits[st.wHour][st.wMinute]){
			LOGD << "!g_time_bits[st.wHour][st.wMinute]: " << st.wHour<< " :"<< st.wMinute;
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


		return true;
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

	void map_expired_monitor(void*) {

		while (true)
		{
			SYSTEMTIME localTime{};
			GetLocalTime(&localTime);

			for (auto expiredTime: g_expiredTimes) {
				if (localTime.wHour == expiredTime && 
					localTime.wMinute==1) {
					move_out_players();
					Sleep(20 * 1000);
					break;
				}
			}



			Sleep(1000);
		}
	}

	void start() {

		auto section = "customized_1";

		if (!GameConfiguration::GetBoolean(section, "enable", false)) {
			return;
		}

		// add party limitation
		auto map = GameConfiguration::GetInteger(section, "map", -1);
		if (map == -1) {
			LOGD << "invalid map value";
			ExitProcess(0);
		}
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
		g_process_name.Hook(0x00426c96, Naked_process_name, 5);
		g_process_name_2.Hook(0x00426c91, Naked_process_name_2, 5);


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


			BYTE data[] = { 0xE9 ,0xAD ,0x01 ,0x00 ,0x00};
			ShaiyaUtility::WriteCurrentProcessMemory((void*)0x00426AE4, data, 5);
		}

		//
		_beginthread(map_expired_monitor, 0, 0);
	}

}

#endif