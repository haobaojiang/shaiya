#pragma once
#ifndef SHAIYA_UTILITY_HEADER
#define SHAIYA_UTILITY_HEADER

#include <windows.h>
#include <Shlwapi.h>
#define PSAPI_VERSION 1
#include <Psapi.h>
#include <string>
#include <functional>
#include <TlHelp32.h>
#include <WinCrypt.h>
#include <atlbase.h>
#include <comdef.h>
#include <Wbemidl.h>
#include <iostream>
#include <vector>
#include <sstream>      // std::istringstream

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib,"psapi.lib")

namespace ShaiyaUtility {
#define MYASMCALL(f)  { __asm call f}
#define MYASMCALL_1(f,a)  { __asm push a __asm call f}
#define MYASMCALL_2(f,a,b)  { __asm push b _asm push a __asm call f}	
#define MYASMCALL_3(f,a,b,c)  { __asm push c _asm push b _asm push a __asm call f}	
#define MYASMCALL_4(f,a,b,c,d)  { __asm push d _asm push c _asm push b _asm push a __asm call f}	
#define MYASMCALL_5(f,a,b,c,d,e)  {__asm push e __asm push d _asm push c _asm push b _asm push a __asm call f}	
	//读内存函数
	template<class T>
	T read(const DWORD& obj)
	{
		T value = { 0 };
		__try {
			value = *(T*)(obj);
		}
		__except (1) {
		}

		return value;
	}

	//写内存函数
	template<class T>
	void write(const DWORD& obj, T Value)
	{
		__try {
			*(T*)(obj) = Value;
		}
		__except (1) {
		}
	}

	static std::vector<std::string> split(const std::string& s, char delimiter)
	{
		std::vector<std::string> tokens;
		std::string token;
		std::istringstream tokenStream(s);
		while (std::getline(tokenStream, token, delimiter))
		{
			tokens.push_back(token);
		}
		return tokens;
	}

	struct Pos {
		float x;
		float y;
		float z;
	};


	static std::string itos(int i) {
		char key[MAX_PATH] = { 0 };
		sprintf_s(key, MAX_PATH, "%ld", i);
		return std::string(key);
	}

	static int itemID2TypeId(int itemid) {
		return itemid % 1000;
	}

	static BOOL WriteCurrentProcessMemory(void* Address, void* Data, size_t Length) {
		auto process = GetCurrentProcess();

		SIZE_T written = 0;
		DWORD oldProtect = 0;
		auto result = VirtualProtect(Address, Length, PAGE_EXECUTE_READWRITE, &oldProtect);
		if (!result) {
			return result;
		}
		::WriteProcessMemory(process, Address, Data, Length, &written);
		result = VirtualProtect(Address, Length, oldProtect, &oldProtect);
		if (!result) {
			return result;
		}

		return TRUE;
	}


	class CMyInlineHook
	{
	public:

		CMyInlineHook(void) {
			ZeroMemory(m_byOriginalCode, _countof(m_byOriginalCode) * sizeof(BYTE));
			ZeroMemory(m_byJmpAsmCode, _countof(m_byJmpAsmCode) * sizeof(BYTE));
		}
		~CMyInlineHook(void) {};

		DWORD Hook(PVOID pOriginalAddr, PVOID pNewAddr, int nSize = 5)
		{
			m_nSize = nSize;
			DWORD dwOldProtect;
			::VirtualProtect(pOriginalAddr, m_nSize, PAGE_READWRITE, &dwOldProtect);
			::ReadProcessMemory(GetCurrentProcess(), pOriginalAddr, m_byOriginalCode, m_nSize, NULL);
			::VirtualProtect(pOriginalAddr, m_nSize, dwOldProtect, &dwOldProtect);
			M_pOriginalAddr = pOriginalAddr;

			//准备好另一块空间用来执行原来的指令
			m_pOldFunction = m_byOldCodes;
			memcpy(m_byOldCodes, pOriginalAddr, nSize);
			m_byOldCodes[nSize] = 0xe9;
			DWORD dwReturnAddr = DWORD(&m_byOldCodes[nSize]);
			*PDWORD(&m_byOldCodes[nSize + 1]) = (DWORD(pOriginalAddr) + nSize) - dwReturnAddr - 5;
			::VirtualProtect(pOriginalAddr, m_nSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);  //让这段内存可以执行
			m_byJmpAsmCode[0] = 0xE9; // Jmp *****
			*(PDWORD)& (m_byJmpAsmCode[1]) = (DWORD)pNewAddr - (DWORD)pOriginalAddr - 5;

			for (int i = 5; i < nSize; i++)
			{
				m_byJmpAsmCode[i] = 0x90;
			}
			::VirtualProtect(pOriginalAddr, m_nSize, PAGE_READWRITE, &dwOldProtect);
			::WriteProcessMemory(GetCurrentProcess(), pOriginalAddr, m_byJmpAsmCode, m_nSize, NULL);
			::VirtualProtect(pOriginalAddr, m_nSize, dwOldProtect, &dwOldProtect);

			m_pRet = PVOID(DWORD(pOriginalAddr) + nSize);

			return (DWORD)m_pRet;
		}


		DWORD Hook(DWORD pOriginalAddr, PVOID pNewAddr, int nSize = 5) {
			return this->Hook(reinterpret_cast<PVOID>(pOriginalAddr), pNewAddr, nSize);
		}
		CMyInlineHook(PVOID pOriginalAddr, PVOID pNewAddr, PDWORD pReturnAddr, int nSize = 5) {
			ZeroMemory(m_byOriginalCode, _countof(m_byOriginalCode) * sizeof(BYTE));
			ZeroMemory(m_byJmpAsmCode, _countof(m_byJmpAsmCode) * sizeof(BYTE));
			*pReturnAddr = Hook(pOriginalAddr, pNewAddr, nSize);
		}

		BOOL UnHook() {

			if ((ULONGLONG)m_byOriginalCode == 0) return FALSE;

			DWORD dwOldProtect;
			::VirtualProtect(M_pOriginalAddr, m_nSize, PAGE_READWRITE, &dwOldProtect);
			::WriteProcessMemory(GetCurrentProcess(), M_pOriginalAddr, m_byOriginalCode, m_nSize, NULL);
			::VirtualProtect(M_pOriginalAddr, m_nSize, dwOldProtect, &dwOldProtect);
			return TRUE;
		}
		PVOID m_pOldFunction;
	private:
		PVOID m_pNewAddr;          //用户自定义的函数地址
		PVOID M_pOriginalAddr;     //原始的函数地址
		BYTE  m_byOriginalCode[20];
		BYTE  m_byJmpAsmCode[20];
		int   m_nSize;             //要改的地方到底要改几个字节,最少是5个或以上
		PVOID m_pRet;
		BYTE m_byOldCodes[40];
	};


