#include "stdafx.h"
#include "recrune.h"





namespace RecRune
{
	CRITICAL_SECTION g_cs;
	bool g_IsEnable = false;
	ShaiyaUtility::CMyInlineHook g_objIsEnable;
	ShaiyaUtility::CMyInlineHook g_objSetSignature;
	ShaiyaUtility::CMyInlineHook g_objSetSignature1;
	ShaiyaUtility::CMyInlineHook g_objSetSignature2;
	ShaiyaUtility::CMyInlineHook g_objSetStats;
	WORD g_oldStats[9] = { 0 };

	bool IsPlusHp = false;

	std::array<bool,255> supported_types;


	
	void __stdcall fun_setSignature1(DWORD pGear)
	{
		if (!g_IsEnable)
			return;

		memcpy(g_oldStats, PVOID(pGear + 0x4c), sizeof(WORD) * 9);
		IsPlusHp = false;
	}


	void __stdcall fun_setSignature(DWORD pGear)
	{
		auto item_type = *PBYTE(pGear + 0x40);
		if(!supported_types.at(item_type))
		{
			return;
		}
			

		memcpy(g_oldStats, PVOID(pGear + 0x4c), sizeof(WORD) * 9);
		IsPlusHp = true;
	}



	void __stdcall fun_IsEnable(DWORD recRune, DWORD gear)
	{
		EnterCriticalSection(&g_cs);
		ZeroMemory(g_oldStats, sizeof(g_oldStats));

		if (*PDWORD(recRune) == 100171)
			return;

		g_IsEnable = true;

	}



	void __stdcall fun_setStats(DWORD gear)
	{
		if (g_IsEnable){
			g_IsEnable = false;

			PWORD pstats = PWORD(gear + 0x4c);
			DWORD gearAddr = *PDWORD(gear + 0x30);
			WORD reqwis = *PWORD(gearAddr + 0x3e);


			for (DWORD i = 0; i < 9; i++) {
				char strTemp[3] = { 0 };
				memcpy(strTemp, &g_oldStats[i], 2);//提取之前保存的旧属性

				WORD NewStats = atoi(strTemp);     //转成数字
				NewStats += 3;
				if (NewStats > reqwis)
					NewStats = reqwis;
				sprintf(strTemp, "%02hd", NewStats); //再转回字符
				memcpy(&g_oldStats[i], strTemp, 2); //写回去
			}

			//写到物品
			if (IsPlusHp) {
				memcpy(pstats, g_oldStats, sizeof(WORD) * 9);
			}
			else {
				memcpy(pstats, g_oldStats, sizeof(WORD) * 6);
			}
		}
		LeaveCriticalSection(&g_cs);
	}

	
	
	__declspec(naked) void Naked_SetSignature2()
	{
		_asm {
			mov byte ptr [esp+edx+0xc],0x0
			xor edi, edi
			lea ebp, dword ptr ds : [esi + 0x84]

			cmp dword ptr[esp + 0x28], 0x0046068b
			je _fun
			cmp dword ptr[esp + 0x28], 0x004606e3
			je _fun
			jmp _Org

			_fun:
			pushad
				MYASMCALL_1(fun_setSignature1, esi)
				popad

				_Org:
			jmp g_objSetSignature2.m_pRet
		}
	}




	__declspec(naked) void Naked_SetSignature1()
	{
		_asm
		{
			mov byte ptr ss : [esp + 0x14], 0x0
			xor edi, edi
			lea ebp, dword ptr ds : [esi + 0x84]

			cmp dword ptr[esp + 0x28], 0x0046068b
			je _fun
			cmp dword ptr[esp + 0x28], 0x004606e3
			je _fun
			jmp _Org

			_fun :
			pushad
				MYASMCALL_1(fun_setSignature1, esi)
				popad

				_Org :
			jmp g_objSetSignature1.m_pRet

		}
	}


	__declspec(naked) void Naked_SetSignature()
	{
		_asm
		{
			xor edi, edi
			lea ebp, dword ptr ds : [esi + 0x84]

			_fun :

			pushad
			MYASMCALL_1(fun_setSignature, esi)
			popad

			_Org :
			jmp g_objSetSignature.m_pRet

		}
	}
	__declspec(naked) void Naked_setStats()
	{
		_asm
		{
			cmp dword ptr[esp + 0x28], 0x0046068b
			je _fun
			cmp dword ptr[esp + 0x28], 0x004606e3
			je _fun
			jmp _Org

			_fun :
			pushad
				push esi
				call g_objSetStats.m_pfun_call
				popad

				_Org :
			pop edi
				xor eax, eax
				cmp dword ptr ss : [esp + 0x28], eax
				jmp g_objSetStats.m_pRet
		}
	}
	__declspec(naked) void Naked_IsEnable()
	{
		_asm
		{
			lea edx, dword ptr ss : [esp + 0x11C]
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
		g_objSetSignature.Hook(PVOID(0x004d2bc5), Naked_SetSignature, 8, fun_setSignature);
	
		g_objSetSignature1.Hook(PVOID(0x004baca1), Naked_SetSignature1, 5);
		g_objSetSignature1.m_pRet =(PVOID) 0x004bacae;

		
		g_objSetSignature2.Hook(PVOID(0x004bac76), Naked_SetSignature2, 5);
		g_objSetSignature2.m_pRet = (PVOID)0x004bacae;

		g_objIsEnable.Hook(PVOID(0x00460620), Naked_IsEnable, 7, fun_IsEnable);
		g_objSetStats.Hook(PVOID(0x004bad63), Naked_setStats, 7, fun_setStats);

		{
			BYTE temp[] = { 0xeb,0x53 };
			MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004bac4c, temp, sizeof(temp));
		}

	}
}

/*


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

	void __stdcall fun_IsEnable(DWORD recRune,DWORD gear)
	{
		EnterCriticalSection(&g_cs);
		ZeroMemory(g_craftname, sizeof(g_craftname));

		g_wRange = *PWORD(recRune + 0x42);//range
		if (!g_wRange)
			return;

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
			pushad
			lea eax,[esp+0x2c]
			push eax
			call g_objSetSignature.m_pfun_call
			popad
			jmp g_objSetSignature.m_pRet

		}
	}
__declspec(naked) void Naked_setStats()
{
	_asm
	{
		pushad
		push esi
		call g_objSetStats.m_pfun_call
		popad
		pop edi
		xor eax,eax
		cmp dword ptr ss : [esp + 0x28], eax
		jmp g_objSetStats.m_pRet
	}
}
	__declspec(naked) void Naked_IsEnable()
	{
		_asm
		{
			lea edx, dword ptr ss : [esp + 0x11C]
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
		g_objSetSignature.Hook(PVOID(0x004baca6), Naked_SetSignature, 8,fun_setSignature);
		g_objIsEnable.Hook(PVOID(0x00460620), Naked_IsEnable, 7, fun_IsEnable);
		g_objSetStats.Hook(PVOID(0x004bad63), Naked_setStats, 7, fun_setStats);

	}
}





*/