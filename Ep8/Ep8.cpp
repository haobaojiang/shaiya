#include <MyClass.h>
#include <windows.h>
#include <fstream>


#include <MyInlineHook.h>
#include <MyPython.h>
#include  <vector>
#include "VMProtectSDK.h"
#include <Shlwapi.h>
#include <stddef.h>
#include <iostream>
#pragma comment(lib,"Shlwapi.lib")
#include <map>


using namespace std;
#include <common.h>
#include "Ep8.h"


#pragma comment(linker, "/EXPORT:DebugConnect=dbgengOrg.DebugConnect,@1")
#pragma comment(linker, "/EXPORT:DebugConnectWide=dbgengOrg.DebugConnectWide,@2")
#pragma comment(linker, "/EXPORT:DebugCreate=dbgengOrg.DebugCreate,@3")

using namespace EP6CALL;

#pragma warning(disable:4244)
#pragma warning(disable:4996)
#pragma warning(disable:4005)

#define reNewMarketTail(pdst,psrc,_size) memcpy(PVOID((DWORD)pdst+sizeof(COMMON_ITEMBODY_MARKET)),\
	PVOID((DWORD)psrc+sizeof(OLD_COMMON_ITEMBODY_MARKET)),_size\
	)

#define reNewMarketHead(pNew,pOld,_type) (memcpy(pNew,pOld,offsetof(_type,body)))

#define CHAR_TO_WCHAR(lpChar,lpW_Char) \
MultiByteToWideChar(CP_ACP,0,lpChar,-1,lpW_Char,_countof(lpW_Char));



#define IsPet(byType) (byType==150||byType==120 || byType==121?TRUE:FALSE)
#define DefaultDays (30)


void fun_ProcessOnLogin(Pshaiya_player player);
void __stdcall ShowPet(DWORD, DWORD, DWORD);

std::vector<CHAOTICSQUARE_MATERIALS> g_vecMaterials;
std::vector<CHAOTICSQUARE_synthesizeItem> g_vecsynthesizeItems;
std::vector<PSETITEMDATA> g_vecSetItemArmor;
std::vector<PSETITEMDATA> g_vecSetitemAcc;


//vector<BATTLEFIELDINFO> g_vecBattleField;

BATTLEFIELDINFO g_BattleFields[300] = { 0 };

//用来处理6个任务的
typedef struct
{
	BYTE _type_4;
	BYTE _typeid_4;
	BYTE _type_5;
	BYTE _typeid_5;
	BYTE _type_6;
	BYTE _typeid_6;
}QUEST_REWARD, *PQUEST_REWARD;

Pshaiya_player g_rewardPlayer = NULL;
DWORD g_rewardQuestId = 0;
DWORD g_rewardIndex = 0;
QUEST_REWARD g_pQuestRewardExtend[5000] = { 0 };


using std::vector;

CMyInlineHook Hookobj_loginName, g_ObjGetenchant, g_objMoveitem, g_objMoveWhItem, Hookobj_showStats, HookObj_ShowKill, g_HookObj_HpOnlogin;
CMyInlineHook g_Hookobj_GetItemTimer, g_Hookobj_MallTimer, g_Hookobj_WareHouse, g_Hookobj_GetwareHouse_1;
CMyInlineHook g_Hookobj_Meet;
CMyInlineHook g_Hookobj_Timer1;
CMyInlineHook g_Hookobj_Timer2;
CMyInlineHook g_HookObj_MoveWhTobag;
CMyInlineHook g_objAutoStats;
CMyInlineHook g_objgmMoveSpeed;
CMyInlineHook g_objEnchantResult;
CMyInlineHook g_objMoveWhToWh;
CMyInlineHook g_objSort;
CMyInlineHook objQuickslot;
CMyInlineHook g_objSendQuickslot;
CMyInlineHook g_objReadCharItemDetail;
CMyInlineHook g_objsendDyeAppreance;

CMyInlineHook g_objIllageFix1;
CMyInlineHook g_objuseTransform;
CMyInlineHook g_objDisableTransform;
CMyInlineHook g_objShowDye_Entermap;
CMyInlineHook g_objConfrimCharname;
CMyInlineHook g_objEnterZoneNotice_1;
CMyInlineHook g_objEnterZoneNotice_2;
CMyInlineHook g_objMarkectRecivedOnsale;
CMyInlineHook g_objMarketRecivedItem;
CMyInlineHook g_objMarketBidItem;
CMyInlineHook g_objMarketCompeltedItem;
CMyInlineHook g_objMarketCompletedMoney;
CMyInlineHook g_objMarketAddingtoFavorite;

CMyInlineHook g_objMarketSaleRegisteredItem;
CMyInlineHook g_objMarketInterestedItems;
CMyInlineHook g_objMarketBuyitem;
CMyInlineHook g_objMarketCompletedAdditem_1, g_objMarketCompletedAdditem_2;
CMyInlineHook g_objMarketUnknown[20];
CMyInlineHook g_objRead_UserstoredItems;
CMyInlineHook g_objGuild_readItem;

CMyInlineHook g_objBuyItemmallItem;
CMyInlineHook g_objDragTradeItem;
CMyInlineHook g_objRecivedTradeItem;
CMyInlineHook g_objShopView;
CMyInlineHook g_objMarket_dyecolor;
CMyInlineHook g_objNewMountAppreance;
CMyInlineHook g_HookObj_Setitem;
CMyInlineHook g_objRemoveEnchantMaterials;
CMyInlineHook g_objRemoveEnchantMaterials_1;
CMyInlineHook g_objnpcQuestType;
CMyInlineHook g_objQuestRewardIndex;
CMyInlineHook g_objExtractLapis;
CMyInlineHook g_objQuestRewardExtend;
CMyInlineHook g_objRemoteNpc;
CMyInlineHook g_objEnchantResult_2;
CMyInlineHook g_objSendHookList;
CMyInlineHook g_objReadCharInfo_callBack;
CMyInlineHook g_objSaveCharInfo_callBack;
CMyInlineHook g_objLevelupHpset;
CMyInlineHook g_objHpset;
CMyInlineHook g_objEspCheak;
CRITICAL_SECTION g_cs, g_csQuest;


MyPython* g_objPy = NULL;
WEAPONENCHANT_ADDVALUE g_Enchant_Addvalue;



typedef void(__stdcall *fun_PacketReNew)(Pshaiya_player, PVOID, DWORD);
std::map<official_packet_const, fun_PacketReNew> g_mapCommonPacketFun;
void __stdcall fun_antuStats_send(Pshaiya_player player);




HOOKADDR g_hookAddr;





//遍历寻找匹配的封包更新函数
void __stdcall fun_commonPacketReNew(Pshaiya_player player, PVOID packet, DWORD packetSize) {
	std::map <official_packet_const, fun_PacketReNew>::iterator Iter;
	WORD cmd = *PWORD(packet);
	//去找匹配的处理函数
	for (Iter = g_mapCommonPacketFun.begin(); Iter != g_mapCommonPacketFun.end(); Iter++) {
		if (Iter->first == cmd) {
			return Iter->second(player, packet, packetSize);
		}
	}

	//找不到就直接发包
	SendPacketToPlayer(player, packet, packetSize);

}



void __stdcall fun_useMount(Pshaiya_player player, SHOW_MOUNT* pPacket, DWORD dwSize)
{
	if (pPacket->model == 0xde)
	{
		PplayerItemcharacterise pItem = player->BagItem[0][13];
		if (pItem)
		{
			SHOW_MOUNT stc;
			stc.dwCharid = pPacket->dwCharid;
			stc.model = pPacket->model;
			stc._type = pItem->ItemType;
			stc._typeid = pItem->Typeid;
			SendPacketToPlayer(player, &stc, sizeof(stc));
		}
	}
	else
	{
		SendPacketToPlayer(player, pPacket, dwSize);
	}
}




void __stdcall fun_newmount(Pshaiya_player player, SHOW_MOUNT* pPacket, PDWORD pSize)
{
	if (pPacket->model == 0xde)//range是200的话就是这个0xde
	{
		PplayerItemcharacterise pItem = player->BagItem[0][13];
		if (pItem)
		{
			pPacket->_type = pItem->ItemType;
			pPacket->_typeid = pItem->Typeid;
			*pSize = sizeof(SHOW_MOUNT);
		}
	}
}


void getBasicDyeColor(DWORD charid, PBASICDYEAPPREANCE pcolor)
{
	Pshaiya_player player = (Pshaiya_player)GetPlayerByCharid(charid);
	if (!player)
		return;

	for (DWORD i = 0; i < 17; i++)
	{
		PplayerItemcharacterise pItem = player->BagItem[0][i];
		if (pItem) {
			DWORD color = pItem->color.Hue.value;
			pcolor->color[0].IsEnable[i] = color ? true : false;
			pcolor->color[0].color[i] = color;
		}
	}
}


void Mylog(char* strMessage)
{
	SYSTEMTIME stc;
	GetLocalTime(&stc);
	MyLog("%d-%d %02d:%02d:%02d,Event:%s\n", stc.wMonth, stc.wDay, stc.wHour, stc.wMinute, stc.wSecond, strMessage);
}



void getBasicDyeColor(std::string strCharname, PBASICDYEAPPREANCE pcolor)
{
	for (DWORD i = 0; i < 17; i++)
	{
		std::string strResult = "";
		g_objPy->lock();
		try
		{

#ifdef _DEBUG
			Mylog("getBasicDyeColor begin");
#endif
			strResult = boost::python::call_method<std::string>(g_objPy->m_module->get(), "get_dyecolor_byName", strCharname.c_str(), i);
		}
		catch (...)
		{
			MyLog("getBasicDyeColor:%s\n", getPythonException().c_str());
		}

#ifdef _DEBUG
		Mylog("getBasicDyeColor end");
#endif
		g_objPy->Unlock();
		if (strResult.empty())
			continue;;

		DWORD color_1 = 0;
		DWORD color_2 = 0;
		DWORD color_3 = 0;
		sscanf(strResult.c_str(), "%ld,%ld,%ld", &color_1, &color_2, &color_3);  //暂时只取一个颜色
		pcolor->color[0].IsEnable[i] = color_1 ? true : false;
		pcolor->color[0].color[i] = color_1;
	}
}


void __stdcall LoginNameFix(Pshaiya_player player, Pold_LoginToon pOld, DWORD dwSize)
{
	PLoginToon pNew = new LoginToon;
	InitPacket(pNew, sizeof(LoginToon));
	memcpy(pNew, pOld, DWORD(&pNew->item_type) - DWORD(pNew));//一直复制到item_type

	//把8个itemtype 和id拷贝过来
	for (DWORD i = 0; i < 8; i++)
	{
		pNew->item_type[i] = pOld->item_type[i];
		pNew->item_id[i] = pOld->item_id[i];
	}

	//获取 宠物 时装 翅膀的id
	DWORD dwRet = 0;
	std::string strResult = "";

	g_objPy->lock();
	try
	{
#ifdef _DEBUG
		Mylog("LoginNameFix begin");
#endif
		strResult = boost::python::call_method<std::string>(g_objPy->m_module->get(), "GetPetAndCostumeId", pOld->charname);
	}
	catch (...)
	{
		;
	}
#ifdef _DEBUG
	Mylog("LoginNameFix end");
#endif
	g_objPy->Unlock();

	if (!strResult.empty())
	{
		int pet_type = 0;
		int pet_typeid = 0;
		int costume_type = 0;
		int costume_typeid = 0;
		int wing_type = 0;
		int wing_typeid = 0;

		sscanf(strResult.c_str(), "%d,%d,%d,%d,%d,%d",
			&pet_type, &pet_typeid,
			&costume_type, &costume_typeid,
			&wing_type, &wing_typeid);

		pNew->item_type[14] = pet_type;
		pNew->item_id[14] = pet_typeid;
		pNew->item_type[15] = costume_type;
		pNew->item_id[15] = costume_typeid;
		pNew->item_type[16] = wing_type;
		pNew->item_id[16] = wing_typeid;
	}

	//复制角色名
	memcpy(pNew->charname, pOld->charname, sizeof(pOld->charname));

	//获取身上装的装备颜色
	getBasicDyeColor(pNew->charname, &pNew->color);

	SendPacketToPlayer(player, pNew, sizeof(LoginToon));

	player->set_item.IsSetted = false;
	ZeroMemory(&player->set_item.BackUpValue, sizeof(player->set_item.BackUpValue));


	delete pNew;
}

void __stdcall FixPPLName(Pshaiya_player player, Pold_Meet pPacket, DWORD dwPacketSize)
{
	Pshaiya_player pTargetPlayer = Pshaiya_player(GetPlayerByCharName(pPacket->name));
	if (!pTargetPlayer) {
		return SendPacketToPlayer(player, pPacket, dwPacketSize);
	}


	Meet stcMeet;
	InitPacket(&stcMeet, sizeof(Meet));
	memcpy(&stcMeet, pPacket, DWORD(&pPacket->name) - DWORD(pPacket));//一直复制到老包的角色名

	strcpy(stcMeet.name, pPacket->name); //角色名

	//老包只有8件装备，所以这里要加多一些装备
	PplayerItemcharacterise pItem = pTargetPlayer->BagItem[0][0xe];
	if (pItem)
	{
		stcMeet._item[0xe].type = pItem->ItemType;
		stcMeet._item[0xe]._typeid = pItem->Typeid;
	}
	pItem = pTargetPlayer->BagItem[0][0xf];
	if (pItem)
	{
		stcMeet._item[0xf].type = pItem->ItemType;
		stcMeet._item[0xf]._typeid = pItem->Typeid;
	}

	pItem = pTargetPlayer->BagItem[0][0x10];
	if (pItem)
	{
		stcMeet._item[0x10].type = pItem->ItemType;
		stcMeet._item[0x10]._typeid = pItem->Typeid;
	}


	//说明有行会
	if (dwPacketSize > sizeof(old_Meet))
	{
		if (pPacket->guildname)
			memcpy(stcMeet.guildname, &pPacket->guildname, 20);                     //会长是在这
		else
			memcpy(stcMeet.guildname, (char*)(DWORD(&pPacket->guildname) + 6), 20);  //非会长在这
	}

	SendPacketToPlayer(player, &stcMeet, sizeof(stcMeet));


	//查看是否有dyeed装备有的话也发个包给别人
	MEETAPPREANCE stc;
	_InitPacket(stc);
	stc.charid = pTargetPlayer->Charid;
	stc.unKnown = 0;
	for (DWORD i = 0; i < 17; i++)
	{
		pItem = pTargetPlayer->BagItem[0][i];
		if (pItem&&pItem->color.Hue.bEnable)
		{
			stc.Item_type = pItem->ItemType;
			stc.slot = i;
			stc.type_id = pItem->Typeid;
			memcpy(&stc.color, &pItem->color, sizeof(DyeColor));
			SendPacketToPlayer(player, &stc, sizeof(stc));
		}
	}


	//if transform skill using
	if (pTargetPlayer->isUsedTransformationSkill)
	{
		TRANSFORM stc;
		stc.charid = pTargetPlayer->Charid;
		stc.isUsedTransform = true;
		SendPacketToPlayer(player, &stc, sizeof(stc));
	}
}



