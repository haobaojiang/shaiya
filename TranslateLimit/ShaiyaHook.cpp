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


CString g_szFilePath = L"";//保存全局配置文件路径


DWORD     g_dwPlayerBaseAddr[400] = { 0 };                //表示可同时支持400个玩家


//************************************************************************随机复活点
vector<RandomSpawn> gRandom_vecRandomInfo;  	//这个是时钟函数里需要的一个全局变量
PVOID               gRandom_pBaseAddr = (PVOID)0x587960;	//修改的地址
UINT                gRandom_uTimerId = 0;

//************************************************************************VIP角色名颜色
CMyInlineHook g_HookObj_Color;                            //hook的对象
CMyInlineHook g_HookObj_Color1;                           //hook的对象
CMyInlineHook g_HookObj_Color2;                           //hook的对象
PVOID gColor_pBackAddr    = (PVOID)0x0042A49C;	          //CALL里需要的一个返回地址
PVOID gColor_pBackAddr1 = (PVOID)0x0041c7ec;	          //CALL里需要的一个返回地址
PVOID gColor_pBackAddr2 = NULL;	          //CALL里需要的一个返回地址

#define VIPMAX 200
DWORD gColor_dwUid[VIPMAX] = { 0 };		                      //共可存100个VIP账号

//************************************************************************获取玩家基址
PVOID       gBaseAddr_pAddr      =nullptr;                //一个指针用来保存玩家基址的
PVOID       gBaseAddr_pHookAddr  =(PVOID)0x00454c4c;
PVOID       gBaseAddr_pAddr2     = (PVOID)0x00454c52;     //CALL内需要的

//************************************************************************调用GM指令CALL
PBYTE       gGmCall_pbyCommand   =nullptr;                //指令地址
DWORD       gGmCall_dwPlayerBase =0;                      //玩家的基址
PVOID       gGmCall_CallAddr     =(PVOID)0x00474940;

/************************************************************************强化上限修改*/
BYTE            g_byEnchantMax = 20;
EnchantAddValue g_stcEnchantAddValue[50];
EnchantRate     g_stcEnchantRate[50];

/**********************************************属性卷轴修改*/
PVOID g_pRecRuneSignature   = NULL;
PVOID g_pRecRuneCustomStats = NULL;
PVOID g_pRecRuneReturnAddr1 = (PVOID)0x004d2bd0;
PVOID g_pRecRuneReturnAddr2 = (PVOID)0x004d2c24;
BOOL  g_bRecRuneOverlay     = TRUE;                       //默认为覆盖原属性

/***********************************************自动添删NPC*/
vector<AUTONPC>     g_vecAutoNpc;
PVOID g_pCreateNpcCall = (PVOID)0x004255D0;
PVOID g_pDeleteNpcCall = (PVOID)0x004257A0;

/***********************************************自动添删怪物*/
vector<AUTOMONSTER> g_vecAutoMonster;
PVOID g_pCreateMobCall = (PVOID)0x004245A0;
PVOID g_pDeleteMobCall = (PVOID)0x00424c30;

/********************************************复活后是否吃无敌*/
PVOID g_pUseSkill           = NULL;               //自己的call
BOOL  g_bRespawnProtected   = 0;                 //从配置文件读取是否需要这功能
DWORD g_dwProtectedSkillid    = 0;
DWORD g_dwProtectedSkillLevel = 0;

PVOID g_pRespawnReturnAddr  = (PVOID)0x00479155;
PVOID g_pRespawnReturnAddr2 = (PVOID)0x00466D4B;


/********************************************100%强化石*/
PVOID g_pCustomEnchantRateReturnAddr1 = (PVOID)0x0046CD45;
PVOID g_pCustomEnchantRateReturnAddr2 = (PVOID)0x0046cd83;

/**************某地图组队限制**********************/
PVOID g_pfnLimitTransferForParty = NULL;
WORD  g_wMap1 = 0;


