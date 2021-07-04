#pragma once
#ifndef SHAIYA_UTILITY_NATIVE_HEADER
#define SHAIYA_UTILITY_NATIVE_HEADER

#include <windows.h>
#include <Shlwapi.h>
#define PSAPI_VERSION 1
#include <Psapi.h>
#include <string>
#include <functional>
#include <TlHelp32.h>
#include <WinCrypt.h>
#include <atlbase.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <iostream>
#include <vector>
#include <Softpub.h>
#include <wincrypt.h>
#include <wintrust.h>
#include <wil/resource.h>

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib,"psapi.lib")
#pragma comment (lib, "wintrust")



typedef struct _SYSTEM_HANDLE
{
	DWORD    dwProcessId;
	BYTE     bObjectType;
	BYTE     bFlags;
	WORD     wValue;
	PVOID    pAddress;
	DWORD    GrantedAccess;
}
SYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
	DWORD         dwCount;
	SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, * PSYSTEM_HANDLE_INFORMATION;

#define STATUS_INFO_LENGTH_MISMATCH 0xc0000004
typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation = 0,
	SystemPerformanceInformation = 2,
	SystemTimeOfDayInformation = 3,
	SystemProcessInformation = 5,
	SystemProcessorPerformanceInformation = 8,
	SystemInterruptInformation = 23,
	SystemExceptionInformation = 33,
	SystemRegistryQuotaInformation = 37,
	SystemLookasideInformation = 45,
	SystemCodeIntegrityInformation = 103,
	SystemPolicyInformation = 134,
	SystemHandleInformation = 0x10,
} SYSTEM_INFORMATION_CLASS;

using  ZwQuerySystemInformationDef = NTSTATUS(WINAPI*)(
	_In_      SYSTEM_INFORMATION_CLASS SystemInformationClass,
	_Inout_   PVOID                    SystemInformation,
	_In_      ULONG                    SystemInformationLength,
	_Out_opt_ PULONG                   ReturnLength
	);

using ZwDuplicateObjectDef = NTSTATUS(WINAPI*) (
	HANDLE      SourceProcessHandle,
	HANDLE      SourceHandle,
	HANDLE      TargetProcessHandle,
	PHANDLE     TargetHandle,
	ACCESS_MASK DesiredAccess,
	ULONG       HandleAttributes,
	ULONG       Options
	);

#define DUPLICATE_SAME_ATTRIBUTES 0x00000004

namespace Utility::Native {



	template<typename T>
	static T GetNativeProcAddress(_In_ const char* Name) {
		static 	auto module = LoadLibrary(L"Ntdll.dll");
		if (module == nullptr) {
			return (T)nullptr;
		}
		return (T)GetProcAddress(module, Name);
	}

	static HRESULT ZwQuerySystemInformation_SysmHndInfo(_In_ PSYSTEM_HANDLE_INFORMATION* Output) {

		static auto ZwQuerySystemInformation = GetNativeProcAddress<ZwQuerySystemInformationDef>("ZwQuerySystemInformation");
		RETURN_IF_NULL_ALLOC(ZwQuerySystemInformation);

		int n = 1000;
		auto p = std::make_unique<BYTE[]>(n);
		while (true) {

			NTSTATUS status = ZwQuerySystemInformation(SystemHandleInformation, p.get(), n, 0);
			if (status == STATUS_INFO_LENGTH_MISMATCH) {
				n *= 2;
				p = std::make_unique<BYTE[]>(n);
				continue;
			}
			RETURN_IF_NTSTATUS_FAILED(status);
			break;
		}
		*Output = reinterpret_cast<PSYSTEM_HANDLE_INFORMATION>(p.release());
		RETURN_HR(S_OK);
	}


	static HRESULT ZwQuerySystemInformation(
		_In_      SYSTEM_INFORMATION_CLASS SystemInformationClass,
		_Inout_   PVOID                    SystemInformation,
		_In_      ULONG                    SystemInformationLength,
		_Out_opt_ PULONG                   ReturnLength
		)
	{
		static auto p = GetNativeProcAddress<ZwQuerySystemInformationDef>("ZwQuerySystemInformation");
		RETURN_IF_NULL_ALLOC(p);
		return p(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
	}


	static HRESULT ZwDuplicatedObject(HANDLE SourceProcessHandle,
		HANDLE      SourceHandle,
		HANDLE      TargetProcessHandle,
		PHANDLE     TargetHandle,
		ACCESS_MASK DesiredAccess,
		ULONG       HandleAttributes,
		ULONG       Options) 
	{
		static auto ZwDuplicateObject = GetNativeProcAddress<ZwDuplicateObjectDef>("ZwDuplicateObject");
		RETURN_IF_NULL_ALLOC(ZwDuplicateObject);
		auto status = ZwDuplicateObject(SourceProcessHandle, SourceHandle,
			TargetProcessHandle, TargetHandle, DesiredAccess, HandleAttributes, Options);
		RETURN_NTSTATUS(status);
	}
}
#endif