void __stdcall fun_Meet_1(DWORD dwEcx, DWORD dwEax, DWORD dwObject, Pold_Meet pPacket, DWORD dwPacketSize)
{
	PMeet p = (PMeet)pPacket;
	bool IsAlloc = false;

	do
	{
		if (pPacket->Cmd != official_packet_const::Meetppl)
			break;

		Pshaiya_player pTargetPlayer = Pshaiya_player(GetPlayerByCharName(pPacket->name));
		if (!pTargetPlayer) {
			break;
		}

		IsAlloc = true;
		p = new Meet;
		dwPacketSize = sizeof(Meet);
		memcpy(p, pPacket, DWORD(&pPacket->name) - DWORD(pPacket));
		strcpy(p->name, pPacket->name);
		PplayerItemcharacterise pItem = pTargetPlayer->BagItem[0][0xe];
		if (pItem)
		{
			p->_item[0xe].type = pItem->ItemType;
			p->_item[0xe]._typeid = pItem->Typeid;
		}
		pItem = pTargetPlayer->BagItem[0][0xf];
		if (pItem)
		{
			p->_item[0xf].type = pItem->ItemType;
			p->_item[0xf]._typeid = pItem->Typeid;
		}

		pItem = pTargetPlayer->BagItem[0][0x10];
		if (pItem)
		{
			p->_item[0x10].type = pItem->ItemType;
			p->_item[0x10]._typeid = pItem->Typeid;
		}

		if (dwPacketSize > sizeof(old_Meet))
		{
			if (pPacket->guildname) {
				memcpy(p->guildname, &pPacket->guildname, 20);
			}
			else {
				memcpy(p->guildname, &(PCHAR(&pPacket->guildname)[6]), 20);
			}
		}

	} while (0);

	DWORD dwCall = 0x00427470;
	_asm
	{
		push dwPacketSize
		push p
		push dwObject
		mov ecx, dwEcx
		mov eax, dwEax
		call dwCall
	}

	if (IsAlloc)
		delete p;

}

void __stdcall fun_getGh(Pshaiya_player player, PCOMMONHEAD packet, DWORD dwSize)
{
	BYTE byCount = packet->byCount;

	//1.申请新包:根据原先物品的数量
	DWORD dwNewSize = sizeof(GHITEM)*byCount + sizeof(COMMONHEAD);
	PVOID pNewPacket = (PGHITEM)new BYTE[dwNewSize];
	ZeroMemory(pNewPacket, dwNewSize);

	//2.复制头
	memcpy(pNewPacket, packet, sizeof(COMMONHEAD));

	//3.复制主体里的每个物品
	PGHITEM pNewItem = PGHITEM(getBodyOfcommonHeader(pNewPacket));
	POLDGHITEM pOldItem = POLDGHITEM(getBodyOfcommonHeader(packet));


	for (DWORD i = 0; i < byCount; i++)
	{
		memcpy(&pNewItem[i], &pOldItem[i], offsetof(OLDGHITEM, gems));//到lapis复制完

		for (DWORD j = 0; j < _countof(pOldItem[i].gems); j++)
		{
			pNewItem[i].gems[j] = pOldItem[i].gems[j];
		}
		pNewItem[i].count = pOldItem[i].count;
		memcpy(pNewItem[i].craftname, pOldItem[i].craftname, sizeof(pOldItem->craftname));
	}
	SendPacketToPlayer((DWORD)player, (DWORD)pNewPacket, dwNewSize);

	delete[] pNewPacket;
}

void __stdcall GetWareHouse(Pshaiya_player player, Pgetwh packet, DWORD dwSize)
{
	BYTE byCount = packet->byCount;

	//1.申请新包:根据原先物品的数量
	DWORD dwNewSize = sizeof(newWhitem)*byCount + sizeof(getwh);
	Pgetwh pNewPacket = (Pgetwh)new BYTE[dwNewSize];
	ZeroMemory(pNewPacket, dwNewSize);

	//2.复制头
	memcpy(pNewPacket, packet, offsetof(getwh, item)); //item 之前的都复制

	//3.复制主体里的每个物品
	PnewWhitem pNewItem = PnewWhitem(&pNewPacket->item);
	PoldWhitem pOldItem = PoldWhitem(&packet->item);
	for (DWORD i = 0; i < byCount; i++)
	{
		memcpy(&pNewItem[i], &pOldItem[i], offsetof(oldWhitem, lapis));//到lapis复制完

		for (DWORD j = 0; j < _countof(pOldItem[i].lapis); j++)
		{
			pNewItem[i].lapis[j] = pOldItem[i].lapis[j];
		}

		pNewItem[i].count = pOldItem[i].count;
		memcpy(pNewItem[i].craftname, pOldItem[i].craftname, 21);
	}
	SendPacketToPlayer((DWORD)player, (DWORD)pNewPacket, dwNewSize);

	delete[] pNewPacket;
}

void __stdcall FixShowSkill(Pshaiya_player player, PBYTE pPacket, DWORD dwSize)
{
	//1.制新包
	DWORD dwNewSize = dwSize + 2;
	PBYTE pbuff = new BYTE[dwNewSize];//我们的包缺了2个字节
	ZeroMemory(pbuff, dwNewSize);

	//2.复制头
	CopyMemory(pbuff, pPacket, 2);
	*PWORD(&pbuff[2]) = player->skillpoint; //技能点数

	//3.复制尾
	CopyMemory(&pbuff[4], &pPacket[2], dwSize - 2);

	//4.把原来的包发给用户
	SendPacketToPlayer(player, pbuff, dwNewSize);

	delete[] pbuff;
}


void __stdcall FixHpOnLogin(DWORD dwPlayer, PBYTE pPacket, DWORD dwSize)
{
	//1.制新包
	PBYTE pbuff = new BYTE[dwSize + 12];//我们的包缺了12个字节
	ZeroMemory(pbuff, dwSize + 12);

	//2.复制头
	CopyMemory(pbuff, pPacket, 2);                //前面空的12个字节暂时不管
	CopyMemory(&pbuff[2 + 12], &pPacket[2], dwSize - 2);

	//4.把原来的包发给用户
	SendPacketToPlayer(dwPlayer, (DWORD)pbuff, dwSize + 12);

	/*	fun_ProcessOnLogin((Pshaiya_player)dwPlayer);*/


	delete[] pbuff;
}


void __stdcall useSkill(Pshaiya_player player, DWORD skill)
{
	WORD TypeDetail = *PWORD(skill + 0x32);
	if (TypeDetail == 132 && !player->isUsedTransformationSkill)
	{
		ZeroMemory(&player->TransformationValue, sizeof(player->TransformationValue));
		commonfun_setTransform(player, true);
		ShowStatsFix((PBYTE)player);
	}
}



void reNewMarketBody(PCOMMON_ITEMBODY_MARKET pNew, POLD_COMMON_ITEMBODY_MARKET pOld)
{
	ZeroMemory(pNew, sizeof(COMMON_ITEMBODY_MARKET));
	pNew->_type = pOld->_type;
	pNew->_typeid = pOld->_typeid;
	pNew->QA = pOld->QA;
	for (DWORD i = 0; i < _countof(pNew->gem); i++)
	{
		pNew->gem[i] = pOld->gem[i];
	}
	pNew->count = pOld->count;
	memcpy(pNew->craftname, pOld->craftname, _countof(pNew->craftname));

	//设置颜色
}




void __stdcall fun_extractLapis(Pshaiya_player player, POLDEXTRACTLINK pOld, DWORD dwSize)
{
	EXTRACTLINK stc;
	_InitPacket(stc);
	memcpy(&stc, pOld, offsetof(EXTRACTLINK, gem_typeid));
	for (DWORD i = 0; i < _countof(stc.gem_typeid); i++) {
		stc.gem_typeid[i] = pOld->gem_typeid[i];
	}

	memcpy(stc.gem_count, pOld->gem_count, sizeof(stc.gem_count));
	stc.money = pOld->money;
	SendPacketToPlayer(player, &stc, sizeof(stc));

}

void __stdcall fun_linkResult(Pshaiya_player player, POLDLINK_RESULT pOld, DWORD dwSize) {


	LINK_RESULT stc;
	_InitPacket(stc);
	memcpy(&stc, pOld, offsetof(OLDLINK_RESULT, money));
	stc.money = pOld->money;
	SendPacketToPlayer(player, &stc, sizeof(stc));
}



void __stdcall fun_FixItemMall(Pshaiya_player player, ITEMMALLITEMHEAD* pItemMallHead, DWORD dwSize)
{
	DWORD dwNewSize = pItemMallHead->count * sizeof(ITEMMALLITEMBODY) + sizeof(ITEMMALLITEMHEAD) + 4;
	ITEMMALLITEMHEAD* p = (ITEMMALLITEMHEAD*) new BYTE[dwNewSize];
	ZeroMemory(p, dwNewSize);

	ITEMMALLITEMBODY* pNewBody = (ITEMMALLITEMBODY*)getBodyofAnystc(p, ITEMMALLITEMHEAD);
	OLD_ITEMMALLITEMBODY* pOldBody = (OLD_ITEMMALLITEMBODY*)getBodyofAnystc(pItemMallHead, ITEMMALLITEMHEAD);


	//head
	memcpy(p, pItemMallHead, sizeof(ITEMMALLITEMHEAD));

	//body
	for (DWORD i = 0; i < pItemMallHead->count; i++)
	{
		memcpy(&pNewBody[i], &pOldBody[i], sizeof(OLD_ITEMMALLITEMBODY));
		if (IsPet(pNewBody[i]._type))
		{
			BYTE bag = pNewBody[i].bag;
			BYTE slot = pNewBody[i].slot;
			pNewBody[i].createTime = (player->BagItem[bag][slot])->createtime;
			pNewBody[i].expriedTime = (player->BagItem[bag][slot])->createtime;
		}

	}

	SendPacketToPlayer(player, p, dwNewSize);
	delete[] p;


}


void __stdcall fun_common_marketbodycorrectionAndSend(Pshaiya_player player, PVOID packet, DWORD dwSize)
{
	//1.just the body size has changed
	DWORD dwTailSize = 0;
	if (*PWORD(packet) == official_packet_const::completedAddItem_market&&dwSize == 0x3e)
	{
		dwTailSize = 0x4;
	}

	//老大小+新body差值+尾部有可能产生的大小
	DWORD dwNewSize = dwSize + sizeof(COMMON_ITEMBODY_MARKET) - sizeof(OLD_COMMON_ITEMBODY_MARKET) + dwTailSize;
	PVOID p = new BYTE[dwNewSize];


	//2.process head
	DWORD headSize = dwSize - sizeof(OLD_COMMON_ITEMBODY_MARKET) - dwTailSize;
	memcpy(p, packet, headSize);

	//2.process body
	POLD_COMMON_ITEMBODY_MARKET pOldBody = POLD_COMMON_ITEMBODY_MARKET(DWORD(packet) + headSize);
	PCOMMON_ITEMBODY_MARKET pNewBody = PCOMMON_ITEMBODY_MARKET(DWORD(p) + headSize);
	reNewMarketBody(pNewBody, pOldBody);

	//3.Process tail
	reNewMarketTail(pNewBody, pOldBody, dwTailSize);

	//3.send packet
	SendPacketToPlayer(player, p, dwNewSize);
	delete[] p;
}


void __stdcall fun_SaleRegisteredItem(Pshaiya_player player, PCOMMONHEAD packet, DWORD dwSize)
{
	BYTE byCount = packet->byCount;
	DWORD NewSize = byCount * sizeof(REGISTEREDITEM_MARKET) + sizeof(COMMONHEAD);
	PVOID p = new BYTE[NewSize];

	//head
	memcpy(p, packet, sizeof(COMMONHEAD));

	PREGISTEREDITEM_MARKET pNew = PREGISTEREDITEM_MARKET(DWORD(p) + sizeof(COMMONHEAD));
	POLD_REGISTEREDITEM_MARKET pOld = POLD_REGISTEREDITEM_MARKET(DWORD(packet) + sizeof(COMMONHEAD));

	//body
	for (DWORD i = 0; i < byCount; i++)
	{
		reNewMarketHead(&pNew[i], &pOld[i], REGISTEREDITEM_MARKET);
		reNewMarketBody(&pNew[i].body, &pOld[i].body);
	}

	SendPacketToPlayer(player, p, NewSize);
	delete[] p;
}


