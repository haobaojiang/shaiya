
#include "pch.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 头文件
#include <Windows.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "../utility/utility.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 导出函数
/*
#pragma comment(linker, "/EXPORT:SymGetOmapBlockBase=dbghelpOrg.SymGetOmapBlockBase,@1")
#pragma comment(linker, "/EXPORT:DbgHelpCreateUserDump=dbghelpOrg.DbgHelpCreateUserDump,@2")
#pragma comment(linker, "/EXPORT:DbgHelpCreateUserDumpW=dbghelpOrg.DbgHelpCreateUserDumpW,@3")
#pragma comment(linker, "/EXPORT:EnumDirTree=dbghelpOrg.EnumDirTree,@4")
#pragma comment(linker, "/EXPORT:EnumDirTreeW=dbghelpOrg.EnumDirTreeW,@5")
#pragma comment(linker, "/EXPORT:EnumerateLoadedModules64=dbghelpOrg.EnumerateLoadedModules64,@6")
#pragma comment(linker, "/EXPORT:EnumerateLoadedModules=dbghelpOrg.EnumerateLoadedModules,@7")
#pragma comment(linker, "/EXPORT:EnumerateLoadedModulesEx=dbghelpOrg.EnumerateLoadedModulesEx,@8")
#pragma comment(linker, "/EXPORT:EnumerateLoadedModulesExW=dbghelpOrg.EnumerateLoadedModulesExW,@9")
#pragma comment(linker, "/EXPORT:EnumerateLoadedModulesW64=dbghelpOrg.EnumerateLoadedModulesW64,@10")
#pragma comment(linker, "/EXPORT:ExtensionApiVersion=dbghelpOrg.ExtensionApiVersion,@11")
#pragma comment(linker, "/EXPORT:FindDebugInfoFile=dbghelpOrg.FindDebugInfoFile,@12")
#pragma comment(linker, "/EXPORT:FindDebugInfoFileEx=dbghelpOrg.FindDebugInfoFileEx,@13")
#pragma comment(linker, "/EXPORT:FindDebugInfoFileExW=dbghelpOrg.FindDebugInfoFileExW,@14")
#pragma comment(linker, "/EXPORT:FindExecutableImage=dbghelpOrg.FindExecutableImage,@15")
#pragma comment(linker, "/EXPORT:FindExecutableImageEx=dbghelpOrg.FindExecutableImageEx,@16")
#pragma comment(linker, "/EXPORT:FindExecutableImageExW=dbghelpOrg.FindExecutableImageExW,@17")
#pragma comment(linker, "/EXPORT:FindFileInPath=dbghelpOrg.FindFileInPath,@18")
#pragma comment(linker, "/EXPORT:FindFileInSearchPath=dbghelpOrg.FindFileInSearchPath,@19")
#pragma comment(linker, "/EXPORT:GetTimestampForLoadedLibrary=dbghelpOrg.GetTimestampForLoadedLibrary,@20")
#pragma comment(linker, "/EXPORT:ImageDirectoryEntryToData=dbghelpOrg.ImageDirectoryEntryToData,@21")
#pragma comment(linker, "/EXPORT:ImageDirectoryEntryToDataEx=dbghelpOrg.ImageDirectoryEntryToDataEx,@22")
#pragma comment(linker, "/EXPORT:ImageNtHeader=dbghelpOrg.ImageNtHeader,@23")
#pragma comment(linker, "/EXPORT:ImageRvaToSection=dbghelpOrg.ImageRvaToSection,@24")
#pragma comment(linker, "/EXPORT:ImageRvaToVa=dbghelpOrg.ImageRvaToVa,@25")
#pragma comment(linker, "/EXPORT:ImagehlpApiVersion=dbghelpOrg.ImagehlpApiVersion,@26")
#pragma comment(linker, "/EXPORT:ImagehlpApiVersionEx=dbghelpOrg.ImagehlpApiVersionEx,@27")
#pragma comment(linker, "/EXPORT:MakeSureDirectoryPathExists=dbghelpOrg.MakeSureDirectoryPathExists,@28")
#pragma comment(linker, "/EXPORT:MapDebugInformation=dbghelpOrg.MapDebugInformation,@29")
#pragma comment(linker, "/EXPORT:MiniDumpReadDumpStream=dbghelpOrg.MiniDumpReadDumpStream,@30")
#pragma comment(linker, "/EXPORT:MiniDumpWriteDump=dbghelpOrg.MiniDumpWriteDump,@31")
#pragma comment(linker, "/EXPORT:SearchTreeForFile=dbghelpOrg.SearchTreeForFile,@32")
#pragma comment(linker, "/EXPORT:SearchTreeForFileW=dbghelpOrg.SearchTreeForFileW,@33")
#pragma comment(linker, "/EXPORT:StackWalk64=dbghelpOrg.StackWalk64,@34")
#pragma comment(linker, "/EXPORT:StackWalk=dbghelpOrg.StackWalk,@35")
#pragma comment(linker, "/EXPORT:SymAddSourceStream=dbghelpOrg.SymAddSourceStream,@36")
#pragma comment(linker, "/EXPORT:SymAddSourceStreamA=dbghelpOrg.SymAddSourceStreamA,@37")
#pragma comment(linker, "/EXPORT:SymAddSourceStreamW=dbghelpOrg.SymAddSourceStreamW,@38")
#pragma comment(linker, "/EXPORT:SymAddSymbol=dbghelpOrg.SymAddSymbol,@39")
#pragma comment(linker, "/EXPORT:SymAddSymbolW=dbghelpOrg.SymAddSymbolW,@40")
#pragma comment(linker, "/EXPORT:SymCleanup=dbghelpOrg.SymCleanup,@41")
#pragma comment(linker, "/EXPORT:SymDeleteSymbol=dbghelpOrg.SymDeleteSymbol,@42")
#pragma comment(linker, "/EXPORT:SymDeleteSymbolW=dbghelpOrg.SymDeleteSymbolW,@43")
#pragma comment(linker, "/EXPORT:SymEnumLines=dbghelpOrg.SymEnumLines,@44")
#pragma comment(linker, "/EXPORT:SymEnumLinesW=dbghelpOrg.SymEnumLinesW,@45")
#pragma comment(linker, "/EXPORT:SymEnumProcesses=dbghelpOrg.SymEnumProcesses,@46")
#pragma comment(linker, "/EXPORT:SymEnumSourceFileTokens=dbghelpOrg.SymEnumSourceFileTokens,@47")
#pragma comment(linker, "/EXPORT:SymEnumSourceFiles=dbghelpOrg.SymEnumSourceFiles,@48")
#pragma comment(linker, "/EXPORT:SymEnumSourceFilesW=dbghelpOrg.SymEnumSourceFilesW,@49")
#pragma comment(linker, "/EXPORT:SymEnumSourceLines=dbghelpOrg.SymEnumSourceLines,@50")
#pragma comment(linker, "/EXPORT:SymEnumSourceLinesW=dbghelpOrg.SymEnumSourceLinesW,@51")
#pragma comment(linker, "/EXPORT:SymEnumSym=dbghelpOrg.SymEnumSym,@52")
#pragma comment(linker, "/EXPORT:SymEnumSymbols=dbghelpOrg.SymEnumSymbols,@53")
#pragma comment(linker, "/EXPORT:SymEnumSymbolsForAddr=dbghelpOrg.SymEnumSymbolsForAddr,@54")
#pragma comment(linker, "/EXPORT:SymEnumSymbolsForAddrW=dbghelpOrg.SymEnumSymbolsForAddrW,@55")
#pragma comment(linker, "/EXPORT:SymEnumSymbolsW=dbghelpOrg.SymEnumSymbolsW,@56")
#pragma comment(linker, "/EXPORT:SymEnumTypes=dbghelpOrg.SymEnumTypes,@57")
#pragma comment(linker, "/EXPORT:SymEnumTypesByName=dbghelpOrg.SymEnumTypesByName,@58")
#pragma comment(linker, "/EXPORT:SymEnumTypesByNameW=dbghelpOrg.SymEnumTypesByNameW,@59")
#pragma comment(linker, "/EXPORT:SymEnumTypesW=dbghelpOrg.SymEnumTypesW,@60")
#pragma comment(linker, "/EXPORT:SymEnumerateModules64=dbghelpOrg.SymEnumerateModules64,@61")
#pragma comment(linker, "/EXPORT:SymEnumerateModules=dbghelpOrg.SymEnumerateModules,@62")
#pragma comment(linker, "/EXPORT:SymEnumerateModulesW64=dbghelpOrg.SymEnumerateModulesW64,@63")
#pragma comment(linker, "/EXPORT:SymEnumerateSymbols64=dbghelpOrg.SymEnumerateSymbols64,@64")
#pragma comment(linker, "/EXPORT:SymEnumerateSymbols=dbghelpOrg.SymEnumerateSymbols,@65")
#pragma comment(linker, "/EXPORT:SymEnumerateSymbolsW64=dbghelpOrg.SymEnumerateSymbolsW64,@66")
#pragma comment(linker, "/EXPORT:SymEnumerateSymbolsW=dbghelpOrg.SymEnumerateSymbolsW,@67")
#pragma comment(linker, "/EXPORT:SymFindDebugInfoFile=dbghelpOrg.SymFindDebugInfoFile,@68")
#pragma comment(linker, "/EXPORT:SymFindDebugInfoFileW=dbghelpOrg.SymFindDebugInfoFileW,@69")
#pragma comment(linker, "/EXPORT:SymFindExecutableImage=dbghelpOrg.SymFindExecutableImage,@70")
#pragma comment(linker, "/EXPORT:SymFindExecutableImageW=dbghelpOrg.SymFindExecutableImageW,@71")
#pragma comment(linker, "/EXPORT:SymFindFileInPath=dbghelpOrg.SymFindFileInPath,@72")
#pragma comment(linker, "/EXPORT:SymFindFileInPathW=dbghelpOrg.SymFindFileInPathW,@73")
#pragma comment(linker, "/EXPORT:SymFromAddr=dbghelpOrg.SymFromAddr,@74")
#pragma comment(linker, "/EXPORT:SymFromAddrW=dbghelpOrg.SymFromAddrW,@75")
#pragma comment(linker, "/EXPORT:SymFromIndex=dbghelpOrg.SymFromIndex,@76")
#pragma comment(linker, "/EXPORT:SymFromIndexW=dbghelpOrg.SymFromIndexW,@77")
#pragma comment(linker, "/EXPORT:SymFromName=dbghelpOrg.SymFromName,@78")
#pragma comment(linker, "/EXPORT:SymFromNameW=dbghelpOrg.SymFromNameW,@79")
#pragma comment(linker, "/EXPORT:SymFromToken=dbghelpOrg.SymFromToken,@80")
#pragma comment(linker, "/EXPORT:SymFromTokenW=dbghelpOrg.SymFromTokenW,@81")
#pragma comment(linker, "/EXPORT:SymFunctionTableAccess64=dbghelpOrg.SymFunctionTableAccess64,@82")
#pragma comment(linker, "/EXPORT:SymFunctionTableAccess=dbghelpOrg.SymFunctionTableAccess,@83")
#pragma comment(linker, "/EXPORT:SymGetFileLineOffsets64=dbghelpOrg.SymGetFileLineOffsets64,@84")
#pragma comment(linker, "/EXPORT:SymGetHomeDirectory=dbghelpOrg.SymGetHomeDirectory,@85")
#pragma comment(linker, "/EXPORT:SymGetHomeDirectoryW=dbghelpOrg.SymGetHomeDirectoryW,@86")
#pragma comment(linker, "/EXPORT:SymGetLineFromAddr64=dbghelpOrg.SymGetLineFromAddr64,@87")
#pragma comment(linker, "/EXPORT:SymGetLineFromAddr=dbghelpOrg.SymGetLineFromAddr,@88")
#pragma comment(linker, "/EXPORT:SymGetLineFromAddrW64=dbghelpOrg.SymGetLineFromAddrW64,@89")
#pragma comment(linker, "/EXPORT:SymGetLineFromName64=dbghelpOrg.SymGetLineFromName64,@90")
#pragma comment(linker, "/EXPORT:SymGetLineFromName=dbghelpOrg.SymGetLineFromName,@91")
#pragma comment(linker, "/EXPORT:SymGetLineFromNameW64=dbghelpOrg.SymGetLineFromNameW64,@92")
#pragma comment(linker, "/EXPORT:SymGetLineNext64=dbghelpOrg.SymGetLineNext64,@93")
#pragma comment(linker, "/EXPORT:SymGetLineNext=dbghelpOrg.SymGetLineNext,@94")
#pragma comment(linker, "/EXPORT:SymGetLineNextW64=dbghelpOrg.SymGetLineNextW64,@95")
#pragma comment(linker, "/EXPORT:SymGetLinePrev64=dbghelpOrg.SymGetLinePrev64,@96")
#pragma comment(linker, "/EXPORT:SymGetLinePrev=dbghelpOrg.SymGetLinePrev,@97")
#pragma comment(linker, "/EXPORT:SymGetLinePrevW64=dbghelpOrg.SymGetLinePrevW64,@98")
#pragma comment(linker, "/EXPORT:SymGetModuleBase64=dbghelpOrg.SymGetModuleBase64,@99")
#pragma comment(linker, "/EXPORT:SymGetModuleBase=dbghelpOrg.SymGetModuleBase,@100")
#pragma comment(linker, "/EXPORT:SymGetModuleInfo64=dbghelpOrg.SymGetModuleInfo64,@101")
#pragma comment(linker, "/EXPORT:SymGetModuleInfo=dbghelpOrg.SymGetModuleInfo,@102")
#pragma comment(linker, "/EXPORT:SymGetModuleInfoW64=dbghelpOrg.SymGetModuleInfoW64,@103")
#pragma comment(linker, "/EXPORT:SymGetModuleInfoW=dbghelpOrg.SymGetModuleInfoW,@104")
#pragma comment(linker, "/EXPORT:SymGetOmaps=dbghelpOrg.SymGetOmaps,@105")
#pragma comment(linker, "/EXPORT:SymGetOptions=dbghelpOrg.SymGetOptions,@106")
#pragma comment(linker, "/EXPORT:SymGetScope=dbghelpOrg.SymGetScope,@107")
#pragma comment(linker, "/EXPORT:SymGetScopeW=dbghelpOrg.SymGetScopeW,@108")
#pragma comment(linker, "/EXPORT:SymGetSearchPath=dbghelpOrg.SymGetSearchPath,@109")
#pragma comment(linker, "/EXPORT:SymGetSearchPathW=dbghelpOrg.SymGetSearchPathW,@110")
#pragma comment(linker, "/EXPORT:SymGetSourceFile=dbghelpOrg.SymGetSourceFile,@111")
#pragma comment(linker, "/EXPORT:SymGetSourceFileFromToken=dbghelpOrg.SymGetSourceFileFromToken,@112")
#pragma comment(linker, "/EXPORT:SymGetSourceFileFromTokenW=dbghelpOrg.SymGetSourceFileFromTokenW,@113")
#pragma comment(linker, "/EXPORT:SymGetSourceFileToken=dbghelpOrg.SymGetSourceFileToken,@114")
#pragma comment(linker, "/EXPORT:SymGetSourceFileTokenW=dbghelpOrg.SymGetSourceFileTokenW,@115")
#pragma comment(linker, "/EXPORT:SymGetSourceFileW=dbghelpOrg.SymGetSourceFileW,@116")
#pragma comment(linker, "/EXPORT:SymGetSourceVarFromToken=dbghelpOrg.SymGetSourceVarFromToken,@117")
#pragma comment(linker, "/EXPORT:SymGetSourceVarFromTokenW=dbghelpOrg.SymGetSourceVarFromTokenW,@118")
#pragma comment(linker, "/EXPORT:SymGetSymFromAddr64=dbghelpOrg.SymGetSymFromAddr64,@119")
#pragma comment(linker, "/EXPORT:SymGetSymFromAddr=dbghelpOrg.SymGetSymFromAddr,@120")
#pragma comment(linker, "/EXPORT:SymGetSymFromName64=dbghelpOrg.SymGetSymFromName64,@121")
#pragma comment(linker, "/EXPORT:SymGetSymFromName=dbghelpOrg.SymGetSymFromName,@122")
#pragma comment(linker, "/EXPORT:SymGetSymNext64=dbghelpOrg.SymGetSymNext64,@123")
#pragma comment(linker, "/EXPORT:SymGetSymNext=dbghelpOrg.SymGetSymNext,@124")
#pragma comment(linker, "/EXPORT:SymGetSymPrev64=dbghelpOrg.SymGetSymPrev64,@125")
#pragma comment(linker, "/EXPORT:SymGetSymPrev=dbghelpOrg.SymGetSymPrev,@126")
#pragma comment(linker, "/EXPORT:SymGetSymbolFile=dbghelpOrg.SymGetSymbolFile,@127")
#pragma comment(linker, "/EXPORT:SymGetSymbolFileW=dbghelpOrg.SymGetSymbolFileW,@128")
#pragma comment(linker, "/EXPORT:SymGetTypeFromName=dbghelpOrg.SymGetTypeFromName,@129")
#pragma comment(linker, "/EXPORT:SymGetTypeFromNameW=dbghelpOrg.SymGetTypeFromNameW,@130")
#pragma comment(linker, "/EXPORT:SymGetTypeInfo=dbghelpOrg.SymGetTypeInfo,@131")
#pragma comment(linker, "/EXPORT:SymGetTypeInfoEx=dbghelpOrg.SymGetTypeInfoEx,@132")
#pragma comment(linker, "/EXPORT:SymGetUnwindInfo=dbghelpOrg.SymGetUnwindInfo,@133")
#pragma comment(linker, "/EXPORT:SymInitialize=dbghelpOrg.SymInitialize,@134")
#pragma comment(linker, "/EXPORT:SymInitializeW=dbghelpOrg.SymInitializeW,@135")
#pragma comment(linker, "/EXPORT:SymLoadModule64=dbghelpOrg.SymLoadModule64,@136")
#pragma comment(linker, "/EXPORT:SymLoadModule=dbghelpOrg.SymLoadModule,@137")
#pragma comment(linker, "/EXPORT:SymLoadModuleEx=dbghelpOrg.SymLoadModuleEx,@138")
#pragma comment(linker, "/EXPORT:SymLoadModuleExW=dbghelpOrg.SymLoadModuleExW,@139")
#pragma comment(linker, "/EXPORT:SymMatchFileName=dbghelpOrg.SymMatchFileName,@140")
#pragma comment(linker, "/EXPORT:SymMatchFileNameW=dbghelpOrg.SymMatchFileNameW,@141")
#pragma comment(linker, "/EXPORT:SymMatchString=dbghelpOrg.SymMatchString,@142")
#pragma comment(linker, "/EXPORT:SymMatchStringA=dbghelpOrg.SymMatchStringA,@143")
#pragma comment(linker, "/EXPORT:SymMatchStringW=dbghelpOrg.SymMatchStringW,@144")
#pragma comment(linker, "/EXPORT:SymNext=dbghelpOrg.SymNext,@145")
#pragma comment(linker, "/EXPORT:SymNextW=dbghelpOrg.SymNextW,@146")
#pragma comment(linker, "/EXPORT:SymPrev=dbghelpOrg.SymPrev,@147")
#pragma comment(linker, "/EXPORT:SymPrevW=dbghelpOrg.SymPrevW,@148")
#pragma comment(linker, "/EXPORT:SymRefreshModuleList=dbghelpOrg.SymRefreshModuleList,@149")
#pragma comment(linker, "/EXPORT:SymRegisterCallback64=dbghelpOrg.SymRegisterCallback64,@150")
#pragma comment(linker, "/EXPORT:SymRegisterCallback=dbghelpOrg.SymRegisterCallback,@151")
#pragma comment(linker, "/EXPORT:SymRegisterCallbackW64=dbghelpOrg.SymRegisterCallbackW64,@152")
#pragma comment(linker, "/EXPORT:SymRegisterFunctionEntryCallback64=dbghelpOrg.SymRegisterFunctionEntryCallback64,@153")
#pragma comment(linker, "/EXPORT:SymRegisterFunctionEntryCallback=dbghelpOrg.SymRegisterFunctionEntryCallback,@154")
#pragma comment(linker, "/EXPORT:SymSearch=dbghelpOrg.SymSearch,@155")
#pragma comment(linker, "/EXPORT:SymSearchW=dbghelpOrg.SymSearchW,@156")
#pragma comment(linker, "/EXPORT:SymSetContext=dbghelpOrg.SymSetContext,@157")
#pragma comment(linker, "/EXPORT:SymSetHomeDirectory=dbghelpOrg.SymSetHomeDirectory,@158")
#pragma comment(linker, "/EXPORT:SymSetHomeDirectoryW=dbghelpOrg.SymSetHomeDirectoryW,@159")
#pragma comment(linker, "/EXPORT:SymSetOptions=dbghelpOrg.SymSetOptions,@160")
#pragma comment(linker, "/EXPORT:SymSetParentWindow=dbghelpOrg.SymSetParentWindow,@161")
#pragma comment(linker, "/EXPORT:SymSetScopeFromAddr=dbghelpOrg.SymSetScopeFromAddr,@162")
#pragma comment(linker, "/EXPORT:SymSetScopeFromIndex=dbghelpOrg.SymSetScopeFromIndex,@163")
#pragma comment(linker, "/EXPORT:SymSetSearchPath=dbghelpOrg.SymSetSearchPath,@164")
#pragma comment(linker, "/EXPORT:SymSetSearchPathW=dbghelpOrg.SymSetSearchPathW,@165")
#pragma comment(linker, "/EXPORT:SymSrvDeltaName=dbghelpOrg.SymSrvDeltaName,@166")
#pragma comment(linker, "/EXPORT:SymSrvDeltaNameW=dbghelpOrg.SymSrvDeltaNameW,@167")
#pragma comment(linker, "/EXPORT:SymSrvGetFileIndexInfo=dbghelpOrg.SymSrvGetFileIndexInfo,@168")
#pragma comment(linker, "/EXPORT:SymSrvGetFileIndexInfoW=dbghelpOrg.SymSrvGetFileIndexInfoW,@169")
#pragma comment(linker, "/EXPORT:SymSrvGetFileIndexString=dbghelpOrg.SymSrvGetFileIndexString,@170")
#pragma comment(linker, "/EXPORT:SymSrvGetFileIndexStringW=dbghelpOrg.SymSrvGetFileIndexStringW,@171")
#pragma comment(linker, "/EXPORT:SymSrvGetFileIndexes=dbghelpOrg.SymSrvGetFileIndexes,@172")
#pragma comment(linker, "/EXPORT:SymSrvGetFileIndexesW=dbghelpOrg.SymSrvGetFileIndexesW,@173")
#pragma comment(linker, "/EXPORT:SymSrvGetSupplement=dbghelpOrg.SymSrvGetSupplement,@174")
#pragma comment(linker, "/EXPORT:SymSrvGetSupplementW=dbghelpOrg.SymSrvGetSupplementW,@175")
#pragma comment(linker, "/EXPORT:SymSrvIsStore=dbghelpOrg.SymSrvIsStore,@176")
#pragma comment(linker, "/EXPORT:SymSrvIsStoreW=dbghelpOrg.SymSrvIsStoreW,@177")
#pragma comment(linker, "/EXPORT:SymSrvStoreFile=dbghelpOrg.SymSrvStoreFile,@178")
#pragma comment(linker, "/EXPORT:SymSrvStoreFileW=dbghelpOrg.SymSrvStoreFileW,@179")
#pragma comment(linker, "/EXPORT:SymSrvStoreSupplement=dbghelpOrg.SymSrvStoreSupplement,@180")
#pragma comment(linker, "/EXPORT:SymSrvStoreSupplementW=dbghelpOrg.SymSrvStoreSupplementW,@181")
#pragma comment(linker, "/EXPORT:SymUnDName64=dbghelpOrg.SymUnDName64,@182")
#pragma comment(linker, "/EXPORT:SymUnDName=dbghelpOrg.SymUnDName,@183")
#pragma comment(linker, "/EXPORT:SymUnloadModule64=dbghelpOrg.SymUnloadModule64,@184")
#pragma comment(linker, "/EXPORT:SymUnloadModule=dbghelpOrg.SymUnloadModule,@185")
#pragma comment(linker, "/EXPORT:UnDecorateSymbolName=dbghelpOrg.UnDecorateSymbolName,@186")
#pragma comment(linker, "/EXPORT:UnDecorateSymbolNameW=dbghelpOrg.UnDecorateSymbolNameW,@187")
#pragma comment(linker, "/EXPORT:UnmapDebugInformation=dbghelpOrg.UnmapDebugInformation,@188")
#pragma comment(linker, "/EXPORT:WinDbgExtensionDllInit=dbghelpOrg.WinDbgExtensionDllInit,@189")
#pragma comment(linker, "/EXPORT:block=dbghelpOrg.block,@190")
#pragma comment(linker, "/EXPORT:chksym=dbghelpOrg.chksym,@191")
#pragma comment(linker, "/EXPORT:dbghelp=dbghelpOrg.dbghelp,@192")
#pragma comment(linker, "/EXPORT:dh=dbghelpOrg.dh,@193")
#pragma comment(linker, "/EXPORT:fptr=dbghelpOrg.fptr,@194")
#pragma comment(linker, "/EXPORT:homedir=dbghelpOrg.homedir,@195")
#pragma comment(linker, "/EXPORT:itoldyouso=dbghelpOrg.itoldyouso,@196")
#pragma comment(linker, "/EXPORT:lmi=dbghelpOrg.lmi,@197")
#pragma comment(linker, "/EXPORT:lminfo=dbghelpOrg.lminfo,@198")
#pragma comment(linker, "/EXPORT:omap=dbghelpOrg.omap,@199")
#pragma comment(linker, "/EXPORT:srcfiles=dbghelpOrg.srcfiles,@200")
#pragma comment(linker, "/EXPORT:stack_force_ebp=dbghelpOrg.stack_force_ebp,@201")
#pragma comment(linker, "/EXPORT:stackdbg=dbghelpOrg.stackdbg,@202")
#pragma comment(linker, "/EXPORT:sym=dbghelpOrg.sym,@203")
#pragma comment(linker, "/EXPORT:symsrv=dbghelpOrg.symsrv,@204")
#pragma comment(linker, "/EXPORT:vc7fpo=dbghelpOrg.vc7fpo,@205")
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

*/
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <string>
#include <vector>
#include <process.h>
#include "../utility/utility.h"



