
#include <string>
#include <vector>
#include <windows.h>
#include <MyInlineHook.h>
#include "ChatFilter.h"
using std::vector;
using std::string;



CRITICAL_SECTION g_csCharFilter;

vector<std::string> g_vec_szFilter;
DWORD g_dwReturnAddr = 0;
DWORD g_dwFalseAddr = 0x0047FC57;
PVOID g_pfunChatFilter = NULL;

__declspec(naked) void  Naked_ChatFilter()
{
	_asm
	{
		pushad
			push ebx
			call g_pfunChatFilter
			test al,al
			popad
			jne __orginal	           
			jmp  g_dwFalseAddr       //跳到false的地方
__orginal:
			movzx eax, byte ptr  [eax + 0x47FCA8]
			jmp g_dwReturnAddr
	}
}



bool __stdcall ChatFilter(PBYTE pPackets)
{
	std::string szChat =(char*) &pPackets[3];

	bool bRet = true;
	EnterCriticalSection(&g_csCharFilter);
	for (DWORD i = 0; i < g_vec_szFilter.size(); i++)
	{
		if (szChat.find(g_vec_szFilter[i]) != -1)
		{
			bRet = false;
			break;
		}
	}
	LeaveCriticalSection(&g_csCharFilter);
	return bRet;
}






void StartChatFilter()
{
	InitializeCriticalSection(&g_csCharFilter);
	CMyInlineHook obj1((PVOID)0x0047f4ba, Naked_ChatFilter, &g_dwReturnAddr, 7);
	g_pfunChatFilter = ChatFilter;
}