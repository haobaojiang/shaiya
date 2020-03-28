// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <stdlib.h>
#include <string>
#define PSAPI_VERSION 1
#include <Psapi.h>
#include <TlHelp32.h>
#pragma comment(lib,"Psapi.lib")

#include <detours.h>
#include <MyClass.h>
#include <MyInlineHook.h>
#include <inttypes.h>
#include <define.h>
#include <process.h>


#pragma comment (lib,"User32.lib")


extern "C" __declspec(dllexport)  int Start(void)
{
	return 0;
}





#define MyLog(...) FILE* f=fopen("c:/log.txt","a+");\
fprintf(f,__VA_ARGS__ );\
fclose(f);



char g_strOptionName[] = "A";

char g_strPs_game[] = "ps_game_A";
char g_strPs_login[] = "ps_login_A";
char g_strPs_session[] = "ps_session_A";
char g_strPs_userLog[] = "ps_userLog_A";
char g_strPs_gameLog[] = "ps_gameLog_A";
char g_strPs_dbAgent[] = "ps_dbAgent_A";





namespace MyAPIHook {


	typedef HANDLE (WINAPI* fun_CreateEventA)(
			__in_opt LPSECURITY_ATTRIBUTES lpEventAttributes,
			__in     BOOL bManualReset,
			__in     BOOL bInitialState,
			__in_opt LPCSTR lpName
		);


	typedef HANDLE (WINAPI* fun_OpenFileMappingA)(
			__in DWORD dwDesiredAccess,
			__in BOOL bInheritHandle,
			__in LPCSTR lpName
		);

	typedef HANDLE (WINAPI* fun_CreateMutexA)(
			__in_opt LPSECURITY_ATTRIBUTES lpMutexAttributes,
			__in     BOOL bInitialOwner,
			__in_opt LPCSTR lpName
		);

	typedef HANDLE (WINAPI* fun_CreateFileMappingA)(
			__in     HANDLE hFile,
			__in_opt LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
			__in     DWORD flProtect,
			__in     DWORD dwMaximumSizeHigh,
			__in     DWORD dwMaximumSizeLow,
			__in_opt LPCSTR lpName
		);

	fun_OpenFileMappingA g_oldOpenFileMappingA = OpenFileMappingA;
	fun_CreateMutexA g_oldCreateMutexA = CreateMutexA;
	fun_CreateFileMappingA g_oldCreateFileMappingA = CreateFileMappingA;
	fun_CreateEventA g_oldCreateEventA = CreateEventA;



	HANDLE WINAPI MyCreateEventA(
		__in_opt LPSECURITY_ATTRIBUTES lpEventAttributes,
		__in     BOOL bManualReset,
		__in     BOOL bInitialState,
		__in_opt LPCSTR lpName
	)
	{
		if (lpName&&lpName[0] != '\0') {
			char newName[MAX_PATH] = { 0 };
			sprintf(newName, "%s_%s", lpName, g_strOptionName);
			return g_oldCreateEventA(lpEventAttributes, bManualReset, bInitialState, newName);
		}
		return g_oldCreateEventA(lpEventAttributes, bManualReset, bInitialState, lpName);
	}


	HANDLE WINAPI MyOpenFileMappingA(
		__in DWORD dwDesiredAccess,
		__in BOOL bInheritHandle,
		__in LPCSTR lpName
	) 
	{
		if (lpName&&lpName[0] != '\0') {
			char newName[MAX_PATH] = { 0 };
			sprintf(newName, "%s_%s", lpName, g_strOptionName);
			return g_oldOpenFileMappingA(dwDesiredAccess, bInheritHandle, newName);
		}
		return g_oldOpenFileMappingA(dwDesiredAccess, bInheritHandle, lpName);
	}