namespace GamelogFix {


	struct CServerSystemInfo {
		BYTE byId;
		WORD ServerPort;
		WORD wMaxConnection;
		char szName[256];
		DWORD dwVersion;
	};

	bool StartFileChkutf8(int append, int replace) {
		bool res = false;
		auto EDI = ".\\Log\\PS_GAMELOG__system.log";
		auto callAddr = 0x0040c790;//0x54FCC0;
		_asm {
			mov edi, replace
			push edi
			mov ebx, append
			mov edi, EDI
			mov esi, 0x54FCC0
			call callAddr
			mov res, al
		}
		return res;
	}

	bool initlogFunction()
	{

		CServerSystemInfo* g_server = reinterpret_cast<CServerSystemInfo*>(0x0046c578);

		const char* configFile = ".\\Config\\ps_gamelog.ini";
		if (GetPrivateProfileStringA("SERVER", "ServerName", "", g_server->szName, 256, configFile) == 0) {
			strcpy_s(g_server->szName, "testServer");
		}

		int replace = GetPrivateProfileIntA("LOGFILE", "Replace", 1, configFile);
		int append = GetPrivateProfileIntA("LOGFILE", "Append", 1, configFile);

		auto result = StartFileChkutf8(append, replace);
		return result;
	}

	bool CServerStart(int port, int connections) {

		DWORD addr = 0x00405ee0;
		bool result = false;
		_asm {
			push  connections
			push port
			call addr
			mov result, al
		}
		return result;
	}