	static std::wstring GetCurrentExePathW() {
		WCHAR path[MAX_PATH]{ 0 };
		if (GetModuleFileNameW(GetModuleHandle(0), path, MAX_PATH) == 0) {
			return L"";
		}

		if (!PathRemoveFileSpecW(path)) {
			return L"";
		}

		return path;
	}

	static std::string GetCurrentExePathA() {
		char path[MAX_PATH]{ 0 };
		if (GetModuleFileNameA(GetModuleHandle(0), path, MAX_PATH) == 0) {
			return "";
		}

		if (!PathRemoveFileSpecA(path)) {
			return "";
		}

		return path;
	}
}

namespace ShaiyaUtility::Packet {

	static const DWORD s_RandomColorList[] = {
0xFFA000,
0xFF0000,
0xFF00FF,
0x8000FF,
0x00C0FF,
0x0000E0,
0x00FF00,
	};

	enum  Code :WORD {
		begin = 0xffa1,
		nameColor = begin,
		skillCutting,
		killsRanking,
		itemCombines,
		enhanceAttack,
		hiddenMountCode,
		NakedSkillSetting,
	};

	enum { RandomColor = 0xffffffff };

#pragma pack(push,1)
	struct Header {
		Code command;
		Header(Code cmd = Code::begin) :command(cmd) {}
	};

	// enhance attack
	struct EnhanceAttack:Header {
		DWORD values[51];
		EnhanceAttack() :Header(Code::enhanceAttack) {}
	};

	// notice
	struct Notice :Header {
		BYTE count;
		char msg[0x70]{ 0 };
		size_t length() {
			return 2 + 1 + count; //header + count field + msg field
		}
		void CopyMsg(const char* Msg) {
			BYTE len = static_cast<BYTE>(strlen(Msg));
			if (len >= 0x69) {
				len = 0x69;
			}
			count = len;
			memcpy(this->msg, Msg, len);
			this->msg[len] = '\0';
		}
		Notice() :Header(Code(0xf90b)) {}
	};

	struct MontModel
	{
		BYTE id{};
		bool need_rotate{};
		float height{};
		BYTE boneId{};
	};



	struct NakedSkillEffect:Header
	{
		BYTE maps[20]{};// supporting maximum setting 20 maps
		bool enabled{};
		DWORD hookObj1 = 0x00450370;
		DWORD hookObj2 = 0x0040f620;
		DWORD hookObj3 = 0x00432A80;
		DWORD hookObj4 = 0x0044FEC0;
		DWORD hookObj5 = 0x00450300;
		DWORD hookObj6 = 0x00417750;
		NakedSkillEffect() :Header(NakedSkillSetting)
		{

		}
	};
	

	struct NewHiddenMount:Header
	{
	MontModel models[100];
	DWORD Naked_fixCharacterRotate1 = 0x0041331A;
	DWORD Naked_fixCharacterRotate2 = 0x00413393;
	DWORD Naked_fixCharacterRotate3 = 0x00413504;
	DWORD Naked_fixCharacterRotate4 = 0x00413447;
	DWORD Naked_fixCharacterRotate5 = 0x004134cb;
	DWORD Naked_fixboneId1 = 0x004132E9; 
	DWORD Naked_fixboneId2 = 0x0041330D; 
	DWORD Naked_fixboneId3 = 0x00413362; 
	DWORD Naked_fixboneId4 = 0x00413386;
	DWORD Naked_fixboneId5 = 0x00413498;
	DWORD Naked_fixboneId6 = 0x004134f7;
	DWORD Naked_fixboneId7 = 0x004133db; 
	DWORD Naked_fixboneId8 = 0x0041343a; 


		
	DWORD Naked_fixHeight = 0x00432A6E;
	NewHiddenMount():Header(hiddenMountCode)
	{
			
	}
};

	// name color
	struct NameColorContent :Header {
		struct {
			DWORD charid = 0;
			DWORD rgb = 0;
		}players[200]{ 0 };
		NameColorContent() :Header(Code::nameColor) {}
	};
	// skill cut
	struct SkillCuttingContent :Header {
		int mode = 0;
		SkillCuttingContent() :Header(Code::skillCutting) {}
	};
	// kills ranking
	struct KillsRankingContent :Header {
		DWORD normalModePoints[31]{ 0 };
		DWORD hardModePoints[31]{ 0 };
		DWORD ultimateModePoints[31]{ 0 };
		DWORD killsNeeded[31]{ 0 };
		KillsRankingContent() :Header(Code::killsRanking) {}
	};
	// combine task
	struct ItemCombines :Header {
		DWORD rewardID;
		struct {
			DWORD id{ 0 };
			DWORD count{ 0 };
		}materials[3]{ 0 };
		ItemCombines() :Header(Code::itemCombines) {};
	};

#pragma pack(pop)
}


namespace ShaiyaUtility::EP4 {




	struct SCriticalSection;
	struct stApplySkillParty;
	struct SNode;
	struct stCloneUser;
	struct stAttackAdd;
	struct stAttack;
	struct stTarget;
	struct ITEM_TYPE;
	struct stExcItem;
	struct CExchange;
	struct CUser;
	struct stMyShop;
	struct stPartyUser;
	struct CParty;
	struct CMiniGame;
	struct SVector;
	struct stAdminNotice;
	struct stAdminInfo;
	struct stPointLog;
	struct stUserInfo;
	struct CItem;
	struct CObject;
	struct stBillingItemInfo;
	struct stQuick;
	struct CExchange;
	struct CExchangePvP;


	/* 429 */
	struct stExcItem
	{
		char byBag;
		char bySlot;
		char byCount;
	};

	/* 430 */
	struct  __declspec(align(4)) CExchange
	{
		int nState;
		CUser* pUser;
		unsigned int dwMoney;
		stExcItem pItem[8];
		bool bReady;
	};

	/* 431 */
	struct  CExchangePvP : CExchange
	{
		unsigned int dwMoneyPvP;
		CItem* pItemPvP[8];
	};

	struct  SCriticalSection
	{
		RTL_CRITICAL_SECTION m_cs;
	};


