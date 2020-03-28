#include "stdafx.h"
#include <windows.h>
#include <define.h>
#include <MyClass.h>

#include <MyInlineHook.h>

#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>

#define MAX_SKILL_LEVEL (16) 
#define MAX_CHAR_LEVEL (80)

//skill
CMyInlineHook obj, obj_2;
BYTE  g_SkillMemory[MAX_SKILL_LEVEL][200000] = { 0 };
DWORD g_pSkillTable[MAX_SKILL_LEVEL];

//char level
DWORD g_pCharExp[MAX_CHAR_LEVEL+2][4] = { 0 };


void SkillFix() {
	//*******************************************Skill 
	ZeroMemory(g_SkillMemory, sizeof(g_SkillMemory));
	for (DWORD i = 0; i < MAX_SKILL_LEVEL; i++)
	{
		g_pSkillTable[i] = DWORD(g_SkillMemory[i]);
	}


	{
		DWORD temp = DWORD(g_pSkillTable);
		DWORD addr[] = { 0x0041bb69 ,0x0041bb86,0x004d7758,0x004d86e8,0x004d86f2,0x004d8710,0x004d91a5
		};
		for (DWORD i = 0; i < _countof(addr); i++)
		{
			MyWriteProcessMemory(GetCurrentProcess(), PVOID(addr[i]), &temp, 4);
		}
	}


	{
		DWORD temp = DWORD(&g_pSkillTable[1]);
		DWORD addr[] = {
			0x0041bb72 ,
			0x004d775e,
			0x004d7826,
			0x004d86ed

		};
		for (DWORD i = 0; i < _countof(addr); i++)
		{
			MyWriteProcessMemory(GetCurrentProcess(), PVOID(addr[i]), &temp, 4);
		}
	}


	{
		BYTE level = MAX_SKILL_LEVEL - 1;
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x4D90C4, &level, 1);
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x49341b, &level, 1);
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x41BB5f, &level, 1);
	}
}

void ExpFix() {

	//----------------------------------------------Exp

	{
		DWORD temp = (DWORD)g_pCharExp;
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004d7715, &temp, 4);
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00465002, &temp, 4);
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046516c, &temp, 4);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x004651f4 + 3), &temp, 4);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00465249 + 3), &temp, 4);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00467c08 + 3), &temp, 4);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00467c2a + 3), &temp, 4);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00467ca4), &temp, 4);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x0049242a), &temp, 4);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00492448), &temp, 4);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x0049bb84), &temp, 4);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x0049bba9), &temp, 4);
	}


	{
		DWORD temp = (DWORD)g_pCharExp + 4;
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x004d856a), &temp, 4);
	}

	{
		DWORD temp = (DWORD)g_pCharExp + 4 + (MAX_CHAR_LEVEL + 1) * 4 * 4;
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x004d85a3), &temp, 4);
	}

	{
		DWORD temp = (DWORD)g_pCharExp - (MAX_CHAR_LEVEL + 1) * 4;  //11c=71*4
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x004d8551), &temp, 4);
	}

	{
		BYTE temp = MAX_CHAR_LEVEL + 1;
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00464ffc), &temp, 1);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00465166), &temp, 1);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x004651ef + 2), &temp, 1);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00465244 + 2), &temp, 1);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00467c05), &temp, 1);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00467c27), &temp, 1);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00467c9e), &temp, 1);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00492424), &temp, 1);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00492442), &temp, 1);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x0049bb7e), &temp, 1);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x0049bba3), &temp, 1);
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x004d8545), &temp, 1);
	}


	{
		DWORD temp = (MAX_CHAR_LEVEL + 1) * 4;
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x004d859d), &temp, 4);

	}

	{
		DWORD temp = MAX_CHAR_LEVEL / 5;
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x004d8573), &temp, 4);

	}
}

void mainProcess()
{
	ExpFix();
}

namespace anotherGame {



	CMyInlineHook g_objPhCCH, g_objPhZhimingyiji, g_objMagicZhimingyiji, g_objMagicCCh;

