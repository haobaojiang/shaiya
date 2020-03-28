
#include "stdafx.h"
#include "Detours.h"
#include <MyInlineHook.h>
#include <stdio.h>
DWORD dwReTurnAdd = 0;

DWORD Address = 0;         
DWORD ReturnAddress = 0;
DWORD Address2 = 0;
DWORD ReturnAddress2 = 0;
DWORD Address3 = 0;

void DisplayPacket(char *direction, char *buf, int len);
void AnalyzePacket(char *direction, WORD opcode, char *buf, int len);

char *direction;
short opcode;
char *buf;
int len;




void __declspec(naked) Detour2()
{
	__asm
	{
		pushad
		pushfd
		mov ebp, esp
		mov eax, [esp+0x28]
		mov direction, eax;   //send or recv   ,[esp+4]
		mov ax, [esp+0x2C]    //command   [esp+8]
		cmp ax,0x0101
		jne _test
		push eax
		pop eax
	_test:
		mov opcode, ax;
		mov eax, [esp+0x30]  //pbuffer [esp+c]
		mov buf, eax;
		mov eax, [esp+0x34] //len       [esp+0x10]
		mov len, eax;
	}

	DisplayPacket(direction, buf, len);
//	AnalyzePacket(direction, opcode, buf, len);

	//神泣R
	//__asm
	//{
	//	popfd
	//	popad
	//	push 0xffffffff
	//	push 0x71945B
	//	jmp dwReTurnAdd
	//}


	//凤凰区
// 	_asm
// 	{
// 		popfd
// 			popad
// 		push 0xffffffff
// 			push 0x0072013B
// 			jmp dwReTurnAdd
// 	}
	//新的ep6 game
// 	_asm
// 	{
// 		popfd
// 			popad
// 			push 0xffffffff
// 			push 0x71D49B
// 			jmp dwReTurnAdd
// 	}

	//美服为
// 	_asm
// 	{
// 		popfd
// 			popad
// 			push 0xffffffff
// 			push 0x7A52DB
// 			jmp dwReTurnAdd
// 	}
	//shaiya pt

	//shaiya os
	 	_asm
	 	{
	 		popfd
	 			popad
	 			push 0xffffffff
				push 0x7BACAB
	 			jmp dwReTurnAdd
	 	}

}

void SetDetour()
{
	CMyInlineHook HookObj;
//	HookObj.Hook((PVOID)0x005c8e20, Detour2, 7);  //神泣r为0x005c8e20 
//	dwReTurnAdd=HookObj.Hook((PVOID)0x005cd4f0, Detour2, 7);  //凤凰区为0x005cd4f0 
//	dwReTurnAdd = HookObj.Hook((PVOID)0x0064c200, Detour2, 7);  //美服区为0x0064c200 
//	dwReTurnAdd = HookObj.Hook((PVOID)0x005c9b80, Detour2, 7);  //新的ep6
//	dwReTurnAdd = HookObj.Hook((PVOID)0x005ea700, Detour2, 7);  //shaiya pt
	dwReTurnAdd = HookObj.Hook((PVOID)0x00660d80, Detour2, 7);  //shaiya os  005EA700 
}

void SendPacket(char *buf, int len)
{	
	void(*InternalCall)(char *, int) = (void(__cdecl *)(char *, int))0x00661020;// 0x005cd790;//phoenix
	(* InternalCall)(buf, len);
}



bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
    for(;*szMask;++szMask,++pData,++bMask)
        if(*szMask=='x' && *pData!=*bMask ) 
            return false;
    return (*szMask) == NULL;
}

DWORD dwFindPattern(DWORD dwAddress,DWORD dwLen,BYTE *bMask,char * szMask)
{
    for(DWORD i=0; i < dwLen; i++)
        if( bDataCompare( (BYTE*)( dwAddress+i ),bMask,szMask) )
            return (DWORD)(dwAddress+i);  
    return 0;
}

void RecvPacket(char *buf, DWORD len)
{
	RecvPacketStruct *Packet = new RecvPacketStruct;
	memcpy(Packet->Data, buf, len);
	Packet->len = len;
	Packet->ptr = (RecvPacketStruct *)((DWORD)Packet + 2);

	WORD Opcode;
	memcpy(&Opcode, Packet, sizeof(WORD));

//	void(*ReceivePacketPtr)(WORD Opcode, RecvPacketStruct *Packet) = (void(__cdecl *)(WORD, RecvPacketStruct *))0x005d0f60;//phoenix
//	void(*ReceivePacketPtr)(WORD Opcode, RecvPacketStruct *Packet) = (void(__cdecl *)(WORD, RecvPacketStruct *))0x005D4900;//phoenix
//	void(*ReceivePacketPtr)(WORD Opcode, RecvPacketStruct *Packet) = (void(__cdecl *)(WORD, RecvPacketStruct *))0x005F1E10; //pt 
	void(*ReceivePacketPtr)(WORD Opcode, RecvPacketStruct *Packet) = (void(__cdecl *)(WORD, RecvPacketStruct *))0x006687B0; //os
	(* ReceivePacketPtr)(Opcode, Packet);

	delete Packet;
}