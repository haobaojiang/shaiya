#include "stdafx.h"
#include "levelReward.h"




namespace levelReward
{
	DWORD g_Items[MAXWORD] = { 0 };
	DWORD g_Money[MAXWORD] = { 0 };


	CMyInlineHook g_obj;

	void __stdcall fun(Pshaiya_player player) {
		DWORD Index = player->Level - 1;
		DWORD item =  g_Items[Index];
		DWORD money = g_Money[Index];
		if (item) {
			sendItem(player, item);
		}
		if (money) {
			AddMoney(player, money);
		}
	}


	__declspec(naked) void Naked()
	{
		_asm {
			mov ecx, dword ptr ss : [ebp + 0x582C]
			pushad
			MYASMCALL_1(fun,ebp)
			popad
			jmp g_obj.m_pRet
		}
	}

	void start()
	{
		WCHAR appname[] = L"levelReward";
		for (DWORD i = 0; i < MAXWORD;i++) {
			WCHAR temp1[50] = { 0 };
			WCHAR temp2[50] = { 0 };
			swprintf(temp1, L"level_%d_item", i+1);
			swprintf(temp2, L"level_%d_money", i +1);
			g_Items[i] = GetPrivateProfileInt(appname, temp1, 0, g_szFilePath);
			g_Money[i] = GetPrivateProfileInt(appname, temp2, 0, g_szFilePath);
		}


		g_obj.Hook(PVOID(0x0049ba12), Naked, 6);

	}
}





