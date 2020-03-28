
#include <string>
#include <vector>
#include <windows.h>
#include <MyInlineHook.h>
#include "common.h"
#include "CustomPackets.h"

using std::vector;
using std::string;

vector<Packet_Processedfunction> g_vecPacketProcessfun;
vector<fun_TradeProcessCallBack> g_vecTradeProcess;

namespace CustomPackets
{
	void __stdcall PacketProcess(Pshaiya_player, PBYTE);
	void __stdcall TradeProcess(Pshaiya_player, DWORD dwUserOption);
	PVOID g_pfunTradeProcess = TradeProcess;
	PVOID g_pfuPacketProcess = PacketProcess;

	CMyInlineHook obj1, obj2;
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

	__declspec(naked) void  Naked_TradeHook()
	{
		_asm{
			pushad
				movzx eax, byte ptr[edi + 0x2]  //0是确定 1是否定
				push eax
				push ebx
				call g_pfunTradeProcess
				popad
				cmp dword ptr[ebx + 0x15c0], 0x2
				jmp obj2.m_pRet
		}
	}

	
	void __stdcall TradeProcess(Pshaiya_player player,DWORD dwUserOption)
	{
		for (DWORD i = 0; i < g_vecTradeProcess.size(); i++)
		{
			fun_TradeProcessCallBack pFun = g_vecTradeProcess[i];
			pFun(player, dwUserOption);
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
				stcPacketFunction.pFun(player, pPackets);
				break;
			}
		}
	}

	void Start()
	{
		obj1.Hook((PVOID)0x004750A6, Naked_PacketProcess, 5);  //拦截自定义封包
//		obj2.Hook((PVOID)0x0047da8d, Naked_TradeHook, 7);      //拦截利用交易接口的自定义封包
	}
}
