#include <windows.h>
#include "MyInlineHook.h"



CMyInlineHook::CMyInlineHook(void)
{
	ZeroMemory(m_byOriginalCode,_countof(m_byOriginalCode)*sizeof(BYTE));
	ZeroMemory(m_byJmpAsmCode, _countof(m_byJmpAsmCode)*sizeof(BYTE));
}
CMyInlineHook::CMyInlineHook(PVOID pOriginalAddr, PVOID pNewAddr, PDWORD pReturnAddr, int nSize)
{
	ZeroMemory(m_byOriginalCode, _countof(m_byOriginalCode)*sizeof(BYTE));
	ZeroMemory(m_byJmpAsmCode, _countof(m_byJmpAsmCode)*sizeof(BYTE));
	*pReturnAddr = Hook(pOriginalAddr, pNewAddr, nSize);
}



CMyInlineHook::~CMyInlineHook(void)
{
}

DWORD CMyInlineHook::Hook(PVOID pOriginalAddr, PVOID pNewAddr, int nSize)
{
	m_nSize = nSize;
	DWORD dwOldProtect;
	::VirtualProtect(pOriginalAddr, m_nSize, PAGE_READWRITE, &dwOldProtect);
	::ReadProcessMemory(GetCurrentProcess(), pOriginalAddr, m_byOriginalCode, m_nSize, NULL);
	::VirtualProtect(pOriginalAddr, m_nSize, dwOldProtect, &dwOldProtect);
	M_pOriginalAddr = pOriginalAddr;

	//准备好另一块空间用来执行原来的指令
	m_pOldFunction = m_byOldCodes;
	memcpy(m_byOldCodes, pOriginalAddr, nSize);
	m_byOldCodes[nSize] = 0xe9;
	DWORD dwReturnAddr = DWORD(&m_byOldCodes[nSize]);
	*PDWORD(&m_byOldCodes[nSize + 1]) = (DWORD(pOriginalAddr) + nSize) - dwReturnAddr-5;
	::VirtualProtect(pOriginalAddr, m_nSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);  //让这段内存可以执行


	




	m_byJmpAsmCode[0] = 0xE9; // Jmp *****
	*(PDWORD)&(m_byJmpAsmCode[1]) = (DWORD)pNewAddr - (DWORD)pOriginalAddr - 5;

	for (int i = 5; i < nSize; i++)
	{
		m_byJmpAsmCode[i] = 0x90; 
	}
	::VirtualProtect(pOriginalAddr, m_nSize, PAGE_READWRITE, &dwOldProtect);
	::WriteProcessMemory(GetCurrentProcess(), pOriginalAddr, m_byJmpAsmCode, m_nSize, NULL);
	::VirtualProtect(pOriginalAddr, m_nSize, dwOldProtect, &dwOldProtect);

	m_pRet = PVOID(DWORD(pOriginalAddr) + nSize);

	return (DWORD)m_pRet;
}
BOOL CMyInlineHook::UnHook()
{

	if((ULONGLONG)m_byOriginalCode==0) return FALSE;

	DWORD dwOldProtect;
	::VirtualProtect(M_pOriginalAddr, m_nSize, PAGE_READWRITE, &dwOldProtect);
	::WriteProcessMemory(GetCurrentProcess(), M_pOriginalAddr, m_byOriginalCode, m_nSize, NULL);
	::VirtualProtect(M_pOriginalAddr, m_nSize, dwOldProtect, &dwOldProtect);
	return TRUE;
}










