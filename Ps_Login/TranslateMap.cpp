// TranslateMap.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "MyInlineHook.h"
#include "TranslateMap.h"
#include <afxwin.h>
#include <vector>
#include "DataBase.h"
using std::vector;

#define VIPMAX 200

DWORD gColor_dwUid[VIPMAX] = { 0 };		                      //共可存100个VIP账号
CMyInlineHook g_HookObj_Color;
PVOID gColor_pBackAddr = NULL;


__declspec(naked) void Naked_Color()
{
	__asm
	{	pushad
		lea eax, gColor_dwUid[0]      //给他第一个数组的位置
		mov ebx, 0x01			     //添加一个计数器
		mov esi, [esi+0x100]         //UID
	_Loop:
	mov edi, [eax]
		cmp edi, 0x00
		je _No                        //如果是0的话就跳走                    
		cmp esi, edi
		je _Yes                       //如果UID匹配的话
		cmp ebx, 0xc8
		jge _no                       //如果计数器到200了就走跳
		add eax, 0x04                 //移动4个字节
		add ebx, 0x01                 //计数器加1
		jmp _Loop
	_Yes :
	popad
		mov byte ptr   [esp + 0x7], 0xa
		mov ecx, dword ptr  [edx + 0x4]
		jmp gColor_pBackAddr
	_No :
	popad
		mov byte ptr   [esp + 0x7], cl     //原始代码
		mov ecx, dword ptr  [edx + 0x4]
		jmp gColor_pBackAddr
	}
}



DWORD WINAPI TimerProc_Dynamic(_In_  LPVOID lpParameter)
{
	//设置路径
	WCHAR szTemp[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(0), szTemp, MAX_PATH);
	PathRemoveFileSpec(szTemp);
	CString szFilePath = szTemp;
	szFilePath = szFilePath + L"\\config.ini";


	CDataBase objData;
	if (!objData.LinkDataBase(L"ps_userdata"))
	{
		TerminateProcess(GetCurrentProcess(), 0);
	}
	while (1)
	{
		//取UID
		Sleep(10000);
		objData.clear();
		objData.ExeSqlByCommand(L"select useruid from users_master where vip=1");
		DWORD i = 0;
		ZeroMemory(gColor_dwUid, sizeof(gColor_dwUid));
		while (!objData.IsEmpty())
		{
			gColor_dwUid[i++] = _tcstoul(objData.GetValueByField(L"useruid"), 0, 10);
			objData.MoveNext();
		}
// 		WCHAR  szReturn[MAX_PATH*6] = { 0 };
// 		CString szTemp[VIPMAX];
// 		DWORD dwTemp[VIPMAX] = { 0 };
// 		GetPrivateProfileString(L"VopColor", L"UID", L"", szReturn, MAX_PATH*6, szFilePath);
// 		ZeroMemory(dwTemp, sizeof(dwTemp));
// 		for (DWORD j=0; j < _countof(szTemp); j++)
// 		{
// 			AfxExtractSubString(szTemp[j], szReturn, j, L',');
// 			if (szTemp[j] == L"")
// 				break;
// 			else
// 				dwTemp[j] = _tcstoul(szTemp[j], 0, 10);
// 		}
// 		memmove_s(gColor_dwUid, sizeof(gColor_dwUid), dwTemp, sizeof(dwTemp));
	}
	return 0;
}


DWORD WINAPI ThreadProc(_In_  LPVOID lpParameter)
{
	//读取VIP颜色的功能
	ZeroMemory(gColor_dwUid, sizeof(DWORD)*_countof(gColor_dwUid));
	::CreateThread(0, 0, TimerProc_Dynamic, 0, 0, 0);
	Sleep(5000);
	//HOOK :不一样的ps_login
//	gColor_pBackAddr=(PVOID)g_HookObj_Color.Hook((PVOID)0x00404936, Naked_Color, 7);
	gColor_pBackAddr = (PVOID)g_HookObj_Color.Hook((PVOID)0x00404974, Naked_Color, 7);
	return 0;
}





// 这是导出函数的一个示例。
TRANSLATEMAP_API int Start(void)
{
	return 42;
}