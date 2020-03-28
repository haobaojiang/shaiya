// ItemMallFix.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "ItemMallFix.h"
#include  <MyInlineHook.h>
#include <MyPython.h>
#include <vector>
#include "shaiya_itemmall.h"
#include <common.h>

using std::vector;


CMyInlineHook Hookobjone;
CMyInlineHook HookobjTwo;
CMyInlineHook HookobjThree;
CMyInlineHook HookobjFour;




typedef struct _PURCHASE
{
	Pshaiya_player player;
	ITEMMALL_PURCHASE stc;
}PURCHASE;

vector<DWORD> g_vecPurchase;
vector<PURCHASE> g_vecGifts;

CRITICAL_SECTION g_cs;



MyPython* g_objPy = NULL;

ITEMMALLFIX_API int Start(void)
{
	return 0;
}

void save(DWORD dwUid, DWORD command);
void WINAPI fun_gift(Pshaiya_player player, char* CharName, char* ID);
void  WINAPI GetPoint(Pshaiya_player player);
bool WINAPI IsBuyAble(Pshaiya_player player, DWORD dwUsePoint);

//过图的时候显示点卡
__declspec(naked) void Naked_One()
{
	_asm
	{
		pushad
		MYASMCALL_1(GetPoint, ecx)
		popad
		xor ebp, ebp
		cmp byte ptr[esi + 0xA], 0x0
		jmp Hookobjone.m_pRet
	}
}

//检测点卡是否足够的,从数据库读一下
DWORD dwFalseAddr = 0x00488027;
__declspec(naked) void Naked_Two()
{
	_asm
	{
		pushad
		MYASMCALL_2(IsBuyAble, edi, ecx)
		cmp al, 0x1
		popad
		jne _false

		cmp ecx, dword ptr[edi + 0x5AC0]
		JMP HookobjTwo.m_pRet
		_false :
		jmp dwFalseAddr
	}
}

__declspec(naked) void Naked_Four()
{
	_asm
	{
		pushad
		mov eax, dword ptr[edi + 0x582c]
		push eax                        //uid
		call HookobjFour.m_pfun_call                //扣点并且清除缓存之类的
		popad
		jmp HookobjFour.m_pRet
	}
}


__declspec(naked) void Naked_Three()
{
	_asm
	{
		pushad
		lea eax, [esi + 0x2]
		lea ebp, [esi + 0x17]
		MYASMCALL_3(fun_gift, edi, ebp, eax)
		popad
		jmp HookobjThree.m_pRet
	}
}




void WINAPI fun_gift(Pshaiya_player player, char* CharName, char* ID)
{

	do
	{
		Pshaiya_player pTargetPlayer = (Pshaiya_player)GetPlayerByCharName(CharName);

		if (!pTargetPlayer)
			break;

		DWORD dwItemMall = getItemMallByMallCode(ID);
		if (!dwItemMall)
			break;

		DWORD pointNeeded = *PDWORD(dwItemMall + 0x90);

		DWORD Mallpoint = player->Mallpoint;

		if (Mallpoint < pointNeeded)
			break;

		//set points
		Mallpoint -= pointNeeded;


		g_objPy->lock();
		try
		{
			setPoint(player->uid, Mallpoint);
		}
		catch (...)
		{
			;
		}
		g_objPy->Unlock();


		player->Mallpoint = Mallpoint;

		g_objPy->lock();
		try
		{
			setPoint(pTargetPlayer->uid, pTargetPlayer->Mallpoint + pointNeeded);
		}
		catch (...)
		{
			;
		}
		g_objPy->Unlock();



		//send back point remain
		ITEMALL_QUERYPOINT stcQueryPoint;
		stcQueryPoint.point = player->Mallpoint;
		SendPacketToPlayer(player, &stcQueryPoint, sizeof(stcQueryPoint));

		PURCHASE stc;
		stc.player = pTargetPlayer;
		strcpy(stc.stc.ID, ID);
		EnterCriticalSection(&g_cs);
		g_vecGifts.push_back(stc);
		LeaveCriticalSection(&g_cs);


	} while (0);

}


void WINAPI Clear(DWORD dwUid)
{
	EnterCriticalSection(&g_cs);
	g_vecPurchase.push_back(dwUid);
	LeaveCriticalSection(&g_cs);
}




void  WINAPI GetPoint(Pshaiya_player player)
{
	player->Mallpoint = 0;
	g_objPy->lock();
	try
	{
		player->Mallpoint = getpoint(player->uid);
	}
	catch (...)
	{
		;
	}
	g_objPy->Unlock();
}



bool WINAPI IsBuyAble(Pshaiya_player player, DWORD dwUsePoint)
{

	//检测点数
	g_objPy->lock();
	try
	{
		player->Mallpoint = getpoint(player->uid);
	}
	catch (...)
	{
		;
	}
	g_objPy->Unlock();

	return player->Mallpoint >= dwUsePoint ? true:false;
}








DWORD WINAPI ThreadProc1(_In_  LPVOID lpParameter)
{
	g_objPy = new MyPython();

	initshaiya_itemmall();
	InitializeCriticalSection(&g_cs);


	Hookobjone.Hook((PVOID)0x0047d151, Naked_One, 6, GetPoint);     //进入地图时显示点卡
	HookobjTwo.Hook((PVOID)0x0048801f, Naked_Two, 6);     //显示点卡
	HookobjFour.Hook((PVOID)0x0048876f, Naked_Four, 5, Clear);      //保存修改,更改到ps_dbagent.exe
	HookobjThree.Hook(PVOID(0x0048885c), Naked_Three, 5, fun_gift);
	HookobjThree.m_pRet = (PVOID)0x00488D5F;
	HookobjFour.m_pRet = (PVOID)0x00488D5F;




	//让ps_dbagent.exe能使用上这个
	while (1)
	{
		Sleep(500);
		EnterCriticalSection(&g_cs);
		if (g_vecPurchase.size())
		{
			save(g_vecPurchase[0], 0x1b02000b);
			g_vecPurchase.erase(g_vecPurchase.begin());
		}

		if (g_vecGifts.size())
		{
			server_process_packet((DWORD)g_vecGifts[0].player, &g_vecGifts[0].stc);
			g_vecGifts.erase(g_vecGifts.begin());
		}

		LeaveCriticalSection(&g_cs);
	}
	return 0;
}

void save(DWORD dwUid, DWORD command)
{
	char buff[90] = { 0 };
	ZeroMemory(buff, sizeof(buff));

	//修改第5-8个字节,这是一个指针指向后面的
	*(PDWORD(&buff[4])) = DWORD(&buff[12]);

	//这是一个封包的command
	*(PDWORD(&buff[12])) = command;

	//添加账号id
	*(PDWORD(&buff[16])) = dwUid;

	DWORD dwTempCall = 0x00406960;
	DWORD dwBuffAddr = (DWORD)buff;
	_asm
	{
		push dwBuffAddr
		mov ecx, 0
		call dwTempCall
	}
}