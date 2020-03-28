
#include <windows.h>

#include <vector>
using std::vector;
#include <Shlwapi.h>
#include <MyInlineHook.h>
#pragma comment(lib,"Shlwapi.lib")

BYTE g_byMap = 0;

BOOL g_bEnable = FALSE;
DWORD g_dwReturnAddr1 = 0;
PVOID g_pfun1 = NULL;

DWORD g_dwReturnAddr2 = 0;



//写入地图
__declspec(naked) void Naked_1()
{
	_asm
	{
		mov eax, dword ptr ss : [esp + 0x8]
			push eax
			pushad
			add eax, 0x6
			mov al, byte ptr ds : [eax]
			mov g_byMap, al
			popad
			jmp g_dwReturnAddr1
	}
}

//读取地图
__declspec(naked) void Naked_2()
{
	_asm
	{
		pushad
			push eax
			call g_pfun1
			popad

			//原语句
			add esp, 0x4
			push eax
			push 0x6C57D8

			jmp g_dwReturnAddr2


	}
}




void __stdcall fun(char* strNotice)
{
	char strTemp[MAX_PATH] = { 0 };
	DWORD dwTemp = (DWORD)strTemp;


	//1.获取地图名
	DWORD dwCall = 0x004D2E90;
	DWORD dwMap = g_byMap;
	_asm
	{
		push dwMap
			push dwTemp
			mov ecx, 0x8655E8
			call dwCall
	}

	//2.组装字符串
	strcat(strNotice, "\nmap:");
	strcat(strNotice, strTemp);
}
void StartHook()
{
	//写
	CMyInlineHook obj1;
	g_dwReturnAddr1 = (DWORD)obj1.Hook((PVOID)0x0059d37c, Naked_1, 5);

	//读
	CMyInlineHook obj2;
	g_dwReturnAddr2 = (DWORD)obj2.Hook((PVOID)0x00528e8a, Naked_2, 9);
	g_pfun1 = fun;
}
