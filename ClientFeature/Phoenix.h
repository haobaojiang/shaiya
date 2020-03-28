#include <MyClass.h>
#include <windows.h>
#include <MyInlineHook.h>
#include <define.h>
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")


namespace Phoenix
{
	namespace common{
		void SubStart();
		void Start();
	}

	namespace Hide_pet
	{
		PVOID g_pHidePetReturnAddr = NULL;
		PVOID g_pfunHidePet = NULL;
		bool  g_bIsHidePet = false;
		PVOID g_pHidePetReturnAddr1 = NULL;

		__declspec(naked) void Naked_HidePet_Signature()
		{
			_asm
			{
				mov cl, byte ptr[esp + 0x2008]
					cmp cl, 0x78                         //"clap"
					je __begin
					jmp g_pHidePetReturnAddr
				__begin :


				cmp g_bIsHidePet, 0x1
					je __disable
					mov g_bIsHidePet, 0x1
					jmp g_pHidePetReturnAddr
				__disable :
				mov g_bIsHidePet, 0x0
					jmp g_pHidePetReturnAddr
			}
		}

		__declspec(naked) void Naked_HidePet_Action()
		{
			_asm
			{
				cmp g_bIsHidePet, 0x1
					je __disable

					mov eax, dword ptr[ebx + 0x42C]
					jmp g_pHidePetReturnAddr1

				__disable :
				mov eax, 0x0
					jmp g_pHidePetReturnAddr1
			}
		}

		void Start()
		{
			CMyInlineHook obj1;
			g_pHidePetReturnAddr = (PVOID)obj1.Hook((PVOID)0x005ce918, Naked_HidePet_Signature, 7);


			CMyInlineHook obj2;
			g_pHidePetReturnAddr1 = (PVOID)obj2.Hook((PVOID)0x00416766, Naked_HidePet_Action, 6);

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
		DWORD g_delayTime = 0;


		CMyInlineHook obj1, obj2;
		__declspec(naked) void Naked_1()
		{
			_asm
			{
				push ebx
					mov ebx, g_dwEnchant
					movzx eax, word ptr[ebx + eax * 2]
					pop ebx
					jmp obj1.m_pRet
			}
		}




		DWORD dwFailEnchant = 0x005CEDdf;
		__declspec(naked) void Naked_2()
		{
			_asm
			{
				mov byte ptr ss : [esp + 0x11], dl
				mov dword ptr ss : [esp + 0x12], eax
				cmp g_delayTime,0x0
				je _orginal

				jmp dwFailEnchant

				_orginal:
				mov g_delayTime,0x64
				jmp obj2.m_pRet
			}
		}

		DWORD WINAPI DecreaseDelay(_In_ LPVOID lpParameter)
		{
			while (1)
			{
				if (g_delayTime)
					g_delayTime -= 1;
				Sleep(200);
			}
		}

		void Start()
		{
			//1.从文件中读取数植
			WCHAR szTemp[MAX_PATH] = { 0 };
			GetModuleFileName(GetModuleHandle(0), szTemp, MAX_PATH);
			PathRemoveFileSpec(szTemp);
			wcscat_s(szTemp, MAX_PATH, L"\\ItemEnchant.ini");


			for (WORD i = 0; i < _countof(g_stcEnchantAddValue); i++)
			{
				WCHAR szWeapon[MAX_PATH];
				swprintf_s(szWeapon, MAX_PATH, L"WeaponStep%02d", i);
				g_stcEnchantAddValue[i].wWeaponValue = GetPrivateProfileInt(L"LapisianEnchantAddValue", szWeapon, 0, szTemp);
			}

			obj1.Hook((PVOID)0x004abc3f, Naked_1, 17);
			obj2.Hook(PVOID(0x005cedd2), Naked_2, 6);
			::CreateThread(0, 0, DecreaseDelay, 0, 0, 0);

		}
	}
	namespace skill_potion_delay
	{
		CMyInlineHook obj1;
		DWORD dwTemp = 0x005CD790, dwTemp2 = 0x004EECD4;
		__declspec(naked) void Naked_1()
		{
			_asm
			{
				cmp byte ptr[eax + 0x2a], 0x0         // "special" field of item
					jne __orginal
				sub esp, 0x28
					pushad
					mov eax, dword ptr ss : [esp + 0x74]
					mov word ptr ss : [esp + 0x20], 0x50A
					mov byte ptr ss : [esp + 0x22], bl      //bag
		//			movzx eax,dword ptr  [esp+0x74]
					mov byte ptr ss : [esp + 0x23], al      //slot
					mov byte ptr ss : [esp + 0x24], 0x0
					push 0x4
					lea eax, dword ptr ss : [esp + 0x24]
					push eax
					call dwTemp
					add esp, 0x8
					popad
					add esp, 0x28
					add esp, 0x8
					jmp dwTemp2

				__orginal:
				mov esi,eax
					cmp byte ptr[esi+0x8],0x2c
					jmp obj1.m_pRet

			}
		}
		void Start()
		{
			{
				BYTE byTemp = 0xeb;
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00443e84, &byTemp, 1);
			}

