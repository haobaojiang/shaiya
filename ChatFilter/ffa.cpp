
#include "stdafx.h"
#include "ffa.h"







namespace FFA
{
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




	bool __stdcall isSameFaction(Pshaiya50_player Attacker, Pshaiya50_player Target)
	{
		bool bRet =( Attacker->Country != Target->Country ? false : true);
		for (auto iter = g_factionCallBack.begin(); iter != g_factionCallBack.end(); iter++) {
			(*iter)(Attacker, Target, &bRet);
		}
		return bRet;
	}



	void __stdcall addKills(Pshaiya50_player killer, Pshaiya50_player deather)
	{
		for (auto iter = g_addkillsCallBack.begin(); iter != g_addkillsCallBack.end(); iter++) {
			(*iter)(killer, deather);
		}
	}





	void __stdcall killLog(Pshaiya50_player killer, Pshaiya50_player deather)
	{
		for (auto iter = g_killedCallBack.begin(); iter != g_killedCallBack.end(); iter++) {
			(*iter)(killer, deather);
		}
	}


	//从b里面拿阵营
	BYTE __stdcall GetFaction(Pshaiya50_player playerA, Pshaiya50_player playerB)
	{
		BYTE bRet=playerB->Country;
		for (auto iter = g_getfactionCallBack.begin(); iter != g_getfactionCallBack.end(); iter++) {
			(*iter)(playerA, playerB, &bRet);
		}
		return bRet;
	}


	bool __stdcall IsPartyAble(Pshaiya50_player srcPlayer, Pshaiya50_player dstPlayer)
	{
		bool bRet = (srcPlayer->Country == dstPlayer->Country);

		for (auto iter = g_partyAbleCallBack.begin(); iter != g_partyAbleCallBack.end(); iter++) {
			(*iter)(srcPlayer, dstPlayer, &bRet);
		}
		return bRet;
	}



	__declspec(naked) void  Naked_Attack()
	{
		_asm {
			pushad
			MYASMCALL_2(isSameFaction, eax, edx)
			cmp al, 0x1             //把结果存放在zf标志位里先
			popad
			sete al
			pop esi
			retn
		}
	}


	

__declspec(naked) void  Naked_addKill()
	{
		_asm {
			pushad
			MYASMCALL_2(addKills, ecx, edx)
			popad
			sub esp,0x9c
			jmp g_objAddkill.m_pRet
		}
	}

	PVOID g_ShareKillsTempCall = (PVOID)0x0045b050;
	__declspec(naked) void  Naked_killLog()
	{
		_asm {
			pushad
			MYASMCALL_2(killLog,ecx,edi)
			popad
			call g_ShareKillsTempCall
			jmp g_objKill.m_pRet
		}
	}

	DWORD partyWords_addr = 0x004678cd;
	DWORD partyWords_SuccessAddr = 0x004678D6;
	__declspec(naked) void  Naked_partyWords()
	{
		_asm {
		pushad
		MYASMCALL_2(IsPartyAble,ebp,edi)
		cmp al,0x1
		popad
		je _org
		jmp partyWords_addr

		_org:
			jmp partyWords_SuccessAddr
		
		}
	}
	
	DWORD partyJoin_addr = 0x00467b91;
	DWORD partyJoinSuccessAddr = 0x00467BA2;
	__declspec(naked) void  Naked_PartyJoin()
	{
		_asm {
			pushad
			MYASMCALL_2(IsPartyAble,ebp,edi)
			cmp al,0x1
			popad
			je _org
			jmp partyJoin_addr

			_org:
				jmp partyJoinSuccessAddr

		}
	}

	DWORD partyMenu_addr = 0x0046779d;
	DWORD partyMenu_SuccessAddr = 0x004677A7;
	__declspec(naked) void  Naked_partyMenu()
	{
		_asm {
			pushad
			MYASMCALL_2(IsPartyAble,esi,ebp)
			cmp al,0x1
			popad
			je __org
			jmp partyMenu_addr

			__org:
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
			(*iter)(playerA, playerB,&bRet);
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
			MYASMCALL_2(MoveAble,esi,ebp)
			test al,al
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

MYASMCALL_2(GetFaction,esi,ebx)
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
			MYASMCALL_2(MoveAble,esi,ebp)
			test al,al
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

	bool __stdcall isCommonSkillUsable(Pshaiya50_player attack,Pshaiya50_player target) {

		bool bRet = (attack->Country == target->Country);

		for (auto iter = g_commonSkillUsableCallBack.begin(); iter != g_commonSkillUsableCallBack.end(); iter++) {
			(*iter)(attack,target,&bRet);
		}

		return bRet;
	}


	__declspec(naked) void  Naked_isCommonSkillUsable()
	{
		_asm {
			pushad
			MYASMCALL_2(isCommonSkillUsable, esi, edx)
			cmp al,0x1
			popad
			sete al
			retn 
		}
	}

	





	void Start()
	{
		//是否可攻击
		obj_Attack.Hook((PVOID)0x0044be5b, Naked_Attack, 6);

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


		g_objKill.Hook((PVOID)0x00458c78, Naked_killLog, 5);        //绝对人头的
		g_objAddkill.Hook(PVOID(0x0045aed0), Naked_addKill, 6);
		
		//不是同一阵营不可移动
		g_objMoveLimited.Hook(PVOID(0x00465eb4), Naked_MoveLimited, 5);

		//不是同一阵营不可召唤
		g_objSummonLimited.Hook(PVOID(0x0048db0a), Naked_summonLimited, 6);

	}
}





