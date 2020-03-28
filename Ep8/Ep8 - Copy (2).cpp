// PetSystem.cpp : 定义 DLL 应用程序的导出函数。
//

#include <afxwin.h>
#include <fstream>

#include <MyClass.h>
#include <MyInlineHook.h>
#include <MyPython.h>
#include  <vector>
#include "VMProtectSDK.h"
//#include <common.h>
#include <Shlwapi.h>
#include <stddef.h>
#include <iostream>
#pragma comment(lib,"Shlwapi.lib")
#include "Ep8.h"

#define reNewMarketTail(pdst,psrc,_size) memcpy(PVOID((DWORD)pdst+sizeof(COMMON_ITEMBODY_MARKET)),\
	PVOID((DWORD)psrc+sizeof(OLD_COMMON_ITEMBODY_MARKET)),_size\
	)

#define reNewMarketHead(pNew,pOld,_type) (memcpy(pNew,pOld,offsetof(_type,body)))

#define CHAR_TO_WCHAR(lpChar,lpW_Char) \
MultiByteToWideChar(CP_ACP,0,lpChar,-1,lpW_Char,_countof(lpW_Char));



#define IsPet(byType) (byType==150||byType==120 || byType==121?TRUE:FALSE)
#define DefaultDays (30)


void fun_ProcessOnLogin(Pshaiya_player player);
void __stdcall ShowPet(DWORD , DWORD , DWORD );

std::vector<CHAOTICSQUARE_MATERIALS> g_vecMaterials;
std::vector<CHAOTICSQUARE_synthesizeItem> g_vecsynthesizeItems;


std::vector<PSETITEMDATA> g_vecSetItem;



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


CMyInlineHook g_objEnchantResult_2;

CRITICAL_SECTION g_cs;
MyPython* g_objPy = NULL;
WEAPONENCHANT_ADDVALUE g_Enchant_Addvalue;


void __stdcall fun_antuStats_send(Pshaiya_player player);

__declspec(naked) void Naked_LoginNameFix()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x24] //packet buff
		LoginNameFix(ecx, eax);
		popad
		retn 0x8
	}
}



__declspec(naked) void Naked_FixPPLName()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x28]  //size       
		mov edx, [esp + 0x24] //packet buff
		FixPPLName(ecx, ebx, edx, eax);
		popad
		return 0x8
	}
}


__declspec(naked) void Naked_FixWareHouse()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x28]  //size       
		mov edx, [esp + 0x24] //packet buff
		GetWareHouse(ecx, edx, eax);
			popad
		retn 0x8
	}
}


__declspec(naked) void Naked_GetWareHouse()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x28]  //size       
		mov edx, [esp + 0x24] //packet buff
		GetWareHouse(ecx, edx, eax);
		popad
		retn 0x8
	}
}



__declspec(naked) void Naked_FixTimer1()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x28]  //size       
		mov edx, [esp + 0x24] //packet buff
		send_charItems(ecx, edx, eax);
		popad
		retn 0x8
	}
}


__declspec(naked) void Naked_FixHpOnLogin()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x28]  //size       
		mov edx, [esp + 0x24] //packet buff
		FixHpOnLogin(ecx, edx, eax);
		popad
	retn 0x8

	}
}

__declspec(naked) void Naked_FixShowSkill()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x24]  //size       
		mov edx, [esp + 0x20] //packet buff
		FixShowSkill(ecx, edx, eax);
			popad
			add esp, 0x8
			jmp HookObj_ShowKill.m_pRet
	}
}




__declspec(naked) void Naked_gmMoveSpeed()
{
	_asm
	{	
		cmp byte ptr [eax+0x5808],0x1
			jne __orginal
		mov eax,0xa
			retn
		__orginal :
		mov eax, dword ptr[eax+0x12f8]
			jmp g_objgmMoveSpeed.m_pRet
		
	}
}
__declspec(naked) void Naked_autoStats()
{
	_asm
	{
		pushad
		illagePacketProcess(ebx, edi);

			popad
			jmp g_objAutoStats.m_pRet

	}
}
__declspec(naked) void Naked_sort()
{
	_asm
	{
		pushad
		illagePacketProcess(ebp, edi);

			popad
			jmp g_objSort.m_pRet

	}
}

__declspec(naked) void Naked_quickslot()
{
	_asm
	{
		pushad
		fun_quickslot_recv(ebx, edi);
			popad
			mov dword ptr [ebx + 0xEC0], esi
			jmp objQuickslot.m_pRet

	}
}
__declspec(naked) void Naked_sendquickslot()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x20]//packet buffer
		fun_quislot_send(ecx, eax);
			popad
			add esp,0x8
			jmp g_objSendQuickslot.m_pRet

	}
}



__declspec(naked) void Naked_market_readItem()
{
	_asm
	{
		pushad
		fun_getdyecolor(edi);
		popad
		mov edi,eax
		imul edi,edi,0x3e8
		jmp g_objMarket_dyecolor.m_pRet

	}
}

__declspec(naked) void Naked_guildReadItem()
{
	_asm
	{
		mov dword ptr ds : [eax + edx * 4 + 0xA4], edi
		pushad
		fun_getdyecolor(edi);

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
		fun_getdyecolor(edi);
		popad
		jmp g_objRead_UserstoredItems.m_pRet

	}
}

__declspec(naked) void Naked_readCharItemdetail()
{
	_asm
	{
		pushad
		fun_getdyecolor(edi);
			popad
			lea eax, dword ptr ds : [eax + eax * 2]
			lea edx, dword ptr ds : [ecx + eax * 8]
			jmp g_objReadCharItemDetail.m_pRet
	}
}







