#include "stdafx.h"
#include "bothMobRespawn.h"

namespace bothMobRespawn {

	CMyInlineHook g_objInitreSpawn, g_objMarkTime;
	map<DWORD, DWORD> g_mapBothMobRespawnTime;
	vector<PMapBoth> g_vecMarktime;


	//��־ֱ�Ӱ����Ǹ��´�ˢ��ʱ���gettickcount,�����ؿ�������һ�����������ӳٲ����������30����

	/************************/
	/*ÿ��BB�����Ժ��¼ʱ��*/
	/**********************/
	void __stdcall fun_markTime(PMapBoth pMapBoth) {

		//Ӧ��Ҫд����־��

// 		auto it = find(g_vecMarktime.begin(), g_vecMarktime.end(), pMapBoth);
// 		if (it == g_vecMarktime.end()) {
// 			g_vecMarktime.push_back(pMapBoth);
// 		}

	}

	

	/********************************/
	/*��ʼʱ����Ҫ�����BB�ʹ����ʱ��*/
	/*******************************/
	bool __stdcall fun_IsInitRespawn(PEp4Zone pZone){
		auto it = g_mapBothMobRespawnTime.find(pZone->ListBoth->dwID);
		if (it != g_mapBothMobRespawnTime.end()) {
			pZone->ListBoth->ListBoth.pRespawn.dwRespawnTime= it->second;
			pZone->ListBoth->ListBoth.pRespawn.nCurrCount -= 1;
			g_mapBothMobRespawnTime.erase(it);
			return false;
		}
		return true;
	}



	DWORD falseAddr = 0x0041ba95;
	__declspec(naked) void  Naked_markTime()
	{
		_asm
		{
			pushad
			MYASMCALL_1(fun_markTime,edi)
			popad
			cmp dword ptr [eax+0xc0],ecx
			 jmp g_objMarkTime.m_pRet
		}
	}



	DWORD call_1 = 0x00416860;
	__declspec(naked) void  Naked_InitRespawn()
	{
		_asm
		{
			pushad
			MYASMCALL_1(fun_IsInitRespawn, edi)
			test al,al
			popad
			jne _Org

			add esp,0x4
			jmp g_objInitreSpawn.m_pRet

			_Org:
			call call_1
			cmp dword ptr[eax + 0xc0], ecx
			jmp g_objInitreSpawn.m_pRet
		}
	}


	void Start()
	{
		g_objMarkTime.Hook((PVOID)0x00421cf3, Naked_markTime, 6);
		g_objInitreSpawn.Hook(PVOID(0x0041b5e2), Naked_InitRespawn, 5);

		
	}
}

