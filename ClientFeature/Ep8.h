#include <MyClass.h>
#include <windows.h>
#include <MyInlineHook.h>

#include <time.h>
#include <define.h>
#include <float.h>
#include <MyClass.h>
#include "odib.h"
#include <common.h>
#include "resource.h"
#include <process.h>
#include "PictureWidthHeight.h"

extern HMODULE g_hModule;

//#define _SHAIYA_CHN


namespace Ep8
{
	namespace common {
		void Hook(HOOKADDR* stcHook);
	};

	bool g_IsShowPet = true;
	bool g_IsShowWing = true;
	bool g_IsShowEffect = true;
	PBYTE g_pMap = PBYTE(0x0086b008);

	char g_sahPath[MAX_PATH] = { 0 };
	char g_safPath[MAX_PATH] = { 0 };


	namespace sahReader {
		void Init();
		PFile getShiyaFile(const char* filepath);
	};

	//从服务端获取的 hook addr

	HOOKADDR g_hookAddr;


	void drawImage(void* pimage, DWORD x, DWORD y) {
		PVOID pTemp = (PVOID)0x0059D780;  
		_asm
		{
			push y
			push x
			mov ecx, pimage
			call pTemp
		}
	}

	void enumSub(DWORD Eax, vector<DWORD> vecAddr, vector<DWORD>& vecPlayers) {
		auto iter = find(vecAddr.begin(), vecAddr.end(), Eax);
		if (iter != vecAddr.end()) {
			return;
		}
		vecAddr.push_back(Eax);


		BYTE sinature = *PBYTE(Eax + 21);
		if (!sinature) {
			DWORD player = *PDWORD(Eax + 16);
			if (player) {
				auto iter = find(vecPlayers.begin(), vecPlayers.end(), player);
				if (iter == vecPlayers.end()) {
					vecPlayers.push_back(player);
				}
			}
		}

		DWORD leftNode = *PDWORD(Eax);
		DWORD rightNode = *PDWORD(Eax + 8);

		{
			enumSub(leftNode, vecAddr, vecPlayers);
		}

		{
			enumSub(rightNode, vecAddr, vecPlayers);
		}
	}

	void enumPlayers(std::vector<DWORD>& vecPlayers) {
		DWORD base = 0x0221D000;
		base += 0x13DA10;
		base = *PDWORD(base + 4);
		base = *PDWORD(base + 4);
		std::vector<DWORD> vecAddr;
		enumSub(base, vecAddr, vecPlayers);
	}

	void sendPacket(PVOID packet, DWORD dwSize)
	{

		void(*InternalCall)(PVOID, DWORD) = (void(__cdecl *)(PVOID, DWORD))0x00661020;
		(*InternalCall)(packet, dwSize);

	}

	bool InitImageEx(const char* pstrPath, const char* pstrFileName, void* point, int len = 0x20, int height = 0x20)
	{
		bool bRet = false;
		DWORD Path = (DWORD)pstrPath;
		DWORD FileName = (DWORD)pstrFileName;

		DWORD dwCall = 0x0059D480;
		_asm {
			push height
			push len
			push FileName
			push Path
			mov ecx, point
			call dwCall
			mov bRet, al
		}
		return bRet;
	}


	struct RecvPacketStruct
	{
		char Data[0x2000];
		DWORD len;
		RecvPacketStruct *ptr;
	};
	void recivedPacket(PVOID buf, DWORD len)
	{
		RecvPacketStruct *Packet = new RecvPacketStruct;
		memcpy(Packet->Data, buf, len);
		Packet->len = len;
		Packet->ptr = (RecvPacketStruct *)((DWORD)Packet + 2);
		WORD Opcode;
		memcpy(&Opcode, Packet, sizeof(WORD));
		void(*ReceivePacketPtr)(WORD Opcode, RecvPacketStruct *Packet) = (void(__cdecl *)(WORD, RecvPacketStruct *))0x006687B0;
		(*ReceivePacketPtr)(Opcode, Packet);

		delete Packet;
	}


	_inline bool  IsChatModeOn()
	{
		return  *PBOOL(0x0236680c);
	}

	namespace customclock
	{


		void fun_clock(PVOID p)
		{

			Pcmd_clock pClock = Pcmd_clock(p);
			FILETIME svft = { 0 };
			SystemTimeToFileTime(&pClock->stcTime, &svft);
			FILETIME cft = { 0 };
			SYSTEMTIME lt = { 0 };
			GetLocalTime(&lt);
			SystemTimeToFileTime(&lt, &cft);

			LARGE_INTEGER uiSv, uiCl;
			uiSv.LowPart = svft.dwLowDateTime;
			uiSv.HighPart = svft.dwHighDateTime;


			uiCl.LowPart = cft.dwLowDateTime;
			uiCl.HighPart = cft.dwHighDateTime;

			*PINT64(0x221c770) = (uiSv.QuadPart-uiCl.QuadPart)/ 10000000;
		}

	}

	namespace customMessage {


		void sendMessage(char* strMessage, DWORD MessageType = 16) {
			DWORD dwCall = 0x004329c0;

			//如果要在上面显示就是第一个0x21 第二个0x5

			_asm {
				push 0x2e     
				push strMessage
				push 0x36
				mov ecx, 0x8644f0
				mov ecx, dword ptr[ecx]
				call dwCall
			}
		}

		void Process(PVOID packet) {
			PPLAYERMESSAGE p = (PPLAYERMESSAGE)packet;
			sendMessage(p->strMessage, p->MessageType);
		}
	}


	namespace namecolor
	{
		CMyInlineHook  objNormal, ObjParty;

		DWORD g_colors[20000];


		DWORD g_dwRandomSign = 0xffff7fff;  //如果服务器传来的是这个颜色值,则说明是随机颜色
		DWORD g_dwRandCoor = 0xffffffff;

		DWORD  __stdcall ChangeColor(DWORD dwPlayer, DWORD oldColor)
		{
			DWORD dwCharid = *PDWORD(dwPlayer + 0x74);
			return g_colors[dwCharid]?g_colors[dwCharid]:oldColor;
		}

