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
		//1.�ȱ����buff
		char strTemp[MAX_PATH * 2] = { 0 };
		ZeroMemory(strTemp, sizeof(strTemp));
		memcpy(strTemp, strNewbuff, dwSize);

		//2.��ʼ����
		strNewbuff += dwIndex;
		memcpy(strNewbuff, strInsert, strlen(strInsert));
		strNewbuff += strlen(strInsert);
		memcpy(strNewbuff, &strTemp[2], dwSize - 2);
	}

	void DeleteStr(char* strNewbuff, DWORD dwSize, DWORD dwIndex, DWORD dwLen)
	{
		//1.�ȱ����buff
		char strTemp[MAX_PATH * 2] = { 0 };
		ZeroMemory(strTemp, sizeof(strTemp));
		memcpy(strTemp, strNewbuff, dwSize);

		//2.��ʼɾ��
		memcpy(&strNewbuff[dwIndex], &strTemp[dwIndex + dwLen], dwSize - dwIndex - dwLen);
	}

	BOOL __stdcall GetChatlog(DWORD dwPlayer, char* strPackets, BYTE byCountry, char* strFullPackets, DWORD dwSize)
	{
		BOOL bResult = FALSE;
		vector<TradeChat>::iterator iter;
		EnterCriticalSection(&g_cs);
		for (iter = g_vecPlayer.begin(); iter != g_vecPlayer.end(); iter++)
		{
			//�ҵ����
			if (dwPlayer == iter->dwPlayer&&
				strPackets == iter->strPackets
				)
			{
				bResult = TRUE;
				g_vecPlayer.erase(iter);
				break;
			}
		}
		//���û�ҵ����,������Ҫ�ٷ�һ����
		if (!bResult)
		{
			//ֻ�ܷŽ����е���һ���߳�Ū,��Ϊ����ɵݹ�
			TradeChat stcTradeChat = { dwPlayer, strPackets };
			g_vecPlayer.push_back(stcTradeChat);
			LeaveCriticalSection(&g_cs);
		}
		//��ô��Ҫ�İ���
		else
		{
			LeaveCriticalSection(&g_cs);
			//�����С
			char* strTemp = NULL;
			byCountry ? strTemp = g_strFury : strTemp = g_strLight;//�жϹ������Ƿ�ŭ
			char strNewBuff[MAX_PATH] = { 0 };
			ZeroMemory(strNewBuff, sizeof(strNewBuff));
			memcpy(strNewBuff, strFullPackets, dwSize);       //����������
			dwSize = dwSize + strlen(strTemp);

			InsertStr(strNewBuff, dwSize, 2, strTemp);        //����tag
			DeleteStr(strNewBuff, dwSize, strlen(strNewBuff) + 2, strlen(strTemp));//�м䲿��Ҫȥ��һ����,���������
			//����ֻ����GM
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
				jg __orginal                   //�������GMȨ��
				cmp dl, 0x2
				je __SuccessAddr                //ֻ����2��Ӫ,��Ϊ2�����Լ��յ�

				jmp g_dwCmpFactionReturnAddr3   //������0 �� 1 ��Ӫ

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
				push eax                       //�������ݰ���С
				push ecx                       //�������ݰ�
				push edx                       //��Ӫ
				push ebx                       //���׷��
				push ebp                      //dwPlayer
				call g_pfunGetChatlog
				cmp al, 0x1
				jne __orginal
				popad
				add esp, 0x4
				jmp  g_dwGetChatlogReturnAddr
			__orginal :
			popad
				call g_pfunTemp                //ԭ���
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
					push dwPlayer    //���
						mov edi, dwTemp  //���
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







