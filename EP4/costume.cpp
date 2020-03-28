#include "stdafx.h"
#include "costume.h"


namespace costume {


	CMyInlineHook g_objUseSkill;
	CMyInlineHook g_objUserShape;
	CMyInlineHook g_objUserShape2;
	CMyInlineHook g_objUserShape3;
	CMyInlineHook g_objUserShape4;
	CMyInlineHook g_objUserShape5;


	typedef struct {
		DWORD skillid;
		DWORD items[5];
	}costumeGroup, *PcostumeGroup;


	vector<DWORD> g_vecPlayers;
	CRITICAL_SECTION g_cs;


	
	vector<costumeGroup> g_costumegroups;

	void SendUserShape(DWORD player) {

		WORD map = Pshaiya50_player(player)->Map;

		if (g_ffp1&&g_ffp1->isFFAMap(map)) {
			return;
		}

		DWORD dwCall = 0x00482ad0;
		_asm {
			push player
			call dwCall
		}
	}


	PcostumeGroup getCostumeGroup(WORD skillid) {
		for (DWORD i=0;i<g_costumegroups.size();i++) {
			if (g_costumegroups[i].skillid == skillid) {
				return &g_costumegroups[i];
			}
		}
		return NULL;
	}


	bool IsCostume(WORD skillid) {
		return getCostumeGroup(skillid) ? true : false;
	}



 	void costumeChange(Pold_Meet packet, PcostumeGroup group) {

		for (DWORD i = 0; i < 5; i++) {
			packet->_item[i].type = group->items[i] / 1000;
			packet->_item[i]._typeid = group->items[i] % 1000;
		}
 
 	}



	void __stdcall refreshPlayerShape(DWORD player) {
		EnterCriticalSection(&g_cs);
		g_vecPlayers.push_back(player);
		LeaveCriticalSection(&g_cs);
	}



	DWORD getLivingCostume(DWORD player) {
		DWORD skillHead = *PDWORD(player + 0xa94);
		DWORD pNext = *PDWORD(skillHead + 0x4);
		do
		{
			if (pNext == skillHead) {
				break;
			}

			DWORD skill = *PDWORD(pNext + 0x30);
			WORD skillid = *PWORD(skill);
			if (IsCostume(skillid)) {
				return pNext;
			}
			pNext = *PDWORD(pNext + 0x4);
		} while (1);
		return 0;
	}



	bool removeExistedCostume(DWORD player) {
		DWORD playerSkill = getLivingCostume(player);
		if (playerSkill) {
			DWORD skillOrder = *PDWORD(playerSkill + 0x8);
			removeApplySkill(player, skillOrder);
			return true;
		}
		return false;
	}



	void __stdcall useSkill(PVOID p, DWORD  pSkill)
	{
		DWORD player = (DWORD)p;



		WORD Skillid = *PWORD(pSkill);	
		if (IsCostume(Skillid)) {
			//因为删除costume那里已经会刷新外观
			if (!removeExistedCostume(player)) {
				refreshPlayerShape(player);
			}		
		}
	}



	void __stdcall userShape(PVOID player, PVOID p) {
		Pold_Meet packet = (Pold_Meet)p;

		DWORD playerSkill = getLivingCostume((DWORD)player);
		if (playerSkill) {
			DWORD skill = *PDWORD(playerSkill + 0x30);
			WORD skillid = *PWORD(skill);
			PcostumeGroup group = getCostumeGroup(skillid);
			if (group) {
				costumeChange(packet, group);
			}
		}
	}

	void __stdcall CheckCostumeExpired(DWORD player,DWORD skill) {
		WORD skillid = *PWORD(skill);
		if (IsCostume(skillid)) {
			refreshPlayerShape(player);
		}
	}


	bool __stdcall IsSendEquipmentAble(DWORD player,DWORD slot) {
		if (slot >= 5) {
			return true;
		}
		return getLivingCostume(player) ? false : true;
	}


