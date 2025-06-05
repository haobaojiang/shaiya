#include "stdafx.h"
#include "config.hpp"



namespace RecRune
{

	typedef struct singleStateRune {
		DWORD itemId{};
		BOOL is_double{};
		BYTE index; // from 0 - 8
	};

	std::vector<singleStateRune> runes;



	CRITICAL_SECTION g_cs;
	bool g_IsEnable = false;
	ShaiyaUtility::CMyInlineHook g_objIsEnable;
	ShaiyaUtility::CMyInlineHook g_objSetSignature;
	ShaiyaUtility::CMyInlineHook g_objSetStats;

	ShaiyaUtility::CMyInlineHook g_hookReqWis1;
	ShaiyaUtility::CMyInlineHook g_hookReqWis2;
	ShaiyaUtility::CMyInlineHook g_hookReqWis3;

	char g_craftname[20]{};
	singleStateRune g_active_rune;


	void __stdcall fun_setSignature(PBYTE pSignature,DWORD gear)
	{
		if (!g_IsEnable)
			return;

		PWORD pstats = PWORD(gear + 0x4c);
		memcpy(g_craftname, pstats, 20);
		ZeroMemory(pSignature, 9);
		pSignature[g_active_rune.index] = 0xff;
	}

	bool __stdcall fun_IsEnable(DWORD recRune, DWORD gear)
	{
		EnterCriticalSection(&g_cs);
		auto itemid = *reinterpret_cast<PDWORD>(recRune);
		PWORD pstats = PWORD(gear + 0x4c);
		for (const auto& rune : runes) {
			if (rune.itemId == itemid) {
				if (pstats[rune.index] == 0x3030) {
					LeaveCriticalSection(&g_cs);
					return false;
				}
				g_IsEnable = true;
				g_active_rune = rune;
				break;
			}
		}
		return true;
	}

	void __stdcall fun_setStats(DWORD gear)
	{

		do
		{
			if (!g_IsEnable)
				break;

			g_IsEnable = false;
			PWORD pstats = PWORD(gear + 0x4c);
			DWORD gearAddr = *PDWORD(gear + 0x30);
			auto stat = pstats[g_active_rune.index];
			memcpy(pstats, g_craftname, 20);
			if (stat == 0x3030) {
				stat = 0x3130;
			}
			pstats[g_active_rune.index] = stat;


		} while (0);

		LeaveCriticalSection(&g_cs);
	}

	DWORD __stdcall get_reqwis(DWORD orgValue) {
		if (!g_IsEnable) {
			return orgValue;
		}
		if (g_active_rune.is_double) {
			orgValue *= 2;
			if (orgValue > 99) {
				orgValue = 99;
			}
		}
		return orgValue;
	}




	__declspec(naked) void Naked_SetSignature()
	{
		_asm
		{
			lea ebp, ds: [esi + 0x84]
			lea ecx, ds: [ecx]
			cmp dword ptr[esp + 0x28], 0x0046d727
			je _fun
			cmp dword ptr[esp + 0x28], 0x0046d77e
			je _fun
			jmp _org

			_fun :
			pushad
				lea eax, dword ptr[esp + 0x2c]
				MYASMCALL_2(fun_setSignature,eax,esi)
				popad

				_org :
			jmp g_objSetSignature.m_pRet

		}
	}



	__declspec(naked) void Naked_set_req_wis_1()
	{
		_asm
		{
			mov edx, dword ptr ds : [esi + 0x30]
			movzx ecx, word ptr ds : [edx + 0x3E]
			push eax
			push ebx
			push edx
			push edi
			push esi
			push ebp
			push esp
			MYASMCALL_1(get_reqwis,ecx)
			mov ecx,eax
			pop esp 
			pop ebp
			pop esi
			pop edi
			pop edx
			pop ebx
			pop eax
			jmp g_hookReqWis1.m_pRet
		}
	}

	__declspec(naked) void Naked_set_req_wis_2()
	{
		_asm
		{
			mov edx, dword ptr ds : [esi + 0x30]
			movzx ecx, word ptr ds : [edx + 0x3E]
			push eax
			push ebx
			push edx
			push edi
			push esi
			push ebp
			push esp
			MYASMCALL_1(get_reqwis, ecx)
			mov ecx, eax
			pop esp
			pop ebp
			pop esi
			pop edi
			pop edx
			pop ebx
			pop eax
			jmp g_hookReqWis2.m_pRet
		}
	}

	__declspec(naked) void Naked_set_req_wis_3()
	{
		_asm
		{
			mov edx, dword ptr ds : [esi + 0x30]
			movzx ecx, word ptr ds : [edx + 0x3E]
			push eax
			push ebx
			push edx
			push edi
			push esi
			push ebp
			push esp
			MYASMCALL_1(get_reqwis, ecx)
			mov ecx, eax
			pop esp
			pop ebp
			pop esi
			pop edi
			pop edx
			pop ebx
			pop eax
			jmp g_hookReqWis3.m_pRet
		}
	}


