#include "stdafx.h"
#include "ShaiyaHook.h"

extern bool MySplit(WCHAR* Desc, WCHAR* Src,WCHAR mark,int nIndex);
extern BOOL IsNumeric(WCHAR* src);
extern void CStringToChar(CString Src,CHAR* Dest);
extern void c2w(wchar_t *pwstr,size_t len,const char *str);
extern void MyWriteProcessMemory(HANDLE, LPVOID, LPCVOID, SIZE_T, SIZE_T * =0);

enum RecRuneType
{
	CoverAll = 1,
	StatsPlus = 2,
	CoverSpecify = 3,
	PlusSpecifyAndRemoveOthers=4
};


CString g_szFilePath = L"";//����ȫ�������ļ�·��


DWORD     g_dwPlayerBaseAddr[400] = { 0 };                //��ʾ��ͬʱ֧��400�����


//************************************************************************��������
vector<RandomSpawn> gRandom_vecRandomInfo;  	//�����ʱ�Ӻ�������Ҫ��һ��ȫ�ֱ���
PVOID               gRandom_pBaseAddr = (PVOID)0x587960;	//�޸ĵĵ�ַ
UINT                gRandom_uTimerId = 0;

//************************************************************************VIP��ɫ����ɫ
CMyInlineHook g_HookObj_Color;                            //hook�Ķ���
CMyInlineHook g_HookObj_Color1;                           //hook�Ķ���
CMyInlineHook g_HookObj_Color2;                           //hook�Ķ���
PVOID gColor_pBackAddr    = (PVOID)0x0042A49C;	          //CALL����Ҫ��һ�����ص�ַ
PVOID gColor_pBackAddr1 = (PVOID)0x0041c7ec;	          //CALL����Ҫ��һ�����ص�ַ
PVOID gColor_pBackAddr2 = NULL;	          //CALL����Ҫ��һ�����ص�ַ

#define VIPMAX 200
DWORD gColor_dwUid[VIPMAX] = { 0 };		                      //���ɴ�100��VIP�˺�

//************************************************************************��ȡ��һ�ַ
PVOID       gBaseAddr_pAddr      =nullptr;                //һ��ָ������������һ�ַ��
PVOID       gBaseAddr_pHookAddr  =(PVOID)0x00454c4c;
PVOID       gBaseAddr_pAddr2     = (PVOID)0x00454c52;     //CALL����Ҫ��

//************************************************************************����GMָ��CALL
PBYTE       gGmCall_pbyCommand   =nullptr;                //ָ���ַ
DWORD       gGmCall_dwPlayerBase =0;                      //��ҵĻ�ַ
PVOID       gGmCall_CallAddr     =(PVOID)0x00474940;

/************************************************************************ǿ�������޸�*/
BYTE            g_byEnchantMax = 20;
EnchantAddValue g_stcEnchantAddValue[50];
EnchantRate     g_stcEnchantRate[50];

/**********************************************���Ծ����޸�*/
PVOID g_pRecRuneSignature   = NULL;
PVOID g_pRecRuneCustomStats = NULL;
PVOID g_pRecRuneReturnAddr1 = (PVOID)0x004d2bd0;
PVOID g_pRecRuneReturnAddr2 = (PVOID)0x004d2c24;
BOOL  g_bRecRuneOverlay     = TRUE;                       //Ĭ��Ϊ����ԭ����

/***********************************************�Զ���ɾNPC*/
vector<AUTONPC>     g_vecAutoNpc;
PVOID g_pCreateNpcCall = (PVOID)0x004255D0;
PVOID g_pDeleteNpcCall = (PVOID)0x004257A0;

/***********************************************�Զ���ɾ����*/
vector<AUTOMONSTER> g_vecAutoMonster;
PVOID g_pCreateMobCall = (PVOID)0x004245A0;
PVOID g_pDeleteMobCall = (PVOID)0x00424c30;

/********************************************������Ƿ���޵�*/
PVOID g_pUseSkill           = NULL;               //�Լ���call
BOOL  g_bRespawnProtected   = 0;                 //�������ļ���ȡ�Ƿ���Ҫ�⹦��
DWORD g_dwProtectedSkillid    = 0;
DWORD g_dwProtectedSkillLevel = 0;

PVOID g_pRespawnReturnAddr  = (PVOID)0x00479155;
PVOID g_pRespawnReturnAddr2 = (PVOID)0x00466D4B;


/********************************************100%ǿ��ʯ*/
PVOID g_pCustomEnchantRateReturnAddr1 = (PVOID)0x0046CD45;
PVOID g_pCustomEnchantRateReturnAddr2 = (PVOID)0x0046cd83;

/**************ĳ��ͼ�������**********************/
PVOID g_pfnLimitTransferForParty = NULL;
WORD  g_wMap1 = 0;


/*���͵�ͼ���Ķ���*/

typedef struct _FunctionTime
{
	DWORD dwHours;
	DWORD dwMins;
}FunctionTIme,*PFunctionTime;

PVOID  g_pTranslateMap = TranslateMap;
PVOID  g_pFailedAddr = (PVOID)0x0047679A;
PVOID  g_pSuccessAddr = (PVOID)0x00476798;
PVOID  g_pDeleteItemCall = (PVOID)0x004728E0;
WORD   g_wMap = 0;
DWORD  g_dwItemid = 0;
BOOL   g_bTranslateEnable = FALSE;
BOOL   g_bEnableMap = FALSE;
vector<FunctionTIme> g_vecStartTime;
vector<FunctionTIme> g_vecEndTime;



BOOL LimitTransferForParty(PVOID pPlayer);

__declspec(naked) void Naked()
{
	_asm
	{
		pushad
			movzx ecx, cx
			push ecx        //map
			push ebp        //pPlayer
			call g_pTranslateMap
			cmp al, 0
			popad
			je _Wrong
			jmp _Right
		_Right :
		cmp ecx, 0xC8
			jmp g_pSuccessAddr
		_Wrong :
		jmp g_pFailedAddr
	}
}


//�����ҩˮ
__declspec(naked) void Naked_RespawnProtected()
{
	_asm
	{
		pushad
			push edi                  //player addr
			push g_dwProtectedSkillLevel                  //skill level
			push g_dwProtectedSkillid                 //skillid
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
			push g_dwProtectedSkillLevel                  //skill level
			push g_dwProtectedSkillid                 //skillid
			call g_pUseSkill
			add esp, 0xc
			popad
			jmp g_pRespawnReturnAddr2  //ȱ�ٷ���
	}

}

