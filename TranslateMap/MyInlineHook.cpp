#include "stdafx.h"
#include "MyInlineHook.h"


CMyInlineHook::CMyInlineHook(void)
{
	//初始化一下
	ZeroMemory(m_byOriginalCode,_countof(m_byOriginalCode)*sizeof(BYTE));
}


CMyInlineHook::~CMyInlineHook(void)
{
}

BOOL CMyInlineHook::Hook(PVOID pOriginalAddr,PVOID pNewAddr,int nSize)
{
	m_nSize=nSize;
	//1.初始化信息
	DWORD dwOldProtect;
	   //1.1改变原来的5个字节为可写
	::VirtualProtect(pOriginalAddr, m_nSize, PAGE_READWRITE, &dwOldProtect);
	   //1.2保存原来的5个字节的内容方便卸载的时候还原
	::ReadProcessMemory(GetCurrentProcess(), pOriginalAddr,m_byOriginalCode, m_nSize, NULL);
	   //1.3还原属性
	::VirtualProtect(pOriginalAddr, m_nSize, dwOldProtect, &dwOldProtect);
	M_pOriginalAddr=pOriginalAddr;

	//2.计算Jmp地址
	m_byJmpAsmCode[0]= 0xE9; // Jmp *****
	*(PDWORD)&(m_byJmpAsmCode[1]) = (DWORD)pNewAddr - (DWORD)pOriginalAddr - 5;
	   //2.1前5个字节搞定,后面几个字节填Nop
	for(int i=5;i<nSize;i++)
	{
		m_byJmpAsmCode[i]= 0x90; // 后面全部填nop[
	}
	//3.修改跳转
	::VirtualProtect(pOriginalAddr, m_nSize, PAGE_READWRITE, &dwOldProtect);
	::WriteProcessMemory(GetCurrentProcess(), pOriginalAddr, m_byJmpAsmCode, m_nSize, NULL);
	::VirtualProtect(pOriginalAddr, m_nSize, dwOldProtect, &dwOldProtect);
	return TRUE;
}
BOOL CMyInlineHook::UnHook()
{
	//如果都没有给改过，肯定不能UnHook
	if((ULONGLONG)m_byOriginalCode==0) return FALSE;

	DWORD dwOldProtect;
	//修改属性
	::VirtualProtect(M_pOriginalAddr, m_nSize, PAGE_READWRITE, &dwOldProtect);
	//还原5个字节的内容
	::WriteProcessMemory(GetCurrentProcess(), M_pOriginalAddr, m_byOriginalCode, m_nSize, NULL);
	//还原属性
	::VirtualProtect(M_pOriginalAddr, m_nSize, dwOldProtect, &dwOldProtect);
	return TRUE;
}
