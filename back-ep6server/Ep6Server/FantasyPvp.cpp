
#include <string>
#include <vector>
#include <windows.h>
#include <MyInlineHook.h>
#include "common.h"
#include "FantasyPvp.h"


using std::vector;
using std::string;


bool g_bEnableFantasyPvp = false;
WORD g_wFantasyPvpMap = 0;
WORD g_wFantasyPvpIndex = 0;


extern vector<Packet_Processedfunction> g_vecPacketProcessfun;



namespace FantasyPvp
{
	CMyInlineHook obj_GetFantasyPvp, obj_SetObj;
	vector<DWORD> g_vecPlayers;
	DWORD g_dwObjSrc = 0;         //源map的信息,有map就直接可以获取得到
	DWORD g_dwObjDst = 0;          //要从第一个玩家那里获取
	bool g_IsFantasys = false;
	CRITICAL_SECTION g_csTradeProcess;
	CRITICAL_SECTION g_csTimeLeft;
	DWORD dwTemp = 0x00430260;
	DWORD __stdcall fun_getMapInfo();
	void ResetCustomMsgStats(Pshaiya_player player);
	HANDLE g_Event = NULL;


	vector<Pshaiya_player> g_vecTradeInfo;
	


	DWORD __stdcall fun_SetMap(Pshaiya_player,DWORD);
	PVOID g_pfunSetMap = fun_SetMap;

	Pshaiya_party g_pParty = NULL;
	Pshaiya_party g_pTargetParty = NULL;

	DWORD __stdcall fun_SetMap(Pshaiya_player Player, DWORD dwOldObjMap)
	{
		DWORD dwRet = dwOldObjMap;
		if (!g_pParty || !g_pTargetParty)
			return dwRet;

		if (Player->party == g_pParty || Player->party == g_pTargetParty)
		{
			//如果没有说明是第一个
			if (g_dwObjDst)
				dwRet = g_dwObjDst;
			Player->dwObjMap = dwRet;
			SetEvent(g_Event);
		}
			

		return dwRet;
	}




	//取出地图信息,并不是new出来的object
	DWORD __stdcall fun_getMapInfo(DWORD dwObjMap)
	{

		//取出来的是全局地图的一个信息,但不是地图object
		DWORD dwRet = 0;
		DWORD dwCall = 0x00430260;
		_asm
		{
			movzx eax, g_wFantasyPvpIndex
				mov ecx, dwObjMap         //0xe0
				mov ecx, dword ptr[ecx + 0x28]
				call dwCall
				mov dwRet, eax
		}
		return dwRet;
	}

	__declspec(naked) void  Naked_GetMapInfo()
	{
		_asm
		{
			cmp eax, 0xff
				jne _orginal
				mov eax,g_dwObjSrc
				jmp obj_GetFantasyPvp.m_pRet

			_orginal :
			call dwTemp
				jmp obj_GetFantasyPvp.m_pRet
		}
	}

	__declspec(naked) void  Naked_SetNewMap()
	{
		_asm
		{
			push ecx
				push edx
				push ebx
				push esp
				push ebp
				push esi
				push edi

				push eax
				push esi
				call g_pfunSetMap

				pop edi
				pop esi
				pop ebp
				pop esp
				pop ebx
				pop edx
				pop ecx
				lea ebp, dword ptr[esi + 0x6284]
				jmp obj_SetObj.m_pRet
		}
	}




	//线程处理交易的接口,判断是否我们的pvp消息
	DWORD WINAPI ThreadProc(_In_  LPVOID lpParameter)
	{
		while (1)
		{
			//处理pvp消息
			EnterCriticalSection(&g_csTradeProcess);
			for (vector<Pshaiya_player>::iterator iter = g_vecTradeInfo.begin(); iter < g_vecTradeInfo.end(); iter++)
			{
				Pshaiya_player Player = *iter;
				Pshaiya_player pTargetPlayer = NULL;
				official_enterFirePort packets;
				pTargetPlayer = Pshaiya_player(Player->CustomMsg.player);
				

				//看看是否有点ok
				if (!Player->CustomMsg.bEnable)
				{
				   //超时清除,或是循环遍历下一个	
					DWORD dwTime = GetTickCount();
					if (dwTime > Player->CustomMsg.dwTime)
						goto __end;
					else
						continue;
				}

				//把对方队员一个一个传送
				g_pParty = Player->party;
				g_pTargetParty = pTargetPlayer->party;
				if (!g_pParty || !g_pTargetParty)
					goto __end;
				{
					//先传送一个人过去,再传送其它人过去
					g_dwObjDst = NULL;
					packets.byMapIndex = 0xff;
					server_process_packet((DWORD)pTargetPlayer, &packets);
					WaitForSingleObject(g_Event, 10000);
					ResetEvent(g_Event);
					g_dwObjDst = pTargetPlayer->dwObjMap;

					//接下来传送所有人
					for (DWORD i = 0; i < g_pTargetParty->Count; i++)
					{
						Pshaiya_player _player = Pshaiya_player(g_pTargetParty->member[i].player);
						if (_player != pTargetPlayer&&_player)
						{
							server_process_packet((DWORD)_player, &packets);
							WaitForSingleObject(g_Event, 10000);
							ResetEvent(g_Event);
						}
					}
					for (DWORD i = 0; i < g_pParty->Count; i++)
					{
						Pshaiya_player _player = Pshaiya_player(g_pParty->member[i].player);
						if (_player)
						{
							server_process_packet((DWORD)_player, &packets);
							WaitForSingleObject(g_Event, 10000);
							ResetEvent(g_Event);
						}
					}
				}

			__end:
				g_pParty = NULL;
				g_pTargetParty = NULL;
				ResetCustomMsgStats(pTargetPlayer);
				ResetCustomMsgStats(Player);
				g_vecTradeInfo.erase(iter);
				break;
			}
			LeaveCriticalSection(&g_csTradeProcess);
			Sleep(500);
		}
		return 0;
	}

