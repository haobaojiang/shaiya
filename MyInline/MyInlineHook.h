#ifdef _DEBUG
#pragma comment(lib,"D:/IncludeFiels/include/debug/MyInline.lib")
#else
#pragma comment(lib,"D:/IncludeFiels/include/release/MyInline.lib")
#endif


#pragma once
class CMyInlineHook
{
public:
	CMyInlineHook(PVOID pOriginalAddr, PVOID pNewAddr, PDWORD pReturnAddr,int nSize = 5);
	CMyInlineHook(void);
	~CMyInlineHook(void);

	DWORD CMyInlineHook::Hook(PVOID pOriginalAddr, PVOID pNewAddr, int nSize = 5);
	BOOL UnHook();
	PVOID m_pOldFunction;
private:
	PVOID m_pNewAddr;          //�û��Զ���ĺ�����ַ
	PVOID M_pOriginalAddr;     //ԭʼ�ĺ�����ַ
	BYTE  m_byOriginalCode[20];
	BYTE  m_byJmpAsmCode[20];  
	int   m_nSize;             //Ҫ�ĵĵط�����Ҫ�ļ����ֽ�,������5��������
	PVOID m_pRet;

	BYTE m_byOldCodes[40];

};



