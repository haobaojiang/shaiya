#include <windows.h>
#include <mutex>
#include <wil/result.h>
#include <wil/resource.h>
#include "../utility/utility.h"
#include "../utility/process.h"
#include "../utility/file.h"
#include "../utility/native.h"
#include "../utility/utility.h"
#include "VMProtectSDK.h"
#include <vector>
#include <map>
#include <optional>

/*
#ifndef _DEBUG
#pragma comment(lib,"VMProtectSDK32.lib")
#endif
		*/

//3.06mbµÄ
namespace Shaiya50
{
	std::optional<DWORD> useruid() {
		auto obj = ShaiyaUtility::read<DWORD>(0x02230ac8);
		if (obj == 0) {
			return std::nullopt;
		}
		auto uid = ShaiyaUtility::read<DWORD>(obj + 0x1f8);
		if (uid == 0) {
			return std::nullopt;
		}
		return std::optional<DWORD>(uid);
	}

	std::optional<DWORD> charid() {
		auto charid =  ShaiyaUtility::read<DWORD>(0x00850764);
		if (charid == 0) {
			return std::nullopt;
		}
		return std::optional<DWORD>(charid);
	}

	std::string charName() {
		return std::string( (const char*)0x00852ab6);
	}

	std::string userid() {
		return std::string((const char*)0x006f71e0);
	}

	namespace antiHacker {

		namespace AntiSpeedHacke {

			DWORD Get_InternalEnumWindows_Addr()
			{
				PBYTE base = (PBYTE)GetProcAddress(GetModuleHandle(L"user32.dll"), "EnumWindows");
				DWORD addr = 0;
				while (TRUE)
				{
					if (*base == 0xe8) {
						addr = DWORD(base + *PINT(base + 1) + 5);
						break;
					}
					base++;
				}
				return addr;
			}

			bool IsHacked()
			{
#ifndef _DEBUG
				VMProtectBegin(__FUNCTION__);
#endif
				auto IsHook = [](PBYTE pfun)->bool {
					if (pfun && *pfun == 0xe9)
						return true;
					return false;
				};


				HMODULE hKernel32 = GetModuleHandle(L"Kernel32.dll");
				HMODULE hWinmmm = GetModuleHandle(L"Winmm.dll");
				HMODULE hUser32 = GetModuleHandle(L"user32.dll");
				HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
				PBYTE pfunGetTickCount64 = NULL;
				PBYTE pfunQueryPerformanceCounter = NULL;
				PBYTE pfunGetTickCount = NULL;
				PBYTE pfuntimeGetTime = NULL;
				PBYTE pfunFindWindow = NULL;
				PBYTE pDbgUiRemoteBreakin = NULL;

				bool bRet = true;

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

				if (hNtdll) {
					pDbgUiRemoteBreakin = (PBYTE)GetProcAddress(hNtdll, "DbgUiRemoteBreakin");
				}



				DWORD InternalEnumWindows_addr = Get_InternalEnumWindows_Addr();

				do
				{
					if ((bRet = IsHook(pfunGetTickCount)) == true) {
						break;
					}

					if ((bRet = IsHook((BYTE*)0x0041c3a1)) == true) {
						break;
					}

					if ((bRet = IsHook((BYTE*)0x0041c41c)) == true) {
						break;
					}

					if ((bRet = IsHook(pfunGetTickCount64)) == true) {
						break;
					}

					if ((bRet = IsHook(pfunQueryPerformanceCounter)) == true) {
						break;
					}

					if ((bRet = IsHook(pfuntimeGetTime)) == true) {
						break;
					}

					if ((bRet = IsHook(pfunFindWindow)) == true) {
						break;
					}

					if ((bRet = IsHook(pDbgUiRemoteBreakin)) == true) {
						break;
					}

					if ((bRet = IsHook((PBYTE)InternalEnumWindows_addr)) == true) {
						break;
					}

				} while (0);

#ifndef _DEBUG
				VMProtectEnd();
#endif	

				return bRet;
			}
		}

		namespace Blacklist {

			bool IsHacked() {

				bool result = false;

				auto callBack = [&](const WCHAR* ProcessFullPath) {
#ifndef _DEBUG
					VMProtectBegin(__FUNCTION__);
#endif
					LARGE_INTEGER fileSize{ 0 };
					auto hr = Utility::File::GetFileSize(ProcessFullPath, &fileSize);
					if (FAILED(hr)) {
						return;
					}


					if (fileSize.QuadPart != 2777088 && fileSize.QuadPart != 6946816 && fileSize.QuadPart!= 33410048) {
#ifdef _DEBUG
						if (fileSize.QuadPart != 149784) {
							return;
						}
#else
						return;
#endif
					}

					std::string md5;
					hr = Utility::File::GetFileMD5(ProcessFullPath, &md5);
					if (FAILED(hr)) {
						wprintf(L"failed to get md5 %ws\n", ProcessFullPath);
						return;
					}

					if (md5 == "256d3b76fbec283e1a00b5b33d872693" ||
						md5 == "e46e7734abcbeba8fc24215a994510af" ||
						md5 =="4ddcd752d58e33697f376afde896d453") {
						result = true;
					}
#ifndef _DEBUG
					VMProtectEnd();
#endif	
				};

				Utility::Process::EnumProcessFileFullPath(callBack);
				return result;
			}
		}

