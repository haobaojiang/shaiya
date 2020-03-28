#include "stdafx.h"
#include "killtitle.h"




namespace killtitle
{

	//-------------------使用物品形式的
	CMyInlineHook g_obj;
	PLAYERTITLES st;
	std::map<DWORD,DWORD> g_mapKillTitleSkills;
	DWORD g_mode = 0;

	bool isKilltitleSkill(DWORD skillid) {

		auto iter = g_mapKillTitleSkills.find(skillid);

		return iter != g_mapKillTitleSkills.end() ? true : false;

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
			if (isKilltitleSkill(skillid)) {
				return pNext;
			}
			pNext = *PDWORD(pNext + 0x4);
		} while (1);
		return 0;
	}

	bool removeExitedKillTitleSkill(Pshaiya50_player player) {
		DWORD playerSkill = getLivingCostume((DWORD)player);
		if (playerSkill) {
			DWORD skillOrder = *PDWORD(playerSkill + 0x8);
			removeApplySkill((DWORD)player, skillOrder);
			return true;
		}
		return false;
	}



	void applykillTitleSkill(Pshaiya50_player player,DWORD skillid) {

		

		//获取技能对应的下标
		auto iter = g_mapKillTitleSkills.find(skillid);
		if (iter == g_mapKillTitleSkills.end())
			return;


		DWORD picIndex = iter->second;

		for (DWORD i = 0; i < _countof(st.playerImages); i++) {

			//己有的
			if (st.playerImages[i].charid == player->Charid) {
				st.playerImages[i].Inedx = picIndex;
				break;
			}


			//空位
			if (!st.playerImages[i].charid) {
				st.playerImages[i].charid = player->Charid;
				st.playerImages[i].Inedx = picIndex;
				break;
			}
		}
		send_packet_all(&st, sizeof(st));
	}


	void onLogin(PVOID p) {
		SendPacketToPlayer(p, &st, sizeof(st));
	}


	void __stdcall useSkill(PVOID player, DWORD  pSkill)
	{
		WORD Skillid = *PWORD(pSkill);
		if (isKilltitleSkill(Skillid)) {
			removeExitedKillTitleSkill((Pshaiya50_player)player);
			applykillTitleSkill((Pshaiya50_player)player, Skillid);
		}
	}


	//如果是称号药水，就不让它消耗
	bool __stdcall preUseItemSkill(Pshaiya50_player player, DWORD  pSkill) {
		return isKilltitleSkill(*PWORD(pSkill));
	}


	DWORD addr = 0x00465e2d;
	__declspec(naked) void Naked()
	{
		_asm
		{
			pushad
			MYASMCALL_2(preUseItemSkill, ebp, esi)
			test al,al
			popad
			je _Org
			jmp addr

			_Org:
			mov ecx,dword ptr [esp+0xc4]
			jmp g_obj.m_pRet
		}
	}



	//---------------------配置文件形式的

	bool isTime() {

		DWORD  static nextTime = GetTickCount() + 15 * 1000;
		DWORD curTime = GetTickCount();
		if (nextTime > curTime) {
			return false;
		}
		else {
			nextTime = curTime+ 15 * 1000;
			return true;
		}
	}



	void loadConfig() {


		//技能buff模式的
		if (g_mode == 2) {
			st.imageCount = GetPrivateProfileIntA("killtitle", "imagecount", 0, g_strFilePath);
			st.x = GetPrivateProfileIntA("killtitle", "x", 0, g_strFilePath);
			st.y = GetPrivateProfileIntA("killtitle", "y", 0, g_strFilePath);
			return;
		}


		//兼容配置文件读取模式的
		ZeroMemory(&st, sizeof(st));
		st.cmd = PacketOrder::additional_title;
		st.x = GetPrivateProfileIntA("killtitle", "x", 0, g_strFilePath);
		st.y = GetPrivateProfileIntA("killtitle", "y", 0, g_strFilePath);
		st.imageCount = GetPrivateProfileIntA("killtitle", "imagecount", 0, g_strFilePath);

		DWORD i = 0;
		for (; i < _countof(st.playerImages); i++)
		{
			char key[50] = { 0 };
			char strTemp[MAX_PATH] = { 0 };
			sprintf(key, "%d", i + 1);
			GetPrivateProfileStringA("killtitle", key, "", strTemp, MAX_PATH, g_strFilePath);

			if (strTemp[0] == '\0')
				break;

			sscanf(strTemp, "%d,%d", &st.playerImages[i].charid, &st.playerImages[i].Inedx);
		}

		if (i) {
			send_packet_all(&st, sizeof(st));
		}

	}



	void Loop()
	{

		if (isTime()) 
		{	
			loadConfig();
		}
	}





	void Start()
	{
		g_vecLoopFunction.push_back(Loop);
		g_mode = GetPrivateProfileInt(L"killtitle", L"mode", 1, g_szFilePath);
	//	loadConfig();

		if (g_mode == 2) {
		//	g_obj.Hook((void*)0x004658e7, Naked, 7);
			g_UseSkillCallBack.push_back(useSkill);
			g_vecOnloginCallBack.push_back(onLogin);

			for (DWORD i = 0; i < st.imageCount; i++) {
				char temp[MAX_PATH] = { 0 };
				DWORD skillid = 0;
				DWORD imageIndex = 0;
				char key[50] = { 0 };
				sprintf(key, "bufficon_%d", i + 1);
				GetPrivateProfileStringA("killtitle", key, "", temp, MAX_PATH, g_strFilePath);

				sscanf(temp, "%d,%d", &skillid, &imageIndex);
				if (skillid) {
					LOGD << "killtitle-->skillid:" << skillid << ",imageIndex:" << imageIndex;
					g_mapKillTitleSkills.insert(std::pair<DWORD, DWORD>(skillid, imageIndex));
				}
			}
		}
		
	}
}




