
#ifndef SHAIYA_LOGIN_HEADER
#define SHAIYA_LOGIN_HEADER

#include "stdafx.h"
#include "config.hpp"
#include <mutex>


namespace onLogin{


	ShaiyaUtility::CMyInlineHook g_objToonselection;
	std::vector<std::function<void(void*)>> g_onLoginCallBacks;




	void __stdcall fun(void* player) {

		// ZeroMemory(&player->nextMalltime, sizeof(shaiya_player) - offsetof(shaiya_player, nextMalltime));
		for (auto const& iter : g_onLoginCallBacks) {
			iter(player);
		}
	}


	__declspec(naked) void  Naked_toonSelection()
	{
		_asm {
			pushad
			MYASMCALL_1(fun, ebp)
			popad
			mov word ptr [esp+0x7c],cx
			jmp g_objToonselection.m_pRet
		}
	}

	void AddCallBack(std::function<void(void*)> f) {
		g_onLoginCallBacks.push_back(f);
	}

	void Start(){
		g_objToonselection.Hook(reinterpret_cast<void*>(0x0047b4c3), Naked_toonSelection, 5);
	}

}





#endif