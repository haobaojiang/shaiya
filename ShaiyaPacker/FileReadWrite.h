#pragma once
#include <windows.h>



class CFileReadWrite
{
public:
	CFileReadWrite();
	~CFileReadWrite(void);
	void Open(LPWSTR lpPath);              //打开文件
	void ReadData  (PVOID pBuffer,int nSize);//读数据

	void ReadStr(PVOID pBuffer); //读字符串 
	void WriteStr(PVOID pBuffer);//写字符串
	void AppendStr(PVOID pBuffer);//追加一行字符串


	void WriteData (PVOID pBuffer,int nSize);//数据
	void SetOffset(LONG lOffset);		   //设置偏移
	void Close();						   //关闭文件句柄
	BOOL IsEnd();//看看是否到结尾了
	
public:
	LPCTSTR m_lpPath;//路径
	int     m_nType; //文件的打开方式
	HANDLE  m_hFile; //文件指针
};

