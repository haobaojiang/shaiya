
#include <windows.h>
#include <mutex>
#include "../utility/utility.h"
#include "../utility/native.h"
#include "../utility/file.h"
#include "kiero.h"
#include <assert.h>
#include <wil/resource.h>
#include <map>
#include "VMProtectSDK.h"

namespace Shaiya90
{
	namespace SkillCutting {

		ShaiyaUtility::CMyInlineHook g_fully;


		void* g_fullySkipAddr = reinterpret_cast<void*>(0x0043d635);
		void* g_fullyBlockAddr = reinterpret_cast<void*>(0x004418ca);
		__declspec(naked) void Naked_fully()
		{
			_asm
			{
				cmp eax, 0x2    // normal attack
				je _skip
				cmp eax, 0x9    // skill
				je _skip

				cmp eax, 0x8       // org
				jne _blockAddr
				jmp g_fully.m_pRet


				_blockAddr :
				jmp g_fullyBlockAddr

					_skip :
				jmp g_fullySkipAddr
			}
		}

		void FullySkillCutting() {
			static std::once_flag flag;
			std::call_once(flag, []() {
				g_fully.Hook(reinterpret_cast<void*>(0x0043d62f), Naked_fully, 6);
				});
		}

		void ProcessPacket(PVOID Packet) {
			FullySkillCutting();
		}
	}

	namespace namecolor
	{
		ShaiyaUtility::CMyInlineHook  objNormal, ObjParty;
		DWORD g_randomColor = 0xffffff;
#define MAX_COLORS  (100000)
		DWORD g_colors[MAX_COLORS] = { 0 };

		void StartWorker();


		void __stdcall SetColor(DWORD Player, PDWORD ColotOutput) {

			DWORD charid = *PDWORD(Player + 0x34);

			if (charid > MAX_COLORS - 1) {
				return;
			}

			auto rgb = g_colors[charid];
			if (rgb == ShaiyaUtility::Packet::RandomColor) {
				*ColotOutput = g_randomColor;
				return;
			}
			else if (rgb == 0) {
				return;
			}

			*ColotOutput = rgb;
		}


		void ProcessPacket(PVOID Packet) {

			auto nameColorPacket = reinterpret_cast<ShaiyaUtility::Packet::NameColorContent*>(Packet);

			StartWorker();

			for (DWORD i = 0; i < ARRAYSIZE(nameColorPacket->players); i++) {

				auto charid = nameColorPacket->players[i].charid;
				auto rgb = nameColorPacket->players[i].rgb;


				if (!charid) {
					continue;
				}

				if (!rgb) {
					continue;
				}

				if (charid > MAX_COLORS - 1) {
					return;
				}

				g_colors[charid] = 0xff000000 | rgb;
			}
		}

		void GenerateRandomColor(void*) {

			DWORD dwTemp = 0;
			while (true)
			{
				for (int i = 0; i < ARRAYSIZE(ShaiyaUtility::Packet::s_RandomColorList); i++) {
					g_randomColor = 0xff000000 | ShaiyaUtility::Packet::s_RandomColorList[i];
					Sleep(500);
				}

			}

		}

		__declspec(naked) void Naked_Normal()
		{
			_asm
			{
				mov dword ptr ss : [esp + 0x18] , 0xffffffff
				pushad
				lea eax, [esp + 0x38]
				push eax //color
				push esi
				call SetColor
				popad
				sub eax, 0x4
				jmp objNormal.m_pRet
			}
		}
		__declspec(naked) void Naked_Party()
		{

			_asm
			{
				mov dword ptr ss : [esp + 0x18] , 0xFF00FF00
				pushad
				lea eax, [esp + 0x38]
				push eax //color
				push esi
				call SetColor
				popad
				jmp ObjParty.m_pRet
			}
		}

		void StartWorker() {

			static std::once_flag flag;
			std::call_once(flag, []() {
				objNormal.Hook((PVOID)0x0044b5c6, Naked_Normal, 11);
				ObjParty.Hook((PVOID)0x0044b664, Naked_Party, 8);
				_beginthread(GenerateRandomColor, 0, nullptr);
				});
		}
	}

	namespace killsRanking {

		void ProcessPacket(PVOID p) {

			auto packet = reinterpret_cast<ShaiyaUtility::Packet::KillsRankingContent*>(p);

			static std::once_flag flag;
			std::call_once(flag, [&]() {
				ShaiyaUtility::WriteCurrentProcessMemory(reinterpret_cast<void*>(0x00708298),
					packet->killsNeeded,
					sizeof(packet->killsNeeded));

				ShaiyaUtility::WriteCurrentProcessMemory(reinterpret_cast<void*>(0x00708118),
					packet->normalModePoints,
					sizeof(packet->normalModePoints));


				ShaiyaUtility::WriteCurrentProcessMemory(reinterpret_cast<void*>(0x00708198),
					packet->hardModePoints,
					sizeof(packet->hardModePoints));

				ShaiyaUtility::WriteCurrentProcessMemory(reinterpret_cast<void*>(0x00708218),
					packet->ultimateModePoints,
					sizeof(packet->ultimateModePoints));
				});
		}

	}

	namespace Combination {

