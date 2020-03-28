#include "stdafx.h"
#include "DropsRange.h"




namespace DropsRange
{

	DropGradeTable g_DropsTable[MAX_GRADE + 1];
	CMyInlineHook g_objCs1, g_objCs2, g_objCs3;

	//DWORD g_oldCs = 0x109A884;
	DWORD g_oldobj = 0x1091878;


	__declspec(naked) void Naked_cs1()
	{
		_asm {
			pushad
			lea eax, g_DropsTable
			cmp edi, eax
			popad
			jne _Org
			
			mov edi, g_oldobj

			_Org:
			add edi,0x900c
				jmp g_objCs1.m_pRet
		}
	}

	__declspec(naked) void Naked_cs2()
	{
		_asm {
			pushad
			lea eax, g_DropsTable
			cmp ecx, eax
			popad
			jne _Org

			mov ecx, g_oldobj

			_Org :
			add ecx, 0x900c
				jmp g_objCs2.m_pRet
		}
	}


	__declspec(naked) void Naked_cs3()
	{
		_asm {
			pushad
			lea eax, g_DropsTable
			cmp ecx, eax
			popad
			jne _Org


			mov ecx, g_oldobj
			_Org :
			add ecx, 0x900c
				jmp g_objCs3.m_pRet
		}
	}

	void start()
	{
		////////////////////////////////////----------------------------------
		{
			DWORD addr[] = { /*0x00473ab2,*/0x00473b73,/*0x00473b9e,*/
				0x00473bac,0x004babdc,0x004d5ccf,0x004d5cf3,0x004d5d3d,
				0x004d5d88,0x004d5e64,0x004d5fde,0x004d6025,0x004d6050,
				0x004d6081,0x004d60b2,0x004d60e3,0x004d6108,0x004d6184,
				0x004d6192,0x004d61e2,0x004d61f0,0x004d6249,0x004d6257,
				0x004d62b3,0x004d62c1,0x004d631d,0x004D632B,0x004D6361,
				0x004D63C4,0x004D63D2,0x004D6422,0x004D6430,0x004D6489,
				0x004D6497,0x004D64F3,0x004D6501,0x004D655D,0x004D656B,
				0x004D66E3,0x004D6D9C,0x004D7072
			};
			DWORD value = DWORD(g_DropsTable);
			for (DWORD i = 0; i < _countof(addr); i++) {
				MyWriteProcessMemory(GetCurrentProcess(), PVOID(addr[i]), &value, sizeof(value));
			}
		}
		////////////////////////////////////----------------------------------
		{
			DWORD addr[] = { 0x004D618B,
				0x004D61E9,
				0x004D6250,
				0x004D62BA,
				0x004D6324,
				0x004D63CB,
				0x004D6429,
				0x004D6490,
				0x004D64FA,
				0x004D6564};
			DWORD value = DWORD(g_DropsTable)+8;
			for (DWORD i = 0; i < _countof(addr); i++) {
				MyWriteProcessMemory(GetCurrentProcess(), PVOID(addr[i]), &value, sizeof(value));
			}		
		}
		////////////////////////////////////----------------------------------
		{
			DWORD addr[] = { 0x004d5e91,0x004d5f35,0x004d6004,0x004d6155,0x004d61ac,0x004d620a,
				0x004d6274,0x004d62de,0x004d6395,0x004d63ec,0x004d644a,0x004d64b4,0x004d651e,
				0x004d6d84,0x004d704c,0x004d7528
			};
			DWORD value = MAX_GRADE;
			for (DWORD i = 0; i < _countof(addr); i++) {
				MyWriteProcessMemory(GetCurrentProcess(), PVOID(addr[i]), &value, sizeof(value));
			}
		}
		////////////////////////////////////----------------------------------
		{
			DWORD addr[] = { 0x004d5e5d,
			};
			DWORD value = MAX_GRADE+1;
			for (DWORD i = 0; i < _countof(addr); i++) {
				MyWriteProcessMemory(GetCurrentProcess(), PVOID(addr[i]), &value, sizeof(value));
			}
		}

		////////////////////////////////////----------------------------------
		{
			DWORD addr[] = { 0x004d5cec,0x004d5fd6};
			DWORD value = sizeof(g_DropsTable);
			for (DWORD i = 0; i < _countof(addr); i++) {
				MyWriteProcessMemory(GetCurrentProcess(), PVOID(addr[i]), &value, sizeof(value));
			}
		}

		////////////////////////////////////----------------------------------
		{
			DWORD addr[] = { 0x004d613e,0x004d636e};
			DWORD value = DWORD(g_DropsTable)+ sizeof(g_DropsTable);
			for (DWORD i = 0; i < _countof(addr); i++) {
				MyWriteProcessMemory(GetCurrentProcess(), PVOID(addr[i]), &value, sizeof(value));
			}
		}


		//------------------------------------------------------------
		g_objCs1.Hook(PVOID(0x004d5ec5), Naked_cs1, 6);
		g_objCs2.Hook(PVOID(0x0054df49), Naked_cs2, 6);
		g_objCs3.Hook(PVOID(0x0054dfa3), Naked_cs3, 6);
	}
}





