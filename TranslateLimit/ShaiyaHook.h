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

//���������
typedef struct _RandomSpawn
{
	int   nMapid;                //��ͼ
	float fPosx; //��
	float fPosy; //��
	float fPosz; //��
}RandomSpawn;

//GMָ����
typedef struct _GMCOMMAND
{
	BYTE  byCommand[2];	//GMָ��
	BYTE* pbyParameter1;//����1
	BYTE* pbyParameter2;//����2
	BYTE* pbyParameter3;//����3
}GMCOMMAND,PGMCOMMAND;

//��ɫ��Ϣ
typedef struct _PLAYERINFO
{
	WCHAR szUid		[10];//�˺�UID
	WCHAR szCharid	[10];//��ɫID
	WCHAR szLevel	[10];//�ȼ�
	WCHAR szKills	[10];//ս��
	WCHAR szPosx	[10];//posx
	WCHAR szPosy	[10];//posy
	WCHAR szPosz	[10];//posz
	WCHAR szCharName[20];//��ɫ��
	WCHAR szBaseAddr[10];//��ַ
	WCHAR szMap     [10];//��ͼ
}PLAYERINFO,*PPLAYERINFO;

//NPC���
typedef struct _POS
{
	float x;
	float y;
	float z;
}POS, *PPOS;

//�Զ�NPC�Ľṹ
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

//�Զ���ɾ�ֵĽṹ
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


void InitHook();                                                         //��ʼ��������Ϣ
void LoadConfig();//��ȡ������Ϣ
void Change_UpdateItem(DWORD dwItemid);                                  //����Item�ڴ�����
void Call_GmCommand(BYTE* pbyCommand, DWORD dwBaseAddr);                 //ִ��GMָ��call
void ScanPlayerInfo();                                                   //ɨ�����Hook��ʽ����ȡ��һ�ַ��Ϣ
BOOL GetPlayerInfoByBaseAddr(DWORD pBaseAddr, PLAYERINFO &stcPlayerInfo);//������һ�ַ��ȡ�����Ϣ
void Hook_VipColor(BOOL nSwitch, DWORD dwUid[] = 0);                     //Hook VIP��ɫ������ɫ

void EnchantAdd();//ǿ��
void RecRune();   //���Ծ���
BYTE __stdcall RecRune_CustomStats(BYTE byReqwis, PBYTE pAddr, BYTE byTimes, PVOID pRecRune);        //���Ծ���
void RecRune_Signature(PBYTE pSignature, PVOID pRecRune, BYTE byTargetType);//���Ծ���
BOOL __stdcall TranslateMap(PVOID pPlayer, DWORD dwMap);

//�������
PVOID  GetSkillBaseAddr(DWORD dwSkillid, DWORD dwSkillLevel);
void   UseSkill(DWORD dwSkillid, DWORD dwSkillLevel, PVOID pPlayerAddr);



//��ʱ��ص�
VOID CALLBACK AddPointPerMin(_In_  HWND hwnd, _In_  UINT uMsg, _In_  UINT_PTR idEvent, _In_  DWORD dwTime);//�ݵ�
DWORD WINAPI AutoTask(_In_  LPVOID lpParameter);//�Զ�NPC����������