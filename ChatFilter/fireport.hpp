#pragma once
#ifndef SHAIYA_FIRE_PORT_HEADER
#define SHAIYA_FIRE_PORT_HEADER


#include "stdafx.h"
#include "config.hpp"
#include "party_hook.hpp"

namespace fire_port {

	ShaiyaUtility::CMyInlineHook g_file_port,g_file_port_2;


	using fire_port_callBack_def = std::function<bool(void*, WORD, ShaiyaUtility::Pos*)>;
	using fire_port_callBack_def_2 = std::function<bool(void*, WORD)>;
	std::vector<fire_port_callBack_def> g_callBacks;
	std::vector<fire_port_callBack_def_2> g_callBacks_2;

	void register_callBack(fire_port_callBack_def callback) {
		g_callBacks.push_back(callback);
	}

	void register_callBack_2(fire_port_callBack_def_2 callback) {
		g_callBacks_2.push_back(callback);
	}


	bool __stdcall fire_port_validate(void* player, WORD Map, ShaiyaUtility::Pos*  pos)
	{
		for (auto callback : g_callBacks) {
			if (!callback(player, Map, pos)) {
				return false;
			}
		}
		return true;
	}


	bool __stdcall fire_port_validate_2(void* player, WORD Map)
	{
		for (auto callback : g_callBacks_2) {
			if (!callback(player, Map)) {
				return false;
			}
		}
		return true;
	}

	PVOID  g_pFailedAddr = (PVOID)0x0047679A;
	__declspec(naked) void Naked()
	{
		_asm
		{
			pushad
			add edi, 0x24
			MYASMCALL_3(fire_port_validate, ebp, ecx, edi)
			cmp al, 0x0
			popad
			je _wrong

			cmp ecx, 0xc8
			jmp g_file_port.m_pRet
			_wrong :
			jmp g_pFailedAddr
		}
	}


	// 004768F6    C64424 1B 04    mov byte ptr ss:[esp+0x1B],0x4


	//
	PVOID  g_pFailedAddr_2 = (PVOID)0x004768F6;
	__declspec(naked) void Naked_2()
	{
		_asm
		{
			mov edx,dword ptr [esp+0x20]
			mov edx,dword ptr [edx+0x20]
			pushad
			MYASMCALL_2(fire_port_validate_2, ebp,edx)
			cmp al, 0x0
			popad
			je _wrong
			mov edx, dword ptr ss : [ebp + 0xE0]
			jmp g_file_port_2.m_pRet

			_wrong:
			jmp g_pFailedAddr_2
		}
	}

	void start() {

		g_file_port.Hook(0x00476792, Naked, 6);

		//
		//004768F6    C64424 1B 04    mov byte ptr ss:[esp+0x1B],0x4

		//00476937    8B95 E0000000   mov edx,dword ptr ss:[ebp+0xE0]



		g_file_port_2.Hook(0x00476937, Naked_2, 6);

	}
}
#endif