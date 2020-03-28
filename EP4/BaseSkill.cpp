#include "stdafx.h"
#include "recrune.h"
#include <map>

using std::map;

namespace BaseSkill
{

	std::map<WORD,BYTE> g_mapSkills;
	WORD g_level = 0;

	void  EnterMap(void* p) {
		Pshaiya50_player player = (Pshaiya50_player)p;

		if (player->Level <= g_level) {
			for (auto iter = g_mapSkills.begin(); iter != g_mapSkills.end(); iter++) {
				UseItemSkill((DWORD)player, iter->first, iter->second);
			}
		}
	}


	


	void Start()
	{	
	

		g_vecEnteredMapCallBack.push_back(EnterMap);

		g_level = GetPrivateProfileInt(L"BaseSkill", L"level",60,g_szFilePath);


		for (DWORD i = 0; i < 100; i++) {
			WCHAR key[50] = { 0 };
			WCHAR temp[MAX_PATH] = { 0 };
			swprintf(key, L"%d", i+1);
			GetPrivateProfileString(L"BaseSkill", key, L"", temp, MAX_PATH, g_szFilePath);
			if (temp[0] == L'0')
				break;

			WORD skill = 0;
			BYTE skillLevel = 0;
			swscanf(temp, L"%hd,%hhd", &skill, &skillLevel);
			if (skill&&skillLevel) {
				LOGD << "baseSkill,Skill added:" << skill << "::" << skillLevel;
				g_mapSkills.insert(std::pair<WORD, BYTE>(skill, skillLevel));
			}
		}
	}
}





