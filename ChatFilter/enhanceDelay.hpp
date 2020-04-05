#pragma once
#ifndef SHAIYA_ENHANCE_DELAY_HEADER
#define SHAIYA_ENHANCE_DELAY_HEADER

#include "stdafx.h"
#include "config.hpp"
#include "callBack.hpp"
#include <array>

namespace EnhanceDelay {

	ShaiyaUtility::CMyInlineHook g_obj, g_obj2;
	static std::map<DWORD, DWORD> g_data;
	DWORD g_delay = 1100;

	bool __stdcall IsEnhanceAble(void* Player) {
		auto uid = ShaiyaUtility::EP6::PlayerUid(Player);
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


	void __stdcall SetDelay(void* Player) {
		auto uid = ShaiyaUtility::EP6::PlayerUid(Player);
		g_data[uid] = GetTickCount() + g_delay;
	}

	PVOID  g_pFailedAddr = (PVOID)0x0046D5BE;
	__declspec(naked) void Naked()
	{
		_asm
		{
			mov ebx, ecx
			movzx esi, byte ptr ds : [ebx + 0x2]
			pushad
			MYASMCALL_1(IsEnhanceAble, edi)
			test al, al
			popad
			je _wrong

			jmp g_obj.m_pRet
			_wrong :
			push edi
				jmp g_pFailedAddr
		}
	}

	__declspec(naked) void Naked1()
	{
		_asm
		{
			pushad
			MYASMCALL_1(SetDelay, ebp)
			popad
			inc al
			movzx edx, al
			jmp g_obj2.m_pRet
		}
	}


	void start() {

		if (!GameConfiguration::IsFeatureEnabled("enhanceDelay")) {
			return;
		}

		g_delay = GameConfiguration::GetInteger("enhanceDelay", "delay", 1111);

		g_obj.Hook(0x0046CBC4, Naked, 6);//0046CBC4  |.  8BD9          mov ebx,ecx
		g_obj2.Hook(0x0046CD97, Naked1, 5);//0046CD97  |.  FEC0          inc al

		auto restoreOldDllDelay = [](void*)->void {
			Sleep(5 * 1000);
			BYTE data[] = {
			0x81 ,0xEC ,0x18 ,0x0C,0x00 ,0x00
			};
			ShaiyaUtility::WriteCurrentProcessMemory((void*)0x0046cba0, data, sizeof(data));
		};
		_beginthread(restoreOldDllDelay, 0, 0);
	}
}
#endif