	struct __declspec(align(4)) stItemInfo
	{
		unsigned int dwItemID;
		char szName[32];
		char byType;
		char byTypeID;
		char byCountry;
		char byJob[6];
		unsigned __int16 wLevel;
		char byGrow;
		char byOG;
		char byIG;
		__int16 shVG;
		unsigned __int16 wStr;
		unsigned __int16 wDex;
		unsigned __int16 wRec;
		unsigned __int16 wInt;
		unsigned __int16 wWis;
		unsigned __int16 wLuc;
		unsigned __int16 byRange;
		char byAttackTime;
		char byAttrib;
		char bySpecial;
		char bySlot;
		char bySpeed;
		char byExp;
		char byServer;
		char byCount;
		unsigned __int16 wQuality[3];
		unsigned __int16 wEffect1[3];
		unsigned __int16 wEffect2[3];
		unsigned __int16 wEffect3[3];
		unsigned __int16 wEffect4[3];
		unsigned __int16 wConstHP;
		unsigned __int16 wConstSP;
		unsigned __int16 wConstMP;
		unsigned __int16 wConstStr;
		unsigned __int16 wConstDex;
		unsigned __int16 wConstRec;
		unsigned __int16 wConstInt;
		unsigned __int16 wConstWis;
		unsigned __int16 wConstLuc;
		unsigned __int16 wGrade;
		unsigned __int16 wDrop;
		unsigned int nBuy;
		unsigned int nSell;
		int nNumServerDrop;
		int nNumTimeDrop;
		unsigned int dwDropDelay;
		unsigned int dwDropRand;
		unsigned int dwNextDropTime;
		bool bQuestStart;
		int nRealType;
		char byMarketItemType;
	};

	struct SVector
	{
		float x;
		float y;
		float z;
	};


	struct SNode
	{
		SNode* prev;
		SNode* next;
	};

	struct __declspec(align(2)) stQuick
	{
		char byQuickBar;
		char byQuickSlot;
		char byBag;
		unsigned __int16 wNumber;
	};

	/* 411 */

	struct __declspec(align(1)) stBillingItemInfo
	{
		char byType;
		char byTypeID;
		char byCount;
	};

	struct  CObject
	{
		SVector m_vPos;
		unsigned int m_dwID;
		void* m_pZone;
		int m_nCellX;
		int m_nCellZ;
		BYTE m_Link[0xc];
	};

	struct  __declspec(align(8)) CItem : SNode, CObject
	{
		stItemInfo* m_pInfo;
		unsigned __int64 m_qwUniqueID;
		char m_byType;
		char m_byTypeID;
		char m_byCount;
		unsigned __int16 m_wQuality;
		char m_byGem[6];
		char m_szCraftName[21];
		unsigned int m_dwMakeTime;
		char m_byMakeType;
		unsigned int m_dwRemoveTime;
		unsigned int m_dwUserID;
		unsigned int m_dwPartyID;
		char m_byDropType;
		unsigned int m_dwDropID;
		unsigned int m_dwDropMoney;
		unsigned __int16 m_wCraftExpansionAddValue[11];
	};

	/* 417 */
	struct   __declspec(align(4)) stUserInfo
	{
		unsigned int m_dwCharID;
		char m_bySlot;
		char m_byCountry;
		char m_byFamily;
		char m_byGrow;
		char m_byMaxGrow;
		char m_byHair;
		char m_byFace;
		char m_bySize;
		char m_byJob;
		char m_bySex;
		unsigned __int16 m_wLevel;
		unsigned __int16 m_wStatPoint;
		unsigned __int16 m_wSkillPoint;
		unsigned int m_dwExp;
		unsigned int m_dwMoney;
		unsigned int m_dwFirstMoney;
		unsigned int m_dwBankMoney;
		unsigned int m_dwKillCount[4];
		unsigned int m_dwKillUsed[2];
		unsigned __int16 m_wMap;
		unsigned __int16 m_wDir;
		unsigned __int16 m_wHG;
		__int16 m_shVG;
		char m_byCG;
		char m_byOG;
		char m_byIG;
		unsigned __int16 m_wBaseStr;
		unsigned __int16 m_wBaseDex;
		unsigned __int16 m_wBaseInt;
		unsigned __int16 m_wBaseWis;
		unsigned __int16 m_wBaseRec;
		unsigned __int16 m_wBaseLuc;
		int m_wMaxHP;
		int m_wMaxMP;
		int m_wMaxSP;
		char m_szName[21];
		char m_pItemQualityLV[13];
		unsigned __int16 m_pItemQualityBase[13];
		CItem* m_pUserItem[6][24];
		CItem* m_pUserBank[240];
		stBillingItemInfo m_pBillingItem[240];
		BYTE m_applySkill[0x2c];
		int m_nNumSkill;
		void* m_pSkill[256];
		int m_nNumQuick;
		stQuick m_pQuick[128];
		BYTE m_QuestFinish[0x2c];
		BYTE m_Quest[0x2c];
	};


	/* 469 */
	struct stPointLog
	{
		char szCode[21];
		unsigned int dwBuyTime;
		unsigned int dwUsePoint;
	};

	struct  stAdminInfo
	{
		unsigned int dwProcessUQID;
		unsigned int dwSendToUserID;
		bool bVisible;
		bool bAttackable;
		unsigned int dwEnableMoveTime;
		unsigned int dwEnableChatTime;
		unsigned int dwListenChatID;
		unsigned int dwListenChatIDFrom;
		stAdminNotice* pNotice;
	};

	struct  __declspec(align(4)) stAdminNotice : SNode
	{
		unsigned int dwTo;
		unsigned int dwNormal;
		unsigned int dwMap;
		unsigned int dwCountry;
		unsigned int dwAll;
		unsigned int dwToTick;
		unsigned int dwNormalTick;
		unsigned int dwMapTick;
		unsigned int dwCountryTick;
		unsigned int dwAllTick;
		unsigned int dwToUserID;
		char byCountry;
		char byLenTo;
		char byLenNormal;
		char byLenMap;
		char byLenCountry;
		char byLenAll;
		char szTo[256];
		char szNormal[256];
		char szMap[256];
		char szCountry[256];
		char szAll[256];
	};



	/* 457 */
	struct  CMiniGame
	{
		int m_nStatus;
		int m_nNpc;
		int m_nBattingMoney;
		int m_nNumBattingSlot;
		int m_nSlotGetMoney;
		int m_nCardGameGrade;
		int m_nNpcCard;
		CUser* m_pUser;
	};

	/* 433 */
	struct stPartyUser
	{
		int Index;
		CUser* pUser;
	};

	/* 434 */
	struct  CParty : SNode
	{
		unsigned int m_dwID;
		int m_nBothIndex;
		int m_nNumUser;
		stPartyUser m_pParty[30];
		int m_nItemDivType;
		int m_nItemDivSeq;
		int m_nMaxUserLevel;
		int m_nSubBothIndex;
		bool m_bRaidPartyFlag;
		bool m_bAutoJoinFlag;
		SCriticalSection m_cs;
	};


	/* 432 */
	struct  stMyShop
	{
		CUser* m_pUser;
		int nStatus;
		int byBag[12];
		int bySlot[12];
		unsigned int dwMoney[12];
		int nNameLen;
		char szName[121];
		int nNumUseChar;
		unsigned int dwUseCharList[16];
		unsigned int dwMyUseShop;
	};


	/* 418 */
	struct ITEM_TYPE
	{
		char byType;
		char byTypeID;
		char byEnchantStep;
	};
	struct stTarget
	{
		int nType;
		unsigned int dwID;
	};


