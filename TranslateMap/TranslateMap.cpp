// TranslateMap.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "MyInlineHook.h"
#include "TranslateMap.h"


BOOL TranslateMap(PVOID pPlayer);
PVOID  GetSkillBaseAddr(DWORD dwSkillid, DWORD dwSkillLevel);
void   UseSkill(DWORD dwSkillid, DWORD dwSkillLevel, PVOID pPlayerAddr);
/********************************************复活后是否吃无敌*/
PVOID g_pUseSkill = NULL;               //自己的call
PVOID g_pRespawnReturnAddr = (PVOID)0x00479155;
PVOID g_pRespawnReturnAddr2 = (PVOID)0x00466D4B;



/*传送地图消耗东西*/
PVOID  pTranslateMap   = TranslateMap;
PVOID  pFailedAddr     = (PVOID)0x0047679A;
PVOID  pSuccessAddr    = (PVOID)0x00476798;
PVOID  pDeleteItemCall = (PVOID)0x004728E0;
WORD   wMap            = 106;
DWORD  dwItemid        = 100200;


//PVOID  pFailedAddr = (PVOID)0x0047679A;
//PVOID  pSuccessAddr = (PVOID)0x00476c3e;
//PVOID  pDeleteItemCall = (PVOID)0x004728E0;

/*自动删除NPC*/
typedef struct _POS
{
	float x;
	float y;
	float z;
}POS, *PPOS;
PVOID g_pCreateNpcCall = (PVOID)0x004255D0;
PVOID g_pDeleteNpcCall = (PVOID)0x004257A0;


/*强化上限修改*/

#define MAXENCHANT 49
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

EnchantAddValue g_stcEnchantAddValue[MAXENCHANT+1];
EnchantRate     g_stcEnchantRate[MAXENCHANT+1];

/*属性卷轴修改*/
PVOID pRecRuneSignature   = NULL;
PVOID pRecRuneCustomStats = NULL;
PVOID pReturnAddr1 = (PVOID)0x004d2bd0;
PVOID pReturnAddr2 = (PVOID)0x004d2c24;



//复活保护药水
__declspec(naked) void Naked_RespawnProtected()
{
	_asm
	{
		pushad
			push edi                  //player addr
			push 0x3                  //skill level
			push 0xc7                 //skillid
			call g_pUseSkill
			add esp, 0xc
			popad
			jmp g_pRespawnReturnAddr  //缺少返回
	}

}

__declspec(naked) void Naked_RespawnProtected2()
{
	_asm
	{
		mov dword ptr ss : [ebp + 0x1234], ecx
			pushad
			push edi                  //player addr
			push 0x3                  //skill level
			push 0xc7                 //skillid
			call g_pUseSkill
			add esp, 0xc
			popad
			jmp g_pRespawnReturnAddr2  //缺少返回
	}

}

__declspec(naked) void Naked_RecRunePartOne()
{
	_asm
	{
		lea ebp, dword ptr[esi+0x84]  //orginnal code
		mov ecx,esp
		pushad
		mov edi, [ecx+0x4]
		mov edi, [edi+0x30]
		mov edi, [edi+0xa0]
		push edi               //ItemType
		mov edi, [ecx + 0x54]
		mov edi, [edi + 0x30]  //rec rune 
		push edi
		lea  ecx, [ecx+0xc]
		push ecx               //addr of signature
		call pRecRuneSignature
		add esp,0xc
		popad
		jmp pReturnAddr1
	}
}


__declspec(naked) void Naked_RecRunePartTwo()
{
	_asm
	{
		push ecx
		mov ecx, esp
		pushad
		mov edx, [ecx + 0x58]
		mov edx, [edx + 0x30]
		cmp dword ptr [edx], 0x1874b   //100171
		je _Return
		push edx            //rec rune addr
		push edi            //times
		push ebp            //pbuffer
		call pRecRuneCustomStats
		add esp,0xc
		mov [esp+0x20],eax  //刚好把ecx的给覆盖了
		popad
		pop ecx
	    mov bl,cl
		movzx dx, bl
		mov word ptr [ebp], dx
		jmp pReturnAddr2
_Return:
		popad
		pop ecx
		movzx dx, bl
		mov word ptr[ebp], dx
		jmp pReturnAddr2
	}
}