void __stdcall fun_InterestedItems(Pshaiya_player player, PCOMMONHEAD packet, DWORD dwSize)
{
	BYTE byCount = packet->byCount;
	DWORD NewSize = byCount * sizeof(INTERESTED_ITEMS) + sizeof(COMMONHEAD);
	PVOID p = new BYTE[NewSize];

	//head
	memcpy(p, packet, sizeof(COMMONHEAD));

	PINTERESTED_ITEMS pNew = PINTERESTED_ITEMS(DWORD(p) + sizeof(COMMONHEAD));
	POLD_INTERESTED_ITEMS pOld = POLD_INTERESTED_ITEMS(DWORD(packet) + sizeof(COMMONHEAD));

	//body
	for (DWORD i = 0; i < byCount; i++)
	{
		reNewMarketHead(&pNew[i], &pOld[i], INTERESTED_ITEMS);
		reNewMarketBody(&pNew[i].body, &pOld[i].body);
	}

	SendPacketToPlayer(player, p, NewSize);
	delete[] p;
}



void __stdcall fun_completedMoney(Pshaiya_player player, PCOMMONHEAD packet, DWORD dwSize)
{
	BYTE byCount = packet->byCount;
	DWORD NewSize = byCount * sizeof(COMPLETED_MONEY_MARKET) + sizeof(COMMONHEAD);
	PVOID p = new BYTE[NewSize];

	//head
	memcpy(p, packet, sizeof(COMMONHEAD));

	PCOMPLETED_MONEY_MARKET pNew = PCOMPLETED_MONEY_MARKET(DWORD(p) + sizeof(COMMONHEAD));
	POLD_COMPLETED_MONEY_MARKET pOld = POLD_COMPLETED_MONEY_MARKET(DWORD(packet) + sizeof(COMMONHEAD));

	//body
	for (DWORD i = 0; i < byCount; i++)
	{
		reNewMarketHead(&pNew[i], &pOld[i], COMPLETED_MONEY_MARKET);
		reNewMarketBody(&pNew[i].body, &pOld[i].body);
	}

	SendPacketToPlayer(player, p, NewSize);
	delete[] p;
}

void __stdcall fun_compeletedItem(Pshaiya_player player, PCOMMONHEAD packet, DWORD dwSize)
{
	BYTE byCount = packet->byCount;
	DWORD NewSize = byCount * sizeof(COMPELETEDITEM_MARKET) + sizeof(COMMONHEAD);
	PVOID p = new BYTE[NewSize];

	//head
	memcpy(p, packet, sizeof(COMMONHEAD));

	PCOMPELETEDITEM_MARKET pNew = PCOMPELETEDITEM_MARKET(DWORD(p) + sizeof(COMMONHEAD));
	POLD_COMPELETEDITEM_MARKET pOld = POLD_COMPELETEDITEM_MARKET(DWORD(packet) + sizeof(COMMONHEAD));

	//body
	for (DWORD i = 0; i < byCount; i++)
	{
		reNewMarketHead(&pNew[i], &pOld[i], COMPELETEDITEM_MARKET);
		reNewMarketBody(&pNew[i].body, &pOld[i].body);
	}

	SendPacketToPlayer(player, p, NewSize);
	delete[] p;
}


void __stdcall  fun_recivedMarketOnSaleInfo(Pshaiya_player player, RECIVEDONSALE_MARKECT_HEAD* packet, DWORD dwSize)
{
	BYTE byCount = packet->count;
	DWORD NewSize = byCount * sizeof(ONSALEBODY_MARKECT) + sizeof(RECIVEDONSALE_MARKECT_HEAD);
	ONSALEBODY_MARKECT* p = (ONSALEBODY_MARKECT*)new BYTE[NewSize];

	//head
	memcpy(p, packet, sizeof(RECIVEDONSALE_MARKECT_HEAD));

	ONSALEBODY_MARKECT* pNew = PONSALEBODY_MARKECT(DWORD(p) + sizeof(RECIVEDONSALE_MARKECT_HEAD));
	OLD_ONSALEBODY_MARKECT* pOld = POLD_ONSALEBODY_MARKECT(DWORD(packet) + sizeof(RECIVEDONSALE_MARKECT_HEAD));

	//body
	for (DWORD i = 0; i < byCount; i++)
	{
		reNewMarketHead(&pNew[i], &pOld[i], ONSALEBODY_MARKECT);
		reNewMarketBody(&pNew[i].body, &pOld[i].body);
	}

	SendPacketToPlayer(player, p, NewSize);
	delete[] p;
}

void __stdcall  fun_enterZoneNotice_1(Pshaiya_player player, PENTERZONE_NOTICE packet, DWORD packetSize)
{
	ENTERZONE_NOTICE stc;
	_InitPacket(stc);

	//head
	memcpy(&stc, packet, offsetof(ENTERZONE_NOTICE, color));

	//body
	getBasicDyeColor(player->Charid, &stc.color);

	//send
	SendPacketToPlayer(player, &stc, sizeof(stc));
	commonfun_setTransform(player, false);
}

void __stdcall  fun_enterZoneNotice_2(Pshaiya_player player, PENTERZONE_NOTICE packet, DWORD dwEcx, DWORD dwEax, DWORD dwEbp)
{
	ENTERZONE_NOTICE stc;
	_InitPacket(stc);

	//head
	memcpy(&stc, packet, offsetof(ENTERZONE_NOTICE, color));

	//body
	getBasicDyeColor(player->Charid, &stc.color);

	DWORD dwCall = 0x00427470;
	DWORD dwSize = sizeof(stc);
	PVOID pTemp = &stc;

	//becaeuse of size of stack ,call here
	_asm
	{
		push dwSize
		push pTemp
		push dwEbp
		mov eax, dwEax
		mov ecx, dwEcx
		call dwCall
	}
	commonfun_setTransform(player, false);
}


void __stdcall fun_HpsetChange(PHPSETCHANGE packet)
{
	DWORD Charid = packet->charid;
	Pshaiya_player player = (Pshaiya_player)GetPlayerByCharid(Charid);
	if (player)
	{
		packet->hp = player->currentHp;
		packet->mp = player->currentMp;
		packet->sp = player->currentSp;
	}
}


void __stdcall fun_sendHooklist(Pshaiya_player player)
{
	SendPacketToPlayer(player, &g_hookAddr, sizeof(g_hookAddr));
}



void __stdcall fun_EspCheck(DWORD dwAddress)
{
	MyLog("ExceptionAddr:%x\n", dwAddress);
}

DWORD __stdcall  fun_remoteNpc(STORENPC_PURCHASE* packet)
{
	if (packet->cmd != official_packet_const::StoreNpc_purchase) {
		return 0;
	}
	return GetNpcAddr(1, packet->npc_typeid);
}

_inline void fun_SaveAutoStats(Pshaiya_player player)
{
	g_objPy->lock();
	try
	{
#ifdef _DEBUG
		Mylog("fun_SaveAutoStats begin");
#endif
		boost::python::call_method<bool>(g_objPy->m_module->get(), "set_auto_stats", player->Charid,
			player->autoStats.str,
			player->autoStats.dex,
			player->autoStats.rec,
			player->autoStats.luc,
			player->autoStats.wis,
			player->autoStats._int);
	}
	catch (...)
	{
		MyLog("fun_autoStats_set:%s\n", getPythonException().c_str());
	}
#ifdef _DEBUG
	Mylog("fun_SaveAutoStats end");
#endif
	g_objPy->Unlock();

}


void __stdcall  fun_SaveCharInfo_CallBack(Pshaiya_player player)
{
	fun_SaveAutoStats(player);
}







void __stdcall fun_DisableSkill(Pshaiya_player player, DWORD skill)
{
	if (skill&&player) {
		WORD TypeDetail = *PWORD(skill + 0x32);
		if (TypeDetail == 132 && player->isUsedTransformationSkill)
		{
			commonfun_setTransform(player, false);
			ShowStatsFix((PBYTE)player);
		}
	}
}


void __stdcall fun_sendDyeAppreance(PMEETAPPREANCE pold, DWORD dwEdi, DWORD dwEax, DWORD dwEcx)
{
	PMEETAPPREANCE stc = new MEETAPPREANCE;
	ZeroMemory(stc, sizeof(MEETAPPREANCE));
	memcpy(stc, pold, offsetof(MEETAPPREANCE, color));

	Pshaiya_player player = (Pshaiya_player)GetPlayerByCharid(stc->charid);
	PplayerItemcharacterise pItem = player->BagItem[0][stc->slot];
	if (pItem)
		memcpy(&stc->color, &pItem->color, sizeof(DyeColor));

	//发包,原来的栈太小了,不好覆盖,这里发包
	DWORD dwCall = 0x00427470;
	DWORD dwSize = sizeof(MEETAPPREANCE);
	_asm
	{
		push dwSize
		push stc
		push dwEdi
		mov eax, dwEax
		mov ecx, dwEcx
		call dwCall
	}

	delete stc;
}

void __stdcall FixMoveItem(Pshaiya_player player, POLDMOVEITEM pOld, DWORD dwSize)
{

	MOVEITEM stc;
	_InitPacket(stc);

	//复制移动源
	{
		memcpy(&stc.src, &pOld->src, 7);
		memcpy(stc.src.craftname, pOld->src.craftname, 21);
		stc.src.MakeType = 'D';  //因为是未知值，随便整的

		for (DWORD i = 0; i < _countof(stc.src.lapis); i++) {
			stc.src.lapis[i] = pOld->src.lapis[i];
		}

		PplayerItemcharacterise pItem = NULL;
		BYTE bag = stc.src.bag;
		BYTE slot = stc.src.slot;
		if (bag == 100)
			pItem = player->whItem[slot];
		else
			pItem = player->BagItem[bag][slot];

		if (pItem)
			memcpy(&stc.src.color, &pItem->color, sizeof(DyeColor));
	}


	//复制移动目标
	{
		memcpy(&stc.dst, &pOld->dst, 7);
		memcpy(stc.dst.craftname, pOld->dst.craftname, 21);
		stc.dst.MakeType = 'D';

		for (DWORD i = 0; i < _countof(stc.dst.lapis); i++) {
			stc.dst.lapis[i] = pOld->dst.lapis[i];
		}

		PplayerItemcharacterise pItem = NULL;
		BYTE bag = stc.dst.bag;
		BYTE slot = stc.dst.slot;
		if (bag == 100)
			pItem = player->whItem[slot];
		else
			pItem = player->BagItem[bag][slot];

		if (pItem)
			memcpy(&stc.dst.color, &pItem->color, sizeof(DyeColor));
	}

	stc.moneyRemain = player->money;
	SendPacketToPlayer(player, &stc, sizeof(stc));
}



void __stdcall ShowPet(Pshaiya_player player, BYTE bag, BYTE dwSlot)
{
	if (dwSlot > 23)
		return;

	//1.制作新包
	BYTE byPacket[0x10] = { 0x2E, 0x02, 0x00, 0x0E, 0xE0, 0x30, 0xE7, 0x3D, 0xE0, 0x30, 0x25, 0x3E, 0x00, 0x00, 0x00, 0x00 };
	byPacket[2] = bag;  //背包
	byPacket[3] = dwSlot;  //格子

	//1.1 取创建时间
	if (!player)
		return;

	PplayerItemcharacterise pItem = player->BagItem[bag][dwSlot];
	if (!pItem)
		return;

	DWORD dwCreateTime = pItem->createtime;//时间的偏移
	if (!dwCreateTime)
		return;

	*PDWORD(&byPacket[4]) = dwCreateTime;
	*PDWORD(&byPacket[8]) = dwCreateTime;

	DWORD dwSize = 0x10;
	SendPacketToPlayer((DWORD)player, (DWORD)byPacket, dwSize);
}

void __stdcall send_charItems(Pshaiya_player player, PBYTE pPacket, DWORD dwSize)
{
	DWORD itemCount = pPacket[2];
	DWORD dwNewSize = itemCount * sizeof(GETCHARITEM) + sizeof(COMMONHEAD);
	PVOID p = new BYTE[dwNewSize];
	ZeroMemory(p, dwNewSize);
	//1. 复制头
	memcpy(p, pPacket, sizeof(COMMONHEAD));
	PGETCHARITEM pNew = PGETCHARITEM(getBodyOfcommonHeader(p));


	//2.复制主体
	pPacket += 3;
	for (DWORD i = 0; i < itemCount; i++)
	{
		//复制bag slot type typeid
		CopyMemory(&pNew[i].bag, &pPacket[i * 0x22], 0x4);

		//复制持久
		pNew[i].QA = *PWORD(&pPacket[i * 0x22 + 4]);

		//复制count
		pNew[i].item_count = pPacket[i * 0x22 + 12];

		//复制6个石头
		pNew[i].gem1 = pPacket[i * 0x22 + 6];
		pNew[i].gem2 = pPacket[i * 0x22 + 7];
		pNew[i].gem3 = pPacket[i * 0x22 + 8];
		pNew[i].gem4 = pPacket[i * 0x22 + 9];
		pNew[i].gem5 = pPacket[i * 0x22 + 10];
		pNew[i].gem6 = pPacket[i * 0x22 + 11];

		//复制Craftname
		CopyMemory(pNew[i].craftname, &pPacket[i * 0x22 + 13], 20);//复制尾部

		//color
		BYTE bag = pNew[i].bag;
		BYTE slot = pNew[i].slot;
		memcpy(&pNew[i].color, &(player->BagItem[bag][slot])->color, sizeof(DyeColor));
	}
	pPacket -= 3;

	//发包
	SendPacketToPlayer(player, p, dwNewSize);


	//3.发送时装外观,发送染色装备外观
	for (DWORD i = 0; i < itemCount; i++)
	{
		BYTE byBag = pNew[i].bag;
		BYTE bySlot = pNew[i].slot;
		BYTE byType = pNew[i]._type;
		if (IsPet(byType))
		{
			ShowPet(player, byBag, bySlot);
		}
	}
	delete[] p;
}



