#pragma once
#include <fstream>

using std::fstream;
using std::ios_base;

class CFileReadWrite
{
public:
	CFileReadWrite();
	~CFileReadWrite(void);
	BOOL Open(LPWSTR lpPath);              //打开文件
	void ReadData  (PVOID pBuff,int nSize);//读数据
	void WriteData (PVOID pBuff,int nSize);//数据
	void SetOffset(LONG lOffset);		   //设置偏移
	void Close();						   //关闭文件句柄
public:
	LPCTSTR m_lpPath;//路径
	int     m_nType; //文件的打开方式
	HANDLE  m_hFile; //文件指针
};

