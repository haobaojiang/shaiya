#include <MyClass.h>
#include <windows.h>
#include <MyInlineHook.h>
//#include <VMProtectSDK.h>

#include <time.h>
#include <process.h>


namespace Shaiya70
{
	namespace namecolor
	{
		bool g_bEnable = false;
		CMyInlineHook  objNormal, ObjParty, objEnemy;
		CRITICAL_SECTION g_cs;
		cmd_color g_stcColor[200];

		void __stdcall ChangeColor(DWORD, PDWORD);
		PVOID g_pfunChangeColor = ChangeColor;

		void __stdcall ChangeColor(DWORD dwPlayer, PDWORD pColor)
		{
			DWORD dwCharid = *PDWORD(dwPlayer + 0x34);

			for (DWORD i = 0; i < _countof(g_stcColor); i++)
			{
				if (!g_stcColor[i].dwCharid)
					break;

				if (g_stcColor[i].dwCharid == dwCharid)
				{
					*pColor = g_stcColor[i].dwRGB;
					break;
				}
			}

		}


		void fun_getvipcolor(PVOID pColor)
		{
			memcpy(g_stcColor, PNAMECOLOR(pColor)->colors, sizeof(g_stcColor));
		}

		__declspec(naked) void Naked_Normal()
		{
			_asm
			{
				mov dword ptr ss : [esp + 0x18], 0xffffffff
					pushad
					lea eax, [esp + 0x38]
					push eax //color
					push esi
					call g_pfunChangeColor
					popad
					sub eax, 0x4
					jmp objNormal.m_pRet
			}
		}
		__declspec(naked) void Naked_Party()
		{

			_asm
			{
				mov dword ptr ss : [esp + 0x18], 0xFF00FF00
					pushad
					lea eax, [esp + 0x38]
					push eax //color
					push esi
					call g_pfunChangeColor
					popad
					jmp ObjParty.m_pRet
			}
		}

		void Start()
		{
			InitializeCriticalSection(&g_cs);
			//正常角色名字颜色地方
			objNormal.Hook((PVOID)0x0044c586, Naked_Normal, 11);
			//组队
			ObjParty.Hook((PVOID)0x0044c624, Naked_Party, 8);
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
				namecolor::fun_getvipcolor((Pcmd_color)pCmd);
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
			//自定义封包处理的地方
			objPacket.Hook((PVOID)0x0058fA3F, Naked1, 5);
		}
	}
	namespace fastrr
	{
		DWORD g_time = 0;
		CMyInlineHook g_objEnchant;


		DWORD dwCall = 0x004D8680;
		__declspec(naked) void Naked_enchant()
		{
			_asm {
				cmp g_time, 0
				je __orginal
				mov eax,0x0
				jmp g_objEnchant.m_pRet

				__orginal :
				call dwCall
				mov g_time, 0x3

					jmp g_objEnchant.m_pRet
			}
		}
		void ThreadProc(_In_ LPVOID lpParameter)
		{
			while (1)
			{
				if (g_time) {
					g_time--;
				}
				Sleep(500);
			}
		}
		void Start()
		{
			{
				BYTE temp[] = { 0x83 ,0xC4 ,0x04 ,0x90 ,0x90 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004d926f, temp, sizeof(temp));
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004d9278, temp, sizeof(temp));
			}

			{
				BYTE temp[] = { 0x90 ,0x90 ,0x90 ,0x90 ,0x90};
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004a8db3, temp, sizeof(temp));
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004d8540, temp, sizeof(temp));
			}




			_beginthread(ThreadProc, 0, 0);
			g_objEnchant.Hook(PVOID(0x004d922d), Naked_enchant, 5);
		}
	}



	namespace common
	{
		void Start()
		{
			namecolor::Start();
			custompacket::Start();
			fastrr::Start();
		}
	}

}




