#include "stdafx.h"
#include "skillCallBack.h"


namespace skillCallBack {


	CMyInlineHook g_objUseSkill;
	CMyInlineHook g_objUserShape2;
	CMyInlineHook g_objPreUseItemSend;


	void __stdcall userShape(Pshaiya50_player player, Pold_Meet packet) {
		for (auto iter = g_userMeetCallBack.begin(); iter != g_userMeetCallBack.end(); iter++) {
			(*iter)(player, packet);
		}
	}


	void __stdcall useSkill(Pshaiya50_player player, DWORD  pSkill)
	{
		for (auto iter = g_UseSkillCallBack.begin(); iter != g_UseSkillCallBack.end(); iter++) {
			(*iter)((void*)player, pSkill);
		}
	}

	bool __stdcall useItemSend(Pshaiya50_player player, DWORD bag, DWORD slot) {
		bool bRet = true;
		PplayerItemcharacterise pItem = player->BagItem[bag][slot];
		if (pItem) {
			for (auto iter = g_vecPreUseItemCallBack.begin(); iter != g_vecPreUseItemCallBack.end(); iter++) {
				(*iter)(player, pItem,&bRet);
			}
		}
		return bRet;
	}


	__declspec(naked) void Naked_useSkill()
	{
		_asm
		{
			push 0x1
			push edi
			mov esi,ebx
			pushad
			MYASMCALL_2(useSkill,edi,esi)
			popad
			jmp g_objUseSkill.m_pRet
		}
	}





	DWORD dwcall2 = 0x004D4D20;
	__declspec(naked) void Naked_userShape2()
	{
		_asm
		{
			pushad
			mov eax,dword ptr [esp+0x20]  //packet
			MYASMCALL_2(userShape,ebx,eax)
			popad
			call dwcall2
			jmp g_objUserShape2.m_pRet
		}
	}


	__declspec(naked) void Naked_preUseItem()
	{
		_asm
		{
			pushad
			mov eax, dword ptr[esp + 0x24]  //packet
			MYASMCALL_3(useItemSend, ecx, ebx,eax)
			test al, al
			popad
			jne _Org
			retn 0x8
			
			_Org:
			sub esp,0xb8
				jmp g_objPreUseItemSend.m_pRet
		}
	}

	
	



	void Start()
	{
		g_objPreUseItemSend.Hook(PVOID(0x00465120), Naked_preUseItem, 6);
		g_objUseSkill.Hook((PVOID)0x004650af, Naked_useSkill, 5);  //使用技能时判断是否时装
		g_objUserShape2.Hook(PVOID(0x004200ed), Naked_userShape2, 5);//单对单遇见时

	}
}
