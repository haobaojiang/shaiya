#include "stdafx.h"
#include "tradecolor.h"




namespace TradeColor
{
	CMyInlineHook g_obj1;
	CMyInlineHook g_obj2;
	CRITICAL_SECTION g_cs;
	TRADECOLOR st;

	void Loop(void* p)
	{
		while (1)
		{
			EnterCriticalSection(&g_cs);
			ZeroMemory(&st, sizeof(st));
			DWORD i = 0;
			for (; i <200; i++)
			{
				WCHAR szKey[MAX_PATH] = { 0 };
				WCHAR szRet[MAX_PATH] = { 0 };
				swprintf_s(szKey, MAX_PATH, L"%d", i + 1);
				GetPrivateProfileString(L"tradecolor", szKey, L"", szRet, MAX_PATH, g_szFilePath);
				if (szRet[0] != L'\0') {
					swscanf_s(szRet, L"%d,%d", &st.colors[i].uid, &st.colors[i].rgb);
					st.colors[i].rgb|= 0xff000000;
				}
			}
			LeaveCriticalSection(&g_cs);
			Sleep(30 * 1000);
		}
	}


	void __stdcall fun(BYTE* pTrade, DWORD dwUid)
	{
		pTrade[0] = 0x0;
		pTrade[1] = 0x0;
		pTrade[2] = 0x0;
		EnterCriticalSection(&g_cs);
		for (DWORD i = 0; i < 200; i++){

			DWORD uid = st.colors[i].uid;

			if (!uid)
				break;

			if (uid == dwUid){
				DWORD color = st.colors[i].rgb;
				pTrade[0] = (color >> 16) & 0x0000ff;
				pTrade[1] = (color >> 8) & 0x0000ff;
				pTrade[2] = color & 0x0000ff;
				break;
			}
		}
		LeaveCriticalSection(&g_cs);
	}

	__declspec(naked) void  Naked_1()
	{
		_asm
		{
			add eax, 0x18
			lea ecx, dword ptr[esp + 0x20]
			pushad
			lea eax, [ecx + eax]    //交易说话内容的尾部
			mov ebx, [ebx + 0x578c]
			MYASMCALL_2(fun,eax,ebx)
			popad
			add eax, 0x3    //加3个字节放rgb
			jmp g_obj1.m_pRet
		}
	}

	__declspec(naked) void  Naked_2()
	{
		_asm
		{
			push ecx
			lea ecx, dword ptr[esp + 0x30]
			pushad
			lea eax, [ecx + eax]    //交易说话内容的尾部
			mov ebx, [esp + 0x30]      //player
			mov ebx, [ebx + 0x578c]  //uid
			MYASMCALL_2(fun,eax,ebx)
			popad
			add eax, 0x3    //加3个字节放rgb
			jmp g_obj2.m_pRet
		}
	}

	void enableTradeColor(void* p) {
		Pshaiya50_player player = Pshaiya50_player(p);
		WORD cmd = PacketOrder::Trade_color;
		SendPacketToPlayer(player, &cmd, sizeof(cmd));
	}

	void Start()
	{
		InitializeCriticalSection(&g_cs);
		g_obj1.Hook((PVOID)0x0047129d, Naked_1, 7);
		g_obj2.Hook((PVOID)0x0047088c, Naked_2, 5);

		g_vecOnloginCallBack.push_back(enableTradeColor);

		_beginthread(Loop, 0, 0);
	}
}