//100%ǿ��ʯ
__declspec(naked) void Naked_CustomEnchantRate()
{
	_asm
	{
		add edi, eax
			mov dword ptr ss : [esp + 0x38], edi
			push eax
			mov eax, dword ptr ss : [esp + 0x18]
			mov eax, dword ptr ss : [eax + 0x30]
			mov eax, dword ptr ds : [eax]

			cmp eax, 0x17319
			je ___exit
			cmp eax, 0x1731A
			je ___exit
			cmp eax, 0x1731E
			je ___exit
			cmp eax, 0x1731F
			je ___exit
			pop eax
			jmp g_pCustomEnchantRateReturnAddr2     //100% ���ʵĵط�
		___exit:
			pop eax
			jmp g_pCustomEnchantRateReturnAddr1
	}

}



//���Ծ���
PVOID g_pFailedRecRune = (PVOID)0x0046d901;
PVOID g_pSucessRecRune = (PVOID)0x0046d6bd;
__declspec(naked) void Naked_RecRunePartOne()
{
	_asm
	{
		cmp dword ptr[ecx],0x1874b
			je __orgina
			mov al, byte ptr[edi+0x136]   //player level
			cmp al, byte ptr[ecx+0x2e]    //rec rune level
			jb __failed

		__orgina:
		cmp byte ptr[ebp+0x68],0x51
			je __failed
			jmp g_pSucessRecRune
	__failed:
		jmp g_pFailedRecRune


	}
}



//���Ծ���
__declspec(naked) void Naked_RecRunePartTwo()
{
	_asm
	{
		lea ebp, dword ptr[esi + 0x84]  //orginnal code
			mov ecx, esp
			pushad
			mov edi, [ecx + 0x4]
			mov edi, [edi + 0x30]
			mov edi, [edi + 0xa0]
			push edi               //ItemType
			mov edi, [ecx + 0x54]
			mov edi, [edi + 0x30]  //rec rune 
			push edi
			lea  ecx, [ecx + 0xc]
			push ecx               //addr of signature
			call g_pRecRuneSignature
			add esp, 0xc
			popad
			jmp g_pRecRuneReturnAddr1
	}
}

__declspec(naked) void Naked_RecRunePartThree()
{
	_asm
	{
		push ecx
			mov ecx, esp
			pushad
			mov edx, [ecx + 0x58]
			mov edx, [edx + 0x30]
			cmp dword ptr[edx], 0x1874b   //100171
			je _Return
			push edx                      //rec rune addr
			push edi                      //times
			push ebp                      //pbuffer
			mov esi, [esi + 0x30]
			movzx esi, byte ptr[esi + 0x3e]
			push esi                     //reqwis
			call g_pRecRuneCustomStats
			mov[esp + 0x20], eax          //�պð�ecx�ĸ�������
			popad
			pop ecx
			mov bl, cl
			movzx dx, bl
			mov word ptr[ebp], dx
			jmp g_pRecRuneReturnAddr2
		_Return :
		popad
			pop ecx
			movzx dx, bl
			mov word ptr[ebp], dx
			jmp g_pRecRuneReturnAddr2
	}
}

//GMָ��CALL
__declspec(naked) void Naked_GmCommandCall()
{
	__asm
	{	
		mov edx,gGmCall_pbyCommand           //ָ��ṹ��   �����ǰ2���ֽ�Ϊ����   ������һϵ�в���
		mov ebx,gGmCall_dwPlayerBase         //����GMָ��Ľ�ɫ��ַ
		mov ecx,ebx
		//��ʱȨ�޴���
		push eax                             
		mov al, [ebx+0x5808 ]                
		push eax                            
		mov byte ptr [ebx+0x5808],0x01
		//ִ�е����   
		mov ecx,ebx
		call gGmCall_CallAddr
		//��ʱȨ�޴���
		pop eax
		mov byte ptr [ebx+0x5808],al        
		pop eax                              
		ret
	}
}


//HOOK�����Ӫȡ��ַ
__declspec(naked) void Naked_GetBaseAddr()
{
	__asm
	{	
		push eax
		mov eax,gBaseAddr_pAddr
	    mov [eax],edi
		pop eax
		add gBaseAddr_pAddr,0x4
		mov dword ptr [edi],0x00571AA8  //ԭ���
		jmp gBaseAddr_pAddr2
	}
}


__declspec(naked) void Naked_Color()
{
	__asm
	{	pushad
		lea eax,gColor_dwUid[0]      //������һ�������λ��
	    mov ebx,0x01			     //���һ��������
_Loop:    
	    mov edi,[eax]
		cmp edi,0x00                  
		je _No                       //�����0�Ļ�������                    
		cmp [esi+0x582c],edi
		je _Yes                      //���UIDƥ��Ļ�
		cmp ebx,0xc8
		jge _no                      //�����������100�˾�����
		add eax,0x04                 //�ƶ�4���ֽ�
		add ebx,0x01                 //��������1
		jmp _Loop
_Yes:
		popad
		cmp cl,0xA
		jmp gColor_pBackAddr
_No:
        popad
        cmp cl,[esi+0x5808]
		jmp gColor_pBackAddr
	}
}

__declspec(naked) void Naked_Color1()
{
	__asm
	{	pushad
		lea eax, gColor_dwUid[0]      //������һ�������λ��
		mov ebx, 0x01			     //���һ��������
	_Loop:
	mov esi, [eax]
		cmp esi, 0x00
		je _No                       //�����0�Ļ�������                    
		cmp [edi + 0x582c], esi
		je _Yes                      //���UIDƥ��Ļ�
		cmp ebx, 0xc8
		jge _no                      //�����������100�˾�����
		add eax, 0x04                 //�ƶ�4���ֽ�
		add ebx, 0x01                 //��������1
		jmp _Loop
	_Yes :
	popad
		cmp cl, 0xA
		jmp gColor_pBackAddr1
	_No :
	popad
		cmp cl, byte ptr [edi + 0x5808]
		jmp gColor_pBackAddr1
	}
}


