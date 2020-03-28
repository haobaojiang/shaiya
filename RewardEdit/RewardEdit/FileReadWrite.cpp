#include "stdafx.h"
#include "FileReadWrite.h"


CFileReadWrite::CFileReadWrite(void)
{
	m_hFile =NULL;
	m_lpPath=NULL;
}
//打开文件
BOOL CFileReadWrite::Open(LPWSTR lpPath)
{
	if(m_hFile) CloseHandle(m_hFile);
	m_lpPath=lpPath;
	m_hFile=::CreateFile(m_lpPath,GENERIC_READ | GENERIC_WRITE,0, 0 ,OPEN_EXISTING ,FILE_ATTRIBUTE_NORMAL,0);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	return TRUE;
}

CFileReadWrite::~CFileReadWrite(void)
{
 if(m_hFile) CloseHandle(m_hFile);
}

//读文件
void CFileReadWrite::ReadData(PVOID pBuff,int nSize)
{
	DWORD dwReadSize=0;
	::ReadFile(m_hFile,pBuff,nSize,&dwReadSize,0);
}
//写文件
void CFileReadWrite::WriteData(PVOID pBuff,int nSize)
{
	DWORD dwWritenSize = 0;
	::WriteFile(m_hFile,pBuff,nSize,&dwWritenSize,0);
}
//设置偏移
void CFileReadWrite::SetOffset(LONG lOffset)
{
	SetFilePointer(m_hFile,lOffset,0,FILE_BEGIN);
}
//关闭文件句柄
void CFileReadWrite::Close()
{
	CloseHandle(m_hFile);
}