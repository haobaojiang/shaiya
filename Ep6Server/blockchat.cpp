#include "stdafx.h"
#include "ChatFilter.h"






namespace BlockChat
{
	vector<DWORD> g_vecBacllist;

	void fun(void* p, const char* pstrMsg,bool* passed){
		Pshaiya_player Player = (Pshaiya_player)p;

		for (auto iter = g_vecBacllist.begin(); iter != g_vecBacllist.end(); iter++) {
			if (Player->uid == *iter) {
				LOGD << "illegal chat by uid:" << Player->uid;
				*passed = false;
				return;
			}
		}
	}



	void Start()
	{
		g_vecChatCallBack.push_back(fun);

		for (DWORD i = 0; i < 200; i++) {
			WCHAR key[50] = { 0 };
			swprintf(key, L"%d", i + 1);
			DWORD uid= GetPrivateProfileInt(L"BlockChat", key, 0, g_szFilePath);
			if (!uid)
				break;
			LOGD << "BlockChat uid config reading:" << uid << "added to black list!";
			g_vecBacllist.push_back(uid);
		}
	}
}
