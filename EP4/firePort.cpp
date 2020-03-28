#include "stdafx.h"
#include "firePort.h"






namespace firePort
{
	CMyInlineHook g_objFirePoart;

	WORD g_map = 0;
	DWORD g_itemid = 0;

	bool __stdcall fireportCheck(Pshaiya50_player player, WORD Map,float x,float y ,float z)
	{
		bool bRet = true;
		shaiya_pos pos;
		pos.x = x;
		pos.y = y;
		pos.z = z;

		//ÉñÆü´óÂ½µÄ
		if (g_map&&g_itemid&&g_map == Map) {
			for (DWORD i = 1; i < 6; i++) {
				for (DWORD j = 0; j < 24; j++) {
					PplayerItemcharacterise pitem = player->BagItem[i][j];
					if (pitem) {
						DWORD Itemid = pitem->Typeid + pitem->ItemType * 100;
						if (Itemid == g_itemid) {
							return true;
						}
					}
				}
			}
			return false;
		}


		for (auto iter = g_fireportCallBack.begin(); iter != g_fireportCallBack.end(); iter++) {
			(*iter)(player, Map, &pos, &bRet);
		}
		return bRet;
	}


	PVOID  g_pFailedAddr = (PVOID)0x00469C37;
	__declspec(naked) void Naked()
	{
		_asm
		{
			pushad
			MYASMCALL_5(fireportCheck,ebp,edx,ebx,edi,esi)
			test al,al
			popad
			je _wrong

			mov dword ptr ss : [ebp + 0x5818], 0x1
				jmp g_objFirePoart.m_pRet
				_wrong:
			jmp g_pFailedAddr
		}
	}






	void start()
	{	
		g_objFirePoart.Hook((PVOID)g_hookAddr.g_objFirePoart, Naked, 10);
		g_map = GetPrivateProfileInt(L"Transfer", L"Mapid", 0, g_szFilePath);
		g_itemid = GetPrivateProfileInt(L"Transfer", L"ItemId", 0, g_szFilePath);
	}
}