void __stdcall fun_quislot_send(Pshaiya_player player, PCOMMONHEAD packet, DWORD PacketSize)
{
	DWORD dwSize = packet->byCount * sizeof(QUICKSLOT) + sizeof(COMMONHEAD);
	PVOID p = new BYTE[dwSize];
	ZeroMemory(p, dwSize);

	memcpy(p, packet, sizeof(COMMONHEAD));

	PQUICKSLOT pNew = PQUICKSLOT(getBodyOfcommonHeader(p));
	POLDQUICKSLOT pold = POLDQUICKSLOT(getBodyOfcommonHeader(packet));

	//复制主体的每一部分
	for (DWORD i = 0; i < packet->byCount; i++)
	{
		pNew[i].bag = pold[i].bag;
		pNew[i].number = pold[i].number;
		pNew[i].quickBar = pold[i].quickBar;
		pNew[i].quickSlot = pold[i].quickSlot;
	}
	SendPacketToPlayer(player, p, dwSize);
	delete[] p;
}

void __stdcall fun_quickslot_recv(Pshaiya_player player, PCOMMONHEAD packet)
{
	DWORD dwSize = packet->byCount * sizeof(OLDQUICKSLOT);
	POLDQUICKSLOT pold = new OLDQUICKSLOT[packet->byCount];
	ZeroMemory(pold, dwSize);


	PQUICKSLOT      pnew = PQUICKSLOT(getBodyOfcommonHeader(packet));

	//复制主体的每一部分
	for (DWORD i = 0; i < packet->byCount; i++)
	{
		pold[i].bag = pnew[i].bag;
		pold[i].number = pnew[i].number;
		pold[i].quickBar = pnew[i].quickBar;
		pold[i].quickSlot = pnew[i].quickSlot;
	}

	//把主体覆盖OS client发过来的包
	memcpy(pnew, pold, dwSize);
	delete[] pold;
}


void __stdcall fun_enchantResult(Pshaiya_player player, POLDENCHANT_SEND pOld, DWORD dwSize)
{
	ENCHANT_SEND stc;
	_InitPacket(stc);
	stc.isSuccess = pOld->isSuccess;

	memcpy(&stc, pOld, offsetof(OLDENCHANT_SEND, materials));
	stc.item_bag = pOld->item_bag;
	stc.item_slot = pOld->item_slot;
	memcpy(stc.craftname, pOld->craftname, 21);
	stc.moneyRemain = pOld->moneyRemain;

	SendPacketToPlayer((DWORD)player, (DWORD)&stc, sizeof(stc));
}

void __stdcall fun_sort(Pshaiya_player player, PVOID p)
{
	do
	{
		PCOMMONHEAD packet = (PCOMMONHEAD)p;
		if (packet->byCount > 5 * 24)  //要排序的物品数不可能大于物品的所有数量
			break;

		PPREPARESWITCHITEM pItem = PPREPARESWITCHITEM(DWORD(packet) + 3);
		for (DWORD i = 0; i < packet->byCount; i++)
		{
			if (illageSlot(pItem[i].tagBag, pItem[i].srcSlot))
				return DisConnectPlayer(player);

			SwitchItem(player, pItem[i].tagBag, pItem[i].tagSlot, pItem[i].srcBag, pItem[i].srcSlot);
		}
	} while (0);

}


void __stdcall fun_getEnchant(Pshaiya_player player, PVOID pPacket)
{

	PEnchantInfo_recv p_recv = (PEnchantInfo_recv)pPacket;
	EnchantInfo_send  stc_send;
	InitPacket(&stc_send, sizeof(stc_send));
	for (DWORD i = 0; i < 10; i++)
	{
		stc_send.lapisia_bag[i] = 0xff;
		stc_send.lapisia_slot[i] = 0xff;
		BYTE bag = p_recv->lapisia_bag[i];
		BYTE slot = p_recv->lapisia_slot[i];

		if (illageSlot(bag, slot)) {
			return DisConnectPlayer(player);
		}


		PplayerItemcharacterise bag_ItemAddr = player->BagItem[bag][slot];

		if (!bag_ItemAddr) {
			continue;
		}


		DWORD dwItemAddr = GetItemAddress(bag_ItemAddr->ItemType, bag_ItemAddr->Typeid);
		DWORD Money = GetEnchantMoneyCost(dwItemAddr);
		stc_send.lapisia_bag[i] = bag;
		stc_send.lapisia_slot[i] = slot;
		stc_send.price[i] = Money;
		stc_send.success_rate[i] = 100 * 10000;//暂时不去处理  ,100的机率
	}
	return SendPacketToPlayer((DWORD)player, (DWORD)&stc_send, sizeof(stc_send));

}


void __stdcall fun_InitAutoStats(Pshaiya_player player)
{

	std::string strStats = "";
	g_objPy->lock();
	try
	{

#ifdef _DEBUG
		Mylog("fun_InitAutoStats begin");
#endif
		strStats = boost::python::call_method<std::string>(g_objPy->m_module->get(), "get_auto_stats", player->Charid);
	}
	catch (...)
	{
		MyLog("fun_InitAutoStats:%s\n", getPythonException().c_str());
	}
#ifdef _DEBUG
	Mylog("fun_InitAutoStats end");
#endif
	g_objPy->Unlock();

	int str = 0;
	int dex = 0;
	int rec = 0;
	int luc = 0;
	int wis = 0;
	int _int = 0;

	if (!strStats.empty())
	{
		sscanf(strStats.c_str(), "%d,%d,%d,%d,%d,%d", &str, &dex, &rec, &luc, &wis, &_int);
	}

	player->autoStats.str = str;
	player->autoStats.dex = dex;
	player->autoStats.luc = luc;
	player->autoStats._int = _int;
	player->autoStats.wis = wis;
	player->autoStats.rec = rec;
}


void __stdcall fun_antuStats_send(Pshaiya_player player)
{
	AUTOSTATS_GET stc;
	_InitPacket(stc);
	memcpy(&stc.stats, &player->autoStats, sizeof(PLAYERSTATS));
	SendPacketToPlayer(player, &stc, sizeof(stc));
}


void __stdcall fun_autoStats_set(Pshaiya_player player, PAUTOSTATS_SET packet)
{
	memcpy(&player->autoStats, &packet->stats, sizeof(packet->stats));
	fun_antuStats_send(player);
}





void load_BattleFieldInfo()
{
	char strFile[MAX_PATH] = { 0 };
	GetModuleFileNameA(GetModuleHandle(0), strFile, MAX_PATH);
	PathRemoveFileSpecA(strFile);
	strcat(strFile, "\\data\\BattleFieldMoveInfo_Client.ini");

	DWORD count = GetPrivateProfileIntA("BATTLEFIELD_INFO", "BATTLEFIELD_COUNT", 0, strFile);
	for (DWORD i = 1; i <= count; i++)
	{
		BATTLEFIELDINFO st;
		char section[50] = { 0 };
		char pos1[50] = { 0 };
		char pos2[50] = { 0 };
		sprintf(section, "BATTLEFIELD_%d", i);

		st.country = GetPrivateProfileIntA(section, "country", 0, strFile);
		st.minlevel = GetPrivateProfileIntA(section, "LEVEL_MIN", 0, strFile);
		st.maxlevel = GetPrivateProfileIntA(section, "LEVEL_MAX", 0, strFile);
		BYTE map = GetPrivateProfileIntA(section, "MAP_NO", 0, strFile);


		GetPrivateProfileStringA(section, "pos1", "", pos1, 50, strFile);
		GetPrivateProfileStringA(section, "pos2", "", pos2, 50, strFile);

		sscanf(pos1, "%f,%f,%f", &st.pos1.x, &st.pos1.y, &st.pos1.z);
		sscanf(pos2, "%f,%f,%f", &st.pos2.x, &st.pos2.y, &st.pos2.z);
		st.IsEnable = true;

		memcpy(&g_BattleFields[map], &st, sizeof(st));

	}
}


void __stdcall fun_moveBattleField(Pshaiya_player player, PBATTLEFIELD packet)
{
	if (!player->dwObjMap) {
		return;
	}


	WORD Map = packet->map;
	if (Map > _countof(g_BattleFields))
	{
		return DisConnectPlayer(player);
	}

	//必须本身不在战场
	{
		BATTLEFIELDINFO st = g_BattleFields[player->Map];
		if (st.IsEnable)
			return;
	}



	BATTLEFIELDINFO st = g_BattleFields[Map];
	if (st.IsEnable&&
		player->Level >= st.minlevel &&
		player->Level <= st.maxlevel &&
		(player->Country == st.country || st.country == 2)
		)
	{
		if (player->Country == 0)
			MovePlayer((DWORD)player, Map, st.pos1.x, st.pos1.z);
		else
			MovePlayer((DWORD)player, Map, st.pos2.x, st.pos2.z);
	}

}




void __stdcall fun_Dying_getSample(Pshaiya_player player, PDYING_SAMPLING packet)
{
	DYING_SAMPLING stc;
	_InitPacket(stc);
	std::string strSampleColor = "";
	g_objPy->lock();
	try
	{
#ifdef _DEBUG
		Mylog("fun_Dying_getSample begin");
#endif
		strSampleColor = boost::python::call_method<std::string>(g_objPy->m_module->get(), "get_random_dye");
	}
	catch (...)
	{
		MyLog("fun_Dying_getSample:%s\n", getPythonException().c_str());
	}
#ifdef _DEBUG
	Mylog("fun_Dying_getSample end");
#endif
	g_objPy->Unlock();


	if (!strSampleColor.empty())
	{
		DWORD temp[5] = { 0 };
		sscanf(strSampleColor.c_str(), "%ld,%ld,%ld,%ld,%ld", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4]);
		for (DWORD i = 0; i < 5; i++)
		{
			stc.Ay_color[i].Hue.bEnable = true;
			stc.Ay_color[i].Hue.value = temp[i];
		}
	}

	memcpy(player->dye_color, stc.Ay_color, sizeof(stc.Ay_color));
	SendPacketToPlayer(player, &stc, sizeof(stc));

}



void save_dyecolor(Pshaiya_player player, PplayerItemcharacterise pitem)
{
	g_objPy->lock();
	try
	{
#ifdef _DEBUG
		Mylog("save_dyecolor begin");
#endif
		boost::python::call_method<void>(g_objPy->m_module->get(), "save_dyecolor",
			pitem->itemuid,
			pitem->color.Hue.value,
			pitem->color.saturation.value,
			pitem->color.lightness.value);
	}
	catch (...)
	{
		MyLog("save_dyecolor:%s\n", getPythonException().c_str());
	}
#ifdef _DEBUG
	Mylog("save_dyecolor end");
#endif
	g_objPy->Unlock();

}

void __stdcall  fun_Dying_Excute(Pshaiya_player player, PDYING_EXCUTE packet)
{
	PplayerItemcharacterise pItem = NULL;
	//1.check item exsited
	{
		BYTE bag = packet->item_bag;
		BYTE slot = packet->item_slot;
		if (!player->BagItem[bag][slot])
			return;
		pItem = player->BagItem[bag][slot];
	}

	//2.delete material
	{
		BYTE bag = packet->materialbag;
		BYTE slot = packet->materialslot;
		if (!player->BagItem[bag][slot])
			return;
		DeleteItem(player, bag, slot);
	}

	//3
	DYING_EXCUTE_RESULT stc;
	DWORD i = GetRand(5);
	memcpy(&stc.color, &player->dye_color[i], sizeof(stc.color));   //复制到包
	memcpy(&pItem->color, &player->dye_color[i], sizeof(stc.color));//复制到物品处

	//4.save to database
	save_dyecolor(player, pItem);  //更新到数据库

	SendPacketToPlayer(player, &stc, sizeof(stc));
}

//不管是什么物品，都返回样本颜色
void __stdcall  fun_Dying_prepareItem(Pshaiya_player player, PDYING_PREPAREITEM packet)
{
	player->IspapredDye = true;
	DYING_PREPAREITEM stc;
	SendPacketToPlayer(player, &stc, sizeof(stc));
}

void __stdcall fun_move_town(Pshaiya_player player, PMOVETOWN packet)
{
	do
	{
		BYTE byBag = packet->item_bag;
		BYTE bySlot = packet->item_slot;
		PplayerItemcharacterise pItem = player->BagItem[byBag][bySlot];
		if (!pItem)
			break;


		BYTE dwType = pItem->ItemType;
		BYTE dwTypeid = pItem->Typeid;

		PITEMINFO pMemItem = (PITEMINFO)GetItemAddress(dwType, dwTypeid);
		if (!pMemItem)
			break;

		BYTE country = pMemItem->Country;
		//阵营要匹配
		if (
			(country == 2 && player->Country == 0) ||
			(country == 5 && player->Country == 1) ||
			country == 6
			)
		{
			if (pMemItem->Special != 104)
				break;

			//Move Player
			DWORD dwNpcType = 2;
			DWORD dwNpcId = pMemItem->ReqVg;
			DWORD dwNpc = GetNpcAddr(dwNpcType, dwNpcId);

			if (!dwNpc)
				break;

			//Delete item
			DeleteItem(player, byBag, bySlot);
			DWORD dwMap = 0;
			float fX, fY = 0;
			GetPosfromKeeper(dwNpc, packet->translate_Index, &dwMap, &fX, &fY);
			MovePlayer((DWORD)player, dwMap, fX, fY);

		}

	} while (0);
}


void __stdcall  fun_chaoticSquare_Getmaterials(Pshaiya_player player, CHAOTICSQUARE_getmaterials_RECV* packet)
{

	BYTE _type = packet->_type;
	BYTE _typeid = packet->_typeid;
	CHAOTICSQUARE_getmaterials_SEND stc;
	_InitPacket(stc);

	for (DWORD j = 0; j < g_vecMaterials.size(); j++)
	{
		if (g_vecMaterials[j].item_type == _type&&g_vecMaterials[j].item_typeid == _typeid)
		{
			memcpy(&stc.materials, &g_vecMaterials[j], sizeof(CHAOTICSQUARE_MATERIALS));
			return SendPacketToPlayer(player, &stc, sizeof(stc));
		}
	}
	DisConnectPlayer(player);
}



