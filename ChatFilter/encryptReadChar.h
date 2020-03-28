#pragma once
#ifndef SHAIYA_ENCRYPT_READCHAR_HEADER
#define SHAIYA_ENCRYPT_READCHAR_HEADER

#include "../utility/utility.h"

namespace EncryptReadChar {

	ShaiyaUtility::CMyInlineHook g_obj;
	ShaiyaUtility::CMyInlineHook g_obj1;

	void __stdcall Encrypt(_In_ ShaiyaUtility::EP4::CUser* Player, _In_ const BYTE* Buffer, _In_ DWORD Length) {

		auto p = std::make_unique<BYTE[]>(Length + 2);
		BYTE signature = 15;
		p.get()[0] = Buffer[0];
		p.get()[1] = Buffer[1];
		p.get()[2] = signature;       // encrypt signature
		p.get()[3] = static_cast<BYTE>(Length);
		for (DWORD i = 2; i < Length; i++) {
			p.get()[i + 2] = Buffer[i] ^ signature;
		}
		ShaiyaUtility::EP4::SendPacket(Player, p.get(), Length + 2);
	}

	__declspec(naked) void Naked()
	{
		_asm
		{
			pushad
			mov ebx, [esp + 0x20]
			mov edx, [esp + 0x24]
			MYASMCALL_3(Encrypt, ecx, ebx, edx)
			popad
			add esp,0x8
			jmp g_obj.m_pRet
		}
	}

	__declspec(naked) void Naked1()
	{
		_asm
		{
			pushad
			mov ebx, [esp + 0x20]
			mov edx, [esp + 0x24]
			MYASMCALL_3(Encrypt, ecx, ebx, edx)
			popad
			add esp, 0x8
			jmp g_obj1.m_pRet
		}
	}


	void Start() {

		if (!GameConfiguration::IsFeatureEnabled("encReadChar")) {
			return;
		}

		g_obj.Hook(0x0046d32c, Naked);
		g_obj1.Hook(0x0046d0a8, Naked1);
	}
}

#endif