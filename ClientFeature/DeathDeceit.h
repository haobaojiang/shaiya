#include <windows.h>
#include <MyInlineHook.h>
//#include <VMProtectSDK.h>
#include <MyClass.h>
#include <time.h>


namespace DeathDeceit
{
	namespace namecolor
	{
		bool g_bEnable = false;
		CMyInlineHook  objNormal, ObjParty, objEnemy;
		CRITICAL_SECTION g_cs;
		cmd_color g_stcColor[200];


		DWORD g_dwRandomSign = 0xffff7fff;  //如果服务器传来的是这个颜色值,则说明是随机颜色
		DWORD g_dwRandCoor = 0xffffffff;

		DWORD __stdcall ChangeColor(DWORD, DWORD);
		PVOID g_pfunChangeColor = ChangeColor;

		DWORD __stdcall ChangeColor(DWORD dwPlayer,DWORD oldColor)
		{
			DWORD color = oldColor;
			DWORD dwCharid = *PDWORD(dwPlayer + 0x34);
			for (DWORD i = 0; i < _countof(g_stcColor); i++)
			{
				if (!g_stcColor[i].dwCharid)
					break;

				//判断是否随机颜色
				if (g_stcColor[i].dwCharid == dwCharid)
				{
					if (g_stcColor[i].dwRGB == g_dwRandomSign)
						color = g_dwRandCoor;
					else
						color = g_stcColor[i].dwRGB;
					break;
				}

			}
			return color;
		}


		void fun_getvipcolor(Pcmd_color pColor)
		{
			EnterCriticalSection(&g_cs);
			ZeroMemory(g_stcColor, sizeof(g_stcColor));
			for (DWORD i = 0; i < _countof(g_stcColor); i++)
			{
				if (!pColor[i].dwCharid)
					break;

				g_stcColor[i].dwCharid = pColor[i].dwCharid;
				g_stcColor[i].dwRGB    = pColor[i].dwRGB;
			}
			LeaveCriticalSection(&g_cs);
		}

		__declspec(naked) void Naked_Normal()
		{
			_asm
			{
				or ebp,0xffffffff

				push eax
				push ebx
				push ecx
				push edx
				push edi
				push esi
				push esp

				push ebp
				push esi
				call g_pfunChangeColor
				mov ebp, eax

				pop esp
				pop esi
				pop edi
				pop edx
				pop ecx
				pop ebx
				pop eax

			    sub eax,0x4   //orgianl

				jmp objNormal.m_pRet
			}
		}
		__declspec(naked) void Naked_Party()
		{

			_asm
			{
				mov ebp, 0xFF00FF00

				push eax
				push ebx
				push ecx
				push edx
				push edi
				push esi
				push esp

				push ebp //color
				push esi //player
				call g_pfunChangeColor
				mov ebp,eax

				pop esp
				pop esi
				pop edi
				pop edx
				pop ecx
				pop ebx
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
			InitializeCriticalSection(&g_cs);
			objNormal.Hook((PVOID)0x0044be75, Naked_Normal, 6); 
			ObjParty.Hook((PVOID)0x0044bef6, Naked_Party, 5);

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
			objPacket.Hook((PVOID)0x005d642c, Naked1, 5);
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