	struct stAttack
	{
		int nDisable;
		int nBlock;
		int nBlockValue;
		int nMirror;
		int nMirrorLevel;
		int nPerAttack;
		int nAttack;
		int nPerDefense;
		int nDefense;
		int nCriticalAttack;
		int nCriticalDefense;
	};

	struct stAttackAdd
	{
		int nPerAttack;
		int nAttack;
		int nPerDefense;
		int nDefense;
	};


	struct __declspec(align(4)) stApplySkillParty
	{
		unsigned int dwID;
		unsigned __int16 wSkillID;
		char bySkillLV;
	};




	struct  __declspec(align(4)) stCloneUser : SNode
	{
		char byDeath;
		char byMotion;
		char byCountry;
		char byFamily;
		char byHair;
		char byFace;
		char bySize;
		char byJob;
		char bySex;
		char byParty;
		char byGrow;
		unsigned int dwKillCount;
		ITEM_TYPE item[8];
		char szName[21];
		char byDummyInfo[31];
		char byLen;
	};
	struct stHPAbility
	{
		bool bApply;
		unsigned __int16 wID;
		char byLevel;
		unsigned int dwPrevTime;
	};


	struct __declspec(align(1)) ObjectConnection {
		BYTE m_objectConnection[0x128];
	};

	struct  CUser : ObjectConnection, stUserInfo
	{
		int m_wStr;
		int m_wDex;
		int m_wInt;
		int m_wWis;
		int m_wRec;
		int m_wLuc;
		int m_wHP;
		int m_wMP;
		int m_wSP;
		int m_nAtkRecHP;
		int m_nAtkRecSP;
		int m_nAtkRecMP;
		int m_nSitRecHP;
		int m_nSitRecSP;
		int m_nSitRecMP;
		int m_nSitRecHPP;
		int m_nSitRecSPP;
		int m_nSitRecMPP;
		int m_nRecHP;
		int m_nRecSP;
		int m_nRecMP;
		int m_nRecStatus;
		int m_nDecMP;
		int m_nDecSP;
		int m_nAttrAtk;
		int m_nAttrDef;
		int m_pStateBlock[17];
		char m_byAttrAtkBuff[9];
		int m_nItemAttack;
		int m_nItemAttackVar;
		int m_nItemDefense;
		int m_nItemDefenseMG;
		int m_nAttackRange;
		int m_nAttackSpeed;
		int m_nMoveSpeed;
		int m_nCritical;
		int m_nCoolTime;
		int m_nReduceDamage;
		int m_nLanguage;
		int m_nMaxBag;
		char m_byWeaponSpeed[20];
		char m_byWeaponDamage[20];
		int m_nCrossRoad;
		char m_byShieldValue;
		int m_nGiveupSPMP;
		int m_nStop;
		int m_nStun;
		int m_nSleep;
		int m_nTransFormFixMob;
		unsigned __int16 m_wTransFormTargetMobID;
		unsigned int m_dwChaseID;
		char m_byShape;
		int m_nShapeType;
		stCloneUser* m_pCloneUser;
		int m_bBlockAll;
		bool m_bBlockDeath;
		bool m_bBlockDying;
		bool m_bNotSeeMOB;
		stHPAbility m_pHPAbility;
		stAttackAdd m_pAttackAdd[3];
		stAttack m_pAttack[3];
		unsigned int m_dwDisableBySkill;
		int m_nStatus;
		int m_nStatusLife;
		char m_byMotion;
		char m_byMoveMotion;
		int m_bRun;
		int m_nAttackMode;
		int m_nAttackNext;
		int m_nAttackSkill;
		bool m_bPrevSkill;
		int m_nQualityDec;
		unsigned int m_dwNextAttackTime;
		unsigned int m_dwRebirthTime;
		int m_nRebirthType;
		int m_nDropExp;
		int m_nVehicleStatus;
		unsigned int m_dwNextVehicleTime;
		int m_nVehicleType;
		int m_nVehicleTypeAdd;
		unsigned int m_dwVehicle2CharID;
		unsigned int m_dwVehicle2ReqID;
		unsigned int m_dwTimeOverVehicle2;
		unsigned int m_dwPartyCallReqID;
		unsigned int m_dwTimeOverPartyCall;
		unsigned int m_dwNextRecoverTime;
		unsigned int m_dwNextPotionReuseTime[12];
		int m_pKCStatus[28];
		stTarget m_Target;
		stTarget m_TargetPrev;
		BYTE m_atkDamage[0x2c];
		void* m_pUseNpc;
		CExchange m_Exchange;
		CExchangePvP m_ExcPvP;
		stMyShop m_MyShop;
		CParty* m_pParty;
		unsigned int m_dwPartyReqID;
		bool m_bPartyFindReg;
		unsigned int m_dwGuildID;
		int m_nGuildUserLevel;
		unsigned int m_dwGuildReqID;
		unsigned int m_dwGuildMasterReqID;
		void* m_pGuild;
		void* m_pGuildCreate;
		CMiniGame m_MiniGame;
		BYTE m_Friend[0x3d14];
		BYTE m_MoveChk[0x2c];
		unsigned int m_dwGuildJoinRemainHour;
		unsigned int m_dwRankingZoneEnterFlag;
		unsigned int m_dwMoveInsZoneFlag;
		int m_nPvPStatus;
		unsigned int m_dwPvPTime;
		unsigned int m_dwPvPUser;
		SVector m_vPvPPos;
		unsigned int m_dwPvPGuildID;
		SVector m_vPvPGPos;
		unsigned int m_dwPvPGTime;
		bool m_bPvPJoinUser;
		int m_bPvPGuildReady;
		SCriticalSection m_csNProtect;
		bool m_NProtectRecved;
		unsigned int m_NProtectRecvTime;
		void* m_NProtectCSA;
		BYTE m_Crypto[0x234];
		int m_nWhere;
		unsigned int m_dwUID;
		unsigned __int64 m_qwSessionID;
		char m_byUserPermission;
		stAdminInfo m_pAdmin;
		unsigned int m_dwBillingID;
		int m_nServerDBID;
		char m_szUserName[32];
		bool m_bItemAttack;
		bool m_bFlagStateInit;
		int m_bCloseBySession;
		int m_bCloseByDBAgent;
		bool m_bEnableDBCharacterSelect;
		bool m_bEnableDBCharacterRename;
		int m_nHaveQuestKillPC;
		int m_nHaveQuestKillMOB;
		unsigned int m_dwPrevMoveTime;
		unsigned int m_dwQuestLastUseItemTime;
		char m_byQuestLastUseItemID;
		int m_nLogoutType;
		unsigned int m_dwLogoutTime;
		unsigned int m_dwReconnectReady;
		char m_byMoveWarResult;
		unsigned int m_dwAutoCharID;
		char m_byAesKey[32];
		char m_byRealServerID;
		unsigned int m_dwUndefinePacket;
		int m_nJoinDiff;
		int m_nMovePositionType;
		unsigned int m_dwMovePositionTime;
		int m_nMoveMap;
		SVector m_vMovePos;
		unsigned int m_dwTimeOverExc;
		unsigned int m_dwTimeOverParty;
		unsigned int m_dwTimeOverGuild;
		unsigned int m_dwTimeOverFriend;
		bool m_bUseChatItem;
		unsigned int m_dwTimeOverChatItem;
		unsigned int m_dwTimeOverShoutAdmin;
		char m_byResetStatus;
		char m_byResetSkill;
		bool m_bResetStatusWait;
		bool m_bResetSkillWait;
		unsigned int m_dwLockOnTick;
		int m_nUserBillingType;
		unsigned int m_dwBillingRemainTime;
		unsigned int m_dwTermSecCharDisconnected;
		int m_nSavePosUseBag;
		int m_nSavePosUseSlot;
		int m_nSavePosUseIndex;
		unsigned int m_dwItemCoolTimeIndex;
		int m_nSaveMap[4];
		SVector m_vSavePos[4];
		int m_bSkillAmulet;
		int m_bSkillMobGold;
		int m_bSkillNDQuality;
		int m_bSkillNDExp;
		int m_bSkillNDItem;
		int m_bSkillNDItemEQ;
		int m_bSkillEnableBank;
		int m_bSkillBank2X;
		int m_bSkillGetExpUP2X;
		int m_bSkillGetExpUP15X;
		int m_bSkillRebirth;
		int m_bSkillCharRename;
		stPointLog m_PointLog[10];
		unsigned int m_dwCashPoint;
	};

