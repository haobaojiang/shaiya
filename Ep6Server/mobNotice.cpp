#include "stdafx.h"
#include "mobNotice.h"






namespace mobNotice
{


	vector<DWORD> g_vecMobs;
	CMyInlineHook g_obj;
	char notice_fmt1[MAX_PATH] = { 0 };
	char notice_fmt2[MAX_PATH] = { 0 };


	bool isBoss(DWORD mobid) {
		for (auto iter = g_vecMobs.begin(); iter != g_vecMobs.end(); iter++) {
			if (*iter == mobid) {
				return true;
			}
		}
		return false;
	}


	const char* getmapNameByid(WORD mapId) {
		return g_strMapName[mapId];
	}


	int getMapidByMapobj(DWORD mapObj){
		for (int i = 0; i < _countof(g_zones); i++) {
			if (g_zones[i] == mapObj) {
				return i;
			}
		}
		return -1;
	}


	void __stdcall fun_obelisk(DWORD obe,DWORD mapObj) {
		DWORD mobid = *PDWORD(obe + 0x34);
		if (!isBoss(mobid)) {
			return;
		}
		Pshaiya_pos pos = Pshaiya_pos(obe + 0x1c);
		DWORD mobObj = *PDWORD(obe + 0x74);
		const char* mobName = PCHAR(mobObj + 0x2);
		char strNotice[MAX_PATH] = { 0 };

		int nMapid = getMapidByMapobj(mapObj);
		if (nMapid == -1) {
			sprintf(strNotice, notice_fmt1, mobName, DWORD(pos->x), DWORD(pos->z));
		}
		else {
			const char* pstrMapName = getmapNameByid(nMapid);
			sprintf(strNotice, notice_fmt2, mobName, pstrMapName, DWORD(pos->x), DWORD(pos->z));
		}

		sendNotice(strNotice);
	}


	DWORD dwOrgCall1 = 0x00422de0;
	__declspec(naked) void  Naked_obelisk()
	{
		_asm {
			pushad
			MYASMCALL_2(fun_obelisk,eax,ebx)
			popad
			call dwOrgCall1
			jmp g_obj.m_pRet
		}
	}



	void Start()
	{
		//∂¡≈‰÷√
		char strMobs[MAX_PATH] = { 0 };
		GetPrivateProfileStringA("mobNotice", "mobs", "", strMobs, MAX_PATH, g_strFilePath);
		GetPrivateProfileStringA("mobNotice", "fmt1", "", notice_fmt1, MAX_PATH, g_strFilePath);
		GetPrivateProfileStringA("mobNotice", "fmt2", "", notice_fmt2, MAX_PATH, g_strFilePath);

		if (strMobs[0] == '\0') return;

		char *delim = ",";
		char* p = strtok(strMobs, delim);
		while (p) {

			DWORD mobid;
			sscanf(p, "%d", &mobid);

			if (!mobid)
				break;

			LOGD << "mobNotice config reading, mobid:" << mobid ;
			g_vecMobs.push_back(mobid);

			p = strtok(NULL, delim);
		}

		//hook
		g_obj.Hook(PVOID(0x004228ec), Naked_obelisk, 5);
	}
}