void __stdcall  fun_chaoticSquare_Excute(Pshaiya_player player, CHAOTICSQUARE_Excute_RECV* packet)
{

	BYTE bag = packet->bag;
	BYTE slot = packet->slot;

	if (illageSlot(bag, slot)) {
		return DisConnectPlayer(player);
	}

	//10 syn items max
	if (packet->Index > 9) {
		return DisConnectPlayer(player);
	}


	//check special field
	PplayerItemcharacterise pItem = player->BagItem[bag][slot];
	if (!pItem || pItem->ItemAddr->Special != 99) {
		return DisConnectPlayer(player);
	}



	for (DWORD i = 0; i < g_vecsynthesizeItems.size(); i++)
	{
		//find synthesizeItem
		CHAOTICSQUARE_synthesizeItem synItem = g_vecsynthesizeItems[i];
		if (pItem->Typeid == synItem.synthesize_typeid&&pItem->ItemType == synItem.synthesize_Itemtype)
		{
			BYTE synType = synItem._type[packet->Index];
			BYTE synTypeid = synItem._typeid[packet->Index];
			if (!synType || !synTypeid)
				break;

			//find Materials
			for (DWORD j = 0; j < g_vecMaterials.size(); j++)
			{
				CHAOTICSQUARE_MATERIALS stcMaterial = g_vecMaterials[j];
				if (stcMaterial.item_type == synType&&
					stcMaterial.item_typeid == synTypeid)
				{

					//delete Materials
					for (DWORD k = 0; k < 24; k++)
					{
						BYTE _type = stcMaterial.material_type[k];
						BYTE _typeid = stcMaterial.material_typeid[k];
						BYTE _count = stcMaterial.meterial_count[k];

						//no more  Materials delete needed
						if (!_type || !_typeid)
							break;

						//norma packet will never failed
						if (!DeleteItemByItemID(player, _type, _typeid, _count))
							return;
					}

					sendItem(player, MakeItemId(synType, synTypeid));
					CHAOTICSQUARE_Excute_SEND stc;
					_InitPacket(stc);
					stc.successed_result = 1;  //always successed
					return SendPacketToPlayer(player, &stc, sizeof(stc));
				}
			}
		}
	}
}



void __stdcall  fun_chaoticSquare_getitems(Pshaiya_player player, CHAOTICSQUARE_getitems_RECV* packet)
{

	BYTE byBag = packet->bag;
	BYTE bySlot = packet->slot;

	if (illageSlot(byBag, bySlot))
		return DisConnectPlayer(player);

	PplayerItemcharacterise pItem = player->BagItem[byBag][bySlot];

	if ((pItem->ItemAddr)->Special != 99)
		return DisConnectPlayer(player);

	BYTE _type = pItem->ItemType;
	BYTE _typeid = pItem->Typeid;

	CHAOTICSQUARE_getitems_SEND stc;
	_InitPacket(stc);
	for (DWORD i = 0; i < g_vecsynthesizeItems.size(); i++)
	{
		//find g_vecsynthesizeItem
		if (_type == g_vecsynthesizeItems[i].synthesize_Itemtype&&_typeid == g_vecsynthesizeItems[i].synthesize_typeid)
		{
			memcpy(stc._type, g_vecsynthesizeItems[i]._type, 20);
			return SendPacketToPlayer(player, &stc, sizeof(stc));
		}
	}
}
void __stdcall  fun_use_TownScroll(Pshaiya_player player, PUSETOWNSCROOL packet)
{
	BOOL bAble = FALSE;
	do
	{
		if (packet->Townid != 1)
			break;

		BYTE byBag = packet->bag;
		BYTE bySlot = packet->slot;

		if (illageSlot(byBag, bySlot))
			return DisConnectPlayer(player);

		PplayerItemcharacterise pItem = player->BagItem[byBag][bySlot];

		if (pItem->ItemType != 102 && pItem->Typeid != 92)
			return DisConnectPlayer(player);


		std::string strResult = "";
		g_objPy->lock();
		try
		{
#ifdef _DEBUG
			Mylog("fun_use_TownScroll begin");
#endif
			strResult = boost::python::call_method<std::string>(g_objPy->m_module->get(), "GeneralMoveTowns_moveable", packet->scrollID, player->Country);
		}
		catch (...)
		{
			MyLog("fun_use_TownScroll:%s\n", getPythonException().c_str());
		}
#ifdef _DEBUG
		Mylog("fun_use_TownScroll end");
#endif
		g_objPy->Unlock();


		if (strResult.empty())
			break;


		DWORD map = 0;
		float x, y, z;

		sscanf(strResult.c_str(), "%d,%d,%f,%f,%f", &bAble, &map, &x, &y, &z);

		if (!bAble)
			return DisConnectPlayer(player);

		DeleteItem(player, byBag, bySlot);
		MovePlayer((DWORD)player, map, x, z);

	} while (0);
}


void __stdcall  fun_confrim_charname(Pshaiya_player player, PBYTE packet)
{
	BYTE byPacket[] = { 0x19, 0x1, 0x1 };
	packet = packet + 2;      //挪动2字节准备取角色名

	if (strlen((char*)packet) > 19)
		return DisConnectPlayer(player);
	g_objPy->lock();
	try
	{
#ifdef _DEBUG
		Mylog("fun_confrim_charname begin");
#endif
		byPacket[2] = boost::python::call_method<int>(g_objPy->m_module->get(), "AvailiableCharName", (const char*)packet);
	}
	catch (...)
	{
		MyLog("fun_confrim_charname:%s\n", getPythonException().c_str());

	}
#ifdef _DEBUG
	Mylog("fun_confrim_charname ed");
#endif
	g_objPy->Unlock();
	SendPacketToPlayer(player, byPacket, sizeof(byPacket));
}

void __stdcall illagePacketProcess(Pshaiya_player player, PWORD packet)
{

	switch (*packet)
	{
	case official_packet_const::Item_Sort:
		fun_sort(player, packet);
		break;

	case official_packet_const::getEnchant:
		fun_getEnchant(player, packet);
		break;

	case official_packet_const::Mall_Box:
		break;

	case official_packet_const::Battle_Field:
		fun_moveBattleField(player, (PBATTLEFIELD)packet);
		break;

	case official_packet_const::AutoStats_get:
		fun_antuStats_send(player);
		break;


	case official_packet_const::AutoStats_set:
		fun_autoStats_set(player, (PAUTOSTATS_SET)packet);
		break;

	case official_packet_const::Close_Board:
		break;

	case official_packet_const::Dying_prepareItem:
		fun_Dying_prepareItem(player, PDYING_PREPAREITEM(packet));
		break;

	case official_packet_const::Dying_getSample:
		fun_Dying_getSample(player, PDYING_SAMPLING(packet));
		break;

	case official_packet_const::Dying_Excute:
		fun_Dying_Excute(player, PDYING_EXCUTE(packet));
		break;

	case official_packet_const::Move_Town:
		fun_move_town(player, (PMOVETOWN)packet);
		break;
	case official_packet_const::confrim_charname:
		fun_confrim_charname(player, (PBYTE)packet);
		break;
	case official_packet_const::use_TownScroll:
		fun_use_TownScroll(player, PUSETOWNSCROOL(packet));
		break;

	case official_packet_const::chaoticSquare_getItems:
		fun_chaoticSquare_getitems(player, PCHAOTICSQUARE_getitems_RECV(packet));
		break;

	case official_packet_const::chaoticSquare_Excute:
		fun_chaoticSquare_Excute(player, PCHAOTICSQUARE_Excute_RECV(packet));
		break;

	case official_packet_const::chaoticSquare_Getmaterials:
		fun_chaoticSquare_Getmaterials(player, PCHAOTICSQUARE_getmaterials_RECV(packet));
		break;
	case 0x102:
	case 0x104:
	case 0x234:
	case 0x10b://退出游戏时有的
	case 0x533:
	case 0x539:
	case 0x834:
	case 0x833:
	case 0xa303:
		break;

	default:
	{
		SYSTEMTIME stc;
		GetLocalTime(&stc);
		MyLog("%d-%d %02d:%02d:%02d,illage packet:%x\n", stc.wMonth, stc.wDay, stc.wHour, stc.wMinute, stc.wSecond, *packet);
	//	DisConnectPlayer(player);
		break;
	}
	}
}


void __stdcall fun_getdyecolor(PplayerItemcharacterise pItemcharacterise)
{
	std::string strResult = "";

	g_objPy->lock();
	try
	{
#ifdef _DEBUG
		Mylog("fun_getdyecolor begin");
#endif
		strResult = boost::python::call_method<std::string>(g_objPy->m_module->get(), "get_dyecolor", (UINT64)pItemcharacterise->itemuid);
	}
	catch (...)
	{
		MyLog("fun_getdyecolor:%s\n", getPythonException().c_str());
	}
#ifdef _DEBUG
	Mylog("fun_getdyecolor end");
#endif
	g_objPy->Unlock();

	ZeroMemory(&pItemcharacterise->color, sizeof(DyeColor));

	DWORD color_1 = 0;
	DWORD color_2 = 0;
	DWORD color_3 = 0;
	if (!strResult.empty())
		sscanf(strResult.c_str(), "%ld,%ld,%ld", &color_1, &color_2, &color_3);

	if (color_1) {
		pItemcharacterise->color.Hue.bEnable = true;
		pItemcharacterise->color.Hue.value = color_1;
	}

	if (color_2) {
		pItemcharacterise->color.lightness.bEnable = true;
		pItemcharacterise->color.lightness.value = color_2;
	}

	if (color_3) {
		pItemcharacterise->color.saturation.bEnable = true;
		pItemcharacterise->color.saturation.value = color_3;
	}

}


_inline void fun_sendWeaponEnchant_addvalue(Pshaiya_player player) {
	SendPacketToPlayer(player, &g_Enchant_Addvalue, sizeof(g_Enchant_Addvalue));
}


void fun_sendClock(Pshaiya_player player) {

	cmd_clock st;
	GetLocalTime((PSYSTEMTIME)&st.stcTime);
	SendPacketToPlayer(player, &st, sizeof(st));
}

void __stdcall fun_readChar_CallBack(Pshaiya_player player)
{
	fun_InitAutoStats(player);
	fun_sendWeaponEnchant_addvalue(player);
	fun_sendHooklist(player);
	fun_sendClock(player);
}


bool load_npcquest()
{
	bool bRet = true;
	std::string strResult = "";
	g_objPy->lock();
	try
	{
#ifdef _DEBUG
		Mylog("load_npcquest");
#endif
		strResult = boost::python::call_method<std::string>(g_objPy->m_module->get(), "getQuestRewardExtend");
	}
	catch (...)
	{
		bRet = false;
		MyLog("getQuestRewardExtend:%s\n", getPythonException().c_str());
	}
	g_objPy->Unlock();

	if (strResult.empty())
		return false;


	char* str = new char[strResult.size() + 1];
	strcpy(str, strResult.c_str());

	DWORD i = 0;
	while (1) {
		DWORD reward_itemid4, reward_itemid5, reward_itemid6;
		sscanf(str, "%ld,%ld,%ld", &reward_itemid4, &reward_itemid5, &reward_itemid6);

		g_pQuestRewardExtend[i]._type_4 = reward_itemid4 / 1000;
		g_pQuestRewardExtend[i]._type_5 = reward_itemid5 / 1000;
		g_pQuestRewardExtend[i]._type_6 = reward_itemid6 / 1000;

		g_pQuestRewardExtend[i]._typeid_4 = reward_itemid4 % 1000;
		g_pQuestRewardExtend[i]._typeid_5 = reward_itemid5 % 1000;
		g_pQuestRewardExtend[i]._typeid_6 = reward_itemid6 % 1000;

		i++;

		str = strstr(str, "\r\n");
		if (str == NULL)
			break;
		str += strlen("\r\n");

	}

	return bRet;
}


bool load_setItem()
{
	char strFile[MAX_PATH] = { 0 };
	GetModuleFileNameA(GetModuleHandle(0), strFile, MAX_PATH);
	PathRemoveFileSpecA(strFile);
	strcat(strFile, "\\data\\Dbsetitem.csv");


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


			//如果这块没属性，就采用上一块的
			if (i)
			{
				if (memcmp(&pSetitem->dwValue[i], &temp, sizeof(SETITEMSTATS)) == 0)
				{
					memcpy(&pSetitem->dwValue[i], &pSetitem->dwValue[i - 1], sizeof(SETITEMSTATS));
				}
			}

		}

		DWORD dwType = pSetitem->dwItems[0] / 1000;

#ifdef _DEBUG
		Mylog(szLine);
#endif
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


void loadEnchant_addValue() {
	WCHAR szPath[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(0), szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);
	wcscat(szPath, L"\\data\\ItemEnchant.ini");


	for (WORD i = 0; i < 21; i++) {

		WCHAR szKey[MAX_PATH] = { 0 };
		swprintf_s(szKey, L"WeaponStep%02d", i);
		g_Enchant_Addvalue.Add_value[i] = GetPrivateProfileInt(L"LapisianEnchantAddValue", szKey, 0, szPath);
	}
}


