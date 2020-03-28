#include "stdafx.h"
#include "antiHacker.h"




namespace antiHacker
{
	CRITICAL_SECTION g_cs;
	CMyInlineHook g_objaddPlayer, g_objGetPacket, g_objRemovePlayer;
#define HEARTBEATTIME (180*1000)


	void lock() {
		EnterCriticalSection(&g_cs);
	}
	void unlock() {
		LeaveCriticalSection(&g_cs);
	}


	DWORD getNextCheckTime() {
		return GetTickCount() + HEARTBEATTIME;
	}

	void enableHeartBeat(Pshaiya50_player player) {

#ifdef _DEBUG
		LOGD << "enableHeartBeat:" << player->charname;
#endif
		ANTIHACKPACKET stcPacket;
		player->HeartbeatCheck.enable = true;
		player->HeartbeatCheck.NextCheckTime = getNextCheckTime();
		ZeroMemory(&stcPacket, sizeof(stcPacket));
		stcPacket.cmd = PacketOrder::AntiHacker;
		stcPacket.actiontype = Antibehavior::action_heartbeat;
		SendPacketToPlayer(player, &stcPacket, sizeof(stcPacket));
	}


	void __stdcall addPlayer(Pshaiya50_player player) {
		lock();
		enableHeartBeat(player);
		unlock();
	}

	



	bool __stdcall ProcessPacket(Pshaiya50_player player, PVOID p) {

		PANTIHACKPACKET packet = PANTIHACKPACKET(p);

		if (packet->cmd != PacketOrder::AntiHacker) {
			return false;
		}


		switch (packet->actiontype)
		{
		case action_heartbeat:
			player->HeartbeatCheck.NextCheckTime = getNextCheckTime();
			break;

		default:
			break;
		}

		return true;
	}


	void __stdcall removePlayer(Pshaiya50_player player)
	{
		ZeroMemory(&player->HeartbeatCheck, sizeof(player->HeartbeatCheck));
	}



	DWORD packetPassAddr = 0x004674dd;
	__declspec(naked) void Naked_getPacket()
	{
		_asm {
			cmp eax, 0xff00
			jl __orgianl
			pushad
			MYASMCALL_2(ProcessPacket, edi, edx)
			popad
			jmp packetPassAddr

			__orgianl :
			cmp eax, 0xFA00
				jmp g_objGetPacket.m_pRet
		}
	}


	__declspec(naked) void Naked_addPlayer()
	{
		_asm {
			pushad
			MYASMCALL_1(addPlayer, edi)
			popad
			push 0x71
			lea edx, dword ptr[esp + 0x14]
			jmp g_objaddPlayer.m_pRet
		}
	}




	__declspec(naked) void Naked_removePlayer()
	{
		_asm {
			pushad
			MYASMCALL_1(removePlayer, ecx);
			popad
				push esi
				mov esi, ecx
				cmp dword ptr ds : [esi + 0x18], 0x0
				jmp g_objRemovePlayer.m_pRet
		}
	}









	bool heartBeatTimeover(PDWORD pTime, DWORD deLay) {

		DWORD CurrentTime = GetTickCount();
		DWORD nextTime = *pTime;



		if (!nextTime) {
			*pTime = CurrentTime + deLay;
			return false;
		}


		if (nextTime > CurrentTime) {

			return false;
		}
		else {
			*pTime = 0;
			return true;
		}
	}


	void antiCheckLoop(void* p)
	{
		while (1)
		{
			lock();
			for (DWORD i = 0; i < _countof(g_players);i++) {


				Pshaiya50_player player = Pshaiya50_player(g_players[i]);
			
				if (!player)
					break;

				if(player->nSocket==SOCKET_ERROR||!player->dwObjMap)
					continue;



				//ÐÄÌø¼ì²â
				if (player->HeartbeatCheck.enable&& heartBeatTimeover(&(player)->HeartbeatCheck.NextCheckTime, HEARTBEATTIME)) {

					LOGD << "player:<<" << player->charname << ">>:heartBeat failed!";

					DisConnectPlayer(player);
					
					break;
				}
			}
			unlock();

			Sleep(500);
		}
	}






	void Start()
	{
		g_objaddPlayer.Hook(PVOID(0x0044a91b), Naked_addPlayer, 6);
		g_objGetPacket.Hook(PVOID(0x004674c9), Naked_getPacket, 5);
		g_objRemovePlayer.Hook(PVOID(0x004d43a0), Naked_removePlayer, 7);


		InitializeCriticalSection(&g_cs);
		_beginthread(antiCheckLoop, 0, 0);
	}
}