__declspec(naked) void Naked_Color2()
{
	__asm
	{	pushad
		lea eax, gColor_dwUid[0]      //������һ�������λ��
		mov ebx, 0x01			     //���һ��������
	_Loop:
	mov esi, [eax]
		cmp esi, 0x00
		je _No                       //�����0�Ļ�������                    
		cmp[ebp + 0x582c], esi
		je _Yes                      //���UIDƥ��Ļ�
		cmp ebx, 0xc8
		jge _no                      //�����������100�˾�����
		add eax, 0x04                 //�ƶ�4���ֽ�
		add ebx, 0x01                 //��������1
		jmp _Loop
	_Yes :
	popad
		mov al,0xa
		jmp gColor_pBackAddr2
	_No :
	popad
		mov al, byte ptr  [ebp + 0x5808]  //ԭʼ�����
		jmp gColor_pBackAddr2
	}
}






//�Զ���ɾ���NPC
DWORD WINAPI AutoTask(_In_  LPVOID lpParameter)
{
	CEvent stcEvent;
	while (1)
	{
		WaitForSingleObject(stcEvent, 5000);//ÿ5��ͣһ��
		//NPC
		for (vector<AUTONPC>::iterator it = g_vecAutoNpc.begin(); it != g_vecAutoNpc.end();)
		{
			DWORD  dwNpcid = it->dwNpcId;   //NPC ID
			DWORD  dwNpcType = it->dwNpcType; //NPC TYPE
			PVOID  pPos = &it->x;        //����ָ��
			PDWORD pMapTable = PDWORD(*PDWORD((*PDWORD(0x010a2018 + 4)) + 0x98 * it->byMap));//��ͼ��,ɾ����ʱ����Ҫ��,Ҳ������һ�ַ+0xe0
			PDWORD pNpcTable = PDWORD(*pMapTable);                                           //NPC��,���涼��NPC����,����NPC��ʱ����Ҫ
			//Ĭ��1Ϊɾ��NPC
			if (it->bFlag)
			{
				BYTE byHour = _tcstoul(it->szDeathHours, 0, 10);
				BYTE byMin = _tcstoul(it->szDeathMins, 0, 10);
				CTime objTime = CTime::GetCurrentTime();       //��ȡϵͳʱ��
				if (objTime.GetHour() == byHour&&objTime.GetMinute() == byMin)
				{
					//remove npc
					int    nTempX = it->x / 40;//����1
					int    nTempZ = it->z / 40;//����2

					_asm
					{
						push pPos                   //����ָ��
							push 1                      //�̶�1
							push dwNpcid        //id
							push dwNpcType      //type
							push pNpcTable              //pNpcTable
							mov ecx, nTempX
							mov eax, nTempZ
							call g_pDeleteNpcCall
					}
					//�����ѭ��,ֻ��һ���Եľ�ɾ�������
					if (!it->bLoop)
					{
						it = g_vecAutoNpc.erase(it);       //ɾ�������һ��Ԫ�ظ�it
						continue;
					}
					it->bFlag = !it->bFlag;//�ı�־λ
				}
			}
			//0Ϊ���NPC
			else
			{
				BYTE byHour = _tcstoul(it->szBornHours, 0, 10);
				BYTE byMin = _tcstoul(it->szBornMins, 0, 10);
				CTime objTime = CTime::GetCurrentTime();       //��ȡϵͳʱ��
				if (objTime.GetHour() == byHour&&objTime.GetMinute() == byMin)
				{
					_asm
					{
						push dwNpcid                    //id
							push dwNpcType                  //type
							mov edi, pPos                   //point to postion
							mov ecx, pNpcTable              //NpcTable
							call g_pCreateNpcCall
					}
					it->bFlag = !it->bFlag;//�ı�־λ
				}
			}
			it++;
		}

		//Monster
		for (vector<AUTOMONSTER>::iterator it = g_vecAutoMonster.begin(); it != g_vecAutoMonster.end();)
		{
			DWORD  dwMobid = it->dwMobId;   //ID
			PVOID  pPos = &it->x;        //����ָ��
			PDWORD pMapTable = PDWORD(*PDWORD((*PDWORD(0x010a2018 + 4)) + 0x98 * it->byMap));//
			PDWORD pNpcTable = PDWORD(*pMapTable);                                           //����������ǵ�ͼ����
			//Ĭ��1Ϊɾ��
			if (it->bFlag)
			{
				BYTE byHour = _tcstoul(it->szDeathHours, 0, 10);
				BYTE byMin = _tcstoul(it->szDeathMins, 0, 10);
				CTime objTime = CTime::GetCurrentTime();       //��ȡϵͳʱ��
				if (objTime.GetHour() == byHour&&objTime.GetMinute() == byMin)
				{
					//remove
					int    nTempX = it->x / 40;//����1
					int    nTempZ = it->z / 40;//����2
					BYTE   bySuccess = 0;
					_asm
					{
					_loop:
						push pPos             //����������ָ��
							mov edx, nTempX       //����1
							mov ecx, nTempZ       //����2
							push 0x1              //��������
							push dwMobid          //����ID
							push pNpcTable        //��ͼ����
							call g_pDeleteMobCall //����call
							cmp al, 0x2
							je _loop
					}

					//�����ѭ��,ֻ��һ���Եľ�ɾ�������
					if (!it->bLoop)
					{
						it = g_vecAutoMonster.erase(it);       //ɾ�������һ��Ԫ�ظ�it
						continue;
					}
					it->bFlag = !it->bFlag;//�ı�־λ

				}
			}
			//0Ϊ���NPC
			else
			{
				BYTE byHour = _tcstoul(it->szBornHours, 0, 10);
				BYTE byMin = _tcstoul(it->szBornMins, 0, 10);
				CTime objTime = CTime::GetCurrentTime();        //��ȡϵͳʱ��
				if (objTime.GetHour() == byHour&&objTime.GetMinute() == byMin)
				{
					_asm
					{
						mov eax, 0x1    //��������
							mov ecx, dwMobid        //����ID
							push pNpcTable         //��ͼ����
							mov ebx, pPos           //����
							call g_pCreateMobCall  //����CALL
					}
					it->bFlag = !it->bFlag;    //�ı�־λ
				}
			}
			it++;
		}
	}
}



