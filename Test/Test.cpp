
#include "MemoryModule.h"
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <MyClass.h>
#include <windows.h>
#include <string>
#include <vector>
#pragma comment(lib,"user32.lib")

#define PSAPI_VERSION 1

#include <string>

#include <common.h>

 



typedef struct _prepare_offset
{
	std::string type_name;
	std::string fieldname;
	DWORD dwOffset;
	DWORD dwSize;
}prepare_offset, *Pprepare_offset;



prepare_offset g_offset_player[] = {
	{ "PVOID", "pVtable",0,0},
	{"DWORD","dwObjMap",0xe0,0},
	{ "DWORD", "Charid", 0x128,0},
	{"BYTE","Country",0x12d,0},
	{"BYTE", "Grow", 0x12f, 0},
	{ "BYTE" ,"MaxGrow",0x130,0 },
	{ "BYTE", "Level" ,0x136,0},


	{"shaiya_pos","pos", 0xd0,0xc },

	{ "WORD", "statspoint", 0x138, 0 },

	{ "WORD", "skillpoint", 0x13a, 0 },


	{ "DWORD" ,"kill",0x148,0 },
	{ "BYTE","Map",0x160,0 },
	{ "WORD", "str" ,0x16c,0},
	{ "WORD", "dex" ,0x16c + 2,0},
	{ "WORD", "_Int_" ,0x16c + 4,0},
	{ "WORD", "wis", 0x16c + 6, 0},
	{ "WORD", "rec", 0x16c + 8, 0 },
	{ "WORD", "luc" ,0x16c + 10,0},

	{ "DWORD", "Maxhp", 0x178, 0 },

	{ "DWORD", "Maxmp", 0x17c , 0 },

	{ "DWORD", "Maxsp", 0x180, 0 },

	{ "DWORD", "money", 0x140, 0 },

	{ "char", "charname", 0x184, 20 },
	{ "PplayerItemcharacterise","BagItem",0x1c0,24 * 6 * 4 } ,
	{ "DWORD", " total_str", 0x121c, 0 },
	{ "DWORD", " total_dex", 0x121c + 4, 0 },
	{ "DWORD", "total__Int_", 0x121c + 8, 0 },
	{ "DWORD", " total_wis", 0x121c + 12, 0 },
	{ "DWORD", " total_rec", 0x121c + 16, 0 },
	{ "DWORD", " total_luc", 0x121c + 20, 0 },

	{ "DWORD", " Attack_hight", 0x12e4, 0 },
	{ "DWORD", " Ph_defense", 0x13d4, 0 },
	{ "DWORD", " PhAttack_low", 0x13f8, 0 },
	{ "DWORD", " MagicAttack_low", 0x1424, 0 },
	{ "DWORD", " Magic_defense", 0x142c, 0 },
	{ "Pshaiya_party", " party", 0x17f4, 0 },
	{ "DWORD", " IsInPrivateMap", 0x58b4, 0 },
	{ "DWORD", " PrepareMap", 0x58bc, 0 },
	{ "shaiya_pos", " Parepos", 0x58c0, 0 },

 {"DWORD","uid",0x582c,0},

 {"PplayerItemcharacterise","whItem",0x400,40 * 6 * 4},
 {"PVOID","pShop",0x1634,0},
 {"DWORD","Mallpoint",0x5ac0,0}

};


