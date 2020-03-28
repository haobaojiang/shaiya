#include <windows.h>
#include <MyInlineHook.h>
#include <vector>
#include <common.h>
#include "recrune.h"


namespace RecRune
{
	CMyInlineHook g_objIsEnable;
	CMyInlineHook g_objIsRecreate1, g_objIsRecreate2;
	bool g_IsEnable = false;
	CRITICAL_SECTION g_cs;
	BYTE g_Index = 0;




	void __stdcall fun_IsEnable(PITEMINFO recRune, PplayerItemcharacterise gear)
	{
		EnterCriticalSection(&g_cs);
		switch (recRune->itemid)
		{
		case 101001:
			g_Index = en_str;
			break;
		case 101002:
			g_Index = en_dex;
			break;
		case 101003:
			g_Index = en_int;
			break;
		case 101004:
			g_Index = en_wis;
			break;
		case 101005:
			g_Index = en_rec;
			break;
		case 101006:
			g_Index = en_luc;
			break;
		default:
			return ;
		}

		g_IsEnable = true;
		char temp[3] = { 0 };
		memcpy(temp, &(gear->craftname[g_Index * 2]), 2);
		DWORD stat=atoi(temp)+10;
		WORD reqwis = gear->ItemAddr->ReqWis;

		if (stat > reqwis) {
			stat = reqwis;
		}


		sprintf(temp, "%02d",stat);

		memcpy(&(gear->craftname[g_Index * 2]),temp , 2);
	}



	void __stdcall Recreate(PplayerItemcharacterise pItem)
	{
		DWORD dwRecreateCall = 0x004D29C0;
		if (!g_IsEnable) 
		{
			_asm 
			{
				push 0x1
				mov esi,pItem
				call dwRecreateCall
			}
		}
		g_IsEnable = false;
		LeaveCriticalSection(&g_cs);

	}
	


	__declspec(naked) void Naked_IsEnable()
	{
		_asm
		{
			lea edx, dword ptr ss : [esp + 0x130]
			pushad
			push ebp  //gear
			push ecx  //rec rune
			call g_objIsEnable.m_pfun_call
			popad
			jmp g_objIsEnable.m_pRet
		}
	}

	__declspec(naked) void Naked_IsRecreate()
	{
		_asm
		{
			pushad
			MYASMCALL_1(Recreate,esi)
			popad
			retn 0x4
		}
	}

	


	void start()
	{
		InitializeCriticalSection(&g_cs);
		g_objIsEnable.Hook(PVOID(0x0046d6c0), Naked_IsEnable, 7, fun_IsEnable);
		g_objIsRecreate1.Hook(PVOID(0x0046d722), Naked_IsRecreate, 5,0,1);
		g_objIsRecreate1.Hook(PVOID(0x0046d779), Naked_IsRecreate, 5,0,1);

	}
}





