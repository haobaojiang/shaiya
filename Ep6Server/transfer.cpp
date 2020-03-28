#include <windows.h>
#include <MyInlineHook.h>
#include <vector>
#include <common.h>
#include <MyClass.h>
#include "firePort.h"


using std::vector;
using namespace EP6CALL;

extern WCHAR g_szFilePath[MAX_PATH];
namespace firePort
{
	CMyInlineHook objHook1;
	CMyInlineHook objHook2;
	CMyInlineHook objHook3;
	CMyInlineHook objHook4;
	CMyInlineHook objHook5;

	DWORD Addr = 0x00479155;
	DWORD Addr2 =0x00466D4B;
	DWORD g_skillId = 0;
	BYTE  g_skillLevel = 0;



	void __stdcall useProtectSKill(Pshaiya_player player)
	{
		UseItemSkill(player, g_skillId, g_skillLevel);
	}


	__declspec(naked) void Naked_RespawnProtected()
	{
		_asm
		{
			pushad
			MYASMCALL_1(useProtectSKill,edi)
			popad
			jmp Addr
		}

	}

	__declspec(naked) void Naked_RespawnProtected2()
	{
		_asm
		{
			mov dword ptr ss : [ebp + 0x1234], ecx
			pushad
			MYASMCALL_1(useProtectSKill, edi)
			popad
			jmp Addr2
		}
	}

	void start()
	{
		g_skillId    = GetPrivateProfileInt(L"resProtect", L"Skillid", 0, g_szFilePath);
		g_skillLevel = GetPrivateProfileInt(L"resProtect", L"Skilllevel", 0, g_szFilePath);
		objHook1.Hook((PVOID)0x00478E4E, Naked_RespawnProtected);
		objHook2.Hook((PVOID)0x00478ECB, Naked_RespawnProtected);
		objHook3.Hook((PVOID)0x00478EFA, Naked_RespawnProtected);
		objHook4.Hook((PVOID)0x00478F2D, Naked_RespawnProtected);
		objHook5.Hook((PVOID)0x00466D45, Naked_RespawnProtected2, 6);
	}
}





