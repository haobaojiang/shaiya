
#include <iostream>
#include <Windows.h>





namespace Game {

	/*
	static DWORD ִ��GM����call = 0x470A10;
	static DWORD �˺Ż�ַ�ڴ��ַ = 0x40A182;
	static DWORD �˺Ż�ַ = 0x22258A0;
	static DWORD ����������ʾ��ַ = 0x21F22E0;
	static DWORD GMȨ�޻�ַ = 0x84F6AC;
	static DWORD ��ʾ����Ѫ�ڴ��ַ = 0x004FA554;
	//------------------------------ - ��Ϸ��ַ------------------------;
	// ƫ�� = 0xE;
	static DWORD ��Ϸ��ַ�ڴ��ַ = 0x40DC1E;
	static DWORD ��Ϸ��ַ = 0x7071E0;

	static DWORD ����Ѫƫ���ڴ��ַ = 0x4F6DBF;
	static DWORD ����Ѫƫ�� = 0x130;
	//------------------------------ - �س�����ַ------------------------;
	// ƫ�� = 0x16;
	static DWORD �س����ڴ��ַ = 0x46D55D;
	static DWORD �س�����ַ = 0x84EEE8;
	static DWORD �س�����ַ2 = 0x703B58;
	//------------------------------ - ��ݼ���ַ------------------------;
	// ƫ�� = 0xFFFFFFFC;
	static DWORD ��ݼ��ڴ��ַ = 0x40999D;
	static DWORD ��ݼ���ַ = 0x7070A0;
	//------------------------------ - ��ͼƫ��------------------------;
	//ƫ�� = 0xFFFFFFFC;
	static DWORD ��ͼ1��ƫ���ڴ��ַ = 0x4BB785;
	static DWORD ��ͼ1��ƫ�� = 0x5DC;
	//ƫ�� = 0x5;
	static DWORD ��ͼ2��ƫ���ڴ��ַ = 0x42FB20;
	static DWORD ��ͼ2��ƫ�� = 0x19C;
	//------------------------------ - ʹ����Ʒcall------------------------;
	// ƫ�� = 0x19;
	static DWORD ʹ����Ʒcall�ڴ��ַ = 0x4E7315;
	static DWORD ʹ����Ʒcall = 0x4C8C80;
	static DWORD ʹ����Ʒ��ַ�ڴ��ַ = 0x4E7310;
	static DWORD ʹ����Ʒ��ַ = 0x84F6A8;
	static DWORD ʹ����Ʒ��ȴ�ڴ��ַ = 0x4C8D2F;
	static DWORD ʹ����Ʒ��ȴ = 0x19978E0;
	//------------------------------ - ��ȡ��Ʒ����call------------------------;
	// ƫ�� = 0x11;
	static DWORD ��ȡ��Ʒ����call�ڴ��ַ = 0x461110;
	static DWORD ��ȡ��Ʒ����call = 0x461110;
	static DWORD ��ȡ��Ʒ��ƫ��1_Ʒ�� = 0x10C2;
	static DWORD ��ȡ��Ʒ��ƫ��2_���� = 0x10C1;
	static DWORD ��ȡ��Ʒ��ƫ��3 = 0x7894;
	//------------------------------ - ������ַ------------------------;
	// ƫ�� = 0xB;
	static DWORD ������ַ�ڴ��ַ = 0x45E70B;
	static DWORD ������ַ = 0x850769;
	//------------------------------ - ����Ʒcall------------------------;
	// ƫ�� = 0xFFFFFFF6;
	static DWORD ����Ʒcall�ڴ��ַ = 0x5872B0;
	static DWORD ����Ʒcall = 0x5872B0;
	//------------------------------ - Ŀ���쳣�ڴ��ַ------------------------;
	// ƫ�� = 0x0;
	static DWORD Ŀ���쳣�ڴ��ַ = 0x41C41C;
	//------------------------------ - ��������ƫ��------------------------;
	// ƫ�� = 0x2;
	static DWORD ��������ƫ���ڴ��ַ = 0x449779;
	static DWORD ��������ƫ�� = 0x13D89C;
	//------------------------------ - �����ַƫ��------------------------;
	//ƫ�� = 0xE;
	static DWORD ��ַ = 0x8470AC;
	static DWORD �����ַƫ���ڴ��ַ = 0x4409B8;
	static DWORD �����ַƫ�� = 0x13FECC;
	static DWORD ���ӵ����ڴ��ַ = 0x59FAA5;
	static DWORD ���ӵ���2�ڴ��ַ = 0x58DCCD;
	static DWORD ��ǽ�ڴ��ַ = 0x594885;
	static DWORD ��ǽ�ڴ��ַ2 = 0x59336A;
	static DWORD ��ǽ�ڴ��ַ_�ӽ����� = 0x5932AC;
	//------------------------------ - ����Ʒcall------------------------;
	//ƫ�� = 0x15;
	static DWORD ����Ʒcall�ڴ��ַ = 0x4F93E6;
	static DWORD ����Ʒcall = 0x587C30;
	//------------------------------ - ��ݼ�call------------------------;
	//ƫ�� = 0x14;
	static DWORD ��ݼ�call�ڴ��ַ = 0x4D6BF6;
	static DWORD ��ݼ�call = 0x4D5FC0;
	//------------------------------ - static DWORD ����call1------------------------;
	//ƫ�� = 0x0;
	static DWORD ����call1�ڴ��ַ = 0x41C390;
	static DWORD ����call1 = 0x41C390;
	//------------------------------ - static DWORD ����call2------------------------;
	//ƫ�� = 0x0;
	static DWORD ����call2�ڴ��ַ = 0x41E4E0;
	static DWORD ����call2 = 0x41E4E0;
	//------------------------------ - ����call��ַ------------------------;
	//ƫ�� = 0xFFFFFFFC;
	static DWORD ����call��ַ�ڴ��ַ = 0x438486;
	static DWORD ����call��ַ = 0x703418;
	//------------------------------ - ѡ��call------------------------;
	static DWORD ѡ��call�ڴ��ַ = 0x41ED11;
	static DWORD ѡ��call = 0x447DA0;
	static DWORD ѡ��call��ַ�ڴ��ַ = 0x41ECE7;
	static DWORD ѡ��call��ַ = 0x851A88;
	//------------------------------ - ��ǽ����ڴ��ַ------------------------;
	static DWORD ��ǽ����ڴ��ַ = 0x43852F;
	static DWORD ��ǽ����ڴ��ַ1 = 0x438579;
	static DWORD ��ǽ����ڴ��ַ2 = 0x4385C1;
	static DWORD ��ǽ����ڴ��ַ3 = 0x4385E1;
	static DWORD ��ǽ����ڴ��ַ4 = 0x43859D;
	//------------------------------ - �������ϰ��＼���ڴ��ַ------------------------;
	static DWORD ��ǽ����ڴ��ַ = 0x441394;
	static DWORD ��Զ�̼���call = 0x587BA0;
	static DWORD ����call = 0x587BA0;
	//------------------------------ - ������call------------------------;
	static DWORD ������call�ڴ��ַ = 0x44154C;
	static DWORD ������call = 0x587A50;
	static DWORD ������call_����ڴ��ַ = 0x587A20;
	static DWORD ������call_��� = 0x587A20;


	static DWORD ��������ǽ�ڴ��ַ = 0x4414F6;
	static DWORD ��·��ʾ��ַ�ڴ��ַ = 0x441557;
	static DWORD ��·��ʾ��ַ = 0x7070F0;
	static DWORD ѡ��ƫ�� = 0x160;
	static DWORD ��·��ʾ��ַ�ڴ��ַ = 0x44AF39;
	static DWORD ��·��ʾƫ��X = 0x148;
	static DWORD ��·��ʾƫ��Y = 0x150;
	//------------------------------ - ����call------------------------;
	static DWORD ����call�ڴ��ַ = 0x588050;
	static DWORD ����call = 0x588050;
	static DWORD ����call��ʾ�ڴ��ַ = 0x4B8557;
	static DWORD ����call��ʾ = 0x279;
	//------------------------------ - ������Ż�ȡ����ID------------------------;
	static DWORD ������Ż�ȡ����ID�ڴ��ַ = 0x43B7C0;
	static DWORD ������Ż�ȡ����IDcall = 0x43B7C0;
	static DWORD ������Ż�ȡ����ID��ַ�ڴ��ַ = 0x4407C2;
	static DWORD ������Ż�ȡ����ID��ַ = 0x21E6854;
	//------------------------------ - ��ȡ�������ֻ�ַ + ����״̬------------------------;
	static DWORD ��ȡ�������ֻ�ַ�ڴ��ַ = 0x4212D7;
	static DWORD ��ȡ�������ֻ�ַ = 0x856F3C;
	static DWORD ��ȡ��������call�ڴ��ַ = 0x4212DC;
	static DWORD ��ȡ��������call = 0x461FE0;
	//------------------------------ - ��ȡ����ʹ����ȴ��ַ------------------------;
	static DWORD ��ȡ����ʹ����ȴ��ַ�ڴ��ַ = 0x4B95C5;
	static DWORD ��ȡ����ʹ����ȴ��ַ = 0x21E685C;
	//------------------------------ - ��ȡ״̬��ַ------------------------;
	static DWORD ��ȡ״̬���ֻ�ַ�ڴ��ַ = 0x4B96B3;
	static DWORD ��ȡ״̬���ֻ�ַ = 0x21E68A4;
	static DWORD ��ȡ����״̬���ֻ�ַ�ڴ��ַ = 0x4B96DE;
	static DWORD ��ȡ����״̬���ֻ�ַ = 0x856F6C;
	//------------------------------ - ����������Ʒƫ��------------------------;
	static DWORD ����������Ʒƫ���ڴ��ַ = 0x43BB23;
	static DWORD ����������Ʒƫ�� = 0x13D88C;
	//------------------------------ - ��������call------------------------;
	static DWORD ��������call�ڴ��ַ = 0x587B60;
	static DWORD ��������call = 0x587B60;
	//------------------------------ - ��ȡ���������ڴ��ַ------------------------;
	static DWORD ��ȡ��������call�ڴ��ַ = 0x461650;
	static DWORD ��ȡ��������call = 0x461650;
	static DWORD ��ȡ��������call��ַ�ڴ��ַ = 0x553223;
	static DWORD ��ȡ��������call��ַ = 0x856F3C;
	static DWORD ��ȡ��������call��ַ2 �� ����_ʮ����ʮ(static DWORD ��ȡ��������call��ַ)
		//------------------------------ - ������Ұ��ַ------------------------;
		static DWORD ������Ұ��ַ�ڴ��ַ = 0x40F8A0;
	static DWORD ������Ұ��ַ = 0x21F1F90;
	//------------------------------ - ����call------------------------;
	static DWORD ����call�ڴ��ַ = 0x5889B0;
	static DWORD ����call = 0x5889B0;
	static DWORD ����call_��ͨ�ڴ��ַ = 0x5887C0;
	static DWORD ����call_��ͨ = 0x5887C0;
	static DWORD ����call_�л��ڴ��ַ = 0x588940;
	static DWORD ����call_�л� = 0x588940;
	static DWORD ����call_�����ڴ��ַ = 0x588A20;
	static DWORD ����call_���� = 0x588A20;
	//------------------------------ - ������Ʒcall------------------------;
	static DWORD ������Ʒcall�ڴ��ַ = 0x587BE0;
	static DWORD ������Ʒcall = 0x587BE0;
	//' ''��------------------------ǿ���ϳ�װ��------------------------;
		static DWORD ǿ��װ��call�ڴ��ַ = 0x587CF0;
	static DWORD ǿ��װ��call = 0x587CF0;
	static DWORD �ϳ�װ��call�ڴ��ַ = 0x587C80;
	static DWORD �ϳ�װ��call = 0x587C80;
	static DWORD ����װ��call�ڴ��ַ = 0x587260;
	static DWORD ����װ��call = 0x587260;
	static DWORD ���Ը���call = 0x00587D60;
	//------------------------------ - ȷ�Ͻ������call------------------------;
	static DWORD ȷ�Ͻ������call��ַ = 0x850764;
	static DWORD ȷ�Ͻ������callƫ�� = 0x238;
	static DWORD ȷ�Ͻ������call�ڴ��ַ = 0x48EFDD;
	static DWORD ȷ�Ͻ������call = 0x588350;
	static DWORD �ر�ȷ�Ͻ�����ӻ�ַ = 0x852ACC;
	static DWORD �����call�ڴ��ַ = 0x588320;
	static DWORD �����call = 0x588320;

	//------------------------------ - ��ͨ��ӱ�����ַ------------------------;
	static DWORD ��ͨ��ӱ�����ַ = 0x21E68B8;
	static DWORD ��ͨ��ӱ�����ַ�ڴ��ַ = 0x44BD7D;
	static DWORD ������ӱ�����ַ = 0x21E68CC;
	static DWORD ������ӱ�����ַ�ڴ��ַ = 0x505472;
	static DWORD �����������ƫ�� = 0x250;
	static DWORD �����������ƫ���ڴ��ַ = 0x418E8F;
	static DWORD ������Ҷӳ�Ȩ��ƫ�� = 0x2C0;
	static DWORD ������Ҷӳ�Ȩ��ƫ���ڴ��ַ = 0x44C063;
	static DWORD ������ҹ�������IDƫ�� = 0x1B8;
	static DWORD ������ҹ�������IDƫ���ڴ��ַ = 0x54EFD7;
	static DWORD ���������Ӫƫ�� = 0x276;  //' �������������call ���治Զ����ƫ��
	static DWORD ���������Ӫƫ���ڴ��ַ = 0x490FE0;

	//------------------------------ - ����call------------------------;
	static DWORD ����call�ڴ��ַ = 0x587A80;
	static DWORD ����call = 0x587A80;
	//------------------------------ - ������call------------------------;
	static DWORD ������call�ڴ��ַ = 0x587F40;
	static DWORD ������call = 0x587F40;
	//------------------------------ - ������call------------------------;
	static DWORD ������call�ڴ��ַ = 0x587F00;
	static DWORD ������call = 0x587F00;
	//------------------------------ - ��ǰ�Ի�NPC��ַ------------------------;
	static DWORD ��ǰ�Ի�NPC��ַ = 0x852AD4;
	static DWORD ��ǰ�Ի�NPC��ַ�ڴ��ַ = 0x48D40A;
	static DWORD ��ǰ�Ի�����ID = 0x856F0E;
	static DWORD ��ǰ�Ի�����ID�ڴ��ַ = 0x44387C;
	//------------------------------ - ��������call------------------------;
	static DWORD ��������call�ڴ��ַ = 0x5873B0;
	static DWORD ��������call = 0x5873B0;
	//------------------------------ - ʥ�⸴���Աcall------------------------;
	static DWORD ʥ�⸴���Աcall�ڴ��ַ = 0x4C902B;
	static DWORD ʥ�⸴���Աcall = 0x58AE40;
	//------------------------------------------------------ - ;
	static DWORD ������Ʒcall�ڴ��ַ = 0x5872E0;
	static DWORD ������Ʒcall = 0x5872E0;
	static DWORD ��ͼID��ַ = 0x850750;
	static DWORD �ӵ�call = 0x587330;
	static DWORD ���׺������� = 0x4782f1;


	HRESULT GetGamePid(__inout DWORD* Pid) {

		HRESULT hr = S_OK;
		*Pid = 0;

		auto callBack = [&](PROCESSENTRY32W entry) {

			if (wcsicmp(entry.szExeFile, L"shaiya.exe") == 0 ||
				wcsicmp(entry.szExeFile, L"game.exe") == 0)
			{
				*Pid = entry.th32ProcessID;
			}
		};

		hr = Utility::Process::EnumProcess(callBack);
		if (FAILED(hr)) {
			return hr;
		}

		return hr;
	}


	HRESULT GetGameVersion() {

		DWORD pid = 0;
		auto hr = GetGamePid(&pid);
		if (FAILED(hr)) {
			return hr;
		}
	}

	namespace Self {

		struct Info {
			BYTE padding[0x10];
			float x;    //0x10
			float y;
			float z;

			char name[20]; //0xf0

			BYTE padding_1[0x11];
			int hp;     //0x130
			int maxhp;
			int mp;
			int maxmp;
			int sp;
			int maxsp;
		};

		BYTE GetMap() {
		//	return Game::Process::Read(��ͼID��ַ);
		}
	}
	*/



}



#include "inventory.h"

int main(int , char** )
{	
	Game::Inventory::GetAllItems();
	return 0;
}