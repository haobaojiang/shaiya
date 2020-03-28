#include "stdafx.h"
#include "vipCrossTrade.h"




namespace vipCrossTrade
{
	CMyInlineHook g_obj;
	vector<DWORD> g_vecPlayers;



	bool __stdcall isVip(Pshaiya50_player player) {
		for (auto iter = g_vecPlayers.begin(); iter != g_vecPlayers.end(); iter++) {
			if ((*iter) == player->uid) {
				return true;
			}
		}
		return false;
	}


	DWORD addr1 = 0x00420e54;
	__declspec(naked) void  Naked_1()
	{
		_asm
		{
			pushad
			MYASMCALL_1(isVip,ecx)
			test al,al
			popad
			je _Org
			jmp addr1

			_Org:
			cmp dl,byte ptr [ecx+0x12d]
			jmp g_obj.m_pRet

		}
	}



	void Start()
	{
		for (DWORD i = 0; i < 200; i++) {
			WCHAR key[50] = { 0 };
			swprintf(key, L"%d", i + 1);
			DWORD uid = GetPrivateProfileInt(L"vipCrossTradeChannel", key, 0, g_szFilePath);
			if (!uid) {
				break;
			}
			g_vecPlayers.push_back(uid);
		}

		if (g_vecPlayers.size()) {
			g_obj.Hook((PVOID)0x00420e48, Naked_1, 6);
		}
	}
}




