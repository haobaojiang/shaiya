#pragma once
#ifndef SHAIYA_INSTANT_MOUNT_HEADER
#define SHAIYA_INSTANT_MOUNT_HEADER


#include "stdafx.h"
#include "config.hpp"
#include "../utility/utility.h"


namespace InstantMount {

	
	BOOLEAN g_ids[255] = { FALSE };

	bool __stdcall IsInstantMount(DWORD itemObject) {
		auto typeID = ShaiyaUtility::read<BYTE>(itemObject + 0x25);
		return g_ids[typeID];
	}


	ShaiyaUtility::CMyInlineHook obj;
	__declspec(naked) void Naked()
	{
		_asm
		{
			pushad
			MYASMCALL_1(IsInstantMount,eax)
			test al, al
			popad
			je _org
			jmp obj.m_pRet

				_org :
			mov dword ptr ss : [ebp + 0x1464] , esi
				jmp obj.m_pRet
		}
	}

	void Start() {


		if (!GameConfiguration::IsFeatureEnabled("instantMount")) {
			return;
		}

		for (int i = 0; i < 100; i++) {
			auto itemid = GameConfiguration::GetInteger("instantMount", ShaiyaUtility::itos(i));

		

			if (itemid == 0) {
				continue;
			}

			auto typeID = ShaiyaUtility::itemID2TypeId(itemid);
			if (typeID == 0) {
				continue;
			}

			LOGD << "typeid: " << typeID << "\n";

			g_ids[typeID] = TRUE;
		}

		
		obj.Hook(0x00469515, Naked, 6);
	}
}

#endif