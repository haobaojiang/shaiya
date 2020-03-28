#include <MyClass.h>
#include <windows.h>
#include <MyInlineHook.h>

#include <time.h>
#include <common.h>

namespace Shaiya90
{

	namespace moveRune {
		void Start() {

			{
				BYTE temp[] = { 0xe9,0x66,0xff,0xff,0xff,0x90 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004CEDAD, temp, sizeof(temp));
			}

		}
	}

	namespace namecolor
	{
		bool g_bEnable = false;
		CMyInlineHook  objNormal, ObjParty, objEnemy;
		CRITICAL_SECTION g_cs;
		cmd_color g_stcColor[200];

		void __stdcall ChangeColor(DWORD, PDWORD);
		PVOID g_pfunChangeColor = ChangeColor;

		void __stdcall ChangeColor(DWORD dwPlayer, PDWORD pColor)
		{
			DWORD dwCharid = *PDWORD(dwPlayer + 0x34);
			EnterCriticalSection(&g_cs);
			for (DWORD i = 0; i < _countof(g_stcColor); i++)
			{
				if (!g_stcColor[i].dwCharid)
					break;

				if (g_stcColor[i].dwCharid == dwCharid)
				{
					*pColor = g_stcColor[i].dwRGB;
					break;
				}
			}
			LeaveCriticalSection(&g_cs);
		}


		void fun_getvipcolor(PVOID packet)
		{
			Pcmd_color pColor = Pcmd_color(PNAMECOLOR(packet)->colors);


			EnterCriticalSection(&g_cs);
			ZeroMemory(g_stcColor, sizeof(g_stcColor));
			for (DWORD i = 0; i < _countof(g_stcColor); i++)
			{
				if (!pColor[i].dwCharid)
					break;

				g_stcColor[i].dwCharid = pColor[i].dwCharid;
				g_stcColor[i].dwRGB = pColor[i].dwRGB;
			}
			LeaveCriticalSection(&g_cs);
		}

		__declspec(naked) void Naked_Normal()
		{
			_asm
			{
				mov dword ptr ss : [esp + 0x18], 0xffffffff
				pushad
				lea eax, [esp + 0x38]
				push eax //color
				push esi
				call g_pfunChangeColor
				popad
				sub eax, 0x4
				jmp objNormal.m_pRet
			}
		}
		__declspec(naked) void Naked_Party()
		{

			_asm
			{
				mov dword ptr ss : [esp + 0x18], 0xFF00FF00
				pushad
				lea eax, [esp + 0x38]
				push eax //color
				push esi
				call g_pfunChangeColor
				popad
				jmp ObjParty.m_pRet
			}
		}

		void Start()
		{
			InitializeCriticalSection(&g_cs);
			//正常角色名字颜色地方
			objNormal.Hook((PVOID)0x0044b5c6, Naked_Normal, 11);
			//组队
			ObjParty.Hook((PVOID)0x0044b664, Naked_Party, 8);
		}
	}
	namespace custompacket
	{
		CMyInlineHook objPacket;
		void __stdcall CustomPacket(PVOID pCmd);
		PVOID g_punCustomPacket = CustomPacket;

		void __stdcall CustomPacket(PVOID pCmd)
		{
			WORD cmd = *PWORD(pCmd);
			switch (cmd)
			{
				//总之只有接到时钟才算
			case PacketOrder::Clock:
				//	fun_clock((Pcmd_clock)++pCmd);
				break;
			case PacketOrder::VipColor:
				namecolor::fun_getvipcolor(pCmd);
				break;
			default:
				break;
			}
		}

		__declspec(naked) void Naked1()
		{
			_asm
			{
				cmp eax, 0xff00
				jl _orginal
				pushad
				mov eax, [esp + 0x28]
				push eax            //packets
				call g_punCustomPacket
				popad
				_orginal :
				sub eax, 0x0000FA08
					jmp objPacket.m_pRet
			}
		}

		void Start()
		{
			//自定义封包处理的地方
			objPacket.Hook((PVOID)0x0059bd2d, Naked1, 5);
		}
	}
	
