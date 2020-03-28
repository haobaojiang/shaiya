#include "stdafx.h"
#include "Hunting.h"


namespace Hunting
{
	DWORD g_Itemid = 0;
	DWORD g_rewardItem = 0;
	char* g_strCmd = "追杀 ";
	char* g_fmt = "%s,你已经被通缉,自己小心点吧!";


	vector<WORD> g_vecMaps;
	CMyInlineHook g_obj;

	void useItem(void* p, PplayerItemcharacterise pItem, bool* pRet) {
		Pshaiya50_player player = Pshaiya50_player(p);
		if (pItem->ItemAddr->itemid==g_Itemid) {
			player->isHuntingBuffPadding = true;
			sendNoticeToplayer(player, "你可以使用\"追杀 角色名\"来通缉玩家!");
		}
	}

	bool isHunted(Pshaiya50_player player) {
		return player->isHunted;
	}



	void onLogin(PVOID p) {
		Pshaiya50_player player = (Pshaiya50_player)p;
		EnterCriticalSection(&g_csHutingplayers);
		auto it = find(g_vecHuntedPlayerCid.begin(), g_vecHuntedPlayerCid.end(), player->Charid);
		if (it != g_vecHuntedPlayerCid.end()) {
			player->isHunted = true;
		}
		LeaveCriticalSection(&g_csHutingplayers);
	}


	void addHutingPlayer(Pshaiya50_player player) {
		player->isHunted = true;
		EnterCriticalSection(&g_csHutingplayers);
		auto it = find(g_vecHuntedPlayerCid.begin(), g_vecHuntedPlayerCid.end(), player->Charid);
		if (it == g_vecHuntedPlayerCid.end()) {
			g_vecHuntedPlayerCid.push_back(player->Charid);
		}
		
		LeaveCriticalSection(&g_csHutingplayers);
	}

	void fun_chat(PVOID p, const char* strChat, bool* pRet) {
		Pshaiya50_player player = Pshaiya50_player(p);

		if (!player->isHuntingBuffPadding)
			return;
		
		if (strncmp(g_strCmd,strChat,strlen(g_strCmd)) == 0) {
			*pRet = false;
			char strName[MAX_PATH] = { 0 };
			strChat += strlen(g_strCmd);
			memcpy(strName, strChat, 20);
			Pshaiya50_player pTarget = GetPlayerByCharName(strName);
			if (pTarget!=player&&pTarget) {
				
				//添加追杀
				addHutingPlayer(pTarget);
				
				//发公告
				char strMsg[MAX_PATH] = { 0 };
				sprintf(strMsg, g_fmt, pTarget->charname);
				sendNotice(strMsg);	
				player->isHuntingBuffPadding = false;
			}
			else {
				sendNoticeToplayer(player, "玩家不存在或不在线!");
			}
			
		}
	}

	void  enableKills(Pshaiya50_player deather , Pshaiya50_player killer,bool* pRet)
	{
		if (!*pRet)
			return;

		if (deather->isHunted) {
			EnterCriticalSection(&g_csHutingplayers);
			auto it = find(g_vecHuntedPlayerCid.begin(), g_vecHuntedPlayerCid.end(), deather->Charid);
			if (it != g_vecHuntedPlayerCid.end()) {
				g_vecHuntedPlayerCid.erase(it);
			}
			deather->isHunted = false;
			sendItem(killer, g_rewardItem);
			char strMsg[MAX_PATH] = { 0 };
			sprintf(strMsg, "[%s] 被 [%s] 杀死,通缉己解除,奖励己发送!", deather->charname, killer->charname);
			sendNotice(strMsg);
			LeaveCriticalSection(&g_csHutingplayers);
		}
	}
	


	void Start()
	{
		
		g_Itemid = GetPrivateProfileInt(L"Hunting", L"itemid", 0, g_szFilePath);
		g_rewardItem = GetPrivateProfileInt(L"Hunting", L"reward", 0, g_szFilePath);
		LOGD << "g_itemid:" << g_Itemid<<"g_rewardItem"<< g_rewardItem;
		g_vecEnteredMapCallBack.push_back(onLogin);
		g_chatCallBack.push_back(fun_chat);
		g_vecPreUseItemCallBack.push_back(useItem);
		g_enablekillsCallBack.push_back(enableKills);

	}
	
}