#include <MyClass.h>
#include <windows.h>
#include <MyInlineHook.h>
#include <Shlwapi.h>
#include <define.h>
#pragma comment(lib,"Shlwapi.lib")


namespace Incu
{

	bool g_IsShowPet = true;
	bool g_IsShowWing = true;


	namespace namecolor
	{
		bool g_bEnable = false;
		CMyInlineHook  objNormal, ObjParty, objEnemy;
		cmd_color g_stcColor[200];

		DWORD g_dwRandomSign = 0xffff7fff;  //如果服务器传来的是这个颜色值,则说明是随机颜色
		DWORD g_dwRandCoor = 0xffffffff;

		DWORD __stdcall ChangeColor(DWORD,DWORD);
		PVOID g_pfunChangeColor = ChangeColor;

		DWORD __stdcall ChangeColor(DWORD dwPlayer,DWORD dwOldColor)
		{
			DWORD dwCharid = *PDWORD(dwPlayer + 0x34);
			DWORD dwRet = dwOldColor;
			for (DWORD i = 0; i < _countof(g_stcColor); i++)
			{
				if (!g_stcColor[i].dwCharid)
					break;

				if (g_stcColor[i].dwCharid == dwCharid)
				{
					if (g_stcColor[i].dwRGB == g_dwRandomSign)
						dwRet = g_dwRandCoor;
					else
						dwRet = g_stcColor[i].dwRGB;
					break;
				}

			}

			return dwRet;
		}


		//此函数一直不用变
		void fun_getvipcolor(PVOID p)
		{
			
			PPLAYERCOLOR playerColor = (PPLAYERCOLOR)p;

			ZeroMemory(g_stcColor, sizeof(g_stcColor));
			for (DWORD i = 0; i < _countof(g_stcColor); i++)
			{
				if (!playerColor->color[i].dwCharid)
					break;

				g_stcColor[i].dwCharid = playerColor->color[i].dwCharid;
				g_stcColor[i].dwRGB = playerColor->color[i].dwRGB;
			}

		}

		__declspec(naked) void Naked_Normal()
		{
			_asm
			{
				or ebp, 0xFFFFFFFF

					push eax
					push ecx
					push edx
					push ebx
					push esp
					push esi
					push edi

					push ebp
					push esi
					call g_pfunChangeColor
					mov ebp,eax
					
					pop edi
					pop esi
					pop esp
					pop ebx
					pop edx
					pop ecx
					pop eax


					sub eax, 0x4
					jmp objNormal.m_pRet
			}
		}
		__declspec(naked) void Naked_Party()
		{

			_asm
			{
				mov ebp, 0xFF00FF00
					
					//保存
					push eax
					push ecx
					push edx
					push ebx
					push esp
					push esi
					push edi


					push ebp
					push esi
					call g_pfunChangeColor
					mov ebp, eax
					//还原 
					pop edi
					pop esi
					pop esp
					pop ebx
					pop edx
					pop ecx
					pop eax

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
			//正常角色名字颜色地方
			objNormal.Hook((PVOID)0x00453485, Naked_Normal, 6);
			//组队
			ObjParty.Hook((PVOID)0x00453506, Naked_Party, 5);

			::CreateThread(0, 0, ThreadProc1, 0, 0, 0);
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
			case PacketOrder::VipColor:
				namecolor::fun_getvipcolor(pCmd);
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
			objPacket.Hook((PVOID)0x005f3a13, Naked1, 5);  
		}
	}

	namespace Hide_pet
	{
		CMyInlineHook objPet, objWing;

		DWORD dwCallPet = 0x005876A0;
		__declspec(naked) void Naked_HidePet()
		{
			_asm
			{

				lea eax, g_IsShowPet
				cmp byte ptr[eax], 0x0
				jne __orginal
				xor eax, eax
				add esp, 0x4
				jmp objPet.m_pRet
				__orginal :
				call  dwCallPet
					jmp objPet.m_pRet
			}
		}

		__declspec(naked) void Naked_HideWing()
		{
			_asm
			{

				lea eax, g_IsShowWing
				cmp byte ptr[eax], 0x0
				jne __orginal
				xor eax, eax
				add esp, 0x4
				jmp objWing.m_pRet
				__orginal :
				call  dwCallPet
					jmp objWing.m_pRet
			}
		}


		void Start()
		{
			objPet.Hook((PVOID)0x0043950b, Naked_HidePet, 5);
			objWing.Hook(PVOID(0x0043b1e6), Naked_HideWing, 5);
		}
	}


	namespace chat_command
	{
		CMyInlineHook g_obj1;
		void  __stdcall fun_chat_command(char* strChat)
		{
			char* strInput = new char[MAX_PATH];
			char* strCommand = new char[MAX_PATH];
			char* strOption = new char[MAX_PATH];
			do
			{
				strcpy_s(strInput, MAX_PATH, strChat);
				sscanf_s(strInput, "%s %s", strCommand, MAX_PATH, strOption, MAX_PATH);
				if (strCommand[0] == '\0' || strOption[0] == '\0')
					break;

				if (strcmp("/pet", strCommand) == 0 &&
					strcmp("on", strOption) == 0) {
					g_IsShowPet = true;
					break;
				}

				if (strcmp("/pet", strCommand) == 0 &&
					strcmp("off", strOption) == 0) {
					g_IsShowPet = false;
					break;
				}

				if (strcmp("/wing", strCommand) == 0 &&
					strcmp("on", strOption) == 0) {
					g_IsShowWing = true;
					break;
				}

				if (strcmp("/wing", strCommand) == 0 &&
					strcmp("off", strOption) == 0) {
					g_IsShowWing = false;
					break;
				}


			} while (0);
			delete[] strInput;
			delete[] strCommand;
			delete[] strOption;

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
				call g_obj1.m_pfun_call
				popfd
				popad
				jmp g_obj1.m_pRet

			}
		}
		void Start()
		{
			g_obj1.Hook((PVOID)0x004863c2, Naked_1, 5, fun_chat_command);
		}
	}

	namespace common
	{

		void Start()
		{
			custompacket::Start();
			namecolor::Start();
			chat_command::Start();
			Hide_pet::Start();

		}

	}
}