#include "stdafx.h"
#include "onLineInfToClient.h"




namespace onLineInfToClient
{
	char g_strCheckMap[MAX_PATH] = { 0 };
	char g_strCheckFaction[MAX_PATH] = { 0 };
	char g_strCheckAll[MAX_PATH] = { 0 };


	char g_strWelcomeMap[MAX_PATH] = { 0 };
	char g_strWelcomeFaction[MAX_PATH] = { 0 };
	char g_strWelcomeAll[MAX_PATH] = { 0 };

	DWORD g_keepTime = 0;

	bool __stdcall isCheckAble(Pshaiya50_player player) {
		return isLegalTime(&player->nextCheckOnlineTime, 10*1000);
	}

	void checkMapOnline(void* p, const char* pstrChat, bool* IsShowChat) {

		if (strcmp(pstrChat, g_strCheckMap) != 0)
			return;

		Pshaiya50_player player = (Pshaiya50_player)p;
		SHAIYAONLINEINFO st;
		_InitPacket(st);
		strcpy(st.title, g_strWelcomeMap);
		st.keepTime = g_keepTime;
		IsShowChat = false;

		if (!isCheckAble(player)) {
			sendNoticeToplayer(player, "查询时间是有间隔的,请晚点再查!");
			return;
		}

		for (DWORD i = 0; i < _countof(g_players); i++) {

			Pshaiya50_player target = (Pshaiya50_player)g_players[i];

			if (IsIllagePlayer(target))
				continue;

			if (target->dwObjMap != player->dwObjMap)
				continue;

			strcpy(st.playerNames[st.count], target->charname);
			if (++st.count >= 90) {
				break;
			}
		}

		if (st.count) {
			SendPacketToPlayer(player, &st, sizeof(st));
		}
		else {
			sendNoticeToplayer(player, "未能取到玩家在线!");
		}
	}
	void checkFactionOnline(void* p, const char* pstrChat, bool* IsShowChat) {

		if (strcmp(pstrChat, g_strCheckFaction) != 0)
			return;

		Pshaiya50_player player = (Pshaiya50_player)p;
		SHAIYAONLINEINFO st;
		_InitPacket(st);
		IsShowChat = false;
		strcpy(st.title, g_strWelcomeFaction);
		st.keepTime = g_keepTime;


		if (!isCheckAble(player)) {
			sendNoticeToplayer(player, "查询时间是有间隔的,请晚点再查!");
			return;
		}

		for (DWORD i = 0; i < _countof(g_players); i++) {

			Pshaiya50_player target = (Pshaiya50_player)g_players[i];

			if (IsIllagePlayer(target))
				continue;

			if (target->Country != player->Country)
				continue;

			strcpy(st.playerNames[st.count], target->charname);
			if (++st.count >= 90) {
				break;
			}
		}

		if (st.count) {
			SendPacketToPlayer(player, &st, sizeof(st));
		}
		else {
			sendNoticeToplayer(player, "未能取到玩家在线!");
		}
	}
	void checkAllOnline(void* p, const char* pstrChat, bool* IsShowChat) {

		if (strcmp(pstrChat, g_strCheckAll) != 0)
			return;

		Pshaiya50_player player = (Pshaiya50_player)p;
		SHAIYAONLINEINFO st;
		_InitPacket(st);
		IsShowChat = false;

		strcpy(st.title, g_strWelcomeAll);
		st.keepTime = g_keepTime;

		if (!isCheckAble(player)) {
			sendNoticeToplayer(player, "查询时间是有间隔的,请晚点再查!");
			return;
		}

		for (DWORD i = 0; i < _countof(g_players); i++) {

			Pshaiya50_player target = (Pshaiya50_player)g_players[i];

			if (IsIllagePlayer(target))
				continue;


			strcpy(st.playerNames[st.count], target->charname);
			if (++st.count >= 90) {
				break;
			}
		}

		if (st.count) {
			SendPacketToPlayer(player, &st, sizeof(st));
		}
		else {
			sendNoticeToplayer(player, "未能取到玩家在线!");
		}
	}

	void fun(void* p, const char* pstrChat, bool* IsShowChat){
		checkMapOnline(p, pstrChat, IsShowChat);
		checkFactionOnline(p, pstrChat, IsShowChat);
		checkAllOnline(p, pstrChat, IsShowChat);
	 }




	void Start()
	{
		g_keepTime = GetPrivateProfileIntA("onLineInfToClient", "keeptime", 10, g_strFilePath);
		GetPrivateProfileStringA("onLineInfToClient", "welcome1", "", g_strWelcomeAll, MAX_PATH, g_strFilePath);
		GetPrivateProfileStringA("onLineInfToClient", "welcome2", "", g_strWelcomeMap, MAX_PATH, g_strFilePath);
		GetPrivateProfileStringA("onLineInfToClient", "welcome3", "", g_strWelcomeFaction, MAX_PATH, g_strFilePath);

		GetPrivateProfileStringA("onLineInfToClient", "cmdMap", "", g_strCheckMap, MAX_PATH, g_strFilePath);
		GetPrivateProfileStringA("onLineInfToClient", "cmdFaction", "", g_strCheckFaction, MAX_PATH, g_strFilePath);
		GetPrivateProfileStringA("onLineInfToClient", "cmdAll", "", g_strCheckAll, MAX_PATH, g_strFilePath);
		g_chatCallBack.push_back(fun);
	}
}




