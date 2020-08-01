#pragma once
#ifndef SHAIYA_ENCRYPT_READCHAR_HEADER
#define SHAIYA_ENCRYPT_READCHAR_HEADER

#include "../utility/utility.h"
#include <vector>
namespace EncryptReadChar {

	ShaiyaUtility::CMyInlineHook g_obj;
	ShaiyaUtility::CMyInlineHook g_obj1;

	void __stdcall Encrypt(_In_ ShaiyaUtility::EP4::CUser* Player, _In_ const BYTE* Buffer, _In_ DWORD Length) {


		BYTE signature = 18;
		// get buff to encrypt
		std::vector<BYTE> toBeEncrypted;
		for (DWORD i = 2; i < Length; i++) {
			toBeEncrypted.push_back(Buffer[i]);
		}
		// base64 buf
		std::string encryptedStr =  Utility::base64_encode(toBeEncrypted.data(), toBeEncrypted.size());

		// xor buf
		for (DWORD i = 0; i < encryptedStr.size(); i++) {
			encryptedStr[i] ^= signature;
		}

		//
		auto p = std::make_unique<BYTE[]>(encryptedStr.size() + 4);
		p.get()[0] = Buffer[0];
		p.get()[1] = Buffer[1];
		p.get()[2] = signature - 3;       // encrypt signature
		p.get()[3] = static_cast<BYTE>(encryptedStr.size());
		memcpy(&p.get()[4], encryptedStr.c_str(), encryptedStr.size());

		ShaiyaUtility::EP4::SendPacket(Player, p.get(), encryptedStr.size() + 4);
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
			add esp, 0x8
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