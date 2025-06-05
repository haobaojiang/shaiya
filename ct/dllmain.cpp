// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <windows.h>
#include <process.h>
#include "../utility/process.h"
#include <memory>
#include "../utility/utility.h"

#include "hijack.h"


ShaiyaUtility::CMyInlineHook obj1;

DWORD orgAddr = 0;


bool __stdcall Filter(void* Player, PBYTE pPackets)
{
	char* strChat = (char*)&pPackets[3];
	return strstr(strChat, "<<999c01") == NULL;
}

DWORD falledAddr = 0x0047FC57;
__declspec(naked) void  Naked_ChatFilter()
{
	_asm
	{
		pushad
		MYASMCALL_2(Filter, ebp, ebx)
		test al, al
		popad
		jne _Org
		jmp falledAddr

		_Org:
		jmp orgAddr
	}
}


void mainFun(void*)
{
	Sleep(6000);
	orgAddr =(DWORD) LoadLibrary(L"dbghelp.dll");
	orgAddr += 0x97e0;
    obj1.Hook((PVOID)0x0047f4ba, Naked_ChatFilter, 7);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        _beginthread(mainFun, 0, 0);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

