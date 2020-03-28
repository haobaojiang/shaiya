#include "stdafx.h"
#include "FantasyPvp.h"








namespace FantasyPvp
{
	bool g_bEnableFantasyPvp = false;
	WORD g_wFantasyPvpMap = 0;
	WORD g_wFantasyPvpIndex = 0;
	CMyInlineHook obj_GetFantasyPvp, obj_SetObj, obj_Battle, obj_ProcessFaction,obj_Attack,obj_Pos;
	vector<DWORD> g_vecPlayers;
	DWORD g_dwObjSrc = 0;         //Դmap����Ϣ,��map��ֱ�ӿ��Ի�ȡ�õ�
	DWORD g_dwObjDst = 0;         //Ҫ�ӵ�һ����������ȡ
	BYTE g_byBattleMap = 0;      //
	bool g_IsFantasys = false;
	CRITICAL_SECTION g_cs;
	DWORD dwTemp = 0x00430260;
	DWORD __stdcall fun_getMapInfo(DWORD );
	bool __stdcall fun_CheckBattle(Pshaiya_player, Pshaiya_player);
	bool __stdcall AttackAble(Pshaiya_player, Pshaiya_player);
	BYTE __stdcall GetFaction(Pshaiya_player, Pshaiya_player);
	DWORD __stdcall fun_SetMap(Pshaiya_player, DWORD);
	void __stdcall funChangePos(Pshaiya_player);
	PVOID g_pfunChangePos = funChangePos;
	PVOID g_pfunGetFaction = GetFaction;
	PVOID g_pfunCheckBattle = fun_CheckBattle;	
	PVOID g_pfunAttackAble = AttackAble;

	PVOID g_pfunSetMap = fun_SetMap;

	Pshaiya_party g_pPartyA = NULL;
	Pshaiya_party g_pPartyB = NULL;
	HANDLE g_Event = NULL;

	typedef struct _BattleInfo
	{
		Pshaiya_party partyA;
		Pshaiya_party partyB;
	}BattleInfo,*PBattleInfo;

	shaiya_pos g_lightPos = { 0 };
	shaiya_pos g_FuryPos = { 0 };


	vector<BattleInfo> g_vecBattleInfo;

	void __stdcall funChangePos(Pshaiya_player player)
	{
		Pshaiya_party party = player->party;
		if (!party)
			return;

		if (party != g_pPartyA && party != g_pPartyB)
			return;

		player->Country ? player->Parepos = g_FuryPos : player->Parepos = g_lightPos;
	}

	bool __stdcall AttackAble(Pshaiya_player Attacker, Pshaiya_player Target)
	{
		bool bRet = (Attacker->Country != Target->Country ? false : true); //ԭ���,��Ӫ�������
		if (Attacker->Map == g_byBattleMap&&g_byBattleMap)
		{
			bRet = (Attacker->party != Target->party ? false : true);      //���鲻�����
		}
		return bRet;
	}

	//��b��������Ӫ
	BYTE __stdcall GetFaction(Pshaiya_player playerA, Pshaiya_player playerB)
	{
		BYTE bRet = playerB->Country;
		if (playerB->Map == g_byBattleMap &&playerB->party!=playerA->party&&g_byBattleMap) //�����ͼ��ȵ��ǲ���ͬһ������
		{
			bRet = ~(playerA->Country);      //ȡ����Ӫ
		}
		return bRet;
	}
	
	bool __stdcall fun_CheckBattle(Pshaiya_player playerA,Pshaiya_player playerB)
	{
		bool bRet = false;
		Pshaiya_party partyA = playerA->party;
		Pshaiya_party partyB = playerB->party;
		
		//���¶���,׼�����͹�ȥ
		if (partyA && partyB && playerA->Map == 40 &&  //������40��ͼ
			playerA == GetLeader(partyA) &&           //�����Ƕӳ�
			playerB == GetLeader(partyB)
			)
		{
			EnterCriticalSection(&g_cs);
			BattleInfo stc;
			stc.partyA = partyA;
			stc.partyB = partyB;
			g_vecBattleInfo.push_back(stc);
			LeaveCriticalSection(&g_cs);
			bRet = true;
		}
		return bRet;
	}


