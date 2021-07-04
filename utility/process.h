#pragma once
#ifndef SHAIYA_UTILITY_PROCESS_HEADER
#define SHAIYA_UTILITY_PROCESS_HEADER

#include <windows.h>
#include <wil/resource.h>
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
#include <optional>
#include "file.h"

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib,"psapi.lib")

namespace Utility::Process {

	static HRESULT EnableDebugPrivilege()
	{
		TOKEN_PRIVILEGES tp;
		LUID luid;

		auto hr = S_OK;

		HANDLE token = nullptr;
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
			hr = HRESULT_FROM_WIN32(GetLastError());
			return hr;
		}


		if (!LookupPrivilegeValue(
			NULL,            // lookup privilege on local system
			SE_DEBUG_NAME,   // privilege to lookup 
			&luid))        // receives LUID of privilege
		{
			CloseHandle(token);
			hr = HRESULT_FROM_WIN32(GetLastError());
			return hr;
		}

		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = luid;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;


		// Enable the privilege or disable all privileges.

		if (!AdjustTokenPrivileges(
			token,
			FALSE,
			&tp,
			sizeof(TOKEN_PRIVILEGES),
			(PTOKEN_PRIVILEGES)NULL,
			(PDWORD)NULL))
		{
			CloseHandle(token);
			hr = HRESULT_FROM_WIN32(GetLastError());
			return hr;
		}