DWORD WINAPI TimerProc_Dynamic(_In_  LPVOID lpParameter)
{
	while (1)
	{
		//ȡUID
		Sleep(10000);
		WCHAR  szReturn[MAX_PATH*4] = { 0 };
		CString szTemp[VIPMAX];
		GetPrivateProfileString(L"VopColor", L"UID", L"", szReturn, MAX_PATH*4, g_szFilePath);
		DWORD j = 0;
		DWORD dwTemp[VIPMAX] = { 0 };
		ZeroMemory(dwTemp, sizeof(dwTemp));

		for ( ; j < _countof(szTemp); j++)
		{
			AfxExtractSubString(szTemp[j], szReturn, j, L',');
			if (szTemp[j] == L"")
				break;
			else
				dwTemp[j] = _tcstoul(szTemp[j], 0, 10);
		}
		memmove_s(gColor_dwUid, sizeof(gColor_dwUid), dwTemp, sizeof(dwTemp));

		//����������
		g_dwItemid = GetPrivateProfileInt(L"Transfer", L"ItemId", 0, g_szFilePath);
		g_wMap = GetPrivateProfileInt(L"Transfer", L"Mapid", 0, g_szFilePath);
	}
	return 0;
}
DWORD WINAPI TimerProc_Random(_In_  LPVOID lpParameter)
{
	while (1)
	{
		Sleep(500);
		DWORD dwOldProtect;
		for (UINT i = 0; i < gRandom_vecRandomInfo.size(); i++)
		{
			//���ȵü�����������ͼ�ж������ظ��Ĳ���ȥ���
			int nCount = 1;
			int nStart = i;
			for (UINT j = i + 1; j < gRandom_vecRandomInfo.size(); j++)
			{
				if (gRandom_vecRandomInfo[j].nMapid == gRandom_vecRandomInfo[i].nMapid)
				{
					nCount++;        //��������
					i++;             //��Ϊ����ͬ��ͼ,�´�ѭ���Ӳ�ͬ��ͼ��ʼ
				}
			}
			//ִ��һϵ�в���
			srand(GetTickCount());
			BYTE byRadomIndex = rand() % nCount + nStart; //�����=rand()%��ͼ����+��ʼ����
			//�ó������������
			RandomSpawn stcTemp = gRandom_vecRandomInfo[byRadomIndex];
			//�����Ǽ����ַ��
			PVOID pAddr1 = NULL;
			PVOID pAddr2 = NULL;
			//������ĵ�ַ[587960]
			ReadProcessMemory(GetCurrentProcess(), gRandom_pBaseAddr, &pAddr1, 4, NULL);
			//������ĵ�ַ[[587960]+c]
			pAddr1 = PVOID((DWORD)pAddr1 + 0xc);
			ReadProcessMemory(GetCurrentProcess(), pAddr1, &pAddr2, 4, NULL);
			//ȡ�����յ�ַ[[587960]+c]+188+208*��ͼ��
			pAddr2 = PVOID((DWORD)pAddr2 + 0x188 + 0x208 * stcTemp.nMapid);
			//д�����������
			MyWriteProcessMemory(GetCurrentProcess(), pAddr2, &stcTemp.fPosx, 4, NULL);                  //x
			MyWriteProcessMemory(GetCurrentProcess(), PVOID((DWORD)pAddr2 + 4), &stcTemp.fPosy, 4, NULL);//y
			MyWriteProcessMemory(GetCurrentProcess(), PVOID((DWORD)pAddr2 + 8), &stcTemp.fPosz, 4, NULL);//z
			//д���ŭ������
			pAddr2 = PVOID((DWORD)pAddr2 + 0x10);
			MyWriteProcessMemory(GetCurrentProcess(), pAddr2, &stcTemp.fPosx, 4, NULL);                  //x
			MyWriteProcessMemory(GetCurrentProcess(), PVOID((DWORD)pAddr2 + 4), &stcTemp.fPosy, 4, NULL);//y
			MyWriteProcessMemory(GetCurrentProcess(), PVOID((DWORD)pAddr2 + 8), &stcTemp.fPosz, 4, NULL);//z


		}
	}
	return 0;
}


//����ִ���ݵ㡢��ս��
VOID CALLBACK AddPointPerMin(_In_  HWND hwnd, _In_  UINT uMsg, _In_  UINT_PTR idEvent, _In_  DWORD dwTime)
{
	CString  szMap = L"42";  //��ͼ
	DWORD dwPoint = 1;     //�ӵĵ㿨��
	PLAYERINFO stcPlayerInfo;
	for (DWORD i = 0; i < _countof(g_dwPlayerBaseAddr) && g_dwPlayerBaseAddr[i] != 0; i++)
	{
		if (GetPlayerInfoByBaseAddr(g_dwPlayerBaseAddr[i], stcPlayerInfo))//���ݻ�ַ��ȡ��Ҹ�����Ϣ
		{
			//�жϵ�ͼ�Ƿ����
			if (szMap.Compare(stcPlayerInfo.szMap) == 0)  
			{
				//��ȡ����˺�
				DWORD dwUid=_tcstoul(stcPlayerInfo.szUid, 0, 10);
				CString szSql;
				szSql.Format(L"update ps_userdata.dbo.users_master set point=point+%d where useruid=%d", dwPoint, dwUid);
			//	g_DataBaseObj.ExeSqlByCommand(szSql);
			}
		}
	}
}