	__declspec(naked) void Naked_setStats()
	{
		_asm
		{
			cmp dword ptr[esp + 0x1c], 0x0046d727
			je _fun
			cmp dword ptr[esp + 0x1c], 0x0046d77e
			je _fun
			jmp _org

			_fun :
			pushad
				MYASMCALL_1(fun_setStats,esi)
				popad

				_org :

			xor ecx, esp
				mov byte ptr ds : [esi + 0x60] , 0x0
				jmp g_objSetStats.m_pRet
		}
	}

	DWORD is_enable_fail_addr = 0x0046d91e;
	__declspec(naked) void Naked_IsEnable()
	{
		_asm
		{

			mov dword ptr ss : [esp + 0x20] , eax
			sub edx, eax
			pushad
			MYASMCALL_2(fun_IsEnable,ecx,ebp)
			test al,al
			popad
			je __fail

			_org:
			jmp g_objIsEnable.m_pRet

				__fail:
			jmp is_enable_fail_addr
		}
	}

	void loading(void*) {

		Sleep(8*1000);

		auto section = "specific_state_rec_rune";
		if (!GameConfiguration::IsFeatureEnabled("specific_state_rec_rune")) {
			return;
		}

		{
			std::string val = GameConfiguration::Get(section, "str", "");
			if (val != "") {
				singleStateRune rune;
				sscanf(val.c_str(), "%d,%d", &rune.itemId, &rune.is_double);
				rune.index = 0;
				runes.push_back(rune);
			}
		}

		{
			std::string val = GameConfiguration::Get(section, "dex", "");
			if (val != "") {
				singleStateRune rune;
				sscanf(val.c_str(), "%d,%d", &rune.itemId, &rune.is_double);
				rune.index = 1;
				runes.push_back(rune);
			}
		}

		{
			std::string val = GameConfiguration::Get(section, "rec", "");
			if (val != "") {
				singleStateRune rune;
				sscanf(val.c_str(), "%d,%d", &rune.itemId, &rune.is_double);
				rune.index = 2;
				runes.push_back(rune);
			}
		}
		{
			std::string val = GameConfiguration::Get(section, "int", "");
			if (val != "") {
				singleStateRune rune;
				sscanf(val.c_str(), "%d,%d", &rune.itemId, &rune.is_double);
				rune.index = 3;
				runes.push_back(rune);
			}
		}

		{
			std::string val = GameConfiguration::Get(section, "wis", "");
			if (val != "") {
				singleStateRune rune;
				sscanf(val.c_str(), "%d,%d", &rune.itemId, &rune.is_double);
				rune.index = 4;
				runes.push_back(rune);
			}
		}



		{
			std::string val = GameConfiguration::Get(section, "luc", "");
			if (val != "") {
				singleStateRune rune;
				sscanf(val.c_str(), "%d,%d", &rune.itemId, &rune.is_double);
				rune.index = 5;
				runes.push_back(rune);
			}
		}

		{
			std::string val = GameConfiguration::Get(section, "hp", "");
			if (val != "") {
				singleStateRune rune;
				sscanf(val.c_str(), "%d,%d", &rune.itemId, &rune.is_double);
				rune.index = 6;
				runes.push_back(rune);
			}
		}


		{
			std::string val = GameConfiguration::Get(section, "mp", "");
			if (val != "") {
				singleStateRune rune;
				sscanf(val.c_str(), "%d,%d", &rune.itemId, &rune.is_double);
				rune.index = 7;
				runes.push_back(rune);
			}
		}


		{
			std::string val = GameConfiguration::Get(section, "sp", "");
			if (val != "") {
				singleStateRune rune;
				sscanf(val.c_str(), "%d,%d", &rune.itemId, &rune.is_double);
				rune.index = 8;
				runes.push_back(rune);
			}
		}

		InitializeCriticalSection(&g_cs);
		if (*(PBYTE)0x004D2BC5 == 0xe9) {
			g_objSetSignature.Hook(0x004d2bcb, Naked_SetSignature, 5);
		}
		else {
			g_objSetSignature.Hook(0x004d2bc7, Naked_SetSignature, 6);
		}
		
		g_objIsEnable.Hook(0x0046D6C7, Naked_IsEnable, 6);
		g_objSetStats.Hook(0x004d2c82, Naked_setStats, 6);
		g_hookReqWis1.Hook(0x004D2BDC, Naked_set_req_wis_1, 7);
		g_hookReqWis2.Hook(0x004d2bee, Naked_set_req_wis_2, 7);
		g_hookReqWis3.Hook(0x004d2c0b, Naked_set_req_wis_3, 7);
	}



	void start()
	{
		_beginthread(loading,0,0);
	}
}





