#include "stdafx.h"
#include "namecolor.h"



namespace NameColor
{



	void Loop(void* p)
	{
#ifndef _DEBUG
		VMProtectBegin("NameColor loop");
#endif 
		while (1)
		{
			PLAYERCOLOR playerColor;
			ZeroMemory(playerColor.color, sizeof(playerColor.color));
			DWORD i = 0;
			for (; i < 200; i++)
			{
				WCHAR szKey[MAX_PATH] = { 0 };
				WCHAR szRet[MAX_PATH] = { 0 };
				swprintf_s(szKey, MAX_PATH, L"%d", i + 1);
				GetPrivateProfileString(L"namecolor", szKey, L"", szRet, MAX_PATH, g_szFilePath);
				if (szRet[0] != L'\0') {
					swscanf_s(szRet, L"%d,%d", &(playerColor.color[i].dwCharid), &(playerColor.color[i].dwRGB));
					playerColor.color[i].dwRGB |= 0xff000000;
				}
			}

			if (i)
			{
				send_packet_all(PBYTE(&playerColor), sizeof(playerColor));
			}
			Sleep(30 * 1000);
		}
#ifndef _DEBUG
		VMProtectEnd();
#endif
	}

	void Start()
	{
		_beginthread(Loop, 0, 0);
	}
}




