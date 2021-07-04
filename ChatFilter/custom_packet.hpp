
#ifndef SHAIYA_CUSTOM_PACKET_HEADER
#define SHAIYA_CUSTOM_PACKET_HEADER


#include "stdafx.h"
#include "config.hpp"
#include "callBack.hpp"

namespace CustomPacket
{
	ShaiyaUtility::CMyInlineHook g_obj;
	
	void __stdcall process_packet(void* player,void * packet)
	{
		WORD cmd = *(reinterpret_cast<PDWORD>(packet));
		if(cmd==0xfe01)
		{
			std::string name = ShaiyaUtility::EP6::PlayerName(player);
			LOGD << "player used game hacker,name:" << name;
		}
	}
	
	DWORD failedAddr = 0x00474d6b;

	//004750D3 | 8B4C24 34                          | mov ecx,dword ptr ss:[esp+34]                         |
	DWORD successAddr = 0x004750D3;
	__declspec(naked) void  Naked()
	{
		_asm {
			cmp eax,0xfe00
			jl _org

			pushad
			MYASMCALL_2(process_packet,ebx,edi)
			popad
			jmp successAddr
			
			_org:
			cmp eax,0xfa00
			jne _failed
			jmp g_obj.m_pRet
			
			_failed:
			jmp failedAddr
		}
	}

	
	void start()
	{
		//004750AB | 0F85 BAFCFFFF                      | jne ps_game.474D6B                                    | should hoook here
		g_obj.Hook(0x004750AB, Naked, 6);
	}
}

#endif
