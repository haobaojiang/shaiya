
#include <windows.h>
#include <string>
#include <mutex>

namespace Game::Util {

	static HANDLE handle = nullptr;


	template<class T>
	T read(const DWORD& obj)
	{
#ifndef _DEBUG
		T value = { 0 };
		__try {
			value = *(T*)(obj);
		}
		__except (1) {
		}

		return value;
#else
		static std::once_flag flag;
		std::call_once(flag, [&]() 
			{
				handle = ::OpenProcess(PROCESS_VM_READ, FALSE, 12200);
			});
		T value = { 0 };
		DWORD bytesRead = 0;
		if (!ReadProcessMemory(handle, (void*)obj, &value, sizeof(value), &bytesRead)) {
			return value;
		}
		return value;
#endif
	}

	std::string readstr(const DWORD& obj) {

#ifndef _DEBUG
		const char* value = nullptr;
		__try {
			value = (const char*)(obj);
		}
		__except (1) {
		}

		return value;
#else
		char buf[50]{};
		DWORD bytesRead = 0;
		if (!ReadProcessMemory(handle, (void*)obj, buf, sizeof(buf), &bytesRead)) {
			return buf;
		}
		return buf;
#endif
	}
}