		void fun_getvipcolor(PVOID packet)
		{
			Pcmd_color pColor = ((PPLAYERCOLOR)packet)->color;

			ZeroMemory(g_colors, sizeof(g_colors));
			for (DWORD i = 0; i < 200; i++)
			{
				DWORD charid = pColor[i].dwCharid;
				if (!charid)
					break;

				g_colors[charid]= pColor[i].dwRGB;
			}

		}


		__declspec(naked) void Naked_Normal()
		{
			_asm
			{
				or ebp,0xffffffff
				push eax
				push ebx
				push ecx
				push edx
				push esi
				push edi
				push esp
				MYASMCALL_2(ChangeColor,esi,ebp)
				mov ebp,eax

				pop esp
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx
				pop eax
					sub eax, 0x4
					jmp objNormal.m_pRet
			}
		}
		__declspec(naked) void Naked_Party()
		{

			_asm
			{
				mov ebp, 0xFF00FF00
				pushad
				MYASMCALL_2(ChangeColor, esi, ebp);
				popad
					jmp ObjParty.m_pRet
			}
		}


		void RandomColorChange(LPVOID p)
		{
			DWORD dwTemp = 0;
			while (true)
			{
				Sleep(400);
				dwTemp += 100;
				if (dwTemp > 0xffffff)
				{
					dwTemp = 0;
				}
				g_dwRandCoor = 0xff000000 + dwTemp;
			}
		}

		void Start()
		{
			//正常角色名字颜色地方
			objNormal.Hook((PVOID)g_hookAddr.NormalColorHook, Naked_Normal, 6);
			//组队
			ObjParty.Hook((PVOID)g_hookAddr.PartyColorHook, Naked_Party, 5);

			_beginthread(RandomColorChange, 0, 0);
		}
	}

	namespace processShopView {

		PVOID packet = NULL;
		DWORD len = 20 * sizeof(SHOPITEM) + sizeof(COMMONHEAD);
		PCOMMONHEAD packetHead = NULL;
		PSHOPITEM   packetBody = NULL;


		void Init() {
			if (!packet) {
				packet = new BYTE[len];
				packetHead = (PCOMMONHEAD)packet;
			}
			ZeroMemory(packet, len);
			PCOMMONHEAD(packet)->cmd = official_packet_const::Recived_shopList;
			packetBody = (PSHOPITEM)getBodyOfcommonHeader(packet);
		}



		void process(PVOID p) {
			SPLITSHOP* pSplit = (SPLITSHOP*)p;
			DWORD newBodylen = pSplit->head.byCount * sizeof(SHOPITEM);//计算body大小
			packetBody += packetHead->byCount;           //body 偏移往后挪动
			packetHead->byCount += pSplit->head.byCount; //加上新的数量
			memcpy(packetBody, pSplit->body, newBodylen);//复制body

			if (pSplit->part == 2) {
				recivedPacket(packet, len);
				Init();
			}
		}

	}

	namespace additional_image {

		CMyInlineHook g_objDrawAboveName, g_objDrawAboveGuild;

		typedef struct {
			int un1;
			int un2;
			float width;
			float height;
		}ShaiyaImgObj, *PShaiyaImgObj;

		ShaiyaImgObj g_pImages[100] = { 0 };
		PShaiyaImgObj g_Playerimages[2000] = { 0 };
		CRITICAL_SECTION g_cs;
		std::map<DWORD, DWORD> g_mapPlayerImages;

		int g_x_plus = 0;   //
		int g_y_plus = 0;   //这些可以作为图片的长宽来作调整
		DWORD g_imageCount = 0;
		bool g_isEnable = false;

		void resetImageObj(DWORD charid, DWORD ImageIndex) {
			g_Playerimages[charid] = &(g_pImages[ImageIndex]);
		}

		PShaiyaImgObj getImageObj(DWORD charid) {
			return g_Playerimages[charid];
		}



		void getpos(DWORD player, float x, DWORD y) {
			*PDWORD(player + 0x7e0) = x;
			*PDWORD(player + 0x7e4) = y;
		}

		void __stdcall fun_ondropGuild(DWORD player, float x, DWORD y) {
			getpos(player, x, y);
		}

		void __stdcall fun_ondropName(DWORD player, float x, DWORD y) {

			//如果没有行会则取
			if (!*PDWORD(player + 0x3b8)) {
				getpos(player, x, y);
			}
		}


		DWORD dwOrgCall_1 = 0x0059F9C0;
		__declspec(naked) void Naked_ondrawGuild() {
			_asm {
				pushad
				mov eax, [esp + 0x28]  //y
				mov ebx, [esp + 0x48]  //x
				MYASMCALL_3(fun_ondropGuild, esi, ebx, eax)
				popad

				call dwOrgCall_1
				jmp g_objDrawAboveGuild.m_pRet
			}
		}

		__declspec(naked) void Naked_onDrawName() {
			_asm {
				pushad
				mov eax, [esp + 0x28]  //y
				mov ebx, [esp + 0x48]  //x
				MYASMCALL_3(fun_ondropName, esi, ebx, eax)
				popad

				call dwOrgCall_1
				jmp g_objDrawAboveName.m_pRet
			}
		}

		void __stdcall fun_draw(std::vector<DWORD>& vecPlayers) {

			if (g_isEnable) {
				for (auto iter = vecPlayers.begin(); iter != vecPlayers.end(); iter++) {
					DWORD player = *iter;
					if (!player) continue;
					DWORD charid = *PDWORD(*iter + 0x74);
					//获取图像
					PShaiyaImgObj pImage = getImageObj(charid);
					if (pImage) {
						//要调整的坐标位,从服务端获取的
						DWORD x = *PDWORD(*iter + 0x7e0) + g_x_plus - pImage->width / 2;
						DWORD y = *PDWORD(*iter + 0x7e4) + g_y_plus - pImage->height - 15;
						drawImage(pImage, x, y);
					}
				}
			}
		}



