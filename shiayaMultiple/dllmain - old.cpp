// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <windows.h>

extern "C" __declspec(dllexport)  int Start(void)
{
	return 0;
}


bool IsHook(PBYTE pfun)
{
	if (pfun&&*pfun == 0xe9)
		return true;
	return false;	
}



bool CheckSpeedHacker()
{
	do
	{
		HMODULE hKernel32 = GetModuleHandle(L"Kernel32.dll");
		HMODULE hWinmmm = GetModuleHandle(L"Winmm.dll");
		HMODULE hUser32 = GetModuleHandle(L"user32.dll");
		PBYTE pfunGetTickCount64 = NULL;
		PBYTE pfunQueryPerformanceCounter = NULL;
		PBYTE pfunGetTickCount = NULL;
		PBYTE pfuntimeGetTime = NULL;

		PBYTE pfunFindWindow = NULL;

		if (hKernel32)
		{
			pfunGetTickCount64 = (PBYTE)GetProcAddress(hKernel32, "GetTickCount64");
			pfunQueryPerformanceCounter = (PBYTE)GetProcAddress(hKernel32, "QueryPerformanceCounter");
			pfunGetTickCount = (PBYTE)GetProcAddress(hKernel32, "GetTickCount");
		}
		if (hWinmmm)
			pfuntimeGetTime = (PBYTE)GetProcAddress(hWinmmm, "timeGetTime");

		if (hUser32)
		{
			pfunFindWindow = (PBYTE)GetProcAddress(hUser32, "FindWindowA");
		}

		while (1)
		{
			Sleep(1000);
			if (pfunGetTickCount64&&
				*pfunGetTickCount64 == 0xe9)
				TerminateProcess(GetCurrentProcess(), 0);

			if (pfunQueryPerformanceCounter&&*pfunQueryPerformanceCounter == 0xe9)
				TerminateProcess(GetCurrentProcess(), 0);

			if (pfunGetTickCount&&*pfunGetTickCount == 0xe9)
				TerminateProcess(GetCurrentProcess(), 0);

			if (pfuntimeGetTime&&*pfuntimeGetTime == 0xe9)
				TerminateProcess(GetCurrentProcess(), 0);

			if (IsHook(pfunFindWindow))
				TerminateProcess(GetCurrentProcess(), 0);

		}

	} while (0);


	return 0;
}




DWORD WINAPI ThreadProc(_In_  LPVOID lpParameter)
{
	

}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		::CreateThread(0, 0, ThreadProc, 0, 0, 0);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

