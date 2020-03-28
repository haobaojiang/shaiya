#include "stdafx.h"
#include "ffa.h"





#define _SHAIYA_YING 1
#ifdef _SHAIYA_YING
extern	vector<PARTYKILLRECORD> g_vecYingParty;
extern	CRITICAL_SECTION g_csParty;
#endif




namespace FFA
{
	CMyInlineHook obj_Attack;
	CMyInlineHook g_objPartyMenu;
	CMyInlineHook g_objPartyWords;
	CMyInlineHook g_objPartyJoin;
	CMyInlineHook g_objKill;

	CMyInlineHook obj_ProcessFaction;

	WORD g_ffaMap = 0;
	bool g_isPartyFightonly = false;





	bool __stdcall isSameFaction(Pshaiya_player Attacker, Pshaiya_player Target)
	{
		bool bRet = (Attacker->Country != Target->Country ? false : true); //原语句,阵营不相等则

		if (Attacker->Map == g_ffaMap && Attacker->party && Target->party)
		{
			if (g_isPartyFightonly){
				bRet = (Attacker->party != Target->party ? false : true);      
			}else {
				bRet = true;
			}
		}
		return bRet;
	}


	__declspec(naked) void  Naked_Attack()
	{
		_asm {
			pushad
			MYASMCALL_2(isSameFaction,eax,edx)
			cmp al, 0x1             //把结果存放在zf标志位里先
			popad
			sete al
			pop esi
			retn
		}
	}



	void sendPlayerMessage(Pshaiya_player player,const char* strMessage) {
		PLAYERMESSSAGE stcMsg;
		strcpy(stcMsg.strMessage, strMessage);
		send_packet_specific_area(player, &stcMsg, sizeof(stcMsg));
	}



	void __stdcall killLog(Pshaiya_player killer, Pshaiya_player deather)
	{
#ifdef _SHAIYA_YING
		if (killer->Map == g_ffaMap&&
			killer->party)
		{	
			EnterCriticalSection(&g_csParty);    
			for (auto iter = g_vecYingParty.begin(); iter != g_vecYingParty.end(); iter++) {
				if (iter->pParty == killer->party) {
					char strMessage[100] = { 0 };
					sprintf(strMessage, "%s,has killed:%d", iter->strLeaderName, ++iter->kills);
					sendPlayerMessage(killer, strMessage);
					break;
				}
			}
			LeaveCriticalSection(&g_csParty);
		}
#endif
	}


	//从b里面拿阵营
	BYTE __stdcall GetFaction(Pshaiya_player playerA, Pshaiya_player playerB)
	{
		BYTE bRet = playerB->Country;
		if (playerB->Map == g_ffaMap &&
			playerB->party != playerA->party&&
			g_ffaMap) //如果地图相等但是不在同一个队伍
		{
			bRet = ~(playerA->Country);      //取反阵营
		}
		return bRet;
	}


	bool __stdcall fun_IsPartyAble(Pshaiya_player srcPlayer, Pshaiya_player dstPlayer)
	{
		bool bRet = true;

#ifdef _SHAIYA_YING
		if (srcPlayer->Map == g_ffaMap) {
			bRet = false;
			Pshaiya_party party = dstPlayer->party;
			if (party) {
				for (auto iter = g_vecYingParty.begin(); iter != g_vecYingParty.end(); iter++) {
					if (iter->pParty == party) {
						for (DWORD i = 0; i < iter->copyParty.Count; i++) {
							if (srcPlayer == iter->copyParty.member[i].player) {
								bRet = true;
								break;
							}
						}
						break;
					}
				}
			}
		}
#endif
		return bRet;
	}


	PVOID g_ShareKillsTempCall = (PVOID)0x00467EE0;
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

	DWORD partyWords_addr = 0x00475505;
	__declspec(naked) void  Naked_partyWords()
	{
		_asm {
		pushad
		MYASMCALL_2(fun_IsPartyAble,ebp,edi)
		cmp al,0x1
		popad
		je _org
		jmp partyWords_addr

		_org:
		mov cl, byte ptr ds : [edi + 0x12D]
			jmp g_objPartyWords.m_pRet
		
		}
	}
	
	DWORD partyJoin_addr = 0x004757c8;
	__declspec(naked) void  Naked_PartyJoin()
	{
		_asm {
			pushad
			MYASMCALL_2(fun_IsPartyAble,ebp,edi)
			cmp al,0x1
			popad
			je _org
			jmp partyJoin_addr

			_org:
			mov dl, byte ptr ss : [ebp + 0x12D]
				jmp g_objPartyJoin.m_pRet

		}
	}

	DWORD partyMenu_addr = 0x004753a7;
	__declspec(naked) void  Naked_partyMenu()
	{
		_asm {
			pushad
			MYASMCALL_2(fun_IsPartyAble,esi,ebp)
			cmp al,0x1
			popad
			je __org
			jmp partyMenu_addr

			__org:
			mov dl, byte ptr ds : [esi + 0x12D]
			jmp g_objPartyMenu.m_pRet
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

MYASMCALL_2(GetFaction,ebp,ebx)
			movzx ecx, al

			pop edi
			pop esi
			pop ebp
			pop esp
			pop ebx
			pop edx
			pop eax
			cmp dword ptr[ebx + 00001444], 01
			jmp obj_ProcessFaction.m_pRet
		}
	}




	void Start()
	{
		obj_Attack.Hook((PVOID)0x00457e27, Naked_Attack, 6);
		WCHAR* appName = L"ffa";
		g_isPartyFightonly = GetPrivateProfileInt(appName, L"partyRequire", 0, g_szFilePath);
		g_ffaMap = GetPrivateProfileInt(appName, L"map", 0, g_szFilePath);

		g_objPartyJoin.Hook((PVOID)0x004757ba, Naked_PartyJoin, 6);
		g_objPartyMenu.Hook(PVOID(0x00475396), Naked_partyMenu, 6);
		g_objPartyWords.Hook(PVOID(0x004754f7), Naked_partyWords, 6);
		obj_ProcessFaction.Hook((PVOID)0x00426af7, Naked_Faction, 7);


#ifdef _SHAIYA_YING
		g_objKill.Hook((PVOID)0x00465aad, Naked_killLog, 5);
#endif
	}
}





