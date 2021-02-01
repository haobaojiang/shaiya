#include <Windows.h>
#include <wil/resource.h>
#include "../utility/process.h"



int main(int argc, char* argv[])
{

	DWORD pid{};
	const auto hr = Utility::Process::FindProcessIdByName(L"game.exe", &pid);
	if(FAILED(hr))
	{
		return 0;
	}

	wil::unique_handle process(OpenProcess(PROCESS_VM_READ, FALSE, pid));
	if(!process)
	{
		return 0;
	}

	auto buf = std::make_unique<BYTE[]>(1000);
	DWORD nRead = 0;
	if(!ReadProcessMemory(process.get(), reinterpret_cast<void*>(0x00405555), buf.get(), 1000, &nRead))
	{
		return 0;
	}
	
	
	return 0;
}