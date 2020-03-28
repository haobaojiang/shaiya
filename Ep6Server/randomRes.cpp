#include "stdafx.h"

#include "randomRes.h"



namespace randomRes
{
	CMyInlineHook g_obj;
	WORD  g_Map = 0;
	vector<shaiya_pos> g_vecPos;

	void __stdcall fun(Pshaiya_player player,WORD map,Pshaiya_pos pPos)
	{
		if (map == g_Map)
		{
			DWORD index= GetRand(g_vecPos.size());
			memcpy(pPos, &g_vecPos[index], sizeof(shaiya_pos));
		}
	}


	__declspec(naked) void  Naked()
	{
		_asm {
			push eax
			lea ecx,dword ptr [esp+0x24]
			pushad
			MYASMCALL_3(fun,edi,eax,ecx)
			popad
			jmp g_obj.m_pRet
		}
	}
	

	void Start()
	{
		g_obj.Hook(PVOID(0x00466931), Naked, 5);


		g_Map = GetPrivateProfileInt(L"randomRes", L"map", 0, g_szFilePath);

		for (DWORD i=0;i<100;i++){
			WCHAR key[50] = { 0 };
			WCHAR szPos[MAX_PATH] = { 0 };
			shaiya_pos pos;

			swprintf_s(key, L"%ld", i+1);
			GetPrivateProfileString(L"randomRes", key, L"", szPos, MAX_PATH, g_szFilePath);
			if (szPos[0] == '\0'){
				break;
			}
			swscanf(szPos, L"%f,%f,%f", &pos.x, &pos.y, &pos.z);
			g_vecPos.push_back(pos);	
		}

	}
}





