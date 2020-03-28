#include <MyClass.h>
#include <windows.h>
#include <MyInlineHook.h>
#include <fstream>
#include <iostream>
#include <time.h>
#include <detours.h>
#include <process.h>
#include <WinSock2.h>
#include <TlHelp32.h>
#pragma comment(lib,"Ws2_32.lib")
#pragma comment (lib,"User32.lib")
#include <iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")
#include <map>
#include <vector>
#include "PictureWidthHeight.h"

using std::vector;






//3.06mb的
namespace Shaiya50
{

	Folder g_root_directory;
	char g_sahPath[MAX_PATH] = { 0 };
	char g_safPath[MAX_PATH] = { 0 };

	namespace sahReader {
		bool read_sah(Folder* folder, char* strPath);
	};

	void InitSahReader() {
		
		char strTemp[MAX_PATH] = { 0 };
		GetModuleFileNameA(GetModuleHandle(0), strTemp, MAX_PATH);
		PathRemoveFileSpecA(strTemp);
		sprintf(g_sahPath,"%s\\data.sah",strTemp);
		sprintf(g_safPath, "%s\\data.saf", strTemp);

		sahReader::read_sah(&g_root_directory, g_sahPath);
	}


	std::string getItemname(DWORD itemType, DWORD itemTypeid) {
		DWORD dwRet = 0;
		DWORD dwCall = 0x00461110;
		_asm {
			push itemTypeid
			push itemType
			mov ecx, 0x856F3C
			call dwCall
			mov dwRet, eax
		}
		if (!dwRet) {
			return  "";
		}
		const char* pItemName = PCHAR(*PDWORD(dwRet));
		return std::string(pItemName);
	}

	std::string getItemname(DWORD itemid) {
		return getItemname(itemid / 1000, itemid % 1000);
	}

	DWORD g_dwCallDrawBG = 0x004FF1A0;
	void drawBackground(int x, int y, DWORD width, DWORD height) {
		
		_asm {
			push 0xCD1D1F24
			push height
			push width
			push y
			push x
			call g_dwCallDrawBG
			add esp, 0x14
		}
	}

	void drawWords_sub(const char* strWords, DWORD x, DWORD y, DWORD RGB = 0xffffff) {

		DWORD dwCall = 0x0049e4d0;

		DWORD r = (RGB & 0xff0000) >> 16;
		DWORD g = (RGB & 0xff00) >> 8;
		DWORD b = (RGB & 0xff);

		_asm {
			push strWords
			push 0x0
			push 0x0
			push b  //b
			push g  //g
			push r  //r
			push y
			push x
			call dwCall
			add esp, 0x20
		}
	}

	DWORD drawWords(const char* strWords, DWORD x, DWORD y, DWORD RGB = 0xffffff, DWORD lineHeight = 15) {
		DWORD lines = 0;
		if (!strWords || strWords[0] == '\0') {
			return lines;
		}

		char* pstrDraw = new char[strlen(strWords) * 2];
		strcpy(pstrDraw, strWords);


		char *delim = "\n";
		char* p = strtok(pstrDraw, delim);
		while (p) {
			drawWords_sub(p, x, y, RGB);
			y += lineHeight;
			lines++;
			p = strtok(NULL, delim);
		}
		delete[] pstrDraw;
		return lines;
	}

	void drawImage(void* pimage, DWORD x, DWORD y) {
		PVOID pTemp = (PVOID)0x005378d0;  //0059D780  /$  8B11          mov edx,dword ptr ds:[ecx]
		_asm
		{
			push y
			push x
			mov ecx, pimage
			call pTemp
		}
	}

	void getDrawPosByPlayerPos(DWORD player, float (&pos)[3]) {


		DWORD playerPos = (player + 0x10);
		DWORD dwCall = 0x0052FEC0;
		_asm {
			push playerPos
			push pos
			mov ecx, 0x21F1ED0
			call dwCall
		}
	}

	void enumSub(DWORD Eax, vector<DWORD> vecAddr, vector<DWORD>& vecPlayers) {
		auto iter = find(vecAddr.begin(), vecAddr.end(), Eax);
		if (iter != vecAddr.end()) {
			return;
		}
		vecAddr.push_back(Eax);


		BYTE sinature = *PBYTE(Eax + 21);
		if (!sinature) {
			DWORD player = *PDWORD(Eax + 16);
			if (player) {
				auto iter = find(vecPlayers.begin(), vecPlayers.end(), player);
				if (iter == vecPlayers.end()) {
					vecPlayers.push_back(player);
				}
			}
		}

		DWORD leftNode = *PDWORD(Eax);
		DWORD rightNode = *PDWORD(Eax + 8);

		{
			enumSub(leftNode, vecAddr, vecPlayers);
		}

		{
			enumSub(rightNode, vecAddr, vecPlayers);
		}
	}

	void enumPlayers(std::vector<DWORD>& vecPlayers) {
		DWORD base = 0x007071e0;
		base += 0x13d874;
		base = *PDWORD(base + 4);
		base = *PDWORD(base + 4);
		std::vector<DWORD> vecAddr;
		enumSub(base, vecAddr, vecPlayers);
	}

	DWORD g_sendAddr = 0;
	void sendPacket(void* buff, DWORD len) {
		if (g_sendAddr) {
			void(*InternalCall)(PVOID, DWORD) = (void(__cdecl *)(PVOID, DWORD))g_sendAddr;
			(*InternalCall)(buff, len);
		}
	}


	DWORD ScanForAddress(DWORD beginAddr, DWORD scanLen, const void* sinature, BYTE sinatureLen)
	{

		for (DWORD i = 0; i < scanLen; i++) {
			DWORD aadr = beginAddr + i;
			if (memcmp(PVOID(aadr), sinature, sinatureLen) == 0) {
				return aadr;
			}
		}
		return 0;
	}


	void getSendAddr()
	{
		DWORD hModule = 0x00400000;
		PIMAGE_DOS_HEADER pDosH = (PIMAGE_DOS_HEADER)hModule;
		PIMAGE_NT_HEADERS32 pNtH = (PIMAGE_NT_HEADERS32)(hModule + pDosH->e_lfanew);
		PIMAGE_SECTION_HEADER pSecH = IMAGE_FIRST_SECTION(pNtH);

		for (auto i = 0; i < pNtH->FileHeader.NumberOfSections; i++)
		{
			if (pSecH[i].VirtualAddress + hModule == 0x00401000)  //就算加完壳，也是这个起始地址的
			{
				DWORD scanHead = pSecH[i].VirtualAddress + hModule;
				DWORD scanSize = pSecH[i].Misc.VirtualSize;

				BYTE Signature[] = { 0x8B, 0xCB, 0x8B, 0xD1, 0xC1, 0xE9, 0x02, 0x8D, 0x43, 0x02,0x66, 0x89, 0x44, 0x24, 0x20 };

				DWORD sendAddr = ScanForAddress(scanHead, scanSize, Signature, sizeof(Signature));
				if (sendAddr) {
					g_sendAddr = sendAddr - 0x38;
					return;
				}		
			}
		}
		OutputDebugString(L"cuold not find send addr!!");
	}

	void removeDetection(PVOID addr, PVOID OrgData, PVOID changedata, DWORD len) {
		if (memcmp(OrgData, addr, len) == 0) {
			MyWriteProcessMemory(GetCurrentProcess(), addr, changedata, len);
		}
	}
	void removeijl15Detection() {


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

		{
			BYTE temp[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0x74 ,0x61 };
			BYTE temp2[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0xeb ,0x61 };
			removeDetection(PVOID(0x0040C88F), temp, temp2, sizeof(temp));
		}

		{
			BYTE temp[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0x74 ,0x61};
			BYTE temp2[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0xeb ,0x61 };
			removeDetection(PVOID(0x0040bd16), temp, temp2, sizeof(temp));
		}

	}

	namespace antiHacker {

		typedef enum _HARDERROR_RESPONSE_OPTION {
			OptionAbortRetryIgnore,
			OptionOk,
			OptionOkCancel,
			OptionRetryCancel,
			OptionYesNo,
			OptionYesNoCancel,
			OptionShutdownSystem,
			OptionOkNoWait,
			OptionCancelTryContinue
		} HARDERROR_RESPONSE_OPTION;
		typedef LONG(WINAPI *pfnZwRaiseHardError)(LONG ErrorStatus, ULONG NumberOfParameters, ULONG UnicodeStringParameterMask, PULONG_PTR Parameters, HARDERROR_RESPONSE_OPTION ValidResponseOptions, PULONG Response);
		typedef struct _UNICODE_STRING {
			USHORT Length;
			USHORT MaximumLength;
			PWCH   Buffer;
		} UNICODE_STRING;


