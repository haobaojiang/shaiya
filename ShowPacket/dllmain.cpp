// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "Form.h"
#include "Detours.h" 
CRITICAL_SECTION g_cs;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		InitializeCriticalSection(&g_cs);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ShowDialog, hModule, 0, NULL);     //显示窗口
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CatchKeystrokes, hModule, 0, NULL);//捕获快捷键
		SetDetour();  //hook封包处
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		DeleteCriticalSection(&g_cs);
		break;
	}
	return TRUE;
}

