// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <windows.h>
#pragma comment(lib,"user32.lib")

int getPos(int x, int y)
{
	_asm
	{
		mov eax, x
			sal eax, 0x4
			add eax, 0x27
			shl eax, 0x10
			mov x, eax
	}

	_asm
	{
		mov eax, y
			sal eax, 0x4
			sub eax, 0x4
			mov y, eax
	}
	return x + y;
}

DWORD WINAPI ThreadProc(_In_  LPVOID lpParameter)
{
	PBYTE pSlot = PBYTE(0x1005340); //初级[9][9]
	PBOOL pIsOver = (PBOOL)0x1005164;
	HWND hWin = FindWindow(L"扫雷", NULL);
_loop:
	for (DWORD i = 1; i < 10; i++)
	{
		for (DWORD j = 1; j < 10; j++)
		{
			if (pSlot[i * 0x20 + j] == 0x0f)
			{
				SendMessage(hWin, WM_LBUTTONDOWN, 1, getPos(i, j));
				SendMessage(hWin, WM_LBUTTONUP, 0, 0);
				Sleep(100);
				goto _loop;
			}
		}
	}
	return 0;
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