	DWORD __stdcall fun_SetMap(Pshaiya_player Player, DWORD dwOldObjMap)
	{
		DWORD dwRet = dwOldObjMap;
		if (!g_pPartyA || !g_pPartyB)
			return dwRet;

		if (Player->party == g_pPartyA || Player->party == g_pPartyB)
		{
			//���û��˵���ǵ�һ��
			if (g_dwObjDst)
				dwRet = g_dwObjDst;
			Player->dwObjMap = dwRet;
			SetEvent(g_Event);
		}
			
		return dwRet;
	}




	//ȡ����ͼ��Ϣ,������new������object
	DWORD __stdcall fun_getMapInfo(DWORD dwObjMap)
	{

		//ȡ��������ȫ�ֵ�ͼ��һ����Ϣ,�����ǵ�ͼobject
		DWORD dwRet = 0;
		DWORD dwCall = 0x00430260;
		_asm
		{
			movzx eax, g_wFantasyPvpIndex
				mov ecx, dwObjMap         //0xe0
				mov ecx, dword ptr[ecx + 0x28]
				call dwCall
				mov dwRet, eax
		}
		return dwRet;
	}

	
	DWORD dwFalse = 0x0048C902;
	__declspec(naked) void  Naked_Battle()
	{
		_asm
		{
			pushad
			push esi
			push ebp
			call g_pfunCheckBattle
			cmp al,0x1
			popad
			je  __false
			jmp obj_Battle.m_pRet
		__false:
			jmp dwFalse
		}
	}
	__declspec(naked) void  Naked_GetMapInfo()
	{
		_asm
		{
			cmp eax, 0xff
				jne _orginal
				mov eax,g_dwObjSrc
				jmp obj_GetFantasyPvp.m_pRet

			_orginal :
			call dwTemp
				jmp obj_GetFantasyPvp.m_pRet
		}
	}

	__declspec(naked) void  Naked_SetNewMap()
	{
		_asm
		{
			push ecx
				push edx
				push ebx
				push esp
				push ebp
				push esi
				push edi

				push eax
				push esi
				call g_pfunSetMap

				pop edi
				pop esi
				pop ebp
				pop esp
				pop ebx
				pop edx
				pop ecx
				lea ebp, dword ptr[esi + 0x6284]
				jmp obj_SetObj.m_pRet
		}
	}

	__declspec(naked) void  Naked_Faction()
	{
		_asm
		{
			push eax
				push edx
				push ebx
				push esp
				push ebp
				push esi
				push edi

				
				push ebx               //��Ҫ��ebx����faction
				push ebp
				call g_pfunGetFaction
				movzx ecx,al

				pop edi
				pop esi
				pop ebp
				pop esp
				pop ebx
				pop edx
				pop eax
				cmp dword ptr[ebx + 00001444], 01
				jmp obj_ProcessFaction.m_pRet
		}
	}
	

	

	__declspec(naked) void  Naked_ChangePos()
	{
		_asm{
			fstp dword ptr  [ebp + 0x58C8]
			   pushad
			   push ebp
			   call g_pfunChangePos
			   popad
			jmp obj_Pos.m_pRet
		}

	}


	__declspec(naked) void  Naked_Attack()
	{
		_asm{
			pushad
				push edx
				push eax
				call g_pfunAttackAble
				cmp al,0x1             //�ѽ�������zf��־λ����
				popad
				sete al
				pop esi
				retn
		}
		
	}
	