prepare_offset g_offset_item[] = {
	{ "DWORD", "itemid", 0, 0 },
	{ "char", "itemname", 0x4, 20 + 0xe },
	{ "BYTE", "Reqlevel", 0x2e, 0 },
	{ "BYTE", "Country", 0x26, 0 },
	{ "BYTE", "Attackfighter", 0x27, 0 },
	{ "BYTE", "Defensefighter", 0x28, 0 },
	{ "BYTE", "Patrolrogue", 0x29, 0 },
	{ "BYTE", "Shootrogue", 0x2a, 0 },
	{ "BYTE", "Attackmage", 0x2b, 0 },
	{ "BYTE", "Defensemage", 0x2c, 0 },
	{ "BYTE", "Grow", 0x30, 0 },
	{ "BYTE", "ReqOg", 0x31, 0 },
	{ "BYTE", "ReqIg", 0x32, 0 },
	{ "WORD", "ReqVg", 0x34, 0 },

	{ "WORD", "ReqStr", 0x36, 0 },
	{ "WORD", "ReqDex", 0x38, 0 },
	{ "WORD", "ReqRec", 0x3a, 0 },
	{ "WORD", "ReqInt", 0x3c, 0 },
	{ "WORD", "ReqWis", 0x3e, 0 },

	{ "WORD", "Range", 0x42, 0 },
	{ "BYTE", "AttackTime", 0x44, 0 },
	{ "BYTE", "Attrib", 0x45, 0 },
	{ "BYTE", "Special", 0x46, 0 },
	{ "BYTE", "Slot", 0x47, 0 },


	{ "WORD", "Quality", 0x4c, 0 },
	{ "WORD", "Effect1", 0x52, 0 },
	{ "WORD", "Effect2", 0x58, 0 },
	{ "WORD", "Effect3", 0x5e, 0 },
	{ "WORD", "Effect4", 0x64, 0 },
	{ "WORD", "ConstHP", 0x6a, 0 },
	{ "WORD", "ConstSP", 0x6c, 0 },
	{ "WORD", "ConstMP", 0x6e, 0 },
	{ "WORD", "ConstStr", 0x70, 0 },
	{ "WORD", "ConstDex", 0x72, 0 },
	{ "WORD", "ConstRec", 0x74, 0 },
	{ "WORD", "ConstInt", 0x76, 0 },
	{ "WORD", "ConstWis", 0x78, 0 },
	{ "WORD", "ConstLuc", 0x7a, 0 },

	{ "DWORD", "Buy", 0x80, 0 },
	{ "DWORD", "Sell", 0x84, 0 },
	{ "WORD", "Grade", 0x7c, 0 },

	{ "BYTE", "Speed", 0x48, 0 },
	{ "BYTE", "Exp", 0x49, 0 },
	{ "BYTE", "Count", 0x4b, 0 }

};


prepare_offset g_offset_playeritem[] = {
	{ "PplayerItemcharacterise", "pNext", 0x4, 0 },
	{"BYTE","gems",0x18,6},
	{ "PITEMINFO", "ItemAddr", 0x30, 0 },
	{ "BYTE", "ItemType", 0x40, 0 },
	{ "BYTE", "Typeid", 0x41, 0 },
	{ "BYTE", "count", 0x42, 0 },
	{ "char", "craftname", 0x4c, 20 },
	{ "DWORD", "createtime", 0x64, 0 },
	{ "DWORD", "expiredtime", 0xa4, 0 },
	{ "BYTE", "color1", 0xa8, 9},
	{ "BYTE", "color2", 0xa8 + 9, 9 },
	{ "BYTE", "color3", 0xa8 + 18, 9 },
	{"UINT64","itemuid",0x38,0},
{"WORD","QA",0x44,0}

};

std::string& trim(std::string& text)
{
	if (!text.empty())
	{
		text.erase(0, text.find_first_not_of(" \n\r\t"));
		text.erase(text.find_last_not_of(" \n\r\t") + 1);
	}
	return text;
}

void getstruct(Pprepare_offset g_offset, DWORD dwCount)
{
	//冒泡排序一下
	for (DWORD i = 0; i < dwCount; i++)
	{
		for (DWORD j = i + 1; j < dwCount; j++)
		{
			if (g_offset[i].dwOffset >
				g_offset[j].dwOffset
				)
			{
				prepare_offset temp = g_offset[i];
				g_offset[i] = g_offset[j];
				g_offset[j] = temp;
			}
		}
	}
	//去处两边空格

	DWORD dwLastOffset = 0;
	DWORD j = 0;
	for (DWORD i = 0; i < dwCount; i++)
	{
		g_offset[i].fieldname = trim(g_offset[i].fieldname);
		g_offset[i].type_name = trim(g_offset[i].type_name);

		DWORD dwLeft = g_offset[i].dwOffset - dwLastOffset;


		if (dwLeft)
			printf("BYTE unKnown_%d[0x%x];\n", j++, dwLeft);

		if (!g_offset[i].dwSize)
		{
			std::string type_name = g_offset[i].type_name;
			if (type_name.compare("BYTE") == 0)
				g_offset[i].dwSize = 1;
			else if (type_name.compare("WORD") == 0)
				g_offset[i].dwSize = 2;
			else if (type_name.compare("UINT64") == 0)
				g_offset[i].dwSize = 8;
			else if (type_name.compare("shaiya_pos") == 0)
				g_offset[i].dwSize = 12;
			else
				g_offset[i].dwSize = 4;

			printf("%s %s;  //0x%x", g_offset[i].type_name.c_str(), g_offset[i].fieldname.c_str(), g_offset[i].dwOffset);
		}
		else
		{
			printf("%s %s[%d];  //0x%x", g_offset[i].type_name.c_str(), g_offset[i].fieldname.c_str(), g_offset[i].dwSize, g_offset[i].dwOffset);
		}

		printf("\n");

		dwLastOffset = g_offset[i].dwOffset + g_offset[i].dwSize;
	}



}




