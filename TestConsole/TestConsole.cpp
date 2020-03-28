

#include "stdafx.h"
#include <vector>
#include <windows.h>
#include <string>
#include <stdio.h>
#include <inttypes.h>
#include <TlHelp32.h>
#include <capstone/capstone.h>
#include <define.h>


#define PSAPI_VERSION 1
#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")
#include <ImageHlp.h>
#pragma comment (lib, "imagehlp.lib")

#include <Shlwapi.h>
#include <float.h>

#define CODE "\x68\xEC\xA5\x6B\x00\xE8\x18\xFA\xFF\xFF"

#include <iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib")


#pragma comment(lib,"user32.lib")


#include <MyClass.h>
void testNpc()
{
	DWORD npcBase = (0x0221D000 + 0x13da48);
	EnableDebugPrivilege(TRUE);
	HWND hWnd= FindWindowA("GAME", "Shaiya");
	DWORD pid = 0;
	GetWindowThreadProcessId(hWnd, &pid);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	DWORD dwTemp = 0;

	ReadProcessMemory(hProcess, (PVOID)npcBase, &dwTemp, 4, 0);

}

int _tmain(int argc, _TCHAR* argv[])
{
	testNpc();

	system("pause");
	return 0;
}

