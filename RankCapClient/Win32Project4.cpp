// Win32Project4.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include "FileReadWrite.h"
#include <MyClass.h>

using std::vector;


typedef struct _RankData
{
	DWORD dwNmPoint;
	DWORD dwHmPoint;
	DWORD dwUmPoint;
	DWORD dwKills;
}RankData, *PRankData;


RankData g_RankData[100] = { 0 };


//#define _EP4 1

int _tmain(int argc, _TCHAR* argv[])
{

	//1.从文件中读取数植
	
	WCHAR szTemp[MAX_PATH] = { 0 };
	WCHAR szFilePath[MAX_PATH] = { 0 };
	WCHAR szGame[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(0), szTemp, MAX_PATH);
	PathRemoveFileSpec(szTemp);
	swprintf_s(szFilePath, MAX_PATH, L"%s\\config.ini", szTemp);
	swprintf_s(szGame,     MAX_PATH, L"%s\\game.exe",   szTemp);

#ifdef _DEBUG
	wcscpy_s(szFilePath,MAX_PATH, L"E:\\MyShaiyaWorkingSpace\\Rankcap\\config.ini");
	wcscpy_s(szGame, MAX_PATH, L"E:\\MyShaiyaWorkingSpace\\Rankcap\\local_rankcap.exe");
#endif  




#ifdef _EP4
	DWORD dwOffset = 0x01e45000;
#else
	//DWORD dwOffset = 0x00345e70;
	DWORD dwOffset = 0x003c0b00;
#endif


	CFileReadWrite obj;
	obj.Open(szGame);
	obj.SetOffset(dwOffset+4);//要加4个字节,因为。。。。

	DWORD i = 0;
	for (i = 0; i < 100; i++)
	{
		WCHAR szKey[MAX_PATH] = { 0 };
		WCHAR szReturn[MAX_PATH] = { 0 };
		swprintf_s(szKey, MAX_PATH, L"%d", i);
		if (GetPrivateProfileString(L"RankCap", szKey, L"", szReturn, MAX_PATH, szFilePath) <= 0) break;

		//切割出字符串出来
		CString szTemp[4];
		for (DWORD j = 0; j < _countof(szTemp); j++)
		{
			AfxExtractSubString(szTemp[j], szReturn, j, L',');
		}
		g_RankData[i].dwKills   = _tcstoul(szTemp[0], 0, 10);
		g_RankData[i].dwNmPoint = _tcstoul(szTemp[1], 0, 10);
		g_RankData[i].dwHmPoint = _tcstoul(szTemp[2], 0, 10);
		g_RankData[i].dwUmPoint = _tcstoul(szTemp[3], 0, 10);
	}

	{
		printf("wring dwNmPoint\n");
		for (DWORD j = 0; j < i; j++)
		{
			obj.WriteData(&g_RankData[j].dwNmPoint, sizeof(DWORD));
		}
	}

	{
		printf("writing dwHmPoint\n");
		for (DWORD j = 0; j < i; j++)
		{
			obj.WriteData(&g_RankData[j].dwHmPoint, sizeof(DWORD));
		}
	}

	{
		printf("writing dwUmPoint\n");
		for (DWORD j = 0; j < i; j++)
		{
			obj.WriteData(&g_RankData[j].dwUmPoint, sizeof(DWORD));
		}
	}

	{
		printf("writing dwKills\n");
		for (DWORD j = 0; j < i; j++)
		{
			obj.WriteData(&g_RankData[j].dwKills, sizeof(DWORD));
		}
	}

	

#ifdef _EP4
	DWORD dwLevelTemp[] = { 0x001068a9, 0x00106965, 0x00106476, 0x00106493 };
	DWORD dwNmPointTemp[] = { 0x001066a1, 0x00107128 };
	DWORD dwHmPointTemp[] = { 0x001066b2, 0x00107136 };
	DWORD dwUmPointTemp[] = { 0x001066c4, 0x00107144 };
	dwOffset = 0x02245000;//要转变成内存地址
	DWORD dwRankTemp[] = { 0x001068cd, 0x001064b7, 0x001064d4, 0x0010698a, 0x00106b3c, 0x00106b9e, 0x001071f1, 0x00107301 };

#else
	// 	DWORD dwLevelTemp[] = { 0x001412f5, 0x00141328, 0x00141792, 0x0014184e };
	// 	DWORD dwNmPointTemp[] = { 0x00141540 };
	// 	DWORD dwHmPointTemp[] = { 0x00141552 };
	// 	DWORD dwUmPointTemp[] = { 0x00141564 };
	// 	dwOffset = 0x00745e70;  //要转变成内存地址
	// 	DWORD dwRankTemp[] = { 0x00141350,0x0014136d,0x001417ba,0x00141877,0x00141a2a,0x00141a9f,0x001421ae,0x001422e1};
	DWORD dwLevelTemp[] = { 0x0012e0e5, 0x0012e118, 0x0012e582, 0x0012e63d };
	DWORD dwNmPointTemp[] = { 0x0012e330 };
	DWORD dwHmPointTemp[] = { 0x0012e342 };
	DWORD dwUmPointTemp[] = { 0x0012e354 };
	dwOffset = 0x2308100;  //要转变成内存地址
	DWORD dwRankTemp[] = { 0x0012e140, 0x0012e15d, 0x0012e5aa, 0x0012e667, 0x0012e81a, 0x0012e88f, 0x0012ef8e, 0x0012f0c1 };
#endif


	DWORD dwLevel = i;
	DWORD dwTemp = i - 1;
	//修改最高等级限制
	{
		for (DWORD j = 0; j < _countof(dwLevelTemp); j++)
		{
			obj.WriteData(dwLevelTemp[j], &dwLevel, sizeof(DWORD));
		}
	}

	
#ifdef _EP4
	obj.WriteData(0x440fc, &dwTemp, sizeof(DWORD));
	obj.WriteData(0xf43d2, &dwTemp, sizeof(DWORD));
#else
// 	obj.WriteData(0x4ebf4, &dwTemp, sizeof(DWORD));
// 	obj.WriteData(0x1299ab, &dwTemp, sizeof(DWORD));
	obj.WriteData(0x46ce4, &dwTemp, sizeof(DWORD));
	obj.WriteData(0x11858b, &dwTemp, sizeof(DWORD));
#endif


	//修改普通
	DWORD dwNmPoint = dwOffset + i * 0;
	for (DWORD j = 0; j < _countof(dwNmPointTemp); j++)
	{
		obj.WriteData(dwNmPointTemp[j] ,&dwNmPoint, sizeof(DWORD));
	}


	
	//修改困难
	DWORD dwHmPoint = dwOffset+i*4;
	for (DWORD j = 0; j < _countof(dwHmPointTemp); j++)
	{
		obj.WriteData(dwHmPointTemp[j] ,& dwHmPoint, sizeof(DWORD));
	}


	//修改死亡
	DWORD dwUmPoint = dwOffset + i * 8;
	for (DWORD j = 0; j < _countof(dwUmPointTemp); j++)
	{
		obj.WriteData(dwUmPointTemp[j] ,& dwUmPoint, sizeof(DWORD));
	}

	//修改战功
	DWORD dwRank = dwOffset + i * 12;
	for (DWORD j = 0; j < _countof(dwRankTemp); j++)
	{
		obj.WriteData(dwRankTemp[j], &dwRank, sizeof(DWORD));
	}




	dwRank += 4;
#ifdef _EP4
	obj.WriteData(0x000f43d9, &dwRank, sizeof(DWORD));
	obj.WriteData(0x00044103, &dwRank, sizeof(DWORD));
#else
// 	obj.WriteData(0x4ec03, &dwRank, sizeof(DWORD));
// 	obj.WriteData(0x1299b3, &dwRank, sizeof(DWORD));
	obj.WriteData(0x46cf3, &dwRank, sizeof(DWORD));
	obj.WriteData(0x118593, &dwRank, sizeof(DWORD));
#endif

	
	obj.Close();
	system("pause");
	return 0;
}