HANDLE hProcess = NULL;


void PrintNpc(DWORD npc)
{
	BYTE nType = 0;
	WORD wtypeId = 0;
	DWORD subNpc = 0;
	ReadProcessMemory(hProcess, (PVOID)npc, &subNpc, 4, 0);
	ReadProcessMemory(hProcess, PVOID(subNpc + 0x60), &nType, 1, 0);
	ReadProcessMemory(hProcess, PVOID(subNpc + 0x62), &wtypeId, 2, 0);
	printf("type:%d,typeid:%d\n", nType, wtypeId);
}


void EnumLeft(DWORD npc)
{

}


BYTE getSignature(DWORD npc)
{
	BYTE bSignature = 0;
	ReadProcessMemory(hProcess, PVOID(npc + 0x15), &bSignature, 1, 0);
	return bSignature;
}

DWORD getLeft(DWORD npc)
{
	DWORD left = 0;
	ReadProcessMemory(hProcess, PVOID(npc + 0x4), &left, 4, 0);
	return left;
}


DWORD getRight(DWORD npc)
{
	DWORD right = 0;
	ReadProcessMemory(hProcess, PVOID(npc + 0x8), &right, 4, 0);
	return right;
}

DWORD getSelf(DWORD npc)
{
	DWORD value = 0;
	ReadProcessMemory(hProcess, PVOID(npc), &value, 4, 0);
	return value;
}

void PreEnumNpc(DWORD npc)
{
	DWORD NextNPc = getSelf(npc);
	do
	{

		PrintNpc(NextNPc + 0x10);
		if (getSignature(NextNPc)) {
			continue;
		}


		DWORD right = getRight(NextNPc);

		if (getSignature(right))
		{
			DWORD NextNPcBackup = 0;

			do
			{
				NextNPcBackup = NextNPc;
				NextNPc = getLeft(NextNPc);

				if (getSignature(NextNPc)) {
					break;
				}
	
				if (NextNPcBackup != getRight(NextNPc)) {
					break;
				}
			} while (1);

		}
		else
		{
			NextNPc = right;
			do 
			{		
				DWORD temp = getSelf(NextNPc);
				if (getSignature(temp)){
					break;
				}
				NextNPc = temp;
			} while (1);
		}
	} while (npc != NextNPc);

}

void testNpc()
{
	DWORD npcBase = (0x0221D000 + 0x13da48);
	EnableDebugPrivilege(TRUE);
	HWND hWnd = FindWindowA("GAME", "Shaiya");
	DWORD pid = 0;
	GetWindowThreadProcessId(hWnd, &pid);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
	DWORD dwTemp1 = 0;
	DWORD firstNpc = 0;

	ReadProcessMemory(hProcess, (PVOID)npcBase, &dwTemp1, 4, 0);
	PreEnumNpc(dwTemp1);
}




HHOOK hHook = NULL;
LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	if (code >= 0)
		return 1;                              //消息不再传递个下一个HOOK子程，也不会再发送给目的窗口
	else
		return CallNextHookEx(hHook, HC_ACTION, wParam, lParam);
}

void testHook(){
	hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, 0, GetCurrentThreadId());


	int a;
	scanf("%d", &a);
}





#include <Shlwapi.h>
#include <define.h>






