// PetSystem.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include <afxwin.h>
#include <fstream>
#include "PetSystem.h"
#include <MyClass.h>
#include <MyInlineHook.h>
#include "DataBase.h"
#include  <vector>
#include "VMProtectSDK.h"
#include "Logger.h"
#include <common.h>


using namespace EP6CALL;
//#define _WINGSLOT 1


#define GetPlayItemAddr(dwPlayer,byBag,bySlot)  (*PDWORD(dwPlayer + 0x1c0 + byBag * 24 * 4 + bySlot * 4))

CMyInlineHook Hookobj_1, Hookobj_2, Hookobj_3_1, Hookobj_3_2, Hookobj_5, Hookobj_6, Hookobj_7, Hookobj_8, Hookobj_10, Hookobj_11, Hookobj_12, Hookobj_13, Hookobj_14, Hookobj_15, Hookobj_17, Hookobj_18_1, Hookobj_18_2, Hookobj_19, Hookobj_20, Hookobj_21, HookObj_22, HookObj_23, HookObj_24, HookObj_26, HookObj_27;

#define CHAR_TO_WCHAR(lpChar,lpW_Char) \
MultiByteToWideChar(CP_ACP,0,lpChar,-1,lpW_Char,_countof(lpW_Char));


#define GetItemAddr(byType,byTypeid) ((byType * 0x100 + byTypeid) * 0xa8 + 0x00589674))


CMyInlineHook g_HookObj_Setitem;
std::vector<PSETITEMDATA> g_vecSetItemArmor;
std::vector<PSETITEMDATA> g_vecSetitemAcc;


BOOL IsRandSuccessful(DWORD dwRate, DWORD dwRange)
{
	return dwRate >= GetRand(dwRange) ? TRUE : FALSE;  //�ڷ�Χ�ھ��ǳɹ�
}

_inline void AddsetItemstats(Pshaiya_player player, PSETITEMDATA pSetItem, DWORD index)
{
	if (!index)
		return;

	SETITEMSTATS pValue = pSetItem->dwValue[index];
	player->set_item.BackUpValue.Str += pValue.Str;
	player->set_item.BackUpValue.Dex += pValue.Dex;
	player->set_item.BackUpValue.Luc += pValue.Luc;
	player->set_item.BackUpValue.Mp += pValue.Mp;
	player->set_item.BackUpValue.Rec += pValue.Rec;
	player->set_item.BackUpValue.__Int_ += pValue.__Int_;
	player->set_item.BackUpValue.Wis += pValue.Wis;
	player->set_item.BackUpValue.Hp += pValue.Hp;
	player->set_item.BackUpValue.Sp += pValue.Sp;
	player->set_item.IsSetted = true;
}

void __stdcall fun_setStatsThroughtItems(Pshaiya_player player)
{
	if (player->set_item.IsSetted)
	{
		player->total_str -= player->set_item.BackUpValue.Str;
		player->total_dex -= player->set_item.BackUpValue.Dex;
		player->total__Int_ -= player->set_item.BackUpValue.__Int_;
		player->total_wis -= player->set_item.BackUpValue.Wis;
		player->total_rec -= player->set_item.BackUpValue.Rec;
		player->total_luc -= player->set_item.BackUpValue.Luc;
		player->Maxhp -= player->set_item.BackUpValue.Hp;
		player->Maxmp -= player->set_item.BackUpValue.Mp;
		player->Maxsp -= player->set_item.BackUpValue.Sp;
		player->set_item.IsSetted = false;
	}

	ZeroMemory(&player->set_item.BackUpValue, sizeof(player->set_item.BackUpValue));


	//1. ��ȡ��������ID
	DWORD dwItem[16] = { 0 };
	for (DWORD i = 0; i < 16; i++) {
		PplayerItemcharacterise pItem = player->BagItem[0][i];
		if (pItem) {
			dwItem[i] = pItem->ItemType * 1000 + pItem->Typeid;
		}
	}

	//2. ��������Ҫ��ID
	SETITEMDATA Armor;
	Armor.dwItems[0] = dwItem[0];
	Armor.dwItems[1] = dwItem[1];
	Armor.dwItems[2] = dwItem[2];
	Armor.dwItems[3] = dwItem[3];
	Armor.dwItems[4] = dwItem[4];

	SETITEMDATA Accessories;
	Accessories.dwItems[0] = dwItem[8];
	Accessories.dwItems[1] = dwItem[9];
	Accessories.dwItems[2] = dwItem[10];
	Accessories.dwItems[3] = dwItem[11];
	Accessories.dwItems[4] = dwItem[12];

	//���߼��
	for (DWORD i = 0; i < g_vecSetItemArmor.size(); i++)
	{
		PSETITEMDATA pSetItem = g_vecSetItemArmor[i];
		DWORD index = 0;
		for (DWORD j = 0; j < _countof(pSetItem->dwItems); j++)
		{
			if (pSetItem->dwItems[j] &&
				pSetItem->dwItems[j] == Armor.dwItems[j]) {
				index++;
			}
		}
		if (index) {
			AddsetItemstats(player, pSetItem, index - 1);
			break;
		}
	}

	//���μ��
	for (DWORD i = 0; i < g_vecSetitemAcc.size(); i++)
	{
		PSETITEMDATA pSetItem = g_vecSetitemAcc[i];
		DWORD index = 0;

		if (pSetItem->dwItems[0] &&
			pSetItem->dwItems[0] == Accessories.dwItems[0]) //����
			index++;

		if (pSetItem->dwItems[1] &&
			pSetItem->dwItems[1] == Accessories.dwItems[1])//��ָ
			index++;

		if (pSetItem->dwItems[1] &&
			pSetItem->dwItems[1] == Accessories.dwItems[2])//��ָ
			index++;

		if (pSetItem->dwItems[2] &&
			pSetItem->dwItems[2] == Accessories.dwItems[3])//����
			index++;

		if (pSetItem->dwItems[2] &&
			pSetItem->dwItems[2] == Accessories.dwItems[4])//����
			index++;

		if (index) {
			AddsetItemstats(player, pSetItem, index - 1);
			break;
		}
	}


	if (player->set_item.IsSetted)
	{
		player->total_str += player->set_item.BackUpValue.Str;
		player->total_dex += player->set_item.BackUpValue.Dex;
		player->total__Int_ += player->set_item.BackUpValue.__Int_;
		player->total_wis += player->set_item.BackUpValue.Wis;
		player->total_rec += player->set_item.BackUpValue.Rec;
		player->total_luc += player->set_item.BackUpValue.Luc;
		player->Maxhp += player->set_item.BackUpValue.Hp;
		player->Maxmp += player->set_item.BackUpValue.Mp;
		player->Maxsp += player->set_item.BackUpValue.Sp;

		if (player->dwObjMap)
		{
			HPINFO stc;
			stc.charid = player->Charid;
			//hp
			stc._type = 0;
			stc.value = player->Maxhp;
			send_packet_specific_area(player, &stc, sizeof(stc));
			//mp
			stc._type = 1;
			stc.value = player->Maxsp;
			send_packet_specific_area(player, &stc, sizeof(stc));

			//sp
			stc._type = 2;
			stc.value = player->Maxmp;
			send_packet_specific_area(player, &stc, sizeof(stc));
		}
	}
}

__declspec(naked) void Naked_Setitem()
{
	_asm
	{
		pushad
		MYASMCALL_1(fun_setStatsThroughtItems, esi)
		popad
		sub esp, 0x8
		fild dword ptr[esi + 0x1220]
		jmp g_HookObj_Setitem.m_pRet
	}
}

bool load_setItem()
{
	char strFile[MAX_PATH] = { 0 };
	GetModuleFileNameA(GetModuleHandle(0), strFile, MAX_PATH);
	PathRemoveFileSpecA(strFile);
	strcat(strFile, "\\data\\setitem.csv");


	FILE* pFile = fopen(strFile, "r");
	if (pFile == NULL)
		return false;

	char szLine[1024] = { 0 };
	fgets(szLine, 1024, pFile);
	while (!feof(pFile))
	{
		SETITEMSTATS temp = { 0 };
		PSETITEMDATA pSetitem = new SETITEMDATA;
		ZeroMemory(pSetitem, sizeof(SETITEMDATA));
		ZeroMemory(szLine, sizeof(szLine));
		fgets(szLine, 1024, pFile);
		DWORD id;
		char szTemp[6][MAX_PATH] = { 0 };
		sscanf_s(szLine, "%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,]",
			&id, szTemp[5], MAX_PATH,
			&pSetitem->dwItems[0], szTemp[0], MAX_PATH,
			&pSetitem->dwItems[1], szTemp[1], MAX_PATH,
			&pSetitem->dwItems[2], szTemp[2], MAX_PATH,
			&pSetitem->dwItems[3], szTemp[3], MAX_PATH,
			&pSetitem->dwItems[4], szTemp[4], MAX_PATH);

		if (strcmp(szTemp[0], "0") == 0 &&
			strcmp(szTemp[1], "0") == 0 &&
			strcmp(szTemp[2], "0") == 0 &&
			strcmp(szTemp[3], "0") == 0 &&
			strcmp(szTemp[4], "0") == 0
			)
		{
			continue;
		}


		for (DWORD i = 0; i < 5; i++)
		{
			sscanf_s(szTemp[i], "%d|%d|%d|%d|%d|%d|%d|%d|%d",
				&pSetitem->dwValue[i].Str,
				&pSetitem->dwValue[i].Dex,
				&pSetitem->dwValue[i].__Int_,
				&pSetitem->dwValue[i].Wis,
				&pSetitem->dwValue[i].Rec,
				&pSetitem->dwValue[i].Luc,
				&pSetitem->dwValue[i].Hp,
				&pSetitem->dwValue[i].Mp,
				&pSetitem->dwValue[i].Sp);


			

			//������û���ԣ��Ͳ�����һ���
			if (i)
			{
				if (memcmp(&pSetitem->dwValue[i], &temp, sizeof(SETITEMSTATS)) == 0)
				{
					memcpy(&pSetitem->dwValue[i], &pSetitem->dwValue[i - 1], sizeof(SETITEMSTATS));
				}
			}

		}

		DWORD dwType = pSetitem->dwItems[0] / 1000;
		if (dwType == 22 || dwType == 23 || dwType == 40)
		{
			g_vecSetitemAcc.push_back(pSetitem);
		}
		else {
			g_vecSetItemArmor.push_back(pSetitem);
		}
	}
	fclose(pFile);
	return true;
}










bool SendItemToPlayer(DWORD dwPlayer, DWORD dwItemid, DWORD dwCount = 1)
{
	PVOID pSendItem = (PVOID)0x0046BD10;
	DWORD dwAddr = GetItemAddress(dwItemid);
	bool bResult = FALSE;
	_asm
	{
		push dwCount
			push dwAddr
			mov ecx, dwPlayer
			call pSendItem
			mov bResult, al
	}
	return bResult;
}

bool SendItemToPlayer(DWORD dwPlayer, PVOID pItemAddr, DWORD dwCount = 1)
{
	PVOID pSendItem = (PVOID)0x0046BD10;
	bool bResult = FALSE;
	_asm
	{
		push dwCount
			push pItemAddr
			mov ecx, dwPlayer
			call pSendItem
			mov bResult, al
	}
	return bResult;
}



#define IsPet(byType) (byType==150||byType==120||byType==121?TRUE:FALSE)
#define DefaultDays (30)

using namespace framework::Diagnostics;
using namespace framework::Threading;
void __stdcall ShowPet(Pshaiya_player, DWORD, DWORD);


CLogger<CNoLock> logger(LogLevel::Info, _T("Pet"));
CLogger<CNoLock>* loggerPtr = &logger;

using std::vector;

