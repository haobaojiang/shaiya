#include <windows.h>

namespace Game::Process {

	HANDLE s_handle = nullptr;



	HRESULT Initialize(DWORD pid) {

		HRESULT hr = S_OK;

		HANDLE h = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION, FALSE, pid);
		if (h == nullptr) {
			hr = HRESULT_FROM_WIN32(GetLastError());
			return hr;
		}
		s_handle = h;

		return hr;
	}

	HRESULT Fiialize() {

		HRESULT hr = S_OK;

		if (s_handle != nullptr) {
			CloseHandle(s_handle);
			s_handle = nullptr;
		}

		return hr;
	}

	template<class T>
	HRESULT Read(std::vector<DWORD> Addrs, T* Output) {

		HRESULT hr = S_OK;

		DWORD addr = 0;
		for (int i = 0; i < Addrs.size(); i++) {

			addr = addr + Addrs[i];
			DWORD numRead = 0;
			if (i == Addrs.size() - 1) {
				if (!ReadProcessMemory(s_handle, (void*)Addr, Output, sizeof(T), &numRead)) {
					hr = HRESULT_FROM_WIN32(GetLastError());
					return hr;
				}
			}
			else {
				if (!ReadProcessMemory(s_handle, (void*)Addr, &addr, DWORD, &numRead)) {
					hr = HRESULT_FROM_WIN32(GetLastError());
					return hr;
				}
			}
		}

		return hr;
	}

	template<class T>
	HRESULT Write(DWORD& Addr, T* Data) {

		HRESULT hr = S_OK;

		DWORD written = 0;
		if (!WriteProcessMemory(s_handle, (void*)Addr, Data, sizeof(T), &written)) {
			hr = HRESULT_FROM_WIN32(GetLastError());
			return hr;
		}

		return hr;
	}

	HRESULT MakeCall() {

	}
}