		BOOL DebugPrivilege(LPCTSTR PName, BOOL bEnable)
		{
			BOOL              bResult = TRUE;
			HANDLE            hToken;
			TOKEN_PRIVILEGES  TokenPrivileges;

			if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
			{
				bResult = FALSE;
				return bResult;
			}
			TokenPrivileges.PrivilegeCount = 1;
			TokenPrivileges.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;
			LookupPrivilegeValue(NULL, PName, &TokenPrivileges.Privileges[0].Luid);
			AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
			if (GetLastError() != ERROR_SUCCESS)
			{
				bResult = FALSE;
			}
			CloseHandle(hToken);
			return bResult;
		}
		void TakeBuleScreen() {
			UNICODE_STRING str = { 8, 10, L"test" };
			ULONG x, args[] = { 0x12345678, 0x87654321, (ULONG)&str };
			HMODULE hDll = GetModuleHandle(TEXT("ntdll.dll"));
			pfnZwRaiseHardError ZwRaiseHardError = (pfnZwRaiseHardError)GetProcAddress(hDll, "ZwRaiseHardError");

			DebugPrivilege(SE_SHUTDOWN_NAME, TRUE);
			ZwRaiseHardError(0xC000021A, 3, 4, args, OptionShutdownSystem, &x);
		}


		namespace AntipacketEditor {

			bool isFound(PVOID addr, PVOID checkaddr, PVOID Signature, DWORD len) {
				if (memcmp(addr, Signature, len) == 0) {
					if (*PBYTE(checkaddr) == 0xe9) {
						return true;
					}
				}
				return false;
			}


			bool IsHacked() {

				//tommic 服的  0x005880E5是包含特征码  检测0x005880e0是否0xe9
				{
					BYTE Signature[] = { 0xE8 ,0x46  ,0x27  ,0x03  ,0x00  ,0xF6  ,0x05  ,0x44  ,0x5B  ,0x23  ,0x02  ,0x40 };
					if (isFound((PVOID)0x005880E5, PVOID(0x005880e0), Signature, sizeof(Signature))) {
						return true;
					}

				}
				return false;
			}
		}


		namespace AntiDeug {

			bool g_IsHacked = false;

			void OnGetTickCount()
			{
				if (IsDebuggerPresent())
				{
					g_IsHacked = true;
					return;
				}
				DWORD dTime1;
				DWORD dTime2;
				dTime1 = GetTickCount();
				GetCurrentProcessId();
				GetCurrentProcessId();
				GetCurrentProcessId();
				GetCurrentProcessId();
				dTime2 = GetTickCount();
				if (dTime2 - dTime1 > 200)
				{
					g_IsHacked = true;
				}
			}


			void ThreadProc(_In_ LPVOID lpParameter)
			{
				while (1)
				{
					OnGetTickCount();
					Sleep(300);
				}
			}

			bool IsHacked() {
				return g_IsHacked;
			}


			void Init() {
				_beginthread(ThreadProc, 0, 0);
			}

		}

		namespace AutoKeyCHeck {



			HMODULE g_hInput8 = NULL;
			HMODULE get_GetDeviceStateAddr()
			{
				HMODULE hModDInput8 = NULL;
				hModDInput8 = GetModuleHandle(L"dinput8.dll");
				if (!hModDInput8) {
					return hModDInput8;
				}
				MODULEINFO mInf;
				GetModuleInformation(GetCurrentProcess(), hModDInput8, &mInf, sizeof(mInf));
				return (HMODULE)findPattern((DWORD)mInf.lpBaseOfDll, DWORD((DWORD)mInf.lpBaseOfDll + (DWORD)mInf.SizeOfImage), "8B FF 55 8B EC 53 56 57 8B 7D 08 8B 07 33 DB 53");  //找特证码
			}


			bool IsInputHooked()
			{
				if (!g_hInput8)
					g_hInput8 = get_GetDeviceStateAddr();

				if (g_hInput8&&*PBYTE(g_hInput8) == 0xe9)
				{
					return true;
				}
				return false;
			}

			void Init() {
				g_hInput8 = get_GetDeviceStateAddr();
			}

			bool IsHacked()
			{
				return  IsInputHooked();
			}
		}

		namespace AntiSpeedHacke {

			bool IsHook(PBYTE pfun)
			{
				if (pfun&&*pfun == 0xe9)
					return true;
				return false;
			}

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
					pDbgUiRemoteBreakin= (PBYTE)GetProcAddress(hNtdll, "DbgUiRemoteBreakin");
				}



				DWORD InternalEnumWindows_addr = Get_InternalEnumWindows_Addr();

