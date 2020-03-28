#include "stdafx.h"
#include "FileReadWrite.h"


CFileReadWrite::CFileReadWrite(void)
{
	m_hFile =NULL;
	m_lpPath=NULL;
}
//���ļ�
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

//���ļ�
void CFileReadWrite::ReadData(PVOID pBuff,int nSize)
{
	DWORD dwReadSize=0;
	::ReadFile(m_hFile,pBuff,nSize,&dwReadSize,0);
}
//д�ļ�
void CFileReadWrite::WriteData(PVOID pBuff,int nSize)
{
	DWORD dwWritenSize = 0;
	::WriteFile(m_hFile,pBuff,nSize,&dwWritenSize,0);
}
//����ƫ��
void CFileReadWrite::SetOffset(LONG lOffset)
{
	SetFilePointer(m_hFile,lOffset,0,FILE_BEGIN);
}
//�ر��ļ����
void CFileReadWrite::Close()
{
	CloseHandle(m_hFile);
}