void loadChaotic()
{
	WCHAR szPath[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(0), szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);
	wcscat(szPath, L"\\data\\chaotic.ini");

	//scroll
	for (DWORD i = 0; i < 100; i++)
	{
		WCHAR szSection[MAX_PATH] = { 0 };
		swprintf_s(szSection, MAX_PATH, L"scroll_%d", i + 1);
		DWORD itemid = GetPrivateProfileInt(szSection, L"itemid", 0, szPath);
		if (!itemid)
			break;
		CHAOTICSQUARE_synthesizeItem stc;
		ZeroMemory(&stc, sizeof(stc));
		stc.synthesize_Itemtype = itemid / 1000;
		stc.synthesize_typeid = itemid % 1000;

		for (DWORD j = 0; j < 10; j++)
		{
			WCHAR key[MAX_PATH] = { 0 };
			swprintf_s(key, MAX_PATH, L"synthesize_itemid_%d", j + 1);
			DWORD itemid = GetPrivateProfileInt(szSection, key, 0, szPath);
			if (!itemid)
				break;
			stc._type[j] = itemid / 1000;
			stc._typeid[j] = itemid % 1000;
		}
		g_vecsynthesizeItems.push_back(stc);
	}

	//material
	for (DWORD i = 0; i < 200; i++)
	{
		WCHAR szSection[MAX_PATH] = { 0 };
		WCHAR szMeterrials[1024] = { 0 };
		swprintf_s(szSection, MAX_PATH, L"material_%d", i + 1);
		DWORD itemid = GetPrivateProfileInt(szSection, L"synthesize_itemid", 0, szPath);
		if (!itemid)
			break;

		GetPrivateProfileString(szSection, L"successRate", L"", szMeterrials, 1024, szPath);

		CHAOTICSQUARE_MATERIALS stc;
		ZeroMemory(&stc, sizeof(stc));
		stc.item_type = itemid / 1000;
		stc.item_typeid = itemid % 1000;
		stc.item_count = 1;

		swscanf(szMeterrials, L"%f", &stc.successRate);
		stc.successRate *= 100;

		ZeroMemory(szMeterrials, sizeof(szMeterrials));
		GetPrivateProfileString(szSection, L"meterrials", L"", szMeterrials, 1024, szPath);
		DWORD mat_ID[24] = { 0 };
		DWORD mat_count[24] = { 0 };
		swscanf(szMeterrials, L"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\
			%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d,\
			%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d,\
			%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
			&mat_ID[0], &mat_count[0], &mat_ID[1], &mat_count[1], &mat_ID[2], &mat_count[2], &mat_ID[3], &mat_count[3],
			&mat_ID[4], &mat_count[4], &mat_ID[5], &mat_count[5], &mat_ID[6], &mat_count[6], &mat_ID[7], &mat_count[7],
			&mat_ID[8], &mat_count[8], &mat_ID[9], &mat_count[9], &mat_ID[10], &mat_count[10], &mat_ID[11], &mat_count[11],
			&mat_ID[12], &mat_count[12], &mat_ID[13], &mat_count[13], &mat_ID[14], &mat_count[14], &mat_ID[15], &mat_count[15],
			&mat_ID[16], &mat_count[16], &mat_ID[17], &mat_count[17], &mat_ID[18], &mat_count[18], &mat_ID[19], &mat_count[19],
			&mat_ID[20], &mat_count[20], &mat_ID[21], &mat_count[21], &mat_ID[22], &mat_count[22], &mat_ID[23], &mat_count[23]);

		for (DWORD j = 0; j < 24; j++)
		{
			stc.material_type[j] = mat_ID[j] / 1000;
			stc.material_typeid[j] = mat_ID[j] % 1000;
			stc.meterial_count[j] = mat_count[j];
		}
		g_vecMaterials.push_back(stc);
	}
}



void __stdcall fun_inspect(Pshaiya_player player, COMMONHEAD* oldpacket, DWORD dwSize)
{
	DWORD newSize = oldpacket->byCount * sizeof(INSPECT) + sizeof(COMMONHEAD);
	PVOID p = new BYTE[newSize];
	ZeroMemory(p, newSize);

	//copy head
	memcpy(p, oldpacket, sizeof(COMMONHEAD));

	//get body
	INSPECT* pNewBody = (INSPECT*)getBodyOfcommonHeader(p);
	OLDINSPECT* pOldBOdy = (OLDINSPECT*)getBodyOfcommonHeader(oldpacket);

	for (DWORD i = 0; i < oldpacket->byCount; i++)
	{
		INSPECT& newStc = pNewBody[i];
		OLDINSPECT& oldStc = pOldBOdy[i];

		memcpy(&newStc, &oldStc, offsetof(INSPECT, gems));
		for (DWORD j = 0; j < _countof(newStc.gems); j++)
		{
			newStc.gems[j] = oldStc.gems[j];
		}
		memcpy(newStc.craftname, oldStc.craftname, sizeof(oldStc.craftname));
	}
	SendPacketToPlayer(player, p, newSize);
	delete[] p;
}
void __stdcall Recived_Money(Pshaiya_player player, OLD_RECIVEDTRADEITEM* pPacket, DWORD dwSize)
{
	do
	{
		if (dwSize != 0x10) {
			break;
		}

		//这个数值好像都是固定的一个特征,这2个地方都是为0一般
		if (pPacket->_typeid || pPacket->slot) {
			break;
		}


		RECIVEDTRADEITEM stc;
		_InitPacket(stc);
		memcpy(&stc, pPacket, offsetof(OLD_RECIVEDTRADEITEM, gem));

		//在这个case下 其实是money,这里不再自定义结构体了，知道这个case就好
		*PDWORD(DWORD(&stc) + 0x9) = *PDWORD(DWORD(pPacket) + 0x8);
		return SendPacketToPlayer(player, &stc, sizeof(stc));
	} while (0);

	SendPacketToPlayer(player, pPacket, dwSize);
}


void __stdcall fun_RecivedTradeItem(Pshaiya_player player, OLD_RECIVEDTRADEITEM* pPacket, DWORD dwSize, PplayerItemcharacterise pItem)
{
	//如果大小是10则是处理游戏币的
	if (dwSize == 0x10) {
		return Recived_Money(player, pPacket, dwSize);
	}

	RECIVEDTRADEITEM stc;
	_InitPacket(stc);
	memcpy(&stc, pPacket, offsetof(OLD_RECIVEDTRADEITEM, gem));

	for (DWORD i = 0; i < _countof(pPacket->gem); i++)
	{
		stc.gem[i] = pPacket->gem[i];
	}

	//do not know createtime and expiredTime,skip them
	memcpy(stc.craftname, pPacket->craftname, sizeof(pPacket->craftname));
	memcpy(&stc.color, &pItem->color, sizeof(stc.color));
	SendPacketToPlayer(player, &stc, sizeof(stc));
}


//split 1 packet to 2 parts
void __stdcall fun_shopView(Pshaiya_player player, PCOMMONHEAD oldpacket, DWORD dwSize, PLAYERSHOP* pShop)
{
	Pshaiya_player pshopOwner = pShop->player;
	OLD_SHOPITEM* pOldBOdy = (OLD_SHOPITEM*)getBodyOfcommonHeader(oldpacket);

	DWORD c1 = oldpacket->byCount / 2;
	DWORD c2 = oldpacket->byCount - c1;

	if (c1) 
	{
		SPLITSHOP packet;
		_InitPacket(packet);
		memcpy(&packet.head, oldpacket, sizeof(COMMONHEAD));
		packet.head.byCount = c1;
		packet.part = 1;
		SHOPITEM* pNewBody = packet.body;


		//process body
		for (DWORD i = 0; i < c1; i++, pOldBOdy++) {
			memcpy(&pNewBody[i], pOldBOdy, offsetof(OLD_SHOPITEM, gem));
			for (DWORD j = 0; j < 6; j++) {
				pNewBody[i].gem[j] = pOldBOdy->gem[j];
			}
			memcpy(pNewBody[i].craftname, pOldBOdy[i].craftname, sizeof(pOldBOdy[i].craftname));

			//get info from target players
			BYTE bag = pShop->bag[i];
			BYTE slot = pShop->slot[i];
			PplayerItemcharacterise pItem = pshopOwner->BagItem[bag][slot];
			if (pItem){
				if (IsPet(pItem->ItemType)) {
					pNewBody[i].createTime = pItem->createtime;
				}
				memcpy(&pNewBody[i].color, &pItem->color, sizeof(DyeColor));
			}
		}
		SendPacketToPlayer(player, &packet, sizeof(packet));
	}


	
	if (c2) 
	{	
		SPLITSHOP packet;
		_InitPacket(packet);
		memcpy(&packet.head, oldpacket, sizeof(COMMONHEAD));
		packet.head.byCount = c2;
		packet.part = 2;
		SHOPITEM* pNewBody = packet.body;

		//process body
		for (DWORD i = 0; i < c2; i++, pOldBOdy++) {
			memcpy(&pNewBody[i], pOldBOdy, offsetof(OLD_SHOPITEM, gem));
			for (DWORD j = 0; j < 6; j++) {
				pNewBody[i].gem[j] = pOldBOdy->gem[j];
			}
			memcpy(pNewBody[i].craftname, pOldBOdy[i].craftname, sizeof(pOldBOdy[i].craftname));

			//get info from target players
			BYTE bag = pShop->bag[i];
			BYTE slot = pShop->slot[i];
			PplayerItemcharacterise pItem = pshopOwner->BagItem[bag][slot];
			if (pItem){
				if (IsPet(pItem->ItemType)) {
					pNewBody[i].createTime = pItem->createtime;
				}
				memcpy(&pNewBody[i].color, &pItem->color, sizeof(DyeColor));
			}
		}
		SendPacketToPlayer(player, &packet, sizeof(packet));
	}
}








void __stdcall fun_dragTradeItem(Pshaiya_player player, OLD_DRAGTRADEITEM* pPacket, DWORD dwSize, PplayerItemcharacterise pItem)
{
	DRAGTRADEITEM stc;
	_InitPacket(stc);
	memcpy(&stc, pPacket, offsetof(OLD_DRAGTRADEITEM, gem));

	for (DWORD i = 0; i < _countof(pPacket->gem); i++)
	{
		stc.gem[i] = pPacket->gem[i];
	}

	stc.createTime = pItem->createtime;
	stc.expiredTime = pItem->createtime;
	memcpy(stc.craftname, pPacket->craftname, sizeof(pPacket->craftname));
	memcpy(&stc.color, &(pItem->color), sizeof(stc.color));

	SendPacketToPlayer(player, &stc, sizeof(stc));
}

void __stdcall fun_QuestReward(Pshaiya_player player, DWORD QuestId, DWORD Index)
{
	EnterCriticalSection(&g_csQuest);
	if (Index > 2) {
		g_rewardQuestId = QuestId;
		Index -= 3;
		g_rewardIndex = Index;
		g_rewardPlayer = player;
	}
	DWORD dwCall = 0x0048DBB0;

	_asm {
		mov ecx, Index
		mov eax, QuestId
		mov edx, player
		call dwCall
	}

	g_rewardQuestId = 0;
	g_rewardIndex = 0;
	g_rewardPlayer = NULL;
	LeaveCriticalSection(&g_csQuest);

}



void AddsetItemstats(Pshaiya_player player, PSETITEMDATA pSetItem, DWORD index)
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
	bool isHpChanged = false;
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
		isHpChanged = true;
	}

	ZeroMemory(&player->set_item.BackUpValue, sizeof(player->set_item.BackUpValue));


	//1. 提取身上所有ID
	DWORD dwItem[16] = { 0 };
	for (DWORD i = 0; i < 16; i++) {
		PplayerItemcharacterise pItem = player->BagItem[0][i];
		if (pItem) {
			dwItem[i] = pItem->ItemType * 1000 + pItem->Typeid;
		}
	}

	//2. 分类所需要的ID
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

	//防具检测
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

	//首饰检测
	for (DWORD i = 0; i < g_vecSetitemAcc.size(); i++)
	{
		PSETITEMDATA pSetItem = g_vecSetitemAcc[i];
		DWORD index = 0;

		if (pSetItem->dwItems[0] &&
			pSetItem->dwItems[0] == Accessories.dwItems[0]) //项链
			index++;

		if (pSetItem->dwItems[1] &&
			pSetItem->dwItems[1] == Accessories.dwItems[1])//戒指
			index++;

		if (pSetItem->dwItems[1] &&
			pSetItem->dwItems[1] == Accessories.dwItems[2])//戒指
			index++;

		if (pSetItem->dwItems[2] &&
			pSetItem->dwItems[2] == Accessories.dwItems[3])//手镯
			index++;

		if (pSetItem->dwItems[2] &&
			pSetItem->dwItems[2] == Accessories.dwItems[4])//手镯
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
	}

	if ((isHpChanged||player->set_item.IsSetted)&&player->dwObjMap)
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



__declspec(naked) void Naked_commonPacketReNew()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x28]  //size       
		mov edx, [esp + 0x24] //packet buff
		MYASMCALL_3(fun_commonPacketReNew, ecx, edx, eax)
		popad
		retn 0x8
	}
}



__declspec(naked) void Naked_FixPPLName()
{
	_asm
	{
		pushad
		mov ebx, dword ptr[esp + 0x24]    //object
		mov edx, dword ptr[esp + 0x28]    //buff
		mov esi, dword ptr[esp + 0x2c]    //size
		MYASMCALL_5(fun_Meet_1, ecx, eax, ebx, edx, esi)
		popad
		retn 0xc
	}
}



__declspec(naked) void Naked_gmMoveSpeed()
{
	_asm
	{
		cmp byte ptr[eax + 0x5808], 0x1
		jne __orginal
		mov eax, 0xa
		retn
		__orginal :
		mov eax, dword ptr[eax + 0x12f8]
			jmp g_objgmMoveSpeed.m_pRet

	}
}
__declspec(naked) void Naked_autoStats()
{
	_asm
	{
		pushad
		MYASMCALL_2(illagePacketProcess, ebx, edi)
		popad
		jmp g_objAutoStats.m_pRet
	}
}
__declspec(naked) void Naked_sort()
{
	_asm
	{
		pushad
		MYASMCALL_2(illagePacketProcess, ebp, edi)
		popad
		jmp g_objSort.m_pRet
	}
}

