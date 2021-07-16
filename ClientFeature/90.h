
#include <windows.h>
#include "../utility/utility.h"
#include "../utility/native.h"
#include "../utility/file.h"
#include "../utility/process.h"
#include "kiero.h"
#include <assert.h>
#include <wil/resource.h>
#include <map>
#include "VMProtectSDK.h"
#include <httplib.h>

namespace Shaiya90
{

	std::optional<WORD> get_cur_map()
	{
		__try
		{
			auto cur_map = *reinterpret_cast<PWORD>(0x00863624);
			return cur_map;
		}
		__except (1) {
			return std::nullopt;
		}
	}
	
	void SendPacket(void* p, DWORD  l)
	{
		DWORD dwCall = 0x00595140;
		_asm {
			push l
			push p
			call dwCall
			add esp, 0x8
		}
	}

	namespace skillEffect
	{
		bool g_enabled{};
		bool g_ignore_maps[255]{};

		ShaiyaUtility::CMyInlineHook g_obj1;
		__declspec(naked) void Naked_1()
		{
			_asm
			{
				lea eax, g_enabled
				cmp byte ptr[eax], 0x0
				je __org

				push eax
				push ebx
				mov eax, 0x00863624
				mov al, byte ptr [eax]
				movzx eax,al
				lea ebx, g_ignore_maps
				lea ebx,[ebx+eax]
				mov al,byte ptr [ebx]
				test al,al
				pop ebx
				pop eax
				jne __org

				ret 0x1c

				__org:
				sub esp, 0x24
					push ebp
					mov ebp, [esp + 0x40]
					jmp g_obj1.m_pRet
			}
		}



		ShaiyaUtility::CMyInlineHook g_obj2;
		__declspec(naked) void Naked_2()
		{
			_asm
			{
				lea eax, g_enabled
				cmp byte ptr[eax], 0x0
				je __org

				push eax
				push ebx
				mov eax, 0x00863624
				mov al, byte ptr[eax]
				movzx eax, al
				lea ebx, g_ignore_maps
				lea ebx, [ebx + eax]
				mov al, byte ptr[ebx]
				test al, al
				pop ebx
				pop eax
				jne __org


				ret 0x8

				__org:
				sub esp, 0x1C
					mov eax, [esp + 0x20]
					jmp g_obj2.m_pRet
			}
		}


		ShaiyaUtility::CMyInlineHook g_obj3;
		__declspec(naked) void Naked_3()
		{
			_asm
			{
				lea eax, g_enabled
				cmp byte ptr[eax], 0x0
				je __org

				push eax
				push ebx
				mov eax, 0x00863624
				mov al, byte ptr[eax]
				movzx eax, al
				lea ebx, g_ignore_maps
				lea ebx, [ebx + eax]
				mov al, byte ptr[ebx]
				test al, al
				pop ebx
				pop eax
				jne __org


				ret 0x8

				__org:
				mov eax, [esp + 0x04]
					sub esp, 0x10
					jmp g_obj3.m_pRet
			}
		}

		ShaiyaUtility::CMyInlineHook g_obj4;
		__declspec(naked) void Naked_4()
		{
			_asm
			{
				lea eax, g_enabled
				cmp byte ptr[eax], 0x0
				je __org

				push eax
				push ebx
				mov eax, 0x00863624
				mov al, byte ptr[eax]
				movzx eax, al
				lea ebx, g_ignore_maps
				lea ebx, [ebx + eax]
				mov al, byte ptr[ebx]
				test al, al
				pop ebx
				pop eax
				jne __org


				ret 0x18

				__org:
				mov eax, [esp + 0x04]
					sub esp, 0x8
					jmp g_obj4.m_pRet
			}
		}

		ShaiyaUtility::CMyInlineHook g_obj5;
		__declspec(naked) void Naked_5()
		{
			_asm
			{
				lea eax, g_enabled
				cmp byte ptr[eax], 0x0
				je __org

				push eax
				push ebx
				mov eax, 0x00863624
				mov al, byte ptr[eax]
				movzx eax, al
				lea ebx, g_ignore_maps
				lea ebx, [ebx + eax]
				mov al, byte ptr[ebx]
				test al, al
				pop ebx
				pop eax
				jne __org


				ret 0x14

				__org:
				push ebx
					mov ebx, [esp + 0x14]
					jmp g_obj5.m_pRet
			}
		}


		ShaiyaUtility::CMyInlineHook g_obj6;
		__declspec(naked) void Naked_6()
		{
			_asm
			{
				lea eax, g_enabled
				cmp byte ptr[eax], 0x0
				je __org

				push eax
				push ebx
				mov eax, 0x00863624
				mov al, byte ptr[eax]
				movzx eax, al
				lea ebx, g_ignore_maps
				lea ebx, [ebx + eax]
				mov al, byte ptr[ebx]
				test al, al
				pop ebx
				pop eax
				jne __org


				ret 0xc

				__org:
				mov eax, [esp + 0x04]
					sub esp, 0x10
					jmp g_obj6.m_pRet
			}
		}

