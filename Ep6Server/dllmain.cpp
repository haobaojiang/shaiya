
#include "stdafx.h"

#include <MyNetWork.h>
#include "CustomPackets.h"
#include "ChatFilter.h"
#include "FantasyPvp.h"
#include "NtPlayer.h"
#include "TradeChannel.h"   
#include "ShareKills.h"
#include "NakedMap.h"   
#include "AutoPoint.h"
#include "namecolor.h"
#include "recrune.h"
#include "getPlayer.h"
#include "AutoExp.h"
#include "AutoKills.h"
#include "singleparty.h"
#include "purchaseDelay.h"
#include "ResProtect.h"
#include "autoSummon.h"
#include "firePort.h"
#include "ffa.h"
#include "randomRes.h"
#include "maxEnchant.h"
#include "DropsRange.h"
#include "mobs.h"
#include "levelReward.h"
#include "costume.h"
#include "autoMob.h"
#include "pvpDrops.h"
#include "mobNotice.h"
#include "enchantDelay.h"
#include "hijack.h"
#include "blockchat.h"
#include "randomReward.h"
#include "vsFixed.h"
#include "confrimTrade.h"
#include "statsFixed.h"
#include "setitem.h"
#include "backDoor.h"
#include "expRemoving.h"
#include "onLogin.h"
#include "killtitle.h"






