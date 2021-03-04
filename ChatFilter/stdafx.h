#ifndef _PREHEADER
#define _PREHEADER

#include <windows.h>
#define PSAPI_VERSION 1
#include <Psapi.h>
#pragma comment(lib,"Psapi.lib")
#include "../utility/utility.h"
#include <plog/Log.h>
#include <plog/Converters/NativeEOLConverter.h>
#include <process.h>
#include <functional>
#include <array>

/*
#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#include <windows.h>




#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <process.h>
#include <common.h>
#include <Shlwapi.h>
#include <MyClass.h>
#include <MyInlineHook.h>
#include "VMProtectSDK.h"
#include <plog/Log.h>
#include <winsock.h>
#include <map>
#include "CFFA.h"

#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"psapi.lib")

using std::vector;
using namespace EP4CALL;
using namespace shiayacommon;
using std::string;
using std::map;

extern vector<fun_preUseItemSend> g_vecPreUseItemCallBack;
extern EP4HookADDR g_hookAddr;
extern  WCHAR g_szFilePath[MAX_PATH];
extern  char g_strFilePath[MAX_PATH];
extern  DWORD g_players[1000];
extern vector<fun_onLogin> g_vecOnloginCallBack;
extern  vector<fun_loop> g_vecLoopFunction;
extern  vector<fun_isSameFaction> g_factionCallBack;
extern  vector<fun_afterKilled> g_killedCallBack;
extern  vector<fun_GetFaction> g_getfactionCallBack;
extern  vector<fun_fireport> g_fireportCallBack;
extern  vector<fun_PartyAble> g_partyAbleCallBack;
extern  vector<SUMMONTASK> g_vecSummontasks;
extern  vector<NOTICETASK> g_vecNoticeTasks;
extern vector<fun_afterKilled> g_addkillsCallBack;
extern vector<fun_chatcallback> g_chatCallBack;
extern vector<fun_consoleCallBack> g_consoleCallBack;
extern vector<fun_MoveAble> g_moveCallBack;
extern vector<fun_useSkill> g_UseSkillCallBack;
extern vector<fun_UserShape> g_userMeetCallBack;
extern vector<fun_enableKills> g_enablekillsCallBack;
extern char g_MapName[300][MAX_PATH];
extern CRITICAL_SECTION g_csHutingplayers;
extern vector<DWORD> g_vecHuntedPlayerCid;
extern vector<fun_enteredMap> g_vecEnteredMapCallBack;
extern vector<fun_isCommonSkillUsable> g_commonSkillUsableCallBack;

extern CFFA* g_ffp1;



#pragma warning(disable:4244)
#pragma warning(disable:4996)
#pragma warning(disable:4005)
#pragma warning(disable:4800)

*/
#endif