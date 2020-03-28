#include <MyClass.h>
#include <windows.h>
#include <MyInlineHook.h>
//#include <VMProtectSDK.h>

#include <time.h>


namespace Micus
{
	namespace namecolor
	{
		bool g_bEnable = false;
		CMyInlineHook  objNormal, ObjParty, objEnemy;
		CRITICAL_SECTION g_cs;
		DWORD g_Color[10000];


		DWORD g_dwRandomSign = 0xffff7fff;  //如果服务器传来的是这个颜色值,则说明是随机颜色
		DWORD g_dwRandCoor = 0xffffffff;

		void __stdcall ChangeColor(DWORD, PDWORD);
		PVOID g_pfunChangeColor = ChangeColor;

		void __stdcall ChangeColor(DWORD dwPlayer,PDWORD pColor)
		{
			DWORD dwCharid = *PDWORD(dwPlayer + 0x34);
			DWORD color = g_Color[dwCharid];
			if (color)
			{
				if (color == g_dwRandomSign)
					color = g_dwRandCoor;
				*pColor = color;
			}
		}


		void fun_getvipcolor(Pcmd_color pColor)
		{
			ZeroMemory(g_Color, sizeof(g_Color));
			DWORD i = 0;
			while (pColor[i].dwCharid)
			{
				DWORD charId = pColor[i].dwCharid;
				g_Color[charId] = pColor[i].dwRGB|0xff000000;
				i++;
			}
		}

		__declspec(naked) void Naked_Normal()
		{
			_asm
			{
			mov dword ptr  [esp + 0x18],0xffffffff
			   pushad
			   lea eax, dword ptr [esp+0x38]
			   push eax //color
			   push esi
			   call g_pfunChangeColor
			   popad
		sub eax,0x4
			
			   jmp objNormal.m_pRet
			}
		}
		__declspec(naked) void Naked_Party()
		{

			_asm
			{
				mov dword ptr [esp + 0x18], 0xFF00FF00
				pushad
				lea eax, [esp + 0x38]
				push eax //color
					push esi
					call g_pfunChangeColor
					popad
					jmp ObjParty.m_pRet
			}
		}
		DWORD WINAPI ThreadProc1(_In_  LPVOID lpParameter)
		{
			DWORD dwTemp = 0;
			while (true)
			{
				Sleep(400);
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
			ZeroMemory(g_Color, sizeof(g_Color));
			objNormal.Hook((PVOID)0x0044ba46, Naked_Normal, 11);//0044B9AF   .  C74424 14 FFFFFFFF             mov dword ptr ss:[esp+0x14],-0x1
			ObjParty.Hook((PVOID)0x0044bae4, Naked_Party, 8);

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
			//0059EBF0  |> \2D 08FA0000   sub eax,0xFA08
			objPacket.Hook((PVOID)0X0059EBF0, Naked1, 5);
		}
	}

	namespace common
	{
		void Start()
		{
			namecolor::Start();
			custompacket::Start();
		}
	}

}


