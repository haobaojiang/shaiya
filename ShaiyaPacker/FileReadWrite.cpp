#include "stdafx.h"
#include "FileReadWrite.h"


CFileReadWrite::CFileReadWrite(void)
{
	m_hFile =NULL;
	m_lpPath=NULL;
}
//���ļ�
void CFileReadWrite::Open(LPWSTR lpPath)
{
	if(m_hFile) CloseHandle(m_hFile);
	m_lpPath=lpPath;
	m_hFile=::CreateFile(m_lpPath,GENERIC_READ | GENERIC_WRITE,0, 0 ,OPEN_ALWAYS ,FILE_ATTRIBUTE_NORMAL,0);//����������򴴽�
}

CFileReadWrite::~CFileReadWrite(void)
{
 if(m_hFile) CloseHandle(m_hFile);
}

//���ļ�
void CFileReadWrite::ReadData(PVOID pBuffer,int nSize)
{
	DWORD dwReadSize=0;
	::ReadFile(m_hFile,pBuffer,nSize,&dwReadSize,0);
}
//д�ļ�
void CFileReadWrite::WriteData(PVOID pBuffer,int nSize)
{
	DWORD dwWritenSize = 0;
	::WriteFile(m_hFile,pBuffer,nSize,&dwWritenSize,0);
}

//���ַ���һ�� 
void CFileReadWrite::ReadStr(PVOID pBuffer)
{
	PBYTE pTemp=(PBYTE)pBuffer;
	DWORD dwReadSize=0;
	BYTE  byTemp[2]={0};
	while(ReadFile(m_hFile,&byTemp[0],1,&dwReadSize,0))
	{
		LONG lOffset=SetFilePointer(m_hFile,0,0,FILE_CURRENT); //�Ȼ�ȡ��ǰ��ȡ��ָ��λ
		if(byTemp[0]==0x0d)
		{
			::ReadFile(m_hFile,&byTemp[1],1,&dwReadSize,0);
			if(byTemp[1]==0x0a) 
				break;			           //����ж����ǻس��Ļ����Ǿ�ֹͣ��ȡ
			SetFilePointer(m_hFile,lOffset,0,FILE_BEGIN);//��ָ��λ�˻�ȥ
		}
		*pTemp=byTemp[0];
		pTemp++;
	}
}
//д�ַ���
void CFileReadWrite::WriteStr(PVOID pBuffer)
{
	PBYTE pTemp=(PBYTE)pBuffer;
	DWORD dwWritenSize = 0;
	while(*pTemp)
	{
		::WriteFile(m_hFile,pTemp,1,&dwWritenSize,0);
		pTemp++;
	}
	//����ټ�һ��0d0a,��Ϊ�س�����
	WORD dwEndStr=0x0a0d;
	::WriteFile(m_hFile,&dwEndStr,2,&dwWritenSize,0);
}

//׷��һ���ַ���,���ı�offset
void CFileReadWrite::AppendStr(PVOID pBuffer)
{
	LONG lOffset=SetFilePointer(m_hFile,0,0,FILE_CURRENT); //�Ȼ�ȡ��ǰ��ȡ��ָ��λ
	SetFilePointer(m_hFile,0,0,FILE_END);

	PBYTE pTemp=(PBYTE)pBuffer;
	DWORD dwWritenSize = 0;
	while(*pTemp)
	{
		::WriteFile(m_hFile,pTemp,1,&dwWritenSize,0);
		pTemp++;
	}

	//����ټ�һ��0d0a,��Ϊ�س�����
	WORD dwEndStr=0x0a0d;
	::WriteFile(m_hFile,&dwEndStr,2,&dwWritenSize,0);

	//Ȼ��ԭoffset
	SetFilePointer(m_hFile,lOffset,0,FILE_BEGIN);
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
	m_hFile=NULL;
}

//�����Ƿ񵽽�β��
BOOL CFileReadWrite::IsEnd()
{
	DWORD dwReadSize=0;
	LONG lCurrent=SetFilePointer(m_hFile,0,0,FILE_CURRENT);//��ȡ��ǰ�Ķ�дλ��
	LONG lEnd=SetFilePointer(m_hFile,0,0,FILE_END);        //��ȡ����λ��
	SetFilePointer(m_hFile,lCurrent,0,FILE_BEGIN);         //��ԭλ��

	if(lCurrent==lEnd) return TRUE;

	return FALSE;

	//BYTE byTemp=0;

	//	SetFilePointer(m_hFile,lOffset,0,FILE_BEGIN);     //��ԭ��дλ��
	//	return FALSE;
	//}
	//else
	//{
	//	return TRUE;
	//}
}