//备份---原来的是NPC传送
//__declspec(naked) void Naked()
//{
//	_asm
//	{
//		push ebx
//			mov bx, wMap
//			cmp word ptr[eax + 0x00000124], bx   //check is our map
//			pop ebx
//			jne _Right
//			pushad
//			push ebp
//			call pTranslateMap
//			add esp, 0x4
//			cmp al, 0
//			popad
//			je _Wrong
//		_Right :
//		cmp[eax + 0x00000124], cx
//			jmp pSuccessAddr
//		_Wrong :
//		jmp pFailedAddr
//	}
//}



__declspec(naked) void Naked()
{
	_asm
	{
		push ebx
		mov bx, wMap
		cmp cx, bx   //check is our map
		pop ebx
		jne _Right
		pushad
		push ebp
		call pTranslateMap
		add esp,0x4
		cmp al,0
		popad
		je _Wrong
	_Right:
		cmp ecx, 0xC8
		jmp pSuccessAddr 
	_Wrong:
		jmp pFailedAddr
	}
}

BOOL TranslateMap(PVOID pPlayer)
{
	BYTE bySuccess = 0;
	BYTE byMaxBag = *(PBYTE((DWORD)pPlayer + 0x130c));//get max bags
	if (!byMaxBag)  return FALSE;                     //must greater than 0
	PDWORD pBagBase = PDWORD((DWORD)pPlayer + 0x220); //get bagbase at bag1
	for (DWORD i = 1; i <= byMaxBag; i++)
	{
		for (DWORD j = 0; j < 24; j++, pBagBase++)
		{
			if (*pBagBase==NULL) continue;                            //if no item in this slot
			DWORD dwTempItemid = *PDWORD(*(PDWORD(*pBagBase + 0x30)));//get itemid
			
			if (dwTempItemid == dwItemid)
			{
				//delete item
				_asm
				{
					push 0
					push j
					push i
					mov ecx, pPlayer
					call pDeleteItemCall
				}
				return TRUE;                //this will not failed
			}
		}
	}
	return FALSE;
}

void AutoNpc()
{
	//声明与定义一些必备变量
	CEvent objEvent;
	WaitForSingleObject(objEvent, 5000);       //暂停,让程序读完地图与其它内容再说
	BOOL bCreate = FALSE;                      //标识
	/*这些是我们要修改的参数*/
	BYTE byMap = 104;                            //map,这里我们测试的时候记得不要改大,否则出错
	POS stcTempPos = { 1820, 17, 514 };         //坐标
	int  nCreateHours = 19;                    //创建时间
	int  nCreateMinus = 30;
	int  nRemoveHours = 19;                    //删除时间
	int  nRemoveMinus = 45;
	/*************************************************************************/

	PPOS pPos = &stcTempPos;                  //坐标指针
	PDWORD pMapTable = PDWORD(*PDWORD((*PDWORD(0x010a2018 + 4)) + 0x98 * byMap));//地图表,删除的时候需要的,也就是玩家基址+0xe0
	PDWORD pNpcTable = PDWORD(*pMapTable);                                       //NPC表,里面都是NPC对象,创建NPC的时候需要
	while (1)
	{
		WaitForSingleObject(objEvent, 2000);     //暂停2秒
		CTime objTime = CTime::GetCurrentTime();//获取系统时间
		int nHour = objTime.GetHour();
		int nMinu = objTime.GetMinute();
		if (nHour == nCreateHours && nMinu == nCreateMinus && bCreate == FALSE)
		{
			//create npc
			_asm
			{
				push 0x2ae                  //id
				push 7                      //type
				mov edi, pPos               //point to postion
				mov ecx, pNpcTable          //NpcTable
				call g_pCreateNpcCall
			}
			bCreate = TRUE;

		}

		if (nHour == nRemoveHours && nMinu == nRemoveMinus && bCreate == TRUE)
		{
			//remove npc
			int   nTempX = stcTempPos.x / 40;
			int   nTempZ = stcTempPos.z / 40;

			_asm
			{
				push pPos                   //坐标指针
				push 1                      //固定1
				push 0x2ae                  //id
				push 7                      //type
				push pNpcTable              //pNpcTable
				mov ecx, nTempX
				mov eax, nTempZ
				call g_pDeleteNpcCall
			}
			bCreate = FALSE;

		}
	}
}