//��ʼ��Ϣ
void InitHook()
{

	CEvent objEvent;
	WaitForSingleObject(objEvent, 5000); //��ͣ,�ó�������ͼ������������˵

// 	if (!g_DataBaseObj.LinkDataBase())
// 	{
// 		OutputDebugString(L"connect db failed");
// 		ExitProcess(0);
// 	}   

	g_pUseSkill =  UseSkill;
	g_pfnLimitTransferForParty = LimitTransferForParty;

	//ɨ���ȡ��һ�ַ
	ScanPlayerInfo();                     

	//��ȡ������Ϣ
	LoadConfig();

	//�ݵ㡢��ս��
	BYTE byTemp = 0x2;
	BYTE byTemp1 = 0x1;
	BYTE byTemp2 = 0xeb;
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0042509c, &byTemp, 1); //����ɾ��call���һ��ֵ
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004942a0, &byTemp1, 1);//���Ĵ̿�������ԭ��
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00476f66, &byTemp2, 1);//���Ĵ̿�������ԭ��


	//ǿ�������޸�
	EnchantAdd();           

	//���Ծ���
	RecRune();

	//�����ҩˮ
	//�ȼ��һ���Ƿ���Ҫhook
	if (g_bRespawnProtected&&
		g_dwProtectedSkillid&&
		g_dwProtectedSkillLevel)
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

	//������Ʒ����ͼ
	if ((g_wMap&&g_dwItemid)
		||g_wMap1)
	{
		CMyInlineHook objHook6;
		objHook6.Hook((PVOID)0x00476792, Naked, 6);
	}

	
	//���Ӧ���ô�����,��ʱ����Ҫ��
	//SetTimer(0, 1, 60000, AddPointPerMin);

	//�Զ�NPC����������
	if (g_vecAutoMonster.size() || g_vecAutoNpc.size())
	{
		::CreateThread(0, 0, AutoTask, 0, 0, 0);
	}

	//�Զ���������
	if (gRandom_vecRandomInfo.size())
	{
		::CreateThread(0, 0, TimerProc_Random, 0, 0, 0);
	}

	

	//100%ǿ��ʯ
	CMyInlineHook objHook7;
	objHook7.Hook((PVOID)0x0046cd3f, Naked_CustomEnchantRate, 6);

	//��ȡVIP��ɫ�Ĺ���
	::CreateThread(0, 0, TimerProc_Dynamic, 0, 0, 0);
	Sleep(5000);
	g_HookObj_Color.Hook((PVOID)0x0042a496, Naked_Color, 6);
	g_HookObj_Color1.Hook((PVOID)0x0041c7e6, Naked_Color1, 6);
	gColor_pBackAddr2 = (PVOID)g_HookObj_Color2.Hook((PVOID)0x0047fd52, Naked_Color2,6);


	
}


void Hook_VipColor(BOOL nSwitch,DWORD dwUid[])
{
	//���Ҫ�������ܵĻ�
	if(nSwitch)
	{   //��UID����һ��
		memcpy_s(gColor_dwUid,sizeof(DWORD)*100,dwUid,sizeof(DWORD)*100);
		g_HookObj_Color.Hook((PVOID)0x0042a496, Naked_Color, 6);
	}
	//���Ҫ�رչ��ܵĻ�
	else
	{
		g_HookObj_Color.UnHook();
	}
}

//ִ��GMָ��CALL
void Call_GmCommand(BYTE* pbyCommand, DWORD dwBaseAddr)
{
	gGmCall_dwPlayerBase = dwBaseAddr; //��Ҷ���
	gGmCall_pbyCommand = pbyCommand;   //ָ��
	Naked_GmCommandCall();             //��������������ȫ�ֱ���
}



//HOOK��ȡ��ַ
void ScanPlayerInfo()
{
	ZeroMemory(g_dwPlayerBaseAddr,_countof(g_dwPlayerBaseAddr)*sizeof(DWORD));
	gBaseAddr_pAddr = g_dwPlayerBaseAddr;
	CMyInlineHook HookObj_GetPlayerBaseAddr;
	HookObj_GetPlayerBaseAddr.Hook(gBaseAddr_pHookAddr, Naked_GetBaseAddr, 6);
}


//ͨ����һ�ַ��ȡ�����Ϣ
BOOL GetPlayerInfoByBaseAddr(DWORD pBaseAddr,PLAYERINFO &stcPlayerInfo)
{
	DWORD dwTemp 	  =0;  	 //������ȡ4�ֽ��������ݵ�
	FLOAT flTemp      =0;    //������ȡ������
	BYTE  byTemp      =0;  	 //������ȡ1�ֽ����ݵ�
	BYTE  byCharName[20]={0};//��ȡ20���ַ��Ľ�ɫ��

	//�����UID:+0x582c
	memcpy_s(&dwTemp,4,PVOID(pBaseAddr+0x582c),4);
	swprintf_s(stcPlayerInfo.szUid,10,L"%d",dwTemp);
	if(dwTemp==0) return FALSE;

	//�����Charid:+0x128
	memcpy_s(&dwTemp,4,PVOID(pBaseAddr+0x128),4);
	swprintf_s(stcPlayerInfo.szCharid,10,L"%d",dwTemp);
	if(dwTemp==0) return FALSE;

	//����ҵȼ�:+0x136
	memcpy_s(&byTemp,1,PVOID(pBaseAddr+0x136),1);
	swprintf_s(stcPlayerInfo.szLevel,10,L"%d",byTemp);

	//��ս��+148
	memcpy_s(&dwTemp,4,PVOID(pBaseAddr+0x148),4);
	swprintf_s(stcPlayerInfo.szKills,10,L"%d",dwTemp);

	//��posx:+0xd0
	memcpy_s(&flTemp,4,PVOID(pBaseAddr+0xd0),4);
	swprintf_s(stcPlayerInfo.szPosx,10,L"%.0f",flTemp);

	//��posy:+0xd4
	memcpy_s(&flTemp,4,PVOID(pBaseAddr+0xd4),4);
	swprintf_s(stcPlayerInfo.szPosy,10,L"%.0f",flTemp);

	//��posz:+0xd8
	memcpy_s(&flTemp,4,PVOID(pBaseAddr+0xd8),4);
	swprintf_s(stcPlayerInfo.szPosz,10,L"%.0f",flTemp);

	//��ɫ��:+0x184 
	memcpy_s(&byCharName,_countof(byCharName),PVOID(pBaseAddr+0x184),_countof(byCharName));
	c2w(stcPlayerInfo.szCharName,20,(char*)byCharName);
	
	//��ɫ��ͼ
	byTemp = *PBYTE(pBaseAddr + 0x160);
	swprintf_s(stcPlayerInfo.szMap, 10, L"%d", byTemp);

	//��ɫ��ַ:
	swprintf_s(stcPlayerInfo.szBaseAddr,10,L"%x",pBaseAddr);

	return TRUE;
}


