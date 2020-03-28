
// 头文件
#include <Windows.h>
#include <map>
#include <string>
#include <MyInlineHook.h>
#include <MyClass.h>
#include <define.h>

CMyInlineHook g_objitemMall, g_objNpcPurchase;

DWORD refuseItemMallAddr = 0x00479882;
DWORD refuseNpcAddr = 0x0046b6d6;
void DisConnectPlayer(DWORD player) {
	DWORD dwCall = 0x004d43a0;
	_asm {
		push 0x0
		push 0x9
		mov ecx,player
		call dwCall
	}
}

#define DELAYTIME (500)

bool WINAPI IsBuyAble(DWORD player)
{
	//检测时间间隔
	bool bRet = true;
	DWORD CurrentTime = GetTickCount();
	DWORD nextMallTime = *PDWORD(player + 0x5eb0);

	//有时，这个时间会不对

	do
	{
		if (!nextMallTime){
			break;
		}

		if (nextMallTime > CurrentTime)
		{
			bRet = false;
			DWORD leftTime = nextMallTime - CurrentTime;

			
			if (leftTime > DELAYTIME){
				bRet = true;          //特殊情况
				break;
			}

			if (leftTime < 200){
				DisConnectPlayer(player);
				break;
			}
		}

	} while (0);

	if (bRet) {
		*PDWORD(player + 0x5eb0) = CurrentTime + DELAYTIME;
	}
	return bRet;
}

__declspec(naked) void  Naked_itemMall()
{
	_asm {
		pushad
		MYASMCALL_1(IsBuyAble,ebx)
		cmp al,0x1
		popad

		je _org
		jmp refuseItemMallAddr

		_org:
		mov eax,dword ptr [0x546f6c]
		jmp g_objitemMall.m_pRet
	}
}

__declspec(naked) void  Naked_NpcPurchase()
{
	_asm {
		pushad
		MYASMCALL_1(IsBuyAble,edi)
		cmp al,0x1
		popad
		je _org
		jmp refuseNpcAddr

		_org:
		mov eax,dword ptr [ebx+0x2]
		push eax
		mov eax,dword ptr [edi+0xe0]
			jmp g_objNpcPurchase.m_pRet

	}
}


DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
{
	DWORD dwNewSize = 0x5eb8;
	MyWriteProcessMemory(GetCurrentProcess(),(PVOID)0x0040aa69,&dwNewSize,4);

	g_objitemMall.Hook((PVOID)0x00479811, Naked_itemMall, 5);
	g_objNpcPurchase.Hook(PVOID(0x0046b3a4), Naked_NpcPurchase, 10);
	return 0;
}




BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		::CreateThread(0, 0, ThreadProc, 0, 0, 0);	
		DisableThreadLibraryCalls(hModule);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
	}

	return TRUE;
}