void RecRune_Signature(PBYTE pSignature, PVOID pRecRune, BYTE byTargetType)
{
	if (*PDWORD(pRecRune) == 100171) return;
    //先让标识位全部可用
	for (DWORD i = 0; i < 9; i++)
	{
		pSignature[i] = 0xff;
	}
	
	//如果是武器或是首饰的话,让血黄蓝不可洗
	/*
	if ((byTargetType >= 1 && byTargetType < 16) ||
		byTargetType == 22 || byTargetType == 23 || byTargetType == 25)
	{
		pSignature[6] = 0;
		pSignature[7] = 0;
		pSignature[8] = 0;
	}*/
}

BYTE RecRune_CustomStats(PBYTE pAddr, BYTE byTimes, PVOID pRecRune)
{
    
	//1.设置属性卷轴的偏移
	
	DWORD dwOffset = byTimes * 2;
	if (byTimes <= 5)
		dwOffset += 0x70;               //如果是常规属性就这个开头
	else
		dwOffset += 0x5e;              //如果是血典蓝就是这个开头


	//2.先读出里面的属性
	BYTE byRecRuneValue = *PBYTE((DWORD)pRecRune + dwOffset);// from rec rune
	BYTE byItemValue = *pAddr;

	byItemValue += byRecRuneValue;
	if (byItemValue > 99) byItemValue = 99;
    
	//3.返回
	return byItemValue;
	
}

