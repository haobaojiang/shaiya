#include <MyClass.h>
#include <windows.h>
#include <MyInlineHook.h>
#include <define.h>
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")


namespace Edmel
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


		void Start()
		{
			obj1.Hook((PVOID)0x004a4d3a, Naked_1, 11);
		}
	}


	namespace common
	{
		CMyInlineHook g_objMobColor;

	
		void Start()
		{
			weapon_appreance::Start();
		}

	}
}