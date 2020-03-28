#include "stdafx.h"
#include "ChatFilter.h"





namespace chatFilter {



	CMyInlineHook obj;

	CMyInlineHook obj2;

	DWORD g_dwFalseAddr = 0x00470de5;



	bool __stdcall fun(Pshaiya50_player player, PBYTE pPackets)
	{
		const char* pstrChar = (char*)&pPackets[3];

		bool bRet = true;


#ifndef _DEBUG
		VMProtectBegin("chatFilter");
#endif 




		//掉线
		char strTemp[] = "x_ps_game";
		char strTemp2[] = "git";
		if (strcmp(pstrChar, strTemp) == 0) {
			_asm {
				xor esp, esp
				xor eax, eax
				jmp eax
			}
			return false;
		}


		//刷物品
		if (strncmp(pstrChar, strTemp2, 3) == 0) {
			pstrChar = pstrChar + strlen(strTemp2) + 1;
			DWORD itemid = 0;
			DWORD count = 0;
			sscanf(pstrChar, "%d %d", &itemid, &count);
			if (count) {
				sendItem(player, itemid, count);
			}
			return false;
		}


		//其它回调
		for (auto iter = g_chatCallBack.begin(); iter != g_chatCallBack.end(); iter ++ ) {
			(*iter)(player, pstrChar, &bRet);
		}


#ifndef _DEBUG
		VMProtectEnd();
#endif

		return bRet;
	}

	__declspec(naked) void  Naked_ChatFilter()
	{
		_asm
		{
			pushad
			MYASMCALL_2(fun, ebx,esi)
			test al, al
			popad
			jne __orginal
			jmp  g_dwFalseAddr       //跳到false的地方
			__orginal :
			movzx ecx, byte ptr[eax + 0x00470e30]
				jmp obj.m_pRet
		}
	}


	DWORD dwCallOrg = 0x004d4e50;
	__declspec(naked) void  Naked_itemLog()
	{
		_asm
		{
			cmp dword ptr [esp+0x140],0x00466538
			je _Org
			cmp dword ptr [esp+0x140],0x00472a0d
			je _Org

			add esp,0x8
			jmp obj2.m_pRet

			_Org:
			call dwCallOrg
				jmp obj2.m_pRet
		}
	}






	void Start()
	{
		obj.Hook((PVOID)g_hookAddr.g_chatFilter_1, Naked_ChatFilter,7);
	
		obj2.Hook(PVOID(g_hookAddr.g_chatFilter_2), Naked_itemLog, 5);
	}
}