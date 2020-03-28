#include <MyClass.h>
#include <windows.h>
#include <MyInlineHook.h>


#define PSAPI_VERSION 1




namespace KEFARE
{

	namespace weapon_appreance
	{
#define MAXENCHANT 49
		typedef struct _EnchantAddValue
		{
			WORD wWeaponValue;
		}EnchantAddValue, *PEnchantAddValue;


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
				mov eax,[esp+0xc]
				push ebx
				mov ebx, g_dwEnchant
				movzx eax, word ptr ds : [ebx + eax * 2]  //eax为阶段
				pop ebx
				retn 0xc
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


// 			for (WORD i = 0; i < _countof(g_stcEnchantAddValue); i++)
// 			{
// 				WCHAR szWeapon[MAX_PATH];
// 				swprintf_s(szWeapon, MAX_PATH, L"WeaponStep%02d", i);
// 				g_stcEnchantAddValue[i].wWeaponValue = GetPrivateProfileInt(L"LapisianEnchantAddValue", szWeapon, 0, szTemp);
// 			}

			obj1.Hook((PVOID)0x004d0ab0, Naked_1, 17);
		}
	}



	namespace common
	{
		void Start()
		{
			weapon_appreance::Start();

		}
	}





}