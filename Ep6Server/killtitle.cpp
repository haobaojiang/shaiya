#include "stdafx.h"
#include "killtitle.h"




namespace killtitle
{

	PLAYERTITLES g_st;





	bool isTime() {

		DWORD  static nextTime = GetTickCount() + 15 * 1000;
		DWORD curTime = GetTickCount();
		if (nextTime > curTime) {
			return false;
		}
		else {
			nextTime = curTime+ 15 * 1000;
			return true;
		}
	}



	void loadConfig() {

		//兼容配置文件读取模式的
		ZeroMemory(&g_st, sizeof(g_st));
		g_st.cmd = PacketOrder::additional_title;
		g_st.x = GetPrivateProfileIntA("killtitle", "x", 0, g_strFilePath);
		g_st.y = GetPrivateProfileIntA("killtitle", "y", 0, g_strFilePath);
		g_st.imageCount = GetPrivateProfileIntA("killtitle", "imagecount", 0, g_strFilePath);

		DWORD i = 0;
		for (; i < _countof(g_st.playerImages); i++)
		{
			char key[50] = { 0 };
			char strTemp[MAX_PATH] = { 0 };
			sprintf(key, "%d", i + 1);
			GetPrivateProfileStringA("killtitle", key, "", strTemp, MAX_PATH, g_strFilePath);

			if (strTemp[0] == '\0')
				break;

			sscanf(strTemp, "%d,%d", &g_st.playerImages[i].charid, &g_st.playerImages[i].Inedx);
		}

		if (i) {
			send_packet_all(PBYTE(&g_st), sizeof(g_st));
		}

	}



	void Loop()
	{

		if (isTime()) 
		{	
			loadConfig();
		}
	}





	void Start()
	{
		g_vecLoopFunction.push_back(Loop);
	}
}




