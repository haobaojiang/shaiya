#pragma once
#ifndef SHAIYA_SKILL_TRADE_CHAT_HEADER
#define SHAIYA_SKILL_TRADE_CHAT_HEADER

#include "stdafx.h"
#include "config.hpp"

namespace SkillTradeChat
{
	/*

	static WORD g_skill_id = 0;
	
	bool is_skill_enabled(DWORD player) {
		DWORD skillHead = *PDWORD(player + 0xa94);
		DWORD pNext = *PDWORD(skillHead + 0x4);
		do
		{
			if (pNext == skillHead) {
				break;
			}

			DWORD skill = *PDWORD(pNext + 0x34);
			if (!skill) {
				break;
			}

			WORD skillid = *PWORD(skill);
			if (skillid == g_skill_id) {
				return true;
			}
			pNext = *PDWORD(pNext + 0x4);
		} while (1);
		return false;
	}



	ShaiyaUtility::CMyInlineHook g_tradeChat;
	__declspec(naked) void Naked_OnTradeChat()
	{
		_asm
		{
			lea eax, dword ptr ds : [edi + 0x18]
			lea ecx, dword ptr ss : [esp + 0x20]
			pushad
			MYASMCALL_3(onTradeChat, ebp, ecx, eax)
			popad
			jmp g_tradeChat.m_pRet
		}
	}

	void start()
	{

		auto section = "buf_cross_trade_chat";
		if (!GameConfiguration::GetBoolean(section, "enable", false)) {
			return;
		}

		auto skillId = GameConfiguration::GetInteger(section, "skill_id", false);
		if (skillId == 0)
		{
			LOGD << "skill id is 0";
			return;
		}
		g_skill_id = skillId;
		
		// trade chat
		g_tradeChat.Hook(0x0047F78B, Naked_OnTradeChat, 7);
	}
	*/
}


#endif
