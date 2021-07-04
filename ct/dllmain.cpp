// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <process.h>
#include "../utility/process.h"
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// µ¼³öº¯Êý
#pragma comment(linker, "/EXPORT:ijlGetLibVersion=ijl15Org.ijlGetLibVersion,@1")
#pragma comment(linker, "/EXPORT:ijlInit=ijl15Org.ijlInit,@2")
#pragma comment(linker, "/EXPORT:ijlFree=ijl15Org.ijlFree,@3")
#pragma comment(linker, "/EXPORT:ijlRead=ijl15Org.ijlRead,@4")
#pragma comment(linker, "/EXPORT:ijlWrite=ijl15Org.ijlWrite,@5")
#pragma comment(linker, "/EXPORT:ijlErrorStr=ijl15Org.ijlErrorStr,@6")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void testMem(void*)
{
    auto* p = reinterpret_cast<PDWORD>(0x00500000);


    wchar_t str[MAX_PATH]{};
	for(auto i=0;i<1000;i++)
	{
        const auto v = *(p);
        StringCchPrintfW(str, MAX_PATH, L"%d", v);
        OutputDebugStringW(str);
        p++;
        Sleep(100);
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
        _beginthread(testMem, 0, 0);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