/*传送地图消耗东西*/

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


//复活保护药水
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
			push g_dwProtectedSkillLevel                  //skill level
			push g_dwProtectedSkillid                 //skillid
			call g_pUseSkill
			add esp, 0xc
			popad
			jmp g_pRespawnReturnAddr2  //缺少返回
	}

}

//100%强化石
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
			jmp g_pCustomEnchantRateReturnAddr2     //100% 机率的地方
		___exit:
			pop eax
			jmp g_pCustomEnchantRateReturnAddr1
	}

}



//属性卷轴
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



//属性卷轴
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
			mov[esp + 0x20], eax          //刚好把ecx的给覆盖了
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

//GM指令CALL
__declspec(naked) void Naked_GmCommandCall()
{
	__asm
	{	
		mov edx,gGmCall_pbyCommand           //指令结构体   里面的前2个字节为命令   接着是一系列参数
		mov ebx,gGmCall_dwPlayerBase         //触发GM指令的角色基址
		mov ecx,ebx
		//临时权限处理
		push eax                             
		mov al, [ebx+0x5808 ]                
		push eax                            
		mov byte ptr [ebx+0x5808],0x01
		//执行的语句   
		mov ecx,ebx
		call gGmCall_CallAddr
		//临时权限处理
		pop eax
		mov byte ptr [ebx+0x5808],al        
		pop eax                              
		ret
	}
}


//HOOK玩家阵营取基址
__declspec(naked) void Naked_GetBaseAddr()
{
	__asm
	{	
		push eax
		mov eax,gBaseAddr_pAddr
	    mov [eax],edi
		pop eax
		add gBaseAddr_pAddr,0x4
		mov dword ptr [edi],0x00571AA8  //原语句
		jmp gBaseAddr_pAddr2
	}
}


__declspec(naked) void Naked_Color()
{
	__asm
	{	pushad
		lea eax,gColor_dwUid[0]      //给他第一个数组的位置
	    mov ebx,0x01			     //添加一个计数器
_Loop:    
	    mov edi,[eax]
		cmp edi,0x00                  
		je _No                       //如果是0的话就跳走                    
		cmp [esi+0x582c],edi
		je _Yes                      //如果UID匹配的话
		cmp ebx,0xc8
		jge _no                      //如果计数器到100了就走跳
		add eax,0x04                 //移动4个字节
		add ebx,0x01                 //计数器加1
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
		lea eax, gColor_dwUid[0]      //给他第一个数组的位置
		mov ebx, 0x01			     //添加一个计数器
	_Loop:
	mov esi, [eax]
		cmp esi, 0x00
		je _No                       //如果是0的话就跳走                    
		cmp [edi + 0x582c], esi
		je _Yes                      //如果UID匹配的话
		cmp ebx, 0xc8
		jge _no                      //如果计数器到100了就走跳
		add eax, 0x04                 //移动4个字节
		add ebx, 0x01                 //计数器加1
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
		lea eax, gColor_dwUid[0]      //给他第一个数组的位置
		mov ebx, 0x01			     //添加一个计数器
	_Loop:
	mov esi, [eax]
		cmp esi, 0x00
		je _No                       //如果是0的话就跳走                    
		cmp[ebp + 0x582c], esi
		je _Yes                      //如果UID匹配的话
		cmp ebx, 0xc8
		jge _no                      //如果计数器到100了就走跳
		add eax, 0x04                 //移动4个字节
		add ebx, 0x01                 //计数器加1
		jmp _Loop
	_Yes :
	popad
		mov al,0xa
		jmp gColor_pBackAddr2
	_No :
	popad
		mov al, byte ptr  [ebp + 0x5808]  //原始的语句
		jmp gColor_pBackAddr2
	}
}






