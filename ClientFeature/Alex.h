#include <MyClass.h>
#include <windows.h>
#include <MyInlineHook.h>
//#include <VMProtectSDK.h>

#include <time.h>


//alex��
namespace Alex
{
	

	namespace namecolor
	{
		bool g_bEnable = false;
		CMyInlineHook  objNormal, ObjParty, objEnemy;
		CRITICAL_SECTION g_cs;
		cmd_color g_stcColor[200];


		DWORD g_dwRandomSign = 0xffff7fff;  //����������������������ɫֵ,��˵���������ɫ
		DWORD g_dwRandCoor = 0xffffffff;

		void __stdcall ChangeColor(DWORD, PDWORD);
		PVOID g_pfunChangeColor = ChangeColor;

		void __stdcall ChangeColor(DWORD dwPlayer,PDWORD pColor)
		{
			DWORD dwCharid = *PDWORD(dwPlayer + 0x34);
			EnterCriticalSection(&g_cs);
			for (DWORD i = 0; i < _countof(g_stcColor); i++)
			{
				if (!g_stcColor[i].dwCharid)
					break;


				//�ж��Ƿ������ɫ
				if (g_stcColor[i].dwCharid == dwCharid)
				{
					if (g_stcColor[i].dwRGB == g_dwRandomSign)
						*pColor = g_dwRandCoor;
					else
						*pColor = g_stcColor[i].dwRGB;
					break;
				}

			}
			LeaveCriticalSection(&g_cs);
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
			mov dword ptr  [esp + 0x14],0xffffffff
			   pushad
			   pushfd            //ֻ��4�ֽڴ�С
			   lea eax, dword ptr [esp+0x38]
			   push eax //color
			   push edi
			   call g_pfunChangeColor
			   popfd
			   popad
			
			   jmp objNormal.m_pRet
			}
		}
		__declspec(naked) void Naked_Party()
		{

			_asm
			{
				mov dword ptr [esp + 0x14], 0xFF00FF00
				pushad
				lea eax, [esp + 0x34]
				push eax //color
					push edi
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
			objNormal.Hook((PVOID)0x0044B9AF, Naked_Normal, 8);//0044B9AF   .  C74424 14 FFFFFFFF             mov dword ptr ss:[esp+0x14],-0x1
			ObjParty.Hook((PVOID)0x0044BA4A, Naked_Party, 8);

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
				//��ֻ֮�нӵ�ʱ�Ӳ���
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
			//�Զ���������ĵط�
			objPacket.Hook((PVOID)0x00584903, Naked1, 5);
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



//7.0
/*
namespace Shaiya50
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
			EnterCriticalSection(&g_cs);
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
			LeaveCriticalSection(&g_cs);
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
			//������ɫ������ɫ�ط�
			objNormal.Hook((PVOID)0x0044c586, Naked_Normal, 11);
			//���
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
				//��ֻ֮�нӵ�ʱ�Ӳ���
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
			//�Զ���������ĵط�
			objPacket.Hook((PVOID)0x0058af3f, Naked1, 5);
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


*/


//regvn�� 5.0
/*

namespace Shaiya50
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
			EnterCriticalSection(&g_cs);
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
			LeaveCriticalSection(&g_cs);
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


		//����ʱһ��ֻ��Ҫ�ҳ���ַ����
		void Start()
		{
			InitializeCriticalSection(&g_cs);
			//������ɫ������ɫ�ط�
			objNormal.Hook((PVOID)0x0044bb76, Naked_Normal, 11);
			//���
			ObjParty.Hook((PVOID)0x0044bc14, Naked_Party, 8);
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
				//��ֻ֮�нӵ�ʱ�Ӳ���
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
			//�Զ���������ĵط�
			objPacket.Hook((PVOID)0x00585ea1, Naked1, 5);
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

*/