__declspec(naked) void Naked_quickslot()
{
	_asm
	{
		pushad
		MYASMCALL_2(fun_quickslot_recv, ebx, edi)
		popad
		mov dword ptr[ebx + 0xEC0], esi
		jmp objQuickslot.m_pRet

	}
}


__declspec(naked) void Naked_market_readItem()
{
	_asm
	{
		pushad
		MYASMCALL_1(fun_getdyecolor, edi)
		popad
		mov edi, eax
		imul edi, edi, 0x3e8
		jmp g_objMarket_dyecolor.m_pRet

	}
}

__declspec(naked) void Naked_guildReadItem()
{
	_asm
	{
		mov dword ptr ds : [eax + edx * 4 + 0xA4], edi
		pushad
		MYASMCALL_1(fun_getdyecolor, edi)
		popad
		jmp g_objGuild_readItem.m_pRet

	}
}

__declspec(naked) void Naked_userStoredItems()
{
	_asm
	{
		mov eax, dword ptr[esp + 0x14]
		movzx edx, byte ptr[ebx + 0x6]
		pushad
		MYASMCALL_1(fun_getdyecolor, edi)
		popad
		jmp g_objRead_UserstoredItems.m_pRet

	}
}

__declspec(naked) void Naked_readCharItemdetail()
{
	_asm
	{
		pushad
		MYASMCALL_1(fun_getdyecolor, edi)
		popad
		lea eax, dword ptr ds : [eax + eax * 2]
		lea edx, dword ptr ds : [ecx + eax * 8]
		jmp g_objReadCharItemDetail.m_pRet
	}
}




DWORD dwCallEnchantExcute = 0x0046CBA0;
__declspec(naked) void Naked_RemoveEnchantMaterials_1()
{
	_asm
	{
		push eax
		mov al, byte ptr ds : [esi + 0x4]
		mov byte ptr ds : [esi + 0x7], al
		mov al, byte ptr ds : [esi + 0x5]
		mov byte ptr ds : [esi + 0x8], al
		mov al, byte ptr ds : [esi + 0x6]
		mov byte ptr ds : [esi + 0x9], al
		pop eax
		push edi
		mov ecx, esi
		call dwCallEnchantExcute
		jmp g_objRemoveEnchantMaterials_1.m_pRet
	}
}





DWORD dwRemoteNPcAddr = 0x004796e9;
__declspec(naked) void Naked_remoteNpc()
{
	_asm
	{
		cmp eax, 0x0
		jne __orgianl
		MYASM_PUSH_ALl
		MYASMCALL_1(fun_remoteNpc, ebx)
		MYASM_POP_ALL
		test eax, eax
		je __orgianl
		mov dword ptr ss : [esp + 0x58], eax
		jmp dwRemoteNPcAddr
		__orgianl :
		mov dword ptr ss : [esp + 0x58], eax
			jmp g_objRemoteNpc.m_pRet
	}
}


__declspec(naked) void Naked_hpSet()
{
	_asm
	{
		//
		lea edx, [esp + 0x3C]
		push edx
		//

		push eax
		mov eax, [edi + 0x1234]
		mov dword ptr[edx + 0x6], eax
		mov eax, [edi + 0x1238]
		mov dword ptr[edx + 0xa], eax
		mov eax, [edi + 0x123c]
		mov dword ptr[edx + 0xe], eax
		pop eax

		jmp g_objHpset.m_pRet
	}
}


__declspec(naked) void Naked_EspCheck()
{
	_asm
	{
		pushad
		mov eax, dword ptr[esp + 0x20]
		MYASMCALL_1(fun_EspCheck, eax)
		popad

		mov edi, edi
		push ebp
		sub esp, 0x328
		jmp g_objEspCheak.m_pRet

	}
}

__declspec(naked) void Naked_g_LevelupHpset()
{
	_asm
	{
		lea edx, dword ptr ss : [esp + 0x2C]
		push edx
		push eax
		mov eax, [esi + 0x1234]
		mov dword ptr[edx + 0x6], eax
		mov eax, [esi + 0x1238]
		mov dword ptr[edx + 0xa], eax
		mov eax, [esi + 0x123c]
		mov dword ptr[edx + 0xe], eax
		pop eax

		jmp g_objLevelupHpset.m_pRet
	}
}


__declspec(naked) void Naked_SaveCharInfo_CallBack()
{
	_asm
	{
		mov word ptr ss : [esp + 0x93], ax
		pushad
		MYASMCALL_1(fun_SaveCharInfo_CallBack, ebx)
		popad
		jmp g_objSaveCharInfo_callBack.m_pRet
	}
}


__declspec(naked) void Naked_readChar_CallBack()
{
	_asm
	{
		mov dword ptr[ebp + 0x123C], ecx
		pushad
		MYASMCALL_1(fun_readChar_CallBack, ebp)
		popad

		jmp g_objReadCharInfo_callBack.m_pRet
	}
}


__declspec(naked) void Naked_sendHookList()
{
	_asm
	{
		mov byte ptr ss : [ebp + 0x5860], al
		pushad
		MYASMCALL_1(fun_sendHooklist, ebp)
		popad
		jmp g_objSendHookList.m_pRet
	}
}







__declspec(naked) void Naked_npcQuestType()
{
	_asm
	{
		cmp byte ptr[ebx + 0x147], 0x3
		jle __originalcode
		mov byte ptr[ebx + 0x147], 0x1
		__originalcode:
		cmp byte ptr[ebx + 0x00000148], 0x0
			jmp g_objnpcQuestType.m_pRet
	}
}


__declspec(naked) void Naked_rewardExtend()
{
	_asm
	{
		movzx eax, byte ptr[ebp + 0x17c]
		cmp edi, g_rewardPlayer
		jne _orginal
		cmp g_rewardQuestId, 0x0
		je _orginal

		push ebx
		lea eax, g_pQuestRewardExtend
		mov ebx, g_rewardQuestId
		mov ecx, g_rewardIndex
		dec ebx                 //下标从0开始，要减去1
		imul ebx, ebx, 6          //得到结构体首地址偏移
		imul ecx, ecx, 2          //得到结构体成员地址偏移
		add eax, ebx
		add eax, ecx              //基地址+2个偏移
		movzx ecx, byte ptr[eax + 0x1]       //typeid
		movzx eax, byte ptr[eax]           //type
		pop ebx

		_orginal :
		jmp g_objQuestRewardExtend.m_pRet

	}
}
__declspec(naked) void Naked_rewardIndex()
{
	_asm
	{
		movzx ecx, byte ptr ds : [esi + 0x8]
		movzx eax, word ptr ds : [esi + 0x6]
		mov edx, edi
		pushad
		MYASMCALL_3(fun_QuestReward, edx, eax, ecx)
		popad
		jmp g_objQuestRewardIndex.m_pRet

	}
}



__declspec(naked) void Naked_RemoveEnchantMaterials()
{
	_asm
	{
		mov word ptr ss : [esp + 0xA2E], 0x0
		mov byte ptr ss : [esp + 0xA2C], cl
		jmp g_objRemoveEnchantMaterials.m_pRet
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


__declspec(naked) void Naked_NewMountAppreance()
{
	_asm
	{
		push 0x7
		push ecx
		mov byte ptr ss : [esp + 0x46], al  //orgianl code
		pushad
		lea eax, dword ptr[esp + 0x24]  //len
		mov ebx, dword ptr[esp + 0x20]  //buff
		MYASMCALL_3(fun_newmount, edi, ebx, eax);
		popad
			jmp g_objNewMountAppreance.m_pRet
	}
}
__declspec(naked) void Naked_shopView()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x28]  //size       
		mov edx, [esp + 0x24] //packet buff
		MYASMCALL_4(fun_shopView, ecx, edx, eax, edi)
		popad
		retn 0x8
	}
}


__declspec(naked) void Naked_RecivedTradeItem()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x28]  //size       
		mov edx, [esp + 0x24] //packet buff
		MYASMCALL_4(fun_RecivedTradeItem, ecx, edx, eax, ebp)
		popad
		retn 0x8
	}
}

__declspec(naked) void Naked_DragTradeItem()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x28]  //size       
		mov edx, [esp + 0x24] //packet buff
		sub ebx, 0x4c
		MYASMCALL_4(fun_dragTradeItem, ecx, edx, eax, ebx)
		popad
		retn 0x8
	}
}

__declspec(naked) void Naked_Marketunknown_common()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x28]  //size       
		mov edx, [esp + 0x24] //packet buff
		MYASMCALL_3(fun_common_marketbodycorrectionAndSend, ecx, edx, eax)
		popad
		retn 0x8

	}
}

__declspec(naked) void Naked_enterZoneNotice_2()
{
	_asm
	{
		pushad
		mov ebx, dword ptr[esp + 0x24]
		MYASMCALL_5(fun_enterZoneNotice_2, edi, ebx, ecx, eax, ebp)
		popad
		add esp, 0xc
		jmp g_objEnterZoneNotice_2.m_pRet
	}
}



__declspec(naked) void Naked_ConfrimCharName()
{
	_asm
	{
		pushad
		MYASMCALL_2(illagePacketProcess, esi, edi)
		popad
		jmp g_objConfrimCharname.m_pRet
	}
}

__declspec(naked) void Naked_DisableTransform()
{
	_asm
	{
		mov eax, dword ptr[esi + 0xa94]
		pushad
		MYASMCALL_2(fun_DisableSkill, esi, ecx)
		popad
		jmp g_objDisableTransform.m_pRet
	}
}

__declspec(naked) void Naked_usetransform()
{
	_asm
	{
		push eax
		lea ecx, dword ptr[esp + 0x27]
		pushad
		MYASMCALL_2(useSkill, edi, eax)
		popad
		jmp g_objuseTransform.m_pRet
	}
}

__declspec(naked) void Naked_IllageFix1()
{
	_asm
	{
		pushad
		MYASMCALL_2(illagePacketProcess, ecx, ebp)
		popad
		add esp, 0x8
		jmp g_objIllageFix1.m_pRet
	}
}

__declspec(naked) void Naked_sendDyeAppreance()
{
	_asm
	{
		pushad
		mov edi, [esp + 0x20]
		MYASMCALL_4(fun_sendDyeAppreance, edx, edi, eax, ecx)
		popad
		add esp, 0xc                        //原来没调用m弹上去
		jmp g_objsendDyeAppreance.m_pRet

	}
}


__declspec(naked) void Naked_fixEnchant()
{
	_asm
	{
		pushad
		MYASMCALL_2(illagePacketProcess, edi, esi)
		popad
		jmp g_ObjGetenchant.m_pRet

	}
}









DWORD WINAPI ThreadProc1(_In_  LPVOID lpParameter)
{

	InitializeCriticalSection(&g_cs);
	InitializeCriticalSection(&g_csQuest);
	g_objPy = new MyPython("shaiya_py");


	//一些公用的，修复老包，更换成新包的
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::login_toon, (fun_PacketReNew)LoginNameFix));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::GetWh, (fun_PacketReNew)GetWareHouse));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::Send_ItemInfo, (fun_PacketReNew)send_charItems));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::Send_StatsInfo, (fun_PacketReNew)FixHpOnLogin));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::Move_Item, (fun_PacketReNew)FixMoveItem));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::Send_SkillInfo, (fun_PacketReNew)FixShowSkill));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::Enchant_send, (fun_PacketReNew)fun_enchantResult));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::Quick_Slot, (fun_PacketReNew)fun_quislot_send));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::enterzone_notice, (fun_PacketReNew)fun_enterZoneNotice_1));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::recivedOnsaleInfo_market, (fun_PacketReNew)fun_recivedMarketOnSaleInfo));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::compeltedItem_market, (fun_PacketReNew)fun_compeletedItem));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::compeletdMoney_market, (fun_PacketReNew)fun_completedMoney));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::InterestedItems_market, (fun_PacketReNew)fun_InterestedItems));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::ProcessedSell_market, (fun_PacketReNew)fun_SaleRegisteredItem));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::ItemMall_purchase, (fun_PacketReNew)fun_FixItemMall));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::Linkgem_result, (fun_PacketReNew)fun_linkResult));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::extract_lapis, (fun_PacketReNew)fun_extractLapis));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::use_Mount, (fun_PacketReNew)fun_useMount));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::Meetppl, (fun_PacketReNew)FixPPLName));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::Recived_TradeItem, (fun_PacketReNew)Recived_Money));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::View_inspect, (fun_PacketReNew)fun_inspect));
	g_mapCommonPacketFun.insert(std::pair<official_packet_const, fun_PacketReNew>(official_packet_const::get_GH, (fun_PacketReNew)fun_getGh));



#ifdef _DEBUG
	DWORD PacketReNewHookAddr[] = {
		0x0047b9b3,0x00492731,0x0049276b,0x004925e9,0x00492638,
		0x004924d5,0x00468e5f,0x0046992f,0x004693d3,0x00469be7,
		0x0049282b,0x0046cf89,0x0046d286,0x00492ac3,0x0042a504,
		0x00440ff9, 0x004427cb, 0x0044299c, 0x4443e9, 0x004423a1,0x00488704,0x0046ec78,0x0046e90f,0x0047155f,0x00471796,0x00470910,
		0x0042a579,0x0049138f,0x004913b8,0x004913d3,0x00465d27,0x0046a36d,0x0046af4a,0x0046b0df,0x0046b22a,0x0046bc60,0x0048d5a8,
		0x00426c9b,0x004427ab,0x004427ab,0x00477dfd,0x0043508f,0x00435050
	};