//自动添删怪物、NPC
DWORD WINAPI AutoTask(_In_  LPVOID lpParameter)
{
	CEvent stcEvent;
	while (1)
	{
		WaitForSingleObject(stcEvent, 5000);//每5秒停一次
		//NPC
		for (vector<AUTONPC>::iterator it = g_vecAutoNpc.begin(); it != g_vecAutoNpc.end();)
		{
			DWORD  dwNpcid = it->dwNpcId;   //NPC ID
			DWORD  dwNpcType = it->dwNpcType; //NPC TYPE
			PVOID  pPos = &it->x;        //坐标指针
			PDWORD pMapTable = PDWORD(*PDWORD((*PDWORD(0x010a2018 + 4)) + 0x98 * it->byMap));//地图表,删除的时候需要的,也就是玩家基址+0xe0
			PDWORD pNpcTable = PDWORD(*pMapTable);                                           //NPC表,里面都是NPC对象,创建NPC的时候需要
			//默认1为删除NPC
			if (it->bFlag)
			{
				BYTE byHour = _tcstoul(it->szDeathHours, 0, 10);
				BYTE byMin = _tcstoul(it->szDeathMins, 0, 10);
				CTime objTime = CTime::GetCurrentTime();       //获取系统时间
				if (objTime.GetHour() == byHour&&objTime.GetMinute() == byMin)
				{
					//remove npc
					int    nTempX = it->x / 40;//坐标1
					int    nTempZ = it->z / 40;//坐标2

					_asm
					{
						push pPos                   //坐标指针
							push 1                      //固定1
							push dwNpcid        //id
							push dwNpcType      //type
							push pNpcTable              //pNpcTable
							mov ecx, nTempX
							mov eax, nTempZ
							call g_pDeleteNpcCall
					}
					//如果不循环,只是一次性的就删除掉这个
					if (!it->bLoop)
					{
						it = g_vecAutoNpc.erase(it);       //删除后把下一个元素给it
						continue;
					}
					it->bFlag = !it->bFlag;//改标志位
				}
			}
			//0为添加NPC
			else
			{
				BYTE byHour = _tcstoul(it->szBornHours, 0, 10);
				BYTE byMin = _tcstoul(it->szBornMins, 0, 10);
				CTime objTime = CTime::GetCurrentTime();       //获取系统时间
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
					it->bFlag = !it->bFlag;//改标志位
				}
			}
			it++;
		}

		//Monster
		for (vector<AUTOMONSTER>::iterator it = g_vecAutoMonster.begin(); it != g_vecAutoMonster.end();)
		{
			DWORD  dwMobid = it->dwMobId;   //ID
			PVOID  pPos = &it->x;        //坐标指针
			PDWORD pMapTable = PDWORD(*PDWORD((*PDWORD(0x010a2018 + 4)) + 0x98 * it->byMap));//
			PDWORD pNpcTable = PDWORD(*pMapTable);                                           //好像这个才是地图对象
			//默认1为删除
			if (it->bFlag)
			{
				BYTE byHour = _tcstoul(it->szDeathHours, 0, 10);
				BYTE byMin = _tcstoul(it->szDeathMins, 0, 10);
				CTime objTime = CTime::GetCurrentTime();       //获取系统时间
				if (objTime.GetHour() == byHour&&objTime.GetMinute() == byMin)
				{
					//remove
					int    nTempX = it->x / 40;//坐标1
					int    nTempZ = it->z / 40;//坐标2
					BYTE   bySuccess = 0;
					_asm
					{
					_loop:
						push pPos             //浮点数坐标指针
							mov edx, nTempX       //坐标1
							mov ecx, nTempZ       //坐标2
							push 0x1              //怪物数量
							push dwMobid          //怪物ID
							push pNpcTable        //地图对象
							call g_pDeleteMobCall //调用call
							cmp al, 0x2
							je _loop
					}

					//如果不循环,只是一次性的就删除掉这个
					if (!it->bLoop)
					{
						it = g_vecAutoMonster.erase(it);       //删除后把下一个元素给it
						continue;
					}
					it->bFlag = !it->bFlag;//改标志位

				}
			}
			//0为添加NPC
			else
			{
				BYTE byHour = _tcstoul(it->szBornHours, 0, 10);
				BYTE byMin = _tcstoul(it->szBornMins, 0, 10);
				CTime objTime = CTime::GetCurrentTime();        //获取系统时间
				if (objTime.GetHour() == byHour&&objTime.GetMinute() == byMin)
				{
					_asm
					{
						mov eax, 0x1    //怪物数量
							mov ecx, dwMobid        //怪物ID
							push pNpcTable         //地图对象
							mov ebx, pPos           //坐标
							call g_pCreateMobCall  //调用CALL
					}
					it->bFlag = !it->bFlag;    //改标志位
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
		//取UID
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

		//传送门限制
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
			//首先得计算出有这个地图有多少张重复的才能去随机
			int nCount = 1;
			int nStart = i;
			for (UINT j = i + 1; j < gRandom_vecRandomInfo.size(); j++)
			{
				if (gRandom_vecRandomInfo[j].nMapid == gRandom_vecRandomInfo[i].nMapid)
				{
					nCount++;        //计数增加
					i++;             //因为有相同地图,下次循环从不同地图开始
				}
			}
			//执行一系列操作
			srand(GetTickCount());
			BYTE byRadomIndex = rand() % nCount + nStart; //随机数=rand()%地图数量+起始坐标
			//拿出随机到的数据
			RandomSpawn stcTemp = gRandom_vecRandomInfo[byRadomIndex];
			//下面是计算地址了
			PVOID pAddr1 = NULL;
			PVOID pAddr2 = NULL;
			//读出里的地址[587960]
			ReadProcessMemory(GetCurrentProcess(), gRandom_pBaseAddr, &pAddr1, 4, NULL);
			//读出里的地址[[587960]+c]
			pAddr1 = PVOID((DWORD)pAddr1 + 0xc);
			ReadProcessMemory(GetCurrentProcess(), pAddr1, &pAddr2, 4, NULL);
			//取得最终地址[[587960]+c]+188+208*地图数
			pAddr2 = PVOID((DWORD)pAddr2 + 0x188 + 0x208 * stcTemp.nMapid);
			//写入光明的坐标
			MyWriteProcessMemory(GetCurrentProcess(), pAddr2, &stcTemp.fPosx, 4, NULL);                  //x
			MyWriteProcessMemory(GetCurrentProcess(), PVOID((DWORD)pAddr2 + 4), &stcTemp.fPosy, 4, NULL);//y
			MyWriteProcessMemory(GetCurrentProcess(), PVOID((DWORD)pAddr2 + 8), &stcTemp.fPosz, 4, NULL);//z
			//写入愤怒的坐标
			pAddr2 = PVOID((DWORD)pAddr2 + 0x10);
			MyWriteProcessMemory(GetCurrentProcess(), pAddr2, &stcTemp.fPosx, 4, NULL);                  //x
			MyWriteProcessMemory(GetCurrentProcess(), PVOID((DWORD)pAddr2 + 4), &stcTemp.fPosy, 4, NULL);//y
			MyWriteProcessMemory(GetCurrentProcess(), PVOID((DWORD)pAddr2 + 8), &stcTemp.fPosz, 4, NULL);//z


		}
	}
	return 0;
}


//用来执行泡点、泡战功
VOID CALLBACK AddPointPerMin(_In_  HWND hwnd, _In_  UINT uMsg, _In_  UINT_PTR idEvent, _In_  DWORD dwTime)
{
	CString  szMap = L"42";  //地图
	DWORD dwPoint = 1;     //加的点卡数
	PLAYERINFO stcPlayerInfo;
	for (DWORD i = 0; i < _countof(g_dwPlayerBaseAddr) && g_dwPlayerBaseAddr[i] != 0; i++)
	{
		if (GetPlayerInfoByBaseAddr(g_dwPlayerBaseAddr[i], stcPlayerInfo))//根据基址获取玩家各项信息
		{
			//判断地图是否符合
			if (szMap.Compare(stcPlayerInfo.szMap) == 0)  
			{
				//提取玩家账号
				DWORD dwUid=_tcstoul(stcPlayerInfo.szUid, 0, 10);
				CString szSql;
				szSql.Format(L"update ps_userdata.dbo.users_master set point=point+%d where useruid=%d", dwPoint, dwUid);
			//	g_DataBaseObj.ExeSqlByCommand(szSql);
			}
		}
	}
}






//初始信息
void InitHook()
{

	CEvent objEvent;
	WaitForSingleObject(objEvent, 5000); //暂停,让程序读完地图与其它内容再说

// 	if (!g_DataBaseObj.LinkDataBase())
// 	{
// 		OutputDebugString(L"connect db failed");
// 		ExitProcess(0);
// 	}   

	g_pUseSkill =  UseSkill;
	g_pfnLimitTransferForParty = LimitTransferForParty;

	//扫描获取玩家基址
	ScanPlayerInfo();                     

	//读取配置信息
	LoadConfig();

	//泡点、泡战功
	BYTE byTemp = 0x2;
	BYTE byTemp1 = 0x1;
	BYTE byTemp2 = 0xeb;
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0042509c, &byTemp, 1); //更改删怪call里的一个值
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004942a0, &byTemp1, 1);//更改刺客隐身保持原速
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00476f66, &byTemp2, 1);//更改刺客隐身保持原速


	//强化上限修改
	EnchantAdd();           

	//属性卷轴
	RecRune();

	//复活保护药水
	//先检测一下是否需要hook
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

	//消耗物品进地图
	if ((g_wMap&&g_dwItemid)
		||g_wMap1)
	{
		CMyInlineHook objHook6;
		objHook6.Hook((PVOID)0x00476792, Naked, 6);
	}

	
	//这个应该用处不大,暂时不需要用
	//SetTimer(0, 1, 60000, AddPointPerMin);

	//自动NPC跟怪物任务
	if (g_vecAutoMonster.size() || g_vecAutoNpc.size())
	{
		::CreateThread(0, 0, AutoTask, 0, 0, 0);
	}

	//自动复活任务
	if (gRandom_vecRandomInfo.size())
	{
		::CreateThread(0, 0, TimerProc_Random, 0, 0, 0);
	}

	

	//100%强化石
	CMyInlineHook objHook7;
	objHook7.Hook((PVOID)0x0046cd3f, Naked_CustomEnchantRate, 6);

	//读取VIP颜色的功能
	::CreateThread(0, 0, TimerProc_Dynamic, 0, 0, 0);
	Sleep(5000);
	g_HookObj_Color.Hook((PVOID)0x0042a496, Naked_Color, 6);
	g_HookObj_Color1.Hook((PVOID)0x0041c7e6, Naked_Color1, 6);
	gColor_pBackAddr2 = (PVOID)g_HookObj_Color2.Hook((PVOID)0x0047fd52, Naked_Color2,6);


	
}


