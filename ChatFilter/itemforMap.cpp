
#include "stdafx.h"
#include "itemforMap.h"



namespace itemForMap
{
	CMyInlineHook g_obj;

	WORD g_map = 0;
	DWORD g_item = 0;
	DWORD g_itemCount = 1;


	void __stdcall fun(Pshaiya50_player player, WORD Map, Pshaiya_pos pos,bool* pRet)
	{
		if (player->Map == Map)
			goto _suc;

		if (Map != g_map)
			goto _suc;

		if (!player->party) {
			goto _fail;
		}


		//是否有队友已经在里面了
		for (DWORD i = 0; i < player->party->Count; i++) {
			Pshaiya50_player playerB = Pshaiya50_player(player->party->member[i].player);
			if (playerB&&playerB->Map== Map) {
				goto _suc;
			}
		}
		
	
		if (DeleteItemByItemID(player, g_item, g_itemCount)) {
			goto _suc;
		}else {
			goto _fail;
		}

	_suc:
		return;

	_fail:
		*pRet = true;
		return;
	}







	void start()
	{	
		g_fireportCallBack.push_back(fun);

		g_item = GetPrivateProfileInt(L"itemformap", L"item", 0, g_szFilePath);
		g_itemCount=GetPrivateProfileInt(L"itemformap", L"count", 1, g_szFilePath);
		g_map = GetPrivateProfileInt(L"itemformap", L"map", 0, g_szFilePath);

		
	}
}