		void InitImage()
		{

			FILE *pfRead;
			if (fopen_s(&pfRead, g_safPath, "rb") != 0) {
				return;
			}

			for (DWORD i = 0; i < g_imageCount; i++) {

				char filename[50] = { 0 };
				char fullName[MAX_PATH] = { 0 };

				sprintf(filename, "title_%02d.png", i);
				sprintf(fullName, "interface\\%s", filename);


				//获取sah里的file属性拿offset
				PFile stFile = sahReader::getShiyaFile(fullName);
				if (!stFile) {
					continue;
				}
				
				unsigned int width, height;
				GetPNGWidthHeight(pfRead, &width, &height, stFile->offset);


				if (!InitImageEx("data\\interface", filename, &(g_pImages[i]), width, height)) {
					break;
				}
			}
		}

		void Init() {
			if (g_isEnable) {
				return;
			}

			g_isEnable = true;
			InitImage();

			//这2个hook是获取坐标的
			g_objDrawAboveName.Hook(PVOID(0x00480d25), Naked_onDrawName, 5);
			g_objDrawAboveGuild.Hook(PVOID(0x00480dea), Naked_ondrawGuild, 5);

		}
		void ProcessPacket(void* p) {

			PPLAYERTITLES packet = (PPLAYERTITLES)p;
			g_x_plus = packet->x;
			g_y_plus = packet->y;
			g_imageCount = packet->imageCount;

			Init();

			ZeroMemory(g_Playerimages, sizeof(g_Playerimages));
			for (DWORD i = 0; i < _countof(packet->playerImages); i++) {
				DWORD charid = packet->playerImages[i].charid;
				if (!charid) {
					break;
				}
				resetImageObj(charid, packet->playerImages[i].Inedx);
			}
		}
	}

	namespace EffectDrawing {

		CMyInlineHook g_obj;

		void __stdcall fun_onThreadLoop() {
			if (additional_image::g_isEnable) {
				vector<DWORD> vecPlayers;
				enumPlayers(vecPlayers);
				additional_image::fun_draw(vecPlayers);
			}

			//	PlayerEffect::draw(vecPlayers);
		}

		__declspec(naked) void Naked_onThreadLoop()
		{
			_asm
			{
				mov edi, dword ptr ds : [esi + 0x1D0]
				pushad
				MYASMCALL(fun_onThreadLoop)
				popad
				jmp g_obj.m_pRet

			}
		}



		void Init() {
			static bool isInit = false;
			if (isInit)
				return;
			isInit = true;
			g_obj.Hook(PVOID(0x0048fde7), Naked_onThreadLoop, 6);
		}
	}

	namespace custompacket
	{
		CMyInlineHook objClock;

		void __stdcall CustomPacket(PWORD pCmd)
		{
			switch (*pCmd)
			{
			case PacketOrder::VipColor:namecolor::fun_getvipcolor(pCmd);break;
			case PacketOrder::HookAddr:common::Hook((HOOKADDR*)pCmd);break;
			case PacketOrder::SplitShop:processShopView::process(pCmd);break;
			case PacketOrder::Clock:customclock::fun_clock(pCmd);break;
			case PacketOrder::PlayerMessage:customMessage::Process(pCmd);break;
			case PacketOrder::additional_title:
				additional_image::ProcessPacket(pCmd);
				EffectDrawing::Init();
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
				MYASMCALL_1(CustomPacket, eax);
				popad
					_orginal :
				sub eax, 0x0000FA08
					jmp objClock.m_pRet

			}
		}

		void Start()
		{
			//特证码:0066A626      2D 08FA0000            sub eax,0xFA08
		   //	0066A62B | .  83F8 0A                cmp eax, 0xA
			objClock.Hook((PVOID)0x0066A626, Naked1, 5);
		}
	}

	namespace Enchant {
		CMyInlineHook g_obj;

		__declspec(naked) void Naked_enchantAddValue()
		{
			_asm
			{
				movzx eax, word ptr ds : [ecx + eax * 2 + 0x75C8]  //ecx 是基地址 eax为阶段 75c8是强化数值的偏移
				retn 0xc
			}
		}


		void Start()
		{
			g_obj.Hook(PVOID(g_hookAddr.enchantHook), Naked_enchantAddValue, 5,NULL,1);
		}

	}

	namespace Ep5SkilFix
	{
		CMyInlineHook g_objWindSpinSkill;

		DWORD addr = 0x005de9c8;
		__declspec(naked) void Naked_WindSpin()
		{
			_asm {
				
				cmp bp,0x9c
				jne _org
				jmp addr

				_org:
				mov edx, 0x26
				jmp g_objWindSpinSkill.m_pRet
			}
		}

		void Start()
		{
			bool IsInit = false;
			if (IsInit)
				return;
			IsInit = true;
			DWORD Skillid = 0x26;
			MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x005E12E1 + 1), &Skillid, sizeof(Skillid));
//			MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x005DE253 + 1), &Skillid, sizeof(Skillid));