void Hook_VipColor(BOOL nSwitch,DWORD dwUid[])
{
	//如果要开启功能的话
	if(nSwitch)
	{   //把UID拷贝一下
		memcpy_s(gColor_dwUid,sizeof(DWORD)*100,dwUid,sizeof(DWORD)*100);
		g_HookObj_Color.Hook((PVOID)0x0042a496, Naked_Color, 6);
	}
	//如果要关闭功能的话
	else
	{
		g_HookObj_Color.UnHook();
	}
}

//执行GM指令CALL
void Call_GmCommand(BYTE* pbyCommand, DWORD dwBaseAddr)
{
	gGmCall_dwPlayerBase = dwBaseAddr; //玩家对象
	gGmCall_pbyCommand = pbyCommand;   //指令
	Naked_GmCommandCall();             //上两个参数传到全局变量
}



//HOOK获取基址
void ScanPlayerInfo()
{
	ZeroMemory(g_dwPlayerBaseAddr,_countof(g_dwPlayerBaseAddr)*sizeof(DWORD));
	gBaseAddr_pAddr = g_dwPlayerBaseAddr;
	CMyInlineHook HookObj_GetPlayerBaseAddr;
	HookObj_GetPlayerBaseAddr.Hook(gBaseAddr_pHookAddr, Naked_GetBaseAddr, 6);
}


