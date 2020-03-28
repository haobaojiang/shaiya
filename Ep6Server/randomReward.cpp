#include "stdafx.h"
#include "randomReward.h"





namespace randomReward
{
	vector<PITEMINFO> g_vecRewards;

	char g_fmt[MAX_PATH] = { 0 };
	DWORD g_delay = 0;


	bool isTime() {
		static DWORD nextTime = GetTickCount() + g_delay;

		DWORD curTime = GetTickCount();

		if (nextTime > curTime) {
			return false;
		}
		nextTime = curTime + g_delay;
		return true;
	}

	void Loop()
	{

		if (!isTime()) {
			return;
		}


		vector<Pshaiya_player> vecTemp;

		for (DWORD i = 0; i < _countof(g_players); i++) {
			Pshaiya_player player = g_players[i];

			if (!player)
				break;
			
			if (!player->dwObjMap)
				continue;

			if (player->shopType == 2) {
				vecTemp.push_back(player);
			}
		}


		if (vecTemp.size()) {
			DWORD playerIndex = GetRand(vecTemp.size());
			DWORD rewardIndex = GetRand(g_vecRewards.size());

			Pshaiya_player player = vecTemp[playerIndex];
			PITEMINFO pItem = g_vecRewards[rewardIndex];
			sendItem(player, pItem);
			char strNotice[MAX_PATH] = { 0 };
			sprintf(strNotice, g_fmt, player->charname, pItem->itemname);
			LOGD << strNotice;
			sendNotice(strNotice);
		}
	}


	void start()
	{
		

		//¶ÁÅäÖÃ
		char strItems[MAX_PATH] = { 0 };

		GetPrivateProfileStringA("randomReward", "notice_fmt", "", g_fmt, MAX_PATH, g_strFilePath);
		GetPrivateProfileStringA("randomReward", "items", "", strItems, MAX_PATH, g_strFilePath);

		g_delay=GetPrivateProfileIntA("randomReward", "delay", 0, g_strFilePath)*1000;


		if (strItems[0] == '\0') return;

		char *delim = ",";
		char* p = strtok(strItems, delim);
		while (p) {

			DWORD itemid;
			sscanf(p, "%d", &itemid);

			if (!itemid)
				break;

			LOGD << "randomReward config reading,itemid:"<<itemid;

			PITEMINFO pItem=(PITEMINFO)GetItemAddress(itemid);
			g_vecRewards.push_back(pItem);

			p = strtok(NULL, delim);
		}

		if (g_vecRewards.size()) {
			
			g_vecLoopFunction.push_back(Loop);
		}
			
	}
}