	//当说话内容给我们拦截以后
	void ChatProcess(Pshaiya_player player, char* szChat)
	{
		do
		{
			char* strChat = strstr(szChat, "invitepvp");

			if (strChat == NULL || strChat[0] == '\0')
				break;

			if (player->IsInPrivateMap)
				break;

			strChat += strlen("invitepvp");


			DWORD count = 0;
			char strTargetPlayer[50] = { 0 };
			sscanf(strChat, "%d %s", &count, strTargetPlayer);

			if (!count || strTargetPlayer[0] == '\0')
				break;


			Pshaiya_player player_target = (Pshaiya_player)GetPlayerByCharName(strTargetPlayer);
			if (!player_target || player_target == player)//不能自己跟自己发起
				break;


			//检测是否已经重复发消息了
			if (player->CustomMsg.MsgType)
				break;

			if (player_target->CustomMsg.MsgType)
				break;



			//检测双方人数,必须匹配才行
			Pshaiya_party pParty = player->party;
			if (!pParty)
				break;

			Pshaiya_party pTargetParty = player_target->party;
			if (!pTargetParty)
				break;
			if (pTargetParty->Count != count)
				break;

			//必须双方都是leader才行

			if (GetLeader(pParty) != player)
				break;

			if (GetLeader(pTargetParty) != player_target)
				break;

			//向对方的leader发送请求
			official_requestTrade stcTrade;
			char strMessage[50] = { 0 };
			sprintf(strMessage,"accept the battle from %s of %d vs %d !", player->charname, pParty->Count, count);
			send_msg_toplayer(player_target, &stcTrade.wCmd, sizeof(stcTrade), strMessage);

			//处理标志位
			DWORD dwTime = GetTickCount() + 1000 * 10; //加10秒

			player->CustomMsg.MsgType = Message_Type::PrivatePvp;
			player->CustomMsg.player = player_target;
			player->CustomMsg.dwTime = dwTime;
			player->CustomMsg.bEnable = FALSE;

			player_target->CustomMsg.MsgType = Message_Type::PrivatePvp;
			player_target->CustomMsg.player = player;
			player_target->CustomMsg.dwTime = dwTime;
			player_target->CustomMsg.bEnable = FALSE;
		
			//放进队列
			EnterCriticalSection(&g_csTradeProcess);
			g_vecTradeInfo.push_back(player);
			g_vecTradeInfo.push_back(player_target);
			LeaveCriticalSection(&g_csTradeProcess);

		} while (0);
	}

	void ResetCustomMsgStats(Pshaiya_player player)
	{
		if (!player)
			return;
		player->CustomMsg.MsgType = Message_Type::None;
		player->CustomMsg.player = NULL;
		player->CustomMsg.dwTime = 0;
		player->CustomMsg.bEnable = FALSE;
	}

	void TradeProcess(Pshaiya_player Player, DWORD dwOption)
	{
		//交给上面的线程处理,因为玩家的封包处理都是同步的,所以如果这里必须要让回调继续走下去
		if (dwOption == 0)
		{
			EnterCriticalSection(&g_csTradeProcess);
			for (DWORD i = 0; i < g_vecTradeInfo.size(); i++)
			{
				if (g_vecTradeInfo[i] == Player)
				{
					Player->CustomMsg.bEnable = TRUE;
				}
			}
			LeaveCriticalSection(&g_csTradeProcess);
		}

	}

	void Start()
	{
		InitializeCriticalSection(&g_csTradeProcess);
		g_Event=::CreateEvent(0, TRUE, 0, 0);
		obj_GetFantasyPvp.Hook((PVOID)0x0047675d, Naked_GetMapInfo, 5);
		obj_SetObj.Hook((PVOID)0x0041369e, Naked_SetNewMap, 6);
		
		//比如是map 47地图的存在玩家的0xe0数据
		g_dwObjSrc=fun_getMapInfo(*PDWORD(*PDWORD(0x010A201c) + g_wFantasyPvpMap * 0x98 + 0x4));
		::CreateThread(0, 0, ThreadProc, 0, 0, 0);
	}
}

























// 				if (Player->CustomMsg.MsgType != Message_Type::PrivatePvp)
// 					break;


// 				if (!pTargetPlayer)
// 					goto __end;



// 				//看看对方的对象是不是自己
// 				if (Pshaiya_player(pTargetPlayer->CustomMsg.player) != Player)
// 					goto __end;;
