#include "stdafx.h"
#include "ffg.h"
#include <utility.h>

namespace FFG {

	using namespace ShaiyaUtility;

	CMyInlineHook obj_Attack;
	CMyInlineHook g_objPartyMenu;
	CMyInlineHook g_objPartyWords;
	CMyInlineHook g_objPartyJoin;
	CMyInlineHook g_objKill;
	CMyInlineHook g_objAddkill;
	CMyInlineHook g_objMoveLimited, g_objSummonLimited;
	CMyInlineHook obj_ProcessFaction;
	CMyInlineHook g_objEnableKills;
	CMyInlineHook g_objIsCommonSkillUsable;


	WORD g_map = 45;

	bool __stdcall IsAttackAble(bool preAckable, void* Attacker, void* Target)
	{
		// 是否地图
		if (EP6::PlayerMap(Attacker) != g_map) {
			return preAckable;
		}
		// 判断行会
		return EP6::PlayerGuide(Attacker) != EP6::PlayerGuide(Target);
	}


	//从b里面拿阵营
	BYTE __stdcall GetFaction(BYTE preCountry, void* playerA, void* playerB){

		// 是否地图
		if (EP6::PlayerMap(playerA) != g_map) {
			return preCountry;
		}
		
		//
		bool isSameGuid = EP6::PlayerGuide(playerA) != EP6::PlayerGuide(playerB);
		if (isSameGuid) {
			return preCountry;
		}

		// 要知道B的阵营,拿A的反阵营出来
		auto country = EP6::PlayerCountry(playerA);
		if (country == 0) {
			return 1;
		}
		else {
			return 0;
		}

	}


	bool __stdcall IsPartyAble(bool preValue, void* Attacker, void* Target)
	{
		return !IsAttackAble(preValue, Attacker, Target);
	}



	__declspec(naked) void  Naked_Attack()
	{
		_asm {
			mov dl, byte ptr ds : [edx + 0x12D]
			cmp dl, byte ptr ds : [eax + 0x12D]
			jne _Org   //本身是不同阵营就没必要对比了
			pushad
			MYASMCALL_2(IsAttackAble,0, eax, edx)
			cmp al,0x1
			popad
			sete al
			pop esi
			retn

			_Org: 
			jmp obj_Attack.m_pRet
		}
	}




	DWORD partyWords_addr = 0x004678cd;
	DWORD partyWords_SuccessAddr = 0x004678D6;
	__declspec(naked) void  Naked_partyWords()
	{
		_asm {
			pushad
			MYASMCALL_2(IsPartyAble, ebp, edi)
			cmp al, 0x1
			popad
			je _org
			jmp partyWords_addr

			_org :
			jmp partyWords_SuccessAddr

		}
	}

	DWORD partyJoin_addr = 0x00467b91;
	DWORD partyJoinSuccessAddr = 0x00467BA2;
	__declspec(naked) void  Naked_PartyJoin()
	{
		_asm {
			pushad
			MYASMCALL_2(IsPartyAble, ebp, edi)
			cmp al, 0x1
			popad
			je _org
			jmp partyJoin_addr

			_org :
			jmp partyJoinSuccessAddr

		}
	}

	DWORD partyMenu_addr = 0x0046779d;
	DWORD partyMenu_SuccessAddr = 0x004677A7;
	__declspec(naked) void  Naked_partyMenu()
	{
		_asm {
			pushad
			MYASMCALL_2(IsPartyAble, esi, ebp)
			cmp al, 0x1
			popad
			je __org
			jmp partyMenu_addr

			__org :
			jmp partyMenu_SuccessAddr
		}
	}


	bool __stdcall MoveAble(Pshaiya50_player playerA, Pshaiya50_player playerB) {


		//只要是2个人同一地图,那么必然允许
		if (playerA->Map == playerB->Map) {
			return true;
		}

		//不是同一阵营必然不可跨地图
		if (playerB->Country != playerA->Country) {
			return false;
		}


		bool bRet = true;
		for (auto iter = g_moveCallBack.begin(); iter != g_moveCallBack.end(); iter++) {
			(*iter)(playerA, playerB, &bRet);
		}

		return bRet;
	}