//通过玩家基址获取玩家信息
BOOL GetPlayerInfoByBaseAddr(DWORD pBaseAddr,PLAYERINFO &stcPlayerInfo)
{
	DWORD dwTemp 	  =0;  	 //用来读取4字节整形数据的
	FLOAT flTemp      =0;    //用来读取浮点数
	BYTE  byTemp      =0;  	 //用来读取1字节数据的
	BYTE  byCharName[20]={0};//读取20个字符的角色名

	//找玩家UID:+0x582c
	memcpy_s(&dwTemp,4,PVOID(pBaseAddr+0x582c),4);
	swprintf_s(stcPlayerInfo.szUid,10,L"%d",dwTemp);
	if(dwTemp==0) return FALSE;

	//找玩家Charid:+0x128
	memcpy_s(&dwTemp,4,PVOID(pBaseAddr+0x128),4);
	swprintf_s(stcPlayerInfo.szCharid,10,L"%d",dwTemp);
	if(dwTemp==0) return FALSE;

	//找玩家等级:+0x136
	memcpy_s(&byTemp,1,PVOID(pBaseAddr+0x136),1);
	swprintf_s(stcPlayerInfo.szLevel,10,L"%d",byTemp);

	//找战功+148
	memcpy_s(&dwTemp,4,PVOID(pBaseAddr+0x148),4);
	swprintf_s(stcPlayerInfo.szKills,10,L"%d",dwTemp);

	//找posx:+0xd0
	memcpy_s(&flTemp,4,PVOID(pBaseAddr+0xd0),4);
	swprintf_s(stcPlayerInfo.szPosx,10,L"%.0f",flTemp);

	//找posy:+0xd4
	memcpy_s(&flTemp,4,PVOID(pBaseAddr+0xd4),4);
	swprintf_s(stcPlayerInfo.szPosy,10,L"%.0f",flTemp);

	//找posz:+0xd8
	memcpy_s(&flTemp,4,PVOID(pBaseAddr+0xd8),4);
	swprintf_s(stcPlayerInfo.szPosz,10,L"%.0f",flTemp);

	//角色名:+0x184 
	memcpy_s(&byCharName,_countof(byCharName),PVOID(pBaseAddr+0x184),_countof(byCharName));
	c2w(stcPlayerInfo.szCharName,20,(char*)byCharName);
	
	//角色地图
	byTemp = *PBYTE(pBaseAddr + 0x160);
	swprintf_s(stcPlayerInfo.szMap, 10, L"%d", byTemp);

	//角色基址:
	swprintf_s(stcPlayerInfo.szBaseAddr,10,L"%x",pBaseAddr);

	return TRUE;
}


