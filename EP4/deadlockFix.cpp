#include "stdafx.h"
#include "deadlockFix.h"




namespace deadlockFix
{
	CMyInlineHook g_obj;
	
	HANDLE g_hMap;
	bool* pIsReconnect;
	WCHAR g_strMapName[] = L"Global\\shaiya_isdbreconnect";



	void __stdcall fun() {
		*pIsReconnect = true;
		LOGD << "deadlockFix-->set pIsReconnect to true!";
	}


	
	__declspec(naked) void  Naked()
	{
		_asm
		{
			pushad
			MYASMCALL(fun)
			popad
			jmp g_obj.m_pRet
		}
	}



	void Start()
	{
		g_hMap = ::CreateFileMapping(INVALID_HANDLE_VALUE,
			NULL,
			PAGE_READWRITE,
			0,
			sizeof(bool),
			g_strMapName
			);


		pIsReconnect = (bool*)MapViewOfFile(g_hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		g_obj.Hook(PVOID(0x0040534d),Naked,7);

		BYTE temp[] = { 0x90,0x90,0x90,0x90,0x90 };
		MyWriteProcessMemory(GetCurrentProcess(), (void*)0x0040535b, temp, sizeof(temp));
	}
}




