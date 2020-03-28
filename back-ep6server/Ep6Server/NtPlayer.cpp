
#include <string>
#include <vector>
#include <windows.h>
#include <MyInlineHook.h>
#include "NtPlayer.h"
#include "common.h"

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



DWORD dwConsolTemp = 0x004F627C;
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
	obj1.Hook((PVOID)0x004f6270, Naked_Console,5);
}