#include <MyClass.h>
#include <windows.h>
#include <MyInlineHook.h>

#include <time.h>

namespace Ep3
{
	bool g_bEnable = false;


	namespace customclock
	{
		INT64 g_nTimeLeft = 0;  //时间差

		bool g_bLoaded = false;
		CMyInlineHook objClock, objClock1, objClock2;

		BYTE  g_AyImage[100] = { 0 };
		void __stdcall fun_showImage(DWORD x, DWORD  y);
		PVOID g_pfunShowImage = fun_showImage;


		bool load_img()
		{
			//1.clock
			ZeroMemory(g_AyImage, sizeof(g_AyImage));

			//1.1载入图像
			char* strPath = "data/interface";
			char* strFileName = "main_servertime.tga";



			 PVOID pLoadImage = (PVOID)0x005248D0;
			PVOID pAyImage = g_AyImage;
			_asm
			{
				push 0x40
					push 0x100
					push strFileName
					push strPath
					mov ecx, pAyImage
					call pLoadImage
					mov g_bLoaded, al
			}
			return g_bLoaded;
		}

		void __stdcall fun_showImage(DWORD x, DWORD  y)
		{

			if (!g_bEnable)
				return;
			if (!g_bLoaded)
			{
				if (!load_img())
					return;
			}



			x += 0xa;  //5
			y += 0x95; //95
			//现图像
			PVOID pTemp = (PVOID)0x00524BA0;
			PVOID pAyImage = g_AyImage;
			_asm
			{
				push y
					push x
					mov ecx, pAyImage
					call pTemp
			}


			//现文字
			SYSTEMTIME stcTemp = { 0 };
			FILETIME ft = { 0 };
			ULARGE_INTEGER ui;
			GetLocalTime(&stcTemp);
			SystemTimeToFileTime(&stcTemp, &ft);
			ui.LowPart = ft.dwLowDateTime;
			ui.HighPart = ft.dwHighDateTime;
			ui.QuadPart -= g_nTimeLeft;
			ft.dwLowDateTime = ui.LowPart;
			ft.dwHighDateTime = ui.HighPart;
			FileTimeToSystemTime(&ft, &stcTemp);
			char nowtime[26];
			sprintf_s(nowtime, 26, "%04d-%02d-%02d %02d:%02d:%02d", stcTemp.wYear, stcTemp.wMonth, stcTemp.wDay, stcTemp.wHour, stcTemp.wMinute, stcTemp.wSecond);
			x += 0x1b;
			y += 0xa;

			pTemp = (PVOID)0x0051C360;
			DWORD dwTemp = (DWORD)nowtime;
			_asm
			{
				push dwTemp
					push 0x0
					push 0xFF
					push 0xFF
					push 0xFF
					push y
					push x
					push 0x21D9838
					call pTemp
					add esp, 0x20
			}
		}

		void fun_clock(Pcmd_clock pClock)
		{
			FILETIME svft = { 0 };
			SystemTimeToFileTime(&pClock->stcTime, &svft);
			FILETIME cft = { 0 };
			SYSTEMTIME lt = { 0 };
			GetLocalTime(&lt);
			SystemTimeToFileTime(&lt, &cft);
			ULARGE_INTEGER uiSv, uiCl;
			uiSv.LowPart = svft.dwLowDateTime;
			uiSv.HighPart = svft.dwHighDateTime;
			uiCl.LowPart = cft.dwLowDateTime;
			uiCl.HighPart = cft.dwHighDateTime;
			g_nTimeLeft = uiCl.QuadPart - uiSv.QuadPart;
			g_bEnable = true;
		}

		__declspec(naked) void Naked()
		{
			_asm
			{
				pushad
					mov eax, [esi + 0x8]
					push eax
					mov eax, [esi + 0x4]
					push eax
					call g_pfunShowImage
					popad
					fld dword ptr[ebp + 0x24]
					mov edx, [ebp + 0x1C]
					jmp objClock1.m_pRet

			}
		}


		void Start()
		{
			//1.3 图像呈现
           objClock1.Hook((PVOID)0x004B0503, Naked, 6);
		}
	}
	namespace custompacket
	{
		CMyInlineHook objClock;
		void __stdcall CustomPacket(PWORD);
		PVOID g_pCustomPacket = CustomPacket;

		void __stdcall CustomPacket(PWORD pCmd)
		{
			switch (*pCmd)
			{
				//总之只有接到时钟才算
			case PacketOrder::Clock:
				customclock::fun_clock((Pcmd_clock)++pCmd);
				break;
	//		case PacketOrder::VipColor:
	//			namecolor::fun_getvipcolor((Pcmd_color)++pCmd);
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
					call g_pCustomPacket
					popad
				_orginal :
				sub eax, 0x0000FA08
					jmp objClock.m_pRet

			}
		}

		void Start()
		{
			objClock.Hook((PVOID)0x0057632f, Naked1, 5);
		}
	}


	namespace common
	{
		void Start()
		{
			Sleep(5000);
	//		Whisper::Start();
			custompacket::Start();
			customclock::Start();
	//		namecolor::Start();
		}
	}

}