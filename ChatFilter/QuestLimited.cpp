#include "stdafx.h"
#include "QuestLimited.h"

namespace QuestLimited {

	CMyInlineHook g_obj;


	DWORD g_questId = 0;


	bool g_isOpenTime[24][60] = { false };


	


	bool  __stdcall fun(DWORD questId) {



		if (g_questId == questId) {
			SYSTEMTIME st;
			GetLocalTime(&st);
			return g_isOpenTime[st.wHour][st.wMinute];
		}
		return true;
	}


	__declspec(naked) void  Naked()
	{
		_asm
		{
			pushad
			mov eax,dword ptr [esp+0x24]
			MYASMCALL_1(fun,eax)
			test al,al
			popad
			jne _Org
			xor eax,eax
			retn 0x4
	
			_Org:
			push ebx
			push ebp
				mov ebp,dword ptr [esp+0xc]
			jmp g_obj.m_pRet
		}
	}






	void Start()
	{
		g_obj.Hook((PVOID)0x0047e4b0, Naked, 6);

		char* appName = "Questlimited";
		char strBeginTime[MAX_PATH] = { 0 };
		WORD g_beginHour = 0;
		WORD g_beginMinute = 0;

		g_questId = GetPrivateProfileIntA(appName, "questid", 0, g_strFilePath);
		GetPrivateProfileStringA(appName, "begintime", "", strBeginTime, MAX_PATH, g_strFilePath);
		DWORD duration = GetPrivateProfileIntA(appName, "duration",0, g_strFilePath);


		if (strBeginTime[0] == '\0' || !duration) {
			LOGD << "QuestLimited-->strBeginTime or strEndTime no corrected";
			return;
		}

		sscanf(strBeginTime, "%hd:%hd", &g_beginHour, &g_beginMinute);



		LOGD << "QuestLimited--id:"<<g_questId;
		LOGD << "strBeginTime--hour:" << g_beginHour <<",minute:"<<g_beginMinute;
		LOGD << "duration--minutes:" << duration;

		memset(&g_isOpenTime[g_beginHour][g_beginMinute], true, duration);

	}
}