//������Ʒ
void Change_UpdateItem(DWORD dwItemid)
{

// 	//1.������Ʒ��ַ
// 	DWORD dwItemType = dwItemid / 1000;//ȡType
// 	DWORD dwId = dwItemid % 1000;//ȡID
// 	PVOID pItemAddress = PVOID((dwItemType * 0x100 + dwId) * 0xa8 + 0x00589674); //������Ʒ��ַ
// 	CString szSql;
// 	szSql.Format(L"select top 1 * from ps_gamedefs.dbo.items where itemid=%d", dwItemid);
// 
// 	//2.�����ݿ��ȡ������Ʒ��Ϣ
// //	if (!g_DataBaseObj.LinkDataBase())return;
// 		
// 	//3.���ڴ��ж�ȡ����
// 	BYTE byData[0x88] = { 0 };
// 	ZeroMemory(byData, sizeof(BYTE)*_countof(byData));
//	::ReadProcessMemory(GetCurrentProcess(), pItemAddress, byData, _countof(byData), NULL);//����������
// 	//4.�����ݿ��ж�ȡ����,���޸�
// //	g_DataBaseObj.ExeSqlByCommand(szSql);
// 	byData[0x2e]               = _tcstoul(g_DataBaseObj.GetValueByField(L"Reqlevel"), 0, 10);//+2e
// 	byData[0x26]               = _tcstoul(g_DataBaseObj.GetValueByField(L"Country"), 0, 10);//+26
// 	byData[0x27]               = _tcstoul(g_DataBaseObj.GetValueByField(L"Attackfighter"), 0, 10);//+27
// 	byData[0x28]               = _tcstoul(g_DataBaseObj.GetValueByField(L"Defensefighter"), 0, 10);//+28
// 	byData[0x29]               = _tcstoul(g_DataBaseObj.GetValueByField(L"Patrolrogue"), 0, 10);//+29
// 	byData[0x2a]               = _tcstoul(g_DataBaseObj.GetValueByField(L"Shootrogue"), 0, 10);//+2a
// 	byData[0x2b]               = _tcstoul(g_DataBaseObj.GetValueByField(L"Attackmage"), 0, 10);//+2b
// 	byData[0x2c]               = _tcstoul(g_DataBaseObj.GetValueByField(L"Defensemage"), 0, 10);//+2c
// 	byData[0x30]               = _tcstoul(g_DataBaseObj.GetValueByField(L"Grow"), 0, 10);//+30
// 	*((WORD*)(&byData[0x36]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"ReqStr"), 0, 10);//+36
// 	*((WORD*)(&byData[0x38]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"ReqDex"), 0, 10);//+38
// 	*((WORD*)(&byData[0x3a]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"ReqRec"), 0, 10);//+3a
// 	*((WORD*)(&byData[0x3c]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"ReqInt"), 0, 10);//+3c
// 	*((WORD*)(&byData[0x3e]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"ReqWis"), 0, 10);//+3e
// 	*((WORD*)(&byData[0x40]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"Reqluc"), 0, 10);//+40
// 	*((WORD*)(&byData[0x34]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"ReqVg"), 0, 10);//+34
// 	byData[0x31]               = _tcstoul(g_DataBaseObj.GetValueByField(L"ReqOg"), 0, 10);//+31
// 	byData[0x32]               = _tcstoul(g_DataBaseObj.GetValueByField(L"ReqIg"), 0, 10);//+32
// 	*((WORD*)(&byData[0x42]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"Range"), 0, 10);//+42
// 	byData[0x44]               = _tcstoul(g_DataBaseObj.GetValueByField(L"AttackTime"), 0, 10);//+44
// 	byData[0x45]               = _tcstoul(g_DataBaseObj.GetValueByField(L"Attrib"), 0, 10);//+45
// 	byData[0x46]               = _tcstoul(g_DataBaseObj.GetValueByField(L"Special"), 0, 10);//+46
// 	byData[0x47]               = _tcstoul(g_DataBaseObj.GetValueByField(L"Slot"), 0, 10);//+47
// 	*((WORD*)(&byData[0x4c]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"Quality"), 0, 10);//+4c
// 	*((WORD*)(&byData[0x52]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"Effect1"), 0, 10);//+52
// 	*((WORD*)(&byData[0x58]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"Effect2"), 0, 10);//+58
// 	*((WORD*)(&byData[0x5e]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"Effect3"), 0, 10);//+5e
// 	*((WORD*)(&byData[0x64]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"Effect4"), 0, 10);//+64
// 	*((WORD*)(&byData[0x6a]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"ConstHP"), 0, 10);//+6a
// 	*((WORD*)(&byData[0x6c]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"ConstSP"), 0, 10);//6c
// 	*((WORD*)(&byData[0x6e]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"ConstMP"), 0, 10);//6e
// 	*((WORD*)(&byData[0x70]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"ConstStr"), 0, 10);//70
// 	*((WORD*)(&byData[0x72]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"ConstDex"), 0, 10);//72
// 	*((WORD*)(&byData[0x74]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"ConstRec"), 0, 10);//74
// 	*((WORD*)(&byData[0x76]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"ConstInt"), 0, 10);//76
// 	*((WORD*)(&byData[0x78]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"ConstWis"), 0, 10);//78
// 	*((WORD*)(&byData[0x7a]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"ConstLuc"), 0, 10);//7a
// 	byData[0x48]               = _tcstoul(g_DataBaseObj.GetValueByField(L"Speed"), 0, 10);//48
// 	byData[0x49]               = _tcstoul(g_DataBaseObj.GetValueByField(L"Exp"), 0, 10);//49
// 	*((DWORD*)(&byData[0x80])) = _tcstoul(g_DataBaseObj.GetValueByField(L"Buy"), 0, 10);//80
// 	*((DWORD*)(&byData[0x84])) = _tcstoul(g_DataBaseObj.GetValueByField(L"Sell"), 0, 10);//84
// 	*((WORD*)(&byData[0x7c]))  = _tcstoul(g_DataBaseObj.GetValueByField(L"Grade"), 0, 10);//7c
// 	byData[0x4b]               = _tcstoul(g_DataBaseObj.GetValueByField(L"Count"), 0, 10);//4b
// 	//5.���޸ĺõ�д���ڴ�
//	MyWriteProcessMemory(GetCurrentProcess(), pItemAddress, byData, _countof(byData), NULL	
}



void EnchantAdd()
{
	//1.�ж��Ƿ�Ĭ�ϵ�ǿ��
	if (g_byEnchantMax == 20) return;

	//2.�޸�ǿ��
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046ccc1, &g_byEnchantMax, 1, NULL);//�����ط�Ҫ�޸�����
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004d28d2, &g_byEnchantMax, 1, NULL);

	//3.���ļ��ж�ȡ��ֲ
	CString szFilePath=L"Data\\ItemEnchant.ini";
	CString szWeapon;
	CString szDefense;
	for (WORD i = 0; i < _countof(g_stcEnchantAddValue); i++)
	{
		szWeapon.Format(L"WeaponStep%02d", i);
		szDefense.Format(L"DefenseStep%02d", i);
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
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)dwValueAddr[i], &dwAddrOfValue, 4, NULL);//�޸�ÿ�׶���ֵ
	}

	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046cd3b, &dwAddrOfRate, 4, NULL);         //�޸Ļ���
}


