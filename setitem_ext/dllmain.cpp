// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <MyInlineHook.h>
#include <MyClass.h>
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>

#include <common.h>
#include <vector>
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")


CMyInlineHook g_HookObj_Setitem;
std::vector<PSETITEMDATA> g_vecSetItemArmor;
std::vector<PSETITEMDATA> g_vecSetitemAcc;

_inline void AddsetItemstats(Pshaiya_player player, PSETITEMDATA pSetItem, DWORD index)
{
	if (!index)
		return;

	SETITEMSTATS pValue = pSetItem->dwValue[index];
	player->set_item.BackUpValue.Str += pValue.Str;
	player->set_item.BackUpValue.Dex += pValue.Dex;
	player->set_item.BackUpValue.Luc += pValue.Luc;
	player->set_item.BackUpValue.Mp += pValue.Mp;
	player->set_item.BackUpValue.Rec += pValue.Rec;
	player->set_item.BackUpValue.__Int_ += pValue.__Int_;
	player->set_item.BackUpValue.Wis += pValue.Wis;
	player->set_item.BackUpValue.Hp += pValue.Hp;
	player->set_item.BackUpValue.Sp += pValue.Sp;
	player->set_item.IsSetted = true;
}

void __stdcall fun_setStatsThroughtItems(Pshaiya_player player)
{
	if (player->set_item.IsSetted)
	{
		player->total_str -= player->set_item.BackUpValue.Str;
		player->total_dex -= player->set_item.BackUpValue.Dex;
		player->total__Int_ -= player->set_item.BackUpValue.__Int_;
		player->total_wis -= player->set_item.BackUpValue.Wis;
		player->total_rec -= player->set_item.BackUpValue.Rec;
		player->total_luc -= player->set_item.BackUpValue.Luc;
		player->Maxhp -= player->set_item.BackUpValue.Hp;
		player->Maxmp -= player->set_item.BackUpValue.Mp;
		player->Maxsp -= player->set_item.BackUpValue.Sp;
		player->set_item.IsSetted = false;
	}

	ZeroMemory(&player->set_item.BackUpValue, sizeof(player->set_item.BackUpValue));


	//1. 提取身上所有ID
	DWORD dwItem[16] = { 0 };
	for (DWORD i = 0; i < 16; i++) {
		PplayerItemcharacterise pItem = player->BagItem[0][i];
		if (pItem) {
			dwItem[i] = pItem->ItemType * 1000 + pItem->Typeid;
		}
	}

	//2. 分类所需要的ID
	SETITEMDATA Armor;
 	Armor.dwItems[0] = dwItem[0];
 	Armor.dwItems[1] = dwItem[1];
 	Armor.dwItems[2] = dwItem[2];
 	Armor.dwItems[3] = dwItem[3];
 	Armor.dwItems[4] = dwItem[4];

	SETITEMDATA Accessories;
	Accessories.dwItems[0] = dwItem[8];
	Accessories.dwItems[1] = dwItem[9];
	Accessories.dwItems[2] = dwItem[10];
	Accessories.dwItems[3] = dwItem[11];
	Accessories.dwItems[4] = dwItem[12];

	//防具检测
	for (DWORD i = 0; i < g_vecSetItemArmor.size(); i++)
	{
		PSETITEMDATA pSetItem = g_vecSetItemArmor[i];
		DWORD index = 0;
		for (DWORD j = 0; j < _countof(pSetItem->dwItems); j++)
		{
			if (pSetItem->dwItems[j] &&
				pSetItem->dwItems[j] == Armor.dwItems[j]) {
				index++;
			}
		}
		if (index) {
			AddsetItemstats(player, pSetItem, index - 1);
			break;
		}
	}

	//首饰检测
	for (DWORD i = 0; i < g_vecSetitemAcc.size(); i++)
	{
		PSETITEMDATA pSetItem = g_vecSetitemAcc[i];
		DWORD index = 0;

		if (pSetItem->dwItems[0] &&
			pSetItem->dwItems[0] == Accessories.dwItems[0]) //项链
			index++;

		if (pSetItem->dwItems[1] &&
			pSetItem->dwItems[1] == Accessories.dwItems[1])//戒指
			index++;

		if (pSetItem->dwItems[1] &&
			pSetItem->dwItems[1] == Accessories.dwItems[2])//戒指
			index++;

		if (pSetItem->dwItems[2] &&
			pSetItem->dwItems[2] == Accessories.dwItems[3])//手镯
			index++;

		if (pSetItem->dwItems[2] &&
			pSetItem->dwItems[2] == Accessories.dwItems[4])//手镯
			index++;

		if (index) {
			AddsetItemstats(player, pSetItem, index - 1);
			break;
		}
	}


	if (player->set_item.IsSetted)
	{
		player->total_str += player->set_item.BackUpValue.Str;
		player->total_dex += player->set_item.BackUpValue.Dex;
		player->total__Int_ += player->set_item.BackUpValue.__Int_;
		player->total_wis += player->set_item.BackUpValue.Wis;
		player->total_rec += player->set_item.BackUpValue.Rec;
		player->total_luc += player->set_item.BackUpValue.Luc;
		player->Maxhp += player->set_item.BackUpValue.Hp;
		player->Maxmp += player->set_item.BackUpValue.Mp;
		player->Maxsp += player->set_item.BackUpValue.Sp;

		if (player->dwObjMap)
		{
			HPINFO stc;
			stc.charid = player->Charid;
			//hp
			stc._type = 0;
			stc.value = player->Maxhp;
			send_packet_specific_area(player, &stc, sizeof(stc));
			//mp
			stc._type = 1;
			stc.value = player->Maxsp;
			send_packet_specific_area(player, &stc, sizeof(stc));

			//sp
			stc._type = 2;
			stc.value = player->Maxmp;
			send_packet_specific_area(player, &stc, sizeof(stc));
		}
	}
}