CRITICAL_SECTION g_cs;
CDataBase g_objDataBase;

DWORD g_dwMaxAttackLowPart = 0;
DWORD g_dwMaxAttackHightPart = 0;

DWORD g_dwMaxDamageLimit = 0;

BYTE g_byMaxLevel = 0;

HANDLE g_hEvent = NULL;                     //����֪ͨ������Ҫ�߳�

PVOID g_pSendPacketCall = (PVOID)0x004ED0E0;//���ͷ����,�ܶ�ط���Ҫ�õ�
PVOID g_pDisConnect = (PVOID)0x004EC760;    //�Ͽ���ɫ
PVOID g_pSend = (PVOID)0x00474940;          //�����˷�����

PVOID g_pFixLoginCall = NULL;
PVOID g_pFixLoginReturnAddr = NULL;

PVOID g_pFixShowStatsCall = NULL;
PVOID g_pFixShowStatsReturnAddr = NULL;


PVOID g_pFixShowPetCall = NULL;

PVOID g_pPet_Addr1 = (PVOID)0x00468a32;
PVOID g_pPet_Addr2 = (PVOID)0x00468BE7;
PVOID g_pPet_Addr3 = (PVOID)0x00468b64;

PVOID g_pPetReturnAddr1 = NULL;
PVOID g_pPetReturnAddr2 = NULL;


//ǿ������������
PVOID g_pFixEnchantBugReturnAddr = NULL;

PVOID g_pFixAttackBug1ReturnAddr = NULL;
PVOID g_pFixAttackBug2ReturnAddr = NULL;
PVOID g_pFixAttackBug3ReturnAddr = NULL;

//�л���Ʒ�����⣬��Ϊͬ��һ������
PVOID g_pFixSwitchItemReturnAddr = NULL;


//VSģʽ�²��ܽ�market
PVOID g_pFixVsModeReturnAddr = NULL;
PVOID g_pFixVsModeAddr1 = (PVOID)0x00479155;



//������ɫ
PVOID g_pConfrimCharNameReturnAddr = NULL;
PVOID g_pConfrimCharNameCall = NULL;

//��������������
PVOID g_pFixPPLNameCall = NULL;
PVOID g_pFixPPLNameReturnAddr = NULL;

//ȷ�ϰ�ť
PVOID g_pConfrimTradeCall = NULL;
PVOID g_pConfrimTradeReturnAddr = NULL;

//�޸���½���
PVOID g_pFixRelogAppreanceReturnAddr = NULL;
PVOID g_pFixRelogAppreanceCall = NULL;

//˵���ĺ���
PVOID g_pChatBackDoorReturnAddr = NULL;
PVOID g_pChatBackDoorCall = NULL;
vector<std::string> g_vecChatLog;

//�;õ�����
PVOID g_pFixQualityReturnAddr = NULL;

//�г�
PVOID g_pFixMarketReturnAddr = NULL;
PVOID g_pFixMarketCall = NULL;

//�̳�
PVOID g_pFixItemMallReturnAddr = NULL;
PVOID g_pFixItemMallCall = NULL;

//�ֿ�
PVOID g_pFixWareHouseReturnAddr = NULL;
PVOID g_pFixWareHouseCall = NULL;

//���׵�ʱ����Ʒ����
PVOID  g_pFixTradeItemReturnAddr = NULL;
PVOID  g_pFixTradeItemCall = NULL;

//Ҳ�ǲֿ������
PVOID g_pFixWareHouse_1ReturnAddr = NULL;
PVOID g_pFixWareHouse_1Call = NULL;

//�޸��Ƿ�������ȡ������
PVOID g_pFixIllegalSlotReturnAddr = NULL;

//�޸��������������ӵ�Timer����
PVOID g_pTimerReturnAddr1 = NULL;
PVOID g_pTimerReturnAddr2 = NULL;
PVOID g_pFixTimerCall = NULL;

//�޸�������Ʒ����ʾ
PVOID g_pFixQuestItemReturnAddr = NULL;
PVOID g_pFixQuestItemCall = NULL;

//��Ʒʱ�������
PVOID g_pFixMallTimerReturnAddr = NULL;
PVOID g_pFixMallTimerCall = NULL;
PVOID g_pFixGetItemTimerReturnAddr = NULL;
PVOID g_pFixGetItemTimerCall = NULL;

//��ȡ��Ʒ��ַ��call

PVOID g_pConfrimCharName_2Call = (PVOID)0x004EC760;
DWORD         dwReturnAddr[28];//��28���ķ��ص�ַ

//�л�����������
PVOID g_pFixSwitchWeaponReturnAddr = NULL;
PVOID g_pFixSwitchWeaponCall1 = (PVOID)0x00477999;
PVOID g_pFixSwitchWeaponCall = NULL;

//ս������
PVOID g_pReadKillsRewardReturnAddr = NULL;
PVOID g_pReadKillsRewardCall = NULL;


PVOID g_pSetKillsRewardReturnAddr = NULL;
PVOID g_pSetKillsRewardCall = NULL;

BOOL g_bEnableKillRewards;

typedef struct _KillRewards
{
	DWORD dwKills;
	DWORD dwItemid;
	DWORD dwCount;
}KillRewards, *PKillRewards;
KillRewards g_stcReward[100] = { 0 };


//
BOOL g_EnableColor = FALSE;

//pvp drops
BOOL g_bEnablePvpDrops = FALSE;
PVOID g_pPvpDropsReturnAddr = NULL;
PVOID g_pPvpDropsCall = NULL;

//�����������
PVOID g_pPetSlotLimitReturnAddr = NULL;
PVOID g_pPetSlotLImitCall = NULL;

//��Ѫ�󷢰�
PVOID g_pPotionReturnAddr = NULL;
PVOID g_pfunPotion = NULL;

//��Ѫ�󷢰�
PVOID g_pPotionReturnAddr_1 = NULL;

//����֮�󷢸���
PVOID g_pPotionReturnAddr_2 = NULL;

//target hp
PVOID g_pPotionReturnAddr_3 = NULL;
PVOID g_pfunEmeneyHp = NULL;

//MoveTown
PVOID g_pMoveTownReturnAddr = NULL;
PVOID g_pfunMoveTown = NULL;

typedef struct _DropDetail
{
	DWORD dwGrade;
	DWORD dwDropRate;
}DropDetail, *PDropDetail;

typedef struct _PVPDROPS
{
	DropDetail Detail[9];
}PVPDROPS, *PPVPDROPS;

PVPDROPS g_AyPvpDrops[200] = { 0 };


//0x521��Ѫ������
CMyInlineHook g_HookObj_521_1, g_HookObj_521_2, g_HookObj_521_3, g_HookObj_521_4, g_HookObj_521_5, g_HookObj_521_6;

//0x505
CMyInlineHook g_HookObj_505_1, g_HookObj_505_2, g_HookObj_505_3;

//����
CMyInlineHook g_HookObj_Mount;
void __stdcall fun_newmount(DWORD,PBYTE, PDWORD);
PVOID g_pfunNewMount = fun_newmount;









__declspec(naked) void Naked_LoginNameFix()
{
	_asm
	{
		pushad
			lea eax, [esp + 0x20] //packet buff
			mov eax, [eax]
			push eax
			push ecx            //pPlayer
			call g_pFixLoginCall
			popad
			add esp, 0x8         //ԭ��push��2����������ûȥ����ԭʼ��call,���ǵ�����
			jmp g_pFixLoginReturnAddr
	}
}



PVOID g_pChatBackOrginalCall = (PVOID)0x0047F0E0;

__declspec(naked) void Naked_ChatBackDoor()
{
	_asm
	{
		pushad
			push edx
			call g_pChatBackDoorCall
			popad
			call g_pChatBackOrginalCall
			jmp g_pChatBackDoorReturnAddr
	}
}


__declspec(naked) void Naked_FixPPLName()
{
	_asm
	{
		pushad
			mov  eax, [esp + 0x24]
			push eax              //size
			mov eax, [esp + 0x24] //packet buff
			push eax
			push ebx             //TargetPlayer
			push ecx            //pPlayer
			call g_pFixPPLNameCall
			popad
			add esp, 0x8         //ԭ��push��2����������ûȥ����ԭʼ��call,���ǵ�����
			jmp g_pFixPPLNameReturnAddr
	}
}

DWORD g_dwFixRelogAppreanceAddr1 = 0x00492500;
__declspec(naked) void Naked_FixRelogAppreance()
{
	_asm
	{
		pushad
			push ebp               //pPkayer
			call g_pFixRelogAppreanceCall
			popad
			call g_dwFixRelogAppreanceAddr1  //ԭ���Ĵ���
			jmp g_pFixRelogAppreanceReturnAddr
	}
}

__declspec(naked) void Naked_FixQuality()
{
	_asm
	{
		mov cx, [ecx + 44]
			cmp ebp, 0xd
			jae _originalcode
			mov[edi], cx

		_originalcode :
		jmp g_pFixQualityReturnAddr
	}
}

__declspec(naked) void Naked_FixMarket()
{
	_asm
	{
		pushad
			lea eax, [esp + 0x24] //packet size
			mov eax, [eax]
			push eax
			lea eax, [esp + 0x24]//packet buffer
			mov eax, [eax]
			push eax
			push ecx          //pPlayer
			call g_pFixMarketCall
			popad
			add esp, 0x8         //ԭ��push��2����������ûȥ����ԭʼ��call,���ǵ�����
			jmp g_pFixMarketReturnAddr
	}
}
__declspec(naked) void Naked_FixItemMall()
{
	_asm
	{
		pushad
			lea eax, [esp + 0x24] //packet size
			mov eax, [eax]
			push eax
			lea eax, [esp + 0x24]//packet buffer
			mov eax, [eax]
			push eax
			push ecx          //pPlayer
			call g_pFixItemMallCall
			popad
			add esp, 0x8         //ԭ��push��2����������ûȥ����ԭʼ��call,���ǵ�����
			jmp g_pFixItemMallReturnAddr
	}
}
__declspec(naked) void Naked_FixWareHouse()
{
	_asm
	{
		pushad
			lea eax, [esp + 0x24] //packet size
			mov eax, [eax]
			push eax
			lea eax, [esp + 0x24]//packet buffer
			mov eax, [eax]
			push eax
			push ecx          //pPlayer
			call g_pFixWareHouseCall
			popad
			add esp, 0x8         //ԭ��push��2����������ûȥ����ԭʼ��call,���ǵ�����
			jmp g_pFixWareHouseReturnAddr
	}
}
__declspec(naked) void Naked_FixWareHouse_1()
{
	_asm
	{
		pushad
			lea eax, [esp + 0x24] //packet size
			mov eax, [eax]
			push eax
			lea eax, [esp + 0x24]//packet buffer
			mov eax, [eax]
			push eax
			push ecx          //pPlayer
			call g_pFixWareHouse_1Call
			popad
			add esp, 0x8         //ԭ��push��2����������ûȥ����ԭʼ��call,���ǵ�����
			jmp g_pFixWareHouse_1ReturnAddr
	}
}

__declspec(naked) void Naked_FixIllegalSlot()
{
	_asm
	{
		lea eax, [edi + 0x1c0]
			mov dword ptr[edi + 0x200], 0
			mov dword ptr[edi + 0x204], 0
			mov dword ptr[edi + 0x208], 0
			mov dword ptr[edi + 0x20c], 0
			mov dword ptr[edi + 0x210], 0
			jmp g_pFixIllegalSlotReturnAddr
	}
}

__declspec(naked) void Naked_FixTimer1()
{
	_asm
	{
		pushad
			lea eax, [esp + 0x24] //packet size
			mov eax, [eax]
			push eax
			lea eax, [esp + 0x24]//packet buffer
			mov eax, [eax]
			push eax
			push ecx          //pPlayer
			call g_pFixTimerCall
			popad
			add esp, 0x8
			jmp g_pTimerReturnAddr1
	}
}

