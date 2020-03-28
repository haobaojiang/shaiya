#include <windows.h>
#include <stdio.h>
#include <string>
#include "MyPython.h"




#define BUFSIZE 4096 

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;

HANDLE g_hInputFile = NULL;


void CreateChildProcess(WCHAR* szCmdline1)
{
	
	WCHAR szCmdline[MAX_PATH] = { 0 };
	wcscpy(szCmdline, szCmdline1);
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;
	BOOL bSuccess = FALSE;

	// Set up members of the PROCESS_INFORMATION structure. 

	ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDIN and STDOUT handles for redirection.

	ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.hStdError = g_hChildStd_OUT_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.hStdInput = g_hChildStd_IN_Rd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process. 

	bSuccess = CreateProcess(NULL,
		szCmdline,    
		NULL,         
		NULL,         
		TRUE,         
		0,            
		NULL,         
		NULL,         
		&siStartInfo, 
		&piProcInfo); 


	if (!bSuccess)
		return;
	else
	{
		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
	}
}

void WriteToPipe(char* str)
{
	DWORD dwRead, dwWritten;
	CHAR chBuf[BUFSIZE] = { 0 };
	strcpy(chBuf, str);
	BOOL bSuccess = FALSE;
	bSuccess = WriteFile(g_hChildStd_IN_Wr, chBuf, strlen(chBuf), &dwWritten, NULL);
}

void ReadFromPipe(void)
{
	DWORD dwRead, dwWritten;
	CHAR chBuf[BUFSIZE];
	ZeroMemory(chBuf, sizeof(chBuf));
	BOOL bSuccess = FALSE;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
	printf("%s", chBuf);
}





bool InitPython(WCHAR* szCmdline)
{
	SECURITY_ATTRIBUTES saAttr;
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
		return false;

	if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
		return false;

	if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
		return false;

	if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
		return false;

	CreateChildProcess(szCmdline);
	g_hInputFile = CreateFile(
		L"c:\\1.txt",
		GENERIC_READ,
		0,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_READONLY,
		NULL);

	if (g_hInputFile == INVALID_HANDLE_VALUE)
		return false;



//	WriteToPipe();


	return true;



}