	__declspec(naked) void Naked_useSkill()
	{
		_asm
		{
			push 0x1
			push edi
			mov esi,ebx
			pushad
			MYASMCALL_2(useSkill,edi,esi)
			popad
			jmp g_objUseSkill.m_pRet
		}
	}


	__declspec(naked) void Naked_userShape()
	{
		_asm
		{
			mov ecx,dword ptr [ebp+0xe4]
			pushad
			MYASMCALL_2(userShape, ebp, edx)
			popad
			jmp g_objUserShape.m_pRet
		}
	}


	DWORD dwcall2 = 0x004D4D20;

	__declspec(naked) void Naked_userShape2()
	{
		_asm
		{
			pushad
			mov eax,dword ptr [esp+0x20]  //packet
			MYASMCALL_2(userShape,ebx,eax)
			popad
			call dwcall2
			jmp g_objUserShape2.m_pRet
		}
	}

	DWORD dwCall3 = 0x0044b8a0;
	__declspec(naked) void Naked_userShape3()
	{
		_asm
		{
			pushad
			MYASMCALL_1(refreshPlayerShape,eax)
			popad
			call dwCall3
			jmp g_objUserShape3.m_pRet
		}
	}
	
	


	__declspec(naked) void Naked_userShape4()
	{
		_asm
		{
			pushad
			MYASMCALL_2(CheckCostumeExpired, ecx,edx)
			popad
			push ebp
			mov ebp,esp
			and esp,0xfffffff8
			jmp g_objUserShape4.m_pRet
		}
	}


	__declspec(naked) void Naked_userShape5()
	{
		_asm
		{

			pushad
			MYASMCALL_2(IsSendEquipmentAble,esi,ebx)
			cmp al,0x1
			popad
			je _Org
			retn

			_Org :
			sub esp, 0x14
				mov eax, 0x0054059c
				mov eax,dword ptr [eax]
				jmp g_objUserShape5.m_pRet
		}
	}
	
	
	
	void ThreadProc(_In_ LPVOID lpParameter) {

		while (1)
		{
			EnterCriticalSection(&g_cs);
			if (g_vecPlayers.size()) {
				Sleep(1000);
				SendUserShape(g_vecPlayers[0]);
				g_vecPlayers.erase(g_vecPlayers.begin());
			}	
			LeaveCriticalSection(&g_cs);

			Sleep(1000);
		}
	}


	void Start()
	{
		for (WORD i = 0; i < 200; i++) {
			WCHAR key[50] = { 0 };
			WCHAR szReturn[MAX_PATH] = { 0 };
			swprintf_s(key, L"%d", i + 1);
			GetPrivateProfileString(L"costume", key, 0, szReturn, MAX_PATH, g_szFilePath);
			if (g_szFilePath[0] == L'\0') {
				break;
			}
			costumeGroup st = { 0 };
			swscanf(szReturn, L"%d,%d,%d,%d,%d,%d", &st.skillid, &st.items[0], &st.items[1], &st.items[2], &st.items[3], &st.items[4]);
			if (st.skillid&&st.items[0] && st.items[1] && st.items[2] && st.items[3] && st.items[4]) {
				g_costumegroups.push_back(st);
			}
		}

		g_userMeetCallBack.push_back(userShape);
		g_UseSkillCallBack.push_back(useSkill);


		g_objUserShape.Hook(PVOID(0x00482e75), Naked_userShape, 6);//发送外观时判断是否时装
		g_objUserShape3.Hook(PVOID(0x00466f67), Naked_userShape3, 5);//过图时
		g_objUserShape4.Hook(PVOID(0x00485130), Naked_userShape4, 6);//删除技能的时候 
		g_objUserShape5.Hook(PVOID(0x004839e0), Naked_userShape5, 8);//切换装备时


		InitializeCriticalSection(&g_cs);
		_beginthread(ThreadProc, 0, 0);
	}
}