		bool __stdcall is_skip_chat(BYTE* packet)
		{
			auto str = reinterpret_cast<char*>(packet + 3);
			if (strcmp(str, "开技效") == 0)
			{
				g_enabled = false;
				return true;
			}
			if (strcmp(str, "关技效") == 0)
			{
				g_enabled = true;
				return true;
			}
			return false;
		}


		ShaiyaUtility::CMyInlineHook g_hookGmChat;
		DWORD orgGmCall = 0x00595140;
		__declspec(naked) void Naked_GmChat()
		{
			_asm
			{
				pushad
				mov eax, dword ptr[esp + 0x20]
				MYASMCALL_1(is_skip_chat, eax)
				test al, al
				popad
				je _org
				jmp g_hookGmChat.m_pRet

				_org :
				call orgGmCall
					jmp g_hookGmChat.m_pRet

			}
		}



		void process_server_message(void* packet)
		{
#ifndef _DEBUG
			VMProtectBegin(__FUNCTION__);
#endif

			auto p = reinterpret_cast<ShaiyaUtility::Packet::NakedSkillEffect*>(packet);
			static bool flag = false;
			if (flag)
			{
				return;
			}
			flag = true;

			ZeroMemory(g_ignore_maps, sizeof(g_ignore_maps));
			for(auto i=0;i<_countof(p->maps);i++)
			{
				const BYTE m = p->maps[i];
				if(m==0)
				{
					continue;
				}
				//in case of map 0
				g_ignore_maps[m-1] = true;
			}
			
			g_obj1.Hook(p->hookObj1, Naked_1, 8);
			g_obj2.Hook(p->hookObj2, Naked_2, 7);
			g_obj3.Hook(p->hookObj3, Naked_3, 7);
			g_obj4.Hook(p->hookObj4, Naked_4, 7);
			g_obj5.Hook(p->hookObj5, Naked_5, 5);
			g_obj6.Hook(p->hookObj6, Naked_6, 7);
			g_hookGmChat.Hook(0x00596c9c, Naked_GmChat, 5);


#ifndef _DEBUG
			VMProtectEnd();
#endif
		}

		void start()
		{
#ifdef _DEBUG
			g_enabled = true;
			/*
			g_obj1.Hook(0x00450370, Naked_1, 8);
			g_obj2.Hook(0x0040f620, Naked_2, 7);
			g_obj3.Hook(0x00432A80, Naked_3, 7);
			g_obj4.Hook(0x0044FEC0, Naked_4, 7);
			g_obj5.Hook(0x00450300, Naked_5, 5);
			g_obj6.Hook(0x00417750, Naked_6, 7);
			g_hookGmChat.Hook(0x00596c9c, Naked_GmChat, 5);
			*/
#endif
		}
	}

	namespace NewMountFeature
	{
		struct model
		{
			bool enabled{};
			bool need_rotate{};
			float height{};
			DWORD boneId{};
		};
		
		std::array<model, 255> g_models;

		ShaiyaUtility::CMyInlineHook g_objRotate1, g_objRotate2, g_objRotate3, g_objRotate4;
		ShaiyaUtility::CMyInlineHook g_objBoneId1; // human
		ShaiyaUtility::CMyInlineHook g_objBoneId2;
		ShaiyaUtility::CMyInlineHook g_objBoneId3;// 
		ShaiyaUtility::CMyInlineHook g_objBoneId4;
		ShaiyaUtility::CMyInlineHook g_objBoneId5;// 
		ShaiyaUtility::CMyInlineHook g_objBoneId6;
		ShaiyaUtility::CMyInlineHook g_objBoneId7;// 
		ShaiyaUtility::CMyInlineHook g_objBoneId8;
		ShaiyaUtility::CMyInlineHook g_objFixHeight;

		DWORD g_bone1Addr = 0x004132E9;


		void __stdcall fix_height(DWORD mountObj,BYTE modelId)
		{
			auto model = g_models[modelId];
			if (!model.enabled)
			{
				return;
			}
			
			if(model.height< 0.1)
			{
				return;
			}
			
			*(PFLOAT(mountObj + 0xf8)) = model.height;
		}


		bool __stdcall is_rotate_player(BYTE modelId)
		{
			return  g_models[modelId].enabled && g_models[modelId].need_rotate;
		}

		DWORD __stdcall get_bone_id(BYTE modelId)
		{
			auto model = g_models[modelId];
			if(!model.enabled)
			{
				return 0;
			}
			return model.boneId;
		}

