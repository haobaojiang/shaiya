// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <windows.h>
#include "../utility/utility.h"
#include <queue>

#include <plog/Log.h>
#include <plog/Converters/NativeEOLConverter.h>
#include <process.h>

struct itemTask {
	DWORD Item{ 0 };
	DWORD TickCount{ 0 };
	DWORD Addr{ 0 };
};

std::queue<itemTask>* g_items;
CRITICAL_SECTION g_cs;

void __stdcall fun(DWORD Item, DWORD Addr) {


	if (Addr == 0x46ddd7 || Addr == 0x46dfc7) {
		return;
	}

	itemTask info;
	info.Item = Item;
	info.TickCount = GetTickCount() + 200;
	info.Addr = Addr;

	EnterCriticalSection(&g_cs);
	g_items->push(info);
	LeaveCriticalSection(&g_cs);
}


BOOL InitLogging() {

	auto path = ShaiyaUtility::GetCurrentExePathW();
	path += L"\\ItemLog";

	static plog::RollingFileAppender<plog::TxtFormatter, plog::NativeEOLConverter<> > fileAppender(path.c_str(), 999999, 200); // Create an appender with NativeEOLConverter.
	plog::init(plog::debug, &fileAppender);
	return TRUE;
}




void threadProc(void*) {

	while (true)
	{

		EnterCriticalSection(&g_cs);
		auto tickCount = GetTickCount();
		for (int i = 0; i < 50; i++) {

			if (g_items->empty()) {
				break;
			}

			auto info = g_items->front();
			if (tickCount < info.TickCount) {
				break;
			}

			auto item = info.Item;

			auto itemType = ShaiyaUtility::EP4::GetItemType(item);
			auto itemTypeid = ShaiyaUtility::EP4::GetItemTypeid(item);

			if (itemType == 44 && itemTypeid == 22) {

				auto uid = ShaiyaUtility::EP4::GetItemTypeUID(item);
				auto count = ShaiyaUtility::EP4::GetItemTypeCount(item);


				char str[MAX_PATH]{ 0 };
				sprintf_s(str, MAX_PATH, "type:%hhu,id:%hhu,uid:%llu,count:%hhu,addr:%x",
					itemType,
					itemTypeid,
					uid,
					count,
					info.Addr
				);

				LOGD << str;
			}

			g_items->pop();
		}
		LeaveCriticalSection(&g_cs);

		Sleep(500);
	}

}



__declspec(naked) void Naked()
{
	_asm
	{
		mov eax, esi
		pop esi
		pop ebx

		pushad
		mov ebx, [esp + 0x20]
		MYASMCALL_2(fun, eax, ebx)
		popad
		retn
	}
}


void fixBackDoor(void*) {

	bool fix1 = false;
	bool fix2 = false;

	while (true)
	{
		if (!fix1) {
			auto addr = 0x0045f132;
			if (*PBYTE(addr) == 0xe9) {
				BYTE temp[] = { 0xE8 ,0x19 ,0x5D ,0x07 ,0x00 };
				ShaiyaUtility::WriteCurrentProcessMemory((void*)addr, temp, sizeof(temp));
				fix1 = true;
			}
		}

		if (!fix2) {
			auto addr = 0x00470540;
			if (*PBYTE(addr) == 0xe9) {
				BYTE temp[] = { 0x0F ,0xB6 ,0x88 ,0x30 ,0x0E ,0x47 ,0x00 };
				ShaiyaUtility::WriteCurrentProcessMemory((void*)addr, temp, sizeof(temp));
				fix2 = true;
			}
		}

		if (fix1 && fix2) {
			break;
		}

		Sleep(5000);
	}
}



void mainProcess() {

	InitializeCriticalSection(&g_cs);

	InitLogging();



	g_items = new std::queue<itemTask>;

	ShaiyaUtility::CMyInlineHook g_hook;
	g_hook.Hook((void*)0x0040a57d, Naked, 5);

	_beginthread(threadProc, 0, 0);
	_beginthread(fixBackDoor, 0, 0);
}



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		mainProcess();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