		HRESULT DetectAndCloseHakcingProcHnd() {		
#ifndef _DEBUG
			VMProtectBegin(__FUNCTION__);
#endif
			static std::map<DWORD, DWORD> ignoredPids;
			auto isIgnoredPid = [&](DWORD pid)->bool
			{
				if (pid == 4)
					return true;
				auto iter = ignoredPids.find(pid);
				return iter != ignoredPids.end();
			};

			PSYSTEM_HANDLE_INFORMATION p;
			RETURN_IF_FAILED(Utility::Native::ZwQuerySystemInformation_SysmHndInfo(&p));
			auto cleanup = wil::scope_exit([&]() {delete p; });

			for (DWORD i = 0; i < p->dwCount; i++) {

				const auto& handle = p->Handles[i];

				// not process type
				if (handle.bObjectType != 7) {
					continue;
				}

				// ignore current process
				if (handle.dwProcessId == GetCurrentProcessId()) {
					continue;
				}

				// must have write process memory
				if (!(handle.GrantedAccess & PROCESS_VM_WRITE))
				{
					continue;
				}

				//
				if (isIgnoredPid(handle.dwProcessId)) {
					continue;
				}

				// get handle process id
				HANDLE hObject = reinterpret_cast<HANDLE>(handle.wValue);

				// try to open target process
				wil::unique_process_handle proHnd(OpenProcess(
					PROCESS_DUP_HANDLE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ
					, 0, handle.dwProcessId));
				if (!proHnd) {
					ignoredPids.insert(std::pair<DWORD, DWORD>(handle.dwProcessId, GetTickCount()));
					continue;
				}

				wil::unique_process_handle dupHnd;
				if (!(DuplicateHandle(proHnd.get(),
					hObject,
					GetCurrentProcess(),
					&dupHnd, 0, 0,
					DUPLICATE_SAME_ATTRIBUTES | DUPLICATE_SAME_ACCESS)))
				{
					continue;
				}

				// check handle process id
				if (GetProcessId(dupHnd.get()) != GetCurrentProcessId()) {
					continue;
				}

				// get exe file path
				WCHAR name[MAX_PATH]{};
				if (!GetModuleFileNameEx(proHnd.get(), nullptr, name, MAX_PATH)) {
					continue;
				}

				BOOLEAN trusted = FALSE;
				if (FAILED(Utility::File::HasTrustedSignature(name, &trusted))) {
					continue;
				}
				if (trusted) {
					ignoredPids.insert(std::pair<DWORD, DWORD>(handle.dwProcessId, GetTickCount()));
					continue;
				}

				dupHnd.release();
				// now we try to close source handle
				DuplicateHandle(proHnd.get(), hObject, GetCurrentProcess(), &dupHnd, 0,
					FALSE, DUPLICATE_CLOSE_SOURCE);
			}
			
#ifndef _DEBUG
			VMProtectEnd();
#endif		
			RETURN_HR(S_OK);
		}


		void ThreadProc(_In_  LPVOID lpParameter) {
#ifndef _DEBUG
			VMProtectBegin("ThreadProc");
#endif
			while (true)
			{
				DetectAndCloseHakcingProcHnd();
				if (AntiSpeedHacke::IsHacked() || Blacklist::IsHacked()) {
					Sleep(10 * 1000);
					ExitProcess(0);
				}
				Sleep(5000);
			}
#ifndef _DEBUG
			VMProtectEnd();
#endif
		}

		void Start() {
			_beginthread(ThreadProc, 0, 0);
		}
	}

	namespace ReadCharDecryption {

		void __stdcall Decrypt(_In_ BYTE* Buf) {

#ifndef _DEBUG
			VMProtectBegin(__FUNCTION__);
#endif
			if (Buf[2] < 10) {
				return;
			}

			BYTE signature = Buf[2]+3;
			BYTE len = Buf[3];

			// xor buf
			std::string encryptedStr;
			for (BYTE i = 0; i < len; i++) {
				encryptedStr.push_back(Buf[i+4] ^ signature);
			}
			// base64 buf
			auto decryptedStr = Utility::base64_decode(encryptedStr.c_str());
			memcpy(&Buf[2], decryptedStr.data(), decryptedStr.length());


#ifndef _DEBUG
			VMProtectEnd();
#endif
		}

		ShaiyaUtility::CMyInlineHook g_obj;
		__declspec(naked) void Naked()
		{
			_asm
			{
				mov eax, dword ptr ss : [esp + 0x8]
				push eax
				pushad
				MYASMCALL_1(Decrypt, eax)
				popad
				jmp g_obj.m_pRet
			}
		}

		void Start() {
			g_obj.Hook(0x00584557, Naked);
		}
	}

	namespace collection {

		void threadProc(void*) {

			while (true)
			{
				Sleep(1000);
			}
		}

		void Start() {
			_beginthread(threadProc, 0, 0);
		}
	}


	void Start() {
		antiHacker::Start();
		ReadCharDecryption::Start();
	}
}