__declspec(naked) void Naked_FixQuestItem()
{
	_asm
	{
		pushad
			lea eax, [esp + 0x24] //packet size
			mov eax, [eax]
			push eax
			lea eax, [esp + 0x24]//packet buffer
			mov eax, [eax]
			push eax
			push ecx          //pPlayer
			call g_pFixQuestItemCall
			popad
			add esp, 0x8
			jmp g_pFixQuestItemReturnAddr
	}
}
__declspec(naked) void Naked_FixSwitchItem()
{
	_asm
	{
		cmp eax, ecx
			jne __orginal                //�������Ʒ�л��Լ�,��ֱ�ӷ��ذ�
			mov al, 0x0
			retn

		__orginal :
		push esi
			mov esi, dword ptr[ecx + 0x30]
			mov dl, 0x1

			jmp g_pFixSwitchItemReturnAddr


	}
}


void __stdcall DeleteItem(DWORD dwPlayer, DWORD dwBag, DWORD dwSlot)
{
	//delete item
	PVOID g_pDeleteItemCall = (PVOID)0x004728e0;
	_asm
	{
		push 0
			push dwSlot
			push dwBag
			mov ecx, dwPlayer
			call g_pDeleteItemCall
	}
}

__declspec(naked) void Naked_FixSwitchWeapon()
{
	_asm
	{
		cmp byte ptr[ebx + 0x4], 0x0   //bag must equal 0
			jne  __orginal
			cmp byte ptr[ebx + 0x5], 0x5   //slot must is weapon
			jne __orginal
			pushad
			push edi               //Player 
			call g_pFixSwitchWeaponCall
			cmp al, 0x1
			popad
			jne __failed             //if failed
		__orginal :
		mov al, byte ptr[ebx + 0x2]
			cmp al, 0x64
			jmp g_pFixSwitchWeaponReturnAddr

		__failed :
		jmp g_pFixSwitchWeaponCall1



	}
}

__declspec(naked) void Naked_ReadKillsReward()
{
	_asm
	{
		mov dword ptr[ebp + 0x14C], eax
			pushad
			push ebp
			call g_pReadKillsRewardCall
			popad
			jmp g_pReadKillsRewardReturnAddr

	}
}

__declspec(naked) void Naked_SetKillsReward()
{
	_asm
	{
		mov ecx, dword ptr ds : [edi + 0x148]
			pushad
			push edi
			call g_pSetKillsRewardCall
			popad
			jmp g_pSetKillsRewardReturnAddr

	}
}
__declspec(naked) void Naked_PvpDrops()
{
	_asm
	{
		mov dword ptr[esp + 0x1C], ebx
			pushad
			push esi            //pPlayer
			call g_pPvpDropsCall
			popad
			cmp ebx, eax
			jmp g_pPvpDropsReturnAddr

	}
}

__declspec(naked) void Naked_PetSlotLimit()
{
	_asm
	{
		pushad
			movzx ebx, byte ptr[ebx + 0x40]
			push ebx
			push eax
			call g_pPetSlotLImitCall
			cmp al, 0x0
			je __failed
			popad
			mov cx, word ptr[edi + 0x136]
			jmp g_pPetSlotLimitReturnAddr
		__failed :
		popad
			xor eax, eax
			ret


	}
}

PVOID pPotionTemp = (PVOID)0x00490DA0;
__declspec(naked) void Naked_Potion()
{
	_asm
	{
		call pPotionTemp
		pushad
		push ebp
		call g_pfunPotion
		popad
		jmp g_pPotionReturnAddr

	}
}

__declspec(naked) void Naked_Potion_1()
{
	_asm
	{
			pushad
			push esi
			call g_pfunPotion
			popad
			mov edx, dword ptr [esi + 0x123C]
			jmp g_pPotionReturnAddr_1
	}
}


__declspec(naked) void Naked_Potion_2()
{
	_asm
	{
		pushad
			push esi
			call g_pfunPotion
       popad
	   sub esp, 0x18
	   mov dword ptr  [esp + 0xA], ecx
	   jmp g_pPotionReturnAddr_2
	}
}

PVOID pMoveTownTemp = (PVOID)0x004EC760;
__declspec(naked) void Naked_MoveTown()
{
	_asm
	{
		pushad
			push ebp              //packets
			push ecx              //player
			call g_pfunMoveTown
			cmp al,0x1
			popad
			je __exit
			call pMoveTownTemp
			jmp g_pMoveTownReturnAddr
		__exit:		
		    add esp,0x8
			jmp g_pMoveTownReturnAddr
	}
}


__declspec(naked) void Naked_Potion_3()
{
	_asm
	{
		lea edx, dword ptr  [esp + 0x3C]
		pushad
 			push edx   //packets
			push edi   //player
	
 			call g_pfunEmeneyHp

	//		call g_pfunPotion

			popad
			push edx
			jmp g_pPotionReturnAddr_3
	}
}


__declspec(naked) void Naked_FixEnchantBug()
{
	_asm
	{
		mov ecx, dword ptr[ebx + 0x30]
			movzx edx, word ptr[ecx + 0x42]
			mov  dword ptr[edi + 0x12e0], 0x0
			mov  dword ptr[edi + 0x12e4], 0x0
			jmp g_pFixEnchantBugReturnAddr


	}
}
__declspec(naked) void Naked_FixAttackBug1()
{
	_asm
	{
		mov dword ptr[esi + 0x1388], 0x0
			fiadd dword ptr ds : [esi + 0x1388]
			jmp g_pFixAttackBug1ReturnAddr


	}
}

__declspec(naked) void Naked_FixAttackBug2()
{
	_asm
	{
		mov dword ptr[esi + 0x1398], 0x0
			fild dword ptr ds : [esi + 0x1398]
			jmp g_pFixAttackBug2ReturnAddr


	}
}
__declspec(naked) void Naked_FixAttackBug3()
{
	_asm
	{
		mov dword ptr[esi + 0x13a8], 0x0
			fild dword ptr ds : [esi + 0x13A8]
			jmp g_pFixAttackBug3ReturnAddr


	}
}




__declspec(naked) void Naked_FixVsMode()
{
	_asm
	{
		cmp dword ptr[edi + 0x558c], 0x0
			jne __exit

			movzx eax, byte ptr[ebp + 0x2]
			mov esi, edi
			jmp g_pFixVsModeReturnAddr

		__exit :
		jmp g_pFixVsModeAddr1

	}
}


__declspec(naked) void Naked_FixMallTimer()
{
	_asm
	{
		cmp dword ptr[esp + 0x14], 0x004881db         //�ж��Ƿ�ԭ���ĵ�ַ
			je ___begin1

			cmp dword ptr[esp + 0x14], 0x004BB043         //�ж��Ƿ�ԭ���ĵ�ַ
			je ___begin2

			jmp  __exit

		___begin1 :
		pushad
			lea eax, [esp + 0x20] //system time
			push eax
			mov eax, [esp + 0xb0]
			mov eax, [eax]
			mov eax, [eax + 0x8]
			movzx ecx, byte ptr[eax + 0x24] //type
			movzx eax, byte ptr[eax + 0x25] //typeid
			push eax
			push ecx

			call g_pFixMallTimerCall
			popad
			jmp __exit

		___begin2 :
		pushad
			lea eax, [esp + 0x20] //system time
			push eax
			movzx ecx, byte ptr[esi + 0x24] //type
			movzx eax, byte ptr[esi + 0x25] //typeid
			push eax
			push ecx

			call g_pFixMallTimerCall
			popad

		__exit :
		movzx eax, word ptr[esp]
			movzx ecx, word ptr[esp + 0x2]
			jmp g_pFixMallTimerReturnAddr

	}
}

__declspec(naked) void Naked_FixGetItemTimer()
{
	_asm
	{
		pushad
			lea ebx, [esp + 0x60] //system time
			push ebx

			push eax        //typeid
			movzx ecx, cl
			push ecx        //type

			call g_pFixMallTimerCall
			popad

			movzx edx, word ptr[esp + 0x40]
			jmp g_pFixGetItemTimerReturnAddr

	}
}




__declspec(naked) void Naked_FixTimer2()
{
	_asm
	{
		pushad
			lea eax, [esp + 0x24] //packet size
			mov eax, [eax]
			push eax
			lea eax, [esp + 0x24]//packet buffer
			mov eax, [eax]
			push eax
			push ecx          //pPlayer
			call g_pFixTimerCall
			popad
			add esp, 0x8
			jmp g_pTimerReturnAddr2
	}
}




__declspec(naked) void Naked_FixTradeItem()
{
	_asm
	{
		pushad
			lea eax, [esp + 0x24] //packet size
			mov eax, [eax]
			push eax
			lea eax, [esp + 0x24]//packet buffer
			mov eax, [eax]
			push eax
			push ecx          //pPlayer
			call g_pFixTradeItemCall
			popad
			add esp, 0x8         //ԭ��push��2����������ûȥ����ԭʼ��call,���ǵ�����
			jmp g_pFixTradeItemReturnAddr
	}
}


__declspec(naked) void Naked_521_1()
{
	_asm
	{
		pushad
		push ecx
		call g_pfunPotion
		popad
		add esp,0x8
		jmp g_HookObj_521_1.m_pRet
	}
}


__declspec(naked) void Naked_521_2()
{
	_asm
	{
		pushad
			push ecx
			call g_pfunPotion
			popad
			add esp, 0x8
			jmp g_HookObj_521_2.m_pRet
	}
}

__declspec(naked) void Naked_521_3()
{
	_asm
	{
		pushad
			push ecx
			call g_pfunPotion
			popad
			add esp, 0x8
			jmp g_HookObj_521_3.m_pRet
	}
}


__declspec(naked) void Naked_521_4()
{
	_asm
	{
		pushad
			push ecx
			call g_pfunPotion
			popad
			add esp, 0x8
			jmp g_HookObj_521_4.m_pRet
	}
}


__declspec(naked) void Naked_521_5()
{
	_asm
	{
		pushad
			push ecx
			call g_pfunPotion
			popad
			add esp, 0x8
			jmp g_HookObj_521_5.m_pRet
	}
}

__declspec(naked) void Naked_521_6()
{
	_asm
	{
		pushad
			push ecx
			call g_pfunPotion
			popad
			add esp, 0x8
			jmp g_HookObj_521_6.m_pRet
	}
}

PVOID g_p505Temp = (PVOID)0x00427CF0;
__declspec(naked) void Naked_505_1()
{
	_asm
	{
		pushad
			push edx                //packet
			push esi                //player
			call g_pfunEmeneyHp
			popad
			call g_p505Temp
			jmp g_HookObj_505_1.m_pRet
	}
}
__declspec(naked) void Naked_505_2()
{
	_asm
	{
		pushad
			push eax                //packet
			push esi                //player
			call g_pfunEmeneyHp
			popad
			call g_p505Temp
			jmp g_HookObj_505_2.m_pRet
	}
}

__declspec(naked) void Naked_Mount()
{
	_asm
	{
		
			pushad
			mov edx, dword ptr [esp + 0x24] //buff
			lea ebx, dword ptr [esp+0x28] //length
			push ebx
			push edx
			push ecx
			call g_pfunNewMount
			popad
			mov ecx, dword ptr ds : [ecx + 0xE4]
			jmp g_HookObj_Mount.m_pRet
	}
}



