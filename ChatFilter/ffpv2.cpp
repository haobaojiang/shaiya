
#include "stdafx.h"
#include "ffpv2.h"







namespace FFPV2
{
	WORD g_map = 0;
	CMyInlineHook g_objcompletedParty, g_objcompletedRaid, g_objQuitRaid, g_objQuitParty, g_objKickfromParty;
	vector<Pshaiya50_player> g_shapePlayers;
	CRITICAL_SECTION g_cs;
	


	void fun_sendshape(void* p) {
		while (1)
		{
			EnterCriticalSection(&g_cs);
			if (g_shapePlayers.size()) {
				Pshaiya50_player player = g_shapePlayers[0];
				Pshaiya_party party = player->party;
				if (party) {
					for (DWORD i = 0; i < party->Count; i++) {
						Pshaiya50_player teammate = Pshaiya50_player(party->member[i].player);
						if (teammate&&teammate->Map == player->Map&&teammate != player) {
							SendUserShape(teammate, player->Charid);
							SendUserShape(player, teammate->Charid);
						}
					}
				}
				g_shapePlayers.erase(g_shapePlayers.begin());
			}
			LeaveCriticalSection(&g_cs);

			Sleep(1000);
		}
	}





	//attack µÄÊ±ºò
	void __stdcall isSameFaction(Pshaiya50_player Attacker, Pshaiya50_player Target, bool* pRet)
	{
		if (Attacker->Map == g_map) {

			if (Attacker->party&&Target->party==Attacker->party) {
				*pRet = 1;
			}
			else {
				*pRet = 0;
			}
		}
	}


	void __stdcall GetFaction(Pshaiya50_player playerA, Pshaiya50_player playerB,BYTE* pCountry)
	{
		if (playerA->Map==g_map) {
			if (playerA->party&&playerA->party == playerB->party) {
				*pCountry=playerA->Country;
			}
			else {
				*pCountry=playerA->Country ? 0 : 1;
			}
		}
	}


	void __stdcall IsPartyAble(Pshaiya50_player srcPlayer, Pshaiya50_player dstPlayer,bool* pRet)
	{
		if (srcPlayer->Map == g_map&&
			dstPlayer->Map == g_map) {
			*pRet = true;
		}
	}


	void addshapeuser(Pshaiya50_player player) {
		EnterCriticalSection(&g_cs);

		g_shapePlayers.push_back(player);

		LeaveCriticalSection(&g_cs);

	}


	void __stdcall completedParty(Pshaiya50_player player) {
		
		if (player->Map != g_map) {
			return;
		}
		
		addshapeuser(player);
	}


	__declspec(naked) void  Naked_completedraid()
	{
		_asm
		{
			pushad
			MYASMCALL_1(completedParty,ebp)
			popad
			mov dword ptr [esp+0x8a],edx
			jmp g_objcompletedRaid.m_pRet
		}
	}

	__declspec(naked) void  Naked_completedParty()
	{
		_asm
		{
			pushad
			MYASMCALL_1(completedParty, ebp)
			popad
			mov dword ptr[esp + 0x82], eax
			jmp g_objcompletedParty.m_pRet
		}
	}


	void __stdcall QuitParty(Pshaiya50_player player,Pshaiya_party party) {
		if (player->Map == g_map) {
			player->party = NULL;

			for (DWORD i = 0; i < party->Count; i++) {
				Pshaiya50_player teammate = Pshaiya50_player(party->member[i].player);
				if (teammate&&teammate->Map == player->Map&&teammate != player) {
					SendUserShape(teammate, player->Charid);
					SendUserShape(player, teammate->Charid);
				}
			}
		}
	}


	__declspec(naked) void  Naked_QuitParty()
	{
		_asm
		{
			pushad
			MYASMCALL_2(QuitParty,edi,ebp)
			popad
			mov eax,dword ptr [edi+0xdc]
			jmp g_objQuitParty.m_pRet
		}
	}


	__declspec(naked) void  Naked_QuitRaid()
	{
		_asm
		{
			pushad
			MYASMCALL_2(QuitParty, eax, ebp)
			popad
			mov ecx,dword ptr [eax+0xdc]
			jmp g_objQuitRaid.m_pRet
		}
	}

	
	__declspec(naked) void  Naked_kickfromParty()
	{
		_asm
		{
			pushad
			MYASMCALL_2(QuitParty, edi, ebp)
			popad
			cmp byte ptr [ebp+0x114],0x1

			jmp g_objKickfromParty.m_pRet
		}
	}


	void Start() {
		
		g_map = GetPrivateProfileInt(L"ffpv2", L"map", 0, g_szFilePath);

		if (g_map) {
			InitializeCriticalSection(&g_cs);
			LOGD << "ffpv2 config loaded,map:" << g_map;
			g_factionCallBack.push_back(isSameFaction);
			g_getfactionCallBack.push_back(GetFaction);
			g_partyAbleCallBack.push_back(IsPartyAble);

			g_objcompletedRaid.Hook(PVOID(0x00445fb4), Naked_completedraid, 7);
			g_objcompletedParty.Hook(PVOID(0x00446449), Naked_completedParty, 7);


			g_objQuitParty.Hook(PVOID(0x004469cd), Naked_QuitParty, 6);
			g_objQuitRaid.Hook(PVOID(0x004465b2), Naked_QuitRaid, 6);

			g_objKickfromParty.Hook(PVOID(0x0044722c), Naked_kickfromParty, 7);

			_beginthread(fun_sendshape, 0, 0);
		}

	}
}