__declspec(naked) void Naked_Setitem()
{
	_asm
	{
		pushad
		MYASMCALL_1(fun_setStatsThroughtItems, esi)
		popad
		sub esp, 0x8
		fild dword ptr[esi + 0x1220]
		jmp g_HookObj_Setitem.m_pRet
	}
}

bool load_setItem()
{
	char strFile[MAX_PATH] = { 0 };
	GetModuleFileNameA(GetModuleHandle(0), strFile, MAX_PATH);
	PathRemoveFileSpecA(strFile);
	strcat(strFile, "\\data\\setitem.csv");


	FILE* pFile = fopen(strFile, "r");
	if (pFile == NULL)
		return false;

	char szLine[1024] = { 0 };
	fgets(szLine, 1024, pFile);
	while (!feof(pFile))
	{
		SETITEMSTATS temp = { 0 };
		PSETITEMDATA pSetitem = new SETITEMDATA;
		ZeroMemory(pSetitem, sizeof(SETITEMDATA));
		ZeroMemory(szLine, sizeof(szLine));
		fgets(szLine, 1024, pFile);
		DWORD id;
		char szTemp[6][MAX_PATH] = { 0 };
		sscanf_s(szLine, "%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,]",
			&id, szTemp[5], MAX_PATH,
			&pSetitem->dwItems[0], szTemp[0], MAX_PATH,
			&pSetitem->dwItems[1], szTemp[1], MAX_PATH,
			&pSetitem->dwItems[2], szTemp[2], MAX_PATH,
			&pSetitem->dwItems[3], szTemp[3], MAX_PATH,
			&pSetitem->dwItems[4], szTemp[4], MAX_PATH);

		if (strcmp(szTemp[0], "0") == 0 &&
			strcmp(szTemp[1], "0") == 0 &&
			strcmp(szTemp[2], "0") == 0 &&
			strcmp(szTemp[3], "0") == 0 &&
			strcmp(szTemp[4], "0") == 0
			)
		{
			continue;
		}


		for (DWORD i = 0; i < 5; i++)
		{
			sscanf_s(szTemp[i], "%d|%d|%d|%d|%d|%d|%d|%d|%d",
				&pSetitem->dwValue[i].Str,
				&pSetitem->dwValue[i].Dex,
				&pSetitem->dwValue[i].__Int_,
				&pSetitem->dwValue[i].Wis,
				&pSetitem->dwValue[i].Rec,
				&pSetitem->dwValue[i].Luc,
				&pSetitem->dwValue[i].Hp,
				&pSetitem->dwValue[i].Mp,
				&pSetitem->dwValue[i].Sp);



			//如果这块没属性，就采用上一块的
			if (i)
			{
				if (memcmp(&pSetitem->dwValue[i], &temp, sizeof(SETITEMSTATS)) == 0)
				{
					memcpy(&pSetitem->dwValue[i], &pSetitem->dwValue[i - 1], sizeof(SETITEMSTATS));
				}
			}

		}

		DWORD dwType = pSetitem->dwItems[0] / 1000;
		if (dwType == 22 || dwType == 23 || dwType == 40)
		{
			g_vecSetitemAcc.push_back(pSetitem);
		}
		else {
			g_vecSetItemArmor.push_back(pSetitem);
		}
	}
	fclose(pFile);
	return true;
}


void mainProcess()
{
	//调整玩家的大小
	{
		DWORD temp = sizeof(shaiya_player);
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00411f74, &temp, sizeof(temp));
	}

	load_setItem();
	if (g_vecSetitemAcc.size() || g_vecSetItemArmor.size())
		g_HookObj_Setitem.Hook((PVOID)0x00460e40, Naked_Setitem, 9, fun_setStatsThroughtItems);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		mainProcess();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

