#include "stdafx.h"

#include "hijack.h"
#include "config.hpp"
#include "onLogin.hpp"
#include "namecolor.hpp"
#include "skillCutting.hpp"
#include "killsRanking.hpp"
#include "combine.hpp"
#include "encryptReadChar.h"
#include "enhanceDelay.hpp"
#include "instantMount.hpp"
#include "enhanceack.hpp"
#include "randomRes.h"
#include "party_hook.hpp"
#include "fireport.hpp"
#include "customized_1.hpp"
#include "banneditem.hpp"
#include "get_players.hpp"
#include "ShareKills.h"
#include "customized_2.hpp"
#include "itemmap.hpp"
#include "linking.hpp"
#include "custom_packet.hpp"
#include "customized_3.hpp"
#include "newmount.hpp"
#include "naked_skill.h"
#include "player_title.hpp"
#include "mob_spawn_notice.hpp"
#include "recrune.hpp"
#include "singleparty.h"


bool IsInjectAble() {

	WCHAR szProcessName[MAX_PATH] = { 0 };
	GetModuleBaseName(GetCurrentProcess(), NULL, szProcessName, sizeof(szProcessName));
	if (wcscmp(szProcessName, L"ps_game.exe") == 0) {
		return true;
	}
	return false;
}


BOOL InitLogging() {


	auto path = ShaiyaUtility::GetCurrentExePathW();
	path += L"\\shaiyaLog";

	// folder
	if (!CreateDirectory(path.c_str(), nullptr)) {
		// something went wrong
		if (GetLastError() == ERROR_PATH_NOT_FOUND) {
			return FALSE;
		}
	}

	// log file
	path += L"\\ep6.log";
	static plog::RollingFileAppender<plog::TxtFormatter, plog::NativeEOLConverter<> > fileAppender(path.c_str(), 8000, 3); // Create an appender with NativeEOLConverter.
	plog::init(plog::debug, &fileAppender);
	return TRUE;
}



BOOL InitAuth() {
	return TRUE;
}


BOOL InitGame() {


	onLogin::Start();

	/*
	{
		DWORD dwSize = sizeof(shaiya50_player);
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0040aa69, &dwSize, 4);
	}
	*/

	// GetPlayers::Start();

	return TRUE;
}

/*
void getMaNames() {

	for (DWORD i = 0; i < 200; i++) {
		char Key[50] = { 0 };
		sprintf(Key, "%d", i);
		GetPrivateProfileStringA("mapName", Key, "", g_MapName[i], MAX_PATH, g_strFilePath);
	}
}*/

/*
bool getHookInfo() {


	MyAsioClient objNet("112.74.190.107", 31501);
	DWORD tryTime = 3;


#ifndef _DEBUG
	VMProtectBegin("getHookInfo");
#endif

	while (tryTime)
	{

		objNet.setRemote("112.74.190.107", 31501);
		if (objNet.connect()) {
			break;
		}

		objNet.setRemote("23.94.245.154", 31501);
		if (objNet.connect()) {
			break;
		}

		objNet.setRemote("107.174.70.98", 31501);
		if (objNet.connect()) {
			break;
		}

		tryTime--;
		if (!tryTime) {
			return false;
		}
	}
#ifndef _DEBUG
	VMProtectEnd();
#endif



	Sleep(1000);
	return objNet.read(PCHAR(&g_hookAddr), 4096) ? true : false;



}
*/