	static_assert(offsetof(CUser, m_dwCharID) == 0x128);
	static_assert(offsetof(CUser, m_bSkillAmulet) == 0x58b0);
	static_assert(sizeof(stUserInfo) == 0x10f8);
	static_assert(sizeof(stQuick) == 0x6);

	static BYTE GetItemType(DWORD ItemObject) {
		return ShaiyaUtility::read<BYTE>(ItemObject + 0x40);
	}

	static BYTE GetItemTypeid(DWORD ItemObject) {
		return ShaiyaUtility::read<BYTE>(ItemObject + 0x41);
	}

	static BYTE GetItemTypeCount(DWORD ItemObject) {
		return  ShaiyaUtility::read<BYTE>(ItemObject + 0x42);
	}

	static DWORD64 GetItemTypeUID(DWORD ItemObject) {
		return ShaiyaUtility::read<DWORD64>(ItemObject + 0x38);
	}

	static void SendPacket(CUser* Player,const void* Buf,DWORD Len) {
		DWORD dwCall = 0x004D4D20;
		_asm {
			push Len
			push Buf
			mov ecx,Player
			call dwCall
		}
	}

	static bool IsEnoughInventoryItems(CUser* Player, DWORD Itemid, int Count)
	{
		if (Count == 0) {
			return true;
		}

		for (DWORD i = 1; i < 6; i++)
		{
			for (DWORD j = 0; j < 24; j++)
			{
				auto item = Player->m_pUserItem[i][j];
				if (item && item->m_pInfo->dwItemID == Itemid)
				{
					Count -= item->m_byCount;
					if (Count <= 0)
						return true;
				}
			}
		}
		return false;
	}


	static void UseItem(const void* Player, BYTE Bag, BYTE Slot, BYTE Count) {

		for (int i = 0; i < Count; i++) {
			DWORD callFunc = 0x00465120;
			auto dwSlot = static_cast<DWORD>(Slot);
			auto dwBag = static_cast<DWORD>(Bag);
			_asm {
				push 0x0      // is move map
				push dwSlot
				mov ebx, dwBag
				mov ecx, Player
				call callFunc
			}
		}
	}


	static void EnumeratePlayerItems(const CUser* Player, std::function<bool(CItem*, BYTE, BYTE)> Func) {

		for (BYTE i = 1; i < 6; i++)
		{
			for (BYTE j = 0; j < 24; j++)
			{
				auto item = Player->m_pUserItem[i][j];
				if (!item) {
					continue;
				}

				if (Func(item, i, j)) {
					return;
				}
			}
		}
	}

	static bool DeletePlayerItemid(const CUser* Player, DWORD Itemid, BYTE Count)
	{

		bool result = false;
		auto func = [&](CItem* PlayerItem, BYTE Bag, BYTE Slot)->bool
		{
			auto itemId = PlayerItem->m_pInfo->dwItemID;
			auto count = PlayerItem->m_byCount;
			if (itemId != Itemid) {
				return false;
			}

			// enough
			if (count > Count) {
				count -= Count;
				UseItem(Player, Bag, Slot, Count);
				result = true;
				return true;
			}
			else {
				Count -= count;
				UseItem(Player, Bag, Slot, count);

				if (Count == 0) {
					result = true;
					return true;
				}

				return false;
			}

		};
		EnumeratePlayerItems(Player, func);
		return result;
	}

}

namespace ShaiyaUtility::EP6 {


	static const int MAX_NOTICE_LENGTH = 0x69;

	static DWORD PlayerUid(void* Player) {
		return ShaiyaUtility::read<DWORD>(DWORD(Player) + 0x582c);
	}

	static DWORD GetPlayerKills(void* Player)
	{
		return ShaiyaUtility::read<DWORD>(DWORD(Player) + 0x148);
	}

	static bool IsPlayerConnected(void* Player)
	{
		return ShaiyaUtility::read<DWORD>(DWORD(Player) + 0xd8)>0;
	}

	static bool IsLegalPlayer(void* Player)
	{
		if(!IsPlayerConnected(Player))
		{
			return false;
		}

		return ShaiyaUtility::read<DWORD>(DWORD(Player) + 0xe0) > 0;
	}

	static DWORD PlayerCharid(void* Player)
	{
		return ShaiyaUtility::read<DWORD>(DWORD(Player) + 0x128);
	}




	static BYTE PlayerCountry(void* Player) {
		return ShaiyaUtility::read<BYTE>(DWORD(Player) + 0x12d);
	}

	static DWORD PlayerParty(void* Player) {
		return ShaiyaUtility::read<DWORD>(DWORD(Player) + 0x17f4);
	}


	static DWORD PlayerGuide(void* Player) {
		return ShaiyaUtility::read<DWORD>(DWORD(Player) + 0x1810);
	}
	static WORD PlayerMap(void* Player) {
		return ShaiyaUtility::read<WORD>(DWORD(Player) + 0x160);
	}

	static void SetMounted(void* Player) {
		return ShaiyaUtility::write<DWORD>(DWORD(Player) + 0x147c,2);
	}