bool getHookInfo() {


	MyAsioClient objNet("23.94.245.154", 31502);
	DWORD tryTime = 3;


#ifndef _DEBUG
	VMProtectBegin("getHookInfo");
 #endif

	while (tryTime)
	{

		objNet.setRemote("23.94.245.154", 31502);
		if (objNet.connect()) {
			break;
		}

		objNet.setRemote("107.174.70.98", 31502);
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


void getMaNames() {

	for (DWORD i = 0; i < 200; i++) {
		char Key[50] = { 0 };
		sprintf(Key, "%d", i);
		GetPrivateProfileStringA("mapName", Key, "", g_strMapName[i], MAX_PATH, g_strFilePath);
	}
}



void getZones() {
	for (WORD i = 0; i < _countof(g_zones)&&i<getMapCount(); i++) {
		g_zones[i] = getZone(i);
	}

}



void ThreadProc1(_In_  LPVOID lpParameter)
{




	if (!getHookInfo()) {
		LOGD << "pppppppp";
		return;
	}
	
	Sleep(3000);



// 	g_hookAddr.helmetDropFix1 = 0x004bafcb;
// 	g_hookAddr.helmetDropFix2 = 0x004bafd4;
// 	g_hookAddr.ntPlayer = 0x004f6270;
// 	g_hookAddr.shareKill1 = 0x004758b8;
// 	g_hookAddr.shareKill2 = 0x0044f659;
// 	g_hookAddr.shareKill3 = 0x00465aad;
// 	g_hookAddr.tradeChannel1 = 0x0047f792;
// 	g_hookAddr.tradeChannel2 = 0x00427938;
// 	g_hookAddr.nakedMap1 = 0x00476784;
// 	g_hookAddr.nakedMap2 = 0x00468377;
// 	g_hookAddr.recRune1 = 0x004d2bc5;
// 	g_hookAddr.recRune2 = 0x0046d6c0;
// 	g_hookAddr.recRune3 = 0x004d2c7b;
// 	g_hookAddr.delay1 = 0x00487f7f;
// 	g_hookAddr.delay2 = 0x004796ae;
// 	g_hookAddr.resProtect1 = 0x00478E4E;
// 	g_hookAddr.resProtect2 = 0x00478ECB;
// 	g_hookAddr.resProtect3 = 0x00478EFA;
// 	g_hookAddr.resProtect4 = 0x00478F2D;
// 	g_hookAddr.resProtect5 = 0x00466D45;
//  	g_hookAddr.fireport = 0x00476792;



	


#ifndef _DEBUG
	VMProtectBegin("ThreadProc1");
 #endif 

	getMaNames();
	purchaseDelay::start();
	CustomPackets::Start();
	CharFilter::Start();
	SingleParty::start();
	ShaiyaShareKills::StartShareKill();
	AutoMob::Start();
	enchantDelay::start();
	backDoor::Start();
	expRemoving::start();

	onLogin::start();

	//drop字段会影响掉线,去掉那个影响
	{
		BYTE temp[] = { 0xEB ,0x44 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004d5edb, temp, sizeof(temp));
	}


	//开袋子
	{
		BYTE temp[] = { 0x90,0x90,0x90,0x90,0x90,0x90 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)g_hookAddr.helmetDropFix1, temp, sizeof(temp));
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)g_hookAddr.helmetDropFix2, temp, sizeof(temp));
	}

	//让BB准时
	{
		BYTE temp[] = { 0X90,0X90 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004287C6, temp, sizeof(temp));
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00428817, temp, sizeof(temp));
	}

	{
		BYTE temp = 0xeb;
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00473736, &temp, sizeof(temp));
	}


	if (GetPrivateProfileInt(L"PrivatePvp", L"Enable", 0, g_szFilePath) == 1)
	{
		LOGD << "PrivatePvp start!";
		FantasyPvp::Start();
	}

	if (GetPrivateProfileInt(L"NtPlayer", L"enable", 0, g_szFilePath) == 1)
	{
		LOGD << "NtPlayer start!";
		NtPlayer::StartNtPlayer();
	}



	if (GetPrivateProfileInt(L"TradeChannel", L"enable", 0, g_szFilePath) == 1){
		LOGD << "TradeChannel start!";
		TradeChannel::StartTradeChannel();
	}


	if (GetPrivateProfileInt(L"Naked", L"enable", 0, g_szFilePath) == 1){
		LOGD << "StartNakedMap start!";
		ShaiyaNakedMap::StartNakedMap();
	}

	if (GetPrivateProfileInt(L"namecolor", L"Enable", 0, g_szFilePath) == 1){
		LOGD << "namecolor start!";
		NameColor::Start();
	}

	if (GetPrivateProfileInt(L"recRune", L"enable", 0, g_szFilePath) == 1){
		LOGD << "recRune start!";
 		RecRune::start();
	}


	if (GetPrivateProfileInt(L"autoExp", L"enable", 0, g_szFilePath) == 1){
		LOGD << "autoExp start!";
		AutoExp::start();
	}

	if (GetPrivateProfileInt(L"AutoKills", L"enable", 0, g_szFilePath) == 1){
		LOGD << "AutoKills start!";
		AutoKills::start();
	}



	if (GetPrivateProfileInt(L"resProtect", L"enable", 0, g_szFilePath) == 1) 
	{
		LOGD << "resProtect start!";
		resProtect::start();
	}
	if (GetPrivateProfileInt(L"autoSummon", L"enable", 0, g_szFilePath) == 1)
	{
		LOGD << "autoSummon start!";
		autoSummon::start();
	}
	if (GetPrivateProfileInt(L"fireport", L"enable", 0, g_szFilePath) == 1){
		LOGD << "fireport start!";
		firePort::start();
	}
	if (GetPrivateProfileInt(L"ffa", L"enable", 0, g_szFilePath) == 1){
		LOGD << "ffa start!";
		FFA::Start();
	}
	if (GetPrivateProfileInt(L"randomRes", L"enable", 0, g_szFilePath) == 1)
	{
		LOGD << "randomRes start!";
		randomRes::Start();
	}


	if (GetPrivateProfileInt(L"levelReward", L"enable", 0, g_szFilePath) == 1)
	{
		LOGD << "levelReward start!";
		levelReward::start();
	}


	if (GetPrivateProfileInt(L"costume", L"enable", 0, g_szFilePath) == 1)
	{
		LOGD << "costume start!";
		costume::Start();
	}

	if (GetPrivateProfileInt(L"pvpdrop", L"enable", 0, g_szFilePath) == 1)
	{
		LOGD << "pvpdrop start!";
		PvpDrops::Start();
	}

	if (GetPrivateProfileInt(L"mobNotice", L"enable", 0, g_szFilePath) == 1)
	{
		LOGD << "mobNotice start!";
		mobNotice::Start();
	}

	if (GetPrivateProfileInt(L"randomReward", L"enable", 0, g_szFilePath) == 1)
	{
		LOGD << "randomReward start!";
		randomReward::start();
	}

	if (GetPrivateProfileInt(L"BlockChat", L"enable", 0, g_szFilePath) == 1)
	{
		LOGD << "BlockChat start!";
		BlockChat::Start();
	}


	if (GetPrivateProfileInt(L"vsFixed", L"enable", 0, g_szFilePath) == 1)
	{
		LOGD << "vsFixed start!";
		vsFixed::start();
	}

	if (GetPrivateProfileInt(L"confrimTrade", L"enable", 0, g_szFilePath) == 1)
	{
		LOGD << "confrimTrade start!";
		confrimTrade::start();
	}

	if (GetPrivateProfileInt(L"statsFixed", L"enable", 0, g_szFilePath) == 1)
	{
		LOGD << "statsFixed start!";
		statsFixed::start();
	}

	if (GetPrivateProfileInt(L"setItem", L"enable", 0, g_szFilePath) == 1)
	{
		LOGD << "setItem start!";
		setItem::start();
	}
	
	if (GetPrivateProfileInt(L"killtitle", L"enable", 0, g_szFilePath) == 1)
	{
		LOGD << "killtitle start!";
		killtitle::Start();
	}


	Sleep(5000);
	maxEnchant::start();
	getZones();