#else

	VMProtectBegin("ThreadProc");
	DWORD PacketReNewHookAddr[43] = { 0 };
	std::string strResult = "";
	g_objPy->lock();
	try
	{
		strResult = boost::python::call_method<std::string>(g_objPy->m_module->get(), "getRemoteAddr");
	}
	catch (...)
	{
		_asm {
			jmp eax
		}

	}
	g_objPy->Unlock();
	sscanf_s(strResult.c_str(),
		"%ld,%ld,%ld,%ld,%ld,\
		%ld,%ld,%ld,%ld,%ld,\
		%ld,%ld,%ld,%ld,%ld,\
		%ld,%ld,%ld,%ld,%ld,\
		%ld,%ld,%ld,%ld,%ld,\
		%ld,%ld,%ld,%ld,%ld,\
		%ld,%ld,%ld,%ld,%ld,\
		%ld,%ld,%ld,%ld,%ld,\
		%ld,%ld,%ld",
		&PacketReNewHookAddr[0], &PacketReNewHookAddr[1], &PacketReNewHookAddr[2], &PacketReNewHookAddr[3], &PacketReNewHookAddr[4],
		&PacketReNewHookAddr[5], &PacketReNewHookAddr[6], &PacketReNewHookAddr[7], &PacketReNewHookAddr[8], &PacketReNewHookAddr[9],
		&PacketReNewHookAddr[10], &PacketReNewHookAddr[11], &PacketReNewHookAddr[12], &PacketReNewHookAddr[13], &PacketReNewHookAddr[14],
		&PacketReNewHookAddr[15], &PacketReNewHookAddr[16], &PacketReNewHookAddr[17], &PacketReNewHookAddr[18], &PacketReNewHookAddr[19],
		&PacketReNewHookAddr[20], &PacketReNewHookAddr[21], &PacketReNewHookAddr[22], &PacketReNewHookAddr[23], &PacketReNewHookAddr[24],
		&PacketReNewHookAddr[25], &PacketReNewHookAddr[26], &PacketReNewHookAddr[27], &PacketReNewHookAddr[28], &PacketReNewHookAddr[29],
		&PacketReNewHookAddr[30], &PacketReNewHookAddr[31], &PacketReNewHookAddr[32], &PacketReNewHookAddr[33], &PacketReNewHookAddr[34],
		&PacketReNewHookAddr[35], &PacketReNewHookAddr[36], &PacketReNewHookAddr[37], &PacketReNewHookAddr[38], &PacketReNewHookAddr[39],
		&PacketReNewHookAddr[40], &PacketReNewHookAddr[41], &PacketReNewHookAddr[42]
	);
		VMProtectEnd();
#endif






	CMyInlineHook objPacketReNewHook;
	for (DWORD i = 0; i < _countof(PacketReNewHookAddr); i++) {
		objPacketReNewHook.Hook(PVOID(PacketReNewHookAddr[i]), Naked_commonPacketReNew, 5, fun_commonPacketReNew, 1);
	}


	//发给客户端的
	g_hookAddr.enchantHook = 0x004a8526;
	g_hookAddr.MoveTownHook = 0x006675c8;
	g_hookAddr.MoveTownProcessBarHook = 0x00498a2b;
	g_hookAddr.MobColorHook = 0x0044f0ac;
	g_hookAddr.DrawWord1Hook = 0x004DF8A9;
	g_hookAddr.DrawWord2Hook = 0x00599bf5;
	g_hookAddr.InitWordHook = 0x0042fd19;
	g_hookAddr.Pos1Hook = 0x004DFCAF;
	g_hookAddr.Pos2Hook = 0x004DF803;
	g_hookAddr.NormalColorHook = 0x00480771;
	g_hookAddr.PartyColorHook = 0x004807f6;








	//调整宠物格子
	{
		BYTE MaxSLot = 16;
		//	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00477de5, &MaxSLot, sizeof(MaxSLot)); //原本的栈空间不够,强行加只会栈溢出
	}

	{
		BYTE NoOff = 0x0;
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0058794e, &NoOff, sizeof(NoOff));
	}

	{
		BYTE byWareHouseCount = 10;
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0049271e, &byWareHouseCount, sizeof(byWareHouseCount));
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0043503d, &byWareHouseCount, sizeof(byWareHouseCount));
	}



	//调整玩家的大小
	{
		DWORD temp = sizeof(shaiya_player);
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00411f74, &temp, sizeof(temp));
	}



	g_Hookobj_Meet.Hook((PVOID)0x00491eb9, Naked_FixPPLName, 5, fun_Meet_1, 1);
	g_Hookobj_Meet.Hook((PVOID)0x00491ccd, Naked_FixPPLName, 5, fun_Meet_1, 1);



	g_ObjGetenchant.Hook((PVOID)0x0047a040, Naked_fixEnchant, 11, illagePacketProcess);
	g_objAutoStats.Hook((PVOID)0x00474d6b, Naked_autoStats, 10, illagePacketProcess);
	g_objSort.Hook(PVOID(0x0047798e), Naked_sort, 11, illagePacketProcess);
	g_objgmMoveSpeed.Hook((PVOID)0x0049b320, Naked_gmMoveSpeed, 6);

	//保存玩家格子,客户端发过来的有问题,要去修成我们ps_game能识别的	
	objQuickslot.Hook(PVOID(0x00474c50), Naked_quickslot, 6, fun_quickslot_recv);


	{
		DWORD dwSize = sizeof(playerItemcharacterise);
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004122b6, &dwSize, 4);
	}
	//14.1 readCharItemdetail on login screen
	g_objReadCharItemDetail.Hook(PVOID(0x0047c6ca), Naked_readCharItemdetail, 6, fun_getdyecolor);
	//14.2 Read_User_StoredItems on login screen
	g_objRead_UserstoredItems.Hook(PVOID(0x0047c997), Naked_userStoredItems, 8, fun_getdyecolor);
	//14.3 guild	on login screen
	g_objGuild_readItem.Hook((PVOID)0x004061e8, Naked_guildReadItem, 8, fun_getdyecolor);
	//14.4 marketon login screen
	g_objMarket_dyecolor.Hook(PVOID(0x004064f4), Naked_market_readItem, 8, fun_getdyecolor);

	//发送Dye颜色给别人
	{
		BYTE dwSize = sizeof(MEETAPPREANCE);
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00492cd8, &dwSize, 1);
	}
	g_objsendDyeAppreance.Hook(PVOID(0x00492ceb), Naked_sendDyeAppreance, 5, fun_sendDyeAppreance);
	g_objIllageFix1.Hook((PVOID)0x00479150, Naked_IllageFix1, 5, illagePacketProcess);

	//变身技能
	g_objuseTransform.Hook((PVOID)0x00478c7c, Naked_usetransform, 5, useSkill);
	g_objDisableTransform.Hook(PVOID(0x004960ef), Naked_DisableTransform, 6, fun_DisableSkill);
	//确认角色名
	g_objConfrimCharname.Hook((PVOID)0x0047a569, Naked_ConfrimCharName, 11, illagePacketProcess);
	//过图时显示时装
	g_objEnterZoneNotice_2.Hook(PVOID(0x0041c85e), Naked_enterZoneNotice_2, 5, fun_enterZoneNotice_2);

	//Market common part
	{
		DWORD phookAddr[] = { 0x0046baa6 ,0x00443bb3,0x00443123,0x00486703,0x0048695b,0x00444686,
			0x00442c55,0x0044043f,0x00443261,0x0044334f,0x00440153,0x00440331,0x00442d36,
			0x0044334f,0x00443950,0x0044489c,0x00444d51 };

		for (DWORD i = 0; i < _countof(phookAddr); i++)
		{
			g_objMarketUnknown[i].Hook((PVOID)phookAddr[i], Naked_Marketunknown_common, 5, fun_common_marketbodycorrectionAndSend, 1);
		}
	}

	//23. chaotic square
	loadChaotic();

	//24. Drag TradeItem
	g_objDragTradeItem.Hook(PVOID(0x0047df2f), Naked_DragTradeItem, 5, fun_dragTradeItem, 1);

	//25.Recived TradeItem
	g_objRecivedTradeItem.Hook((PVOID)0x0047ef76, Naked_RecivedTradeItem, 5, fun_RecivedTradeItem, 1);
	g_objRecivedTradeItem.Hook((PVOID)0x0046a864, Naked_RecivedTradeItem, 5, fun_RecivedTradeItem, 1);
	g_objRecivedTradeItem.Hook((PVOID)0x0046b3f4, Naked_RecivedTradeItem, 5, fun_RecivedTradeItem, 1);
	g_objRecivedTradeItem.Hook((PVOID)0x0046c22a, Naked_RecivedTradeItem, 5, fun_RecivedTradeItem, 1);



	//26. shop view
	g_objShopView.Hook(PVOID(0x0048734b), Naked_shopView, 5, fun_shopView, 1);

	//27. Enchant Values
	loadEnchant_addValue();

	//28. new mount appreance fixed
	g_objNewMountAppreance.Hook(PVOID(0x004263b1), Naked_NewMountAppreance, 7, fun_newmount);


	//29. set Items
	load_setItem();
	if (g_vecSetitemAcc.size() || g_vecSetItemArmor.size())
		g_HookObj_Setitem.Hook((PVOID)0x00460e40, Naked_Setitem, 9, fun_setStatsThroughtItems);

	//30. remomve materials of enchanting
	{
		BYTE temp[] = { 0x90,0x90,0x90,0x90,0x90 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046d5b9, temp, sizeof(temp));
	}

	{
		BYTE temp[] = { 0x36 ,0x88 ,0x94 ,0x24 ,0x2C ,0x0A ,0x00 ,0x00 ,0xB0 ,0x00 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046cf03, temp, sizeof(temp));
	}

	{
		BYTE temp[] = { 0x36 ,0xC6 ,0x84 ,0x24 ,0x30 ,0x0A ,0x00 ,0x00 ,0x00 ,0x90 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046cf32, temp, sizeof(temp));
	}
	{
		BYTE temp[] = { 0xeb };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046ce17, temp, sizeof(temp));
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046d5af, temp, sizeof(temp));
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046d17e, temp, sizeof(temp));
	}
	{
		BYTE temp[] = { 0xEB ,0x32 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046cc45, temp, sizeof(temp));
	}

	{
		BYTE temp[] = { 0xEB ,0x0F };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046cbfb, temp, sizeof(temp));
	}
	{
		BYTE temp[] = { 0xEB ,0x10 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046CA29, temp, sizeof(temp));
	}

	{
		BYTE temp[] = { 0xEB ,0x10 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046CA29, temp, sizeof(temp));
	}

	{
		BYTE temp[] = { 0xEB ,0x24 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0046D219, temp, sizeof(temp));
	}

	g_objRemoveEnchantMaterials.Hook(PVOID(0x0046d1f0), Naked_RemoveEnchantMaterials, 15);
	g_objRemoveEnchantMaterials_1.Hook(PVOID(0x00479ff7), Naked_RemoveEnchantMaterials_1, 8);

	//31 . 6 rewards
	if (!load_npcquest())
	{
		MyLog("load_npcquest error\n");
		TerminateProcess(GetCurrentProcess(), 0);
	}
	g_objQuestRewardIndex.Hook(PVOID(0x0047a0d0), Naked_rewardIndex, 14);
	g_objQuestRewardExtend.Hook(PVOID(0x0048df5c), Naked_rewardExtend, 7);
	g_objnpcQuestType.Hook(PVOID(0x0048e316), Naked_npcQuestType, 7);      //主要是一些新手技能任务的这个npctype不一样
	{
		BYTE temp = 'S';
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0048d7bc, &temp, sizeof(temp));
	}

	//32. max bags
	{
		BYTE temp = 0x5;
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00461040, &temp, sizeof(temp));
	}

	//33. 强行让2级驱散变成3级
	{
		BYTE temp[] = { 0x90 ,0x90 ,0x90 ,0x90 ,0x90 ,0x90 ,0x90 ,0x90 ,0x90, 0x90 ,0x90 ,0x90 ,0x90 ,0x90 ,0x90 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00499333, temp, sizeof(temp));
	}

	//34. 拍卖物品的信息列表
	{
		BYTE temp = 0x10;
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00442792, &temp, sizeof(temp));
	}

	//35.远程买物品的NPC
	g_objRemoteNpc.Hook(PVOID(0x004796cb), Naked_remoteNpc, 6);

	//36.在登录时获取5个角色信息那里发送hook列表
//	g_objSendHookList.Hook(PVOID(0x0047b9d8), Naked_sendHookList, 6);

	//37. read_char call Back
	g_objReadCharInfo_callBack.Hook(PVOID(0x0047bff6), Naked_readChar_CallBack, 6);

	//38.save char Call Back
	g_objSaveCharInfo_callBack.Hook(PVOID(0x00455db2), Naked_SaveCharInfo_CallBack, 8);

	//39
	load_BattleFieldInfo();

	//40.hp mp sp 
	g_objLevelupHpset.Hook(PVOID(0x00490e83), Naked_g_LevelupHpset, 5);
	g_objHpset.Hook(PVOID(0x00490fa0), Naked_hpSet, 5);

	//41.右键查看装备有问题,暂时先取消
	{
		// 		BYTE temp[] = { 0x83,0xC4,0x08,0x90 ,0x90 };
		// 		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00477dfd), temp, sizeof(temp));
	}

	//42.range steal
	{
		BYTE temp = 0x1;
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x004942a0), &temp, sizeof(temp));
	}

	//42.栈溢出的时候看一下上一层是什么
	{
		g_objEspCheak.Hook(PVOID(0x00526e35), Naked_EspCheck, 5);
	}


	//43. drop字段会影响掉线,去掉那个影响
	{
		BYTE temp[] = { 0xEB ,0x44 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004d5edb, temp, sizeof(temp));
	}

	//44. 让原来的clock不发送
	{
		BYTE temp[] = { 0x83,0xc4,0x8,0x90,0x90 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004921b3, temp, sizeof(temp));
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0047bce2, temp, sizeof(temp));
	}

	return 0;
}


