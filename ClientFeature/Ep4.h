#include <MyClass.h>
#include <windows.h>
#include <MyInlineHook.h>

#include <time.h>

namespace Ep4
{
	bool g_bEnable = false;
	namespace Whisper
	{
		CMyInlineHook obj1, obj2, obj3, obj4, obj5, obj6;
		PVOID g_pChat = NULL;
		void RightClick();
		PVOID g_pfunRightClick = RightClick;

#pragma pack(1)
		typedef struct _ChatText
		{
			_ChatText* pstrNext;
			_ChatText* pstrBefore;
			PCHAR pstrText;
		}ChatText, *PChatText;
#pragma pack()

		void AddPlayerToWhisper(char* strName)
		{
			DWORD dwEcx = *PDWORD(0x705060);
			if (!dwEcx)
				return;

			dwEcx = *PDWORD(dwEcx);
			if (!dwEcx)
				return;

			dwEcx = *PDWORD(dwEcx + 0x184);


			DWORD dwTemp = (DWORD)strName;
			PVOID pTemp = (PVOID)0x00470490;
			_asm
			{
				push dwTemp
				mov ecx,dwEcx
				call pTemp
			}
		}


		void RightClick()
		{

			if (!g_pChat)
				return;

			//是否移动到了聊天窗口
			if (!*PBYTE(DWORD(g_pChat) + 0x3c8))
				return;

			int x = *PINT(0x00704354);

			if (x > 50)               //尽量要靠左点角色名
				return;

			int y = *PINT(0x00704358);
			y -= 0x10;  //似乎坐标不对,所以减个0x10;
			y -= 0x3e;
			y -= ((*PBYTE((DWORD)g_pChat + 0x3ac) + 2) * 0x10);


			DWORD dwRet = 0;
			_asm
			{
				mov esi, g_pChat

				mov eax, 0x0
					movzx edi, byte ptr ds : [esi + 0x3AC]
					add eax, 0x4
					add edi, 0x2
					shl eax, 0x4
					mov ecx, dword ptr ds : [0x6F1560]

					shl edi, 0x4
					sub ecx, eax
					sub ecx, edi
					add ecx, -0x5C
					mov dwRet, ecx
			}
			y -= dwRet;

			y /= 16;
			int nTargetLine = /**PBYTE((DWORD)g_pChat + 0x3c4)*/ 0- y;
			if (nTargetLine < 0)
				return;

			//log
			char szTemp[MAX_PATH] = { 0 };
			sprintf_s(szTemp, "line:%d offset:%d\n", nTargetLine,dwRet);
			OutputDebugStringA(szTemp);
			//log

			
			PChatText pChatText = PChatText(*PDWORD(*PDWORD(0x7013D8) + 0xc + 4));
			pChatText=pChatText->pstrNext;
			PChatText pTemp = pChatText;

			int nLine = 0;
			while (pChatText->pstrNext != pTemp&&
				nTargetLine >= nLine)
			{
				if (pChatText->pstrText != NULL)
				{
					if ((pChatText->pstrText)[0] != '\0'&&nLine == nTargetLine)
					{
						//获取角色名
						char* strFind = strstr(pChatText->pstrText, ":");
						if (strFind != NULL)
						{
							char strName[MAX_PATH] = { 0 };
							memcpy(strName, pChatText->pstrText, strFind- pChatText->pstrText);

							strFind = strstr(strName, "]");
							//添加至框里
							if (strFind != NULL)
							{
								AddPlayerToWhisper(strFind+1);
							}
							else
							{
								AddPlayerToWhisper(strName);
							}
						}
						break;
					}
				}
				nLine++;
				pChatText = pChatText->pstrNext;
			}
		}


		__declspec(naked) void Naked_GetPchat()
		{
			_asm
			{
				mov g_pChat, esi
					mov dword ptr[esi], 0x6A6B48
					jmp obj1.m_pRet
			}
		}


		__declspec(naked) void Naked_RightClick()
		{
			_asm
			{
				mov dword ptr ds : [esi + 0x20], 0x1
					pushad
					call g_pfunRightClick
					popad
					jmp obj2.m_pRet
			}
		}



		void Start()
		{
			obj1.Hook((PVOID)0x00423e24, Naked_GetPchat, 6);
			obj2.Hook((PVOID)0x0051e7e9, Naked_RightClick, 7);
		}
	}

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
			PVOID pLoadImage = (PVOID)0x00536790; //ep8 :0059D480  /$  81EC 2C010000 sub esp,0x12C

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
			PVOID pTemp = (PVOID)0x00536A60;  //0059D780  /$  8B11          mov edx,dword ptr ds:[ecx]

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
			pTemp = (PVOID)0x0052ED10;
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
					push 0x21EFE48
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

		__declspec(naked) void Naked2()
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
			objClock1.Hook((PVOID)0x004bef37, Naked2, 6);
		}
	}
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
				mov dword ptr ss : [esp + 0x14], 0xffffffff
					add eax,0x4
					pushad
					lea eax, [esp + 0x34]
					push eax           //color
					push edi          //player
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
				mov dword ptr ss : [esp + 0x14], 0xFF00FF00
					pushad
					lea eax, [esp + 0x34]
					push eax //color
					push edi //player
					call g_pfunChangeColor
					popad
					jmp ObjParty.m_pRet
			}
		}

		void Start()
		{
			InitializeCriticalSection(&g_cs);
			//正常角色名字颜色地方
			objNormal.Hook((PVOID)0x0044b9af, Naked_Normal, 8);
			//组队
			ObjParty.Hook((PVOID)0x0044ba4a, Naked_Party, 8);
		}
	}

	namespace showcount
	{
		CMyInlineHook obj;
		void __stdcall fun(char*,char*,DWORD,DWORD);
		PVOID pfun = fun;
		void __stdcall fun(char* strOutBuff,char* strItemName,DWORD dwBag,DWORD dwSlot)
		{
			DWORD dwCount = *PBYTE((dwBag + dwSlot * 24) * 34 + 0x84e6fb);
		//	sprintf(strOutBuff, "%s\n[Count]:%d", strItemName,dwCount );
		}

		__declspec(naked) void Naked()
		{
			_asm
			{
				movzx edi, byte ptr [esp+0x934]  //slot
				movzx edx, byte ptr [esp+0x930]  
				pushad
				push edx//slot
				push edi//bag
				push eax                        //item_name
				push ecx                        //buff
				call pfun
				popad
				jmp obj.m_pRet
			}
		}
		
		void Start()
		{
			obj.Hook((PVOID)0x004a27bf, Naked, 15);
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
					call g_pCustomPacket
					popad
				_orginal :
				sub eax, 0x0000FA08
					jmp objClock.m_pRet

			}
		}

		void Start()
		{
			objClock.Hook((PVOID)0x00584903, Naked1, 5);
		}
	}


	namespace common
	{
		void Start()
		{
			Sleep(5000);
			Whisper::Start();
			custompacket::Start();
			customclock::Start();
			namecolor::Start();
			showcount::Start();
		}
	}

}