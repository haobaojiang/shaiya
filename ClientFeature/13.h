
#include <windows.h>
#include "../utility/utility.h"

namespace Shaiya13
{
	namespace SkillCutting {

		ShaiyaUtility::CMyInlineHook g_fully;


		ShaiyaUtility::CMyInlineHook g_hook;
		DWORD g_skipAddr = 0x0045A289;
		__declspec(naked) void Naked()
		{
			_asm
			{
				cmp al, 0x2f    // normal attack
				jne _org

				mov eax, 0x221d000
				mov eax, dword ptr[eax + 0x140da4]
				mov dword ptr[eax + 17c], 0x0
				jmp g_skipAddr

				_org :
				mov eax, 000000C7
					jmp g_hook.m_pRet
			}
		}

		void start(void*) {
			Sleep(10 * 2000);
			g_fully.Hook(reinterpret_cast<void*>(0x0045a260), Naked, 5);

		}
	}

	void Start()
	{
		_beginthread(SkillCutting::start, 0, 0);
	}
}

