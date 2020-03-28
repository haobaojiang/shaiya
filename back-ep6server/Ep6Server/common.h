
#include "E:/我自己的工具源码/ShaiyaFixProject/CustomGame/define.h"


//send custom message
void send_msg_toplayer(Pshaiya_player Player, PVOID pOrgianlPacket, DWORD dwSize, char* strAdditionalMsg);



//send packet to server
void server_process_packet(DWORD dwPlayer, PVOID pPacket);
void server_process_packet(WORD wCmd, DWORD dwPlayer, PVOID pPacket, DWORD dwSize);


//send packet to client
void __stdcall SendPacketToPlayer(DWORD dwPlayer, DWORD dwBuff, DWORD dwSize);
void __stdcall SendPacketToPlayer(WORD wCmd, DWORD dwPlayer, DWORD dwBuff, DWORD dwSize);
void send_packet_all(PBYTE pPacket, DWORD dwSize);
void send_packet_all(WORD wCmd, PBYTE pPacket, DWORD dwSize);

//get information
DWORD GetPlayerByCharName(char* strName);
DWORD GetPlayerByCharid(int nCharid);

Pshaiya_player GetLeader(Pshaiya_party p);