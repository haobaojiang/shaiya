
#include "stdafx.h"
#include "confrimTrade.h"




namespace confrimTrade
{

	CMyInlineHook g_obj;

	void __stdcall fun(Pshaiya_player player, PCONFRIMTRADE packet)
	{
	
		if (packet->cmd != official_packet_const::confrim_trade) {
			return;
		}

		CONFRIMTRADE st;
		DWORD len = sizeof(st);
		st._type = packet->_target;  //�������İ���ʵ����ֶ��ǲ�һ����

		st._target = 1; //��ʾ���Լ�

		SendPacketToPlayer(player, &st, sizeof(st));

		Pshaiya_player pTarget = player->processTarget;
		st._target = 2; //���Է�
		SendPacketToPlayer(pTarget, &st, sizeof(st));
	}

	__declspec(naked) void Naked()
	{
		_asm
		{
			pushad
			MYASMCALL_2(fun, ecx, edi)
			popad
			fstsw ax
			test ah, 0x5
			jmp g_obj.m_pRet
		}
	}


	void start()
	{
		g_obj.Hook((PVOID)0x0047e0d1, Naked, 9);
	}
}





