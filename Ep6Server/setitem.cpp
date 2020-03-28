
#include "stdafx.h"
#include "setItem.h"




namespace setItem
{

	CMyInlineHook g_obj, g_objAfterResetStatus;
	SETITEMDATA g_setitems[300] = { 0 };
	void sendStatschangedView(Pshaiya_player player, BYTE setCase, DWORD value, char* strName = NULL);
	bool IsSetitem(PplayerItemcharacterise pItem) {

		if (!pItem) {
			return false;
		}

		PITEMINFO item = pItem->ItemAddr;
		if (!item->ReqStr || item->ReqStr > _countof(g_setitems)) {
			return false;
		}
		return true;
	}



	void AddsetItemstats(Pshaiya_player player, WORD ReqStr, BYTE count)
	{
		if (!ReqStr || ReqStr > _countof(g_setitems) || !count)
			return;

		SETITEMSTATS pValue = g_setitems[ReqStr - 1].dwValue[count - 1];
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


	void sendStatschangedView(Pshaiya_player player, BYTE setCase, DWORD value, char* strName) {
		GM_SETCOMMAND StcView;
		ZeroMemory(&StcView, sizeof(StcView));
		StcView.cmd = 0xf701;
		StcView.setCase = setCase;
		StcView.setValue = value;
		if (strName) {
			strcpy(StcView.strPlayer, strName);
		}
		SendPacketToPlayer((DWORD)player, DWORD(&StcView), sizeof(StcView));
	}



	void sendStrChangeView(Pshaiya_player player) {
		sendStatschangedView(player, gmSetCommandCase::SetStr, player->str);
	}

	void sendDexChangeView(Pshaiya_player player) {
		sendStatschangedView(player, gmSetCommandCase::SetDex, player->dex);
	}

	void sendLucChangeView(Pshaiya_player player) {
		sendStatschangedView(player, gmSetCommandCase::SetLuc, player->luc);
	}

	void sendRecChangeView(Pshaiya_player player) {
		sendStatschangedView(player, gmSetCommandCase::SetRec, player->rec);
	}

	void sendIntChangeView(Pshaiya_player player) {
		sendStatschangedView(player, gmSetCommandCase::SetInt, player->_Int_);
	}

	void sendWisChangeView(Pshaiya_player player) {
		sendStatschangedView(player, gmSetCommandCase::SetWis, player->wis);
	}


	void sendBaiscStatsChange(Pshaiya_player player) {
		sendStrChangeView(player);
		sendDexChangeView(player);
		sendLucChangeView(player);
		sendRecChangeView(player);
		sendIntChangeView(player);
		sendWisChangeView(player);
	}

	void __stdcall Mainfun(Pshaiya_player player)
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



			player->str -= player->set_item.BackUpValue.Str;
			player->dex -= player->set_item.BackUpValue.Dex;
			player->_Int_ -= player->set_item.BackUpValue.__Int_;
			player->wis -= player->set_item.BackUpValue.Wis;
			player->rec -= player->set_item.BackUpValue.Rec;
			player->luc -= player->set_item.BackUpValue.Luc;


			player->set_item.IsSetted = false;
			isHpChanged = true;
		}
		ZeroMemory(&player->set_item.BackUpValue, sizeof(player->set_item.BackUpValue));

		//衣服套装属性
		for (DWORD i = 0; i < 5; i++) {
			if (IsSetitem(player->BagItem[0][i])) {
				BYTE count = 1;
				for (DWORD j = i + 1; j < 5; j++) {
					if (IsSetitem(player->BagItem[0][j])) {
						count++;
					}
				}
				AddsetItemstats(player, player->BagItem[0][i]->ItemAddr->ReqStr, count);
				break;
			}
		}

