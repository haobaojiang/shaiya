#include <Windows.h>
#include <wil/resource.h>
#include <algorithm>
#include "../utility/process.h"








int main(int argc, char* argv[])
{

	int a[] = { 0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0x10,0x10 ,0x10 ,0x10 ,0x10 ,0x11,0x12,0x10 ,0x10 ,0x10 ,0x10 ,0x10 ,0x15,0x10,0x10 };

	printf("%p", a);
	/*
	while (!IsDebuggerPresent())
	{
		Sleep(1000);
	}
	Sleep(1000);
	*/

	DWORD pid{};
	auto hr = Utility::Process::FindProcessIdByName(L"shaiya.exe", &pid);
	if (FAILED(hr))
	{
		hr = Utility::Process::FindProcessIdByName(L"game.exe", &pid);
		if(FAILED(hr))
		{
			printf("failed to find process");
			return 0;
		}
	}

	printf("pid:%d\n", pid);
	wil::unique_handle process(OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid));
	if (!process) {
		printf("failed to open process,last err:%ld",GetLastError());
		return 0;
	}

	auto memInfos = Utility::Process::GetMemRanges(process.get(), [](const MEMORY_BASIC_INFORMATION& info)->bool {

		                                               if (info.Type != MEM_PRIVATE) {
			                                               return false;
		                                               }

		                                               if (info.RegionSize<= 0x1000) {
			                                               return false;
		                                               }

		                                               return (info.Protect & PAGE_EXECUTE) ||
			                                               (info.Protect & PAGE_EXECUTE_READ) ||
			                                               (info.Protect & PAGE_EXECUTE_READWRITE) ||
			                                               (info.Protect & PAGE_EXECUTE_WRITECOPY);
	                                               }
	);
	
	for (const auto& info : memInfos) {
		printf("BaseAddress:%p,AllocationBase:%p,size:%x\n",
			info.BaseAddress,
			info.AllocationBase,
			info.RegionSize);
	}

	std::vector<MODULEINFO> mods;
	Utility::Process::EnumProcessModules(process.get(),&mods);

	for(const auto& mem:memInfos)
	{
		bool legal = false;
		for(const auto& mod:mods)
		{
			auto modTail = reinterpret_cast<INT64>(mod.EntryPoint)+mod.SizeOfImage;
			auto memTail = reinterpret_cast<INT64>(mem.BaseAddress) + mem.RegionSize;
			if(mod.EntryPoint>=mem.BaseAddress&&modTail> memTail)
			{
				legal = true;
				break;
			}
		}
		if(!legal)
		{
			printf("illegal mem region found:%p\n",mem.BaseAddress);
		}
	}
	
	printf("end\n");
	system("pause");
	return 0;
}