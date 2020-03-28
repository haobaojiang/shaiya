

#pragma once
#include "targetver.h"
#define  PSAPI_VERSION 1  
#define WIN32_LEAN_AND_MEAN            
#include <windows.h>


#include "VMProtectSDK.h"
#include <process.h>
#include <MyInlineHook.h>
#include <vector>
#include "DataBase.h"
#include <common.h>
#include <Shlwapi.h>
#include <shellapi.h>
#include <Psapi.h>
#include <plog/Log.h>



#pragma comment(lib,"psapi.lib")
#pragma comment(lib,"Shlwapi.lib")
#include <MyClass.h>

using std::string;
using std::vector;
using namespace EP6CALL;
using namespace shiayacommon;


extern Pshaiya_player g_players[1000];
extern WCHAR g_szFilePath[MAX_PATH];
extern char g_strFilePath[MAX_PATH];
extern EP6HookADDR g_hookAddr;
extern CDataBase g_db;
extern DWORD g_zones[200];
extern char g_strMapName[200][MAX_PATH];
extern vector<fun_loop> g_vecLoopFunction;
extern vector<fun_chatcallback> g_vecChatCallBack;
extern vector<Packet_Processedfunction> g_vecPacketProcessfun;
extern vector<fun_onLogin> g_vecOnloginCallBack;

#pragma warning(disable:4244)
#pragma warning(disable:4996)
#pragma warning(disable:4005)