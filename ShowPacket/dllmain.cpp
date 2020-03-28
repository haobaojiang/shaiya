// dllmain.cpp : ���� DLL Ӧ�ó������ڵ㡣
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
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&ShowDialog, hModule, 0, NULL);     //��ʾ����
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&CatchKeystrokes, hModule, 0, NULL);//�����ݼ�
		SetDetour();  //hook�����
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

