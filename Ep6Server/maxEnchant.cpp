#include "stdafx.h"
#include "maxEnchant.h"





namespace maxEnchant
{
	typedef struct _EnchantAddValue
	{
		WORD wWeaponValue;
		WORD wArrmorValue;
	}EnchantAddValue, *PEnchantAddValue;

	typedef struct _EnchantRate
	{
		DWORD dwWeaponValue;
		DWORD dwArrmorValue;
	}EnchantRate, *PEnchantRate;


	EnchantAddValue g_stcEnchantAddValue[50] = { 0 };
	EnchantRate     g_stcEnchantRate[50] = { 0 };


	void start()
	{
		BYTE enchantLevel = GetPrivateProfileInt(L"MaxEnchant",L"level",20, g_szFilePath);
		if (enchantLevel == 20){ 
			return;
		}

		OutputDebugStringA("MaxEnchant");


		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046ccc1, &enchantLevel, 1);//两处地方要修改上限
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004d28d2, &enchantLevel, 1);

		//3.从文件中读取数植
		WCHAR* szFilePath = L"Data\\ItemEnchant.ini";

		for (WORD i = 0; i < _countof(g_stcEnchantAddValue); i++){
			WCHAR szWeapon[50] = { 0 };
			WCHAR szDefense[50] = { 0 };
			swprintf(szWeapon, L"WeaponStep%02d", i);
			swprintf(szDefense, L"DefenseStep%02d", i);
			g_stcEnchantRate[i].dwWeaponValue = GetPrivateProfileInt(L"LapisianEnchantPercentRate", szWeapon, 0, szFilePath);
			g_stcEnchantRate[i].dwArrmorValue = GetPrivateProfileInt(L"LapisianEnchantPercentRate", szDefense, 0, szFilePath);
			g_stcEnchantAddValue[i].wWeaponValue = GetPrivateProfileInt(L"LapisianEnchantAddValue", szWeapon, 0, szFilePath);
			g_stcEnchantAddValue[i].wArrmorValue = GetPrivateProfileInt(L"LapisianEnchantAddValue", szDefense, 0, szFilePath);
		}

		DWORD dwAddrOfRate = (DWORD)g_stcEnchantRate;     //取一下首地址
		DWORD dwAddrOfValue = (DWORD)g_stcEnchantAddValue;//取一下首地址

														  //4.往内存里修改偏移
		DWORD dwValueAddr[] = { 0x004d28fa, 0x0046cd87, 0x0046cda3, 0x004d28e0, };                    //修改4处

		for (DWORD i = 0; i < _countof(dwValueAddr); i++)
		{
			MyWriteProcessMemory(GetCurrentProcess(), (PVOID)dwValueAddr[i], &dwAddrOfValue, 4);//修改每阶段数值
		}

		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046cd3b, &dwAddrOfRate, 4);         //修改机率
	

	}
}