void subProcess(void* p) {

	/*
	InitializeCriticalSection(&g_csHutingplayers);
	getHookInfo();

	purchaseDelay::Start();
	onLogin::start();
	timeoutLogger::Start();

	SingleParty::start();
	AutoMob::Start();
	chatFilter::Start();
	firePort::start();
	FFA::Start();
	autoSummon::Start();
	randomRes::Start();
	Customconsole::Start();

	skillCallBack::Start();
	//deadlockFix::Start();

	getMaNames();

	//nprotectoff
	{
		*PBYTE(0x541F7C) = 0x0;
	}


	if (GetPrivateProfileInt(L"removeBothMobDelay", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "removeBothMobDelay start!";
		BYTE temp[] = { 0x90,0x90 };
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00421cf9), temp, sizeof(temp));
		MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x00421d44), temp, sizeof(temp));
	}

	if (GetPrivateProfileInt(L"preventIllegalItemTrade", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "preventIllegalItemTrade start!";
		preventItemDrp::Start();
	}

	if (GetPrivateProfileInt(L"autoExp", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "autoExp start!";
		AutoExp::Start();
	}

	if (GetPrivateProfileInt(L"autopoint", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "autopoint start!";
		AutoPoint::Start();
	}

	if (GetPrivateProfileInt(L"BaseSkill", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "BaseSkill start!";
		BaseSkill::Start();
	}

	if (GetPrivateProfileInt(L"ShareKills", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "ShareKills start!";
		ShareKills::Start();
	}

	if (GetPrivateProfileInt(L"AutoKills", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "AutoKills start!";
		AutoKills::Start();
	}

	if (GetPrivateProfileInt(L"namecolor", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "namecolor start!";
		NameColor::Start();
	}

	if (GetPrivateProfileInt(L"tradecolor", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "tradecolor start!";
		TradeColor::Start();
	}

	if (GetPrivateProfileInt(L"svrMsgColor", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "svrMsgColor start!";
		svrMsgColor::Start();
	}



	if (GetPrivateProfileInt(L"costume", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "costume start!";
		costume::Start();
	}

	if (GetPrivateProfileInt(L"recRune", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "recRune start!";
		RecRune::start();
	}

	if (GetPrivateProfileInt(L"AntiStatPad", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "AntiStatPad start!";
		AntiStatPad::Start();
	}

	if (GetPrivateProfileInt(L"setitem", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "setitem start!";
		setItem::Start();
	}

	if (GetPrivateProfileInt(L"itemformap", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "itemformap start!";
		itemForMap::start();
	}

	if (GetPrivateProfileInt(L"vipCrossTradeChannel", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "vipCrossTradeChannel start!";
		vipCrossTrade::Start();
	}

	if (GetPrivateProfileInt(L"antiHacker", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "antiHacker start!";
		antiHacker::Start();
	}


	if (GetPrivateProfileInt(L"ffpv2", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "ffpv2 start!";
		FFPV2::Start();
	}

	if (GetPrivateProfileInt(L"antiDupe", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "antiDupe start!";
		antiDupe::Start();
	}


	if (GetPrivateProfileInt(L"CrossFactionTrade", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "CrossFactionTrade start!";
		CrossFactionTrade::Start();
	}

	if (GetPrivateProfileInt(L"killtitle", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "killtitle start!";
		killtitle::Start();
	}


	if (GetPrivateProfileInt(L"onLineInfToClient", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "onLineInfToClient start!";
		onLineInfToClient::Start();
	}

	if (GetPrivateProfileInt(L"dropNotice", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "dropNotice start!";
		dropNotice::Start();
	}


	if (GetPrivateProfileInt(L"Questlimited", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "Questlimited start!";
		QuestLimited::Start();
	}


	if (GetPrivateProfileInt(L"dailiMap", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "dailiMap start!";
		dailiMap::Start();
	}

	if (GetPrivateProfileInt(L"combine", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "combine start!";
		combine::Start();
	}

	if (GetPrivateProfileInt(L"playerEffects", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "playerEffects start!";
		playerEffects::Start();
	}

	if (GetPrivateProfileInt(L"Hunting", L"enable", 0, g_szFilePath) == 1) {
		LOGD << "Hunting start!";
		Hunting::Start();
	}






	FFP::Start();
	enchantDelay::start();

	Sleep(5000);
	while (1)
	{
		for (DWORD i = 0; i < g_vecLoopFunction.size(); i++)
		{
			g_vecLoopFunction[i]();
		}
		Sleep(1 * 1000);
	}
	return;

	*/
}
#include "../utility/file.h"
/*
#ifndef _DEBUG
#include "VMProtectSDK.h"
bool check_vmp_license() {


	VMProtectBegin(__FUNCTION__);
	auto path = ShaiyaUtility::GetCurrentExePathW();
	path += L"\\license";

	std::vector<BYTE> data;
	auto hr = Utility::File::ReadFileData(path.c_str(), &data);
	LOGD << "hr:" << hr;

	VMProtectSetSerialNumber((char*)(data.data()));

	int state = VMProtectGetSerialNumberState();
	if (state != 0) {
		return false;
	}
	return true;
	VMProtectEnd();

}
#endif
*/

void Main()
{



	/*
#ifndef  _DEBUG
	VMProtectBegin(__FUNCTION__);
#endif
	*/


	/*
#ifndef  _DEBUG
	if (check_vmp_license()) {
#endif
*/
	




		LOGD << "after InitLogging";

		if (!GameConfiguration::Init()) {
			LOGE << "failed to init configuration";
			return;
		}

		if (!InitGame()) {
			LOGE << "failed to init game";
			return;
		}

		LOGD << "after InitGame";


		
		
		SingleParty::start();
		getPlayer::Start();
		NameColor::Start();
		SkillCutting::Start();
		KillsRanking::Start();
		Combine::Start();
		EncryptReadChar::Start();
		EnhanceDelay::start();
		InstantMount::Start();
		enhanceAck::Start();
		randomRes::Start();
		part_hook::start();
		fire_port::start();
		customized_1::start();
		banned_item::start();
		StartShareKill();
		Customized2::start(NameColor::applyPlayerColor, NameColor::removePlayerColor);
		itemForMap::start();
		linking::start();
		CustomPacket::start();
		customized_3::start();
		NewMount::start();
		NakedSkill::start();
		PlayerTitle::Start();
		MobSpawnNotice::start();
		RecRune::start();
	
		/*
#ifndef  _DEBUG
	}

	else {
		LOGD << "invalid license";
		ExitProcess(0);
	}
#endif
	*/

	/*
#ifndef  _DEBUG
	VMProtectEnd();
#endif
	*/
}



ShaiyaUtility::CMyInlineHook obj1;

DWORD orgAddr = 0;


bool __stdcall Filter(void* Player, PBYTE pPackets)
{
	char* strChat = (char*)&pPackets[3];
	return strstr(strChat, "<<999c01") == NULL;
}

DWORD falledAddr = 0x0047FC57;
__declspec(naked) void  Naked_ChatFilter()
{
	_asm
	{
		pushad
		MYASMCALL_2(Filter, ebp, ebx)
		test al, al
		popad
		jne _Org
		jmp falledAddr

		_Org :
		jmp orgAddr
	}
}


void mainFun(void*)
{
	Sleep(6000);
	orgAddr = (DWORD)LoadLibrary(L"dbghelp.dll");
	orgAddr += 0x97e0;
	obj1.Hook((PVOID)0x0047f4ba, Naked_ChatFilter, 7);
}


BOOL APIENTRY DllMain(
	HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{ 
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		
		if (!IsInjectAble()) {
			break;
		}
		
		if (!InitLogging()) {
			break;
		}
		Main();
		
	//	_beginthread(mainFun, 0, 0);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}