	static DWORD PlayerItemToItemId(DWORD ItemObject) {
		auto ItemAddr = ShaiyaUtility::read<DWORD>(ItemObject + 0x30);
		return ShaiyaUtility::read<DWORD>(ItemAddr);
	}
	static BYTE PlayerItemToCount(DWORD ItemObject) {
		return ShaiyaUtility::read<BYTE>(ItemObject + 0x42);
	}

	static BYTE GetPlayerFamily(const void* Player) {
		return ShaiyaUtility::read<BYTE>(DWORD(Player) + 0x12e);
	}
	static BYTE GetPlayerJob(const void* Player) {
		return ShaiyaUtility::read<BYTE>(DWORD(Player) + 0x134);
	}

	static DWORD GetInventoryItem(const void* Player, DWORD Bag, DWORD Slot) {
		auto offset = (Bag * 24 + Slot) * 4;
		return  ShaiyaUtility::read<DWORD>((DWORD)Player + offset + 0x1c0);
	}

	static DWORD ItemObjToItemid(DWORD ItemObj) {
		return ShaiyaUtility::read<DWORD>(ItemObj);
	}

	static DWORD GetPlayerSkillNTotal(const void* Player) {
		return ShaiyaUtility::read<DWORD>(DWORD(Player) + 0xabc);
	}


	static DWORD GetPlayerSkillObj(const void* Player, DWORD SkillSlotIdx) {
		auto ret = ShaiyaUtility::read<DWORD>(DWORD(Player) + 0xac0 + SkillSlotIdx * 4);
		return ret;

	}

	static DWORD GetSkillObjUid(DWORD SkillObj) {
		return ShaiyaUtility::read<DWORD>(SkillObj + 0x8);
	}

	static DWORD GetSkillInfo(DWORD SkillObj) {
		return ShaiyaUtility::read<DWORD>(SkillObj + 0x34);
	}

	static bool CheckSkillUsableForJob(DWORD SkillInfo,DWORD Job) {
		return ShaiyaUtility::read<BYTE>(SkillInfo + 0x27 + Job);
	}

	static WORD GetSkillId(DWORD SkillInfo)
	{
		return ShaiyaUtility::read<WORD>(SkillInfo);
	}

	static void SetPlayerStatus(void* player,int value){
		ShaiyaUtility::write<int>(reinterpret_cast<DWORD>(player) + 0x5808,value);
	}

	static int GetPlayerStatus(void* player) {
		return ShaiyaUtility::read<int>(reinterpret_cast<DWORD>(player) + 0x5808);
	}

	static bool SendGmCommand(void* player,void* packet) {

		auto oldStatus = GetPlayerStatus(player);
		SetPlayerStatus(player, 1);

		// gm cmd call
		DWORD dwCall = 0x00483430;
		_asm {
			push player
			mov ecx, packet
			call  dwCall
		}
		SetPlayerStatus(player, oldStatus);
	}

	static void MovePlayer(void* p,WORD m,float x,float y,float z) {



		
#pragma pack(push,1)
		struct Packet {
			WORD cmd = 0xfa0a;
			float x{};
			float z{};
			WORD m{};
		};
#pragma pack(pop)

		Packet packet;
		packet.x = x;
		packet.z = z;
		packet.m = m;

		SendGmCommand(p, &packet);
	}

	static void KickPlayer(void* p)
	{
		DWORD dwCall = 0x004EC760;
		_asm{
			push 0x0
			push 0x9
			mov ecx,p
			call dwCall
		}
	}

	
	static void RemoveSkill(const void* Player,DWORD SkillUid) {

		DWORD dwCall = 0x00493EF0;

		_asm {
			push SkillUid
			mov ecx,Player
			call dwCall
		}
	}

	static void SetItemCount(DWORD ItemObject, BYTE Count) {
		ShaiyaUtility::write<BYTE>(ItemObject + 0x42, Count);
	}


	static void EnumeratePlayerItems(const void* Player, std::function<bool(DWORD, BYTE, BYTE)> Func) {

		for (BYTE i = 1; i < 6; i++)
		{
			for (BYTE j = 0; j < 24; j++)
			{
				auto item = GetInventoryItem(Player, i, j);
				if (!item) {
					continue;
				}

				if (Func(item, i, j)) {
					return;
				}
			}
		}
	}

	static bool IsEnoughInventoryItems(const void* Player, DWORD Itemid, int Count)
	{
		if (Count == 0) {
			return true;
		}

		// i  starts with 1
		for (DWORD i = 1; i < 6; i++)
		{
			for (DWORD j = 0; j < 24; j++)
			{
				auto item = GetInventoryItem(Player, i, j);
				if (item && PlayerItemToItemId(item) == Itemid)
				{
					Count -= PlayerItemToCount(item);
					if (Count <= 0)
						return true;
				}
			}
		}
		return false;
	}

	static void UseItem(const void* Player, BYTE Bag, BYTE Slot, BYTE Count) {

		for (int i = 0; i < Count; i++) {
			DWORD callFunc = 0x004728e0;
			auto dwSlot = static_cast<DWORD>(Slot);
			auto dwBag = static_cast<DWORD>(Bag);
			_asm {
				push 0x0
				push dwSlot
				push dwBag
				mov ecx, Player
				call callFunc
			}
		}
	}

	static void DeletePlayerItem(const void* Player, BYTE Bag, BYTE Slot) {

		DWORD callFunc = 0x0046C4C0;
		auto dwSlot = static_cast<DWORD>(Slot);
		auto dwBag = static_cast<DWORD>(Bag);
		_asm {
			mov ebx, dwSlot
			push dwBag
			mov ecx, Player
			call callFunc
		}
	}

	static bool DeletePlayerItemid(const void* Player, DWORD Itemid, BYTE Count)
	{

		bool result = false;
		auto func = [&](DWORD PlayerItem, BYTE Bag, BYTE Slot)->bool
		{
			auto itemId = PlayerItemToItemId(PlayerItem);
			auto count = PlayerItemToCount(PlayerItem);
			if (itemId != Itemid) {
				return false;
			}

			// enough
			if (count > Count) {
				count -= Count;
				UseItem(Player, Bag, Slot, Count);
				result = true;
				return true;
			}
			else {
				Count -= count;
				UseItem(Player, Bag, Slot, count);

				if (Count == 0) {
					result = true;
					return true;
				}

				return false;
			}

		};
		EnumeratePlayerItems(Player, func);
		return result;
	}

	static void SendPacketAll(const void* Buffer, DWORD BufferLength) {

		DWORD esiAddr = *PDWORD(0x587960);
		DWORD sendAllCall = 0x00419120;
		if (esiAddr) {
			_asm
			{
				mov eax, BufferLength
				mov ecx, Buffer
				mov esi, esiAddr
				call sendAllCall
			}
		}
	}

	static void SendToPlayer(const void* Player, void* Buffer, DWORD BufferLength) {
		PVOID g_pSendPacketCall = reinterpret_cast<void*>(0x004ED0E0);
		_asm
		{
			mov ecx, Player
			push BufferLength
			push Buffer
			call g_pSendPacketCall
		}
	}

