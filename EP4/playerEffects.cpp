#include "stdafx.h"
#include "playerEffects.h"




namespace playerEffects
{




	//---------------------配置文件形式的

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

		PLAYEREFFECTS st;
		st.count = 0;
		DWORD i = 0;
		for (; i < _countof(st.effects); i++)
		{
			char key[50] = { 0 };
			char strTemp[MAX_PATH] = { 0 };
			sprintf(key, "%d", i + 1);
			GetPrivateProfileStringA("playerEffects", key, "", strTemp, MAX_PATH, g_strFilePath);

			if (strTemp[0] == '\0')
				break;

			sscanf(strTemp, "%d,%d", &st.effects[i].charid, &st.effects[i].effectIndex);
		}

		if (i) {
			st.count = i;
			send_packet_all(&st, sizeof(st));
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




