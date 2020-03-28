#include <windows.h>
#include "common.h"
#include "E:/我自己的工具源码/ShaiyaFixProject/CustomGame/define.h"


Pshaiya_player GetLeader(Pshaiya_party p)
{
	return Pshaiya_player(p->member[p->LeaderInx].player);
}


DWORD GetPlayerByCharid(int nCharid)
{
	DWORD dwRet = 0;
	do
	{
		if (!nCharid)
			break;

		DWORD dwCall = 0x00414CC0;
		_asm
		{
			push nCharid
				call dwCall
				mov dwRet, eax
		}
	} while (0);

	return dwRet;
}

DWORD GetPlayerByCharName(char* strName)
{
	DWORD dwRet = 0;
	DWORD dwTemp = (DWORD)strName;
	DWORD dwCall = 0x00414db0;
	_asm
	{
		mov eax,dwTemp
			call dwCall
			mov dwRet,eax
	}
	return dwRet;
}

void __stdcall SendPacketToPlayer(DWORD dwPlayer, DWORD dwBuff, DWORD dwSize)
{
	PVOID g_pSendPacketCall = (PVOID)0x004ED0E0;//发送封包的,很多地方需要用到
	_asm
	{
		mov ecx, dwPlayer
			push dwSize
			push dwBuff
			call g_pSendPacketCall
	}
}


void __stdcall SendPacketToPlayer(WORD wCmd,DWORD dwPlayer, DWORD dwBuff, DWORD dwSize)
{
	PBYTE pNewPacket = new byte[dwSize + 2];
	memcpy(pNewPacket, &wCmd, 2);           //header
	memcpy(&pNewPacket[2], (PVOID)dwBuff, dwSize);//detail
	SendPacketToPlayer(dwPlayer,(DWORD)pNewPacket, dwSize + 2);
	delete[] pNewPacket;
}

void send_packet_all(PBYTE pPacket, DWORD dwSize)
{
	DWORD dwEsi = *PDWORD(0x587960);
	DWORD dwSendAll = 0x00419120;
	if (dwEsi)
	{
		_asm
		{
			mov eax, dwSize   //length
				mov ecx, pPacket  //packet
				mov esi, dwEsi
				call dwSendAll
		}
	}
}

void send_packet_all(WORD wCmd, PBYTE pPacket, DWORD dwSize)
{
	PBYTE pNewPacket = new byte[dwSize + 2];
	memcpy(pNewPacket, &wCmd, 2);           //header
	memcpy(&pNewPacket[2], pPacket, dwSize);//detail
	send_packet_all(pNewPacket, dwSize + 2);
	delete[] pNewPacket;
}


void server_process_packet(DWORD dwPlayer, PVOID pPacket)
{
	DWORD dwTemp = (DWORD)pPacket;
	DWORD dwCall = 0x00474940;
	_asm{
		mov edx,dwTemp
		mov ecx,dwPlayer
		call dwCall
	}
}


void server_process_packet(WORD wCmd, DWORD dwPlayer, PVOID pPacket, DWORD dwSize)
{
	PBYTE pNewPacket = new byte[dwSize + 2];
	memcpy(pNewPacket, &wCmd, 2);           
	memcpy(&pNewPacket[2], pPacket, dwSize);
	server_process_packet(dwPlayer,pNewPacket);
	delete[] pNewPacket;
}


void send_msg_toplayer(Pshaiya_player Player,PVOID pOrgianlPacket, DWORD dwSize, char* strAdditionalMsg)
{
	DWORD dwNewSize = dwSize + strlen(strAdditionalMsg) + 1;
	PBYTE pNewPacket = new byte[dwNewSize];
	memcpy(pNewPacket, pOrgianlPacket, dwSize);
	strcpy((&PCHAR(pNewPacket)[dwSize]), strAdditionalMsg);
	SendPacketToPlayer((DWORD)Player, (DWORD)pNewPacket, dwNewSize);
	delete[] pNewPacket;
}