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
	PVOID m_pNewAddr;          //用户自定义的函数地址
	PVOID M_pOriginalAddr;     //原始的函数地址
	BYTE  m_byOriginalCode[20];
	BYTE  m_byJmpAsmCode[20];  
	int   m_nSize;             //要改的地方到底要改几个字节,最少是5个或以上
	PVOID m_pRet;

	BYTE m_byOldCodes[40];

};



