// TranslateMap.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "MyInlineHook.h"
#include "TranslateMap.h"
#include <afxwin.h>
#include <vector>
#include <MyClass.h>
using std::vector;

//#define  _EP4 1

typedef struct _RankData
{
	DWORD dwLevel;  //�׶�
	DWORD dwKills;
	DWORD dwNmPoint;
	DWORD dwHmPoint;
	DWORD dwUmPoint;
}RankData,*PRankData;


RankData g_RankData[100] = { 0 }; 

DWORD WINAPI ThreadProc(_In_  LPVOID lpParameter)
{
	//��ȡconfig
	WCHAR szFilePath[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(0), szFilePath, MAX_PATH);
	PathRemoveFileSpec(szFilePath);
	wcscat_s(szFilePath, MAX_PATH, L"\\config.ini");

	ZeroMemory(g_RankData, sizeof(g_RankData));
	DWORD i = 0;
	for (i = 0; i < 100; i++)
	{
		WCHAR szKey[MAX_PATH] = { 0 };
		WCHAR szReturn[MAX_PATH] = { 0 };
		swprintf_s(szKey, MAX_PATH, L"%d", i);
		
		if (GetPrivateProfileString(L"RankCap", szKey, L"", szReturn, MAX_PATH, szFilePath) <= 0) break;

		//�и���ַ�������
		CString szTemp[4];
		for (DWORD j = 0; j < _countof(szTemp); j++)
		{
			AfxExtractSubString(szTemp[j], szReturn, j, L',');
		}

		g_RankData[i].dwLevel   = i+1;
		g_RankData[i].dwKills   = _tcstoul(szTemp[0], 0, 10);
		g_RankData[i].dwNmPoint = _tcstoul(szTemp[1], 0, 10);
		g_RankData[i].dwHmPoint = _tcstoul(szTemp[2], 0, 10);
		g_RankData[i].dwUmPoint = _tcstoul(szTemp[3], 0, 10);

	}

	//���ս���ȼ�����ȥ��
	DWORD dwLevel = i;
#ifdef _EP4
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00541f70, &dwLevel, sizeof(DWORD));
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00541f74, &dwLevel, sizeof(DWORD));
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00541f78, &dwLevel, sizeof(DWORD));
#else
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x582234, &dwLevel, sizeof(DWORD));
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x582238, &dwLevel, sizeof(DWORD));
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x58223c, &dwLevel, sizeof(DWORD));
#endif



	//ս��������
	{
#ifdef _EP4
		DWORD pTemp[] = { 0x0048c432, 0x0048c40a, 0x0048c4ff, 0x0048c58f};
#else
		DWORD pTemp[] = { 0x0049cdf6, 0x0049ce20, 0x0049cebf, 0x0049cf1f };
#endif
		DWORD pValue = (DWORD)&g_RankData[0].dwKills;
		for (DWORD j = 0; j < _countof(pTemp); j++)
		{
			MyWriteProcessMemory(GetCurrentProcess(), PVOID(pTemp[j]), &pValue, sizeof(DWORD));
		}
	}

	//��������
	{
		DWORD pValue = (DWORD)&g_RankData[0].dwNmPoint;
#ifdef _EP4
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0048c446, &pValue, sizeof(DWORD));
#else
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0049ce32, &pValue, sizeof(DWORD));	
#endif	
	}
    
	//���ս������
// 	{
// 		DWORD pValue =(DWORD) &g_RankData[i-1].dwKills;
// 		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00472870, &pValue, sizeof(DWORD));
// 	}

#ifdef _EP4
	DWORD pValue = 0x541ea0;
#else
	DWORD pValue = 0x582224;
#endif 
	
	*(PDWORD)pValue = g_RankData[i - 1].dwKills;
	DWORD dwTemp = DWORD(&g_RankData[0])-0x10;
#ifdef _EP4
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0048c613, &dwTemp, sizeof(DWORD));
#else
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0049cf93, &dwTemp, sizeof(DWORD));
#endif
	
	return 0;
}





// ���ǵ���������һ��ʾ����
TRANSLATEMAP_API int Start(void)
{
	return 42;
}