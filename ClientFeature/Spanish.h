#include <MyClass.h>
#include <windows.h>
#include <MyInlineHook.h>


#define PSAPI_VERSION 1





namespace Spanish
{

	typedef struct _MultiplePointer
	{
		DWORD dwObject;
		DWORD PointerCount;
		DWORD Pointer[20];
	}MultiplePointer, *PMultiplePointer;


	typedef struct _chat_cmd
	{
		char* strCommand;
		char* strOption;
		MultiplePointer Addr;
		BYTE byChangedCodes[50];
		DWORD dwCodesSize;
	}chat_cmd, *Pchat_cmd;

	DWORD GetAddrOfObject(MultiplePointer stc)
	{
		DWORD dwRet = stc.dwObject;
		if (!stc.PointerCount)
			return dwRet;

		//	dwRet = *PDWORD(dwRet);
		for (DWORD i = 0; i < stc.PointerCount; i++)
		{
			DWORD dwTemp = (*PDWORD(dwRet) + stc.Pointer[i]);
			//有可能内存不存在
			if ((void*)dwTemp == nullptr)
				return 0;
			dwRet = dwTemp;
		}
		return dwRet;
	}


	bool g_IsShowPet = true;
	bool g_IsShowWing = true;


	namespace Hide_pet
	{
		CMyInlineHook obj1;
		__declspec(naked) void Naked_HidePet1()
		{
			_asm
			{
				cmp dword ptr[esp], 0x00439a70   //宠物第二层地址
				je __pet
				cmp dword ptr[esp], 0x0043b74b //翅膀
				je __wing
				jmp __orgianl

				__pet :
				cmp dword ptr[esp + 0xa4], 0x004183c0  //宠物的首层地址
					jne __orgianl
					lea eax, g_IsShowPet
					movzx eax, byte ptr[eax]
					cmp eax, 0x1
					jz __orgianl
					mov eax, 0x0
					jmp obj1.m_pRet

					__wing :
				lea eax, g_IsShowWing
					movzx eax, byte ptr[eax]
					cmp eax, 0x1
					jz __orgianl
					mov eax, 0x0
					jmp obj1.m_pRet




					__orgianl :
				mov eax, dword ptr[ecx + 0x0000012C]
					jmp obj1.m_pRet
			}
		}



		void Start()
		{
			obj1.Hook((PVOID)0x00587bc4, Naked_HidePet1, 6);
		}
	}

	namespace chat_command
	{

		chat_cmd g_chatcmd[] = {
			//		                  基址                偏移层数    多层偏移 buffer 大小
			{ "/pet", "on",{ (DWORD)&g_IsShowPet, 0,{ 0 } },{ 0x1 },  1 },
			{ "/pet", "off",{ (DWORD)&g_IsShowPet, 0,{ 0 } },{ 0x0 }, 1 },
			{ "/wing", "on",{ (DWORD)&g_IsShowWing, 0,{ 0 } },{ 0x1 }, 1 },
			{ "/wing", "off",{ (DWORD)&g_IsShowWing, 0,{ 0 } },{ 0x0 }, 1 }
		};


		CMyInlineHook g_obj1;
		bool __stdcall fun_chat_command(char*);
		PVOID g_pfun = fun_chat_command;


		bool __stdcall fun_chat_command(char* strChat)
		{
			bool bRet = false;
			char* strInput = new char[MAX_PATH];
			char* strCommand = new char[MAX_PATH];
			char* strOption = new char[MAX_PATH];
			do
			{
				strcpy_s(strInput, MAX_PATH, strChat);

				//取出命令跟参数
				sscanf_s(strInput, "%s %s", strCommand, MAX_PATH, strOption, MAX_PATH);
				if (strCommand[0] == '\0' || strOption[0] == '\0')
					break;

				//循环匹配
				for (DWORD i = 0; i < _countof(g_chatcmd); i++)
				{
					Pchat_cmd pCmd = &g_chatcmd[i];
					if (strcmp(pCmd->strCommand, strCommand) == 0 && strcmp(pCmd->strOption, strOption) == 0)
					{

						DWORD dwAddr = Spanish::GetAddrOfObject(pCmd->Addr);  //从基址偏移找出最终的成员
						if (dwAddr)
							MyWriteProcessMemory(GetCurrentProcess(), (PVOID)dwAddr, &pCmd->byChangedCodes, pCmd->dwCodesSize);
						bRet = true;
						break;
					}
				}
			} while (0);
			delete[] strInput;
			delete[] strCommand;
			delete[] strOption;

			return bRet;
		}

		__declspec(naked) void Naked_1()
		{
			_asm
			{
				mov eax, edi
				lea edx, dword ptr[eax + 1]  //orginal
				pushad
				pushfd
				push eax
				call g_pfun
				popfd
				popad
				jmp g_obj1.m_pRet

			}
		}
		void Start()
		{
			g_obj1.Hook((PVOID)0x00486902, Naked_1, 5);
		}
	}



	namespace weapon_appreance
	{
#define MAXENCHANT 49
		typedef struct _EnchantAddValue
		{
			WORD wWeaponValue;
			//	WORD wArrmorValue;
		}EnchantAddValue, *PEnchantAddValue;
		EnchantAddValue g_stcEnchantAddValue[MAXENCHANT + 1];
		DWORD g_dwEnchant = (DWORD)&g_stcEnchantAddValue[0];

		CMyInlineHook obj1;
		__declspec(naked) void Naked_1()
		{
			_asm
			{
				push ebx
					mov ebx, g_dwEnchant
					movzx eax, word ptr ds : [ebx + eax * 2]  //eax为阶段
					pop ebx
					jmp obj1.m_pRet
			}
		}

		void Start()
		{
			//1.从文件中读取数植
			WCHAR szTemp[MAX_PATH] = { 0 };
			GetCurrentDirectory(MAX_PATH, szTemp);
			GetModuleFileName(GetModuleHandle(NULL),szTemp, MAX_PATH);
			PathRemoveFileSpec(szTemp);
			wcscat_s(szTemp, MAX_PATH, L"\\ItemEnchant.ini");

		
			for (WORD i = 0; i < _countof(g_stcEnchantAddValue); i++)
			{
				WCHAR szWeapon[MAX_PATH];
				swprintf_s(szWeapon,MAX_PATH,L"WeaponStep%02d", i);
				g_stcEnchantAddValue[i].wWeaponValue = GetPrivateProfileInt(L"LapisianEnchantAddValue", szWeapon, 0, szTemp);
			}

			obj1.Hook((PVOID)0x004b8a6f, Naked_1, 17);
		}
	}







	namespace common
	{
		void Start()
		{
			weapon_appreance::Start();
			Hide_pet::Start();
			chat_command::Start();

		}
	}





}