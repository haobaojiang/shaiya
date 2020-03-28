#include "stdafx.h"
#include "ipRestriction.h"






namespace ipRestriction
{
	CMyInlineHook g_obj;

	WORD g_lightMap = 0;
	WORD g_furyMap = 0;

	vector<Pshaiya50_player> g_vecIllegalPlayers;
	CRITICAL_SECTION g_cs;


	DWORD g_whiteUid[200] = { 0 };


	bool isRestrictedMap(WORD Map) {
		return (g_lightMap == Map || g_furyMap == Map);
	}
	


	void ipRestrictionforMap(){
		for (DWORD i = 0; i < _countof(g_players); i++) {
			DWORD player = g_players[i];

			if (!player)
				break;

			if (!*PDWORD(player + 0xe0))
				continue;

			WORD  Map = *PWORD(player + 0x164);
			if (!isRestrictedMap(Map))
				continue;

			DWORD ip = *PDWORD(player + 0x70);

			for (DWORD j = i + 1; j < _countof(g_players); j++) {

				DWORD player2 = g_players[j];
				if (!player2)
					break;

				if (!*PDWORD(player2 + 0xe0))
					continue;

				WORD  Map2 = *PWORD(player2 + 0x164);
				DWORD ip2 = *PDWORD(player2 + 0x70);

				if (ip2 == ip&&isRestrictedMap(Map2)) {
					MovePlayer((DWORD)player2, 42, 50.0, 50.0);
				}
			}
		}
	}


	void ipRestrictionforfaction() {
		EnterCriticalSection(&g_cs);
		if (g_vecIllegalPlayers.size()) {
			DisConnectPlayer(g_vecIllegalPlayers[0]);
			g_vecIllegalPlayers.erase(g_vecIllegalPlayers.begin());
		}
		LeaveCriticalSection(&g_cs);
	}

	void loop()
	{
		ipRestrictionforMap();
		ipRestrictionforfaction();
	}


	bool isInwhitelist(Pshaiya50_player player) {
		for (DWORD i = 0; i < _countof(g_whiteUid); i++) {

			if (!g_whiteUid[i])
				break;

			if (player->uid == g_whiteUid[i]) {
				return true;
			}
		}
		return false;
	}
	



	void __stdcall fun(Pshaiya50_player p1) {


		if (isInwhitelist(p1)) {
			return;
		}


		for (DWORD i = 0; i <_countof(g_players);i++) {
			Pshaiya50_player p2 = Pshaiya50_player(g_players[i]);

			if (!p2) {
				break;
			}

			if (p1 == p2) {
				continue;
			}
		


			if (p2->nSocket == INVALID_SOCKET) {
				continue;
			}

		


			if (p2->ip == p1->ip) {
				if (p2->Country != p1->Country) {
					EnterCriticalSection(&g_cs);
					g_vecIllegalPlayers.push_back(p1);
					LeaveCriticalSection(&g_cs);
				}
				break;
			}

		
		}
	}


	__declspec(naked) void  Naked()
	{
		_asm {
			mov byte ptr [ebp+0x12d],al
			pushad
			MYASMCALL_1(fun,ebp)
			popad
			jmp g_obj.m_pRet
		}
	}
	




	void Start()
	{
		InitializeCriticalSection(&g_cs);
		g_obj.Hook(PVOID(0x0046cfb5), Naked, 6);


		g_lightMap = GetPrivateProfileInt(L"ipRestriction", L"LightMap", 0, g_szFilePath);
		g_furyMap = GetPrivateProfileInt(L"ipRestriction", L"FuryMap", 0, g_szFilePath);
		g_vecLoopFunction.push_back(loop);


		for (DWORD i = 0; i < 200; i++) {
			WCHAR key[50] = { 0 };
			swprintf(key, L"%d", i + 1);
			g_whiteUid[i] = GetPrivateProfileInt(L"whitelist", key, 0, g_szFilePath);
		}


	}
}