	bool StartAcception() {
		auto acceptor = 0x551EF0;
		DWORD addr = 0x00418460;
		bool res = false;
		_asm {
			mov ecx, acceptor
			call addr
			mov res, al
		}
		return res;
	}



	bool InitServer() {

		CServerSystemInfo* g_server = reinterpret_cast<CServerSystemInfo*>(0x0046c578);
		const char* configFile = ".\\Config\\ps_gamelog.ini";
		g_server->wMaxConnection = GetPrivateProfileIntA("SERVER", "MaxConnection", 32767, configFile);
		g_server->ServerPort = GetPrivateProfileIntA("SERVER", "ServerPort", 0, configFile);

		if (!CServerStart(g_server->ServerPort, g_server->wMaxConnection)) {
			return false;
		}
		return StartAcception();
	}


	__declspec(naked) void  Naked_Initlog() {
		_asm {
			MYASMCALL(initlogFunction)
			retn
		}
	}
	__declspec(naked) void  Naked_InitServer() {
		_asm {
			MYASMCALL(InitServer)
			retn
		}
	}


	void fix_ps_gamelog() {

		std::wstring exe = ShaiyaUtility::GetCurrentExePathW();

		if (_wcsicmp(exe.c_str(), L"ps_gamelog.exe") == 0) {
			ShaiyaUtility::CMyInlineHook obj;
			obj.Hook(0x00408d60, Naked_Initlog, 5);

			ShaiyaUtility::CMyInlineHook obj1;
			obj1.Hook(0x00408ea0, Naked_InitServer, 5);
		}
	}
}


