#ifndef _CFFA
#define _CFFA





class CFFA
{
public:
	CFFA(WCHAR* appname);
	~CFFA();

	typedef struct {
		WORD dayOfweek;
		WORD hour;
		WORD minute;
		WORD duration;
		WORD beginDay;
		WORD endDay;
	}FFPTASK, *PFFPTASK;


public:
	bool process(Pshaiya50_player player, WORD Map, Pshaiya_pos pos);
	void loadConfig();
	void killLog(Pshaiya50_player player,Pshaiya50_player);
	void addKills(Pshaiya50_player player);
	void outPut();
	bool ispartyAble(Pshaiya50_player srcPlayer, Pshaiya50_player dstPlayer);
	bool isSameFaction(Pshaiya50_player Attacker, Pshaiya50_player Target);
	bool isFFAMap(WORD Map);
	BYTE getFaction(Pshaiya50_player playerA, Pshaiya50_player playerB);

public:
	bool isAttackTime();
	bool isgateTime();
	bool isligalTime();
	bool isExistedMember(Pshaiya50_player player);
	bool addParty(Pshaiya50_player player);
	PPARTYKILLRECORD getsignedParty(Pshaiya_party party);
	
	void lock();
	void unlock();

public:
	std::vector<PARTYKILLRECORD> m_Party;
	CRITICAL_SECTION m_cs;
	WORD m_Map;

	bool m_isAttackTime[7][24][60];
	bool m_IsgateTime[7][24][60];

	WCHAR m_appName[MAX_PATH];
	DWORD m_maxPartys;
	DWORD m_minmembers;
	DWORD m_minlevel;
	DWORD m_kills;
	DWORD m_maxDeadCount;
	BYTE m_country;
	vector<FFPTASK> m_vectasks;
};



#endif






