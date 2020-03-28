
#include <string>
#include <vector>
#include <windows.h>
#include <MyInlineHook.h>
#include "NtPlayer.h"


using std::vector;
using std::string;

CMyInlineHook obj1;
void __stdcall fun_console(PCHAR);
PVOID g_pfunConsole = fun_console;

#pragma pack(1)
typedef struct _notice
{
	WORD wCmd = 0xf106;
	WORD wLen = 0;
	char strMessage[MAX_PATH];
}notice,*Pnotice;
#pragma pack()

//ep4
DWORD GetPlayerByCharid(int nCharid)
{
	DWORD dwRet = 0;
	do
	{
		if (!nCharid)
			break;

		DWORD dwCall = 0x0040d640;
		_asm
		{
			mov eax, 0x546F38
				mov eax, [eax]
			push nCharid
				call dwCall
				mov dwRet, eax
		}
	} while (0);
	return dwRet;
}


//ep4
void __stdcall SendPacketToPlayer(DWORD dwPlayer, DWORD dwBuff, DWORD dwSize)
{
	PVOID g_pSendPacketCall = (PVOID)0x004D4D20;//发送封包的,很多地方需要用到
	_asm
	{
		mov ecx, dwPlayer
			push dwSize
			push dwBuff
			call g_pSendPacketCall
	}
}


DWORD dwConsolTemp = 0x004D5ECC;
__declspec(naked) void  Naked_Console()
{
	_asm
	{
		pushad
			push ebx
			call g_pfunConsole
			popad
			cmp eax, 0x2
			jnz __tag
			jmp obj1.m_pRet
		__tag:
		jmp dwConsolTemp
	}
}



void __stdcall fun_console(PCHAR pInput)
{
	do 
	{
		if (pInput == NULL || pInput[0] == '\0')
			break;

		if (strncmp(pInput,"/ntplayer ",strlen("/ntplayer ")) != 0)
			break;

		char szTemp1[MAX_PATH] = { 0 };
		int nCharid = 0;
		char strMessage[MAX_PATH] = { 0 };
		sscanf_s(pInput, "%s %d %[^\n]", szTemp1, MAX_PATH, &nCharid, strMessage, MAX_PATH);

		if (!nCharid || strMessage[0] == '\0')
			break;

		//找角色id
		DWORD dwPlayer = GetPlayerByCharid(nCharid);
		if (!dwPlayer)
			break;


		//发送封包内容
		notice stcNotice;
		stcNotice.wLen = strlen(strMessage) + 1;
		strcpy_s(stcNotice.strMessage, MAX_PATH, strMessage);
		SendPacketToPlayer(dwPlayer, (DWORD)&stcNotice, stcNotice.wLen+4);

	} while (0);
}



void StartNtPlayer()
{
	obj1.Hook((PVOID)0x004D5EC0, Naked_Console, 5);//004D5EC0  |.  83F8 02       cmp eax,0x2
}