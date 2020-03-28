// TranslateMap.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "MyInlineHook.h"
#include "TranslateMap.h"

BOOL TranslateMap(PVOID pPlayer);

PVOID  pTranslateMap   = TranslateMap;
PVOID  pFailedAddr     = (PVOID)0x0047679A;
PVOID  pSuccessAddr    = (PVOID)0x00476c3e;
PVOID  pDeleteItemCall = (PVOID)0x004728E0;
WORD   wMap            = 33;
DWORD  dwItemid        = 100200;


__declspec(naked) void Naked()
{
	_asm
	{
		push ebx
		mov bx, wMap
		cmp word ptr [eax + 0x00000124], bx   
		pop ebx
		jne _Right
		pushad
		push ebp
		call pTranslateMap
		add esp,0x4
		cmp al,0
		popad
		je _Wrong
	_Right:
		cmp [eax + 0x00000124], cx
		jmp pSuccessAddr 
	_Wrong:
		jmp pFailedAddr
	}
}

BOOL TranslateMap(PVOID pPlayer)
{
	BYTE bySuccess = 0;
	BYTE byMaxBag = *(PBYTE((DWORD)pPlayer + 0x130c));
	if (!byMaxBag)  return FALSE;                     
	PDWORD pBagBase = PDWORD((DWORD)pPlayer + 0x220); 
	for (DWORD i = 1; i <= byMaxBag; i++)
	{
		for (DWORD j = 0; j < 24; j++, pBagBase++)
		{
			if (*pBagBase==NULL) continue;                           
			DWORD dwTempItemid = *PDWORD(*(PDWORD(*pBagBase + 0x30)));
			
			if (dwTempItemid == dwItemid)
			{
				//delete item
				_asm
				{
					push 0
					push j
					push i
					mov ecx, pPlayer
					call pDeleteItemCall
				}
				return TRUE;             
			}
		}
	}
	return FALSE;
}


DWORD WINAPI ThreadProc(_In_  LPVOID lpParameter)
{
	CMyInlineHook Hookobjone;
	Hookobjone.Hook((PVOID)0x00476c37, Naked, 7);     
	return 0;
}





// 这是导出函数的一个示例。
TRANSLATEMAP_API int Start(void)
{
	return 42;
}