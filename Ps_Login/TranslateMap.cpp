// TranslateMap.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "MyInlineHook.h"
#include "TranslateMap.h"
#include <afxwin.h>
#include <vector>
#include "DataBase.h"
using std::vector;

#define VIPMAX 200

DWORD gColor_dwUid[VIPMAX] = { 0 };		                      //���ɴ�100��VIP�˺�
CMyInlineHook g_HookObj_Color;
PVOID gColor_pBackAddr = NULL;


__declspec(naked) void Naked_Color()
{
	__asm
	{	pushad
		lea eax, gColor_dwUid[0]      //������һ�������λ��
		mov ebx, 0x01			     //���һ��������
		mov esi, [esi+0x100]         //UID
	_Loop:
	mov edi, [eax]
		cmp edi, 0x00
		je _No                        //�����0�Ļ�������                    
		cmp esi, edi
		je _Yes                       //���UIDƥ��Ļ�
		cmp ebx, 0xc8
		jge _no                       //�����������200�˾�����
		add eax, 0x04                 //�ƶ�4���ֽ�
		add ebx, 0x01                 //��������1
		jmp _Loop
	_Yes :
	popad
		mov byte ptr   [esp + 0x7], 0xa
		mov ecx, dword ptr  [edx + 0x4]
		jmp gColor_pBackAddr
	_No :
	popad
		mov byte ptr   [esp + 0x7], cl     //ԭʼ����
		mov ecx, dword ptr  [edx + 0x4]
		jmp gColor_pBackAddr
	}
}



DWORD WINAPI TimerProc_Dynamic(_In_  LPVOID lpParameter)
{
	//����·��
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
		//ȡUID
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
	//��ȡVIP��ɫ�Ĺ���
	ZeroMemory(gColor_dwUid, sizeof(DWORD)*_countof(gColor_dwUid));
	::CreateThread(0, 0, TimerProc_Dynamic, 0, 0, 0);
	Sleep(5000);
	//HOOK :��һ����ps_login
//	gColor_pBackAddr=(PVOID)g_HookObj_Color.Hook((PVOID)0x00404936, Naked_Color, 7);
	gColor_pBackAddr = (PVOID)g_HookObj_Color.Hook((PVOID)0x00404974, Naked_Color, 7);
	return 0;
}





// ���ǵ���������һ��ʾ����
TRANSLATEMAP_API int Start(void)
{
	return 42;
}