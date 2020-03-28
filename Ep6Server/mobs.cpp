#include "stdafx.h"
#include "mobs.h"


 

namespace enumMobs
{
#pragma pack(1)

	typedef struct {
		DWORD id;
		DWORD count;
		DWORD remainCount;
		DWORD refreshTime;
		DWORD mobAddr;
		DWORD unk1;
		DWORD unk2;
		DWORD unk3;
		DWORD unk4;
	}SvmapMobDetail,*PSvmapMobDetail;

	typedef struct {
		float x1;
		float y1;
		float z1;
		float x2;
		float y2;
		float z2;
		DWORD unk1;
		DWORD unk2;
		DWORD rowCount;
		PSvmapMobDetail mobDetail;
	}SvmapMob,*PSvmapMob;


	typedef struct {
		BYTE unk[0x2dc];
		DWORD count;
		PSvmapMob mobs;
	}mapObj,*PmapObj;

#pragma pack()


	void CheckMob(PmapObj svMapMobs) {
		if (!svMapMobs) {
			return;
		}

		for (DWORD i = 0; i < svMapMobs->count; i++) {

			PSvmapMob pMobs = svMapMobs->mobs;
			pMobs += i;
			for (DWORD j = 0; j < pMobs->rowCount;j++) {
				PSvmapMobDetail mobDetail = pMobs->mobDetail;	
#ifdef _DEBUG
		//		MyLog("mobid:%f,%f,%f,%d\n", pMobs->x1,pMobs->y1,pMobs->z1,mobDetail->id);
#endif
			}
		}

	}


	void fun_enum(DWORD Addr,DWORD baseAddr) {
		if (!Addr||baseAddr== Addr) {
			return;
		}


		CheckMob(PmapObj(*PDWORD(Addr + 0x8)));
		fun_enum(*PDWORD(Addr + 0x4),baseAddr);  //next
	}

	void RemoveRandomTime() {
		BYTE temp[] = { 0x90,0x90 };
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004287c6, temp, sizeof(temp));
		MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x00428817, temp, sizeof(temp));
	}


	void start()
	{
		Sleep(10000);
		DWORD baseAddr = *PDWORD(0x0109ed30 + 0x14);
		DWORD addr = *PDWORD(baseAddr + 0x4);
		fun_enum(addr, baseAddr);
	}
}