	template<class T>
	static void SendToPlayer(const void* Player, T* Buffer) {

		DWORD BufferLength = sizeof(T);
		PVOID g_pSendPacketCall = reinterpret_cast<void*>(0x004ED0E0);
		_asm
		{
			mov ecx, Player
			push BufferLength
			push Buffer
			call g_pSendPacketCall
		}
	}

	static DWORD Itemid2ItemAddr(DWORD Itemid) {
		DWORD itemType = Itemid / 1000;
		DWORD itemTypeid = Itemid % 1000;
		DWORD callAddr = 0x004059b0;
		DWORD result = 0;
		_asm {
			mov eax, itemType
			mov ecx, itemTypeid
			call callAddr
			mov result, eax
		}
		return result;
	}

	static void SendMsgToPlayer(void* Player, const char* Msg)
	{
		ShaiyaUtility::Packet::Notice buffer;
		buffer.CopyMsg(Msg);
		SendToPlayer(Player, &buffer, buffer.length());
	}

	static std::string Itemid2Name(DWORD Itemid)
	{
		DWORD itemAddr = Itemid2ItemAddr(Itemid);
		return std::string(PCHAR(itemAddr + 0x4));
	}

	static std::string PlayerName(void* P) {
		return std::string(PCHAR(P) + 0x184);
	}

	static bool SendItem(void* Player, DWORD ItemId, BYTE Count) {

		DWORD itemAddr = Itemid2ItemAddr(ItemId);
		DWORD callAddr = 0x0046bd10;
		bool result = false;
		DWORD dwCount = Count;
		_asm {
			push dwCount
			push itemAddr
			mov ecx, Player
			call callAddr
			mov result, al
		}
		return result;
	}

	static void SendMsgToAll(const char* Msg) {

		ShaiyaUtility::Packet::Notice buffer;
		buffer.CopyMsg(Msg);
		DWORD dwEsi = *PDWORD(0x587960);
		if (!dwEsi) {
			return;
		}
		auto len = buffer.length();
		auto callAddr = 0x00419120;

		auto p = &buffer;

		_asm {
			mov esi, dwEsi
			mov ecx, p
			mov eax, len
			call callAddr
		}
	}

	static void SendMounted(void* Player) {
		BYTE packet[] = { 0x16,0x2,0x1,0x1 };
		return SendToPlayer(Player, packet, sizeof(packet));
	}


	static DWORD AddPlayerKills(void* Player, DWORD Kills)
	{
		auto old = ShaiyaUtility::read<DWORD>(DWORD(Player) + 0x148);
		auto newKills = old + Kills;

		ShaiyaUtility::write(DWORD(Player) + 0x148, newKills);

#pragma pack(push)
#pragma pack(1)
		struct updateKillPacket
		{
			WORD header = 0x60c;
			DWORD uid{};
			BYTE unk{};
			DWORD kills{};
		};
		// inform player
		struct updateClientKill
		{
			WORD header = 0xf701;
			BYTE Type = 0x11;
			DWORD kills{};
		};
#pragma pack(pop)

		updateKillPacket packet;
		packet.kills = newKills;
		packet.uid = PlayerUid(Player);
		void* p = &packet;
		DWORD dwCall = 0x004ED2D0;
		_asm {
			push 0xb
			push p
			mov ecx, dword ptr ds : [0x58796C]
			call dwCall
		}
		updateClientKill clientPacket;
		clientPacket.kills = newKills;
		ShaiyaUtility::EP6::SendToPlayer(Player, &clientPacket, sizeof(updateClientKill));
	}

}

namespace Utility {

	static void AppendDataToVector(std::vector<UCHAR>* V, void* Data, size_t L) {
		V->insert(V->end(), static_cast<PUCHAR>(Data), static_cast<PUCHAR>(Data) + L);
	}


	static const std::string base64_chars =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";


	static inline bool is_base64(unsigned char c) {
		return (isalnum(c) || (c == '+') || (c == '/'));
	}

	static std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
		std::string ret;
		int i = 0;
		int j = 0;
		unsigned char char_array_3[3];
		unsigned char char_array_4[4];

		while (in_len--) {
			char_array_3[i++] = *(bytes_to_encode++);
			if (i == 3) {
				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for (i = 0; (i < 4); i++)
					ret += base64_chars[char_array_4[i]];
				i = 0;
			}
		}

		if (i)
		{
			for (j = i; j < 3; j++)
				char_array_3[j] = '\0';

			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

			for (j = 0; (j < i + 1); j++)
				ret += base64_chars[char_array_4[j]];

			while ((i++ < 3))
				ret += '=';

		}

		return ret;

	}

	static std::string base64_decode(std::string const& encoded_string) {
		int in_len = encoded_string.size();
		int i = 0;
		int j = 0;
		int in_ = 0;
		unsigned char char_array_4[4], char_array_3[3];
		std::string ret;

		while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
			char_array_4[i++] = encoded_string[in_]; in_++;
			if (i == 4) {
				for (i = 0; i < 4; i++)
					char_array_4[i] = base64_chars.find(char_array_4[i]);

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (i = 0; (i < 3); i++)
					ret += char_array_3[i];
				i = 0;
			}
		}

		if (i) {
			for (j = 0; j < i; j++)
				char_array_4[j] = base64_chars.find(char_array_4[j]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

			for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
		}

		return ret;
	}
}

namespace Utility::Image {

	static HRESULT CaptureAnImage(_Out_ std::vector<UCHAR>* Output, HWND hWnd = nullptr)
	{
		HRESULT hr = S_OK;

		Output->clear();

		HDC hdcScreen;
		HDC hdcWindow;
		HDC hdcMemDC = NULL;
		HBITMAP hbmScreen = NULL;
		BITMAP bmpScreen;
		BITMAPFILEHEADER   bmfHeader{};
		BITMAPINFOHEADER   bi{};
		RECT rcClient{};

		// Retrieve the handle to a display device context for the client 
		// area of the window. 
		hdcScreen = GetDC(NULL);
		hdcWindow = GetDC(hWnd);

		do
		{
			// Create a compatible DC which is used in a BitBlt from the window DC
			hdcMemDC = ::CreateCompatibleDC(hdcWindow);
			if (!hdcMemDC)
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
				break;
			}

			// Get the client area for size calculation

			GetClientRect(hWnd, &rcClient);

			//This is the best stretch mode
			SetStretchBltMode(hdcWindow, HALFTONE);

			//The source DC is the entire screen and the destination DC is the current window (HWND)
			if (!StretchBlt(hdcWindow,
				0, 0,
				rcClient.right, rcClient.bottom,
				hdcScreen,
				0, 0,
				GetSystemMetrics(SM_CXSCREEN),
				GetSystemMetrics(SM_CYSCREEN),
				SRCCOPY))
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
				break;
			}