namespace GameFix {

	struct CServerSystemInfo {
		BYTE byId;
		WORD ServerPort;
		WORD wMaxConnection;
		char szName[256];
		DWORD dwVersion;
	};

	CServerSystemInfo* g_server = reinterpret_cast<CServerSystemInfo*>(0x1257C70);
	char* g_publicIP = (char*)0x5BDF58;

	template <class Container>
	void split1(const std::string& str, Container& cont)
	{
		std::istringstream iss(str);
		std::copy(std::istream_iterator<std::string>(iss),
			std::istream_iterator<std::string>(),
			std::back_inserter(cont));
	}

	bool ServerConnectToMgr(const char* ip,int port) {

		DWORD addr = 0x00518d50;
		bool res = false;

		DWORD clientToMgr = DWORD(g_server) + 0x59d0;

		_asm {
			push port
			push ip
			mov ecx, clientToMgr
			call addr
			mov res,al
		}
		return res;
	}

	bool ConnectDBAgent(const char* ip, int port) {

		DWORD addr = 0x00518d50;
		bool res = false;

		DWORD dbToAgent = DWORD(g_server) + 0x5ab8;

		_asm {
			push port
			push ip
			mov ecx, dbToAgent
			call addr
			mov res, al
		}
		return res;
	}

	bool ConnectLog(DWORD obj) {
		DWORD addr = 0x00402600;
		bool res = false;
		_asm {
			push obj
			call addr
			mov res,al
		}
		return res;
	}

