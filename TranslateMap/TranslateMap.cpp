// TranslateMap.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "MyInlineHook.h"
#include "TranslateMap.h"


BOOL TranslateMap(PVOID pPlayer);
PVOID  GetSkillBaseAddr(DWORD dwSkillid, DWORD dwSkillLevel);
void   UseSkill(DWORD dwSkillid, DWORD dwSkillLevel, PVOID pPlayerAddr);
/********************************************������Ƿ���޵�*/
PVOID g_pUseSkill = NULL;               //�Լ���call
PVOID g_pRespawnReturnAddr = (PVOID)0x00479155;
PVOID g_pRespawnReturnAddr2 = (PVOID)0x00466D4B;



/*���͵�ͼ���Ķ���*/
PVOID  pTranslateMap   = TranslateMap;
PVOID  pFailedAddr     = (PVOID)0x0047679A;
PVOID  pSuccessAddr    = (PVOID)0x00476798;
PVOID  pDeleteItemCall = (PVOID)0x004728E0;
WORD   wMap            = 106;
DWORD  dwItemid        = 100200;


//PVOID  pFailedAddr = (PVOID)0x0047679A;
//PVOID  pSuccessAddr = (PVOID)0x00476c3e;
//PVOID  pDeleteItemCall = (PVOID)0x004728E0;

/*�Զ�ɾ��NPC*/
typedef struct _POS
{
	float x;
	float y;
	float z;
}POS, *PPOS;
PVOID g_pCreateNpcCall = (PVOID)0x004255D0;
PVOID g_pDeleteNpcCall = (PVOID)0x004257A0;


/*ǿ�������޸�*/

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

/*���Ծ����޸�*/
PVOID pRecRuneSignature   = NULL;
PVOID pRecRuneCustomStats = NULL;
PVOID pReturnAddr1 = (PVOID)0x004d2bd0;
PVOID pReturnAddr2 = (PVOID)0x004d2c24;



//�����ҩˮ
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
			jmp g_pRespawnReturnAddr  //ȱ�ٷ���
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
			jmp g_pRespawnReturnAddr2  //ȱ�ٷ���
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
		mov [esp+0x20],eax  //�պð�ecx�ĸ�������
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

//����---ԭ������NPC����
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
	//�����붨��һЩ�ر�����
	CEvent objEvent;
	WaitForSingleObject(objEvent, 5000);       //��ͣ,�ó�������ͼ������������˵
	BOOL bCreate = FALSE;                      //��ʶ
	/*��Щ������Ҫ�޸ĵĲ���*/
	BYTE byMap = 104;                            //map,�������ǲ��Ե�ʱ��ǵò�Ҫ�Ĵ�,�������
	POS stcTempPos = { 1820, 17, 514 };         //����
	int  nCreateHours = 19;                    //����ʱ��
	int  nCreateMinus = 30;
	int  nRemoveHours = 19;                    //ɾ��ʱ��
	int  nRemoveMinus = 45;
	/*************************************************************************/

	PPOS pPos = &stcTempPos;                  //����ָ��
	PDWORD pMapTable = PDWORD(*PDWORD((*PDWORD(0x010a2018 + 4)) + 0x98 * byMap));//��ͼ��,ɾ����ʱ����Ҫ��,Ҳ������һ�ַ+0xe0
	PDWORD pNpcTable = PDWORD(*pMapTable);                                       //NPC��,���涼��NPC����,����NPC��ʱ����Ҫ
	while (1)
	{
		WaitForSingleObject(objEvent, 2000);     //��ͣ2��
		CTime objTime = CTime::GetCurrentTime();//��ȡϵͳʱ��
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
				push pPos                   //����ָ��
				push 1                      //�̶�1
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
    //���ñ�ʶλȫ������
	for (DWORD i = 0; i < 9; i++)
	{
		pSignature[i] = 0xff;
	}
	
	//����������������εĻ�,��Ѫ��������ϴ
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
    
	//1.�������Ծ����ƫ��
	
	DWORD dwOffset = byTimes * 2;
	if (byTimes <= 5)
		dwOffset += 0x70;               //����ǳ������Ծ������ͷ
	else
		dwOffset += 0x5e;              //�����Ѫ�������������ͷ


	//2.�ȶ������������
	BYTE byRecRuneValue = *PBYTE((DWORD)pRecRune + dwOffset);// from rec rune
	BYTE byItemValue = *pAddr;

	byItemValue += byRecRuneValue;
	if (byItemValue > 99) byItemValue = 99;
    
	//3.����
	return byItemValue;
	
}

void EnchantAdd()
{
	CEvent objEvent;
	WaitForSingleObject(objEvent, 5000);       //��ͣ,�ó�������ͼ������������˵
	//3.1�޸�һЩ�ط�����֧�����49
	BYTE byMaxEnchant = MAXENCHANT;
	DWORD dwOldProtect;
	VirtualProtect((PVOID)0x0046ccc1, 1, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046ccc1, &byMaxEnchant, 1, NULL);
	VirtualProtect((PVOID)0x0046ccc1, 1, dwOldProtect, &dwOldProtect);

	VirtualProtect((PVOID)0x004d28d2, 1, PAGE_READWRITE, &dwOldProtect);
	WriteProcessMemory(GetCurrentProcess(), (PVOID)0x004d28d2, &byMaxEnchant, 1, NULL);
	VirtualProtect((PVOID)0x004d28d2, 1, dwOldProtect, &dwOldProtect);
	//3.2���ļ��ж�ȡ��ֲ
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
	
	DWORD dwAddrOfRate = (DWORD)g_stcEnchantRate;     //ȡһ���׵�ַ
	DWORD dwAddrOfValue = (DWORD)g_stcEnchantAddValue;//ȡһ���׵�ַ
	//3.3���ڴ����޸�ƫ��
	DWORD dwValueAddr[] = { 0x004d28fa, 0x0046cd87, 0x0046cda3, 0x004d28e0, };  //�޸�4��


	for (DWORD i = 0; i < _countof(dwValueAddr); i++)
	{
		//�޸�ÿ�׶���ֵ
		//�Ժ�����Ŀ��Է�װ����,����ÿ�ζ����ڴ�����
		VirtualProtect((PVOID)dwValueAddr[i], 4, PAGE_READWRITE, &dwOldProtect);
		WriteProcessMemory(GetCurrentProcess(), (PVOID)dwValueAddr[i], &dwAddrOfValue, 4, NULL);
		VirtualProtect((PVOID)dwValueAddr[i], 4, dwOldProtect, &dwOldProtect);
	}

	//�޸Ļ���
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
	if (GetPrivateProfileInt(L"�޵б���", L"����", 0, szFilePath) == 1)
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

	//1.������Ʒ����ͼ
	CMyInlineHook Hookobjone;
	Hookobjone.Hook((PVOID)0x00476792, Naked, 6);//0x00476c37


	//2.�޸�ǿ����49����
	EnchantAdd();

    //3.���Ծ���   
	RecRune();

	//4.�Զ�ˢNPC��ɾ��NPC ˢ����ɾ��
	AutoNpc();






	return 0;
}



//��ȡ���ܻ�ַ
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


//ʹ�ü���
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

// ���ǵ���������һ��ʾ����
TRANSLATEMAP_API int Start(void)
{
	return 42;
}