	namespace setItem {

		CMyInlineHook g_obj, g_objDraw;

		const char* g_pstrItemDecription = NULL;
		DWORD g_isEnable = false;
		DWORD g_x = 0;
		DWORD g_y = 0;
		DWORD g_height = 0;
		DWORD g_width = 0;



		DWORD dwCallDrawBG = 0x0050D4D0;
		
		const char* getsetItemDescription(BYTE itemType, BYTE itemTypeid) {
			return "123456\n778899";
		}



		void drawBackground(DWORD x, DWORD y, DWORD width, DWORD height) {
			_asm {
				push 0x0
				push 0xCD1D1F24
				push height
				push width
				push y
				push x
				call dwCallDrawBG
				add esp,0x18
			}
		}
		
		void drawWords(char* strWords,DWORD x,DWORD y) {
			char strTemp[MAX_PATH] = { 0 };
			DWORD temp = (DWORD)strTemp;
			DWORD dwCall = 0x004A1130;
			_asm {
				push strWords
				push 0x0
				push 0x0
				push 0xff
				push temp
				push 0x752F31FF  //rgb
				push y
				push x
				call dwCall
				add esp,0x20

			}
		}

		void __stdcall drawsetItemSub() {

			if (!g_isEnable)
				return;

			char* ItemDecription = new char[strlen(g_pstrItemDecription) * 2];
			strcpy(ItemDecription, g_pstrItemDecription);

	/*		g_x = g_x - g_width - 10;*/

			g_x = g_x + g_width + 10;


			drawBackground(g_x, g_y, g_width, g_height);

			g_x += 10; //不要直接靠左
			g_y += 15;

			char *delim = "\n";
			char* p = strtok(ItemDecription, delim);
			while (p) {
				g_y += 15;
				drawWords(p, g_x, g_y);	
				p = strtok(NULL, delim);

			}
			delete[] ItemDecription;
		}


		void __stdcall drawsetItem() {
			if (g_isEnable) {
				g_x = g_x-g_width-10;
				drawsetItemSub();
			}
		}

		void __stdcall getDrawInfo(DWORD x,DWORD y,DWORD width,DWORD height,DWORD obj) {

			g_isEnable = false;
			BYTE itemType = *PBYTE(obj + 0x4);
			BYTE itemTypeid= *PBYTE(obj + 0x5);
			g_pstrItemDecription = getsetItemDescription(itemType, itemTypeid);

			if (!g_pstrItemDecription) {
				return;
			}

			g_x = x;
			g_y = y;
			g_width = width;
			g_height = height;
			g_isEnable = true;

			drawsetItemSub();
		}


		__declspec(naked) void Naked_drawBackground() {
			_asm {
				cmp dword ptr [esp+0x93c],0x004a6ee3
				jne  _Org

				pushad
				mov ebp,dword ptr [esp+0x38]
				MYASMCALL_5(getDrawInfo,edi,esi,edx,ecx,ebp)
				popad

				_Org:
				call dwCallDrawBG
					jmp g_obj.m_pRet
			}
		}


		DWORD addr2 = 0x00460210;
		__declspec(naked) void Naked_draw() {
			_asm {
				call addr2
				test eax,eax
				je _Org
				pushad
				MYASMCALL(drawsetItem)
				popad
				jmp g_objDraw.m_pRet

			_Org:
				pushad
				MYASMCALL(drawsetItemSub)
				popad
				jmp g_objDraw.m_pRet

			}
		}

		void Start() {
			g_obj.Hook(PVOID(0x004a42d0), Naked_drawBackground, 5);
	/*		g_objDraw.Hook(PVOID(0x004a6e52), Naked_draw, 5);*/
		}
	}
	
	
	namespace common
	{
		void Start()
		{
			namecolor::Start();
			custompacket::Start();
			moveRune::Start();
			setItem::Start();
		}
	}
}