		if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
		{
			CloseHandle(token);
			hr = HRESULT_FROM_WIN32(GetLastError());
			return hr;
		}
		CloseHandle(token);
		return TRUE;
	}

	template<typename F>
	static HRESULT EnumProcess(F CallBack) {

		HRESULT hr = S_OK;

		auto hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (!hProcSnap) {
			hr = HRESULT_FROM_WIN32(GetLastError());
			return hr;
		}

		PROCESSENTRY32W tEntry = { 0 };
		tEntry.dwSize = sizeof(PROCESSENTRY32W);

		// Iterate threads
		for (BOOL success = Process32FirstW(hProcSnap, &tEntry);
			success != FALSE;
			success = Process32NextW(hProcSnap, &tEntry))
		{
			CallBack(tEntry);
		}

		CloseHandle(hProcSnap);

		if (GetLastError() != ERROR_NO_MORE_FILES) {
			hr = HRESULT_FROM_WIN32(GetLastError());
			return hr;
		}

		return hr;
	}

	using MemFilter = std::function<bool(const MEMORY_BASIC_INFORMATION&)>;
	static std::vector<MEMORY_BASIC_INFORMATION> GetMemRanges(HANDLE Process, MemFilter Filter)
	{
		std::vector<MEMORY_BASIC_INFORMATION> result;
		unsigned char* p = nullptr;
		MEMORY_BASIC_INFORMATION info;

		while (true) {
			const SIZE_T s = VirtualQueryEx(Process, p, &info, sizeof(info));
			if (s == 0) {
				break;
			}
			p += info.RegionSize;
			if (!Filter(info)) {
				continue;
			}
			result.push_back(info);
		}
		return result;
	}


	static HRESULT FindProcessIdByName(_In_ const std::wstring& ProcessName, _Inout_ DWORD* Pid)
	{
		
		*Pid = 0;

		wil::unique_handle hProcSnap(::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
		RETURN_LAST_ERROR_IF(!hProcSnap);

		PROCESSENTRY32W tEntry = { 0 };
		tEntry.dwSize = sizeof(PROCESSENTRY32W);
	
		RETURN_IF_WIN32_BOOL_FALSE(::Process32FirstW(hProcSnap.get(), &tEntry));
		while (true)
		{	
			if (_wcsicmp(tEntry.szExeFile, ProcessName.c_str()) == 0) {
				*Pid = tEntry.th32ProcessID;
				RETURN_HR(S_OK);
			}
			
			if(!Process32NextW(hProcSnap.get(), &tEntry)){
				break;
			}
		}
		if (GetLastError() == ERROR_NO_MORE_FILES) {
			RETURN_HR(ERROR_NOT_FOUND);
		}
		RETURN_LAST_ERROR();
	}


	static HRESULT GetProcessFullPath(_In_ DWORD Pid, _Inout_ std::wstring* Path) {

		HRESULT hr = S_OK;

		HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, Pid);
		if (hProcess == nullptr) {
			hr = HRESULT_FROM_WIN32(GetLastError());
			return hr;
		}


		WCHAR filename[512] = {};
		if (!GetModuleFileNameEx(hProcess, NULL, filename, 512)) {
			hr = HRESULT_FROM_WIN32(GetLastError());
			::CloseHandle(hProcess);
			return hr;
		}


		*Path = filename;
		::CloseHandle(hProcess);

		return hr;
	}

	static HRESULT GetProcessFileSize(_In_ DWORD Pid, _Inout_ LARGE_INTEGER* FileSize) {

		HRESULT hr = S_OK;

		FileSize->QuadPart = 0;

		std::wstring filePath;
		hr = GetProcessFullPath(Pid, &filePath);
		if (FAILED(hr)) {
			return hr;
		}

		hr = Utility::File::GetFileSize(filePath.c_str(), FileSize);
		if (FAILED(hr)) {
			return hr;
		}

		return hr;
	}


	template<typename F>
	static HRESULT EnumProcessFileFullPath(F CallBack) {

		HRESULT hr = S_OK;

		const size_t maxPids = 1024;
		DWORD pids[maxPids] = {};
		DWORD bytesReturned = 0;

		if (!EnumProcesses(pids, sizeof pids, &bytesReturned)) {
			hr = HRESULT_FROM_WIN32(GetLastError());
			return hr;
		}

		DWORD cProcesses = bytesReturned / sizeof * pids;
		for (DWORD i = 0; i < cProcesses; i++) {

			std::wstring filePath;
			if (FAILED(GetProcessFullPath(pids[i], &filePath)))
			{
				continue;
			}

			CallBack(filePath.c_str());

		}

		return hr;
	}

	static HRESULT GetCurProcessParentId(_Inout_ DWORD* Output)
	{

		wil::unique_handle snapshot(::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));
		RETURN_LAST_ERROR_IF(!snapshot);

		PROCESSENTRY32 procentry{};
		procentry.dwSize = sizeof(PROCESSENTRY32);
		RETURN_IF_WIN32_BOOL_FALSE(Process32First(snapshot.get(), &procentry));

		DWORD pid = 0;

		// While there are processes, keep looping.
		DWORD  crtpid = GetCurrentProcessId();
		while (true)
		{
			if (crtpid == procentry.th32ProcessID)
			{
				*Output = procentry.th32ParentProcessID;
				RETURN_HR(S_OK);
			}
			procentry.dwSize = sizeof(PROCESSENTRY32);
			RETURN_IF_WIN32_BOOL_FALSE(Process32Next(snapshot.get(), &procentry));
		}
	}

	std::optional<std::wstring> GetProcessName(DWORD processID)
	{
		// Get a handle to the process.
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
			PROCESS_VM_READ,
			FALSE, processID);
		if (hProcess == nullptr)
		{
			return std::nullopt;
		}

		// Get the process name.
		wchar_t szProcessName[MAX_PATH]{};
		GetProcessImageFileName(hProcess, szProcessName, MAX_PATH);
		auto ret = PathFindFileName(szProcessName);
		CloseHandle(hProcess);

		return ret;
	}



	
	static HRESULT EnumProcessModules(HANDLE Process,std::vector<MODULEINFO>* Modules){
		
		HMODULE hMods[1024]{};
		DWORD cbNeeded{};

		RETURN_IF_WIN32_BOOL_FALSE(::EnumProcessModules(Process, hMods, sizeof(hMods), &cbNeeded));

		for (DWORD i = 0; i < (cbNeeded / sizeof(HMODULE)); i++){
			MODULEINFO info{};
			DWORD cb{};
			if (!::GetModuleInformation(Process, hMods[i], &info,sizeof(info))){
				continue;
			}
			Modules->push_back(info);
		}
		RETURN_HR(S_OK);
	}
}
#endif