		//首饰套装属性
		for (DWORD i = 8; i < 13; i++) {
			if (IsSetitem(player->BagItem[0][i])) {
				BYTE count = 1;
				for (DWORD j = i + 1; j < 13; j++) {
					if (IsSetitem(player->BagItem[0][j])) {
						count++;
					}
				}
				AddsetItemstats(player, player->BagItem[0][i]->ItemAddr->ReqStr, count);
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

			player->str += player->set_item.BackUpValue.Str;
			player->dex += player->set_item.BackUpValue.Dex;
			player->_Int_ += player->set_item.BackUpValue.__Int_;
			player->wis += player->set_item.BackUpValue.Wis;
			player->rec += player->set_item.BackUpValue.Rec;
			player->luc += player->set_item.BackUpValue.Luc;
		}

		if ((isHpChanged || player->set_item.IsSetted) && player->dwObjMap)
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

			//
			sendBaiscStatsChange(player);
		}
	}

	void __stdcall fun_afterResetStats(Pshaiya_player player) {
		player->set_item.IsSetted = false;
		Mainfun(player);
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
			SETITEMDATA stSetitem;
			ZeroMemory(szLine, sizeof(szLine));
			fgets(szLine, 1024, pFile);
			DWORD id;
			char szTemp[6][MAX_PATH] = { 0 };
			sscanf_s(szLine, "%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,],%d,%[^,]", &id, szTemp[5], MAX_PATH,
				&stSetitem.dwItems[0], szTemp[0], MAX_PATH,
				&stSetitem.dwItems[1], szTemp[1], MAX_PATH,
				&stSetitem.dwItems[2], szTemp[2], MAX_PATH,
				&stSetitem.dwItems[3], szTemp[3], MAX_PATH,
				&stSetitem.dwItems[4], szTemp[4], MAX_PATH);

			if (!id) {
				continue;
			}


			PSETITEMDATA psetItem = &(g_setitems[id - 1]);
			memcpy(psetItem->dwItems, stSetitem.dwItems, sizeof(stSetitem.dwItems));


			for (DWORD i = 0; i < 5; i++) {
				sscanf_s(szTemp[i], "%d|%d|%d|%d|%d|%d|%d|%d|%d",
					&psetItem->dwValue[i].Str,
					&psetItem->dwValue[i].Dex,
					&psetItem->dwValue[i].__Int_,
					&psetItem->dwValue[i].Wis,
					&psetItem->dwValue[i].Rec,
					&psetItem->dwValue[i].Luc,
					&psetItem->dwValue[i].Hp,
					&psetItem->dwValue[i].Mp,
					&psetItem->dwValue[i].Sp);


				LOGD << "setitem:" << id << szTemp[i] << psetItem->dwValue[i].Str <<
					psetItem->dwValue[i].Dex <<
					psetItem->dwValue[i].__Int_ <<
					psetItem->dwValue[i].Wis <<
					psetItem->dwValue[i].Rec <<
					psetItem->dwValue[i].Luc <<
					psetItem->dwValue[i].Hp <<
					psetItem->dwValue[i].Mp <<
					psetItem->dwValue[i].Sp;


				//如果这块没属性，就采用上一块的
				if (memcmp(&psetItem->dwValue[i], &temp, sizeof(SETITEMSTATS)) == 0 && i) {
					memcpy(&psetItem->dwValue[i], &psetItem->dwValue[i - 1], sizeof(SETITEMSTATS));
				}
			}
		}
		fclose(pFile);
		return true;
	}

	
	DWORD resetStatusCall = 0x0048f710;
	__declspec(naked) void Naked_afterResetStatus()
	{
		_asm
		{
			call resetStatusCall
			pushad
			MYASMCALL_1(fun_afterResetStats,ebp)
			popad
			jmp g_objAfterResetStatus.m_pRet
		}
	}

	__declspec(naked) void Naked()
	{
		_asm
		{
			pushad
			MYASMCALL_1(Mainfun, esi)
			popad
			sub esp, 0x8
			fild dword ptr[esi + 0x1220]
			jmp g_obj.m_pRet
		}
	}

	void sendPacketForEnableSetitem(void* p) {
		Pshaiya_player player = (Pshaiya_player)p;
		COMMONHEAD st;
		st.cmd = PacketOrder::SetItem_Enable;
		SendPacketToPlayer(player, &st, sizeof(st));
	}


	void start()
	{
		load_setItem();
		g_obj.Hook((PVOID)0x00460e40, Naked, 9);
		g_vecOnloginCallBack.push_back(sendPacketForEnableSetitem);
		g_objAfterResetStatus.Hook(PVOID(0x00473353), Naked_afterResetStatus, 5);
	}
}