	bool __stdcall enableKill(Pshaiya50_player deather, Pshaiya50_player killer) {

		bool bRet = true;
		for (auto iter = g_enablekillsCallBack.begin(); iter != g_enablekillsCallBack.end(); iter++) {
			(*iter)(deather, killer, &bRet);
		}

		return bRet;
	}


	DWORD dwfailMove = 0x00465f37;
	__declspec(naked) void  Naked_MoveLimited()
	{
		_asm
		{
			pushad
			MYASMCALL_2(MoveAble, esi, ebp)
			test al, al
			popad
			jne __Org

			jmp dwfailMove


			__Org :
			movzx edi, dx
				mov eax, edi
				jmp g_objMoveLimited.m_pRet

		}
	}



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

			MYASMCALL_2(GetFaction, esi, ebx)
			movzx ecx, al

			pop edi
			pop esi
			pop ebp
			pop esp
			pop ebx
			pop edx
			pop eax
			cmp dword ptr[ebx + 0x142c], 01
			jmp obj_ProcessFaction.m_pRet
		}
	}

	DWORD dwfailSummon = 0x0048db38;
	__declspec(naked) void  Naked_summonLimited()
	{
		_asm
		{
			pushad
			MYASMCALL_2(MoveAble, esi, ebp)
			test al, al
			popad
			jne _Org
			jmp dwfailSummon

			_Org :
			mov edx, dword ptr[ebp + 0xdc]
				jmp g_objSummonLimited.m_pRet
		}
	}


	DWORD dwFalseAddr = 0x00458cbe;
	__declspec(naked) void  Naked_enablekills()
	{
		_asm {
			pushad
			MYASMCALL_2(enableKill, edi, ebp)
			cmp al, 0x1
			popad

			je _org
			jmp dwFalseAddr

			_org :
			mov esi, dword ptr[ebp + 0x173c]
				jmp g_objEnableKills.m_pRet
		}
	}

	bool __stdcall isCommonSkillUsable(Pshaiya50_player attack, Pshaiya50_player target) {

		bool bRet = (attack->Country == target->Country);

		for (auto iter = g_commonSkillUsableCallBack.begin(); iter != g_commonSkillUsableCallBack.end(); iter++) {
			(*iter)(attack, target, &bRet);
		}

		return bRet;
	}


	__declspec(naked) void  Naked_isCommonSkillUsable()
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







	void Start()
	{
		//是否可攻击
		// 0044BE61  |.  3A90 2D010000       cmp dl,byte ptr ds:[eax+0x12D]
		obj_Attack.Hook((PVOID)0x0044BE61, Naked_Attack, 6);

		//限制组队的
		g_objPartyJoin.Hook((PVOID)0x00467b83, Naked_PartyJoin, 6);
		g_objPartyMenu.Hook(PVOID(0x0046778f), Naked_partyMenu, 6);
		g_objPartyWords.Hook(PVOID(0x004678bf), Naked_partyWords, 6);

		//见到敌人的时候拿阵营
		obj_ProcessFaction.Hook((PVOID)0x0041ff41, Naked_Faction, 7);


		//是否让用辅助技能
		g_objIsCommonSkillUsable.Hook(PVOID(0x0044bf70), Naked_isCommonSkillUsable, 6);

		//是否给人头
		g_objEnableKills.Hook((PVOID)0x004588c3, Naked_enablekills, 6);


		//不是同一阵营不可移动
		g_objMoveLimited.Hook(PVOID(0x00465eb4), Naked_MoveLimited, 5);

		//不是同一阵营不可召唤
		g_objSummonLimited.Hook(PVOID(0x0048db0a), Naked_summonLimited, 6);

	}

}