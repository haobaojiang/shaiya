#include "stdafx.h"
#include "ChatFilter.h"






namespace CharFilter
{
	CMyInlineHook obj1;



	//过滤所有类型的说话内容
	bool __stdcall Filter(Pshaiya_player Player, PBYTE pPackets)
	{

		bool bRet = true;
		char* strChat = (char*)&pPackets[3];
		for (auto iter=g_vecChatCallBack.begin();iter!=g_vecChatCallBack.end();iter++){
			(*iter)(Player, strChat, &bRet);
		}
		return bRet;
	}

	DWORD falledAddr = 0x0047FC57;
	__declspec(naked) void  Naked_ChatFilter()
		{
			_asm
			{
				pushad
				MYASMCALL_2(Filter,ebp,ebx)
					test al,al
					popad
					jne _Org
					jmp falledAddr

					_Org:
				movzx eax, byte ptr[eax + 0x47FCA8]
					jmp obj1.m_pRet
			}
		}


	 
	

	void Start()
	{
	    obj1.Hook((PVOID)0x0047f4ba, Naked_ChatFilter, 7);
	}
}
