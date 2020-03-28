#include <MyClass.h>
#include <windows.h>
#include <MyInlineHook.h>
#include <process.h>


#define PSAPI_VERSION 1







namespace Pt
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

	bool g_IsShowPet = true;
	bool g_IsShowWing = true;




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


	namespace globalmessage
	{
		CMyInlineHook obj1, obj2, obj3, obj4;
		void __stdcall ChangeMessageColor(char*);
		void __stdcall GetColor(BYTE*);
		void __stdcall ChangeMessageColor2(BYTE*);

		PVOID g_pfunChangeColor2 = ChangeMessageColor2;
		PVOID g_pfunGetColor = GetColor;
		PVOID g_pfunChangeMessageColor = ChangeMessageColor;
		CRITICAL_SECTION g_Messagecs;
		DWORD g_dwRgb = 0;

		void __stdcall GetColor(BYTE* strPacket)
		{
			DWORD dwPacket = (DWORD)strPacket;
			strPacket += 0x18;
			strPacket += strlen((char*)strPacket);


			DWORD dwCall = 0x005e5540;
			g_dwRgb = 0xffff80ff;      //此处GB是反过来的

			//如果有颜色就取颜色,否则就默认

			if (*(PDWORD)strPacket != 0)
			{
				(PBYTE(&g_dwRgb))[3] = 0xff;
				(PBYTE(&g_dwRgb))[2] = strPacket[1];
				(PBYTE(&g_dwRgb))[1] = strPacket[2];
				(PBYTE(&g_dwRgb))[0] = strPacket[3];
			}
			


			EnterCriticalSection(&g_Messagecs);
			_asm{
				push dwPacket
					call dwCall
					add esp, 0x4
			}
			LeaveCriticalSection(&g_Messagecs);
		}

		//屏幕左下角颜色
		void __stdcall ChangeMessageColor2(BYTE* color)
		{
			color[0] = (PBYTE(&g_dwRgb))[2];
			color[1] = (PBYTE(&g_dwRgb))[1];
			color[2] = (PBYTE(&g_dwRgb))[0];
		}


		//设置屏幕中间颜色
		void __stdcall ChangeMessageColor(char* strChat)
		{
			if (strncmp("[全体聊天]", strChat, strlen("[全体聊天]")) != 0)
			{
				return;
			}
			DWORD dwLen = strlen(strChat);
			*PDWORD(strChat + dwLen + 1) = g_dwRgb;
		}

		//设置左下角颜色
		__declspec(naked) void Naked1()
		{
			_asm
			{
				pushad
					lea eax, [eax + 0x108]
					push eax
					call g_pfunChangeColor2
					popad
					retn 0x4
			}
		}


		//设置屏幕中间颜色
		__declspec(naked) void Naked2()
		{
			_asm
			{

				mov ecx, dword ptr ds : [edi + 0x4]
					mov dword ptr ds : [ecx], edi
					pushad
					mov edi, dword ptr ds : [edi + 0x8]
					push edi
					call g_pfunChangeMessageColor
					popad
					jmp obj2.m_pRet

			}
		}

		//屏幕中间全体读取颜色,颜色已经保存在文字内容里了
		__declspec(naked) void Naked3()
		{
			_asm
			{
				push 0xFFFF80FF
					pushad
				__loop :
				cmp byte ptr ds : [edi], 0x0
					lea edi, dword ptr ds : [edi + 0x1]
					jnz __loop
					cmp dword ptr ds : [edi], 0x0
					je __tga
					mov edi, dword ptr ds : [edi]
					mov dword ptr ss : [esp + 0x20], edi
				__tga :
				popad
					jmp obj3.m_pRet


			}
		}

		//封包最开始处理的地方,用来保存颜色
		__declspec(naked) void Naked4()
		{
			_asm
			{
				mov edx, dword ptr[esp + 0x8]
					pushad
					push edx
					call g_pfunGetColor
					popad
					retn
			}
		}

		void Start()
		{
			obj1.Hook((PVOID)0x0041c05f, Naked1, 8);
			//没完成
			obj2.Hook((PVOID)0x00422CB9, Naked2, 5);//00422CB9  |.  8B4F 04       mov ecx,dword ptr ds:[edi+0x4]

			obj3.Hook((PVOID)0x004D6DB7, Naked3, 5); //004D6DB7 | .  68 FF80FFFF | push 0xFFFF80FF
			obj4.Hook((PVOID)0x005F2FA9, Naked4, 5);  //005F2FA9  |> \8B5424 08     mov edx,dword ptr ss:[esp+0x8]                                         ;  Case 1108 of switch 005F1E17



			InitializeCriticalSection(&g_Messagecs);
		}
	}
	namespace weapon_appreance
	{
#define MAXENCHANT 49
		typedef struct _EnchantAddValue
		{
			WORD wWeaponValue;
		}EnchantAddValue, *PEnchantAddValue;
/*		EnchantAddValue g_stcEnchantAddValue[MAXENCHANT + 1];*/

		EnchantAddValue g_stcEnchantAddValue[MAXENCHANT + 1] = {
			0,
			7,
			14,
			21,
			31,
			41,
			51,
			64,
			77,
			90,
			106,
			122,
			138,
			157,
			176,
			195,
			217,
			239,
			261,
			286,
			311
		};
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
// 			//1.从文件中读取数植
// 			WCHAR szTemp[MAX_PATH] = { 0 };
// 			GetCurrentDirectory(MAX_PATH, szTemp);
// 			GetModuleFileName(GetModuleHandle(NULL),szTemp, MAX_PATH);
// 			PathRemoveFileSpec(szTemp);
// 			wcscat_s(szTemp, MAX_PATH, L"\\ItemEnchant.ini");
// 
// 
// 			for (WORD i = 0; i < _countof(g_stcEnchantAddValue); i++)
// 			{
// 				WCHAR szWeapon[MAX_PATH];
// 				swprintf_s(szWeapon, MAX_PATH, L"WeaponStep%02d", i);
// 				g_stcEnchantAddValue[i].wWeaponValue = GetPrivateProfileInt(L"LapisianEnchantAddValue", szWeapon, 0, szTemp);
// 			}

			obj1.Hook((PVOID)0x004b8755, Naked_1, 17);
		}
	}
	namespace SkillEffect
	{
		CMyInlineHook obj1, obj2, obj3, obj4, obj5, obj6;
		PVOID pEnable = (PVOID)0x007c0e18;

		__declspec(naked) void Naked_1()
		{
			_asm
			{
				mov eax, pEnable
					mov al, byte ptr[eax]
					cmp al, 0x1
					je _orginal
					retn 0x18
				_orginal:
				mov eax, dword ptr[esp + 0x4]
					sub esp, 0x10
					jmp obj1.m_pRet
			}
		}
		__declspec(naked) void Naked_2()
		{
			_asm
			{
				mov eax, pEnable
					mov al, byte ptr[eax]
					cmp al, 0x1
					je _orginal
					retn 0x40
				_orginal:
				push 0xffffff
					push 0x72E541
					jmp obj2.m_pRet
			}
		}
		__declspec(naked) void Naked_3()
		{
			_asm
			{
				mov eax, pEnable
					mov al, byte ptr[eax]
					cmp al, 0x1
					je _orginal
					retn 0x1c
				_orginal:
				push ebp
					mov ebp, esp
					and esp, 0xFFFFFFF8
					jmp obj3.m_pRet
			}
		}
		__declspec(naked) void Naked_4()
		{
			_asm
			{
				mov eax, pEnable
					mov al, byte ptr[eax]
					cmp al, 0x1
					je _orginal
					retn 0x18
				_orginal:
				mov eax, dword ptr ss : [esp + 0x4]
					sub esp, 0x10
					jmp obj4.m_pRet
			}
		}
		__declspec(naked) void Naked_5()
		{
			_asm
			{
				mov eax, pEnable
					mov al, byte ptr[eax]
					cmp al, 0x1
					je _orginal
					retn 0x1c
				_orginal:
				mov eax, dword ptr[esp + 0x4]
					sub esp, 0x10
					jmp obj5.m_pRet
			}
		}
		__declspec(naked) void Naked_6()
		{
			_asm
			{
				mov eax, pEnable
					mov al, byte ptr[eax]
					cmp al, 0x1
					je _orginal
					retn 0x14
				_orginal:
				push ebx
					mov ebx, dword ptr ss : [esp + 0x14]
					jmp obj6.m_pRet
			}
		}



		void Start()
		{
			obj1.Hook((PVOID)0x00459120, Naked_1, 7);
			obj2.Hook((PVOID)0x0045cba0, Naked_2, 7);
			obj3.Hook((PVOID)0x00459690, Naked_3, 6);
	//		obj4.Hook((PVOID)0x0043a300, Naked_4, 7);
	//		obj5.Hook((PVOID)0x0041a2c0, Naked_5, 7);
	//		obj6.Hook((PVOID)0x00459620, Naked_6, 5);
		}
	}
	namespace skill_potion_delay
	{
		CMyInlineHook obj1;
		DWORD dwTemp = 0x005ea9a0, dwTemp2 = 0x004fef04;
		__declspec(naked) void Naked_1()
		{
			_asm
			{
				cmp byte ptr [eax+0x2c],0x0         // "special" field of item
					jne __orginal
				sub esp, 0x28
					pushad
					mov eax, dword ptr ss : [esp + 0x34]       //slot
					mov word ptr ss : [esp + 0x20], 0x50A
					mov byte ptr ss : [esp + 0x22], bl         //bag
					movzx eax, dword ptr[esp + 0x74]
					mov byte ptr ss : [esp + 0x23], al         //slot
					mov byte ptr ss : [esp + 0x24], 0x0
					push 0x4
					lea eax, dword ptr ss : [esp + 0x24]
					push eax
					call dwTemp
					add esp, 0x8
					popad
					add esp, 0x28
					jmp dwTemp2

				__orginal:
				mov esi,eax
					cmp byte ptr ds : [esi + 0x8], 0x2C
				jmp obj1.m_pRet
			}
		}


		void Start()
		{
			{
				BYTE byTemp = 0xeb;
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0044b1e6, &byTemp, 1);
			}

			{
				BYTE byTemp[] = { 0x90, 0x90 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x005ebd0c, byTemp, 2);
			}

			obj1.Hook((PVOID)0x004FEE3b, Naked_1, 6);


		}
	}
	namespace SahProtection
	{
		CMyInlineHook obj1;
		void __stdcall fun_decrypt(PDWORD pCode);
		PVOID g_pfunDecrypt = fun_decrypt;

		void __stdcall fun_decrypt(PDWORD pCode)
		{
			DWORD dwRet = *pCode;
			
			_asm
			{
				xor dwRet, 0x74
					xor dwRet, 0x61
					xor dwRet, 0x68
					xor dwRet, 0x39          //null
			}
			*pCode = dwRet;
		}

		__declspec(naked) void Naked_1()
		{
			_asm
			{
				pushad
					 mov esi, dword ptr ss : [esp + 0x24]
					 push esi
					call g_pfunDecrypt
					popad
					add esp, 0xC
					cmp dword ptr ss : [esp + 0x14], ebx
					jmp obj1.m_pRet
			}
		}


		void Start()
		{
			char strHeader[] = "fff";
			MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x748010, strHeader, strlen(strHeader));
			obj1.Hook((PVOID)0x0040e0bd, Naked_1, 7);
		}
	}
	namespace Hide_pet
	{
		CMyInlineHook obj1;
		__declspec(naked) void Naked_HidePet1()
		{
			_asm
			{
				cmp dword ptr[esp], 0x00439740   //宠物第二层地址
					je __pet
					cmp dword ptr[esp],0x0043b41b //翅膀
					je __wing
					jmp __orgianl

				__pet :
				cmp dword ptr [esp+0xa4],0x0041833f  //宠物的首层地址
				jne __orgianl
				lea eax, g_IsShowPet
				movzx eax, byte ptr [eax]
				cmp eax,0x1
				jz __orgianl
				mov eax,0x0
				jmp obj1.m_pRet

			__wing :
				lea eax, g_IsShowWing
					movzx eax, byte ptr[eax]
					cmp eax, 0x1
					jz __orgianl
					mov eax, 0x0
					jmp obj1.m_pRet




				__orgianl:
				mov eax, dword ptr [ecx + 0x0000012C]
				jmp obj1.m_pRet
			}
		}



		void Start()
		{
			obj1.Hook((PVOID)0x00587124, Naked_HidePet1, 6);
		}
	}
	namespace Custom_Button
	{
		CMyInlineHook g_obj1;

		DWORD dwTemp = 0x0051C070;
		DWORD g_dwCommand []= { 0x66, 0x79, 0x6c, 0x6e, 0x6f, 0x65 };
		__declspec(naked) void Naked_1()
		{
			_asm
			{
				cmp edi, 0x5
					jg __orgianl
					push ecx
				lea ecx, g_dwCommand
					mov ecx, [ecx + edi * 4]
					mov dword ptr ds : [0x9144E4], ecx
					pop ecx
				__orgianl :
				call dwTemp
					jmp g_obj1.m_pRet
			}
		}
		void Start()
		{
			g_obj1.Hook((PVOID)0x00522160, Naked_1, 5);
		}
	}

	namespace chat_command
	{

 		chat_cmd g_chatcmd[] = { 
 	           //		                  基址                偏移层数    多层偏移 buffer 大小
			{ "/pet", "on", {  (DWORD)&g_IsShowPet, 0,     { 0 } }, { 0x1 },  1 },
			{ "/pet", "off", { (DWORD)&g_IsShowPet, 0,     { 0 } }, { 0x0 }, 1 },
			{ "/wing", "on", { (DWORD)&g_IsShowWing, 0,    { 0 } }, { 0x1 }, 1 },
			{ "/wing", "off", { (DWORD)&g_IsShowWing, 0,   { 0 } }, { 0x0 }, 1 }
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
				strcpy_s(strInput,MAX_PATH, strChat);

				//取出命令跟参数
				sscanf_s(strInput, "%s %s", strCommand, MAX_PATH,strOption,MAX_PATH);
				if (strCommand[0] == '\0' || strOption[0] == '\0')
					break;

				//循环匹配
				for (DWORD i = 0; i < _countof(g_chatcmd); i++)
				{
					Pchat_cmd pCmd = &g_chatcmd[i];
					if (strcmp(pCmd->strCommand, strCommand) == 0 && strcmp(pCmd->strOption, strOption) == 0)
					{

						DWORD dwAddr = GetAddrOfObject(pCmd->Addr);  //从基址偏移找出最终的成员
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
				mov eax,edi
					lea edx, dword ptr[eax+1]  //orginal
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
			g_obj1.Hook((PVOID)0x004866a2, Naked_1, 5);
		}
	}


	namespace namecolor
	{

		DWORD g_dwRandomSign = 0xffff7fff;  //如果服务器传来的是这个颜色值,则说明是随机颜色
		DWORD g_dwRandCoor   = 0xffffffff;


		bool g_bEnable = false;
		CMyInlineHook  objNormal, ObjParty, objEnemy;
		CRITICAL_SECTION g_cs;
		cmd_color g_stcColor[200];

		DWORD __stdcall ChangeColor(DWORD, DWORD);
		PVOID g_pfunChangeColor = ChangeColor;

		DWORD __stdcall ChangeColor(DWORD dwPlayer, DWORD dwOldColor)
		{
			DWORD dwCharid = *PDWORD(dwPlayer + 0x34);
			DWORD dwRet = dwOldColor;
			EnterCriticalSection(&g_cs);
			for (DWORD i = 0; i < _countof(g_stcColor); i++)
			{
				if (!g_stcColor[i].dwCharid)
					break;

				//判断是否随机颜色
				if (g_stcColor[i].dwCharid == dwCharid)
				{
					if (g_stcColor[i].dwRGB == g_dwRandomSign)
						dwRet = g_dwRandCoor;
					else
						dwRet = g_stcColor[i].dwRGB;
					break;
				}
			}
			LeaveCriticalSection(&g_cs);
			return dwRet;
		}


		//此用一直不用变
		void fun_getvipcolor(Pcmd_color pColor)
		{
			EnterCriticalSection(&g_cs);
			ZeroMemory(g_stcColor, sizeof(g_stcColor));
			for (DWORD i = 0; i < _countof(g_stcColor); i++)
			{
				if (!pColor[i].dwCharid)
					break;

				g_stcColor[i].dwCharid = pColor[i].dwCharid;
				g_stcColor[i].dwRGB = pColor[i].dwRGB;
			}
			LeaveCriticalSection(&g_cs);
		}

		__declspec(naked) void Naked_Normal()
		{
			_asm
			{
				mov dword ptr ss : [esp + 0x18], 0xffffffff
					pushad
					pushfd      //4个字节
					mov ebp,dword ptr [esp+0x3c]   
					push ebp   //old color
					push esi
					call g_pfunChangeColor
					mov dword ptr [esp+0x3c], eax

					popfd
					popad
					jmp objNormal.m_pRet
			}
		}
		__declspec(naked) void Naked_Party()
		{

			_asm
			{
				mov dword ptr ss : [esp + 0x18], 0xFF00FF00

		           pushad
				   pushfd
				   mov ebp,[esp+0x3c]
					push ebp
					push esi
					call g_pfunChangeColor
					mov dword ptr [esp+0x3c], eax

					popfd
					popad

					jmp ObjParty.m_pRet
			}
		}
		DWORD WINAPI ThreadProc1(_In_  LPVOID lpParameter)
		{
			DWORD dwTemp = 0;
			while (true)
			{
				Sleep(100);
				dwTemp += 100;
				if (dwTemp > 0xffffff)
				{
					dwTemp = 0;
				}
				g_dwRandCoor = 0xff000000 + dwTemp;
			}
		}
		void Start()
		{
			InitializeCriticalSection(&g_cs);
			//正常角色名字颜色地方
			objNormal.Hook((PVOID)0x004537d5, Naked_Normal, 8);
			//组队
			//0044BAE4   .  C74424 18 00FF00FF      mov dword ptr ss:[esp+0x18],0xFF00FF00
			ObjParty.Hook((PVOID)0x0044BAE4, Naked_Party, 8);

			::CreateThread(0, 0, ThreadProc1,0,0,0);
		}
	}


	namespace custompacket
	{
		CMyInlineHook objPacket;
		void __stdcall CustomPacket(PWORD pCmd);
		PVOID g_punCustomPacket = CustomPacket;

		void __stdcall CustomPacket(PWORD pCmd)
		{
			switch (*pCmd)
			{
				//总之只有接到时钟才算
			case PacketOrder::Clock:
				//	fun_clock((Pcmd_clock)++pCmd);
				break;
			case PacketOrder::VipColor:
				namecolor::fun_getvipcolor((Pcmd_color)++pCmd);
				break;
			default:
				break;
			}
		}

		__declspec(naked) void Naked1()
		{
			_asm
			{
				cmp eax, 0xff00
					jl _orginal
					pushad
					mov eax, [esp + 0x28]
					push eax            //packets
					call g_punCustomPacket
					popad
				_orginal :
				sub eax, 0x0000FA08
					jmp objPacket.m_pRet
			}
		}

		void Start()
		{
			//自定义封包处理的地方  找特证码拿地址，其它不用改全一样
			objPacket.Hook((PVOID)0x005f39a8, Naked1, 5);
		}
	}



	namespace bufficon
	{
		CMyInlineHook obj1,obj2;
		__declspec(naked) void Naked2()
		{
			_asm
			{
				push edx
					push ecx
					mov ecx, 0x5                       //总长除以2
					xor edx, edx
					mov eax, dword ptr ds : [0x7AB0E8]    //max x
					div ecx
					add eax, dword ptr ds : [0x7AB0D8]   //posx
					pop ecx
					pop edx
					jmp obj2.m_pRet
			}
		}

		__declspec(naked) void Naked1()
		{
			_asm
			{
				push edx
					 push ecx
					 mov ecx, 0x5                           //总长除以2
					 xor edx, edx
					 mov eax, dword ptr ds : [0x7AB0E8]    //max x
					 add eax,eax
					 div ecx
					 add eax, dword ptr ds : [0x7AB0D8]   //posx
					 pop ecx
					 pop edx
					jmp obj1.m_pRet
			}
		}

		void Start()
		{
			obj1.Hook((PVOID)0x004d73da, Naked1, 5);
			obj2.Hook((PVOID)0x004d74f7, Naked2, 5);
		}
	}



	namespace common
	{

		void proc(void* p) {
			Sleep(5000);
			//1.minimap
			{
				BYTE byCodes[3] = { 0x90, 0x90, 0x90 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004d954b, byCodes, sizeof(byCodes));
			}

			//2.hp
			{
				BYTE byCodes[] = { 0x74, 0x22, 0x80, 0xB8, 0xE9, 0x01, 0x00, 0x00, 0x00, 0x89, 0xB0, 0x58, 0x01, 0x00, 0x00, 0x89, 0x90, 0x60, 0x01, 0x00, 0x00, 0x89, 0x88, 0x68, 0x01, 0x00, 0x00, 0x74, 0x07, 0xC6, 0x80, 0xE9,0x01, 0x00, 0x00, 0x00, 0x39, 0x3D, 0xF4, 0xE2, 0x90, 0x00, 0x75, 0x12, 0x89, 0x35, 0x78, 0x44, 0x91, 0x00, 0x89, 0x15, 0x7C, 0x44, 0x91, 0x00, 0x89, 0x0D, 0x80, 0x44, 0x91, 0x00 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x005942cf, byCodes, sizeof(byCodes));
			}

			//3.fast rr and enchanting
			{
				BYTE byCodes[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00501643, byCodes, sizeof(byCodes));
			}

			{
				BYTE byCodes[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004c4d2e, byCodes, sizeof(byCodes));
			}

			//4. enable /party
			{
				BYTE byCodes[] = { 0x5 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00486D7a, byCodes, sizeof(byCodes));
			}

			weapon_appreance::Start();
			//		skill_potion_delay::Start();
			//		SkillEffect::Start();
			Custom_Button::Start();
			chat_command::Start();
			Hide_pet::Start();

			custompacket::Start();
	//		namecolor::Start();
	//		bufficon::Start();

			//		globalmessage::Start();

			//	SahProtection::Start();
		}


	
		void Start()
		{

			_beginthread(proc, NULL,NULL);



		}
	}





}