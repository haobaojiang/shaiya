#include "stdafx.h"
#include "dailiMap.h"

namespace dailiMap {


	WORD g_map = 0;
	BYTE g_days = 0;
	std::map<DWORD, DWORD> g_mapPlayerDailyInfo;


	DWORD getNextGateTime(DWORD CurTime) {
		SYSTEMTIME lt;
		GetLocalTime(&lt);
		CurTime += (g_days * 24 * 60 * 60 * 1000);
		CurTime -= (lt.wHour * 60 * 60 * 1000);
		CurTime -= (lt.wMinute * 60 * 1000);
		CurTime -= (lt.wSecond * 1000);
		return CurTime;
	}


	void  __stdcall fun(Pshaiya50_player player, WORD Map, Pshaiya_pos pos, bool* pRet) {

	/*	if (Map != g_map&&g_map) {*/

		//���ǵĻ���ר�õ�
		if(Map==48||Map==49){
			auto iter = g_mapPlayerDailyInfo.find(player->uid);

			DWORD CurTime = GetTickCount();

			//���û�������ͼ
			if (iter == g_mapPlayerDailyInfo.end()) {
				g_mapPlayerDailyInfo.insert(std::pair<DWORD, DWORD>(player->uid, getNextGateTime(CurTime)));
				return;
			}else
			{
				//���˵����ͬһ�죬���ǽ�ȥ��
				if (iter->second>=CurTime) {

					char strNotice[MAX_PATH] = { 0 };
					sprintf(strNotice, "�㻹��Ҫ%ld���Ӳ��ܽ�ȥ!", (iter->second - CurTime) / 1000 / 60);
					sendNoticeToplayer(player, strNotice);
					*pRet = false;
				}
				//��������,���Ǽ�¼�����Ѿ�������
				else {
					iter->second = getNextGateTime(CurTime);
				}
			}
		}
	}



	void Start()
	{
		g_fireportCallBack.push_back(fun);
		g_map = GetPrivateProfileIntA("dailiMap", "map", 0, g_strFilePath);
		g_days= GetPrivateProfileIntA("dailiMap", "days", 0, g_strFilePath);
	}
}

