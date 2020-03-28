// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <string>
#include <vector>
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")
using std::vector;
using std::string;


#include "common.h"

#include "CustomPackets.h"
#include "DataBase.h"
#include "ChatFilter.h"
#include "NakedMap.h"
#include "ShareKills.h"
#include "SummonWithMap.h"
#include "TradeChannel.h"
#include "NtPlayer.h"
#include "FantasyPvp.h"




extern vector<fun_chatcallback> g_vecChatCallBack;
extern vector<fun_TradeProcessCallBack> g_vecTradeProcess;
extern vector<Packet_Processedfunction> g_vecPacketProcessfun;


//chat filter
extern bool g_bEnableChatfilter;

//private pvp
extern bool g_bEnableFantasyPvp;
extern WORD g_wFantasyPvpMap;
extern WORD g_wFantasyPvpIndex;

//自定义封包


// Naked Map
// BOOL g_bEnableNaked = FALSE;
// extern legalItem g_ayLegalItem[24];
// extern DWORD g_dwNakedMap;
// 
// sharekills
// BOOL g_bEnableShareKill;
// 
// TradeChannel
// BOOL g_bEnableTradeChannel = FALSE;
// 
// ntPlayer
// bool g_bEnableNtPlayer = false;
// 
// color
// bool g_bEnable_Color = false;





#define CHAR_TO_WCHAR(lpChar,lpW_Char) \
MultiByteToWideChar(CP_ACP,0,lpChar,-1,lpW_Char,_countof(lpW_Char));

#define WCHAR_TO_CHAR(lpW_Char,lpChar) \
WideCharToMultiByte(CP_ACP,0,lpW_Char,-1,lpChar,_countof(lpChar),0,0);


HMODULE g_hModule;
DWORD WINAPI ThreadProc1(_In_  LPVOID lpParameter)
{	
	WCHAR szFilePath[MAX_PATH] = { 0 };
	GetModuleFileName(g_hModule, szFilePath, MAX_PATH);
	PathRemoveFileSpec(szFilePath);
	wcscat_s(szFilePath, MAX_PATH, L"\\config.ini");

	char strFilePath[MAX_PATH] = { 0 };
	WCHAR_TO_CHAR(szFilePath, strFilePath)

		

	//一次性读取的配置文件
	//-------------------------------------------------------------------------------------------------
// 	g_bEnableNtPlayer     = GetPrivateProfileInt(L"NtPlayer", L"enable",0, szFilePath);
// 	g_bEnableShareKill    = GetPrivateProfileInt(L"ShareKills", L"enable", 0, szFilePath);
// 	g_bEnableNaked        = GetPrivateProfileInt(L"Naked", L"enable", 0, szFilePath);
// 	g_bEnableTradeChannel = GetPrivateProfileInt(L"TradeChannel", L"enable", 0, szFilePath);
// 	bool bEnable_Color    = GetPrivateProfileInt(L"namecolor", L"Enable", 0, szFilePath);

	g_bEnableFantasyPvp = GetPrivateProfileInt(L"PrivatePvp", L"Enable", 0, szFilePath);
	g_wFantasyPvpMap    = GetPrivateProfileInt(L"PrivatePvp", L"map", 0, szFilePath);
	g_wFantasyPvpIndex  = GetPrivateProfileInt(L"PrivatePvp", L"index", 0, szFilePath);
	
	//-------------------------------------------------------------------------------------------------

	Sleep(10000);

	//这2个必开的
	CustomPackets::Start(); //自定义封包与利用交易接口的封包拦截  
	CharFilter::Start();

	
	

	if (g_bEnableFantasyPvp)
	{
		g_vecChatCallBack.push_back(FantasyPvp::ChatProcess);//说话内容的回调
	//	g_vecTradeProcess.push_back(FantasyPvp::TradeProcess);
		FantasyPvp::Start();
	}
		




// 	CDataBase objDataBase;
// 	BOOL bSuccessLink = false;
// 	if (bEnable_Color)
// 	{	
// 		bSuccessLink = objDataBase.LinkDataBase(L"ps_gamedata");
// 	}



	while (1)
	{
		//chatfilter__begin
// 		EnterCriticalSection(&g_csCharFilter);
// 		g_vec_szFilter.clear();
// 		for (DWORD i = 0; i < 100; i++)
// 		{
// 			char szKey[MAX_PATH] = { 0 };
// 			char szReturn[MAX_PATH] = { 0 };
// 			sprintf_s(szKey, MAX_PATH, "%d", i + 1);
// 			if (GetPrivateProfileStringA("ChatFilter", szKey, "", szReturn, MAX_PATH, strFilePath) <= 0) break;
// 			g_vec_szFilter.push_back(szReturn);
// 		}
// 		LeaveCriticalSection(&g_csCharFilter);

		//NakedMap__begin
// 		g_dwNakedMap = GetPrivateProfileInt(L"Naked", L"map", 0, szFilePath);
// 		for (DWORD i = 0; i < 24; ++i)
// 		{
// 			WCHAR szKey[MAX_PATH] = { 0 };
// 			WCHAR   szReturn[MAX_PATH] = { 0 };
// 			swprintf_s(szKey, MAX_PATH, L"%d", i);
// 			if (GetPrivateProfileString(L"Naked", szKey, L"", szReturn, MAX_PATH, szFilePath) <= 0)
// 				continue;
// 			CString szTemp[2];
// 
// 			swscanf_s(szReturn, L"%s,%s", szTemp[0], szTemp[1]);
// 			g_ayLegalItem[i].dwItemid = _tcstoul(szTemp[0], 0, 10);
// 			g_ayLegalItem[i].bAlow    = _tcstoul(szTemp[1], 0, 10);
// 		}
		//vip color


		
// 
// 		if (bEnable_Color&&bSuccessLink)
// 		{
// 			cmd_color stcColor[200] = { 0 };
// 			ZeroMemory(stcColor, sizeof(stcColor));
// 
// 			DWORD i = 0;
// 
// 			//从数据库取数据
// 			objDataBase.ExeSqlByCommand(L"select top 200 charid,color from chars where color>0");
// 			for (; i < objDataBase.m_nRecordCount; i++)
// 			{
// 				stcColor[i].dwCharid = _tcstoul(objDataBase.GetValueByField(L"charid"), 0, 10);
// 				stcColor[i].dwRGB    = _tcstoul(objDataBase.GetValueByField("color"), 0, 10);
// 				stcColor[i].dwRGB |= 0xff000000;
// 				objDataBase.MoveNext();
// 			}
// 			if (i)
// 			{
// 				objDataBase.clear();
// 				DWORD dwSize = (i + 2) * sizeof(cmd_color);
// 				send_packet_all(PacketOrder::VipColor, (PBYTE)stcColor, dwSize);
// 			}
// 
// 		}

		Sleep(20 * 1000);
	}

	return 0;
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
		g_hModule = hModule;
		::CreateThread(0, 0, ThreadProc1, 0, 0, 0);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}








