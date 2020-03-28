// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "TransLate.h"

extern DWORD g_dwMap;
extern DWORD g_dwItemid;


DWORD WINAPI ThreadProc(_In_  LPVOID lpParameter)
{

	//设置路径
	WCHAR szTemp[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(0), szTemp, MAX_PATH);
	PathRemoveFileSpec(szTemp);
	CString g_szFilePath = szTemp;
	g_szFilePath = g_szFilePath + L"\\config.ini";

	//1.地图消耗物品
	g_dwItemid = GetPrivateProfileInt(L"Transfer", L"ItemId", 0, g_szFilePath);
	g_dwMap = GetPrivateProfileInt(L"Transfer", L"Mapid", 0, g_szFilePath);
	StartTranslateLimit();

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

