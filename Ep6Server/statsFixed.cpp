
#include "stdafx.h"
#include "statsFixed.h"




namespace statsFixed
{

	CMyInlineHook g_obj;

	void __stdcall ShowStatsFix(PBYTE pPlayer)
	{
		//1.制作新包
		BYTE   byPacket[0x32] = { 0 };
		ZeroMemory(byPacket, sizeof(BYTE)*_countof(byPacket));
		*(PWORD)byPacket = 0x526;                             //命令是26 05
		PDWORD pPacket = (PDWORD)&byPacket[2];                //用来在下面的地方方便修改

															  //2.把玩家属性数据复制过去
		PDWORD pTotal = PDWORD(pPlayer + 0x121c);//总属性处
		PWORD  pExtend = PWORD(pPlayer + 0x16c);  //身上属性处

		pPacket[0] = *pTotal - *pExtend;
		pTotal++;
		pExtend++;

		//3.2 附加敏捷
		pPacket[4] = *pTotal - *pExtend;
		pTotal++;
		pExtend++;

		//3.3 附加智力
		pPacket[2] = *pTotal - *pExtend;
		pTotal++;
		pExtend++;

		//3.4 附加精神
		pPacket[3] = *pTotal - *pExtend;
		pTotal++;
		pExtend++;

		//3.5 附加体质
		pPacket[1] = *pTotal - *pExtend;
		pTotal++;
		pExtend++;

		//3.6 附加运
		pPacket[5] = *pTotal - *pExtend;
		pTotal++;
		pExtend++;


		DWORD dwPhAttackLow = *PDWORD(pPlayer + 0x13cc);                   //物攻下限:老的是13cc
		DWORD dwPhAttackHig = dwPhAttackLow + *PDWORD(pPlayer + 0x12e4);   //物攻上限
		pPacket[6] = dwPhAttackLow;
		pPacket[7] = dwPhAttackHig;

		DWORD dwMagicAttackLow = *PDWORD(pPlayer + 0x1424);                   //魔攻下限
		DWORD dwMagicAttackHig = dwMagicAttackLow + *PDWORD(pPlayer + 0x12e4);//魔攻上限
		pPacket[8] = dwMagicAttackLow;
		pPacket[9] = dwMagicAttackHig;

		pPacket[10] = *PDWORD(pPlayer + 0x13d4);                   //防御
		pPacket[11] = *PDWORD(pPlayer + 0x142c);                   //抵抗

		SendPacketToPlayer((DWORD)pPlayer, (DWORD)byPacket, 0x32);

	}

	__declspec(naked) void Naked_ShowStatsFix()
	{
		_asm
		{
			mov[esi + 0x1434], ecx //orginal code
			pushad
			MYASMCALL_1(ShowStatsFix,esi)
			popad
			jmp g_obj.m_pRet
		}
	}


	void start()
	{
		//2.显示属性处
		g_obj.Hook((PVOID)0x00460FFF, Naked_ShowStatsFix, 6);
	}
}