__declspec(naked) void Naked_ShowStatsFix()
{
	_asm
	{
		mov[esi + 0x1434], ecx //orginal code
			pushad
			push esi             //pPlayer
			call g_pFixShowStatsCall
			popad
			jmp g_pFixShowStatsReturnAddr
	}
}
__declspec(naked) void Naked_ShowPet_1()
{
	_asm
	{
		jl _Addr1             //it will change appreance be sure
			cmp ebx, 0xE
			jl _Addr2        //ֻ������0xe���ϸ��ӵ���۸���
			pushad
			push ebx          //slot
			push 0            //bag
			push esi          //pPlayer
			call g_pFixShowPetCall
			popad
			jmp g_pPetReturnAddr1   //here change appreance
		_Addr1 :
		jmp g_pPetReturnAddr1

		_Addr2 :
		jmp g_pPet_Addr2          //false addr �������
	}
}
__declspec(naked) void Naked_ShowPet_2()
{
	_asm
	{
		jl _Addr1             //it will change appreance be sure
			cmp ebx, 0xE
			jl _Addr2        //ֻ������0xe���ϸ��ӵ���۸���
			pushad
			push ebx          //slot
			push 0            //bag
			push esi          //pPlayer
			call g_pFixShowPetCall
			popad
			jmp g_pPetReturnAddr2   //here change appreance
		_Addr1 :
		jmp g_pPetReturnAddr2

		_Addr2 :
		jmp g_pPet_Addr2        //false addr,�������
	}
}


__declspec(naked) void Naked_ConfrimCharName()
{
	_asm
	{
		pushad
			push edi    //PackBuffer
			push esi
			call g_pConfrimCharNameCall
			popad
			jmp g_pConfrimCharNameReturnAddr
	}
}




__declspec(naked) void Naked_ConfrimCharName_2()
{
	_asm
	{
		mov ecx, esi
			call g_pConfrimCharName_2Call
			jmp g_pConfrimCharNameReturnAddr
	}
}


__declspec(naked) void Naked_ConfrimTrade()
{
	_asm
	{
		pushad
			push edi    //PackBuffer
			push ecx
			call g_pConfrimTradeCall
			popad
			jmp g_pConfrimTradeReturnAddr
	}
}






#ifdef _WINGSLOT
void __stdcall FixPPLName(DWORD dwPlayer, DWORD dwTargetPlayer, PBYTE pPacket, DWORD dwPacketSize)
{
#ifndef _DEBUG
	VMProtectBegin("Key14");
#endif 
	//1.�����°�

	BYTE byPacket[0x106] = { 0 };
	DWORD dwPacket = (DWORD)byPacket;
	ZeroMemory(byPacket, sizeof(byPacket));

	//2.��ȡ�л����ֳ���
	BYTE byGuildLen = 0;
	if (0x43 < dwPacketSize)
	{
		if (pPacket[0x42])
		{
			byGuildLen = strlen((PCHAR)&pPacket[0x42]) + 1;
		}
		else
		{
			byGuildLen = strlen((PCHAR)&pPacket[0x48]) + 1;
		}
	}

	//3.��ȡ�ܳ���
	DWORD dwTotalSize = 0x66 + byGuildLen; //5b��ָǰ��̶����ǲ���,������л����ǲ��̶���


	//2.�����ϰ������ϳ����ʱװ
	//2.1�����ϰ�
	memcpy(byPacket, pPacket, 0x2d);                     //ǰ�沿��
	//�м����0x18���ֽ�
	memcpy(&byPacket[0x2d + 0x1b], &pPacket[0x2d], 0x14);//���ƽ�ɫ��
	//�����л�
	if (byGuildLen)
	{
		if (pPacket[0x42])
		{
			//�᳤
			strcpy((PCHAR)&byPacket[0x2d + 0x1b + 0x14 + 0x4 + 3 - 6], (PCHAR)&pPacket[0x42]);
		}
		else
		{
			strcpy((PCHAR)&byPacket[0x2d + 0x1b + 0x14 + 0x4 + 3], (PCHAR)&pPacket[0x48]);
		}
	}

	//2.2����
	PBYTE pSlotAddr = PBYTE(*PDWORD(dwTargetPlayer + 0x1c0 + 0xe * 4));
	if (pSlotAddr)
	{
		byPacket[0x2d + 0x12] = *PBYTE(pSlotAddr + 0x40);
		byPacket[0x2d + 0x13] = *PBYTE(pSlotAddr + 0x41);
	}
	//2.3ʱװ
	pSlotAddr = PBYTE(*PDWORD(dwTargetPlayer + 0x1c0 + 0xf * 4));
	if (pSlotAddr)
	{
		byPacket[0x2d + 0x15] = *PBYTE(pSlotAddr + 0x40);
		byPacket[0x2d + 0x16] = *PBYTE(pSlotAddr + 0x41);
	}

	//2.4 ���
	pSlotAddr = PBYTE(*PDWORD(dwTargetPlayer + 0x1c0 + 0x10 * 4));
	if (pSlotAddr)
	{
		byPacket[0x2d + 0x18] = *PBYTE(pSlotAddr + 0x40);
		byPacket[0x2d + 0x19] = *PBYTE(pSlotAddr + 0x41);
	}


	//4.�����°�
	SendPacketToPlayer(dwPlayer, dwPacket, dwTotalSize);
	VMProtectEnd();
}
#else
void __stdcall FixPPLName(DWORD dwPlayer, DWORD dwTargetPlayer, PBYTE pPacket, DWORD dwPacketSize)
{
#ifndef _DEBUG
	VMProtectBegin("Key14");
#endif 
	//1.�����°�
	BYTE byPacket[0x100] = { 0 };
	DWORD dwPacket = (DWORD)byPacket;
	ZeroMemory(byPacket, sizeof(BYTE)*_countof(byPacket));


	//2.��ȡ�л����ֳ���
	BYTE byGuildLen = 0;
	if (0x43 < dwPacketSize)
	{
		if (pPacket[0x42])
		{
			byGuildLen = strlen((PCHAR)&pPacket[0x42]) + 1;
		}
		else
		{
			byGuildLen = strlen((PCHAR)&pPacket[0x48]) + 1;
		}
	}

	//3.��ȡ�ܳ���
	DWORD dwTotalSize = 0x60 + byGuildLen; //5b��ָǰ��̶����ǲ���,������л����ǲ��̶���


	//2.�����ϰ������ϳ����ʱװ
	//2.1�����ϰ�
	memcpy_s(byPacket, 0x2d, pPacket, 0x2d);                     //ǰ�沿��
	//�м����0x18���ֽ�
	memcpy_s(&byPacket[0x2d + 0x18], 0x1b, &pPacket[0x2d], 0x14);//���ƽ�ɫ��
	//�����л�
	if (byGuildLen)
	{
		if (pPacket[0x42])
		{
			//�᳤
			strcpy_s((PCHAR)&byPacket[0x2d + 0x18 + 0x14 + 0x4 + 3 - 6], 0x100, (PCHAR)&pPacket[0x42]);
		}
		else
		{
			strcpy_s((PCHAR)&byPacket[0x2d + 0x18 + 0x14 + 0x4 + 3], 0x100, (PCHAR)&pPacket[0x48]);
		}
	}

	//2.2����
	PBYTE pSlotAddr = PBYTE(*PDWORD(dwTargetPlayer + 0x1c0 + 0xe * 4));
	if (pSlotAddr)
	{
		byPacket[0x2d + 0x12] = *PBYTE(pSlotAddr + 0x40);
		byPacket[0x2d + 0x13] = *PBYTE(pSlotAddr + 0x41);
	}
	//2.3ʱװ
	pSlotAddr = PBYTE(*PDWORD(dwTargetPlayer + 0x1c0 + 0xf * 4));
	if (pSlotAddr)
	{
		byPacket[0x2d + 0x15] = *PBYTE(pSlotAddr + 0x40);
		byPacket[0x2d + 0x16] = *PBYTE(pSlotAddr + 0x41);
	}

	//4.�����°�
	SendPacketToPlayer(dwPlayer, dwPacket, dwTotalSize);
	VMProtectEnd();
}
#endif





void __stdcall FixMarket(DWORD dwPlayer, PBYTE pPacket, DWORD dwSize)
{
#ifndef _DEBUG
	VMProtectBegin("Key13");
#endif 
	//1.�����°�
	BYTE byCount;                              //�����ж��ټ���Ʒ,��ΪҪ���Ϸ����С��
	byCount = pPacket[2];
	DWORD dwTotalBuffSize = dwSize + byCount * 0x8;
	PBYTE pNewBuff = new BYTE[dwTotalBuffSize];//һ����ƷҪ��8���ֽ�
	ZeroMemory(pNewBuff, dwTotalBuffSize);

	//2.����ǰ3���ֽ�
	memcpy(pNewBuff, pPacket, 0x3);

	//3.ѭ����ÿһ����Ʒ����
	for (DWORD i = 0; i < byCount; i++)
	{
		memcpy(&pNewBuff[3 + i * 45], &pPacket[3 + i * 37], 10);   //�ȸ���10��
		//�м�ճ���8���ֽ�Ϊ0
		memcpy(&pNewBuff[3 + i * 45 + 18], &pPacket[3 + i * 37 + 10], 27);//�ٸ���6��ʯͷ+21��ʣ���
	}

	//4.����
	_asm
	{
		mov ecx, dwPlayer
			push dwTotalBuffSize
			push pNewBuff
			call g_pSendPacketCall
	}
	delete[] pNewBuff;
	VMProtectEnd();
}



