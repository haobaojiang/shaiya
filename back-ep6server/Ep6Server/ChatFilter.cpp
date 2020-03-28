
#include <string>
#include <vector>
#include <windows.h>
#include <MyInlineHook.h>
#include "ChatFilter.h"
#include "E:/我自己的工具源码/ShaiyaFixProject/CustomGame/define.h"
using std::vector;
using std::string;


bool g_bEnableChatfilter = FALSE;

vector<fun_chatcallback> g_vecChatCallBack;

namespace CharFilter
{

	CRITICAL_SECTION g_csCharFilter;
	CMyInlineHook obj1;
	DWORD g_dwFalseAddr = 0x0047FC57;
	void __stdcall Filter(Pshaiya_player dwPlayer, PBYTE pPackets);
	PVOID g_pfun = Filter;

	__declspec(naked) void  Naked_ChatFilter()
		{
			_asm
			{
				pushad
					push ebx
					push ebp
					call g_pfun
					popad
				movzx eax, byte ptr[eax + 0x47FCA8]
					jmp obj1.m_pRet
			}
		}


	//过滤所有类型的说话内容
	void __stdcall Filter(Pshaiya_player Player,PBYTE pPackets)
	{
		char* strChat = (char*)&pPackets[3];
		EnterCriticalSection(&g_csCharFilter);
		for (DWORD i = 0; i < g_vecChatCallBack.size(); i++)
		{
			fun_chatcallback pfun = fun_chatcallback(g_vecChatCallBack[i]);
			pfun(Player, strChat);
		}
		LeaveCriticalSection(&g_csCharFilter);
	}

	void Start()
	{
		InitializeCriticalSection(&g_csCharFilter);
	    obj1.Hook((PVOID)0x0047f4ba, Naked_ChatFilter, 7);
	}
}