			Skillid = 0x38;
			MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x005E13A2 + 1), &Skillid, sizeof(Skillid));
			MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x005DE2E7 + 1), &Skillid, sizeof(Skillid));

			Skillid = 0x155;
			MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x005DE261 + 1), &Skillid, sizeof(Skillid));

			Skillid = 0x161;
			MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x005E140D + 1), &Skillid, sizeof(Skillid));

			Skillid = 0xa3;
			MyWriteProcessMemory(GetCurrentProcess(), PVOID(0x005E1417 + 1), &Skillid, sizeof(Skillid));



			g_objWindSpinSkill.Hook((PVOID)0x005de253, Naked_WindSpin, 5);
		}
	}

	namespace TownMoveRune {
		PVOID g_packet = NULL;
		DWORD g_packetSize = 0;
		bool g_enableMoveTown = false;
		CRITICAL_SECTION g_cs;
		CMyInlineHook g_objMoveTown, g_objMoveTownProcessBar;

		DWORD WINAPI Thread_MoveTown(_In_ LPVOID lpParameter)
		{
			while (1)
			{
				EnterCriticalSection(&g_cs);
				if (g_packet)
				{
					Sleep(5000);   //那个进度条默认都是5秒的
					if (g_enableMoveTown)
					{
						sendPacket(g_packet, g_packetSize);
					}
					g_enableMoveTown = false;
					delete[] g_packet;
					g_packet = NULL;
					g_packetSize = 0;
				}
				LeaveCriticalSection(&g_cs);
				Sleep(200);
			}
			return 0;
		}

		
		void __stdcall fun_MoveTown(PVOID packet,DWORD packetSize)
		{
			EnterCriticalSection(&g_cs);
			if(g_packetSize){
				delete[] g_packet;
				g_packet = NULL;
				g_packetSize = 0;
			}


			//开始画进度条
			g_enableMoveTown = false;
			{
				DWORD object = *PDWORD(0x235dda4);
				BYTE temp[] = { 0x21 ,0x02 ,0x1c ,0x00 ,0x00 ,0x00 };
				*PDWORD(&temp[2]) = *PDWORD(object + 0x74);;  //charid
				recivedPacket(temp, sizeof(temp));
			}

			//把buff复制，交给另一条线程处理
			g_packet = new BYTE[packetSize];
			memcpy(g_packet, packet, packetSize);
			g_packetSize = packetSize;


			LeaveCriticalSection(&g_cs);
		}

		

		__declspec(naked) void Naked_ProcessResult()
		{
			_asm {
				mov dword ptr ds : [0x2211524], edi
				mov g_enableMoveTown,0x1
				jmp g_objMoveTownProcessBar.m_pRet
			}

		}
		__declspec(naked) void Naked_Use()
		{
			_asm {
				pushad
				mov eax, [esp + 0x24] //packet
				mov ebx, [esp + 0x28] //size
				MYASMCALL_2(fun_MoveTown, eax, ebx)
				popad
				retn
			}

		}



		void Start() 
		{
			InitializeCriticalSection(&g_cs);
			g_objMoveTown.Hook(PVOID(g_hookAddr.MoveTownHook), Naked_Use, 5, fun_MoveTown, 1);
			g_objMoveTownProcessBar.Hook(PVOID(g_hookAddr.MoveTownProcessBarHook), Naked_ProcessResult, 7);
			::CreateThread(0, 0, Thread_MoveTown, 0, 0, 0);
		}
	}

	namespace mobColor {

		CMyInlineHook g_objMobColor;

		DWORD __stdcall getMobColor(int level) {
			if (level >= 10) {
				return 0xFF808080;
			}
			if (level >= 8) {
				return 0xFFFF00FF;
			}
			if (level >= 6) {
				return 0xFFFF0000;
			}
			if (level >= 4) {
				return 0xFFFF8000;
			}
			if (level >= 2) {
				return 0xFFFFFF00;
			}
			if (level >= -1) {
				return 0xFF00FF00;
			}
			if (level >= -3) {
				return 0xFF0000FF;
			}
			if (level >= -5) {
				return 0xFF80FFFF;
			}
			return 0xffffffff;

		}

		__declspec(naked) void Naked_mobColor()
		{
			_asm
			{
				push ebx
				push ecx
				push edx
				push esi
				push edi
				push ebp
				push esp

				push eax
				call g_objMobColor.m_pfun_call

				pop esp
				pop ebp
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx

				pop esi
				retn 0x4

			}
		}


		void Start() {
			g_objMobColor.Hook(PVOID(0x0044f0ac), Naked_mobColor, 6, getMobColor);  //只需要给这个地址就行了,其它不用变
		}

	}

	namespace Hide_pet
	{
		CMyInlineHook objPet, objWing, objEffect;

		DWORD ignorePet = 0x00603E6B;
		DWORD ignoreWing = 0x0060B76D;
		DWORD ignoreEffect = 0x0060A101;

		__declspec(naked) void Naked_HidePet()
		{
			_asm
			{

				lea eax, g_IsShowPet
				cmp byte ptr[eax], 0x0
				jne __orginal

				jmp ignorePet
				__orginal :
				cmp dword ptr ss : [esp + 0x30], 0x0
					jmp objPet.m_pRet
			}
		}

		__declspec(naked) void Naked_HideWing()
		{
			_asm
			{

				lea eax, g_IsShowWing
				cmp byte ptr[eax], 0x0
				jne __orginal
				jmp ignoreWing
				__orginal:
				fld dword ptr ds : [esi + 0x3C4]
					jmp objWing.m_pRet
			}
		}



		DWORD effectCall = 0x005FCAB0;
		__declspec(naked) void Naked_effect()
		{
			_asm
			{

				lea eax, g_IsShowEffect
				cmp byte ptr[eax], 0x0
				jne __orginal
				jmp ignoreEffect
				__orginal :
				call effectCall
					jmp objEffect.m_pRet
			}
		}




		void Start()
		{
			objPet.Hook((PVOID)0x00603ca7, Naked_HidePet, 5);
			objWing.Hook(PVOID(0x0060b5a5), Naked_HideWing, 6);
			objEffect.Hook(PVOID(0x00609ff5), Naked_effect, 5);
		}
	}

	namespace chat_command
	{
		CMyInlineHook g_obj1;
		void  __stdcall fun_chat_command(char* strChat)
		{
			char* strInput = new char[MAX_PATH];
			char* strCommand = new char[MAX_PATH];
			char* strOption = new char[MAX_PATH];
			do
			{
				strcpy_s(strInput, MAX_PATH, strChat);
				sscanf_s(strInput, "%s %s", strCommand, MAX_PATH, strOption, MAX_PATH);
				if (strCommand[0] == '\0' || strOption[0] == '\0')
					break;

				if (strcmp("/pet", strCommand) == 0 && 
					strcmp("on", strOption) == 0){
					g_IsShowPet = true;
					break;
				}

				if (strcmp("/pet", strCommand) == 0 &&
					strcmp("off", strOption) == 0) {
					g_IsShowPet = false;
					break;
				}

				if (strcmp("/wing", strCommand) == 0 &&
					strcmp("on", strOption) == 0) {
					g_IsShowWing = true;
					break;
				}

				if (strcmp("/wing", strCommand) == 0 &&
					strcmp("off", strOption) == 0) {
					g_IsShowWing = false;
					break;
				}

				if (strcmp("/effect", strCommand) == 0 &&
					strcmp("on", strOption) == 0) {
					g_IsShowEffect = true;
					break;
				}

				if (strcmp("/effect", strCommand) == 0 &&
					strcmp("off", strOption) == 0) {
					g_IsShowEffect = false;
					break;
				}


			} while (0);
			delete[] strInput;
			delete[] strCommand;
			delete[] strOption;

		}

		__declspec(naked) void Naked_1()
		{
			_asm
			{
				mov eax, edi
				lea edx, dword ptr[eax + 1]  //orginal
				pushad
				pushfd
				push eax
				call g_obj1.m_pfun_call
				popfd
				popad
				jmp g_obj1.m_pRet

			}
		}
		void Start()
		{
			g_obj1.Hook((PVOID)0x004e07c2, Naked_1, 5, fun_chat_command);
		}
	}

	namespace Emoji
	{
		int g_Emojiwidth = 32;
		CMyInlineHook g_objPos1, g_objPos2, g_objDrawWord1, g_objDrawWord2;
		CMyInlineHook g_objInitWord;
		HWND g_hWnd = NULL;
		typedef struct 
		{
			char szNickName[100];
			char szFileName[100];
			DWORD resId; 
			BYTE pBuff[100];
		}MYEMOJI,*PMYEMOJI;



#ifdef _SHAIYA_CHN
		MYEMOJI g_Emoji[] = {
			{ "emj01","1.png",IDI_ICON1,{ 0 } },
			{ "emj02","2.png",IDI_ICON2,{ 0 } },
			{ "emj03","3.png",IDI_ICON3,{ 0 } },
			{ "emj04","4.png",IDI_ICON4,{ 0 } },
			{ "emj05","5.png",IDI_ICON5,{ 0 } },
			{ "emj06","6.png",IDI_ICON6,{ 0 } },
			{ "emj07","7.png",IDI_ICON7,{ 0 } },
			{ "emj08","8.png",IDI_ICON8,{ 0 } },
			{ "emj09","9.png",IDI_ICON9,{ 0 } },
			{ "emj10","10.png",IDI_ICON10,{ 0 } },
			{ "emj11","11.png",IDI_ICON11,{ 0 } },
			{ "emj12","12.png",IDI_ICON12,{ 0 } },
			{ "emj13","13.png",IDI_ICON13,{ 0 } },
			{ "emj14","14.png",IDI_ICON14,{ 0 } },
			{ "emj15","15.png",IDI_ICON15,{ 0 } },
			{ "emj16","16.png",IDI_ICON16,{ 0 } },
			{ "emj17","17.png",IDI_ICON17,{ 0 } },
			{ "emj18","18.png",IDI_ICON18,{ 0 } },
			{ "emj19","19.png",IDI_ICON19,{ 0 } },
			{ "emj20","20.png",IDI_ICON20,{ 0 } },
			{ "emj21","21.png",IDI_ICON21,{ 0 } },
			{ "emj22","22.png",IDI_ICON22,{ 0 } },
			{ "emj23","23.png",IDI_ICON23,{ 0 } },
			{ "emj24","24.png",IDI_ICON24,{ 0 } },
			{ "emj25","25.png",IDI_ICON25,{ 0 } },
			{ "emj26","26.png",IDI_ICON26,{ 0 } },
			{ "emj27","27.png",IDI_ICON27,{ 0 } },
			{ "emj28","28.png",IDI_ICON28,{ 0 } },
			{ "emj29","29.png",IDI_ICON29,{ 0 } },
			{ "emj30","30.png",IDI_ICON30,{ 0 } },
			{ "emj31","31.png",IDI_ICON31,{ 0 } },
			{ "emj32","32.png",IDI_ICON32,{ 0 } },
			{ "emj33","33.png",IDI_ICON33,{ 0 } },
			{ "emj34","34.png",IDI_ICON34,{ 0 } },
			{ "emj35","35.png",IDI_ICON35,{ 0 } },
			{ "emj36","36.png",IDI_ICON36,{ 0 } },
			{ "emj37","37.png",IDI_ICON37,{ 0 } },
			{ "emj38","38.png",IDI_ICON38,{ 0 } },
			{ "emj39","39.png",IDI_ICON39,{ 0 } },
			{ "emj40","40.png",IDI_ICON40,{ 0 } },
			{ "emj41","41.png",IDI_ICON41,{ 0 } },
			{ "emj42","42.png",IDI_ICON42,{ 0 } },
			{ "emj43","43.png",IDI_ICON43,{ 0 } },
			{ "emj44","44.png",IDI_ICON44,{ 0 } },
			{ "emj45","45.png",IDI_ICON45,{ 0 } },
			{ "emj46","46.png",IDI_ICON46,{ 0 } },
			{ "emj47","47.png",IDI_ICON47,{ 0 } },
			{ "emj48","48.png",IDI_ICON48,{ 0 } },
			{ "emj49","49.png",IDI_ICON49,{ 0 } },
			{ "emj50","50.png",IDI_ICON50,{ 0 } },
			{ "emj51","51.png",IDI_ICON51,{ 0 } },
			{ "emj52","52.png",IDI_ICON52,{ 0 } },
			{ "emj53","53.png",IDI_ICON53,{ 0 } },
			{ "emj54","54.png",IDI_ICON54,{ 0 } }
		};
#else
		MYEMOJI g_Emoji[] = {
			{ "emoji--01","1.png",IDI_ICON1,{ 0 } },
			{ "emoji--02","2.png",IDI_ICON2,{ 0 } },
			{ "emoji--03","3.png",IDI_ICON3,{ 0 } },
			{ "emoji--04","4.png",IDI_ICON4,{ 0 } },
			{ "emoji--05","5.png",IDI_ICON5,{ 0 } },
			{ "emoji--06","6.png",IDI_ICON6,{ 0 } },
			{ "emoji--07","7.png",IDI_ICON7,{ 0 } },
			{ "emoji--08","8.png",IDI_ICON8,{ 0 } },
			{ "emoji--09","9.png",IDI_ICON9,{ 0 } },
			{ "emoji--10","10.png",IDI_ICON10,{ 0 } },
			{ "emoji--11","11.png",IDI_ICON11,{ 0 } },
			{ "emoji--12","12.png",IDI_ICON12,{ 0 } },
			{ "emoji--13","13.png",IDI_ICON13,{ 0 } },
			{ "emoji--14","14.png",IDI_ICON14,{ 0 } },
			{ "emoji--15","15.png",IDI_ICON15,{ 0 } },
			{ "emoji--16","16.png",IDI_ICON16,{ 0 } },
			{ "emoji--17","17.png",IDI_ICON17,{ 0 } },
			{ "emoji--18","18.png",IDI_ICON18,{ 0 } },
			{ "emoji--19","19.png",IDI_ICON19,{ 0 } },
			{ "emoji--20","20.png",IDI_ICON20,{ 0 } },
			{ "emoji--21","21.png",IDI_ICON21,{ 0 } },
			{ "emoji--22","22.png",IDI_ICON22,{ 0 } },
			{ "emoji--23","23.png",IDI_ICON23,{ 0 } },
			{ "emoji--24","24.png",IDI_ICON24,{ 0 } },
			{ "emoji--25","25.png",IDI_ICON25,{ 0 } },
			{ "emoji--26","26.png",IDI_ICON26,{ 0 } },
			{ "emoji--27","27.png",IDI_ICON27,{ 0 } },
			{ "emoji--28","28.png",IDI_ICON28,{ 0 } },
			{ "emoji--29","29.png",IDI_ICON29,{ 0 } },
			{ "emoji--30","30.png",IDI_ICON30,{ 0 } },
			{ "emoji--31","31.png",IDI_ICON31,{ 0 } },
			{ "emoji--32","32.png",IDI_ICON32,{ 0 } },
			{ "emoji--33","33.png",IDI_ICON33,{ 0 } },
			{ "emoji--34","34.png",IDI_ICON34,{ 0 } },
			{ "emoji--35","35.png",IDI_ICON35,{ 0 } },
			{ "emoji--36","36.png",IDI_ICON36,{ 0 } },
			{ "emoji--37","37.png",IDI_ICON37,{ 0 } },
			{ "emoji--38","38.png",IDI_ICON38,{ 0 } },
			{ "emoji--39","39.png",IDI_ICON39,{ 0 } },
			{ "emoji--40","40.png",IDI_ICON40,{ 0 } },
			{ "emoji--41","41.png",IDI_ICON41,{ 0 } },
			{ "emoji--42","42.png",IDI_ICON42,{ 0 } },
			{ "emoji--43","43.png",IDI_ICON43,{ 0 } },
			{ "emoji--44","44.png",IDI_ICON44,{ 0 } },
			{ "emoji--45","45.png",IDI_ICON45,{ 0 } },
			{ "emoji--46","46.png",IDI_ICON46,{ 0 } },
			{ "emoji--47","47.png",IDI_ICON47,{ 0 } },
			{ "emoji--48","48.png",IDI_ICON48,{ 0 } },
			{ "emoji--49","49.png",IDI_ICON49,{ 0 } },
			{ "emoji--50","50.png",IDI_ICON50,{ 0 } },
			{ "emoji--51","51.png",IDI_ICON51,{ 0 } },
			{ "emoji--52","52.png",IDI_ICON52,{ 0 } },
			{ "emoji--53","53.png",IDI_ICON53,{ 0 } },
			{ "emoji--54","54.png",IDI_ICON54,{ 0 } }
		};
#endif






		bool InitImage()
		{
			for (DWORD i = 0; i < _countof(g_Emoji); i++)
			{
				MYEMOJI& stc = g_Emoji[i];
				if (!InitImageEx("data\\interface\\emoji", stc.szFileName, stc.pBuff))
					return false;
			}
			return true;
		}

		void DrawImage(MYEMOJI& emoji,DWORD x,DWORD y)
		{
			DWORD point = DWORD(emoji.pBuff);
			DWORD dwCall = 0x0059D780;
			_asm
			{
				push y
				push x
				mov ecx, point
				call dwCall
			}
		}

		_inline void  DrawWords(DWORD x, DWORD y, DWORD r, DWORD g, DWORD b, char* strWords, DWORD unkonwn, DWORD dwEcx)
		{
			DWORD dwCall = 0x00594ba0;
			_asm
			{
				mov ecx,dwEcx
				push unkonwn
				push strWords
				push b
				push g
				push r
				push y
				push x
				call dwCall
			}
		}

		
		void __stdcall fun_drawWord(DWORD dwEcx,DWORD x,DWORD y,DWORD r,DWORD g,DWORD b, char* strWords,DWORD unkonwn)
		{
			char pstrWords[100] = { 0 };
			strcpy(pstrWords, strWords);

			for (DWORD i = 0; i < _countof(g_Emoji); i++)
			{
				char* pstrNickName = strstr(pstrWords, g_Emoji[i].szNickName);
				if(!pstrNickName)
					continue;

				//图像要画的位置
#ifdef _SHAIYA_CHN
				DWORD ImageX = x + (pstrNickName - pstrWords)*6;
#else
				DWORD ImageX = x + (pstrNickName - pstrWords) *6;
#endif
				//把符号代号变成空格
				for (DWORD j = 0; j < strlen(g_Emoji[i].szNickName); j++)
				{
					pstrNickName[j] = ' ';
				}

				DrawImage(g_Emoji[i], ImageX, y);
			}
			DrawWords(x, y, r, g, b, pstrWords, unkonwn, dwEcx);
		}


		_inline DWORD InitWord(DWORD dwEcx,char* strWords)
		{
			DWORD dwRet = 0;
			DWORD dwCall = 0x0059F220;
			_asm
			{
				mov ecx, dwEcx
				push strWords
				call dwCall
				mov dwRet, eax
			}
			return dwRet;
		}


		DWORD __stdcall fun_InitWord(DWORD dwEcx, const char* pstrWords)
		{

			char strWords[200] = { 0 };
			strcpy(strWords, pstrWords);

			for (DWORD i = 0; i < _countof(g_Emoji); i++)
			{
				char* pstrNickName = strstr(strWords, g_Emoji[i].szNickName);
				if (!pstrNickName)
					continue;

				//把符号代号变成空格
				for (DWORD j = 0; j < strlen(g_Emoji[i].szNickName); j++)
				{
					pstrNickName[j] = ' ';
				}
			}
			return InitWord(dwEcx, strWords);
		}


		__declspec(naked) void Naked_drawWord2()
		{
			_asm {
				MYASMCALL_1(fun_drawWord, ecx)
				jmp g_objDrawWord2.m_pRet
			}
		}

		__declspec(naked) void Naked_drawWord1()
		{
			_asm {
				MYASMCALL_1(fun_drawWord,ecx)
				jmp g_objDrawWord1.m_pRet
			}
		}


		__declspec(naked) void Naked_pos1()
		{
			_asm {
				add eax, 04

				push edx
				push ecx
				cdq
				mov ecx, 0x2
				xor edx, edx
				idiv ecx
				pop ecx
				pop edx

				cmp dword ptr [esp + 0x34], eax

				jmp g_objPos1.m_pRet
			}
		}
	
		__declspec(naked) void Naked_InitWord()
		{
			_asm
			{
				push ebx
				push ecx
				push edx
				push esi
				push edi
				push ebp
				push esp

				MYASMCALL_2(fun_InitWord,ecx,eax)

				pop esp
				pop ebp
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx

				add esp,0x4

				jmp g_objInitWord.m_pRet
			}
		}
		__declspec(naked) void Naked_pos2()
		{
			_asm
			{
				push eax
				movzx eax, byte ptr[ebx + 0xc18]
				shl eax, 0x4
				sub edx, eax
				pop eax

				add edx, [ebx + 0x24]
				lea ecx, [edx + eax + 0x5E]
				jmp g_objPos2.m_pRet
			}
		}

		void InsertChat(char* szNickName)
		{
			for (DWORD i = 0; i < strlen(szNickName); i++) {
				SendMessage(g_hWnd, WM_KEYDOWN, szNickName[i], 0);
				SendMessage(g_hWnd, WM_CHAR, szNickName[i], 0);
				SendMessage(g_hWnd, WM_KEYUP, szNickName[i], 0);
			}
		}


		void EmojiClick(char* strFileName)
		{
			if (!IsChatModeOn())
				return;

			
			if (*PCHAR(0x02366f24) == '\0') {
				InsertChat(" ");
			}

			for (DWORD i = 0; i < _countof(g_Emoji); i++)
			{
				if (strcmp(g_Emoji[i].szFileName, strFileName) == 0)
				{
					return InsertChat(g_Emoji[i].szNickName);
				}
			}
		}


		void InitEmojiWindow()
		{
			//1.设置图片
			for (DWORD i = 0; i < _countof(g_Emoji); i++)
			{
				SHAIYAOWNERDRAW::AddEmoji(g_Emoji[i].szFileName, g_Emoji[i].resId);
			}

			//2.创建窗口跟按钮
			g_hWnd =FindWindow(L"GAME",L"Shaiya");
			SHAIYAOWNERDRAW::InitWindow(g_hModule, g_hWnd);


			//3.设置点击窗口后的回调
			SHAIYAOWNERDRAW::SetCallBack(EmojiClick);
		}

		void MonitChatModeOn(void* p)
		{
			while (1)
			{
				if (IsChatModeOn()) {
					SHAIYAOWNERDRAW::ShowWindow();
				}
				else {
					SHAIYAOWNERDRAW::HideWindow();
				}
				Sleep(1000);
			}
		}

		void Start()
		{

			while (1)
			{
				if (InitImage())
					break;
				Sleep(1000);
			}

			//2.
			g_objDrawWord1.Hook(PVOID(g_hookAddr.DrawWord1Hook), Naked_drawWord1, 5);  //
			g_objDrawWord2.Hook(PVOID(g_hookAddr.DrawWord2Hook), Naked_drawWord2, 5);  //聊天输入窗
			g_objInitWord.Hook(PVOID(g_hookAddr.InitWordHook), Naked_InitWord, 5);

			//3.
			g_objPos1.Hook(PVOID(g_hookAddr.Pos1Hook), Naked_pos1, 7);
			g_objPos2.Hook(PVOID(g_hookAddr.Pos2Hook), Naked_pos2, 7);
			BYTE temp = 0x5;
			MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004df802, &temp, sizeof(temp));

			//4.
			InitEmojiWindow();

			//5. 用来监视聊天窗口是否打开
			_beginthread(MonitChatModeOn, NULL,NULL);
		}
	}



	namespace sahReader {

		Folder g_root_directory;


		void read_folder(Folder* folder, std::ifstream* input) {



			//读文件数量 再根据数量循环
			int number_files_current_dir;
			input->read((char*)&number_files_current_dir, 4);
			folder->file_count = number_files_current_dir;
			for (int i = 0; i < folder->file_count; i++) {
				File current_file;


				input->read((char*)&current_file.name_length, 4);

				input->read(current_file.file_name, current_file.name_length);
				input->read((char*)&current_file.offset, 8);
				input->read((char*)&current_file.length, 8);

				folder->files.push_back(current_file);
			}



			//读文件夹数量 再根据数量循环
			int sub_folder_count;
			input->read((char*)&sub_folder_count, 4);
			folder->sub_folder_count = sub_folder_count;
			for (int i = 0; i < folder->sub_folder_count; i++) {
				Folder directory;
				directory.parent_folder = folder;
				input->read((char*)&directory.name_length, 4);
				input->read(directory.folder_name, directory.name_length); //只读了文件夹的名
				read_folder(&directory, input);
				folder->folders.push_back(directory);
			}
		}

		bool read_sah(Folder* folder, char* strPath) {
			std::ifstream* input = new std::ifstream(strPath, std::ifstream::in | std::ifstream::binary);
			if (!input->is_open()) {
				std::cout << "Unable to open the file specified!" << std::endl;
				false;
			}

			char header[3];
			input->read((char*)&header, 3);
			input->ignore(4);
			int total_number_files;
			input->read((char*)&total_number_files, 4);
			input->ignore(45);
			strcpy(folder->folder_name, "data");
			sahReader::read_folder(folder, input);
			input->close();
			return true;
		}



		PFile getShiyaFile(const char* filepath) {

			Folder* pMainFolder = &g_root_directory;

			char* pstrFile = new char[strlen(filepath) * 2];
			strcpy(pstrFile, filepath);

			char *delim = "\\";
			char* p = strtok(pstrFile, delim);

			PFolder pFolder = pMainFolder;

			PFile pRet = NULL;

			while (1) {

				char* pNext = strtok(NULL, delim);


				//特殊情况
				if (stricmp(p, "data") == 0) {
					p = pNext;
					continue;
				}

				//找文件夹
				if (pNext) {
					bool isFind = false;
					for (auto iter = pFolder->folders.begin(); iter != pFolder->folders.end(); iter++) {
						if (stricmp(iter->folder_name, p) == 0) {
							isFind = true;
							pFolder = &(*iter);
							break;
						}
					}
					p = pNext;
					if (!isFind) {
						goto __exit;
					}
				}
				//找文件
				else {
					for (auto iter = pFolder->files.begin(); iter != pFolder->files.end(); iter++) {
						if (stricmp(iter->file_name, p) == 0) {
							pRet = &(*iter);
							goto __exit;
						}
					}
					goto __exit;
				}
			}
		__exit:

			delete[] pstrFile;
			return pRet;
		}

		void Init() {

			char strTemp[MAX_PATH] = { 0 };
			GetModuleFileNameA(GetModuleHandle(0), strTemp, MAX_PATH);
			PathRemoveFileSpecA(strTemp);
			sprintf(g_sahPath, "%s\\data.sah", strTemp);
			sprintf(g_safPath, "%s\\data.saf", strTemp);

			sahReader::read_sah(&g_root_directory, g_sahPath);
		}

	}

	namespace common
	{


		CMyInlineHook obj_cnWord;
		__declspec(naked) void Naked_cnWord()
		{
			_asm {
				mov edi, dword ptr ss : [esp + 0x24]
				add esi, eax
				pushad
				mov edx, dword ptr ds : [edi + 0x68]
				mov dword ptr ds : [edi + 0x6C], esi
				mov dword ptr ds : [edi + 0x70], edx
				popad
				jmp obj_cnWord.m_pRet
			}

		}


		void HookThread(LPVOID p)
		{
			Enchant::Start();
			mobColor::Start();
			chat_command::Start();
			Hide_pet::Start();
			TownMoveRune::Start();
	
			Emoji::Start();
			namecolor::Start();
			processShopView::Init();
#ifndef _SHAIYA_CHN
			Ep5SkilFix::Start();

			char strDonate[] = "%s\Internet Explorer\IEXPLORE.EXE http://shaiya-rebuild.com/donate.php";
			strcpy((char*)0x007D64D8, strDonate);
#endif 


		}


		void Hook(HOOKADDR* stcHook)
		{
			bool static IsInit = false;
			if (IsInit) {
				return;
			}
			IsInit = true;

			memcpy(&g_hookAddr, stcHook,sizeof(HOOKADDR));

			::_beginthread(HookThread, 0, 0);
		}



		bool ChangeIp()
		{

			bool bRet = false;
			if (*PDWORD(0x7C95D8) == 0x2E383031)
			{
				//ip 
				char strIp[] = "164.132.203.7";
				strcpy(PCHAR(0x7C95D8), strIp);

				//font
				char strFont[] = "Verdana";
				strcpy(PCHAR(0x7DD2AC), strFont);
				strcpy(PCHAR(0x7DD2A4), strFont);
				strcpy(PCHAR(0x7DD29C), strFont);
				strcpy(PCHAR(0x7DD294), strFont);
				strcpy(PCHAR(0x7DD278), strFont);
				strcpy(PCHAR(0x7DD270), strFont);
				strcpy(PCHAR(0x7EC480), strFont);
				bRet = true;
			}


			return bRet;
		}


		DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter)
		{
			while (1)
			{
#ifdef _SHAIYA_CHN
				if (*PBYTE(0x0083c15c) == 0x6)
				{
					*PBYTE(0x0083c15c) = 0x2;
				}
				if (*PBYTE(0x0083c15c) == 0x2)
					break;
#else
				if (ChangeIp())
				{
					break;
				}
#endif
				Sleep(50);
			}


			//ijl15
			BYTE temp = 0xeb;
			MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004115a8, &temp, 1);

			Sleep(3000);

			//更改玩家结构大小
			DWORD playerobjSize = 0x800;
			DWORD addr[] = { 0x0060AD8E,0x0045E766,0x004B51FD,0x004CF774,0x004D5DF7,0x004F30A7,0x005258F8,0x005C2E14,0x005C317C,0x00608709 };
			for (DWORD i = 0; i < _countof(addr); i++) {
				MyWriteProcessMemory(GetCurrentProcess(), PVOID(addr[i]+1), &playerobjSize, sizeof(playerobjSize));
			}

			obj_cnWord.Hook(PVOID(0x004A3B11), Naked_cnWord, 6);


			{
				BYTE temp[] = { 0xeb ,0x47 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x005CFE3A, &temp, sizeof(temp));
			}

			{
				BYTE temp[] = { 0x90 ,0x90 ,0x90 ,0x90 ,0x90 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x0047428c, &temp, sizeof(temp));
			}

			{
				BYTE temp[] = { 0xeb,0x52 };
				MyWriteProcessMemory(GetCurrentProcess(), (PVOID)0x004115a8, &temp, sizeof(temp));
			}


			custompacket::Start();

			sahReader::Init();

			return 0;
		}


		void Start()
		{
			::CreateThread(0, 0, ThreadProc, 0, 0, 0);
		}
	}

}