	bool DBSendConnect(const char* name) {
		DWORD addr = 0x00406820;
		bool res = false;

		_asm {
			push name
			call addr
			mov res, al
		}
		return res;
	}

	bool ClientSendConnect(const char* ip,const char* name) {

		DWORD addr = 0x00408730;
		bool res = false;
		_asm {
			mov esi,ip
			push name
			call  addr
			mov res,al
		}
		return res;
	}

	bool CServerStart(int port,int connections) {

		DWORD addr = 0x00401fd0;
		bool res = false;
		_asm {
			push port
			push connections
			mov ebx, g_server
			call addr
			mov res,al
		}
		return res;
	}

	bool InitServer() {


		auto parseIP = [](_In_ const char* S, _Inout_ std::string* IP, _Inout_ int* Port)->bool {
			std::vector<std::string> words;
			split1(S, words);
			if (words.size() < 2) {
				return false;
			}
			*IP = words[0];
			*Port = atoi(words[1].c_str());
			return true;
		};

		
		const char* configFile = ".\\Config\\ps_game.ini";
		g_server->wMaxConnection = GetPrivateProfileIntA("SERVER", "MaxConnection", 32767, configFile);
		g_server->ServerPort = GetPrivateProfileIntA("SERVER", "ServerPort", 30810, configFile);


		if (!CServerStart(g_server->ServerPort, g_server->wMaxConnection)) {
			return false;
		}

		// session
		{
			char value[256]{ 0 };
			if (!GetPrivateProfileStringA("CONNECTION", "Session", "", value, 256, configFile)) {
				return false;
			}
			std::string ip;
			int port = 0;
			if (!parseIP(value, &ip, &port)) {
				return false;
			}

			if (!ServerConnectToMgr(ip.c_str(), port)) {
				return false;
			}
		}

		//
		auto version = GetPrivateProfileIntA("GAMEINFO", "GameVersion", 0, configFile);
		GetPrivateProfileStringA("GAMEINFO", "GameName", "", g_server->szName, 256, configFile);
		GetPrivateProfileStringA("GAMEINFO", "GamePublicIP", "", g_publicIP, 256, configFile);
		if (g_publicIP[0] == '\0') {
			return false;
		}

		if (!ClientSendConnect(g_publicIP, g_server->szName)) {
			return false;
		}

		// db
		{
			char value[256]{ 0 };
			GetPrivateProfileStringA("CONNECTION", "DBAgent", "", value, 256, configFile);

			std::string ip;
			int port = 0;
			if (!parseIP(value, &ip, &port)) {
				return false;
			}

			if (!ConnectDBAgent(ip.c_str(), port)) {
				return false;
			}

			//差2个threadstart
			if (!DBSendConnect(g_server->szName)) {
				return false;
			}
		}
		// user log
		{
			char value[256]{ 0 };
			GetPrivateProfileStringA("CONNECTION", "UserLog", "", value, 256, configFile);

			std::string ip;
			int port = 0;
			if (!parseIP(value, &ip, &port)) {
				return false;
			}

		
	//		if (!ConnectLog()) {

		//	}

		}


		return true;
	}

