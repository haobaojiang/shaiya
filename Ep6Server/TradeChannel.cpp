#include "stdafx.h"
#include "TradeChannel.h"



namespace TradeChannel
{

	typedef struct _TradeChat{
		DWORD dwPlayer;
		string strPackets;
	}TradeChat, *PTradeChat;

	CRITICAL_SECTION g_cs;
	vector<TradeChat> g_vecPlayer;
	DWORD g_dwGetChatlogReturnAddr = NULL;
	PVOID g_pfunGetChatlog = NULL;

	DWORD g_dwCmpFactionReturnAddr = NULL;
	DWORD g_dwCmpFactionReturnAddr2 = 0x00427944;
	DWORD g_dwCmpFactionReturnAddr3 = 0x004279ce;
	PVOID g_pfunCmpFaction = NULL;

	char* g_strLight = "[light]";
	char* g_strFury = "[fury]";




	void InsertStr(char* strNewbuff, DWORD dwSize, DWORD dwIndex, char* strInsert)
	{
		//1.先保存的buff
		char strTemp[MAX_PATH * 2] = { 0 };
		ZeroMemory(strTemp, sizeof(strTemp));
		memcpy(strTemp, strNewbuff, dwSize);

		//2.开始插入
		strNewbuff += dwIndex;
		memcpy(strNewbuff, strInsert, strlen(strInsert));
		strNewbuff += strlen(strInsert);
		memcpy(strNewbuff, &strTemp[2], dwSize - 2);
	}

	void DeleteStr(char* strNewbuff, DWORD dwSize, DWORD dwIndex, DWORD dwLen)
	{
		//1.先保存的buff
		char strTemp[MAX_PATH * 2] = { 0 };
		ZeroMemory(strTemp, sizeof(strTemp));
		memcpy(strTemp, strNewbuff, dwSize);

		//2.开始删除
		memcpy(&strNewbuff[dwIndex], &strTemp[dwIndex + dwLen], dwSize - dwIndex - dwLen);
	}

	BOOL __stdcall GetChatlog(DWORD dwPlayer, char* strPackets, BYTE byCountry, char* strFullPackets, DWORD dwSize)
	{
		BOOL bResult = FALSE;
		vector<TradeChat>::iterator iter;
		EnterCriticalSection(&g_cs);
		for (iter = g_vecPlayer.begin(); iter != g_vecPlayer.end(); iter++)
		{
			//找到玩家
			if (dwPlayer == iter->dwPlayer&&
				strPackets == iter->strPackets
				)
			{
				bResult = TRUE;
				g_vecPlayer.erase(iter);
				break;
			}
		}
		//如果没找到玩家,我们需要再发一个包
		if (!bResult)
		{
			//只能放进队列等另一个线程弄,因为会造成递归
			TradeChat stcTradeChat = { dwPlayer, strPackets };
			g_vecPlayer.push_back(stcTradeChat);
			LeaveCriticalSection(&g_cs);
		}
		//那么需要改包了
		else
		{
			LeaveCriticalSection(&g_cs);
			//申请大小
			char* strTemp = NULL;
			byCountry ? strTemp = g_strFury : strTemp = g_strLight;//判断光明还是愤怒
			char strNewBuff[MAX_PATH] = { 0 };
			ZeroMemory(strNewBuff, sizeof(strNewBuff));
			memcpy(strNewBuff, strFullPackets, dwSize);       //先完整拷贝
			dwSize = dwSize + strlen(strTemp);

			InsertStr(strNewBuff, dwSize, 2, strTemp);        //插入tag
			DeleteStr(strNewBuff, dwSize, strlen(strNewBuff) + 2, strlen(strTemp));//中间部分要去除一部分,否则会乱码
			//这条只发给GM
			PVOID pSendTrade = (PVOID)0x004192f0;
			DWORD dwTemp = (DWORD)strNewBuff;
			_asm
			{
				push 0x2
					mov eax, dwSize
					mov ecx, dwTemp
					call pSendTrade
			}
		}
		return bResult;

	}



	PVOID g_pBypass = (PVOID)0x00427944;
	PVOID g_pRefuse = (PVOID)0x004279ce;

	__declspec(naked) void  Naked_CmpFaction()
	{
		_asm
		{
			cmp byte ptr[ecx + 0x5808], 0xa
				jg __orginal                   //如果不是GM权限
				cmp dl, 0x2
				je __SuccessAddr                //只接收2阵营,因为2才是自己收的

				jmp g_dwCmpFactionReturnAddr3   //不接收0 和 1 阵营

			__SuccessAddr :

			jmp g_dwCmpFactionReturnAddr2


			__orginal :
			cmp dl, byte ptr[ecx + 0x12D]
				jmp g_dwCmpFactionReturnAddr
		}
	}

	PVOID g_pfunTemp = (PVOID)0x004192F0;
	__declspec(naked) void  Naked_GetChatlog()
	{
		_asm
		{
			pushad
				push eax                       //完整内容包大小
				push ecx                       //完整内容包
				push edx                       //阵营
				push ebx                       //简易封包
				push ebp                      //dwPlayer
				call g_pfunGetChatlog
				cmp al, 0x1
				jne __orginal
				popad
				add esp, 0x4
				jmp  g_dwGetChatlogReturnAddr
			__orginal :
			popad
				call g_pfunTemp                //原语句
				jmp g_dwGetChatlogReturnAddr
		}
	}


	DWORD WINAPI ThreadProc(PVOID p)
	{
		while (1)
		{
			Sleep(2000);
			DWORD dwPlayer = 0;
			char strPackets[MAX_PATH] = { 0 };
			ZeroMemory(strPackets, sizeof(strPackets));
			EnterCriticalSection(&g_cs);
			if (g_vecPlayer.size())
			{
				vector<TradeChat>::iterator iter;
				iter = g_vecPlayer.begin();
				dwPlayer = iter->dwPlayer;
				strcpy_s(strPackets, MAX_PATH, iter->strPackets.c_str());
			}
			LeaveCriticalSection(&g_cs);
			if (dwPlayer)
			{
				Sleep(2000);
				PVOID pTemp = (PVOID)0x0047F400;
				DWORD dwTemp = (DWORD)strPackets;
				_asm
				{
					push dwPlayer    //玩家
						mov edi, dwTemp  //封包
						mov ebx, edi
						call pTemp
				}
			}
		}
		return 0;
	}

	void StartTradeChannel()
	{
		InitializeCriticalSection(&g_cs);
		CMyInlineHook obj1((PVOID)g_hookAddr.tradeChannel1, Naked_GetChatlog, &g_dwGetChatlogReturnAddr);
		g_pfunGetChatlog = GetChatlog;
		CMyInlineHook obj2((PVOID)g_hookAddr.tradeChannel2, Naked_CmpFaction, &g_dwCmpFactionReturnAddr, 6);
		::CreateThread(0, 0, ThreadProc, 0, 0, 0);
	}
}







