#pragma once
#include <windows.h>
#include <MyInlineHook.h>
#include <vector>
#include "Rankcap.h"
#include <MyClass.h>
using std::vector;
RankData g_RankData[100] = { 0 }; 



void StartRankCap()
{
	//最高战功等级限制去掉
	DWORD i = 0;
	DWORD dwLevel = i;
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00541f70, &dwLevel, sizeof(DWORD));
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00541f74, &dwLevel, sizeof(DWORD));
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00541f78, &dwLevel, sizeof(DWORD));

	//战功需求数
	{
		DWORD pTemp[] = { 0x0048c432, 0x0048c40a, 0x0048c4ff, 0x0048c58f };
		DWORD pValue = (DWORD)&g_RankData[0].dwKills;
		for (DWORD j = 0; j < _countof(pTemp); j++)
		{
			MyWriteProcessMemory(GetCurrentProcess(), PVOID(pTemp[j]), &pValue, sizeof(DWORD));
		}
	}

	//奖励点数
	{
		DWORD pValue = (DWORD)&g_RankData[0].dwNmPoint;
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0048c446, &pValue, sizeof(DWORD));
	}

	//最高战功限制
	// 	{
	// 		DWORD pValue =(DWORD) &g_RankData[i-1].dwKills;
	// 		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00472870, &pValue, sizeof(DWORD));
	// 	}
	DWORD pValue = 0x541ea0;
	*(PDWORD)pValue = g_RankData[i - 1].dwKills;

	DWORD dwTemp = DWORD(&g_RankData[0]) - 0x10;

	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0048c613, &dwTemp, sizeof(DWORD));
}