//更新物品
void Change_UpdateItem(DWORD dwItemid)
{

// 	//1.计算物品地址
// 	DWORD dwItemType = dwItemid / 1000;//取Type
// 	DWORD dwId = dwItemid % 1000;//取ID
// 	PVOID pItemAddress = PVOID((dwItemType * 0x100 + dwId) * 0xa8 + 0x00589674); //计算物品地址
// 	CString szSql;
// 	szSql.Format(L"select top 1 * from ps_gamedefs.dbo.items where itemid=%d", dwItemid);
// 
// 	//2.从数据库获取各项物品信息
// //	if (!g_DataBaseObj.LinkDataBase())return;
// 		
// 	//3.从内存中读取镜像
// 	BYTE byData[0x88] = { 0 };
// 	ZeroMemory(byData, sizeof(BYTE)*_countof(byData));
//	::ReadProcessMemory(GetCurrentProcess(), pItemAddress, byData, _countof(byData), NULL);//保存老数据
// 	//4.从数据库中读取内容,并修改
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
// 	//5.把修改好的写入内存
//	MyWriteProcessMemory(GetCurrentProcess(), pItemAddress, byData, _countof(byData), NULL	
}



void EnchantAdd()
{
	//1.判断是否默认的强化
	if (g_byEnchantMax == 20) return;

	//2.修改强化
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046ccc1, &g_byEnchantMax, 1, NULL);//两处地方要修改上限
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004d28d2, &g_byEnchantMax, 1, NULL);

	//3.从文件中读取数植
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

	DWORD dwAddrOfRate = (DWORD)g_stcEnchantRate;     //取一下首地址
	DWORD dwAddrOfValue = (DWORD)g_stcEnchantAddValue;//取一下首地址

	//4.往内存里修改偏移
	DWORD dwValueAddr[] = { 0x004d28fa, 0x0046cd87, 0x0046cda3, 0x004d28e0, };                    //修改4处

	for (DWORD i = 0; i < _countof(dwValueAddr); i++)
	{
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)dwValueAddr[i], &dwAddrOfValue, 4, NULL);//修改每阶段数值
	}

	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046cd3b, &dwAddrOfRate, 4, NULL);         //修改机率
}


