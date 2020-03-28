#pragma once
#include <vector>
#include <algorithm>  
#include "MyInlineHook.h"

using namespace std;
using std::vector;



typedef struct _EnchantAddValue
{
	WORD wWeaponValue;
	WORD wArrmorValue;
}EnchantAddValue, *PEnchantAddValue;

typedef struct _EnchantRate
{
	DWORD dwWeaponValue;
	DWORD dwArrmorValue;
}EnchantRate, *PEnchantRate;

//随机复活点的
typedef struct _RandomSpawn
{
	int   nMapid;                //地图
	float fPosx; //左
	float fPosy; //高
	float fPosz; //右
}RandomSpawn;

//GM指令封包
typedef struct _GMCOMMAND
{
	BYTE  byCommand[2];	//GM指令
	BYTE* pbyParameter1;//参数1
	BYTE* pbyParameter2;//参数2
	BYTE* pbyParameter3;//参数3
}GMCOMMAND,PGMCOMMAND;

//角色信息
typedef struct _PLAYERINFO
{
	WCHAR szUid		[10];//账号UID
	WCHAR szCharid	[10];//角色ID
	WCHAR szLevel	[10];//等级
	WCHAR szKills	[10];//战功
	WCHAR szPosx	[10];//posx
	WCHAR szPosy	[10];//posy
	WCHAR szPosz	[10];//posz
	WCHAR szCharName[20];//角色名
	WCHAR szBaseAddr[10];//基址
	WCHAR szMap     [10];//地图
}PLAYERINFO,*PPLAYERINFO;

//NPC相关
typedef struct _POS
{
	float x;
	float y;
	float z;
}POS, *PPOS;

//自动NPC的结构
typedef struct _AUTONPC
{
	DWORD dwNpcType;
	DWORD dwNpcId;
	BYTE  byMap;
	float x;
	float y;
	float z;
	CString szBornHours;
	CString szBornMins;
	CString szDeathHours;
	CString szDeathMins;
	BOOL  bLoop;
	BOOL  bFlag;
}AUTONPC,*PAUTONPC;

//自动添删怪的结构
typedef struct _AUTOMONSTER
{
	DWORD dwMobId;
	BYTE  byMap;
	float x;
	float y;
	float z;
	CString szBornHours;
	CString szBornMins;
	CString szDeathHours;
	CString szDeathMins;
	BOOL  bLoop;
	BOOL  bFlag;
}AUTOMONSTER,PAUTOMONSTER;


void InitHook();                                                         //初始化所有信息
void LoadConfig();//读取配置信息
void Change_UpdateItem(DWORD dwItemid);                                  //更新Item内存数据
void Call_GmCommand(BYTE* pbyCommand, DWORD dwBaseAddr);                 //执行GM指令call
void ScanPlayerInfo();                                                   //扫描或是Hook方式来获取玩家基址信息
BOOL GetPlayerInfoByBaseAddr(DWORD pBaseAddr, PLAYERINFO &stcPlayerInfo);//根据玩家基址获取玩家信息
void Hook_VipColor(BOOL nSwitch, DWORD dwUid[] = 0);                     //Hook VIP角色名字颜色

void EnchantAdd();//强化
void RecRune();   //属性卷轴
BYTE __stdcall RecRune_CustomStats(BYTE byReqwis, PBYTE pAddr, BYTE byTimes, PVOID pRecRune);        //属性卷轴
void RecRune_Signature(PBYTE pSignature, PVOID pRecRune, BYTE byTargetType);//属性卷轴
BOOL __stdcall TranslateMap(PVOID pPlayer, DWORD dwMap);

//技能相关
PVOID  GetSkillBaseAddr(DWORD dwSkillid, DWORD dwSkillLevel);
void   UseSkill(DWORD dwSkillid, DWORD dwSkillLevel, PVOID pPlayerAddr);



//定时相关的
VOID CALLBACK AddPointPerMin(_In_  HWND hwnd, _In_  UINT uMsg, _In_  UINT_PTR idEvent, _In_  DWORD dwTime);//泡点
DWORD WINAPI AutoTask(_In_  LPVOID lpParameter);//自动NPC、怪物任务