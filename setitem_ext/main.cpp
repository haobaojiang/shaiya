// FixDroprate.cpp : 定义 DLL 应用程序的导出函数。
//

#include <afxwin.h>
#include "FixDroprate.h"
#include <MyInlineHook.h>
#include <time.h>
#include <string>
//#include <VMProtectSDK.h>
#include "E:\\我自己的工具源码\\ShaiyaFixProject\\CustomGame\\define.h"
#include "DataBase.h"

#define CHAR_TO_WCHAR(lpChar,lpW_Char) \
MultiByteToWideChar(CP_ACP,0,lpChar,-1,lpW_Char,_countof(lpW_Char));

#define WCHAR_TO_CHAR(lpW_Char,lpChar) \
WideCharToMultiByte(CP_ACP,0,lpW_Char,-1,lpChar,_countof(lpChar),0,0);

#define FEATURE_NAMECOLOR 1
//#define FEATURE_BACKDOOR 1
#define FEATURE_TRADECOLOR 1




void send_packet(DWORD dwPlayer,PBYTE pPacket,DWORD dwSize)
{
	PVOID pSend = (PVOID)0x004a8d20;
	DWORD dwTemp = (DWORD)pPacket;
	_asm
	{
		push 0x12
			push dwTemp
			mov ecx, dwPlayer
			call pSend
	}
}

void send_packet(DWORD dwPlayer, WORD wCmd, PBYTE pPacket, DWORD dwSize)
{
	PBYTE pNewPacket = new byte[dwSize + 2];
	memcpy(pNewPacket, &wCmd, 2);         //header
	memcpy(&pNewPacket[2], pPacket, dwSize);//detail
	send_packet(dwPlayer, pNewPacket, dwSize + 2);
	delete[] pNewPacket;
}



CMyInlineHook obj;
void WINAPI fun(DWORD dwPlayer);
PVOID g_pfun = fun;



void WINAPI fun(DWORD dwPlayer)
{
	BYTE byPacket[18] = { 0 };
	*PWORD(byPacket) = PacketOrder::Clock;
	GetLocalTime((PSYSTEMTIME)&byPacket[2]);
	send_packet(dwPlayer,byPacket, 18);
}


__declspec(naked) void Naked()
{
	_asm
	{
		pushad
			push ebp
			call g_pfun
		popad
		mov word ptr  [esp + 0x40], 0x101
		jmp obj.m_pRet
	}
}






DWORD WINAPI ThreadProc(_In_  LPVOID lpParameter)
{
	//clock
	obj.Hook((PVOID)0x0045376c, Naked, 7);
	return 0;
}












// 这是导出函数的一个示例。
FIXDROPRATE_API int fnFixDroprate(void)
{
	return 0;
}