void LoadConfig()
{
	//设置路径
	WCHAR szTemp[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szTemp);
	 g_szFilePath = szTemp;
	 g_szFilePath = g_szFilePath + L"\\config.ini";

	//读取强化
	 g_byEnchantMax = GetPrivateProfileInt(L"MaxEnchant", L"Max", 0, g_szFilePath);

	//读属性卷轴
	 g_bRecRuneOverlay = GetPrivateProfileInt(L"CustomRecRune", L"CoverOldStats", 0, g_szFilePath);

	//读自动NPC
	for (DWORD i = 1; i < 20; i++)
	{
		CString szKey;                   //读取项
		WCHAR   szReturn[MAX_PATH] = {0};
		CString szTemp[9];
		szKey.Format(L"%d", i);
		if (GetPrivateProfileString(L"AutoNpc", szKey, L"", szReturn, MAX_PATH, g_szFilePath) <= 0) break;

		//切割出字符串出来
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

	//读取复活点
	do 
	{
		//读地图
		int nMapid = GetPrivateProfileInt(L"RandomRespawn1", L"Mapid", 0, g_szFilePath);//地图
		if (!nMapid) break;

		//读坐标
		for (DWORD i = 1; i <= 20; i++)       //支持4个复活点
		{
			RandomSpawn stcRes;
			stcRes.nMapid = nMapid;

			CString szKey;                  
			WCHAR   szReturn[MAX_PATH] = { 0 };
			CString szTemp[3];               //切3部分
			szKey.Format(L"pos%d", i);
			if (GetPrivateProfileString(L"RandomRespawn1", szKey, L"", szReturn, MAX_PATH, g_szFilePath) <= 0) break;

			//切割出字符串出来
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

	//读取复活点
	do
	{
		//读地图
		int nMapid = GetPrivateProfileInt(L"RandomRespawn2", L"Mapid", 0, g_szFilePath);//地图
		if (!nMapid) break;

		//读坐标
		for (DWORD i = 1; i <= 4; i++)       //支持4个复活点
		{
			RandomSpawn stcRes;
			stcRes.nMapid = nMapid;

			CString szKey;
			WCHAR   szReturn[MAX_PATH] = { 0 };
			CString szTemp[3];               //切3部分
			szKey.Format(L"pos%d", i);
			if (GetPrivateProfileString(L"RandomRespawn2", szKey, L"", szReturn, MAX_PATH, g_szFilePath) <= 0) break;

			//切割出字符串出来
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

	//读取怪物
	for (DWORD i = 1; i < 20; i++)
	{
		CString szKey;                   //读取项
		WCHAR   szReturn[MAX_PATH] = { 0 };
		CString szTemp[9];
		szKey.Format(L"%d", i);
		if (GetPrivateProfileString(L"AutoMonster", szKey, L"", szReturn, MAX_PATH, g_szFilePath) <= 0) break;

		//切割出字符串出来
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

	//读取:是否需要复活后使用无敌
	g_bRespawnProtected = GetPrivateProfileInt(L"RespawnProtected", L"Enable", 0, g_szFilePath);
	g_dwProtectedSkillid = GetPrivateProfileInt(L"RespawnProtected", L"Skillid", 0, g_szFilePath);
	g_dwProtectedSkillLevel = GetPrivateProfileInt(L"RespawnProtected", L"Skilllevel", 0, g_szFilePath);

	//读取:是否需要指定地图用ID进
	g_dwItemid = GetPrivateProfileInt(L"Transfer", L"ItemId", 0, g_szFilePath);
	g_wMap = GetPrivateProfileInt(L"Transfer", L"Mapid", 0, g_szFilePath);
	
	//读取:是否需要指定地图限制单人进
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
	//先让标识位全部可用
	for (DWORD i = 0; i < 9; i++)
	{
		pSignature[i] = 0xff;
	}

	//如果是武器或是首饰的话,让血黄蓝不可洗
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

	//1.设置属性卷轴的偏移
	DWORD dwOffset = byTimes * 2;
	if (byTimes <= 5)
		dwOffset += 0x70;               //如果是常规属性就这个开头
	else
		dwOffset += 0x5e;              //如果是血典蓝就是这个开头

	//2.先读出里面的属性
	BYTE byRecRuneValue = *PBYTE((DWORD)pRecRune + dwOffset);// from rec rune
	BYTE byItemValue = *pAddr;//物品洗之前的附加属性


	switch (g_bRecRuneOverlay)
	{
	case CoverAll:
		byItemValue = byRecRuneValue;  //一律覆盖
		break;

	case StatsPlus:
		byItemValue += byRecRuneValue;
		break;

	case CoverSpecify:
		if (byRecRuneValue)           //如果本身的值是0的话不覆盖
			byItemValue = byRecRuneValue;
		break;

	case PlusSpecifyAndRemoveOthers:
		if (!byRecRuneValue)
			byItemValue = byRecRuneValue; //是0就覆盖
		else
			byItemValue += byRecRuneValue; //否则就加上
		break;

	default:
		break;
	}

// 	if (g_bRecRuneOverlay)
// 	{
// 		//如果覆盖的话就把属性卷轴上的属性写入就行了
// 		if (byRecRuneValue)                         //如果本身的值是0的话不覆盖
// 			byItemValue = byRecRuneValue;
// 	}
// 	else
// 	{
// 		//假如不覆盖,那就累加
// 		byItemValue += byRecRuneValue;
// 	}
	
	if (byItemValue > byReqwis) byItemValue = byReqwis;
	if (byItemValue > 99) byItemValue = 99;

	//3.返回
	return byItemValue;

}


//获取技能基址
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


//使用技能
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
	//1.看看是否有组队要求
	do 
	{
		if (g_wMap1!=dwMap) break;   
		if (*(PDWORD((DWORD)pPlayer + 0x17f4)))
		{
			return FALSE;
		}
	} while (0);



	
	//2.消耗物品
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
	//不允许组队进
	DWORD dwParty = *(PDWORD((DWORD)pPlayer + 0x17f4));
	if (dwParty)
		return FALSE;
	else
		return TRUE;
}