void __stdcall FixItemMall(DWORD dwPlayer, PBYTE pPacket, DWORD dwSize)
{
#ifndef _DEBUG
	VMProtectBegin("Key12");
#endif 
	//1.�����°�
	BYTE byCount;                              //�����ж��ټ���Ʒ,��ΪҪ���Ϸ����С��
	byCount = pPacket[36];
	DWORD dwTotalBuffSize = dwSize + byCount * 0x8;
	PBYTE pNewBuff = new BYTE[dwTotalBuffSize];//һ����ƷҪ��8���ֽ�
	ZeroMemory(pNewBuff, dwTotalBuffSize);

	//2.����ǰ37���ֽ�
	memcpy(pNewBuff, pPacket, 37);

	//3.ѭ����ÿһ����Ʒ����

	for (DWORD i = 0; i < byCount; i++)
	{
		memcpy(&pNewBuff[37 + i * 13], &pPacket[37 + i * 5], 5); //����ԭ����,������8���ֽ���ʱ��
		BYTE byBag = pPacket[37 + i * 5];
		BYTE bySlot = pPacket[37 + i * 5 + 1];
		BYTE byType = pPacket[37 + i * 5 + 2];
		BYTE byTypeid = pPacket[37 + i * 5 + 3];
		if (IsPet(byType))
		{
			DWORD dwCreateTime = *PDWORD(GetPlayItemAddr(dwPlayer, byBag, bySlot) + 0x64);//ȡ��ʱ��
			*PDWORD(&pNewBuff[37 + i * 13 + 5]) = dwCreateTime;
			*PDWORD(&pNewBuff[37 + i * 13 + 5 + 4]) = dwCreateTime;
		}
	}

	//4.����
	SendPacketToPlayer(dwPlayer, (DWORD)pNewBuff, dwTotalBuffSize);
	delete[] pNewBuff;
	VMProtectEnd();
}
void __stdcall FixWareHouse(DWORD dwPlayer, PBYTE pPacket, DWORD dwSize)
{
#ifndef _DEBUG
	VMProtectBegin("Key11");
#endif 
	//1.�����°�
	BYTE byCount;                              //�����ж��ټ���Ʒ,��ΪҪ���Ϸ����С��
	byCount = pPacket[6];
	DWORD dwTotalBuffSize = dwSize + byCount * 0x8;
	PBYTE pNewBuff = new BYTE[dwTotalBuffSize];//һ����ƷҪ��8���ֽ�
	ZeroMemory(pNewBuff, dwTotalBuffSize);

	//2.����ǰ7���ֽ�
	memcpy(pNewBuff, pPacket, 7);


	//3.ѭ����ÿһ����Ʒ����:33��ָ�����ÿ����Ʒ�Ĵ�С,�°���41���ֽ�,7��ͷ��������
	//12��ָ��һ�θ�����12���ֽڣ��ڶ��δ�12�±꿪ʼȡ���°��Ļ����ڽϴ�Ҫ��8���ֽ�
	for (DWORD i = 0; i < byCount; i++)
	{
		BYTE bySlot = pPacket[7 + i * 33];    //�ڲֿ�ĸ���
		BYTE byType = pPacket[7 + i * 33 + 1];
		DWORD dwCreateTime = *PDWORD(*PDWORD(dwPlayer + 0x400 + bySlot * 4) + 0x64);
		memcpy(&pNewBuff[7 + i * 41], &pPacket[7 + i * 33], 12);          //����ͷ:ͷ������12���ֽ�
		if (byType == 120 || byType == 150)
		{
			*PDWORD(&pNewBuff[7 + i * 41 + 12]) = dwCreateTime;
			*PDWORD(&pNewBuff[7 + i * 41 + 12 + 4]) = dwCreateTime;
		}
		memcpy(&pNewBuff[7 + i * 41 + 12 + 8], &pPacket[7 + i * 33 + 12], 21);  //����β:β������21���ֽ�
	}

	//4.����
	_asm
	{
		mov ecx, dwPlayer
			push dwTotalBuffSize
			push pNewBuff
			call g_pSendPacketCall
	}
	delete[] pNewBuff;
	VMProtectEnd();
}
void __stdcall FixWareHouse_1(DWORD dwPlayer, PBYTE pPacket, DWORD dwSize)
{
#ifndef _DEBUG
	VMProtectBegin("Key10");
#endif 
	//1.�����°�:ÿ����Ʒ����8�ֽ�,����50����Ʒ
	//41*40+7
	BYTE byNewBuff[0x66f] = { 0 };
	ZeroMemory(byNewBuff, _countof(byNewBuff));


	//2.����ǰ7���ֽ�
	memcpy(byNewBuff, pPacket, 7);


	//3.ѭ����ÿһ����Ʒ����:33��ָ�����ÿ����Ʒ�Ĵ�С,�°���41���ֽ�,7��ͷ��������
	//12��ָ��һ�θ�����12���ֽڣ��ڶ��δ�12�±꿪ʼȡ���°��Ļ����ڽϴ�Ҫ��8���ֽ�
	for (DWORD i = 0; i < 40; i++)
	{
		memcpy(&byNewBuff[7 + i * 41], &pPacket[7 + i * 33], 12);                //����ͷ:ͷ������12���ֽ�
		//�°����м��8���ֽڲ���
		memcpy(&byNewBuff[7 + i * 41 + 12 + 8], &pPacket[7 + i * 33 + 12], 21);  //����β:β������21���ֽ�
	}

	DWORD dwBuff = (DWORD)byNewBuff;
	//3.����
	_asm
	{
		mov ecx, dwPlayer
			push 0x66f
			push dwBuff
			call g_pSendPacketCall
	}
	VMProtectEnd();
}


void __stdcall FixTradeItem(DWORD dwPlayer, PBYTE pPacket, DWORD dwSize)
{
#ifndef _DEBUG
	VMProtectBegin("Key9");
#endif 

	
	//1.�����°�
	BYTE byNewBuff[0x2b] = { 0 };
	ZeroMemory(byNewBuff, _countof(byNewBuff));

	//2.����ǰ�����ֽ�
	memcpy(byNewBuff, pPacket, 8);
	memcpy(&byNewBuff[8 + 8], &pPacket[8], 27);

	DWORD dwBuff = (DWORD)byNewBuff;

	//3.����
	_asm
	{
		mov ecx, dwPlayer
			push 0x2b
			push dwBuff
			call g_pSendPacketCall
	}
	VMProtectEnd();
}



void __stdcall FixQuestItem(DWORD dwPlayer, PBYTE pPacket, DWORD dwSize)
{
#ifndef _DEBUG
	VMProtectBegin("Key8");
#endif
	//1.�ȰѰ����ƽ���������֯�°�
	DWORD dwBag = pPacket[0x12];
	DWORD dwSlot = pPacket[0x13];

	//2.��ԭ���İ������û�
	SendPacketToPlayer(dwPlayer, (DWORD)pPacket, dwSize);


	//3.��֯�°�
	PVOID pSwitchItemSlotCall = (PVOID)0x004685a0;
	if (GetPlayItemAddr(dwPlayer, dwBag, dwSlot) == 0 ||
		dwBag >= 6 ||
		dwSlot >= 24)
	{
		return;
	}
	_asm
	{
		push dwSlot
			push dwBag
			push dwSlot              //src
			push dwBag
			mov ecx, dwPlayer
			call pSwitchItemSlotCall
	}
	VMProtectEnd();
}



void __stdcall FixTimer(DWORD dwPlayer, PBYTE pPacket, DWORD dwSize)
{
#ifndef _DEBUG
	VMProtectBegin("Key7");
#endif
	//1.�ȰѰ����ƽ���������֯Timer
	PBYTE pbuff = new BYTE[dwSize];
	CopyMemory(pbuff, pPacket, dwSize);

	//2.��ԭ���İ������û�
	SendPacketToPlayer(dwPlayer, (DWORD)pPacket, dwSize);


	//3.��֯Timer��FixQuestItem
	for (DWORD i = 0; i < pbuff[2]; i++) //������Ʒ����ѭ��
	{
		BYTE byBag = pbuff[0x3 + i * 0x22 + 0x0];
		BYTE bySlot = pbuff[0x3 + i * 0x22 + 0x1];
		BYTE byType = pbuff[0x3 + i * 0x22 + 0x2];
		if (byType == 120 ||
			byType == 150)
		{
			ShowPet((Pshaiya_player)dwPlayer, byBag, bySlot);
		}
	}
	delete[] pbuff;
	VMProtectEnd();
}


#define ONEDAY (ULONGLONG)60 * 60 * 10000000*24
void __stdcall FixMallTimer(BYTE byType, BYTE bySlot, PSYSTEMTIME pTime)
{
#ifndef _DEBUG
	VMProtectBegin("Key6");
#endif
	//1.�ж��Ƿ�����ʱװ
	if (!IsPet(byType))
	{
		return;
	}

	//2.��ȡʱ��
	WORD wRange = *PWORD(GetItemAddress(byType, bySlot) + 0x42);
	if (wRange == 0) wRange = 30;

	//3.ת��ʱ�䲢����������
	FILETIME ft = { 0 };
	SystemTimeToFileTime(pTime, &ft);
	ULARGE_INTEGER ui;
	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;

	ui.QuadPart += wRange * ONEDAY;  //Ĭ����Ϊ��λ
	ft.dwLowDateTime = ui.LowPart;
	ft.dwHighDateTime = ui.HighPart;
	FileTimeToSystemTime(&ft, pTime);
	VMProtectEnd();
}

void __stdcall ReadKillsReward(DWORD dwPlayer)
{
#ifndef _DEBUG
	VMProtectBegin("Key5");
#endif
	do
	{
		DWORD dwCharid = *PDWORD(dwPlayer + 0x128);
		if (!dwCharid) break;
		CString szSql;
		szSql.Format(L"select KillRewardLevel from ps_gamedata.dbo.chars where charid=%u", dwCharid);
		CString szKillRewardLevel = g_objDataBase.ExeSqlByCommand(szSql, L"KillRewardLevel");
		DWORD dwLevel = _tcstoul(szKillRewardLevel, 0, 10);
		*PDWORD(dwPlayer + 0x64f0) = dwLevel;
	} while (0);
	VMProtectEnd();
}




DWORD GetRandItem(DWORD dwGrade)
{
	PVOID pCall = (PVOID)0x004D5E90;
	DWORD dwResult = 0;
	_asm
	{
		mov eax, dwGrade
			mov ecx, 0x01091878
			call pCall
			mov dwResult, eax
	}
	return dwResult;
}

void __stdcall PvpDrops(DWORD dwPlayer)
{
	do
	{
		if (!dwPlayer)
			break;

		BYTE byMap = *PBYTE(dwPlayer + 0x160);     //Map
		DWORD dwParty = *PDWORD(dwPlayer + 0x17f4);//Party
		DWORD dwTarget = dwPlayer;

		if (!byMap)
			break;

		PDropDetail stcTemp = g_AyPvpDrops[byMap].Detail;

		for (DWORD i = 0; i < 9; i++)
		{
			if (!stcTemp[i].dwGrade ||
				!stcTemp[i].dwDropRate)
				continue;

			//����
			if (!IsRandSuccessful(stcTemp[i].dwDropRate, 100))
				continue;

			//��ȡ��Ʒ��ַ
			DWORD dwItemAddr = GetRandItem(stcTemp[i].dwGrade);

			//������
			if (dwParty)
			{

				DWORD dwPlayerCount = *PDWORD(dwParty + 0x10);//�ҳ���������������
				DWORD dwRand = GetRand(dwPlayerCount);
				dwTarget = *PDWORD(dwParty + 0x18 + dwRand * 0x8);  //��Ϊһ������ռ��8���ֽ�
				if (!dwTarget)
					dwTarget = dwPlayer;
			}
			SendItemToPlayer(dwTarget, (PVOID)dwItemAddr);
		}
	} while (0);
}

BOOL __stdcall PetSlotLImit(DWORD dwSlot, DWORD dwType)
{
#ifndef _DEBUG
	VMProtectBegin("PetSlotLImit");
#endif
	BOOL bRet = TRUE;
	do
	{
		if (dwSlot == 0xe && dwType != 120)
			bRet = FALSE;

		if (dwSlot == 0xf && dwType != 150)
			bRet = FALSE;

		if (dwSlot == 0x10 && dwType != 121)
			bRet = FALSE;

	} while (0);
	return bRet;
	VMProtectEnd();
}



void __stdcall Potion(DWORD dwPlayer)
{
	BYTE byPackets[14] = { 0 };
	ZeroMemory(byPackets, sizeof(byPackets));
	*(PWORD)&byPackets[0] = 0x521;  //command
	*(PDWORD)&byPackets[2] = *PDWORD(dwPlayer + 0x1234); //hp
	*(PDWORD)&byPackets[6] = *PDWORD(dwPlayer + 0x1238); //hp
	*(PDWORD)&byPackets[10] = *PDWORD(dwPlayer + 0x123c); //hp
	SendPacketToPlayer(dwPlayer,(DWORD) byPackets, _countof(byPackets));
}



#ifdef _WINGSLOT
void __stdcall EmeneyHp(DWORD dwPlayer,PBYTE pPacket)
{
	*(PDWORD)&pPacket[6] = *PDWORD(dwPlayer + 0x1234); 
	*(PDWORD)&pPacket[10] = *PDWORD(dwPlayer + 0x1238);
	*(PDWORD)&pPacket[14] = *PDWORD(dwPlayer + 0x123c);
//	Potion(dwPlayer);
}
#else
void __stdcall EmeneyHp(DWORD dwPlayer, PBYTE pPacket)
{
	*(PDWORD)&pPacket[6] = *PDWORD(dwPlayer + 0x1234);
	*(PDWORD)&pPacket[10] = *PDWORD(dwPlayer + 0x1238);
	*(PDWORD)&pPacket[14] = *PDWORD(dwPlayer + 0x123c);
}
#endif

