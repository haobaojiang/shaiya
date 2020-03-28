#include "stdafx.h"
#include "CustomPackets.h"






namespace CustomPackets
{
	void __stdcall PacketProcess(Pshaiya_player, PBYTE);
	PVOID g_pfuPacketProcess = PacketProcess;

	CMyInlineHook obj1;
	DWORD g_dwSuccessedAddr = 0x004750B9;
	__declspec(naked) void  Naked_PacketProcess()
	{
		_asm
		{
			cmp eax, 0xff00
				jl _orgianl
				pushad
				push edi   //packets
				push ebx   //player
				call g_pfuPacketProcess
				popad
				jmp g_dwSuccessedAddr
			_orgianl :
			cmp eax, 0xfa00
				jmp obj1.m_pRet
		}
	}

	

	void __stdcall PacketProcess(Pshaiya_player player, PBYTE pPackets)
	{

		WORD wCmd = *PWORD(pPackets);
		for (DWORD i = 0; i < g_vecPacketProcessfun.size(); i++)
		{
			Packet_Processedfunction stcPacketFunction = g_vecPacketProcessfun[i];
			//包头匹配则调用函数
			if (wCmd == stcPacketFunction.wCmd)
			{
#ifndef _DEBUG
				VMProtectBegin("PacketProcess");
#endif 
				stcPacketFunction.pFun(player, pPackets);
#ifndef _DEBUG
				VMProtectEnd();
#endif
				break;
			}
		}

	}

	void Start()
	{
		obj1.Hook((PVOID)0x004750A6, Naked_PacketProcess, 5);  //拦截自定义封包
	}
}