/*

namespace Shaiya90
{
// 	namespace PartyBattle
// 	{
// 		CMyInlineHook obj1, obj2;
// 		void __stdcall ChangeMsg(char* strMsg);
// 		PVOID g_pfunChangeMsg = ChangeMsg;
//
//
// 		cmd_Battle stcBattleInfo;
//
//
// 		void __stdcall ChangeMsg(char* strMsg)
// 		{
// 			if (g_bEnable)
// 			{
// 				strcpy(strMsg, g_strMsg);
// 				g_bEnable = false;
// 			}
// 		}
//
//
// 		DWORD dwEnableBattle = 0x0044bf36;
// 		__declspec(naked) void Naked_CheckMap()
// 		{
// 			_asm
// 			{
// 				cmp byte ptr[0x00903ccc],0x0   //地图不能为0
// 					je  __orgianl
// 					push eax
// 					mov al, stcBattleInfo.Map    //取地图进行匹配
// 					cmp byte ptr[0x00903ccc], al
// 					pop eax
// 					je dwEnableBattle              //如果是同一地图则强行进入Battle模式
//
// 				__orgianl:
// 				cmp dword ptr ds : [0x7B9A70], eax
// 					jmp obj1.m_pRet
// 			}
// 		}
//
// 		DWORD dwSuccessAddr=
// 		__declspec(naked) void Naked_GetEnemyId()
// 		{
// 			_asm
// 			{
// 				cmp byte ptr[0x00903ccc], 0x0   //地图不能为0
// 					je  __orgianl
// 					push eax
// 					mov al, stcBattleInfo.Map
// 					cmp byte ptr[0x00903ccc], al
// 					pop eax
// 					jne __orgianl
//
// 					push eax
// 					push ecx
// 					lea eax,stcBattleInfo.dwEmeny
// 					xor ecx,ecx
// 				_loop:
// 			       cmo dowrd  ptr [eax],edx
// 					je _success
//
//
// 					cmp ecx,0x1e           //循环30次
// 					jl _loop
// 					pop ecx
// 					pop eax
// 					jmp obj2.m_pRet
//
// 				_success:
// 				   pop ecx
// 					   pop eax
// 					   mov ebp, 0xFFFF0000   //改一下红色即可
// 					   jmp obj2.m_pRet
//
//
// 				__orgianl :
// 				cmp edx, dword ptr ds : [0x7B9A74]
// 					jmp obj2.m_pRet
// 			}
// 		}
//
//
//
// 		void Start()
// 		{
// 			//判断是否Battle地图以便开启
// 			obj1.Hook(PVOID(0x0044bf2e), Naked_CheckMap, 6);
//
// 			//获取敌人的char id
// 			obj2.Hook(PVOID(0x0044bf39), Naked_GetEnemyId, 6);
// 		}
// 	}


	namespace custompacket
	{
		CMyInlineHook objPacket;
		void __stdcall CustomPacket(PWORD pCmd);
		PVOID g_punCustomPacket = CustomPacket;

		void __stdcall CustomPacket(PWORD pCmd)
		{
			switch (*pCmd)
			{
				//总之只有接到时钟才算
			case PacketOrder::Clock:
				//	fun_clock((Pcmd_clock)++pCmd);
				break;
			case PacketOrder::VipColor:
			//	namecolor::fun_getvipcolor((Pcmd_color)++pCmd);
				break;
// 			case PacketOrder::Battle:
// 				PartyBattle::GetEnemy();
			default:
				break;
			}
		}

		__declspec(naked) void Naked1()
		{
			_asm
			{
				cmp eax, 0xff00
					jl _orginal
					pushad
					mov eax, [esp + 0x28]
					push eax            //packets
					call g_punCustomPacket
					popad
				_orginal :
				sub eax, 0x0000FA08
					jmp objPacket.m_pRet
			}
		}

		void Start()
		{
			//自定义封包处理的地方
			objPacket.Hook((PVOID)0x0059bd2d, Naked1, 5);
		}
	}

	namespace common
	{
		void Start()
		{

			PartyBattle::Start();
		}
	}
}

*/


//这个利用交易接口的不好用

