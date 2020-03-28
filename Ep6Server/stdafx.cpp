// stdafx.cpp : 只包括标准包含文件的源文件
// TradeChannel.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO:  在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用
Pshaiya_player g_players[1000];
WCHAR g_szFilePath[MAX_PATH];
char g_strFilePath[MAX_PATH];
EP6HookADDR g_hookAddr;
CDataBase g_db;
DWORD g_zones[200];
char g_strMapName[200][MAX_PATH];
vector<fun_loop> g_vecLoopFunction;
vector<fun_chatcallback> g_vecChatCallBack;
vector<fun_onLogin> g_vecOnloginCallBack;
vector<Packet_Processedfunction> g_vecPacketProcessfun;