__declspec(naked) void Naked_CompletedMoney()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x24]  //size       
		mov edx, [esp + 0x20] //packet buff
		fun_completedMoney(ecx, edx, eax);
		popad
		add esp, 0x8
		jmp g_objMarketCompletedMoney.m_pRet

	}
}


__declspec(naked) void Naked_SaleRegisteredItem()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x24]  //size       
		mov edx, [esp + 0x20] //packet buff
		fun_SaleRegisteredItem(ecx, edx, eax);
		popad
		add esp, 0x8
		jmp g_objMarketSaleRegisteredItem.m_pRet

	}
}


__declspec(naked) void Naked_InterestedItems()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x24]  //size       
		mov edx, [esp + 0x20] //packet buff
		fun_InterestedItems(ecx, edx, eax);
		popad
		add esp, 0x8
		jmp g_objMarketInterestedItems.m_pRet

	}
}


__declspec(naked) void Naked_CompeletedItem()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x24]  //size       
		mov edx, [esp + 0x20] //packet buff
		fun_compeletedItem(ecx, edx, eax);
		popad
		add esp, 0x8
		jmp g_objMarketCompeltedItem.m_pRet

	}
}

__declspec(naked) void Naked_Setitem()
{
	_asm
	{

		pushad
		fun_setStatsThroughtItems(esi);
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
		fun_newmount(edi, ebx, eax);
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
		fun_shopView(ecx,edx,eax,edi);
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
		fun_RecivedTradeItem(ecx, edx, eax, ebp);
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
		fun_dragTradeItem(ecx, edx, eax, ebx-0x4c);
		popad
		retn 0x8
	}
}



__declspec(naked) void Naked_FixItemMall()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x28]  //size       
		mov edx, [esp + 0x24] //packet buff
		fun_FixItemMall(ecx, edx, eax);
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
		fun_common_marketbodycorrectionAndSend(ecx,edx,eax);
		popad
		retn 0x8

	}
}










__declspec(naked) void Naked_RecivedMarketOnSaleInfo()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x28]  //size       
		mov edx, [esp + 0x24] //packet buff
		fun_recivedMarketOnSaleInfo(ecx, edx, eax);
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
		fun_enterZoneNotice_2(edi, ebx, ecx, eax, ebp);
		popad
		add esp,0xc
		jmp g_objEnterZoneNotice_2.m_pRet
	}
}


__declspec(naked) void Naked_enterZoneNotice_1()
{
	_asm
	{
		pushad
		fun_enterZoneNotice_1(ecx, eax);
		popad
		add esp,0x8
		jmp g_objEnterZoneNotice_1.m_pRet
	}
}
__declspec(naked) void Naked_ConfrimCharName()
{
	_asm
	{
		pushad
		illagePacketProcess(esi, edi);
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
		fun_DisableSkill(esi, ecx);
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
		fun_useSkill(edi, eax);
			popad
			jmp g_objuseTransform.m_pRet
	}
}


__declspec(naked) void Naked_IllageFix1()
{
	_asm
	{
		pushad
		illagePacketProcess(ecx, ebp);
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
		fun_sendDyeAppreance(edx, edi, eax, ecx);
			popad
			add esp,0xc                        //原来没调用m弹上去
			jmp g_objsendDyeAppreance.m_pRet

	}
}




__declspec(naked) void Naked_enchantResult()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x28] //packet size
		mov edx, [esp + 0x24]//packet buffer
		fun_enchantResult(ecx, edx, eax);
			popad
			retn 0x8

	}
}

__declspec(naked) void Naked_fixEnchant()
{
	_asm
	{
		pushad
		illagePacketProcess(edi, esi);
			popad
			jmp g_ObjGetenchant.m_pRet
				
	}
}






__declspec(naked) void Naked_MoveWhToWh()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x24] //packet size
		mov edx, [esp + 0x20]//packet buffer
		FixMoveItem(ecx, edx, eax);
			popad
			add esp, 0x8
			jmp g_objMoveWhToWh.m_pRet
	}
}
__declspec(naked) void Naked_MoveWhTobag()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x24] //packet size
		mov edx, [esp + 0x20]//packet buffer
		FixMoveItem(ecx, edx, eax);
			popad
			add esp, 0x8
			jmp g_HookObj_MoveWhTobag.m_pRet
	}
}


__declspec(naked) void Naked_MoveItem()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x24] //packet size
		mov edx, [esp + 0x20]//packet buffer
		FixMoveItem(ecx, edx, eax);
			popad
			add esp, 0x8
			jmp g_objMoveitem.m_pRet
	}
}
__declspec(naked) void Naked_MoveWhitem()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x24] //packet size
		mov edx, [esp + 0x20]//packet buffer
		FixMoveItem(ecx, edx, eax);
			popad
			add esp, 0x8
			jmp g_objMoveWhItem.m_pRet
	}
}






__declspec(naked) void Naked_FixTimer2()
{
	_asm
	{
		pushad
		mov eax, [esp + 0x24] //packet size
		mov edx, [esp + 0x20]//packet buffer
		send_charItems(ecx, edx, eax);
			popad
			add esp, 0x8
			jmp g_Hookobj_Timer2.m_pRet
	}
}



void __stdcall fun_newmount(Pshaiya_player player, SHOW_MOUNT* pPacket, PDWORD pSize)
{
	if (pPacket->model== 0xde)//range是200的话就是这个0xde
	{
		PplayerItemcharacterise pItem = player->BagItem[0][13];
		if (pItem)
		{
			pPacket->_type= pItem->ItemType;
			pPacket->_typeid= pItem->Typeid;
			*pSize = sizeof(SHOW_MOUNT);
		}
	}
}