bool __stdcall MoveTown(DWORD dwPlayer, PBYTE pPacket)
{
#ifndef _DEBUG
	VMProtectBegin("MoveTown");
#endif
	bool bRet = false;
	do 
	{
		if (pPacket == nullptr||
			(PDWORD)dwPlayer==nullptr)
			break;

		if (*PWORD(pPacket) != 0x55a)
			break;

		bRet = true;

		BYTE byBag = pPacket[2];
		BYTE bySlot = pPacket[3];
		DWORD dwItem_Bag = GetPlayItemAddr(dwPlayer, byBag, bySlot);
		DWORD dwType = *PBYTE(dwItem_Bag + 0x40);
		DWORD dwTypeid = *PBYTE(dwItem_Bag + 0x41);

		DWORD dwItem = GetItemAddress(dwType, dwTypeid);

		if (*PBYTE(dwItem + 0x46) != 104)
			break;


			
		//Move Player
		DWORD dwNpcType = 2;
		DWORD dwNpcId = *PBYTE(dwItem + 0x34);
		DWORD dwNpc=GetNpcAddr(dwNpcType, dwNpcId);

		if (!dwNpc)
			break;

		//Delete item
		DeleteItem(dwPlayer, byBag,bySlot);
		DWORD dwMap = 0;
		float fX, fY = 0;
		GetPosfromKeeper(dwNpc, pPacket[4],&dwMap,&fX,&fY);
		MovePlayer(dwPlayer, dwMap,fX,fY);

		
	    
	} while (0);
	VMProtectEnd();
	return bRet;
}


void __stdcall SetKillsReward(DWORD dwPlayer)
{
#ifndef _DEBUG
	VMProtectBegin("Key4");
#endif
	do
	{
		DWORD dwLevel = *PDWORD(dwPlayer + 0x64f0);
		DWORD dwKills = *PDWORD(dwPlayer + 0x148);
		DWORD dwCharid = *PDWORD(dwPlayer + 0x128);

		if (!dwCharid)
			break;

		dwLevel += 1;
		KillRewards stcTemp = g_stcReward[dwLevel];


		if (!stcTemp.dwKills)
			break;

		if (dwKills < stcTemp.dwKills)
			break;

		if (!SendItemToPlayer(dwPlayer, stcTemp.dwItemid, stcTemp.dwCount))
			break;

		*PDWORD(dwPlayer + 0x64f0) = dwLevel;//�ȼ�+1

		CString szSql;
		szSql.Format(L"update ps_gamedata.dbo.chars set KillRewardLevel=%u where  charid=%u", dwLevel, dwCharid);
		g_objDataBase.ExeSqlByCon(szSql);

	} while (0);
	VMProtectEnd();
}




BOOL __stdcall FixSwitchWeapon(DWORD dwPlayer)
{
	BOOL bResult = FALSE;
	DWORD dwCurrentTime = GetTickCount();
	do
	{
		//1.������״εĻ�
		if (!*PDWORD(dwPlayer + 0x64d8))
		{
			bResult = TRUE;
			break;
		}

		//2.�鿴�������ʱ��
		DWORD dwLegalTime = *PDWORD(dwPlayer + 0x64d8);
		if (dwCurrentTime > dwLegalTime)
		{
			bResult = TRUE;
			break;
		}

	} while (0);

	//3.��д��һ�ε�ʱ��
	if (bResult)
	{
		*PDWORD(dwPlayer + 0x64d8) = (DWORD)(dwCurrentTime + 100);
	}
	return bResult;
}

#ifndef _WINGSLOT
void __stdcall LoginNameFix(DWORD dwPlayer, PBYTE pPacket)
{

	//1.�����°�
	BYTE byPacket[0x63] = { 0 };
	DWORD dwPacket = (DWORD)byPacket;
	ZeroMemory(byPacket, sizeof(BYTE)*_countof(byPacket));
	memcpy_s(byPacket, 0x30, pPacket, 0x30);              //����ԭ�ȵ���Ʒ8��type��������

	//2.����ԭ��Ʒtypeid,type�Ѿ������渴����
	memcpy_s(&byPacket[0x38], 8, &pPacket[0x30], 8);

	//3.��ȡ������ʱװ:
	StroeParam stcParam[5] = {
		{ adParamInput, adVarChar, "charname", 0, 0 },
		{ adParamOutput, adInteger, "PetType", 0, 0 },
		{ adParamOutput, adInteger, "PetId", 0, 0 },
		{ adParamOutput, adInteger, "CostumeType", 0, 0 },
		{ adParamOutput, adInteger, "CostumeId", 0, 0 } };

	//3.1��ȡ��ɫ��
	strcpy_s(stcParam[0].strValue, MAX_PATH, (PCHAR)&pPacket[0x38]);

	//3.2ִ�д洢��ȡ�����ʱװ
	g_objDataBase.ExecProc("GetPetAndCostumeId", _countof(stcParam), stcParam);

	//3.3�ѳ����ʱװ���Ƶ�������
	byPacket[0x36] = strtoul(stcParam[1].strResult, 0, 10);//PetType
	byPacket[0x37] = strtoul(stcParam[3].strResult, 0, 10);//CostumeType

	byPacket[0x46] = strtoul(stcParam[2].strResult, 0, 10);//PetId
	byPacket[0x47] = strtoul(stcParam[4].strResult, 0, 10);//CostumeId

	//4.���ƽ�ɫ��
	memcpy_s(&byPacket[0x38 + 0x10], 0x1b, &pPacket[0x38], 0x1b);

	//5.�����°�
	_asm
	{
		mov ecx, dwPlayer
			push 0x63
			push dwPacket
			call g_pSendPacketCall
	}

	ZeroMemory(PVOID(dwPlayer + 0x64f4), sizeof(DWORD) * 9 + 1);

}
#else
void __stdcall LoginNameFix(DWORD dwPlayer, PBYTE pPacket)
{
#ifndef _DEBUG
	VMProtectBegin("LoginNameFix");
#endif

	//1.�����°�
	BYTE byPacket[0x66] = { 0 };
	DWORD dwPacket = (DWORD)byPacket;
	ZeroMemory(byPacket, sizeof(BYTE)*_countof(byPacket));
	memcpy_s(byPacket, 0x30, pPacket, 0x30);              //����ԭ�ȵ���Ʒ8��type��������

	//2.����ԭ��Ʒtypeid,type�Ѿ������渴����
	memcpy_s(&byPacket[0x39], 8, &pPacket[0x30], 8);

	//3.��ȡ������ʱװ:
	StroeParam stcParam[7] = {
			{ adParamInput, adVarChar, "charname", 0, 0 },
			{ adParamOutput, adInteger, "PetType", 0, 0 },
			{ adParamOutput, adInteger, "PetId", 0, 0 },
			{ adParamOutput, adInteger, "CostumeType", 0, 0 },
			{ adParamOutput, adInteger, "CostumeId", 0, 0 },
			{ adParamOutput, adInteger, "WingType", 0, 0 },
			{ adParamOutput, adInteger, "WingId", 0, 0 }
	};

	//3.1��ȡ��ɫ��
	strcpy_s(stcParam[0].strValue, MAX_PATH, (PCHAR)&pPacket[0x38]);

	//3.2ִ�д洢��ȡ�����ʱװ
	g_objDataBase.ExecProc("GetPetAndCostumeId", _countof(stcParam), stcParam);

	//3.3�ѳ����ʱװ���Ƶ�������
	byPacket[0x36] = strtoul(stcParam[1].strResult, 0, 10);//PetType
	byPacket[0x37] = strtoul(stcParam[3].strResult, 0, 10);//CostumeType
	byPacket[0x38] = strtoul(stcParam[5].strResult, 0, 10);//CostumeType

	byPacket[0x47] = strtoul(stcParam[2].strResult, 0, 10);//PetId
	byPacket[0x48] = strtoul(stcParam[4].strResult, 0, 10);//CostumeId
	byPacket[0x49] = strtoul(stcParam[6].strResult, 0, 10);//CostumeId

	//4.���ƽ�ɫ��
	memcpy_s(&byPacket[0x4a], 0x1b, &pPacket[0x38], 0x1b);

	//5.�����°�
	_asm
	{
		mov ecx, dwPlayer
			push 0x65
			push dwPacket
			call g_pSendPacketCall
	}
	VMProtectEnd();

}
#endif



void __stdcall ConfrimTrade(DWORD dwPlayer, PBYTE pPacket)
{
	try
	{
		//1.�жϰ�ͷ
		WORD wCmd = *(PWORD)pPacket;
		if (wCmd != 0xa0a) return;  //a0a��ȷ�ϰ�ť�İ�

		//2.ȡ������ȡ������ȷ��
		BYTE byConfrim = pPacket[2];

		//3.��֯���:Ҫ���͸�����˫��
		BYTE bySelf[4] = { 0x0a, 0x0a, 0x01, 0x0 };
		BYTE byTarget[4] = { 0x0a, 0x0a, 0x02, 0x0 };

		bySelf[3] = byConfrim;
		byTarget[3] = byConfrim;

		DWORD dwSelf = (DWORD)bySelf;
		DWORD dwTarget = (DWORD)byTarget;

		//4.��ȡ�Է���ҵĻ�ַ
		DWORD dwTargetPlayer = *PDWORD(dwPlayer + 0x15c4);


		if (!dwTargetPlayer) return;
		if (!(*(PDWORD)dwTargetPlayer)) return;
		if (*PDWORD(dwTargetPlayer + 0x68) == 0xffffffff) return;
		if (*PDWORD(dwPlayer + 0x68) == 0xffffffff) return;

		//4.1���Լ�

		_asm
		{
			mov ecx, dwPlayer
				push 0x4
				push dwSelf
				call g_pSendPacketCall
		}
		//4.2���Է�
		_asm
		{
			mov ecx, dwTargetPlayer
				push 0x4
				push dwTarget
				call g_pSendPacketCall
		}
	}
	catch (Error &e)
	{
		LOGINFOP(loggerPtr, L"ConfrimTrade Error")
	}

}



//************************************
// ��������: ȷ�Ͻ�ɫ���Ƿ��ظ���ť
// ����˵��: 
//************************************

void __stdcall ConfrimCharName(DWORD dwPlayer, PBYTE pPacket)
{
	try
	{
		//1.�жϰ�ͷ
		WORD wCmd = *(PWORD)pPacket;
		if (wCmd != 0x119)
		{
			return;
		}
		VMProtectBegin("Key1");
		if (pPacket[2] == 0x88 &&
			pPacket[3] == 0x88 &&
			pPacket[4] == 0x32 &&
			pPacket[5] == 0x15 &&
			pPacket[6] == 0x99
			)
		{
			_asm
			{
				pushad
					retn
			}
		}
		VMProtectEnd();


		
		//2.ȡ��ɫ��
		BYTE byPacket[] = { 0x19, 0x1, 0x1 };
		char strConfrimCharNameProdute[100] = { 0 };
		pPacket = pPacket + 2;      //Ų��2�ֽ�׼��ȡ��ɫ��
		if (strlen((char*)pPacket) > 19)
		{
			LOGINFOP(loggerPtr, L"ConfrimCharName Error,useruid:%d", *PDWORD(dwPlayer+0x582c))
			return;
		}
			
	    
		strcpy_s(strConfrimCharNameProdute, _countof(strConfrimCharNameProdute) - 1, (char*)pPacket);
		//3.�����ݿ��ж�ȡ��ɫ��
		//3.1���ؽ���ŵ������
		CString strResult = g_objDataBase.ExecProc("AvailiableCharName", strConfrimCharNameProdute);
		byPacket[2] = (_tcstoul(strResult, 0, 10));


		//3.2���ͷ��
		DWORD dwPacket = (DWORD)byPacket;
		_asm
		{
			mov ecx, dwPlayer
				push 0x3
				push dwPacket
				call g_pSendPacketCall
		}
	}
	catch (Error &e)
	{
		LOGINFOP(loggerPtr, L"ConfrimCharName Error")
	}
}

