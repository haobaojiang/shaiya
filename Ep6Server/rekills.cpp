
#include <string>
#include <vector>
#include <windows.h>
#include <MyInlineHook.h>
#include "Relevel.h"
#include <common.h>
#include <map>
#include <MyPython.h>

#include "shaiya_relevel.h"

using std::vector;
using std::string;



extern vector<fun_chatcallback> g_vecChatCallBack;

extern WCHAR g_szFilePath[MAX_PATH];

namespace Rekills
{
	MyPython*  g_objPy = NULL;
	CMyInlineHook obj;
	DWORD dwTemp = 0x004730AF;
	std::map<std::string, BYTE>g_mapGmset;
	

	int g_relevel_pointsperlevel = 0;
	int g_relevel_MaxLevel = 0;
	BYTE g_relevel_RequireLevel = 0;
	bool g_relevel_RequireVip = false;
	DWORD g_relevel_rewardItem = 0;

	__declspec(naked) void Naked_statsPointUsed()
	{
		_asm
		{

			pushad
			push edi
			call obj.m_pfun_call
			popad
			jmp dwTemp     //原语句就是这个
		}
	}



	void __stdcall fun_resetStatspoint(Pshaiya_player player)
	{
		DWORD reLevelPoints = 0;
		g_objPy->lock();
		try
		{
			reLevelPoints = get_relevelPoints(player->Charid)*g_relevel_pointsperlevel;
		}
		catch (...)
		{
			;
		}
		g_objPy->Unlock();



		if (reLevelPoints)
		{
			DWORD dwStatsPoints = player->statspoint + reLevelPoints;
			gmcall::gmsetCommand(player, gmSetCommandCase::SetStatpoint, dwStatsPoints);
		}

	}

	void  CheckrelevelOrder(Pshaiya_player player, char* str)
	{
		//relevel
		if (strncmp(str, "我要转生", strlen("我要转生")) == 0&&player->Level== g_relevel_RequireLevel)
		{
			//check if weared gear
			for (DWORD i = 0; i < 24; i++) {
				if (player->BagItem[0][i]) {
					return;
				}
			}


			gmcall::gmsetCommand(player, gmSetCommandCase::SetLeve, 1);

			DWORD dwRebornTime = 0;
			g_objPy->lock();
			try
			{
				add_relevel(player->Charid, g_relevel_MaxLevel);
				dwRebornTime = get_relevelPoints(player->Charid);
			}
			catch (...)
			{
				;
			}
			g_objPy->Unlock();


			if (g_relevel_rewardItem&&dwRebornTime>1) {
				sendItem(player, g_relevel_rewardItem);
			}

			gmcall::gmsetCommand(player, gmSetCommandCase::SetStatpoint, dwRebornTime*g_relevel_pointsperlevel);


		}

		//help add stats
		char* strTemp = new char[MAX_PATH];
		DWORD value = 0;
		sscanf(str, "%s %d", strTemp, &value);  

		std::map <std::string, BYTE>::iterator mapIter;
		for (mapIter = g_mapGmset.begin(); mapIter != g_mapGmset.end(); mapIter++){
			if (mapIter->first.compare(strTemp) == 0 && value){
				gmcall::addstats_helper(player, gmSetCommandCase(mapIter->second), value);
				break;
			}
		}

		delete[] strTemp;
	}

	void Start(){

		g_objPy = new MyPython();

		initshaiya_relevel();
		g_relevel_MaxLevel= GetPrivateProfileInt(L"Relevel", L"maxtimes", 0, g_szFilePath);
		g_relevel_pointsperlevel= GetPrivateProfileInt(L"Relevel", L"pointsperlevel", 0, g_szFilePath);
		g_relevel_RequireLevel = GetPrivateProfileInt(L"Relevel", L"requirelevel", 0, g_szFilePath);
		g_relevel_RequireVip= GetPrivateProfileInt(L"Relevel", L"requireVip", 0, g_szFilePath);
		g_relevel_rewardItem= GetPrivateProfileInt(L"Relevel", L"rewardItem", 0, g_szFilePath);


		g_vecChatCallBack.push_back(CheckrelevelOrder);
		obj.Hook((PVOID)0x00473358, Naked_statsPointUsed, 5, fun_resetStatspoint);

	}
}
