// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"


#include "detours.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// µ¼³öº¯Êý
#pragma comment(linker, "/EXPORT:ijlGetLibVersion=ijl15OrgOrg.ijlGetLibVersion,@1")
#pragma comment(linker, "/EXPORT:ijlInit=ijl15OrgOrg.ijlInit,@2")
#pragma comment(linker, "/EXPORT:ijlFree=ijl15OrgOrg.ijlFree,@3")
#pragma comment(linker, "/EXPORT:ijlRead=ijl15OrgOrg.ijlRead,@4")
#pragma comment(linker, "/EXPORT:ijlWrite=ijl15OrgOrg.ijlWrite,@5")
#pragma comment(linker, "/EXPORT:ijlErrorStr=ijl15OrgOrg.ijlErrorStr,@6")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//#pragma comment(lib,"Detours.lib")

static HANDLE (WINAPI* TrueCreateFileA)(
	_In_ LPCSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile) = CreateFileA;


static  BOOL(WINAPI* TrueCloseHandle)(_In_ _Post_ptr_invalid_ HANDLE hObject) = CloseHandle;



static BOOL(WINAPI* TrueReadFile)(
    _In_ HANDLE hFile,
    _Out_writes_bytes_to_opt_(nNumberOfBytesToRead, *lpNumberOfBytesRead) __out_data_source(FILE) LPVOID lpBuffer,
    _In_ DWORD nNumberOfBytesToRead,
    _Out_opt_ LPDWORD lpNumberOfBytesRead,
    _Inout_opt_ LPOVERLAPPED lpOverlapped
    ) = ReadFile;

static HANDLE g_handle = nullptr;
static HANDLE g_outHandle = nullptr;

BOOL WINAPI MyReadFile(
    _In_ HANDLE hFile,
    _Out_writes_bytes_to_opt_(nNumberOfBytesToRead, *lpNumberOfBytesRead) __out_data_source(FILE) LPVOID lpBuffer,
    _In_ DWORD nNumberOfBytesToRead,
    _Out_opt_ LPDWORD lpNumberOfBytesRead,
    _Inout_opt_ LPOVERLAPPED lpOverlapped
)
{
    auto ret = TrueReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    if (ret && hFile == g_handle)
    {
        DWORD written = 0;
        WriteFile(g_outHandle, lpBuffer, *lpNumberOfBytesRead, &written, nullptr);
    }
    return ret;
}

BOOL WINAPI MyCloseHandle(_In_ _Post_ptr_invalid_ HANDLE hObject) {
    auto ret = TrueCloseHandle(hObject);
    if (hObject == g_handle) {
        OutputDebugStringA("closing handle\n");
        TrueCloseHandle(g_outHandle);
        g_outHandle = nullptr;
    }
    return ret;
}


HANDLE WINAPI MyCreateFileA(
    _In_ LPCSTR lpFileName,
    _In_ DWORD dwDesiredAccess,
    _In_ DWORD dwShareMode,
    _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    _In_ DWORD dwCreationDisposition,
    _In_ DWORD dwFlagsAndAttributes,
    _In_opt_ HANDLE hTemplateFile
) 
{
	auto ret = TrueCreateFileA(lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile
	);

    OutputDebugStringA(lpFileName);
    if (strcmp(lpFileName,"data.sah")==0) {
        OutputDebugStringA("111");
        OutputDebugStringA(lpFileName);
        g_handle = ret;
    }

    return ret;
}




void func(void* p) {

    auto handle = ::CreateFileA(
        "data.sah",
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );



    auto buf = new BYTE[1024*1024*2]{};
    DWORD bytesRead = 0;
    ReadFile(handle, buf, 1024*1024*2, &bytesRead, nullptr);
    CloseHandle(handle);

    auto output = ::CreateFileW(
        L"d:\\output",
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        0,
        nullptr
    );

    WriteFile(output, buf, bytesRead, &bytesRead, nullptr);
    CloseHandle(output);
}

void fun1() {

   g_outHandle = ::CreateFileW(
		L"d:\\output",
		GENERIC_WRITE,
		0,
		nullptr,
		CREATE_ALWAYS,
		0,
		nullptr
	);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        fun1();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourAttach(&(PVOID&)TrueCreateFileA, MyCreateFileA);
        DetourAttach(&(PVOID&)TrueReadFile, MyReadFile);
        DetourAttach(&(PVOID&)TrueCloseHandle, MyCloseHandle);
		DetourTransactionCommit();

        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)TrueCreateFileA, MyCreateFileA);
        DetourDetach(&(PVOID&)TrueReadFile, MyReadFile);
        DetourDetach(&(PVOID&)TrueCloseHandle, MyCloseHandle);
		DetourTransactionCommit();

        if (g_outHandle) {
            CloseHandle(g_outHandle);
        }

        break;
    }
    return TRUE;
}

