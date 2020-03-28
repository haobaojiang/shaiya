// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <windows.h>
#include "Ep8.h"
#define  PSAPI_VERSION 1
#include <Psapi.h>
#pragma comment(lib,"psapi.lib")
#ifdef _X86_
extern "C" { int _afxForceUSRDLL; }
#else
extern "C" { int __afxForceUSRDLL; }
#endif

// 这是导出函数的一个示例。
extern "C" int Start(void)
{
	return 0;
}




bool isInject() {
	WCHAR szProcessName[MAX_PATH] = { 0 };
	GetModuleBaseName(GetCurrentProcess(), NULL, szProcessName, sizeof(szProcessName));
	if (wcscmp(szProcessName, L"ps_game.exe") == 0) {
		return true;

	}
	return false;
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		if (isInject()) {
			::CreateThread(0, 0, ThreadProc1, 0, 0, 0);
		}
		
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

