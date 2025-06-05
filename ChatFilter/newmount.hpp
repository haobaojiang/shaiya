#pragma once
#ifndef SHAIYA_NEW_MOUNT_HEADER
#define SHAIYA_NEW_MOUNT_HEADER

#include "stdafx.h"
#include "config.hpp"
//#include "VMProtectSDK.h"

namespace NewMount
{


	std::string g_encodeData;
	ShaiyaUtility::Packet::NewHiddenMount g_packet;
	ShaiyaUtility::CMyInlineHook g_obj;

	std::array<BYTE,100> g_models;


	void __stdcall fix_new_mount_model(void* player, DWORD invItem)
	{
		/*
#ifndef _DEBUG
		VMProtectBegin(__FUNCDNAME__);
#endif
		*/
		const auto itemId = ShaiyaUtility::EP6::PlayerItemToItemId(invItem);
		if (itemId == 0)
		{
			return;
		}

		auto typeId = itemId % 1000;
		
		const auto modelId = g_models[typeId];
		if (modelId == 0)
		{
			return;
		}

		*PDWORD(DWORD(player) + 0x01484) = 0xe;
		*PDWORD(DWORD(player) + 0x01488) = static_cast<DWORD>(modelId);
		/*
#ifndef _DEBUG
		VMProtectEnd();
#endif
		*/
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


	void read_config(void* p)
	{
		while (1)
		{
			ShaiyaUtility::Packet::NewHiddenMount tempPacket;
			std::array<BYTE, 100> tempModels;
			tempModels.fill(0);

		
			for (int i = 0;i < 100;i++) {
				auto section = "new_mount";
				std::string v = GameConfiguration::Get(section, std::to_string(i), "");
				if (v.empty())
				{
					continue;
				}
				DWORD itemId = 0;
				DWORD modelId = 0;
				float height = 0;
				DWORD need_rotate = 0;
				DWORD boneId = 0;
				sscanf_s(v.c_str(), "%d,%d,%d,%f,%d", &itemId, &modelId, &boneId, &height, &need_rotate);

				ShaiyaUtility::Packet::MontModel model;
				model.height = height;
				model.id = static_cast<BYTE>(modelId);
				model.need_rotate = static_cast<bool>(need_rotate);
				model.boneId = static_cast<BYTE>(boneId - 1);

				tempPacket.models[i] = model;
				tempModels[static_cast<BYTE>(itemId %  1000)] = static_cast<BYTE>(modelId);
			}

			if(tempModels!=g_models)
			{
				g_models = tempModels;
			}
			
			if(memcmp(tempPacket.models,g_packet.models,sizeof(g_packet.models))!=0)
			{
				LOGD << "conf changed:";
				for (int i = 0;i < 100;i++) {
					auto model = tempPacket.models[i];
					if(model.id==0)
					{
						continue;
					}
					LOGD << " modelId:" << model.id << " need_rotate:" << model.need_rotate << " height:" << model.height << " bone:" << model.boneId;
				}
				g_packet = tempPacket;
				
				ShaiyaUtility::EP6::SendPacketAll(&g_packet, sizeof(g_packet));
			}
			
			Sleep(2000);
		}
	}
	

	void start()
	{
		/*
#ifndef _DEBUG
		VMProtectBegin(__FUNCTION__);
#endif
		*/
		auto section = "new_mount";
		if (!GameConfiguration::GetBoolean(section, "enable", false)) {
			return;
		}
		
	    g_models.fill(0);
		for (int i = 0;i < 100;i++) {
			
			std::string v = GameConfiguration::Get(section, std::to_string(i), "");
			if (v.empty())
			{
				continue;
			}
			DWORD itemId = 0;
			DWORD modelId = 0;
			float height = 0;
			DWORD need_rotate = 0;
			DWORD boneId = 0;
			sscanf_s(v.c_str(), "%d,%d,%d,%f,%d", &itemId, &modelId,&boneId,&height,&need_rotate);
			LOGD << "itemId:" << itemId << " modelId:" << modelId <<  " boneId:" << boneId << " need_rotate:" << need_rotate << " height:" << height;

			ShaiyaUtility::Packet::MontModel model;
			model.height = height;
			model.id = static_cast<BYTE>(modelId);
			model.need_rotate = static_cast<bool>(need_rotate);
			model.boneId = static_cast<BYTE>(boneId-1);
			
			g_packet.models[i] = model;
			g_models[static_cast<BYTE>(itemId%1000)] = static_cast<BYTE>(modelId);
		}

	
		if (GameConfiguration::GetBoolean(section, "enable_debug", false)) {
			_beginthread(read_config, 0, 0);
		}
		
		g_encodeData =  Utility::Crypt::base64_encode(reinterpret_cast<const unsigned char*>(&g_packet),sizeof(g_packet));
		GameEventCallBack::AddLoginCallBack([&](void* player)
			{
	
				auto uid = ShaiyaUtility::EP6::PlayerUid(player);
				uid = uid % 250;
				auto p = std::make_unique<ShaiyaUtility::Packet::EncodeNewHiddenMount>();
			
				p->data_size = static_cast<WORD>(g_encodeData.size()+55);
	
			    for(size_t i=0;i< g_encodeData.size();i++)
			    {
					p->encoded_data[i] = static_cast<BYTE>(g_encodeData[i] ^ uid);
			    }
		
			   

				
							ShaiyaUtility::EP6::SendToPlayer(player,p.get(),sizeof(ShaiyaUtility::Packet::EncodeNewHiddenMount) );
				
			});
		
	
		
		g_obj.Hook((void*)0x0049dbcf, Naked, 7);
		/*
#ifndef _DEBUG
		VMProtectEnd();
#endif
		*/
	}

}


#endif