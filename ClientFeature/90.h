
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

	std::optional<DWORD> useruid() {
		auto obj = ShaiyaUtility::read<DWORD>(0x2246f80);
		if (obj == 0) {
			return std::nullopt;
		}
		auto uid = ShaiyaUtility::read<DWORD>(obj + 0x1f8);
		if (uid == 0) {
			return std::nullopt;
		}
		return std::optional<DWORD>(uid);
	}


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
				mov al, byte ptr[eax]
				movzx eax, al
				lea ebx, g_ignore_maps
				lea ebx, [ebx + eax]
				mov al, byte ptr[ebx]
				test al, al
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
			for (auto i = 0;i < _countof(p->maps);i++)
			{
				const BYTE m = p->maps[i];
				if (m == 0)
				{
					continue;
				}
				//in case of map 0
				g_ignore_maps[m - 1] = true;
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

		ShaiyaUtility::CMyInlineHook g_objRotate1,
			g_objRotate2,
			g_objRotate3,
			g_objRotate4,
			g_objRotateDouble1, g_objRotateDouble2;
		ShaiyaUtility::CMyInlineHook g_objBoneId1; // human
		ShaiyaUtility::CMyInlineHook g_objBoneId2;
		ShaiyaUtility::CMyInlineHook g_objBoneId3;// 
		ShaiyaUtility::CMyInlineHook g_objBoneId4;
		ShaiyaUtility::CMyInlineHook g_objBoneId5;// 
		ShaiyaUtility::CMyInlineHook g_objBoneId6;
		ShaiyaUtility::CMyInlineHook g_objBoneId7;// 
		ShaiyaUtility::CMyInlineHook g_objBoneId8;
		ShaiyaUtility::CMyInlineHook g_objBoneId9;
		ShaiyaUtility::CMyInlineHook g_objBoneId10;
		ShaiyaUtility::CMyInlineHook g_objBoneId11;
		ShaiyaUtility::CMyInlineHook g_objBoneId12;
		ShaiyaUtility::CMyInlineHook g_objBoneId13;
		ShaiyaUtility::CMyInlineHook g_objBoneId14;
		ShaiyaUtility::CMyInlineHook g_objBoneId15;
		ShaiyaUtility::CMyInlineHook g_objBoneId16;
		ShaiyaUtility::CMyInlineHook g_objBoneId17;
		ShaiyaUtility::CMyInlineHook g_objBoneId18;
		ShaiyaUtility::CMyInlineHook g_objBoneId19;
		ShaiyaUtility::CMyInlineHook g_objBoneId20;
		ShaiyaUtility::CMyInlineHook g_objBoneId21;
		ShaiyaUtility::CMyInlineHook g_objBoneId22;
		ShaiyaUtility::CMyInlineHook g_objBoneId23;
		ShaiyaUtility::CMyInlineHook g_objBoneId24;
		ShaiyaUtility::CMyInlineHook g_objBoneId25;
		ShaiyaUtility::CMyInlineHook g_objBoneId26, g_objBoneId27, g_objBoneId28, g_objBoneId29, g_objBoneId30, g_objBoneId31, g_objBoneId32, g_objBoneId33, g_objBoneId34, g_objBoneId35, g_objBoneId36, g_objBoneId37, g_objBoneId38, g_objBoneId39, g_objBoneId40, g_objBoneId41, g_objBoneId42, g_objBoneId43, g_objBoneId44;
		ShaiyaUtility::CMyInlineHook g_objFixHeight;
		ShaiyaUtility::CMyInlineHook g_objFixRotate_obj;
		ShaiyaUtility::CMyInlineHook g_objFixRotate_obj_1;
		ShaiyaUtility::CMyInlineHook g_objFixRotate_obj_2;
		ShaiyaUtility::CMyInlineHook g_objFixRotate_obj_3;
		ShaiyaUtility::CMyInlineHook g_objFixRotate_obj_4;
		ShaiyaUtility::CMyInlineHook g_objFixRotate_obj_5;
		ShaiyaUtility::CMyInlineHook g_objFixRotate_obj_6;
		ShaiyaUtility::CMyInlineHook g_objSkipSwitchAni;
		ShaiyaUtility::CMyInlineHook g_objSkipSwitchAni_2;

		DWORD g_bone1Addr = 0x004132E9;
		DWORD g_boneDouble1Addr = 0x004121C3;

		std::map<const char*, BYTE> g_aniBones;
		BYTE g_boneId = 0;

		float rotate_float(float x) {
			if (x > 0) {
				return 0 - x;
			}
			else {
				return std::abs(x);
			}
		}

		void __stdcall markRotate(DWORD aniObject, DWORD unkArg) {
			auto aniName = reinterpret_cast<char*>(aniObject + 0x28);
			for (const auto& aniBone : g_aniBones) {
				if (strstr(aniName, aniBone.first) != 0) {
					g_boneId = aniBone.second;
					break;
				}
			}

			DWORD dwCall = 0x0054FC10;
			_asm {
				push unkArg
				mov ecx,aniObject
				call dwCall
			}
			g_boneId = 0;
		}

		bool __stdcall skipSwitchAni(DWORD aniObject) {
			auto aniName = reinterpret_cast<char*>(aniObject + 0x28);
			for (const auto& aniBone : g_aniBones) {
				if (strstr(aniName, aniBone.first) != 0) {
					return true;
				}
			}
			return false;
		}

		void __stdcall doRotate(DWORD obj,DWORD boneId) {
			if (boneId != 0 && boneId == g_boneId) {
				auto p1 = reinterpret_cast<float*>(obj - 0x20);
				auto p2 = reinterpret_cast<float*>(obj - 0x20+0x8);
				*p1 = rotate_float(*p1);
				*p2 = rotate_float(*p2);
			}

		}

		void __stdcall fix_rotate(char** fileName,BYTE bone,float* theFloatPoint) {
			if (*fileName== nullptr) {
				return;
			}
		
			std::string vFileName = *fileName;
			if ( 
				(
					vFileName.find("china_8yx_25_idle.ANI")==0||
					vFileName.find("china_8yx_25_jump.ANI") == 0||
					vFileName.find("china_8yx_25_run.ANI") == 0||
					vFileName.find("china_8yx_25_walk.ANI") == 0 ||
					vFileName.find("china_8yx_25_br.ANI") == 0 
				)

				&&bone==0x18) {
				*theFloatPoint = rotate_float(*theFloatPoint);
			
			}	
		}

		void __stdcall fix_height(DWORD mountObj, BYTE modelId)
		{
			auto model = g_models[modelId];
			if (!model.enabled)
			{
				return;
			}

			if (model.height < 0.1)
			{
				return;
			}

			*(PFLOAT(mountObj + 0xf8)) = model.height;
		}

		void rotate_num(float* val) {
			static int method = 1;
			if (method == 1) {
				if (*val < 0) {
					auto absValue = std::abs(*val);
					*val = absValue;
				}
			}
			else {
				static float valToAdjust = 1.0;
				auto val1 = 0.0 + *val;
				*val = valToAdjust + val1;
			}
		}

		void __stdcall is_rotate_player(BYTE modelId,DWORD p)
		{
			static bool is_debug = true;
			if (g_models[modelId].enabled && g_models[modelId].need_rotate) {
		//		static float valToAdjust = 1.5;
		//		auto absValue = std::abs(*rotateVal);
		//		auto remain = 1.0 - absValue;
		//		*rotateVal = valToAdjust +remain;
				if (is_debug) {
					rotate_num(reinterpret_cast<float*>(p + 0x28));
					rotate_num(reinterpret_cast<float*>(p + 0x20));
			//		*reinterpret_cast<float*>(p + 0x28) = std::abs(*reinterpret_cast<float*>(p + 0x28));
			//		*reinterpret_cast<float*>(p + 0x20) = std::abs(*reinterpret_cast<float*>(p + 0x20));
			//		*rotateVal = valToAdjust;
				//	WCHAR str[30]{};
				//	StringCchPrintf(str, 30, L"val: %f\n", *rotateVal);
				//	OutputDebugStringW(str);
				}
			}
		}

		DWORD __stdcall get_bone_id_2(BYTE modelId)
		{
			auto model = g_models[modelId];
			if (!model.enabled)
			{
				return 0;
			}
			if (modelId == 33) {
				return 44;
			}
			return 0;
		}


		DWORD __stdcall get_bone_id(BYTE modelId)
		{
			auto model = g_models[modelId];
			if (!model.enabled)
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
				push g_objBoneId1.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
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

		__declspec(naked) void Naked_fixBone9()
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
				push g_objBoneId9.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId9.m_pRet
			}
		}


		__declspec(naked) void Naked_fixBone10()
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
				push g_objBoneId10.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId10.m_pRet
			}
		}


		__declspec(naked) void Naked_fixBone11()
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
				MYASMCALL_1(get_bone_id_2, eax)

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
				push g_objBoneId11.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId11.m_pRet
			}
		}





		__declspec(naked) void Naked_fixBone12()
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
				MYASMCALL_1(get_bone_id_2, eax)

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
				push g_objBoneId12.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId12.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone13()
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
				jmp g_boneDouble1Addr
				_Org :
				jmp g_objBoneId13.m_pRet
			}
		}


		__declspec(naked) void Naked_fixBone14()
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
				jmp g_boneDouble1Addr
				_Org :
				jmp g_objBoneId14.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone15()
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
				jmp g_boneDouble1Addr
				_Org :
				jmp g_objBoneId15.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone16()
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
				jmp g_boneDouble1Addr
				_Org :
				jmp g_objBoneId16.m_pRet
			}
		}



		__declspec(naked) void Naked_fixBone17()
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
				push g_objBoneId17.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId17.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone18()
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
				push g_objBoneId18.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId18.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone19()
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
				push g_objBoneId19.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId19.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone20()
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
				push g_objBoneId20.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId20.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone21()
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
				push g_objBoneId21.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId21.m_pRet
			}
		}



		__declspec(naked) void Naked_fixBone22()
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
				push g_objBoneId22.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId22.m_pRet
			}
		}


		__declspec(naked) void Naked_fixBone23()
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
				push g_objBoneId23.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId24.m_pRet
			}
		}


		__declspec(naked) void Naked_fixBone24()
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
				push g_objBoneId24.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId24.m_pRet
			}
		}


		__declspec(naked) void Naked_fixBone25()
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
				push g_objBoneId25.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId25.m_pRet
			}
		}


		__declspec(naked) void Naked_fixBone26()
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
				push g_objBoneId26.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId26.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone27()
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
				push g_objBoneId27.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId27.m_pRet
			}
		}


		__declspec(naked) void Naked_fixBone28()
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
				push g_objBoneId28.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId28.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone29()
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
				push g_objBoneId29.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId29.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone30()
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
				push g_objBoneId30.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId30.m_pRet
			}
		}


		__declspec(naked) void Naked_fixBone31()
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
				push g_objBoneId31.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId31.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone32()
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
				push g_objBoneId32.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId32.m_pRet
			}
		}


		__declspec(naked) void Naked_fixBone33()
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
				push g_objBoneId33.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId33.m_pRet
			}
		}



		__declspec(naked) void Naked_fixBone34()
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
				push g_objBoneId34.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId34.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone35()
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
				push g_objBoneId35.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId35.m_pRet
			}
		}


		__declspec(naked) void Naked_fixBone36()
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
				push g_objBoneId36.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId36.m_pRet
			}
		}


		__declspec(naked) void Naked_fixBone37()
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
				push g_objBoneId37.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId37.m_pRet
			}
		}



		__declspec(naked) void Naked_fixBone38()
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
				push g_objBoneId38.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId38.m_pRet
			}
		}


		__declspec(naked) void Naked_fixBone39()
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
				push g_objBoneId39.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId39.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone40()
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
				push g_objBoneId40.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId40.m_pRet
			}
		}



		__declspec(naked) void Naked_fixBone41()
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
				push g_objBoneId41.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId42.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone42()
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
				push g_objBoneId42.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId22.m_pRet
			}
		}



		__declspec(naked) void Naked_fixBone43()
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
				push g_objBoneId43.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId43.m_pRet
			}
		}

		__declspec(naked) void Naked_fixBone44()
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
				push g_objBoneId44.m_pRet
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId44.m_pRet
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
				add dword ptr[esp], 0x2
				ret
				_Org :
				jmp g_objBoneId4.m_pRet
			}
		}




		__declspec(naked) void Naked_rotate()
		{
			_asm
			{
				pushad
				movzx ebx, byte ptr[ebx + 0x385]
				MYASMCALL_2(is_rotate_player, ebx,eax)
				popad
				push eax
					lea ecx, dword ptr[esp + 0x54]
					jmp g_objRotate1.m_pRet
			}
		}



		__declspec(naked) void Naked_rotate_double_1()
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
					lea ecx, ss : [esp + 0x128]
					jmp g_objRotateDouble1.m_pRet
			}
		}

		__declspec(naked) void Naked_rotate_double_2()
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
					lea eax, ss : [esp + 0xE4]
					jmp g_objRotateDouble2.m_pRet
			}
		}

		DWORD dwCallFicRotate = 0x005E1D18;
		__declspec(naked) void Naked_fixRotate()
		{
			_asm
			{
				call dwCallFicRotate
				pushad
				mov eax,dword ptr [esp+0x20] //fileName
				mov ebx,dword ptr [esp+0x34] // bone
				add esi,0x20
				MYASMCALL_3(fix_rotate, eax, ebx,esi)
				popad
				jmp g_objFixRotate_obj.m_pRet
			}
		}

		
			__declspec(naked) void Naked_markRotate_2()
		{
			_asm
			{
				pushad
				MYASMCALL_2(markRotate, esi, ecx)
				popad
				jmp g_objFixRotate_obj_3.m_pRet
			}
		}

		__declspec(naked) void Naked_markRotate()
		{
			_asm
			{
				pushad
				MYASMCALL_2(markRotate,ebp ,ecx)
				popad
				jmp g_objFixRotate_obj_1.m_pRet
			}
		}
		__declspec(naked) void Naked_doRotate()
		{
			_asm
			{
				pushad
				mov eax,dword ptr [esp+0x38] //bone id
				MYASMCALL_2(doRotate, edi, eax)
				popad
				mov eax, dword ptr ds : [ebx + 0x100]
				jmp g_objFixRotate_obj_2.m_pRet
			}
		}



		DWORD dwCallSwitchAni = 0x0054FFB0;
		__declspec(naked) void Naked_skipSwitchAni()
		{
			_asm
			{
				pushad
				MYASMCALL_1(skipSwitchAni,ecx)
				test al,al
				popad
				je _org
				add esp ,0xc
				jmp g_objSkipSwitchAni.m_pRet

				_org:
				call dwCallSwitchAni
				jmp g_objSkipSwitchAni.m_pRet
			}
		}

		

			__declspec(naked) void Naked_skipSwitchAni_2()
		{
			_asm
			{
				pushad
				MYASMCALL_1(skipSwitchAni, ecx)
				test al, al
				popad
				je _org
				add esp, 0xc
				jmp g_objSkipSwitchAni_2.m_pRet

				_org :
				call dwCallSwitchAni
					jmp g_objSkipSwitchAni_2.m_pRet
			}
		}


		
		



		DWORD dwCallOrgFixHeightCall = 0x00552EC0;
		__declspec(naked) void Naked_fixHeight()
		{
			_asm
			{
				pushad
				mov eax, dword ptr[esi + 0x74]
				MYASMCALL_2(fix_height, ecx, eax)
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

			ShaiyaUtility::Packet::EncodeNewHiddenMount* p1 = (struct ShaiyaUtility::Packet::EncodeNewHiddenMount*)packet;
			p1->data_size = p1->data_size - 55;




			const BYTE uid = useruid().value() % 250;

			const auto tmp = std::make_unique<BYTE[]>(p1->data_size);
			for (int i = 0;i < p1->data_size;i++)
			{
				tmp.get()[i] = p1->encoded_data[i] ^ uid;
			}
			auto decode = Utility::Crypt::base64_decode(reinterpret_cast<const char*>(tmp.get()));
			auto p = reinterpret_cast<ShaiyaUtility::Packet::NewHiddenMount*>(decode.data());

			g_models.fill({});

			for (int i = 0;i < 100;i++)
			{
				const BYTE id = p->models[i].id;
				if (id == 0)
				{
					continue;
				}
				g_models[id] = { true,
					p->models[i].need_rotate,
					p->models[i].height,
					p->models[i].boneId };
			}
			static bool flag = false;
			if (flag)
			{
				return;
			}
			flag = true;
			BYTE limitMountModel = 0x57;
			ShaiyaUtility::WriteCurrentProcessMemory((void*)0x004C81F5, &limitMountModel, 1);
	//		g_objRotate1.Hook(reinterpret_cast<void*>(p->Naked_fixCharacterRotate1), Naked_rotate, 5);

			/*
			g_objRotateDouble1.Hook(reinterpret_cast<void*>(0x004121F3), Naked_rotate_double_1, 8);
			g_objRotateDouble2.Hook(reinterpret_cast<void*>(0x00412280), Naked_rotate_double_2, 8);
			*/


			g_objBoneId1.Hook(reinterpret_cast<void*>(p->Naked_fixboneId1), Naked_fixBone1, 6);
			g_objBoneId2.Hook(reinterpret_cast<void*>(p->Naked_fixboneId2), Naked_fixBone2, 6);
			g_objBoneId3.Hook(reinterpret_cast<void*>(p->Naked_fixboneId3), Naked_fixBone3, 6);
			g_objBoneId5.Hook(reinterpret_cast<void*>(p->Naked_fixboneId5), Naked_fixBone5, 6);
			g_objBoneId7.Hook(reinterpret_cast<void*>(p->Naked_fixboneId7), Naked_fixBone7, 6);


			// these 4 hooks are for double
			/*
			g_objBoneId9.Hook(reinterpret_cast<void*>(0x004121C3), Naked_fixBone9, 6);
			g_objBoneId10.Hook(reinterpret_cast<void*>(0x004121E6), Naked_fixBone10, 6);
			g_objBoneId11.Hook(reinterpret_cast<void*>(0x0041224C), Naked_fixBone11, 6);//for second people
			g_objBoneId12.Hook(reinterpret_cast<void*>(0x00412273), Naked_fixBone12, 6);// for second people


			g_objBoneId13.Hook(reinterpret_cast<void*>(0x004122D1), Naked_fixBone13, 6);
			g_objBoneId13.Hook(reinterpret_cast<void*>(0x004123DF), Naked_fixBone14, 6);
			g_objBoneId13.Hook(reinterpret_cast<void*>(0x004124ED), Naked_fixBone15, 6);
			*/

			/*

			g_objBoneId13.Hook(reinterpret_cast<void*>(0x004122D1), Naked_fixBone13, 6);
			g_objBoneId14.Hook(reinterpret_cast<void*>(0x004122F4), Naked_fixBone14, 6);



			g_objBoneId15.Hook(reinterpret_cast<void*>(0x004123DF), Naked_fixBone15, 6);
			g_objBoneId16.Hook(reinterpret_cast<void*>(0x00412402), Naked_fixBone16, 6);


			g_objBoneId17.Hook(reinterpret_cast<void*>(0x004124ED), Naked_fixBone17, 6);
			g_objBoneId18.Hook(reinterpret_cast<void*>(0x00412510), Naked_fixBone18, 6);

			g_objBoneId19.Hook(reinterpret_cast<void*>(0x0041260E), Naked_fixBone19, 6);
			g_objBoneId20.Hook(reinterpret_cast<void*>(0x0041262E), Naked_fixBone20, 6);

			g_objBoneId21.Hook(reinterpret_cast<void*>(0x00412704), Naked_fixBone21, 6);
			g_objBoneId22.Hook(reinterpret_cast<void*>(0x00412724), Naked_fixBone22, 6);

			g_objBoneId23.Hook(reinterpret_cast<void*>(0x004127E0), Naked_fixBone23, 6);
			g_objBoneId24.Hook(reinterpret_cast<void*>(0x00412800), Naked_fixBone24, 6);


			g_objBoneId25.Hook(reinterpret_cast<void*>(0x004128D6), Naked_fixBone25, 6);
			g_objBoneId26.Hook(reinterpret_cast<void*>(0x004128F6), Naked_fixBone26, 6);


			g_objBoneId27.Hook(reinterpret_cast<void*>(0x00412AC7), Naked_fixBone27, 6);
			g_objBoneId28.Hook(reinterpret_cast<void*>(0x00412AE7), Naked_fixBone28, 6);

			g_objBoneId29.Hook(reinterpret_cast<void*>(0x00412BBD), Naked_fixBone29, 6);
			g_objBoneId30.Hook(reinterpret_cast<void*>(0x00412BDD), Naked_fixBone30, 6);


			g_objBoneId31.Hook(reinterpret_cast<void*>(0x00412CB3), Naked_fixBone31, 6);
			g_objBoneId32.Hook(reinterpret_cast<void*>(0x00412CD3), Naked_fixBone32, 6);

			g_objBoneId33.Hook(reinterpret_cast<void*>(0x00412D9F), Naked_fixBone33, 6);
			g_objBoneId34.Hook(reinterpret_cast<void*>(0x00412DBF), Naked_fixBone34, 6);


			g_objBoneId35.Hook(reinterpret_cast<void*>(0x00412E95), Naked_fixBone35, 6);
			g_objBoneId36.Hook(reinterpret_cast<void*>(0x00412EB5), Naked_fixBone36, 6);

			g_objBoneId37.Hook(reinterpret_cast<void*>(0x00412F8B), Naked_fixBone37, 6);
			g_objBoneId38.Hook(reinterpret_cast<void*>(0x00412FAB), Naked_fixBone38, 6);

			g_objBoneId39.Hook(reinterpret_cast<void*>(0x00413067), Naked_fixBone39, 6);
			g_objBoneId40.Hook(reinterpret_cast<void*>(0x00413087), Naked_fixBone40, 6);

			g_objBoneId41.Hook(reinterpret_cast<void*>(0x004131B7), Naked_fixBone41, 6);
			g_objBoneId42.Hook(reinterpret_cast<void*>(0x004131DB), Naked_fixBone42, 6);

			g_objBoneId43.Hook(reinterpret_cast<void*>(0x00413230), Naked_fixBone43, 6);
			g_objBoneId44.Hook(reinterpret_cast<void*>(0x00413266), Naked_fixBone44, 6);
			*/

			// 00412F8B 00412FAB 00413067 00413087 004131B7 004131DB 00413230 00413266


	//		g_objFixHeight.Hook(reinterpret_cast<void*>(p->Naked_fixHeight), Naked_fixHeight, 5);

			// 这个不大好用, 会变形也会有ani切换时的假动作
	//		g_objFixRotate_obj.Hook(reinterpret_cast<void*>(0x00550664), Naked_fixRotate, 5);

			g_aniBones.insert({ "china_8yx_25_" ,0x18 });
			
			g_objFixRotate_obj_1.Hook(reinterpret_cast<void*>(0x00431d39), Naked_markRotate, 8);
			g_objFixRotate_obj_2.Hook(reinterpret_cast<void*>(0x0054FF2D), Naked_doRotate, 6);
			g_objFixRotate_obj_3.Hook(reinterpret_cast<void*>(0x00431307), Naked_markRotate_2, 8);

			// 005502FF       | 8B4C24 64                        | mov ecx,dword ptr ss:[esp+64]                                                 |
			// 00550383       | C78424 24010000 00000000         | mov dword ptr ss:[esp+124],0                                                  |
			g_objFixRotate_obj_4.Hook(reinterpret_cast<void*>(0x0054FF2D), Naked_doRotate, 6);
			g_objFixRotate_obj_5.Hook(reinterpret_cast<void*>(0x0054FF2D), Naked_doRotate, 6);
			g_objFixRotate_obj_6.Hook(reinterpret_cast<void*>(0x0054FF2D), Naked_doRotate, 6);

	//		g_objSkipSwitchAni.Hook(0x00431d2f, Naked_skipSwitchAni, 5);
	//		g_objSkipSwitchAni_2.Hook(0x004312fd, Naked_skipSwitchAni_2, 5);
		

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

		ShaiyaUtility::CMyInlineHook g_obj, g_obj1;

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

		__declspec(naked) void Naked_1()
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
				jmp g_obj1.m_pRet
			}
		}




		void Start() {
			g_obj.Hook(reinterpret_cast<void*>(0x004A45E4), Naked, 5);
			g_obj1.Hook(reinterpret_cast<void*>(0x00501edd), Naked_1, 5);
		}
	}

	namespace title {
		ShaiyaUtility::CMyInlineHook g_hook_no_guide_name;
		ShaiyaUtility::CMyInlineHook g_hook_guide_name;
		ShaiyaUtility::CMyInlineHook g_hook_create_text_object;

		struct PlayerTitleAndObject {
			ShaiyaUtility::Packet::PlayerTitle title;
			void* textObject{};
		};

		PlayerTitleAndObject g_titles[100];

		void* create_text_object(const char* text) {
			DWORD dwCall = 0x00547B90;
			void* result = 0;
			_asm {
				push text
				mov ecx, dword ptr ds : [0x02206474]
				call dwCall
				mov result, eax
			}
			return result;
		}

		void draw_text(void* textObject, DWORD x, DWORD y, DWORD color, float opaque) {

			DWORD dwCall = 0x00547910;
			_asm {
				push color
				push opaque
				push y
				push x
				push textObject
				call dwCall
			}
		}



		void draw_title(DWORD player, DWORD y, float x, float opaque) {
			DWORD charid = *PDWORD(player + 0x34);
			for (auto& item : g_titles) {
				if (item.title.charId == charid) {

					if (item.textObject == nullptr) {
						item.textObject = create_text_object(item.title.text);
					}

					draw_text(
						item.textObject, int(x) - strlen(item.title.text) * 3,
						y,
						item.title.Color | 0xff000000,
						opaque);

					break;
				}
			}
		}


		void __stdcall on_guild_name_color(DWORD player, DWORD y, float x, float opaque) {
			draw_title(player, y - 15, x, opaque);
		}

		void __stdcall on_normal_name_color(DWORD player, DWORD y, float x, float opaque) {
			DWORD guiNameObj = *PDWORD(player + 0x2e0);
			if (guiNameObj != 0) {
				return;
			}
			draw_title(player, y - 18, x, opaque);
		}

		DWORD dwCallDrawNameColor = 0x00547910;
		__declspec(naked) void Naked_normal_name_color()
		{
			_asm
			{
				pushad
				mov eax, dword ptr[esp + 0x9c] // x without adjust,it might be the center
				mov ebx, dword ptr[esp + 0x28] // y
				mov edx, dword ptr[esp + 0x2c] // opaque
				MYASMCALL_4(on_normal_name_color, esi, ebx, eax, edx)
				popad
				call dwCallDrawNameColor
				jmp g_hook_no_guide_name.m_pRet
			}
		}

		__declspec(naked) void Naked_guild_name_color()
		{
			_asm
			{
				pushad
				mov eax, dword ptr[esp + 0x9c] // x without adjust,it might be the center
				mov ebx, dword ptr[esp + 0x28] // y
				mov edx, dword ptr[esp + 0x2c] // opaque
				MYASMCALL_4(on_guild_name_color, esi, ebx, eax, edx)
				popad
				call dwCallDrawNameColor
				jmp g_hook_guide_name.m_pRet
			}
		}


		/*
		__declspec(naked) void Naked_create_text_object()
		{
			_asm
			{
				pushad
				MYASMCALL_1(on_create_text_object, esi)
				popad
				mov dword ptr ds : [esi + 0x2D8] , eax
				jmp g_hook_create_text_object.m_pRet
			}
		}
		*/


		void process_packet(void* p) {
#ifndef _DEBUG
			VMProtectBegin("process_packet_player_title");
#endif
			ShaiyaUtility::Packet::TitleInfo* titles = reinterpret_cast<ShaiyaUtility::Packet::TitleInfo*>(p);
			if (titles->isFirstPart) {
				for (int i = 0;i < 50;i++) {
					if (strcmp(g_titles[i].title.text, titles->titles[i].text) != 0) {
						g_titles[i].textObject = nullptr;
					}
					g_titles[i].title = titles->titles[i];
				}
			}
			else {
				for (int i = 50;i < 100;i++) {
					if (strcmp(g_titles[i].title.text, titles->titles[i - 50].text) != 0) {
						g_titles[i].textObject = nullptr;
					}
					g_titles[i].title = titles->titles[i - 50];
				}
			}
#ifndef _DEBUG
			VMProtectEnd();
#endif
			static bool is_init = false;
			if (!is_init) {
				is_init = true;
				auto uid = useruid();
				titles->addr1 ^= uid.value();
				titles->addr2 ^= uid.value();
				g_hook_no_guide_name.Hook(titles->addr1, Naked_normal_name_color, 5);
				g_hook_guide_name.Hook(titles->addr2, Naked_guild_name_color, 5);
				//		g_hook_create_text_object.Hook(0x0044b7ce, Naked_create_text_object, 6);
			}

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
			case ShaiyaUtility::Packet::playerTitleCode:
			{
				title::process_packet(P);
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

	namespace crashFix
	{
		ShaiyaUtility::CMyInlineHook g_obj1;

		DWORD dwCall1 = 0x0052c1e3;
		__declspec(naked) void Naked_1()
		{
			_asm
			{
				cmp eax, 0
				jne __org

				jmp dwCall1

				__org :
				xor ecx, ecx
					add eax, 0x78
					jmp g_obj1.m_pRet
			}
		}

		void start()
		{


			g_obj1.Hook(reinterpret_cast<void*>(0x0052c1c4), Naked_1, 5);
		}
	}




	void Start()
	{
		//		VerifyParentProcess();

		custompacket::Start();
		ijl15Detection::Start();
		//		antiHacker::Start();
		ReadCharDecryption::Start();
		getEnhanceAttack::Start();
		multipleClient::start();
		NewMountFeature::start();
		skillEffect::start();
		crashFix::start();

		// just for testing
		/*
		ShaiyaUtility::Packet::TitleInfo titleInfo;
		titleInfo.isFirstPart = true;
		titleInfo.titles[0].charId = 16;
		titleInfo.titles[0].Color = 0xffffff;
		strcpy(titleInfo.titles[0].text, "this is title");
		title::process_packet(&titleInfo);
		*/
	}

		}

