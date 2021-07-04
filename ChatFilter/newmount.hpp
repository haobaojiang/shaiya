#pragma once
#ifndef SHAIYA_NEW_MOUNT_HEADER
#define SHAIYA_NEW_MOUNT_HEADER

#include "stdafx.h"
#include "config.hpp"
#include "VMProtectSDK.h"

namespace NewMount
{


	ShaiyaUtility::Packet::NewHiddenMount g_packet;
	ShaiyaUtility::CMyInlineHook g_obj;

#pragma pack(push,1)
	struct MountPacket
	{
		WORD cmd{};
		DWORD unk{};
		BYTE model_id{};
	};
#pragma pack(pop)

	std::map<DWORD, BYTE> g_models;


	void __stdcall fix_new_mount_model(void* player, DWORD invItem)
	{
#ifndef _DEBUG
		VMProtectBegin(__FUNCDNAME__);
#endif
		const auto itemId = ShaiyaUtility::EP6::PlayerItemToItemId(invItem);
		if (itemId == 0)
		{
			return;
		}
		const auto model = g_models.find(itemId);
		if (model == g_models.end())
		{
			return;
		}

		*PDWORD(DWORD(player) + 0x01484) = 0xe;
		*PDWORD(DWORD(player) + 0x01488) = static_cast<DWORD>(model->second);
#ifndef _DEBUG
		VMProtectEnd();
#endif
	}

	__declspec(naked) void  Naked()
	{
		_asm {
			pushad
			MYASMCALL_2(fix_new_mount_model, edi, eax)
			popad
			mov eax, dword ptr ds : [eax + 0x30]
			movzx ecx, byte ptr ds : [eax + 0x32]
			jmp g_obj.m_pRet
		}
	}

	void start()
	{
#ifndef _DEBUG
		VMProtectBegin(__FUNCDNAME__);
#endif
		auto section = "new_mount";
		if (!GameConfiguration::GetBoolean(section, "enable", false)) {
			return;
		}
		

		for (int i = 0;i < 100;i++) {
			std::string v = GameConfiguration::Get(section, std::to_string(i), "");
			if (v.empty())
			{
				continue;
			}
			DWORD itemId = 0;
			DWORD modelId = 0;
			sscanf_s(v.c_str(), "%d,%d", &itemId, &modelId);
			LOGD << "itemId:" << itemId << " modelId:" << modelId;


			g_packet.modelIds[i] = static_cast<BYTE>(modelId);
			g_models.insert({ itemId, static_cast<BYTE>(modelId) });
		}

		
		GameEventCallBack::AddLoginCallBack([&](void* player)
			{
			    
				ShaiyaUtility::EP6::SendToPlayer(player,&g_packet,sizeof(g_packet) );
			});

		
		g_obj.Hook((void*)0x0049dbcf, Naked, 7);
#ifndef _DEBUG
		VMProtectEnd();
#endif
	}

}


#endif