	void fix_ps_game() {

	}
	
}


/*
void __stdcall SThreadStartMonitor(DWORD ESP,DWORD obj) {
	OutputDebugStringW(std::to_wstring(obj).c_str());
	if (ESP < 0x00401000 || ESP>0x00584fff) {	
		OutputDebugStringW(std::to_wstring(obj).c_str());
	}
}
*/

/*
ShaiyaUtility::CMyInlineHook g_thredstartHook;
__declspec(naked) void  Naked_SThreadStart() {
	_asm {
		pushad
		mov eax,[esp+0x20]
		MYASMCALL_2(SThreadStartMonitor,eax,ecx)
		popad
		push esi
		mov esi,ecx
		cmp byte ptr [esi+0x8],0x0
		jmp g_thredstartHook.m_pRet
	}
}

void testHook() {

	g_thredstartHook.Hook(0x00515be0, Naked_SThreadStart, 7);

}
*/
void threadProc(void*) {

	//GamelogFix::fix_ps_gamelog();

	//testHook();

	/*
	DWORD pid = 0;
	auto hr = Utility::Process::FindProcessIdByName(L"ps_game.exe", &pid);
	if (FAILED(hr)) {
		fprintf(stderr, "not process found\n");
		return;
	}

	HANDLE process = OpenProcess(PROCESS_VM_READ, false, pid);
	if (process == nullptr) {
		fprintf(stderr, "failed to open process\n");
		return;
	}

	std::vector<BYTE> data;
	for (auto addr = 0x00401000; addr < 0x00584A19;) {

		BYTE buffer[4096]{};
		DWORD  nRead = 0;
		if (!ReadProcessMemory(process, (void*)addr, buffer, 4096, &nRead)) {
			fprintf(stderr, "failed to ReadProcessMemory process\n");
			return ;
		}
		data.insert(data.end(), buffer, buffer + nRead);
		addr += nRead;
	}

	 hr = Utility::File::WriteFileData(L"d:\\tmp", data.data(), data.size());
	if (FAILED(hr)) {
		printf("failed to write file:%08x", hr);
		return;
	}*/

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 入口函数
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, PVOID pvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
//		DisableThreadLibraryCalls(hModule);
//		threadProc(nullptr);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
	}

	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
