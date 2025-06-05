
#include <windows.h>
#include <process.h>
#include "../utility/utility.h"

namespace Shaiya11
{
	std::optional<DWORD> useruid() {
		auto obj = ShaiyaUtility::read<DWORD>(0x022F1598);
		if (obj == 0) {
			return std::nullopt;
		}
		auto uid = ShaiyaUtility::read<DWORD>(obj + 0x230);
		if (uid == 0) {
			return std::nullopt;
		}
		return std::optional<DWORD>(uid);
	}


	
  namespace HideCostume{

      ShaiyaUtility::CMyInlineHook g_hideCostume, g_hideCostume1;
      ShaiyaUtility::CMyInlineHook g_set_enabled;
      bool g_enabled{};

      bool __stdcall set_enabled(char* packet)
      {
          auto str = &packet[3];
      	  if(strcmp(str,"开时装")==0)
      	  {
              g_enabled = false;
              return true;
      	  }else if(strcmp(str,"关时装")==0)
      	  {
              g_enabled = true;
              return true;
      	  }
          return false;
      }
  	

      bool __stdcall is_hide_costume( )
      {
          return g_enabled;
      }



      DWORD dwCall = 0x005CD790;
      __declspec(naked) void Naked_set_enabled()
      {
          _asm {
              pushad
              MYASMCALL_1(set_enabled, ecx)
              cmp al, 0x0
              popad
              je _Org
             jmp g_set_enabled.m_pRet

              _Org:
          	call dwCall
              jmp g_set_enabled.m_pRet
          }
      }

   

      DWORD dwCall1 = 0x005C2970;
      __declspec(naked) void Naked_hide_costume()
      {
      	_asm{
            pushad
            MYASMCALL(is_hide_costume)
      		cmp al,0x0
      		popad
      		je _Org
            mov byte ptr[edx + 0x42], 0x0
            mov byte ptr[edx + 0x43], 0x0
      		
      		_Org:
            call dwCall1
            jmp g_hideCostume.m_pRet
      	}
      }

      DWORD dwCall2 = 0x005C40D0;
      __declspec(naked) void Naked_hide_costume_2()
      {
          _asm {
              pushad
              MYASMCALL(is_hide_costume)
              cmp al, 0x0
              popad
              je _Org
              jmp g_hideCostume1.m_pRet

          	
              _Org:
              call dwCall2
                  jmp g_hideCostume1.m_pRet
          }
      }

  	
  	void start()
  	{
        g_hideCostume.Hook(0x005d4d3d, Naked_hide_costume, 5);
        g_set_enabled.Hook(0x005cff8c, Naked_set_enabled, 5);
        g_hideCostume1.Hook(0x005d4e9e, Naked_hide_costume_2, 5);
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

	  ShaiyaUtility::CMyInlineHook g_objRotate1;
	  ShaiyaUtility::CMyInlineHook g_objBoneId1; // human
	  ShaiyaUtility::CMyInlineHook g_objBoneId2;
	  ShaiyaUtility::CMyInlineHook g_objBoneId3;// 
	  ShaiyaUtility::CMyInlineHook g_objBoneId5;// 
	  ShaiyaUtility::CMyInlineHook g_objBoneId7;// 
	  ShaiyaUtility::CMyInlineHook g_objFixHeight;


	  DWORD g_bone1Addr = 0x00412E87;

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


	  bool __stdcall is_rotate_player(BYTE modelId)
	  {
		  return  g_models[modelId].enabled && g_models[modelId].need_rotate;
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
			  mov ecx, dword ptr ds : [ebx + 0x2CC]
			  push ebx
			  push ecx
			  push edx
			  push esi
			  push edi
			  push esp
			  push ebp


			  movzx eax, byte ptr[ebx + 0x3bd]
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
			  mov ecx, dword ptr ds : [ebx + 0x2CC]
			  push ebx
			  push ecx
			  push edx
			  push esi
			  push edi
			  push esp
			  push ebp


			  movzx eax, byte ptr[ebx + 0x3bd]
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



	  __declspec(naked) void Naked_fixBone2()
	  {
		  _asm
		  {
			  mov ecx, dword ptr ds : [ebx + 0x2CC]
			  push ebx
			  push ecx
			  push edx
			  push esi
			  push edi
			  push esp
			  push ebp


			  movzx eax, byte ptr[ebx + 0x3bd]
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
			  mov ecx, dword ptr ds : [ebx + 0x2CC]
			  push ebx
			  push ecx
			  push edx
			  push esi
			  push edi
			  push esp
			  push ebp


			  movzx eax, byte ptr[ebx + 0x3bd]
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
			  mov ecx, dword ptr ds : [ebx + 0x2CC]
			  push ebx
			  push ecx
			  push edx
			  push esi
			  push edi
			  push esp
			  push ebp


			  movzx eax, byte ptr[ebx + 0x3bd]
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



	  __declspec(naked) void Naked_rotate()
	  {
		  _asm
		  {
			  pushad
			  movzx eax, byte ptr[ebx + 0x3bd]
			  MYASMCALL_1(is_rotate_player, eax)
			  test al, al
			  popad
			  je __Org
			  mov dword ptr[eax + 0x28], 0x3f800000 // float 1

			  __Org:
			  push eax
				  lea ecx, dword ptr[esp + 0x18]
				  jmp g_objRotate1.m_pRet
		  }
	  }

	  DWORD dwCallOrgFixHeightCall = 0x0056D030;
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
		  BYTE limitMountModel = 0x65;
		  ShaiyaUtility::WriteCurrentProcessMemory((void*)0x004D5445, &limitMountModel, 1);
		  g_objRotate1.Hook(reinterpret_cast<void*>(p->Shaiya11_Naked_fixCharacterRotate1), Naked_rotate, 5);
		  g_objBoneId1.Hook(reinterpret_cast<void*>(p->Shaiya11_Naked_fixboneId1), Naked_fixBone1, 6);
		  g_objBoneId2.Hook(reinterpret_cast<void*>(p->Shaiya11_Naked_fixboneId2), Naked_fixBone2, 6);

		  g_objBoneId3.Hook(reinterpret_cast<void*>(p->Shaiya11_Naked_fixboneId3), Naked_fixBone3, 6);
		  g_objBoneId5.Hook(reinterpret_cast<void*>(p->Shaiya11_Naked_fixboneId5), Naked_fixBone5, 6);
		  g_objBoneId7.Hook(reinterpret_cast<void*>(p->Shaiya11_Naked_fixboneId7), Naked_fixBone7, 6);
		  g_objFixHeight.Hook(reinterpret_cast<void*>(p->Shaiya11_Naked_fixHeight), Naked_fixHeight, 5);



#ifndef _DEBUG
		  VMProtectEnd();
#endif
	  }
  }

  namespace custompacket
  {
	  ShaiyaUtility::CMyInlineHook objPacket;
	  static DWORD codeBoundary = ShaiyaUtility::Packet::Code::begin;

	  void __stdcall CustomPacket(PVOID P) {

		  auto cmd = static_cast<ShaiyaUtility::Packet::Header*>(P);
		  switch (cmd->command) {

		  case ShaiyaUtility::Packet::hiddenMountCode:
		  {
			  NewMountFeature::process_server_message(P);
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
		  objPacket.Hook(reinterpret_cast<void*>(0x005D637A), Naked1, 5);
	  }
  }
	
	void start()
  {
		custompacket::Start();
        HideCostume::start();
  }
}