	//�̴߳���battle�Ľӿ�
	DWORD WINAPI ThreadProc(_In_  LPVOID lpParameter)
	{
		Sleep(4000);

		//������map 47��ͼ�Ĵ�����ҵ�0xe0����
		g_dwObjSrc = fun_getMapInfo(*PDWORD(*PDWORD(0x010A201c) + g_wFantasyPvpMap * 0x98 + 0x4));

		{
			BYTE byCodes[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
			MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00476789, byCodes, sizeof(byCodes));//����Ҫǿ����
		}

		while (1)
		{
			//����pvp��Ϣ
			EnterCriticalSection(&g_cs);
			if (g_vecBattleInfo.size())
			{
				vector<BattleInfo>::iterator iter = g_vecBattleInfo.begin();
				g_pPartyA = (*iter).partyA;
				g_pPartyB = (*iter).partyB;
				official_enterFirePort packets;
				packets.byMapIndex = 0xff;
				g_dwObjDst = NULL;

				cmd_Battle packet_BattleA, packet_BattleB;
				ZeroMemory(&packet_BattleA, sizeof(packet_BattleA));
				ZeroMemory(&packet_BattleB, sizeof(packet_BattleB));
				
				for (DWORD i = 0; i < g_pPartyA->Count; i++)
				{
					Pshaiya_player _player = Pshaiya_player(g_pPartyA->member[i].player);
					if (_player)
					{
						//���͵�ͼ
						server_process_packet((DWORD)_player, &packets);
						if (WAIT_TIMEOUT == WaitForSingleObject(g_Event, 10000))
							SetEvent(g_Event);
						ResetEvent(g_Event);

						if (!g_dwObjDst)
						{
							g_dwObjDst    = _player->dwObjMap;   //��һ���˴�����Ҫ�ѵ�ͼ�ó���
							g_byBattleMap = _player->Map;
						}
					}
				}

				for (DWORD i = 0; i < g_pPartyB->Count; i++)
				{
					Pshaiya_player _player = Pshaiya_player(g_pPartyB->member[i].player);
					if (_player)
					{

						//���͵�ͼ
						server_process_packet((DWORD)_player, &packets);
						if (WAIT_TIMEOUT == WaitForSingleObject(g_Event, 10000))
							SetEvent(g_Event);
						ResetEvent(g_Event);					
					}
				}	
				g_pPartyA = NULL;
				g_pPartyB = NULL;
				g_dwObjDst = NULL;
				g_byBattleMap = NULL;
				g_vecBattleInfo.erase(iter);
			}
			LeaveCriticalSection(&g_cs);
			Sleep(500);
		}
		return 0;
	}



	void Start()
	{


		InitializeCriticalSection(&g_cs);
		g_Event=::CreateEvent(0, TRUE, 0, 0);

		g_wFantasyPvpMap = GetPrivateProfileInt(L"PrivatePvp", L"map", 0, g_szFilePath);
		g_wFantasyPvpIndex = GetPrivateProfileInt(L"PrivatePvp", L"index", 0, g_szFilePath);

		WCHAR szTemp[MAX_PATH] = { 0 };
		GetPrivateProfileString(L"PrivatePvp", L"pos1", L"", szTemp, MAX_PATH, g_szFilePath);
		swscanf_s(szTemp, L"%f,%f,%f",&g_lightPos.x,&g_lightPos.y,&g_lightPos.z);

		GetPrivateProfileString(L"PrivatePvp", L"pos2", L"", szTemp, MAX_PATH, g_szFilePath);
		swscanf_s(szTemp, L"%f,%f,%f", &g_FuryPos.x, &g_FuryPos.y, &g_FuryPos.z);

		
		//battle�ӿ�����
		obj_Battle.Hook((PVOID)0x0048c3da, Naked_Battle, 6);

		//���͹�����
		obj_GetFantasyPvp.Hook((PVOID)0x0047675d, Naked_GetMapInfo, 5);
		obj_SetObj.Hook((PVOID)0x0041369e, Naked_SetNewMap, 6);

		//��ȡ��Ӫ
		obj_ProcessFaction.Hook((PVOID)0x00426af7, Naked_Faction, 7);

		//����Ŀ��
		obj_Attack.Hook((PVOID)0x00457e27, Naked_Attack, 6);

		//�ı䴫�͹�ȥ��ͼ������
		obj_Pos.Hook((PVOID)0x00476ba8, Naked_ChangePos, 6);
		


		::CreateThread(0, 0, ThreadProc, 0, 0, 0);
	}
}

