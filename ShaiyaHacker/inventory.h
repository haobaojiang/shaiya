#pragma once

#include <vector>
#include <windows.h>
#include <string>
#include "util.h"
#include "global.h"
#include <optional>

namespace Game::Inventory {

	static DWORD ItemBase = (0x84F6A8);
	static DWORD offsetItemType = 0x10c1;
	static DWORD offsetTypeId = 0x10c2;
	static DWORD offsetReadItemName = 92;


	static DWORD 读取怪物名字call基址 = 0x856F3C;

	using namespace Game::Global;

	std::string GetItemName(DWORD ItemAddr) {
		auto p= Game::Util::read<DWORD>(ItemAddr);
		return Game::Util::readstr(p);
	}


	void SendPacket(void* Buf,int Len) {
		DWORD addr = 0x00586f50;
		_asm {
			push Len
			push Buf
			call addr
			add esp,0x8
		}
	}


	void SellItem(BYTE Bag,BYTE Slot,BYTE Count) {
#pragma pack(1)
		struct Packet {
			WORD cmd = 0x703;
			BYTE bag;
			BYTE slot;
			BYTE count;
		};
#pragma pop()
		Packet p;
		p.bag = Bag;
		p.slot = Slot;
		p.count = Count;
		SendPacket(&p, sizeof(p));
	}

	DWORD GetItemAddr(BYTE ItemType, BYTE TypeId) {
		auto Ecx = 读取怪物名字call基址;
		auto Edx = Game::Util::read<DWORD>(Ecx + 4);
		auto Eax = ItemType << 2;
		auto Esi = Game::Util::read<DWORD>(Edx + Eax - 4);
		Edx = TypeId * offsetReadItemName;
		DWORD itemAddr = Edx + Esi - offsetReadItemName;
		return itemAddr;
	}

	std::vector<UserItem> GetAllItems() {

		std::vector<UserItem> result;
		for (BYTE bag = 0; bag < 5; bag++) {
			for (BYTE slot = 0; slot < 24; slot++) {

				/*
				0055BCA4   .  8D0440        lea eax,dword ptr ds:[eax+eax*2]
				0055BCA7   .  8D04C2        lea eax,dword ptr ds:[edx+eax*8]
				0055BCAA   .  6BC0 22       imul eax,eax,0x22
				*/
				DWORD offset = bag * 3;
				offset = slot + offset * 8;
				offset *= 0x22;
				offset += ItemBase;

				// quality
				UserItem item;
				auto typeId = Game::Util::read<BYTE>(offset + offsetTypeId);
				auto itemType = Game::Util::read<BYTE>(offset + offsetItemType);


				if (typeId == 0) {
					continue;
				}

				std::string attribute = Game::Util::readstr(offset + offsetTypeId + 10);
				auto count = Game::Util::read<BYTE>(offset + offsetTypeId + 1);
				item.count = count;

				DWORD itemAddr = GetItemAddr(itemType, typeId);

				auto coolDown = Game::Util::read<DWORD>(itemAddr + 0x25);
				if (coolDown > 2000000) {
					coolDown = 0;
				}

				item.coolDown = coolDown;
				item.address = itemAddr;
				item.name = GetItemName(itemAddr);
				item.level = Game::Util::read<BYTE>(itemAddr + 12);
				item.job = Game::Util::read<BYTE>(itemAddr + 14);
				item.attackSpeed = Game::Util::read<BYTE>(itemAddr + 40);
				item.attr = static_cast<ItemAttribute>(Game::Util::read<BYTE>(itemAddr + 41));
				item.qualityMax = Game::Util::read<WORD>(itemAddr + 44);
				item.attackMin = Game::Util::read<WORD>(itemAddr + 46);
				item.attackMax = Game::Util::read<WORD>(itemAddr + 48);
				item.attackMax = item.attackMax + item.attackMin;
				item.hp = Game::Util::read<WORD>(itemAddr + 54);
				item.sp = Game::Util::read<WORD>(itemAddr + 56);
				item.mp = Game::Util::read<WORD>(itemAddr + 58);
				item.str = Game::Util::read<WORD>(itemAddr + 60);
				item.dex = Game::Util::read<WORD>(itemAddr + 62);
				item.rec = Game::Util::read<WORD>(itemAddr + 64);
				item.Int = Game::Util::read<WORD>(itemAddr + 66);
				item.wis = Game::Util::read<WORD>(itemAddr + 68);
				item.luc = Game::Util::read<WORD>(itemAddr + 70);
				item.speed = Game::Util::read<BYTE>(itemAddr + 72);
				item.bag = bag;
				item.slot = slot;
				item.Itemid = itemType * 1000 + typeId;

				result.push_back(item);
			}
		}
		return result;
	}

}