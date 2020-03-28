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


		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046ccc1, &enchantLevel, 1);//�����ط�Ҫ�޸�����
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004d28d2, &enchantLevel, 1);

		//3.���ļ��ж�ȡ��ֲ
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

		DWORD dwAddrOfRate = (DWORD)g_stcEnchantRate;     //ȡһ���׵�ַ
		DWORD dwAddrOfValue = (DWORD)g_stcEnchantAddValue;//ȡһ���׵�ַ

														  //4.���ڴ����޸�ƫ��
		DWORD dwValueAddr[] = { 0x004d28fa, 0x0046cd87, 0x0046cda3, 0x004d28e0, };                    //�޸�4��

		for (DWORD i = 0; i < _countof(dwValueAddr); i++)
		{
			MyWriteProcessMemory(GetCurrentProcess(), (PVOID)dwValueAddr[i], &dwAddrOfValue, 4);//�޸�ÿ�׶���ֵ
		}

		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046cd3b, &dwAddrOfRate, 4);         //�޸Ļ���
	

	}
}