		__declspec(naked) void Naked_fixBone1()
		{
			_asm
			{
				mov ecx, dword ptr ds : [ebx + 0x294]
				push ebx
				push ecx
				push edx
				push esi
				push edi
				push esp
				push ebp

				
				movzx eax,byte ptr [ebx+0x385]
				MYASMCALL_1(get_bone_id,eax)
				
				pop ebp
				pop esp
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx

				cmp eax,0x0
				je _Org
				push eax
				push g_objBoneId1.m_pRet
				add dword ptr [esp],0x2
				ret
				_Org:
				jmp g_objBoneId1.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone7()
		{
			_asm
			{
				mov ecx, dword ptr ds : [ebx + 0x294]
				push ebx
				push ecx
				push edx
				push esi
				push edi
				push esp
				push ebp


				movzx eax, byte ptr[ebx + 0x385]
				MYASMCALL_1(get_bone_id, eax)

				pop ebp
				pop esp
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx

				cmp eax, 0x0
				je _Org
				jmp g_bone1Addr
				ret
				_Org :
				jmp g_objBoneId7.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone8()
		{
			_asm
			{
				mov ecx, dword ptr ds : [ebx + 0x294]
				push ebx
				push ecx
				push edx
				push esi
				push edi
				push esp
				push ebp


				movzx eax, byte ptr[ebx + 0x385]
				MYASMCALL_1(get_bone_id, eax)

				pop ebp
				pop esp
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx

				cmp eax, 0x0
				je _Org
				push eax
				push g_objBoneId8.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId8.m_pRet
			}
		}



		__declspec(naked) void Naked_fixBone2()
		{
			_asm
			{
				mov ecx, dword ptr ds : [ebx + 0x294]
				push ebx
				push ecx
				push edx
				push esi
				push edi
				push esp
				push ebp


				movzx eax, byte ptr[ebx + 0x385]
				MYASMCALL_1(get_bone_id, eax)

				pop ebp
				pop esp
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx

				cmp eax, 0x0
				je _Org
				push eax
				push g_objBoneId2.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId2.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone3()
		{
			_asm
			{
				mov ecx, dword ptr ds : [ebx + 0x294]
				push ebx
				push ecx
				push edx
				push esi
				push edi
				push esp
				push ebp


				movzx eax, byte ptr[ebx + 0x385]
				MYASMCALL_1(get_bone_id, eax)

				pop ebp
				pop esp
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx

				cmp eax, 0x0
				je _Org
				jmp g_bone1Addr
				
				ret
				_Org :
				jmp g_objBoneId3.m_pRet
			}
		}



		__declspec(naked) void Naked_fixBone5()
		{
			_asm
			{
				mov ecx, dword ptr ds : [ebx + 0x294]
				push ebx
				push ecx
				push edx
				push esi
				push edi
				push esp
				push ebp


				movzx eax, byte ptr[ebx + 0x385]
				MYASMCALL_1(get_bone_id, eax)

				pop ebp
				pop esp
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx

				cmp eax, 0x0
				je _Org
				jmp g_bone1Addr
				_Org :
				jmp g_objBoneId5.m_pRet
			}
		}


		__declspec(naked) void Naked_fixBone6()
		{
			_asm
			{
				mov ecx, dword ptr ds : [ebx + 0x294]
				push ebx
				push ecx
				push edx
				push esi
				push edi
				push esp
				push ebp


				movzx eax, byte ptr[ebx + 0x385]
				MYASMCALL_1(get_bone_id, eax)

				pop ebp
				pop esp
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx
				
				cmp eax, 0x0
				je _Org
				push eax
				push g_objBoneId6.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId6.m_pRet
			}
		}


		__declspec(naked) void Naked_fixBone4()
		{
			_asm
			{
				mov ecx, dword ptr ds : [ebx + 0x294]
				push ebx
				push ecx
				push edx
				push esi
				push edi
				push esp
				push ebp


				movzx eax, byte ptr[ebx + 0x385]
				MYASMCALL_1(get_bone_id, eax)

				pop ebp
				pop esp
				pop edi
				pop esi
				pop edx
				pop ecx
				pop ebx

				cmp eax, 0x0
				je _Org
				push eax
				push g_objBoneId4.m_pRet
				add dword ptr [esp], 0x2
				ret
				_Org :
				jmp g_objBoneId4.m_pRet
			}
		}


		__declspec(naked) void Naked_rotate_4()
		{
			_asm
			{
				pushad
				movzx eax, byte ptr[ebx + 0x385]
				MYASMCALL_1(is_rotate_player, eax)
				test al, al
				popad
				je __Org
				mov dword ptr[eax + 0x28], 0x3f800000 // float 1

				__Org:
				push eax
					lea eax, dword ptr [esp + 0x1B8]
					jmp g_objRotate4.m_pRet
			}
		}



		__declspec(naked) void Naked_rotate_3()
		{
			_asm
			{
				pushad
				movzx eax, byte ptr[ebx + 0x385]
				MYASMCALL_1(is_rotate_player, eax)
				test al, al
				popad
				je __Org
				mov dword ptr[eax + 0x28], 0x3f800000 // float 1

				__Org:
				push eax
					lea eax, dword ptr  [esp + 0x168]
					jmp g_objRotate3.m_pRet
			}
		}



		__declspec(naked) void Naked_rotate_2()
		{
			_asm
			{
				pushad
				movzx eax, byte ptr[ebx + 0x385]
				MYASMCALL_1(is_rotate_player, eax)
				test al, al
				popad
				je __Org
				mov dword ptr[eax + 0x28], 0x3f800000 // float 1

				__Org:
				push eax
					lea ecx, dword ptr[esp + 0x54]
					jmp g_objRotate2.m_pRet
			}
		}
		

		__declspec(naked) void Naked_rotate()
		{
			_asm
			{
				pushad
				movzx eax,byte ptr [ebx+0x385]
				MYASMCALL_1(is_rotate_player,eax)
				test al,al
				popad
				je __Org
				mov dword ptr[eax + 0x28],0x3f800000 // float 1

				__Org:
				push eax
				lea ecx, dword ptr [esp + 0x54]
				jmp g_objRotate1.m_pRet
			}
		}

		DWORD dwCallOrgFixHeightCall = 0x00552EC0;
		__declspec(naked) void Naked_fixHeight()
		{
			_asm
			{
				pushad
				mov eax,dword ptr [esi+0x74] 
				MYASMCALL_2(fix_height,ecx,eax)
				popad
				call dwCallOrgFixHeightCall
				jmp g_objFixHeight.m_pRet
			}
		}


		void process_server_message(void* packet)
		{
#ifndef _DEBUG
			VMProtectBegin(__FUNCTION__);
#endif
			ShaiyaUtility::Packet::NewHiddenMount* p = (struct ShaiyaUtility::Packet::NewHiddenMount*)packet;

			g_models.fill({});

			for (int i = 0;i <100;i++)
			{
				const BYTE id = p->models[i].id;
				if(id==0)
				{
					continue;
				}
				g_models[id] = { true, p->models[i].need_rotate,p->models[i].height,p->models[i].boneId };
			}
			static bool flag = false;
			if (flag)
			{
				return;
			}
			flag = true;
			BYTE limitMountModel = 0x57;
			ShaiyaUtility::WriteCurrentProcessMemory((void*)0x004C81F5, &limitMountModel, 1);
		    g_objRotate1.Hook(reinterpret_cast<void*>(p->Naked_fixCharacterRotate1), Naked_rotate, 5);
	//		g_objRotate2.Hook(reinterpret_cast<void*>(p->Naked_fixCharacterRotate2), Naked_rotate_2, 5);
	//		g_objRotate3.Hook(reinterpret_cast<void*>(p->Naked_fixCharacterRotate3), Naked_rotate_3, 8);
	//		g_objRotate4.Hook(reinterpret_cast<void*>(p->Naked_fixCharacterRotate4), Naked_rotate_4, 8);
			
			g_objBoneId1.Hook(reinterpret_cast<void*>(p->Naked_fixboneId1), Naked_fixBone1, 6);
		    g_objBoneId2.Hook(reinterpret_cast<void*>(p->Naked_fixboneId2), Naked_fixBone2, 6);
			
			g_objBoneId3.Hook(reinterpret_cast<void*>(p->Naked_fixboneId3), Naked_fixBone3, 6);
			g_objBoneId5.Hook(reinterpret_cast<void*>(p->Naked_fixboneId5), Naked_fixBone5, 6);
			g_objBoneId7.Hook(reinterpret_cast<void*>(p->Naked_fixboneId7), Naked_fixBone7, 6);



			g_objFixHeight.Hook(reinterpret_cast<void*>(p->Naked_fixHeight), Naked_fixHeight, 5);

     
			
#ifndef _DEBUG
			VMProtectEnd();
#endif
		}

		void start()
		{
#ifndef _DEBUG
			VMProtectBegin(__FUNCTION__);
#endif
	//		BYTE maxObjectSize = 0x28;
	//		ShaiyaUtility::WriteCurrentProcessMemory((void*)0x0041799A, &maxObjectSize, 1);

	
			
#ifndef _DEBUG
			VMProtectEnd();
#endif
		}
	}





	namespace multipleClient
	{


		std::optional<std::wstring> get_process_name(DWORD processID)
		{
			// Get a handle to the process.
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
				PROCESS_VM_READ,
				FALSE, processID);
			if (hProcess == nullptr)
			{
				return std::nullopt;
			}

			// Get the process name.
			wchar_t szProcessName[MAX_PATH]{};
			GetProcessImageFileName(hProcess, szProcessName, MAX_PATH);
			auto ret = PathFindFileName(szProcessName);
			CloseHandle(hProcess);

			return ret;
		}


		std::optional<int> get_game_nums()
		{
			DWORD aProcesses[1024]{};
			DWORD cbNeeded{ };
			DWORD cProcesses{};
			unsigned int i{};

			if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
			{
				return std::nullopt;
			}

			auto count = 0;
			cProcesses = cbNeeded / sizeof(DWORD);
			for (i = 0; i < cProcesses; i++)
			{
				if (aProcesses[i] == 0)
				{
					continue;
				}

				auto name = get_process_name(aProcesses[i]);
				if (!name)
				{
					continue;
				}
				if (wcsicmp(name->c_str(), L"Game.exe") == 0)
				{
					count++;
				}
			}

			return count;
		}


		int get_client_allowed_num()
		{
			char dir[MAX_PATH]{};
			GetCurrentDirectoryA(MAX_PATH, dir);

			std::string iniFile(dir);
			iniFile.append("/ctserver.ini");

			char ip[256]{};
			GetPrivateProfileStringA("servershuangxian", "server1_ip", "127.0.217.58", ip, 256, iniFile.c_str());


			char url[MAX_PATH]{};
			sprintf_s(url, MAX_PATH, "http://%s:7088", ip);

			httplib::Client cli(url);
			auto res = cli.Get("/shaiya/allowed_open_clients.txt");
			if (!res)
			{
				return 1;
			}
			return std::stoi(res->body);
		}

		void worker(void*)
		{
			auto allowed_clients = get_client_allowed_num();
			auto cur_clients = get_game_nums();
			if (!cur_clients)
			{
				return;
			}

			if (cur_clients.value() > allowed_clients)
			{
				ExitProcess(0);
			}
		}

		void start()
		{
			_beginthread(worker, 0, 0);
		}

	}


	namespace SkillCutting {

		ShaiyaUtility::CMyInlineHook g_fully;


		void* g_fullySkipAddr = reinterpret_cast<void*>(0x0043d635);
		void* g_fullyBlockAddr = reinterpret_cast<void*>(0x004418ca);
		__declspec(naked) void Naked_fully()
		{
			_asm
			{
				cmp eax, 0x2    // normal attack
				je _skip
				cmp eax, 0x9    // skill
				je _skip

				cmp eax, 0x8       // org
				jne _blockAddr
				jmp g_fully.m_pRet


				_blockAddr :
				jmp g_fullyBlockAddr

					_skip :
				jmp g_fullySkipAddr
			}
		}

		void FullySkillCutting() {
			static bool  flag = false;
			if (flag)
			{
				return;
			}
			g_fully.Hook(reinterpret_cast<void*>(0x0043d62f), Naked_fully, 6);
			flag = true;

		}

		void ProcessPacket(PVOID Packet) {
			FullySkillCutting();
		}
	}

	namespace namecolor
	{
		ShaiyaUtility::CMyInlineHook  objNormal, ObjParty;
		DWORD g_randomColor = 0xffffff;
#define MAX_COLORS  (100000)
		DWORD g_colors[MAX_COLORS] = { 0 };

		void StartWorker();


		void __stdcall SetColor(DWORD Player, PDWORD ColotOutput) {

			DWORD charid = *PDWORD(Player + 0x34);

			if (charid > MAX_COLORS - 1) {
				return;
			}

			auto rgb = g_colors[charid];
			if (rgb == ShaiyaUtility::Packet::RandomColor) {
				*ColotOutput = g_randomColor;
				return;
			}
			else if (rgb == 0) {
				return;
			}

			*ColotOutput = rgb;
		}


		void ProcessPacket(PVOID Packet) {

			auto nameColorPacket = reinterpret_cast<ShaiyaUtility::Packet::NameColorContent*>(Packet);

			StartWorker();

			memset(g_colors, 0, sizeof(g_colors));
			for (DWORD i = 0; i < ARRAYSIZE(nameColorPacket->players); i++) {

				auto charid = nameColorPacket->players[i].charid;
				auto rgb = nameColorPacket->players[i].rgb;


				if (!charid) {
					continue;
				}

				if (!rgb) {
					continue;
				}

				if (charid > MAX_COLORS - 1) {
					return;
				}

				g_colors[charid] = 0xff000000 | rgb;
			}
		}

		void GenerateRandomColor(void*) {

			DWORD dwTemp = 0;
			while (true)
			{
				for (int i = 0; i < ARRAYSIZE(ShaiyaUtility::Packet::s_RandomColorList); i++) {
					g_randomColor = 0xff000000 | ShaiyaUtility::Packet::s_RandomColorList[i];
					Sleep(500);
				}

			}

		}

		__declspec(naked) void Naked_Normal()
		{
			_asm
			{
				mov dword ptr ss : [esp + 0x18] , 0xffffffff
				pushad
				lea eax, [esp + 0x38]
				push eax //color
				push esi
				call SetColor
				popad
				sub eax, 0x4
				jmp objNormal.m_pRet
			}
		}
		__declspec(naked) void Naked_Party()
		{

			_asm
			{
				mov dword ptr ss : [esp + 0x18] , 0xFF00FF00
				pushad
				lea eax, [esp + 0x38]
				push eax //color
				push esi
				call SetColor
				popad
				jmp ObjParty.m_pRet
			}
		}

		void StartWorker() {

			static bool flag = false;
			if (flag)
			{
				return;
			}
			flag = true;
			objNormal.Hook((PVOID)0x0044b5c6, Naked_Normal, 11);
			ObjParty.Hook((PVOID)0x0044b664, Naked_Party, 8);
			_beginthread(GenerateRandomColor, 0, nullptr);

		}
	}

	namespace killsRanking {

		void ProcessPacket(PVOID p) {

			auto packet = reinterpret_cast<ShaiyaUtility::Packet::KillsRankingContent*>(p);

			static bool flag = false;
			if (flag)
			{
				return;
			}
			flag = true;

			ShaiyaUtility::WriteCurrentProcessMemory(reinterpret_cast<void*>(0x00708298),
				packet->killsNeeded,
				sizeof(packet->killsNeeded));

			ShaiyaUtility::WriteCurrentProcessMemory(reinterpret_cast<void*>(0x00708118),
				packet->normalModePoints,
				sizeof(packet->normalModePoints));


			ShaiyaUtility::WriteCurrentProcessMemory(reinterpret_cast<void*>(0x00708198),
				packet->hardModePoints,
				sizeof(packet->hardModePoints));

			ShaiyaUtility::WriteCurrentProcessMemory(reinterpret_cast<void*>(0x00708218),
				packet->ultimateModePoints,
				sizeof(packet->ultimateModePoints));

		}

	}

	namespace Combination {

		void Process(void* P) {
			{
				BYTE temp[] = { 0x90,0x90 };
				ShaiyaUtility::WriteCurrentProcessMemory((void*)0x004A954D, temp, sizeof(temp));
				ShaiyaUtility::WriteCurrentProcessMemory((void*)0x004A9555, temp, sizeof(temp));
			}

			{
				BYTE temp[] = { 0xE9 ,0xA6 ,0x00 ,0x00 ,0x00 };
				ShaiyaUtility::WriteCurrentProcessMemory((void*)0x004A945B, temp, sizeof(temp));
			}


			{
				BYTE temp[] = { 0xb0,0x01 };
				ShaiyaUtility::WriteCurrentProcessMemory((void*)0x004A9600, temp, sizeof(temp));
			}
		}
	}


	namespace getEnhanceAttack {

		ShaiyaUtility::CMyInlineHook g_obj;

		DWORD g_data[51] = {};

		void Process(void* P) {
			auto packet = static_cast<ShaiyaUtility::Packet::EnhanceAttack*>(P);
			for (auto i = 0; i < ARRAYSIZE(g_data); i++) {
				g_data[i] = packet->values[i];
			}
		}

		DWORD __stdcall  ReadAttack(DWORD Index) {
			if (Index > ARRAYSIZE(g_data)) {
				return 0;
			}
			return g_data[Index];
		}

		__declspec(naked) void Naked()
		{
			_asm {
				push ebx
				push ecx
				push edx
				push ebp
				push esp
				push edi
				push esi
				MYASMCALL_1(ReadAttack, eax)
				pop esi
				pop edi
				pop esp
				pop ebp
				pop edx
				pop ecx
				pop ebx
				add esp, 0x4
				jmp g_obj.m_pRet
			}
		}

		void Start() {
			g_obj.Hook(reinterpret_cast<void*>(0x004A45E4), Naked, 5);
		}
	}


	namespace custompacket
	{
		ShaiyaUtility::CMyInlineHook objPacket;
		static DWORD codeBoundary = ShaiyaUtility::Packet::Code::begin;

		void __stdcall CustomPacket(PVOID P) {

			auto cmd = static_cast<ShaiyaUtility::Packet::Header*>(P);
			switch (cmd->command) {

			case ShaiyaUtility::Packet::Code::nameColor: {
				namecolor::ProcessPacket(P);
				break;
			}

			case ShaiyaUtility::Packet::Code::skillCutting: {
				SkillCutting::ProcessPacket(P);
				break;
			}

			case ShaiyaUtility::Packet::Code::killsRanking: {
				killsRanking::ProcessPacket(P);
				break;
			}

			case ShaiyaUtility::Packet::Code::itemCombines: {
				Combination::Process(P);
				break;
			}
			case ShaiyaUtility::Packet::enhanceAttack: {
				getEnhanceAttack::Process(P);
				break;
			}
			case ShaiyaUtility::Packet::hiddenMountCode:
				{
				NewMountFeature::process_server_message(P);
				break;
				}
			case ShaiyaUtility::Packet::NakedSkillSetting:
				{
				skillEffect::process_server_message(P);
				break;
				}
				
			default:
				break;
			}
		}

		__declspec(naked) void Naked1()
		{
			_asm
			{
				cmp eax, codeBoundary
				jl _orginal
				pushad
				mov eax, [esp + 0x28]
				push eax            //packets
				call CustomPacket
				popad
				_orginal :
				cmp eax, 0xFA07
					jmp objPacket.m_pRet
			}
		}



		void Start()
		{
			//	0059BC9B | > \3D 07FA0000         cmp eax, 0xFA07
			objPacket.Hook(reinterpret_cast<void*>(0x0059BC9B), Naked1, 5);
		}
	}

	namespace ijl15Detection {

		void Start() {

			auto removeDetection = [](PVOID addr, PVOID OrgData, PVOID changedata, DWORD len) {
				if (memcmp(OrgData, addr, len) == 0) {
					ShaiyaUtility::WriteCurrentProcessMemory(addr, changedata, len);
				}
			};

			{
				BYTE temp[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0x74 ,0x52 };
				BYTE temp2[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0xEB ,0x52 };
				removeDetection(PVOID(0x0040c513), temp, temp2, sizeof(temp));
			}


			{
				BYTE temp[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0x74 ,0x61 };
				BYTE temp2[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0xeb ,0x61 };
				removeDetection(PVOID(0x0040B5B6), temp, temp2, sizeof(temp));
			}

			// 9.0 3.14mb
			{
				BYTE temp[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0x74 ,0x61 };
				BYTE temp2[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0xeb ,0x61 };
				removeDetection(PVOID(0x0040C88F), temp, temp2, sizeof(temp));
			}

			{
				BYTE temp[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0x74 ,0x61 };
				BYTE temp2[] = { 0x3D ,0x00 ,0xB0 ,0x05 ,0x00 ,0xeb ,0x61 };
				removeDetection(PVOID(0x0040bd16), temp, temp2, sizeof(temp));
			}
		}
	}

	namespace ReadCharDecryption {

		void __stdcall Decrypt(_In_ BYTE* Buf) {

			if (Buf[2] < 10) {
				return;
			}

			BYTE signature = Buf[2];
			BYTE len = Buf[3];
			for (BYTE i = 0; i < len; i++) {
				Buf[i + 2] = Buf[i + 4] ^ signature;
			}
		}

		ShaiyaUtility::CMyInlineHook g_obj;
		__declspec(naked) void Naked()
		{
			_asm
			{
				mov eax, dword ptr ss : [esp + 0x8]
				push eax
				pushad
				MYASMCALL_1(Decrypt, eax)
				popad
				jmp g_obj.m_pRet
			}
		}

		void Start() {
			g_obj.Hook(0x0059a187, Naked);
		}
	}

	namespace antiHacker {

		namespace AntiSpeedHacke {

			DWORD Get_InternalEnumWindows_Addr()
			{
				PBYTE base = (PBYTE)GetProcAddress(GetModuleHandle(L"user32.dll"), "EnumWindows");
				DWORD addr = 0;
				while (TRUE)
				{
					if (*base == 0xe8) {
						addr = DWORD(base + *PINT(base + 1) + 5);
						break;
					}
					base++;
				}
				return addr;
			}

			bool IsHacked()
			{

				auto IsHook = [](PBYTE pfun)->bool {
					if (pfun && *pfun == 0xe9)
						return true;
					return false;
				};


				HMODULE hKernel32 = GetModuleHandle(L"Kernel32.dll");
				HMODULE hWinmmm = GetModuleHandle(L"Winmm.dll");
				HMODULE hUser32 = GetModuleHandle(L"user32.dll");
				HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
				PBYTE pfunGetTickCount64 = NULL;
				PBYTE pfunQueryPerformanceCounter = NULL;
				PBYTE pfunGetTickCount = NULL;
				PBYTE pfuntimeGetTime = NULL;
				PBYTE pfunFindWindow = NULL;
				PBYTE pDbgUiRemoteBreakin = NULL;

				bool bRet = true;

				if (hKernel32)
				{
					pfunGetTickCount64 = (PBYTE)GetProcAddress(hKernel32, "GetTickCount64");
					pfunQueryPerformanceCounter = (PBYTE)GetProcAddress(hKernel32, "QueryPerformanceCounter");
					pfunGetTickCount = (PBYTE)GetProcAddress(hKernel32, "GetTickCount");
				}
				if (hWinmmm)
					pfuntimeGetTime = (PBYTE)GetProcAddress(hWinmmm, "timeGetTime");

				if (hUser32)
				{
					pfunFindWindow = (PBYTE)GetProcAddress(hUser32, "FindWindowA");
				}

				if (hNtdll) {
					pDbgUiRemoteBreakin = (PBYTE)GetProcAddress(hNtdll, "DbgUiRemoteBreakin");
				}



				DWORD InternalEnumWindows_addr = Get_InternalEnumWindows_Addr();

				std::vector<DWORD> addrs = { 0x00595e40 ,
	0x00595ea0,
	0x0059a64c,
	0x0059a710,
	0x0059a72c,
	0x0041d21c,
	DWORD(pfunGetTickCount),
	DWORD(pfunGetTickCount64) ,
	DWORD(pfunQueryPerformanceCounter),
	DWORD(pfuntimeGetTime),
	DWORD(pfunFindWindow),
	DWORD(pDbgUiRemoteBreakin),
	DWORD(InternalEnumWindows_addr) };




				for (auto addr : addrs) {
					bRet = IsHook((PBYTE)addr);
					if (bRet) {
						break;
					}
				}

				return bRet;
			}
		}

		HRESULT tectAndCloseHakcingProcHnd() {
#ifndef _DEBUG
			VMProtectBegin("DetectAndCloseHakcingProcHnd");
#endif
			static std::map<DWORD, DWORD> ignoredPids;
			auto isIgnoredPid = [&](DWORD pid)->bool
			{
				if (pid == 4)
					return true;
				auto iter = ignoredPids.find(pid);
				return iter != ignoredPids.end();
			};

			PSYSTEM_HANDLE_INFORMATION p;
			RETURN_IF_FAILED(Utility::Native::ZwQuerySystemInformation_SysmHndInfo(&p));
			auto cleanup = wil::scope_exit([&]() {delete p; });

			for (DWORD i = 0; i < p->dwCount; i++) {

				const auto& handle = p->Handles[i];

				// not process type
				if (handle.bObjectType != 7) {
					continue;
				}

				// ignore current process
				if (handle.dwProcessId == GetCurrentProcessId()) {
					continue;
				}

				// must have write process memory
				if (!(handle.GrantedAccess & PROCESS_VM_WRITE))
				{
					continue;
				}

				//
				if (isIgnoredPid(handle.dwProcessId)) {
					continue;
				}

				// get handle process id
				HANDLE hObject = reinterpret_cast<HANDLE>(handle.wValue);

				// try to open target process
				wil::unique_process_handle proHnd(OpenProcess(
					PROCESS_DUP_HANDLE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ
					, 0, handle.dwProcessId));
				if (!proHnd) {
					ignoredPids.insert(std::pair<DWORD, DWORD>(handle.dwProcessId, GetTickCount()));
					continue;
				}

				wil::unique_process_handle dupHnd;
				if (!(DuplicateHandle(proHnd.get(),
					hObject,
					GetCurrentProcess(),
					&dupHnd, 0, 0,
					DUPLICATE_SAME_ATTRIBUTES | DUPLICATE_SAME_ACCESS)))
				{
					continue;
				}

				// check handle process id
				if (GetProcessId(dupHnd.get()) != GetCurrentProcessId()) {
					continue;
				}

				// get exe file path
				WCHAR name[MAX_PATH]{};
				if (!GetModuleFileNameEx(proHnd.get(), nullptr, name, MAX_PATH)) {
					continue;
				}

				BOOLEAN trusted = FALSE;
				if (FAILED(Utility::File::HasTrustedSignature(name, &trusted))) {
					continue;
				}
				if (trusted) {
					ignoredPids.insert(std::pair<DWORD, DWORD>(handle.dwProcessId, GetTickCount()));
					continue;
				}

				dupHnd.release();
				// now we try to close source handle
				DuplicateHandle(proHnd.get(), hObject, GetCurrentProcess(), &dupHnd, 0,
					FALSE, DUPLICATE_CLOSE_SOURCE);
			}

#ifndef _DEBUG
			VMProtectEnd();
#endif		
			RETURN_HR(S_OK);
		}

		bool illegalMemCheck()
		{
#ifndef _DEBUG
			VMProtectBegin("illegalMemCheck");
#endif
			auto memInfos = Utility::Process::GetMemRanges(GetCurrentProcess(), [](const MEMORY_BASIC_INFORMATION& info)->bool {

				if (info.Type != MEM_PRIVATE) {
					return false;
				}

				if (info.RegionSize < 0x2000) {
					return false;
				}

				return (info.Protect & PAGE_EXECUTE) ||
					(info.Protect & PAGE_EXECUTE_READ) ||
					(info.Protect & PAGE_EXECUTE_READWRITE) ||
					(info.Protect & PAGE_EXECUTE_WRITECOPY);
				}
			);

			if (memInfos.empty()) {
				return false;
			}

			std::vector<MODULEINFO> mods;
			auto hr = Utility::Process::EnumProcessModules(GetCurrentProcess(), &mods);
			if (FAILED(hr)) {
				return false;
			}

			for (const auto& mem : memInfos) {

				bool legal = false;
				for (const auto& mod : mods) {
					const auto modTail = reinterpret_cast<INT64>(mod.EntryPoint) + mod.SizeOfImage;
					const auto memTail = reinterpret_cast<INT64>(mem.BaseAddress) + mem.RegionSize;
					if (mem.BaseAddress >= mod.EntryPoint && modTail > memTail) {
						legal = true;
						break;
					}
				}

				if (!legal) {
					WCHAR temp[MAX_PATH]{};
					StringCchPrintf(temp, MAX_PATH, L"found illegal mem:%p,size:%x\n", mem.BaseAddress, mem.RegionSize);
					OutputDebugStringW(temp);
					return true;
				}
			}
			return false;
#ifndef _DEBUG
			VMProtectEnd();
#endif
		}

		void SendServer()
		{
			while (true)
			{
				if (*PDWORD(0x00863638))
				{
					Sleep(50 * 1000);
					BYTE p[4] = {};
					*reinterpret_cast<PDWORD>(p) = 0xfe01;
					SendPacket(p, sizeof(p));
					return;
				}
				Sleep(3 * 1000);
			}
		}


		void ThreadProc(_In_  LPVOID lpParameter) {
#ifndef _DEBUG
			VMProtectBegin("ThreadProc");
#endif
			while (true)
			{
				if (illegalMemCheck())
				{
			//		TerminateProcess(GetCurrentProcess(), 0);
					SendServer();
					break;
				}
				tectAndCloseHakcingProcHnd();
				if (AntiSpeedHacke::IsHacked()) {
					SendServer();
					break;
				}
				Sleep(5000);
			}
#ifndef _DEBUG
			VMProtectEnd();
#endif
		}

		void Start() {
			_beginthread(ThreadProc, 0, 0);
		}
	}


	void VerifyParentProcess()
	{
		DWORD parentPid = {};
		auto hr = Utility::Process::GetCurProcessParentId(&parentPid);
		if (FAILED(hr))
		{
			ExitProcess(0);
		}

		auto name = Utility::Process::GetProcessName(parentPid);
		if (!name)
		{
			ExitProcess(0);
		}

		if (wcsicmp(name.value().c_str(), L"updater.exe") != 0)
		{
			ExitProcess(0);
		}
	}




	void Start()
	{
		//		VerifyParentProcess();

		custompacket::Start();
		ijl15Detection::Start();
		antiHacker::Start();
		ReadCharDecryption::Start();
		getEnhanceAttack::Start();
		multipleClient::start();
		NewMountFeature::start();
		skillEffect::start();
	}

}