void LoadConfig()
{
	//����·��
	WCHAR szTemp[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szTemp);
	 g_szFilePath = szTemp;
	 g_szFilePath = g_szFilePath + L"\\config.ini";

	//��ȡǿ��
	 g_byEnchantMax = GetPrivateProfileInt(L"MaxEnchant", L"Max", 0, g_szFilePath);

	//�����Ծ���
	 g_bRecRuneOverlay = GetPrivateProfileInt(L"CustomRecRune", L"CoverOldStats", 0, g_szFilePath);

	//���Զ�NPC
	for (DWORD i = 1; i < 20; i++)
	{
		CString szKey;                   //��ȡ��
		WCHAR   szReturn[MAX_PATH] = {0};
		CString szTemp[9];
		szKey.Format(L"%d", i);
		if (GetPrivateProfileString(L"AutoNpc", szKey, L"", szReturn, MAX_PATH, g_szFilePath) <= 0) break;

		//�и���ַ�������
		AUTONPC stcTemp = { 0 };
		for (DWORD j = 0; j < _countof(szTemp);j++)
		{
			AfxExtractSubString(szTemp[j], szReturn, j, L',');
		}
		stcTemp.dwNpcType    = _tcstoul(szTemp[0], 0, 10);
		stcTemp.dwNpcId      = _tcstoul(szTemp[1], 0, 10);
		stcTemp.byMap        = _tcstoul(szTemp[2], 0, 10);
		stcTemp.x            = _tcstoul(szTemp[3], 0, 10);
		stcTemp.y            = _tcstoul(szTemp[4], 0, 10);
		stcTemp.z            = _tcstoul(szTemp[5], 0, 10);
		stcTemp.bLoop        = _tcstoul(szTemp[8], 0, 10);
		AfxExtractSubString(stcTemp.szBornHours,  szTemp[6], 0, L':');
		AfxExtractSubString(stcTemp.szBornMins,   szTemp[6], 1, L':');
		AfxExtractSubString(stcTemp.szDeathHours, szTemp[7], 0, L':');
		AfxExtractSubString(stcTemp.szDeathMins,  szTemp[7], 1, L':');
		stcTemp.bFlag        = 0;
		g_vecAutoNpc.push_back(stcTemp);           
	}

	//��ȡ�����
	do 
	{
		//����ͼ
		int nMapid = GetPrivateProfileInt(L"RandomRespawn1", L"Mapid", 0, g_szFilePath);//��ͼ
		if (!nMapid) break;

		//������
		for (DWORD i = 1; i <= 20; i++)       //֧��4�������
		{
			RandomSpawn stcRes;
			stcRes.nMapid = nMapid;

			CString szKey;                  
			WCHAR   szReturn[MAX_PATH] = { 0 };
			CString szTemp[3];               //��3����
			szKey.Format(L"pos%d", i);
			if (GetPrivateProfileString(L"RandomRespawn1", szKey, L"", szReturn, MAX_PATH, g_szFilePath) <= 0) break;

			//�и���ַ�������
			for (DWORD j = 0; j < _countof(szTemp); j++)
			{
				AfxExtractSubString(szTemp[j], szReturn, j, L',');
			}
			stcRes.fPosx = _wtof(szTemp[0]);
			stcRes.fPosy = _wtof(szTemp[1]);
			stcRes.fPosz = _wtof(szTemp[2]);
			gRandom_vecRandomInfo.push_back(stcRes);
		}
	} while (0);

	//��ȡ�����
	do
	{
		//����ͼ
		int nMapid = GetPrivateProfileInt(L"RandomRespawn2", L"Mapid", 0, g_szFilePath);//��ͼ
		if (!nMapid) break;

		//������
		for (DWORD i = 1; i <= 4; i++)       //֧��4�������
		{
			RandomSpawn stcRes;
			stcRes.nMapid = nMapid;

			CString szKey;
			WCHAR   szReturn[MAX_PATH] = { 0 };
			CString szTemp[3];               //��3����
			szKey.Format(L"pos%d", i);
			if (GetPrivateProfileString(L"RandomRespawn2", szKey, L"", szReturn, MAX_PATH, g_szFilePath) <= 0) break;

			//�и���ַ�������
			for (DWORD j = 0; j < _countof(szTemp); j++)
			{
				AfxExtractSubString(szTemp[j], szReturn, j, L',');
			}
			stcRes.fPosx = _wtof(szTemp[0]);
			stcRes.fPosy = _wtof(szTemp[1]);
			stcRes.fPosz = _wtof(szTemp[2]);
			gRandom_vecRandomInfo.push_back(stcRes);
		}
	} while (0);

	//��ȡ����
	for (DWORD i = 1; i < 20; i++)
	{
		CString szKey;                   //��ȡ��
		WCHAR   szReturn[MAX_PATH] = { 0 };
		CString szTemp[9];
		szKey.Format(L"%d", i);
		if (GetPrivateProfileString(L"AutoMonster", szKey, L"", szReturn, MAX_PATH, g_szFilePath) <= 0) break;

		//�и���ַ�������
		AUTOMONSTER stcTemp = { 0 };
		for (DWORD j = 0; j < _countof(szTemp); j++)
		{
			AfxExtractSubString(szTemp[j], szReturn, j, L',');
		}
		stcTemp.dwMobId    = _tcstoul(szTemp[0], 0, 10);
		stcTemp.byMap      = _tcstoul(szTemp[1], 0, 10);
		stcTemp.x          = _tcstoul(szTemp[2], 0, 10);
		stcTemp.y          = _tcstoul(szTemp[3], 0, 10);
		stcTemp.z          = _tcstoul(szTemp[4], 0, 10);
		stcTemp.bLoop      = _tcstoul(szTemp[7], 0, 10);
		AfxExtractSubString(stcTemp.szBornHours,  szTemp[5], 0, L':');
		AfxExtractSubString(stcTemp.szBornMins,   szTemp[5], 1, L':');
		AfxExtractSubString(stcTemp.szDeathHours, szTemp[6], 0, L':');
		AfxExtractSubString(stcTemp.szDeathMins,  szTemp[6], 1, L':');
		stcTemp.bFlag = 0;
		g_vecAutoMonster.push_back(stcTemp);
	}

	//��ȡ:�Ƿ���Ҫ�����ʹ���޵�
	g_bRespawnProtected = GetPrivateProfileInt(L"RespawnProtected", L"Enable", 0, g_szFilePath);
	g_dwProtectedSkillid = GetPrivateProfileInt(L"RespawnProtected", L"Skillid", 0, g_szFilePath);
	g_dwProtectedSkillLevel = GetPrivateProfileInt(L"RespawnProtected", L"Skilllevel", 0, g_szFilePath);

	//��ȡ:�Ƿ���Ҫָ����ͼ��ID��
	g_dwItemid = GetPrivateProfileInt(L"Transfer", L"ItemId", 0, g_szFilePath);
	g_wMap = GetPrivateProfileInt(L"Transfer", L"Mapid", 0, g_szFilePath);
	
	//��ȡ:�Ƿ���Ҫָ����ͼ���Ƶ��˽�
	g_wMap1 = GetPrivateProfileInt(L"TransferLimitForParty", L"Mapid", 0, g_szFilePath);
}

