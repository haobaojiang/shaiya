// Win32Project4.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <vector>
#include "FileReadWrite.h"

using std::vector;

#define MAXENCHANT 49
typedef struct _EnchantAddValue
{
	WORD wWeaponValue;
//	WORD wArrmorValue;
}EnchantAddValue, *PEnchantAddValue;
EnchantAddValue g_stcEnchantAddValue[MAXENCHANT + 1];



int _tmain(int argc, _TCHAR* argv[])
{
	//1.���ļ��ж�ȡ��ֲ
	WCHAR szTemp[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szTemp);   
	CString szFilePath = szTemp;
	szFilePath = szFilePath + L"\\ItemEnchant.ini";

	CString szWeapon;
	CString szDefense;
	for (WORD i = 0; i < _countof(g_stcEnchantAddValue); i++)
	{
		szWeapon.Format(L"WeaponStep%02d", i);
		szDefense.Format(L"DefenseStep%02d", i);
		g_stcEnchantAddValue[i].wWeaponValue = GetPrivateProfileInt(L"LapisianEnchantAddValue", szWeapon, 0, szFilePath);
//		g_stcEnchantAddValue[i].wArrmorValue = GetPrivateProfileInt(L"LapisianEnchantAddValue", szDefense, 0, szFilePath);
		printf("%d\n", g_stcEnchantAddValue[i].wWeaponValue);
	}
	DWORD dwAddrOfValue = (DWORD)g_stcEnchantAddValue;//ȡһ���׵�ַ

	//2.д��exe��
	CFileReadWrite obj;
	obj.Open(L"game.exe");
	obj.SetOffset(0x3c0a00);  //redwhite:0x003c5000 0x00345f00  //other:0x003C3E00   //0x00345f00
	for (DWORD i = 0; i <= (MAXENCHANT); i++)
	{
		obj.WriteData(&g_stcEnchantAddValue[i].wWeaponValue, 2);//д����ֵ
//		obj.WriteData(&g_stcEnchantAddValue[i].wArrmorValue, 2);//д����ֵ
	}
	obj.Close();
	printf("Successful!");
	system("pause");
	return 0;
}