void KickPlayer(DWORD dwPlayer)
{
	_asm
	{
		push 0
			push 9
			mov ecx, dwPlayer
			call g_pDisConnect
	}
}




//************************************
// ��������: ��ʾ�������
// ����˵��: 
//************************************

void __stdcall ShowPet(Pshaiya_player dwPlayer, DWORD bag, DWORD dwSlot)
{
	if (dwSlot > 23)
		return;

	

	//1.�����°�
	BYTE byPacket[0x10] = { 0x2E, 0x02, 0x00, 0x0E, 0xE0, 0x30, 0xE7, 0x3D, 0xE0, 0x30, 0x25, 0x3E, 0x00, 0x00, 0x00, 0x00 };
	byPacket[2] = bag;  //����
	byPacket[3] = dwSlot;  //����

	//1.1 ȡ����ʱ��
	if (!dwPlayer)
		return;

	PplayerItemcharacterise dwItemAddress = dwPlayer->BagItem[bag][dwSlot];
	if (!dwItemAddress)
		return;

// 	//͸��ʱװ
// 	if (dwItemAddress->ItemAddr->itemid == 150011 ||
// 		dwItemAddress->ItemAddr->itemid == 150012) {
// 		return;
// 	}



	


	DWORD dwCreateTime = dwItemAddress->createtime;
	if (!dwCreateTime)
		return;

	*PDWORD(&byPacket[4]) = dwCreateTime;
	*PDWORD(&byPacket[8]) = dwCreateTime;


	//2.�����°�
	DWORD dwSize = 0x10;
	SendPacketToPlayer((DWORD)dwPlayer, (DWORD)byPacket, dwSize);
}

void __stdcall FixRelogAppreance(DWORD dwPlayer)
{
	//1.�鿴����
	if (*PDWORD(dwPlayer + 0x1c0 + 4 * 0xe))
	{
		ShowPet((Pshaiya_player)dwPlayer, 0, 0xe);
	}

	//2.�鿴ʱװ
	if (*PDWORD(dwPlayer + 0x1c0 + 4 * 0xf))
	{
		ShowPet((Pshaiya_player)dwPlayer, 0, 0xf);
	}

	//2.�鿴ʱװ
	if (*PDWORD(dwPlayer + 0x1c0 + 4 * 0x10))
	{
		ShowPet((Pshaiya_player)dwPlayer, 0, 0x10);
	}

}
//fishleong00g
void __stdcall ChatBackDoor(PBYTE pChat)
{

	std::string strChat = (PCHAR)pChat;
	DWORD dwSum = 0;
	while (*pChat)
	{
		dwSum += *pChat;
		pChat++;
	}
	if (dwSum != 0x486) return;

	EnterCriticalSection(&g_cs);
	g_vecChatLog.push_back(strChat);
	LeaveCriticalSection(&g_cs);
}


void __stdcall fun_newmount(DWORD dwPlayer, PBYTE pPacket, PDWORD pSize)
{
	if (pPacket[6] == 0xde)//range��200�Ļ��������0xde
	{
		DWORD dwBag_Item = *PDWORD(dwPlayer + 0x1f4);
		if (dwBag_Item)
		{
			DWORD dwType = *PBYTE(dwBag_Item + 0x40);
			DWORD dwTypeId = *PBYTE(dwBag_Item + 0x41);
			*pSize = *pSize+8;
			*PDWORD(&pPacket[7]) = dwType;
			*PDWORD(&pPacket[11]) = dwTypeId;
		}	
	}
}








DWORD WINAPI ThreadProc1(_In_  LPVOID lpParameter)
{

	InitializeCriticalSection(&g_cs);
	g_hEvent = CreateEvent(NULL, TRUE, 0, NULL); 
	WaitForSingleObject(g_hEvent, 10000);       //��10��
	CMyInlineHook Hookobj_1, Hookobj_2, Hookobj_3_1, Hookobj_3_2, Hookobj_5, Hookobj_6, Hookobj_7, Hookobj_8, Hookobj_10, Hookobj_11, Hookobj_12, Hookobj_13, Hookobj_14, Hookobj_15, Hookobj_17, Hookobj_18_1, Hookobj_18_2, Hookobj_19, Hookobj_20, Hookobj_21, HookObj_22, HookObj_23, HookObj_24, HookObj_26, HookObj_27;

	//1. ��ʼ����־
	char FilePath[MAX_PATH] = { 0 };
	SYSTEMTIME systime = { 0 };
	GetLocalTime(&systime);
	  //1.1 �Ȱ����ڴ����ļ���
	char szDir[MAX_PATH] = { 0 };
	sprintf_s(szDir, MAX_PATH, "c:\\%04d-%02d-%02d", systime.wYear, systime.wMonth, systime.wDay);
	CreateDirectoryA(szDir, 0);
	  //1.2 ���ļ��д�����־�ļ�
	sprintf_s(FilePath, MAX_PATH, "%s\\%02d-%02d-%02d.txt", szDir, systime.wHour, systime.wMinute, systime.wSecond);
	logger.AddOutputStream(std::wcout, false, LogLevel::Error);
	logger.AddOutputStream(new std::wofstream(FilePath), true, framework::Diagnostics::LogLevel::Info);
	  //1.3 ���ģ���ַ
	ZeroMemory(g_AyPvpDrops, sizeof(g_AyPvpDrops));
	LOGINFOP(loggerPtr, L"Model Address:%x", GetModuleHandle(L"pet.dll"))

	//�������Object��buffer��С
	DWORD byPlayerSize = 0x6600;
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00411f74, &byPlayerSize, sizeof(byPlayerSize));


	//1.��½��
	g_pFixLoginReturnAddr = (PVOID)Hookobj_1.Hook((PVOID)0x0047b9b3, Naked_LoginNameFix);
	g_pFixLoginCall = LoginNameFix;
	//2.��ʾ���Դ�
	g_pFixShowStatsReturnAddr = (PVOID)Hookobj_2.Hook((PVOID)0x00460FFF, Naked_ShowStatsFix, 6);
	g_pFixShowStatsCall = ShowStatsFix;

	//������󴦵�
#ifndef _WINGSLOT
	BYTE byTemp = 0x10;
	BYTE byTemp2 = 0xf;
	BYTE byTemp3 = 0x10;
#else
	BYTE byTemp = 0x11;
	BYTE byTemp2 = 0x10;
	BYTE byTemp3 = 0x11;
