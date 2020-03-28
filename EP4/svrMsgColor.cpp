#include "stdafx.h"
#include "namecolor.h"




namespace svrMsgColor
{
	CMyInlineHook g_objGet;
	CMyInlineHook g_objClear;
	CMyInlineHook g_objSend;
	CMyInlineHook g_objSendNorMal;


	__declspec(naked) void  Naked_sendNormal()
	{
		_asm
		{
			pushad
			lea ecx, [esp + 0x44]
			lea ecx, [ecx + eax]
			lea ecx, [ecx + 0x18]   //the end of string
			mov ax, word ptr[ebx + 0x5841]  //r g
			mov word ptr[ecx], ax
			lea ecx, [ecx + 0x2]
			mov al, byte ptr[ebx + 0x5843]  //b
			mov byte ptr[ecx], al
			popad
			mov ecx, [ebx + 0x000000DC]
				jmp g_objSendNorMal.m_pRet
		}
	}



	__declspec(naked) void  Naked_get()
	{
		_asm
		{
			mov byte ptr[ebp + 0x00005840], 01
			pushad
			movzx ebx, byte ptr[eax + 0x6a]
			mov byte ptr[ebp + 0x5841], bl
			movzx ebx, byte ptr[eax + 0x6c]
			mov byte ptr[ebp + 0x5842], bl
			movzx ebx, byte ptr[eax + 0x6e]
			mov byte ptr[ebp + 0x5843], bl
			popad
			jmp g_objGet.m_pRet
		}
	}


	__declspec(naked) void  Naked_clear()
	{
		_asm
		{
			mov dword ptr[esi + 0x00005840], 00
			jmp g_objClear.m_pRet
		}
	}


	__declspec(naked) void  Naked_send()
	{
		_asm
		{
			pushad
			lea ecx, [esp + 0x44]
			lea ecx, [ecx + eax]
			lea ecx, [ecx + 0x18]   //the end of string

			mov ax, word ptr[ebx + 0x5841]  //r g
			mov word ptr[ecx], ax
			lea ecx, [ecx + 0x2]

			mov al, byte ptr[ebx + 0x5843]  //b
			mov byte ptr[ecx], al
			popad
			mov ecx, [ebx + 0x000000DC]
			jmp g_objSend.m_pRet
		}
	}



	void Start()
	{
		g_objGet.Hook((PVOID)0x00466086, Naked_get, 7);
		g_objSend.Hook((PVOID)0x00471077, Naked_send, 6);
		g_objClear.Hook((PVOID)0x004710d3, Naked_clear, 7);
		g_objSendNorMal.Hook((PVOID)0x00470657, Naked_sendNormal, 6);

		BYTE temp = 0x1b;
		{
			MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00470668, &temp, sizeof(temp));
			MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00471088, &temp, sizeof(temp));
		}

	}
}




