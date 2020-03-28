#include "stdafx.h"
#include "dropNotice.h"

namespace dropNotice {

	CMyInlineHook g_obj;
	char g_strFormat[MAX_PATH] = { 0 };

	vector<DWORD> g_vecItems;
	vector<WORD> g_vecGrades;

	


	


	void  __stdcall fun(Pshaiya50_player player, PplayerItemcharacterise pItem) {

		//id
		{
			DWORD Itemid = pItem->ItemAddr->itemid;
			auto iter = find(g_vecItems.begin(), g_vecItems.end(), Itemid);
			if (iter != g_vecItems.end()) {
				char strNotice[MAX_PATH] = { 0 };
				const char* pstrMapName = g_MapName[player->Map];
				sprintf(strNotice, g_strFormat, player->charname, pstrMapName, pItem->ItemAddr->itemname);
				sendNotice(strNotice);
				return;
			}
		}

		//grade
		{
			WORD grade = pItem->ItemAddr->Grade;
			auto iter = find(g_vecGrades.begin(), g_vecGrades.end(), grade);
			if (iter != g_vecGrades.end()) {
				char strNotice[MAX_PATH] = { 0 };
				const char* pstrMapName = g_MapName[player->Map];
				sprintf(strNotice, g_strFormat, player->charname, pstrMapName, pItem->ItemAddr->itemname);
				sendNotice(strNotice);
				return;
			}
		}
	}


	__declspec(naked) void  Naked()
	{
		_asm
		{
			mov ecx,0x00546f4c
			mov ecx,dword ptr [ecx]
			pushad
			MYASMCALL_2(fun,edi,ebp)
			popad
			jmp g_obj.m_pRet
		}
	}


	void Start()
	{
		g_obj.Hook((PVOID)0x0045e713, Naked, 6);
		GetPrivateProfileStringA("dropNotice", "fmt", "",g_strFormat, MAX_PATH,g_strFilePath);


		char strItems[1024] = { 0 };
		char strGrades[1024] = { 0 };
		GetPrivateProfileStringA("dropNotice", "items","",strItems,1024,g_strFilePath);
		GetPrivateProfileStringA("dropNotice", "grades", "", strGrades, 1024, g_strFilePath);

		{
			char *delim = ",";
			char* p = strtok(strItems, delim);
			while (p) {

				DWORD itemid = 0;
				sscanf(p, "%ld", &itemid);
				if (itemid) {
					LOGD << "dropNotice itemid added:" << itemid;
					g_vecItems.push_back(itemid);
				}
				p = strtok(NULL, delim);
			}
		}



		{
			char *delim = ",";
			char* p = strtok(strGrades, delim);
			while (p) {

				WORD grade = 0;
				sscanf(p, "%hd", &grade);
				if (grade) {
					LOGD << "dropNotice grade added:" << grade;
					g_vecGrades.push_back(grade);
				}
				p = strtok(NULL, delim);
			}
		}



	}
}

