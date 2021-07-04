// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <windows.h>
#include <process.h>
#include <sstream>
//#include <INIReader.h>
#include <plog/Log.h>
//#include <plog/Converters/NativeEOLConverter.h>



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        {
	 LOGD << "1234";
       // std::wostringstream a;
       // std::wistringstream b;
    }
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