void getBasicDyeColor(DWORD charid, PBASICDYEAPPREANCE pcolor)
{
	for (DWORD i = 0; i < 17; i++)
	{
		std::string strResult = "";
		g_objPy->lock();
		try
		{
			strResult = boost::python::call_method<std::string>(g_objPy->m_module->get(), "get_dyecolor_byCharid", charid, i);
		}
		catch (...)
		{
			MyLog("getBasicDyeColor:%s\n", getPythonException().c_str());
		}
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


void getBasicDyeColor(std::string strCharname,PBASICDYEAPPREANCE pcolor)
{
	for (DWORD i = 0; i < 17; i++)
	{
		std::string strResult = "";
		g_objPy->lock();
		try
		{
			strResult = boost::python::call_method<std::string>(g_objPy->m_module->get(), "get_dyecolor_byName", strCharname.c_str(), i);
		}
		catch (...)
		{
			MyLog("getBasicDyeColor:%s\n", getPythonException().c_str());
		}
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


void __stdcall LoginNameFix(Pshaiya_player player, Pold_LoginToon pOld)
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
		strResult = boost::python::call_method<std::string>(g_objPy->m_module->get(), "GetPetAndCostumeId", pOld->charname);
	}
	catch (...)
	{
		MyLog("LoginNameFix:%s\n", getPythonException().c_str());
	}
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
	getBasicDyeColor(pNew->charname,&pNew->color);


	//5.发送新包
	SendPacketToPlayer(player, pNew, sizeof(LoginToon));
	delete pNew;
}




void __stdcall FixPPLName(Pshaiya_player player, Pshaiya_player pTargetPlayer, Pold_Meet pPacket, DWORD dwPacketSize)
{
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
	pItem = pTargetPlayer->BagItem[0 ][0xf];
	if (pItem)
	{
		stcMeet._item[0xf].type = pItem->ItemType;
		stcMeet._item[0xf]._typeid = pItem->Typeid;
	}

	pItem = pTargetPlayer->BagItem[0 ][0x10];
	if (pItem)
	{
		stcMeet._item[0x10].type = pItem->ItemType;
		stcMeet._item[0x10]._typeid = pItem->Typeid;
	}



	//说明有行会
	if (dwPacketSize>sizeof(old_Meet))
	{
		memcpy(stcMeet.guildname, &pPacket->guildname, dwPacketSize - 0x43+1);
//		if (pPacket->guildname)
//			memcpy(stcMeet.guildname, &pPacket->guildname, dwPacketSize-0x43);     //会长是在这
//		else	
//			strcpy(&stcMeet.guildname[6], (char*)(DWORD(&pPacket->guildname)+6));  //非会长在这
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
		if (pItem&&pItem->color.color1.bEnable)
		{
			stc.Item_type = pItem->ItemType;
			stc.slot = i;
			stc.type_id = pItem->Typeid;
			memcpy(&stc.color, &pItem->color, sizeof(DyeColor));
			SendPacketToPlayer(player, &stc, sizeof(stc));
		}
	}


	//if transform skill using
	if(pTargetPlayer->isUsedTransformationSkill)
	{
		TRANSFORM stc;
		stc.charid = pTargetPlayer->Charid;
		stc.isUsedTransform = true;
		SendPacketToPlayer(player, &stc, sizeof(stc));
	}
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
		memcpy(&pNewItem[i], &pOldItem[i], offsetof(oldWhitem, count));//到lapis复制完
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
	CopyMemory(&pbuff[4], &pPacket[2], dwSize-2);

	//4.把原来的包发给用户
	SendPacketToPlayer(player, pbuff, dwNewSize);

	delete[] pbuff;
}


void __stdcall FixHpOnLogin(DWORD dwPlayer, PBYTE pPacket, DWORD dwSize)
{
	//1.制新包
	PBYTE pbuff = new BYTE[dwSize+12];//我们的包缺了12个字节
	ZeroMemory(pbuff, dwSize + 12);

	//2.复制头
	CopyMemory(pbuff, pPacket, 2);                //前面空的12个字节暂时不管
	CopyMemory(&pbuff[2+12], &pPacket[2],dwSize-2);

	//4.把原来的包发给用户
	SendPacketToPlayer(dwPlayer, (DWORD)pbuff, dwSize + 12);

	fun_ProcessOnLogin((Pshaiya_player)dwPlayer);


	delete[] pbuff;
}



void __stdcall fun_useSkill(Pshaiya_player player,DWORD skill)
{
	WORD TypeDetail = *PWORD(skill + 0x32);
	if (TypeDetail == 132)
	{
		commonfun_setTransform(player,true);
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
void __stdcall fun_FixItemMall(Pshaiya_player player, ITEMMALLITEMHEAD* pItemMallHead, DWORD dwSize)
{
	DWORD dwNewSize = pItemMallHead->count * sizeof(ITEMMALLITEMBODY) + sizeof(ITEMMALLITEMHEAD)+4;
	ITEMMALLITEMHEAD* p =(ITEMMALLITEMHEAD*) new BYTE[dwNewSize];
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


void __stdcall fun_common_marketbodycorrectionAndSend(Pshaiya_player player,PVOID packet,DWORD dwSize)
{
	 //1.just the body size has changed
	DWORD dwTailSize = 0;
	if (*PWORD(packet) == official_packet_const::completedAddItem_market&&dwSize == 0x3e)
	{
		dwTailSize = 0x4;
	}

	   //老大小+新body差值+尾部有可能产生的大小
	DWORD dwNewSize = dwSize + sizeof(COMMON_ITEMBODY_MARKET) - sizeof(OLD_COMMON_ITEMBODY_MARKET)+dwTailSize;
	PVOID p = new BYTE[dwNewSize];


	//2.process head
	DWORD headSize = dwSize - sizeof(OLD_COMMON_ITEMBODY_MARKET)-dwTailSize;
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


void __stdcall  fun_recivedMarketOnSaleInfo(Pshaiya_player player, RECIVEDONSALE_MARKECT_HEAD* packet,DWORD dwSize)
{
	BYTE byCount = packet->count;
	DWORD NewSize = byCount * sizeof(ONSALEBODY_MARKECT) + sizeof(RECIVEDONSALE_MARKECT_HEAD);
	ONSALEBODY_MARKECT* p = (ONSALEBODY_MARKECT*)new BYTE[NewSize];
	
	//head
	memcpy(p, packet, sizeof(RECIVEDONSALE_MARKECT_HEAD));

	ONSALEBODY_MARKECT* pNew = PONSALEBODY_MARKECT(DWORD(p) + sizeof(RECIVEDONSALE_MARKECT_HEAD));
	OLD_ONSALEBODY_MARKECT* pOld= POLD_ONSALEBODY_MARKECT(DWORD(packet) + sizeof(RECIVEDONSALE_MARKECT_HEAD));

	//body
	for (DWORD i = 0; i < byCount; i++)
	{
		reNewMarketHead(&pNew[i], &pOld[i], ONSALEBODY_MARKECT);
		reNewMarketBody(&pNew[i].body,&pOld[i].body);
	}

	SendPacketToPlayer(player, p, NewSize);
	delete[] p;
}

void __stdcall  fun_enterZoneNotice_1(Pshaiya_player player, PENTERZONE_NOTICE packet)
{
	ENTERZONE_NOTICE stc;
	_InitPacket(stc);
	
	//head
	memcpy(&stc, packet, offsetof(ENTERZONE_NOTICE, color));

	//body
	getBasicDyeColor(player->Charid, &stc.color);

	//send
	SendPacketToPlayer(player, &stc, sizeof(stc));
}

void __stdcall  fun_enterZoneNotice_2(Pshaiya_player player, PENTERZONE_NOTICE packet,DWORD dwEcx,DWORD dwEax,DWORD dwEbp)
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
		mov eax,dwEax
		mov ecx,dwEcx
		call dwCall
	}

}



void __stdcall fun_DisableSkill(Pshaiya_player player, DWORD skill)
{
	WORD TypeDetail = *PWORD(skill + 0x32);
	if (TypeDetail == 132)
	{
		commonfun_setTransform(player, false);
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
		memcpy(&stc.src, &pOld->src, offsetof(OLDMOVEITEM, src.lapis));
		stc.src.count = pOld->src.count;
		memcpy(stc.src.craftname, pOld->src.craftname, 21);
		stc.src.MakeType = 'D';  //因为是未知值，随便整的

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
		memcpy(&stc.dst, &pOld->dst, offsetof(OLDMOVEITEM, dst.lapis));
		stc.dst.count = pOld->dst.count;
		memcpy(stc.dst.craftname, pOld->dst.craftname, 21);
		stc.dst.MakeType = 'D';

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
		pNew[i].QA= *PWORD(&pPacket[i * 0x22 + 4]);

		//复制count
		pNew[i].item_count= pPacket[i * 0x22 + 12];

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
	for (DWORD i = 0; i <itemCount; i++) 
	{
		BYTE byBag  = pNew[i].bag;          
		BYTE bySlot = pNew[i].slot;
		BYTE byType = pNew[i]._type;
		if (IsPet(byType))
		{
			ShowPet(player, byBag, bySlot);
		}
	}
	delete[] p;
}



void __stdcall fun_quislot_send(Pshaiya_player player, PCOMMONHEAD packet)
{
	DWORD dwSize = packet->byCount*sizeof(QUICKSLOT) + sizeof(COMMONHEAD);
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
	DWORD dwSize = packet->byCount*sizeof(OLDQUICKSLOT);
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


void __stdcall fun_antuStats_send(Pshaiya_player player)
{
	AUTOSTATS_GET stc;
	_InitPacket(stc);

	std::string strStats = "";
	try
	{
		g_objPy->lock();
		strStats = boost::python::call_method<std::string>(g_objPy->m_module->get(), "get_auto_stats", player->Charid);	
	}
	catch (...)
	{
		MyLog("fun_antuStats_send:%s\n", getPythonException().c_str());
	}
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
	
	stc.stats.str = str;
	stc.stats.dex = dex;
	stc.stats.rec = rec;
	stc.stats.luc = luc;
	stc.stats.wis = wis;
	stc.stats._int = _int;
	SendPacketToPlayer(player, &stc, sizeof(stc));
}


void __stdcall fun_autoStats_set(Pshaiya_player player, PAUTOSTATS_SET packet)
{
	bool bRet = false;
	try
	{
		g_objPy->lock();
		bRet = boost::python::call_method<bool>(g_objPy->m_module->get(), "set_auto_stats", player->Charid,
			packet->stats.str, 
			packet->stats.dex,
			packet->stats.rec,
			packet->stats.luc,
			packet->stats.wis,
			packet->stats._int);
	}
	catch (...)
	{
		MyLog("fun_autoStats_set:%s\n", getPythonException().c_str());
		g_objPy->Unlock();
		return;
	}
	g_objPy->Unlock();

	if (bRet){
		AUTOSTATS_GET stc;
		_InitPacket(stc);
		memcpy(&stc.stats, &packet->stats, sizeof(PLAYERSTATS));
		SendPacketToPlayer(player, &stc, sizeof(stc));
	}else {
		DisConnectPlayer(player);
	}
		
}

void __stdcall fun_battle_field(Pshaiya_player player, PBATTLEFIELD packet)
{
	std::string strResult = "";
	g_objPy->lock();
	try
	{	
		strResult = boost::python::call_method<std::string>(g_objPy->m_module->get(), "battlefield_moveable", player->Country, player->Level, packet->map);
	}
	catch (...)
	{
		MyLog("fun_battle_field:%s\n", getPythonException().c_str());
	}
	g_objPy->Unlock();

	if (strResult.empty())
		return;

	BOOL bAble = FALSE;
	DWORD map = 0;
	float x, y, z;

	sscanf(strResult.c_str(), "%d,%d,%f,%f,%f",&bAble,&map,&x,&y,&z );

	if (bAble){
		MovePlayer((DWORD)player, map, x, z);
	}

}




void __stdcall fun_Dying_getSample(Pshaiya_player player, PDYING_SAMPLING packet)
{
	DYING_SAMPLING stc;
	std::string strSampleColor = "";
	g_objPy->lock();
	try
	{
		strSampleColor = boost::python::call_method<std::string>(g_objPy->m_module->get(), "get_random_dye");
	}
	catch (...)
	{
		MyLog("fun_Dying_getSample:%s\n", getPythonException().c_str());
	}
	g_objPy->Unlock();

	ZeroMemory(stc.Ay_color, sizeof(DyeColor) * 5);
	if (!strSampleColor.empty())
	{
		DWORD temp[5] = { 0 };
		sscanf(strSampleColor.c_str(), "%ld,%ld,%ld,%ld,%ld", &temp[0], &temp[1], &temp[2], &temp[3], &temp[4]);
		for (DWORD i = 0; i < 5; i++)
		{
			stc.Ay_color[i].color1.bEnable = true;
			stc.Ay_color[i].color1.cmyk = temp[i];
		}
	}

	memcpy(player->dye_color, stc.Ay_color, sizeof(stc.Ay_color));
	SendPacketToPlayer(player, &stc, sizeof(stc));
}



void save_dyecolor(Pshaiya_player player, PplayerItemcharacterise pitem)
{

	try
	{
		g_objPy->lock();
		 boost::python::call_method<void>(g_objPy->m_module->get(), "save_dyecolor",
			pitem->itemuid,
			pitem->color.color1.cmyk,
			pitem->color.color2.cmyk,
			pitem->color.color3.cmyk);
	}
	catch (...)
	{	
		MyLog("save_dyecolor:%s\n", getPythonException().c_str());
	}
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
	memcpy(&stc.color, &player->dye_color[i],sizeof(stc.color));   //复制到包
	memcpy(&pItem->color, &player->dye_color[i], sizeof(stc.color));//复制到物品处

	//4.save to database
	save_dyecolor(player, pItem);  //更新到数据库

	SendPacketToPlayer(player, &stc, sizeof(stc));
}

//不管是什么物品，都返回样本颜色
void __stdcall  fun_Dying_prepareItem(Pshaiya_player player, PDYING_PREPAREITEM packet)
{
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

		DWORD dwItem = GetItemAddress(dwType, dwTypeid);

		if (*PBYTE(dwItem + 0x46) != 104)
			break;

		//Move Player
		DWORD dwNpcType = 2;
		DWORD dwNpcId = *PBYTE(dwItem + 0x34);
		DWORD dwNpc = GetNpcAddr(dwNpcType, dwNpcId);

		if (!dwNpc)
			break;

		//Delete item
		DeleteItem(player, byBag, bySlot);
		DWORD dwMap = 0;
		float fX, fY = 0;
		GetPosfromKeeper(dwNpc,packet->translate_Index, &dwMap, &fX, &fY);
		MovePlayer((DWORD)player, dwMap, fX, fY);
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

						sendItem(player, MakeItemId(synType,synTypeid));
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
		try
		{
			g_objPy->lock();
			strResult = boost::python::call_method<std::string>(g_objPy->m_module->get(), "GeneralMoveTowns_moveable", packet->scrollID, player->Country);
		}
		catch (...)
		{
			MyLog("fun_use_TownScroll:%s\n", getPythonException().c_str());
		}
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

	try
	{
		g_objPy->lock();
		byPacket[2] = boost::python::call_method<int>(g_objPy->m_module->get(), "AvailiableCharName", (const char*)packet);
	}
	catch (...)
	{
		MyLog("fun_confrim_charname:%s\n", getPythonException().c_str());

	}
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
		fun_battle_field(player, (PBATTLEFIELD)packet);
		break;

	case official_packet_const::AutoStats_get:
		//fun_antuStats_send(player, packet);
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

	case 0x104:
		break;

	default:
	{
		MyLog("illage packet:%x\n", *packet);
		DisConnectPlayer(player);
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
		strResult = boost::python::call_method<std::string>(g_objPy->m_module->get(), "get_dyecolor",(UINT64)pItemcharacterise->itemuid);	
	}
	catch (...)
	{	
		MyLog("fun_getdyecolor:%s\n", getPythonException().c_str());
	}	
	g_objPy->Unlock();

	ZeroMemory(&pItemcharacterise->color, sizeof(DyeColor));

	DWORD color_1 = 0;
	DWORD color_2 = 0;
	DWORD color_3 = 0;
	if (!strResult.empty())
		sscanf(strResult.c_str(), "%ld,%ld,%ld", &color_1, &color_2, &color_3);

	if (color_1){
		pItemcharacterise->color.color1.bEnable = true;
		pItemcharacterise->color.color1.cmyk = color_1;
	}

	if (color_2){
		pItemcharacterise->color.color2.bEnable = true;
		pItemcharacterise->color.color2.cmyk = color_2;
	}

	if (color_3){
		pItemcharacterise->color.color3.bEnable = true;
		pItemcharacterise->color.color3.cmyk = color_3;
	}
}


void loadEnchant_addValue() {
	WCHAR szPath[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(0), szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);
	wcscat(szPath, L"\\data\\ItemEnchant.ini");

	
	for (WORD i = 0; i <21; i++){

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
			stc.synthesize_Itemtype = itemid/1000;
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
			stc.successRate = _tcstof(szMeterrials, 0)*100;

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


void __stdcall fun_RecivedTradeItem(Pshaiya_player player, OLD_RECIVEDTRADEITEM* pPacket, DWORD dwSize, PplayerItemcharacterise pItem)
{
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
void __stdcall fun_shopView(Pshaiya_player player, PCOMMONHEAD oldpacket, DWORD dwSize, PLAYERSHOP* pShop)
{
	DWORD newSize = oldpacket->byCount * sizeof(SHOPITEM) + sizeof(COMMONHEAD);
	PVOID p = new BYTE[newSize];
	ZeroMemory(p, newSize);

	//copy head
	memcpy(p, oldpacket, sizeof(COMMONHEAD));

	//get body
	SHOPITEM* pNewBody = (SHOPITEM*)getBodyOfcommonHeader(p);
	OLD_SHOPITEM* pOldBOdy = (OLD_SHOPITEM*)getBodyOfcommonHeader(oldpacket);


	Pshaiya_player pshopOwner = pShop->player;
	for (DWORD i = 0; i <oldpacket->byCount; i++)
	{
		//get info from old packet
		memcpy(&pNewBody[i], &pOldBOdy[i], offsetof(OLD_SHOPITEM, gem)); 
		pNewBody->gem[i] = pOldBOdy->gem[i];                             
		memcpy(pNewBody[i].craftname, pOldBOdy[i].craftname, _countof(pOldBOdy[i].craftname));

		//get info from target players
		BYTE bag = pShop->bag[i];
		BYTE slot = pShop->slot[i];
		PplayerItemcharacterise pItem = pshopOwner->BagItem[bag][slot];
		if (pItem)
		{
			if(IsPet(pItem->ItemType))
				pNewBody[i].createTime = pItem->createtime;
			memcpy(&pNewBody[i].color, &pItem->color, sizeof(DyeColor));
		}
	}
	SendPacketToPlayer(player, p, newSize);

	delete[] p;
}



void fun_sendWeaponEnchant_addvalue(Pshaiya_player player) {
	SendPacketToPlayer(player, &g_Enchant_Addvalue, sizeof(g_Enchant_Addvalue));
}


void fun_ProcessOnLogin(Pshaiya_player player) {
	fun_antuStats_send(player);
	fun_sendWeaponEnchant_addvalue(player);
}


void __stdcall fun_dragTradeItem(Pshaiya_player player, OLD_DRAGTRADEITEM* pPacket, DWORD dwSize,PplayerItemcharacterise pItem)
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
	memcpy(stc.craftname, pPacket->craftname,sizeof(pPacket->craftname));
	memcpy(&stc.color, &pItem->color,sizeof(stc.color));

	SendPacketToPlayer(player, &stc, sizeof(stc));
}




void __stdcall fun_setStatsThroughtItems(Pshaiya_player player)
{

		if (player->set_item.IsSetted)
		{
			player->total_str -= player->set_item.BackUpValue.Str;
			player->total_dex-= player->set_item.BackUpValue.Dex;
			player->total__Int_ -= player->set_item.BackUpValue.__Int_;
			player->total_wis -= player->set_item.BackUpValue.Wis;
			player->total_rec -= player->set_item.BackUpValue.Rec;
			player->total_luc -= player->set_item.BackUpValue.Luc;
			player->Maxhp-= player->set_item.BackUpValue.Hp;
			player->Maxmp -= player->set_item.BackUpValue.Mp;
			player->Maxsp -= player->set_item.BackUpValue.Sp;
			player->set_item.IsSetted = false;
		}

		ZeroMemory(&player->set_item.BackUpValue, sizeof(player->set_item.BackUpValue));


		//1. get weared items
		DWORD dwItem[16] = { 0 };
		for (DWORD i = 0; i < 16; i++){
		 PplayerItemcharacterise pItem = player->BagItem[0][i];
			if (pItem){
				dwItem[i] = pItem->ItemType * 1000 + pItem->Typeid;
			}
		}

		//2.从套装中遍历
		for (DWORD i = 0; i < g_vecSetItem.size(); i++)
		{
			//2.1看看套装表中有多少块装备符合的
			DWORD dwCount = 0;
			PSETITEMDATA pSetItem = g_vecSetItem[i];
			for (DWORD j = 0; j < 13; j++)
			{
				if (!pSetItem[j].dwItemid)   //列表的13件
					break;

				for (DWORD k = 0; k < 16; k++) //身上的16件装备
				{
					if (dwItem[k] == pSetItem[j].dwItemid)
					{
						dwCount++;
					}
				}

			}
			//2.2 找出符合的块数,去对应的Pieces_value 中找属性点
			if (dwCount)
			{
				PDWORD pValue = pSetItem[dwCount - 1].dwValue;
				memcpy(&player->set_item.BackUpValue, pSetItem[dwCount - 1].dwValue, sizeof(player->set_item.BackUpValue));
				player->total_str +=    player->set_item.BackUpValue.Str;
				player->total_dex +=    player->set_item.BackUpValue.Dex;
				player->total__Int_+=   player->set_item.BackUpValue.__Int_;
				player->total_wis +=    player->set_item.BackUpValue.Wis;
				player->total_rec +=    player->set_item.BackUpValue.Rec;
				player->total_luc +=    player->set_item.BackUpValue.Luc;
				player->Maxhp +=        player->set_item.BackUpValue.Hp;
				player->Maxmp +=        player->set_item.BackUpValue.Mp;
				player->Maxsp +=        player->set_item.BackUpValue.Sp;
				player->set_item.IsSetted = true;
			}
		}
}

__inline void checkLicense()
{
#ifndef _DEBUG
	VMProtectBegin("checkLicense");
	int nSize = VMProtectGetCurrentHWID(NULL, 0);
	char *buf = new char[nSize];
	VMProtectGetCurrentHWID(buf, nSize);

	char* strCode[] = {
		"2I5hovH6IRTXhUsQbh84NSY41lp6rpDS",
		"gAhCqJWE08jDybEbzgFTmg=="

	};

	for(DWORD i=0;i<_countof(strCode);i++){
		if (strcmp(buf, strCode[i]) == 0) {
			delete[] buf;
			return;
		}
	}
	delete[] buf;
	TerminateProcess(GetCurrentProcess(), 0);
	VMProtectEnd();
#endif
}


DWORD WINAPI ThreadProc1(_In_  LPVOID lpParameter)
{
	

	checkLicense();


	InitializeCriticalSection(&g_cs);
	g_objPy = new MyPython("shaiya_py");
 
	//1.登陆处
	Hookobj_loginName.Hook((PVOID)0x0047b9b3, Naked_LoginNameFix,5, LoginNameFix,1);


	//2.修复看不到其它人的角色名
	g_Hookobj_Meet.Hook((PVOID)0x00426c9b, Naked_FixPPLName);
	g_Hookobj_Meet.m_pfun_call = FixPPLName;

	//3.修复仓库的包对不的问题
	g_Hookobj_WareHouse.Hook((PVOID)0x00492731, Naked_GetWareHouse);
	g_Hookobj_WareHouse.m_pfun_call = GetWareHouse;

	g_Hookobj_GetwareHouse_1.Hook((PVOID)0x0049276b, Naked_FixWareHouse);
	g_Hookobj_GetwareHouse_1.m_pfun_call = GetWareHouse;

	BYTE byWareHouseCount = 0x28;           
	MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0049271e, &byWareHouseCount, sizeof(BYTE));//多少个物品发一个包

	//4.处理其它背包跟格子的Timer显示,在重进游戏读取背包的时候
	g_Hookobj_Timer1.Hook((PVOID)0x004925e9, Naked_FixTimer1);
	g_Hookobj_Timer2.Hook((PVOID)0x00492638, Naked_FixTimer2);
	g_Hookobj_Timer1.m_pfun_call = send_charItems;
	g_Hookobj_Timer2.m_pfun_call = send_charItems;

	//5.登录的时候发送坐标等问题修复
	g_HookObj_HpOnlogin.Hook((PVOID)0x004924d5, Naked_FixHpOnLogin);
	g_HookObj_HpOnlogin.m_pfun_call = FixHpOnLogin;

	//6. 解决交换物品时的问题
	g_objMoveitem.Hook((PVOID)0x00468e5f, Naked_MoveItem);
	g_objMoveWhItem.Hook((PVOID)0x0046992f, Naked_MoveWhitem);
	g_HookObj_MoveWhTobag.Hook((PVOID)0x004693d3, Naked_MoveWhTobag);
	g_objMoveWhToWh.Hook((PVOID)0x00469be7, Naked_MoveWhToWh);

	g_objMoveWhToWh.m_pfun_call = FixMoveItem;
	g_HookObj_MoveWhTobag.m_pfun_call = FixMoveItem;
	g_objMoveitem.m_pfun_call = FixMoveItem;
	g_objMoveWhItem.m_pfun_call = FixMoveItem;

	//7. 解决技能显示的问题
    HookObj_ShowKill.Hook((PVOID)0x0049282b, Naked_FixShowSkill);
	HookObj_ShowKill.m_pfun_call = FixShowSkill;

	//8.强化相关	
	g_ObjGetenchant.Hook((PVOID)0x0047a040, Naked_fixEnchant, 11);
	g_ObjGetenchant.m_pfun_call = illagePacketProcess;

	//9.处理自动属性的问题	
	g_objAutoStats.Hook((PVOID)0x00474d6b, Naked_autoStats, 10);
	g_objAutoStats.m_pfun_call = illagePacketProcess;

	//10.GM跑步速度:只要是GM就跳过速度	
	g_objgmMoveSpeed.Hook((PVOID)0x0049b320, Naked_gmMoveSpeed, 6);

	//11.强化返回的结果
	g_objEnchantResult.Hook(PVOID(0x0046cf89), Naked_enchantResult, 5, fun_enchantResult,1);
	g_objEnchantResult_2.Hook((PVOID)0x0046d286, Naked_enchantResult, 5, fun_enchantResult, 1);



	//12.排序
	g_objSort.Hook(PVOID(0x0047798e), Naked_sort, 11);
	g_objSort.m_pfun_call = illagePacketProcess;

	//13.保存玩家格子	
	objQuickslot.Hook(PVOID(0x00474c50), Naked_quickslot, 6);
	g_objSendQuickslot.Hook(PVOID(0x00492ac3), Naked_sendquickslot, 5);

	g_objSendQuickslot.m_pfun_call = fun_quislot_send;
	objQuickslot.m_pfun_call = fun_quickslot_recv;

	//14.先把玩家的背包物品的宽度增加一下
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



	//15.发送Dye颜色给别人
	{
		BYTE dwSize = sizeof(MEETAPPREANCE);
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00492cd8, &dwSize, 1);
	}
	g_objsendDyeAppreance.Hook(PVOID(0x00492ceb), Naked_sendDyeAppreance, 5);
	g_objsendDyeAppreance.m_pfun_call = fun_sendDyeAppreance;


	//16.
	g_objIllageFix1.Hook((PVOID)0x00479150, Naked_IllageFix1, 5);
	g_objIllageFix1.m_pfun_call = illagePacketProcess;

	//17.
	g_objuseTransform.Hook((PVOID)0x00478c7c, Naked_usetransform, 5);
	g_objuseTransform.m_pfun_call = fun_useSkill;

	//18.
	g_objDisableTransform.Hook(PVOID(0x004960ef), Naked_DisableTransform, 6);
	g_objDisableTransform.m_pfun_call = fun_DisableSkill;

	//19.	
	g_objConfrimCharname.Hook((PVOID)0x0047a569, Naked_ConfrimCharName, 11);
	g_objConfrimCharname.m_pfun_call = illagePacketProcess;

	//20.过图时显示时装
	g_objEnterZoneNotice_1.Hook(PVOID(0x0042a504), Naked_enterZoneNotice_1, 5, fun_enterZoneNotice_1);
	g_objEnterZoneNotice_2.Hook(PVOID(0x0041c85e), Naked_enterZoneNotice_2, 5, fun_enterZoneNotice_2);

	//21. markect

	   // recived item information from markect
	g_objMarkectRecivedOnsale.Hook((PVOID)0x00440ff9, Naked_RecivedMarketOnSaleInfo, 5, fun_recivedMarketOnSaleInfo,1);

	  //compelted Item market
	g_objMarketCompeltedItem.Hook(PVOID(0x004427cb), Naked_CompeletedItem, 5, fun_compeletedItem);
	
	  //completed money market
	g_objMarketCompletedMoney.Hook(PVOID(0x0044299c), Naked_CompletedMoney, 5, fun_completedMoney);

	  //interested list
	g_objMarketInterestedItems.Hook(PVOID(0x4443e9), Naked_InterestedItems, 5, fun_InterestedItems);

	  //sale Registered list
	g_objMarketSaleRegisteredItem.Hook(PVOID(0x004423a1), Naked_SaleRegisteredItem, 5, fun_SaleRegisteredItem);


	{
		DWORD phookAddr[] = { 0x0046baa6 ,0x00443bb3,0x00443123,0x00486703,0x0048695b,0x00444686,
			0x00442c55,0x0044043f,0x00443261,0x0044334f,0x00440153,0x00440331,0x00442d36,
			0x0044334f,0x00443950,0x0044489c,0x00444d51 };

		for (DWORD i = 0; i < _countof(phookAddr); i++)
		{
			g_objMarketUnknown[i].Hook((PVOID)phookAddr[i], Naked_Marketunknown_common, 5, fun_common_marketbodycorrectionAndSend, 1);
		}
	}

	//22. item mall
	g_objBuyItemmallItem.Hook((PVOID)0x00488704, Naked_FixItemMall,5,fun_FixItemMall,1);

	//23. chaotic square
	loadChaotic();

	//24. Drag TradeItem
	g_objDragTradeItem.Hook(PVOID(0x0047df2f), Naked_DragTradeItem, 5, fun_dragTradeItem,1);

	//25.Recived TradeItem
	g_objRecivedTradeItem.Hook((PVOID)0x0047ef76, Naked_RecivedTradeItem, 5, fun_RecivedTradeItem, 1);

	//26. shop view
	g_objShopView.Hook(PVOID(0x0048734b), Naked_shopView, 5, fun_shopView, 1);
	
	//27. Enchant Values
	loadEnchant_addValue();

	//28. new mount appreance fixed
	g_objNewMountAppreance.Hook(PVOID(0x004263b8), Naked_NewMountAppreance, 7, fun_newmount);


	//29. set Items
	FILE* pFile = fopen("data\\setitem.csv", "r");
	if (pFile != NULL)
	{
		char szLine[1024] = { 0 };
		fgets(szLine, 1024, pFile);
		while (!feof(pFile))
		{
			PSETITEMDATA stcSetitem = new SETITEMDATA[13];
			ZeroMemory(stcSetitem, sizeof(SETITEMDATA) * 13);
			fgets(szLine, 1024, pFile);
			char szTemp[14][MAX_PATH] = { 0 };
			sscanf_s(szLine, "%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,]",
				&stcSetitem[13].dwItemid, szTemp[13], MAX_PATH,
				&stcSetitem[0].dwItemid, szTemp[0], MAX_PATH,
				&stcSetitem[1].dwItemid, szTemp[1], MAX_PATH,
				&stcSetitem[2].dwItemid, szTemp[2], MAX_PATH,
				&stcSetitem[3].dwItemid, szTemp[3], MAX_PATH,
				&stcSetitem[4].dwItemid, szTemp[4], MAX_PATH,
				&stcSetitem[5].dwItemid, szTemp[5], MAX_PATH,
				&stcSetitem[6].dwItemid, szTemp[6], MAX_PATH,
				&stcSetitem[7].dwItemid, szTemp[7], MAX_PATH,
				&stcSetitem[8].dwItemid, szTemp[8], MAX_PATH,
				&stcSetitem[9].dwItemid, szTemp[9], MAX_PATH,
				&stcSetitem[10].dwItemid, szTemp[10], MAX_PATH,
				&stcSetitem[11].dwItemid, szTemp[11], MAX_PATH,
				&stcSetitem[12].dwItemid, szTemp[12], MAX_PATH);

			for (DWORD i = 0; i < 13; i++)
			{
				sscanf_s(szTemp[i], "%d|%d|%d|%d|%d|%d|%d|%d|%d",
					&stcSetitem[i].dwValue[0],
					&stcSetitem[i].dwValue[1],
					&stcSetitem[i].dwValue[2],
					&stcSetitem[i].dwValue[3],
					&stcSetitem[i].dwValue[4],
					&stcSetitem[i].dwValue[5],
					&stcSetitem[i].dwValue[6],
					&stcSetitem[i].dwValue[7],
					&stcSetitem[i].dwValue[8]);
			}

			//筛选至少有2件或以上的
			if (stcSetitem[0].dwItemid&&stcSetitem[1].dwItemid)
			{
				g_vecSetItem.push_back(stcSetitem);
			}
		}
		fclose(pFile);

		if (g_vecSetItem.size())
			g_HookObj_Setitem.Hook((PVOID)0x00460e40, Naked_Setitem, 9, fun_setStatsThroughtItems);
	}


	




	return 0;
}


