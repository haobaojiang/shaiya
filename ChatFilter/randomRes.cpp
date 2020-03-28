#include "stdafx.h"
#include "randomRes.h"






namespace randomRes
{
	CMyInlineHook g_obj;

	vector<RANDOMRES> g_vecRandomRes;
	
	bool g_isResProtect = false;
	DWORD g_skillObj;
	CRITICAL_SECTION g_cs;
	map<Pshaiya50_player, DWORD> g_mapResPlayers;

	void ResProtect(void* p) {
		while (1)
		{
			EnterCriticalSection(&g_cs);
			auto iter = g_mapResPlayers.begin();
			if (iter != g_mapResPlayers.end()) {
				if (GetTickCount() > iter->second) {
					UseItemSkill((DWORD)iter->first, g_skillObj);
					g_mapResPlayers.erase(iter);
				}
			}
			LeaveCriticalSection(&g_cs);
			Sleep(200);
		}
	}

	

	void __stdcall fun(Pshaiya50_player player,WORD Map,Pshaiya_pos pPos)
	{
		//随机复活
		for (auto iter = g_vecRandomRes.begin(); iter != g_vecRandomRes.end();iter++) {
			if (iter->Map == Map) {
				DWORD index = GetRand(iter->vecPos.size());
				Pshaiya_pos Pos = &iter->vecPos[index];
				memcpy(pPos, Pos, sizeof(shaiya_pos));
				break;
			}
		}

		//复活保护
		if (g_isResProtect) {
			EnterCriticalSection(&g_cs);
			g_mapResPlayers.insert(std::pair<Pshaiya50_player, DWORD>(player, GetTickCount() + 1 * 1000));
			LeaveCriticalSection(&g_cs);
		}
	}


	__declspec(naked) void  Naked_1()
	{
		_asm {

			pushad
			mov eax, dword ptr[esp + 0x24]
			mov esi, dword ptr[esp + 0x2c]
			MYASMCALL_3(fun,ecx,esi,eax)
			popad
			sub esp,0x84
			jmp g_obj.m_pRet
		}
	}


	

	void Start()
	{
		g_obj.Hook(PVOID(0x00459ec0), Naked_1, 6);


		for (DWORD i = 0; i < 100; i++) {
			RANDOMRES st;
			WCHAR appName[MAX_PATH] = { 0 };
			swprintf(appName, L"randomRes_%d", i + 1);
			 st.Map = GetPrivateProfileInt(appName, L"map", 0, g_szFilePath);
			 if (!st.Map) {
				 break;
			 }

			 for (DWORD j = 0; j < 100; j++) {
				 WCHAR key[50] = { 0 };
				 WCHAR szPos[MAX_PATH] = { 0 };
				 shaiya_pos pos;
				 swprintf_s(key, L"%ld", j + 1);

				 GetPrivateProfileString(appName, key, L"", szPos, MAX_PATH, g_szFilePath);
				 if (szPos[0] == '\0') {
					 break;
				 }
				 swscanf(szPos, L"%f,%f,%f", &pos.x, &pos.y, &pos.z);
				 
				 LOGD << "randomRes_" << st.Map << ','<<pos.x <<','<< pos.z;
				 st.vecPos.push_back(pos);
			 }

			 if (st.vecPos.size()) {
				 g_vecRandomRes.push_back(st);
			 }
		}

		//resProtect
		g_isResProtect = GetPrivateProfileInt(L"resBuff", L"enable", 0, g_szFilePath);
		if (g_isResProtect) {
			InitializeCriticalSection(&g_cs);
			DWORD skillid = GetPrivateProfileInt(L"resBuff", L"skillid", 0, g_szFilePath);
			DWORD skilllevel = GetPrivateProfileInt(L"resBuff", L"skilllevel", 0, g_szFilePath);
			g_skillObj=getSKillObject(skillid, skilllevel);
			_beginthread(ResProtect, 0, 0);
			LOGD << "skillobject:" << g_skillObj;
		}


	}
}