void shiayaInvason(PVOID pbuff,DWORD len) {

	static DWORD istatCount = 0;
	
	_asm {
		mov ebx,pbuff
		mov eax, len
		add istatCount, eax
		mov ecx, istatCount
		_loop:
		dec eax
		dec ecx
		lea edi, dword ptr ds : [eax + ebx]
		mov dl, byte ptr ds : [edi]
		xor dl, cl
		ror dl, 1
		xor dl, 0x7F
		ror dl, 0x3
		mov byte ptr ds : [edi], dl
		test eax, eax
		jnz _loop
	}
}


void decryptShaiyaInvason() {
	FILE* pfile = fopen("D:\\Shaiya_Invasion_V3\\Shaiya Invasion V3\\data.sah", "rb");
	FILE* pWrite = fopen("D:\\Shaiya_Invasion_V3\\Shaiya Invasion V3\\data1.sah", "wb");


	BYTE Temp[100] = { 0 };
	fread(Temp, 3, 1, pfile);
	shiayaInvason(Temp, 4);
	fwrite(Temp, 3, 1, pWrite);

	while (!feof(pfile))
	{
		BYTE Temp[100] = { 0 };
		fread(Temp, 4, 1, pfile);
		shiayaInvason(Temp, 4);
		fwrite(Temp, 4, 1, pWrite);
	}
	fclose(pfile);
	fclose(pWrite);
}


#include <define.h>








#define PR_OFFSET(_t, _m)       \
        (void)printf("['%s',%d,%d]\n",  #_t "." #_m,(int)offsetof(_t, _m),(int)sizeof(((_t *)0)->_m))











HANDLE getShaiyaHandle() {
	HANDLE hProcess = 0;
	DWORD pid = 0;
	HWND hwnd = FindWindow(NULL, L"Shaiya");
	if (hwnd) {	
		GetWindowThreadProcessId(hwnd, &pid);
		if (pid) {
			hProcess = OpenProcess(PROCESS_VM_READ, FALSE, pid);
		}
	}
	return hProcess;
}


void getShaiyaPos() {
	while (true)
	{
		static HANDLE hProcess = NULL;
		if (hProcess) {
			float y = 0;
			DWORD dwRead = 0;
			if (!ReadProcessMemory(hProcess, (PVOID)0x21f6b38, &y, 4, &dwRead)) {
				hProcess = NULL;
				continue;
			}
			printf("%f\n", y);
			Sleep(500);
		}
		else {
			hProcess = getShaiyaHandle();
		}

		system("cls");
		Sleep(1000);
	}
}

#include <define.h>




//检测天天
DWORD g_WindowCount = 0;
DWORD g_currentProcessId = 0;
BOOL CALLBACK EnumChildWindowCallBack(HWND hWnd, LPARAM lParam)
{
	DWORD pid = 0;
	GetWindowThreadProcessId(hWnd, &pid);
	if (g_currentProcessId == pid) {
		++g_WindowCount;
		EnumChildWindows(hWnd, EnumChildWindowCallBack, 0);
	}

	return TRUE;
}

BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)
{
	DWORD pid = 0;
	GetWindowThreadProcessId(hWnd, &pid);
	if (g_currentProcessId == pid) {
		++g_WindowCount;
		EnumChildWindows(hWnd, EnumChildWindowCallBack, 0);
	}
	return true;
}



int _tmain(int argc, _TCHAR* argv[])
{
	/*********************/
	HWND hwnd = FindWindow(NULL, L"Shaiya");
	GetWindowThreadProcessId(hwnd, &g_currentProcessId);
	g_WindowCount = 0;
	EnumWindows(EnumWindowCallBack, NULL);
	printf("%d", g_WindowCount);
	/*********************/


	PBYTE base =(PBYTE) GetProcAddress(GetModuleHandle(L"user32.dll"), "EnumWindows");
	DWORD addr = 0;
	while (TRUE)
	{
		if (*base == 0xe8) {
			addr = DWORD(base + *PINT(base + 1) + 5);
			break;
		}
		base++;	
	}
	


//	getShaiyaPos();

//	MemoryLoadLibrary("test.dll");

	system("pause");
	return 0;

}