void EnchantAdd()
{
	CEvent objEvent;
	WaitForSingleObject(objEvent, 5000);       //暂停,让程序读完地图与其它内容再说
	//3.1修改一些地方让其支持最大化49
	BYTE byMaxEnchant = MAXENCHANT;
	DWORD dwOldProtect;
	VirtualProtect((PVOID)0x0046ccc1, 1, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046ccc1, &byMaxEnchant, 1, NULL);
	VirtualProtect((PVOID)0x0046ccc1, 1, dwOldProtect, &dwOldProtect);

	VirtualProtect((PVOID)0x004d28d2, 1, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(GetCurrentProcess(), (PVOID)0x004d28d2, &byMaxEnchant, 1, NULL);
	VirtualProtect((PVOID)0x004d28d2, 1, dwOldProtect, &dwOldProtect);
	//3.2从文件中读取数植
	CString szFilePath = L"Data\\ItemEnchant.ini";
	CString szWeapon;
	CString szDefense;
	for (WORD i = 0; i < _countof(g_stcEnchantAddValue); i++)
	{
		szWeapon.Format (L"WeaponStep%02d",  i);
		szDefense.Format(L"DefenseStep%02d", i);
		g_stcEnchantRate[i].dwWeaponValue = GetPrivateProfileInt(L"LapisianEnchantPercentRate", szWeapon, 0, szFilePath);
		g_stcEnchantRate[i].dwArrmorValue = GetPrivateProfileInt(L"LapisianEnchantPercentRate", szDefense, 0, szFilePath);
		g_stcEnchantAddValue[i].wWeaponValue = GetPrivateProfileInt(L"LapisianEnchantAddValue", szWeapon, 0, szFilePath);
		g_stcEnchantAddValue[i].wArrmorValue = GetPrivateProfileInt(L"LapisianEnchantAddValue", szDefense, 0, szFilePath);
	}
	
	DWORD dwAddrOfRate = (DWORD)g_stcEnchantRate;     //取一下首地址
	DWORD dwAddrOfValue = (DWORD)g_stcEnchantAddValue;//取一下首地址
	//3.3往内存里修改偏移
	DWORD dwValueAddr[] = { 0x004d28fa, 0x0046cd87, 0x0046cda3, 0x004d28e0, };  //修改4处


	for (DWORD i = 0; i < _countof(dwValueAddr); i++)
	{
		//修改每阶段数值
		//以后这里的可以封装成类,不用每次都改内存属性
		VirtualProtect((PVOID)dwValueAddr[i], 4, PAGE_READWRITE, &dwOldProtect);
		WriteProcessMemory(GetCurrentProcess(), (PVOID)dwValueAddr[i], &dwAddrOfValue, 4, NULL);
		VirtualProtect((PVOID)dwValueAddr[i], 4, dwOldProtect, &dwOldProtect);
	}

	//修改机率
	VirtualProtect((PVOID)0x0046cd3b, 4, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046cd3b, &dwAddrOfRate, 4, NULL);
	VirtualProtect((PVOID)0x0046cd3b, 4, dwOldProtect, &dwOldProtect);
}

void RecRune()
{

	CMyInlineHook HookobjTwo;
	CMyInlineHook HookobjThree;
	pRecRuneSignature = RecRune_Signature;
	pRecRuneCustomStats = RecRune_CustomStats;
	
	HookobjTwo.Hook((PVOID)0x004d2bc7, Naked_RecRunePartOne, 9);
	HookobjThree.Hook((PVOID)0x004d2c1c, Naked_RecRunePartTwo, 8);

}
DWORD WINAPI ThreadProc(_In_  LPVOID lpParameter)
{

	g_pUseSkill = UseSkill;
	CString szFilePath = L"Data\\ItemEnchant.ini";
	if (GetPrivateProfileInt(L"无敌保护", L"开启", 0, szFilePath) == 1)
	{
		CMyInlineHook objHook1;
		CMyInlineHook objHook2;
		CMyInlineHook objHook3;
		CMyInlineHook objHook4;
		CMyInlineHook objHook5;
		objHook1.Hook((PVOID)0x00478E4E, Naked_RespawnProtected);
		objHook2.Hook((PVOID)0x00478ECB, Naked_RespawnProtected);
		objHook3.Hook((PVOID)0x00478EFA, Naked_RespawnProtected);
		objHook4.Hook((PVOID)0x00478F2D, Naked_RespawnProtected);
		objHook5.Hook((PVOID)0x00466D45, Naked_RespawnProtected2, 6);
	}

	//1.消耗物品进地图
	CMyInlineHook Hookobjone;
	Hookobjone.Hook((PVOID)0x00476792, Naked, 6);//0x00476c37


	//2.修改强化至49上限
	EnchantAdd();

    //3.属性卷轴   
	RecRune();

	//4.自动刷NPC与删除NPC 刷怪与删怪
	AutoNpc();






	return 0;
}



//获取技能基址
PVOID GetSkillBaseAddr(DWORD dwSkillid, DWORD dwSkillLevel)
{
	PVOID pCall = (PVOID)0x0041BB30;
	PVOID pReturn = NULL;
	_asm
	{
		mov edx, dwSkillLevel
			mov eax, dwSkillid
			call pCall
			mov pReturn, eax
	}
	return pReturn;
}


//使用技能
void UseSkill(DWORD dwSkillid, DWORD dwSkillLevel, PVOID pPlayerAddr)
{
	PVOID pSkillAddr = GetSkillBaseAddr(dwSkillid, dwSkillLevel);
	PVOID pCall = (PVOID)0x004725B0;
	BYTE  bResulT;
	_asm
	{
		mov eax, pSkillAddr
			mov edi, pPlayerAddr
			call pCall
	}
}

// 这是导出函数的一个示例。
TRANSLATEMAP_API int Start(void)
{
	return 42;
}