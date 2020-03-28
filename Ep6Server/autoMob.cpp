#include "stdafx.h"
#include "autoMob.h"






namespace AutoMob
{
	DWORD g_dwExp[81] = { 0 };
	WORD g_wMap = 0;
	DWORD g_sleepTime = 0;

	vector<MOBTASK> g_vecMobTasks;


	void loop()
	{
		SYSTEMTIME stTime;
		GetLocalTime(&stTime);

		for (auto iter = g_vecMobTasks.begin(); iter != g_vecMobTasks.end(); iter++) {
			for (auto it = iter->vecTasks.begin(); it != iter->vecTasks.end(); it++) {
				if (it->dayOfWeek == stTime.wDay&&
					it->hour == stTime.wHour&&
					it->minute == stTime.wMinute&&
					it->LastDay != stTime.wDay) {
						{
							DWORD index = GetRand(iter->vecPos.size());
							shaiya_pos pos = iter->vecPos[index];
							LOGD << iter->strNotice;
							createMob(iter->Map, iter->id, pos.x, pos.z);
							sendNotice(iter->strNotice);
							it->LastDay = stTime.wDay;  // 修改一下标志位，防止重复刷新
						}

				}
			}
		}
	}



	void Start()
	{
#ifndef _DEBUG
		VMProtectBegin("autmob start");
#endif 
		for (DWORD i = 0; i < 2000; i++) {
			MOBTASK st;
			ZeroMemory(&st, sizeof(st));
			char strTime[MAX_PATH] = { 0 };
			char appName[50] = { 0 };
			sprintf(appName, "automob_%d", i + 1);
			GetPrivateProfileStringA(appName, "time", "", strTime, MAX_PATH, g_strFilePath);
			GetPrivateProfileStringA(appName, "notice", "", st.strNotice, MAX_PATH, g_strFilePath);
			st.Map = GetPrivateProfileIntA(appName, "map", 0, g_strFilePath);
			st.id = GetPrivateProfileIntA(appName, "id", 0, g_strFilePath);


			if (strTime[0] == '\0') break;


			//读时间
			char *delim = ")";
			char* p = strtok(strTime, delim);
			while (p) {
				SHAIYATASK task = { 0 };
				sscanf(p, "(%hd,%hd,%hd", &task.dayOfWeek, &task.hour, &task.minute);
				LOGD << "autmob-->map" << st.Map << ",mobid:" << st.id << "dayofWeek:" << task.dayOfWeek << ",hour:" << task.hour << ",minute:" << task.minute;
				st.vecTasks.push_back(task);
				p = strtok(NULL, delim);
			}


			//读坐标
			for (DWORD j = 0; j < 5; j++) {
				shaiya_pos pos;
				char strFloat[MAX_PATH] = { 0 };
				char key[50] = { 0 };
				sprintf(key, "pos%d", j + 1);
				GetPrivateProfileStringA(appName, key, "", strFloat, MAX_PATH, g_strFilePath);
				if (strFloat[0] == '\0') break;
				sscanf(strFloat, "%f,%f", &pos.x, &pos.z);
				st.vecPos.push_back(pos);
			}

			g_vecMobTasks.push_back(st);
		}

#ifndef _DEBUG
		VMProtectEnd();
#endif
		g_vecLoopFunction.push_back(loop);
	}
}