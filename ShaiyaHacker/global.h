#pragma once

#include <vector>
#include <windows.h>
#include <string>

namespace Game::Global {

	enum ItemAttribute :BYTE {
		None = 0,
		Fire = 1,
		Water = 2,
		Wind = 3
	};

	struct UserItem {
		DWORD Itemid{};
		BYTE  slot{};
		BYTE bag{};
		std::string name;
		BYTE count{};
		DWORD address{};
//		std::string desc{};
		DWORD coolDown{};
		std::string addtionalAttr;
		DWORD ebp{}; // ?
		DWORD hp{};
		DWORD sp{};
		DWORD mp{};
		DWORD str{};
		DWORD dex{};
		DWORD rec{};
		DWORD Int{};
		DWORD wis{};
		DWORD luc{};
		BYTE level{};
		BYTE job{};
		WORD qualityMax{};
		WORD attackMin{};
		WORD attackMax{};
		BYTE attackSpeed{};
		ItemAttribute attr{};
		std::string lapis[6];
		bool isHasLapis{};
		BYTE speed{};
	};

	struct GearAttribute {
		DWORD str{};
		DWORD dex{};
		DWORD rec{};
		DWORD Int{};
		DWORD wis{};
		DWORD luc{};
		DWORD hp{};
		DWORD mp{};
		DWORD sp{};
		BYTE enhance{};
	};

	struct Pos {
		float x;
		float z;
		float y;
	};

	struct Mob {
		DWORD id{};
		DWORD curHP{};
		DWORD maxHP{};
		DWORD index;
		Pos pos{};
		std::string targetName;
		DWORD distance{};
		std::string name;
		DWORD mobIndex{};
		DWORD status{};
		ItemAttribute attribute{};
		BYTE level{};
		DWORD mobType{};
	};


	struct DropedItem {
		std::string object;
		DWORD id{};
		std::string name;
		DWORD distance{};
		Pos pos{};
	};



}