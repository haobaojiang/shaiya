#include "stdafx.h"
#include "PreventItemDrop.h"

vector<DWORD> g_vecUnDropAbleItems;


namespace preventItemDrp {

	CMyInlineHook g_objDrop, g_objTrade, g_objItemToguildBank, g_objItemToBank;

	bool __stdcall DragAble(Pshaiya50_player player,DWORD bag,DWORD slot) {
		PplayerItemcharacterise pItem = player->BagItem[bag][slot];
		if (pItem) {
			return pItem->ItemAddr->ReqOg ? false : true;
		}
		return true;
	}


	__declspec(naked) void  Naked()
	{
		_asm
		{
			pushad
			mov eax, [esp + 0x24]
			mov ebx, [esp + 0x28]
			MYASMCALL_3(DragAble, ecx, eax, ebx)
			test al, al
			popad
			jne _Org
			retn 0xc
			_Org:
			sub esp, 0xb4
				jmp g_objDrop.m_pRet
		}
	}


	DWORD dwTradeFalseAddr = 0x0046F2DA;
	__declspec(naked) void  Naked_Trade()
	{
		_asm
		{
			pushad
			MYASMCALL_3(DragAble,ebx,eax,esi)
			test al,al
			popad
			jne _Org
			jmp dwTradeFalseAddr
			_Org:
			 lea eax,dword ptr [eax+eax*2]
			lea eax,dword ptr [esi+eax*8]
			 jmp g_objTrade.m_pRet
		}
	}


	__declspec(naked) void  Naked_ItemToGuildBank()
	{
		_asm
		{
			pushad
			mov eax,[esp+0x28]
			mov ebx,[esp+0x2c]
			MYASMCALL_3(DragAble, ecx, eax, ebx)
			test al, al
			popad
			jne _Org
			xor eax,eax
			retn 0x14
			_Org :
			sub esp,0x208
				jmp g_objItemToguildBank.m_pRet
		}
	}

	__declspec(naked) void  Naked_ItemToBank()
	{
		_asm
		{
			pushad
			mov eax, [esp + 0x24]
			mov ebx, [esp + 0x28]
			MYASMCALL_3(DragAble, edx, eax, ebx)
			test al, al
			popad
			jne _Org
			xor eax, eax
			retn 0xc
			_Org :
			sub esp, 0x198
				jmp g_objItemToBank.m_pRet
		}
	}


	void Start()
	{
		g_objDrop.Hook((PVOID)0x0045cd10, Naked, 6);
		g_objTrade.Hook((PVOID)0x0046f025, Naked_Trade, 6);
		g_objItemToguildBank.Hook(PVOID(0x0042f390), Naked_ItemToGuildBank, 6);
	//	g_objItemToBank.Hook(PVOID(0x0045c4e0), Naked_ItemToBank, 6);
	}
}

