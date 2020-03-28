// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <windows.h>



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
#pragma comment(linker, "/EXPORT:ijlGetLibVersion=ijl15OrgOrg.ijlGetLibVersion,@1")
#pragma comment(linker, "/EXPORT:ijlInit=ijl15OrgOrg.ijlInit,@2")
#pragma comment(linker, "/EXPORT:ijlFree=ijl15OrgOrg.ijlFree,@3")
#pragma comment(linker, "/EXPORT:ijlRead=ijl15OrgOrg.ijlRead,@4")
#pragma comment(linker, "/EXPORT:ijlWrite=ijl15OrgOrg.ijlWrite,@5")
#pragma comment(linker, "/EXPORT:ijlErrorStr=ijl15OrgOrg.ijlErrorStr,@6")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers  
#include <windows.h>
#define  PSAPI_VERSION 1
#include <Psapi.h>
#include <string>


#pragma comment (lib,"Psapi.lib")


#include "90.h"
#include "50.h"

HMODULE g_hModule = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_hModule = hModule;
		Shaiya50::Start();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