		void Process(void* P) {
			{
				BYTE temp[] = { 0x90,0x90 };
				ShaiyaUtility::WriteCurrentProcessMemory((void*)0x004A954D, temp, sizeof(temp));
				ShaiyaUtility::WriteCurrentProcessMemory((void*)0x004A9555, temp, sizeof(temp));
			}

			{
				BYTE temp[] = { 0xE9 ,0xA6 ,0x00 ,0x00 ,0x00 };
				ShaiyaUtility::WriteCurrentProcessMemory((void*)0x004A945B, temp, sizeof(temp));
			}


			{
				BYTE temp[] = { 0xb0,0x01 };
				ShaiyaUtility::WriteCurrentProcessMemory((void*)0x004A9600, temp, sizeof(temp));
			}
		}
	}


	namespace getEnhanceAttack {

		ShaiyaUtility::CMyInlineHook g_obj;

		DWORD g_data[51] = {};

		void Process(void* P) {
			auto packet = static_cast<ShaiyaUtility::Packet::EnhanceAttack*>(P);
			for (auto i = 0; i <ARRAYSIZE(g_data); i++) {
				g_data[i] = packet->values[i];
			}
		}

		DWORD __stdcall  ReadAttack(DWORD Index) {
			if (Index > ARRAYSIZE(g_data)) {
				return 0;
			}
			return g_data[Index];
		}

		__declspec(naked) void Naked()
		{
			_asm {
				push ebx
				push ecx
				push edx
				push ebp
				push esp
				push edi
				push esi
				MYASMCALL_1(ReadAttack, eax)
				pop esi
				pop edi
				pop esp
				pop ebp
				pop edx
				pop ecx
				pop ebx
				add esp, 0x4
				jmp g_obj.m_pRet
			}
		}

		void Start() {
			g_obj.Hook(reinterpret_cast<void*>(0x004A45E4), Naked, 5);
		}
	}


	namespace custompacket
	{
		ShaiyaUtility::CMyInlineHook objPacket;
		static DWORD codeBoundary = ShaiyaUtility::Packet::Code::begin;

		void __stdcall CustomPacket(PVOID P) {

			auto cmd = static_cast<ShaiyaUtility::Packet::Header*>(P);
			switch (cmd->command) {

			case ShaiyaUtility::Packet::Code::nameColor: {
				namecolor::ProcessPacket(P);
				break;
			}

			case ShaiyaUtility::Packet::Code::skillCutting: {
				SkillCutting::ProcessPacket(P);
				break;
			}

			case ShaiyaUtility::Packet::Code::killsRanking: {
				killsRanking::ProcessPacket(P);
				break;
			}

			case ShaiyaUtility::Packet::Code::itemCombines: {
				Combination::Process(P);
				break;
			}
			case ShaiyaUtility::Packet::enhanceAttack: {
				getEnhanceAttack::Process(P);
				break;
			}
			default:
				break;
			}
		}

		__declspec(naked) void Naked1()
		{
			_asm
			{
				cmp eax, codeBoundary
				jl _orginal
				pushad
				mov eax, [esp + 0x28]
				push eax            //packets
				call CustomPacket
				popad
				_orginal :
				cmp eax, 0xFA07
					jmp objPacket.m_pRet
			}
		}



		void Start()
		{
			//	0059BC9B | > \3D 07FA0000         cmp eax, 0xFA07
			objPacket.Hook(reinterpret_cast<void*>(0x0059BC9B), Naked1, 5);
		}
	}

	namespace ijl15Detection {

		void Start() {

			auto removeDetection = [](PVOID addr, PVOID OrgData, PVOID changedata, DWORD len) {
				if (memcmp(OrgData, addr, len) == 0) {
					ShaiyaUtility::WriteCurrentProcessMemory(addr, changedata, len);
				}
			};

			{
				BYTE temp[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0x74 ,0x52 };
				BYTE temp2[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0xEB ,0x52 };
				removeDetection(PVOID(0x0040c513), temp, temp2, sizeof(temp));
			}


			{
				BYTE temp[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0x74 ,0x61 };
				BYTE temp2[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0xeb ,0x61 };
				removeDetection(PVOID(0x0040B5B6), temp, temp2, sizeof(temp));
			}

			// 9.0 3.14mb
			{
				BYTE temp[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0x74 ,0x61 };
				BYTE temp2[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0xeb ,0x61 };
				removeDetection(PVOID(0x0040C88F), temp, temp2, sizeof(temp));
			}

			{
				BYTE temp[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0x74 ,0x61 };
				BYTE temp2[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0xeb ,0x61 };
				removeDetection(PVOID(0x0040bd16), temp, temp2, sizeof(temp));
			}
		}
	}

	namespace ReadCharDecryption {

		void __stdcall Decrypt(_In_ BYTE* Buf) {

			if (Buf[2] < 10) {
				return;
			}

			BYTE signature = Buf[2];
			BYTE len = Buf[3];
			for (BYTE i = 0; i < len; i++) {
				Buf[i + 2] = Buf[i + 4] ^ signature;
			}
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
			g_obj.Hook(0x0059a187, Naked);
		}
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

					if ((bRet = IsHook((BYTE*)0x0041d21c)) == true) {
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

				return bRet;
			}
		}

		HRESULT DetectAndCloseHakcingProcHnd() {
#ifndef _DEBUG
			VMProtectBegin("DetectAndCloseHakcingProcHnd");
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
				if (AntiSpeedHacke::IsHacked()) {
					Sleep(10 * 1000);
					TerminateProcess(GetCurrentProcess(), 0);
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

	void Start()
	{
		custompacket::Start();
		ijl15Detection::Start();
		antiHacker::Start();
		ReadCharDecryption::Start();
		getEnhanceAttack::Start();
	}

}

