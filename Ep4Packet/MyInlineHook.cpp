#include "stdafx.h"
#include "MyInlineHook.h"


CMyInlineHook::CMyInlineHook(void)
{
	//��ʼ��һ��
	ZeroMemory(m_byOriginalCode,_countof(m_byOriginalCode)*sizeof(BYTE));
}


CMyInlineHook::~CMyInlineHook(void)
{
}

BOOL CMyInlineHook::Hook(PVOID pOriginalAddr,PVOID pNewAddr,int nSize)
{
	m_nSize=nSize;
	//1.��ʼ����Ϣ
	DWORD dwOldProtect;
	   //1.1�ı�ԭ����5���ֽ�Ϊ��д
	::VirtualProtect(pOriginalAddr, m_nSize, PAGE_READWRITE, &dwOldProtect);
	   //1.2����ԭ����5���ֽڵ����ݷ���ж�ص�ʱ��ԭ
	::ReadProcessMemory(GetCurrentProcess(), pOriginalAddr,m_byOriginalCode, m_nSize, NULL);
	   //1.3��ԭ����
	::VirtualProtect(pOriginalAddr, m_nSize, dwOldProtect, &dwOldProtect);
	M_pOriginalAddr=pOriginalAddr;

	//2.����Jmp��ַ
	m_byJmpAsmCode[0]= 0xE9; // Jmp *****
	*(PDWORD)&(m_byJmpAsmCode[1]) = (DWORD)pNewAddr - (DWORD)pOriginalAddr - 5;
	   //2.1ǰ5���ֽڸ㶨,���漸���ֽ���Nop
	for(int i=5;i<nSize;i++)
	{
		m_byJmpAsmCode[i]= 0x90; // ����ȫ����nop[
	}
	//3.�޸���ת
	::VirtualProtect(pOriginalAddr, m_nSize, PAGE_READWRITE, &dwOldProtect);
	::WriteProcessMemory(GetCurrentProcess(), pOriginalAddr, m_byJmpAsmCode, m_nSize, NULL);
	::VirtualProtect(pOriginalAddr, m_nSize, dwOldProtect, &dwOldProtect);
	return TRUE;
}
BOOL CMyInlineHook::UnHook()
{
	//�����û�и��Ĺ����϶�����UnHook
	if((ULONGLONG)m_byOriginalCode==0) return FALSE;

	DWORD dwOldProtect;
	//�޸�����
	::VirtualProtect(M_pOriginalAddr, m_nSize, PAGE_READWRITE, &dwOldProtect);
	//��ԭ5���ֽڵ�����
	::WriteProcessMemory(GetCurrentProcess(), M_pOriginalAddr, m_byOriginalCode, m_nSize, NULL);
	//��ԭ����
	::VirtualProtect(M_pOriginalAddr, m_nSize, dwOldProtect, &dwOldProtect);
	return TRUE;
}