	HANDLE WINAPI MyCreateFileMappingA(
	__in     HANDLE hFile,
		__in_opt LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
		__in     DWORD flProtect,
		__in     DWORD dwMaximumSizeHigh,
		__in     DWORD dwMaximumSizeLow,
		__in_opt LPCSTR lpName
		)
	{
		if (lpName&&lpName[0]!='\0') {
			char newName[MAX_PATH] = { 0 };
			sprintf(newName, "%s_%s", lpName, g_strOptionName);
			return g_oldCreateFileMappingA(hFile, lpFileMappingAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, newName);
		}
		return g_oldCreateFileMappingA(hFile, lpFileMappingAttributes, flProtect, dwMaximumSizeHigh, dwMaximumSizeLow, lpName);
	}

	HANDLE WINAPI MyCreateMutexA(
		__in_opt LPSECURITY_ATTRIBUTES lpMutexAttributes,
		__in     BOOL bInitialOwner,
		__in_opt LPCSTR lpName
	) 
	{
		if (lpName&&lpName[0] != '\0') {
			char newName[MAX_PATH] = { 0 };
			sprintf(newName, "%s_%s", lpName, g_strOptionName);
			return g_oldCreateMutexA(lpMutexAttributes, bInitialOwner, newName);
		}
		return g_oldCreateMutexA(lpMutexAttributes, bInitialOwner, lpName);
	}




	BOOL UnInitDetours()
	{
		BOOL bRet = FALSE;
		__try{
			if (DetourTransactionBegin() != NO_ERROR)
				__leave;
			if (DetourUpdateThread(::GetCurrentThread()) != NO_ERROR)
				__leave;

			DetourDetach(&(PVOID&)g_oldCreateMutexA, MyCreateMutexA);
			DetourDetach(&(PVOID&)g_oldCreateFileMappingA, MyCreateFileMappingA);


			if (DetourTransactionCommit() != NO_ERROR)
				__leave;
			bRet = TRUE;
		}__finally{
			;
		}

		return bRet;
	}

	BOOL InitDetours()
	{
		BOOL bRet = FALSE;
		__try{
			if (DetourTransactionBegin())
				__leave;
	
			if (DetourUpdateThread(::GetCurrentThread()))
				__leave;

 			DetourAttach(&(PVOID&)g_oldCreateMutexA, MyCreateMutexA);
			DetourAttach(&(PVOID&)g_oldCreateFileMappingA, MyCreateFileMappingA);
			DetourAttach(&(PVOID&)g_oldCreateFileMappingA, MyOpenFileMappingA);

			if (DetourTransactionCommit() != NO_ERROR)
				__leave;
			bRet = TRUE;

		}__finally{
			;
		}
		return bRet;
	}



}





void serviceNameChange()
{
	WCHAR szProcessName[MAX_PATH] = { 0 };
	GetModuleBaseName(GetCurrentProcess(), NULL, szProcessName, MAX_PATH);
	if (wcscmp(szProcessName, L"PSM_ServiceMgr.exe") == 0) {
		sprintf((char*)0x0044a340, "ps_game_%s", g_strOptionName);
		sprintf((char*)0x00449110, "PSM_AgentClient_%s", g_strOptionName);
		sprintf((char*)0x00449418, "ps_userLog_%s", g_strOptionName);
		sprintf((char*)0x00449720, "ps_session_%s", g_strOptionName);
		sprintf((char*)0x00449a28, "ps_login_%s", g_strOptionName);
		sprintf((char*)0x00449d30, "ps_gamelog_%s", g_strOptionName);
		sprintf((char*)0x0044a038, "ps_dbAgent_%s", g_strOptionName);
	}
	else if (wcscmp(szProcessName, L"PSMServer_ServiceMgr.exe") == 0) {
		sprintf((char*)0x00448110, "PSM_AgentServer_%s", g_strOptionName);
	}
	else if (wcscmp(szProcessName, L"PSMServer_Mgr.exe") == 0) {

	}
}




 void start()
{
	 MyAPIHook::InitDetours();
	 serviceNameChange();
}

 void stop()
 {
	 MyAPIHook::UnInitDetours();
 }



DWORD g_firstTime = 0;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		start();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		stop();
		break;
	}
	return TRUE;
}

