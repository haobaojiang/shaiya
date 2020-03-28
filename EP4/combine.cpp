#include "stdafx.h"
#include "combine.h"


namespace combine
{
	vector<COMBINETASK> g_vecCombines;
	CMyInlineHook g_obj;


	void  Login(void* p) {
		WORD cmd = PacketOrder::combine_materials;
		SendPacketToPlayer(p, &cmd, sizeof(cmd));
	}

	bool isEnoughItems(Pshaiya50_player player, DWORD Itemid, int count) {

		//不可能没有数量的
		if (!count) {
			return false;
		}

		for (DWORD i = 1; i < 6; i++) {
			for (DWORD j = 0; j < 24; j++) {
				PplayerItemcharacterise pItem = player->BagItem[i][j];
				if (pItem&&pItem->ItemAddr->itemid == Itemid) {
					count -= pItem->ItemAddr->Count;
					if (count<=0)
						return true;
				}
			}
		}
		return false;
	}

	bool isBag(DWORD bag,DWORD slot) {
		return bag < 6 && slot < 24 ? true : false;
	}


	bool __stdcall fun(Pshaiya50_player player, PCOMBINEREQUEST packet) {


		if (!isBag(packet->bags[0], packet->slots[0]) ||
			!isBag(packet->bags[1], packet->slots[1]) ||
			!isBag(packet->bags[2], packet->slots[2])) {
			return false;
		}

		PplayerItemcharacterise pItem1 = player->BagItem[packet->bags[0]][packet->slots[0]];
		PplayerItemcharacterise pItem2 = player->BagItem[packet->bags[1]][packet->slots[1]];
		PplayerItemcharacterise pItem3 = player->BagItem[packet->bags[2]][packet->slots[2]];

		if (pItem1&&pItem2&&pItem3) {

			//先冒泡排序
			DWORD itemid[3] = { 0 };
			itemid[0]= pItem1->ItemAddr->itemid;
			itemid[1]= pItem2->ItemAddr->itemid;
			itemid[2] = pItem3->ItemAddr->itemid;


			for (DWORD i = 0; i < 3; i++) {
				for (DWORD j = i; j < 3; j++) {
					if (itemid[i] > itemid[j]) {
						DWORD temp = itemid[j];
						itemid[j] = itemid[i];
						itemid[i] = temp;
					}
				}
			}




			for (auto iter = g_vecCombines.begin(); iter != g_vecCombines.end(); iter++) {

				//判断ID
				if (iter->Materials[0].id == itemid[0]&&
					iter->Materials[1].id == itemid[1]&&
					iter->Materials[2].id == itemid[2])
				{
					//判断数量
					if (isEnoughItems(player, iter->Materials[0].id, iter->Materials[0].count) &&
						isEnoughItems(player, iter->Materials[1].id, iter->Materials[1].count) &&
						isEnoughItems(player, iter->Materials[2].id, iter->Materials[2].count))
					{
						DeleteItemByItemID(player, iter->Materials[0].id, iter->Materials[0].count);
						DeleteItemByItemID(player, iter->Materials[1].id, iter->Materials[1].count);
						DeleteItemByItemID(player, iter->Materials[2].id, iter->Materials[2].count);
						sendItem(player, iter->rewardId);
						char strNotice[MAX_PATH] = { 0 };
						sprintf(strNotice, "恭喜%s通过 [%d个%s] [%d个%s]", player->charname,
							iter->Materials[0].count, PITEMINFO(getItemAddr(iter->Materials[0].id))->itemname,
							iter->Materials[1].count, PITEMINFO(getItemAddr(iter->Materials[1].id))->itemname);
						sendNotice(strNotice);
 						sprintf(strNotice, "[%d个%s] 兑换 [%s]成功!", 
 							iter->Materials[2].count, PITEMINFO(getItemAddr(iter->Materials[2].id))->itemname,
 							PITEMINFO(getItemAddr(iter->rewardId))->itemname);

						sendNotice(strNotice);
						return true;
					}
					else
					{
						sendNoticeToplayer(player, "材料数量不够!");
					}
				}
			}
		}


		return false;
	}


	DWORD addr = 0x00461015;
	__declspec(naked) void  Naked()
	{
		_asm
		{

			movzx esi, byte ptr[eax + 0x2]
			push edi
			pushad
			MYASMCALL_2(fun,ecx, eax)
			test al,al
			popad
			je _Org

			jmp addr

			_Org:
			jmp g_obj.m_pRet

		}
	}



	void Start()
	{
		for (DWORD i = 0; i < 2000; i++) {

			COMBINETASK st = { 0 };
			WCHAR key[50] = { 0 };
			WCHAR szReturn[MAX_PATH] = { 0 };

			swprintf(key, L"%d", i + 1);
			GetPrivateProfileString(L"combine", key, L"", szReturn, MAX_PATH, g_szFilePath);

			if (szReturn[0]==L'\0') {
				break;
			}

			//1=100001,(30001,3)(30002,4)(30003,5)
			swscanf(szReturn, L"%d,(%ld,%ld),(%ld,%ld),(%ld,%ld)", &st.rewardId,
				&st.Materials[0].id, &st.Materials[0].count,
				&st.Materials[1].id, &st.Materials[1].count,
				&st.Materials[2].id, &st.Materials[2].count);
#ifdef _DEBUG
			LOGD << "i" << i <<"->szreturn:"<<szReturn;
#endif

			if (!st.rewardId ||
				!st.Materials[0].id || !st.Materials[0].count ||
				!st.Materials[1].id || !st.Materials[1].count ||
				!st.Materials[2].id || !st.Materials[2].count) {
				break;
			}

			//冒泡排序一下
			for (DWORD j = 0; j < 3; j++) {
				for (DWORD k = j; k < 3; k++) {
					if (st.Materials[j].id > st.Materials[k].id) {
						COMBINETASK stTemp = { 0 };
						memcpy(&stTemp.Materials[0], &st.Materials[j], sizeof(stTemp.Materials[0]));
						memcpy(&st.Materials[j], &st.Materials[k], sizeof(stTemp.Materials[0]));
						memcpy(&st.Materials[k], &stTemp.Materials[0], sizeof(stTemp.Materials[0]));
					}
				}
			}

			LOGD << "combine-->rewardId:" << st.rewardId
				<< "materials1:" << st.Materials[0].id << "count:" << st.Materials[0].count
				<< "materials2:" << st.Materials[1].id << "count:" << st.Materials[1].count
				<< "materials3:" << st.Materials[2].id << "count:" << st.Materials[2].count;

			g_vecCombines.push_back(st);

		}
		if (g_vecCombines.size()) {
			g_obj.Hook(PVOID(0x004608cc), Naked, 5);
			g_vecOnloginCallBack.push_back(Login);
		}
	}
}