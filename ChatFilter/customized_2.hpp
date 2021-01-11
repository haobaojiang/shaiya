#pragma once
#ifndef SHAIYA_CUSTOMIZED_2_HEADER
#define SHAIYA_CUSTOMIZED_2_HEADER

#include "stdafx.h"
#include "config.hpp"

namespace Customized2
{
	
	static WORD g_skill_id = 0;
	static DWORD g_rgb = {};
	static DWORD g_kills = 0;
	static DWORD g_sleep_time = 0;

	using applyColorDef = std::function<void(void*, DWORD)>;
	using removeColorDef = std::function<void(void*)>;
	applyColorDef g_applyColor;
	removeColorDef g_removeColor;
	

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
			if (skillid==g_skill_id) {
				return true;
			}
			pNext = *PDWORD(pNext + 0x4);
		} while (1);
		return false;
	}


	

	void __stdcall onMapLoading(void* Player)
	{
		if(!is_skill_enabled(DWORD(Player)))
		{
			g_removeColor(Player);
		}else
		{
			g_applyColor(Player,g_rgb);
		}
	}


	void __stdcall useSkill(void* Player,DWORD SkilObj)
	{
		auto skillId = ShaiyaUtility::EP6::GetSkillId(SkilObj);
		if(g_skill_id !=skillId)
		{
			return;
		}

		g_applyColor(Player, g_rgb);
	}


	void __stdcall onSkillRemoved(void* player, DWORD SkilObj) {
		auto skillId = ShaiyaUtility::EP6::GetSkillId(SkilObj);
		if (g_skill_id != skillId)
		{
			return;
		}
		g_removeColor(player);
	}

	void __stdcall onTradeChat(void* Player,void* packet,DWORD packetLength)
	{
		if(!is_skill_enabled((DWORD)Player))
		{
			return;
		}

		auto country = ShaiyaUtility::EP6::PlayerCountry(Player);
		if(country==1)
		{
			country = 0;
		}else
		{
			country = 1;
		}

		// send chat to another country
		DWORD dwCall = 0x004192F0;
		_asm{
			push country
			mov eax, packetLength
			mov ecx, packet
			call dwCall
		}
	}



	// 0045CBB0  /$  83EC 28       sub esp,0x28



	ShaiyaUtility::CMyInlineHook g_hookUseSkill;
	__declspec(naked) void  Naked_UseSkill(){
		_asm{

			pushad
			MYASMCALL_2(useSkill,ecx,esi)
			popad
			
			sub esp, 0x28
			mov eax, dword ptr ds : [0x57F7C4]
			jmp g_hookUseSkill.m_pRet
		}

	}

	ShaiyaUtility::CMyInlineHook g_onSkillRemovedHook;
	__declspec(naked) void Naked_OnSkillRemoved()
	{
		_asm{
			mov eax,[esp+0x28]  //skill
			pushad
			MYASMCALL_2(onSkillRemoved,ecx,eax)
			popad
			mov eax, dword ptr ds : [0x57F7C4]
			xor eax, esp
			mov dword ptr ss : [esp + 0x20] , eax
			jmp g_onSkillRemovedHook.m_pRet
		}
	}

	ShaiyaUtility::CMyInlineHook g_onLoadingHook;
	__declspec(naked) void Naked_OnLoading()
	{
		_asm {
			pushad
			MYASMCALL_1(onMapLoading, ebx)
			popad

			pop edi
			pop esi
			pop ebx
			mov ecx, dword ptr ss : [esp + 0x28]
			jmp g_onLoadingHook.m_pRet
		}
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





	bool set_player_kills(void* player)
	{
		if(!is_skill_enabled((DWORD)player))
		{
			return true;
		}

		ShaiyaUtility::EP6::AddPlayerKills(player, g_kills);
		Sleep(50);
		return true;
	}

	void auto_kills(void* p)
	{
		while (true)
		{
			getPlayer::enumPlayers(set_player_kills);
			
			Sleep(g_sleep_time * 1000);
		}
	}



	void start(applyColorDef appColorFun,removeColorDef removeColorFun)
	{
		auto section = "customized_2";
		if (!GameConfiguration::GetBoolean(section, "enable", false)) {
			return;
		}

		LOGD << "skill_name_color: enable";

		auto skillId = GameConfiguration::GetInteger(section, "skill_id", false);
		if(skillId==0)
		{
			LOGD << "skill id is 0";
			return;
		}
		
		auto rgb = GameConfiguration::GetInteger(section, "rgb", false);
		if (rgb == 0)
		{
			LOGD << "rgb is 0";
			return;
		}
		
		auto kills = GameConfiguration::GetInteger(section, "kills", 0);
		if (kills == 0) {
			LOGD << "kills is 0";
			return;
		}


		auto sleeptime = GameConfiguration::GetInteger(section, "kill_sleep_time", 0);
		if (sleeptime == 0) {
			LOGD << "sleeptime is 0";
			return;
		}


		g_sleep_time = sleeptime;
		g_kills = kills;
		g_applyColor = std::move(appColorFun);
		g_removeColor = std::move(removeColorFun);
		g_skill_id = skillId;
		g_rgb = rgb;
		

		
		// use skill
		g_hookUseSkill.Hook(0x0045CBB0, Naked_UseSkill, 8);
			
		// remove skill
		g_onSkillRemovedHook.Hook(PVOID(0x00494768), Naked_OnSkillRemoved, 6);

		// loading map
		g_onLoadingHook.Hook(0x00474B2B, Naked_OnLoading, 7);

		// auto kills
		_beginthread(auto_kills, 0, 0);

		// cross trade chat
		g_tradeChat.Hook(0x0047F78B, Naked_OnTradeChat, 7);
	}
}


#endif