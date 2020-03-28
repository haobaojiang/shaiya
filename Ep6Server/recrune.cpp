#include "stdafx.h"
#include "recrune.h"



namespace RecRune
{



	CRITICAL_SECTION g_cs;
	bool g_IsEnable = false;
	CMyInlineHook g_objIsEnable;
	CMyInlineHook g_objSetSignature;
	CMyInlineHook g_objSetStats;
	WORD g_craftname[10] = { 0 };
	WORD g_wRange = 0;

	
	void __stdcall fun_setSignature(PBYTE pSignature)
	{
		if (!g_IsEnable)
			return;

		pSignature[g_wRange] = 0xff;
	}

	void __stdcall fun_IsEnable(PITEMINFO recRune, PITEMINFO gear)
	{
		EnterCriticalSection(&g_cs);
		ZeroMemory(g_craftname, sizeof(g_craftname));


		switch (recRune->itemid)
		{
		case 101001:
			g_wRange = 1;
			break;
		case 101002:
			g_wRange = 2;
			break;
		case 101003:
			g_wRange = 4;
			break;
		case 101004:
			g_wRange = 5;
			break;
		case 101005:
			g_wRange = 3;
			break;
		case 101006:
			g_wRange = 6;
			break;
		default:
			return;
			break;
		}

		g_wRange -= 1;   //index
		g_IsEnable = true;

	}
	void __stdcall fun_setStats(DWORD gear)
	{

		do 
		{
			if (!g_IsEnable)
				break;

			g_IsEnable = false;

			PWORD pstats = PWORD(gear + 0x4c);
			DWORD gearAddr = *PDWORD(gear + 0x30);

			//属性双倍
			char strTemp[3] = { 0 };
			memcpy(strTemp, &pstats[g_wRange], 2);
	
		
			WORD oldStats = atoi(strTemp);
			DWORD NewStats = oldStats * 2;
			DWORD reqwis = *PWORD(gearAddr + 0x3e);
			reqwis *= 2;

			if (!NewStats)
				NewStats = 2;

			if (reqwis > 99)
				reqwis = 99;

			if (NewStats > reqwis)
				NewStats = reqwis;

			sprintf(strTemp, "%02d", NewStats);
			memcpy(&pstats[g_wRange], strTemp, 2);

			BYTE count = 0;
			//计算个数
			for (DWORD i = 0; i < 9; i++)
			{
				if (pstats[i] > 0x3030)
				{
					count++;
				}
			}

			BYTE byServer = *PBYTE(gearAddr + 0x4a);
			if (count <= byServer)
				break;


			while (1)
			{
				DWORD Index = GetRand(byServer, 0);
				for (DWORD i = 0,id=0; i < 9; i++)
				{		
					if (pstats[i] > 0x3030)
					{

						//不可以是我们原来的属性
						if (i == g_wRange) {
							id++;
							continue;
						}

						//如果刚好匹配
						if (id == Index) {
							pstats[i] = 0x3030;
							goto __end;
						}

						//统次有属性的
						id++;
					}
				}
			}



		} while (0);

		__end:
		LeaveCriticalSection(&g_cs);
	}


	

__declspec(naked) void Naked_SetSignature()
	{
		_asm
		{
			xor edi,edi
			lea ebp, dword ptr ds : [esi + 0x84]
			cmp dword ptr [esp+0x28],0x0046d727
			je _fun
			cmp dword ptr[esp +0x28], 0x0046d77e
			je _fun
			jmp _org

			_fun:
			pushad
			lea eax,dword ptr [esp+0x2c]
			push eax
			call g_objSetSignature.m_pfun_call
			popad

			_org:
			jmp g_objSetSignature.m_pRet

		}
	}
__declspec(naked) void Naked_setStats()
{
	_asm
	{
		cmp dword ptr[esp + 0x28], 0x0046d727
		je _fun
		cmp dword ptr[esp + 0x28], 0x0046d77e
		je _fun
		jmp _org

		_fun:
		pushad
		push esi
		call g_objSetStats.m_pfun_call
		popad

		_org:

		mov ecx, dword ptr ss : [esp + 0x24]
		pop edi
		jmp g_objSetStats.m_pRet
	}
}
	__declspec(naked) void Naked_IsEnable()
	{
		_asm
		{
			lea edx, dword ptr ss : [esp + 0x130]
			pushad
			push ebp  //gear
			push ecx  //rec rune
			call g_objIsEnable.m_pfun_call
			popad
			jmp g_objIsEnable.m_pRet
		}
	}
	


	void start()
	{
		InitializeCriticalSection(&g_cs);
		g_objSetSignature.Hook(PVOID(g_hookAddr.recRune1), Naked_SetSignature, 8,fun_setSignature);
		g_objIsEnable.Hook(PVOID(g_hookAddr.recRune2), Naked_IsEnable, 7, fun_IsEnable);
		g_objSetStats.Hook(PVOID(g_hookAddr.recRune3), Naked_setStats, 5, fun_setStats);

	}
}





