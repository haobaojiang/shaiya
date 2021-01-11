#pragma once
#ifndef SHAIYA_FIRE_PORT_HEADER
#define SHAIYA_FIRE_PORT_HEADER


#include "stdafx.h"
#include "config.hpp"
#include "party_hook.hpp"

namespace fire_port {

	ShaiyaUtility::CMyInlineHook g_file_port;


	using fire_port_callBack_def = std::function<bool(void*, WORD, ShaiyaUtility::Pos*)>;
	std::vector<fire_port_callBack_def> g_callBacks;

	void register_callBack(fire_port_callBack_def callback) {
		g_callBacks.push_back(callback);
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

	void start() {

		g_file_port.Hook(0x00476792, Naked, 6);

	}
}
#endif