// namespace Shaiya90
// {
// 	namespace custommessage
// 	{
// 		CMyInlineHook obj1,obj2;
// 		void __stdcall CheckPacket(Pofficial_requestTrade pPacket);
// 		void __stdcall ChangeMsg(char* strMsg);
// 		PVOID g_pfunChangeMsg = ChangeMsg;
// 		PVOID g_pfunCheckPacket = CheckPacket;
// 		char g_strMsg[MAX_PATH] = { 0 };
// 		bool g_bEnable = false;
// 
// 		void __stdcall CheckPacket(Pofficial_requestTrade pPacket)
// 		{
// 			//如果没有ID，说明是自定义消息,然后取消息的字符串
// 			if (pPacket->dwCharid == 0&&!g_bEnable)
// 			{
// 				char* strMsg = PCHAR(DWORD(pPacket) + sizeof(official_requestTrade));
// 				strcpy(g_strMsg, strMsg);
// 				g_bEnable = true;
// 			}
// 		}
// 
// 		void __stdcall ChangeMsg(char* strMsg)
// 		{
// 			if (g_bEnable)
// 			{
// 				strcpy(strMsg, g_strMsg);
// 				g_bEnable = false;
// 			}
// 		}
// 
// 		__declspec(naked) void Naked_CheckPacket()
// 		{
// 			_asm
// 			{
// 				mov ecx, dword ptr[esp+0x8]
// 					push ecx
// 					pushad
// 					push ecx
// 					call g_pfunCheckPacket
// 					popad
// 					jmp obj1.m_pRet
// 			}
// 		}
// 
// 		__declspec(naked) void Naked1_ChangeMsg()
// 		{
// 			_asm
// 			{
// 				mov ecx, dword ptr[esi+0x310]
// 					pushad
// 					push eax
// 					call g_pfunChangeMsg
// 					popad
// 					jmp obj2.m_pRet
// 			}
// 		}
// 
// 
// 		void Start()
// 		{
// 			obj1.Hook(PVOID(0x0059d9dd), Naked_CheckPacket, 5);
// 			obj2.Hook(PVOID(0x004910F5), Naked1_ChangeMsg, 6);
// 		}
// 	}
// 
// 	namespace common
// 	{
// 		void Start()
// 		{
// 			custommessage::Start();
// 		}
// 	}
// }









//ep6.3

// 		__declspec(naked) void Naked_CheckPacket()
// 		{
// 			_asm
// 			{
// 				mov edx, dword ptr[esp+0x8]
// 					push edx
// 					pushad
// 					push edx
// 					call g_pfunCheckPacket
// 					popad
// 					jmp obj1.m_pRet
// 			}
// 		}
// 
// 		__declspec(naked) void Naked1_ChangeMsg()
// 		{
// 			_asm
// 			{
// 				mov ecx, dword ptr[esi+0x388]
// 					pushad
// 					push eax
// 					call g_pfunChangeMsg
// 					popad
// 					jmp obj2.m_pRet
// 			}
// 		}
// 
// 		void Start()
// 		{
// 			obj1.Hook(PVOID(0x005d530f), Naked_CheckPacket, 5);
// 			obj2.Hook(PVOID(0x004960f0), Naked1_ChangeMsg, 6);
// 		}




// 	PVOID g_pSend =(PVOID) 0x005ea9a0;
// 
// 	void send_packet(PVOID pBuff, DWORD dwLen)
// 	{
// 		void(*InternalCall)(PVOID, int) = (void(__cdecl *)(PVOID, int))g_pSend;
// 		(*InternalCall)(pBuff, dwLen);
// 	}
// 
// 	void send_packet(WORD wCmd, PVOID pBuff, DWORD dwLen)
// 	{
// 		PBYTE pNewPacket = new byte[dwLen + 2];
// 		memcpy(pNewPacket, &wCmd, 2);           
// 		memcpy(&pNewPacket[2], pBuff, dwLen);  
// 		send_packet((PVOID)pNewPacket, dwLen + 2);
// 		delete[] pNewPacket;
// 	}