void RecRune()
{
	CMyInlineHook HookobjOne;
	CMyInlineHook HookobjTwo;
	CMyInlineHook HookobjThree;
	g_pRecRuneSignature   = RecRune_Signature;
	g_pRecRuneCustomStats = RecRune_CustomStats;


	HookobjOne.Hook((PVOID)0x0046d6b3, Naked_RecRunePartOne, 10);
	HookobjTwo.Hook((PVOID)0x004d2bc7, Naked_RecRunePartTwo, 9);
	HookobjThree.Hook((PVOID)0x004d2c1c, Naked_RecRunePartThree, 8);

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
	/*if ((byTargetType >= 1 && byTargetType < 16) ||
	byTargetType == 22 || byTargetType == 23 || byTargetType == 25)
	{
	pSignature[6] = 0;
	pSignature[7] = 0;
	pSignature[8] = 0;
	}*/
}

BYTE __stdcall RecRune_CustomStats(BYTE byReqwis, PBYTE pAddr, BYTE byTimes, PVOID pRecRune)
{

	//1.�������Ծ����ƫ��
	DWORD dwOffset = byTimes * 2;
	if (byTimes <= 5)
		dwOffset += 0x70;               //����ǳ������Ծ������ͷ
	else
		dwOffset += 0x5e;              //�����Ѫ�������������ͷ

	//2.�ȶ������������
	BYTE byRecRuneValue = *PBYTE((DWORD)pRecRune + dwOffset);// from rec rune
	BYTE byItemValue = *pAddr;//��Ʒϴ֮ǰ�ĸ�������


	switch (g_bRecRuneOverlay)
	{
	case CoverAll:
		byItemValue = byRecRuneValue;  //һ�ɸ���
		break;

	case StatsPlus:
		byItemValue += byRecRuneValue;
		break;

	case CoverSpecify:
		if (byRecRuneValue)           //��������ֵ��0�Ļ�������
			byItemValue = byRecRuneValue;
		break;

	case PlusSpecifyAndRemoveOthers:
		if (!byRecRuneValue)
			byItemValue = byRecRuneValue; //��0�͸���
		else
			byItemValue += byRecRuneValue; //����ͼ���
		break;

	default:
		break;
	}

// 	if (g_bRecRuneOverlay)
// 	{
// 		//������ǵĻ��Ͱ����Ծ����ϵ�����д�������
// 		if (byRecRuneValue)                         //��������ֵ��0�Ļ�������
// 			byItemValue = byRecRuneValue;
// 	}
// 	else
// 	{
// 		//���粻����,�Ǿ��ۼ�
// 		byItemValue += byRecRuneValue;
// 	}
	
	if (byItemValue > byReqwis) byItemValue = byReqwis;
	if (byItemValue > 99) byItemValue = 99;

	//3.����
	return byItemValue;

}


//��ȡ���ܻ�ַ
PVOID GetSkillBaseAddr(DWORD dwSkillid, DWORD dwSkillLevel)
{
	PVOID pCall   =(PVOID) 0x0041BB30;
	PVOID pReturn = NULL;
	_asm
	{
		mov edx, dwSkillLevel
		mov eax, dwSkillid
		call pCall
		mov pReturn,eax
	}
	return pReturn;
}


//ʹ�ü���
void UseSkill(DWORD dwSkillid, DWORD dwSkillLevel, PVOID pPlayerAddr)
{
	PVOID pSkillAddr = GetSkillBaseAddr(dwSkillid, dwSkillLevel);
	PVOID pCall = (PVOID)0x004725B0;
	_asm
	{
		mov eax, pSkillAddr
		mov edi, pPlayerAddr
		call pCall
	}
}




BOOL __stdcall TranslateMap(PVOID pPlayer,DWORD dwMap)
{
	//1.�����Ƿ������Ҫ��
	do 
	{
		if (g_wMap1!=dwMap) break;   
		if (*(PDWORD((DWORD)pPlayer + 0x17f4)))
		{
			return FALSE;
		}
	} while (0);



	
	//2.������Ʒ
	do 
	{
		if (g_wMap != dwMap) return TRUE;
		if (!g_dwItemid) break;

		BYTE byMaxBag = *(PBYTE((DWORD)pPlayer + 0x130c));//get max bags
		if (!byMaxBag)  break;                            //must greater than 0
		PDWORD pBagBase = PDWORD((DWORD)pPlayer + 0x220); //get bagbase at bag1
		for (DWORD i = 1; i <= byMaxBag; i++)
		{
			for (DWORD j = 0; j < 24; j++, pBagBase++)
			{
				if (*pBagBase == NULL) continue;                            //if no item in this slot
				DWORD dwTempItemid = *PDWORD(*(PDWORD(*pBagBase + 0x30)));//get itemid

				if (dwTempItemid == g_dwItemid)
				{
					//delete item
					_asm
					{
						push 0
							push j
							push i
							mov ecx, pPlayer
							call g_pDeleteItemCall
					}
					return TRUE;
				}
			}
		}


	} while (0);
	return FALSE;
}



BOOL LimitTransferForParty(PVOID pPlayer)
{
	//��������ӽ�
	DWORD dwParty = *(PDWORD((DWORD)pPlayer + 0x17f4));
	if (dwParty)
		return FALSE;
	else
		return TRUE;
}