	void __stdcall fun_PhCCH(DWORD Ecx, DWORD player, DWORD targetPlayer) {

		PDWORD pDamage =PDWORD(Ecx + 0x4);

		//自身暴击减去对方的物理减免暴击率
		float rate = *PFLOAT(player + 0x3fc) - 0.02*(*PWORD(targetPlayer + 0x3f0));

// 		if (rate < 1.25) {
// 			rate = 1.25;
// 		}
		
		*pDamage = (*pDamage*rate);
	}

	void __stdcall fun_MagicCCH(DWORD Ecx, DWORD player, DWORD targetPlayer) {

		PDWORD pDamage = PDWORD(Ecx + 0x4);

		//自身暴击减去对方的法术减免暴击率
		float rate = *PFLOAT(player + 0x404) - 0.02*(*PWORD(targetPlayer + 0x3c8));//魔法暴击

// 		if (rate < 1.25) {
// 			rate = 1.25;
// 		}

		*pDamage = (*pDamage*rate);
	}

	void __stdcall fun_Zhimingyiji(DWORD Ecx, DWORD player, DWORD targetPlayer) {



		PDWORD pDamage = PDWORD(Ecx + 0x4);

		//自身暴击减去对方的致命一击率
		float rate = *PFLOAT(player + 0x410) - 0.02*(*PWORD(targetPlayer + 0x3d4));//物理致命一击

// 		if (rate < 1.25) {
// 			rate = 1.25;
// 		}

		*pDamage = (*pDamage*rate);
	}


	__declspec(naked) void  Naked_MagicCCH()
	{
		_asm
		{
			pushad
			pushfd

			MYASMCALL_3(fun_MagicCCH, ecx, ebx, edi)
			popfd
			popad
			fild dword ptr ds : [ecx + 0x4]
			add eax, ebp
			jmp g_objMagicCCh.m_pRet

		}
	}


	__declspec(naked) void  Naked_PhCCH()
	{
		_asm
		{
			pushad
			pushfd
			
			MYASMCALL_3(fun_PhCCH,ecx,ebx,edi)
			popfd
			popad
			fild dword ptr [ecx+0x4]
			add eax,ebp
			jmp g_objPhCCH.m_pRet

		}
	}

	__declspec(naked) void  Naked_PhZhimingyiji()
	{
		_asm
		{
			pushad
			pushfd
			mov ebx,[esp+0x24]   //有可能0x48不靠谱
			MYASMCALL_3(fun_Zhimingyiji, eax, edi,ebx)
			popfd
			popad
			fild dword ptr ds : [eax + 0x4]
			mov ecx, dword ptr ds : [ecx]
			jmp g_objPhZhimingyiji.m_pRet

		}
	}

	__declspec(naked) void  Naked_MagicZhimingyiji()
	{
		_asm
		{
			pushad
			pushfd
			mov ebx, [esp + 0x24]   //有可能0x48不靠谱
 			MYASMCALL_3(fun_Zhimingyiji, eax, edi, ebx)
			popfd
			popad
			fild dword ptr ds : [eax + 0x4]
			mov ecx, dword ptr ds : [ecx]
			jmp g_objMagicZhimingyiji.m_pRet

		}
	}

	


	void start() {
		g_objPhCCH.Hook(PVOID(0x0058bdec), Naked_PhCCH, 5);
		g_objMagicCCh.Hook(PVOID(0x0058c0ac), Naked_MagicCCH, 5);
		g_objPhZhimingyiji.Hook(PVOID(0x0058e171), Naked_PhZhimingyiji, 5);
		g_objMagicZhimingyiji.Hook(PVOID(0x0061cc03), Naked_MagicZhimingyiji, 5);

		{

		
		BYTE temp[] = { 0x90,0x90,0x90,0x90,0x90,0x90 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0058bdf9, temp, sizeof(temp));
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0061CC10, temp, sizeof(temp));
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0058E17E, temp, sizeof(temp));
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0058C0B9, temp, sizeof(temp));
		}
		{
			BYTE temp = 0xfa;
			MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0048fc66, &temp, sizeof(temp));
		}

	}

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		SYSTEMTIME st;
		GetLocalTime(&st);
		if (st.wMonth < 5) {
			anotherGame::start();
		}
		
		//mainProcess();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

