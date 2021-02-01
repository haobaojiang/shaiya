// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <windows.h>
#include "../utility/utility.h"
#include <httplib.h>
#include <optional>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
#pragma comment(linker, "/EXPORT:ijlGetLibVersion=ijl15OrgOrg.ijlGetLibVersion,@1")
#pragma comment(linker, "/EXPORT:ijlInit=ijl15OrgOrg.ijlInit,@2")
#pragma comment(linker, "/EXPORT:ijlFree=ijl15OrgOrg.ijlFree,@3")
#pragma comment(linker, "/EXPORT:ijlRead=ijl15OrgOrg.ijlRead,@4")
#pragma comment(linker, "/EXPORT:ijlWrite=ijl15OrgOrg.ijlWrite,@5")
#pragma comment(linker, "/EXPORT:ijlErrorStr=ijl15OrgOrg.ijlErrorStr,@6")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


std::string get_url_ip()
{
	static bool flag = false;
	static std::string ip;
	if (flag)
	{
		return ip;
	}
	flag = true;

	char dir[MAX_PATH]{};
	GetCurrentDirectoryA(MAX_PATH, dir);

	std::string iniFile(dir);
	iniFile.append("/ctserver.ini");

	char tmp[256]{};
	GetPrivateProfileStringA("servershuangxian", "server1_ip", "127.0.217.58", tmp, 256, iniFile.c_str());
	ip = tmp;
	return ip;
}

std::string get_url_base()
{
	auto ip = get_url_ip();
	char url[MAX_PATH]{};
	sprintf_s(url, MAX_PATH, "http://%s:7088", ip.c_str());
	return url;
}

std::optional<std::string> get_url_resp(const std::string& url_base,const std::string& url_path)
{
	httplib::Client cli(url_base.c_str());
	auto res = cli.Get(url_path.c_str());
	if (!res)
	{
		return std::nullopt;
	}
	return res->body;
}

std::optional<std::string> HttpGet(const std::string& url_path)
{
	const auto url_base = get_url_base();
	return get_url_resp(url_base, url_path);
}

struct GlobalHookAddr
{
	DWORD HidePet;
	DWORD HideWing;
	DWORD HideEffect;
	DWORD HideCostume;
	DWORD CharCommand;
	DWORD SkillCut;
};


GlobalHookAddr g_hook_addr;


bool InitHookAddr()
{
	
#ifdef _DEBUG
	g_hook_addr.HidePet = 0x00603ca7;
	g_hook_addr.HideWing = 0x0060b5a5;
	g_hook_addr.HideEffect = 0x00609ff5;
	g_hook_addr.HideCostume = 0x006022C4;
	g_hook_addr.CharCommand = 0x004e07c2;
	g_hook_addr.SkillCut = 4563552;
#endif
	auto resp = HttpGet("/shaiya/skillcut_addr.txt");
	if(!resp)
	{
		return false;
	}
	
	sscanf(resp.value().c_str(), "%x,%x,%x,%x,%x,%x",
		&g_hook_addr.HidePet,
		&g_hook_addr.HideWing,
		&g_hook_addr.HideEffect,
		&g_hook_addr.HideCostume,
		&g_hook_addr.CharCommand,
		&g_hook_addr.SkillCut);

	return true;
}


namespace HideEffect
{
	ShaiyaUtility::CMyInlineHook objPet, objWing, objEffect, objCostume;


	bool g_IsShowPet = true;
	bool g_IsShowWing = true;
	bool g_IsShowEffect = true;
	bool g_IsShowCostume = true;

	DWORD ignorePet = 0x00603E6B;
	DWORD ignoreWing = 0x0060B76D;
	DWORD ignoreEffect = 0x0060A101;
	DWORD ignoreCostume = 0x0060232F;

	__declspec(naked) void Naked_HidePet()
	{
		_asm
		{

			lea eax, g_IsShowPet
			cmp byte ptr[eax], 0x0
			jne __orginal

			jmp ignorePet
			__orginal :
			cmp dword ptr ss : [esp + 0x30] , 0x0
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
			__orginal :
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

	__declspec(naked) void Naked_costume()
	{
		_asm
		{
			lea ecx, g_IsShowEffect
			cmp byte ptr [ecx],0x0
			jne __orginal
			jmp ignoreCostume

			__orginal:
			mov ecx, dword ptr ds : [eax + 8]
				movzx edx, word ptr ds : [ecx]
			jmp objCostume.m_pRet
		}
	}
	
	void Start()
	{	
		objPet.Hook(g_hook_addr.HidePet, Naked_HidePet, 5);
		objWing.Hook(g_hook_addr.HideWing, Naked_HideWing, 6);
		objEffect.Hook(g_hook_addr.HideEffect, Naked_effect, 5);
		objCostume.Hook(g_hook_addr.HideCostume, Naked_costume, 6);
	}
}

namespace ChatCommand
{
	ShaiyaUtility::CMyInlineHook g_obj1;
	void  __stdcall fun_chat_command(char* strChat)
	{

		do
		{
			if (strcmp("/开启宠物",strChat)== 0) {
				HideEffect::g_IsShowPet = true;
				break;
			}

			if (strcmp("/关闭宠物", strChat) == 0) {
				HideEffect::g_IsShowPet = false;
				break;
			}

			if (strcmp("/开启翅膀", strChat)==0){
				HideEffect::g_IsShowWing = true;
				break;
			}

			if (strcmp("/关闭翅膀", strChat) == 0 ) {
				HideEffect::g_IsShowWing = false;
				break;
			}

			if (strcmp("/开启特效", strChat) == 0) {
				HideEffect::g_IsShowEffect = true;
				break;
			}

			if (strcmp("/关闭特效", strChat) == 0) {
				HideEffect::g_IsShowEffect = false;
				break;
			}

			if (strcmp("/开启时装", strChat) == 0) {
				HideEffect::g_IsShowCostume = true;
				break;
			}

			if (strcmp("/关闭时装", strChat)==0) {
				HideEffect::g_IsShowCostume = false;
				break;
			}


		} while (0);
	}

	__declspec(naked) void Naked_1()
	{
		_asm
		{
			mov eax, edi
			lea edx, dword ptr[eax + 1]  //orginal
			pushad
			MYASMCALL_1(fun_chat_command,eax)
			popad
			jmp g_obj1.m_pRet

		}
	}
	void Start()
	{
		g_obj1.Hook(g_hook_addr.CharCommand, Naked_1,5);
	}
}


namespace SkillCutting {

	ShaiyaUtility::CMyInlineHook g_hook;
	DWORD g_skipAddr = 0x0045A289;
	__declspec(naked) void Naked()
	{
		_asm
		{
			cmp al, 0x2f    // normal attack
			jne _org

			mov eax, 0x221d000
			mov eax, dword ptr [eax + 0x140da4]
			mov dword ptr [eax + 0x17c], 0x0
			jmp g_skipAddr

			_org:
			mov eax, 0xC7
			jmp g_hook.m_pRet
		}
	}

	void start() {
		g_hook.Hook(reinterpret_cast<void*>(g_hook_addr.SkillCut), Naked, 5);
	}
}

void start(void*)
{
	if(!InitHookAddr())
	{
		return;
	}
	Sleep(20 * 1000);
	SkillCutting::start();
	ChatCommand::Start();
	HideEffect::Start();
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		_beginthread(start, 0, 0);
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