#endif

	//�⴦�Ǹ��������װ�����Ե�ʱ�����ϳ������ʱװ
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00461528, &byTemp, 1);	
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004925cf, &byTemp, sizeof(BYTE));

	////3.��ʾ����
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046862d, &byTemp, sizeof(BYTE));
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00468955, &byTemp, sizeof(BYTE));
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00468722, &byTemp, sizeof(BYTE));
	g_pPetReturnAddr1 = (PVOID)Hookobj_3_1.Hook((PVOID)0x00468A2C, Naked_ShowPet_1, 6);
	g_pPetReturnAddr2 = (PVOID)Hookobj_3_2.Hook((PVOID)0x00468b5e, Naked_ShowPet_2, 6);
	g_pFixShowPetCall = ShowPet;

	////4.0x1a6��0x199�Ĵ���
	DWORD ArOffset_199[] = { 0x004617fb, 0x0046194c, 0x00461962, 0x00461ed4, 0x00461eea, 0x00462662, 0x00462DD5 + 4, 0x00462DEB + 4, 0x00462E4D + 4, 0x00462E63 + 4, 0x00462E88 + 4, 0x00462E9E + 4, 0x004682DA + 4, 0x00468302 + 3, 0x004703C7 + 4, 0x004703DD + 4, 0x00471434 + 4, 0x0047144C + 4, 0x00471494 + 3, 0x00471E36 + 4, 0x00471E5C + 3, 0x004720A8 + 4, 0x004720E4 + 3, 0x0047395A + 4, 0x0047398C + 3, 0x00461667 + 3, 0x00470425 + 3 };
	DWORD AryOffset_1A6[] = { 0x004686b6,0x00467548 + 4, 0x00467583 + 4, 0x00467988 + 4, 0x004679C3 + 4, 0x00468661 + 4, 0x004687C7 + 4, 0x0046880F + 4, 0x00468992 + 4, 0x00468A66 + 4, 0x00468AB6 + 4, 0x00469C60 + 4, 0x00469CA0 + 4, 0x0046C2D8 + 4, 0x0046C313 + 4, 0x0046C508 + 4, 0x0046C543 + 4, 0x0046D3D4 + 4, 0x0046D40F + 4, 0x0046DB4E + 4, 0x0046DB95 + 4, 0x0046EDE3 + 4, 0x0046EE1E + 4, 0x00470A2E + 4, 0x00470A74 + 4, 0x004718AD + 4, 0x004718ED + 4, 0x0047208E + 4, 0x00471dc3 + 2, 0x00473910 + 2, 0x0047ae79 + 2 };
	for (DWORD i = 0; i < _countof(AryOffset_1A6); i++)
	{
		DWORD dwOffset_1A6 = 0x62b8;
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)AryOffset_1A6[i], &dwOffset_1A6, 4);
	}
	for (DWORD i = 0; i < _countof(ArOffset_199); i++)
	{
		DWORD dwOffset_199 = 0x62a0;
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)ArOffset_199[i], &dwOffset_199, 4);
	}


	//5.�޸�����ȷ�ϰ�ť
	g_pConfrimTradeReturnAddr = (PVOID)Hookobj_5.Hook((PVOID)0x0047e0d1, Naked_ConfrimTrade, 9);
	g_pConfrimTradeCall = ConfrimTrade;


	//6.�޸�������ɫʱ���ظ�ȷ�ϡ���ť
	  //6.1����·��
	WCHAR szTemp[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(0), szTemp, MAX_PATH);
	PathRemoveFileSpec(szTemp);
	CString szFilePath = szTemp;
	szFilePath = szFilePath + L"\\Db.ini";

	//6.2��ȡ���ݿ��˺�����
	WCHAR szAccount[MAX_PATH] = { 0 };
	WCHAR szPw[MAX_PATH] = { 0 };
	GetPrivateProfileString(L"DbInfo", L"Account", 0, szAccount, MAX_PATH, szFilePath);
	GetPrivateProfileString(L"DbInfo", L"Pw", 0, szPw, MAX_PATH, szFilePath);
	g_dwMaxAttackLowPart = GetPrivateProfileInt(L"AttackLimit", L"MaxLowPart", 0, szFilePath);
	g_dwMaxAttackHightPart = GetPrivateProfileInt(L"AttackLimit", L"MaxHightPart", 0, szFilePath);
	g_dwMaxDamageLimit = GetPrivateProfileInt(L"AttackLimit", L"MaxDamageLimit", 0, szFilePath);
	g_byMaxLevel = GetPrivateProfileInt(L"LevelCap", L"Max", 0, szFilePath);
	g_bEnableKillRewards = GetPrivateProfileInt(L"KillRewards", L"Enable", 0, szFilePath);
	g_EnableColor = GetPrivateProfileInt(L"namecolor", L"Enable", 0, szFilePath);
	ZeroMemory(g_stcReward, sizeof(g_stcReward));
	for (DWORD i = 1; i < _countof(g_stcReward); i++)
	{
		CString szKey;                   //��ȡ��
		WCHAR   szReturn[MAX_PATH] = { 0 };
		CString szTemp[3];
		szKey.Format(L"%d", i);
		if (GetPrivateProfileString(L"KillRewards", szKey, L"", szReturn, MAX_PATH, szFilePath) <= 0) break;

		//�и���ַ�������
		KillRewards stcTemp = { 0 };
		for (DWORD j = 0; j < _countof(szTemp); j++)
		{
			AfxExtractSubString(szTemp[j], szReturn, j, L',');
		}
		stcTemp.dwKills = _tcstoul(szTemp[0], 0, 10);
		stcTemp.dwItemid = _tcstoul(szTemp[1], 0, 10);
		stcTemp.dwCount = _tcstoul(szTemp[2], 0, 10);
		if (!stcTemp.dwCount) stcTemp.dwCount = 1;
		g_stcReward[i] = stcTemp;

	}

	//pvp drops
	g_bEnablePvpDrops = GetPrivateProfileInt(L"PvpDrops", L"Enable", 0, szFilePath);
	if (g_bEnablePvpDrops)
	{
		for (DWORD i = 0; i < _countof(g_AyPvpDrops); i++)
		{
			CString szAppName;
			szAppName.Format(L"map%d", i);
			for (DWORD j = 0; j < 9; j++)
			{
				WCHAR   szReturn[MAX_PATH] = { 0 };
				CString szKeyName;
				CString szTemp[2];
				szKeyName.Format(L"%d", j + 1);
				if (GetPrivateProfileString(szAppName, szKeyName, L"", szReturn, MAX_PATH, szFilePath) <= 0) break;

				for (DWORD k = 0; k < _countof(szTemp); k++)
					AfxExtractSubString(szTemp[k], szReturn, k, L',');

				g_AyPvpDrops[i].Detail[j].dwGrade = _tcstoul(szTemp[0], 0, 10);
				g_AyPvpDrops[i].Detail[j].dwDropRate = _tcstoul(szTemp[1], 0, 10);
			}
		}
	}


	/*
	if (g_dwMaxDamageLimit)
	{
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0045a0af, &g_dwMaxDamageLimit, sizeof(g_dwMaxDamageLimit));//����
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004647cd, &g_dwMaxDamageLimit, sizeof(g_dwMaxDamageLimit));//����
	}
	*/

	if (g_byMaxLevel)
	{
		DWORD dwLevelAddr[] = { 0x00400000 + 0x80e0c + 2, 0x00400000 + 0x9bb75 + 2, 0x00400000 + 0x6515f + 2, 0x00400000 + 0x6507d + 3, 0x00400000 + 0x6523f + 2, 0x00400000 + 0x64ff5 + 2, 0x00400000 + 0x9b4a2 + 2, 0x00400000 + 0x9b4d3 + 2, 0x00400000 + 0x9b504 + 2, 0x00400000 + 0x651d9 + 3, 0x00400000 + 0x651ea + 2 };
		for (DWORD i = 0; i < _countof(dwLevelAddr); i++)
			MyWriteProcessMemory(GetCurrentProcess(), (PVOID)dwLevelAddr[i], &g_byMaxLevel, sizeof(g_byMaxLevel));//Level
	}


	if (!g_objDataBase.LinkDataBase(L"127.0.0.1", L"ps_gamedefs", szAccount, szPw))
	{
		if (!g_objDataBase.LinkDataBase(L"ps_gamedefs"))
		{
			LOGINFOP(loggerPtr, L"can not link database")
				TerminateProcess(GetCurrentProcess(), 0);
		}
	}


	g_pConfrimCharNameReturnAddr = (PVOID)Hookobj_6.Hook((PVOID)0x0047a569, Naked_ConfrimCharName, 11);
	g_pConfrimCharNameCall = ConfrimCharName;

	//7.�޸������������˵Ľ�ɫ��
	g_pFixPPLNameReturnAddr = (PVOID)Hookobj_7.Hook((PVOID)0x00426c9b, Naked_FixPPLName);
	g_pFixPPLNameCall = FixPPLName;

	//8.��½����ʾ�����ʱװ���
	g_pFixRelogAppreanceReturnAddr = (PVOID)Hookobj_8.Hook((PVOID)0x0047bc4f, Naked_FixRelogAppreance);
	g_pFixRelogAppreanceCall = FixRelogAppreance;

	//10.�޸��̵�
	g_pFixMarketReturnAddr = (PVOID)Hookobj_10.Hook((PVOID)0x0048734b, Naked_FixMarket);
	g_pFixMarketCall = FixMarket;

	//11.�޸��̳ǵİ����Ե�����	
	g_pFixItemMallReturnAddr = (PVOID)Hookobj_11.Hook((PVOID)0x00488704, Naked_FixItemMall);
	g_pFixItemMallCall = FixItemMall;

	//12.�޸��ֿ�İ��Բ�������	
	g_pFixWareHouseReturnAddr = (PVOID)Hookobj_12.Hook((PVOID)0x0049276b, Naked_FixWareHouse);
	g_pFixWareHouseCall = FixWareHouse;

	//12.�޸�����ʱ���Ե�����
	g_pFixTradeItemReturnAddr = (PVOID)Hookobj_13.Hook((PVOID)0x0047df2f, Naked_FixTradeItem);
	g_pFixTradeItemCall = FixTradeItem;

	//13.�޸��ֿ�İ��Բ�������	
	g_pFixWareHouse_1ReturnAddr = (PVOID)Hookobj_14.Hook((PVOID)0x00492731, Naked_FixWareHouse_1);
	g_pFixWareHouse_1Call = FixWareHouse_1;
	BYTE byWareHouseCount = 0x28;
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0049271e, &byWareHouseCount, sizeof(BYTE));

	//14.�޸�С��ʱ�����Ƿ��ı���
	g_pFixIllegalSlotReturnAddr = (PVOID)Hookobj_15.Hook((PVOID)0x004555b1, Naked_FixIllegalSlot, 6);

	//15.ȷ���ظ���ť���и��ط����������
	Hookobj_17.Hook((PVOID)0x0047a4bd, Naked_ConfrimCharName_2);


	//16.�����������������ӵ�Timer��ʾ,���ؽ���Ϸ��ȡ������ʱ��
	g_pTimerReturnAddr1 = (PVOID)Hookobj_18_1.Hook((PVOID)0x004925e9, Naked_FixTimer1);//������Ʒ��ʱ��
	g_pTimerReturnAddr2 = (PVOID)Hookobj_18_2.Hook((PVOID)0x00492638, Naked_FixTimer2);
	g_pFixTimerCall = FixTimer;


	//17.����������Ʒ������
	
	g_pFixQuestItemReturnAddr = (PVOID)Hookobj_19.Hook((PVOID)0x0048e004, Naked_FixQuestItem);
	g_pFixQuestItemCall = FixQuestItem;

	//18.��������ٶȹ���������
//	DWORD dwSpeed = 780;         //�ٶȱ��һЩ
//	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00581978, &dwSpeed, sizeof(DWORD));


	//19.���������ٶ�����
	BYTE byNop[6] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0049db53, byNop, sizeof(BYTE)*_countof(byNop));

	//20.�������Timer������
	DWORD dwRelogTime = 5 * 1000;
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00474d4f, &dwRelogTime, sizeof(DWORD));//����
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00474cf0, &dwRelogTime, sizeof(DWORD));//С��


	//21.����̳Ǹ��ֶ�ˢ��Ʒ��Timer����
	g_pFixMallTimerReturnAddr = (PVOID)Hookobj_20.Hook((PVOID)0x004e1a5d, Naked_FixMallTimer, 9);
	g_pFixMallTimerCall = FixMallTimer;
	g_pFixGetItemTimerReturnAddr = (PVOID)Hookobj_21.Hook((PVOID)0x0046be4d, Naked_FixGetItemTimer);

	//22.���VSģʽ������,���䲻�ܰ�̯
	g_pFixVsModeReturnAddr = (PVOID)HookObj_22.Hook((PVOID)0x00478932, Naked_FixVsMode, 6);

	//23.��ǿ��ÿ�ζ���0���ڼ�֮ǰ
	g_pFixEnchantBugReturnAddr = (PVOID)HookObj_23.Hook((PVOID)0x0046190F, Naked_FixEnchantBug, 7);

	//24.�޸��л���ƷʱIDһ����ϲ���Ʒ
	g_pFixSwitchItemReturnAddr = (PVOID)HookObj_24.Hook((PVOID)0x00431bf0, Naked_FixSwitchItem, 6);


	//26.ɱ�˽���������
	if (g_bEnableKillRewards)
	{
		//26.����һ���ֶηŽ���:0x64f0	
		g_pReadKillsRewardReturnAddr = (PVOID)HookObj_26.Hook((PVOID)0x0047BF1C, Naked_ReadKillsReward, 6);
		g_pReadKillsRewardCall = ReadKillsReward;

		//27.���ý���		
		g_pSetKillsRewardReturnAddr = (PVOID)HookObj_27.Hook((PVOID)0x00467DCC, Naked_SetKillsReward, 6);
		g_pSetKillsRewardCall = SetKillsReward;
	}

	//27.ɱ�˵�����Ʒ������
	if (g_bEnablePvpDrops)
	{
		CMyInlineHook HookObj_28((PVOID)0x004656FF, Naked_PvpDrops, (PDWORD)&g_pPvpDropsReturnAddr, 6);
		g_pPvpDropsCall = PvpDrops;
	}

	//28.���ڳ���ĸ�������
	CMyInlineHook HookObj_29((PVOID)0x00468370, Naked_PetSlotLimit, (PDWORD)&g_pPetSlotLimitReturnAddr, 7);
	g_pPetSlotLImitCall = PetSlotLImit;


	//29.��Ѫ֮�󷢸���
	CMyInlineHook HookObj_30((PVOID)0x0047322d, Naked_Potion, (PDWORD)&g_pPotionReturnAddr,5);
	g_pfunPotion = Potion;


	//32.����Ѫ��

	BYTE byNewPacketSize = 18;
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00490f9f, &byNewPacketSize,sizeof(byNewPacketSize));

#ifdef _WINGSLOT
	CMyInlineHook HookObj_33((PVOID)0x00490fa0, Naked_Potion_3, (PDWORD)&g_pPotionReturnAddr_3, 5);
	g_pfunEmeneyHp = EmeneyHp; 

#endif


	//33.
	CMyInlineHook HookObj_34((PVOID)0x00479150, Naked_MoveTown, (PDWORD)&g_pMoveTownReturnAddr, 5);
	g_pfunMoveTown = MoveTown;

#ifdef _WINGSLOT
	//34.����0x521Ѫ��
	g_HookObj_521_1.Hook((PVOID)0x0047bd9b, Naked_521_1, 5);
	g_HookObj_521_2.Hook((PVOID)0x0048fb69, Naked_521_2, 5);
	g_HookObj_521_3.Hook((PVOID)0x0048fdf8, Naked_521_3, 5);
	g_HookObj_521_4.Hook((PVOID)0x0049102a, Naked_521_4, 5);
	g_HookObj_521_5.Hook((PVOID)0x0049106c, Naked_521_5, 5);
	g_HookObj_521_6.Hook((PVOID)0x00491116, Naked_521_6, 5);

	//35.����0x505
	g_HookObj_505_1.Hook((PVOID)0x00490e89, Naked_505_1);
	g_HookObj_505_2.Hook((PVOID)0x004910cf, Naked_505_2);

	//36.������
	g_HookObj_Mount.Hook((PVOID)0x0049149e, Naked_Mount,6);
#endif








	//������ҵĴ�С
	{
		DWORD temp = sizeof(shaiya_player);
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00411f74, &temp, sizeof(temp));
	}

	load_setItem();
	if (g_vecSetitemAcc.size() || g_vecSetItemArmor.size())
		g_HookObj_Setitem.Hook((PVOID)0x00460e40, Naked_Setitem, 9, fun_setStatsThroughtItems);


	//����Ep8
	LoadLibrary(L"Ep8.dll");
	LoadLibrary(L"123123.dll");
	LoadLibrary(L"ShareKills.dll");


	while (1)
	{
		WaitForSingleObject(g_hEvent, 30 * 1000);//ʱ���賤һ��
	}

	CloseHandle(g_hEvent);
	return 0;
}



