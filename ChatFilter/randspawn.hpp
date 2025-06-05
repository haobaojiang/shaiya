#ifndef SHAIYA_RAND_SPAWN_HEADER
#define SHAIYA_RAND_SPAWN_HEADER

#include "stdafx.h"
#include "config.hpp"
#include "callBack.hpp"
#include <optional>

namespace RandSpawn {

	ShaiyaUtility::CMyInlineHook g_obj;
	ShaiyaUtility::CMyInlineHook g_onObMobTrigger;
	bool g_isWantedObId{};
	DWORD g_wantObId{};


	std::array<DWORD, 150> g_zones;
	std::array<std::string, 150> g_mapNames;

	void init_zones() {
		static bool is_init = false;
		if (is_init) {
			return;
		}
		is_init = true;
		for (auto i = 0;i < g_zones.size();i++) {
			g_zones[i] = *PDWORD(0x98 * i + *PDWORD(0x010a2018 + 4) + 4);
		}
	}


	WORD get_map_id_from_zone(DWORD zone) {
		init_zones();
		for (auto i = 0;i < g_zones.size();i++) {
			if (g_zones.at(i) == zone) {
				return WORD(i);
			}
		}
		return 0;
	}

	std::string get_map_name_from_id(WORD mapId) {
		return g_mapNames.at(mapId);
	}

	std::string get_mob_name_from_mob_object(DWORD mobObject) {
		return reinterpret_cast<char*>(*(PDWORD(mobObject + 0xd4)) + 2);
	}

	void __stdcall on_process_ob_id(DWORD id) {
		g_isWantedObId = id >= g_wantObId;
	}


	void __stdcall on_creating_ob_mob(DWORD zoneObject, DWORD mobObject) {

		if (!g_isWantedObId) {
			return;
		}

		//	DWORD mobId = *PDWORD(mobObject + 0x2a4);
		std::string mob_name = get_mob_name_from_mob_object(mobObject);
		WORD mapId = get_map_id_from_zone(zoneObject);
		ShaiyaUtility::Pos* pos = reinterpret_cast<ShaiyaUtility::Pos*>(mobObject + 0x7c);
		std::string mapName = get_map_name_from_id(mapId);
		auto msg = std::make_unique<char[]>(900);

		SYSTEMTIME lt{};
		GetLocalTime(&lt);

		sprintf_s(msg.get(), 900, "怪物[ %s ]出现在了[ %s ]地图,坐标[ %d , %d ] 时间:[ %d:%d ].",
			mob_name.c_str(),
			mapName.c_str(),
			int(pos->x),
			int(pos->z),
			static_cast<DWORD>(lt.wHour),
			static_cast<DWORD>(lt.wMinute));

		ShaiyaUtility::EP6::SendMsgToAll(msg.get());
	}


	DWORD  g_orgCall = 0x0041CFF0;
	__declspec(naked) void Naked_on_creating_ob_mob()
	{
		_asm
		{
			pushad
			MYASMCALL_2(on_creating_ob_mob, ecx, eax)
			popad
			call g_orgCall
			jmp g_obj.m_pRet
		}
	}

	DWORD g_dwCall1 = 0x00422DE0;
	__declspec(naked) void Naked_on_ob_mob_trigger()
	{
		_asm
		{
			pushad
			mov eax, dword ptr[eax]
			MYASMCALL_1(on_process_ob_id, eax)
			popad
			call g_dwCall1
			jmp g_onObMobTrigger.m_pRet
		}
	}


	void start() {

		auto section = "mob_rand_spawn";
		if (!GameConfiguration::IsFeatureEnabled(section)) {
			return;
		}

		for (int i = 0;i < 100;i ++) {
			char key[100]{};
			sprintf_s(key, 100, "%d", i);
			auto val = GameConfiguration::Get(section, key, "");
		}

		g_wantObId = GameConfiguration::GetInteger("mob_rand_spawn", "ob_id", 0);
		if (g_wantObId == 0) {
			return;
		}


		// init map name
		auto path = ShaiyaUtility::GetCurrentExePathA();
		path += "\\map_name.ini";
		auto ini = new INIReader(path.c_str());
		for (auto i = 0;i < g_mapNames.size();i++) {
			char key[100]{};
			sprintf_s(key, 100, "%d", i);
			g_mapNames[i] = ini->Get("map_name", key, "");
		}


		g_obj.Hook(0x00422f75, Naked_on_creating_ob_mob, 5);
		g_onObMobTrigger.Hook(0x004228ec, Naked_on_ob_mob_trigger, 5);
	}
}


#endif