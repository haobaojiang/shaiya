
#include "stdafx.h"
#include "statsFixed.h"




namespace statsFixed
{

	CMyInlineHook g_obj;

	void __stdcall ShowStatsFix(PBYTE pPlayer)
	{
		//1.�����°�
		BYTE   byPacket[0x32] = { 0 };
		ZeroMemory(byPacket, sizeof(BYTE)*_countof(byPacket));
		*(PWORD)byPacket = 0x526;                             //������26 05
		PDWORD pPacket = (PDWORD)&byPacket[2];                //����������ĵط������޸�

															  //2.������������ݸ��ƹ�ȥ
		PDWORD pTotal = PDWORD(pPlayer + 0x121c);//�����Դ�
		PWORD  pExtend = PWORD(pPlayer + 0x16c);  //�������Դ�

		pPacket[0] = *pTotal - *pExtend;
		pTotal++;
		pExtend++;

		//3.2 ��������
		pPacket[4] = *pTotal - *pExtend;
		pTotal++;
		pExtend++;

		//3.3 ��������
		pPacket[2] = *pTotal - *pExtend;
		pTotal++;
		pExtend++;

		//3.4 ���Ӿ���
		pPacket[3] = *pTotal - *pExtend;
		pTotal++;
		pExtend++;

		//3.5 ��������
		pPacket[1] = *pTotal - *pExtend;
		pTotal++;
		pExtend++;

		//3.6 ������
		pPacket[5] = *pTotal - *pExtend;
		pTotal++;
		pExtend++;


		DWORD dwPhAttackLow = *PDWORD(pPlayer + 0x13cc);                   //�﹥����:�ϵ���13cc
		DWORD dwPhAttackHig = dwPhAttackLow + *PDWORD(pPlayer + 0x12e4);   //�﹥����
		pPacket[6] = dwPhAttackLow;
		pPacket[7] = dwPhAttackHig;

		DWORD dwMagicAttackLow = *PDWORD(pPlayer + 0x1424);                   //ħ������
		DWORD dwMagicAttackHig = dwMagicAttackLow + *PDWORD(pPlayer + 0x12e4);//ħ������
		pPacket[8] = dwMagicAttackLow;
		pPacket[9] = dwMagicAttackHig;

		pPacket[10] = *PDWORD(pPlayer + 0x13d4);                   //����
		pPacket[11] = *PDWORD(pPlayer + 0x142c);                   //�ֿ�

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
		//2.��ʾ���Դ�
		g_obj.Hook((PVOID)0x00460FFF, Naked_ShowStatsFix, 6);
	}
}





