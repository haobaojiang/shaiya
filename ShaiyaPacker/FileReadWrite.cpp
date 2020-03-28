#include "stdafx.h"
#include "FileReadWrite.h"


CFileReadWrite::CFileReadWrite(void)
{
	m_hFile =NULL;
	m_lpPath=NULL;
}
//打开文件
void CFileReadWrite::Open(LPWSTR lpPath)
{
	if(m_hFile) CloseHandle(m_hFile);
	m_lpPath=lpPath;
	m_hFile=::CreateFile(m_lpPath,GENERIC_READ | GENERIC_WRITE,0, 0 ,OPEN_ALWAYS ,FILE_ATTRIBUTE_NORMAL,0);//如果不存在则创建
}

CFileReadWrite::~CFileReadWrite(void)
{
 if(m_hFile) CloseHandle(m_hFile);
}

//读文件
void CFileReadWrite::ReadData(PVOID pBuffer,int nSize)
{
	DWORD dwReadSize=0;
	::ReadFile(m_hFile,pBuffer,nSize,&dwReadSize,0);
}
//写文件
void CFileReadWrite::WriteData(PVOID pBuffer,int nSize)
{
	DWORD dwWritenSize = 0;
	::WriteFile(m_hFile,pBuffer,nSize,&dwWritenSize,0);
}

//读字符串一行 
void CFileReadWrite::ReadStr(PVOID pBuffer)
{
	PBYTE pTemp=(PBYTE)pBuffer;
	DWORD dwReadSize=0;
	BYTE  byTemp[2]={0};
	while(ReadFile(m_hFile,&byTemp[0],1,&dwReadSize,0))
	{
		LONG lOffset=SetFilePointer(m_hFile,0,0,FILE_CURRENT); //先获取当前读取的指针位
		if(byTemp[0]==0x0d)
		{
			::ReadFile(m_hFile,&byTemp[1],1,&dwReadSize,0);
			if(byTemp[1]==0x0a) 
				break;			           //如果判定了是回车的话，那就停止读取
			SetFilePointer(m_hFile,lOffset,0,FILE_BEGIN);//把指针位退回去
		}
		*pTemp=byTemp[0];
		pTemp++;
	}
}
//写字符串
void CFileReadWrite::WriteStr(PVOID pBuffer)
{
	PBYTE pTemp=(PBYTE)pBuffer;
	DWORD dwWritenSize = 0;
	while(*pTemp)
	{
		::WriteFile(m_hFile,pTemp,1,&dwWritenSize,0);
		pTemp++;
	}
	//最后再加一个0d0a,意为回车换行
	WORD dwEndStr=0x0a0d;
	::WriteFile(m_hFile,&dwEndStr,2,&dwWritenSize,0);
}

//追加一行字符串,不改变offset
void CFileReadWrite::AppendStr(PVOID pBuffer)
{
	LONG lOffset=SetFilePointer(m_hFile,0,0,FILE_CURRENT); //先获取当前读取的指针位
	SetFilePointer(m_hFile,0,0,FILE_END);

	PBYTE pTemp=(PBYTE)pBuffer;
	DWORD dwWritenSize = 0;
	while(*pTemp)
	{
		::WriteFile(m_hFile,pTemp,1,&dwWritenSize,0);
		pTemp++;
	}

	//最后再加一个0d0a,意为回车换行
	WORD dwEndStr=0x0a0d;
	::WriteFile(m_hFile,&dwEndStr,2,&dwWritenSize,0);

	//然后还原offset
	SetFilePointer(m_hFile,lOffset,0,FILE_BEGIN);
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
	m_hFile=NULL;
}

//看看是否到结尾了
BOOL CFileReadWrite::IsEnd()
{
	DWORD dwReadSize=0;
	LONG lCurrent=SetFilePointer(m_hFile,0,0,FILE_CURRENT);//获取当前的读写位置
	LONG lEnd=SetFilePointer(m_hFile,0,0,FILE_END);        //获取最后的位置
	SetFilePointer(m_hFile,lCurrent,0,FILE_BEGIN);         //还原位置

	if(lCurrent==lEnd) return TRUE;

	return FALSE;

	//BYTE byTemp=0;

	//	SetFilePointer(m_hFile,lOffset,0,FILE_BEGIN);     //还原读写位置
	//	return FALSE;
	//}
	//else
	//{
	//	return TRUE;
	//}
}