// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// TradeChannel.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO:  �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������
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