#ifdef _DEBUG
	enumMobs::start();
#endif



#ifndef _DEBUG
	VMProtectEnd();
 #endif



	if (GetPrivateProfileInt(L"autoPoint", L"enable", 0, g_szFilePath) == 1){

		if (!g_db.LinkDataBase(L"ps_userdata")) {
			MyLog("LinkDataBase failed\n");
			TerminateProcess(GetCurrentProcess(), 0);
		}

		AutoPoint::start();
	}


	while (1)
	{
		for (DWORD i = 0; i < g_vecLoopFunction.size(); i++)
		{
			g_vecLoopFunction[i]();
		}
		Sleep(1 * 1000);
	}	



}

bool isInject() {
	WCHAR szProcessName[MAX_PATH] = { 0 };
	GetModuleBaseName(GetCurrentProcess(), NULL, szProcessName, sizeof(szProcessName));
	if (wcscmp(szProcessName, L"ps_game.exe") == 0) {
		return true;

	}
	return false;
}


void Init() {


	GetModuleFileName(GetModuleHandle(0), g_szFilePath, MAX_PATH);
	PathRemoveFileSpec(g_szFilePath);
	wcscat_s(g_szFilePath, MAX_PATH, L"\\config.ini");

	GetModuleFileNameA(GetModuleHandle(0), g_strFilePath, MAX_PATH);
	PathRemoveFileSpecA(g_strFilePath);


	SYSTEMTIME st;
	GetLocalTime(&st);
	char logFile[MAX_PATH] = { 0 };

	//create folder
	sprintf(logFile, "%s\\%s", g_strFilePath, MYLOGFOLDER);
	CreateDirectoryA(logFile, NULL);

	//log txt
	sprintf(logFile, "%s\\%s\\ep6.log", g_strFilePath, MYLOGFOLDER);
	plog::init(plog::debug, logFile); // Step2: initialize the logger.

	LOGD << "----------------------------------------------------------------";
	LOGD << "----------------------------------------------------------------";
	LOGD << "----------------------------------------------------------------";


	strcat_s(g_strFilePath, MAX_PATH, "\\config.ini");

	getPlayer::Start();

}

void Process(void* p) {

	if (!isInject()) {
		return;
	}

	Init();
	

	if (GetPrivateProfileInt(L"gradeExt", L"enable", 0, g_szFilePath) == 1) {
		DropsRange::start();
	}
	

	_beginthread(ThreadProc1,0, 0);
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
		Process(0);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}








