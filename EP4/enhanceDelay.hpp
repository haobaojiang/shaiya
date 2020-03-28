#pragma once


#include "stdafx.h"
#include "config.hpp"
#include "callBack.hpp"
#include <windows.h>
#include <process.h>
#include "../utility/utility.h"
#include <map>

namespace EnhanceDelay {

	ShaiyaUtility::CMyInlineHook g_obj, g_obj2;
	static std::map<DWORD, DWORD> g_data;
	DWORD g_delay = 1100;

	bool __stdcall IsEnhanceAble(ShaiyaUtility::EP4::CUser* User) {
		auto uid = User->m_dwUID;
		auto iter = g_data.find(uid);
		auto tickCount = GetTickCount();

		if (iter == g_data.end()) {
			return true;
		}

		if (iter->second > tickCount) {
			return false;
		}
		return true;
	}


	void __stdcall SetDelay(ShaiyaUtility::EP4::CUser* User) {
		auto uid = User->m_dwUID;
		g_data[uid] = GetTickCount() + g_delay;
	}

	PVOID  g_pFailedAddr = (PVOID)0x00460525;
	__declspec(naked) void Naked()
	{
		_asm
		{
			pushad
			MYASMCALL_1(IsEnhanceAble,edi)
			test al, al
			popad
			je _wrong
			mov edx, dword ptr ds : [edi + 0x1304]

			jmp g_obj.m_pRet
			_wrong :
			mov dword ptr ss : [esp + 0x18] , edi
			jmp g_pFailedAddr
		}
	}



	__declspec(naked) void Naked1()
	{
		_asm
		{
			pushad
			mov ecx,[esp+0x40]
			MYASMCALL_1(SetDelay, ecx)
			popad
			mov word ptr [esp + 0x4C], ax
			jmp g_obj2.m_pRet
		}
	}


	void start() {

		if (!GameConfiguration::IsFeatureEnabled("enhanceDelay")) {
			return;
		}

		g_delay = GameConfiguration::GetInteger("enhanceDelay", "delay", 1111);

		g_obj.Hook( 0x0045fb43, Naked,6);
		g_obj2.Hook(0x0045fcd4, Naked1, 5);
	}
}