			{
				BYTE byTemp[] = { 0x90,0x90 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x005ceb7c, byTemp, 2);
			}

			{
				BYTE byTemp[] = { 0x90,0x90 ,0x90,0x90,0x90 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00443FB0, byTemp, sizeof(byTemp));
			}


	/*		obj1.Hook((PVOID)0x004EF39B, Naked_1, 6);*/
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
	//				xor dwRet, 0x18          //改这个就行了
	                xor dwRet, 0x55          //[ADM]Asus
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
			MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x72DFDC, strHeader, strlen(strHeader));
			obj1.Hook((PVOID)0x0040DB0D, Naked_1, 7);
		}
	}

	namespace namecolor
	{
		bool g_bEnable = false;
		CMyInlineHook  objNormal, ObjParty, objEnemy;
		CRITICAL_SECTION g_cs;
		cmd_color g_stcColor[200];

		DWORD g_dwRandomSign = 0xffff7fff;  //如果服务器传来的是这个颜色值,则说明是随机颜色
		DWORD g_dwRandCoor = 0xffffffff;

		DWORD __stdcall ChangeColor(DWORD,DWORD);
		PVOID g_pfunChangeColor = ChangeColor;

		DWORD __stdcall ChangeColor(DWORD dwPlayer,DWORD dwOldColor)
		{
			DWORD dwCharid = *PDWORD(dwPlayer + 0x34);
			DWORD dwRet = dwOldColor;
/*			EnterCriticalSection(&g_cs);*/
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
/*			LeaveCriticalSection(&g_cs);*/
			return dwRet;
		}


		//此函数一直不用变
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
			InitializeCriticalSection(&g_cs);
			//正常角色名字颜色地方
			objNormal.Hook((PVOID)0x0044be75, Naked_Normal, 6);
			//组队
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
			common::SubStart();
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
			objPacket.Hook((PVOID)0x005D642C, Naked1, 5);  
		}
	}


	namespace mobColor {

		CMyInlineHook g_objMobColor;

		DWORD __stdcall getMobColor(int level) {
			if (level >= 10) {
				return 0xFF808080;
			}
			if (level >= 8) {
				return 0xFFFF00FF;
			}
			if (level >= 6) {
				return 0xFFFF0000;
			}
			if (level >= 4) {
				return 0xFFFF8000;
			}
			if (level >= 2) {
				return 0xFFFFFF00;
			}
			if (level >= -1) {
				return 0xFF00FF00;
			}
			if (level >= -3) {
				return 0xFF0000FF;
			}
			if (level >= -5) {
				return 0xFF80FFFF;
			}
			return 0xffffffff;

		}

		__declspec(naked) void Naked_mobColor()
		{
			_asm
			{
				push ebx
				push ecx
				push edx
				push esi
				push edi
				push ebp
				push esp

				push eax
				call g_objMobColor.m_pfun_call
				
				pop esp
				pop ebp
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx
				
				retn 0x4

			}
		}


		void Start() {
			g_objMobColor.Hook(PVOID(0x004d5d0b), Naked_mobColor, 6, getMobColor);
		}

	}

	namespace common
	{
		CMyInlineHook g_objMobColor;

		void SubStart()
		{
			static bool bInit = false;
			if (bInit)
				return;

			bInit = true;
		}

		void Start()
		{
			//4. enable /party
			{
				BYTE byCodes[] = { 0x5 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0047c386, byCodes, sizeof(byCodes));
			}
			//5. weapon appreance 还原
			{
				BYTE byCodes[] = { 0x8B, 0x74, 0x24, 0x14, 0x0F, 0xB7, 0x4E, 0x2E, 0x50, 0x0F, 0xB7, 0x46, 0x0C, 0x50, 0x51, 0xB9, 0xC0, 0x2B, 0x90, 0x00, 0xE8, 0x90, 0xC3, 0x02, 0x00 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004abc37, byCodes, sizeof(byCodes));
			}
			//6. 左下角的框一直显示
			{
				BYTE byCodes[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0047A6E9, byCodes, sizeof(byCodes));
			}
			//延迟问题
			{
				BYTE byCodes[] = { 0x90, 0x90, 0x90, 0x90, 0x90 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00443FB0, byCodes, sizeof(byCodes));

			}
			custompacket::Start();
			Hide_pet::Start();
			weapon_appreance::Start();
		//	skill_potion_delay::Start();
			namecolor::Start();
			mobColor::Start();
	


		}

	}
}