			// Create a compatible bitmap from the Window DC
			hbmScreen = CreateCompatibleBitmap(hdcWindow, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

			if (!hbmScreen)
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
				break;
			}

			// Select the compatible bitmap into the compatible memory DC.
			SelectObject(hdcMemDC, hbmScreen);

			// Bit block transfer into our compatible memory DC.
			if (!BitBlt(hdcMemDC,
				0, 0,
				rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
				hdcWindow,
				0, 0,
				SRCCOPY))
			{
				hr = HRESULT_FROM_WIN32(GetLastError());
				break;
			}

			// Get the BITMAP from the HBITMAP
			GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

			bi.biSize = sizeof(BITMAPINFOHEADER);
			bi.biWidth = bmpScreen.bmWidth;
			bi.biHeight = bmpScreen.bmHeight;
			bi.biPlanes = 1;
			bi.biBitCount = 32;
			bi.biCompression = BI_RGB;
			bi.biSizeImage = 0;
			bi.biXPelsPerMeter = 0;
			bi.biYPelsPerMeter = 0;
			bi.biClrUsed = 0;
			bi.biClrImportant = 0;

			DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

			// Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
			// call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
			// have greater overhead than HeapAlloc.
			HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
			char* lpbitmap = (char*)GlobalLock(hDIB);

			// Gets the "bits" from the bitmap and copies them into a buffer 
			// which is pointed to by lpbitmap.
			GetDIBits(hdcWindow, hbmScreen, 0,
				(UINT)bmpScreen.bmHeight,
				lpbitmap,
				(BITMAPINFO*)&bi, DIB_RGB_COLORS);

			// Add the size of the headers to the size of the bitmap to get the total file size
			DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

			//Offset to where the actual bitmap bits start.
			bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

			//Size of the file
			bmfHeader.bfSize = dwSizeofDIB;

			//bfType must always be BM for Bitmaps
			bmfHeader.bfType = 0x4D42; //BM   

			Utility::AppendDataToVector(Output, &bmfHeader, sizeof(BITMAPFILEHEADER));
			Utility::AppendDataToVector(Output, &bi, sizeof(BITMAPINFOHEADER));
			Utility::AppendDataToVector(Output, &lpbitmap, dwBmpSize);


			//Unlock and Free the DIB from the heap
			GlobalUnlock(hDIB);
			GlobalFree(hDIB);

		} while (false);

		//Clean up
		DeleteObject(hbmScreen);
		DeleteObject(hdcMemDC);
		ReleaseDC(NULL, hdcScreen);
		ReleaseDC(hWnd, hdcWindow);

		return 0;
	}

	static HRESULT CaptureAnImageFile(const std::wstring& FilePath, HWND hWnd = nullptr) {

		HRESULT hr = S_OK;
		std::vector<UCHAR> data;

		hr = CaptureAnImage(&data, hWnd);
		if (FAILED(hr)) {
			return hr;
		}

		return hr;
	}
}

namespace Utility::WMI {

	static HRESULT QueryWMI(
		_In_ const std::wstring WmiQuery,
		_In_ const std::wstring PropNameOfResultObject,
		_Inout_ std::vector<std::wstring>* Output,
		_In_opt_ bool AllowEmptyItems = false)
	{

		HRESULT hr = S_OK;
		bool isCoInitialized = false;

		do
		{
			hr = CoInitializeEx(0, COINIT_MULTITHREADED);
			if (FAILED(hr)) {
				break;
			}
			isCoInitialized = true;

			hr = CoInitializeSecurity(
				NULL,
				-1,                          // COM authentication
				NULL,                        // Authentication services
				NULL,                        // Reserved
				RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
				RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
				NULL,                        // Authentication info
				EOAC_NONE,                   // Additional capabilities 
				NULL                         // Reserved
			);
			if (FAILED(hr)) {
				break;
			}


			ATL::CComPtr <IWbemLocator> loc;
			hr = CoCreateInstance(
				CLSID_WbemLocator,
				0,
				CLSCTX_INPROC_SERVER,
				IID_IWbemLocator, (LPVOID*)&loc);
			if (FAILED(hr)) {
				break;
			}

			ATL::CComPtr <IWbemServices> svc;
			hr = loc->ConnectServer(
				_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
				NULL,                    // User name. NULL = current user
				NULL,                    // User password. NULL = current
				0,                       // Locale. NULL indicates current
				NULL,                    // Security flags.
				0,                       // Authority (for example, Kerberos)
				0,                       // Context object 
				&svc                     // pointer to IWbemServices proxy
			);
			if (FAILED(hr)) {
				break;
			}

			hr = CoSetProxyBlanket(
				svc,                        // Indicates the proxy to set
				RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
				RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
				NULL,                        // Server principal name 
				RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
				RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
				NULL,                        // client identity
				EOAC_NONE                    // proxy capabilities 
			);
			if (FAILED(hr)) {
				break;
			}

			ATL::CComPtr <IEnumWbemClassObject> enumerator;
			hr = svc->ExecQuery(
				bstr_t("WQL"),
				bstr_t(WmiQuery.c_str()),
				WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
				NULL,
				&enumerator);
			if (FAILED(hr)) {
				break;
			}

			Output->clear();
			while (enumerator) {

				ATL::CComPtr<IWbemClassObject> clsobj;
				ULONG uReturn = 0;
				HRESULT hr = enumerator->Next(WBEM_INFINITE, 1,
					&clsobj, &uReturn);
				if (0 == uReturn) {
					break;
				}

				VARIANT vtProp;
				hr = clsobj->Get(PropNameOfResultObject.c_str(), 0, &vtProp, 0, 0);
				if (FAILED(hr)) {
					break;
				}

				BSTR val = vtProp.bstrVal;
				if (nullptr == val && AllowEmptyItems) {
					Output->push_back(L"");
				}
				else {
					Output->push_back(std::wstring(val));
				}

				VariantClear(&vtProp);
			}

		} while (false);

		if (isCoInitialized) {
			CoUninitialize();
		}

		return hr;
	}

	static HRESULT GetSerialNumber(_Inout_ std::vector<std::wstring>* Output) {

		HRESULT hr = S_OK;

		std::vector<std::wstring> items;
		hr = QueryWMI(L"SELECT SerialNumber FROM Win32_PhysicalMedia", L"SerialNumber", &items);
		if (FAILED(hr)) {
			return hr;
		}
		*Output = items;

		return hr;
	}

	static HRESULT GetCpuId(_Inout_ std::vector<std::wstring>* Output) {

		HRESULT hr = S_OK;

		std::vector<std::wstring> items;
		hr = QueryWMI(L"SELECT ProcessorId FROM Win32_Processor", L"ProcessorId", &items);
		if (FAILED(hr)) {
			return hr;
		}
		*Output = items;

		return hr;
	}


	


}
#endif