				do
				{
					if ((bRet = IsHook(pfunGetTickCount)) == true) {
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

		namespace CheckWindow {

			DWORD g_gameWindowCount = 0;
			DWORD g_currentProcessId = 0;

			typedef struct {
				std::string ClassName;
				std::string WinText;
			}ANTIWINDOWCLASS, *PANTIWINDOWCLASS;

			ANTIWINDOWCLASS g_window[] = {
				{ "Button","当HP<" },         //classname textname
				{ "Button","When HP<" }
			};

			bool g_IsWindowHacked = false;

			DWORD g_wuWangWindow = 0;




			void wuwangWindowCheck(HWND hWnd, LPARAM lParam) {
				char buff[1024] = { 0 };
				GetClassNameA(hWnd, buff, 1024);
				for (DWORD i = 0; i < _countof(g_window); i++) {
					if (g_window[i].ClassName.compare(buff) == 0) {
						ZeroMemory(buff, 1024);
						GetWindowTextA(hWnd, buff, 1024);
						if (g_window[i].WinText.compare(buff) == 0) {
							g_IsWindowHacked = true;
							return ;
						}
					}
				}

			}







			//检测天天
			DWORD g_WindowCount = 0;
			BOOL CALLBACK EnumChildWindowCallBack(HWND hWnd, LPARAM lParam)
			{
				DWORD pid = 0;
				GetWindowThreadProcessId(hWnd, &pid);
				if (g_currentProcessId == pid) {
					++g_WindowCount;
					EnumChildWindows(hWnd, EnumChildWindowCallBack, 0);
				}

				return TRUE;
			}

			BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)
			{
				DWORD pid = 0;
				GetWindowThreadProcessId(hWnd, &pid);
				if (g_currentProcessId == pid) {
					++g_WindowCount;
					EnumChildWindows(hWnd, EnumChildWindowCallBack, 0);
				}
				return true;
			}


			void checkTT() {
				g_WindowCount = 0;
				EnumWindows(EnumWindowCallBack, NULL);
#ifdef _DEBUG
				char g_str[20] = { 0 };
				sprintf(g_str, "%d", g_WindowCount);
				OutputDebugStringA(g_str);
#endif
				if (g_WindowCount > 20) {
					g_IsWindowHacked = true;
				}
			}


			bool IsHacked()
			{
				if (!g_currentProcessId) {
					g_currentProcessId = GetCurrentProcessId();
				}
				
				//check TT
				checkTT();
				return g_IsWindowHacked;
			}
		}

		namespace HeatBaetCheck {

			CMyInlineHook g_objRecivedPacket;
			bool g_EnableHeartBeat = false;


			void __stdcall PacketProcess(PVOID p)
			{
				PANTIHACKPACKET packet = (PANTIHACKPACKET)p;

				if (packet->cmd != PacketOrder::AntiHacker) {
					return;
				}



				switch (packet->actiontype)
				{
				case action_machinecode:
					break;

				case action_heartbeat:
					g_EnableHeartBeat = true;
					break;

				case action_screenshort:
					break;

				default:
					break;
				}
			}

			bool isTime(PDWORD pTime, DWORD deLay) {

				DWORD CurrentTime = GetTickCount();
				DWORD nextTime = *pTime;



				if (!nextTime) {
					*pTime = CurrentTime + deLay;
					return false;
				}


				if (nextTime > CurrentTime) {
					return false;
				}
				else {
					*pTime = 0;
					return true;
				}
			}

			void HeartBeatPrc()
			{
				DWORD static NextTime = GetTickCount();

				if (isTime(&NextTime,10*1000) && g_EnableHeartBeat) {
					ANTIHACKPACKET stc;
					ZeroMemory(&stc, sizeof(stc));
					stc.cmd = PacketOrder::AntiHacker;
					stc.actiontype = Antibehavior::action_heartbeat;
					sendPacket(&stc, sizeof(stc));
				}

			}
		}

		namespace MyAPIHook {


			bool g_IsHacked = false;


			typedef LRESULT(WINAPI* fun_CallWindowProcA)(
				_In_ WNDPROC lpPrevWndFunc,
				_In_ HWND    hWnd,
				_In_ UINT    Msg,
				_In_ WPARAM  wParam,
				_In_ LPARAM  lParam
				);

			typedef HANDLE(WINAPI* fun_CreateThread)(
				_In_opt_  LPSECURITY_ATTRIBUTES  lpThreadAttributes,
				_In_      SIZE_T                 dwStackSize,
				_In_      LPTHREAD_START_ROUTINE lpStartAddress,
				_In_opt_  LPVOID                 lpParameter,
				_In_      DWORD                  dwCreationFlags,
				_Out_opt_ LPDWORD                lpThreadId
				);


			typedef HANDLE
			(WINAPI*
				fun_CreateFileA)(
					__in     LPCSTR lpFileName,
					__in     DWORD dwDesiredAccess,
					__in     DWORD dwShareMode,
					__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
					__in     DWORD dwCreationDisposition,
					__in     DWORD dwFlagsAndAttributes,
					__in_opt HANDLE hTemplateFile
					);

			typedef BOOL
			(WINAPI*
				fun_CloseHandle)(
					__in HANDLE hObject
					);

			typedef int (PASCAL FAR* fun_connect)(
				__in SOCKET s,
				__in_bcount(namelen) const struct sockaddr FAR *name,
				__in int namelen);


			



			char g_strfile[MAX_PATH] = { 0 };
			HANDLE g_hfile = NULL;
			DWORD g_CreateThreadCount = 0;




			fun_CallWindowProcA  g_pOldCallWindowProcA = CallWindowProcA;
			fun_CreateThread g_pOldCreateThread = CreateThread;
			fun_CreateFileA g_pOldCreateFileA = CreateFileA;
			fun_CloseHandle g_pOldCloseHandle = CloseHandle;
			fun_connect  g_pOldconnect = connect;


			int PASCAL FAR Myconnect(
				__in SOCKET s,
				__in_bcount(namelen) const struct sockaddr FAR *name,
				__in int namelen)
			{
				int nRet = g_pOldconnect(s, name, namelen);
				return nRet;
			}

			BOOL WINAPI MyCloseHandle(__in HANDLE hObject)
			{
				if (hObject == g_hfile) {
					return true;   //不去关闭sah的句柄
				}
				return g_pOldCloseHandle(hObject);
			}




			HANDLE WINAPI
				MyCreateFileA(
					__in     LPCSTR lpFileName,
					__in     DWORD dwDesiredAccess,
					__in     DWORD dwShareMode,
					__in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
					__in     DWORD dwCreationDisposition,
					__in     DWORD dwFlagsAndAttributes,
					__in_opt HANDLE hTemplateFile)
			{

				if (strstr(lpFileName, ".sah") != NULL) {
					strcpy(g_strfile, lpFileName);
					g_hfile = g_pOldCreateFileA(lpFileName, dwDesiredAccess, 0, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
					return g_hfile;
				}
				return g_pOldCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
			}



			HANDLE WINAPI MyCreateThread(
				_In_opt_  LPSECURITY_ATTRIBUTES  lpThreadAttributes,
				_In_      SIZE_T                 dwStackSize,
				_In_      LPTHREAD_START_ROUTINE lpStartAddress,
				_In_opt_  LPVOID                 lpParameter,
				_In_      DWORD                  dwCreationFlags,
				_Out_opt_ LPDWORD                lpThreadId)
			{
				InterlockedIncrement(&g_CreateThreadCount);
				return g_pOldCreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, lpThreadId);
			}


			LRESULT WINAPI MyCallWindowProcA(_In_ WNDPROC lpPrevWndFunc, _In_ HWND    hWnd, _In_ UINT    Msg, _In_ WPARAM  wParam, _In_ LPARAM  lParam)
			{
				return 0;

				DWORD Pid = GetCurrentProcessId();

				HANDLE  hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, Pid);
				if (hModuleSnap == INVALID_HANDLE_VALUE)
				{
					return g_pOldCallWindowProcA(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
				}

				MODULEENTRY32 me32 = { sizeof(MODULEENTRY32) };
				if (!Module32First(hModuleSnap, &me32))
				{
					CloseHandle(hModuleSnap);
					return g_pOldCallWindowProcA(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
				}

				bool bLegal = false;
				do
				{
					if (DWORD(me32.modBaseAddr) < (DWORD)lpPrevWndFunc &&
						DWORD(me32.modBaseAddr) + me32.modBaseSize > (DWORD)lpPrevWndFunc) {
						bLegal = true;
						break;
					}
				} while (Module32Next(hModuleSnap, &me32));
				CloseHandle(hModuleSnap);

				if (bLegal)
					return g_pOldCallWindowProcA(lpPrevWndFunc, hWnd, Msg, wParam, lParam);
			}

			BOOL UnInitDetours()
			{
				BOOL bRet = FALSE;
				__try {
					if (DetourTransactionBegin() != NO_ERROR)
						__leave;
					if (DetourUpdateThread(::GetCurrentThread()) != NO_ERROR)
						__leave;

					//DetourDetach(&(PVOID&)g_pOldCallWindowProcA, MyCallWindowProcA);
					DetourDetach(&(PVOID&)g_pOldCreateFileA, MyCreateFileA);



					if (DetourTransactionCommit() != NO_ERROR)
						__leave;
					bRet = TRUE;
				}
				__finally {
					;
				}

				return bRet;
			}

			BOOL InitDetours()
			{
				BOOL bRet = FALSE;
				__try {
					if (DetourTransactionBegin())
						__leave;

					if (DetourUpdateThread(::GetCurrentThread()))
						__leave;


					DetourAttach(&(PVOID&)g_pOldCreateFileA, MyCreateFileA);
					DetourAttach(&(PVOID&)g_pOldconnect, Myconnect);
					DetourAttach(&(PVOID&)g_pOldCloseHandle, MyCloseHandle);


					if (DetourTransactionCommit() != NO_ERROR)
						__leave;
					bRet = TRUE;

				}
				__finally {
					;
				}
				return bRet;
			}



			void Init() {
				InitDetours();
			}

			bool IsHacked()
			{
				return g_IsHacked;
			}
		}



		namespace AntiIP
		{

			std::string g_BlockedIp[] = {
				"121.41.63.234"
			};

			bool IsHacked()
			{

				bool bRet = false;
				DWORD dwBuf = 0;
				MIB_TCPTABLE_OWNER_PID*  pBuf = NULL;
				GetExtendedTcpTable(pBuf, &dwBuf, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
				pBuf = (MIB_TCPTABLE_OWNER_PID*)new BYTE[dwBuf];

				if (NO_ERROR == GetExtendedTcpTable(pBuf, &dwBuf, FALSE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0))
				{
					char    strState[MAX_PATH];
					struct  in_addr    inadLocal, inadRemote;
					DWORD   dwRemotePort = 0;
					char    szLocalIp[MAX_PATH];
					char    szRemIp[MAX_PATH];
					char szItemText[MAX_PATH] = { 0 };

					for (UINT i = 0; i < pBuf->dwNumEntries; i++)
					{
						switch (pBuf->table[i].dwState)
						{
						case MIB_TCP_STATE_CLOSED:
							strcpy(strState, "CLOSED");
							break;
						case MIB_TCP_STATE_TIME_WAIT:
							strcpy(strState, "TIME_WAIT");
							break;
						case MIB_TCP_STATE_LAST_ACK:
							strcpy(strState, "LAST_ACK");
							break;
						case MIB_TCP_STATE_CLOSING:
							strcpy(strState, "CLOSING");
							break;
						case MIB_TCP_STATE_CLOSE_WAIT:
							strcpy(strState, "CLOSE_WAIT");
							break;
						case MIB_TCP_STATE_FIN_WAIT1:
							strcpy(strState, "FIN_WAIT1");
							break;
						case MIB_TCP_STATE_ESTAB:
							strcpy(strState, "ESTAB");
							break;
						case MIB_TCP_STATE_SYN_RCVD:
							strcpy(strState, "SYN_RCVD");
							break;
						case MIB_TCP_STATE_SYN_SENT:
							strcpy(strState, "SYN_SENT");
							break;
						case MIB_TCP_STATE_LISTEN:
							strcpy(strState, "LISTEN");
							break;
						case MIB_TCP_STATE_DELETE_TCB:
							strcpy(strState, "DELETE");
							break;
						default:
							//	TRACE("Error: unknown state!\n");
							break;
						}
						inadLocal.s_addr = pBuf->table[i].dwLocalAddr;

						if (strcmp(strState, "LISTEN") != 0)
						{
							dwRemotePort = pBuf->table[i].dwRemotePort;
						}
						else
							dwRemotePort = 0;


						inadRemote.s_addr = pBuf->table[i].dwRemoteAddr;
						strcpy(szLocalIp, inet_ntoa(inadLocal));
						strcpy(szRemIp, inet_ntoa(inadRemote));
						sprintf(szItemText, "TCP %s:%u  %s:%u  %s",
							szLocalIp, ntohs((unsigned short)(0x0000FFFF & pBuf->table[i].dwLocalPort)),
							szRemIp, ntohs((unsigned short)(0x0000FFFF & dwRemotePort)),
							strState);

						//我们只关心这
						for (DWORD i = 0; i < _countof(g_BlockedIp); i++) {
							if (g_BlockedIp[i].compare(szRemIp) == 0) {
								delete[] pBuf;
								bRet = true;
								return bRet;
							}
						}

					}
				}

				delete[] pBuf;
				return bRet;

			}
		}







		void ThreadProc(_In_  LPVOID lpParameter)
		{
			Sleep(5000);

			EnableDebugPrivilege(true);


			AutoKeyCHeck::Init();
			AntiDeug::Init();
			MyAPIHook::Init();

#ifndef _DEBUG
			__asm
			{
				_emit 0xEB
				_emit 0x08
				_emit 0x56
				_emit 0x4D
				_emit 0x42
				_emit 0x45
				_emit 0x47
				_emit 0x49
				_emit 0x4E
				_emit 0x31
			}
 #endif

			while (1)
			{
				
				if (AntiSpeedHacke::IsHacked()) {
					break;
				}
				if (AutoKeyCHeck::IsHacked()) {
					break;
				}

				if (CheckWindow::IsHacked()) {
					break;
				}

				if (AntiIP::IsHacked()) {
					break;
				}


				if (AntiDeug::IsHacked()) {
#ifndef _DEBUG
					TakeBuleScreen();
 #endif
					break;
				}

				HeatBaetCheck::HeartBeatPrc();			
				Sleep(10 * 1000);
			}



#ifndef _DEBUG
			__asm
			{
				_emit 0xEB
				_emit 0x08
				_emit 0x56
				_emit 0x4D
				_emit 0x45
				_emit 0x4E
				_emit 0x44
				_emit 0x31
				_emit 0x00
				_emit 0x00
			}
#endif

			while (1)
			{
				BYTE temp[] = { 0xff,0xff,0xff,0xff };
				sendPacket(temp, sizeof(temp));
/*				OutputDebugStringA("1111");*/
				Sleep(1000);
			}

		}



		void start() {
			static bool isinit = false;
			if (isinit)
				return;
			isinit = true;
			_beginthread(ThreadProc, 0, 0);
		}
	}

	namespace sahReader {

		void read_folder(Folder* folder, std::ifstream* input) {



			//读文件数量 再根据数量循环
			int number_files_current_dir;
			input->read((char*)&number_files_current_dir, 4);
			folder->file_count = number_files_current_dir;
			for (int i = 0; i < folder->file_count; i++) {
				File current_file;


				input->read((char*)&current_file.name_length, 4);

				input->read(current_file.file_name, current_file.name_length);
				input->read((char*)&current_file.offset, 8);
				input->read((char*)&current_file.length, 8);

				folder->files.push_back(current_file);
			}



			//读文件夹数量 再根据数量循环
			int sub_folder_count;
			input->read((char*)&sub_folder_count, 4);
			folder->sub_folder_count = sub_folder_count;
			for (int i = 0; i < folder->sub_folder_count; i++) {
				Folder directory;
				directory.parent_folder = folder;
				input->read((char*)&directory.name_length, 4);
				input->read(directory.folder_name, directory.name_length); //只读了文件夹的名
				read_folder(&directory, input);
				folder->folders.push_back(directory);
			}
		}

		bool read_sah(Folder* folder, char* strPath) {
			std::ifstream* input = new std::ifstream(strPath, std::ifstream::in | std::ifstream::binary);
			if (!input->is_open()) {
				std::cout << "Unable to open the file specified!" << std::endl;
				false;
			}

			char header[3];
			input->read((char*)&header, 3);
			input->ignore(4);
			int total_number_files;
			input->read((char*)&total_number_files, 4);
			input->ignore(45);
			strcpy(folder->folder_name, "data");
			sahReader::read_folder(folder, input);
			input->close();
			return true;
		}



		PFile getShiyaFile(const char* filepath, Folder* pMainFolder) {
			char* pstrFile = new char[strlen(filepath) * 2];
			strcpy(pstrFile, filepath);

			char *delim = "\\";
			char* p = strtok(pstrFile, delim);

			PFolder pFolder = pMainFolder;

			PFile pRet = NULL;

			while (1) {

				char* pNext = strtok(NULL, delim);


				//特殊情况
				if (stricmp(p, "data") == 0) {
					p = pNext;
					continue;
				}

				//找文件夹
				if (pNext) {
					bool isFind = false;
					for (auto iter = pFolder->folders.begin(); iter != pFolder->folders.end(); iter++) {
						if (stricmp(iter->folder_name, p) == 0) {
							isFind = true;
							pFolder = &(*iter);
							break;
						}
					}
					p = pNext;
					if (!isFind) {
						goto __exit;
					}
				}
				//找文件
				else {
					for (auto iter = pFolder->files.begin(); iter != pFolder->files.end(); iter++) {
						if (stricmp(iter->file_name, p) == 0) {
							pRet = &(*iter);
							goto __exit;
						}
					}
					goto __exit;
				}
			}
		__exit:

			delete[] pstrFile;
			return pRet;
		}
	}

	namespace trademessage
	{
		CMyInlineHook obj1, obj2, obj3;
		void __stdcall ChangeMessageColor(BYTE*);

		PVOID g_pfunChangeColor2 = ChangeMessageColor;


		CRITICAL_SECTION g_csTradeMessagecs;
		DWORD g_dwRgb = 0;



		void __stdcall SetColor(BYTE* strPacket)
		{
			DWORD dwPacket = (DWORD)strPacket;
			strPacket += 0x18;
			strPacket += strlen((char*)strPacket);


			DWORD dwCall = 0x005801D0;
			g_dwRgb = 0xfff3dea0;      //此处GB是反过来的

									   //如果有颜色就取颜色,否则就默认
			if (*(PDWORD)strPacket != 0)
			{
				(PBYTE(&g_dwRgb))[3] = 0xff;
				(PBYTE(&g_dwRgb))[2] = strPacket[1];
				(PBYTE(&g_dwRgb))[1] = strPacket[2];
				(PBYTE(&g_dwRgb))[0] = strPacket[3];
			}

			EnterCriticalSection(&g_csTradeMessagecs);
			_asm {
				push dwPacket
				call dwCall
				add esp, 0x4
			}
			LeaveCriticalSection(&g_csTradeMessagecs);
		}

		//屏幕左下角颜色
		void __stdcall ChangeMessageColor(BYTE* color)
		{
			color[0] = (PBYTE(&g_dwRgb))[2];
			color[1] = (PBYTE(&g_dwRgb))[1];
			color[2] = (PBYTE(&g_dwRgb))[0];
		}


		//设置左下角颜色
		__declspec(naked) void Naked2()
		{
			_asm
			{
				pushad
				lea eax, [eax + 0x108]
				push eax
				call g_pfunChangeColor2
				popad
				retn 0x4
			}
		}

		//封包最开始处理的地方,用来保存颜色
		__declspec(naked) void Naked1()
		{
			_asm
			{
				pushad
				MYASMCALL_1(SetColor, eax)
				popad
				retn
			}
		}
		//普通玩家的交易频道
		__declspec(naked) void Naked3()
		{
			_asm
			{
				pushad
				MYASMCALL_1(SetColor, ecx)
				popad
				retn
			}
		}

		void Start()
		{
			bool static IsInit = false;
			if (IsInit) {
				return;
			}
			IsInit = true;
			obj1.Hook((PVOID)0x00585d7f, Naked1, 10);//GM交易频道
			obj2.Hook((PVOID)0x0041bfc2, Naked2, 7);
			obj3.Hook((PVOID)0x005855d5, Naked3, 6);

			InitializeCriticalSection(&g_csTradeMessagecs);
		}
	}

	namespace namecolor
	{
		bool g_bEnable = false;
		CMyInlineHook  objNormal, ObjParty, objEnemy;
		CRITICAL_SECTION g_cs;
		cmd_color g_stcColor[200];


		DWORD g_dwRandomSign = 0xffff7fff;  //如果服务器传来的是这个颜色值,则说明是随机颜色
		DWORD g_dwRandCoor = 0xffffffff;

		void __stdcall ChangeColor(DWORD, PDWORD);
		PVOID g_pfunChangeColor = ChangeColor;

		void __stdcall ChangeColor(DWORD dwPlayer, PDWORD pColor)
		{
			DWORD dwCharid = *PDWORD(dwPlayer + 0x34);
			for (DWORD i = 0; i < _countof(g_stcColor); i++)
			{
				if (!g_stcColor[i].dwCharid)
					break;


				//判断是否随机颜色
				if (g_stcColor[i].dwCharid == dwCharid)
				{
					if (g_stcColor[i].dwRGB == g_dwRandomSign)
						*pColor = g_dwRandCoor;
					else
						*pColor = g_stcColor[i].dwRGB;
					break;
				}

			}
		}


		void fun_getvipcolor(PVOID pColor)
		{
			PNAMECOLOR p = (PNAMECOLOR)pColor;
			memcpy(g_stcColor, p->colors, sizeof(p->colors));
		}

		__declspec(naked) void Naked_Normal()
		{
			_asm
			{
				mov dword ptr ss : [esp + 0x18], 0xffffffff
				pushad
				lea eax, [esp + 0x38]
				push eax //color
				push esi
				call g_pfunChangeColor
				popad
				sub eax, 0x4
				jmp objNormal.m_pRet
			}
		}
		__declspec(naked) void Naked_Party()
		{

			_asm
			{
				mov dword ptr ss : [esp + 0x18], 0xFF00FF00
				pushad
				lea eax, [esp + 0x38]
				push eax //color
				push esi
				call g_pfunChangeColor
				popad
				jmp ObjParty.m_pRet
			}
		}
		DWORD WINAPI ThreadProc1(_In_  LPVOID lpParameter)
		{
			DWORD dwTemp = 0;
			while (true)
			{
				Sleep(100);
				dwTemp += 100;
				if (dwTemp > 0xffffff)
				{
					dwTemp = 0;
				}
				g_dwRandCoor = 0xff000000 + dwTemp;
			}
		}


		void Start()
		{
			InitializeCriticalSection(&g_cs);
			//正常角色名字颜色地方
			objNormal.Hook((PVOID)0x0044bd26, Naked_Normal, 11);
			//组队
			ObjParty.Hook((PVOID)0x0044bdc4, Naked_Party, 8);

			::CreateThread(0, 0, ThreadProc1, 0, 0, 0);
		}
	}

	namespace customMessage {


		void sendMessage(char* strMessage, DWORD MessageType = 16) {
			DWORD dwCall = 0x0041e4e0;
			_asm {
				push strMessage
				push MessageType
				mov ecx, 0x00703418
				mov ecx, dword ptr[ecx]
				call dwCall
			}
		}

		void Process(PVOID packet) {
			PPLAYERMESSAGE p = (PPLAYERMESSAGE)packet;
			sendMessage(p->strMessage, p->MessageType);
		}
	}

	
	namespace globalmessage
	{
		CMyInlineHook obj1, obj2, obj3, obj4;
		void __stdcall ChangeMessageColor(char*);
		void __stdcall GetColor(BYTE*);
		void __stdcall ChangeMessageColor2(BYTE*);

		PVOID g_pfunChangeColor2 = ChangeMessageColor2;
		PVOID g_pfunGetColor = GetColor;
		PVOID g_pfunChangeMessageColor = ChangeMessageColor;
		CRITICAL_SECTION g_Messagecs;
		DWORD g_dwRgb = 0;

		void __stdcall GetColor(BYTE* strPacket)
		{
			DWORD dwPacket = (DWORD)strPacket;
			strPacket += 0x18;
			strPacket += strlen((char*)strPacket);


			DWORD dwCall = 0x0057FE30;
			g_dwRgb = 0xffff80ff;      //此处GB是反过来的

			//如果有颜色就取颜色,否则就默认
			if (*(PDWORD)strPacket != 0)
			{
				(PBYTE(&g_dwRgb))[3] = 0xff;
				(PBYTE(&g_dwRgb))[2] = strPacket[1];
				(PBYTE(&g_dwRgb))[1] = strPacket[2];
				(PBYTE(&g_dwRgb))[0] = strPacket[3];
			}

			EnterCriticalSection(&g_Messagecs);
			_asm {
				push dwPacket
				call dwCall
				add esp, 0x4
			}
			LeaveCriticalSection(&g_Messagecs);
		}

		//屏幕左下角颜色
		void __stdcall ChangeMessageColor2(BYTE* color)
		{
			color[0] = (PBYTE(&g_dwRgb))[2];
			color[1] = (PBYTE(&g_dwRgb))[1];
			color[2] = (PBYTE(&g_dwRgb))[0];
		}


		//设置屏幕中间颜色
		void __stdcall ChangeMessageColor(char* strChat)
		{
			if (strncmp("[全体聊天]", strChat, strlen("[全体聊天]")) != 0)
			{
				return;
			}
			DWORD dwLen = strlen(strChat);
			*PDWORD(strChat + dwLen + 1) = g_dwRgb;
		}

		//设置左下角颜色
		__declspec(naked) void Naked1()
		{
			_asm
			{
				pushad
				lea eax, [eax + 0x108]
				push eax
				call g_pfunChangeColor2
				popad
				retn 0x4
			}
		}


		//设置屏幕中间颜色
		__declspec(naked) void Naked2()
		{
			_asm
			{

				mov ecx, dword ptr ds : [edi + 0x4]
				mov dword ptr ds : [ecx], edi
				pushad
				mov edi, dword ptr ds : [edi + 0x8]
				push edi
				call g_pfunChangeMessageColor
				popad
				jmp obj2.m_pRet

			}
		}

		//屏幕中间全体读取颜色,颜色已经保存在文字内容里了
		__declspec(naked) void Naked3()
		{
			_asm
			{
				push 0xFFFF80FF
				pushad
				__loop :
				cmp byte ptr ds : [edi], 0x0
					lea edi, dword ptr ds : [edi + 0x1]
					jnz __loop
					cmp dword ptr ds : [edi], 0x0
					je __tga
					mov edi, dword ptr ds : [edi]
					mov dword ptr ss : [esp + 0x20], edi
					__tga :
				popad
					jmp obj3.m_pRet


			}
		}

		//封包最开始处理的地方,用来保存颜色
		__declspec(naked) void Naked4()
		{
			_asm
			{
				mov eax, dword ptr[esp + 0x8]
				pushad
				push eax
				call g_pfunGetColor
				popad
				retn
			}
		}

		void Start()
		{
			obj1.Hook((PVOID)0x0041c05f, Naked1, 8);
			obj2.Hook((PVOID)0x0041e5d9, Naked2, 5);
			obj3.Hook((PVOID)0x004b8f42, Naked3, 5);
			obj4.Hook((PVOID)0x0058562c, Naked4, 5);

			InitializeCriticalSection(&g_Messagecs);
		}
	}

	namespace fastrr
	{
		DWORD g_time = 0;
		CMyInlineHook g_objEnchant;


		DWORD dwCall = 0x00586f50;
		__declspec(naked) void Naked_enchant()
		{
			_asm {
				cmp g_time, 0
				je __orginal
				jmp g_objEnchant.m_pRet

				__orginal :
				mov g_time, 0x3
					call dwCall
					jmp g_objEnchant.m_pRet


			}
		}
		void ThreadProc(_In_ LPVOID lpParameter)
		{
			while (1)
			{
				if (g_time) {
					g_time--;
				}
				Sleep(500);
			}
		}



		void Start()
		{

			{
				BYTE temp[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
				_MyWriteProcessMemory(0x004a8963, temp);
			}

			{
				BYTE temp[] = { 0xB0, 0x01, 0x90, 0xB0, 0x01 ,0x90 };
				_MyWriteProcessMemory(0x004d8366, temp);
			}
			{
				BYTE temp[] = { 0xC2 ,0x04 ,0x00 };
				_MyWriteProcessMemory(0x004d839c, temp);
			}
			{
				BYTE temp[] = { 0x83 ,0xC4 ,0x04 ,0x90 ,0x90 };
				_MyWriteProcessMemory(0x004a89ab, temp);
			}

			{
				BYTE temp[] = { 0x83 ,0xC4 ,0x04, 0x90, 0x90, 0x6A, 0x02, 0x8B, 0xCE, 0x83 ,0xC4, 0x04, 0x90, 0x90 };
				_MyWriteProcessMemory(0x004d90bf, temp);
			}
			{
				BYTE temp[] = { 0x90,0x90,0x90,0x90,0x90,0x90,0x90 };
				_MyWriteProcessMemory(0x00494562, temp);		 
			}


			_beginthread(ThreadProc, 0, 0); 
			g_objEnchant.Hook(PVOID(0x00587d54), Naked_enchant, 5);

		}




	}

	namespace killTitle {

		CMyInlineHook g_objDrawAboveName, g_objDrawAboveGuild;

		struct KILLTITLE {
			DWORD kills;
			DWORD rgb;
			const char* strLightTitle;
			const char* strFuryTitle;
		}g_Titles[] = {
			{3000,0,"光明守望者","愤怒禁忌者" },
			{5000,0,"伊瑟的卫兵","深渊的咆哮" },
			{10000,0,"黑暗畏惧之风","光明畏惧之风"},
			{30000,0,"曙光","门徒"},
			{50000,0,"战神","人屠"},
			{100000,0,"刺心","逆神"},
			{200000,0,"史诗行走","骸骨传说"}
		};





		//从尾部开始遍历,找title
		const char* getTitle(DWORD kills, BYTE country) {
			for (int i = _countof(g_Titles) - 1; i >= 0; i--) {
				if (kills >= g_Titles[i].kills) {
					return country ? g_Titles[i].strFuryTitle : g_Titles[i].strLightTitle; //根据阵营取
				}
			}
			return NULL;
		}


		DWORD drawTitle(const char* strWords, DWORD x, DWORD y) {

			PVOID dwCall = (PVOID)0x0052fb30;

			_asm
			{
				push strWords
				push 0x0
				push 0xae //b
				push 0x1c  //g
				push 0xff //r
				push y
				push x
				push 0x21F1ED0
				call dwCall
				add esp, 0x20
			}
		}



		void __stdcall fun_dropGuild(DWORD player, float x, DWORD y) {
			DWORD kills = *PDWORD(player + 0x2ec);
			BYTE country = *PBYTE(player + 0x276);
			const char* strTitle = getTitle(kills, country);



			if (strTitle) {
				DWORD pos_x = DWORD(x - 3 * strlen(strTitle));
				drawTitle(strTitle, pos_x, y - 15);
			}
		}


		void __stdcall fun_dropName(DWORD player, float x, DWORD y) {

			if (*PDWORD(player + 0x2d4)) {
				return;
			}

			fun_dropGuild(player, x, y);
		}




		DWORD dwOrgCall_1 = 0x00538A20;
		__declspec(naked) void Naked_drawGuild() {
			_asm {
				pushad
				mov eax, [esp + 0x28]  //y
				mov ebx, [esp + 0x9c]
				MYASMCALL_3(fun_dropGuild, esi, ebx, eax)
				popad

				call dwOrgCall_1
				jmp g_objDrawAboveGuild.m_pRet
			}
		}


		__declspec(naked) void Naked_DrawName() {
			_asm {
				pushad
				mov eax, [esp + 0x28]  //y
				mov ebx, [esp + 0x9c]
				MYASMCALL_3(fun_dropName, esi, ebx, eax)
				popad

				call dwOrgCall_1
				jmp g_objDrawAboveName.m_pRet
			}
		}

		void InitImages() {


		}


		void Start() {
			g_objDrawAboveName.Hook(PVOID(0x0044c2e6), Naked_DrawName, 5);
			g_objDrawAboveGuild.Hook(PVOID(0x0044c3a3), Naked_drawGuild, 5);
		}
	}

	namespace additional_image {

		CMyInlineHook g_objDrawAboveName, g_objDrawAboveGuild;

		typedef struct {
			int un1;
			int un2;
			float width;
			float height;
		}ShaiyaImgObj,*PShaiyaImgObj;

		ShaiyaImgObj g_pImages[100] = { 0 };
		PShaiyaImgObj g_Playerimages[2000] = { 0 };
		CRITICAL_SECTION g_cs;
		std::map<DWORD,DWORD> g_mapPlayerImages;

		int g_x_plus = 0;   //
		int g_y_plus = 0;   //这些可以作为图片的长宽来作调整
		DWORD g_imageCount = 0;
		bool g_isEnable = false;

		void resetImageObj(DWORD charid, DWORD ImageIndex) {
			g_Playerimages[charid] = &(g_pImages[ImageIndex]);
		}

		PShaiyaImgObj getImageObj(DWORD charid) {
			return g_Playerimages[charid];
		}




		void __stdcall fun_dropGuild(DWORD player, float x, DWORD y) {
			//我们这里只获取坐标,由另一处来画
			*PDWORD(player + 0x3e0) = x;
			*PDWORD(player + 0x3e4) = y;
			DWORD charid = *PDWORD(player + 0x34);
		}

		void __stdcall fun_dropName(DWORD player, float x, DWORD y) {

			//如果有行会就不画了
			if (*PDWORD(player + 0x2d4)) {
				return;
			}

			fun_dropGuild(player, x, y);
		}


		DWORD dwOrgCall_1 = 0x00538A20;
		__declspec(naked) void Naked_drawGuild() {
			_asm {
				pushad
				mov eax, [esp + 0x28]  //y
				mov ebx, [esp + 0x9c]
				MYASMCALL_3(fun_dropGuild, esi, ebx, eax)
				popad

				call dwOrgCall_1
				jmp g_objDrawAboveGuild.m_pRet
			}
		}


		__declspec(naked) void Naked_DrawName() {
			_asm {
				pushad
				mov eax, [esp + 0x28]  //y
				mov ebx, [esp + 0x9c]
				MYASMCALL_3(fun_dropName, esi, ebx, eax)
				popad

				call dwOrgCall_1
				jmp g_objDrawAboveName.m_pRet
			}
		}

		void __stdcall fun_test(std::vector<DWORD>& vecPlayers) {

			if (g_isEnable) {
				for (auto iter = vecPlayers.begin(); iter != vecPlayers.end(); iter++) {
					DWORD player = *iter;
					if (!player) continue;
					DWORD charid = *PDWORD(*iter + 0x34);
					//获取图像
					PShaiyaImgObj pImage = getImageObj(charid);
					if (pImage) {
						//要调整的坐标位,从服务端获取的
						DWORD x = *PDWORD(*iter + 0x3e0) + g_x_plus- pImage->width/2;
						DWORD y = *PDWORD(*iter + 0x3e4) + g_y_plus -pImage->height-15;
						drawImage(pImage, x, y);
					}
				}
			}
		}

		


		bool InitImageEx(const char* pstrPath, const char* pstrFileName, PShaiyaImgObj point,DWORD len=176,DWORD height=80)
		{
			bool bRet = false;
			DWORD Path = (DWORD)pstrPath;
			DWORD FileName = (DWORD)pstrFileName;

			DWORD dwCall = 0x00537600;
			try
			{
				_asm {
					push height
					push len
					push FileName
					push Path
					mov ecx, point
					call dwCall
					mov bRet, al
				}
			}
			catch (...)
			{			
			}

			return bRet;
		}


		void InitImage()
		{

			FILE *pfRead;
			if (fopen_s(&pfRead, g_safPath, "rb") != 0) {
				return;
			}

			for (DWORD i = 0; i < g_imageCount; i++){

				char filename[50] = { 0 };
				char fullName[MAX_PATH] = { 0 };
				
				sprintf(filename, "title_%02d.png", i);
				sprintf(fullName, "interface\\%s", filename);


				//获取sah里的file属性拿offset
				PFile stFile = sahReader::getShiyaFile(fullName, &g_root_directory);
				if (!stFile) {
					continue;
				}
				fseek(pfRead, stFile->offset, SEEK_SET);
				unsigned int width, height;
				GetPNGWidthHeight(pfRead, &width, &height, stFile->offset);


				if (!InitImageEx("data\\interface", filename, &(g_pImages[i]),width,height)) {
					break;
				}
			}
		}

		void Init() {
			if (g_isEnable) {
				return;
			}

			g_isEnable = true;
			InitImage();

			//这2个hook是获取坐标的
			g_objDrawAboveName.Hook(PVOID(0x0044c2e6), Naked_DrawName, 5);
			g_objDrawAboveGuild.Hook(PVOID(0x0044c3a3), Naked_drawGuild, 5);

		}
		void ProcessPacket(void* p) {

			PPLAYERTITLES packet = (PPLAYERTITLES)p;
  			g_x_plus = packet->x;
  			g_y_plus = packet->y;
			g_imageCount = packet->imageCount;

			Init();

			ZeroMemory(g_Playerimages, sizeof(g_Playerimages));
			for (DWORD i = 0; i < _countof(packet->playerImages); i++) {
				DWORD charid = packet->playerImages[i].charid;
				if (!charid) {
					break;
				}
				resetImageObj(charid, packet->playerImages[i].Inedx);
			}
		}
	}

	namespace setItem {

		CMyInlineHook g_obj, g_objDraw;

#pragma pack(1)
		typedef struct {
			BYTE type;
			BYTE _typeid;
			BYTE count;
			WORD QA;
			BYTE gems[6];
			char craftname[21];
			BYTE unknown[2];
		/*	BYTE unknown[8];*/
		}PLAYITEMS, *PPLAYITEMS;
#pragma pack()


		PPLAYITEMS g_playerItems = (PPLAYITEMS)0x850769;

		SETITEMDATA g_setitems[300] = { 0 };
		

		DWORD dwCallDrawBG = 0x004FF1A0;
		bool g_isEnable = false;




		void readSetitemSdata() 
		{
			//获取sah里的file属性拿offset
			PFile stFile = sahReader::getShiyaFile("item\\setitem.sdata", &g_root_directory);
			if (!stFile) {
				return;
			}

			std::ifstream* input = new std::ifstream(g_safPath, std::ifstream::in | std::ifstream::binary);
			if (!input->is_open()) {
				return;
			}

			//定位到offset那里
			input->seekg(stFile->offset);

			//获取套装
			DWORD count = 0;
			input->read((char*)&count, sizeof(count));

			for (DWORD i = 0; i < count; i++) {

				SETITEMDATA st = { 0 };

				input->read((char*)&st.id, sizeof(st.id));
				input->read((char*)&(st.setName.len), sizeof(st.setName.len));
				input->read((char*)&(st.setName.name), st.setName.len);


				for (DWORD j = 0; j < 13; j++) {
					input->read((char*)&st.items[j]._type, sizeof(st.items[j]._type));
					input->read((char*)&st.items[j]._typeid, sizeof(st.items[j]._typeid));
				}

				for (DWORD j = 0; j < 13; j++) {
					input->read((char*)&st.Itemdesc[j].len, sizeof(st.Itemdesc[j].len));
					input->read((char*)&st.Itemdesc[j].name, st.Itemdesc[j].len);
					std::cout << st.Itemdesc[j].name << '\n';
				}

				memcpy(&g_setitems[i], &st, sizeof(st));
			}
			input->close();
		}





		PSETITEMDATA getSetitem(BYTE itemType, BYTE itemTypeid) {
			DWORD itemid = itemType * 1000 + itemTypeid;
			for (DWORD i = 0; i < _countof(g_setitems); i++) {
				PSETITEMDATA st = &g_setitems[i];
				for (DWORD j = 0; j < 5; j++) {
					DWORD setItemId = st->items[j]._type * 1000 + st->items[j]._typeid;
					if (setItemId == itemid) {
						return st;
					}
				}
			}
			return NULL;
		}




		void __stdcall getDrawInfo(DWORD x, DWORD y, DWORD width, DWORD height, DWORD obj) {

			if (!g_isEnable) {
				return;
			}


			BYTE itemType = *PBYTE(obj + 0x4);
			BYTE itemTypeid = *PBYTE(obj + 0x5);

			PSETITEMDATA pSetItem = getSetitem(itemType, itemTypeid);

			if (!pSetItem) {
				return;
			}

			//画背景
			x = x + width + 10;
			drawBackground(x, y, width, height);

			//画文字
			x += 10; //不要直接靠左
			y += 15; //首行空着



			BYTE count = 0;
			for (DWORD i = 0; i < 24; i++) {
				DWORD itemId = g_playerItems[0 * 24 + i].type * 1000 + g_playerItems[0 * 24 + i]._typeid;
				if (!itemId)
					continue;

				for (DWORD j = 0; j < 5; j++) {
					DWORD setItemid = pSetItem->items[j]._type * 1000 + pSetItem->items[j]._typeid;
					bool isEnable = pSetItem->isEnable[j];

					//前面计算过的了
					if (pSetItem->isEnable[j]) {
						continue;
					}

					if (itemId == setItemid && !isEnable) {
						count++;
						pSetItem->isEnable[j] = true;
						break;
					}
				}
			}


			//名字
			for (DWORD j = 0; j < 5; j++) {
				DWORD setItemid = pSetItem->items[j]._type * 1000 + pSetItem->items[j]._typeid;
				std::string stItemName = getItemname(setItemid);

				if (pSetItem->isEnable[j]) {
					y = y + 15 * drawWords(stItemName.c_str(), x, y, 0xffff00);
				}
				else {
					y = y + 15 * drawWords(stItemName.c_str(), x, y, 0xffffff);
				}
			}

			y += 15;

			//区块介绍
			for (DWORD j = 0; j < 5; j++) {
				if (j < count) {
					y = y + 15 * (drawWords(pSetItem->Itemdesc[j].name, x, y, 0xffff00) + 1);
				}
				else {
					y = y + 15 * (drawWords(pSetItem->Itemdesc[j].name, x, y, 0xffffff) + 1);
				}
			}
			ZeroMemory(pSetItem->isEnable, sizeof(pSetItem->isEnable));

		}


		__declspec(naked) void Naked_drawBackground() {
			_asm {
				cmp dword ptr[esp + 0x93c], 0x004a3b15
				jne  _Org

				pushad
				mov ebp, dword ptr[esp + 0x34]
				MYASMCALL_5(getDrawInfo, edi, esi, edx, ecx, ebp)
				popad

				_Org :
				call g_dwCallDrawBG
					jmp g_obj.m_pRet
			}
		}




		void ProcessPacket(PVOID p) {
			if (g_isEnable)
				return;
			readSetitemSdata();
			g_obj.Hook(PVOID(0x004a0f0f), Naked_drawBackground, 5);
			g_isEnable = true;

		}


		void Start() {


		}
	}

	namespace PlayerEffect {

		shaiya_pos pos = { 0,2,0 };
		int g_x_plus = 50;
		int g_y_plus = 50;

		bool g_isEnable = false;
		DWORD g_effects[2000] = { 0 };

		void drawEffect(DWORD charid,DWORD effectId) {

			DWORD obj = 0;
			DWORD dwCall_1 = 0x00447d20;
			_asm {

				push charid
				mov ecx,0x007071e0
				call dwCall_1
				mov obj,eax
			}
			if (!obj)
				return;

			//3.
			DWORD dwCall_2 = 0x004505c0;
			
			DWORD temp = (DWORD)&pos;

			_asm {
				mov esi,obj
				push 0x5
				lea eax,dword ptr [esi+0x28]
				push eax
				push temp
				lea ebx,dword ptr [esi+0x10]
				push ebx
				push 0x0
				push effectId
				mov ecx,0x007071e0
				call dwCall_2
			}
		}

		DWORD getEffectId(DWORD charid) {
			return g_effects[charid];
		}



		void draw(std::vector<DWORD>& vecPlayers) {


			if (g_isEnable) {
				DWORD curTime = GetTickCount();

				for (auto iter = vecPlayers.begin(); iter != vecPlayers.end(); iter++) {
					DWORD player = *iter;
					if (!player) continue;
					DWORD charid = *PDWORD(player + 0x34);

					DWORD nextTime= *PDWORD(player + 0x3e8);

					


					if(curTime<nextTime&&nextTime-curTime<4000)
						continue;

					*PDWORD(player + 0x3e8) = curTime + 4000;
					DWORD effectId = getEffectId(charid);
					if (effectId) {
						drawEffect(charid, effectId);
					}
				}

			}
		}



		void ProcessPacket(void* p) {
			g_isEnable = true;
			PPLAYEREFFECTS packet = PPLAYEREFFECTS(p);
			ZeroMemory(g_effects, sizeof(g_effects));
			for (DWORD i = 0; i < packet->count; i++) {
				DWORD charid = packet->effects[i].charid;
				DWORD effectId = packet->effects[i].effectIndex;
				g_effects[charid] = effectId;
			}
		}
	}

	namespace onlineInformation {

		CMyInlineHook g_obj;

		SHAIYAONLINEINFO g_playerInfo;
		DWORD g_drawExpiredTime = 0;

		int g_x = 50;
		int g_y = 50;
#define  ONLINEBG_WIDTH (300)
#define  ONLINEBG_LINE_HEIGHT (18)


		void __stdcall fun() {

			DWORD currentTime = GetTickCount();

			if (currentTime<g_drawExpiredTime) {
				DWORD count = g_playerInfo.count;
				DWORD height = ONLINEBG_LINE_HEIGHT*(count+3);
				
				int x = g_x;
				int y = g_y;

				//画背景
				drawBackground(x, y, ONLINEBG_WIDTH,height);

				y += (ONLINEBG_LINE_HEIGHT*2);
				x += 20;

				char* strTitle = g_playerInfo.title;
				drawWords_sub(strTitle, ONLINEBG_WIDTH / 2 - strlen(strTitle) * 2,y );

				y += ONLINEBG_LINE_HEIGHT;

				//画文字	
				for (DWORD i = 0; i < count; i+=2) {
					char strWords[MAX_PATH] = { 0 };
					sprintf(strWords, "玩家:<%s>---------玩家:<%s>", g_playerInfo.playerNames[i], g_playerInfo.playerNames[i + 1]);
					drawWords_sub(strWords, x, y);
					y += ONLINEBG_LINE_HEIGHT;
				}
			}		
		}


		__declspec(naked) void Naked() {
			_asm {
				mov eax, dword ptr ds : [esi + 0x8]
				mov ecx, dword ptr ds : [esi + 0x4]
				pushad
				MYASMCALL(fun)
				popad

				jmp g_obj.m_pRet
			}
		}

		void Init() {
			bool static isInit = false;
			if (isInit) {
				return;
			}
			g_obj.Hook((PVOID)0x004fa00e, Naked, 6);
		}

		void ProcessPacket(void* p) {

			Init();
#ifdef _DEBUG
			g_drawExpiredTime = GetTickCount() + 100 * 1000;
#else
			g_drawExpiredTime = GetTickCount() + g_playerInfo.keepTime*1000;
#endif
			memcpy(&g_playerInfo, p, sizeof(g_playerInfo));

		}
	}

	namespace combine {
		void ProcessPack(void* p) {

			{
				BYTE temp[] = { 0x90,0x90 };
				MyWriteProcessMemory(GetCurrentProcess(), (void*)0x004a60fb, temp, sizeof(temp));
				MyWriteProcessMemory(GetCurrentProcess(), (void*)0x004a6103, temp, sizeof(temp));
			}

			{
				BYTE temp[] = { 0xE9 ,0x8B,0x00,0x00,0x00 };
				MyWriteProcessMemory(GetCurrentProcess(), (void*)0x004A6028, temp, sizeof(temp));
			}


			{
				BYTE temp[] = {0xb0,0x01 };
				MyWriteProcessMemory(GetCurrentProcess(), (void*)0x004A61A7, temp, sizeof(temp));
			}
		}
	}

	namespace custompacket
	{
		CMyInlineHook objPacket, g_objTest;

		bool isTime() {
			static DWORD NextTime = GetTickCount();
			DWORD curTime = GetTickCount();

			if (NextTime > curTime) {
				return false;
			}
			else {
				NextTime = curTime + 50;
				return true;
			}

		}

		void __stdcall fun_draw() {

			vector<DWORD> vecPlayers;
			enumPlayers(vecPlayers);

			additional_image::fun_test(vecPlayers);
			PlayerEffect::draw(vecPlayers);
		}


		__declspec(naked) void Naked_draw() {
			_asm {
				pushad
				MYASMCALL(fun_draw)
				popad
				mov eax, 0x21ea78b
				mov al, byte ptr[eax]
				jmp g_objTest.m_pRet
			}
		}

		void drawInit() {
			static bool isInit = false;
			if (isInit)
				return;
			isInit = true;
			g_objTest.Hook(PVOID(0x004fa01e), Naked_draw, 5);
		}





		void __stdcall CustomPacket(PVOID pCmd)
		{
			WORD cmd = *PWORD(pCmd);
			switch (cmd)
			{
				//总之只有接到时钟才算
			case PacketOrder::Clock:
				//	fun_clock((Pcmd_clock)++pCmd);
				break;

			case PacketOrder::VipColor:
				namecolor::fun_getvipcolor(pCmd);
				break;

			case PacketOrder::PlayerMessage:
				customMessage::Process(pCmd);
				break;

			case PacketOrder::AntiHacker:
				antiHacker::start();
				antiHacker::HeatBaetCheck::PacketProcess(pCmd);
				break;

			case PacketOrder::additional_title:
				additional_image::ProcessPacket(pCmd);
				drawInit();
				break;

			case PacketOrder::onlineplayer_info:
				onlineInformation::ProcessPacket(pCmd);
				break;

			case PacketOrder::combine_materials:
				combine::ProcessPack(pCmd);
				break;
			case PacketOrder::palyer_effect:
				PlayerEffect::ProcessPacket(pCmd);
				drawInit();
				break;
			case PacketOrder::SetItem_Enable:
				setItem::ProcessPacket(pCmd);
				break;
			case PacketOrder::Trade_color:
				trademessage::Start();
				break;

			default:
				break;
			}
		}

		__declspec(naked) void Naked1()
		{
			_asm
			{
				cmp eax, 0xff00
				jl _orginal
				pushad
				mov eax, [esp + 0x28]
				MYASMCALL_1(CustomPacket,eax)
				popad
				_orginal :
				sub eax, 0x0000FA08
					jmp objPacket.m_pRet
			}
		}

		void Start()
		{
			objPacket.Hook((PVOID)0x00586021, Naked1, 5);
		}
	}

	namespace miniMap
	{
		CMyInlineHook g_obj;

		__declspec(naked) void Naked()
		{
			_asm
			{
				mov ebx,dword ptr [esp+0x114]
				cmp ebx,0x27
				jne _Org
				mov ebx,0x12
				_Org:
				jmp g_obj.m_pRet
			}
		}


		void start() {
			g_obj.Hook(PVOID(0x004ba09e), Naked, 7);
		}
	}
	namespace common
	{
		

		void ThreadProc(_In_ LPVOID lpParameter)
		{
		
			Sleep(2000);


			//更改玩家结构大小
			DWORD playerobjSize = 0x400;
			DWORD addr[] = { 0x00418ed3,0x0041ad86,0x00465ab5,0x00466055,0x0046a37d,0x004c9b6d,0x0047fc22,0x00552edb,0x005532b2 };
			for (DWORD i = 0; i < _countof(addr); i++) {
				MyWriteProcessMemory(GetCurrentProcess(), PVOID(addr[i]), &playerobjSize, sizeof(playerobjSize));
			}

			getSendAddr();

			//h键血量问题
			BYTE temp[] = { 0x90,0x90 };
			MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004FA55D, temp, sizeof(temp));


			//吧服不要开name color
			namecolor::Start();
			custompacket::Start();
			globalmessage::Start();
			miniMap::start();
			//

			fastrr::Start();
			//killTitle::Start();2	2

			antiHacker::start();

			InitSahReader();

		}

		void Start()
		{
			//更改玩家结构大小
			DWORD playerobjSize = 0x400;
			DWORD addr[] = { 0x00418ed3,0x0041ad86,0x00465ab5,0x00466055,0x0046a37d,0x004c9b6d,0x0047fc22,0x00552edb,0x005532b2 };
			for (DWORD i = 0; i < _countof(addr); i++) {
				MyWriteProcessMemory(GetCurrentProcess(), PVOID(addr[i]), &playerobjSize, sizeof(playerobjSize));
			}
			removeijl15Detection();
			_beginthread(ThreadProc, 0, 0);
		}
	}
}


// 		namespace CheckInlineHook
// 		{
// #include <openssl/md5.h>
// #ifdef _DEBUG
// #pragma comment(lib,"libcryptoMTd.lib")
// #pragma comment(lib,"libsslMTd.lib")
// #else
// #pragma comment(lib,"libcryptoMT.lib")
// #pragma comment(lib,"libsslMT.lib")
// #endif
// 
// 			DWORD g_scanHead = 0;
// 			DWORD g_scanSize = 0;
// 			char g_md5[40] = { 0 };
// 
// 			void getMd5(char* buf)
// 			{
// 				MD5_CTX ctx;
// 				unsigned char md[16] = { 0 };
// 
// 				char tmp[3] = { 0 };
// 				BYTE data[1024] = { 0 };
// 
// 				MD5_Init(&ctx);
// 				for (DWORD i = 0; i < g_scanSize / 1024; i++)
// 				{
// 					memcpy(data, PVOID(g_scanHead + i * 1024), 1024);
// 					MD5_Update(&ctx, data, 1024);
// 				}
// 				MD5_Final(md, &ctx);
// 
// 				for (auto i = 0; i < 16; i++) {
// 					sprintf(tmp, "%02X", md[i]);
// 					strcat(buf, tmp);
// 				}
// 			}
// 
// 			void Init()
// 			{
// 				WCHAR moduleName[MAX_PATH] = { 0 };
// 				GetModuleFileName(0, moduleName, MAX_PATH);
// 				DWORD hModule = (DWORD)GetModuleHandle(moduleName);
// 
// 				PIMAGE_DOS_HEADER pDosH = (PIMAGE_DOS_HEADER)hModule;
// 				PIMAGE_NT_HEADERS32 pNtH = (PIMAGE_NT_HEADERS32)(hModule + pDosH->e_lfanew);
// 				PIMAGE_SECTION_HEADER pSecH = IMAGE_FIRST_SECTION(pNtH);
// 
// 				for (auto i = 0; i < pNtH->FileHeader.NumberOfSections; i++)
// 				{
// 					if (pSecH[i].VirtualAddress + hModule == 0x00401000)  //就算加完壳，也是这个起始地址的
// 					{
// 						g_scanHead = pSecH[i].VirtualAddress + hModule;
// 						g_scanSize = pSecH[i].Misc.VirtualSize;
// 						getMd5(g_md5);
// 						break;
// 					}
// 				}
// 			}
// 
// 			bool ismd5Match() {
// #ifndef _DEBUG
// 	//			VMProtectBegin("shaiya50_ismd5Match");
// #endif
// 				char buf[40] = { 0 };
// 				getMd5(buf);
// #ifndef _DEBUG
// 		//		VMProtectEnd();
// #endif
// 				return strcmp(g_md5, buf) == 0 ? true : false;
// 
// 			}
// 
// 
// 
// 			bool IsHacked()
// 			{
// 				static bool isInit = false;
// 				if (!isInit) {
// 					isInit = true;
// 					Init();
// 				}
// 
// 				if (!g_scanHead && !g_scanSize)
// 					return false;
// 
// 				return ismd5Match() ? false : true;
// 
// 			}
// 		}
